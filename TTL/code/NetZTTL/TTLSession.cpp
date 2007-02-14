/*
**
**  File:   TTLSession.cpp
**  Date:   February 13, 2004
**  By:     Bryant Collard
**  Desc:   A TTL specific session.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/NetZTTL/TTLSession.cpp $
**  $Revision: #25 $
**  $DateTime: 2005/06/04 13:15:18 $
**    $Author: Bryant_Collard $
**
*/

// Precompiled header
#include "TTLPCH.h"

// My header
#include "NetZTTL/TTLSession.h"

// NetZ includes
#include "NetZTTL/TTLTranslators.h"

// Well-Known headers
#include "NetZTTL/DOReferee.h"

// TTL headers
#include "environ/ConfigMgr.h"

#ifdef DUMP_REFERENCES

#include <Extensions/VoiceChat/VoiceChannel.h>

/////////////////////////// CDumpReferencesCallback ////////////////////////////

#define DUMP_FREQUENCY  5000

/******************************************************************************/
/******************************************************************************/
CDumpReferencesCallback::CDumpReferencesCallback() :
	OperationCallback(1022),
	d_messageOwner("DumpReferencesCallback")
{
	d_numHandles = 0;
}

/******************************************************************************/
/******************************************************************************/
void CDumpReferencesCallback::CallMethod(Operation* i_operation)
{
	Operation::Type type = i_operation->GetType();
	if ((type == Operation::AddToStore) ||
			(type == Operation::RemoveFromStore))
	{
		DOHandle handle = ((DOOperation*)i_operation)->GetAssociatedDOHandle();
		if (handle.IsAKindOf(DOCLASSID(Station)) ||
				handle.IsAKindOf(DOCLASSID(VoiceChannel)))
		{
			for (int i = 0; i < d_numHandles; i++)
			{
				if (d_handle[i] == handle)
				{
					if (type == Operation::RemoveFromStore)
					{
						char msg[30];
						sprintf(msg, "Removing %s (0x%x)",
								handle.GetClassNameString(), handle.GetValue());
						Dump(msg);
					}
					else
					{
						QTRACE(TRACE_ALWAYS,
								("Trying to add %s (0x%x) already in store",
								handle.GetClassNameString(), handle.GetValue()));
					}
					return;
				}
			}
			if (type == Operation::RemoveFromStore)
			{
				QTRACE(TRACE_ALWAYS, ("Trying to remove %s (0x%x) not in slot",
						handle.GetClassNameString(), handle.GetValue()));
			}
			else if (d_numHandles >= MAX_REFERENCE_HANDLES)
			{
				QTRACE(TRACE_ALWAYS,
						("No slot available to add %s (0x%x)",
						handle.GetClassNameString(), handle.GetValue()));
			}
			else
			{
				char msg[30];
				sprintf(msg, "Adding %s (0x%x)", handle.GetClassNameString(),
						handle.GetValue());
				d_handle[d_numHandles++] = handle;
				Dump(msg);
			}
		}
	}
}

/******************************************************************************/
/******************************************************************************/
void CDumpReferencesCallback::Initialize(void)
{
	d_nextDumpTime = 0;
	d_numHandles = 0;
	d_dumpReferencesHandler.Initialize("DumpReferences",
			MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this);
}

/******************************************************************************/
/******************************************************************************/
void CDumpReferencesCallback::Update(void)
{
	if ((g_timer.GetOSTime() > d_nextDumpTime) && (d_numHandles > 0))
		Dump("Update");
}

/******************************************************************************/
/******************************************************************************/
void CDumpReferencesCallback::Dump(const char* i_label)
{
	QTRACE(TRACE_ALWAYS,
			(">>>****** Begin References Dump: %s ******<<<", i_label));
	DuplicatedObjectStore::GetInstance()->Trace(TRACE_ALWAYS);
	for (int i = 0; i < d_numHandles; i++)
	{
		QTRACE(TRACE_ALWAYS, ("***** Begin References Dump of %s (0x%x) *****",
				d_handle[i].GetClassNameString(), d_handle[i].GetValue()));
		DORef::TraceReferences(TRACE_ALWAYS, d_handle[i]);
		QTRACE(TRACE_ALWAYS, ("***** End References Dump of %s (0x%x) *****",
				d_handle[i].GetClassNameString(), d_handle[i].GetValue()));
	}
	d_nextDumpTime = g_timer.GetOSTime() + DUMP_FREQUENCY;
	QTRACE(TRACE_ALWAYS, (">>>****** End References Dump: %s ******<<<",
			i_label));
}

/******************************************************************************/
/******************************************************************************/
void CDumpReferencesCallback::CDumpReferences::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	d_ref->Dump("Trigger");
}

#endif

///////////////////////// CChangeMasterStationCallback /////////////////////////

/******************************************************************************/
/******************************************************************************/
CChangeMasterStationCallback::CChangeMasterStationCallback() :
	OperationCallback(1023)
{
}

/******************************************************************************/
/******************************************************************************/
void CChangeMasterStationCallback::CallMethod(Operation* i_operation)
{
	if ((i_operation->GetType() == Operation::ChangeMasterStation) &&
			ValidSingleton(CSession))
	{
		ChangeMasterStationOperation* cmsOperation =
				ChangeMasterStationOperation::DynamicCast(i_operation);
		if (cmsOperation->GetAssociatedDO()->IsAKindOf(DOCLASSID(Session)))
		{
			g_session.SetConfig(g_configmgr.GetOptions());

			uint8 category = g_debugOutput.Category("NET");
			Station::Ref station(
					((Session*)cmsOperation->GetAssociatedDO())->GetMasterStation());
			if (station.IsValid())
			{
				g_debugOutput.TSPrint(category, "Session migrated to %s",
						station->GetProcessName().CStr());
			}
			else
			{
				g_debugOutput.TSPrint(category,
						"Session migrated to invalid station");
			}
			SessionDescription* desc = Session::GetLocalSessionDescription();
			g_debugOutput.TSPrint(category, "  Session name = %s, url = %s",
					desc->GetSessionName(), desc->GetStationURL());
		}
	}
}

///////////////////////////////// CTTLSession //////////////////////////////////

/******************************************************************************/
/******************************************************************************/
static void WellKnownDOFactory(void)
{
	DOReferee* referee = DOReferee::CreateWellKnown(g_doReferee);
	referee->Publish();
}

/******************************************************************************/
/******************************************************************************/
void CTTLSession::RegisterCallbacks(void)
{
	RegisterDDLSpecificCallbacks();
	CNetZSession::RegisterCallbacks();
	Session::RegisterWellKnownDOsFactory(WellKnownDOFactory);
	Session::GetOperationManager()->RegisterCallback(
			&d_changeMasterStationCallback);
#ifdef DUMP_REFERENCES
	d_dumpReferencesCallback.Initialize();
	Session::GetOperationManager()->RegisterCallback(&d_dumpReferencesCallback);
#endif
}

/******************************************************************************/
/******************************************************************************/
void CTTLSession::UnregisterCallbacks(void)
{
#ifdef DUMP_REFERENCES
	Session::GetOperationManager()->UnregisterCallback(&d_dumpReferencesCallback);
#endif
	Session::GetOperationManager()->UnregisterCallback(
			&d_changeMasterStationCallback);
	Session::UnregisterWellKnownDOsFactory(WellKnownDOFactory);
	CNetZSession::UnregisterCallbacks();
}

/******************************************************************************/
/******************************************************************************/
void CTTLSession::InstallMessageTranslators(void)
{
	CTTLTranslators::Install();
	CNetZSession::InstallMessageTranslators();
}

/******************************************************************************/
/******************************************************************************/
const char* CTTLSession::GetLocalName(void)
{
	return(g_playerProfile.GetAccountName());
}

#ifdef DUMP_REFERENCES
/******************************************************************************/
/******************************************************************************/
void CTTLSession::Update(void)
{
	CNetZSession::Update();
	d_dumpReferencesCallback.Update();
}
#endif
