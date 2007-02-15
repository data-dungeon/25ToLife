/*
**
**  File:   ActorCreator.cpp
**  Date:   May 28, 2004
**  By:     Bryant Collard
**  Desc:   A mechanism for creating actors with a given name.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/stage/ActorCreator.cpp $
**  $Revision: #3 $
**  $DateTime: 2005/03/29 17:16:52 $
**    $Author: Rob $
**
*/

// Pre-compiled header
#include "stage/StagePCH.h"

// My header
#include "stage/ActorCreator.h"

// Force a reference to object files so the linker won't skip them.
EXPORT_ACTOR_CREATOR(CPlainActor);
EXPORT_ACTOR_CREATOR(CProtagonist);
EXPORT_ACTOR_CREATOR(CCompActor);

// Declare actor creator head. For this stuff to work, the compiler must
// initialize it to null before it is referenced for the first time (possibly
// before this translation unit is processed). ANSI C specifies that any global
// variable declared with the static storage class specifier and of pointer type
// that is not explicitly initialized is assigned a null pointer when program
// execution begins.
CActorCreator* CActorCreator::s_head;

/******************************************************************************/
/******************************************************************************/
CActor* CActorCreator::Create(const char* i_name)
{
	CActorCreator* creator = (CActorCreator*)CCreator::Find(i_name, s_head);
	if (creator != NULL)
		return(creator->Create());
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
bool CActorCreator::Query(const char* i_name)
{
	return(CCreator::Query(i_name, s_head));
}
