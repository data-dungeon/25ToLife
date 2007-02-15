# -----------------------------------------------------------------------------
# TTLWebBackendLogic.py
#
# Python implementation of backend services available to 
# 25 to Life web site via XML-RPC.
#
# This file contains TTL-specific functionality.
# See also EidosWebBackendLogic.py for cross-game functionality.
#
# Based on SBWebBackendLogic.py, originally written for "Project: Snowblind" 
# (by Crystal Dynamics) website.  (Branched from 2005-02-12 (a)).
#
# This file is one of many scripts hosted in Quazal's Rendez-Vous
# environment.
#
#
#
# Author: Dan Kamins <dew@axonchisel.net> / <dkamins@crystald.com>
# Date: 2005-08-24
#
# (c) Copyright 2005, Eidos Inc.
# ALL RIGHTS RESERVED
# -----------------------------------------------------------------------------

# Python core modules:
import datetime
import time
import re

# Quazal modules:
import quazal
import AccountManagement

# TTL game-specific support:
import TTLMatchMaking

# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# CONSTANTS
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------

# Successful result code (shared with web client code):
RES_OKAY = 0

# Generic backend error (shared with web client code):
ERROR_BACKEND_GENERIC = 400

# Backend error for function not yet implemented (shared with web client code):
ERROR_BACKEND_NOT_IMPLEMENTED = 403



# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# API DEFINITION
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# Define public methods of this API:
myPublicMethods = [
	'TTLBack_DescribeInterface',
	
	'TTLBack_Stats_Report',
	
	'TTLBack_Test_01',
	'TTLBack_Test_CrashTest'
	]

# -----------------------------------------------------------------------------
def RegisterMethods(protocol):
	"""Register externally available RPC methods.
	
	This method is called from UserConfiguration.py.
	All RPC methods exposed by this interface are enumerated here.
	"""
	for methodName in myPublicMethods:
		protocol.RegisterMethod(__name__ + "." + methodName)



# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# PUBLIC INTERFACE
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------


# -----------------------------------------------------------------------------
def TTLBack_DescribeInterface():
	"""List available RPC methods.
	"""
	return myPublicMethods




# -----------------------------------------------------------------------------
def TTLBack_Stats_Report(queryUids, queryName, sortBy, sortOrder, resultOffset, resultSize):

	"""Generate top-level stats report, as requested.
	
	****** COMMENTS NOT UP TO DATE -- CURRENTLY TEST FUNCTION BRANCHING FROM SB BACKEND ********

	This function compiles and returns a table of stats, suitable for display.
	Specific queries are supported, but optional.
	Several columns (by name, below) can be sorted on.
	Results can be paginated by using resultOffset and resultSize params.


	@param string queryUids	Optional CSL (comma-separated) list of Uids.
							(for personal stats, friends, etc.).
							Specify '' (empty) for all players.
	@param string queryName Optional specifies name substring to search for.
							Specify '' (empty) for all players.
	@param string sortBy 	Optional column name to sort by.  Valid columns:
							'NAME', 'SCORE', 'KILLS', 'DEATHS', 'WINS', 'LOSSES'.
							Specify '' (empty) for no particular sorting.
	@param int    sortOrder Optional direction (+1 or -1) for sorting.
							Specify 0 for no particular sorting.
	@param int    resultOffset	Offset into results to begin report at.
							Specify 0 for first record.
	@param int    resultSize	Maximum number of rows to query and return,
							beginning at resultOffset.
							Specify -1 for no limit.
	
	@return Result code and List of StatsReportRecords (from 0 to resultSize records).
		Returns:
			( rescode, [StatsReportRecord*] )
		StatsReportRecord:
			(Player, Stats)
		Player:
			(uid, name, clanId, clanAbbr):
		Stats:
			(userRating, kills, deaths, suicides, wins, losses, MVPs, quits, gamesPlayed, deadliestWeapon, timeTotal)
			
		Stats details:
			timeUpdated is Unix Timestamp of last stats update.
			timeTotal is total seconds played.
			deadliestWeapon is empty string or string constant from:
				CARBINE DET_PACK FLECHETTE FRAG_GRENADE HERF MELEE 
				PISTOL RAIL_LASER ROCKET_LAUNCHER SHOTGUN SNIPER
			All other fields are ints.

	Results:
		Rescode   0: Okay.
		Error 10010: Bad sortBy specified.
			
	"""
	# Convert Unicode strings (from XML-RPC lib) to UTF8 raw (for backend):
	queryName = queryName.encode('utf8')

	#
	strMethod = "TTLBack_Stats_Report(%s, %s, %s, %s, %s, %s)" % (queryUids, queryName, sortBy, sortOrder, resultOffset, resultSize)
	LogMethodCall(strMethod, "...")
	c = GetDBCursor()

	# Find Ranking gathering:
	c.execute('SELECT id FROM Gatherings WHERE description="Nonladder" AND type="Ranking"')
	gidStats = int(c.fetchone()[0])

	# Construct WHERE SQL clause:
	sqlWhere = " WHERE (s.gid=%d)" % (gidStats)
	queryUids = str(queryUids)
	if (len(queryUids) > 0): 
		# Sanitize CSL: split into string sequence, filtering out empty elements:
		queryUidsSeq = filter(lambda s: len(s.strip())>0, queryUids.split(','))
		# Reform as CSL string: ->int->str each element (Exception if malformed):
		queryUids = ','.join(map (lambda x: str(int(x)), queryUidsSeq))
		sqlWhere += (" AND (s.pid in (%s))" % (queryUids))
	if (len(queryName) > 0): 
		# Sanitize by \-escaping [\'"%] in SQL (and note: %% => % later):
		queryNameEscaped = re.sub(r'([\\\'"%])', r'\\\1', str(queryName))
		sqlWhere += (" AND (p.name LIKE '%%" + queryNameEscaped + "%%')")

	# Construct ORDER SQL clause:
	sqlSort = ""
	if ((len(sortBy) > 0) and (sortOrder != 0)):
		colMap = { 'NAME':'p.name', 'USER_RATING':'s.v0' }
		if not (sortBy in colMap):
			return (10010, [])
		sqlSort = " ORDER BY " + colMap[sortBy]
		if (sortOrder > 0): sqlSort += " ASC"
		else: sqlSort += " DESC"

	# Construct LIMIT SQL clause:
	sqlLimit = ""
	if (resultOffset < 0): resultOffset = 0
	if (resultSize >= 0):
		sqlLimit += " LIMIT %s, %s" % (int(resultOffset), int(resultSize))
	
	# Determine if we query into extended personal stats (or just v* fields):
	queryPersonalStats = False
	
	# Stats v* columns, for reference:
	# userRating			=  v0
	# copRating				=  v1
	# gangRating			=  v2
	# copWins				=  v3
	# copGamesPlayed		=  v4
	# copPlayersInRound		=  v5
	# gangWins				=  v6
	# gangGamesPlayed		=  v7
	# gangPlayersInRound	=  v8
	# killsAsCop			=  v9
	# deathsAsCop			= v10
	# killsAsGangsta		= v11
	# deathsAsGangsta		= v12
	# copMVPs				= v13
	# gangMVPs				= v14
		
	# Build query:
	fields = [	 's.v0',  's.v1',  's.v2',  's.v3',  's.v4', 
				 's.v5',  's.v6',  's.v7',  's.v8',  's.v9', 
				's.v10', 's.v11', 's.v12', 's.v13', 's.v14',
				's.pid', 'p.name', 'cp.clan_id', 'c.clan_abbr', 
				]
	sqlSelect = ("SELECT " + ','.join(fields) + " FROM Statistics s "
		" LEFT JOIN Principals p ON (s.pid = p.id) "
		" LEFT JOIN EidosClanPlayer cp ON (s.pid = cp.pid) "
		" LEFT JOIN EidosClan c ON (cp.clan_id = c.clan_id) ")
	if (queryPersonalStats):
		sqlSelect += " LEFT JOIN TTLPersonalStats sp ON (s.pid = sp.pid) "
	sql = (sqlSelect + sqlWhere + sqlSort + sqlLimit)
	c.execute(sql, [])	

	# Iterate over results, building up statsReport:
	statsReport = []
	for row in c.fetchall():
		# Make player rec:
		player = list( row[ fields.index('s.pid') : (fields.index('s.pid')+4) ] )
		if (player[2] is None): player[2] = 0	# cleanup no clan id.
		if (player[3] is None): player[3] = ''	# cleanup no clan abbr.
		
		# Make stats rec:
		stats = list( row[ fields.index('s.v0') : (fields.index('s.v0')+15) ] )
		stats = map(int, stats)
		
		# Make/add stats report rec:
		statsReportRec = (player, stats)
		statsReport.append(statsReportRec)
		
	# Return:
	LogDetails(strMethod, "Returning report, size %d." % (len(statsReport)))
	return (RES_OKAY, statsReport)





# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# PRIVATE HELPERS
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------

# -------------------------------------------------------------------------
def Log(strMessage):
	"""Helper function to log a general message, top level. 

	@author Dan Kamins <dew@axonchisel.net>
	"""
	print "TTLBack::%s." % (strMessage)

# -------------------------------------------------------------------------
def LogMethodCall(strMethod, strArgs=""):
	"""Helper function to log a major method invocation + args, with timestamp, top level.
	
	@author Dan Kamins <dew@axonchisel.net>
	"""
	print "TTLBack::>>%s() @ %s - BEGIN - %s." % (
		strMethod, datetime.datetime.now().strftime("%Y-%m-%dT%H:%M:%S%z"), strArgs )

# -------------------------------------------------------------------------
def LogError(strMethod, strErrorMessage):
	"""Helper function to log serious (e.g. DB) errors in method, about to return.
	
	@author Dan Kamins <dew@axonchisel.net>
	"""
	print "TTLBack::! ! %s - ERROR! - %s." % (strMethod, strErrorMessage)

# -------------------------------------------------------------------------
def LogDetails(strMethod, strMessage):
	"""Helper function to log a message, slightly indented, less important than self.Log().

	@author Dan Kamins <dew@axonchisel.net>
	"""
	print "TTLBack::    %s - %s." % (strMethod, strMessage)


# -------------------------------------------------------------------------
def GetDBCursor():
	"""Helper function to get a DB Cursor.
	
	@author Dan Kamins <dew@axonchisel.net>
	"""
	# For now, just hijack one from a neighborhood protocol:
	return AccountManagement.AccountManagementProtocol().DB().cursor()





# =============== TESTS BELOW ==================
# =============== TESTS BELOW ==================
# =============== TESTS BELOW ==================
# =============== TESTS BELOW ==================
# =============== TESTS BELOW ==================
# =============== TESTS BELOW ==================
# =============== TESTS BELOW ==================
# =============== TESTS BELOW ==================
# =============== TESTS BELOW ==================
# =============== TESTS BELOW ==================
# =============== TESTS BELOW ==================
# =============== TESTS BELOW ==================
# =============== TESTS BELOW ==================



def TTLBack_Test_01(inStr):
	return "Yes, we have no %s. We have no %s today." % (inStr, inStr)

def TTLBack_Test_CrashTest(crashPassword):
	if (crashPassword != 'crash'):
		return (10001, "SPECIFY PASSWORD TO CRASH")
		
	print '*** TTLBack_Test_CrashTest executed! ***'
	print '*** SERVER MAY CRASH!               ***'
	
	# Print Unicode string to stdout.  Quazal no like:
	print u'Crash \xa9opy!'
	
	return (RES_OKAY, )

