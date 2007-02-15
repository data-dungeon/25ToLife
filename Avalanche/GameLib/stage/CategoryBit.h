/*
**
**  File:   CategoryBit.h
**  Date:   June 24, 2003
**  By:     Bryant Collard
**  Desc:   A collection of defines for CInteractionCategory constants useful
**          to all games. The constants declared here use the most significant
**          bits. Game specific constants should use the least significant bits.
**          Care should be taken to prevent attempts to use the same bits.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/stage/CategoryBit.h $
**  $Revision: 6 $
**      $Date: 7/28/03 2:40p $
**    $Author: Bcollard $
**
*/

#ifndef CATEGORY_BIT_H
#define CATEGORY_BIT_H

// No category
#define NO_BIT             0x00000000

// Main categories
#define TERRAIN_BIT        0x80000000
#define STATIC_BIT         0x40000000
#define QUERY_BIT          0x20000000
#define ACTOR_BIT          0x10000000

// Actor categories
#define PROCESS_LAST_BIT   0x08000000
#define PLAYER_BIT         0x04000000
#define NO_SUPPORT_BIT     0x02000000
#define ROAMER_BIT         0x01000000

#define PLANT_IGNORES_BIT  0x00800000
#define DEBUG_DRAW_BIT     0x00400000
#define TRANSPARENT_BIT		0x00200000
#define AI_BIT					0x00100000

// This is a camera chord check
#define CAMERAQUERY_BIT		0x00080000

// This is for subsonic projectiles
#define SUBSONIC_BIT			0x00040000

// This is for actors that will NOT move in the world, therefore won't
// dont need to seed collisions
#define FIXED_BIT				0x00020000

#endif // CATEGORY_BIT_H
