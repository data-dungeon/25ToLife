# 
# (c) Copyright 1998-2005, Quazal Technologies Inc.
# ALL RIGHTS RESERVED
# 
# Net-Z, Eterna, Q-Platform, Duplicated Object and Duplication Space 
# are trademarks of Quazal.
# 
# Use of this software indicates acceptance of the End User licence
# Agreement for this product. A copy of the EULA is available from
# www.quazal.com.
#

# Number of seconds to wait before cleaning up session, if host is not an active participant
SESSION_END_WAIT = 10

# Number of seconds to wait for stats to come in after round is over
ROUND_END_WAIT = 20

# Control script period, in seconds
GATHERING_MONITOR_TIMER = 5

try:
    import ParticipationPolicySample
except ImportError:
    import ParticipationPolicies as ParticipationPolicySample

# Custom participation policy for TTL
#
# Standard participation policies only allow the owner to send invitations.
# This custom policy also allows the host to send invitations.
#
# This policy also uses four participation states in the ParticipationStatus
# table (instead of the usual three):
#
#   - invited (pending=1, active=0)
#      -> follows a call to Invite()
#   - participant (pending=0, active=1)
#      -> follows a call to Participate()
#   - invited participant (pending=1, active=1)
#      -> follows a call to AcceptInvitation()
#   - inactive participant (pending=0, active=0)
#      -> follows a call to CancelParticipation(), EndPartcipation(), or a disconnect
#
# This arrangement allows invitations to persist for the life of the
# gathering.  When someone disconnects or ends their participation to a
# gathering to which they were originally invited, the invitation remains for
# as long as the gathering exists: their participation state changes from 'invited
# participant' to 'invited'.
#
# When no active participants remain, the gathering is automatically cleaned
# up.
#
class TTLOngoingParticipation(ParticipationPolicySample.OpenParticipation):

    # Used to check if caller is allowed to issue an Invite/CancelInvitation
    def _checkCallerAccess(self,gid):
        c = self.DB().cursor()
        c.execute('SELECT ownerpid,hostpid FROM Gatherings WHERE id = %s',gid)
        try:
            allowed_PIDs = list(c.fetchone())
        except:
            self.logger.error('Query fails - gathering does not exist.')
            return False
        import BuiltInPrincipals
        allowed_PIDs.append(BuiltInPrincipals.RendezVous)
        if self.GetCurrentPID() not in allowed_PIDs:
            self.logger.error('Query fails - caller is neither gathering owner, host or backend')
            return False
        return True

    # Override base class Invite to enable custom permissions check
    def Invite(self,uiPolicyParam,gid,pid,event,message):
        if not self._checkCallerAccess(gid):
            return False
        c = self.DB().cursor()
        c.execute('INSERT INTO ParticipationStatus(gid,pid,pending,active,message) VALUES(%s,%s,0,0,%s)', [gid,pid,message])
        try:
            c.execute('UPDATE ParticipationStatus SET pending = 1, message = %s WHERE gid=%s AND pid=%s and pending=0', [message,gid,pid])            
        except:
            # Duplicate entry.
            return False
        return c.rowcount == 1

    # Override base class CancelInvitation to enable custom permissions check
    def CancelInvitation(self,uiPolicyParam,gid,pid,event,message):
        if not self._checkCallerAccess(gid):
            return False
        c = self.DB().cursor()
        c.execute('DELETE FROM ParticipationStatus WHERE gid=%s AND pid=%s AND pending=1', [gid,pid])
        return c.rowcount == 1

    # Override to allow an 'invited participant' state where pending = 1 and active = 1
    def AcceptInvitation(self,uiPolicyParam,gid,pid,event,message):
        c = self.DB().cursor()
        c.execute('UPDATE ParticipationStatus SET active = 1, message = %s WHERE gid=%s AND pid=%s AND pending=1', [message,gid,pid])
        return c.rowcount == 1

    # Override as to not reset pending status, i.e. invitation persists beyond a call to Participate()
    def Participate(self,uiPolicyParam,gid,pid,event,message):
        #print 'Participating pid = ', pid
        #print 'Participating gid = ', gid
        c = self.DB().cursor()
        c.execute('SELECT * FROM ParticipationStatus WHERE gid=%s AND pid=%s', [gid,pid])
        #print 'Participating SELECT rowcount =', c.rowcount
        if c.rowcount > 0:
            c.execute('UPDATE ParticipationStatus SET active = 1, message = %s WHERE gid=%s AND pid=%s', [message,gid,pid])
            #print 'Participating UPDATE rowcount = ', c.rowcount
            return True
        # not yet participating...
        c.execute('INSERT INTO ParticipationStatus(gid,pid,pending,active,message) VALUES(%s,%s,0,1,%s)', [gid,pid,message])
        #print 'Participating INSERT rowcount =', c.rowcount
        return c.rowcount == 1
        
    # Override to implement automatic gathering cleanup, a la OngoingParticipation
    def EndParticipation(self,uiPolicyParam,gid,pid,event,message):
        # Set participant's status to inactive
        c = self.DB().cursor()
        c.execute('UPDATE ParticipationStatus SET active = 0, message = %s WHERE gid=%s AND pid=%s AND active = 1', [message,gid,pid])

        #last to leave cleans up the gathering
        lstActiveParticipants = self.GetParticipants(uiPolicyParam,gid,True)
        if len(lstActiveParticipants) == 0:
            if not DeleteSessionGathering(gid):
                self.logger.error('Failed to insert gathering %d in the Deletions table' % gid)
                return False

        c.execute('SELECT hostpid FROM Gatherings WHERE id=%s', gid)
        if c.rowcount==1:   # assert(c.rowcount==1) ?
            hostpid = c.fetchone()[0]
            if hostpid not in lstActiveParticipants:
                # this situation should not occur, except if host is in the
                # process of migrating. Set an expiration timestamp for the
                # condition to change (or not).
                c.execute('UPDATE Gatherings SET expiration = DATE_ADD(NOW(),INTERVAL %s SECOND) WHERE id=%s', (SESSION_END_WAIT, gid));
                assert(c.rowcount==1)

        # Not sure what the following line is here for - PJT
        c.execute('DELETE FROM ParticipationStatus WHERE gid=%s AND pid=%s and pending=0', [gid,pid])
        return True
        
    # redirect to EndParticipation - we want to keep everyone who's ever
    # participated in the ParticipationStatus table.  This enables us to
    # keep track of who used to be invited.
    def CancelParticipation(self,uiPolicyParam,gid,pid,event,message):
        return self.EndParticipation(uiPolicyParam,gid,pid,event,message)

# Custom ParticipationPolicy for round gatherings.
#
# This policy is similar to OpenSessionParticipation, except that the round
# ends as soon as one of the participants calls EndParticipation().
class TTLOpenSessionParticipation(ParticipationPolicySample.OpenSessionParticipation):

    # Override default OpenSessionParticipation.EndParticipation.  Instead of
    # waiting for the last participant to EndParticipation(), we immediately
    # consider the round to be over as soon as any participant calls it.  Note
    # that this won't affect stats processing, as the competition service
    # always waits for all reports to come in anyway.
    def EndParticipation(self,uiPolicyParam,gid,pid,event,message):
        if ParticipationPolicySample.OpenSessionParticipation.EndParticipation(self,uiPolicyParam,gid,pid,event,message):
            # unless this really is a rerouted Cancel or Disconnect...
            import ParticipationEvents
            if event==ParticipationEvents.EndParticipation:
                # Go ahead and set the gathering state to HasStarted|HasFinished
                return self.SetGatheringStateToFinished(gid)
        return False

    # This method sets the gathering state to finished AND sets an expiration
    # timestamp on the gathering, so that it can be cleaned up in a timely
    # fashion, event if there are missing stats reports.
    def SetGatheringStateToFinished(self,gid):
        # Assume caller is the gathering owner if last one left in the gathering.
        import MatchMaking
        proto = MatchMaking.MatchMakingProtocol()
        (bResult,oldState) = proto.GetState(gid)
        import GatheringStates
        if bResult and (oldState & GatheringStates.HasFinished) == 0:
            newState = oldState | GatheringStates.HasFinished
            self.logger.debug('SetGatheringStateToFinished(): setting gathering %s state from %s to %s' % (gid,oldState,newState))
            # We need to pretend the SetState() call is being made with
            # backend credentials rather than the caller's.
            import BuiltInPrincipals
            import quazal
            quazal.SecurityContextManager.GetInstance().Push(BuiltInPrincipals.RendezVous,1)
            bResult = proto.SetState(gid, newState)
            quazal.SecurityContextManager.GetInstance().Pop()

            # Set an expiry timestamp for getting stats reports
            c = proto.DB().cursor()
            c.execute('UPDATE Gatherings SET expiration = DATE_ADD(NOW(),INTERVAL %s SECOND) WHERE id=%s', (ROUND_END_WAIT, gid));
            assert(c.rowcount==1)
        return bResult

# This control script periodically ensures finished gatherings do not wait for
# stats reports forever.
import ControlScript

class FinishedGatheringMonitor(ControlScript.BaseClass):
    
    def OnStart(self):
        self.logger.info('In FinishedGatheringMonitor.OnStart()')
        self.SetTimer(1000*GATHERING_MONITOR_TIMER, self.ProcessGatherings)

    def OnStop(self):
        self.logger.info('In FinishedGatheringMonitor.OnStop()')
        # Nothing special to do.

    def ProcessGatherings(self):
        #self.logger.info('In FinishedGatheringMonitor.ProcessGatherings()')
        import MatchMaking
        import CompetitionStates
        import GatheringStates

        proto = MatchMaking.MatchMakingProtocol()
        c = proto.DB().cursor()

        # Check round gatherings.
        c.execute('SELECT id FROM Gatherings G LEFT JOIN CompetitionCalendar C ON G.id = C.event '
                  'WHERE G.state = %s AND C.state = %s AND G.expiration<NOW() AND G.expiration != 0',
                  (GatheringStates.HasStarted|GatheringStates.HasFinished,CompetitionStates.Finished))
        if c.rowcount:
            lstGatherings = c.fetchall()
            for gid in lstGatherings:
                self.ReportStatsIfNecessary(gid[0])

        # Check session gatherings.
        c.execute('SELECT id FROM Gatherings G WHERE G.type=%s AND G.expiration<NOW() AND G.expiration != 0','TTLSession')
        if c.rowcount:
            lstGatherings = c.fetchall()
            for gid in map(lambda x: x[0],lstGatherings):
                #print 'processing session gathering', gid
                c.execute('SELECT hostpid FROM Gatherings WHERE id=%s', gid)
                if c.rowcount==1:   # assert(c.rowcount==1) ?
                    hostpid = c.fetchone()[0]
                    lstActiveParticipants = proto.GetParticipants(gid)
                    if hostpid in lstActiveParticipants:
                        # remove expiration timestamp
                        self.logger.debug('Session %d gathering host is active, resetting expiration timeout' % gid)
                        c.execute('UPDATE Gatherings SET expiration = 0 WHERE id=%s', (gid));
                    else:
                        self.logger.info('Session gathering %d has expired, deleting' % gid)
                        DeleteSessionGathering(gid)
            
        # always return True
        return True

    def ReportStatsIfNecessary(self,gid):
        import StatisticFlags
        import MatchMaking
        proto = MatchMaking.MatchMakingProtocol()
        (result, stats) = proto.GetStats(gid)
        lstParticipants = proto.GetParticipants(gid,False)
        for pid in lstParticipants:
            if len(stats)==0 or pid not in map(lambda x: x[0], stats):
                self.logger.info('Round gathering %d has expired, reporting missing stats for pid %d' %(gid,pid))
                proto.ReportStats(gid, [(pid, StatisticFlags.Disconnected, [])])

def DeleteSessionGathering(gid):
    import BackEndDB
    c = BackEndDB.ConnectIfRequired().cursor()
    # clear the ParticipationStatus table for this gathering - avoid the 'dangling invitation' problem
    c.execute('DELETE FROM ParticipationStatus WHERE gid=%s', gid)
    # MatchMaking service will clean up this gathering for us.
    import DeletionReasons
    c.execute('INSERT INTO Deletions '
              '(gid, pid, reason)'
              'SELECT id,ownerpid,%s FROM Gatherings WHERE id = %s', [DeletionReasons.Finished, gid])
    return c.rowcount==1
