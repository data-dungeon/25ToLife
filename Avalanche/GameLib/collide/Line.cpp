/*
**
**  File:   Line.cpp
**  Date:   January 31, 2003
**  By:     Bryant Collard
**  Desc:   A line collision object.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/becCollide/Line.cpp $
**  $Revision: 4 $
**      $Date: 4/24/03 11:22a $
**    $Author: Bcollard $
**
*/

#include "collide/Line.h"

const uint CLineCollisionObject::BOUNDARY_NONE       = LINE_BOUNDARY_NONE;
const uint CLineCollisionObject::BOUNDARY_MIN        = LINE_BOUNDARY_MIN;
const uint CLineCollisionObject::BOUNDARY_MAX        = LINE_BOUNDARY_MAX;
const uint CLineCollisionObject::BOUNDARY_MIN_MAX    =
		CLineCollisionObject::BOUNDARY_MIN |
		CLineCollisionObject::BOUNDARY_MAX;

const uint CLineCollisionObject::DETECT_NEITHER_SIDE = 0x00;
const uint CLineCollisionObject::DETECT_FRONT_SIDE   = 0x04;
const uint CLineCollisionObject::DETECT_BACK_SIDE    = 0x08;
const uint CLineCollisionObject::DETECT_BOTH_SIDES   =
		CLineCollisionObject::DETECT_FRONT_SIDE |
		CLineCollisionObject::DETECT_BACK_SIDE;

const uint CLineCollisionObject::FIND_ALL            = LINE_FIND_ALL;
const uint CLineCollisionObject::FIND_CLOSEST        = LINE_FIND_CLOSEST;
const uint CLineCollisionObject::FIND_FURTHEST       = LINE_FIND_FURTHEST;
const uint CLineCollisionObject::FIND_ANY            = LINE_FIND_ANY;

const uint CLineCollisionObject::BOUNDARY_MASK       =
		CLineCollisionObject::BOUNDARY_MIN_MAX;
const uint CLineCollisionObject::DETECT_MASK         =
		CLineCollisionObject::DETECT_BOTH_SIDES;
const uint CLineCollisionObject::FIND_MASK           =
		CLineCollisionObject::FIND_ANY;
