/*
**
**  File:   NetZSessionSearch.h
**  Date:   October 22, 2004
**  By:     Bryant Collard
**  Desc:   Search for NetZ sessions.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/NetZSessionSearch.h $
**  $Revision: #5 $
**  $DateTime: 2005/08/17 16:27:35 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef NETZ_SESSION_SEARCH_H
#define NETZ_SESSION_SEARCH_H

// GameLib includes
#include "Network/SessionSearch.h"

// NetZ includes
#include <netz.h>
#include "Network/NetZ/NetZConnection.h"

class CNetZSessionSearch : public CSessionSearch
{
  public:
	// Construction
	CNetZSessionSearch();
	virtual ~CNetZSessionSearch();

	// Conduct search.
	virtual void Begin(void);
	virtual bool Update(bool reFresh = false);
	virtual EEndStatus End(void);
	virtual bool IsActive(void) {return(d_status != INACTIVE);}

	// Access search results.
	virtual int GetNumFound(void);
	virtual const char* GetName(int i_index);
	virtual const char* GetMapName(int i_index);
	virtual int GetMapIndex(int i_index);
	virtual const char* GetMode(int i_index);
	virtual const char* GetPlayers(int i_index);
	virtual const char* GetPing(int i_index);
	virtual const char* GetConfig(int i_index);
	virtual const char* GetSessionID(int i_index);
	virtual bool GetFilled(int i_index);

	virtual bool SpecifyFoundSession(int i_index, CSessionSpec &i_sessionSpec);

	// Conduct specific search.
	virtual bool SpecifyNamedSession(const char* i_name,
			CSessionSpec &i_sessionSpec, uint32 i_timeout = 1000);
	virtual bool SpecifyHostedSession(const char* i_name,
			CSessionSpec &i_sessionSpec, uint32 i_timeout = 1000);

  private:
	SessionDescription* GetDescription(int i_index);
	void SortResults(qList<SessionDescription>& list);

	enum
	{
		INACTIVE,
		UPDATING,
		ENDING
	} d_status;
	CNetZConnection d_connection;
	CallContext* d_context;
	int d_queryList;
	int d_resultsList;
	qList<SessionDescription> d_list[2];
	char d_sessionID[64];
};

#endif // NETZ_SESSION_SEARCH_H
