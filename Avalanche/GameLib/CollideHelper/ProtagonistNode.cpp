/*
**
**  File:   ProtagonistNode.cpp
**  Date:   July 12, 2004
**  By:     Bryant Collard
**  Desc:   A base class for collision nodes that also provide some
**          functionality expected from protagonists.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/CollideHelper/ProtagonistNode.cpp $
**  $Revision: #2 $
**  $DateTime: 2004/07/28 11:27:13 $
**    $Author: Bryant_Collard $
**
*/

// Pre-compiled header
#include "CollideHelper/CollideHelperPCH.h"

// My header (always first)
#include "CollideHelper/ProtagonistNode.h"

// Engine includes //
#include "Game/Database/Som.h"     // For INVALID_MATERIALINDEX

/******************************************************************************/
/******************************************************************************/
uint16 CProtagonistNode::Color(CCollisionPair* i_pair)
{
	return(0xffff);
}

/******************************************************************************/
/******************************************************************************/
uint8 CProtagonistNode::Material(CCollisionPair* i_pair)
{
	return(INVALID_MATERIALINDEX);
}
