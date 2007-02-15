/*
**
**  File:   DOTalker.h
**  Date:   December 3, 2004
**  By:     Bryant Collard
**  Desc:   Implementation of the talker distributed object.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/DOTalker.h $
**  $Revision: #3 $
**  $DateTime: 2005/03/29 17:16:52 $
**    $Author: Rob $
**
*/

#pragma once

#ifndef DO_TALKER_H
#define DO_TALKER_H

#include <netz.h>
#include "Network/NetZ/DOTalkerDDL.h"
#include "Math/Vector.h"

class DOTalker : public DOCLASS(DOTalker)
{
  public:
	virtual Vector3 GetLocation(void) const {return(Vector3(0.0f, 0.0f, 0.0f));}
	virtual Vector3 GetDirection(void) const {return(Vector3(0.0f, 0.0f, 1.0f));}
	virtual Vector3 GetVelocity(void) const {return(Vector3(0.0f, 0.0f, 0.0f));}
	virtual bool IsAlive(void) const {return(true);}
};

#endif // DO_TALKER
