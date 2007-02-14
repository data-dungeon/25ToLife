/*
**
**  File:   RigidTest.h
**  Date:   January 6, 2004
**  By:     Bryant Collard
**  Desc:   A temporary class that prepares for that design and implementation
**          of a more general dynamically configurable actor.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/ActorsTTL/RigidTest.h $
**  $Revision: #2 $
**  $DateTime: 2005/03/29 17:16:52 $
**    $Author: Rob $
**
*/

#pragma once

#ifndef RIGID_TEST_H
#define RIGID_TEST_H

#include "stage/CCompActor.h"
#include "Components/Component.h"
#include "Components/RigidBody.h"

class CRigidTest : public CCompActor
{
  public:
	CRigidTest() {d_surfaceType = 0;}
	virtual const char* GetClassName(void) {return("RigidTest");}
	uint8 Material(CCollisionPair* i_pair);
	virtual bool Initialize(void);
	virtual void BeginUpdate(void);

  private:
	int d_surfaceType;
};

#endif // RIGID_TEST_H
