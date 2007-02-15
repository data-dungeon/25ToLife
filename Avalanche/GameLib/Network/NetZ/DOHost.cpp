/*
**
**  File:   DOHost.cpp
**  Date:   October 14, 2003
**  By:     Bryant Collard
**  Desc:   Implementation of the host distributed object.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/DOHost.cpp $
**  $Revision: #52 $
**  $DateTime: 2005/08/29 20:13:34 $
**    $Author: Bryant_Collard $
**
*/

// Precompiled header
#include "Network/NetworkPCH.h"

// My header
#include "Network/NetZ/DOHost.h"

// Engine headers
#include "Math/fastrand.h"

// Gamelib headers
#include "GameHelper/Message.h"
#include "cmdcon/cmdcon.h"
#include "GameHelper/LogUtil.h"
#include "GameHelper/DebugOutput.h"

// Local headers
#include "Network/Session.h"
#include "Network/NetZ/DOActor.h"

// Static members
DOHost* DOHost::s_localHost = NULL;
uint8 DOHost::s_lockId = 0;
DOHost::CMigrationLockRequest DOHost::s_migrationLock;
bool DOHost::s_acceptImmigration = false;
bool DOHost::s_faultDetected = false;

// Declare handle curator data.
DeclareHandleCurator(DOHost, HOST_HANDLE_MASK_WIDTH);

// Each new request for a lock is assigned an id. When a host checks to see if
// has been granted the lock, it checks to see if the id the grantor is using
// matches the id of the current request. The purpose of this check is to
// prevent some bizarre problem where a host requests a lock, recinds the
// request then requests it again and then detects that some other host has
// granted it the lock. If the grantor is responding to the first request, it
// is possible that some better host might have requested the lock in the mean
// time and everybody could be confused. So, the id is there to make sure the
// lock is being granted to the right request. It seems that cycling through
// 15 ids should be enough to prevent any problems. Define a couple of macros
// to aid in manipulating ids.
#define GET_REQUEST_ID(v)    ((v) & 0x0F)
#define GET_GRANT_ID(v)      (((v) & 0xF0) >> 4)
#define SET_REQUEST_ID(r, v) r &= 0xF0; r |= (v) & 0x0F;
#define SET_GRANT_ID(r, v)   r &= 0x0F; r |= ((v) << 4) & 0xF0;

// Aide in dumping debug info.
static const char* GetActorName(CActor* i_actor)
{
	if (i_actor != NULL)
	{
		if ((i_actor->InstanceName() != NULL) &&
				(*i_actor->InstanceName() != '\0'))
			return(i_actor->InstanceName());
		if ((i_actor->Name() != NULL) && (*i_actor->Name() != '\0'))
			return(i_actor->Name());
	}
	return("UNNAMED");
}

#pragma warning(disable : 4355)  // Allow this pointer in initializer list
/******************************************************************************/
/******************************************************************************/
DOHost::DOHost() : d_handleCurator(this)
{
}
#pragma warning(default : 4355)

/******************************************************************************/
/******************************************************************************/
void DOHost::Initialize(void)
{
	d_state.d_acceptImmigration = s_acceptImmigration;
#ifdef LATENCY_COMPENSATION
	d_state.d_hostTimePerSessionTime = 0.0f;
	d_state.d_hostVsSessionIntercept = 0.0f;
#endif
	d_lock.d_ids = 0;
	d_lock.d_grantee = INVALID_DOHANDLE;
}

/******************************************************************************/
/******************************************************************************/
void DOHost::InitDO(void)
{
	GetMessageOwner().SetOwnerName("Host", GetName());
}

/******************************************************************************/
/******************************************************************************/
void DOHost::StateChanged(void)
{
	g_debugOutput.TSPrint(g_debugOutput.Category("MIG"),
			"Host %s state changed, accept = %s", GetName(),
			d_state.d_acceptImmigration ? "true" : "false");
}

/******************************************************************************/
/******************************************************************************/
bool DOHost::WantsLock(void)
{
	return(GET_REQUEST_ID(d_lock.d_ids) != 0);
}

/******************************************************************************/
/******************************************************************************/
bool DOHost::HasLock(void)
{
	return(d_lock.d_grantee == GetHandle());
}

/******************************************************************************/
/******************************************************************************/
void DOHost::OperationEnd(DOOperation* i_operation)
{
	if (i_operation->GetType() == Operation::AddToStore)
	{
		uint8 category = g_debugOutput.Category("NET");
		Station::Ref station(GetMasterStation());
		if (station.IsValid())
		{
			g_debugOutput.TSPrint(category, "%s station %s added to session",
					station->IsLocal() ? "Local" : "Remote",
					station->GetProcessName().CStr());
			qList<StationURL> urlList;
			station->GetStationURLs(&urlList);
			qList<StationURL>::iterator iter;
			iter = urlList.begin();
			int i = 0;
			while(iter != urlList.end())
			{
				g_debugOutput.TSPrint(category, "  URL %d - %s", i++, iter->CStr());
				iter++;
			}
		}
		else
		{
			g_debugOutput.TSPrint(category,
					"Station(Invalid Ref) added to session");
		}

		if (((AddToStoreOperation*)i_operation)->AddToDuplicaSelection())
			g_session.AddRemoteHost(this);
	}
	else if (i_operation->GetType() == Operation::RemoveFromStore)
	{
		uint8 category = g_debugOutput.Category("NET");
		Station::Ref station(GetMasterStation());
		if (station.IsValid())
		{
			g_debugOutput.TSPrint(category, "%s station %s removed from session",
					station->IsLocal() ? "Local" : "Remote",
					station->GetProcessName().CStr());
		}
		else
		{
			g_debugOutput.TSPrint(category,
					"Station(Invalid Ref) removed from session");
		}

		if (IsADuplica())
			g_session.RemoveRemoteHost(this);
	}
}

/******************************************************************************/
/******************************************************************************/
bool DOHost::ApproveFaultRecovery()
{
	s_faultDetected = true;
	return(false);
}

/******************************************************************************/
/******************************************************************************/
uint32 DOHost::GetFaultStatus(void)
{
	return(s_faultDetected ? 1 : 0);
}

/******************************************************************************/
/******************************************************************************/
uint32 DOHost::GetIPAddress(void) const
{
	Station::Ref station(GetMasterStation());
	if (station.IsValid())
		return(station->GetIPAddress(0));
	return(0);
}

/******************************************************************************/
/******************************************************************************/
uint16 DOHost::GetPortNumber(void) const
{
	Station::Ref station(GetMasterStation());
	if (station.IsValid())
		return(station->GetPortNumber(0));
	return(0);
}

/******************************************************************************/
/******************************************************************************/
const char* DOHost::GetName(void) const
{
	Station::Ref station(GetMasterStation());
	if (station.IsValid())
		return(station->GetProcessName().CStr());
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
DOHost* DOHost::CreateLocal(void)
{
	ASSERTS(s_localHost == NULL, "Attempting to create multiple local hosts");

	s_migrationLock.Desire(false);

	s_localHost = DOHost::Create();
	s_localHost->Initialize();
	s_localHost->Publish();

	PromotionReferee::RegisterAffinityCallback(ComputeAffinity);
	PromotionReferee::RegisterAffinityValidationCallback(ValidateAffinity);

	return(s_localHost);
}

/******************************************************************************/
/******************************************************************************/
DOHost* DOHost::GetLocal(void)
{
	return(s_localHost);
}

/******************************************************************************/
/******************************************************************************/
DOHandle DOHost::GetLocalHandle(void)
{
	ASSERTS(s_localHost != NULL, "Local host not set");
	return(s_localHost->GetHandle());
}

/******************************************************************************/
/******************************************************************************/
bool DOHost::LocalHasLock(void)
{
	ASSERTS(s_localHost != NULL, "Local host not set");
	return(s_localHost->HasLock());
}

/******************************************************************************/
/******************************************************************************/
bool DOHost::LocalGrantedLock(void)
{
	ASSERTS(s_localHost != NULL, "Local host not set");
	return(s_localHost->d_lock.d_grantee != INVALID_DOHANDLE);
}

/******************************************************************************/
/******************************************************************************/
bool DOHost::LockGrantedToLocal(DOHost* i_host)
{
	ASSERTS(s_localHost != NULL, "Local host not set");
	ASSERT_PTR(i_host);
	return((i_host->d_lock.d_grantee == s_localHost->GetHandle()) &&
			(GET_GRANT_ID(i_host->d_lock.d_ids) ==
			GET_REQUEST_ID(s_localHost->d_lock.d_ids)));
}

/******************************************************************************/
/******************************************************************************/
bool DOHost::SetLockState(bool i_desired, DOHost* i_grantee)
{
	ASSERTS(s_localHost != NULL, "Local host not set");
	if (i_desired != (GET_REQUEST_ID(s_localHost->d_lock.d_ids) != 0))
	{
		if (i_desired)
		{
			if (s_lockId == 0xF)
				s_lockId = 1;
			else
				s_lockId++;
			SET_REQUEST_ID(s_localHost->d_lock.d_ids, s_lockId);
		}
		else
			SET_REQUEST_ID(s_localHost->d_lock.d_ids, 0);
		if (i_grantee == NULL)
		{
			SET_GRANT_ID(s_localHost->d_lock.d_ids, 0);
			s_localHost->d_lock.d_grantee = INVALID_DOHANDLE;
		}
		else
		{
			SET_GRANT_ID(s_localHost->d_lock.d_ids,
					GET_REQUEST_ID(i_grantee->d_lock.d_ids));
			s_localHost->d_lock.d_grantee = i_grantee->GetHandle();
		}
	}
	else if (i_grantee == NULL)
	{
		if (s_localHost->d_lock.d_grantee == INVALID_DOHANDLE)
			return(false);
		SET_GRANT_ID(s_localHost->d_lock.d_ids, 0);
		s_localHost->d_lock.d_grantee = INVALID_DOHANDLE;
	}
	else if ((GET_GRANT_ID(s_localHost->d_lock.d_ids) !=
			GET_REQUEST_ID(i_grantee->d_lock.d_ids)) ||
			(s_localHost->d_lock.d_grantee != i_grantee->GetHandle()))
	{
		SET_GRANT_ID(s_localHost->d_lock.d_ids,
				GET_REQUEST_ID(i_grantee->d_lock.d_ids));
		s_localHost->d_lock.d_grantee = i_grantee->GetHandle();
	}
	else
		return(false);

//	g_debugOutput.TSPrint(g_debugOutput.Category("LOCK"),
//			"Grantor = %s, Grantee = %s, ReqId = %d, GrantId = %d",
//			s_localHost->GetName(), (i_grantee != NULL) ? i_grantee->GetName() :
//			"NONE", GET_REQUEST_ID(s_localHost->d_lock.d_ids),
//			GET_GRANT_ID(s_localHost->d_lock.d_ids));

	s_localHost->d_lock.RequestUpdate();
	return(true);
}

/******************************************************************************/
/******************************************************************************/
void DOHost::PropagateUpdate(void)
{
	if (s_localHost != NULL)
	{
		if (!s_migrationLock.HaveLock())
			s_migrationLock.Desire(s_localHost->NeedToManageActors());
		else
			s_migrationLock.Desire(s_localHost->ManageActors());

		s_localHost->Update();
	}
	else
		s_migrationLock.Desire(false);
}

/******************************************************************************/
/******************************************************************************/
void DOHost::Terminate(void)
{
	if (s_localHost != NULL)
		s_migrationLock.Desire(false);
	s_localHost = NULL;
}

/******************************************************************************/
/******************************************************************************/
void DOHost::SendFlag(uint32 i_hash, DOHandle i_sender)
{
	g_messageDispatcher.SendMessageToLocalHost(i_hash, NULL, GetSender(i_sender));
}

/******************************************************************************/
/******************************************************************************/
void DOHost::PostFlag(uint32 i_hash, DOHandle i_sender)
{
	g_messageDispatcher.PostMessageToLocalHost(i_hash, NULL, GetSender(i_sender));
}

/******************************************************************************/
/******************************************************************************/
void DOHost::SendBuffer(uint32 i_hash, Buffer i_data, DOHandle i_sender)
{
	CMessageGroup* group = g_messageDispatcher.GetMessageGroup(i_hash);
	if (group != NULL)
	{
		ASSERT_PTR(MessageOwner());
		g_messageDispatcher.SendMessageToLocalHost(i_hash,
				group->Unpack((void*)i_data.GetContentPtr(), *MessageOwner()),
				GetSender(i_sender));
	}
}

/******************************************************************************/
/******************************************************************************/
void DOHost::PostBuffer(uint32 i_hash, Buffer i_data, DOHandle i_sender)
{
	CMessageGroup* group = g_messageDispatcher.GetMessageGroup(i_hash);
	if (group != NULL)
	{
		ASSERT_PTR(MessageOwner());
		g_messageDispatcher.PostMessageToLocalHost(i_hash,
				group->Unpack((void*)i_data.GetContentPtr(), *MessageOwner()),
				GetSender(i_sender));
	}
}

/******************************************************************************/
/******************************************************************************/
CMessageOwner* DOHost::MessageOwner(void)
{
	return(&GetMessageOwner());
}

/******************************************************************************/
/******************************************************************************/
void DOHost::AcceptImmigration(bool i_accept)
{
	s_acceptImmigration = i_accept;
	if ((s_localHost != NULL) &&
			(s_localHost->d_state.d_acceptImmigration != s_acceptImmigration))
	{
		g_debugOutput.TSPrint(g_debugOutput.Category("MIG"),
				"Local host changing state, accept = %s",
				s_acceptImmigration ? "true" : "false");
		s_localHost->d_state.d_acceptImmigration = s_acceptImmigration;
		s_localHost->d_state.RequestUpdate();
	}
}

/******************************************************************************/
/******************************************************************************/
void DOHost::ComputeAffinity(DOHandle i_actor, uint8* o_affinity,
		TimeInterval* o_waitTime)
{
	if (i_actor.IsAKindOf(DOCLASSID(DOActor)))
	{
		*o_affinity = FindLoadRank(s_localHost);
		*o_waitTime = (*o_affinity != 0) ? 500 : 1000;
#ifndef CONSUMER_BUILD
//		DOActor::Ref refActor(i_actor);
//		if (refActor.IsValid() && (refActor->GetActor() != NULL))
//		{
//			g_debugOutput.TSPrint(g_debugOutput.Category("MIG"),
//					"Affinity : %d/%d : %s : 0x%x : %s", *o_affinity, *o_waitTime,
//					GetActorName(refActor->GetActor()),
//					refActor->GetHandle().GetValue(),
//					refActor->IsPersistent() ? "Persistent" : "Terminal");
//		}
#endif
	}
	else
	{
		PromotionReferee::Ref refReferee(g_hdoPromotionReferee);
		if (refReferee->IsADuplicationMaster())
		{
			*o_affinity = 100;
			*o_waitTime = 0;
		}
		else
		{
			*o_affinity = 0;
			*o_waitTime = 15000;
		}
	}
}

/******************************************************************************/
/******************************************************************************/
uint8 DOHost::ValidateAffinity(DOHandle i_actor, DOHandle i_station,
		uint8 i_affinity)
{
	if (i_actor.IsAKindOf(DOCLASSID(DOActor)))
	{
		DOHost::SelectionIterator host;
		host.GotoStart();
		while(!host.EndReached())
		{
			if (host->GetMasterStation() == i_station)
				return(host->d_state.d_acceptImmigration ? i_affinity : 0);
			host++;
		}
		return(0);
	}
	return(i_affinity);
}

/******************************************************************************/
/******************************************************************************/
void DOHost::CountPersistent(void)
{
	// Clear counts.
	DOHost::SelectionIterator host;
	host.GotoStart();
	while(!host.EndReached())
	{
		host->d_persistentCount = 0;
		host++;
	}

	// Count
	DOActor::SelectionIterator actor;
	actor.GotoStart();
	while(!actor.EndReached())
	{
		DOHost* actorHost = actor->GetHost();
		if ((actorHost != NULL) && actor->IsPersistent())
			actorHost->d_persistentCount++;
		actor++;
	}
}

/******************************************************************************/
/******************************************************************************/
bool DOHost::NeedToManageActors(void)
{
	// See if there are any defunct actors that need to be managed.
	DOActor::SelectionIterator actor(DUPLICATION_MASTER);
	actor.GotoStart();
	while(!actor.EndReached())
	{
		if (actor->GetActor() == NULL)
			return(true);
		actor++;
	}

	// Load leveling is not performed if the local host does not accept
	// immigration.
	if (!d_state.d_acceptImmigration)
		return(false);

	// Add up the persistent actors and the number of hosts that will accept
	// immigration.
	CountPersistent();
	int totalPersistent = 0;
	int numHosts = 0;
	DOHost::SelectionIterator host;
	host.GotoStart();
	while(!host.EndReached())
	{
		totalPersistent += host->d_persistentCount;
		if (host->d_state.d_acceptImmigration)
			numHosts++;
		host++;
	}

	// Find the max load per host (numHosts > 0 since the local host accepts
	// immigration).
	int load = ((totalPersistent - 1) / numHosts) + 1;

	// See if the local host has any excess.
	return(d_persistentCount > load);
}

/******************************************************************************/
/******************************************************************************/
bool DOHost::ManageActors(void)
{
	// Are any actors being actively managed?
	bool active = false;

	// Process actors whose master's reside on this host.
	int deleting = 0;
	int needMigration = 0;
	DOActor::SelectionIterator actor(DUPLICATION_MASTER);
	actor.GotoStart();
	while(!actor.EndReached())
	{
		// If an actor is in the process of being deleted or migrating, just let
		// things progress.
		if (actor->MainRefIsDeleted())
		{
			ASSERT((actor->GetActor() == NULL) && !actor->MigrationInProgress() &&
					(actor->GetHost() == this));
			active = true;
			deleting++;
		}
		else if (actor->MigrationInProgress())
		{
			ASSERT(actor->GetHost() != this);
			active = true;
		}

		// New action can be taken on actors that are not being deleted or are
		// migrating.
		else
		{
			// When an attempt is made to migrate an actor, it is assigned to its
			// destination host. If migration fails for any reason, it needs to
			// be reassigned back to the local host. Make sure that is true.
			actor->SetHost(this);

			// Net actors without any CActor need to be deleted or migrated.
			if (actor->GetActor() == NULL)
			{
				active = true;
				if (actor->IsPersistent())
					needMigration++;
				else
				{
					actor->DeleteMainRef();
					deleting++;
				}
			}
		}
		actor++;
	}

	// Find load distribution. Currently migrating actors will be included in
	// their destination host's counts.
	CountPersistent();

	// Remove actors that are being deleted from the local actor's count.
	d_persistentCount -= deleting;

	// Add up the persistent actors managed by remote hosts and the number
	// of remote hosts that will accept immigration.
	int totalPersistent = 0;
	int numHosts = 0;
	DOHost::SelectionIterator host(DUPLICA);
	host.GotoStart();
	while(!host.EndReached())
	{
		totalPersistent += host->d_persistentCount;
		if (host->d_state.d_acceptImmigration)
			numHosts++;
		host++;
	}

	// If no remote hosts will accept immigration, delete any defunct actors
	// that are not in the middle of something, i.e. defunct actors slated for
	// migration.
	if (numHosts <= 0)
	{
		actor.GotoStart();
		while(!actor.EndReached())
		{
			if (!actor->MainRefIsDeleted() && !actor->MigrationInProgress() &&
					(actor->GetActor() == NULL))
			{
				active = true;
				actor->DeleteMainRef();
			}
			actor++;
		}
	}

	// There are remote hosts available for migration.
	else
	{
		// Figure out how many actors need to be migrated in addition to the ones
		// migrating because they are defunct. The only way the local host does
		// not accept immigration while other hosts do is for the local host to
		// be quitting in the middle of play. In that case, assume that cleanup
		// will follow from the local CActors being deleted. If the local host
		// accepts immigration, load leveling is being performed and it makes
		// sense to find the excess.
		int load;
		int excess = 0;
		if (d_state.d_acceptImmigration)
		{
			// Find the max load per host including this host and its actors.
			load = ((totalPersistent + d_persistentCount - 1) / (numHosts + 1)) + 1;

			// The number of actors to migrate for load leveling is found by
			// taking the total number of persistent actors associated with this
			// host minus the number that have to be migrated because they are
			// defunct minus the max load. A negative result indicates that the
			// local host does not need to migrate to level the load.
			excess = d_persistentCount - needMigration - load;
			if (excess < 0)
				excess = 0;
		}

		// Find the max load per remote host including the actors to be migrated.
		load = ((totalPersistent + needMigration + excess - 1) / numHosts) + 1;

		// Go through each host and actor, migrating as needed.
		host.GotoStart();
		actor.GotoStart();
		while(!host.EndReached() && ((excess > 0) || (needMigration > 0)))
		{
			while (!actor.EndReached() && host->d_state.d_acceptImmigration &&
					(host->d_persistentCount < load) &&
					((excess > 0) || (needMigration > 0)))
			{
				if (!actor->MainRefIsDeleted() && !actor->MigrationInProgress() &&
						actor->IsPersistent())
				{
					bool migrate = false;
					if (actor->GetActor() == NULL)
					{
						migrate = true;
						needMigration--;
					}
					else if (excess > 0)
					{
						migrate = true;
						excess--;
					}
					if (migrate)
					{
//						g_debugOutput.TSPrint(g_debugOutput.Category("MIG"),
//								"MigrateTo : %s : %s : 0x%x : %s", host->GetName(),
//								GetActorName(actor->GetActor()),
//								actor->GetHandle().GetValue(),
//								actor->IsPersistent() ? "Persistent" : "Terminal");
						active = true;
						bool result = actor->AttemptEmigration(host->GetMasterID());
						ASSERT(result);
						actor->SetHost(*host);
						host->d_persistentCount++;
					}
				}
				actor++;
			}
			host++;
		}
#if _DEBUG
		ASSERT((needMigration == 0) && (excess == 0));
		while (!actor.EndReached())
		{
			ASSERT(actor->MainRefIsDeleted()  || actor->MigrationInProgress() ||
					(actor->GetActor() != NULL));
			actor++;
		}
#endif
	}

	return(active);
}

/******************************************************************************/
/******************************************************************************/
uint8 DOHost::FindLoadRank(DOHost* i_host)
{
	// If no host is specified or the specified host does not accept migrants,
	// rank at the bottom.
	if ((i_host == NULL) || !i_host->d_state.d_acceptImmigration)
		return(0);

	// Count the number of persistent actors on each host.
	CountPersistent();

	// Count the number of hosts with more and the same persistent actors.
	int queryPersistentCount = i_host->d_persistentCount;
	uint8 numGreaterLoad = 0;
	int numSameLoad = 0;
	DOHost::SelectionIterator host;
	host.GotoStart();
	while(!host.EndReached())
	{
		if (host->d_persistentCount > queryPersistentCount)
			numGreaterLoad++;
		else if (host->d_persistentCount == queryPersistentCount)
			numSameLoad++;
		host++;
	}

	// If other hosts have the same count, randomly pick a slot.
	if (numSameLoad > 1)
		numGreaterLoad += (uint8)g_random.UpToExcludeHigh(numSameLoad);

	// Return the rank. Note that the lowest possible rank is 1.
	return(numGreaterLoad + 1);
}

/******************************************************************************/
/******************************************************************************/
void DOHost::CMigrationLockRequest::Desire(bool i_desire)
{
	if (i_desire != (d_lockState != LOCK_NONE))
	{
		SLogMessage logMessage;
		logMessage.d_type = SLogMessage::LOG_ACTION;
		logMessage.d_action = "Migrate";

		if (i_desire)
		{
			logMessage.d_label = "Lock";
			d_lockState = LOCK_REQUESTED;
			g_debugOutput.TSPrint(g_debugOutput.Category("LOCK"),
					"Request Migration Lock");
			if (!g_session.RequestLock(this))
			{
				ASSERT(false);
				d_lockState = LOCK_NONE;
			}
		}
		else
		{
			logMessage.d_label = "Unlock";
			g_debugOutput.TSPrint(g_debugOutput.Category("LOCK"),
					"Cancel Migration Lock");
			g_session.RequestUnlock(this);
			d_lockState = LOCK_NONE;
		}

		g_messageDispatcher.SendMessageToAll("LogLock", &logMessage,
				INVALID_OBJECT_HANDLE);
	}
}

/******************************************************************************/
/******************************************************************************/
void DOHost::CMigrationLockRequest::LockEstablished(void)
{
	g_debugOutput.TSPrint(g_debugOutput.Category("LOCK"),
			"Establish Migration Lock");
	d_lockState = LOCK_ESTABLISHED;
}

#ifdef LATENCY_COMPENSATION
/******************************************************************************/
/******************************************************************************/
void DOHost::SetTimeProperties(float i_hostTimePerSessionTime,
		float i_hostVsSessionIntercept)
{
	if (IsADuplicationMaster())
	{
		d_state.d_hostTimePerSessionTime = i_hostTimePerSessionTime;
		d_state.d_hostVsSessionIntercept = i_hostVsSessionIntercept;
		d_state.RequestUpdate();
	}
}
#endif
