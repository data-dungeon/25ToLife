/*
**
**  File:   GameEntityCategory.cpp
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
**      $File: //depot/TTL/code/sim/GameEntityCategory.cpp $
**  $Revision: #9 $
**  $DateTime: 2005/06/08 16:04:21 $
**    $Author: Bryant_Collard $
**
*/

#include "TTLPCH.h"

#define GAME_ENTITY_CATEGORY_CPP
#include "stage/EntityCategory.cpp"

const CInteractionCategory PENETRABLE_CATEGORY(PENETRABLE_BIT);
const CInteractionCategory PARTICIPANT_CATEGORY(PARTICIPANT_BIT);
const CInteractionCategory NO_WEAPON_HIT_CATEGORY(NO_WEAPON_HIT_BIT);
const CInteractionCategory WATER_CATEGORY(WATER_BIT);

const CInteractionCategory AVATAR_IGNORE_CATEGORIES(
		PROCESS_LAST_CATEGORY | NO_SUPPORT_CATEGORY | NO_WEAPON_HIT_CATEGORY);
