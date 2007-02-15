//============================================================================
//=
//= World.h
//=
//============================================================================

#ifndef _WORLD_H
#define _WORLD_H

//============================================================================

#include "Game/Misc/Bounds.h"		// for BoundingSphere, BoundingBox

//============================================================================

class CInstance;
struct ts_PatchSOM;
struct ts_LOD;

//============================================================================

#define ALIGN_EMITTER_TO_ROTATION	0x00000001

//============================================================================
//= for rash initialization purposes, loadSOM returns a pointer to either
//= a model or an LOD record. this needs to trickle down, ultimately, to
//= MakeInstance which hooks it into the instance

struct ts_Geometry
{
	// only one of two will be set, other will be NULL
	ts_PatchSOM*	pSOM;
	ts_LOD*			pLOD;
};

//============================================================================
// for specifying which texture to animate for an instance

struct ts_TextureToAnimate
{
	// TJC - texture bind never gets resolved into a bind, instead the whole thing gets overwritten (see union below)
	s16			s16TextureBind;	// starts off as a texture index, gets resolved to a bind at load time. can be NO_TEXTURE if no texture to animate
	u16			u16TimerInit;		// animation speed, in milliseconds (= 1000 / FPS)
};

//============================================================================
// used for groups' dynamic instance lists

struct ts_InstanceList
{
	ts_InstanceList*	pNext;
	CInstance*			pInst;
};

//============================================================================
// group u32Flags bits

#define GROUP_IN_VIEW0	0x01
#define GROUP_IN_VIEW1	0x02
#define GROUP_IN_VIEW2	0x04
#define GROUP_IN_VIEW3	0x08

//============================================================================

struct ts_Group
{
/***** if this structure changes, update the correct version of the overloaded function DBLUtil__WriteRecord in MultiGen Plug-ins\Export\DBLWrite.cpp ****/	

	ts_BoundingSphere	BoundingSphere;
	ts_BoundingBox		BoundingBox;

	unsigned short		usDispStatInst;
	unsigned short		usCollStatInst;

	CInstance**			ppDispStatArray;	//array of pointers
	ts_InstanceList*	pDispDynList;			//linked list

	CInstance**			ppCollStatArray;	//array of pointers
	ts_InstanceList*	pCollDynList;			//linked list

	u8						u8Flags;

	// pad to 16-byte alignment so BoundingSphere can be 16-byte aligned for using vu0 on ps2
	char Pad[3];	//yuch-- i don't believe this is necessary anymore after the math library integration, but this is all going to change when we go to trees, so no huury

/***** if this structure changes, update the correct version of the overloaded function DBLUtil__WriteRecord in MultiGen Plug-ins\Export\DBLWrite.cpp ****/	
};

//============================================================================

struct ts_Region
{
	unsigned short	nGroups;
	unsigned short	pad;
	ts_Group*		pGroup;
};

//============================================================================
// for expansion table chunk. header is followed by the expansion tables--
// tables of floats of the specified lengths

struct ts_ExpansionTableHeader
{
/***** if this structure changes, update the correct version of the overloaded function DBLUtil__WriteRecord in MultiGen Plug-ins\Export\DBLWrite.cpp ****/	

	u32 u32ExpansionTableXSize;
	u32 u32ExpansionTableYSize;
	u32 u32ExpansionTableZSize;

/***** if this structure changes, update the correct version of the overloaded function DBLUtil__WriteRecord in MultiGen Plug-ins\Export\DBLWrite.cpp ****/	
};

//============================================================================
// named points are simple-- a Vector3 followed by a variable-length name.
// this is exported as a Vector3 followed by string data, so it doesn't need
// to have its own DBLUtil__WriteRecord function

struct ts_NamedPosition
{
	Vector3Packed	vPos;
	char				Name[1];				// at least one character, typically more
};

//============================================================================
// world emitter records are a position, rotation, and emitter name.
// this supports the artists placing emitters in 3dmax

struct ts_WorldEmitter
{
	Vector3Packed	vPos;
	Vector3Packed	vRotationRadians;
	u16				u16Type;							// for game-specific marking
	u16				u16Flags;
	float				fLOD1RadiusOverride;			// near radius (1000.0 for no override)
	float				fLOD2RadiusOverride;			// far radius (1000.0 for no override)
	char				Name[1];							// at least one character, typically more
};

//============================================================================

#endif //_WORLD_H
