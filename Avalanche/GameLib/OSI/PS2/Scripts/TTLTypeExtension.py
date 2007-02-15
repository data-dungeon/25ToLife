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

import TypeExtension

class TTLTypeExtension(TypeExtension.BaseClass):

    def GetTables(self):
        return ['TTLSessions']

    def Save(self,type,id,specific):
        print 'Saving session %d ' % id
        print 'Specific = ', specific, 'len = ', len(specific)
        assert(len(specific)==19) 
        c = self.DB().cursor()
        args = [id,] + list(specific)
        c.execute('REPLACE INTO TTLSessions(gid,levelName,levelIndex,gameType,timeLimit,scoreLimit,tagLimit,bestOfSeries,maxPlayers,reservedPlayers,totalConnectedPlayers,isCustomQuery,isRanked,isPrivate,playersRespawn,friendlyFire,apprehension,arcadeDamage,selfDamage,alternatingTeams) VALUES (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)',args)

    def Load(self,type,id):
        c = self.DB().cursor()
        c.execute('SELECT levelName,levelIndex,gameType,timeLimit,scoreLimit,tagLimit,bestOfSeries,maxPlayers,reservedPlayers,totalConnectedPlayers,isCustomQuery,isRanked,isPrivate,playersRespawn,friendlyFire,apprehension,arcadeDamage,selfDamage,alternatingTeams FROM TTLSessions WHERE gid = %s',(id))
        specific = c.fetchone()
        print 'Loading session %d ' % id
        print 'Specific = ', specific
        return specific

    def Delete(self,sType,nGID):
        print 'TTLSession.Delete',(sType,nGID)
        c = self.DB().cursor()
        if sType == 'TTLSession':
            c.execute('DELETE FROM TTLSessions WHERE gid = %s',(nGID,))
            return c.rowcount==1
        return False
    