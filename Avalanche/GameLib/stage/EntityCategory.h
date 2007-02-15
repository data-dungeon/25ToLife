/*
**
**  File:   EntityCategory.h
**  Date:   April 1, 2003
**  By:     Bryant Collard
**  Desc:   A collection of CInteractionCategory constants useful to all games.
**          The constants declared here use the most significant bits. Game
**          specific constants should use the least significant bits. Care
**          should be taken to prevent attempts to use the same bits. If we
**          run out, CInteractionCategory should be modified to expand the
**          width of the field.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/stage/EntityCategory.h $
**  $Revision: 7 $
**      $Date: 7/28/03 2:40p $
**    $Author: Bcollard $
**
*/

#ifndef ENTITY_CATEGORY_H
#define ENTITY_CATEGORY_H

#include "stage/InteractionCategory.h"
#include "stage/CategoryBit.h"

// No category
extern const CInteractionCategory NO_CATEGORY;

// Main categories
extern const CInteractionCategory TERRAIN_CATEGORY;
extern const CInteractionCategory STATIC_CATEGORY;
extern const CInteractionCategory QUERY_CATEGORY;
extern const CInteractionCategory ACTOR_CATEGORY;

// Actor categories
extern const CInteractionCategory PROCESS_LAST_CATEGORY;
extern const CInteractionCategory PLAYER_CATEGORY;
extern const CInteractionCategory NO_SUPPORT_CATEGORY;
extern const CInteractionCategory ROAMER_CATEGORY;
extern const CInteractionCategory PLANT_IGNORES_CATEGORY;
extern const CInteractionCategory DEBUG_DRAW_CATEGORY;
extern const CInteractionCategory SUBSONIC_CATEGORY;

extern const CInteractionCategory TRANSPARENT_CATEGORY;	//glass, things you can see through, but collide
extern const CInteractionCategory AI_CATEGORY;  //these are artificial creatures

extern const CInteractionCategory CAMERAQUERY_CATEGORY;
extern const CInteractionCategory FIXED_CATEGORY;

// Computed categories

// All categories
extern const CInteractionCategory ALL_CATEGORIES;

// All of the categories actors may belong to, i.e., everything except the
// TERRAIN, STATIC and QUERY categories.
extern const CInteractionCategory ALL_ACTOR_CATEGORIES;

// Terrain and static objects.
extern const CInteractionCategory WORLD_CATEGORY;

// Entities that are excluded by default
//extern const CInteractionCategory DEFAULT_EXCLUDED_CATEGORIES;

#endif // ENTITY_CATEGORY_H
