/*
**
**  File:   EntityType.cpp
**  Date:   April 1, 2003
**  By:     Bryant Collard
**  Desc:   A hierarchal bitfield from which the class of an entity can be
**          deduced as well as its base classes. Essentially RTTI for
**          stage entities.
**
**          Initializers for the static members contianing the type of
**          descendants of CStageEntity are here to ensure correct
**          initialization order. Initializers for game specific classes
**          can be contianed in game specific code since this file will
**          be processed first.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/stage/EntityType.cpp $
**  $Revision: 10 $
**      $Date: 6/04/03 9:50a $
**    $Author: Bcollard $
**
*/

// Not using pre-compiled headers. //

// My header (always first) //
#include "stage/EntityType.h"

// Stage entity header //
#include "stage/StageEntity.h"

// CStageEntity derived headers //
#include "collide/TerrainEntity.h"
#include "collide/StaticEntity.h"
#include "collide/QueryEntity.h"
#include "stage/cactor.h"

// CQueryEntity derived headers //
#include "CollideHelper/LineEntity.h"
#include "CollideHelper/XZEntity.h"
#include "CollideHelper/AabbEntity.h"
#include "CollideHelper/ObbEntity.h"
#include "CollideHelper/ObbChildEntity.h"
#include "CollideHelper/SphereEntity.h"

// CActor derived headers //
#include "stage/protagonist.h"
#include "stage/CCompActor.h"

// Establish the number of classes derived from each class.

// Ultimate base class.
enum EStageEntityDerived
{
	STAGE_ENTITY_BASE,
	STAGE_ENTITY_TERRAIN,
	STAGE_ENTITY_STATIC,
	STAGE_ENTITY_QUERY,
	STAGE_ENTITY_ACTOR,
	NUM_STAGE_ENTITY_DERIVED
};
#ifndef GAME_STAGE_ENTITY_DERIVED
#define GAME_STAGE_ENTITY_DERIVED   0
#endif

// Derived from CStageEntity.
#define NUM_TERRAIN_DERIVED         0
#ifndef GAME_TERRAIN_DERIVED
#define GAME_TERRAIN_DERIVED        0
#endif

#define NUM_STATIC_DERIVED          0
#ifndef GAME_STATIC_DERIVED
#define GAME_STATIC_DERIVED         0
#endif

enum EQueryEntityDerived
{
	QUERY_ENTITY_BASE,
	QUERY_ENTITY_LINE,
	QUERY_ENTITY_XZ,
	QUERY_ENTITY_AABB,
	QUERY_ENTITY_OBB,
	QUERY_ENTITY_OBB_CHILD_LIST,
	QUERY_ENTITY_SPHERE,
	NUM_QUERY_ENTITY_DERIVED
};
#ifndef GAME_QUERY_DERIVED
#define GAME_QUERY_DERIVED          0
#endif

enum EActorDerived
{
	ACTOR_BASE,
	ACTOR_PROTAGONIST,
	NUM_ACTOR_DERIVED
};
#ifndef GAME_ACTOR_DERIVED
#define GAME_ACTOR_DERIVED          0
#endif

// Derived from CQueryEntity.
#define NUM_LINE_DERIVED            0
#ifndef GAME_LINE_DERIVED
#define GAME_LINE_DERIVED           0
#endif

#define NUM_XZ_DERIVED              0
#ifndef GAME_XZ_DERIVED
#define GAME_XZ_DERIVED             0
#endif

#define NUM_AABB_DERIVED            0
#ifndef GAME_AABB_DERIVED
#define GAME_AABB_DERIVED           0
#endif

#define NUM_OBB_DERIVED             0
#ifndef GAME_OBB_DERIVED
#define GAME_OBB_DERIVED            0
#endif

#define NUM_OBB_CHILD_LIST_DERIVED  0
#ifndef GAME_OBB_CHILD_LIST_DERIVED
#define GAME_OBB_CHILD_LIST_DERIVED 0
#endif

#define NUM_SPHERE_DERIVED          0
#ifndef GAME_SPHERE_DERIVED
#define GAME_SPHERE_DERIVED         0
#endif

// Derived from CActor
enum EProtagonistDerived
{
	PROTAGONIST_BASE,
	PROTAGONIST_COMPACTOR,
	NUM_PROTAGONIST_DERIVED
};
#ifndef GAME_PROTAGONIST_DERIVED
#define GAME_PROTAGONIST_DERIVED    0
#endif

enum ECompActorDerived
{
	COMPACTOR_BASE,
	NUM_COMPACTOR_DERIVED
};
#ifndef GAME_COMPACTOR_DERIVED
#define GAME_COMPACTOR_DERIVED		0
#endif

// Initialize CEntityType
const uint32 CEntityType::NOT_DERIVED = ~0;

// Initialize the class types for each kind of stage entity. Make sure each
// entry appears below its base class.

// Ultimate base class.
const CEntityType CStageEntity::s_stageEntityClass(
		NUM_STAGE_ENTITY_DERIVED + GAME_STAGE_ENTITY_DERIVED);

// Derived from CStageEntity.
const CEntityType CTerrainEntity::s_terrainEntityClass(
		CStageEntity::ClassType(), STAGE_ENTITY_TERRAIN,
		NUM_TERRAIN_DERIVED + GAME_TERRAIN_DERIVED);
const CEntityType CStaticEntity::s_staticEntityClass(
		CStageEntity::ClassType(), STAGE_ENTITY_STATIC,
		NUM_STATIC_DERIVED + GAME_STATIC_DERIVED);
const CEntityType CQueryEntity::s_queryEntityClass(
		CStageEntity::ClassType(), STAGE_ENTITY_QUERY,
		NUM_QUERY_ENTITY_DERIVED + GAME_QUERY_DERIVED);
const CEntityType CActor::s_actorClass(
		CStageEntity::ClassType(), STAGE_ENTITY_ACTOR,
		NUM_ACTOR_DERIVED + GAME_ACTOR_DERIVED);

// Derived from CQueryEntity.
const CEntityType CLineEntity::s_lineEntityClass(
		CQueryEntity::ClassType(), QUERY_ENTITY_LINE,
		NUM_LINE_DERIVED + GAME_LINE_DERIVED);
const CEntityType CXZEntity::s_xzEntityClass(
		CQueryEntity::ClassType(), QUERY_ENTITY_XZ,
		NUM_XZ_DERIVED + GAME_XZ_DERIVED);
const CEntityType CAabbEntity::s_aabbEntityClass(
		CQueryEntity::ClassType(), QUERY_ENTITY_AABB,
		NUM_AABB_DERIVED + GAME_AABB_DERIVED);
const CEntityType CObbEntity::s_obbEntityClass(
		CQueryEntity::ClassType(), QUERY_ENTITY_OBB,
		NUM_OBB_DERIVED + GAME_OBB_DERIVED);
const CEntityType OBBChildListEntity::s_obbChildListClass(
		CQueryEntity::ClassType(), QUERY_ENTITY_OBB_CHILD_LIST,
		NUM_OBB_CHILD_LIST_DERIVED + GAME_OBB_CHILD_LIST_DERIVED);
const CEntityType CSphereEntity::s_sphereEntityClass(
		CQueryEntity::ClassType(), QUERY_ENTITY_SPHERE,
		NUM_SPHERE_DERIVED + GAME_SPHERE_DERIVED);

// Derived from CActor.
const CEntityType CProtagonist::s_protagonistClass(
		CActor::ClassType(), ACTOR_PROTAGONIST,
		NUM_PROTAGONIST_DERIVED + GAME_PROTAGONIST_DERIVED);

// Derived from CProtagonist
const CEntityType CCompActor::s_compActorClass(
	CProtagonist::ClassType(), PROTAGONIST_COMPACTOR,
	NUM_COMPACTOR_DERIVED + GAME_COMPACTOR_DERIVED );
