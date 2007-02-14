/*
**
**  File:   GameEntityCategory.h
**  Date:   April 1, 2003
**  By:     Bryant Collard
**  Desc:   A collection of CInteractionCategory constants useful to this game.
**          The constants declared here use the least significant bits. General
**          constants should use the most significant bits. Care should be
**          taken to prevent attempts to use the same bits. If we run out,
**          CInteractionCategory should be modified to expand the width of the
**          field.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/sim/GameEntityCategory.h $
**  $Revision: #9 $
**  $DateTime: 2005/06/08 16:04:21 $
**    $Author: Bryant_Collard $
**
*/

#ifndef GAME_ENTITY_CATEGORY_H
#define GAME_ENTITY_CATEGORY_H

#include "stage/InteractionCategory.h"
#include "sim/GameCategoryBit.h"

extern const CInteractionCategory PENETRABLE_CATEGORY;
extern const CInteractionCategory PARTICIPANT_CATEGORY;
extern const CInteractionCategory NO_WEAPON_HIT_CATEGORY;
extern const CInteractionCategory WATER_CATEGORY;

extern const CInteractionCategory AVATAR_IGNORE_CATEGORIES;

#endif // GAME_ENTITY_CATEGORY_H
