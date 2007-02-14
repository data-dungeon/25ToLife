/*
**
**  File:   DOGlass.cpp
**  Date:   January 13, 2004
**  By:     Bryant Collard
**  Desc:   Implementation of the glass distributed object.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/NetZTTL/DOGlass.cpp $
**  $Revision: #18 $
**  $DateTime: 2005/05/03 18:53:50 $
**    $Author: Bryant_Collard $
**
*/

// Precompiled header
#include "TTLPCH.h"

// My header
#include "NetZTTL/DOGlass.h"

// TTL headers
#include "environ/glass.h"

/**********************************************************************/
/**********************************************************************/
CActor* DOGlass::CreateProxy(const char* i_class)
{
	if (d_state.d_shattered)
		return(NULL);

	// Create the actor and delete it when no longer in a cast.
	CGlass *actor = new CGlass;
	ASSERT(actor != NULL);

	return((CActor*)actor);
}

/**********************************************************************/
/**********************************************************************/
void DOGlass::InitMaster(void)
{
	d_state.d_shattered = ((CGlass*)d_actor)->GlassShattered();
	d_actor->SetStateDistributed(true);

	DOActor::InitMaster();
}

/******************************************************************************/
/******************************************************************************/
void DOGlass::BecomeMaster(void)
{
	d_actor->SetStateDistributed(true);
	DOActor::BecomeMaster();
}

/**********************************************************************/
/**********************************************************************/
void DOGlass::PropagateMaster(float i_gameTime)
{
	if (!d_state.d_shattered && (d_actor != NULL) &&
			((CGlass*)d_actor)->GlassShattered())
	{
		d_state.d_shattered = true;
		d_state.RequestUpdate();
	}
	DOActor::PropagateMaster(i_gameTime);
}

/**********************************************************************/
/**********************************************************************/
void DOGlass::StateChanged(void)
{
	if (d_state.d_shattered && (d_actor != NULL))
		((CGlass*)d_actor)->ShatterGlass();
}
