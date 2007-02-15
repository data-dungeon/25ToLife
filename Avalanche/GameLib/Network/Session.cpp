/*
**
**  File:   Session.h
**  Date:   October 9, 2003
**  By:     Bryant Collard
**  Desc:   The network session.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/Session.cpp $
**  $Revision: #29 $
**  $DateTime: 2005/07/26 17:14:21 $
**    $Author: Dwain_Skinner $
**
*/

// Precompiled header
#include "Network/NetworkPCH.h"

// My header
#include "Network/Session.h"

// Network headers
#include "Network/Host.h"

// Gamelib includes
#include "GameHelper/Message.h"
#include "EngineHelper/timer.h"
#include "GameHelper/DebugOutput.h"

DeclareSingleton(CSession);

bool CSession::s_logLocking = false;

/**********************************************************************/
/**********************************************************************/
CSession::CSession(void)
{
	// Session state
	d_lanSession = true;
	d_sessionOpen = false;
	d_localHost = NULL;
	d_remoteHostHead = NULL;
	d_populateEnabled = true;

	// Session locking
	d_lockRequestHead = NULL;
	d_lockPermission = ALLOW_LOCK;
}

/**********************************************************************/
/**********************************************************************/
CSession::~CSession(void)
{
	ClearLockRequests();
}

/**********************************************************************/
/**********************************************************************/
bool CSession::Open(void)
{
	d_sessionOpen = true;
	return(true);
}

/**********************************************************************/
/**********************************************************************/
bool CSession::Reopen(void)
{
	return(d_sessionOpen);
}

/**********************************************************************/
/**********************************************************************/
void CSession::Close(void)
{
	d_sessionOpen = false;
}

/**********************************************************************/
/**********************************************************************/
void CSession::Update(void)
{
}

/**********************************************************************/
/**********************************************************************/
const CHost* CSession::GetNextRemoteHost(const CHost* i_lastHost) const
{
	if (i_lastHost == NULL)
		return(d_remoteHostHead);
	return(i_lastHost->d_next);
}

/**********************************************************************/
/**********************************************************************/
int32 CSession::GetNumRemoteHosts()
{
	int32 count = 0;
	const CHost *next = NULL;
	while((next = GetNextRemoteHost(next)))
	{
		count++;
	}
	return(count);
}

/**********************************************************************/
/**********************************************************************/
void CSession::AddRemoteHost(CHost* i_host)
{
	if (i_host != NULL)
	{
		ASSERT((i_host->d_prev == NULL) && (i_host->d_next == NULL));
		i_host->d_next = d_remoteHostHead;
		i_host->d_prev = NULL;
		if (d_remoteHostHead != NULL)
			d_remoteHostHead->d_prev = i_host;
		d_remoteHostHead = i_host;

		g_messageDispatcher.SendMessageToAll("RemoteHostAdded", i_host,
				INVALID_OBJECT_HANDLE);
	}
}

/**********************************************************************/
/**********************************************************************/
void CSession::RemoveRemoteHost(CHost* i_host)
{
#ifdef _DEBUG
	CHost* curr = d_remoteHostHead;
	while(curr != NULL)
	{
		if (i_host == curr)
			break;
		curr = curr->d_next;
	}
	ASSERT(curr == i_host);
#endif

	if (i_host != NULL)
	{
		if (i_host->d_prev != NULL)
			i_host->d_prev->d_next = i_host->d_next;
		else
			d_remoteHostHead = i_host->d_next;

		if (i_host->d_next != NULL)
			i_host->d_next->d_prev = i_host->d_prev;

		i_host->d_prev = NULL;
		i_host->d_next = NULL;

		g_messageDispatcher.SendMessageToAll("RemoteHostRemoved", i_host,
				INVALID_OBJECT_HANDLE);
	}
}

/**********************************************************************/
/**********************************************************************/
int32 CSession::GetSessionTimeMS(void)
{
	return((int32)g_timer.GetStartMS());
}

/**********************************************************************/
/**********************************************************************/
float CSession::GetSessionTimeSec(void)
{
	return(g_timer.GetStartSec());
}

/**********************************************************************/
/**********************************************************************/
bool CSession::RequestLock(CSessionLockRequest* i_lockRequest)
{
	if ((i_lockRequest != NULL) && i_lockRequest->Add(&d_lockRequestHead))
	{
		if ((d_lockPermission == ALLOW_LOCK) &&
				(d_lockRequestHead == i_lockRequest))
			i_lockRequest->LockEstablished();
		return(true);
	}
	return(false);
}

/**********************************************************************/
/**********************************************************************/
void CSession::RequestUnlock(CSessionLockRequest* i_lockRequest)
{
	if (i_lockRequest->Remove(&d_lockRequestHead))
	{
		if (d_lockPermission == LATCH_LOCK)
			d_lockPermission = DISALLOW_LOCK;
		if ((d_lockPermission == ALLOW_LOCK) && (d_lockRequestHead != NULL))
			d_lockRequestHead->LockEstablished();
	}
}

/**********************************************************************/
/**********************************************************************/
void CSession::ClearLockRequests(void)
{
	while (d_lockRequestHead != NULL)
	{
		g_debugOutput.TSPrint(g_debugOutput.Category("LOCK"),
				"Clearing Requested Lock");
		d_lockRequestHead->Remove(&d_lockRequestHead);
	}

	if (d_lockPermission == LATCH_LOCK)
		d_lockPermission = DISALLOW_LOCK;
}

/**********************************************************************/
/**********************************************************************/
bool CSession::IsLockedBy(CSessionLockRequest* i_lockRequest)
{
	return(d_lockRequestHead == i_lockRequest);
}

/**********************************************************************/
/**********************************************************************/
bool CSession::IsUnlocked(void)
{
	return(d_lockRequestHead == NULL);
}

/**********************************************************************/
/**********************************************************************/
void CSession::DisableLock(void)
{
	d_lockPermission = (d_lockRequestHead != NULL) ? LATCH_LOCK : DISALLOW_LOCK;
}

/**********************************************************************/
/**********************************************************************/
void CSession::EnableLock(void)
{
	if ((d_lockPermission == DISALLOW_LOCK) && (d_lockRequestHead != NULL))
		d_lockRequestHead->LockEstablished();
	d_lockPermission = ALLOW_LOCK;
}

/**********************************************************************/
/**********************************************************************/
CSessionLockRequest::CSessionLockRequest()
{
	d_head = NULL;
}

/**********************************************************************/
/**********************************************************************/
CSessionLockRequest::~CSessionLockRequest()
{
	ASSERT(d_head == NULL);
}

/**********************************************************************/
// Add to the end of the request list. The argument is a pointer to the head
// of the list to which the request is added. If the list is empty, this
// request will be made the head of the list.
/**********************************************************************/
bool CSessionLockRequest::Add(CSessionLockRequest** i_head)
{
	if (i_head == NULL)
	{
		ASSERT(false);
		return(false);
	}

	if (d_head == i_head)
		return(true);

	if (d_head != NULL)
	{
		ASSERT(false);
		return(false);
	}

	d_head = i_head;
	d_next = NULL;

	if (*i_head == NULL)
	{
		d_prev = NULL;
		*i_head = this;
	}
	else
	{
		CSessionLockRequest* tail = *i_head;
		while(tail->d_next != NULL)
			tail = tail->d_next;
		tail->d_next = this;
		d_prev = tail;
	}

	return(true);
}

/**********************************************************************/
// Remove the request from the list. Return true if the removed request
// was the head request.
/**********************************************************************/
bool CSessionLockRequest::Remove(CSessionLockRequest** i_head)
{
	ASSERT(d_head == i_head);

	if ((d_head != NULL) && (d_head == i_head))
	{
		d_head = NULL;

		if (d_next != NULL)
			d_next->d_prev = d_prev;

		if (d_prev == NULL)
		{
			ASSERT(*i_head == this);
			*i_head = d_next;
			return(true);
		}

		d_prev->d_next = d_next;
	}

	return(false);
}

/**********************************************************************/
/**********************************************************************/
uint32 CSession::ToSessionID(CActor* i_actor)
{
	return((uint32)CActor::ToHandle(i_actor));
}

/**********************************************************************/
/**********************************************************************/
uint32 CSession::ToSessionID(ActorHandle i_handle)
{
	return((uint32)i_handle);
}

/**********************************************************************/
/**********************************************************************/
CActor* CSession::FromSessionIDToActor(uint32 i_id)
{
	return(CActor::FromHandle(ActorHandle(i_id)));
}

/**********************************************************************/
/**********************************************************************/
ActorHandle CSession::FromSessionIDToHandle(uint32 i_id)
{
	return(ActorHandle(i_id));
}
