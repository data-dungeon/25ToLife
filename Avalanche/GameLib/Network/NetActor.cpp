/*
**
**  File:   NetActor.cpp
**  Date:   October 14, 2003
**  By:     Bryant Collard
**  Desc:   A base for classes that manage the distributed aspects of actors.
**          An actor will point to one of these things if the game is
**          multi-player. Issues specific to the network implementation should
**          be contained here and below.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetActor.cpp $
**  $Revision: #5 $
**  $DateTime: 2004/09/09 20:55:23 $
**    $Author: Bryant_Collard $
**
*/

// Precompiled header
#include "Network/NetworkPCH.h"

// My header
#include "Network/NetActor.h"

/**********************************************************************/
/**********************************************************************/
CNetActor::CNetActor()
{
	d_persistent = false;
	d_actor = NULL;
}

/**********************************************************************/
/**********************************************************************/
CNetActor::~CNetActor()
{
	CNetActor::Unbind();
}

/**********************************************************************/
/**********************************************************************/
void CNetActor::Distribute(CActor* i_master, bool i_persistent)
{
	ASSERT(d_actor == NULL);
	d_actor = i_master;
	d_persistent = i_persistent;
}

/**********************************************************************/
/**********************************************************************/
void CNetActor::BindProxy(CActor* i_proxy)
{
	ASSERT(d_actor == NULL);
	d_actor = i_proxy;
}

/**********************************************************************/
/**********************************************************************/
void CNetActor::Unbind(bool i_kill)
{
	if (d_actor != NULL)
	{
		CActor* actor = d_actor;
		d_actor = NULL;
		actor->UnbindNetActor(i_kill);
	}
}
