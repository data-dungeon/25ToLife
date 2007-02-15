/*
**
**  File:   SessionSpec.h
**  Date:   Septemper 16, 2004
**  By:     Bryant Collard
**  Desc:   Specification used to create or join a session.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/SessionSpec.h $
**  $Revision: #3 $
**  $DateTime: 2005/01/14 12:32:01 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef SESSION_SPEC_H
#define SESSION_SPEC_H

// GameLib includes
#include "EngineHelper/Singleton.h"

class CSessionSpec
{
  public:
	// Construct
	CSessionSpec();

	// Clear any existing spec.
	void Clear(void);

	// Get the session name. If it returns NULL, no name has been set.
	const char* GetName(void);

	// Set the session name. Any entries will be cleared. NULL or empty strings
	// are ignored.
	void SetName(const char* i_name);

	// Get the number of entries.
	uint GetNumEntries(void) const {return(d_numEntries);}

	// Get a specific entry. If it returns NULL, an attempt was made to access
	// a non-existent entry.
	const char* GetEntry(uint i_index);

	// Add an entry. Returns true if successful. Any session name will be
	// cleared. NULL or empty strings are ignored and false will be
	// returned. False will also be returned if no empty entries are available.
	bool AddEntry(const char* i_entry);

  protected:
	virtual char* Entry(uint i_index) = 0;

	uint d_maxEntryLen;
	uint d_maxEntries;

  private:
	uint d_numEntries;
};

template<uint t_maxEntrySize, uint t_maxEntries>
class CSessionSpecInstance : public CSessionSpec
{
  public:
	CSessionSpecInstance()
	{
		d_maxEntryLen = t_maxEntrySize - 1;
		d_maxEntries = t_maxEntries;

		Clear();
		for (int i = 0; i < t_maxEntries; i++)
			d_entry[i][d_maxEntryLen] = '\0';
	}

  private:
	virtual char* Entry(uint i_index) {return(d_entry[i_index]);}

	char d_entry[t_maxEntries][t_maxEntrySize];
};

DefineSingleton(CSessionSpec)
#define g_sessionSpec GetSingleton(CSessionSpec)

#endif // SESSION_SPEC_H
