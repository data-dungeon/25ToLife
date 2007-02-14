/*
**
**  File:   GameCategoryBit.h
**  Date:   June 24, 2003
**  By:     Bryant Collard
**  Desc:   A collection of defines for CInteractionCategory constants useful
**          to this game. The constants declared here use the least significant
**          bits. General constants should use the most significant bits. Care
**          should be taken to prevent attempts to use the same bits.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/sim/GameCategoryBit.h $
**  $Revision: #7 $
**  $DateTime: 2005/06/08 16:04:21 $
**    $Author: Bryant_Collard $
**
*/

#ifndef GAME_CATEGORY_BIT_H
#define GAME_CATEGORY_BIT_H

#define PENETRABLE_BIT					0x00000001
#define PARTICIPANT_BIT					0x00000002
#define NO_WEAPON_HIT_BIT				0x00000004
#define WATER_BIT							0x00000008

#endif // GAME_CATEGORY_BIT_H
