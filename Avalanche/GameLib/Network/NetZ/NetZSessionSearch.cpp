/*
**
**  File:   NetZSessionSearch.cpp
**  Date:   October 22, 2004
**  By:     Bryant Collard
**  Desc:   Search for NetZ sessions.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/NetZSessionSearch.cpp $
**  $Revision: #10 $
**  $DateTime: 2005/08/25 17:27:19 $
**    $Author: Rob $
**
*/

// Precompiled header
#include "Network/NetworkPCH.h"

// My header
#include "Network/NetZ/NetZSessionSearch.h"

// GameLib headers
#include <netz.h>
#include "Network/NetZ/NetZSession.h"
#include "Network/SessionSpec.h"
#include "EngineHelper/timer.h"
#include "Game/Misc/ParameterSet.h"
#include "Network/NetZ/DOHost.h"
#include "math/fastrand.h"

/******************************************************************************/
/******************************************************************************/
CNetZSessionSearch::CNetZSessionSearch()
{
	d_status = INACTIVE;
	d_context = NULL;
}

/******************************************************************************/
/******************************************************************************/
CNetZSessionSearch::~CNetZSessionSearch()
{
	if ((d_status != INACTIVE) && d_connection.IsEnabled())
	{
		if (d_context)
		{
			if (d_context->GetState() == CallContext::CallInProgress)
				d_context->Cancel();
			delete d_context;
		}
		d_connection.Disable();
	}
}

/******************************************************************************/
/******************************************************************************/
void CNetZSessionSearch::Begin(void)
{
	ASSERT((d_status == INACTIVE) && !d_connection.IsEnabled());
	if ((d_status == INACTIVE) && d_connection.Enable(true))
	{
		d_status = UPDATING;
		d_nextQuery = g_timer.GetOSTime();
		d_queryList = 0;
		d_resultsList = 1;
		d_list[d_resultsList].clear();
	}
}

/******************************************************************************/
/******************************************************************************/
//bool CNetZSessionSearch::Update(void)
bool CNetZSessionSearch::Update(bool reFresh)
{
	bool result = false;
	if (d_status == UPDATING)
	{
		ASSERT(d_connection.IsEnabled());
		d_connection.Service();

		if (d_context)
		{
			if (d_context->GetState() == CallContext::CallInProgress)
				return(false);

			if (d_context->GetState() != CallContext::Available)
			{
				if (d_context->GetState() == CallContext::CallSuccess)
				{
					if (d_queryList == 0)
					{
						d_queryList = 1;
						d_resultsList = 0;
					}
					else
					{
						d_queryList = 0;
						d_resultsList = 1;
					}
					result = true;
					SortResults(d_list[d_resultsList]);
				}
				else
				{
					ASSERT(false);
					d_nextQuery = g_timer.GetOSTime();
				}
				d_context->Reset();
			}
			delete d_context;
			d_context = NULL;
		}

		uint32 currentTime = g_timer.GetOSTime();
		d_list[d_queryList].clear();

//		if (currentTime >= d_nextQuery)
		if (reFresh)
		{
			ASSERT(d_context == NULL);
			d_context = new CallContext;
			if (Session::QuerySessions(d_context, &d_list[d_queryList], true,
						d_timeout))
			{
				d_nextQuery = currentTime + d_period;
			}
		}
	}
	return(result);
}

/******************************************************************************/
/******************************************************************************/
CSessionSearch::EEndStatus CNetZSessionSearch::End(void)
{
	if (d_status != INACTIVE)
	{
		CallContext::State callState = CallContext::Available;
		if (d_context)
		{
			if (d_connection.IsEnabled())
			{
				d_connection.Service();
				callState = d_context->GetState();
				if (callState == CallContext::CallInProgress)
				{
					d_status = ENDING;
					return(SEARCH_IN_PROGRESS);
				}
			}
			delete d_context;
			d_context = NULL;
		}
		d_list[0].clear();
		d_list[1].clear();
		d_connection.Disable();
		d_status = INACTIVE;

		if ((callState == CallContext::CallFailure) ||
				(callState == CallContext::CallCancelled))
		{
			ASSERT(false);
			return(SEARCH_ERROR);
		}
	}
	return(SEARCH_CLEAN);
}

/******************************************************************************/
/******************************************************************************/
void CNetZSessionSearch::SortResults(qList<SessionDescription>& list)
{
	// std::sort or list.sort() is wonky right now because we don't really
	// compile too well with STL.


	// --- DAS, ok not sure how to do this properly under STL, so I've just thrown together
	// --- a lame bubble sort to do the job for now.
	int i = list.size() - 1;
	SessionDescription placeHolder;
	const qChar *name1 = NULL;
	const qChar *name2 = NULL;
	qList<SessionDescription>::iterator iter;
	qList<SessionDescription>::iterator iterOld;

	// --- make sure we have at least 2 items to sort
	if( i < 1 ) return;

	// --- sort it
	bool sorted = false;
	while( !sorted )
	{
		// --- assume we are done
		sorted = true;
		iter = list.begin();
		while(iter != list.end())
		{
			iterOld = iter;
			SessionDescription* desc1 = &( *iter );
			if (desc1 != NULL)
			{
				name1 = desc1->GetSessionName();
			}			
			iter++;
			if( iter != list.end() )
			{
				SessionDescription* desc2 = &( *iter );
				if (desc1 != NULL && desc2 != NULL)
				{
					name2 = desc2->GetSessionName();
					if( strcmp( name1, name2 ) > 0 )
					{
						placeHolder = *iter;
						*iter = *iterOld;
						*iterOld = placeHolder;
						sorted = false;
					}
				}
			}			
		}
	}
}

/******************************************************************************/
/******************************************************************************/
int CNetZSessionSearch::GetNumFound(void)
{
	return(d_list[d_resultsList].size());
}

/******************************************************************************/
/******************************************************************************/
SessionDescription* CNetZSessionSearch::GetDescription(int i_index)
{
	int i = 0;
	qList<SessionDescription>::iterator iter;
	iter = d_list[d_resultsList].begin();
	while(iter != d_list[d_resultsList].end())
	{
		if (i_index == i++)
			return(&(*iter));
		iter++;
	}
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
const char* CNetZSessionSearch::GetName(int i_index)
{
	SessionDescription* desc = GetDescription(i_index);
	if (desc != NULL)
		return(desc->GetSessionName());
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
const char* CNetZSessionSearch::GetMapName(int i_index)
{
	// We only support map index
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
int CNetZSessionSearch::GetMapIndex(int i_index)
{
	SessionDescription* desc = GetDescription(i_index);
	if (!desc)
		return(-1);

	ParameterSet parse(CNetZSession::GetConfig(desc));
	int result = parse.GetInt("gm");
	return((result != MAXINT) ? result : -1);
}

/******************************************************************************/
/******************************************************************************/
const char* CNetZSessionSearch::GetMode(int i_index)
{
	SessionDescription* desc = GetDescription(i_index);
	if (!desc)
		return(NULL);

	ParameterSet parse(CNetZSession::GetConfig(desc));
	const char* modeString = parse.GetString("md");
	if (modeString)
	{
		static char tempString[ 16 ];
		ASSERT( strlen(modeString) < sizeof(tempString) );
		strcpy( tempString, modeString );
		return tempString;
	}
	else
		return NULL;
}

/******************************************************************************/
/******************************************************************************/
const char* CNetZSessionSearch::GetPlayers(int i_index)
{
	SessionDescription* desc = GetDescription(i_index);
	if (!desc)
		return ("0/16");

	static char tempString[16];
	ParameterSet parse(CNetZSession::GetConfig(desc));
	const char* countStr = parse.GetString("np");
	const char* maxStr = parse.GetString("mp");
	ASSERT((1 + ((countStr != NULL) ? strlen(countStr) : 1) +
			((maxStr != NULL) ? strlen(maxStr) : 2)) < sizeof(tempString));
	sprintf(tempString, "%s/%s", (countStr != NULL) ? countStr : "0",
			(maxStr != NULL) ? maxStr : "16");
	return(tempString);
}

/******************************************************************************/
/******************************************************************************/
const char* CNetZSessionSearch::GetPing(int i_index)
{
	static char ping[3] = "";
#if 1
	sprintf( ping, "%d\0", g_random.InRange( 3, 7 ) );
#else
	// --- this will only work once we are connected to a session
	// --- Martin from Quazal says there is no way to get the ping
	// --- on the LAN through NetZ right now.
	Station::Ref station( );
	if( ValidSingleton( CSession ) )
	{
		DOHost *localHost = (DOHost*)g_session.GetLocalHost();
		if( localHost )
		{
			DOHandle dohStation = localHost->GetLocalHandle();
			Station::Ref station( dohStation );
			if( station.IsValid() )
				sprintf( ping, "%d\0", station->GetRTT() );
		}
	}
#endif
	return (ping);
}

/******************************************************************************/
/******************************************************************************/
const char* CNetZSessionSearch::GetConfig(int i_index)
{
	return(CNetZSession::GetConfig(GetDescription(i_index)));
}

/******************************************************************************/
/******************************************************************************/
const char* CNetZSessionSearch::GetSessionID(int i_index)
{
	SessionDescription* desc = GetDescription(i_index);
	if (desc == NULL)
		return(NULL);

	sprintf(d_sessionID, "0x%x", desc->GetSessionID());
	return(d_sessionID);
}

/******************************************************************************/
/******************************************************************************/
bool CNetZSessionSearch::GetFilled(int i_index)
{
	SessionDescription* desc = GetDescription(i_index);
	if (!desc)
		return (false);

	ParameterSet parse(CNetZSession::GetConfig(desc));
	const char* countStr = parse.GetString("np");
	const char* maxStr = parse.GetString("mp");
	int count = (countStr != NULL) ? atoi(countStr) : 0;
	int max = (maxStr != NULL) ? atoi(maxStr) : 0;
	if (max == 0)
		max = 16;
	return(count >= max);
}

/******************************************************************************/
/******************************************************************************/
bool CNetZSessionSearch::SpecifyFoundSession(int i_index,
		CSessionSpec &i_sessionSpec)
{
	i_sessionSpec.Clear();
	SessionDescription* desc = GetDescription(i_index);
	if (desc != NULL)
		return(i_sessionSpec.AddEntry(desc->GetStationURL()));
	return(false);
}

/******************************************************************************/
/******************************************************************************/
bool CNetZSessionSearch::SpecifyNamedSession(const char* i_name,
		CSessionSpec &i_sessionSpec, uint32 i_timeout)
{
	ASSERT((d_status == INACTIVE) && !d_connection.IsEnabled());
	bool found = false;
	i_sessionSpec.Clear();
	if ((d_status == INACTIVE) && d_connection.Enable(true))
	{
		if ((i_name != NULL) && (i_name[0] != '\0'))
		{
			qList<SessionDescription> sessionList;
			unsigned int result =
					Session::QuerySessions(&sessionList, true, i_timeout);
			if (result != 0)
			{
				qList<SessionDescription>::iterator iter;
				iter = sessionList.begin();
				while (iter != sessionList.end())
				{
					if (!strcmp(i_name, iter->GetSessionName()) &&
							i_sessionSpec.AddEntry(iter->GetStationURL()))
					{
						found = true;
						break;
					}
					iter++;
				}
			}
		}
		d_connection.Disable();
	}
	return(found);
}

/******************************************************************************/
/******************************************************************************/
bool CNetZSessionSearch::SpecifyHostedSession(const char* i_name,
		CSessionSpec &i_sessionSpec, uint32 i_timeout)
{
	ASSERT((d_status == INACTIVE) && !d_connection.IsEnabled());
	i_sessionSpec.Clear();
	if ((d_status == INACTIVE) && (i_name != NULL) && (i_name[0] != '\0') &&
			d_connection.Enable(true))
	{
		qList<SessionDescription> sessionList;
		unsigned int result =
				Session::QuerySessions(&sessionList, true, i_timeout);
		d_connection.Disable();
		if (result != 0)
		{
			StationURL url(i_name);
			qList<SessionDescription>::iterator iter;
			iter = sessionList.begin();
			while (iter != sessionList.end())
			{
				if (url == iter->GetStationURL())
				{
					if (i_sessionSpec.AddEntry(iter->GetStationURL()))
						return(true);
				}
				iter++;
			}
		}
	}
	return(false);
}
