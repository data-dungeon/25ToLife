/*
**
**  File:   ComponentCreator.cpp
**  Date:   May 28, 2004
**  By:     Bryant Collard
**  Desc:   A mechanism for creating components with a given name.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Components/ComponentCreator.cpp $
**  $Revision: #10 $
**  $DateTime: 2005/08/19 15:38:47 $
**    $Author: Bryant_Collard $
**
*/

// Pre-compiled header
#include "Components/ComponentsPCH.h"

// My header
#include "Components/ComponentCreator.h"

// Local headers
#include "Components/Component.h"

// Force a reference to object files so the linker won't skip them.
EXPORT_COMPONENT_CREATOR(CBasicMover);
EXPORT_COMPONENT_CREATOR(CInstantMover);
EXPORT_COMPONENT_CREATOR(CCollisionInfo);
EXPORT_COMPONENT_CREATOR(CPathFollower);
EXPORT_COMPONENT_CREATOR(CPathFollowerCam);
EXPORT_COMPONENT_CREATOR(CTeleport);
EXPORT_COMPONENT_CREATOR(GroundFollower);
EXPORT_COMPONENT_CREATOR(InteractionMonitor);
EXPORT_COMPONENT_CREATOR(WeaponInventory);
EXPORT_COMPONENT_CREATOR(ViewComponent);
EXPORT_COMPONENT_CREATOR(CRigidBody);
EXPORT_COMPONENT_CREATOR(CInventoryList);
EXPORT_COMPONENT_CREATOR(Rider);
EXPORT_COMPONENT_CREATOR(DOGenericComponent);
EXPORT_COMPONENT_CREATOR(DOPathFollowerComponent);
EXPORT_COMPONENT_CREATOR(CGroundHeight);

// Declare component creator head. For this stuff to work, the compiler must
// initialize it to null before it is referenced for the first time (possibly
// before this translation unit is processed). ANSI C specifies that any global
// variable declared with the static storage class specifier and of pointer type
// that is not explicitly initialized is assigned a null pointer when program
// execution begins.
CComponentCreator* CComponentCreator::s_head;

/******************************************************************************/
/******************************************************************************/
CActorComponent* CComponentCreator::Create(const char* i_name,
		CCompActor &i_actor)
{
	CComponentCreator* creator =
			(CComponentCreator*)CCreator::Find(i_name, s_head);
	if (creator != NULL)
	{
		CActorComponent* component = creator->Create(i_actor);
		if (component != NULL)
			component->SetPriority(creator->Priority());
		return(component);
	}
	return(NULL);
}
