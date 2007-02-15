/*************************************/
/*                                   */
/*   DistortMgr.c                    */
/*   big juju prototype  05/24/99    */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   spacial distortion handling     */
/*                                   */
/*************************************/

/* spacial distortions are things that affect vertex and/or normal locations/values. this
includes wind, ripples, waves, distortion fields, black holes, etc. in general, a global
list of distortions is kept. for each grid section, that list is culled to the distortions
that affect that grid (through simple bounds checking and compatibility checking, e.g. a
ripple distortion might cross a grid section, but if there is no water in the grid, no need
to include the ripple distortion). then for each instance the list is culled further. any
distortions that affect the current instance are processed at the time the instance's
vertices are transformed. distortions must be able to be transformed into camera space, since
an instance's vertices are transformed directly from local to camera coordinates */

/******************* includes ****************************/

#include "Game/GamePCH.h"



#include "EngineHelper/timer.h"



/******************* defines *****************************/

/* point force defines (for ts_PointForce.u8Flags) */

#define POINT_FORCE_OMNIDIRECTIONAL			0x01
#define POINT_FORCE_HAS_VELOCITY				0x02
#define POINT_FORCE_HAS_ACCELERATION		0x04

/* global wind defines (for ts_GlobalWind.u8Flags) */

#define GLOBAL_WIND_HAS_SINE_WAVE			0x01
#define GLOBAL_WIND_HAS_DAMPING				0x02	// damping is active
#define GLOBAL_WIND_DELAY_DAMPING			0x04	// use damping only after lifetime ends

/* vortex wind defines (for ts_VortexWind.u8Flags) */

#define VORTEX_WIND_HAS_SINE_WAVE			0x01
#define VORTEX_WIND_HAS_DAMPING				0x02	// damping is active
#define VORTEX_WIND_DELAY_DAMPING			0x04	// use damping only after lifetime ends

/* wind gust force defines (for ts_WindGust.u8Flags) */

#define WIND_GUST_HAS_VELOCITY				0x01
#define WIND_GUST_HAS_ACCELERATION			0x02
#define WIND_GUST_KILL_ME						0x04
// JMH - more windgust flags defined in .h file for setting externally

/* ripple defines (for ts_Ripple.u8Flags) */

#define RIPPLE_ORIENT_XY						0x01	// origin in x/y plane, amplitude in z
#define RIPPLE_ORIENT_XZ						0x02	// origin in x/z plane, amplitude in y
#define RIPPLE_ORIENT_YZ						0x04	// origin in y/z plane, amplitude in x
#define RIPPLE_HAS_DAMPING						0x08	// damping is active
#define RIPPLE_DELAY_DAMPING					0x10	// use damping only after lifetime ends
#define RIPPLE_NEGATIVE_AMPLITUDE			0x20	// original amplitude was negative

/* point multiplier defines (for ts_PointMultiplier.u8Flags) */

#define POINT_MULTIPLIER_HAS_VELOCITY		0x01
#define POINT_MULTIPLIER_HAS_ACCELERATION	0x02

/* point transform defines (for ts_PointRotate.u8Flags) */

#define POINT_ROTATE_HAS_VELOCITY			0x01
#define POINT_ROTATE_HAS_ACCELERATION		0x02

/* reserved group id's-- group id's in this range can't be used unless they are requested through */
/* DistortionMgr__RequestGroupID */

#define DISTORTION_RESERVED_GROUP_MIN		0x9000
#define DISTORTION_RESERVED_GROUP_MAX		0xa000

/******************* macros ******************************/

#define DISTORTION_GET_RECORD(Handle)		&DistortionMgr.pDistortionAlloc[HANDLE_INDEX(Handle)]

/******************* structures **************************/

/* point force structure (omni-directional force bubble or black hole, or may be directional) */

struct ts_PointForce
{
	Vector3PackedData	vPos;							// current position, usually in world space
	Vector3PackedData	vTransformedPos;			// transformed position, usually in camera space, but stay in world space for particles, for example
	Vector3PackedData	vVelocity;					// point's velocity
	Vector3PackedData	vAcceleration;				// point's acceleration
	Vector3PackedData vStrength;					// directional strength, maybe omnidirectional
	Vector3PackedData vTransformedStrength;		// transformed directional strength
	float Lifetime;						// remaining lifetime, 0.0 means forever
	u8 u8Flags;
};

/* global wind structure (effective everywhere). factors are combined at advance time
to come up with vStrength field */

struct ts_GlobalWind
{
	Vector3PackedData vStrength;					// directional strength
	Vector3PackedData vTransformedStrength;		// transformed directional strength
	Vector3PackedData vBaseStrength;				// original strength
	float Randomness;						// 0.0 means no randomness, positive means stronger, negative means weaker
	float SinAngle;						// for sine-wave strength cycling
	float SinAngleInc;					// for sine-wave strength cycling
	float SinMinMaxCenter;				// for sine-wave strength cycling. minimum sine multiplier
	float SinMinMaxHalfDelta;			// for sine-wave strength cycling. maximum sine multiplier
	float DampingMult;
	float Damping;
	float Lifetime;						// remaining lifetime, 0.0 means forever
	u8 u8Flags;
};

/* vortex wind structure (cylindrical tornado) */
//yuch-- somehow combine with ripple?
struct ts_VortexWind
{
	Vector3PackedData	vPos;					// current position, usually in world space
	Vector3PackedData	vTransformedPos;	// transformed position, usually in camera space, but stay in world space for particles, for example
	float Radius;					// wall's current distance from origin
	float Range;					// effective range in front and behind crest
	float RadiusVelocity;		// velocity of radius expansion
	float RadiusAcceleration;	// useful for friction/energy dissipation
	float Amplitude;				// wind strength at radius
	float Damping;					// amplitude dampens over time, could possibly grow. negative value always dampens
	float Lifetime;				//	remaining lifetime, 0.0 means forever
	u8 u8Flags;
};

/* wind gust-- a sphere (typically moving) that applies vTransformedStrength at center and 0 at radius */

struct ts_WindGust
{
	Vector3PackedData vPos;							// current position, usually in world space
	Vector3PackedData vVelocity;					// point's velocity
	Vector3PackedData vAcceleration;				// point's acceleration (typically 0 for a wind gust moving through world)
	Vector3PackedData vStrength;					// directional strength (typically in same direction as velocity, but not necessarily)
	Vector3PackedData vCurrStrength;				// directional strength (typically in same direction as velocity, but not necessarily)
	Vector3PackedData vTransformedPos;			// transformed position, usually in camera space, but stay in world space for particles, for example
	Vector3PackedData vTransformedStrength;		// transformed directional strength
	float fRadius;							// sphere's radius
	float fRadiusSquared;				// sphere's radius squared for quick distance check
	float Delay;							// if cyclic, how long to remain dormant
	float CurrDelay;				// remaining delay time
	float Lifetime;						// lifetime, 0.0 means forever
	float CurrLifetime;					// remaining lifetime, 0.0 means forever
	float Deathtime;						// remaining time before destruction of emitter, 0.0 means forever
	u8 u8Flags;
	int index;
};

/* updraft-- an upright cylinder of wind (can be used for downdraft too) */

struct ts_Updraft
{
	Vector3PackedData vPos;							// current position (bottom center of cylinder), in world space
	float fYVelocity;						// strength
	float fHeight;							// height of cylinder
	float fRadius;							// radius of cylinder
	float fRadiusSquared;				// radius squared for quick distance check
	float Lifetime;						// remaining lifetime, 0.0 means forever
	u8 u8Flags;
};

/* particle forcefield sphere-- for making particles go around things */

struct ts_Forcefield
{
	Vector3 *pvPos;				// pointer to current position, in world space (can be tied to an actor's position if desired)
	Vector3PackedData	vPos;					// position if repulsor is stationary
	Vector3PackedData	vTransformedPos;	// transformed position, usually in camera space, but stay in world space for particles, for example
	float		fRadius;				// sphere's radius
	float		fRadiusSquared;	// sphere's radius squared for quick distance check
	float		fLifetime;			//	remaining lifetime, 0.0 means forever
	u8			u8Flags;
};

/* ripple structure */

struct ts_Ripple
{
	Vector3PackedData	vPos;					// current position, usually in world space
	Vector3PackedData	vTransformedPos;	// transformed position, usually in camera space, but stay in world space for particles, for example
	float Radius;					// crest's current distance from origin
	float Range;					// effective range in front and behind crest
	float RadiusVelocity;		// velocity of radius expansion
	float RadiusAcceleration;	// useful for friction/energy dissipation
	float Amplitude;				// "height" of ripple, can be negative for depression
	float Damping;					// amplitude dampens over time, could possibly grow. negative value always dampens
	float Lifetime;				// remaining lifetime, 0.0 means forever
	u8 u8Flags;
};

/* point multiplier structure, for normal distortion */

struct ts_PointMultiplier
{
	Vector3PackedData	vPos;					// current position, usually in world space
	Vector3PackedData	vTransformedPos;	// transformed position, usually in camera space, but stay in world space for particles, for example
	Vector3PackedData	vVelocity;			// point's velocity
	Vector3PackedData	vAcceleration;		// point's acceleration
	float MultiplierAtCenter;	// strength at center point
	float MultiplierDelta;		// strength at radius (typically 1.0 for smooth look) - strength at center
	float Radius;					// multiplier is 1.0 at radius
	float RadiusSquared;
	float Lifetime;				// remaining lifetime, 0.0 means forever
	u8 u8Flags;
};

/* point rotate structure, for normal distortion */

struct ts_PointRotate
{
	Vector3PackedData	vPos;					// current position, usually in world space
	Vector3PackedData	vTransformedPos;	// transformed position, usually in camera space, but stay in world space for particles, for example
	Vector3PackedData	vVelocity;			// point's velocity
	Vector3PackedData	vAcceleration;		// point's acceleration
	Vector3PackedData	vRotate;				// rotation for transform
	float Radius;					// multiplier is 1.0 at radius
	float RadiusSquared;
	float Lifetime;				// remaining lifetime, 0.0 means forever
	ts_PoolMatrix *pMatrix;					// allocated transform matrix
	ts_PoolMatrix *pTransformedMatrix;	// allocated transform matrix
	u8 u8Flags;
};

/* general distortion stucture */

struct ts_Distortion
{
	ts_SubFlagsDataHeader	dh;					// data header-- MUST BE FIRST-- see comments in datatypes.h
	t_Handle						Handle;				//	assigned handle for access, invalid handle checking
	u16							u16Group;			// group id, for easy killing of a group of distortions

	union
	{
		ts_PointForce			PointForce;			// for vertices
		ts_GlobalWind			GlobalWind;			// for particles, chains
		ts_VortexWind			VortexWind;			// for particles, chains
		ts_WindGust				WindGust;			// for particles, chains
		ts_Updraft				Updraft;				// for chains (updrafts typically have some visual particle effect representation already)
		ts_Forcefield			Forcefield;			// for particles, chains
		ts_Ripple				Ripple;				// for vertices
		ts_PointMultiplier	PointMultiplier;	// for normals-- may only work in opengl? works by changing normal's length for lighting calculations
		ts_PointRotate			PointRotate;		// for normals
	};
	ts_Distortion				*pNext;				// linked list pointer-- does not have to be first
};

/* distortion manager */

struct ts_DistortionMgr
{
	ts_SLinkList	DistortionFree;		// linked list of free records
	ts_SLinkList	DistortionActive;		// linked list of active records
	ts_Distortion	*pDistortionAlloc;	// pointer to malloc'd records
	u16				u16ID;					// for assigning handles
	int				nMaxDistortions;		// record passed value for debugging, not really necessary otherwise

	/* reserved group id's */

	bool				bGroupIDWrapped;
	u16				u16GroupID;

	/* for advancing/application to instances */

	ts_Distortion	**ppAdvanceList;
	int				nAdvanceListSize;

	/* handy to track whether there is a global wind distortion */

	ts_GlobalWind	*pGlobalWind;
};

/******************* externs *****************************/

/******************* global variables ********************/

/* table of distortion names for parsing .xml file that defines location of world distortions */

#ifndef CONSUMER_BUILD
char *DistortionNames[MAX_DISTORTION_TYPES] =
{
	"POINT_FORCE",			// DISTORTION_POINT_FORCE
	"GLOBAL_WIND",			// DISTORTION_GLOBAL_WIND
	"VORTEX_WIND",			// DISTORTION_VORTEX_WIND
	"WIND_GUST",			// DISTORTION_WIND_GUST
	"FORCEFIELD",			// DISTORTION_FORCEFIELD
	"RIPPLE",				// DISTORTION_RIPPLE
	"POINT_MULTIPLIER",	// DISTORTION_POINT_MULTIPLIER
	"POINT_ROTATE",		// DISTORTION_POINT_ROTATE
};
#endif //CONSUMER_BUILD

/******************* local variables *********************/

/* distortion manager */

static ts_DistortionMgr DistortionMgr;
bool bShowDistortion = false;

/******************* local prototypes ********************/

ts_Distortion *DistortionMgr__AllocDistortion(u16 u16Group);
bool DistortionMgr__GroupUsed(u16 u16Group);
void DistortionMgr__SetGlobalWindStrength(ts_GlobalWind *pGlobalWind);
void Distortion__Kill(ts_Distortion *pDistortion);

/******************* functions ***************************/

/* initialize distortion system */

int DistortionMgr__Init(int nMaxDistortions)
{
	int i;
	ts_Distortion *pDistortion;

	/* check some stuff */

	ASSERT(MAX_DISTORTION_TYPES <= 256);	// sub type field is an unsigned char

	/* clear the manager */

	memset(&DistortionMgr, 0, sizeof(ts_DistortionMgr));

	/* reserved group id's */

	DistortionMgr.u16GroupID = DISTORTION_RESERVED_GROUP_MIN;
	DistortionMgr.bGroupIDWrapped = FALSE;

	/* done if no distortions */

	if (nMaxDistortions == 0)
		return(TRUE);
	DistortionMgr.nMaxDistortions = nMaxDistortions;

	/* allocate records, initialize linked lists */

	MEM_SET_ALLOC_NAME("DistMgr list");
	DistortionMgr.pDistortionAlloc = (ts_Distortion *) memAlloc(DistortionMgr.nMaxDistortions * sizeof(ts_Distortion));
	SLinkList__Init(&DistortionMgr.DistortionFree, offsetof(ts_Distortion, pNext));
	SLinkList__LinkBlock(&DistortionMgr.DistortionFree, DistortionMgr.pDistortionAlloc, DistortionMgr.nMaxDistortions, sizeof(ts_Distortion));
	SLinkList__Init(&DistortionMgr.DistortionActive, offsetof(ts_Distortion, pNext));
	
	/* allocate advance list */

	MEM_SET_ALLOC_NAME("DistMgr advance");
	DistortionMgr.ppAdvanceList = (ts_Distortion **) memAlloc(DistortionMgr.nMaxDistortions * sizeof(ts_Distortion *));

	/* initialize records */

	pDistortion = DistortionMgr.pDistortionAlloc;
	for (i = 0; i < DistortionMgr.nMaxDistortions; i++, pDistortion++)
	{
		pDistortion->dh.u8DataType = DATA_DISTORTION;
		pDistortion->dh.u8SubDataType = 0;
		pDistortion->dh.u16Flags = 0;
		pDistortion->Handle = INVALID_HANDLE;
	}

	/* success */

	return(TRUE);
}

/* reset */

void DistortionMgr__Reset(void)
{
	/* kill all distortions */

	DistortionMgr__KillAllDistortions();

	/* no global wind */

	DistortionMgr.pGlobalWind = NULL;

	/* reserved group id's */

	DistortionMgr.u16GroupID = DISTORTION_RESERVED_GROUP_MIN;
	DistortionMgr.bGroupIDWrapped = FALSE;

	/* all distortions should be in free list now, ready to go */

	ASSERT(SLinkList__Check(&DistortionMgr.DistortionFree) == (signed) DistortionMgr.nMaxDistortions);

	/* clear all handles */

	ts_Distortion *pDistortion = (ts_Distortion *) DistortionMgr.DistortionFree.pHead;
	while (pDistortion)
	{
		pDistortion->Handle = INVALID_HANDLE;
		pDistortion = pDistortion->pNext;
	}
}

/* shutdown */

void DistortionMgr__Shutdown(void)
{
	/* kill all distortions */

	DistortionMgr__KillAllDistortions();

	/* shut down */

	if (DistortionMgr.pDistortionAlloc)
		memFree(DistortionMgr.pDistortionAlloc);
	if (DistortionMgr.ppAdvanceList)
		memFree(DistortionMgr.ppAdvanceList);
	memset(&DistortionMgr, 0, sizeof(ts_DistortionMgr));
}

ts_Distortion *DistortionMgr__AllocDistortion(u16 u16Group)
{
	/* check group id. if id is in reserved range, then it should have been gotten from DistortionMgr__RequestGroupID. */
	/* if not, then this is probably the application using a #define'd id. this breaks the reserved system, so don't */
	/* allow it. typically, the u16GroupID will increment very slowly, so the chances of randomly picking a group id */
	/* between DISTORTION_RESERVED_GROUP_MIN and u16GroupID are very small, so this check should be sufficient */

#ifdef _DEBUG
	if ((u16Group >= DISTORTION_RESERVED_GROUP_MIN) &&
		 (u16Group < DISTORTION_RESERVED_GROUP_MAX) &&
		 (DistortionMgr.bGroupIDWrapped == FALSE) &&	// if it has wrapped, all values between min and max are valid
		 (u16Group >= DistortionMgr.u16GroupID))
	{
		ASSERT(FALSE);	// don't use reserved group id's without going through DistortionMgr__RequestGroupID. see comment above
		return(NULL);
	}
#endif //_DEBUG

	ts_Distortion *pDistortion = (ts_Distortion *) SLinkList__GetAndRemoveHead(&DistortionMgr.DistortionFree);
	if (pDistortion == NULL)
		return(NULL);
	ASSERT(pDistortion->dh.u8DataType == DATA_DISTORTION);

	/* build handle */

	ASSERT(pDistortion->Handle == INVALID_HANDLE);					// record should not have a handle assigned
	HANDLE_INDEX(pDistortion->Handle) = pDistortion - DistortionMgr.pDistortionAlloc;	// index into allocated array of records
	HANDLE_ID(pDistortion->Handle) = DistortionMgr.u16ID++;
	if (pDistortion->Handle == INVALID_HANDLE)
		HANDLE_ID(pDistortion->Handle) = DistortionMgr.u16ID++;	// do it again to avoid INVALID_HANDLE
	ASSERT(pDistortion->Handle != INVALID_HANDLE);					// record should have a valid handle

	/* add to head of active list */

	SLinkList__AddLinkToHead(&DistortionMgr.DistortionActive, pDistortion);

	/* clear record and done */

	pDistortion->dh.u16Flags = DISTORTION_ENABLED;	// enabled by default
	pDistortion->u16Group = u16Group;					// record group
	return(pDistortion);
}

/* count up distortions of a particular type. type == -1 gets total distortion count */

#ifndef CONSUMER_BUILD	// not efficient enough for general use
int DistortionMgr__GetDistortionCountByType(int nType)
{
	ts_Distortion *pDistortion = (ts_Distortion *) DistortionMgr.DistortionActive.pHead;
	int nCount = 0;
	while (pDistortion)
	{
		if ((nType == ALL_DISTORTION_TYPES) || (pDistortion->dh.u8SubDataType == (unsigned) nType))
			nCount++;
		pDistortion = pDistortion->pNext;
	}
	return(nCount);
}
#endif //CONSUMER_BUILD

/* kill all distortions */

void DistortionMgr__KillAllDistortions(void)
{
	/* loop through active distortions */

	while (DistortionMgr.DistortionActive.pHead)
	{
		/* kill it */

		Distortion__Kill((ts_Distortion *) DistortionMgr.DistortionActive.pHead);
	}

	/* all distortions should be in free list now, ready to go */

	ASSERT(SLinkList__Check(&DistortionMgr.DistortionFree) == (signed) DistortionMgr.nMaxDistortions);
}

/* kill distortions of passed type */

void DistortionMgr__KillDistortionsByType(int nType)
{
	ts_Distortion *pNextDistortion = (ts_Distortion *) DistortionMgr.DistortionActive.pHead;
	while (pNextDistortion)
	{
		/* pre-load next distortion */

		ts_Distortion *pDistortion = pNextDistortion;
		pNextDistortion = pNextDistortion->pNext;

		/* maybe kill this one */

		if ((nType == ALL_DISTORTION_TYPES) || (pDistortion->dh.u8SubDataType == (unsigned) nType))
			Distortion__Kill(pDistortion);
	}
}

/* removes all distortions in group */

void DistortionMgr__KillGroup(u16 u16Group)
{
	/* loop through active distortions */

	ts_Distortion *pNextDistortion = (ts_Distortion *) DistortionMgr.DistortionActive.pHead;
	while (pNextDistortion)
	{
		/* record next distortion before possibly removing distortion from active list */

		ts_Distortion *pDistortion = pNextDistortion;
		pNextDistortion = pDistortion->pNext;

		/* remove if it belongs to group */

		if (pDistortion->u16Group == u16Group)
			Distortion__Kill(pDistortion);
	}
}

/* return an unused reserved group id in the distortion set */

u16 DistortionMgr__RequestReservedGroupID(void)
{
	if (DistortionMgr.u16GroupID >= DISTORTION_RESERVED_GROUP_MAX)
	{
		DistortionMgr.bGroupIDWrapped = TRUE;
		DistortionMgr.u16GroupID = DISTORTION_RESERVED_GROUP_MIN;
	}
	while (DistortionMgr__GroupUsed(DistortionMgr.u16GroupID))
		DistortionMgr.u16GroupID++;
	return(DistortionMgr.u16GroupID++);
}

/* check if passed group id is used by any distortion */

bool DistortionMgr__GroupUsed(u16 u16Group)
{
	/* look for it among active distortions */

	ts_Distortion *pDistortion = (ts_Distortion *) DistortionMgr.DistortionActive.pHead;
	while (pDistortion)
	{
		if (pDistortion->u16Group == u16Group)
			return(TRUE);
		pDistortion = pDistortion->pNext;
	}
	return(FALSE);
}

/* apply distortions to passed point. pvPos points to a location vector. pvDest points to
the vector to distort (which might be a normal, not a location) */

void DistortionMgr__ApplyDistortions(Vector3 *pvPos, Vector3 *pvDest, int nCompatibilityFlags)
{
	Vector3 vPos;
	Vector3 vDelta;
	float Magnitude;
	float Multiplier;
	float Range;
	ts_Distortion *pDistortion;
	int i;

	ASSERT((nCompatibilityFlags & ~DISTORTION_COMPATIBILITY_FLAGS) == 0);	// no flags other than compatibility flags

	/* get time slice */

	float fFrameSeconds = g_timer.GetFrameSec();

	/* record original position for stable results, since more than one distortion may */
	/* effect it */
	
	vPos = *pvPos;

	/* go through the active distortions */

	for (i = 0; i < DistortionMgr.nAdvanceListSize; i++)
	{
		/* get distortion, check compatibility */

		pDistortion = DistortionMgr.ppAdvanceList[i];
		if ((pDistortion->dh.u16Flags & nCompatibilityFlags) == 0)
			continue;

		/* can't use camera-space distortions with particles, chains, or meshes */

		ASSERT(((pDistortion->dh.u16Flags & DISTORTION_NO_TRANSFORM) == 0) || ((nCompatibilityFlags & DISTORTION_CANNOT_TRANSFORM) == 0));

		/* handle distortion */

		switch(pDistortion->dh.u8SubDataType)
		{
		/* point forces are things like force bubbles and black holes, though they can */
		/* also be directional */

		case DISTORTION_POINT_FORCE:
			/* distance from passed point to distortion point */

			vDelta = vPos - Vector3(pDistortion->PointForce.vTransformedPos);
			Magnitude = vDelta.Length();

			if (pDistortion->PointForce.u8Flags & POINT_FORCE_OMNIDIRECTIONAL)
			{
				/* vDelta holds vector from distortion point to passed point. If distortion point is */
				/* a black hole, don't want it to draw points past itself, so limit effect to -0.9 */
				/* (-1 would put points right at black hole). this is to prevent black holes from */
				/* turning things inside out */

//yuch-- can speed this up by checking for strength x == y == z, all positive or negative, etc
#define DISTORTION_MIN -0.9f
				Vector3 Distortion = ((Vector3)pDistortion->PointForce.vTransformedStrength) / Magnitude;
				
				if (Distortion.x() < DISTORTION_MIN)
					Distortion.x(DISTORTION_MIN);
				if (Distortion.y() < DISTORTION_MIN)
					Distortion.y(DISTORTION_MIN);
				if (Distortion.z() < DISTORTION_MIN)
					Distortion.z(DISTORTION_MIN);

				vDelta = vDelta.Diag(Distortion);
			}
			else
			{
				/* adjustment vector is some fraction of original strength */
				vDelta = ((Vector3)pDistortion->PointForce.vTransformedStrength) / Magnitude;
			}

			/* and adjust passed point */

			*pvDest += vDelta;
			break;

		/* global wind is effective everywhere */

		case DISTORTION_GLOBAL_WIND:
			*pvDest += ((Vector3)pDistortion->GlobalWind.vTransformedStrength) * fFrameSeconds;
			break;

		/* vortex wind is a cylinder with the wind moving around its surface */

		case DISTORTION_VORTEX_WIND:
			/* distance from passed point to center line of cylinder. assume straight */
			/* up and down in camera space */

			vDelta.Set(vPos.x() - pDistortion->VortexWind.vTransformedPos.x(),
//						  0.0f;
//						  vPos.y - pDistortion->VortexWind.vTransformedPos.y;
						  0.0f,
						  vPos.z() - pDistortion->VortexWind.vTransformedPos.z());
			
			Magnitude = vDelta.Length();

			Range = Math::Abs(pDistortion->VortexWind.Radius - Magnitude);
			if (Range < pDistortion->VortexWind.Range)
			{
				/* within band. calculate direction of wind */

//				Vector3 vSideways(1.0f, 0.0f, 0.0f);	// can be interesting if vortex is on its side
				Vector3 vWind;
				vWind = Vector3::Cross(vDelta, vUp);
				vWind.Normalize();

				/* calculate magnitude of wind */

				Multiplier = pDistortion->VortexWind.Amplitude * (pDistortion->VortexWind.Range - Magnitude) / pDistortion->VortexWind.Range;
				vWind *= Multiplier;

				/* the particle's point plus the wind vector is farther from the center than */
				/* the particle. bring it back to the circle, so the wind blows in somewhat */
				/* and particles tend to go round and round */

//yuch-- rewrite! can't use vortex pos, it could be huge!
#if 0
				vDelta.Set(vPos.x() + vWind.x() - pDistortion->VortexWind.vTransformedPos.x(),
							  0.0f,
							  vPos.z() + vWind.z() - pDistortion->VortexWind.vTransformedPos.z());
				Range = vDelta.Length();
				vDelta.x(vDelta.x() * Magnitude / Range);
				vDelta.z(vDelta.z() * Magnitude / Range);
				vWind.Set(vDelta.x() - vPos.x();
//				vWind.y = vDelta.y - vPos.y;
//				vWind.z = 0.0f;
							 0.0f,
							 vDelta.z() - vPos.z());
#endif //0

				/* and adjust particle's velocity */

				*pvDest += vWind * fFrameSeconds;
			}
			break;

		/* wind gusts add vTransformedStrength at center, 0 at radius */

		case DISTORTION_WIND_GUST:
			if( pDistortion->WindGust.CurrDelay == 0.0f)
			{
				vDelta = vPos - Vector3(pDistortion->WindGust.vTransformedPos);
				Magnitude = vDelta.LengthSquared();
				if (Magnitude < pDistortion->WindGust.fRadiusSquared)
				{
					/* particle is inside wind gust. figure oout how much strength to apply */
					
					if ( pDistortion->WindGust.u8Flags & WIND_GUST_FULL_STRENGTH )
					{
						Multiplier = 1.0f;
					}
					else
					{
						Magnitude = Math::Sqrt(Magnitude);
						Multiplier = pDistortion->WindGust.fRadius / Magnitude;
						Multiplier *= fFrameSeconds;	// take frame rate into account
					}
					
					*pvDest += ((Vector3)pDistortion->WindGust.vTransformedStrength) * Multiplier;
				}
			}
			break;

		/* updrafts are cylinders of upward or downward wind */

		case DISTORTION_UPDRAFT:
			/* within cylinder in y? */

			if ((vPos.y() > pDistortion->Updraft.vPos.y()) && (vPos.y() < pDistortion->Updraft.vPos.y() + pDistortion->Updraft.fYVelocity))
			{
				/* within cylinder in x/z? */

				if ((vPos - Vector3(pDistortion->Updraft.vPos)).LengthXZSquared() < pDistortion->Updraft.fRadiusSquared)
				{
					/* in cylinder-- affect */

					pvDest->y(pvDest->y() + pDistortion->Updraft.fYVelocity * fFrameSeconds);
				}
			}
			break;

		/* forcefields keep particles out and divert them around the sphere */

		case DISTORTION_FORCEFIELD:
			vDelta = vPos - Vector3(pDistortion->Forcefield.vTransformedPos);
			Magnitude = vDelta.LengthSquared();
			if ((Magnitude < pDistortion->Forcefield.fRadiusSquared) && (!Math::Zero(Magnitude)))
			{
				/* particle is within the forcefield. put it back outside */

				Magnitude = Math::Sqrt(Magnitude);
				Multiplier = pDistortion->Forcefield.fRadius / Magnitude;
				*pvPos = ((Vector3)pDistortion->Forcefield.vTransformedPos) + (vDelta * Multiplier);
			}
			break;

		/* ripples are an expanding cylinder that raises vertices within range. */
		/* typically used for water, but can be turned and used for other stuff */

		case DISTORTION_RIPPLE:
			/* distance from passed point to ripple origin, 2d calculation */

			vDelta.x( vPos.x() - pDistortion->Ripple.vTransformedPos.x());
			vDelta.y( vPos.y() - pDistortion->Ripple.vTransformedPos.y());
			Magnitude = Math::Sqrt((vDelta.x() * vDelta.x()) + (vDelta.y() * vDelta.y()));
			Magnitude = Math::Abs(pDistortion->Ripple.Radius - Magnitude);
			if (Magnitude < pDistortion->Ripple.Range)
				pvDest->z(pvDest->z() + pDistortion->Ripple.Amplitude * (pDistortion->Ripple.Range - Magnitude) / pDistortion->Ripple.Range);
			break;

		/* point multipliers multiply the dest vector by the multiplier value at the center */
		/* point and by 1.0 at the radius */

		case DISTORTION_POINT_MULTIPLIER:
			/* distance from passed point to distortion point */

			vDelta = vPos - Vector3(pDistortion->PointMultiplier.vTransformedPos);
			Magnitude = vPos.LengthSquared();
			if (Magnitude < pDistortion->PointMultiplier.RadiusSquared)
			{
				Magnitude = Math::Sqrt(Magnitude);
				Multiplier = pDistortion->PointMultiplier.MultiplierAtCenter + ((Magnitude / pDistortion->PointMultiplier.Radius) * pDistortion->PointMultiplier.MultiplierDelta);
				*pvDest *= Multiplier;
			}
			break;

		case DISTORTION_POINT_ROTATE:
			/* distance from passed point to distortion point */

			vDelta = vPos - Vector3(pDistortion->PointRotate.vTransformedPos);
			Magnitude = vDelta.LengthSquared();
			if (Magnitude < pDistortion->PointRotate.RadiusSquared)
			{
				Vector3 vNewNormal;

				/* calculate transformed normal */

				vNewNormal = *pvDest * pDistortion->PointRotate.pTransformedMatrix->mMatrix;

				/* interpolate between original and new normal. 0.0 means right at center, */
				/* 1.0 means right at radius */

//yuch-- not efficiently done
				Magnitude = Math::Sqrt(Magnitude) / pDistortion->PointRotate.Radius;
				*pvDest = vNewNormal + ((*pvDest - vNewNormal) * Magnitude);
			}
			break;
		default:
			ASSERT(FALSE);	// all cases should be handled
			break;
		}
	}
}

/* add a point force to distortion list */

t_Handle DistortionMgr__AddPointForce(
	Vector3 *vPos,
	Vector3 *vVelocity,
	Vector3 *vAcceleration,
	Vector3 *vStrength,
	float Lifetime,					// in seconds, 0.0 means forever
	u16 u16DistortionFlags,
	u8 u8Flags,
	u16 u16Group /*= DISTORTION_DEFAULT_GROUP*/)
{
	ts_Distortion *pDistortion;

	/* grab a distortion record */

	pDistortion = DistortionMgr__AllocDistortion(u16Group);
	if (pDistortion == NULL)
		return(INVALID_HANDLE);
	pDistortion->dh.u8SubDataType = DISTORTION_POINT_FORCE;	// set sub-type
	pDistortion->dh.u16Flags |= DISTORTION_AFFECTS_VERTICES;	// set compatibility
	pDistortion->dh.u16Flags |= u16DistortionFlags;	// add user-passed flags

	/* set up point force */

	pDistortion->PointForce.vPos = *vPos;
	pDistortion->PointForce.vVelocity = *vVelocity;
	pDistortion->PointForce.vAcceleration = *vAcceleration;
	pDistortion->PointForce.vStrength = *vStrength;
	pDistortion->PointForce.Lifetime = Lifetime;

	/* flags */

	pDistortion->PointForce.u8Flags = u8Flags;
	if ((vVelocity->x() != 0.0f) || (vVelocity->y() != 0.0f) || (vVelocity->z() != 0.0f))
		pDistortion->PointForce.u8Flags |= POINT_FORCE_HAS_VELOCITY;
	if ((vAcceleration->x() != 0.0f) || (vAcceleration->y() != 0.0f) || (vAcceleration->z() != 0.0f))
		pDistortion->PointForce.u8Flags |= POINT_FORCE_HAS_ACCELERATION;
	return(pDistortion->Handle);
}	

/* add a global wind to the distortion list */

t_Handle DistortionMgr__AddGlobalWind(
	Vector3 *vStrength,
	float Randomness,
	float SinAngleStart,
	float SinAngleInc,
	float SinMin,
	float SinMax,
	float Damping,
	float Lifetime,					// in seconds, 0.0 means forever
	u16 u16DistortionFlags,
	u8 u8Flags,
	u16 u16Group /*= DISTORTION_DEFAULT_GROUP*/)
{
	ts_Distortion *pDistortion;
	ASSERT((SinAngleInc == 0.0f) || (SinMin <= SinMax));
	ASSERT(Damping <= 0.0f);

	/* grab a distortion record */

	pDistortion = DistortionMgr__AllocDistortion(u16Group);
	if (pDistortion == NULL)
		return(INVALID_HANDLE);
	pDistortion->dh.u8SubDataType = DISTORTION_GLOBAL_WIND;		// set sub-type
	pDistortion->dh.u16Flags |= (DISTORTION_AFFECTS_PARTICLES | DISTORTION_AFFECTS_CHAINS | DISTORTION_AFFECTS_MESHES);	// set compatibility
	pDistortion->dh.u16Flags |= u16DistortionFlags;					// add user-passed flags

	/* set up global wind */

	pDistortion->GlobalWind.vBaseStrength = *vStrength;
	pDistortion->GlobalWind.Randomness = Randomness;
	pDistortion->GlobalWind.SinAngle = SinAngleStart;
	pDistortion->GlobalWind.SinAngleInc = SinAngleInc;
	pDistortion->GlobalWind.SinMinMaxCenter = (SinMin + SinMax) / 2.0f;
	pDistortion->GlobalWind.SinMinMaxHalfDelta = (SinMax - SinMin) / 2.0f;
	pDistortion->GlobalWind.DampingMult = 1.0f;
	pDistortion->GlobalWind.Damping = Damping;
	pDistortion->GlobalWind.Lifetime = Lifetime;

	/* flags */

	pDistortion->GlobalWind.u8Flags = u8Flags;
	if ((SinAngleInc != 0.0f) && (SinMin < SinMax))
		pDistortion->GlobalWind.u8Flags |= GLOBAL_WIND_HAS_SINE_WAVE;
	if ((Damping != 0.0f) && ((pDistortion->GlobalWind.u8Flags & GLOBAL_WIND_DELAY_DAMPING) == 0))
		pDistortion->GlobalWind.u8Flags |= GLOBAL_WIND_HAS_DAMPING;

	/* track it in manager */

	DistortionMgr.pGlobalWind = &pDistortion->GlobalWind;

	/* set initial strength */

	DistortionMgr__SetGlobalWindStrength(&pDistortion->GlobalWind);
	return(pDistortion->Handle);
}

/* add a vortex wind to distortion list */

t_Handle DistortionMgr__AddVortexWind(
	Vector3 *vPos,
	float StartRadius,
	float Range,
	float RadiusVelocity,
	float RadiusAcceleration,
	float Amplitude,
	float Damping,
	float Lifetime,					// in seconds, 0.0 means forever
	u16 u16DistortionFlags,
	u8 u8Flags,
	u16 u16Group /*= DISTORTION_DEFAULT_GROUP*/)
{
	ts_Distortion *pDistortion;

	ASSERT((Damping == 0.0f) || ((u8Flags & VORTEX_WIND_DELAY_DAMPING) == 0));

	/* grab a distortion record */

	pDistortion = DistortionMgr__AllocDistortion(u16Group);
	if (pDistortion == NULL)
		return(INVALID_HANDLE);
	pDistortion->dh.u8SubDataType = DISTORTION_VORTEX_WIND;		// set sub-type
	pDistortion->dh.u16Flags |= (DISTORTION_AFFECTS_PARTICLES | DISTORTION_AFFECTS_CHAINS | DISTORTION_AFFECTS_MESHES);	// set compatibility
	pDistortion->dh.u16Flags |= u16DistortionFlags;					// add user-passed flags

	/* set up vortex wind */

	pDistortion->VortexWind.vPos = *vPos;
	pDistortion->VortexWind.Radius = StartRadius;
	pDistortion->VortexWind.Range = Range;
	pDistortion->VortexWind.RadiusVelocity = RadiusVelocity;
	pDistortion->VortexWind.RadiusAcceleration = RadiusAcceleration;
	pDistortion->VortexWind.Amplitude = Amplitude;
	pDistortion->VortexWind.Damping = Damping;
	pDistortion->VortexWind.Lifetime = Lifetime;

	/* some flag checking */

	pDistortion->VortexWind.u8Flags = u8Flags;
	if ((Damping != 0.0f) && ((pDistortion->VortexWind.u8Flags & VORTEX_WIND_DELAY_DAMPING) == 0))
		pDistortion->VortexWind.u8Flags |= VORTEX_WIND_HAS_DAMPING;
	return(pDistortion->Handle);
}	

/* add a wind gust to distortion list */

t_Handle DistortionMgr__AddWindGust(
	Vector3 *vPos,
	Vector3 *vVelocity,
	Vector3 *vAcceleration,
	Vector3 *vStrength,
	float Radius,
	float Delay,
	float Lifetime,					// in seconds, 0.0 means forever
	float Deathtime,					// in seconds, 0.0 means forever
	u16 u16DistortionFlags,
	u8 u8Flags,
	u16 u16Group /*= DISTORTION_DEFAULT_GROUP*/)
{
	ts_Distortion *pDistortion;

	/* grab a distortion record */

	pDistortion = DistortionMgr__AllocDistortion(u16Group);
	if (pDistortion == NULL)
		return(INVALID_HANDLE);
	pDistortion->dh.u8SubDataType = DISTORTION_WIND_GUST;	// set sub-type
	pDistortion->dh.u16Flags |= (DISTORTION_AFFECTS_PARTICLES | DISTORTION_AFFECTS_CHAINS | DISTORTION_AFFECTS_MESHES);	// set compatibility
	pDistortion->dh.u16Flags |= u16DistortionFlags;	// add user-passed flags

	/* set up wind gust */

	pDistortion->WindGust.vPos = *vPos;
	pDistortion->WindGust.vVelocity = *vVelocity;
	pDistortion->WindGust.vAcceleration = *vAcceleration;
	pDistortion->WindGust.vStrength = *vStrength;
	pDistortion->WindGust.vCurrStrength = *vStrength;
	pDistortion->WindGust.fRadius = Radius;
	pDistortion->WindGust.fRadiusSquared = Radius * Radius;
	pDistortion->WindGust.Delay = Delay;
	pDistortion->WindGust.Lifetime = Lifetime;
	pDistortion->WindGust.CurrLifetime = Lifetime;
	pDistortion->WindGust.Deathtime = Deathtime;
	pDistortion->WindGust.CurrDelay = 0.0f;

	/* flags */

	pDistortion->WindGust.u8Flags = u8Flags;
	if ((vVelocity->x() != 0.0f) || (vVelocity->y() != 0.0f) || (vVelocity->z() != 0.0f))
		pDistortion->WindGust.u8Flags |= WIND_GUST_HAS_VELOCITY;
	if ((vAcceleration->x() != 0.0f) || (vAcceleration->y() != 0.0f) || (vAcceleration->z() != 0.0f))
		pDistortion->WindGust.u8Flags |= WIND_GUST_HAS_ACCELERATION;

	return(pDistortion->Handle);
}	

/* add an updraft to distortion list */

t_Handle DistortionMgr__AddUpdraft(
	Vector3 *vPos,
	float fYVelocity,
	float Height,
	float Radius,
	float Lifetime,
	u16 u16DistortionFlags,
	u8 u8Flags,
	u16 u16Group /*= DISTORTION_DEFAULT_GROUP*/)
{
	/* grab a distortion record */

	ts_Distortion *pDistortion = DistortionMgr__AllocDistortion(u16Group);
	if (pDistortion == NULL)
		return(INVALID_HANDLE);
	pDistortion->dh.u8SubDataType = DISTORTION_UPDRAFT;		// set sub-type
	pDistortion->dh.u16Flags |= (DISTORTION_AFFECTS_CHAINS | DISTORTION_AFFECTS_MESHES);	// set compatibility
	pDistortion->dh.u16Flags |= u16DistortionFlags;				// add user-passed flags

	/* set up forcefield */

	pDistortion->Updraft.vPos = *vPos;
	pDistortion->Updraft.fYVelocity = fYVelocity;
	pDistortion->Updraft.fHeight = Height;
	pDistortion->Updraft.fRadius = Radius;
	pDistortion->Updraft.fRadiusSquared = Radius * Radius;
	pDistortion->Updraft.Lifetime = Lifetime;

	/* some flag checking */

	pDistortion->Updraft.u8Flags = u8Flags;
	return(pDistortion->Handle);
}

/* add a forcefield to distortion list */

t_Handle DistortionMgr__AddForcefield(
	Vector3 *pvPos,
	float Radius,
	float Lifetime,
	u16 u16DistortionFlags,
	u8 u8Flags,
	u16 u16Group /*= DISTORTION_DEFAULT_GROUP*/)
{
	/* grab a distortion record */

	ts_Distortion *pDistortion = DistortionMgr__AllocDistortion(u16Group);
	if (pDistortion == NULL)
		return(INVALID_HANDLE);
	pDistortion->dh.u8SubDataType = DISTORTION_FORCEFIELD;		// set sub-type
	pDistortion->dh.u16Flags |= FORCEFIELD_AFFECTS_PARTICLES;	// set compatibility
	pDistortion->dh.u16Flags |= u16DistortionFlags;					// add user-passed flags

	/* set up forcefield */

	pDistortion->Forcefield.pvPos = pvPos;
	pDistortion->Forcefield.fRadius = Radius;
	pDistortion->Forcefield.fRadiusSquared = Radius * Radius;
	pDistortion->Forcefield.fLifetime = Lifetime;

	/* stationary? copy passed position in since it may be transient */

	if (pDistortion->dh.u16Flags)
		pDistortion->Forcefield.vPos = *pDistortion->Forcefield.pvPos;

	/* some flag checking */

	pDistortion->Forcefield.u8Flags = u8Flags;
	return(pDistortion->Handle);
}	

/* add a ripple to distortion list */

t_Handle DistortionMgr__AddRipple(
	Vector3 *vPos,
	float StartRadius,
	float Range,
	float RadiusVelocity,
	float RadiusAcceleration,
	float Amplitude,
	float Damping,
	float Lifetime,					// in seconds, 0.0 means forever
	u16 u16DistortionFlags,
	u8 u8Flags,
	u16 u16Group /*= DISTORTION_DEFAULT_GROUP*/)
{
	ts_Distortion *pDistortion;

	ASSERT(RadiusVelocity > 0.0f);
	ASSERT((Damping == 0.0f) || ((u8Flags & RIPPLE_DELAY_DAMPING) == 0));

	/* grab a distortion record */

	pDistortion = DistortionMgr__AllocDistortion(u16Group);
	if (pDistortion == NULL)
		return(INVALID_HANDLE);
	pDistortion->dh.u8SubDataType = DISTORTION_RIPPLE;			// set sub-type
	pDistortion->dh.u16Flags |= DISTORTION_AFFECTS_VERTICES;	// set compatibility
	pDistortion->dh.u16Flags |= u16DistortionFlags;				// add user-passed flags

	/* set up ripple */

	pDistortion->Ripple.vPos = *vPos;
	pDistortion->Ripple.Radius = StartRadius;
	pDistortion->Ripple.Range = Range;
	pDistortion->Ripple.RadiusVelocity = RadiusVelocity;
	pDistortion->Ripple.RadiusAcceleration = RadiusAcceleration;
	pDistortion->Ripple.Amplitude = Amplitude;
	pDistortion->Ripple.Damping = Damping;
	pDistortion->Ripple.Lifetime = Lifetime;

	/* some flag checking */

	pDistortion->Ripple.u8Flags = u8Flags;
	if (Amplitude < 0.0f)
		pDistortion->Ripple.u8Flags |= RIPPLE_NEGATIVE_AMPLITUDE;
	if ((Damping != 0.0f) && ((pDistortion->Ripple.u8Flags & RIPPLE_DELAY_DAMPING) == 0))
		pDistortion->Ripple.u8Flags |= RIPPLE_HAS_DAMPING;
	return(pDistortion->Handle);
}	

t_Handle DistortionMgr__AddPointMultiplier(
	Vector3 *vPos,
	Vector3 *vVelocity,
	Vector3 *vAcceleration,
	float MultiplierAtCenter,
	float MultiplierAtRadius,
	float Radius,
	float Lifetime,					// in seconds, 0.0 means forever
	u16 u16DistortionFlags,
	u8 u8Flags,
	u16 u16Group /*= DISTORTION_DEFAULT_GROUP*/)
{
	ts_Distortion *pDistortion;
	ASSERT((MultiplierAtCenter != 1.0f) || (MultiplierAtRadius != 1.0f));	// no effect if both 1.0

	/* grab a distortion record */

	pDistortion = DistortionMgr__AllocDistortion(u16Group);
	if (pDistortion == NULL)
		return(INVALID_HANDLE);
	pDistortion->dh.u8SubDataType = DISTORTION_POINT_MULTIPLIER;	// set sub-type
	pDistortion->dh.u16Flags |= DISTORTION_AFFECTS_NORMALS;			// set compatibility
	pDistortion->dh.u16Flags |= u16DistortionFlags;						// add user-passed flags

	/* set up point force */

	pDistortion->PointMultiplier.vPos = *vPos;
	pDistortion->PointMultiplier.vVelocity = *vVelocity;
	pDistortion->PointMultiplier.vAcceleration = *vAcceleration;
	pDistortion->PointMultiplier.MultiplierAtCenter = MultiplierAtCenter;
	pDistortion->PointMultiplier.MultiplierDelta = MultiplierAtRadius - MultiplierAtCenter;
	pDistortion->PointMultiplier.Radius = Radius;
	pDistortion->PointMultiplier.RadiusSquared = Radius * Radius;
	pDistortion->PointMultiplier.Lifetime = Lifetime;

	/* flags */

	pDistortion->PointMultiplier.u8Flags = u8Flags;
	if ((vVelocity->x() != 0.0f) || (vVelocity->y() != 0.0f) || (vVelocity->z() != 0.0f))
		pDistortion->PointMultiplier.u8Flags |= POINT_MULTIPLIER_HAS_VELOCITY;
	if ((vAcceleration->x() != 0.0f) || (vAcceleration->y() != 0.0f) || (vAcceleration->z() != 0.0f))
		pDistortion->PointMultiplier.u8Flags |= POINT_MULTIPLIER_HAS_ACCELERATION;
	return(pDistortion->Handle);
}	

t_Handle DistortionMgr__AddPointRotate(
	Vector3 *vPos,
	Vector3 *vVelocity,
	Vector3 *vAcceleration,
	Vector3 *vRotate,
	float Radius,
	float Lifetime,					// in seconds, 0.0 means forever
	u16 u16DistortionFlags,
	u8 u8Flags,
	u16 u16Group /*= DISTORTION_DEFAULT_GROUP*/)
{
	ts_Distortion *pDistortion;

	/* grab a distortion record */

	pDistortion = DistortionMgr__AllocDistortion(u16Group);
	if (pDistortion == NULL)
		return(INVALID_HANDLE);
	pDistortion->dh.u8SubDataType = DISTORTION_POINT_ROTATE;	// set sub-type
	pDistortion->dh.u16Flags |= DISTORTION_AFFECTS_NORMALS;		// set compatibility
	pDistortion->dh.u16Flags |= u16DistortionFlags;					// add user-passed flags

	/* need two matrices */

	pDistortion->PointRotate.pMatrix = MatrixPool__Alloc(pDistortion);
	pDistortion->PointRotate.pTransformedMatrix = MatrixPool__Alloc(pDistortion);
	if ((pDistortion->PointRotate.pMatrix == NULL) || (pDistortion->PointRotate.pTransformedMatrix == NULL))
	{
		Distortion__Kill(pDistortion);	// move alloc'd record back to free list
		return(INVALID_HANDLE);
	}

	/* set up point force */

	pDistortion->PointRotate.vPos = *vPos;
	pDistortion->PointRotate.vVelocity = *vVelocity;
	pDistortion->PointRotate.vAcceleration = *vAcceleration;
	pDistortion->PointRotate.vRotate = *vRotate;
	pDistortion->PointRotate.Radius = Radius;
	pDistortion->PointRotate.RadiusSquared = Radius * Radius;
	pDistortion->PointRotate.Lifetime = Lifetime;

	/* flags */

	pDistortion->PointRotate.u8Flags = u8Flags;
	if ((vVelocity->x() != 0.0f) || (vVelocity->y() != 0.0f) || (vVelocity->z() != 0.0f))
		pDistortion->PointRotate.u8Flags |= POINT_ROTATE_HAS_VELOCITY;
	if ((vAcceleration->x() != 0.0f) || (vAcceleration->y() != 0.0f) || (vAcceleration->z() != 0.0f))
		pDistortion->PointRotate.u8Flags |= POINT_ROTATE_HAS_ACCELERATION;
	return(pDistortion->Handle);
}	

/* set the effective strength of a global wind */

void DistortionMgr__SetGlobalWindStrength(ts_GlobalWind *pGlobalWind)
{
	float Multiplier = 1.0f;
	if (pGlobalWind->u8Flags & GLOBAL_WIND_HAS_SINE_WAVE)
	{
		/* bryant added in the randomness term-- what it does is add another sin wave in on top of the first one, */
		/* changed in frequency and shifted in time. this gives the basic sin wave a more complex shape, but also */
		/* tends to double it (since we are now adding two sin waves together). The randomness value is slippery, */
		/* it's not that a higher value is "more random,", but higher values have a higher frequency, so the result */
		/* wave is choppier. we add a 0.5 term if we are using randomness to cut min/max range back to original */
		/* range */

		if (pGlobalWind->Randomness == 0.0f)
			Multiplier = pGlobalWind->SinMinMaxCenter + (pGlobalWind->SinMinMaxHalfDelta * Math::Sin(pGlobalWind->SinAngle));
		else
			Multiplier = pGlobalWind->SinMinMaxCenter + (pGlobalWind->SinMinMaxHalfDelta * 0.5f * (Math::Sin(pGlobalWind->SinAngle) + Math::Sin(pGlobalWind->Randomness * (pGlobalWind->SinAngle - pGlobalWind->Randomness))));
	}
	Multiplier *= pGlobalWind->DampingMult;
	pGlobalWind->vStrength = ((Vector3)pGlobalWind->vBaseStrength) * Multiplier;
}

/* update distortion system */

void DistortionMgr__Advance(void)
{
	int i;
	bool bDealloc;

	/* quick exit */

	if (DistortionMgr.DistortionActive.pHead == NULL)
		return;

	/* get time slice */

	float fFrameSeconds = g_timer.GetFrameSec();

	/* build list of distortions to advance. this is necessary because distortions may get stolen during advance process */
	/* (mainly due to spawning at life's end), and i don't want to advance new distortions. worse things can happen, too, */
	/* like the chain of links pointing back to the beginning of the list and advancing distortions twice */

	DistortionMgr.nAdvanceListSize = 0;
	ts_Distortion *pNextDistortion = (ts_Distortion *) DistortionMgr.DistortionActive.pHead;
	while (pNextDistortion)
	{
		/* pre-load next distortion */

		ts_Distortion *pDistortion = pNextDistortion;
		pNextDistortion = pNextDistortion->pNext;

		/* enabled? */

		if ((pDistortion->dh.u16Flags & DISTORTION_ENABLED) == 0)
			continue;

		/* add to list */

		DistortionMgr.ppAdvanceList[DistortionMgr.nAdvanceListSize++] = pDistortion;
		pDistortion->dh.u16Flags |= DISTORTION_NEEDS_ADVANCEMENT;	// will be cleared by alloc/dealloc if it gets stolen
	}

	/* loop through distortions */

	for (i = 0; i < DistortionMgr.nAdvanceListSize; i++)
	{
		ts_Distortion *pDistortion = DistortionMgr.ppAdvanceList[i];

		/* check advancement flag */

		if ((pDistortion->dh.u16Flags & DISTORTION_NEEDS_ADVANCEMENT) == 0)
			continue;

		/* still alive */

		bDealloc = FALSE;

		/* handle it */

		switch(pDistortion->dh.u8SubDataType)
		{
		case DISTORTION_POINT_FORCE:
			/* velocity and acceleration */

			if (pDistortion->PointForce.u8Flags & POINT_FORCE_HAS_VELOCITY)
			{
				pDistortion->PointForce.vPos = ((Vector3)pDistortion->PointForce.vPos) + ((Vector3)pDistortion->PointForce.vVelocity) * fFrameSeconds;
			}
			if (pDistortion->PointForce.u8Flags & POINT_FORCE_HAS_ACCELERATION)
			{
				pDistortion->PointForce.vVelocity = ((Vector3)pDistortion->PointForce.vVelocity) + ((Vector3)pDistortion->PointForce.vAcceleration) * fFrameSeconds;
			}

			/* lifetime */

			if (pDistortion->PointForce.Lifetime != 0.0f)
			{
				pDistortion->PointForce.Lifetime -= fFrameSeconds;
				if (pDistortion->PointForce.Lifetime <= 0.0f)
				{
					pDistortion->PointForce.Lifetime = 0.0f;
					bDealloc = TRUE;
				}
			}
			break;

		case DISTORTION_GLOBAL_WIND:
			/* sine wave */

			if (pDistortion->GlobalWind.u8Flags & GLOBAL_WIND_HAS_SINE_WAVE)
				pDistortion->GlobalWind.SinAngle += pDistortion->GlobalWind.SinAngleInc * fFrameSeconds;

			/* damping */

			if (pDistortion->GlobalWind.u8Flags & GLOBAL_WIND_HAS_DAMPING)
			{
				pDistortion->GlobalWind.DampingMult += pDistortion->GlobalWind.Damping * fFrameSeconds;
				if (pDistortion->GlobalWind.DampingMult <= 0.0f)
					bDealloc = TRUE;
			}

			/* lifetime */

			if (pDistortion->GlobalWind.Lifetime != 0.0f)
			{
				pDistortion->GlobalWind.Lifetime -= fFrameSeconds;
				if (pDistortion->GlobalWind.Lifetime <= 0.0f)
				{
					pDistortion->GlobalWind.Lifetime = 0.0f;
					if ((pDistortion->GlobalWind.u8Flags & GLOBAL_WIND_DELAY_DAMPING) && (pDistortion->GlobalWind.Damping != 0.0f))
						pDistortion->GlobalWind.u8Flags |= GLOBAL_WIND_HAS_DAMPING;
					else
						bDealloc = TRUE;
				}
			}

			/* track it in manager */

			if ((bDealloc) && (DistortionMgr.pGlobalWind == &pDistortion->GlobalWind))
				DistortionMgr.pGlobalWind = NULL;	// global wind is going away
			else
				DistortionMgr.pGlobalWind = &pDistortion->GlobalWind;

			/* calculate effective strength */

			DistortionMgr__SetGlobalWindStrength(&pDistortion->GlobalWind);
			break;

		case DISTORTION_VORTEX_WIND:
			/* velocity and acceleration */

			pDistortion->VortexWind.Radius += pDistortion->VortexWind.RadiusVelocity * fFrameSeconds;
			if (pDistortion->VortexWind.Radius <= 0.0f)
				bDealloc = TRUE;
			if (pDistortion->VortexWind.RadiusAcceleration != 0.0f)
			{
				pDistortion->VortexWind.RadiusVelocity += pDistortion->VortexWind.RadiusAcceleration * fFrameSeconds;
				if (pDistortion->VortexWind.RadiusVelocity < 0.0f)
				{
					pDistortion->VortexWind.RadiusVelocity = 0.0f;
					pDistortion->VortexWind.RadiusAcceleration = 0.0f;

					/* make sure it's damping if it stops! */

					if (pDistortion->VortexWind.Damping <= 0.0f)
						pDistortion->VortexWind.Damping = 0.1f;
				}
			}

			/* amplitude */

			if (pDistortion->VortexWind.u8Flags & VORTEX_WIND_HAS_DAMPING)
			{
				/* cheesy flag so that sign sense of damping is consistent. a negative damping */
				/* value also reduces the amplitude of the vortex wind, even if amplitude is negative, */
				/* and always check for going below 0.0 for ending vortex wind */

//				if (pDistortion->VortexWind.u8Flags & VORTEX_WIND_NEGATIVE_AMPLITUDE)
//					pDistortion->VortexWind.Amplitude *= -1.0f;
				pDistortion->VortexWind.Amplitude += pDistortion->VortexWind.Damping * fFrameSeconds;
				if (pDistortion->VortexWind.Amplitude <= 0.0f)
					bDealloc = TRUE;
//				if (pDistortion->VortexWind.u8Flags & VORTEX_WIND_NEGATIVE_AMPLITUDE)
//					pDistortion->VortexWind.Amplitude *= -1.0f;
			}

			/* lifetime */

			if (pDistortion->VortexWind.Lifetime != 0.0f)
			{
				pDistortion->VortexWind.Lifetime -= fFrameSeconds;
				if (pDistortion->VortexWind.Lifetime <= 0.0f)
				{
					pDistortion->VortexWind.Lifetime = 0.0f;
					if ((pDistortion->VortexWind.u8Flags & VORTEX_WIND_DELAY_DAMPING) && (pDistortion->VortexWind.Damping != 0.0f))
						pDistortion->VortexWind.u8Flags |= VORTEX_WIND_HAS_DAMPING;
					else
						bDealloc = TRUE;
				}
			}
			break;

		case DISTORTION_WIND_GUST:
			/* velocity and acceleration */

			if (pDistortion->WindGust.u8Flags & WIND_GUST_HAS_VELOCITY)
			{
				pDistortion->WindGust.vPos = ((Vector3)pDistortion->WindGust.vPos) + ((Vector3)pDistortion->WindGust.vVelocity) * fFrameSeconds;
			}
			if (pDistortion->WindGust.u8Flags & WIND_GUST_HAS_ACCELERATION)
			{
				pDistortion->WindGust.vVelocity = ((Vector3)pDistortion->WindGust.vVelocity) + ((Vector3)pDistortion->WindGust.vAcceleration) * fFrameSeconds;
			}


			/* to visualize */
			if ( bShowDistortion )
				DrawUtility::Sphere(pDistortion->WindGust.vPos, DrawUtility::WHITE, pDistortion->WindGust.fRadius);

			/* lifetime */

			if (pDistortion->WindGust.CurrDelay != 0.0f)
			{
				pDistortion->WindGust.CurrDelay -= fFrameSeconds;
				if (pDistortion->WindGust.CurrDelay <= 0.0f)
				{
					pDistortion->WindGust.CurrDelay = 0.0f;
					pDistortion->WindGust.vCurrStrength = g_random.InRange(0.75, 1.25f) * pDistortion->WindGust.vStrength;
					pDistortion->WindGust.CurrLifetime = pDistortion->WindGust.Lifetime * g_random.InRange(0.75, 1.25f);
				}
			}
			else
			if (pDistortion->WindGust.CurrLifetime != 0.0f)
			{
				pDistortion->WindGust.CurrLifetime -= fFrameSeconds;
				if (pDistortion->WindGust.CurrLifetime <= 0.0f)
				{
					pDistortion->WindGust.CurrLifetime = 0.0f;
					pDistortion->WindGust.CurrDelay = pDistortion->WindGust.Delay * g_random.InRange(0.75f, 1.25f);
				}
			}

			if (pDistortion->WindGust.Deathtime != 0.0f)
			{
				pDistortion->WindGust.Deathtime -= fFrameSeconds;
				if (pDistortion->WindGust.Deathtime <= 0.0f)
				{
					bDealloc = TRUE;
				}
			}

			break;

		case DISTORTION_UPDRAFT:
			/* nothing to do */
			break;

		case DISTORTION_FORCEFIELD:
			/* lifetime */

			/* to visualize */
			if ( bShowDistortion )
				DrawUtility::Sphere( *pDistortion->Forcefield.pvPos, DrawUtility::WHITE, pDistortion->Forcefield.fRadius);

			if (pDistortion->Forcefield.fLifetime != 0.0f)
			{
				pDistortion->Forcefield.fLifetime -= fFrameSeconds;
				if (pDistortion->Forcefield.fLifetime <= 0.0f)
				{
					pDistortion->Forcefield.fLifetime = 0.0f;
					bDealloc = TRUE;
				}
			}
			break;

		case DISTORTION_RIPPLE:
			/* velocity and acceleration */

			pDistortion->Ripple.Radius += pDistortion->Ripple.RadiusVelocity * fFrameSeconds;
			if (pDistortion->Ripple.RadiusAcceleration != 0.0f)
			{
				pDistortion->Ripple.RadiusVelocity += pDistortion->Ripple.RadiusAcceleration * fFrameSeconds;
				if (pDistortion->Ripple.RadiusVelocity < 0.0f)
				{
					pDistortion->Ripple.RadiusVelocity = 0.0f;
					pDistortion->Ripple.RadiusAcceleration = 0.0f;

					/* make sure it's damping if it stops! */

					if (pDistortion->Ripple.Damping <= 0.0f)
						pDistortion->Ripple.Damping = 0.1f;
				}
			}

			/* amplitude */

			if (pDistortion->Ripple.u8Flags & RIPPLE_HAS_DAMPING)
			{
				/* cheesy flag so that sign sense of damping is consistent. a negative damping */
				/* value also reduces the amplitude of the ripple, even if amplitude is negative, */
				/* and always check for going below 0.0 for ending ripple */

				if (pDistortion->Ripple.u8Flags & RIPPLE_NEGATIVE_AMPLITUDE)
					pDistortion->Ripple.Amplitude *= -1.0f;
				pDistortion->Ripple.Amplitude += pDistortion->Ripple.Damping * fFrameSeconds;
				if (pDistortion->Ripple.Amplitude <= 0.0f)
					bDealloc = TRUE;
				if (pDistortion->Ripple.u8Flags & RIPPLE_NEGATIVE_AMPLITUDE)
					pDistortion->Ripple.Amplitude *= -1.0f;
			}

			/* lifetime */

			if (pDistortion->Ripple.Lifetime != 0.0f)
			{
				pDistortion->Ripple.Lifetime -= fFrameSeconds;
				if (pDistortion->Ripple.Lifetime <= 0.0f)
				{
					pDistortion->Ripple.Lifetime = 0.0f;
					if ((pDistortion->Ripple.u8Flags & RIPPLE_DELAY_DAMPING) && (pDistortion->Ripple.Damping != 0.0f))
						pDistortion->Ripple.u8Flags |= RIPPLE_HAS_DAMPING;
					else
						bDealloc = TRUE;
				}
			}
			break;

		case DISTORTION_POINT_MULTIPLIER:
			/* velocity and acceleration */

			if (pDistortion->PointMultiplier.u8Flags & POINT_MULTIPLIER_HAS_VELOCITY)
			{
				pDistortion->PointMultiplier.vPos = ((Vector3)pDistortion->PointMultiplier.vPos) + ((Vector3)pDistortion->PointMultiplier.vVelocity) * fFrameSeconds;
			}
			if (pDistortion->PointMultiplier.u8Flags & POINT_MULTIPLIER_HAS_ACCELERATION)
			{
				pDistortion->PointMultiplier.vVelocity = ((Vector3)pDistortion->PointMultiplier.vVelocity) + ((Vector3)pDistortion->PointMultiplier.vAcceleration) * fFrameSeconds;
			}

			/* lifetime */

			if (pDistortion->PointMultiplier.Lifetime != 0.0f)
			{
				pDistortion->PointMultiplier.Lifetime -= fFrameSeconds;
				if (pDistortion->PointMultiplier.Lifetime <= 0.0f)
				{
					pDistortion->PointMultiplier.Lifetime = 0.0f;
					bDealloc = TRUE;
				}
			}
			break;

		case DISTORTION_POINT_ROTATE:
			/* velocity and acceleration */

			if (pDistortion->PointRotate.u8Flags & POINT_ROTATE_HAS_VELOCITY)
			{
				pDistortion->PointRotate.vPos = ((Vector3)pDistortion->PointRotate.vPos) + ((Vector3)pDistortion->PointRotate.vVelocity) * fFrameSeconds;
			}
			if (pDistortion->PointRotate.u8Flags & POINT_ROTATE_HAS_ACCELERATION)
			{
				pDistortion->PointRotate.vVelocity = ((Vector3)pDistortion->PointRotate.vVelocity) + ((Vector3)pDistortion->PointRotate.vAcceleration) * fFrameSeconds;
			}

			/* lifetime */

			if (pDistortion->PointRotate.Lifetime != 0.0f)
			{
				pDistortion->PointRotate.Lifetime -= fFrameSeconds;
				if (pDistortion->PointRotate.Lifetime <= 0.0f)
				{
					pDistortion->PointRotate.Lifetime = 0.0f;
					bDealloc = TRUE;
				}
			}

			/* build transform matrix */

			pDistortion->PointRotate.pMatrix->mMatrix.SetXYZRotationDeg(pDistortion->PointRotate.vRotate);
			break;
		default:
			ASSERT(FALSE);	// all cases should be handled
			break;
		}

		/* did current distortion end? */

		if (bDealloc)
			Distortion__Kill(pDistortion);
	}
}

/* build a list of distortions that may affect passed instance. check bounding conditions and
compatibility */

int DistortionMgr__BuildApplyList(ts_Bounds *pBounds, int nCompatibilityFlags)
{
	ASSERT((nCompatibilityFlags & ~DISTORTION_COMPATIBILITY_FLAGS) == 0);	// no flags other than compatibility flags
	DistortionMgr.nAdvanceListSize = 0;
	ts_Distortion *pNextDistortion = (ts_Distortion *) DistortionMgr.DistortionActive.pHead;
	while (pNextDistortion)
	{
		/* pre-load next distortion */

		ts_Distortion *pDistortion = pNextDistortion;
		pNextDistortion = pNextDistortion->pNext;

		/* enabled? */

		if ((pDistortion->dh.u16Flags & DISTORTION_ENABLED) == 0)
			continue;

		/* compatible? */

		if ((pDistortion->dh.u16Flags & nCompatibilityFlags) == 0)
			continue;

		/* bounding test */

		if (pBounds)
		{
		}

		/* add distortion to list */

		DistortionMgr.ppAdvanceList[DistortionMgr.nAdvanceListSize++] = pDistortion;
	}
	return(DistortionMgr.nAdvanceListSize);
}

/* transform distortion coordinate-sensitive fields into new space. generally, this is
transforming from world space to camera space for use with transformed vertices. the matrix
pointer may be NULL, however, in which case just copy the coordinates */

void DistortionMgr__TransformDistortions(Graphics4x4 *pmTransform, Graphics4x4 *pmTransformRotationOnly)
{
	/* if first pointer is valid, second must be too. that way i can just check first one */

	ASSERT((pmTransform == NULL) || (pmTransformRotationOnly != NULL));

	/* go through distortions and transform */

	ts_Distortion *pNextDistortion = (ts_Distortion *) DistortionMgr.DistortionActive.pHead;
	while (pNextDistortion)
	{
		/* pre-load next distortion */

		ts_Distortion *pDistortion = pNextDistortion;
		pNextDistortion = pNextDistortion->pNext;

		/* enabled? */

		if ((pDistortion->dh.u16Flags & DISTORTION_ENABLED) == 0)
			continue;

		/* switch on type */

		switch(pDistortion->dh.u8SubDataType)
		{
		case DISTORTION_POINT_FORCE:
			if ((pmTransform == NULL) || (pDistortion->dh.u16Flags & DISTORTION_NO_TRANSFORM))
			{
				pDistortion->PointForce.vTransformedPos = pDistortion->PointForce.vPos;
				pDistortion->PointForce.vTransformedStrength = pDistortion->PointForce.vStrength;
			}
			else
			{
				pDistortion->PointForce.vTransformedPos = ((Vector3)pDistortion->PointForce.vPos) * *pmTransform;
				if (pDistortion->PointForce.u8Flags & POINT_FORCE_OMNIDIRECTIONAL)
//yuch-- this isn't right-- only works if x, y, z components are equal...
					pDistortion->PointForce.vTransformedStrength = pDistortion->PointForce.vStrength;
				else
//yuch-- assume no scaling in transform matrix!
					((Vector3)pDistortion->PointForce.vTransformedStrength).FastBasisTransform(pDistortion->PointForce.vStrength, *pmTransform);
			}
			break;
		case DISTORTION_GLOBAL_WIND:
			if ((pmTransform == NULL) || (pDistortion->dh.u16Flags & DISTORTION_NO_TRANSFORM))
				pDistortion->GlobalWind.vTransformedStrength = pDistortion->GlobalWind.vStrength;
			else
//yuch-- assume no scaling in transform matrix!
				((Vector3)pDistortion->GlobalWind.vTransformedStrength).FastBasisTransform((Vector3)pDistortion->GlobalWind.vStrength, *pmTransform);
			break;
		case DISTORTION_VORTEX_WIND:
			if ((pmTransform == NULL) || (pDistortion->dh.u16Flags & DISTORTION_NO_TRANSFORM))
				pDistortion->VortexWind.vTransformedPos = pDistortion->VortexWind.vPos;
			else
				pDistortion->VortexWind.vTransformedPos = ((Vector3)pDistortion->VortexWind.vPos) * *pmTransform;
			break;
		case DISTORTION_WIND_GUST:
			if ((pmTransform == NULL) || (pDistortion->dh.u16Flags & DISTORTION_NO_TRANSFORM))
			{
				pDistortion->WindGust.vTransformedPos = pDistortion->WindGust.vPos;
				pDistortion->WindGust.vTransformedStrength = pDistortion->WindGust.vCurrStrength;
			}
			else
			{
				pDistortion->WindGust.vTransformedPos = ((Vector3)pDistortion->WindGust.vPos) * *pmTransform;
//yuch-- assume no scaling in transform matrix!
				((Vector3)pDistortion->WindGust.vTransformedStrength).FastBasisTransform((Vector3)pDistortion->WindGust.vCurrStrength, *pmTransform);
			}
			break;
		case DISTORTION_UPDRAFT:
			/* nothing to do */
			break;
		case DISTORTION_FORCEFIELD:
			if ((pmTransform == NULL) || (pDistortion->dh.u16Flags & DISTORTION_NO_TRANSFORM))
				pDistortion->Forcefield.vTransformedPos = *pDistortion->Forcefield.pvPos;
			else
				pDistortion->Forcefield.vTransformedPos = *pDistortion->Forcefield.pvPos * *pmTransform;
			break;
		case DISTORTION_RIPPLE:
			if ((pmTransform == NULL) || (pDistortion->dh.u16Flags & DISTORTION_NO_TRANSFORM))
				pDistortion->Ripple.vTransformedPos = pDistortion->Ripple.vPos;
			else
				pDistortion->Ripple.vTransformedPos = ((Vector3)pDistortion->Ripple.vPos) * *pmTransform;
			break;
		case DISTORTION_POINT_MULTIPLIER:
			if ((pmTransform == NULL) || (pDistortion->dh.u16Flags & DISTORTION_NO_TRANSFORM))
				pDistortion->PointMultiplier.vTransformedPos = pDistortion->PointMultiplier.vPos;
			else
				pDistortion->PointMultiplier.vTransformedPos = ((Vector3)pDistortion->PointMultiplier.vPos) * *pmTransform;
			break;
		case DISTORTION_POINT_ROTATE:
			if ((pmTransform == NULL) || (pDistortion->dh.u16Flags & DISTORTION_NO_TRANSFORM))
			{
				pDistortion->PointRotate.vTransformedPos = pDistortion->PointRotate.vPos;
				memcpy(&pDistortion->PointRotate.pTransformedMatrix->mMatrix, &pDistortion->PointRotate.pMatrix->mMatrix, sizeof(Graphics4x4));
			}
			else
			{
				pDistortion->PointRotate.vTransformedPos = ((Vector3)pDistortion->PointRotate.vPos) * *pmTransform;
				pDistortion->PointRotate.pTransformedMatrix->mMatrix.FastMultiply(pDistortion->PointRotate.pMatrix->mMatrix, *pmTransformRotationOnly);
			}
			break;
		default:
			ASSERT(FALSE);	// all cases should be handled
			break;
		}
	}
}

/* return the strength vector of the global wind distortion, if any */

Vector3PackedData* DistortionMgr__GetGlobalWindStrength()
{
	if (DistortionMgr.pGlobalWind)
		return(&DistortionMgr.pGlobalWind->vStrength);
	else
		return(NULL);
}
Vector3PackedData* DistortionMgr__GetGlobalWindBaseStrength()
{
	if (DistortionMgr.pGlobalWind)
		return(&DistortionMgr.pGlobalWind->vBaseStrength);
	else
		return(NULL);
}

/************************* distortion routines **************************/

void Distortion__Kill(ts_Distortion *pDistortion)
{
	/* some distortions have allocated other things */

	switch(pDistortion->dh.u8SubDataType)
	{
	case DISTORTION_POINT_ROTATE:
		if (pDistortion->PointRotate.pMatrix)
		{
			MatrixPool__Free(pDistortion->PointRotate.pMatrix);
			pDistortion->PointRotate.pMatrix = NULL;
		}
		if (pDistortion->PointRotate.pTransformedMatrix)
		{
			MatrixPool__Free(pDistortion->PointRotate.pTransformedMatrix);
			pDistortion->PointRotate.pTransformedMatrix = NULL;
		}
		break;
	}

//yuch-- notify owner

	/* clear record */

	pDistortion->Handle = INVALID_HANDLE;
	pDistortion->dh.u16Flags = 0;	// clear all flags

	/* move from active to free list */

	SLinkList__RemoveLink(&DistortionMgr.DistortionActive, pDistortion);
	SLinkList__AddLinkToHead(&DistortionMgr.DistortionFree, pDistortion);
}

void Distortion__Kill(t_Handle DistortionHandle)
{
	/* point to distortion, error if invalid id */

	ASSERT(DistortionHandle != INVALID_HANDLE);
	ts_Distortion *pDistortion = DISTORTION_GET_RECORD(DistortionHandle);
	ASSERT(pDistortion->Handle == DistortionHandle);			// error if id does not match. this means that record got deallocated (and maybe allocated again) somewhere else
	if (pDistortion->Handle != DistortionHandle)
		return;

	/* use other routine to actually kill it */

	Distortion__Kill(pDistortion);
}

void Distortion__Enable(t_Handle DistortionHandle, bool bEnable)
{
	/* point to distortion, error if invalid id */

	ASSERT(DistortionHandle != INVALID_HANDLE);
	ts_Distortion *pDistortion = DISTORTION_GET_RECORD(DistortionHandle);
	ASSERT(pDistortion->Handle == DistortionHandle);			// error if id does not match. this means that record got deallocated (and maybe allocated again) somewhere else
	if (pDistortion->Handle != DistortionHandle)
		return;

	if (bEnable)
		pDistortion->dh.u16Flags |= DISTORTION_ENABLED;
	else
		pDistortion->dh.u16Flags &= ~DISTORTION_ENABLED;
}

void Distortion__ToggleDistortion(void)
{
	if ( bShowDistortion )
		bShowDistortion = false;
	else
		bShowDistortion = true;
}


/* this should be generalized to a distortion emitter i think, somehow */

void WindGustEmitters__Add(Vector3 *pvPos, Vector3 *pvVel, Vector3 *pvStrength, float fRadius, float fDelay, float fLifetime, float fDeathtime, u8 u8Flags)
{

	Vector3	vZero;

	vZero.Clear( );

	DistortionMgr__AddWindGust(	pvPos,
											pvVel,
											&vZero,
											pvStrength,
											fRadius,
											fDelay,
											fLifetime,
											fDeathtime,
											0,
											u8Flags);
}


