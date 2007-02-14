/*
**
**  File:   GameElements.h
**  Date:   November 9, 2001
**  By:     David Ross
**  Desc:   Enumeration of game element types: correlates game code names with tables\avatype.tdb gameElement numerics
**
**  Copyright (c) 2001, Avalanche Software Inc.
**  All Rights Reserved.
**
**
*/

#ifndef GAME_ELEMENTS_H
#define GAME_ELEMENTS_H

#define GAME_ELEMENT_NONE			0	//surface is not a game element
#define GAME_ELEMENT_BOUNCE		1	//canopy-trampolines
#define GAME_ELEMENT_LADDER		2	//ladders
#define GAME_ELEMENT_FINISH_AREA	3	//indicates actor reached the finishing area 
#define GAME_ELEMENT_CONVEYOR		4	//conveyor belt
#define GAME_ELEMENT_ARENA_FLOOR	5  //Floor of the Meanie Genie Arena
#define GAME_ELEMENT_LEDGE_CLIMB	6  //surface of any climbable wall
#define GAME_ELEMENT_SLIPPERY		7  //default surface of slippery floor
#define GAME_ELEMENT_WATER			8  //water surface
#define GAME_ELEMENT_LAVA			9  //lava surface

#define RESPONSE_TYPE_HARD			0
#define RESPONSE_TYPE_SOFT			1

//these indices are set for each surface type in avasurf.tbd
#define SURFACE_EFFECT_DEFAULT			0
#define SURFACE_EFFECT_ICE					1     
#define SURFACE_EFFECT_WATER				2
#define SURFACE_EFFECT_SAND				3    
#define SURFACE_EFFECT_STONE				4   
#define SURFACE_EFFECT_DIRT				5
#define SURFACE_EFFECT_HOT					6     
#define SURFACE_EFFECT_GRASS				7   
#define SURFACE_EFFECT_SLIPPERY			8
#define SURFACE_EFFECT_METAL				9   
#define SURFACE_EFFECT_WOOD				10	    
#define SURFACE_EFFECT_SNOW				11
#define SURFACE_EFFECT_MUD					12
#define SURFACE_EFFECT_LEAFY				13
#define SURFACE_EFFECT_SLIPPERY_TIPPY	14
#define SURFACE_EFFECT_BRIDGE				15
#define SURFACE_EFFECT_MUSHROOM			16
#define SURFACE_EFFECT_FEATHER			17
#define SURFACE_EFFECT_WETBEACH			18

#define SURFACE_EFFECT_COUNT				19

#endif //  GAME_ELEMENTS_H
