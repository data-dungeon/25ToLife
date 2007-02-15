/*
**
**  File:   Creator.cpp
**  Date:   May 21, 2004
**  By:     Bryant Collard
**  Desc:   A mechanism for creating an object with a given name.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/GameHelper/Creator.cpp $
**  $Revision: #3 $
**  $DateTime: 2004/12/16 19:58:42 $
**    $Author: Rob $
**
*/

// Pre-compiled header
#include "GameHelper/GameHelperPCH.h"

// My header
#include "GameHelper/Creator.h"

/******************************************************************************/
/******************************************************************************/
void CCreator::Add(CCreator** io_head)
{
	// Make sure a creator is only added once.
	CCreator* head = *io_head;
	while (head != NULL)
	{
		if (head == this)
		{
			ASSERTF(false, ("Creator of '%s' added to creator list multiple times",
					head->Name()));
			return;
		}
		if (!strcmp(head->Name(), Name()))
		{
			ASSERTF(false, ("Creator for '%s' added to creator list multiple times",
					head->Name()));
			return;
		}
		head = head->d_next;
	}
	d_next = *io_head;
	*io_head = this;
}

/******************************************************************************/
/******************************************************************************/
CCreator* CCreator::Find(const char* i_name, CCreator* i_head)
{
	while (i_head != NULL)
	{
		if (!stricmp(i_name, i_head->Name()))
			return(i_head);
		i_head = i_head->d_next;
	}
	ASSERTF(false, ("Creator of '%s' unknown", i_name));
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
bool CCreator::Query(const char* i_name, CCreator* i_head)
{
	while (i_head != NULL)
	{
		if (!stricmp(i_name, i_head->Name()))
			return(true);
		i_head = i_head->d_next;
	}
	return(false);
}
