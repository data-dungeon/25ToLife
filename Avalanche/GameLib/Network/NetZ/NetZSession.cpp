/*
**
**  File:   NetZSession.cpp
**  Date:   November 21, 2003
**  By:     Bryant Collard
**  Desc:   The NetZ network session.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/NetZSession.cpp $
**  $Revision: #160 $
**  $DateTime: 2005/08/24 17:01:26 $
**    $Author: Bryant_Collard $
**
*/

// Precompiled header
#include "Network/NetworkPCH.h"

// My header
#include "Network/NetZ/NetZSession.h"

// Engine includes
#include "Layers/Debug.h"

// Gamelib includes
#include "Network/SessionSpec.h"
#include "GameHelper/LogUtil.h"
#include "GameHelper/ErrorStack.h"
#include "GameHelper/DebugOutput.h"

// NetZ includes
#include <netz.h>
#include "Network/NetZ/DOHost.h"
#include "Network/NetZ/Translators.h"
#include "Network/NetZ/DOActor.h"
#include "cmdcon/cmdcon.h"

// Well-Known headers
#include "Network/NetZ/DOWorld.h"

// Profiling
#include "Layers/Timing.h"
#include "EngineHelper/timer.h"

#define KEY_SIZE              16
#define KEY_NAME              "Config"
#define MAX_NETZ_VALUE_LENGTH 127
#define MAX_CONFIG_LENGTH     1023

#if defined(PS2)
#define APPLICATION_VERSION	0x10000
#elif defined(_XBOX)
#define APPLICATION_VERSION	0x20000
#else
#define APPLICATION_VERSION	0x30000
#endif

///////////////////////////////// Construction /////////////////////////////////

/******************************************************************************/
/******************************************************************************/
CNetZSession::CNetZSession() : d_messageOwner("NetZSession")
{
	d_connectedLock = false;
}

/******************************************************************************/
/******************************************************************************/
CNetZSession::~CNetZSession()
{
	Close();
	ClearLockRequests();
}

///////////////////////////////// Installation /////////////////////////////////

/******************************************************************************/
/******************************************************************************/
static void WellKnownDOFactory(void)
{
	DOWorld* world = DOWorld::CreateWellKnown(g_doWorld);
	if (world == NULL)
	{
		char msg[256];
		SystemError::GetErrorString(SystemError::GetLast(), msg, 256);
		QTRACE(TRACE_ALWAYS, ("NetZError: %s\n", msg));
		g_console.Error("NetZError: %s\n", msg);
		ASSERT(false);
	}
	world->Publish();
}

/******************************************************************************/
/******************************************************************************/
void CNetZSession::RegisterCallbacks(void)
{
	Session::RegisterWellKnownDOsFactory(WellKnownDOFactory);
}

/******************************************************************************/
/******************************************************************************/
void CNetZSession::UnregisterCallbacks(void)
{
	Session::UnregisterWellKnownDOsFactory(WellKnownDOFactory);
}

/******************************************************************************/
/******************************************************************************/
void CNetZSession::InstallMessageTranslators(void)
{
	CTranslators::Install();
}

////////////////////////////////// Management //////////////////////////////////

/******************************************************************************/
/******************************************************************************/
bool CNetZSession::Open(void)
{
	// Don't try to establish multiple simultaneous sessions.
	if (d_sessionOpen)
		return(true);

	// Make sure there is a session spec.
	if (!ValidSingleton(CSessionSpec))
	{
		g_errorStack.SetError("Session", "No session specification",
				CErrorStack::ERROR_FATAL, 5000);
		return(false);
	}

	// Try to enable the network connection.
	if (!d_connection.Enable(d_lanSession))
	{
		g_errorStack.SetError("Session", "Failed to create a connection",
				CErrorStack::ERROR_FATAL, 5000);
		return(false);
	}

	// Register debug output categories.
	g_debugOutput.RegisterCategory("Session Locks", "LOCK");
	g_debugOutput.RegisterCategory("Dead Reckoning", "DR");
	g_debugOutput.RegisterCategory("Migration", "MIG");

	// Get the network category.
	uint8 category = g_debugOutput.Category("NET");

	// Testing and debugging
	d_connection.InitializeTestSuite();

	// Register well-known DO factories and session operations.
	RegisterCallbacks();

	// Set the session time singleton here just in case something tries to
	// access it during session creation. Note that the game timer will be used
	// until the session time is started.
	SetSingleton(CSessionTime, &d_sessionTime);

	// Set the local station's process info.
	String processName;
	const char* localName = GetLocalName();
	if ((localName != NULL) && (localName[0] != '\0'))
		processName = localName;
	else
		processName = "Default";
	Station::SetLocalProcessInfo(Station::ClientProcess, processName);

	// Attempt to establish a session.
	if (g_sessionSpec.GetName() != NULL)
	{
		if (!Session::CreateSession(g_sessionSpec.GetName()))
		{
			QTRACE(TRACE_ALWAYS,
					("Failed to create session %s\n", g_sessionSpec.GetName()));
			g_console.Notify("Failed to create session %s\n",
					g_sessionSpec.GetName());
			g_errorStack.SetError("Session", "Failed to create a session",
					CErrorStack::ERROR_FATAL, 5000);
			PrintNetZError();
			ClearSingleton(CSessionTime);
			UnregisterCallbacks();
			d_connection.TerminateTestSuite();
			d_connection.Disable();
			return(false);
		}
		SessionDescription* sessionDescription =
				Session::GetLocalSessionDescription();
		sessionDescription->SetApplicationVersionNumber(APPLICATION_VERSION);

		strcpy(d_sessionURL, sessionDescription->GetStationURL());
		g_debugOutput.TSPrint(category, "CreateSession: name = %s, url = %s",
				sessionDescription->GetSessionName(), d_sessionURL);
	}
	else if (g_sessionSpec.GetNumEntries() > 0)
	{
#if defined PS2  || defined DIRECTX_PC // ARGH!!
		g_debugOutput.TSPrint(category, "JoinSession URL list:");
		qList<StationURL> urlList;
		for (uint i = 0; i < g_sessionSpec.GetNumEntries(); i++)
		{
			g_debugOutput.TSPrint(category, "  URL %d - %s", i,
					(g_sessionSpec.GetEntry(i) != NULL) ?
					g_sessionSpec.GetEntry(i) : "non-existent entry");
			StationURL url(g_sessionSpec.GetEntry(i));
			urlList.push_back(url);
		}
		if (!Session::JoinSession(urlList))
#else
		StationURL url( g_sessionSpec.GetEntry(0) );
		g_debugOutput.TSPrint(category, "JoinSession URL list:");
		g_debugOutput.TSPrint(category, "  URL 0 - %s",
					(g_sessionSpec.GetEntry(0) != NULL) ?
					g_sessionSpec.GetEntry(0) : "non-existent entry");
		if (!Session::JoinSession( url ))
#endif
		{
			QTRACE(TRACE_ALWAYS, ("Failed to join session\n"));
			g_console.Notify("Failed to join session\n");
#ifdef _XBOX	// JM - added xbox-specific message for TCR compliance
			g_errorStack.SetError("Session",
					"This game session is no longer available",
					CErrorStack::ERROR_FATAL, 5000);
#else
			g_errorStack.SetError("Session", "Failed to join a session",
					CErrorStack::ERROR_FATAL, 5000);
#endif 

			PrintNetZError();
			ClearSingleton(CSessionTime);
			UnregisterCallbacks();
			d_connection.TerminateTestSuite();
			d_connection.Disable();
			return(false);
		}
		SessionDescription* sessionDescription =
				Session::GetLocalSessionDescription();
		strcpy(d_sessionURL, sessionDescription->GetStationURL());
		g_debugOutput.TSPrint(category, "JoinSession: name = %s, url = %s",
				sessionDescription->GetSessionName(), d_sessionURL);
	}
	else
	{
		QTRACE(TRACE_ALWAYS, ("No session specified to create or join\n"));
		g_console.Notify("No session specified to create or join\n");
		g_errorStack.SetError("Session",
				"No Session name was supplied to join or create",
				CErrorStack::ERROR_FATAL, 5000);
		ClearSingleton(CSessionTime);
		UnregisterCallbacks();
		d_connection.TerminateTestSuite();
		d_connection.Disable();
		return(false);
	}
	d_sessionOpen = true;
	d_populateEnabled = false;

	// Debugging
	Operation::SetTraceFilter(NULL);
	d_logHandler.Initialize("LogLock", MESSAGE_HANDLE_IF_MASTER,
			d_messageOwner, this);

	// Setup session
	InstallMessageTranslators();
	BeginConnectedLock();
	d_localHost = DOHost::CreateLocal();

	// Set the session start time
	d_sessionTime.StartSessionTime();

#ifndef CONSUMER_BUILD
	// Print version.
	ProductInfo *pInfo = new ProductInfo;
	char *version = pInfo->GetLongVersionString();
	QTRACE(TRACE_ALWAYS,("%s...\n", version));
	g_console.Echo("%s...\n", version);
	delete pInfo;
#endif

	return(true);
}
/******************************************************************************/
/******************************************************************************/
bool CNetZSession::Reopen(void)
{
	// Don't try to reopen a closed session.
	if (!d_sessionOpen)
		return(false);

	InstallMessageTranslators();

	return(true);
}

/******************************************************************************/
/******************************************************************************/
void CNetZSession::Close(void)
{
	if (!d_sessionOpen)
		return;

	d_sessionOpen = false;

	EndConnectedLock();
	DOHost::Terminate();
	d_localHost = NULL;
	d_sessionTime.StopSessionTime();
	ClearSingleton(CSessionTime);
	UnregisterCallbacks();

	d_connection.TerminateTestSuite();
	d_connection.Disable();

	d_messageOwner.UnregisterAll();
}

/******************************************************************************/
/******************************************************************************/
static u32 lastTime = 0;
void CNetZSession::Update(void)
{
	if (!d_sessionOpen)
		return;

	Profiler__Start(PROFILE_NETWORK_UPDATE);

#ifndef CONSUMER_BUILD
	// --- lets check to see how long since our last update
	u32 curTime = g_timer.GetOSTime( );
	if( ( curTime - lastTime ) > 1000 )
	{
		g_console.Echo( "***** It has been %f seconds since our last update\n", ( ( float )( curTime - lastTime ) ) / 1000.0f );
	}
	lastTime = curTime;
#endif

	d_sessionTime.Update();
	DOHost::PropagateUpdate();
	d_connection.Service();
	d_connection.DumpCurrentStats();

	Profiler__Stop(PROFILE_NETWORK_UPDATE);
}

/******************************************************************************/
/******************************************************************************/
bool CNetZSession::IsMaster(void)
{
	return(d_sessionOpen && Session::GetInstance()->IsADuplicationMaster());
}

/******************************************************************************/
/******************************************************************************/
void CNetZSession::EnablePopulate(bool i_enable)
{
	if (i_enable != IsPopulateEnabled())
	{
		CSession::EnablePopulate(i_enable);
		if (i_enable && d_sessionOpen)
		{
			// Make sure all DOActors have a CActor. In general, all pending
			// DOActors will be duplicas.
			DOActor::SelectionIterator actor;
			actor.GotoStart();
			while (!actor.EndReached())
			{
				actor->Establish();
				g_messageDispatcher.SendMessageToAll("AssignActor", *actor,
						INVALID_OBJECT_HANDLE);
				actor++;
			}
		}
	}
}

/******************************************************************************/
/******************************************************************************/
int32 CNetZSession::GetSessionTimeMS(void)
{
	return(d_sessionTime.GetSessionTime());
}

/******************************************************************************/
/******************************************************************************/
float CNetZSession::GetSessionTimeSec(void)
{
	return((float)d_sessionTime.GetSessionTime() / 1000.0f);
}

/******************************************************************************/
/******************************************************************************/
void CNetZSession::SetSessionTimeDebug(bool i_active)
{
	d_sessionTime.SetDebug(i_active);
}

/******************************************************************************/
/******************************************************************************/
void CNetZSession::ToggleSessionTimeDebug(void)
{
	d_sessionTime.ToggleDebug();
}

/******************************************************************************/
/******************************************************************************/
void CNetZSession::PrintNetZError(void)
{
	char msg[256];
	SystemError::GetErrorString(SystemError::GetLast(), msg, 256);
	QTRACE(TRACE_ALWAYS, ("NetZError: %s\n", msg));
	g_console.Error("NetZError: %s\n", msg);
	g_debugOutput.TSPrint(g_debugOutput.Category("NET"), "NetZ Error: %s", msg);
}

//////////////////////////////// Session Locking ///////////////////////////////

/******************************************************************************/
/******************************************************************************/
void CNetZSession::BeginConnectedLock(void)
{
	// A host cannot enter a session thinking it has a lock.
	ASSERT(d_connectedLock || (d_lockRequestHead == NULL));
	d_connectedLock = true;

	SLogMessage logMessage;
	logMessage.d_type = SLogMessage::LOG_ACTION;
	logMessage.d_label = "Manage";
	logMessage.d_action = "Begin";
	g_messageDispatcher.SendMessageToAll("LogLock", &logMessage,
			INVALID_OBJECT_HANDLE);
}

/******************************************************************************/
/******************************************************************************/
void CNetZSession::EndConnectedLock(void)
{
	SLogMessage logMessage;
	logMessage.d_type = SLogMessage::LOG_ACTION;
	logMessage.d_label = "Manage";
	logMessage.d_action = "End";
	g_messageDispatcher.SendMessageToAll("LogLock", &logMessage,
			INVALID_OBJECT_HANDLE);

	// If leaving a connected lock but locking is allowed, there are outstanding
	// requests, and the local host does not own the lock, tell the requestor the
	// lock has been established.
	if (d_connectedLock && (d_lockPermission == ALLOW_LOCK) &&
			(d_lockRequestHead != NULL) && !DOHost::LocalHasLock())
		d_lockRequestHead->LockEstablished();

	d_connectedLock = false;
}

/******************************************************************************/
/******************************************************************************/
bool CNetZSession::RequestLock(CSessionLockRequest* i_lockRequest)
{
	if ((i_lockRequest != NULL) && i_lockRequest->Add(&d_lockRequestHead))
	{
		if (d_connectedLock)
			UpdateConnectedLock();
		else if ((d_lockPermission == ALLOW_LOCK) &&
				(d_lockRequestHead == i_lockRequest))
			i_lockRequest->LockEstablished();
		return(true);
	}
	return(false);
}

/******************************************************************************/
/******************************************************************************/
void CNetZSession::RequestUnlock(CSessionLockRequest* i_lockRequest)
{
	// Remove the lock and determine if the head lock has changed.
	if (i_lockRequest->Remove(&d_lockRequestHead))
	{
		// Update the lock permission if locking had been disallowed while the
		// local host had the lock.
		if (d_lockPermission == LATCH_LOCK)
			d_lockPermission = DISALLOW_LOCK;

		// Manage locks if connected.
		if (d_connectedLock)
		{
			// If the local host has the lock, it still has a lock request, and
			// locks are enabled, tell the new head request that its lock has been
			// established; otherwise, update.
			if (DOHost::LocalHasLock() && (d_lockRequestHead != NULL) &&
					(d_lockPermission == ALLOW_LOCK))
				d_lockRequestHead->LockEstablished();
			else
				UpdateConnectedLock();
		}

		// If unconnected, locking is allowed and a local request is pending,
		// grant the pending request.
		else if ((d_lockPermission == ALLOW_LOCK) && (d_lockRequestHead != NULL))
			d_lockRequestHead->LockEstablished();
	}
}

/******************************************************************************/
/******************************************************************************/
void CNetZSession::ClearLockRequests(void)
{
	SLogMessage logMessage;
	logMessage.d_type = SLogMessage::LOG_ACTION;
	logMessage.d_label = "Manage";
	logMessage.d_action = "Clear";
	g_messageDispatcher.SendMessageToAll("LogLock", &logMessage,
			INVALID_OBJECT_HANDLE);

	CSession::ClearLockRequests();
	if (d_connectedLock)
		UpdateConnectedLock();
}

/******************************************************************************/
/******************************************************************************/
bool CNetZSession::IsLockedBy(CSessionLockRequest* i_lockRequest)
{
	return((d_lockRequestHead == i_lockRequest) && (!d_connectedLock ||
			DOHost::LocalHasLock()));
}

/******************************************************************************/
/******************************************************************************/
void CNetZSession::UpdateLock(void)
{
	if (d_connectedLock)
		UpdateConnectedLock();
}

/******************************************************************************/
/******************************************************************************/
void CNetZSession::UpdateConnectedLock(void)
{
	// Log state changes.
	SLogMessage logMessage;
	logMessage.d_type = SLogMessage::LOG_ACTION;
	logMessage.d_label = "Set";

	// Declare an iterator for remote hosts.
	DOHost::SelectionIterator host(DUPLICA);

	// Try to find a remote host that claims the lock.
	host.GotoStart();
	while(!host.EndReached())
	{
		if (host->HasLock())
		{
			if (DOHost::SetLockState(((d_lockPermission == ALLOW_LOCK) &&
					(d_lockRequestHead != NULL)), *host))
			{
				logMessage.d_action = "RemoteLock";
				g_messageDispatcher.SendMessageToAll("LogLock", &logMessage,
						INVALID_OBJECT_HANDLE);
			}
#ifdef _DEBUG
			// Make sure only one host has claimed the lock.
			host++;
			while(!host.EndReached())
			{
				ASSERT(!host->HasLock());
				host++;
			}
			ASSERT(!DOHost::LocalHasLock());
#endif
			return;
		}
		host++;
	}

	// Don't do anything if the lock host wants and claims the lock.
	if ((d_lockRequestHead != NULL) && DOHost::LocalHasLock())
		return;

	// At this point, no host has claimed the lock.
	//
	// Make sure the local host won't grant or claim a lock if it has disallowed
	// locks.
	if (d_lockPermission != ALLOW_LOCK)
	{
		if (DOHost::SetLockState(false, NULL))
		{
			logMessage.d_action = "Disabled";
			g_messageDispatcher.SendMessageToAll("LogLock", &logMessage,
					INVALID_OBJECT_HANDLE);
		}
		return;
	}

	// Find the best remote host that wants the lock and determine if all
	// remote hosts have granted the lock to the local host.
	DOHost* bestRemoteApplicant = NULL;
	bool allRemoteGrantedToLocal = true;
	host.GotoStart();
	while(!host.EndReached())
	{
		if (host->WantsLock() && ((bestRemoteApplicant == NULL) ||
				(host->GetHandle() > bestRemoteApplicant->GetHandle())))
			bestRemoteApplicant = *host;
		allRemoteGrantedToLocal = allRemoteGrantedToLocal &&
				DOHost::LockGrantedToLocal(*host);
		host++;
	}

	// See if the local host wants the lock and is a better choice than any of
	// the remote hosts.
	if ((d_lockRequestHead != NULL) && ((bestRemoteApplicant == NULL) ||
			(DOHost::GetLocalHandle() > bestRemoteApplicant->GetHandle())))
	{
		// Set the local lock state indicating that the local host wants the
		// lock. If all the remote hosts have granted the lock to the local host,
		// establish the lock; otherwise, grant it to no one.
		if (allRemoteGrantedToLocal)
		{
			d_lockRequestHead->LockEstablished();
			if (DOHost::SetLockState(true, DOHost::GetLocal()))
			{
				logMessage.d_action = "Claimed";
				g_messageDispatcher.SendMessageToAll("LogLock", &logMessage,
						INVALID_OBJECT_HANDLE);
			}
		}
		else
		{
			if (DOHost::SetLockState(true, NULL))
			{
				logMessage.d_action = "Pending";
				g_messageDispatcher.SendMessageToAll("LogLock", &logMessage,
						INVALID_OBJECT_HANDLE);
			}
		}
	}

	// Set the lock state if the local host does not want the lock or if there
	// is a better choice.
	else
	{
		if (DOHost::SetLockState((d_lockRequestHead != NULL), bestRemoteApplicant))
		{
			logMessage.d_action = "RemoteGrant";
			g_messageDispatcher.SendMessageToAll("LogLock", &logMessage,
					INVALID_OBJECT_HANDLE);
		}
	}
}

/******************************************************************************/
/******************************************************************************/
bool CNetZSession::IsUnlocked(void)
{
	if (d_connectedLock)
		return(!DOHost::LocalGrantedLock());
	return(d_lockRequestHead == NULL);
}

/******************************************************************************/
/******************************************************************************/
void CNetZSession::DisableLock(void)
{
	if (d_connectedLock)
	{
		d_lockPermission = DOHost::LocalHasLock() ? LATCH_LOCK : DISALLOW_LOCK;
	}
	else
	{
		d_lockPermission = (d_lockRequestHead != NULL) ?
				LATCH_LOCK : DISALLOW_LOCK;
	}
}

/******************************************************************************/
/******************************************************************************/
void CNetZSession::EnableLock(void)
{
	bool disallowed = (d_lockPermission == DISALLOW_LOCK);
	d_lockPermission = ALLOW_LOCK;
	if (disallowed)
	{
		if (d_connectedLock)
			UpdateConnectedLock();
		else if (d_lockRequestHead != NULL)
			d_lockRequestHead->LockEstablished();
	}
}

/******************************************************************************/
/******************************************************************************/
bool CNetZSession::IsLockResolved(void)
{
	bool lockIsWanted = false;
	DOHost::SelectionIterator host;
	host.GotoStart();
	while(!host.EndReached())
	{
		if (host->HasLock())
			return(true);
		lockIsWanted = lockIsWanted || host->WantsLock();
		host++;
	}
	return(!lockIsWanted);
}

//////////////////////////////// Configuration /////////////////////////////////

/******************************************************************************/
/******************************************************************************/
void CNetZSession::SetConfig(const char* i_config)
{
	if (IsMaster())
		SetConfig(Session::GetLocalSessionDescription(), i_config);
}

/******************************************************************************/
/******************************************************************************/
const char* CNetZSession::GetConfig(void)
{
	if (d_sessionOpen)
		return(GetConfig(Session::GetLocalSessionDescription()));
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
void CNetZSession::SetConfig(SessionDescription* i_sessionDescription,
		const char* i_config)
{
	if (i_sessionDescription != NULL)
	{
		char key[KEY_SIZE];
		int keyIndex = 0;

		// Break up the config string into a series of attributes.
		if ((i_config != NULL) && (i_config[0] != '\0'))
		{
			int len = strlen(i_config);
			ASSERT(len <= MAX_CONFIG_LENGTH);
			char value[MAX_NETZ_VALUE_LENGTH + 1];
			value[MAX_NETZ_VALUE_LENGTH] = '\0';
			const char* start = i_config;
			while (len > 0)
			{
				sprintf(key, "%s%d", KEY_NAME, keyIndex++);
				strncpy(value, start, MAX_NETZ_VALUE_LENGTH);
				i_sessionDescription->SetAttribute(key, value);
				start += MAX_NETZ_VALUE_LENGTH;
				len -= MAX_NETZ_VALUE_LENGTH;
			}
		}

		// Remove any remaining attributes from previous configurations.
		do
		{
			sprintf(key, "%s%d", KEY_NAME, keyIndex++);
		} while(i_sessionDescription->RemoveAttribute(key));
	}
}

/******************************************************************************/
/******************************************************************************/
const char* CNetZSession::GetConfig(SessionDescription* i_sessionDescription)
{
	if (i_sessionDescription != NULL)
	{
		static char config[MAX_CONFIG_LENGTH + 1];
		char value[MAX_NETZ_VALUE_LENGTH + 1];
		char key[KEY_SIZE];
		int len = 0;
		int keyIndex = 0;
		config[0] = '\0';

		sprintf(key, "%s%d", KEY_NAME, keyIndex++);
		while(i_sessionDescription->GetAttribute(key, value,
				MAX_NETZ_VALUE_LENGTH + 1))
		{
			len += strlen(value);
			if (len > MAX_CONFIG_LENGTH)
			{
				ASSERT(false);
				return(NULL);
			}
			strcat(config, value);
			sprintf(key, "%s%d", KEY_NAME, keyIndex++);
		}
		if (len > 0)
			return(config);
	}
	return(NULL);
}

////////////////////////////////// Migration ///////////////////////////////////

/******************************************************************************/
/******************************************************************************/
bool CNetZSession::EmigrationPending(void)
{
	return(DOHost::EmigrationPending());
}

/******************************************************************************/
/******************************************************************************/
void CNetZSession::AcceptImmigration(bool i_accept)
{
	DOHost::AcceptImmigration(i_accept);
}

////////////////////////////////// Debugging ///////////////////////////////////

/******************************************************************************/
/******************************************************************************/
#define CONSTANT_MASK  0xffff00ff
#define CONSTANT_VALUE 0x0e800001
#define CONSTANT_SHIFT 8
static int ParseHandle(uint32 i_handle)
{
	if (i_handle == INVALID_DOHANDLE_VALUE)
		return(-1);
	else if ((i_handle & CONSTANT_MASK) == CONSTANT_VALUE)
		return((i_handle & ~CONSTANT_MASK) >> CONSTANT_SHIFT);
	return(-2);
}

/******************************************************************************/
/******************************************************************************/
void CNetZSession::CLog::HandleMessage(void* i_data, ActorHandle i_sender,
		bool i_posted)
{
	if (CSession::s_logLocking && !d_ref->d_logFile.IsInvalid())
	{
		int index;
		int i;
		if (!d_ref->d_logFile.IsOpen())
		{
			char rootName[] = "Locks.txt";
			const char* name = g_logUtil.Enabled() ?
					g_logUtil.MakeFullName(rootName) : rootName;
			if (!d_ref->d_logFile.Open(name, 101))
				return;

			char header[10];
			index = 0;
			d_ref->d_logFile.SetField(index++, "Time");
			d_ref->d_logFile.SetField(index++, "Handle");
			d_ref->d_logFile.SetField(index++, "Local");
			d_ref->d_logFile.SetField(index++, "Label");
			d_ref->d_logFile.SetField(index++, "Other");
			for (i = 0; i < 16; i++)
			{
				int j = index + (i * 3);
				sprintf(header, "ReqID%d", i);
				d_ref->d_logFile.SetField(j, header);
				d_ref->d_logFile.SetField(j + 48, header);

				j++;
				sprintf(header, "GrantID%d", i);
				d_ref->d_logFile.SetField(j, header);
				d_ref->d_logFile.SetField(j + 48, header);

				j++;
				sprintf(header, "Grantee%d", i);
				d_ref->d_logFile.SetField(j, header);
				d_ref->d_logFile.SetField(j + 48, header);
			}
			d_ref->d_logFile.Print();
		}

		// Get local id.
		int local = (DOHost::GetLocal() == NULL) ? -1 :
				ParseHandle(DOHost::GetLocal()->GetHandle().GetValue());

		// Set leading records.
		SLogMessage* logMessage = (SLogMessage*)i_data;
		index = 0;
		d_ref->d_logFile.SetField(index++, d_ref->d_sessionTime.GetSessionTime());
		d_ref->d_logFile.SetField(index++, (DOHost::GetLocal() == NULL) ? (uint)~0
				: (uint)DOHost::GetLocal()->GetHandle().GetValue());
		d_ref->d_logFile.SetField(index++, local);
		d_ref->d_logFile.SetField(index++, logMessage->d_label);
		switch(logMessage->d_type)
		{
		 case SLogMessage::LOG_SUBJECT:
			d_ref->d_logFile.SetField(index++, ParseHandle(logMessage->d_subject));
			break;
		 case SLogMessage::LOG_ACTION:
			d_ref->d_logFile.SetField(index++, logMessage->d_action);
			break;
		}

		// Set this host's state.
		if (local >= 0)
		{
			local = index + (local * 3);
			d_ref->d_logFile.SetField(local++,
					(int)(DOHost::GetLocal()->d_lock.d_ids & 0x0F));
			d_ref->d_logFile.SetField(local++,
					(int)((DOHost::GetLocal()->d_lock.d_ids & 0xF0) >> 4));
			d_ref->d_logFile.SetField(local,
					ParseHandle(DOHost::GetLocal()->d_lock.d_grantee.GetValue()));
		}

		// Set each host's record.
		DOHost::SelectionIterator host;
		host.GotoStart();
		while(!host.EndReached())
		{
			i = index + 48 + (ParseHandle(host->GetHandle().GetValue()) * 3);
			d_ref->d_logFile.SetField(i++, (int)(host->d_lock.d_ids & 0x0F));
			d_ref->d_logFile.SetField(i++, (int)((host->d_lock.d_ids & 0xF0) >> 4));
			d_ref->d_logFile.SetField(i,
					ParseHandle(host->d_lock.d_grantee.GetValue()));
			host++;
		}
		d_ref->d_logFile.Print();
	}
}

/******************************************************************************/
/******************************************************************************/
int CNetZSession::GetHostIdFromObject(DOHandle i_handle)
{
	DuplicatedObject::Ref ref(i_handle);
	if (ref.IsValid())
	{
		DOHandle masterStation = ref->GetMasterStation();
		DOHost::SelectionIterator host;
		host.GotoStart();
		while(!host.EndReached())
		{
			if (host->GetMasterStation() == masterStation)
				return(ParseHandle(host->GetHandle().GetValue()));
			host++;
		}
	}
	return(-2);
}

/**********************************************************************/
/**********************************************************************/
uint32 CNetZSession::ToSessionID(CActor* i_actor)
{
	if (i_actor != NULL)
	{
		DOActor* doActor = (DOActor*)i_actor->GetNetActor();
		if (doActor != NULL)
			return(doActor->GetHandleValue());
	}
	return(INVALID_DOHANDLE_VALUE);
}

/**********************************************************************/
/**********************************************************************/
uint32 CNetZSession::ToSessionID(ActorHandle i_handle)
{
	CActor* actor = CActor::FromHandle(i_handle);
	if (actor != NULL)
	{
		DOActor* doActor = (DOActor*)actor->GetNetActor();
		if (doActor != NULL)
			return(doActor->GetHandleValue());
	}
	return(INVALID_DOHANDLE_VALUE);
}

/**********************************************************************/
/**********************************************************************/
CActor* CNetZSession::FromSessionIDToActor(uint32 i_id)
{
	DOHandle doHandle = (qUnsignedInt32)i_id;
	if (doHandle.IsAKindOf(DOCLASSID(DOActor)))
	{
		DOActor::Ref ref(doHandle);
		if (ref.IsValid())
			return(ref->GetActor());
	}
	return(NULL);
}

/**********************************************************************/
/**********************************************************************/
ActorHandle CNetZSession::FromSessionIDToHandle(uint32 i_id)
{
	DOHandle doHandle = (qUnsignedInt32)i_id;
	if (doHandle.IsAKindOf(DOCLASSID(DOActor)))
	{
		DOActor::Ref ref(doHandle);
		if (ref.IsValid())
			return(CActor::ToHandle(ref->GetActor()));
	}
	return(ActorHandle(INVALID_OBJECT_HANDLE));
}

/******************************************************************************/
/******************************************************************************/
bool CNetZSession::GetFaultStatus( void )
{
	if( IsNetworked() )
	{
		DOHost *pDOHost = ( ( DOHost * )GetLocalHost() );
		return( pDOHost->GetFaultStatus() != 0 );
	}
	return( false );
}
