/*
**
**  File:   GameEntityCategory.cpp
**  Date:   April 1, 2003
**  By:     Bryant Collard
**  Desc:   A hierarchal bitfield from which the class of an entity can be
**          deduced as well as its base classes. Essentially RTTI for
**          stage entities.
**
**          This file contains game specific type information. It includes
**          the general one to ensure correct initialization order.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

#include "TTLPCH.h"
#include "components/Plant.h"
#include "environ/glass.h"

enum EGameProtagonistDerived
{
	GAME_PROTAGONIST_GLASS,
	GAME_PROTAGONIST_PLANT,
	NUM_GAME_PROTAGONIST_DERIVED
};
#define GAME_PROTAGONIST_DERIVED  NUM_GAME_PROTAGONIST_DERIVED

enum EGameCompActorDerived
{
	GAME_COMPACTOR_COMBATANT,
	NUM_GAME_COMPACTOR_DERIVED
};
#define GAME_COMPACTOR_DERIVED NUM_GAME_COMPACTOR_DERIVED

#include "stage/EntityType.cpp"

// Derived from CompActor
const CEntityType CCombatant::s_combatantClass(CCompActor::ClassType(),
		GAME_COMPACTOR_COMBATANT + NUM_COMPACTOR_DERIVED, 0);

// Derived from CProtagonist.
const CEntityType CGlass::s_glassClass(CProtagonist::ClassType(),
		GAME_PROTAGONIST_GLASS + NUM_PROTAGONIST_DERIVED, 0);
const CEntityType CPlant::s_plantClass(CProtagonist::ClassType(),
		GAME_PROTAGONIST_GLASS + NUM_PROTAGONIST_DERIVED, 0);
