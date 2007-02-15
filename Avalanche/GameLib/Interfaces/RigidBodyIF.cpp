/*
**
**  File:   RigidBodyIF.cpp
**  Date:   July 2, 2004
**  By:     Bryant Collard
**  Desc:   An interface to a rigid body.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Interfaces/RigidBodyIF.cpp $
**  $Revision: #3 $
**  $DateTime: 2005/03/29 17:16:52 $
**    $Author: Rob $
**
*/

#include "Interfaces/InterfacesPCH.h"
#include "Interfaces/RigidBodyIF.h"
#include "stage/cactor.h"

uint CRigidBodyIF::s_rigidBodyID = INTERFACE_ID_INVALID;

/******************************************************************************/
/******************************************************************************/
CRigidBodyIF* CRigidBodyIF::GetInterface(CActor* i_actor)
{
	if (i_actor != NULL)
		return((CRigidBodyIF*)i_actor->GetInterface(GetID()));
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
CRigidBodyIF* CRigidBodyIF::GetInterface(ActorHandle i_handle)
{
	CActor* actor = CActor::FromHandle(i_handle);
	if (actor != NULL)
		return((CRigidBodyIF*)actor->GetInterface(GetID()));
	return(NULL);
}
