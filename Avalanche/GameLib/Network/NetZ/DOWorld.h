/*
**
**  File:   DOWorld.h
**  Date:   March 2, 2004
**  By:     Bryant Collard
**  Desc:   Implementation of the world distributed object.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

#pragma once

#ifndef DO_WORLD_H
#define DO_WORLD_H

#include <netz.h>
#include "Network/NetZ/DOWorldDDL.h"
#include "Environ/World.h"

class DOWorld : public DOCLASS(DOWorld), public CWorld
{
  public:
	DOWorld();
	virtual void OperationEnd(DOOperation* i_operation);
	virtual void Update(void);
	virtual void ClearExtents(void);
	virtual void SetExtents(Vector3CRef i_center, Vector3CRef i_extent);
	void StartSessionClock(void);
	Time GetStartTime(void);

  protected:
	virtual CMessageOwner* MessageOwner(void);
};

#endif // DO_WORLD
