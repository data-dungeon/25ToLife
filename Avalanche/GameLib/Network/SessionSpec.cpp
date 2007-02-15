/*
**
**  File:   SessionSpec.cpp
**  Date:   Septemper 16, 2004
**  By:     Bryant Collard
**  Desc:   Specification used to create or join a session.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/SessionSpec.cpp $
**  $Revision: #1 $
**  $DateTime: 2004/12/16 19:58:42 $
**    $Author: Rob $
**
*/

// Precompiled header
#include "Network/NetworkPCH.h"

// My header
#include "Network/SessionSpec.h"

// System includes
#include <string.h>

DeclareSingleton(CSessionSpec);

/******************************************************************************/
/******************************************************************************/
CSessionSpec::CSessionSpec()
{
	d_maxEntryLen = 0;
	d_maxEntries = 0;
	d_numEntries = 0;
}

/******************************************************************************/
/******************************************************************************/
void CSessionSpec::Clear(void)
{
	d_numEntries = 0;
	if (d_maxEntries > 0)
	{
		char name[1] = {'\0'};
		strncpy(Entry(0), name, d_maxEntryLen);
	}
}

/******************************************************************************/
/******************************************************************************/
const char* CSessionSpec::GetName(void)
{
	if ((d_numEntries > 0) || (d_maxEntries == 0) || (Entry(0)[0] == '\0'))
		return(NULL);
	return(Entry(0));
}

/******************************************************************************/
/******************************************************************************/
void CSessionSpec::SetName(const char* i_name)
{
	if ((i_name != NULL) || (i_name[0] != '\0') && (d_maxEntries > 0))
	{
		ASSERT(strlen(i_name) <= d_maxEntryLen);
		d_numEntries = 0;
		strncpy(Entry(0), i_name, d_maxEntryLen);
	}
}

/******************************************************************************/
/******************************************************************************/
const char* CSessionSpec::GetEntry(uint i_index)
{
	ASSERT(i_index < d_numEntries);
	if (i_index < d_numEntries)
		return(Entry(i_index));
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
bool CSessionSpec::AddEntry(const char* i_entry)
{
	if ((i_entry != NULL) && (i_entry[0] != '\0') &&
			(d_numEntries < d_maxEntries))
	{
		ASSERT(strlen(i_entry) <= d_maxEntryLen);
		strncpy(Entry(d_numEntries), i_entry, d_maxEntryLen);
		d_numEntries++;
		return(true);
	}
	ASSERT(false);
	return(false);
}
