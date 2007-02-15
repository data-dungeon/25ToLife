# 
# (c) Copyright 1998-2003, Quazal Technologies Inc.
# ALL RIGHTS RESERVED
# 
# Net-Z, Eterna, Q-Platform, Duplicated Object and Duplication Space 
# are trademarks of Quazal.
# 
# Use of this software indicates acceptance of the End User licence
# Agreement for this product. A copy of the EULA is available from
# www.quazal.com.
#

#--------------------------------------------
# I M P O R T S
#--------------------------------------------
import string
import datetime
import re
import Hook

import Protocol
import MatchMaking
import Friends
import PlayerRelationships
import EidosFilter
import EidosLocales
import qResult
from qResultErrors import qResultFacility

#---------------------------------------------
# G L O B A L S
#---------------------------------------------
kRelN,kRelA,kRelB = 0,1,2 # None,Agrees,Blocks
kStateI,kStateO,kStateF = 0,1,2 # Incoming,Outgoing,Friends
kFailureFriendInvitePlayerSelf	= 101
kFailureFriendInvitePlayerName	= 102
kFailureFriendInviteLocalLimit	= 103
kFailureFriendInviteOtherLimit	= 104

def Execute(cursor,query,trace=True):
    if trace: print query
    cursor.execute(query)
    return cursor


StatusFormat = re.compile(r'(.*?)\|(.*?)\|(.*?)($|\|.*)')

def LocalizeStatus(status,locale):
    output = status

    m = StatusFormat.match(status)
    if m:
        (title,platform,region,other) = m.groups()

        title      = EidosLocales.Lookup(title,locale)
        platform   = EidosLocales.Lookup(platform,locale)
        region     = EidosLocales.Lookup(region,locale)

        output = '%s|%s|%s%s' % (title,platform,region,other)
    elif status != 'Offline':
        print 'EidosFriends.LocalizeStatus: Unrecognized status', (status)

    return output


#----------------------------------------------
# C L A S S E S
#----------------------------------------------

# Error class used by clans
# -------------------------------------------------------------------------
class EidosResCodes:
    """Contains common result codes shared with Eidos client-side code.

    Keep this in sync with the RendezVous Error enum in OSIPS2.cpp on the client-side!!!

    """
    kNoError                  = 0
    kErrorGeneral             = 1
    kErrorPermissionDenied    = 2
    kErrorNotInClan           = 3
    kErrorInvalidClan         = 4
    kErrorInvalidPlayer       = 5
    kErrorClanAbbrTaken       = 6
    kErrorClanNameTaken       = 7
    kErrorAlreadyInClan       = 8
    kErrorAlreadyInvited      = 9
    kErrorNotAcceptingInvites = 10
    kErrorInvalidInvitation   = 11
    kErrorClanNotEmpty        = 12
    kErrorClanFull            = 13
    kErrorClanNameNotAllowed  = 14
    kErrorInSameClan          = 15

# -------------------------------------------------------------------------
# -------------------------------------------------------------------------
# -------------------------------------------------------------------------
# Configuration
# -------------------------------------------------------------------------
# -------------------------------------------------------------------------
# -------------------------------------------------------------------------

class Config:
    """Configuration data for EidosClans."""
    
    # SKU Text for parsing STATUS strings, like "Snowblind|PS2|....."
    StatusSKUPrefix = "25ToLife"

    # Current version (logged by EidosClans:: __init__ and FetchMyClanDesc)
    Version = "2004-12-01-B"

    # Min/Max length of clan abbreviations and names:
    (ClanAbbrLenMin, ClanAbbrLenMax) = (1, 3)
    (ClanNameLenMin, ClanNameLenMax) = (4, 20)
    
    # Maximum number of players in a clan:
    MaxClanSize = 100
    
    # Session variable set in the database that determines whether a player is in a game
    SessionString = "TTLSession"


# -------------------------------------------------------------------------
# -------------------------------------------------------------------------
# -------------------------------------------------------------------------
# Class Definitions
# -------------------------------------------------------------------------
# -------------------------------------------------------------------------
# -------------------------------------------------------------------------

# -------------------------------------------------------------------------
class EidosAlerts:
    """Python version of EidosAlerts DDL struct.

    class EidosAlerts
    {
        std_list<principalid> m_friendInvitations;
        std_list<principalid> m_clanInvitations;
        std_list<principalid> m_matchInvitations;
    } concrete;

    """
    def __init__(self):
        self.FriendInvitations = []
        self.ClanInvitations = []
        self.MatchInvitations = []
    def __str__(self): return str(self.Serialize())
    def Deserialize(self, tuple):
        (self.FriendInvitations, self.ClanInvitations, self.MatchInvitations) = tuple
    def Serialize(self):
        return (self.FriendInvitations, self.ClanInvitations, self.MatchInvitations)

# -------------------------------------------------------------------------
class EidosClanNotice:
    """Python version of EidosClanNotice DDL struct.

    class EidosClanNotice
    {
        string  m_From;     // author who updated notice.
        string  m_Body;     // freeform text contents of notice.
    } concrete;
        
    """
    def __init__(self):
        self.From = ''
        self.Body = ''
    def __str__(self): return str(self.Serialize())
    def Deserialize(self, tuple):
        (self.From, self.Body) = tuple
    def Serialize(self):
        return (self.From, self.Body)

# -------------------------------------------------------------------------
class EidosClanProps:
    """Python version of EidosClanProps DDL struct.

    class EidosClanProps
    {
        principalid                     m_ClanID;          // Clan id.
        string                          m_ClanAbbr;        // Clan abbreviation.
        string                          m_ClanName;        // Clan name.
        int32                           m_ClanRank;        // Rank of clan compared to other clans. -1 = unknown.
        int32                           m_ClanRating;      // Magic number rating of clan.
        uint32                          m_ClanKills;       // Number of kills clan members have made while in clan.
        uint32                          m_ClanMemberCount; // Number of members in the clan.
        string                          m_ClanLeader;      // Name of the leader of the clan.
    } concrete;

    """
    def __init__(self):
        self.ClanID   = 0
        self.ClanAbbr = ''
        self.ClanName = ''
        self.ClanRank = -1
        self.ClanRating = 0
        self.ClanKills = 0
        self.ClanMemberCount = 0
        self.ClanLeader = ''
    def __str__(self): return str(self.Serialize())
    def Deserialize(self, tuple):
        (self.ClanID, self.ClanAbbr, self.ClanName, self.ClanRank, self.ClanRating, self.ClanKills, self.ClanMemberCount, self.ClanLeader) = tuple
    def Serialize(self):
        return (self.ClanID, self.ClanAbbr, self.ClanName, self.ClanRank, self.ClanRating, self.ClanKills, self.ClanMemberCount, self.ClanLeader)

# -------------------------------------------------------------------------
class EidosClanMember:
    """Python version of EidosClanMember DDL struct.

    class EidosClanMember
    {
        prinipalid                  m_PlayerID;                 // Player id.
        string                      m_Player;                   // Player name.
        string                      m_ClanAbbr;                 // Clan abbreviation.
        bool                        m_ClanIsLeader;             // Is this member the (1) leader of the clan?
        bool                        m_ClanIsOfficer;            // Is this member an officer in clan?
        bool                        m_IsAcceptingInvites;       // Is this member accepting clan invitations at all?
        std_list<GatheringHolder>   m_NetLobbyMatch;            // match info (server, game, players, etc.) empty=not in game or irrelevant.
        bool                        m_IsFriend;                 // Is this member a friend of mine?
        bool                        m_IsOnline;                 // Is this member online (in this SKU)?
    } concrete;

    """
    def __init__(self):
        self.PlayerID = 0
        self.Player = ''
        self.ClanAbbr = ''
        self.ClanIsLeader = False
        self.ClanIsOfficer = False
        self.IsAcceptingInvites = True
        self.NetLobbyMatch = []
        self.IsFriend = False
        self.IsOnline = False
    def __str__(self): return str(self.Serialize())
    def Deserialize(self, tuple):
        (self.PlayerID, self.Player, self.ClanAbbr, self.ClanIsLeader, self.ClanIsOfficer, self.IsAcceptingInvites, self.NetLobbyMatch, self.IsFriend, self.IsOnline) = tuple
    def Serialize(self):
        return (self.PlayerID, self.Player, self.ClanAbbr, self.ClanIsLeader, self.ClanIsOfficer, self.IsAcceptingInvites, self.NetLobbyMatch, self.IsFriend, self.IsOnline)

# -------------------------------------------------------------------------
class EidosClanInvite:
    """Python version of EidosClanInvite DDL struct.

    class EidosClanInvite
    {
        uint32      m_InviterClanID             // id of clan sending invitation.
        string      m_InviterClanAbbr;          // Abbr of clan sending invitation.
        string      m_InviterClanName;          // Name of clan sending invitation.
        string      m_InviterPlayer;            // Player who sent invitation.
        int32       m_InviterClanRank;          // Rank of inviting clan. -1 = unknown.
        uint32      m_InviterClanMembers;       // Members in inviting clan.
        string      m_InviteePlayer;            // Person who was invited.
        bool        m_IsNew;                    // New invitation? (false = blocked).
    } concrete;

    """
    def __init__(self):
        self.InviterClanID = 0
        self.InviterClanAbbr = ''
        self.InviterClanName = ''
        self.InviterPlayer = ''
        self.InviterClanRank = -1
        self.InviterClanMembers = 0
        self.InviteePlayer = ''
        self.IsNew = True
    def __str__(self): return str(self.Serialize())
    def Deserialize(self, tuple):
        (self.InviterClanID, self.InviterClanAbbr, self.InviterClanName, self.InviterPlayer, self.InviterClanRank, self.InviterClanMembers, self.InviteePlayer, self.IsNew) = tuple
    def Serialize(self):
        return (self.InviterClanID, self.InviterClanAbbr, self.InviterClanName, self.InviterPlayer, self.InviterClanRank, self.InviterClanMembers, self.InviteePlayer, self.IsNew)

# -------------------------------------------------------------------------
class EidosAccountPrivate:
    """Python version of EidosClanInvite DDL struct.

    // NOTE: Update EidosAccounts.py when this changes!
    class EidosAccountPrivate : Data
    {
        // bit0: gender (0=female)
        // bit1: eidosCanSendMeEmails
        // bit2: eidosPartnersCanSendMeEmails
        byte		m_flags;

        string		m_postalCode;
        datetime	m_dateOfBirth;
    } concrete;

    """
    def __init__(self):
        self.Flags = 0
        self.PostalCode = ''
        self.DateOfBirth = ''
    def __str__(self): return str(self.Serialize())
    def Deserialize(self, tuple):
        (self.Flags, self.PostalCode, self.DateOfBirth) = tuple
    def Serialize(self):
        return (self.Flags, self.PostalCode, self.DateOfBirth)

# TTLMatchMaking--This is our API to the RendezVous
# servers.
#----------------------------------------------
class TTLMatchMaking(Protocol.BaseClass):

    def __init__(self):
        Protocol.BaseClass.__init__(self)
        TTLMatchMaking.instance=self
        self.matchMakingProtocol = MatchMaking.MatchMakingProtocol()
        self.join = ' INNER JOIN TTLSessions ON Gatherings.id = TTLSessions.gid '
        self.limit = ' LIMIT %s,%s '

    def InitializeScript(self,scriptEngine):
        Protocol.BaseClass.InitializeScript(self,scriptEngine)
        self.protocolFriends = Friends.FriendsProtocol()
        
    #------------------------------------------------
    # Update the current total connected players field
    # for all gatherings in the list.  This is a bit wonky.
    #------------------------------------------------
    def UpdateTotalConnectedPlayers(self, matchList ):
        print 'Updating total connected players'
        for matchListIter in matchList:
            gid = matchListIter[1][0]
            participantsList = self.matchMakingProtocol.GetParticipants(gid)
            numPart = len(participantsList)
            print 'Updating to ', numPart, ' Players'
            c = self.DB().cursor()
            c.execute('UPDATE TTLSessions SET totalConnectedPlayers=%s WHERE gid=%s', [numPart,gid])
            print 'Updated totalConnectedPlayers.'
            print '************************************'                

    #===============================================
    # Q U E R Y   F U N C T I O N S
    #===============================================
    #-----------------------------------------------
    # Takes an incoming query and performs it on the Gatherings table.
    # The query is assumed to be a search for available matches.  We
    # use the results of the query to update the table data.  This is
    # probably a big mistake, but it is convienent.  After the query, we
    # iterate through all the matches found, and for each one find out
    # how many participants are in the match.  We then update the TTLSessions
    # table totalConnectedPlayers column to have this value.  We then
    # have to search again to get the updated data.  FIXME!
    #------------------------------------------------
    def FindBySQLQuery(self, query, range):
        print '******** FindBySQLQuery ************'
        print 'In TTLMatchMakingProtocol.FindBySQLQuery. query = ', query, 'offset = ', range[0], 'size = ', range[1]
        try:
            matchList = MatchMaking.GatheringSelection(self.join + query + self.limit, [range[0],range[1]])
            #self.UpdateTotalConnectedPlayers( matchList )
            queryExtensionForTCP = ' AND TTLSessions.totalConnectedPlayers < Gatherings.maxparticipants'
            return MatchMaking.GatheringSelection(self.join + query + queryExtensionForTCP + self.limit, [range[0],range[1]])
        except:
            print 'In TTLMatchMakingProtocol.FindBySQLQuery. query = ', query, 'offset = ', range[0], 'size = ', range[1]
            print 'An exception occured'
            return []
    
    def FindByLevelName(self, levelName, range):        
        print 'In TTLMatchMakingProtocol.FindByLevel. level = ', levelName, 'offset = ', range[0], 'size = ', range[1]

        condition = ' WHERE TTLSessions.levelName = %s '
        return MatchMaking.GatheringSelection(self.join + condition + self.limit, [levelName,range[0],range[1]])

    def FindByRanking(self, ranking, maxdiff, range):
        print 'In TTLMatchMakingProtocol.FindByRanking. ranking = ', ranking, 'maxdiff =', maxdiff, 'offset = ', range[0], 'size = ', range[1]
        lolimit = ranking - maxdiff
        if lolimit < 0:
            lolimit = 0
        hilimit = ranking + maxdiff
        condition = ' WHERE TTLSessions.ranking >= %s AND TTLSessions.ranking <=  %s AND TTLSessions.isRanking = 1 AND Gatherings.state = 0'
        return MatchMaking.GatheringSelection(self.join + condition + self.limit, [lolimit,hilimit,range[0],range[1]])

    def FindRankingPositionForPID(self, gid, pid):
        print 'In TTLMatchMakingProtocol.FindStatsPosition. gathering id = ', gid, 'for pid = ',pid
        c = self.DB().cursor()        
        c.execute('SELECT * FROM Statistics WHERE gid = %s AND pid = %s', [gid,pid])
        row = c.fetchone()
        if row == None:
            return 0;
        statValue = row[4]
        print 'statValue = ', statValue, 'in row: ', row
        c = self.DB().cursor()        
        c.execute('SELECT COUNT(*) FROM Statistics WHERE gid = %s AND v0 > %s', [gid, statValue])
        count = c.fetchone()[0]
        print 'total count =', count + 1
        return count + 1
     
    def FindRankingPosition(self, gid):
        print 'In TTLMatchMakingProtocol.FindStatsPosition. gathering id = ', gid, 'for pid = ',self.GetCurrentPID()
        c = self.DB().cursor()        
        c.execute('SELECT * FROM Statistics WHERE gid = %s AND pid = %s', [gid,self.GetCurrentPID()])
        row = c.fetchone()
        if row == None:
            return 0;
        print 'statValue = ', statValue, 'in row: ', row
        c = self.DB().cursor()        
        c.execute('SELECT COUNT(*) FROM Statistics WHERE gid = %s AND v0 > %s', [gid, statValue])
        count = c.fetchone()[0]
        print 'total count =', count + 1
        return count +1

    # NOTE: this must stay in sync with personal stats array in OSIPlayerStats.cpp
    lStatsPersonal = ['stats_copWins', 'stats_copLosses', 'stats_copTies', 'stats_gangWins', 'stats_gangLosses',
        'stats_gangTies', 'stats_copKills', 'stats_gangKills', 'stats_copDeaths', 'stats_gangDeaths', 'stats_copSuicides',
        'stats_gangSuicides', 'stats_copHeadshots', 'stats_gangHeadshots', 'stats_copTeamKills', 'stats_gangTeamKills',
        'stats_copTeamDeaths', 'stats_gangTeamDeaths', 'stats_copNPCKills', 'stats_gangNPCKills', 'stats_apprehensions',
        'stats_apprehended', 'stats_copShotsFired', 'stats_gangShotsFired', 'stats_copShotsHit', 'stats_gangShotsHit',
        'stats_copTimePlayed', 'stats_gangTimePlayed', 'stats_lootScores', 'stats_raidScores', 'stats_tagScores',
        'stats_copMelee', 'stats_copGrenade', 'stats_copSpecial', 'stats_copSubMachinegun', 'stats_copHeavyMachinegun',
        'stats_copShotgun', 'stats_gangMelee', 'stats_gangGrenade', 'stats_gangSpecial', 'stats_gangSubMachinegun',
        'stats_gangHeavyMachinegun', 'stats_gangShotgun', 'stats_copDeathHoldingMelee', 'stats_copDeathHoldingGrenade',
        'stats_copDeathHoldingSpecial', 'stats_copDeathHoldingSubMachinegun', 'stats_copDeathHoldingHeavyMachinegun',
        'stats_copDeathHoldingShotgun', 'stats_gangDeathHoldingMelee', 'stats_gangDeathHoldingGrenade',
        'stats_gangDeathHoldingSpecial', 'stats_gangDeathHoldingSubMachinegun', 'stats_gangDeathHoldingHeavyMachinegun',
        'stats_gangDeathHoldingShotgun', 'stats_copDeathByMelee', 'stats_copDeathByGrenade', 'stats_copDeathBySpecial',
        'stats_copDeathBySubMachinegun', 'stats_copDeathByHeavyMachinegun', 'stats_copDeathByShotgun',
        'stats_gangDeathByMelee', 'stats_gangDeathByGrenade', 'stats_gangDeathBySpecial', 'stats_gangDeathBySubMachinegun',
        'stats_gangDeathByHeavyMachinegun', 'stats_gangDeathByShotgun', 'stats_tagWins', 'stats_tagGamesPlayed',
        'stats_tagTime', 'stats_warWins', 'stats_warGamesPlayed', 'stats_warTime', 'stats_robberyWins',
        'stats_robberyGamesPlayed', 'stats_robberyTime', 'stats_raidWins', 'stats_raidGamesPlayed', 'stats_raidTime',
        'stats_mode5Wins', 'stats_mode5GamesPlayed', 'stats_mode5Time', 'stats_mode6Wins', 'stats_mode6GamesPlayed',
        'stats_mode6Time', 'stats_incompletes']

    def AddStatsPersonal(self, statValues):
        pid = self.GetCurrentPID()
        print 'AddStatsPersonal Called.'
        numStatValues = len(statValues)-1 # don't count first stat because it is ranking
        if numStatValues == 0: return
        
        if numStatValues != len(self.lStatsPersonal):
            print "[Stats]::AddStatsPersonal - ERROR: number of personal stat values does not equal number of personal stat keys"
            return
        
        c = self.DB().cursor()
        c.execute('SELECT id FROM gatherings WHERE description="Nonladder" AND type="Ranking"')
        gidRanking = c.fetchall()[0][0]

        tags,vals = [],[]
        personalStatIndex = 0
        for value in statValues:
            #this mess is to skip the first element of statValues
            if personalStatIndex == 0:
                personalStatIndex += 1
                continue
            key = self.lStatsPersonal[personalStatIndex-1]
            tags.append(key+'='+key+'+%s')
            vals.append(int(value))
            personalStatIndex += 1

        c.execute('INSERT IGNORE TTLPersonalStats SET pid=%s', [pid])
        c.execute('UPDATE TTLPersonalStats SET %s WHERE pid=%%s' % ",".join(tags), vals+[pid])

    def GetLadderStats(self, pivotPlayer, count):
        print 'GetLadderStats: Start for player %s' % pivotPlayer
        lStats = []
        arbStatsFields = ['s.pid','s.v0','s.v1','s.v2','s.v3','s.v4','s.v5','s.v6','s.v7','s.v8','s.v9','s.v10','s.v11','s.v12','s.v13','s.v14']
        c = self.DB().cursor()
        tolerance = 0.01

        # Get Ranking ID
        c.execute('SELECT id FROM gatherings WHERE description="Nonladder" AND type="Ranking"')
        gidRanking = c.fetchall()[0][0]
        print 'GetLadderStats: Got Ranking ID %s' % gidRanking

        # Get the pivot players rating.
        c.execute('SELECT ' + ','.join(arbStatsFields) + ' FROM Statistics s WHERE s.gid=%s AND s.pid=%s', [gidRanking, pivotPlayer])
        if (c.rowcount < 1):
            print 'GetLadderStats: No stats for (pid=%d)' % pivotPlayer
            return [] # return empty list (== bad player)
        playerStats = c.fetchone()
        pivotRating = playerStats[arbStatsFields.index('s.v0')]
        print 'GetLadderStats: Got pivotRating %s' % pivotRating

        # Get the players just above the current player (including the pivot player)
        query = "SELECT " + ','.join(arbStatsFields) + " FROM Statistics s " \
            "INNER JOIN TTLPersonalStats ps ON s.pid = ps.pid " \
            "WHERE s.gid=%u AND (s.v0 > %f + %s OR (s.v0 = %f AND s.pid < %u)) " \
            "ORDER BY s.v0 ASC, s.pid DESC LIMIT %u" % (gidRanking, pivotRating, tolerance, pivotRating, pivotPlayer, count)
        print "High stats query: %s" % query
        c.execute(query)
        lStatsHigherInc = list(c.fetchall())
        lStatsHigherInc.reverse()   # reverse so it goes highest to lowest

        # Query for count lower-scoring players below player (in highest to lowest order):
        query = "SELECT " + ",".join(arbStatsFields) + " FROM Statistics s " \
            "INNER JOIN TTLPersonalStats ps ON s.pid = ps.pid " \
            " WHERE s.gid=%u AND (s.v0 < %f - %f OR (s.v0 = %f AND s.pid > %u))" \
            " ORDER BY s.v0 DESC, s.pid ASC LIMIT %u" % (gidRanking, pivotRating, tolerance, pivotRating, pivotPlayer, count)
        print "Low stats query: %s" % query
        c.execute(query)
        lStatsLower = list(c.fetchall())

        halfCount = (count/2)
        highListAdd = 0
        lowListAdd = count % 2
        if( len(lStatsLower) < halfCount - 1 + lowListAdd ):
            highListAdd = halfCount - 1 + lowListAdd - len(lStatsLower)
        if( len(lStatsHigherInc) < halfCount ):
            lowListAdd += halfCount - len(lStatsHigherInc)
        # get our high list start index starting from the end of highList
        highListBegin = len(lStatsHigherInc) - halfCount - highListAdd
        if( highListBegin < 0 ):
            highListBegin = 0
        # get our low list end index starting from the beginning of lowList
        lowListEnd = lowListAdd + halfCount - 1
        if( lowListEnd > len(lStatsLower) ):
            lowListEnd = len(lStatsLower)
        
        lStats = lStatsHigherInc[highListBegin:] + [playerStats] + lStatsLower[:lowListEnd]

        lTTLStats = []
        finalStats =[]
        for tStats in lStats:
            c = self.DB().cursor()

            # Get the player name
            c.execute("SELECT name FROM Principals WHERE id=%s", [tStats[0]] )
            name = c.fetchone()[0]

            # Get the clan abbreviation if any
            myClan = EidosClanProps()
            myPlayer = EidosClanMember()
            errorReturn = self.GetClanInfoByPlayerID(tStats[0], myClan, myPlayer)
            if(errorReturn == None):
                clanAbbr = myClan.ClanAbbr
                clanRating = myClan.ClanRating
                clanKills = myClan.ClanKills
            else:
                clanAbbr = ""
                clanRating = 0
                clanKills = 0

            # Get Personal Stats
            c.execute("SELECT " + ",".join(self.lStatsPersonal) + " FROM TTLPersonalStats WHERE pid=%s", [tStats[0]])
            personalStats = c.fetchone()

            # Combine arbitrated and personal stats into one list
            arbStats = tStats[1:]
            if personalStats != None:
                finalStats = arbStats + (self.FindRankingPositionForPID( gidRanking, tStats[0] ),) + personalStats
                tTTLStats = (name, clanAbbr, clanRating, clanKills, tStats[0], finalStats)
                lTTLStats.append(tTTLStats)

        return lTTLStats

    def GetTopClans(self, count):
        print 'GetTopClans: Entry...'
        c = self.DB().cursor()
        c.execute('SELECT clan_id FROM EidosClan ORDER BY clan_rating DESC LIMIT %s', [count])
        clanList = map( lambda(f): f[0], c.fetchall() )
        print 'GetTopClans: Top clans are ', clanList

        clanStatsList = []
        for clanid in clanList:
            info = self.ClanGetClanInfoFromClanID(clanid)
            clanStatsList.append(info[1])

        return clanStatsList

    def GetTopPlayers(self, count):
        # Get the ID of the TTLRanking gathering
        print 'GetTopPlayers: Entry...'
        c = self.DB().cursor()

        c.execute('SELECT id FROM gatherings WHERE description="Nonladder" AND type="Ranking"')
        if( c.rowcount == 0 ):
            return []
        
        gidRanking = c.fetchall()[0][0]
        
        c.execute('SELECT s.pid FROM Statistics s INNER JOIN TTLPersonalStats ps ON s.pid = ps.pid WHERE s.gid = %s ORDER BY v0 DESC LIMIT %s', [gidRanking, count])
        pidList = map( lambda(f): f[0], c.fetchall() )
        print 'GetTopPlayers: Top pids are ', pidList

        return self.GetStatsForPlayers(pidList)

    def GetStatsForPlayers(self, pidList):
        # Get the ID of the TTLRanking gathering
        print 'GetStatsForPlayers: Entry...'
        c = self.DB().cursor()
        c.execute('SELECT id FROM gatherings WHERE description="Nonladder" AND type="Ranking"')
        gidRanking = c.fetchall()[0][0]

        arbStatsFields = ['s.pid','s.v0','s.v1','s.v2','s.v3','s.v4','s.v5','s.v6','s.v7','s.v8','s.v9','s.v10','s.v11','s.v12','s.v13','s.v14']
        lTTLStats = []
        for pid in pidList:

            # Get the player name
            c.execute("SELECT name FROM Principals WHERE id=%s", pid )
            name = c.fetchone()[0]
            print 'GetStatsForPlayers: Name= ', name

            # Get the clan abbreviation if any
            myClan = EidosClanProps()
            myPlayer = EidosClanMember()
            errorReturn = self.GetClanInfoByPlayerID(pid, myClan, myPlayer)
            if(errorReturn == None):
                clanAbbr = myClan.ClanAbbr
                clanRating = myClan.ClanRating
                clanKills = myClan.ClanKills
            else:
                clanAbbr = ""
                clanRating = 0
                clanKills = 0
            
            # Get the arbitrated stats and the personal stats and put them together in one list            
            c.execute("SELECT " + ",".join(arbStatsFields) + " FROM Statistics s WHERE (s.gid=%s) AND (s.pid=%s)", [ gidRanking, pid ] )
            arbStats = c.fetchone()
            c.execute("SELECT " + ",".join(self.lStatsPersonal) + " FROM TTLPersonalStats WHERE pid=%s", pid)
            personalStats = c.fetchone()

            if ( arbStats == None or personalStats == None ):
                statsList = []
                print 'GetStatsForPlayers: Empty List, no Stats'
            else:
                statsList = arbStats + (self.FindRankingPositionForPID( gidRanking, pid ),) + personalStats

            # Create a single entry for this player and add it to the final list
            if ( len(statsList) != 0 ):
                statsEntry = (name, clanAbbr, clanRating, clanKills, pid, list(statsList[1:]))
                lTTLStats.append(statsEntry)

        print 'GetStatsForPlayers: Done!'            
        return lTTLStats

    def GetDetailedParticipantsByRanking(self, ladderid, nonladderid, rank, maxdiff, range):
        print 'In TTLMatchMakingProtocol.GetDetailedParticipantsByRanking. ladderid id = ', ladderid, 'nonladder id = ',nonladderid, 'rank = ',rank
        #find total ranks
        c = self.DB().cursor()
        c.execute('SELECT COUNT(*) FROM Statistics WHERE gid = %s',[ladderid])
        totalRanks = c.fetchone()[0]
        #fetch all ladder stats        
        c = self.DB().cursor()        
        lolimit = (rank - maxdiff)-1
        if lolimit < 0:
            lolimit = 0
        totalcount = (maxdiff * 2)+1
        c.execute('SELECT COUNT(*),pid,name,v0,v1,v2,v3,v4 FROM Statistics AS S INNER JOIN Principals AS P ON P.id = S.pid WHERE S.gid = %s GROUP BY S.v2 ORDER BY S.v2 DESC LIMIT %s,%s', [ladderid, lolimit, totalcount])
        lstNames = []
        lstGameStats = []        
        l = c.fetchone()
        if (l!=None):
            firstrank = l[0]+lolimit
        while (l!=None):
            d = self.DB().cursor() 
            d.execute('SELECT v0,v1,v2,v3 FROM Statistics WHERE gid = %s AND pid = %s', [nonladderid, l[1]])
            t = d.fetchone()
            if (t == None):
                t = (0,0,0,0)
            lstNames.append(l[2])
            lstGameStats.append((l[3],l[4],l[5],firstrank,l[6],l[7],totalRanks,t[2],t[3],t[0],t[1]))
            l = c.fetchone()
            firstrank = firstrank + 1
            
        return (lstNames, lstGameStats)
        
    def GetLadderRankingID(self):
        lstRankings = self.matchMakingProtocol.FindByDescription('LadderRanking',(0,1))
        if (len(lstRankings) > 0):
            return lstRankings[0][0]
        return 0
        
    def GetNonLadderRankingID(self):
        lstRankings = self.matchMakingProtocol.FindByDescription('NonLadderRanking',(0,1))
        if (len(lstRankings) > 0):
            return lstRankings[0][0]
        return 0
        
    def GetNumberGamesPlayed(self):
        numLadderGamesPlayed = 0
        numNonLadderGamesPlayed = 0
        ladderRanking = self.GetLadderRankingID()
        if (ladderRanking != 0):
            c = self.DB().cursor()
            c.execute('SELECT ADD(v0) FROM Statistics WHERE gid = %s',[ladderRanking])
            numLadderGamesPlayed = c.fetchone()[0]
        nonLadderRanking = self.GetNonLadderRankingID()
        if (nonLadderRanking != 0):
            c = self.DB().cursor()
            c.execute('SELECT ADD(v2) FROM Statistics WHERE gid = %s',[nonLadderRanking])
            numNonLadderGamesPlayed = c.fetchone()[0]
        return numLadderGamesPlayed + numNonLadderGamesPlayed

    # -------------------------------------------------------------------------
    def GetAlerts(self):
        """Alerts the caller of new data available
        
        DDL Signature:
        
        void GetAlerts(out EidosAlerts outAlerts);
        """
        strMethod = "GetAlerts"
        outAlerts = EidosAlerts()
        pid = self.GetCurrentPID()
        c = self.DB().cursor()
        # get a list of all new friend invitations
        c.execute("SELECT pid1 FROM EidosFriends WHERE pid2=%s AND rel2=%s AND rel1=%s ORDER BY pid1 ASC", [pid, kRelN, kRelA])
        if( c.rowcount > 0 ):
            outAlerts.FriendInvitations = map(lambda(m): m[0], c.fetchall())
        c.execute("SELECT pid2 FROM EidosFriends WHERE pid1=%s AND rel1=%s AND rel2=%s ORDER BY pid2 ASC", [pid, kRelN, kRelA])
        if( c.rowcount > 0 ):
            outAlerts.FriendInvitations += map(lambda(m): m[0], c.fetchall())
        
        # get a list of all new clan invitations
        c.execute("SELECT inv_inviter_pid FROM EidosClanInvite WHERE inv_invitee_pid=%s AND inv_blocked='0' ORDER BY inv_inviter_pid ASC", pid)
        if( c.rowcount > 0 ):
            outAlerts.ClanInvitations = map(lambda(m): m[0], c.fetchall())
        
        # get a list of match invitations sent to me
        resultRange = ( 0, 25 )
        gatheringInvites = self.matchMakingProtocol.FindInvitations( resultRange )
        if( len( gatheringInvites ) > 0 ):
            outAlerts.MatchInvitations = map(lambda f: f[1][1], gatheringInvites)
        
        return outAlerts.Serialize()
    # -------------------------------------------------------------------------
    def GetEidosAccountInfo(self):
        """Alerts the caller of new data available
        
        DDL Signature:
        
        void GetEidosAccountInfo(out EidosAccountPrivate outPrivateData, out EidosAccountPublic outPublicData, out string email);
        """
        strMethod = "GetEidosAccountInfo"
        self.LogMethodCall(strMethod, "", "EidosAccount")
        
        pid = self.GetCurrentPID()
        c = self.DB().cursor()

        email = ""
        c.execute( "SELECT email FROM Principals WHERE id = %s", pid )
        if( c.rowcount != 0 ):
            email = c.fetchone()[0]

        c.execute( "SELECT * FROM EidosAccounts WHERE pid = %s", pid )
        if( c.rowcount != 0 ):
            import EidosAccounts

            eidosAccountObj = EidosAccounts.EidosAccounts()
            myPrivateAccountInfo = eidosAccountObj.Load( "EidosAccountPrivate", pid )
            myPublicAccountInfo = eidosAccountObj.Load( "EidosAccountPublic", pid )
        else:
            myDate = datetime.datetime( 1976, 6, 9 )
            myPrivateAccountInfo = ("EidosAccountPrivate", (0, "00000", myDate.now()))
            myPublicAccountInfo = ("EidosAccountPublic", (195,))

        retVal = (myPrivateAccountInfo[1], myPublicAccountInfo[1], email)
        self.LogReturn( strMethod, str( retVal ), "EidosAccount" )
        return retVal

    # -------------------------------------------------------------------------
    def FilterText(self, textToFilter):
        """Filters text for profanity
        
        DDL Signature:
        
        bool FilterText(string textToFilter)
        """
        strMethod = "FilterText"
        self.LogMethodCall(strMethod, "textToFilter=%s" % (textToFilter))
        
        wordsInText = textToFilter.split()
        passesCheck = True
        for word in wordsInText:
            if( self.ProfanityCheck(word) == False ):
                passesCheck = False
                break
        self.LogReturn(strMethod, str(passesCheck))
        return passesCheck

    # -------------------------------------------------------------------------
    # NOTE: message of the day is in UTF-8 format
    def GetMessageOfTheDay(self, inLanguage):
        """Gets the Eidos Message of the Day for the given language
        
        DDL Signature:
        
        void GetMessageOfTheDay(string inLanguage, out string outMessage);
        """
        c = self.DB().cursor()
        
        c.execute( "SELECT MAX(date) FROM EidosMessageOfTheDay WHERE language=%s", inLanguage )
        if( c.rowcount == 0):
            return ""
        maxDate = c.fetchone()[0]
        c.execute( "SELECT message FROM EidosMessageOfTheDay WHERE language=%s AND date=%s", [inLanguage, maxDate] )
        if( c.rowcount == 0):
            return ""
        
        return c.fetchone()[0]

    # -------------------------------------------------------------------------
    # NOTE: message needs to be passed in as UTF-8 format
    def SetMessageOfTheDay(self, language, message):
        """Sets the Eidos Message of the Day for the given language"""
        c = self.DB().cursor()
        
        c.execute( 'INSERT INTO EidosMessageOfTheDay (date, language, message) VALUES (NOW(), %s, %s)', [language, message] )
        if( c.rowcount == 0):
            return False
        return True

    #
    # Eidos mutual friends support (Xbox Live style)
    #
    #  State -- -- O  I  F  O  -- -- -- -- (what FriendsQuery returns)
    # RelRel -- NN AN NA AA AB BA BB NB BN (1st is caller's rel, crel)
    # ------------------------------------
    # Invite AN AN -- -- -- -- AA AB AB AN
    # Delete -- -- NN -- NN NB -- -- -- --
    # Accept -- -- -- AA -- -- -- -- -- --
    # Reject -- -- -- NN -- -- -- -- -- --
    #  Block -- -- -- BA -- -- -- -- -- --
    #
    # TODO: Extract as separate file EidosFriends.py
    # TODO: Change FriendsQuery to do fewer queries
    # TODO: Add SortPids function for shorter code
    #

    # INVITE: B NAB -> A NAB, N BN -> A BN
    # count crel==A or (crel==N and orel==A)
    # insert crel:=A, orel:=N
    # update crel:=A if crel==B or (crel==N and orel<>A)
    def FriendInvite(self,name):
        # get opid by name
        print 'PID %s inviting %s' % (self.GetCurrentPID(), name )
        c = self.DB().cursor()
        c.execute('SELECT id FROM principals WHERE name=%s', name)
        row = c.fetchone()
        if not row: raise qResult.QRESULT_ERROR(qResultFacility.PythonCore,kFailureFriendInvitePlayerName)
        opid = row[0]
 
        cpid = self.GetCurrentPID()
        print 'EidosFriends.FriendInvite: cpid=%s,opid=%s' % (cpid,opid)
        if opid == cpid: raise qResult.QRESULT_ERROR(qResultFacility.PythonCore,kFailureFriendInvitePlayerSelf)

        # NOTE: Most of the time this will limit to 100 friends,
        # but sometimes to 101, due to potential race condition.
        c = self.DB().cursor()
        q1 = 'SELECT COUNT(*) FROM EidosFriends WHERE %s OR %s'
        q2 = '(pid1=%%s AND (rel1=%s OR (rel1=%s AND rel2=%s)))' % (kRelA,kRelN,kRelA)
        q3 = '(pid2=%%s AND (rel2=%s OR (rel2=%s AND rel1=%s)))' % (kRelA,kRelN,kRelA)
        localFriendCount = Execute(c,q1 % (q2 % cpid, q3 % cpid)).fetchone()[0]
        otherFriendCount = Execute(c,q1 % (q2 % opid, q3 % opid)).fetchone()[0]
        print 'EidosFriends.FriendInvite: localFriendCount=%s,otherFriendCount=%s' % (localFriendCount,otherFriendCount)
        if localFriendCount >= 100: raise qResult.QRESULT_ERROR(qResultFacility.PythonCore,kFailureFriendInviteLocalLimit)
        if otherFriendCount >= 100: raise qResult.QRESULT_ERROR(qResultFacility.PythonCore,kFailureFriendInviteOtherLimit)

        c1 = self.DB().cursor()
        c2 = self.DB().cursor()
        if cpid < opid:
                Execute(c1,'INSERT IGNORE INTO EidosFriends (pid1,pid2,rel1,rel2) VALUES (%s,%s,%s,%s)' % (cpid,opid,kRelA,kRelN))
                Execute(c2,'UPDATE EidosFriends SET rel1=%s WHERE pid1=%s AND pid2=%s AND (rel1=%s OR (rel1=%s AND rel2<>%s))' % (kRelA,cpid,opid,kRelB,kRelN,kRelA))
        else:
                Execute(c1,'INSERT IGNORE INTO EidosFriends (pid1,pid2,rel1,rel2) VALUES (%s,%s,%s,%s)' % (opid,cpid,kRelN,kRelA))
                Execute(c2,'UPDATE EidosFriends SET rel2=%s WHERE pid1=%s AND pid2=%s AND (rel2=%s OR (rel2=%s AND rel1<>%s))' % (kRelA,opid,cpid,kRelB,kRelN,kRelA))
        return c1.rowcount == 1 or c2.rowcount == 1

    #-------------------------------------------------------------
    # Invite a friend by ID rather than name.  Calls FriendInvite.
    #-------------------------------------------------------------
    def FriendInviteByID( self, id ):
        c = self.DB().cursor()
        c.execute('SELECT name FROM principals WHERE id=%s', id)
        row = c.fetchone()
        if not row: raise qResult.QRESULT_ERROR(qResultFacility.PythonCore,kFailureFriendInvitePlayerName)
        name = row[0]
        return self.FriendInvite( name )

    #-------------------------------------------------------------
    # ACCEPT: N A -> A A
    # update crel:=A if crel==N and orel==A
    #-------------------------------------------------------------
    def FriendAccept(self,opid):
        cpid = self.GetCurrentPID()
        print 'EidosFriends.FriendAccept: cpid=%s,opid=%s' % (cpid,opid)
        c = self.DB().cursor()
        if cpid < opid:
            Execute(c,'UPDATE EidosFriends SET rel1=%s WHERE pid1=%s AND pid2=%s AND rel1=%s AND rel2=%s' % (kRelA,cpid,opid,kRelN,kRelA))
        else:
            Execute(c,'UPDATE EidosFriends SET rel2=%s WHERE pid1=%s AND pid2=%s AND rel1=%s AND rel2=%s' % (kRelA,opid,cpid,kRelA,kRelN))
        return c.rowcount == 1

    #-------------------------------------------------------------
    # REJECT: N A -> N N or B A
    # update crel:=NB,orel:=NA if crel==N and orel==A
    # delete if rel1==N and rel2==N
    #-------------------------------------------------------------
    def FriendReject(self,opid,block):
        cpid = self.GetCurrentPID()
        print 'EidosFriends.FriendDelete: cpid=%s,opid=%s,block=%s' % (cpid,opid,block)
        crel = (kRelN,kRelB)[block]
        orel = (kRelN,kRelA)[block]
        c1 = self.DB().cursor()
        c2 = self.DB().cursor()
        if cpid < opid:
            Execute(c1,'UPDATE EidosFriends SET rel1=%s,rel2=%s WHERE pid1=%s AND pid2=%s AND rel1=%s AND rel2=%s' % (crel,orel,cpid,opid,kRelN,kRelA))
            Execute(c2,'DELETE FROM EidosFriends WHERE pid1=%s AND pid2=%s AND rel1=%s AND rel2=%s' % (cpid,opid,kRelN,kRelN))
        else:
            Execute(c1,'UPDATE EidosFriends SET rel1=%s,rel2=%s WHERE pid1=%s AND pid2=%s AND rel1=%s AND rel2=%s' % (orel,crel,opid,cpid,kRelA,kRelN))
            Execute(c2,'DELETE FROM EidosFriends WHERE pid1=%s AND pid2=%s AND rel1=%s AND rel2=%s' % (opid,cpid,kRelN,kRelN))
        return c1.rowcount == 1

    #-------------------------------------------------------------
    # DELETE: A NAB -> N NNB
    # update crel:=N,orel:=N if crel==A and orel<>B
    # update crel:=N,orel:=B if crel==A and orel==B
    # delete if rel1==N and rel2==N
    #-------------------------------------------------------------
    def FriendDelete(self,opid):
        cpid = self.GetCurrentPID()
        print 'EidosFriends.FriendDelete: cpid=%s,opid=%s' % (cpid,opid)
        c1 = self.DB().cursor()
        c2 = self.DB().cursor()
        c3 = self.DB().cursor()
        if cpid < opid:
            Execute(c1,'UPDATE EidosFriends SET rel1=%s,rel2=%s WHERE pid1=%s AND pid2=%s AND rel1=%s AND rel2<>%s' % (kRelN,kRelN,cpid,opid,kRelA,kRelB))
            Execute(c2,'UPDATE EidosFriends SET rel1=%s,rel2=%s WHERE pid1=%s AND pid2=%s AND rel1=%s AND rel2= %s' % (kRelN,kRelB,cpid,opid,kRelA,kRelB))
            Execute(c3,'DELETE FROM EidosFriends WHERE pid1=%s AND pid2=%s AND rel1=%s AND rel2=%s' % (cpid,opid,kRelN,kRelN))
        else:
            Execute(c1,'UPDATE EidosFriends SET rel1=%s,rel2=%s WHERE pid1=%s AND pid2=%s AND rel2=%s AND rel1<>%s' % (kRelN,kRelN,opid,cpid,kRelA,kRelB))
            Execute(c2,'UPDATE EidosFriends SET rel1=%s,rel2=%s WHERE pid1=%s AND pid2=%s AND rel2=%s AND rel1= %s' % (kRelB,kRelN,opid,cpid,kRelA,kRelB))
            Execute(c3,'DELETE FROM EidosFriends WHERE pid1=%s AND pid2=%s AND rel1=%s AND rel2=%s' % (opid,cpid,kRelN,kRelN))
        return c1.rowcount == 1 or c2.rowcount == 1

    def FriendsQuery(self, sLocale):
        trace = False
        cpid = self.GetCurrentPID()
        if trace: print 'EidosFriends.FriendsQuery: cpid=%s' % cpid

        # returns A NAB, NA
        # select if crel==A or (crel==N and orel==A)
        c = self.DB().cursor()
        pidRels  = Execute(c,'SELECT pid2,rel1,rel2 FROM EidosFriends WHERE pid1=%s AND (rel1=%s OR (rel1=%s AND rel2=%s))' % (cpid,kRelA,kRelN,kRelA),trace).fetchall()
        pidRels += Execute(c,'SELECT pid1,rel2,rel1 FROM EidosFriends WHERE pid2=%s AND (rel2=%s OR (rel2=%s AND rel1=%s))' % (cpid,kRelA,kRelN,kRelA),trace).fetchall()
        
        # mark all invites sent to me as read
        #Execute(c, 'UPDATE EidosFriends SET isnew=0 WHERE pid1=%s AND (rel1=%s AND rel2=%s)' % (cpid, kRelN, kRelA), trace);
        #Execute(c, 'UPDATE EidosFriends SET isnew=0 WHERE pid2=%s AND (rel2=%s AND rel1=%s)' % (cpid, kRelN, kRelA), trace);

        # build map from pid to clan abbrs
        mapFriendIDToClanAbbr = {}
        if len(pidRels) > 0:
            c = self.DB().cursor()
            pids = map(lambda f: f[0], pidRels)
            strCSLFriendIDs = reduce(lambda s, fid: str(s)+ (', %d' % fid), pids)
            c.execute(  "SELECT p.id, c.clan_abbr FROM Principals p "
                            " LEFT JOIN EidosClanPlayer cp ON (p.id=cp.pid) "
                            " LEFT JOIN EidosClan c        ON (cp.clan_id=c.clan_id) "
                            " WHERE p.id IN (%s) " % strCSLFriendIDs )
            mapFriendIDToClanAbbr = dict(c.fetchall())
            for k, v in mapFriendIDToClanAbbr.iteritems():
                if v == None: mapFriendIDToClanAbbr[k] = ''

        # Get a list of match invitations sent to me
        resultRange = ( 0, 25 )
        gatheringInvites = self.matchMakingProtocol.FindInvitations( resultRange )

        friends = []
        for opid,crel,orel in pidRels:

            state = None
            if crel==kRelN and orel==kRelA: state=kStateI
            if crel==kRelA and orel==kRelN: state=kStateO
            if crel==kRelA and orel==kRelB: state=kStateO
            if crel==kRelA and orel==kRelA: state=kStateF
            assert state != None

            c = self.DB().cursor()
            row = Execute(c,'SELECT name FROM Principals WHERE id=%s' % opid, trace).fetchone()
            if not row:
                continue

            name = row[0]

            status = 'Offline'
            if state == kStateF:
                c = self.DB().cursor()
                row = Execute(c,'SELECT status FROM PrincipalStatus WHERE pid=%s' % opid, trace).fetchone()
                if row: status = row[0]

            #c = self.DB().cursor()
            #Execute(c,'SELECT %s FROM Gatherings AS g '
            #            'INNER JOIN ParticipationStatus AS ps ON g.id=ps.gid '
            #            'WHERE ps.pid=%s AND ps.pending=0 AND g.type="TTLSession"'
            #        % (MatchMaking.Gathering().GetSelection().replace('Gatherings.','g.'),opid),trace)
            #games = self.matchMakingProtocol.FetchGatheringList(c) # normally has at most one element
            
            # Check if member is playing a game:
            games = []
            isLookingToPlay = False
            if( state == kStateF ):
                lEidosGatherings = MatchMaking.GatheringSelection(
                    "INNER JOIN ParticipationStatus ps ON (Gatherings.id=ps.gid) "
                    "WHERE (ps.pid=%s) AND (ps.pending=0)"
                    "  AND (Gatherings.type='"+Config.SessionString+"')", (opid,))
                
                if (len(lEidosGatherings) > 0):
                    if( lEidosGatherings[0] in gatheringInvites ):
                        isLookingToPlay = True
                    games.append(lEidosGatherings[0])
            
            clanAbbr = mapFriendIDToClanAbbr[opid]

            friendData = (opid,name,state,0,LocalizeStatus(status, sLocale))
            friend = (friendData, clanAbbr, games, isLookingToPlay)
            if trace: print 'EidosFriends.FriendsQuery=>',friend
            friends.append(friend)

        friends.sort(self.FriendsSortFunc)
        return friends

    # -------------------------------------------------------------------------
    def FriendsSortFunc(self, m1, m2):
        """Comparator for FriendsQuery to return friend list in proper order for client.
        
        Takes Friends as m1 and m2.  Returns -1 if m1<m2, 0 if m1=m2, +1 if m1>m2.
        Sorts online first, then offline.
        Among those groups, sorts highest rank to lowest rank.
        Among those, sorts in ascii order.
        
        """

        # Put friends before incoming
        if (m1[0][2] == kStateF and m2[0][2] != kStateF): return -1
        if (m1[0][2] != kStateF and m2[0][2] == kStateF): return 1
        
        # Put incoming before outgoing
        if (m1[0][2] == kStateI and m2[0][2] != kStateI): return -1
        if (m1[0][2] != kStateI and m2[0][2] == kStateI): return 1
        
        # First put in-game kids first:
        if (len(m1[2]) != len(m2[2])):
            if (len(m1[2]) > len(m2[2])): return -1
            return 1
        
        # Now put online before offline:
        if (m1[0][4] != 'Offline' and m2[0][4] == 'Offline'): return -1
        if (m1[0][4] == 'Offline' and m2[0][4] != 'Offline'): return 1

        # Now sort by name:
        if (m1[0][1] < m2[0][1]): return -1
        if (m1[0][1] > m2[0][1]): return 1
        
        # They're the same!
        return 0

    #
    #
    # INTERNAL HELPER CLASSES / FUNCTIONS FOR CLANS:
    #
    #

    # -------------------------------------------------------------------------
    def ErrorReturn(self, rescode, dummyReturn):
        """Helper function for methods to return errors in proper format to client. """
        if (len(dummyReturn) == 1): 
            return rescode
        dummyReturn[0] = rescode
        return tuple(dummyReturn)


    # -------------------------------------------------------------------------
    def Log(self, strMessage, strGroup="EidosClans"):
        """Helper function to log a general message, top level. 
        
        Does not print PID, so can be used in INIT method.
        """
        print "%s:: %s." % (strGroup, strMessage)

    # -------------------------------------------------------------------------
    def LogMiniMethodCall(self, strMethod, strDetails, strGroup="EidosClans"):
        """Helper function to log a general mini message, top level."""
        print "%s::[p%5d] ..%s() @ %s - %s." % (
            strGroup, self.GetCurrentPID(), strMethod,
            datetime.datetime.now().strftime("%Y-%m-%dT%H:%M:%S%z"), 
            strDetails )

    # -------------------------------------------------------------------------
    def LogMethodCall(self, strMethod, strArgs="", strGroup="EidosClans"):
        """Helper function to log a major method invocation + args, with timestamp, top level."""
        print "%s::[p%5d] >>%s() @ %s - BEGIN - %s." % (
            strGroup, self.GetCurrentPID(), strMethod, datetime.datetime.now().strftime("%Y-%m-%dT%H:%M:%S%z"), strArgs )

    # -------------------------------------------------------------------------
    def LogError(self, strMethod, strErrorMessage, strGroup="EidosClans"):
        """Helper function to log serious (e.g. DB) errors in method, about to return."""
        print "%s::[p%5d] ! ! %s - ERROR! - %s." % (strGroup, self.GetCurrentPID(), strMethod, strErrorMessage)

    # -------------------------------------------------------------------------
    def LogReturn(self, strMethod, strReturn, strGroup="EidosClans"):
        """Helper function to log successful or non-fatal returns value from a method."""
        if len(strReturn) > 240:                    # if return log is large, 
            strReturn = strReturn[:240] + "..."     # abbreviate text so we don't flood client (was crashing).
        print "%s::[p%5d]    -%s - Returning: %s." % (strGroup, self.GetCurrentPID(), strMethod, strReturn)
        
    # -------------------------------------------------------------------------
    def LogDetails(self, strMethod, strMessage, strGroup="EidosClans"):
        """Helper function to log a message, slightly indented, less important than self.Log()."""
        print "%s::[p%5d]     %s - %s." % (strGroup, self.GetCurrentPID(), strMethod, strMessage)

    # -------------------------------------------------------------------------
    def LogPlayerInfo(self, strMethod, strPlayer, strClanAbbr):
        """Helper function to log info about player calling the method, slightly indented."""
        if (strClanAbbr == None): strClanAbbr = ""
        self.LogDetails(strMethod, "Caller: [%s] %s." % (strClanAbbr, strPlayer) )


    # -------------------------------------------------------------------------
    def ClanMemberSortFunc(self, m1, m2):
        """Comparator for FetchMyClanDesc to return member list in proper order for client.
        
        Takes EidosClanMember as m1 and m2.  Returns -1 if m1<m2, 0 if m1=m2, +1 if m1>m2.
        Sorts online first, then offline.
        Among those groups, sorts highest rank to lowest rank.
        Among those, sorts in ascii order.
        
        """
        # First put in-game kids first:
        if (len(m1.NetLobbyMatch) != len(m2.NetLobbyMatch)):
            if (len(m1.NetLobbyMatch) > len(m2.NetLobbyMatch)): return -1
            return 1
        
        # Now put online before offline:
        if (m1.IsOnline and (not m2.IsOnline)): return -1
        if ((not m1.IsOnline) and m2.IsOnline): return 1

        # Now put leaders before non-leaders:
        if (m1.ClanIsLeader and (not m2.ClanIsLeader)): return -1
        if ((not m1.ClanIsLeader) and m2.ClanIsLeader): return 1
        
        # Now put officers before non-officers:
        if (m1.ClanIsOfficer and (not m2.ClanIsOfficer)): return -1
        if ((not m1.ClanIsOfficer) and m2.ClanIsOfficer): return 1

        # Now sort by name:     
        if (m1.Player < m2.Player): return -1
        if (m1.Player > m2.Player): return 1
        
        # They're the same! Boy, the things you see if you hang around inside
        # a Python script for too long.  Let me tell you....
        # Hey, buddy, can I bum a smoke?  I'm dying in here.
        return 0
    

    # -------------------------------------------------------------------------
    def ProfanityCheck(self, strToCheck):
        """Checks string for profanity and returns True if OK, False if not allowed.

        Defer to EidosFilter.

        """

        # Let EidosFilter handle it:
        return EidosFilter.Check(strToCheck)

    # -------------------------------------------------------------------------
    def OldProfanityCheck(self, strToCheck):
        """Checks string for profanity and returns True if OK, False if not allowed.
                
        This is the old simple profanity checker.  
        Since we now defer to BO3ProfanityFilter, this is no longer used.
                
        """
        
        # First translate from symbolic chars to letters they may represent:
        leetTrans = string.maketrans("1370!@$|", "IETOIASI")
        strToCheck = strToCheck.translate(leetTrans)
        
        # Downcase string for easier comparison:
        strToCheck = strToCheck.lower()

        # Now define a list of disallowed words (all lowercase!) (includes FCC dirty 7):
        disallowedWords = [
            "shit", "sht", "fuck", "fck", "ass", 
            "piss", "crap", "anus", "bitch", 
            "pussy", "cunt", "whore", "blowjob", "handjob",
            "penis", "vagina", "cock", "clitoris", "tits",
            "lick my", "bite my", "suck my"]

        # Check for who gets coal in their stocking:
        for badword in disallowedWords:
            if (strToCheck.find(badword) >= 0):
                return False

        # Somebody's getting a Christmas present this year!
        return True

    # -------------------------------------------------------------------------
    def CheckForFriendship(self, pid1, pid2):
        c = self.DB().cursor()
        
        if pid1 < pid2:
            c.execute('SELECT * FROM EidosFriends WHERE pid1=%s AND pid2=%s AND rel1=%s AND rel2=%s' % (pid1,pid2,kRelA,kRelA))
        else:
            c.execute('SELECT * FROM EidosFriends WHERE pid1=%s AND pid2=%s AND rel1=%s AND rel2=%s' % (pid2,pid1,kRelA,kRelA))
        
        if(c.rowcount == 1):
            return True
        else:
            return False

    # -------------------------------------------------------------------------
    def GetClanRankFromClanRating(self, clanRating):
        c = self.DB().cursor()
        
        c.execute("SELECT COUNT(clan_id) AS clan_rank FROM EidosClan WHERE clan_rating > %s", [clanRating])
        
        if( c.rowcount == 0 ):
            return -1
        return c.fetchone()[0]+1
    
    # -------------------------------------------------------------------------
    # Get the users name by pid lookup in the principals table
    # returns "" to signify no entry
    def GetNameByPlayerID(self, inPlayerID):
        c = self.DB().cursor()
        
         # Load Principal db rec:
        principalFields = ["p.name"]
        c.execute("SELECT "+",".join(principalFields)+" FROM Principals p WHERE (p.id=%s)", inPlayerID)
        principalRec = c.fetchone()
        if (principalRec == None):
            return ""
        
        return principalRec[principalFields.index('p.name')]

    # -------------------------------------------------------------------------
    def GetClanMemberCount(self, clanID):
        c = self.DB().cursor()
        
        c.execute("SELECT COUNT(*) FROM EidosClanPlayer WHERE (clan_id=%s)", [clanID])
        return c.fetchone()[0]

    # -------------------------------------------------------------------------
    # Fills out an EidosClanMember and EidosClanProps struct for the caller
    # returns a tuple of the error and string message associated with that error
    # returns None to signify no error (means user is in a clan)
    def GetClanInfoByPlayerID(self, inPlayerID, outProps, outMember, insertPlayer = False):
        """Helper function to get clan information for the caller from the database."""
        strMethod = "GetClanInfoByPlayerID"
        
        c = self.DB().cursor()
        pid = inPlayerID
        
        # Get player name first, a value of "" means no valid pid
        outMember.PlayerID = pid
        outMember.Player = self.GetNameByPlayerID(pid)
        if(outMember.Player == ""):
            return (EidosResCodes.kErrorInvalidPlayer, "Cannot resolve pid %s" % pid)
        
        # Find and load clan member db rec:
        clanPlayerFields = ["cp.pid", "cp.clan_id", "cp.clan_isacceptinginvites", "cp.clan_isleader", "cp.clan_isofficer", "cp.clan_alert_kickedout", "cp.clan_alert_newnotice", "cp.clan_kickedoutby_player"]
        c.execute("SELECT "+",".join(clanPlayerFields)+" FROM EidosClanPlayer cp WHERE (cp.pid=%s)", [pid])
        clanPlayerRec = c.fetchone()
        # Insert a new player rec if one does not exist?
        if(insertPlayer and clanPlayerRec == None):
            self.LogDetails(strMethod, "Creating clan player record for new player pid=%s." % pid)
            c.execute("INSERT INTO EidosClanPlayer (pid, clan_isacceptinginvites) VALUES (%s, '1')", [pid])
            c.execute("SELECT "+",".join(clanPlayerFields)+" FROM EidosClanPlayer cp WHERE (cp.pid=%s)", [pid])
            clanPlayerRec = c.fetchone()
        
        if(clanPlayerRec == None):
            # No clan player record found
            if(insertPlayer):
                return (EidosResCodes.kErrorGeneral, "Couldn't create new player record! Aborting.")
            else:
                return (EidosResCodes.kErrorGeneral, "No clan player record for player pid=%s!!! Uh oh." % pid)
        
        # Fill out struct for the player
        clanID = clanPlayerRec[clanPlayerFields.index("cp.clan_id")]
        outMember.ClanIsLeader          = '1' == clanPlayerRec[clanPlayerFields.index("cp.clan_isleader")]
        outMember.ClanIsOfficer         = '1' == clanPlayerRec[clanPlayerFields.index("cp.clan_isofficer")]
        outMember.IsAcceptingInvites    = '1' == clanPlayerRec[clanPlayerFields.index("cp.clan_isacceptinginvites")]
        outMember.NetLobbyMatch         = [] # If I'm calling this it should never be while I'm in a match
        outMember.IsOnline              = True # I'm always online if I'm calling this
        
        # Is player in a clan?
        if(clanID == None):
            return (EidosResCodes.kErrorNotInClan, "kErrorNotInClan - player not in clan: %s" % (str(outMember.Serialize())))
        
        # Load clan db record:
        clanFields = ["p.name", "c.clan_id", "c.clan_abbr", "c.clan_name", "c.clan_notice_from1", "c.clan_notice_body1", "c.clan_notice_from2", "c.clan_notice_body2", "c.clan_notice_from3", "c.clan_notice_body3", "c.clan_notice_from4", "c.clan_notice_body4", "c.clan_rating", "c.clan_kills"]
        c.execute("SELECT "+",".join(clanFields)+" FROM EidosClan c "
                    " LEFT JOIN EidosClanPlayer cp ON (cp.clan_id=c.clan_id)"
                    " LEFT JOIN Principals p ON (p.id=cp.pid)"
                    " WHERE (c.clan_id=%s) AND cp.clan_isleader='1'", [clanID])
        clanRec = c.fetchone()
        
        # Check if we have a clan record, clan player record indicates we should
        if(clanRec == None):
            return (EidosResCodes.kErrorGeneral, "Inconsistent state! Player %s is in clan, but no clan rec found." % pid)

        outProps.ClanID   = clanID
        outProps.ClanAbbr = clanRec[clanFields.index('c.clan_abbr')]
        outProps.ClanName = clanRec[clanFields.index('c.clan_name')]
        outProps.ClanRating = clanRec[clanFields.index('c.clan_rating')]
        outProps.ClanKills = clanRec[clanFields.index('c.clan_kills')]
        outProps.ClanRank = self.GetClanRankFromClanRating(outProps.ClanRating)
        outProps.ClanLeader = clanRec[clanFields.index('p.name')]
        # Get clan member count.
        outProps.ClanMemberCount = self.GetClanMemberCount(clanID)

        # Fill in clan abbr field for player
        outMember.ClanAbbr = outProps.ClanAbbr
        
        # No errors, clan info is good
        return None
        
    # -------------------------------------------------------------------------
    # Fills out an EidosClanMember and EidosClanProps struct for the caller
    # returns a tuple of the error and string message associated with that error
    # returns None to signify no error (means user is in a clan)
    def GetClanInfoByPlayerName(self, inPlayerName, outProps, outMember, insertPlayer = False):
        c = self.DB().cursor()
        # Get PlayerID from name
        c.execute("SELECT p.id FROM Principals p WHERE (p.name=%s)", [inPlayerName])
        playerRec = c.fetchone()
        if(playerRec == None):
            return (EidosResCodes.kErrorInvalidPlayer, "Cannot resolve player: %s" % inPlayerName)
        playerID = playerRec[0]
        
        # Call into GetClanInfoByPlayerID to do all the dirty work
        return self.GetClanInfoByPlayerID(playerID, outProps, outMember, insertPlayer)

    #
    # MAJOR CLAN FUNCTIONS
    #
    #
    

    # -------------------------------------------------------------------------
    def ClanGetMyClanInfo(self):
        """Fetch user's clan description

        DDL Signature:
        
        uint32 ClanGetMyClanInfo(out EidosClanProps outProps, out EidosClanMember outPlayer);
        """
        strMethod = "GetMyClanInfo"
        self.LogMethodCall(strMethod, "PVer=%s" % (Config.Version))

        # Prep:
        outProps = EidosClanProps()
        outPlayer = EidosClanMember()
        dummyReturn = [0, outProps.Serialize(), outPlayer.Serialize()]

        # Get all our clan information from the database
        errorReturn = self.GetClanInfoByPlayerID(self.GetCurrentPID(), outProps, outPlayer, True)
        # If we have an error that is not that we are not in a clan
        if(errorReturn and errorReturn[0] != EidosResCodes.kErrorNotInClan):
            self.LogError(strMethod, errorReturn[1])
            return self.ErrorReturn(errorReturn[0], dummyReturn)

        self.LogPlayerInfo(strMethod, outPlayer.Player, outProps.ClanAbbr)

        # Pack it up and send it home:
        retval = (EidosResCodes.kNoError, outProps.Serialize(), outPlayer.Serialize())
        self.LogReturn(strMethod, str(retval))
        return retval

    # -------------------------------------------------------------------------
    def ClanCreateClan(self, inClanName, inClanAbbr):
        """Create a new clan, according to spec.
                
        DDL Signature:
        
        uint32 CreateClan(string inClanName, string inClanAbbr);
                
        """
        strMethod = "CreateClan"
        self.LogMethodCall(strMethod, "inClanName='%s', inClanAbbr='%s'"%(inClanName, inClanAbbr))

        # Prep:     
        c = self.DB().cursor()
        dummyReturn = [0]
        
        # Get all our clan information from the database
        myClan = EidosClanProps()
        myPlayer = EidosClanMember()
        errorReturn = self.GetClanInfoByPlayerID(self.GetCurrentPID(), myClan, myPlayer)
        # Are we already in a clan?
        if(errorReturn == None):
            self.LogReturn(strMethod, "kErrorAlreadyInClan - Player %s already in clan!" % myPlayer.PlayerID)
            return self.ErrorReturn(EidosResCodes.kErrorAlreadyInClan, dummyReturn)
        # Was there some other error besides that we aren't in a clan?
        elif(errorReturn[0] != EidosResCodes.kErrorNotInClan):
            self.LogError(strMethod, errorReturn[1])
            return self.ErrorReturn(errorReturn[0], dummyReturn)

        self.LogPlayerInfo(strMethod, myPlayer.Player, myClan.ClanAbbr)

        # Validate basic string stuff:
        su8ClanAbbr = unicode(inClanAbbr, 'utf8')
        su8ClanName = unicode(inClanName, 'utf8')
        if (len(su8ClanAbbr) < Config.ClanAbbrLenMin) or (len(su8ClanAbbr) > Config.ClanAbbrLenMax):
            self.LogReturn(strMethod, "for bad abbr '%s' error: kErrorClanNameNotAllowed." % inClanAbbr)
            return self.ErrorReturn(EidosResCodes.kErrorClanNameNotAllowed, dummyReturn)
        if (len(su8ClanName) < Config.ClanNameLenMin) or (len(su8ClanName) > Config.ClanNameLenMax):
            self.LogReturn(strMethod, "for bad name '%s' error: kErrorClanNameNotAllowed." % inClanName)
            return self.ErrorReturn(EidosResCodes.kErrorClanNameNotAllowed, dummyReturn)
        
        # Look for matching abbreviation and name:
        c.execute("SELECT c.clan_id FROM EidosClan c WHERE (c.clan_abbr=%s)", [inClanAbbr])
        if (c.fetchone()):
            self.LogReturn(strMethod, "for abbr taken '%s' error: kErrorClanAbbrTaken." % inClanAbbr)
            return self.ErrorReturn(EidosResCodes.kErrorClanAbbrTaken, dummyReturn)
        c.execute("SELECT c.clan_id FROM EidosClan c WHERE (c.clan_name=%s)", [inClanName])
        if (c.fetchone()):
            self.LogReturn(strMethod, "for abbr taken '%s' error: kErrorClanNameTaken." % inClanName)
            return self.ErrorReturn(EidosResCodes.kErrorClanNameTaken, dummyReturn)

        # Profanity filter:
        if (not self.ProfanityCheck(inClanAbbr) or not self.ProfanityCheck(inClanName)):
            self.LogReturn(strMethod, "kErrorClanNameNotAllowed - Shit! Profanity filter caught: '%s'/'%s'." % (inClanAbbr, inClanName))
            return self.ErrorReturn(EidosResCodes.kErrorClanNameNotAllowed, dummyReturn)
        # Fill in clan details:
        inClanRank = -1   # -1 = '?' in UI
        
        # Maybe stuff in a welcome notice here?  
        # We would need query the user's country code and i18n the proper notice.
        # Create clan:
        c.execute("INSERT INTO EidosClan (clan_abbr, clan_name) VALUES (%s, %s)", [inClanAbbr, inClanName])
        if (c.rowcount != 1):
            self.LogError(strMethod, "adding clan record for %s." % inClanAbbr)
            return self.ErrorReturn(EidosResCodes.kErrorGeneral, dummyReturn)
        c.execute('SELECT LAST_INSERT_ID()')
        clanID = c.fetchone()[0]
        self.LogDetails(strMethod, "Created clan record: clan_id=%s." % (clanID))

        # Assign player as leader of clan, and clear other player member flags:
        c.execute("UPDATE EidosClanPlayer SET clan_id=%s, clan_isleader=%s, clan_isofficer=%s, clan_alert_kickedout=%s "
                " WHERE (pid=%s)",
            [clanID, '1', '0', '0', myPlayer.PlayerID])
        if (c.rowcount < 1):
            print "Could not assign player as leader of clan"
            self.LogError(strMethod, "Couldn't modify membership for player %s in new clan %s" % (myPlayer.PlayerID, inClanAbbr))
            c.execute("DELETE FROM EidosClan WHERE (clan_abbr=%s)", [inClanAbbr] ) # try to cleanup before failure!
            return self.ErrorReturn(EidosResCodes.kErrorGeneral, dummyReturn)
        self.LogDetails(strMethod, "Assigned player as leader of new clan.")
        
        # Pack it up and send it home:
        retval = EidosResCodes.kNoError
        self.LogReturn(strMethod, str(retval))
        return retval

    # -------------------------------------------------------------------------
    def ClanGetClanIDListForPlayer(self, inPid):
        """Fetch user's clan id list from a principal id.
				
        DDL Signature:
        
        uint32 ClanGetClanIDListForPlayerID(principalid					inPid,
                                            out std_list<principalid>	outClansList);
				
        """
        strMethod = "GetClanIDListForPlayer"
        self.LogMethodCall(strMethod, "PVer=%s" % (Config.Version))
        
        # Prep
        pid = inPid
        c = self.DB().cursor()
        outClansList = []
        dummyReturn = [0, outClansList]
        
         # Load Principal db rec:
        principalFields = ["p.name"]
        c.execute("SELECT "+",".join(principalFields)+" FROM Principals p WHERE (p.id=%s)", pid)
        principalRec = c.fetchone()
        if (principalRec == None):
            self.LogError(strMethod, "Cannot resolve pid %s" % pid)
            return self.ErrorReturn(EidosResCodes.kErrorInvalidPlayer, dummyReturn)

        
        # Get clan id from pid
        clanPlayerFields = ["cp.clan_id"]
        c.execute("SELECT "+",".join(clanPlayerFields)+" FROM EidosClanPlayer cp WHERE (cp.pid=%s)", pid)
        clanPlayerRec = c.fetchone()
        if (clanPlayerRec):
            # player is in clan, return id
            outClansList = [ clanPlayerRec[clanPlayerFields.index('cp.clan_id')] ]
            # if clan_id is None, then really not in a clan
            if(outClansList[0] == None):
                outClansList = []
        else:
            # create a clan player record with a clan of NULL meaning no clan affiliation
            # but then the player can receive invites and create a clan
            playerFields = ["cp.pid", "cp.clan_id", "cp.clan_isacceptinginvites", "cp.clan_isleader", "cp.clan_isofficer", "cp.clan_alert_kickedout", "cp.clan_alert_newnotice", "cp.clan_kickedoutby_player"]
            self.LogDetails(strMethod, "Creating clan player record for new player pid=%s." % pid)
            c.execute("INSERT INTO EidosClanPlayer (pid, clan_isacceptinginvites) VALUES (%s, '1')", [pid])
            c.execute("SELECT "+",".join(playerFields)+" FROM EidosClanPlayer cp WHERE (cp.pid=%s)", [pid])
            clanPlayerRec = c.fetchone()
            if (clanPlayerRec == None):
                self.LogError(strMethod, "Couldn't create new clan player record! Aborting.")
                return self.ErrorReturn(EidosResCodes.kErrorGeneral, dummyReturn)
        
        retval = (EidosResCodes.kNoError, outClansList)
        self.LogReturn(strMethod, str(retval))
        return retval

    # -------------------------------------------------------------------------
    def ClanGetClanInfoFromClanID(self, inCid):
        """Fetch user's clan info from a clan id.
                
        DDL Signature:
        
        uint32 ClanGetClanInfoFromClanID(principalid                    inCid,
                                         out EidosClanProps             outProps)
                
        """
        
        strMethod = "GetClanInfoFromClanID"
        self.LogMethodCall(strMethod, "PVer=%s" % (Config.Version))
        
        # Prep
        cid = inCid
        c = self.DB().cursor()
        outProps = EidosClanProps()
        dummyReturn = [0, outProps.Serialize()]
        
        # Get clan info from clan id
        clanFields = ["c.clan_id", "c.clan_abbr", "c.clan_name", "c.clan_rating", "c.clan_kills"]
        c.execute("SELECT "+",".join(clanFields)+" FROM EidosClan c WHERE (c.clan_id=%s)", cid)
        clanRec = c.fetchone()
        if (clanRec == None):
            # no clan matching given id
            self.LogError(strMethod, "Couldn't find clan info from clan_id %i! Aborting." % cid)
            return self.ErrorReturn(EidosResCodes.kErrorInvalidClan, dummyReturn)
        
        outProps.ClanID   = cid
        outProps.ClanAbbr = clanRec[clanFields.index('c.clan_abbr')]
        outProps.ClanName = clanRec[clanFields.index('c.clan_name')]
        outProps.ClanRating = clanRec[clanFields.index('c.clan_rating')]
        outProps.ClanKills = clanRec[clanFields.index('c.clan_kills')]
        outProps.ClanRank = self.GetClanRankFromClanRating(outProps.ClanRating)
        retval = (EidosResCodes.kNoError, outProps.Serialize())
        self.LogReturn(strMethod, str(retval))
        return retval
        
    def ClanGetMyClanMembershipList(self):
        """Fetch user's clan membership list.
				
        DDL Signature:
        
        uint32 ClanGetMyClanMembershipList(out std_list<EidosClanMember> outMembers);
				
        """
        
        strMethod = "GetMyClanMembershipList"
        self.LogMethodCall(strMethod, "PVer=%s" % (Config.Version))
        
        # Prep
        c = self.DB().cursor()
        outMembers = []
        dummyReturn = [0, map(lambda(m): m.Serialize(), outMembers)]
        
        # Get all our clan information from the database
        myClan = EidosClanProps()
        myPlayer = EidosClanMember()
        errorReturn = self.GetClanInfoByPlayerID(self.GetCurrentPID(), myClan, myPlayer)
        # Was there an error while getting clan information?
        if(errorReturn):
            self.LogError(strMethod, errorReturn[1])
            return self.ErrorReturn(errorReturn[0], dummyReturn)
        
        self.LogPlayerInfo(strMethod, myPlayer.Player, myClan.ClanAbbr)

        # Look at all available member statuses and build mapping (to see who is online in our SKU)
        # (Status table will usually have entries for players online and indicate what game they're in)
        c.execute("SELECT cp.pid, s.status FROM PrincipalStatus s "
                    " INNER JOIN EidosClanPlayer cp ON (cp.pid=s.pid) "
                    " WHERE (cp.clan_id=%s)", [myClan.ClanID])
        mapMemberPIDToStatus = dict(c.fetchall())

        # Load all clan members:
        playerFields = ["cp.pid", "cp.clan_id", "cp.clan_isacceptinginvites", "cp.clan_isleader", "cp.clan_isofficer", "cp.clan_alert_kickedout", "cp.clan_alert_newnotice", "cp.clan_kickedoutby_player"]
        c.execute("SELECT p.name, "+",".join(playerFields)+" FROM EidosClanPlayer cp "
                    " INNER JOIN Principals p ON (cp.pid=p.id) "
                    " WHERE (cp.clan_id=%s)", [myClan.ClanID])
        lstClanMemberRecs = c.fetchall()
        for oneMemberRec in lstClanMemberRecs:
            oneMemberPID = oneMemberRec[1+playerFields.index('cp.pid')]
            member = EidosClanMember()
            member.PlayerID = oneMemberPID
            member.Player = oneMemberRec[0]
            member.ClanAbbr = myClan.ClanAbbr
            member.ClanIsLeader = '1' == oneMemberRec[1+playerFields.index("cp.clan_isleader")]
            member.ClanIsOfficer = '1' == oneMemberRec[1+playerFields.index("cp.clan_isofficer")]
            member.IsAcceptingInvites = '1' == oneMemberRec[1+playerFields.index("cp.clan_isacceptinginvites")]
            member.NetLobbyMatch = []
            member.IsFriend = self.CheckForFriendship(myPlayer.PlayerID, oneMemberPID)
            
            # Check if member is playing a game:
            lEidosGatherings = MatchMaking.GatheringSelection(
                "INNER JOIN ParticipationStatus ps ON (Gatherings.id=ps.gid) "
                "WHERE (ps.pid=%s) AND (ps.pending=0)"
                "  AND (Gatherings.type='"+Config.SessionString+"')", (oneMemberPID,))
            if (len(lEidosGatherings) > 0):
                member.NetLobbyMatch.append(lEidosGatherings[0])
            
            # Check if member is online in our SKU:
            if (oneMemberPID in mapMemberPIDToStatus):
                oneStatus = mapMemberPIDToStatus[oneMemberPID]
                if oneStatus.startswith(Config.StatusSKUPrefix):
                    member.IsOnline = True
            
            # Add member to outMember list:
            outMembers.append(member)

        # Sort list of players for client in display order: online/offline, leader/officer/soldier.
        outMembers.sort(self.ClanMemberSortFunc)

        # Pack it up and send it home:
        retval = (EidosResCodes.kNoError, map(lambda(m): m.Serialize(), outMembers))
        self.LogReturn(strMethod, str(retval))
        return retval

    #
    #
    # INVITATIONS:
    #
    #

    # -------------------------------------------------------------------------
    def ClanGetMyInvitations(self):
        """Checks and downloads invitations from/to player, and returns member rec (with relevant flags set).
				
        DDL Signature:
        
        uint32 GetMyInvitations(out std_list<EidosClanInvite> outInvitesToMe,
                                out std_list<EidosClanInvite> outInvitesFromMe);
				
        """
        strMethod = "GetMyInvitations"
        self.LogMethodCall(strMethod)

        # Prep:     
        c = self.DB().cursor()
        outInvitesToMe = []
        outInvitesFromMe = []
        dummyReturn = [0, map(lambda(i): i.Serialize(), outInvitesToMe), map(lambda(i): i.Serialize(), outInvitesFromMe)]

        # Get all our clan information from the database
        myClan = EidosClanProps()
        myPlayer = EidosClanMember()
        errorReturn = self.GetClanInfoByPlayerID(self.GetCurrentPID(), myClan, myPlayer)
        # If we have an error that is not that we are not in a clan
        if(errorReturn and errorReturn[0] != EidosResCodes.kErrorNotInClan):
            self.LogError(strMethod, errorReturn[1])
            return self.ErrorReturn(errorReturn[0], dummyReturn)
        
        self.LogPlayerInfo(strMethod, myPlayer.Player, myClan.ClanAbbr)

        # Load invitations TO player:
        invToFields = ["c.clan_id", "c.clan_abbr", "c.clan_name", "c.clan_rating", "pee.name", "per.name", "ci.inv_isnew"]
        c.execute("SELECT "+",".join(invToFields)+" FROM EidosClanInvite ci "
            " LEFT JOIN EidosClan c      ON (ci.inv_inviter_clan_id=c.clan_id) "
            " LEFT JOIN Principals per ON (ci.inv_inviter_pid=per.id) "
            " LEFT JOIN Principals pee ON (ci.inv_invitee_pid=pee.id) "
            " WHERE (ci.inv_invitee_pid=%s) AND (ci.inv_blocked='0')", [myPlayer.PlayerID] )
        invitesToMe = c.fetchall()
        for inviteToMe in invitesToMe:
            oneOutInviteToMe = EidosClanInvite()
            oneOutInviteToMe.InviterClanID = inviteToMe[invToFields.index('c.clan_id')]
            oneOutInviteToMe.InviterClanAbbr = inviteToMe[invToFields.index('c.clan_abbr')]
            oneOutInviteToMe.InviterClanName = inviteToMe[invToFields.index('c.clan_name')]
            oneOutInviteToMe.InviterPlayer = inviteToMe[invToFields.index('per.name')]
            oneOutInviteToMe.InviterClanRank = self.GetClanRankFromClanRating(inviteToMe[invToFields.index('c.clan_rating')])
            oneOutInviteToMe.InviteePlayer = myPlayer.Player
            oneOutInviteToMe.IsNew = ('1' == inviteToMe[invToFields.index('ci.inv_isnew')])
            oneOutInviteToMe.InviterClanMembers = self.GetClanMemberCount(inviteToMe[invToFields.index('c.clan_id')])
            outInvitesToMe.append(oneOutInviteToMe)
            
        # Mark all my invites as not new
        c.execute("UPDATE EidosClanInvite SET inv_isnew='0' WHERE inv_invitee_pid=%s", [myPlayer.PlayerID])

        # Load invitations FROM player:
        invFromFields = ["pee.name", "ci.inv_isnew"]
        c.execute("SELECT "+",".join(invFromFields)+" FROM EidosClanInvite ci "
            " LEFT JOIN Principals per ON (ci.inv_inviter_pid=per.id) "
            " LEFT JOIN Principals pee ON (ci.inv_invitee_pid=pee.id) "
            " WHERE (ci.inv_inviter_pid=%s) AND (ci.inv_inviter_clan_id=%s) AND (ci.inv_blocked='0')", [myPlayer.PlayerID, myClan.ClanID] )
        invitesFromMe = c.fetchall()
        for inviteFromMe in invitesFromMe:
            oneOutInviteFromMe = EidosClanInvite()
            oneOutInviteFromMe.InviterPlayer = myPlayer.Player
            if (myClan.ClanAbbr != ''):
                oneOutInviteFromMe.InviterClanID = myClan.ClanID
                oneOutInviteFromMe.InviterClanAbbr = myClan.ClanAbbr
                oneOutInviteFromMe.InviterClanName = myClan.ClanName
                oneOutInviteFromMe.InviterClanRank = myClan.ClanRank
                oneOutInviteFromMe.InviterClanMembers = myClan.ClanMemberCount
            oneOutInviteFromMe.InviteePlayer = inviteFromMe[invFromFields.index('pee.name')]
            oneOutInviteFromMe.IsNew = ('1' == inviteFromMe[invFromFields.index('ci.inv_isnew')])
            outInvitesFromMe.append(oneOutInviteFromMe)

        # Pack it up and send it home:
        retval = (EidosResCodes.kNoError, map(lambda(i): i.Serialize(), outInvitesToMe), map(lambda(i): i.Serialize(), outInvitesFromMe))
        self.LogReturn(strMethod, str(retval))
        return retval

    # -------------------------------------------------------------------------
    def ClanInvitePlayer(self, playerToInvite):
        """Send an invitation to a player to join the clan.
				
        DDL Signature:
        
        uint32 InvitePlayer( string player );
				
        """
        strMethod = "InvitePlayer"
        self.LogMethodCall(strMethod , "playerToInvite='%s'"%(playerToInvite))

        # Prep:     
        c = self.DB().cursor()
        dummyReturn = [0]
        
        # Get all our clan information from the database
        myClan = EidosClanProps()
        myPlayer = EidosClanMember()
        errorReturn = self.GetClanInfoByPlayerID(self.GetCurrentPID(), myClan, myPlayer)
        # Was there an error while getting clan information?
        if(errorReturn):
            self.LogError(strMethod, errorReturn[1])
            return self.ErrorReturn(errorReturn[0], dummyReturn)

        self.LogPlayerInfo(strMethod, myPlayer.Player, myClan.ClanAbbr)
        
        # Make sure we have permission (officer or leader):
        if (myPlayer.ClanIsOfficer == False and myPlayer.ClanIsLeader == False):
            self.LogReturn(strMethod, "kErrorPermissionDenied - Player pid=%s is not officer or leader and cannot recruit!" % myPlayer.PlayerID)
            return self.ErrorReturn(EidosResCodes.kErrorPermissionDenied, dummyReturn)

        # Verify clan isn't too large:
        self.LogDetails(strMethod, "Clan '%s' has %d members (%d max)." % (myClan.ClanAbbr, myClan.ClanMemberCount, Config.MaxClanSize))
        if (myClan.ClanMemberCount >= Config.MaxClanSize):
            self.LogReturn(strMethod, "kErrorClanFull - Clan '%s' is full (%d/%d max)!" % (myClan.ClanAbbr, myClan.ClanMemberCount, Config.MaxClanSize))
            return self.ErrorReturn(EidosResCodes.kErrorClanFull, dummyReturn)

        # Get clan information for player I'm inviting
        inviteeClan = EidosClanProps()
        inviteePlayer = EidosClanMember()
        errorReturn = self.GetClanInfoByPlayerName(playerToInvite, inviteeClan, inviteePlayer, True)
        # If we have an error that is not that we are not in a clan
        if(errorReturn and errorReturn[0] != EidosResCodes.kErrorNotInClan):
            self.LogError(strMethod, errorReturn[1])
            return self.ErrorReturn(errorReturn[0], dummyReturn)
        
        # If the player is in a clan, check some things in there:
        if (errorReturn == None):
            # Ensure player isn't in our clan (catches inviting self too):
            if (inviteeClan.ClanID == myClan.ClanID):
                self.LogReturn(strMethod, "kErrorInSameClan - Can't invite player %s to your own clan (%s)!" % (playerToInvite, myClan.ClanID))
                return self.ErrorReturn(EidosResCodes.kErrorInSameClan, dummyReturn)
            # Make sure they're not blocking all invitations:
            if (inviteePlayer.IsAcceptingInvites == False):
                self.LogReturn(strMethod, "kErrorNotAcceptingInvites - Player %s is not accepting invites!" % (playerToInvite))
                return self.ErrorReturn(EidosResCodes.kErrorNotAcceptingInvites, dummyReturn)
        
        # Check if the invitee has an invite from me already.
        # If the block flag is set, then we will error out here since the record persists
        c.execute("SELECT COUNT(*) FROM EidosClanInvite ci "
                " WHERE (ci.inv_invitee_pid=%s) AND ((ci.inv_inviter_pid=%s))",
                [ inviteePlayer.PlayerID, myPlayer.PlayerID ] )
        if (c.fetchone()[0] > 0):
            self.LogReturn(strMethod, "kErrorAlreadyInvited - Already outstanding invites, won't invite again to clan %s!" % myClan.ClanID)
            return self.ErrorReturn(EidosResCodes.kErrorAlreadyInvited, dummyReturn)
        
        # Register the new invitation:
        c.execute("INSERT INTO EidosClanInvite SET "
                    "inv_inviter_clan_id=%s, inv_inviter_pid=%s, inv_invitee_pid=%s, inv_isnew='1'",
                    [ myClan.ClanID, myPlayer.PlayerID, inviteePlayer.PlayerID ] )
        if (c.rowcount != 1):
            self.LogError(strMethod, "Unable to insert new invitation to clan %s!" % (myClan.ClanID))
            return self.ErrorReturn(EidosResCodes.kErrorGeneral, dummyReturn)
        
        # Pack it up and send it home:
        retval = EidosResCodes.kNoError
        self.LogReturn(strMethod, str(retval))
        return retval



    # -------------------------------------------------------------------------
    def ClanCancelInvitation(self, playerToUninvite):
        """Cancel an outgoing clan invitation a specific player.
        
        Invitation must be NEW, since a not-new one is effectively a block, which
        we can't cancel ourselves.

        DDL Signature:
        
        uint32 CancelInvitation( string playerToUninvite );
				
        """
        strMethod = "CancelInvitation"
        self.LogMethodCall(strMethod, "playerToUninvite='%s'"%(playerToUninvite))

        # Prep:     
        c = self.DB().cursor()
        dummyReturn = [0]
        
        # Get all our clan information from the database
        myClan = EidosClanProps()
        myPlayer = EidosClanMember()
        errorReturn = self.GetClanInfoByPlayerID(self.GetCurrentPID(), myClan, myPlayer)
        # If we have an error that is not that we are not in a clan
        if(errorReturn and errorReturn[0] != EidosResCodes.kErrorNotInClan):
            self.LogError(strMethod, errorReturn[1])
            return self.ErrorReturn(errorReturn[0], dummyReturn)

        self.LogPlayerInfo(strMethod, myPlayer.Player, myClan.ClanAbbr)

        # Try to delete the invitation:
        c.execute("DELETE ci FROM EidosClanInvite ci LEFT JOIN Principals pee"
                    " ON ci.inv_invitee_pid = pee.id "
                    " WHERE (ci.inv_inviter_clan_id = %s) "
                    "   AND (ci.inv_inviter_pid     = %s) "
                    "   AND (ci.inv_invitee_pid     = pee.id) AND (pee.name=%s) "
                    "   AND (ci.inv_blocked='0')", [myClan.ClanID, myPlayer.PlayerID, playerToUninvite])
        if (c.rowcount < 1):
            self.LogReturn(strMethod, "kErrorInvalidInvitation - No valid new invitation to %s from %s/%s!" % (playerToUninvite, myClan.ClanID, myPlayer.PlayerID))
            return self.ErrorReturn(EidosResCodes.kErrorInvalidInvitation, dummyReturn)

        # Pack it up and send it home:
        retval = EidosResCodes.kNoError
        self.LogReturn(strMethod, str(retval))
        return retval


    # -------------------------------------------------------------------------
    def ClanAcceptInvitation(self, clanIDInviter, playerNameInviter):
        """Accept a clan invitation from indicated clan/player.
				
        DDL Signature:
        
        uint32 AcceptInvitation(principalid clanIDInviter, string playerNameInviter );
				
        """
        strMethod = "AcceptInvitation"
        self.LogMethodCall(strMethod, "clanIDInviter='%i', playerNameInviter='%s'"%(clanIDInviter, playerNameInviter))

        # Prep:     
        c = self.DB().cursor()
        dummyReturn = [0]

        # Get player clan information
        myClan = EidosClanProps()
        myPlayer = EidosClanMember()
        errorReturn = self.GetClanInfoByPlayerID(self.GetCurrentPID(), myClan, myPlayer)
        # The only way we can accept an invitation is if we are not in a clan, check error for that
        if(errorReturn == None):
            # We are in a clan, can't accept an invitation
            self.LogReturn(strMethod, "kErrorAlreadyInClan - Player pid=%s already in clan!" % myPlayer.PlayerID)
            return self.ErrorReturn(EidosResCodes.kErrorAlreadyInClan, dummyReturn)
        elif(errorReturn[0] != EidosResCodes.kErrorNotInClan):
            self.LogReturn(strMethod, errorReturn[1])
            return self.ErrorReturn(errorReturn[0], dummyReturn)
        
        # Get the new clans abbr and check if the new clan has notices posted, for our alert flag later:
        c.execute("SELECT c.clan_abbr, c.clan_notice_from1 FROM EidosClan c WHERE (c.clan_id=%s)", [clanIDInviter])
        if(c.rowcount < 1):
            self.LogReturn(strMethod, "kErrorInvalidInvitation - Clan %d does not exist!" % clanIDInviter)
            return self.ErrorReturn(EidosResCodes.kErrorInvalidInvitation, dummyReturn)
        (clanAbbrInviter, newClanNotices) = c.fetchone()
        if len(newClanNotices) > 0: cbHasNotices = '1'
        else:                       cbHasNotices = '0'
        
        # Verify clan isn't too large:
        newClanSize = self.GetClanMemberCount(clanIDInviter)
        self.LogDetails(strMethod, "New clan '%s' has %d members (%d max)." % (clanAbbrInviter, newClanSize, Config.MaxClanSize))
        if (newClanSize >= Config.MaxClanSize):
            self.LogReturn(strMethod, "kErrorClanFull - Clan '%s' is full (%d/%d max)!" % (clanAbbrInviter, newClanSize, Config.MaxClanSize))
            return self.ErrorReturn(EidosResCodes.kErrorClanFull, dummyReturn)

        # Try to delete all invitations from this clan:
        c.execute("DELETE FROM EidosClanInvite"
                    " WHERE (inv_inviter_clan_id = %s) "
                    "   AND (inv_invitee_pid     = %s) "
                    "   AND (inv_blocked='0')", [clanIDInviter, myPlayer.PlayerID])
        if (c.rowcount < 1):
            self.LogReturn(strMethod, "kErrorInvalidInvitation - No valid new invitation for %s from %s/%s!" % (myPlayer.PlayerID, myClan.ClanAbbr, playerNameInviter))
            return self.ErrorReturn(EidosResCodes.kErrorInvalidInvitation, dummyReturn)

        # Put user in the clan:
        c.execute("UPDATE EidosClanPlayer SET "
                " clan_id=%s, clan_isleader='0', clan_isofficer='0', clan_alert_kickedout='0', clan_alert_newnotice=%s "
                " WHERE (pid=%s) ",
                [clanIDInviter, cbHasNotices, myPlayer.PlayerID])
        if (c.rowcount < 1):
            self.LogError(strMethod, "Couldn't modify membership for player %s, inv to clan %s" % (myPlayer.PlayerID, clanAbbrInviter))
            return self.ErrorReturn(EidosResCodes.kErrorGeneral, dummyReturn)
                
        # Note: Possibly add i18n "welcome new user" notice too.
        
        # Pack it up and send it home:
        retval = EidosResCodes.kNoError
        self.LogReturn(strMethod, str(retval))
        return retval

    # -------------------------------------------------------------------------
    def ClanRejectInvitation(self, clanIDInviter, playerNameInviter):
        """Reject a clan invitation from indicated clan/player.
        
        Also used to delete blocked invitations.

        DDL Signature:
        
        uint32 RejectInvitation(    principalid clanAbbrInviter, string playerNameInviter );
				
        """
        strMethod = "RejectInvitation"
        self.LogMethodCall(strMethod, "clanIDInviter='%s', playerNameInviter='%s'"%(clanIDInviter, playerNameInviter))

        # Prep:     
        c = self.DB().cursor()        
        dummyReturn = [0]

        # Get player clan information
        myClan = EidosClanProps()
        myPlayer = EidosClanMember()
        errorReturn = self.GetClanInfoByPlayerID(self.GetCurrentPID(), myClan, myPlayer)
        # Check for errors getting our clan info
        if(errorReturn and errorReturn[0] != EidosResCodes.kErrorNotInClan):
            self.LogReturn(strMethod, errorReturn[1])
            return self.ErrorReturn(errorReturn[0], dummyReturn)
        
        self.LogPlayerInfo(strMethod, myPlayer.Player, myClan.ClanAbbr)
        
        # Try to delete the invitation:
        c.execute("DELETE ci FROM EidosClanInvite ci LEFT JOIN Principals per"
                    " ON ci.inv_inviter_pid = per.id "
                    " WHERE (ci.inv_inviter_clan_id = %s) "
                    "   AND (per.name=%s) "
                    "   AND (ci.inv_invitee_pid = %s) ", [clanIDInviter, playerNameInviter, myPlayer.PlayerID])
        if (c.rowcount < 1):
            self.LogReturn(strMethod, "kErrorInvalidInvitation - No valid invitation for %s from %s/%s!" % (myPlayer.PlayerID, clanIDInviter, playerNameInviter))
            return self.ErrorReturn(EidosResCodes.kErrorInvalidInvitation, dummyReturn)

        # Pack it up and send it home:
        retval = EidosResCodes.kNoError
        self.LogReturn(strMethod, str(retval))
        return retval
        
    # -------------------------------------------------------------------------
    def ClanBlockInvitation(self, clanIDInviter, playerNameInviter):
        """Blocks all further invitations from inviting clan.

        DDL Signature:
        
        uint32 ClanBlockInvitation( principalid clanIDInviter, string playerNameInviter );
				
        """
        strMethod = "BlockInvitation"
        self.LogMethodCall(strMethod, "clanIDInviter='%s', playerNameInviter='%s'"%(clanIDInviter, playerNameInviter))

        # Prep:     
        c = self.DB().cursor()
        dummyReturn = [0]
        
        # Get all our clan information from the database
        myClan = EidosClanProps()
        myPlayer = EidosClanMember()
        errorReturn = self.GetClanInfoByPlayerID(self.GetCurrentPID(), myClan, myPlayer)
        # If we have an error that is not that we are not in a clan
        if(errorReturn and errorReturn[0] != EidosResCodes.kErrorNotInClan):
            self.LogError(strMethod, errorReturn[1])
            return self.ErrorReturn(errorReturn[0], dummyReturn)
        
        self.LogPlayerInfo(strMethod, myPlayer.Player, myClan.ClanAbbr)
        
        # Resolve inviting player pid and clan:
        c.execute("SELECT p.id, c.clan_id FROM Principals p LEFT JOIN EidosClanPlayer cp ON (p.id=cp.pid) LEFT JOIN EidosClan c ON (cp.clan_id=c.clan_id) WHERE p.name=%s", [playerNameInviter])
        if (c.rowcount < 1):
            self.LogError(strMethod, "kErrorInvalidInvitation - No player record for playerNameInviter=%s!" % playerNameInviter)
            return self.ErrorReturn(EidosResCodes.kErrorInvalidInvitation, dummyReturn)
        (playerIDInviter, checkClanIDInviter) = c.fetchone()
        
        # Verify the inviter clan id actually exists
        if(checkClanIDInviter != clanIDInviter):
            self.LogError(strMethod, "kErrorInvalidInvitation - No clan record for clanIDInviter=%s!" % clanIDInviter)
            return self.ErrorReturn(EidosResCodes.kErrorInvalidInvitation, dummyReturn)
        
        # Set the block flag on the invitation
        c.execute("UPDATE EidosClanInvite SET inv_blocked='1' "
                    " WHERE (inv_inviter_clan_id = %s) "
                    "   AND (inv_inviter_pid     = %s) "
                    "   AND (inv_invitee_pid     = %s) ",
                    [ clanIDInviter, playerIDInviter, myPlayer.PlayerID] )
        if (c.rowcount < 1):
            self.LogError(strMethod, "kErrorInvalidInvitation - No valid invitation to update for %s from %s/%s!" % (myPlayer.PlayerID, clanIDInviter, playerNameInviter))
            return self.ErrorReturn(EidosResCodes.kErrorInvalidInvitation, dummyReturn)
        
        # Pack it up and send it home:
        retval = EidosResCodes.kNoError
        self.LogReturn(strMethod, str(retval))
        return retval



    # -------------------------------------------------------------------------
    def ClanSetInvitationStatus(self, isAcceptingInvites):
        """Sets overriding invitation status for player (from Toggle Status in UI).
				
        DDL Signature:
        
        uint32 SetInvitationStatus( bool isAcceptingInvites );
				
        """
        strMethod = "SetInvitationStatus"
        self.LogMethodCall(strMethod, "isAcceptingInvites=%s"%(isAcceptingInvites))

        # Prep:     
        c = self.DB().cursor()
        dummyReturn = [0]

        # Get all our clan information from the database
        myClan = EidosClanProps()
        myPlayer = EidosClanMember()
        errorReturn = self.GetClanInfoByPlayerID(self.GetCurrentPID(), myClan, myPlayer)
        # If we have an error that is not that we are not in a clan
        if(errorReturn and errorReturn[0] != EidosResCodes.kErrorNotInClan):
            self.LogError(strMethod, errorReturn[1])
            return self.ErrorReturn(errorReturn[0], dummyReturn)
        
        self.LogPlayerInfo(strMethod, myPlayer.Player, myClan.ClanAbbr)

        # Just do it:
        if (isAcceptingInvites): isAcceptingInvitesVal = '1'
        else: isAcceptingInvitesVal = '0'
        c.execute("UPDATE EidosClanPlayer SET clan_isacceptinginvites=%s WHERE (pid=%s)",
                [isAcceptingInvitesVal, myPlayer.PlayerID] )
        if (c.rowcount < 1):  # Make sure we changed something (FetchClanDesc should have created the record already!)
            self.LogError(strMethod, "kErrorGeneral - No valid EidosClanPlayer record for pid=%s!" % (myPlayer.PlayerID))
            return self.ErrorReturn(EidosResCodes.kErrorGeneral, dummyReturn)
        
        # Pack it up and send it home:
        retval = EidosResCodes.kNoError
        self.LogReturn(strMethod, str(retval))
        return retval



    #
    #
    # MEMBER MANAGEMENT:
    #
    #


    # -------------------------------------------------------------------------
    def ClanRemoveMember(self, playerToRemove):
        """Removes a member (soldier only) from the clan.
				
        DDL Signature:
        
        uint32 RemoveMember( principalid player );
				
        """
        strMethod = "RemoveMember"
        self.LogMethodCall(strMethod, "playerToRemove='%s'"%(playerToRemove))

        # Prep:     
        c = self.DB().cursor()
        dummyReturn = [0]
        
        # Get all our clan information from the database
        myClan = EidosClanProps()
        myPlayer = EidosClanMember()
        errorReturn = self.GetClanInfoByPlayerID(self.GetCurrentPID(), myClan, myPlayer)
        # Was there an error while getting clan information?
        if(errorReturn):
            self.LogError(strMethod, errorReturn[1])
            return self.ErrorReturn(errorReturn[0], dummyReturn)

        self.LogPlayerInfo(strMethod, myPlayer.Player, myClan.ClanAbbr)

        # Get playerToRemove member information:
        removeeClan = EidosClanProps()
        removeePlayer = EidosClanMember()
        errorReturn = self.GetClanInfoByPlayerID(playerToRemove, removeeClan, removeePlayer)
        # Was there an error while getting clan information?
        if(errorReturn):
            self.LogError(strMethod, errorReturn[1])
            return self.ErrorReturn(errorReturn[0], dummyReturn)

        # Ensure playerToRemove is in same clan:
        if (removeeClan.ClanID != myClan.ClanID):
            self.LogReturn(strMethod, "kErrorInvalidPlayer - Can't remove player %s who is in diff clan: %s." % (playerToRemove, removeeClan.ClanID))
            return self.ErrorReturn(EidosResCodes.kErrorInvalidPlayer, dummyReturn)

        # Make sure we have permission (officer or leader):
        if (myPlayer.ClanIsOfficer == False and  myPlayer.ClanIsLeader == False):
            self.LogReturn(strMethod, "kErrorPermissionDenied - Requesting player pid=%s is not officer or leader!" % myPlayer.PlayerID)
            return self.ErrorReturn(EidosResCodes.kErrorPermissionDenied, dummyReturn)

        # Make sure the playerToRemove is a lowly soldier:
        if (removeePlayer.ClanIsOfficer or removeePlayer.ClanIsLeader):
            self.LogReturn(strMethod, "kErrorPermissionDenied - playerToRemove %s is not a soldier!" % (playerToRemove))
            return self.ErrorReturn(EidosResCodes.kErrorPermissionDenied, dummyReturn)

        # Remove membership for kicked out player:
        c.execute("UPDATE EidosClanPlayer SET "
                " clan_id=NULL, clan_isleader='0', clan_isofficer='0', clan_alert_kickedout='0', clan_alert_newnotice='0' "
                " WHERE (pid=%s)",
                [removeePlayer.PlayerID] )
        if (c.rowcount < 1):
            self.LogError(strMethod, "Couldn't remove membership by %s for %s in %s" % (myPlayer.PlayerID, [removeePlayer.PlayerID], myClan.ClanID) )
            return self.ErrorReturn(EidosResCodes.kErrorPermissionDenied, dummyReturn)
        
        # Set up alert for player:
        c.execute("UPDATE EidosClanPlayer SET clan_alert_kickedout='1', clan_kickedoutby_player=%s "
                    " WHERE (pid=%s)", [myPlayer.PlayerID, removeePlayer.PlayerID] )
        
        # Pack it up and send it home:
        retval = EidosResCodes.kNoError
        self.LogReturn(strMethod, str(retval))
        return retval


        
    # -------------------------------------------------------------------------
    def ClanAbandonClan(self):
        """Leave your clan.
				
        DDL Signature:
        
        uint32 AbandonClan( );
				
        """
        strMethod = "AbandonClan"
        self.LogMethodCall(strMethod)

        # Prep:     
        c = self.DB().cursor()
        dummyReturn = [0]
        
        # Get all our clan information from the database
        myClan = EidosClanProps()
        myPlayer = EidosClanMember()
        errorReturn = self.GetClanInfoByPlayerID(self.GetCurrentPID(), myClan, myPlayer)
        # Was there an error while getting clan information?
        if(errorReturn):
            self.LogError(strMethod, errorReturn[1])
            return self.ErrorReturn(errorReturn[0], dummyReturn)

        self.LogPlayerInfo(strMethod, myPlayer.Player, myClan.ClanAbbr)

        # Special handling for Clan Leader:
        if (myPlayer.ClanIsLeader):
            # Make sure we're the only one in our clan:
            if (myClan.ClanMemberCount > 1):
                self.LogReturn(strMethod, "kErrorClanNotEmpty: Leader abandoning non-empty clan %s." % myClan.ClanID)
                return self.ErrorReturn(EidosResCodes.kErrorClanNotEmpty, dummyReturn)
            # Delete clan (we were the last guy) 
            #  (this should be in DB transaction with member removal, if such were supported)
            self.LogDetails(strMethod, "Leader abandoning clan - deleting clan %s." % myClan.ClanID)
            c.execute("DELETE FROM EidosClan WHERE (clan_id=%s)", [myClan.ClanID])
            if (c.rowcount < 1):
                self.LogError(strMethod, "WARNING!!! Couldn't delete EidosClan, clan_id=%s" % myClan.ClanID)

        # Delete clan invitations we've sent except blocked ones
        c.execute("DELETE FROM EidosClanInvite WHERE (inv_inviter_pid=%s) AND (inv_blocked<>'1')", [myPlayer.PlayerID])
        if (c.rowcount > 0):
            self.LogDetails(strMethod, "Deleted %d clan invitations." % (c.rowcount))
        
        # Remove membership from player record:
        c.execute("UPDATE EidosClanPlayer SET "
                    " clan_id=NULL, clan_isleader='0', clan_isofficer='0', clan_alert_kickedout='0', clan_alert_newnotice='0' "
                    " WHERE (pid=%s)", [myPlayer.PlayerID])
        if (c.rowcount < 1):
            self.LogError(strMethod, "WARNING!!! Couldn't remove membership, clan_id=%s, pid=%s" % (myClan.ClanID, myPlayer.PlayerID))
            return self.ErrorReturn(EidosResCodes.kErrorGeneral, dummyReturn)
        self.LogDetails(strMethod, "Cleared clan membership.")

        # Pack it up and send it home:
        retval = EidosResCodes.kNoError
        self.LogReturn(strMethod, str(retval))
        return retval



    # -------------------------------------------------------------------------
    def ClanPromote(self, playerToPromote):
        """Promote soldier to officer status, called by leader only.
				
        DDL Signature:
        
        uint32 Promote( principalid playerToPromote );
				
        """
        strMethod = "Promote"
        self.LogMethodCall(strMethod, "playerToPromote='%s'"%(playerToPromote))

        # Prep:     
        c = self.DB().cursor()
        dummyReturn = [0]
        
        # Get all our clan information from the database
        myClan = EidosClanProps()
        myPlayer = EidosClanMember()
        errorReturn = self.GetClanInfoByPlayerID(self.GetCurrentPID(), myClan, myPlayer)
        # Was there an error while getting clan information?
        if(errorReturn):
            self.LogError(strMethod, errorReturn[1])
            return self.ErrorReturn(errorReturn[0], dummyReturn)

        self.LogPlayerInfo(strMethod, myPlayer.Player, myClan.ClanAbbr)
            
        # Make sure we are the leader:
        if ( myPlayer.ClanIsLeader == False ):
            self.LogError(strMethod, "kErrorPermissionDenied - Player pid=%s is not leader!" % myPlayer.PlayerID)
            return self.ErrorReturn(EidosResCodes.kErrorPermissionDenied, dummyReturn)

        # Get member information:
        promoteeClan = EidosClanProps()
        promoteePlayer = EidosClanMember()
        errorReturn = self.GetClanInfoByPlayerID(playerToPromote, promoteeClan, promoteePlayer)
        # Was there an error while getting clan information?
        if(errorReturn):
            self.LogError(strMethod, errorReturn[1])
            return self.ErrorReturn(errorReturn[0], dummyReturn)
        
        # Make sure player is in our clan:
        if (promoteeClan.ClanID != myClan.ClanID):
            self.LogReturn(strMethod, "kErrorInvalidPlayer - playerToPromote name=%s is not in my clan (%s)!" % (playerToPromote, myClan.ClanID))
            return self.ErrorReturn(EidosResCodes.kErrorInvalidPlayer, dummyReturn)
        
        # Make sure player is a dirty little soldier:
        if (promoteePlayer.ClanIsOfficer or promoteePlayer.ClanIsLeader):
            self.LogReturn(strMethod, "kErrorInvalidPlayer - playerToPromote name=%s is not a soldier!" % (playerToPromote))
            return self.ErrorReturn(EidosResCodes.kErrorInvalidPlayer, dummyReturn)

        # Update target player rec to make him officer:
        c.execute("UPDATE EidosClanPlayer SET clan_isofficer='1', clan_isleader='0' WHERE (pid=%s)", [promoteePlayer.PlayerID] )
        if (c.rowcount < 1):
            self.LogError(strMethod, "Unable to update new officer record for player %s!" % (promoteeClan.ClanID) )
            return self.ErrorReturn(EidosResCodes.kErrorGeneral, dummyReturn)

        # Pack it up and send it home:
        retval = EidosResCodes.kNoError
        self.LogReturn(strMethod, str(retval))
        return retval


        
    # -------------------------------------------------------------------------
    def ClanDemote(self, playerToDemote):
        """Demote officer to soldier status, called by leader only.
				
        DDL Signature:
        
        uint32 Demote(  principalid playerToDemote );
				
        """
        strMethod = "Demote"
        self.LogMethodCall(strMethod, "playerToDemote='%s'"%(playerToDemote))

        # Prep:     
        c = self.DB().cursor()
        dummyReturn = [0]
        
        # Get all our clan information from the database
        myClan = EidosClanProps()
        myPlayer = EidosClanMember()
        errorReturn = self.GetClanInfoByPlayerID(self.GetCurrentPID(), myClan, myPlayer)
        # Was there an error while getting clan information?
        if(errorReturn):
            self.LogError(strMethod, errorReturn[1])
            return self.ErrorReturn(errorReturn[0], dummyReturn)

        self.LogPlayerInfo(strMethod, myPlayer.Player, myClan.ClanAbbr)
            
        # Make sure we are the leader:
        if ( myPlayer.ClanIsLeader == False ):
            self.LogError(strMethod, "kErrorPermissionDenied - Player pid=%s is not leader!" % myPlayer.PlayerID)
            return self.ErrorReturn(EidosResCodes.kErrorPermissionDenied, dummyReturn)

        # Get member information:
        demoteeClan = EidosClanProps()
        demoteePlayer = EidosClanMember()
        errorReturn = self.GetClanInfoByPlayerID(playerToDemote, demoteeClan, demoteePlayer)
        # Was there an error while getting clan information?
        if(errorReturn):
            self.LogError(strMethod, errorReturn[1])
            return self.ErrorReturn(errorReturn[0], dummyReturn)
        
        # Make sure player is in our clan:
        if (demoteeClan.ClanID != myClan.ClanID):
            self.LogError(strMethod, "kErrorInvalidPlayer - playerToDemote name=%s is not in my clan (%s)!" % (playerToDemote, myClan.ClanID))
            return self.ErrorReturn(EidosResCodes.kErrorInvalidPlayer, dummyReturn)
        
        # Make sure player is an officer:
        if ( demoteePlayer.ClanIsOfficer == False ):
            self.LogError(strMethod, "kErrorInvalidPlayer - playerToDemote name=%s is not an officer!" % (playerToDemote))
            return self.ErrorReturn(EidosResCodes.kErrorInvalidPlayer, dummyReturn)

        # Rip that son of a bitch's stripes off his arm and send him back to boot camp:
        c.execute("UPDATE EidosClanPlayer SET clan_isofficer='0', clan_isleader='0' WHERE (pid=%s)", [demoteePlayer.PlayerID] )
        if (c.rowcount < 1):
            self.LogError(strMethod, "Unable to update new soldier record for player %s!" % (demoteePlayer.PlayerID) )
            return self.ErrorReturn(EidosResCodes.kErrorGeneral, dummyReturn)

        # Pack it up and send it home:
        retval = EidosResCodes.kNoError
        self.LogReturn(strMethod, str(retval))
        return retval



    # -------------------------------------------------------------------------
    def ClanNewLeader(self, playerNewLeader):
        """Transfer leadership from player to another member of clan.
				
        DDL Signature:
        
        uint32 NewLeader(   string playerNewLeader );
				
        """
        strMethod = "NewLeader"
        self.LogMethodCall(strMethod, "playerNewLeader='%s'"%(playerNewLeader))

        # Prep:     
        c = self.DB().cursor()
        dummyReturn = [0]
        
        # Get all our clan information from the database
        myClan = EidosClanProps()
        myPlayer = EidosClanMember()
        errorReturn = self.GetClanInfoByPlayerID(self.GetCurrentPID(), myClan, myPlayer)
        # Was there an error while getting clan information?
        if(errorReturn):
            self.LogError(strMethod, errorReturn[1])
            return self.ErrorReturn(errorReturn[0], dummyReturn)

        self.LogPlayerInfo(strMethod, myPlayer.Player, myClan.ClanAbbr)
            
        # Make sure we are the leader:
        if ( myPlayer.ClanIsLeader == False ):
            self.LogError(strMethod, "kErrorPermissionDenied - Player pid=%s is not leader!" % myPlayer.PlayerID)
            return self.ErrorReturn(EidosResCodes.kErrorPermissionDenied, dummyReturn)

        # Get member information:
        newLeaderClan = EidosClanProps()
        newLeaderPlayer = EidosClanMember()
        errorReturn = self.GetClanInfoByPlayerID(playerNewLeader, newLeaderClan, newLeaderPlayer)
        # Was there an error while getting clan information?
        if(errorReturn):
            self.LogError(strMethod, errorReturn[1])
            return self.ErrorReturn(errorReturn[0], dummyReturn)
        
        # Make sure player is in our clan:
        if (newLeaderClan.ClanID != myClan.ClanID):
            self.LogReturn(strMethod, "kErrorInvalidPlayer - playerNewLeader name=%s is not in my clan (%s)!" % (playerNewLeader, myClan.ClanID))
            return self.ErrorReturn(EidosResCodes.kErrorInvalidPlayer, dummyReturn)
        
        # Make sure player is an officer:
        if (newLeaderPlayer.ClanIsOfficer == False):
            self.LogError(strMethod, "kErrorInvalidPlayer - playerNewLeader name=%s is not an officer!" % (playerNewLeader))
            return self.ErrorReturn(EidosResCodes.kErrorInvalidPlayer, dummyReturn)

        # Update target player rec to make him leader:
        c.execute("UPDATE EidosClanPlayer SET clan_isofficer='0', clan_isleader='1' WHERE (pid=%s)", [newLeaderPlayer.PlayerID] )
        if (c.rowcount < 1):
            self.LogError(strMethod, "Unable to update new leader record for player %s!" % (newLeaderPlayer.PlayerID) )
            return self.ErrorReturn(EidosResCodes.kErrorGeneral, dummyReturn)

        # Update my rec to make me officer:
        c.execute("UPDATE EidosClanPlayer SET clan_isofficer='1', clan_isleader='0' WHERE (pid=%s)", [myPlayer.PlayerID] )
        if (c.rowcount < 1):
            self.LogError(strMethod, "Unable to update former leader record %s! Created Inconsistent State!" % (myPlayer.PlayerID))
            return self.ErrorReturn(EidosResCodes.kErrorGeneral, dummyReturn)

        # Pack it up and send it home:
        retval = EidosResCodes.kNoError
        self.LogReturn(strMethod, str(retval))
        return retval



    #
    #
    # MISC:
    #
    #
        

    # -------------------------------------------------------------------------
    def ClanPostNotice(self, newNotice):
        """Post new clan notice, called by officers or leaders.
        
        Inserts a new notice first, shifting other notices down, deleting oldest.

        DDL Signature:
        
        uint32 PostNotice(  string newNotice );
				
        """
        strMethod = "PostNotice"
        self.LogMethodCall(strMethod, "newNotice='%s'"%(newNotice))

        # Prep:     
        c = self.DB().cursor()
        dummyReturn = [0]
        
        # Get all our clan information from the database
        myClan = EidosClanProps()
        myPlayer = EidosClanMember()
        errorReturn = self.GetClanInfoByPlayerID(self.GetCurrentPID(), myClan, myPlayer)
        # Was there an error while getting clan information?
        if(errorReturn):
            self.LogError(strMethod, errorReturn[1])
            return self.ErrorReturn(errorReturn[0], dummyReturn)

        self.LogPlayerInfo(strMethod, myPlayer.Player, myClan.ClanAbbr)

        # Make sure we have permission (officer or leader):
        if (myPlayer.ClanIsOfficer == False and myPlayer.ClanIsLeader == False):
            self.LogReturn(strMethod, "kErrorPermissionDenied - Posting player pid=%s is not officer or leader!" % myPlayer.PlayerID)
            return self.ErrorReturn(EidosResCodes.kErrorPermissionDenied, dummyReturn)

        # TODO: Sanitize notice:
        #  - Profanity check?  No, just leave the fucking clan if you don't like that shit.
        #  - Make sure text isn't too long?  Not now, because text only comes in from VKBD for now.

        # Set values to store in DB:
        bulletUTF8 = "%c%c%c" % (0xE2, 0x80, 0xA2)  # (UTF8 representation of BULLET char)
        # FIXME: front end support for UTF8 characters
        # newNoticeFrom = bulletUTF8 + playerRec[playerFields.index('c.clan_abbr')] + bulletUTF8 + playerRec[playerFields.index('p.name')]
        newNoticeFrom = myPlayer.Player
        newNoticeBody = newNotice

        # Build up SQL to insert notice1 and shift other notices backward (nidx=(nidx-1))
        c.execute("UPDATE EidosClan SET "
                    " clan_notice_from4=clan_notice_from3, clan_notice_body4=clan_notice_body3, "
                    " clan_notice_from3=clan_notice_from2, clan_notice_body3=clan_notice_body2, "
                    " clan_notice_from2=clan_notice_from1, clan_notice_body2=clan_notice_body1, "
                    " clan_notice_from1=%s, clan_notice_body1=%s "
                    " WHERE (EidosClan.clan_id=%s) ", [newNoticeFrom, newNoticeBody, myClan.ClanID] )
        if (c.rowcount < 1):
            self.LogError(strMethod, "Unable to update clan record %s with notice!" % (myClan.ClanID))
            return self.ErrorReturn(EidosResCodes.kErrorGeneral, dummyReturn)
        self.LogDetails(strMethod, "Posted notice.")

        # Notify all clan members of new notice:
        c.execute("SELECT cp.pid FROM EidosClanPlayer cp WHERE cp.clan_id=%s", [myClan.ClanID])
        memberIDRecs = c.fetchall()
        if (len(memberIDRecs) > 0):
            memberIDs = map( lambda prm: str(prm[0]), memberIDRecs)
            strCSLMemberIDs = ",".join(memberIDs)
            c.execute("UPDATE EidosClanPlayer SET clan_alert_newnotice='1' WHERE pid IN (%s)" % strCSLMemberIDs)
            self.LogDetails(strMethod, "Set new notice alert for %d clan member(s)." % (len(memberIDs)))

        # Pack it up and send it home:
        retval = EidosResCodes.kNoError
        self.LogReturn(strMethod, str(retval))
        return retval

    def ClanGetMyNotices(self):
        """Get notices for my clan

        DDL Signature:
        
        uint32 ClanGetMyNotices(out std_list<EidosClanNotice> outNotices);
				
        """
        strMethod = "GetMyNotices"
        self.LogMethodCall(strMethod)

        # Prep:     
        c = self.DB().cursor()
        outNotices = []
        dummyReturn = [0, map(lambda(m): m.Serialize(), outNotices)]
        
        # Get all our clan information from the database
        myClan = EidosClanProps()
        myPlayer = EidosClanMember()
        errorReturn = self.GetClanInfoByPlayerID(self.GetCurrentPID(), myClan, myPlayer)
        # Was there an error while getting clan information?
        if(errorReturn):
            self.LogError(strMethod, errorReturn[1])
            return self.ErrorReturn(errorReturn[0], dummyReturn)

        self.LogPlayerInfo(strMethod, myPlayer.Player, myClan.ClanAbbr)
        
        # Load clan notices:
        clanNoticeFields = ["c.clan_notice_from1", "c.clan_notice_body1", "c.clan_notice_from2", "c.clan_notice_body2", "c.clan_notice_from3", "c.clan_notice_body3", "c.clan_notice_from4", "c.clan_notice_body4"]
        c.execute("SELECT "+",".join(clanNoticeFields)+" FROM EidosClan c WHERE (c.clan_id=%s)", [myClan.ClanID])
        clanRec = c.fetchone()
        # If no clan rec, error back:
        if (clanRec == None):
            self.LogError(strMethod, "Inconsistent state! Player %s is in clan, but no clan rec found." % myPlayer.PlayerID)
            return self.ErrorReturn(EidosResCodes.kErrorGeneral, dummyReturn)
        
        for nidx in range (1, 5):     # (Add notices if there are any)
            if (len(clanRec[clanNoticeFields.index('c.clan_notice_from%d' % nidx)]) > 0):
                notice = EidosClanNotice()
                notice.Deserialize((clanRec[clanNoticeFields.index('c.clan_notice_from%d' % nidx)], clanRec[clanNoticeFields.index('c.clan_notice_body%d' % nidx)]))
                outNotices.append(notice)
        
        # Mark notices read:
        c.execute("UPDATE EidosClanPlayer SET clan_alert_newnotice='0' "
                    " WHERE (pid=%s)", [myPlayer.PlayerID])
        if (c.rowcount < 1):
            self.LogDetails(strMethod, "(probably harmless): Unable to update clan player record %s." % (myPlayer.PlayerID))
        
        # Pack it up and send it home:
        retval = (EidosResCodes.kNoError, map(lambda(m): m.Serialize(), outNotices))
        self.LogReturn(strMethod, str(retval))
        return retval

        
    # -------------------------------------------------------------------------
    def ClanMarkAlertKickedRead(self):
        """Mark player's 'you been kicked out' alert as read, so alert only shows once.
        
        DDL Signature:
        
        uint32 MarkAlertKickedRead( );
				
        """
        strMethod = "MarkAlertKickedRead"
        self.LogMethodCall(strMethod)

        # Prep:     
        c = self.DB().cursor()
        dummyReturn = [0]
        
        # Get all our clan information from the database
        myClan = EidosClanProps()
        myPlayer = EidosClanMember()
        errorReturn = self.GetClanInfoByPlayerID(self.GetCurrentPID(), myClan, myPlayer)
        # Was there an error while getting clan information?
        if(errorReturn and errorReturn[0] != EidosResCodes.kErrorNotInClan):
            self.LogError(strMethod, errorReturn[1])
            return self.ErrorReturn(errorReturn[0], dummyReturn)

        self.LogPlayerInfo(strMethod, myPlayer.Player, myClan.ClanAbbr)

        # Mark alert read:
        c.execute("UPDATE EidosClanPlayer SET clan_alert_kickedout='0', clan_kickedoutby_player=NULL "
                    " WHERE (pid=%s)", [myPlayer.PlayerID])
        if (c.rowcount < 1):
            self.LogDetails(strMethod, "(probably harmless): Unable to update clan player record %s." % (myPlayer.PlayerID))

        # Pack it up and send it home:
        retval = EidosResCodes.kNoError
        self.LogReturn(strMethod, str(retval))
        return retval

    #
    #
    # PC CD Key Functions:
    #
    #
        

    # -------------------------------------------------------------------------
    def CheckForValidCDKey(self, inCDKey):
        c = self.DB().cursor()
        c.execute('SELECT cdkey FROM PCCDKey WHERE cdkey = %s', inCDKey)
        # first check to see if we have a cdkey
        if (c.rowcount == 0):
            return False
        # then check to see if we aren't in the currently playing list
        # or if we are, that we're using our pid
        pid = self.GetCurrentPID()
        c.execute('SELECT pid,cdkey FROM PCOnlineKeys WHERE cdkey = %s', inCDKey)
        row = c.fetchone()
        if (row != None):
            if (row[0] != pid):
                return False
        # add our pid and cdkey to the database
        c.execute('INSERT IGNORE INTO PCOnlineKeys (pid,cdkey) VALUES (%s,%s)', [pid,inCDKey])
        c.execute('UPDATE PCOnlineKeys SET cdkey = %s WHERE pid = %s', [inCDKey,pid])
        return True

class LogoutPreHook(Hook.PreHook):
    def __init__(self,protocol):
        Hook.PreHook.__init__(self,protocol)
        self.proto = protocol


    def __call__(self,methodname,argl,argd):
        print 'TTLMatchMaking.LogoutPreHook',(methodname,argl,argd)
        c = self.DB().cursor()
        c.execute('DELETE FROM PCOnlineKeys WHERE pid = %s', self.GetCurrentPID())

