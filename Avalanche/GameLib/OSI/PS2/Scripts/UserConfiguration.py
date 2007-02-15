#
# A service control script is a script that is called by each service
# whenever the service starts or stops, and at regular intervals if the
# script sets one or more timers. Each service calls its own service
# control script. This is the base class for all service control scripts.
#

import sys

import SandboxConfiguration
import ServiceID
import AccountManagement
import MatchMaking
import TypeExtension
import SanityCheck

import TTLTypeExtension
import EidosAccounts
import EidosFilter

#import MyAccountExtension

class UserConfiguration(SandboxConfiguration.BaseClass):

	#--------------------------------------------------------------------------
	def ConfigureAuthentication(self):
		print 'Authentication User Configuration'
	
	#--------------------------------------------------------------------------
	def CreateSandboxTable(self, tableName, tableDef):
		"""Helper function for creating a table in the sandbox."""
		
		print "Attempting to create or update table " + tableName
		
		self.DB().CreateTable(tableName, tableDef)
		self.UpdateTableDefinition(tableName, tableDef)
		
		print "Table " + tableName + " succesfully created or updated."
	
	#--------------------------------------------------------------------------
	def ConfigureDatabase(self):
		print 'Using shared tables for EOR accounts.'
		# Ensure that all standard tables exist
		self.UpdateTables()
		self.EnableSharedTables('eor', ['Principals',
			'PrincipalStatus', 'EidosFriends',
			'EidosAccounts', 'EidosGamesPlayed',
			'Connections', 'Daemons'])
		# For test purposes - disable in release
		# Keys for players with password "my password"
		import BuiltInGroups
		self.CreateAccount(1001,'player1',BuiltInGroups.Players,'3a130b8ec94c67bafed43096ca37ff6e','p2@toto.com')
		self.CreateAccount(1002,'player2',BuiltInGroups.Players,'481479974a9f4a4856281d0625959496','p2@toto.com')
		self.CreateAccount(1003,'player3',BuiltInGroups.Players,'5244010326f15d600aef6d26d6229342')
		self.CreateAccount(1004,'player4',BuiltInGroups.Players,'60e79c1f98384f158d450f61893acd4a')
		self.CreateAccount(1005,'player5',BuiltInGroups.Players,'0ebadf01da22644ed8ad058cdad58ad7')
		self.CreateAccount(1080,'Admin2',BuiltInGroups.Administrators,'93ff2105b5269de8f1bf657f4e5524a6')
		self.CreateAccount(1100,'DisabledPlayer',BuiltInGroups.Players,'3d091b240be8cc4fb4c2ef3660ecc5df',0)

	#--------------------------------------------------------------------------
	def ConfigureSandbox(self):
		print 'MySandbox Configuration is STARTING...'
		self.scriptengine.LogToRotatingFileBySize('_MySandbox.log',5000,8)
		
		import Registry
		Registry.UpdateStringValue('Parameters/ClientCharacterSet','utf8')
		# These should be disabled in release!
		#self.SetTraceFlag(16) # ServiceRequests - generates lots of requests

		#self.LogToClient()
		self.scriptengine.LogShowDateTime(True)
		self.scriptengine.LogShowPID(True)
		
		SanityCheck.controller.SetFixableStatus(SanityCheck.Bootstrap,
			[SanityCheck.StatusWarning, SanityCheck.StatusError])
		
		self.logger.info('Loading custom participation policies')
		import ParticipationPolicy
		manager = ParticipationPolicy.Manager(None)
		import BuiltInParticipationPolicies
		manager.LoadPolicy('TTLParticipation','TTLOngoingParticipation', BuiltInParticipationPolicies.OngoingParticipation)

		manager.LoadPolicy('TTLParticipation','TTLOpenSessionParticipation', BuiltInParticipationPolicies.OpenSessionParticipation)
		import ServiceID
		self.LoadControlScript(ServiceID.MatchMaking,'FinishedGatheringMonitor','TTLParticipation.py') 

		#-------------------------------------
		# These are for debugging only on TTL2
		#-------------------------------------
		#import MatchMaking
		#proto = MatchMaking.MatchMakingProtocol()
		#from Protocol import TraceProtocolClientCallHook, TraceURLHook
		#proto.hookmanager.AddPreHook('^[A-Za-z0-9]',TraceProtocolClientCallHook(proto))
		#proto.hookmanager.AddPreHook('^_ProcessDisconnectEvent',TraceURLHook(proto))
		#import AccountManagement
		#proto = AccountManagement.AccountManagementProtocol()
		#proto.hookmanager.AddPreHook('^UpdateStatus',TraceURLHook(proto))

		# Create table for the Eidos Accounts
		print 'Creating custom tables'
		ttlEdiosAccounts = [
						('pid', 'INT not NULL'),
						('flags', 'INT'),
						('country', 'INT'),
						('postalcode', 'VARCHAR(60)'),
						('dob', 'DATETIME'),
						(None, 'UNIQUE INDEX(pid)')
					]

		#self.CreateSandboxTable('EidosAccounts', ttlEdiosAccounts)

		ttlEidosGamesPlayed = [
						('pid', 'INT not NULL'), # Player ID.
						('game', 'VARCHAR(60)'), # Title of game user has played (e.g. "TTL" or "SnowBlind") from status string
						(None, 'PRIMARY KEY(pid, game)'),
					]

		#self.CreateSandboxTable('EidosGamesPlayed', ttlEidosGamesPlayed)


		print 'Creating custom tables'
		self.SetNbStatsColumns(16)

		#-------------------------------------------------------------------------
		# TTLSessions table for storing individual sessions (multi-round gatherings)
		#-------------------------------------------------------------------------
		ttlsessionsDef = [
							('gid', 'INT'),
							('levelName', 'VARCHAR(60)'),
							('levelIndex', 'INT'),
							('gameType', 'INT'),
							('timeLimit', 'INT'),
							('scoreLimit', 'INT'),
							('tagLimit', 'INT'),
							('bestOfSeries', 'INT'),
							('maxPlayers', 'INT'),
							('reservedPlayers', 'INT'),
							('totalConnectedPlayers', 'INT'),
							('isCustomQuery', 'BOOL'),
							('isRanked', 'BOOL'),
							('isPrivate', 'BOOL'),
							('playersRespawn', 'BOOL'),
							('friendlyFire', 'BOOL'),
							('apprehension', 'BOOL'),
							('arcadeDamage', 'BOOL'),
							('selfDamage', 'BOOL'),
							('alternatingTeams', 'BOOL'),
							(None, 'PRIMARY KEY (gid)')
						]

		self.CreateSandboxTable('TTLSessions', ttlsessionsDef)		


		#------------------------------------------------------------------------------
		# Register the TTLSession type extension
		#------------------------------------------------------------------------------
		proto = MatchMaking.MatchMakingProtocol()
		proto.GetTypeExtManager().RegisterExtension(TTLTypeExtension.TTLTypeExtension(),['TTLSession'])
		
		#------------------------------------------------------------------------------
		# EidosFriends table set up.  This table is used to store friends information
		#------------------------------------------------------------------------------
		tableEidosFriends = [	('pid1', 'INT not NULL'),
								('pid2', 'INT not NULL'),
								('rel1', 'TINYINT'),
								('rel2', 'TINYINT'),
								('isnew', 'TINYINT(1) not NULL DEFAULT 1'),
								(None, 'PRIMARY KEY (pid1, pid2)' )
							]
		
		#self.CreateSandboxTable('EidosFriends', tableEidosFriends)

		#------------------------------------------------------------------------------
		# EidosClan table set up.  This table is used to store clan information
		#------------------------------------------------------------------------------
		tabledefTTLPersonalStats = 	[	('pid',											'INT not NULL'),
										('stats_copWins',								'INT not NULL'),
										('stats_copLosses',								'INT not NULL'),
										('stats_copTies',								'INT not NULL'),
										('stats_gangWins',								'INT not NULL'),
										('stats_gangLosses',								'INT not NULL'),
										('stats_gangTies',								'INT not NULL'),
										('stats_copKills',								'INT not NULL'),
										('stats_gangKills',								'INT not NULL'),
										('stats_copDeaths',								'INT not NULL'),
										('stats_gangDeaths',							'INT not NULL'),
										('stats_copSuicides',							'INT not NULL'),
										('stats_gangSuicides',							'INT not NULL'),
										('stats_copHeadshots',							'INT not NULL'),
										('stats_gangHeadshots',							'INT not NULL'),
										('stats_copTeamKills',							'INT not NULL'),
										('stats_gangTeamKills',							'INT not NULL'),
										('stats_copTeamDeaths',							'INT not NULL'),
										('stats_gangTeamDeaths',						'INT not NULL'),
										('stats_copNPCKills',							'INT not NULL'),
										('stats_gangNPCKills',							'INT not NULL'),
										('stats_apprehensions',							'INT not NULL'),
										('stats_apprehended',							'INT not NULL'),
										('stats_copShotsFired',							'INT not NULL'),
										('stats_gangShotsFired',						'INT not NULL'),
										('stats_copShotsHit',							'INT not NULL'),
										('stats_gangShotsHit',							'INT not NULL'),
										('stats_copTimePlayed',							'INT not NULL'),
										('stats_gangTimePlayed',						'INT not NULL'),
										('stats_lootScores',							'INT not NULL'),
										('stats_raidScores',							'INT not NULL'),
										('stats_tagScores',								'INT not NULL'),
										('stats_copMelee',								'INT not NULL'),
										('stats_copGrenade',							'INT not NULL'),
										('stats_copSpecial',							'INT not NULL'),
										('stats_copSubMachinegun',						'INT not NULL'),
										('stats_copHeavyMachinegun',					'INT not NULL'),
										('stats_copShotgun',							'INT not NULL'),
										('stats_gangMelee',								'INT not NULL'),
										('stats_gangGrenade',							'INT not NULL'),
										('stats_gangSpecial',							'INT not NULL'),
										('stats_gangSubMachinegun',						'INT not NULL'),
										('stats_gangHeavyMachinegun',					'INT not NULL'),
										('stats_gangShotgun',							'INT not NULL'),
										('stats_copDeathHoldingMelee',					'INT not NULL'),
										('stats_copDeathHoldingGrenade',				'INT not NULL'),
										('stats_copDeathHoldingSpecial',				'INT not NULL'),
										('stats_copDeathHoldingSubMachinegun',			'INT not NULL'),
										('stats_copDeathHoldingHeavyMachinegun',		'INT not NULL'),
										('stats_copDeathHoldingShotgun',				'INT not NULL'),
										('stats_gangDeathHoldingMelee',					'INT not NULL'),
										('stats_gangDeathHoldingGrenade',				'INT not NULL'),
										('stats_gangDeathHoldingSpecial',				'INT not NULL'),
										('stats_gangDeathHoldingSubMachinegun',			'INT not NULL'),
										('stats_gangDeathHoldingHeavyMachinegun',		'INT not NULL'),
										('stats_gangDeathHoldingShotgun',				'INT not NULL'),
										('stats_copDeathByMelee',						'INT not NULL'),
										('stats_copDeathByGrenade',						'INT not NULL'),
										('stats_copDeathBySpecial',						'INT not NULL'),
										('stats_copDeathBySubMachinegun',				'INT not NULL'),
										('stats_copDeathByHeavyMachinegun',				'INT not NULL'),
										('stats_copDeathByShotgun',						'INT not NULL'),
										('stats_gangDeathByMelee',						'INT not NULL'),
										('stats_gangDeathByGrenade',					'INT not NULL'),
										('stats_gangDeathBySpecial',					'INT not NULL'),
										('stats_gangDeathBySubMachinegun',				'INT not NULL'),
										('stats_gangDeathByHeavyMachinegun',			'INT not NULL'),
										('stats_gangDeathByShotgun',					'INT not NULL'),
										('stats_tagWins',								'INT not NULL'),
										('stats_tagGamesPlayed',						'INT not NULL'),
										('stats_tagTime',								'INT not NULL'),
										('stats_warWins',								'INT not NULL'),
										('stats_warGamesPlayed',						'INT not NULL'),
										('stats_warTime',								'INT not NULL'),
										('stats_robberyWins',							'INT not NULL'),
										('stats_robberyGamesPlayed',					'INT not NULL'),
										('stats_robberyTime',							'INT not NULL'),
										('stats_raidWins',								'INT not NULL'),
										('stats_raidGamesPlayed',						'INT not NULL'),
										('stats_raidTime',								'INT not NULL'),
										('stats_mode5Wins',								'INT not NULL'),
										('stats_mode5GamesPlayed',						'INT not NULL'),
										('stats_mode5Time',								'INT not NULL'),
										('stats_mode6Wins',								'INT not NULL'),
										('stats_mode6GamesPlayed',						'INT not NULL'),
										('stats_mode6Time',								'INT not NULL'),
										('stats_incompletes',							'INT not NULL'),
										(None, 											'PRIMARY KEY (pid)'),
										(None, 											'INDEX tagWins (stats_tagWins)'),
										(None, 											'INDEX warWins (stats_warWins)'),
										(None, 											'INDEX robWins (stats_robberyWins)'),
										(None, 											'INDEX raidWins (stats_raidWins)'),
										(None, 											'INDEX apps (stats_apprehensions)'),
										(None, 											'INDEX copHeadshots (stats_copHeadshots)'),
										(None, 											'INDEX gangHeadshots (stats_gangHeadshots)'),
										(None, 											'INDEX copMelee (stats_copMelee)'),
										(None, 											'INDEX gangMelee (stats_gangMelee)')
										]

		self.CreateSandboxTable('TTLPersonalStats', tabledefTTLPersonalStats)
		
		#------------------------------------------------------------------------------
		# EidosClan table set up.  This table is used to store clan information
		#------------------------------------------------------------------------------
		tabledefEidosClan = [	('clan_id', 			'INT not NULL AUTO_INCREMENT'),	# Primary key
								('clan_abbr',			'VARCHAR(15)  not NULL'),		# Short 1-3 (but UTF8) char clan abbreviation.
								('clan_name',			'VARCHAR(84)  not NULL'),		# Clan name (UTF8).
								('clan_notice_from1',	'VARCHAR(150) not NULL'),		# Clan notice 1 author (text) (empty for none).
								('clan_notice_body1',	'VARCHAR(250) not NULL'),		# Clan notice 1 body (text) (empty for none).
								('clan_notice_from2',	'VARCHAR(150) not NULL'),		# ...
								('clan_notice_body2',	'VARCHAR(250) not NULL'),		# ...
								('clan_notice_from3',	'VARCHAR(150) not NULL'),		# ...
								('clan_notice_body3',	'VARCHAR(250) not NULL'),		# ...
								('clan_notice_from4',	'VARCHAR(150) not NULL'),		# ...
								('clan_notice_body4',	'VARCHAR(250) not NULL'),		# ...
								('clan_rating',			'INT not NULL DEFAULT 1600'),		# 
								('clan_kills',			'INT UNSIGNED not NULL DEFAULT 0'),		# 
								('clan_website',		'TEXT not NULL DEFAULT ""'),		# 
								(None,					'PRIMARY KEY (clan_id)'),
								(None,					'INDEX stat_clan_rating (clan_rating)')
								]

		self.CreateSandboxTable('EidosClan', tabledefEidosClan)

			
		#------------------------------------------------------------------------------
		# EidosClanPlayer table set up.  This table is used to store clan player info
		#------------------------------------------------------------------------------
		tabledefEidosClanPlayer = [	('pid', 					'INT not NULL'),					# Player. FK into Quazal Principals table.
									('clan_id',					'INT DEFAULT NULL'),				# Player's clan. FK into bo3clan table. NULL=unaffiliated.
									('clan_isacceptinginvites',	"CHAR(1) not NULL DEFAULT '1'"),	# Player is accepting invites (togglable).
									('clan_isleader',			"CHAR(1) not NULL DEFAULT '0'"),	# Player is leader of his clan (only 1).
									('clan_isofficer',			"CHAR(1) not NULL DEFAULT '0'"),	# Player is officer in his clan.
									('clan_alert_kickedout',	"CHAR(1) not NULL DEFAULT '0'"),	# Alert: player was kicked out.
									('clan_alert_newnotice',	"CHAR(1) not NULL DEFAULT '0'"),	# Alert: new clan notices to read.
									('clan_kickedoutby_player',	'INT DEFAULT NULL'),				# Alert data: who kicked out player (FK into Quazal Principals).
									(None,						'PRIMARY KEY (pid)'),
									(None,'INDEX clan_check (clan_id)')
								]

		self.CreateSandboxTable('EidosClanPlayer', tabledefEidosClanPlayer)
		
		#------------------------------------------------------------------------------
		# EidosClanInvite table set up.  This table is used to store clan invites for a player
		#------------------------------------------------------------------------------
		tabledefEidosClanInvite = [	('inv_inviter_clan_id', 	'INT not NULL'),					# Inviting clan. FK into bo3clan.
									('inv_inviter_pid',			'INT not NULL'),					# Inviting player. FK into Quazal Principals.
									('inv_invitee_pid',			'INT not NULL'),					# Invited player. FK into Quazal Principals.
									('inv_isnew',				"CHAR(1) not NULL DEFAULT '1'"),	# Is new? (0/1).
									('inv_blocked',             "CHAR(1) not NULL DEFAULT '0'"),    # Is blocked? (0/1).
									(None,						'PRIMARY KEY (inv_inviter_pid, inv_invitee_pid)'),
									(None,'INDEX invite_check (inv_invitee_pid )')
								]

		self.CreateSandboxTable('EidosClanInvite', tabledefEidosClanInvite)
		
		#------------------------------------------------------------------------------
		# EidosMessageOfTheDay.  This table is used to store system messages from Eidos
		#------------------------------------------------------------------------------
		tabledefEidosMessageOfTheDay = [
									('date', 'DATETIME not NULL'),
									('language', 'VARCHAR(50) not NULL'),
									('message', 'VARCHAR(250) not NULL'),
									(None, 'INDEX message_check (language, date)')
								]

		self.CreateSandboxTable('EidosMessageOfTheDay', tabledefEidosMessageOfTheDay)
		
		#--------------------------------------------------------------------------------
		# BPT Files
		#--------------------------------------------------------------------------------
		print 'Loading the BPT files'
		self.LoadUserBPT('OSI.bpt')

		#--------------------------------------------------------------------------------
		# Competition Logic
		#--------------------------------------------------------------------------------
		self.LoadCompetitionLogic('NonladderRanking','TTLRanking.py')

		#--------------------------------------------------------------------------------
		# Type extensions script
		#--------------------------------------------------------------------------------		
		print 'Loading type extensions scripts'
		proto = AccountManagement.AccountManagementProtocol()
		proto.GetPrivateTypeExtManager().RegisterExtension(EidosAccounts.EidosAccountsPrivate(),['EidosAccountPrivate'])
		proto.GetPublicTypeExtManager().RegisterExtension(EidosAccounts.EidosAccountsPublic(),['EidosAccountPublic'])
		proto.hookmanager.AddPreHook('CreateAccount',EidosFilter.StringCheck(proto,0))
		proto.hookmanager.AddPreHook('UpdateAccountName',EidosFilter.StringCheck(proto,0))
		proto.hookmanager.AddPreHook('UpdateStatus',EidosAccounts.UpdateStatusPreHook(proto))

		#--------------------------------------------------------------------------------
		# PROTOCOL Scripts
		#--------------------------------------------------------------------------------
		print 'Loading the custom protocol scripts'
		self.LoadProtocolScript(ServiceID.MatchMaking, 'TTLMatchMaking',  'TTLMatchMaking.py',  101)
		#self.LoadProtocolScript(ServiceID.MatchMaking, 'MyCustomProtocol', 'MyCustomProtocol.py', 102)

		#print 'Loading control scripts'

		#print 'Loading competition logics'
		#self.LoadCompetitionLogic('MyStatsLogic','MyStatsLogic.py')

		#self.AddTestData()
		
		# ----- XML-RPC CONFIGURATION ----- BEGIN ----- 
		if (True): 
			print 'Configuring XML-RPC Handlers...' 
			import quazal 
			import QXMLRPCServer 
			import EidosWebBackendLogic 
			import TTLWebBackendLogic 
			
			# Pick XML-RPC port based on sandbox name: 
			xmlRpcPortMap = { 
					'ttl1':       40007, 
					'ttl2':       40008, 
					'ttl3':       40009, 
					'ttlweb':     40000, 
					'ttllivepc':  40002, 
					'ttlliveps2': 40001, 
					} 
			sbName = quazal.ServiceHost.GetInstance().GetSandboxName() 
			xmlRpcPort = xmlRpcPortMap[sbName] 
			
			# Register methods: 
			rpcProtocol = QXMLRPCServer.XMLRPCServerProtocol() 
			EidosWebBackendLogic.RegisterMethods(rpcProtocol) 
			TTLWebBackendLogic.RegisterMethods(rpcProtocol) 
			
			# Start server: 
			print 'Starting XML-RPC Server (port %s)...' % (xmlRpcPort) 
			rpcProtocol.StartServer('localhost', xmlRpcPort) 
			print 'RPC started.' 
		# ----- XML-RPC CONFIGURATION ----- END ----- 

		# Create indices on tables
		print 'Updating table indices'
		
		statisticsExtension 	  = [(None,'INDEX stat_rating (gid,v0)'),
									 (None,'INDEX cop_wins (gid, v3)'),
									 (None,'INDEX gang_wins (gid, v6)'),
									 (None,'INDEX kills_cop (gid, v9)'),
									 (None,'INDEX kills_gang (gid, v11)'),
									 (None,'INDEX cop_games (gid, v4)'),
									 (None,'INDEX gang_games (gid, v7)')]
		self.DB().ExtendTable( 'Statistics', statisticsExtension )
				
		connectionStatsExtension = [(None,'INDEX login_check (logoutdate)')]
		self.DB().ExtendTable( 'ConnectionStats', connectionStatsExtension )

		print 'MySandbox Configuration is DONE'




	def AddTestData(self):
		import BuiltInGroups
		import whrandom
		
		c = self.DB().cursor()

		startingPlayerPID = 300
		
		c.execute( "SELECT * FROM Principals WHERE id=%s" % startingPlayerPID );
		if( c.rowcount == 0 ):
			partialKey = self.scriptengine.InitializeKey("1234")
			currentPlayerPID = startingPlayerPID
			playerToCreate = 1
			while( playerToCreate < 102 ):
				fullHashedKey = self.scriptengine.FinalizeKey(currentPlayerPID, partialKey)
				accountName = "ritual%03d" % playerToCreate
				self.CreateAccount(currentPlayerPID, accountName, BuiltInGroups.Players, fullHashedKey, "test@ritual.com")
				c.execute( "INSERT IGNORE INTO EidosAccounts (pid, flags, country, postalcode, dob) VALUES (%s, 1, 195, 12345, '1976-06-09 00:00:00')" % currentPlayerPID )
				playerToCreate += 1
				currentPlayerPID += 1
			
		# make all our players friends
		c.execute( "SELECT * FROM EidosFriends WHERE pid1=%s" % startingPlayerPID );
		if( c.rowcount == 0 ):
			currentPlayerPID = startingPlayerPID
			while( currentPlayerPID < startingPlayerPID + 101 ):
				otherPlayerPID = currentPlayerPID + 1
				while( otherPlayerPID < startingPlayerPID + 101 ):
					c.execute( "INSERT IGNORE INTO EidosFriends (pid1, pid2, rel1, rel2) VALUES (%s, %s, 1, 1)" % (currentPlayerPID, otherPlayerPID))
					otherPlayerPID += 1
				currentPlayerPID += 1
			
		# make all our players part of the same clan
		c.execute( "SELECT clan_id FROM EidosClan WHERE clan_abbr = 'RiT' AND clan_name = 'RitualTest'" )
		if( c.rowcount == 0 ):
			c.execute( "INSERT IGNORE INTO EidosClan (clan_abbr, clan_name) VALUES ('RiT', 'RitualTest')" )
		c.execute( "SELECT clan_id FROM EidosClan WHERE clan_abbr = 'RiT' AND clan_name = 'RitualTest'" )
		clanID = c.fetchone()[0]
		c.execute( "SELECT * FROM EidosClanPlayer WHERE pid=%s" % startingPlayerPID );
		if( c.rowcount == 0 ):
			currentPlayerPID = startingPlayerPID
			while( currentPlayerPID < startingPlayerPID + 100 ):
				isLeader = False
				isOfficer = False
				if( currentPlayerPID == startingPlayerPID ):
					isLeader = True
				elif( currentPlayerPID < startingPlayerPID + 15 ):
					isOfficer = True
				c.execute( "INSERT IGNORE INTO EidosClanPlayer (pid, clan_id, clan_isleader, clan_isofficer) VALUES (%s, %s, %s, %s)" % (currentPlayerPID, clanID, isLeader, isOfficer) )
				currentPlayerPID += 1
		
		# give all our players random stats
		c.execute('SELECT id FROM gatherings WHERE description="Nonladder" AND type="Ranking"')
		if( c.rowcount == 0 ):
			return
		gidRanking = c.fetchall()[0][0]
		c.execute( "SELECT * FROM Statistics s INNER JOIN TTLPersonalStats ps ON s.pid=ps.pid WHERE s.gid=%s AND s.pid=%s" % (gidRanking, startingPlayerPID) )
		if( c.rowcount == 0 ):
			currentPlayerPID = startingPlayerPID
			while( currentPlayerPID < startingPlayerPID + 101 ):
				playerStats = []
				playerStats.append( whrandom.randint(1580,1620) )
				playerStats.append( whrandom.randint(1580,1620) )
				playerStats.append( whrandom.randint(1580,1620) )
				playerStats.append( whrandom.randint(0,1) )
				playerStats.append( whrandom.randint(0,50) )
				playerStats.append( whrandom.randint(1,8) )
				playerStats.append( whrandom.randint(0,1) )
				playerStats.append( whrandom.randint(0,50) )
				playerStats.append( whrandom.randint(1,8) )
				playerStats.append( whrandom.randint(0,30) )
				playerStats.append( whrandom.randint(0,30) )
				playerStats.append( whrandom.randint(0,30) )
				playerStats.append( whrandom.randint(0,30) )
				playerStats.append( whrandom.randint(0,5) )
				playerStats.append( whrandom.randint(0,5) )
				playerStats.append( whrandom.randint(5,100) )
				playerStats.append( whrandom.randint(5,100) )
				playerStats.append( whrandom.randint(0,1) )
				playerStats.append( whrandom.randint(0,5) )
				playerStats.append( whrandom.randint(5,100) )
				playerStats.append( whrandom.randint(0,1) )
				playerStats.append( whrandom.randint(0,5) )
				playerStats.append( whrandom.randint(5,200) )
				playerStats.append( whrandom.randint(5,200) )
				playerStats.append( whrandom.randint(0,30) )
				playerStats.append( whrandom.randint(0,30) )
				playerStats.append( whrandom.randint(0,10) )
				playerStats.append( whrandom.randint(0,10) )
				playerStats.append( whrandom.randint(0,10) )
				playerStats.append( whrandom.randint(0,10) )
				playerStats.append( whrandom.randint(0,10) )
				playerStats.append( whrandom.randint(0,10) )
				playerStats.append( whrandom.randint(0,10) )
				playerStats.append( whrandom.randint(0,10) )
				playerStats.append( whrandom.randint(0,10) )
				playerStats.append( whrandom.randint(0,10) )
				playerStats.append( whrandom.randint(0,10) )
				playerStats.append( whrandom.randint(0,10) )
				playerStats.append( whrandom.randint(0,500) )
				playerStats.append( whrandom.randint(0,500) )
				playerStats.append( whrandom.randint(0,500) )
				playerStats.append( whrandom.randint(0,500) )
				playerStats.append( whrandom.randint(40,200) )
				playerStats.append( whrandom.randint(40,200) )
				playerStats.append( whrandom.randint(0,10) )
				playerStats.append( whrandom.randint(0,10) )
				playerStats.append( whrandom.randint(0,10) )
				playerStats.append( whrandom.randint(0,100) )
				playerStats.append( whrandom.randint(0,100) )
				playerStats.append( whrandom.randint(0,100) )
				playerStats.append( whrandom.randint(0,100) )
				playerStats.append( whrandom.randint(0,100) )
				playerStats.append( whrandom.randint(0,100) )
				playerStats.append( whrandom.randint(0,100) )
				playerStats.append( whrandom.randint(0,100) )
				playerStats.append( whrandom.randint(0,100) )
				playerStats.append( whrandom.randint(0,100) )
				playerStats.append( whrandom.randint(0,100) )
				playerStats.append( whrandom.randint(0,100) )
				playerStats.append( whrandom.randint(0,50) )
				playerStats.append( whrandom.randint(0,50) )
				playerStats.append( whrandom.randint(0,50) )
				playerStats.append( whrandom.randint(0,50) )
				playerStats.append( whrandom.randint(0,50) )
				playerStats.append( whrandom.randint(0,50) )
				playerStats.append( whrandom.randint(0,50) )
				playerStats.append( whrandom.randint(0,50) )
				playerStats.append( whrandom.randint(0,50) )
				playerStats.append( whrandom.randint(0,50) )
				playerStats.append( whrandom.randint(0,50) )
				playerStats.append( whrandom.randint(0,50) )
				playerStats.append( whrandom.randint(0,50) )
				playerStats.append( whrandom.randint(0,50) )
				playerStats.append( whrandom.randint(0,50) )
				playerStats.append( whrandom.randint(0,50) )
				playerStats.append( whrandom.randint(0,50) )
				playerStats.append( whrandom.randint(0,50) )
				playerStats.append( whrandom.randint(0,50) )
				playerStats.append( whrandom.randint(0,50) )
				playerStats.append( whrandom.randint(0,50) )
				playerStats.append( whrandom.randint(0,50) )
				playerStats.append( whrandom.randint(0,50) )
				playerStats.append( whrandom.randint(0,50) )
				playerStats.append( whrandom.randint(0,10) )
				playerStats.append( whrandom.randint(0,10) )
				playerStats.append( whrandom.randint(0,50) )
				playerStats.append( whrandom.randint(0,10) )
				playerStats.append( whrandom.randint(0,10) )
				playerStats.append( whrandom.randint(0,50) )
				playerStats.append( whrandom.randint(0,10) )
				playerStats.append( whrandom.randint(0,10) )
				playerStats.append( whrandom.randint(0,50) )
				playerStats.append( whrandom.randint(0,30) )
				playerStats.append( whrandom.randint(0,50) )
				playerStats.append( whrandom.randint(0,50) )
				playerStats.append( whrandom.randint(0,40) )
				playerStats.append( whrandom.randint(0,50) )
				playerStats.append( whrandom.randint(0,50) )
				playerStats.append( whrandom.randint(0,30) )
				playerStats.append( whrandom.randint(0,50) )
				playerStats.append( whrandom.randint(0,50) )
				playerStats.append( whrandom.randint(0,10) )
				
				c.execute( "INSERT IGNORE INTO Statistics "
					"(gid, pid, reporting_pid, flags, v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14) VALUES "
					"(%s, %s, 2, 1, " % (gidRanking, currentPlayerPID) + ",".join(["%s" % i for i in playerStats[0:15]]) + " )" )
				
				c.execute( "INSERT IGNORE INTO TTLPersonalStats VALUES "
					"( %s, " % currentPlayerPID + ",".join(["%s" % i for i in playerStats[15:]]) + " )" )
				
				currentPlayerPID += 1

#EOF
