/*
**
**  File:   EntityCategory.cpp
**  Date:   April 1, 2003
**  By:     Bryant Collard
**  Desc:   A collection of CInteractionCategory constants useful to all games.
**          The constants declared here use the most significant bits. Game
**          specific constants should use the least significant bits. Care
**          should be taken to prevent attempts to use the same bits. If we
**          run out, CInteractionCategory should be modified to expand the
**          width of the field.
**
**          Some of the constants depend on other constants being initialized
**          first. The only way to gaurantee initialization order is to
**          put them all in the same translation unit. Apparently, you cannot
**          even depend on the translation units in libraries being processed
**          before those in the application. So, this file should not be built
**          as part of the stage project but should be included in the game
**          specific category .cpp file. (Yeah I know. What a pain. This does
**          give us virtually unlimited category bits.)
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/stage/EntityCategory.cpp $
**  $Revision: 8 $
**      $Date: 7/28/03 8:19p $
**    $Author: Bcollard $
**
*/

// Not using pre-compiled headers. //

// Hokey attempt to prevent mistakes
#ifdef GAME_ENTITY_CATEGORY_CPP
#undef GAME_ENTITY_CATEGORY_CPP

// My header (always first) //
#include "stage/EntityCategory.h"

// No category
const CInteractionCategory NO_CATEGORY(NO_BIT);

// Main categories
const CInteractionCategory TERRAIN_CATEGORY(TERRAIN_BIT);
const CInteractionCategory STATIC_CATEGORY(STATIC_BIT);
const CInteractionCategory QUERY_CATEGORY(QUERY_BIT);
const CInteractionCategory ACTOR_CATEGORY(ACTOR_BIT);

// Actor categories
const CInteractionCategory PROCESS_LAST_CATEGORY(PROCESS_LAST_BIT);
const CInteractionCategory PLAYER_CATEGORY(PLAYER_BIT);
const CInteractionCategory NO_SUPPORT_CATEGORY(NO_SUPPORT_BIT);
const CInteractionCategory ROAMER_CATEGORY(ROAMER_BIT);
const CInteractionCategory PLANT_IGNORES_CATEGORY(PLANT_IGNORES_BIT);
const CInteractionCategory DEBUG_DRAW_CATEGORY(DEBUG_DRAW_BIT);
const CInteractionCategory SUBSONIC_CATEGORY(SUBSONIC_BIT);

const CInteractionCategory TRANSPARENT_CATEGORY(TRANSPARENT_BIT);
const CInteractionCategory AI_CATEGORY(AI_BIT);

const CInteractionCategory CAMERAQUERY_CATEGORY(CAMERAQUERY_BIT);
const CInteractionCategory FIXED_CATEGORY(FIXED_BIT);

// Computed categories

// All categories
const CInteractionCategory ALL_CATEGORIES(~NO_CATEGORY);

// All of the categories actors may belong to, i.e., everything except the
// TERRAIN and STATIC categories.
const CInteractionCategory ALL_ACTOR_CATEGORIES(~(TERRAIN_CATEGORY |
		STATIC_CATEGORY | QUERY_CATEGORY | DEBUG_DRAW_CATEGORY));

// Terrain and static objects.
const CInteractionCategory WORLD_CATEGORY(TERRAIN_CATEGORY | STATIC_CATEGORY);

#endif GAME_ENTITY_CATEGORY_CPP
