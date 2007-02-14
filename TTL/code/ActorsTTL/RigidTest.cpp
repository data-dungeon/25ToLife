/*
**
**  File:   RigidTest.cpp
**  Date:   January 6, 2004
**  By:     Bryant Collard
**  Desc:   A temporary class that prepares for that design and implementation
**          of a more general dynamically configurable actor.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/ActorsTTL/RigidTest.cpp $
**  $Revision: #5 $
**  $DateTime: 2005/03/29 17:16:52 $
**    $Author: Rob $
**
*/

#include "TTLPCH.h"

#include "ActorsTTL/RigidTest.h"
#include "stage/ActorCreator.h"
#include "CollideHelper/ObbNode.h"

// Define a creator and add it to the creator list.
STANDARD_ACTOR_CREATOR(CRigidTest, rigid);

uint8 CRigidTest::Material(CCollisionPair* i_pair)
{
	return(d_surfaceType);
}

bool CRigidTest::Initialize(void)
{
	CObbNode* obbNode = new CObbNode;
	SetCollisionNode(obbNode, SIMPLE, true);
	obbNode->FitObbToActor();
	Vector3 size(obbNode->GetXSize(), obbNode->GetYSize(), obbNode->GetZSize());
	Vector3 maxExtent = obbNode->GetCollisionInActor() + 0.5f * size;
	size.Y(size.X());
	obbNode->SetExtents(maxExtent - size, maxExtent);

	if (!CCompActor::Initialize())
		return(false);

	char surface[32] = { "throne" };
	d_surfaceType =
			g_surfaceType.GetValidDynamicIndex(surface, d_surfaceType, sizeof(surface) - 1);

	CRigidBody* body = (CRigidBody*)AttachComponent("RigidBody");
	ASSERT_PTR(body);

	body->SetMass(10.0f);
	body->SetInertia(10.0f, 10.0f, 10.0f);
	body->SetCG(obbNode->GetCollisionInActor(), obbNode->GetCollisionToActor());
	body->SetBodyAcceleration(Vector3(0.0f, -9.81f, 0.0f));

	DirCos3x3 m;
	m.SetYRotation(Math::QuarterPi);
	m.CatXRotation(0.2f);
	body->SetCGToWorld(m);
	body->SetAngVelocityInCG(Vector3(6.0f, 3.0f, 8.0f));

	InitializeComponents();

//	SetPositiveInclude(DEBUG_DRAW_CATEGORY);

	return(true);
}

void CRigidTest::BeginUpdate(void)
{
	CRigidBody* body = (CRigidBody*)GetComponentByName("RigidBody");
	ASSERT_PTR(body);

	Vector3 force(0.0f, -98.0f, 0.0f);
	body->ApplyForceCG(force / d_bodyToWorld);

	CCompActor::BeginUpdate();
}
