//
//
//  File:   ActorDefines.h
//  Date:   May 1, 2004
//  By:     Bryant Collard
//  Desc:   Some constants used by actors AND within scripts.
//
//  Copyright (c) 2004, Avalanche Software Inc.
//  All Rights Reserved.
//
//      $File: //depot/Avalanche/GameLib/stage/ActorDefines.h $
//  $Revision: #3 $
//  $DateTime: 2004/11/10 16:02:23 $
//    $Author: Bryce $
//
//

#ifndef ACTOR_DEFINES_H
#define ACTOR_DEFINES_H

// Defines to indicate discontinuities. Other bits may be used for specific
// cases.
#define DISCONTINUITY_NONE             0x0
#define DISCONTINUITY_POSITION         0x1
#define DISCONTINUITY_VELOCITY         0x2
#define DISCONTINUITY_ORIENTATION      0x4
#define DISCONTINUITY_ANGULAR_VELOCITY 0x8
#define DISCONTINUITY_STATE            0x1
#define DISCONTINUITY_SPEED_TARGET     0x2
#define DISCONTINUITY_SPEED_CURRENT    0x4
#define DISCONTINUITY_PATH_DISTANCE    0x8

#endif // ACTOR_DEFINES_H
