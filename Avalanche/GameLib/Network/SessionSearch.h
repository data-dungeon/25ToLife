/*
**
**  File:   SessionSearch.h
**  Date:   October 22, 2004
**  By:     Bryant Collard
**  Desc:   Search for sessions.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/SessionSearch.h $
**  $Revision: #4 $
**  $DateTime: 2005/08/17 16:27:35 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef SESSION_SEARCH_H
#define SESSION_SEARCH_H

class CSessionSpec;

class CSessionSearch
{
  public:
	// Constructor
	CSessionSearch() {d_period = 0; d_timeout = 1000;}

	// Destructor
	virtual ~CSessionSearch() {}

	// When a repetitive search is being conducted, a session query is submitted
	// periodically. Set the time between queries in milliseconds.
	void SetQueryPeriodMS(uint32 i_period) {d_period = i_period;}

	// The session queries submitted during a repetitive are asynchronous. Set
	// the maximum amount of time allowed between a query submission and its
	// response.
	void SetQueryTimeoutMS(uint32 i_timeout) {d_timeout = i_timeout;}

	// Setup to perform a repetitive search. Note that this routine does not
	// submit a session query.
	virtual void Begin(void) {}

	// Update a repetitive search. Queries will be submitted at the specified
	// rate. The return value is normally false. A value of true is returned if
	// a query has been successfully completed since the last call to Update().
	// A true return indicates that the found sessions may have changed.
	virtual bool Update(bool reFresh = false) {return(false);}

	// Cleanup when a repetitive search is to be performed no longer. Since
	// repetitive searches are asynchronous by nature, it may not be possible
	// to shut it down immediately. This routine should be called repeatedly
	// until it indicates a clean ending.
	enum EEndStatus
	{
		SEARCH_IN_PROGRESS,
		SEARCH_CLEAN,
		SEARCH_ERROR
	};
	virtual EEndStatus End(void) {return(SEARCH_CLEAN);}

	// Find out if repetitive searching has been activated.
	virtual bool IsActive(void) {return(false);}

	// Get the number of sessions returned by the most recent successful session
	// query.
	virtual int GetNumFound(void) {return(0);}

	// Get the name of a specific session returned by the most recent successful
	// session query. Returns NULL if i_index is less than 0 or greater than
	// or equal to the number of sessions found.
	virtual const char* GetName(int i_index) {return(NULL);}

	// Returns the map name for the specified session
	virtual const char* GetMapName(int i_index) {return(NULL);}

	// Returns the map index for the specified session
	virtual int GetMapIndex(int i_index) {return(-1);}

	// Returns the mode for the specified session
	virtual const char* GetMode(int i_index) {return(NULL);}

	// Returns the number of players connected to the specified session
	virtual const char* GetPlayers(int i_index) {return(NULL);}

	// Returns your ping to the current session
	virtual const char* GetPing(int i_index) {return(NULL);}

	// Get the configuration string of a specific session returned by the most
	// recent successful session query. Returns NULL if i_index is less than 0 or
	// greater than or equal to the number of sessions found.
	virtual const char* GetConfig(int i_index) {return(NULL);}

	// Gets the ID for the current match
	virtual const char* GetSessionID(int i_index) {return(NULL);}

	// Determines if the specified session is full.
	virtual bool GetFilled(int i_index) {return(false);}

	// Load a specific session returned by the most recent successful session
	// query into the supplied session specification. Returns false if i_index is
	// less than 0 or greater than or equal to the number of sessions found.
	virtual bool SpecifyFoundSession(int i_index, CSessionSpec &i_sessionSpec)
		{return(false);}

	// Perform a one-shot search for a session with a given name. If a session is
	// found within i_timeout milliseconds, it is loaded into the supplied
	// session specification and true is returned.
	virtual bool SpecifyNamedSession(const char* i_name,
			CSessionSpec &i_sessionSpec, uint32 i_timeout = 1000)
		{return(false);}

	// Perform a one-shot search for a session with the given host url. If a
	// session is found within i_timeout milliseconds, it is loaded into the
	// supplied session specification and true is returned.
	virtual bool SpecifyHostedSession(const char* i_name,
			CSessionSpec &i_sessionSpec, uint32 i_timeout = 1000)
		{return(false);}

  protected:
	uint32 d_nextQuery;
	uint32 d_period;
	uint32 d_timeout;
};

#endif // SESSION_SEARCH_H
