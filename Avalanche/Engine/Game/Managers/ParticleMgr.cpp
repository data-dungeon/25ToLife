/*************************************/
/*                                   */
/*   ParticleMgr.cpp                 */
/*   big juju prototype  05/24/99    */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   particle manager                */
/*                                   */
/*************************************/

/* particles are basically temporary entities. they can be models or billboards.

a particle "effect" is composed of one or more "channels." each channel can create one or more particles, with
optional delays. the actual particle is a single model or animating image, with a bunch of behavior controls.
the particle data in the database is used to fill in the ts_Particle record, but does not match that format--
it may be variable length to save space, for example */

/*
** Particle Rendering
******************************************************************************
**
** Particles rendering occurs via a set of nested loops.  Like so:
**  foreach ParticleDataFile
**    ParticleMgr__PrepDrawList()
**    foreach group of particles that compare equal with Particle__Compare
**			maxsublistlen = ParticleMgr__PrepDrawSubList()
**			foreach particle in group up to a maximum of maxsublistlen
**				ParticleMgr__AddParticle()
**			next
**			ParticleMgr__FlushDrawSubList()
**		next
**		ParticleMgr__FlushDrawList()
**	next
** 
** If maxsublistlen is exceeded, we dont go on to the next group of particles,
** but ParticleMgr__FlushDrawSubList() and ParticleMgr__PrepDrawSubList() are
** called before continuing on with the group.  This allows the platforms to
** deal with running out of draw list space, and allows them to specify a
** dynamic amount of drawlist space (the GCN needs this because it uses the
** same buffer from sublist to sublist, and cant overwrite old particles until
** the pipeline has drained.
*/

/******************* includes ****************************/

#include "Game/GamePCH.h"

#include "Game/Managers/TextureMgr/PlatformImage.h"

#include "EngineHelper/timer.h"
#include "cmdcon/cmdcon.h"

#ifdef GCN
#include "Display/GCN/Shader.h"
#include "Display/GCN/FrameData.h"
#endif

/* this is bad style.. I need to check collisions with the water pool system, but its */
/* in GameLib.  So I am including a gamelib header here (which seems to happen a lot) */
/* and probably should instead be creating some generic way to collide particles with */
/* bits of GameLib... */
#include "Effects/Water/WaterMgr.h"
#include "Effects/ahfmgr.h"

/******************* compile control ******************************/

/* this is for debugging-- will print out effect and particle names as they create particles */

//#define PRINT_PARTICLE_CREATION
#ifndef _DEBUG
#undef PRINT_PARTICLE_CREATION
#endif //_DEBUG

#if !defined(_XBOX)
#define USE_PARTICULATE_MGR
#endif

/******************* defines *****************************/

/* flags (for ts_Particle.u32Flags) */

#define PARTICLE_NEEDS_ADVANCEMENT		0x00000001	// for AdvanceParticles use
#define PARTICLE_HAS_ROTATION				0x00000002
#define PARTICLE_ALIGN_TO_VELOCITY		0x00000004	// gets copied from source flags
#define PARTICLE_UPDATE_RED				0x00000008	// red color component needs update each frame
#define PARTICLE_UPDATE_GREEN				0x00000010	// green color component needs update each frame
#define PARTICLE_UPDATE_BLUE				0x00000020	// blue color component needs update each frame
#define PARTICLE_UPDATE_ALPHA				0x00000040	// alpha color component needs update each frame
#define PARTICLE_UPDATE_COLOR				(PARTICLE_UPDATE_RED | PARTICLE_UPDATE_GREEN | PARTICLE_UPDATE_BLUE | PARTICLE_UPDATE_ALPHA)
#define PARTICLE_UPDATE_X_SIZE			0x00000080	// x-size needs update each frame
#define PARTICLE_UPDATE_Y_SIZE			0x00000100	// y-size needs update each frame
#define PARTICLE_UPDATE_SIZE				(PARTICLE_UPDATE_X_SIZE | PARTICLE_UPDATE_Y_SIZE)
#define PARTICLE_NEEDS_TEXTURE_ANIM		0x00000200	// particle needs texture animation update
//#define PARTICLE_RENDERED					0x00000400	// particle has been rendered in current RenderMgr__Render call
#define PARTICLE_IN_VIEW_ANY_SCENE		0x00000800	// particle was in view in at least one scene
/* these correspond withe particle emitter flags of the same name (in ParticleDB.h), please leave them here */
#define PARTICLE_ONLY_SCENE_1				0x00001000	// only show this particle in scene 1
#define PARTICLE_ONLY_SCENE_2				0x00002000	// only show this particle in scene 2
#define PARTICLE_ONLY_SCENE_3				0x00004000	// only show this particle in scene 3
#define PARTICLE_ONLY_SCENE_4				0x00008000	// only show this particle in scene 4
/* end of corresponding flags */
#define PARTICLE_NO_MOVE					0x00010000	// don't apply velocity or acceleration to particle
#define PARTICLE_FROM_FLUX_EMITTER		0x00020000	// particle was created by a flux emitter, needs to check itself against flux volume
#define PARTICLE_LEASHED					0x00040000	// particle should die if gets too far from creation position
#define PARTICLE_FIRST_ADVANCE			0x00080000	// first time through Particle__Advance
#define PARTICLE_HAS_NORMAL				0x00100000	// particle has a normal to orient to (particle will face along the normal). Any rotations are local.
#define PARTICLE_WAS_ALIGN_TO_VELOCITY	0x00200000	// particle used to be align-to-velocity, but has stopped moving
#define PARTICLE_GROUND_SPECIAL_CHECK	0x00400000	// candidate for special ground-orient, align-to-velocity, no-gravity check
#define PARTICLE_SKIP_WATER_COLLIDE		0x00800000	// particle has a leash calculated to represent water collision
#define PARTICLE_FOLLOWS_PATH				0x01000000	// particle follows a spline path
#define PARTICLE_USE_OS_TIMER				0x02000000	// use os timer, ignore slowdown of game timer (but still respect pause)

/* which flags do we use to split rendering up? */

#define PARTICLE_STATE_FLAGS	(PARTICLE_LIGHTED | PARTICLE_ZBUFFER_WRITE | PARTICLE_ALPHA_REF | PARTICLE_ADDITIVE_RENDER)	// particles that get grouped must match state of first particle in group

/* water pool effect to trigger on particle collision */

#define PARTICLE_WATER_POOL_EFFECT	3

/* max number of suppression boxes */

#define MAX_SUPPRESSION_BOXES			5

/* ps2 colors are half scale */

#ifdef WIN32
#define MAX_COLOR_VALUE	255.0f
#elif defined(PS2)
#define MAX_COLOR_VALUE	128.0f
#elif defined(GCN)
#define MAX_COLOR_VALUE	255.0f
#endif //WIN32/PS2

/* size of data lists if not in master list mode (they get rendered in immediate mode, so they don't have to
be able to hold all the particles */

#if defined(GCN)
#define PARTICLE_LIST_SIZE		200	// will hold up to 200 particles
#elif defined(DIRECTX)
#define PARTICLE_LIST_SIZE		200	// will hold up to 200 particles
#elif defined(PS2)
#define PARTICLE_BUFFER_HEAD 5
#define PARTICLE_OUTPUT_SIZE 3
#define PARTICLE_MAX_CLIPPED_VERTS 15
#define PARTICLE_BUFFER_OVERHEAD_QWC	(7)
#define PARTICLE_OUTPUT_BUFFER (PARTICLE_OUTPUT_SIZE * PARTICLE_MAX_CLIPPED_VERTS + PARTICLE_BUFFER_OVERHEAD_QWC)
#define PARTICLE_OUTPUT_BUFFERS (PARTICLE_OUTPUT_BUFFER * 2)
#define PARTICLE_CLIP_SIZE 3
#define PARTICLE_CLIP_BUFFER (PARTICLE_CLIP_SIZE * PARTICLE_MAX_CLIPPED_VERTS)
#define PARTICLE_CLIP_BUFFERS (PARTICLE_CLIP_BUFFER * 2)
#define PARTICLE_STATIC_BUFFERS (PARTICLE_OUTPUT_BUFFERS + PARTICLE_CLIP_BUFFERS)
#define PARTICLE_INPUT_SIZE 2
#define PARTICLE_CLIPPER_STACK (5)
#define PARTICLE_VU_SIZE (PARTICLE_INPUT_SIZE)
#define INIT_QWC 46
#define VU_BASE  (INIT_QWC + PARTICLE_CLIPPER_STACK)
#define VU_OFFSET ((1024 - VU_BASE) / 2)
#define PARTICLE_LIST_SIZE		((MIN((VU_OFFSET - PARTICLE_STATIC_BUFFERS) / PARTICLE_VU_SIZE, (255 - PARTICLE_BUFFER_HEAD)/2)) - 20)
#define VU_ITOP (PARTICLE_LIST_SIZE*PARTICLE_INPUT_SIZE)
#endif //PS2

#define MIN(a,b) (((a) > (b)) ? (b) : (a))

/* reserved group id's-- group id's in this range can't be used unless they are requested through */
/* ParticleMgr__RequestEmitterGroupID */

#define EMITTER_RESERVED_GROUP_MIN	0x9000
#define EMITTER_RESERVED_GROUP_MAX	0xa000

/******************* macros ******************************/

#define PARTICLE_GET_RECORD(Handle)				&ParticleMgr.pParticleAlloc[HANDLE_INDEX(Handle)]
#define PARTICLE_EMITTER_GET_RECORD(Handle)	&ParticleMgr.pParticleEmitterAlloc[HANDLE_INDEX(Handle)]

/* for keeping rotation in -270 to 90 range for vu processing. this helps with the sin instruction, which */
/* needs angle in -PI/2 to PI/2 range */

#define CHECK_PARTICLE_ROTATION(vect, axis) {																	\
	if ((vect).axis() < -Math::TwoPi * 0.75f)																		\
		(vect).axis((vect).axis() + Math::TwoPi);																	\
	else if ((vect).axis() > Math::TwoPi * 0.25f)																\
		(vect).axis((vect).axis() - Math::TwoPi);																	\
	ASSERT(((vect).axis() >= -Math::TwoPi * 0.75f) && ((vect).axis() <= Math::TwoPi * 0.25f));	\
}
#define STRONG_CHECK_PARTICLE_ROTATION(vect, axis)	{															\
	if ((vect).axis() < -Math::TwoPi * 0.75f) 																	\
	{ 																															\
		while((vect).axis() < -Math::TwoPi * 0.75f) 																\
			(vect).axis((vect).axis() + Math::TwoPi); 															\
	} else 																													\
		while((vect).axis() > Math::TwoPi * 0.25f) 																\
			(vect).axis((vect).axis() - Math::TwoPi); 															\
	ASSERT(((vect).axis() >= -Math::TwoPi * 0.75f) && ((vect).axis() <= Math::TwoPi * 0.25f));	\
}

/* set up for binary name search. result should be first power of two that is greater than array count */

#define SET_POWER_OF_2(pArray, pValue)		\
														\
	if (pArray->Count() == 0)					\
		pValue = 0;									\
	else												\
	{													\
		pValue = 1;									\
		while (pValue <= pArray->Count())	\
			pValue *= 2;							\
	}

/* test binary search-- all entries should be found */

#ifdef _DEBUG
#define TEST_NAME_SEARCH(pArray, nPowerOfTwo)										\
	{																								\
		for (int nTest = 0; nTest < pArray->Count(); nTest++)						\
		{																							\
			int nTestDataIndex;																\
			int nTestRecordIndex;															\
			ASSERT(ParticleMgr__FindRecordByName(pArray->Get(nTest)->pName,	\
															 nTestDataIndex,					\
															 nTestRecordIndex,				\
															 pArray,								\
															 nPowerOfTwo));					\
		}																							\
	}
#endif //_DEBUG

/******************* structures **************************/

struct sPlane
{
	Vector3 n;	// normal
	Vector3 p;	// point on plane
};

/* for binary search by name of possibly multiple particle data files. as particle files are loaded,
we add more of these records to the name search dynamic arrays, one per particle/effect/emitter in the
data file */

struct ts_ParticleNameSearch
{
	char *pName;			// pointer into name list chunk
	u16 u16DataIndex;		// index of particle data file (equivalent to nDataIndex)
	u16 u16EntryIndex;	// particle, effect, or emitter index within list for specified particle data file
};

/* particle emitter */

struct ts_Particle;		// forward declaration for ts_ParticleEmitter

struct ts_ParticleEmitter
{
	t_Handle						Handle;					//	assigned handle for access, invalid handle checking
	u32							u32Flags;				// flag values are in ParticleDB.h
	int							nDataIndex;				// index of possibly-multiply-loaded data set that emitter takes data from
	u16							u16EffectID;			// particle effect to emit
	const Vector3Packed		*pvPos;					// place to create effects (world coordinates). emitter can move around (be tied to an actor, for example)
	Vector3Packed				vStationaryPos;		// copy stationary emitter's position into here (passed pointer to position may be transient)
	Vector3						vLastFluxEmitterPos;	// flux emitters can't be stationary, so ok to use this space for tracking flux emitter's last position // TJC - unions suck... let just waste a bit of space instead...
	Vector3						vLastPos;				// emitter position after last advance-- for interframe interpolation
	Vector3						vLastEmitPos;			// last position effects were created-- for min distance check
	float							fLastEmitTime;			// fTimer value when last effect was emitted, for min distance check
	const Vector3Packed		*pvEmitterVel;			// velocity at which emitter moves (can be NULL)
	const Vector3Packed		*pvEjectVel;			// velocity at which to create effects (can be NULL, in which case effect will use source data velocity)
	Vector3Packed				vVelUnitVector;		// pre-calculated if emitter velocity does not change
	float							fVelMagnitude;			// pre-calculated if emitter velocity does not change
	float							fVelMult;				// can relate velocity of effect to velocity of emitter (operates on pvEmitterVel)
	float							fAdditiveVelMult;		// can use velocity as additive too (operates on pvEmitterVel)
	float							fTimer;					// time until next emit
	float							fTimerInc;				// for resetting fTimer
	float							fMinDistance;			// minimum distance between particles emitted-- will emit more than one effect per frame if necessary to satisfy this
	float							fRadius;					// radius for orbiting emitter
	float							fRadians;				// current angle for orbiting emitter
	float							fAngularVelRadians;	// angular velocity for orbiting emitter, in radians/second
	float							fGoRadius;				// activation distance-- bounding sphere is tested against scene view frustum(s)
	float							fLOD1Radius;			// near LOD distance-- emit at 100% of emitter rate when closer
	float							fLOD2Radius;			// far LOD distance-- emit at 0% of emitter rate when farther
	float							fLifetime;				// remaining lifetime, 0.0 means forever
	float							fScale;					// scale to apply to particles (default is 1.0, of course)
	int							nScaleRandomPct;		// for continuously random emitters (scale), 0 if no randomness desired
	float							fConstantAlpha;		// particles that emitter creates will use this as a multiplier against alpha envelope
	float							*pfConstantAlpha;		// typically points to record's fConstantAlpha, but can point to an external float
	float							fColorMult;				// particles that emitter creates will use this as a multiplier against color envelope
	float							*pfColorMult;			// typically points to record's fColorMult, but can point to an external float
	float							fDelay;					// original time between effects for LOD calculation
	int							nDelayRandomPct;		// for continuously random emitters (delay), 0 if no randomness desired
	const Vector3Packed		*pvUp;					// up vector for orbiting emitter
	ts_ParticleEmitter		*pNext;					// pointer to next record in linked list (active or free list)
	u16							u16Group;				// group id, for easy killing of a group of emitters
	Graphics4x4					*pVelTransform;		// transform calculated velocity vector, provided to align emitters to instances
	Vector3Packed				*pvTarget;				// target (typically moving) toward which particles created by this emitter will move
	CPlaneList*					pPlaneList;			// for restricting created particles to a defined area. typically NULL
	float							fFluxVolumeHalfSize;	// half-size of flux volume (a cube) for flux emitter
	float							fFluxCeiling;			// in world coordinates, typically used as water level
	ts_BoundingBox				FluxVolume;				// needed to old flux particles to check if they are out of the volume
	Vector3						vFluxFraction;			// fractional effects for x/y/z directions
	ts_Particle					*pParentParticle;		// for emitters that piggyback on a particle
	float							fInvisDistance;		// EMITTER_DISTANT: distance at which emitter particles start to fade in, using fNearScale
	float							fNearDistance;			// EMITTER_DISTANT: distance at which fade in is complete, still using fNearScale
	float							fNearScale;				// EMITTER_DISTANT: scale to use when distance from camera <= fNearDistance
	float							fFarDistance;			// EMITTER_DISTANT: interpolate from fNearScale to fFarScale between fNearDistance and fFarDistance
	float							fFarScale;				// EMITTER_DISTANT: scale to use when distance from camera >= fFarDistance
	SplinePath					*pPath;					// for allowing created particles to follow a spline path
#ifdef PRINT_PARTICLE_CREATION
	char							*pName;					// for debugging
#endif //PRINT_PARTICLE_CREATION
};

/* general particle stucture */

struct ts_Particle
{
	t_Handle						Handle;						//	assigned handle for access, invalid handle checking
	u32							u32Flags;

	/* index of possibly-multiply-loaded data set that particle takes data from */

	int							nDataIndex;

	/* pointer to source data */

	ts_ParticleSource			*pSource;

	/* data unique to this particle */

	Vector3Packed				vPos;							// world location
	Vector3Packed				vVel;							// current velocity
	float							fLifetime;					// remaining lifetime, 0.0 means forever
	float							fOneOverOrigLifetime;	// original lifetime value, for envelope evaluation
	float							fActivationDelay;			// delay before activation
	float							fInitVelMagnitude;		// magnitude of initial velocity
	Vector3Packed				vRotRadians;				// current rotation, in radians, always in -270 to 90 range (for vu particle processing)
	Vector3Packed				vRotVelRadians;			// current rotation velocity, in radians/second
	float							fFirstAdvanceTime;		// value to use for particle's first advance
	float							fScale;						// scale that was applied to particle, for spawning
	float							fConstantAlpha;			// overall alpha to multiply alpha envelope against
	float							fColorMult;					// overall value to multiply color envelope against
	Vector3Packed				*pvTarget;					// target (typically moving) toward which particle will move
	Vector3Packed				vNormal;						// current normal. y-axis of particle will align with this normal

	/* overlay some mutually exclusive (and rarely used) things for space savings */

	union
	{
		float						fLeashSquared;				// particle dies if it gets too far from creation origin, 0.0 means no leash
		ts_ParticleEmitter	*pFluxEmitter;				// pointer back to flux emitter for flux particles
		SplinePath				*pPath;						// path for particle to follow
	};
	Vector3Packed				vCreationPos;				// world location where particle was created, for leash/flux check
#define vPathDisplace vCreationPos						// can't union two Vector3Packed, so do it this way
	float							fPathDistance;				// distance along path, if any

	/* orbiting particles orbit around vPos at a radius and speed. to save a bit of space, we calculate particle's vPos */
	/* normally, then calculate orbit location just before rendering particle so we don't need another Vector3. there are */
	/* also a couple of flags in particle source data */

	float							fOrbitRadius;				// radius of orbit in radians
	float							fOrbitRadiusVel;			// rate of change of radius (meters/second)
	float							fOrbitRotRadians;			// current rotation angle in radians
	float							fOrbitRotVelRadians;		// orbit rotation speed in radians/second

	/* values controlled by envelopes */

	Vector2						vSize;
	Vector2						vSize100;					// size at 100% for envelope control
	ts_bRGBA						Color;

	/* texture animation */

	u16							u16CurrentIndex;
	s8								s8IndexInc;					// for animation type support (loop, ping-pong, one-shot)
	float							fAnimTimer;					// time until switching to next frame of animation
	float							fAnimDelay;					// anim delay, in seconds-- fAnimDelay == 1.0f / source FPS

	/* handle of piggyback emitter, if any-- needs to be killed when particle dies */

	t_Handle						PiggybackEmitterHandle;

	/* callback to call when particle dies */

	t_ParticleDeathCallback	DeathCallback;
	void							*callbackContext;			// the context of the callback

	/* list pointers */

	ts_Particle					*pPrev;						// pointer to prev record in linked list
	ts_Particle					*pNext;						// pointer to next record in linked list
};

/* a bare-bones record for recording a request to create a particle effect. for cache purposes,
particles are actually created at the beginning of ParticleMgr__Advance */

struct ts_ParticleCreate
{
	ts_ParticleCreate			*pNext;					// pointer to pNext field of next record in linked list
	int							nDataIndex;				// which of possibly-multiply-loaded particle data files to use
	int							nEffectID;				// effect to create
	Vector3						vPos;						// passed position, if any
	Vector3						*pvPos;
	Vector3						vVelUnitVector;		// passed velocity direction, if any
	Vector3						*pvVelUnitVector;
	float							fVelMagnitude;			// passed velocity magnitude, if any
	float							*pfVelMagnitude;
	Vector3						vAdditiveVel;			// passed velocity to add to particle after creation process, if any
	Vector3						*pvAdditiveVel;
	float							fFirstAdvanceTime;	// initial value to use for particle's first advance
	float							fScale;					// scale to apply to particle size
	int							iSceneIndex;			// which scene to draw particle in. -1 = all scenes
	float							fConstantAlpha;		// overall alpha multiplier to use on particle
	float							fColorMult;				// overall color multiplier to use on particle
	u32							u32ForceFlags;			// forced bits for ts_Particle.u32Flags
	Graphics4x4					*pVelTransform;		// transform calculated velocity vector, provided to align emitters to instances
	Vector3Packed				*pvTarget;				// target (typically moving) toward which particles created by this effect will move
	CPlaneList					*pPlaneList;			// for restricting created particles to a defined area. typically NULL
	ts_ParticleEmitter		*pFluxEmitter;			// pointer back to flux emitter for flux particles
	SplinePath					*pPath;					// path for particle to follow, typically NULL
};

/* particle manager */

struct ts_ParticleMgr
{
	/* record passed initialization values */

	int						nMaxParticles;
	int						nMaxParticleEmitters;
	int						nMaxParticleCreateRecords;

	/* lists */

	ts_Particle				*pParticleAlloc;			// pointer to malloc'd records
	ts_DLinkList			FreeList;					// linked list of free records

	/* we need an active list per particle data file since the texture set for each may get downloaded as a dynamic */
	/* texture set */

	ts_DLinkList			ActiveList[MAX_PARTICLE_DATA_FILES];				// linked list of active records

	/* i DON'T think we need a pending list per particle data file, since that particle records the data index */
	/* and so can be put into the correct active list. however, this is a bit ingrained here, and not much penalty */

	ts_DLinkList			PendingList[MAX_PARTICLE_DATA_FILES];				// linked list of pending records-- created with a pre-life delay

	/* this is particle source data */

	ts_RecordList			*pEffectList[MAX_PARTICLE_DATA_FILES];				// particle effects data
	ts_RecordList			*pParticleSourceList[MAX_PARTICLE_DATA_FILES];	// particle source data
	ts_RecordList			*pEmitterList[MAX_PARTICLE_DATA_FILES];			// particle emitter data

	/* swoosh source data is in particle .dbl file, so we track it here and report it to swoosh manager */

	ts_RecordList			*pSwooshSourceList[MAX_PARTICLE_DATA_FILES];		// swoosh source data
	ts_RecordList			*pSwooshEmitterList[MAX_PARTICLE_DATA_FILES];	// swoosh emitter source data

	/* emitter lists */

	ts_ParticleEmitter	*pParticleEmitterAlloc;	// pointer to malloc'd records
	ts_SLinkList			EmitterFreeList;			// linked list of free records
	ts_SLinkList			EmitterActiveList;		// linked list of active records

	/* linked list for particle creation. for cache purposes, when ParticleEffect__Create is called, */
	/* just the bare minumum information is recorded. then, at the next call to ParticleMgr__Advance, */
	/* the particles are actually created */

	ts_ParticleCreate		*pParticleCreateAlloc;	// pointer to malloc'd records
	ts_SLinkList			CreateFreeList;			// linked list of free records
	ts_SLinkList			CreatePendingList;		// particle effects to create at next ParticleMgr__Advance

	/* list for advancing particles, to make sure that alloc/dealloc doesn't screw things up */

	ts_Particle				**ppAdvanceList;
	int						nAdvanceListSize;

	/* for handles */

	u16						u16ID;

	/* lists for rendering via DrawArrays */
#ifndef PS2
	Vector3Packed			*pVertexList;
	Vector2					*pUVList;
	ts_bRGBA					*pColorList;
#else // PS2
	u_char					*pInitPos;
	u_char					*pStartPos;
	u_char				   *pCurrPos;
	u32						nOffset;
	u32						nQWC;
#endif // PS2

	/* particle db handle, texture set */

	t_Handle					DBHandle[MAX_PARTICLE_DATA_FILES];
	TextureArray			*pTextureArray[MAX_PARTICLE_DATA_FILES];

	/* routine to call for collision test */

	t_CollisionTest		CollisionTest;

	/* reserved group id's */

	bool						bEmitterGroupIDWrapped;
	u16						u16EmitterGroupID;

	/* active or not */

	bool						bEnable;
	bool						bLocked;

	/* paused? */

	bool						bPaused;

	/* time advance */

	float						fFrameSec;
	float						fOSFrameSec;

	/* additional extension to searched names (usually NULL) */
	const char* pOptionalNameExtension;

	/* search arrays that enable a binary search by name of particles/effects/emitters/swooshes */

	DynamicArray<ts_ParticleNameSearch>	*pParticleNameSearch;
	DynamicArray<ts_ParticleNameSearch>	*pEffectNameSearch;
	DynamicArray<ts_ParticleNameSearch>	*pEmitterNameSearch;
	DynamicArray<ts_ParticleNameSearch>	*pSwooshNameSearch;
	DynamicArray<ts_ParticleNameSearch>	*pSwooshEmitterNameSearch;
	int nParticleSearchPowerOfTwo;
	int nEffectSearchPowerOfTwo;
	int nEmitterSearchPowerOfTwo;
	int nSwooshSearchPowerOfTwo;
	int nSwooshEmitterSearchPowerOfTwo;

	/* lighting adjustment, via ParticleMgr__AdjustLightedParticles */

	bool bAdjustLights;
	float fAmbientMult;
	float fDirectionalMult;
	float fLeadInSeconds;	// gets set to passed fLeadInSeconds
	float fSustainSeconds;	// gets set to passed fLeadInSeconds + fCurrentSeconds
	float fLeadOutSeconds;	// gets set to passed fLeadInSeconds + fCurrentSeconds + fLeadOutSeconds
	float fCurrentSeconds;

	/* one-shots for creating emitters/effects/owned particles. see comments in ParticleMgr.h about one-shots */

	Graphics4x4 *pEmitterVelTransform;
	Vector3Packed *pvEmitterTarget;
	Graphics4x4 *pEffectVelTransform;
	Vector3Packed *pvEffectTarget;
	Graphics4x4 *pOwnedParticleVelTransform;
	Vector3Packed *pvOwnedParticleTarget;

	/* projection matrix for implementing z-bias */

	Matrix4x4 mZBiasProjection;						// projection matrix to diddle with
	Vector3 vZBiasProjectionRow3;						// row 3 to poke
	float fZBiasZShift;									// this is the original z-buffer shift value-- essentially a translation
	float fZBiasFarClipAdjust;							// per-scene adjustment

	/* fog state at render time-- updated with each scene rendered */

	bool bFogEnabled;
	float fNearFog;
	float fFarFog;
	float fOneOverFogDelta;

	/* particulate system for faster particles that follow stricter rules */
	ParticulateMgr::System* pParticulateSystem;

	/* for debugging */

#ifndef CONSUMER_BUILD
	u32						u32ActiveCount;			// how many active particles there are
	u32						u32RenderedCount;			// how many particles actually got rendered in one scene
	bool						bInAdvance;					// true while we are in ParticleMgr__Advance
#endif //CONSUMER_BUILD
};

/******************* externs *****************************/

/******************* global variables ********************/
// If this value is NOT null, limited particles (particles limited to one scene or another)
// will not be drawn when in this box
CActor		*pSuppressionBox[MAX_SUPPRESSION_BOXES];

/******************* local variables *********************/

/* particle system */

static ts_ParticleMgr ParticleMgr;

static FastRandom ParticleRandom(0);	// particle manager has its own random number sequence

/******************* local prototypes ********************/

// platform specific pieces of code
#ifdef PS2
void ParticleMgr__PS2PrepRender(void);
void ParticleMgr__PS2PrepDrawList();
void ParticleMgr__PS2FinishDrawList(u32 nDataIndex);
void ParticleMgr__PS2SendDrawList(TextureDatabase* pDatabase, u32 nParticleCount, u16 u16Index, float fZBias);
void ParticleMgr__StartDrawList();
void ParticleMgr__EndDrawList();
#endif

#ifdef GCN
void GCNParticle__SetupTEV( u16 u16Bind);
void GCNParticle__Render( int first, int count);
#endif
u32 ParticleMgr__AddParticle(ts_Particle * pParticle, u32 &nParticleCount);

void ParticleMgr__TransformParticle(ts_Particle* pParticle, Vector3 &vWorldCenter, Vector3 vParticleWorldVertex[4], Vector3& vNormal, u32 u32State);

void ParticleMgr__AddSearchNames(t_Handle Handle, int nDataIndex, int nChunkType, DynamicArray<ts_ParticleNameSearch> *pArray);
void ParticleMgr__RemoveSearchNames(int nDataIndex, DynamicArray<ts_ParticleNameSearch> *pArray);
bool ParticleMgr__FindRecordByName(const char *pName, int &nDataIndex, int &nRecordIndex, DynamicArray<ts_ParticleNameSearch> *pArray, int nPowerOfTwo);
bool ParticleMgr__SearchForRecordByName(const char *pName, int &nDataIndex, int &nRecordIndex, DynamicArray<ts_ParticleNameSearch> *pArray, int nPowerOfTwo);
ts_Particle *ParticleMgr__AllocParticle(int nDataIndex);
ts_ParticleEmitter *ParticleMgr__AllocParticleEmitter(u16 u16Group);
void ParticleMgr__CreatePendingEffects(void);
void ParticleMgr__CreateEffect(
	const int nDataIndex,
	const int nEffectID,
	const Vector3Packed *pvPos,
	const Vector3Packed *pvVelUnitVector,
	const float *pfVelMagnitude,
	const Vector3Packed *pvAdditiveVel,
	const float fFirstAdvanceTime,
	const float fScale = 1.0f,
	const int nScaleRandomPct = 0,
	const float fConstantAlpha = 1.0f,
	const float fColorMult = 1.0f,
	const u32 u32ForceFlags = 0,
	const int iSceneIndex = -1,
	const CPlaneList *pPlaneList = NULL,
	const SplinePath *pPath = NULL,
	const ts_ParticleEmitter *pFluxEmitter = NULL);
void ParticleMgr__AdvanceEmitters(void);
void ParticleMgr__RenderList(int nDataIndex, int nSceneIndex);
bool ParticleMgr__EmitterGroupUsed(u16 u16Group);
bool IsInside(const DirCos3x3& i_mBoxOrientation, Vector3CRef  i_vBoxPosition, Vector3CRef  i_vPoint);

/*** particle routines ***/

t_Handle Particle__Create(
	const int nDataIndex,
	const Vector3Packed *pvPos,
	const Vector3Packed *pvVelUnitVector,
	const float *pfVelMagnitude,
	const Vector3Packed *pvAdditiveVel,
	const u32 u32ForceFlags,
	const float fFirstAdvanceTime,
	const float fScale,
	const float fConstantAlpha,
	const float fColorMult,
	const float fActivationDelay,
	const ts_ParticleSource *pSource,
	const int iSceneIndex,
	const Graphics4x4 *pVelTransform,
	const Vector3Packed *pvTarget,
	const CPlaneList *pPlaneList,
	const SplinePath *pPath,
	const ts_ParticleEmitter *pFluxEmitter);
void Particle__Advance(ts_Particle *pParticle, u32 u32DistortionsToApply);
void Particle__Kill(ts_Particle *pParticle);
Vector3 Particle__ReflectVector(Vector3CRef Vector, Vector3CRef Normal);
void Particle__ReflectUnitVector(Vector3Packed &UnitVector, Vector3CRef Normal);
void Particle__SpawnEffect(ts_Particle *pParticle, int nIndex, Vector3Packed *pvPos = NULL, Vector3 *pvCollisionNormal = NULL);
float Particle__EvaluateEnvelope(ts_Particle *pParticle, int nIndex);
float Particle__fRandomValue(float fValue, int nRandomPctValue);
int Particle__nRandomValue(int nValue, int nRandomPctValue);
void Particle__InterpolateColor(ts_Particle *pParticle, float fValue);
Vector3Packed *Particle__CalculateOrbit(ts_Particle *pParticle);
void Particle__Resort(ts_Particle* pParticle);
void Particle__SortIntoActive(ts_Particle* pParticle, int nDataIndex);
int Particle__Compare(ts_Particle* pLHS, ts_Particle* pRHS);
int ParticleMgr__PrepDrawSubList( ts_ParticleSource* pParticleSource );
void ParticleMgr__PrepDrawList( void );
void ParticleMgr__FlushDrawSubList( ts_Particle* pFirstParticle, u32 nParticleCount );
void ParticleMgr__FlushDrawList( int nDataIndex );
void Particle__RemoveAsGuess(ts_Particle* pParticle);
bool Particle__IsCulled(ts_Particle* pParticle, int nDataIndex, int nSceneIndex);

void Particle__Create_Print(const int nDataIndex, const ts_ParticleSource* pSource);
static Vector3 Particle__Create_GeneratePosition(const ts_ParticleSource *pSource,
																 const Vector3Packed *pvPos,
																 const Vector3Packed *pvVelUnitVector);

/*** particle emitter routines ***/

t_Handle ParticleEmitter__CreateFromRecord(
	int nDataIndex,
	int nEmitterID,
	const Vector3Packed *pvEmitterPos,
	const Vector3Packed *pvEmitterVel,
	const Vector3Packed *pvEjectVel,
	bool bEjectVelChanges,
	const Vector3Packed *pvUp,
	bool bStationary,
	u16 u16Group = EMITTER_DEFAULT_GROUP,
	int iSceneIndex = -1);
t_Handle ParticleEmitter__Create(
	int nDataIndex,
	u32 u32Type,
	int nEffectID,
	const Vector3Packed *pvEmitterPos,
	const Vector3Packed *pvEmitterVel,
	float fEmitterVelMult,
	float fAdditiveEmitterVelMult,
	const Vector3Packed *pvEjectVel,
	bool bEjectVelChanges,
	float fDelay,
	int nDelayRandomPct,
	float fScale,
	int nScaleRandomPct,
	float fMinDistance,
	float fGoDistance,
	bool bStationary,
	float fLifetime = 0.0f,
	u16 u16Group = EMITTER_DEFAULT_GROUP,
	int iSceneIndex = -1,
	float fLOD1Radius = MAX_E_LOD_RADIUS,
	float fLOD2Radius = MAX_E_LOD_RADIUS,
	float fFluxVolumeHalfSize = 0.0f,
	float fInvisDistance = 0.0f,
	float fNearDistance = 0.0f,
	float fNearScale = 1.0f,
	float fFarDistance = 0.0f,
	float fFarScale = 1.0f);
void ParticleEmitter__MakeReady(ts_ParticleEmitter *pEmitter);
void ParticleEmitter__Advance(ts_ParticleEmitter *pEmitter);

/*** flux emitter routines ***/

void FluxEmitter__Advance(ts_ParticleEmitter *pEmitter, int iSceneIndex);
float FluxEmitter__FillVolume(ts_ParticleEmitter *pEmitter, int iSceneIndex, ts_BoundingBox &FluxVolume, float fFraction);
void FluxEmitter__UpdateDensity(ts_ParticleEmitter *pEmitter, float fDelay);

/*** particle effect routines ***/

bool ParticleEffect__Create(ts_ParticleCreate *pParticleCreate);

/******************* functions ***************************/

/* initialize particle system */

int ParticleMgr__Init(int nMaxParticles, int nMaxParticleEmitters, int nMaxParticleCreateRecords)
{
	int i;
	ts_Particle *pParticle;
	ts_ParticleEmitter *pEmitter;

	/* clear the manager */

	memset(&ParticleMgr, 0, sizeof(ts_ParticleMgr));

	/* no suppression box */
	for (int iTmp=0; iTmp < MAX_SUPPRESSION_BOXES; iTmp++)
		pSuppressionBox[iTmp] = NULL;

	/* no db handle or texture set */

	for (i = 0; i < MAX_PARTICLE_DATA_FILES; i++)
	{
		ParticleMgr.DBHandle[i] = INVALID_HANDLE;
		ParticleMgr.pTextureArray[i] = NULL;
	}

	/* reserved group id's */

	ParticleMgr.u16EmitterGroupID = EMITTER_RESERVED_GROUP_MIN;
	ParticleMgr.bEmitterGroupIDWrapped = FALSE;

	/* empty name search arrays */

	MEM_SET_ONE_SHOT_NEW_NAME("PartMgr NameSearch");
	ParticleMgr.pParticleNameSearch = new DynamicArray<ts_ParticleNameSearch>(0);
	MEM_SET_ONE_SHOT_NEW_NAME("PartMgr NameSearch");
	ParticleMgr.pEffectNameSearch = new DynamicArray<ts_ParticleNameSearch>(0);
	MEM_SET_ONE_SHOT_NEW_NAME("PartMgr NameSearch");
	ParticleMgr.pEmitterNameSearch = new DynamicArray<ts_ParticleNameSearch>(0);
	MEM_SET_ONE_SHOT_NEW_NAME("PartMgr NameSearch");
	ParticleMgr.pSwooshNameSearch = new DynamicArray<ts_ParticleNameSearch>(0);
	MEM_SET_ONE_SHOT_NEW_NAME("PartMgr NameSearch");
	ParticleMgr.pSwooshEmitterNameSearch = new DynamicArray<ts_ParticleNameSearch>(0);
	ParticleMgr.nParticleSearchPowerOfTwo = 0;
	ParticleMgr.nEffectSearchPowerOfTwo = 0;
	ParticleMgr.nEmitterSearchPowerOfTwo = 0;
	ParticleMgr.nSwooshSearchPowerOfTwo = 0;
	ParticleMgr.nSwooshEmitterSearchPowerOfTwo = 0;

	/* done if no particles */

	if (nMaxParticles == 0)
		return(TRUE);

	/* allocate records, initialize linked lists */

	ASSERT(nMaxParticles < MAX_HANDLES);	// see definition of MAX_HANDLES
	ParticleMgr.nMaxParticles = nMaxParticles;
	MEM_SET_ALLOC_NAME("PartMgr list");
	ParticleMgr.pParticleAlloc = (ts_Particle *) memAlloc(ParticleMgr.nMaxParticles * sizeof(ts_Particle));
	DLinkList__Init(&ParticleMgr.FreeList, offsetof(ts_Particle, pNext), offsetof(ts_Particle, pPrev));
	DLinkList__LinkBlock(&ParticleMgr.FreeList, ParticleMgr.pParticleAlloc, ParticleMgr.nMaxParticles, sizeof(ts_Particle));
	for (int nDataIndex = 0; nDataIndex < MAX_PARTICLE_DATA_FILES; nDataIndex++)
	{
		DLinkList__Init(&ParticleMgr.ActiveList[nDataIndex], offsetof(ts_Particle, pNext), offsetof(ts_Particle, pPrev));
		DLinkList__Init(&ParticleMgr.PendingList[nDataIndex], offsetof(ts_Particle, pNext), offsetof(ts_Particle, pPrev));
	}

	/* allocate emitter lists */

	ASSERT(nMaxParticleEmitters < MAX_HANDLES);	// see definition of MAX_HANDLES
	ParticleMgr.nMaxParticleEmitters = nMaxParticleEmitters;
	MEM_SET_ALLOC_NAME("PartMgr emitter");
	ParticleMgr.pParticleEmitterAlloc = (ts_ParticleEmitter *) memAlloc(ParticleMgr.nMaxParticleEmitters * sizeof(ts_ParticleEmitter));
	SLinkList__Init(&ParticleMgr.EmitterFreeList, offsetof(ts_ParticleEmitter, pNext));
	SLinkList__LinkBlock(&ParticleMgr.EmitterFreeList, ParticleMgr.pParticleEmitterAlloc, ParticleMgr.nMaxParticleEmitters, sizeof(ts_ParticleEmitter));
	SLinkList__Init(&ParticleMgr.EmitterActiveList, offsetof(ts_ParticleEmitter, pNext));

	/* allocate creation lists */

	ASSERT(nMaxParticleCreateRecords < MAX_HANDLES);	// see definition of MAX_HANDLES
	ParticleMgr.nMaxParticleCreateRecords = nMaxParticleCreateRecords;
	MEM_SET_ALLOC_NAME("PartMgr create");
	ParticleMgr.pParticleCreateAlloc = (ts_ParticleCreate *) memAlloc(ParticleMgr.nMaxParticleCreateRecords * sizeof(ts_ParticleCreate));
	SLinkList__Init(&ParticleMgr.CreateFreeList, offsetof(ts_ParticleCreate, pNext));
	SLinkList__LinkBlock(&ParticleMgr.CreateFreeList, ParticleMgr.pParticleCreateAlloc, ParticleMgr.nMaxParticleCreateRecords, sizeof(ts_ParticleCreate));
	SLinkList__Init(&ParticleMgr.CreatePendingList, offsetof(ts_ParticleCreate, pNext));

	/* allocate advance list */

	MEM_SET_ALLOC_NAME("PartMgr advance");
	ParticleMgr.ppAdvanceList = (ts_Particle **) memAlloc(ParticleMgr.nMaxParticles * sizeof(ts_Particle *));

	/* initialize records */

	pParticle = ParticleMgr.pParticleAlloc;
	for (i = 0; i < ParticleMgr.nMaxParticles; i++, pParticle++)
	{
		pParticle->u32Flags = 0;
		pParticle->Handle = INVALID_HANDLE;
	}
	pEmitter = ParticleMgr.pParticleEmitterAlloc;
	for (i = 0; i < ParticleMgr.nMaxParticleEmitters; i++, pEmitter++)
	{
		pEmitter->u32Flags = 0;
		pEmitter->Handle = INVALID_HANDLE;
	}

#if !defined(GCN)  && !defined(PS2)
	MEM_SET_ALLOC_NAME("PartMgr vertex");
	ParticleMgr.pVertexList = (Vector3 *) memAlloc(PARTICLE_LIST_SIZE * 4 * sizeof(Vector3));	// each particle contributes 4 vertices
	MEM_SET_ALLOC_NAME("PartMgr UV");
	ParticleMgr.pUVList = (Vector2 *) memAlloc(PARTICLE_LIST_SIZE * 4 * sizeof(Vector2));		// each particle contributes 4 vertices
	MEM_SET_ALLOC_NAME("PartMgr color");
	ParticleMgr.pColorList = (ts_bRGBA *) memAlloc(PARTICLE_LIST_SIZE * 4 * sizeof(ts_bRGBA));	// each particle contributes 4 vertices
#endif // !GCN && !PS2

	/* create a new particulate system */
	ParticleMgr.pParticulateSystem = new ParticulateMgr::System();

	/* we are active, unlocked, unpaused */

	ParticleMgr.bEnable = true;
	ParticleMgr.bLocked = false;
	ParticleMgr.bPaused = false;

	/* success */

	return(TRUE);
}

/* reset. this unloads loaded particle data files */

void ParticleMgr__Reset(void)
{
	int i;

	/* kill all particles and emitters */

	ParticleMgr__KillAllParticles();
	ParticleMgr__KillAllEmitters();
	ASSERT(ParticleMgr__IsEmpty());

	/* clear collision test */

	ParticleMgr.CollisionTest = NULL;

	/* we are active, unlocked, unpaused */

	ParticleMgr.bEnable = true;
	ParticleMgr.bLocked = false;
	ParticleMgr.bPaused = false;

	/* unload data files */

	for (i = 0; i < MAX_PARTICLE_DATA_FILES; i++)
	{
		if (ParticleMgr.DBHandle[i] != INVALID_HANDLE)
			ParticleMgr__UnloadData(ParticleMgr.DBHandle[i]);
	}

	/* reserved group id's */

	ParticleMgr.u16EmitterGroupID = EMITTER_RESERVED_GROUP_MIN;
	ParticleMgr.bEmitterGroupIDWrapped = FALSE;

	/* empty name search arrays (for memory leak control) */

	ParticleMgr.pParticleNameSearch->Reset();
	ParticleMgr.pEffectNameSearch->Reset();
	ParticleMgr.pEmitterNameSearch->Reset();
	ParticleMgr.pSwooshNameSearch->Reset();
	ParticleMgr.pSwooshEmitterNameSearch->Reset();
	ParticleMgr.nParticleSearchPowerOfTwo = 0;
	ParticleMgr.nEffectSearchPowerOfTwo = 0;
	ParticleMgr.nEmitterSearchPowerOfTwo = 0;
	ParticleMgr.nSwooshSearchPowerOfTwo = 0;
	ParticleMgr.nSwooshEmitterSearchPowerOfTwo = 0;

	/* clear one-shot fields */

	ParticleMgr.pEmitterVelTransform = NULL;
	ParticleMgr.pvEmitterTarget = NULL;
	ParticleMgr.pEffectVelTransform = NULL;
	ParticleMgr.pvEffectTarget = NULL;
	ParticleMgr.pOwnedParticleVelTransform = NULL;
	ParticleMgr.pvOwnedParticleTarget = NULL;

	/* reset particulate system */
	ParticleMgr.pParticulateSystem->Reset();

	ParticleMgr.pOptionalNameExtension = NULL;
}

/* shutdown */

void ParticleMgr__Shutdown(void)
{
	/* maybe not active */

	if (ParticleMgr.pParticleAlloc == NULL)
		return;

	/* unload data files */

	for (int i = 0; i < MAX_PARTICLE_DATA_FILES; i++)
	{
		if (ParticleMgr.DBHandle[i] != INVALID_HANDLE)
			ParticleMgr__UnloadData(ParticleMgr.DBHandle[i]);
	}

	/* free memory */

	memFree(ParticleMgr.pParticleAlloc);
	memFree(ParticleMgr.pParticleEmitterAlloc);
	memFree(ParticleMgr.pParticleCreateAlloc);
	memFree(ParticleMgr.ppAdvanceList);

#if !defined(GCN)  &&  !defined(PS2)
	if (ParticleMgr.pVertexList)
		memFree(ParticleMgr.pVertexList);
	if (ParticleMgr.pUVList)
		memFree(ParticleMgr.pUVList);
	if (ParticleMgr.pColorList)
		memFree(ParticleMgr.pColorList);
#endif // !GCN  && !PS2

	/* empty name search arrays */

	ParticleMgr.pParticleNameSearch->Reset();
	ParticleMgr.pEffectNameSearch->Reset();
	ParticleMgr.pEmitterNameSearch->Reset();
	ParticleMgr.pSwooshNameSearch->Reset();
	ParticleMgr.pSwooshEmitterNameSearch->Reset();
	ParticleMgr.nParticleSearchPowerOfTwo = 0;
	ParticleMgr.nEffectSearchPowerOfTwo = 0;
	ParticleMgr.nEmitterSearchPowerOfTwo = 0;
	ParticleMgr.nSwooshSearchPowerOfTwo = 0;
	ParticleMgr.nSwooshEmitterSearchPowerOfTwo = 0;
	delete ParticleMgr.pParticleNameSearch;
	delete ParticleMgr.pEffectNameSearch;
	delete ParticleMgr.pEmitterNameSearch;
	delete ParticleMgr.pSwooshNameSearch;
	delete ParticleMgr.pSwooshEmitterNameSearch;

	/* delete the particulate system */
	delete ParticleMgr.pParticulateSystem;

	/* clear the manager */

	memset(&ParticleMgr, 0, sizeof(ts_ParticleMgr));
}

void ParticleMgr__SetOptionalNameExtension(const char* extension)
{
	ParticleMgr.pOptionalNameExtension = extension;
}

#ifndef CONSUMER_BUILD
bool ParticleMgr__IsEmpty(void)
{
	if (DLinkList__Check(&ParticleMgr.FreeList) != (signed) ParticleMgr.nMaxParticles)
		return(FALSE);
	if (SLinkList__Check(&ParticleMgr.EmitterFreeList) != (signed) ParticleMgr.nMaxParticleEmitters)
		return(FALSE);
	if (SLinkList__Check(&ParticleMgr.CreateFreeList) != (signed) ParticleMgr.nMaxParticleCreateRecords)
		return(FALSE);
	return(TRUE);
}
#endif //CONSUMER_BUILD

/* enable/disable particles */

void ParticleMgr__Enable(void)
{
	ParticleMgr.bEnable = TRUE;
}

void ParticleMgr__Disable(void)
{
	/* we let emitters continue as usual to minimize code disruption */

	ParticleMgr__KillAllParticles();
	ParticleMgr.bEnable = FALSE;
}

void ParticleMgr__Toggle(void)
{
	if( ParticleMgr.bEnable == FALSE )
		ParticleMgr__Enable();
	else
		ParticleMgr__Disable();

	return;
}

/* in locked state, particle manager ignores particle/effect creation requests, and does not advance emitters that
are not specially marked as being active even when particle manager is locked. the intent is to provide a safe way
for the load screen to use the particle manager, but still let scripts set up emitters, call for effects, etc, during
initialization without having those particles show up during the loading screen */

void ParticleMgr__Lock(bool bLocked)
{
	ParticleMgr.bLocked = bLocked;
}

/* pause/unpause particles */

void ParticleMgr__Pause(void)
{
	ParticleMgr.bPaused = TRUE;
}
void ParticleMgr__UnPause(void)
{
	ParticleMgr.bPaused = FALSE;
}

/* load data file */

t_Handle ParticleMgr__LoadData(char *pFilename)
{
	/* find an empty slot */

	int i;
	for (i = 0; i < MAX_PARTICLE_DATA_FILES; i++)
	{
		if (ParticleMgr.DBHandle[i] == INVALID_HANDLE)
			break;
	}
	if (i == MAX_PARTICLE_DATA_FILES)
		return(INVALID_HANDLE);	// no open slot
	t_Handle Handle = dblLoad(pFilename);
	if (Handle == INVALID_HANDLE)
		return(INVALID_HANDLE);
	
	/* don't record the same particle set twice */
	
	for (int j = 0; j < MAX_PARTICLE_DATA_FILES; j++)
	{
		if (ParticleMgr.DBHandle[j] == Handle)
			return Handle;
	}

	ParticleMgr.DBHandle[i] = Handle;
	TextureDatabase* pTextureDatabase  = (TextureDatabase *) dblGetChunkData(DB_TEXTURE_SET, DEFAULT_CHUNK_KEY_STRING, Handle);
	ParticleMgr.pTextureArray[i] = new TextureArray();
	ParticleMgr.pTextureArray[i]->AddEntireDatabase(pTextureDatabase);
	ParticleMgr.pTextureArray[i]->AddTexture( TextureMgr::GetWhiteTexture() );
	ParticleMgr.pTextureArray[i]->AddTexture( TextureMgr::GetBackBuffer() );
	ParticleMgr.pTextureArray[i]->AddTexture( TextureMgr::GetFrontBuffer() );
	ParticleMgr.pEffectList[i] = (ts_RecordList *)dblGetChunkData(DB_PARTICLE_EFFECT, DEFAULT_CHUNK_KEY_STRING, Handle);
	ParticleMgr.pEmitterList[i] = (ts_RecordList *)dblGetChunkData(DB_PARTICLE_EMITTER, DEFAULT_CHUNK_KEY_STRING, Handle);
	ParticleMgr.pParticleSourceList[i] = (ts_RecordList *)dblGetChunkData(DB_PARTICLE_SOURCE, DEFAULT_CHUNK_KEY_STRING, Handle);
	ParticleMgr.pSwooshSourceList[i] = (ts_RecordList *)dblGetChunkData(DB_SWOOSH_SOURCE, DEFAULT_CHUNK_KEY_STRING, Handle);
	ParticleMgr.pSwooshEmitterList[i] = (ts_RecordList *)dblGetChunkData(DB_SWOOSH_EMITTER, DEFAULT_CHUNK_KEY_STRING, Handle);

	/* add particles/effects/emitters to name search arrays */

	ParticleMgr__AddSearchNames(Handle, i, DB_PARTICLE_LIST, ParticleMgr.pParticleNameSearch);
	ParticleMgr__AddSearchNames(Handle, i, DB_EFFECT_LIST, ParticleMgr.pEffectNameSearch);
	ParticleMgr__AddSearchNames(Handle, i, DB_EMITTER_LIST, ParticleMgr.pEmitterNameSearch);
	ParticleMgr__AddSearchNames(Handle, i, DB_SWOOSH_LIST, ParticleMgr.pSwooshNameSearch);
	ParticleMgr__AddSearchNames(Handle, i, DB_SWOOSH_EMITTER_LIST, ParticleMgr.pSwooshEmitterNameSearch);
	SET_POWER_OF_2(ParticleMgr.pParticleNameSearch, ParticleMgr.nParticleSearchPowerOfTwo);
	SET_POWER_OF_2(ParticleMgr.pEffectNameSearch, ParticleMgr.nEffectSearchPowerOfTwo);
	SET_POWER_OF_2(ParticleMgr.pEmitterNameSearch, ParticleMgr.nEmitterSearchPowerOfTwo);
	SET_POWER_OF_2(ParticleMgr.pSwooshNameSearch, ParticleMgr.nSwooshSearchPowerOfTwo);
	SET_POWER_OF_2(ParticleMgr.pSwooshEmitterNameSearch, ParticleMgr.nSwooshEmitterSearchPowerOfTwo);
#ifdef _DEBUG
	TEST_NAME_SEARCH(ParticleMgr.pParticleNameSearch, ParticleMgr.nParticleSearchPowerOfTwo);
	TEST_NAME_SEARCH(ParticleMgr.pEffectNameSearch, ParticleMgr.nEffectSearchPowerOfTwo);
	TEST_NAME_SEARCH(ParticleMgr.pEmitterNameSearch, ParticleMgr.nEmitterSearchPowerOfTwo);
	TEST_NAME_SEARCH(ParticleMgr.pSwooshNameSearch, ParticleMgr.nSwooshSearchPowerOfTwo);
	TEST_NAME_SEARCH(ParticleMgr.pSwooshEmitterNameSearch, ParticleMgr.nSwooshEmitterSearchPowerOfTwo);
#endif //_DEBUG

	/* done */

	return(Handle);
}

/* unload data file. this supports run-time loading and unloading of particle data files-- you could load
a new file when you enter one area and unload the file from the previous area. UNTESTED! */

void ParticleMgr__UnloadData(t_Handle Handle)
{
	/* find it in particle data file list */

	int nDataIndex = ParticleMgr__GetDataIndex(Handle);
	ASSERT(nDataIndex != MAX_PARTICLE_DATA_FILES);	// should find it!
	if (nDataIndex == MAX_PARTICLE_DATA_FILES)
		return;	// handle not found

	/* kill all particles and emitters from this file */

	ParticleMgr__KillAllParticles(Handle);
	ParticleMgr__KillAllEmitters(Handle);
	SwooshMgr__KillAllEmitters(Handle);	// do this first since emitter will kill its own swooshes
	SwooshMgr__KillAllSwooshes(Handle);

	/* remove particles/effects/emitters from name search arrays */

	ParticleMgr__RemoveSearchNames(nDataIndex, ParticleMgr.pParticleNameSearch);
	ParticleMgr__RemoveSearchNames(nDataIndex, ParticleMgr.pEffectNameSearch);
	ParticleMgr__RemoveSearchNames(nDataIndex, ParticleMgr.pEmitterNameSearch);
	ParticleMgr__RemoveSearchNames(nDataIndex, ParticleMgr.pSwooshNameSearch);
	ParticleMgr__RemoveSearchNames(nDataIndex, ParticleMgr.pSwooshEmitterNameSearch);
	SET_POWER_OF_2(ParticleMgr.pParticleNameSearch, ParticleMgr.nParticleSearchPowerOfTwo);
	SET_POWER_OF_2(ParticleMgr.pEffectNameSearch, ParticleMgr.nEffectSearchPowerOfTwo);
	SET_POWER_OF_2(ParticleMgr.pEmitterNameSearch, ParticleMgr.nEmitterSearchPowerOfTwo);
	SET_POWER_OF_2(ParticleMgr.pSwooshNameSearch, ParticleMgr.nSwooshSearchPowerOfTwo);
	SET_POWER_OF_2(ParticleMgr.pSwooshEmitterNameSearch, ParticleMgr.nSwooshEmitterSearchPowerOfTwo);
#ifdef _DEBUG
	TEST_NAME_SEARCH(ParticleMgr.pParticleNameSearch, ParticleMgr.nParticleSearchPowerOfTwo);
	TEST_NAME_SEARCH(ParticleMgr.pEffectNameSearch, ParticleMgr.nEffectSearchPowerOfTwo);
	TEST_NAME_SEARCH(ParticleMgr.pEmitterNameSearch, ParticleMgr.nEmitterSearchPowerOfTwo);
	TEST_NAME_SEARCH(ParticleMgr.pSwooshNameSearch, ParticleMgr.nSwooshSearchPowerOfTwo);
	TEST_NAME_SEARCH(ParticleMgr.pSwooshEmitterNameSearch, ParticleMgr.nSwooshEmitterSearchPowerOfTwo);
#endif //_DEBUG

// rhn 6/4/05 - don't unload the dbl!  This is conflicting with the ResourceManger who
// will take care of unloading it anyway.
//	dblRemove(Handle);

	/* slot is available */

	ParticleMgr.DBHandle[nDataIndex] = INVALID_HANDLE;
	delete ParticleMgr.pTextureArray[nDataIndex];
	ParticleMgr.pTextureArray[nDataIndex] = NULL;
}

/* add search names to a search name array */

void ParticleMgr__AddSearchNames(t_Handle Handle, int nDataIndex, int nChunkType, DynamicArray<ts_ParticleNameSearch> *pArray)
{
	ts_RecordList *pRecordList;
	ts_ParticleNameSearch NameSearch;
	pRecordList = (ts_RecordList *) dblGetChunkData(nChunkType, DEFAULT_CHUNK_KEY_STRING, Handle);
	if (pRecordList)
	{
		/* loop through names in chunk */

		int i;
		for (i = 0; i < pRecordList->usRecordCount; i++)
		{
			/* fill in record to be added to search array */

			NameSearch.pName = (char *) GetRecordAddress(pRecordList, i);
			NameSearch.u16DataIndex = nDataIndex;
			NameSearch.u16EntryIndex = i;

			/* add to list, in case-insensitive alphabetically-sorted position (this enables binary search) */

			int j = 0;
			while ((j < pArray->Count()) && (stricmp(NameSearch.pName, pArray->Get(j)->pName) > 0))
			{
				j++;
			}
			pArray->Add(NameSearch, j);
		}

		/* maybe show new list */

#if 0
		dbgPrint("Particle name search list for ");
		if (pArray == ParticleMgr.pParticleNameSearch)
			dbgPrint("particles:\n");
		else if (pArray == ParticleMgr.pEffectNameSearch)
			dbgPrint("effects:\n");
		else if (pArray == ParticleMgr.pEmitterNameSearch)
			dbgPrint("emitters:\n");
		for (i = 0; i < pArray->Count(); i++)
			dbgPrint("   %d: %s, data index %d, entry %d\n", i, pArray->Get(i)->pName, pArray->Get(i)->u16DataIndex, pArray->Get(i)->u16EntryIndex);
#endif //0/1
	}
}

/* remove search names from a search name array */

void ParticleMgr__RemoveSearchNames(int nDataIndex, DynamicArray<ts_ParticleNameSearch> *pArray)
{
	/* backwards since we'll be removing records */

	int i;
	for (i = pArray->Count() - 1; i >= 0; i--)
	{
		if (pArray->Get(i)->u16DataIndex == nDataIndex)
			pArray->Delete(i);
	}

#if 0
	/* maybe show new list */

	dbgPrint("Particle name search list for ");
	if (pArray == ParticleMgr.pParticleNameSearch)
		dbgPrint("particles:\n");
	else if (pArray == ParticleMgr.pEffectNameSearch)
		dbgPrint("effects:\n");
	else if (pArray == ParticleMgr.pEmitterNameSearch)
		dbgPrint("emitters:\n");
	if (pArray->Count() == 0)
		dbgPrint("   List is empty\n");
	else
	{
		for (i = 0; i < pArray->Count(); i++)
			dbgPrint("   %d: %s, data index %d, entry %d\n", i, pArray->Get(i)->pName, pArray->Get(i)->u16DataIndex, pArray->Get(i)->u16EntryIndex);
	}
#endif //0/1
}

/* find effect record by name-- case insensitive */

bool ParticleMgr__FindEffectByName(const char *pName, int &nDataIndex, int &nEffectIndex)
{
	return(ParticleMgr__FindRecordByName(pName, nDataIndex, nEffectIndex, ParticleMgr.pEffectNameSearch, ParticleMgr.nEffectSearchPowerOfTwo));
}

/* find emitter record by name-- case insensitive */

bool ParticleMgr__FindEmitterByName(const char *pName, int &nDataIndex, int &nEmitterIndex)
{
	return(ParticleMgr__FindRecordByName(pName, nDataIndex, nEmitterIndex, ParticleMgr.pEmitterNameSearch, ParticleMgr.nEmitterSearchPowerOfTwo));
}

/* find particle record by name-- case insensitive */

bool ParticleMgr__FindParticleByName(const char *pName, int &nDataIndex, int &nParticleIndex)
{
	return(ParticleMgr__FindRecordByName(pName, nDataIndex, nParticleIndex, ParticleMgr.pParticleNameSearch, ParticleMgr.nParticleSearchPowerOfTwo));
}

/* find swoosh record by name-- case insensitive */

bool ParticleMgr__FindSwooshByName(const char *pName, int &nDataIndex, int &nSwooshIndex)
{
	return(ParticleMgr__FindRecordByName(pName, nDataIndex, nSwooshIndex, ParticleMgr.pSwooshNameSearch, ParticleMgr.nSwooshSearchPowerOfTwo));
}

/* find swoosh emitter record by name-- case insensitive */

bool ParticleMgr__FindSwooshEmitterByName(const char *pName, int &nDataIndex, int &nSwooshEmitterIndex)
{
	return(ParticleMgr__FindRecordByName(pName, nDataIndex, nSwooshEmitterIndex, ParticleMgr.pSwooshEmitterNameSearch, ParticleMgr.nSwooshEmitterSearchPowerOfTwo));
}

/* generic routine to look through a particle data name list chunk for a particular named record-- case-insensitive */
bool ParticleMgr__SearchForRecordByName(const char *pName, int &nDataIndex, int &nRecordIndex, DynamicArray<ts_ParticleNameSearch> *pArray, int nPowerOfTwo)
{
	/* do a case-insensitive binary search */

	if (pArray->Count() == 0)
		return(false);
	int nIndex = (nPowerOfTwo / 2) - 1;	// nPowerofTwo is first power-of-two value > array length
	int nInc = nPowerOfTwo >> 1;	// first nInc is actually nPowerOfTwo >> 2, but this satisfies the while loop-- have to execute at least once
	ASSERT(nInc > 0);	// since nPowerOfTwo > array length, min value is 2, so min nInc is 1
	while (nInc > 0)
	{
		int nResult;
		if (nIndex >= pArray->Count())
			nResult = -1;	// we don't pad array out to power-of-2 length
		else
			nResult = stricmp(pName, pArray->Get(nIndex)->pName);
		if (nResult == 0)
		{
			/* found it */

			nDataIndex = pArray->Get(nIndex)->u16DataIndex;
			nRecordIndex = pArray->Get(nIndex)->u16EntryIndex;
			return(true);
		}

		/* continue binary search */

		nInc >>= 1;
		if (nResult < 0)
			nIndex -= nInc;	// pName is alphabetically before current entry, move toward start of list
		else
			nIndex += nInc;	// pName is alphabetically after current entry, move toward end of list
	}

#if !defined(CDROM) && defined(IGIVEASHITABOUTMISSINGPARTICLES)
	g_console.Error("********** Particle error: couldn't find %s\n", pName);
#endif

	return(false);	// not found
}



bool ParticleMgr__FindRecordByName(const char *pName, int &nDataIndex, int &nRecordIndex, DynamicArray<ts_ParticleNameSearch> *pArray, int nPowerOfTwo)
{
	if(ParticleMgr.pOptionalNameExtension != NULL)
	{
		ASSERT( strlen( pName ) < 508 );
		char mpName[512];
		strcpy(mpName,  pName );
		strcat( mpName, ParticleMgr.pOptionalNameExtension );
		bool success = ParticleMgr__SearchForRecordByName( mpName, nDataIndex, nRecordIndex, pArray, nPowerOfTwo );
		if(success)
			return true;
	}
	return ParticleMgr__SearchForRecordByName( pName, nDataIndex, nRecordIndex, pArray, nPowerOfTwo );
}

/* get number of active particles */

int ParticleMgr__GetActiveParticleCount(void)
{
	int nCount = 0;
	for (int i = 0; i < MAX_PARTICLE_DATA_FILES; i++)
		nCount += DLinkList__Check(&ParticleMgr.ActiveList[i]);
	return(nCount);
}

/* get number of active particle emitters */

int ParticleMgr__GetActiveEmitterCount(void)
{
	return(SLinkList__Check(&ParticleMgr.EmitterActiveList));
}

/* get max number of particles, emitters */

int ParticleMgr__GetMaxParticleCount(void)
{
	return(ParticleMgr.nMaxParticles);
}
int ParticleMgr__GetMaxEmitterCount(void)
{
	return(ParticleMgr.nMaxParticleEmitters);
}

/* convert a particle database handle into the slot index where it is loaded */

int ParticleMgr__GetDataIndex(t_Handle Handle)
{
	int nDataIndex;
	for (nDataIndex = 0; nDataIndex < MAX_PARTICLE_DATA_FILES; nDataIndex++)
	{
		if (Handle == ParticleMgr.DBHandle[nDataIndex])
			break;
	}
	return(nDataIndex);	// may be MAX_PARTICLE_DATA_FILES, which means not found
}

/* download correct texture set (may be more than one particle data file loaded, each has its own texture set) */

void ParticleMgr__LockTextureSet(int nDataIndex)
{
	if (ParticleMgr.pTextureArray[nDataIndex])
		ParticleMgr.pTextureArray[nDataIndex]->Lock();
}

void ParticleMgr__UnLockTextureSet(int nDataIndex)
{
	if (ParticleMgr.pTextureArray[nDataIndex])
		ParticleMgr.pTextureArray[nDataIndex]->Unlock();
}

/* record location of particle effect data, particle source data, emitter source data */

#if 0
void ParticleMgr__SetEffectList(ts_RecordList *pList)
{
	ASSERT(ParticleMgr.nLoadDataIndex != -1);
	ParticleMgr.pEffectList[ParticleMgr.nLoadDataIndex] = pList;
}
void ParticleMgr__SetParticleSourceList(ts_RecordList *pList)
{
	ASSERT(ParticleMgr.nLoadDataIndex != -1);
	ParticleMgr.pParticleSourceList[ParticleMgr.nLoadDataIndex] = pList;
}
void ParticleMgr__SetEmitterList(ts_RecordList *pList)
{
	ASSERT(ParticleMgr.nLoadDataIndex != -1);
	ParticleMgr.pEmitterList[ParticleMgr.nLoadDataIndex] = pList;
}
#endif

/* register a collision test routine to call */

void ParticleMgr__RegisterCollisionTest(t_CollisionTest CollisionTest)
{
	ParticleMgr.CollisionTest = CollisionTest;
}

void ParticleMgr__FreeParticle(ts_Particle* pParticle)
{
	/* memory check, list check */
	ASSERT(pParticle >= ParticleMgr.pParticleAlloc);
#ifdef _DEBUG
	if (pParticle->fActivationDelay > 0.0f)
	{
		ASSERT(DLinkList__InList(&ParticleMgr.PendingList[pParticle->nDataIndex], pParticle));
	}
	else
	{
		ASSERT(DLinkList__InList(&ParticleMgr.ActiveList[pParticle->nDataIndex], pParticle));
	}
#endif //_DEBUG

	/* clear assigned handle */

	pParticle->Handle = INVALID_HANDLE;

	pParticle->u32Flags = 0;	// clear all flags

	/* move to free list */

	if (pParticle->fActivationDelay > 0.0f)
		DLinkList__RemoveLink(&ParticleMgr.PendingList[pParticle->nDataIndex], pParticle);	// hasn't activated yet, still in pending list
	else
		DLinkList__RemoveLink(&ParticleMgr.ActiveList[pParticle->nDataIndex], pParticle);
	DLinkList__AddLinkToHead(&ParticleMgr.FreeList, pParticle);
}

/* allocate a particle from the free list, steal a particle if none free */

ts_Particle *ParticleMgr__AllocParticle(int nDataIndex)
{
	ts_Particle *pParticle;

	pParticle = (ts_Particle *) DLinkList__GetAndRemoveHead(&ParticleMgr.FreeList);
	if (pParticle == NULL)
		return(NULL);

	/* add to head of pending list. if particle is due for immediate activation, then */
	/* first call to ParticleMgr__Advance will move it to the active list. but i don't */
	/* want particles to be rendered without going through at least one advance loop */

	DLinkList__AddLinkToHead(&ParticleMgr.PendingList[nDataIndex], pParticle);

	/* build handle */

	ASSERT(pParticle->Handle == INVALID_HANDLE);					// record should not have a handle assigned
	HANDLE_INDEX(pParticle->Handle) = pParticle - ParticleMgr.pParticleAlloc;	// index into allocated array of records
	HANDLE_ID(pParticle->Handle) = ParticleMgr.u16ID++;
	if (pParticle->Handle == INVALID_HANDLE)
		HANDLE_ID(pParticle->Handle) = ParticleMgr.u16ID++;	// do it again to avoid INVALID_HANDLE
	ASSERT(pParticle->Handle != INVALID_HANDLE);					// record should have a valid handle

	// this must be positive, so that FreeParticle knows which list its on.
	pParticle->fActivationDelay = 0.0001f;

	/* clear all flags and done */

	pParticle->pSource = NULL;
	pParticle->u32Flags = 0;
	return(pParticle);
}

/* kill all particles */

void ParticleMgr__KillAllParticles(t_Handle Handle /*= INVALID_HANDLE*/)
{
	/* if handle specified, kill all particles that match */

	if (Handle != INVALID_HANDLE)
	{
		/* find data file */

		int nDataIndex = ParticleMgr__GetDataIndex(Handle);
		if (nDataIndex == MAX_PARTICLE_DATA_FILES)
			return;	// handle not found

		/* kill active particles */

		ts_Particle *pParticle;
		ts_Particle *pNextParticle;
		pNextParticle = (ts_Particle *) ParticleMgr.ActiveList[nDataIndex].pHead;
		while (pNextParticle)
		{
			/* pre-load next in case current one gets killed */

			pParticle = pNextParticle;
			pNextParticle = pParticle->pNext;

			/* if from this data file, kill */

			ASSERT(pParticle->nDataIndex == nDataIndex);	// since this is in this particular list, should always match
			if (pParticle->nDataIndex == nDataIndex)
				Particle__Kill(pParticle);
		}

		/* kill pending particles */

		pNextParticle = (ts_Particle *) ParticleMgr.PendingList[nDataIndex].pHead;
		while (pNextParticle)
		{
			/* pre-load next in case current one gets killed */

			pParticle = pNextParticle;
			pNextParticle = pParticle->pNext;

			/* if from this data file, kill */

			ASSERT(pParticle->nDataIndex == nDataIndex);	// since this is in this particular list, should always match
			if (pParticle->nDataIndex == nDataIndex)
				Particle__Kill(pParticle);
		}

		/* and kill pending create effects */

		ts_ParticleCreate *pParticleCreate;
		ts_ParticleCreate *pNextParticleCreate;
		pNextParticleCreate = (ts_ParticleCreate *) ParticleMgr.CreatePendingList.pHead;
		while (pNextParticleCreate)
		{
			/* pre-load next in case current one gets killed */

			pParticleCreate = pNextParticleCreate;
			pNextParticleCreate = pParticleCreate->pNext;

			/* if from this data file, move to free list */

			if (pParticleCreate->nDataIndex == nDataIndex)
			{
				SLinkList__RemoveLink(&ParticleMgr.CreatePendingList, pParticleCreate);
				SLinkList__AddLinkToHead(&ParticleMgr.CreateFreeList, pParticleCreate);
			}
		}

		/* done */

		return;
	}

	/* no handle specified, kill all particles-- this is one-level-deep recursive (runs code immediately above here) */

	for (int nDataIndex = 0; nDataIndex < MAX_PARTICLE_DATA_FILES; nDataIndex++)
	{
		if (ParticleMgr.DBHandle[nDataIndex] != INVALID_HANDLE)
			ParticleMgr__KillAllParticles(ParticleMgr.DBHandle[nDataIndex]);
	}
}

/* kill all particle emitters */

void ParticleMgr__KillAllEmitters(t_Handle Handle /*= INVALID_HANDLE*/)
{
	/* if handle specified, kill all records that match-- this is useful for unloading a particle database file */

	int nDataIndex = -1;
	if (Handle != INVALID_HANDLE)
	{
		nDataIndex = ParticleMgr__GetDataIndex(Handle);
		if (nDataIndex == MAX_PARTICLE_DATA_FILES)
			return;	// handle not found
	}

	/* loop through active records */

	ts_ParticleEmitter *pNextEmitter = (ts_ParticleEmitter *) ParticleMgr.EmitterActiveList.pHead;
	while (pNextEmitter)
	{
		/* pre-load next since this one may be deleted */

		ts_ParticleEmitter *pEmitter = pNextEmitter;
		pNextEmitter = pEmitter->pNext;

		/* if no handle specified, always kill it, otherwise kill it if it belongs to this group */

		if ((Handle == INVALID_HANDLE) || (pEmitter->nDataIndex == nDataIndex))
			ParticleEmitter__Kill(pEmitter->Handle);
	}
}

/* removes all particle emitters in group */

void ParticleMgr__KillEmitterGroup(u16 u16Group, t_Handle Handle /*= INVALID_HANDLE*/)
{
	/* if handle specified, kill all records that match-- this is useful for unloading a particle database file */

	int nDataIndex = -1;
	if (Handle != INVALID_HANDLE)
	{
		nDataIndex = ParticleMgr__GetDataIndex(Handle);
		if (nDataIndex == MAX_PARTICLE_DATA_FILES)
			return;	// handle not found
	}

	/* loop through active records */

	ts_ParticleEmitter *pNextEmitter = (ts_ParticleEmitter *) ParticleMgr.EmitterActiveList.pHead;
	while (pNextEmitter)
	{
		/* pre-load next since this one may be deleted */

		ts_ParticleEmitter *pEmitter = pNextEmitter;
		pNextEmitter = pEmitter->pNext;

		/* if no handle specified, always kill it, otherwise kill it if it belongs to this group */

		if ((pEmitter->u16Group == u16Group) && ((Handle == INVALID_HANDLE) || (pEmitter->nDataIndex == nDataIndex)))
			ParticleEmitter__Kill(pEmitter->Handle);
	}
}

/* allocate a particle emitter record, move to active list */

ts_ParticleEmitter *ParticleMgr__AllocParticleEmitter(u16 u16Group)
{
	/* check group id. if id is in reserved range, then it should have been gotten from ParticleMgr__RequestEmitterGroupID. */
	/* if not, then this is probably the application using a #define'd id. this breaks the reserved system, so don't */
	/* allow it. typically, the u16EmitterGroupID will increment very slowly, so the chances of randomly picking a group id */
	/* between EMITTER_RESERVED_GROUP_MIN and u16EmitterGroupID are very small, so this check should be sufficient */

#ifdef _DEBUG
	if ((u16Group >= EMITTER_RESERVED_GROUP_MIN) &&
		 (u16Group < EMITTER_RESERVED_GROUP_MAX) &&
		 (ParticleMgr.bEmitterGroupIDWrapped == FALSE) &&	// if it has wrapped, all values between min and max are valid
		 (u16Group >= ParticleMgr.u16EmitterGroupID))
	{
		ASSERT(FALSE);	// don't use reserved group id's without going through ParticleMgr__RequestEmitterGroupID. see comment above
		return(NULL);
	}
#endif //_DEBUG

	/* grab a free emitter */

	ts_ParticleEmitter *pEmitter = (ts_ParticleEmitter *) SLinkList__GetAndRemoveHead(&ParticleMgr.EmitterFreeList);
	ASSERT(pEmitter);	// shouldn't run out of records. if so, increase nMaxParticleEmitters count passed to ParticleMgr__Init
	if (pEmitter == NULL)
		return(NULL);
	pEmitter->u32Flags = 0;

	/* build handle */

	ASSERT(pEmitter->Handle == INVALID_HANDLE);					// record should not have a handle assigned
	HANDLE_INDEX(pEmitter->Handle) = pEmitter - ParticleMgr.pParticleEmitterAlloc;	// index into allocated array of records
	HANDLE_ID(pEmitter->Handle) = ParticleMgr.u16ID++;
	if (pEmitter->Handle == INVALID_HANDLE)
		HANDLE_ID(pEmitter->Handle) = ParticleMgr.u16ID++;		// do it again to avoid INVALID_HANDLE
	ASSERT(pEmitter->Handle != INVALID_HANDLE);					// record should have a valid handle

	/* record group */

	pEmitter->u16Group = u16Group;

#ifdef PRINT_PARTICLE_CREATION
	/* emitter only has a name pointer if created from an emitter record (you can also create emitters directly) */

	pEmitter->pName = NULL;
#endif //PRINT_PARTICLE_CREATION

	/* add to active list */

	SLinkList__AddLinkToHead(&ParticleMgr.EmitterActiveList, pEmitter);

	/* return handle */

	return(pEmitter);
}

/* record a creation request for a particle effect. if pvPos, pvVelUnitVector, and/or pfVelMagnitude is non-NULL,
they override particle's settings. for cache purposes, creation requests are logged and then all
particle effects are created at same time */

bool ParticleMgr__CreateEffect(
	const char *pEffectName,							// which effect to create
	const Vector3Packed *pvPos,						// world position to create at, can be NULL (if so, will use particle source data)
	const Vector3Packed *pvVelUnitVector,			// velocity to create at, can be NULL (if so, will use particle source data)
	const float *pfVelMagnitude,						// velocity to create at, can be NULL (if so, will use particle source data)
	const Vector3Packed *pvAdditiveVel,				// additive velocity to use, can be NULL (if so, no additive velocity)
	const float fFirstAdvanceTime,					// value to use for particle's first advance. good default value is 0.0
	const float fScale, /*= 1.0f*/					// scale to apply to created particles
	const int nScaleRandomPct, /*= 0*/				// scale randomness
	const float fConstantAlpha, /*= 1.0f*/			// constant alpha
	const float fColorMult, /*= 1.0f*/				// color multiplier
	const u32 u32ForceFlags, /*= 0*/					// bits to force in ts_Particle.u32Flags on particle creation
	const int iSceneIndex, /*= -1*/					// which scene this effect is drawn in.  -1 = all scenes
	const CPlaneList *pPlaneList, /*= NULL*/	// restricting plane list
	const SplinePath *pPath) /*= NULL*/				// path for particles to follow
{
	/* not if disabled or locked. this is carefully placed, so that outside requests for effects (that have to go */
	/* through this public interface) are blocked if manager is locked, but effect requests from ParticleEmitter__Advance */
	/* (which go through a private routine) are allowed to pass */

	if ((ParticleMgr.bEnable == false) || (ParticleMgr.bLocked))
		return(true);	// treat it as success

	/* find effect by name */

	int nDataIndex;
	int nEffectIndex;
	if (ParticleMgr__FindEffectByName(pEffectName, nDataIndex, nEffectIndex) == false)
		return(false);	// not found

	/* and create */

	ParticleMgr__CreateEffect(
		nDataIndex,				// which of possible multiple loaded particle data files to use
		nEffectIndex,			//	which effect to create
		pvPos,					// world position to create at, can be NULL (if so, will use particle source data)
		pvVelUnitVector,		// velocity to create at, can be NULL (if so, will use particle source data)
		pfVelMagnitude,		// velocity to create at, can be NULL (if so, will use particle source data)
		pvAdditiveVel,			// additive velocity to use, can be NULL (if so, no additive velocity)
		fFirstAdvanceTime,	// value to use for particle's first advance. good default value is 0.0
		fScale,					// scale to apply to created particles
		nScaleRandomPct,		// scale randomness
		fConstantAlpha,		// constant alpha
		fColorMult,				// color multiplier
		u32ForceFlags,			// bits to force in ts_Particle.u32Flags on particle creation
		iSceneIndex,			// which scene this effect is drawn in.  -1 = all scenes
		pPlaneList,				// restricting plane list
		pPath						// path for particles to follow
		// const ts_ParticleEmitter *pFluxEmitter = NULL
		);
	return(true);
}

bool ParticleMgr__CreateEffect(
	const char *pEffectName,							// which effect to create
	const Vector3Padded *pvPos,						// world position to create at, can be NULL (if so, will use particle source data)
	const Vector3Padded *pvVelUnitVector,			// velocity to create at, can be NULL (if so, will use particle source data)
	const float *pfVelMagnitude,						// velocity to create at, can be NULL (if so, will use particle source data)
	const Vector3Padded *pvAdditiveVel,				// additive velocity to use, can be NULL (if so, no additive velocity)
	const float fFirstAdvanceTime,					// value to use for particle's first advance. good default value is 0.0
	const float fScale, /*= 1.0f*/					// scale to apply to created particles
	const int nScaleRandomPct, /*= 0*/				// scale randomness
	const float fConstantAlpha, /*= 1.0f*/			// constant alpha
	const float fColorMult, /*= 1.0f*/				// color multiplier
	const u32 u32ForceFlags, /*= 0*/					// bits to force in ts_Particle.u32Flags on particle creation
	const int iSceneIndex, /*= -1*/					// which scene this effect is drawn in.  -1 = all scenes
	const CPlaneList *pPlaneList, /*= NULL*/		// restricting plane list
	const SplinePath *pPath) /*= NULL*/				// path for particles to follow
{
	/* find effect by name */

	return ParticleMgr__CreateEffect(
		pEffectName,							// which effect to create
		(Vector3Packed*)pvPos,						// world position to create at, can be NULL (if so, will use particle source data)
		(Vector3Packed*)pvVelUnitVector,			// velocity to create at, can be NULL (if so, will use particle source data)
		pfVelMagnitude,						// velocity to create at, can be NULL (if so, will use particle source data)
		(Vector3Packed*)pvAdditiveVel,				// additive velocity to use, can be NULL (if so, no additive velocity)
		fFirstAdvanceTime,					// value to use for particle's first advance. good default value is 0.0
		fScale, /*= 1.0f*/					// scale to apply to created particles
		nScaleRandomPct, /*= 0*/				// scale randomness
		fConstantAlpha, /*= 1.0f*/			// constant alpha
		fColorMult, /*= 1.0f*/				// color multiplier
		u32ForceFlags, /*= 0*/					// bits to force in ts_Particle.u32Flags on particle creation
		iSceneIndex, /*= -1*/					// which scene this effect is drawn in.  -1 = all scenes
		pPlaneList, /*= NULL*/				// restricting plane list
		pPath /*= NULL*/						// path for particles to follow
		// const ts_ParticleEmitter *pFluxEmitter = NULL
		);
}

/* record a creation request for a particle effect. if pvPos, pvVelUnitVector, and/or pfVelMagnitude is non-NULL,
they override particle's settings. for cache purposes, creation requests are logged and then all
particle effects are created at same time */

void ParticleMgr__CreateEffect(
	const int nDataIndex,										// which of possible multiple loaded particle data files to use
	const int nEffectID,											// which effect to create
	const Vector3Packed *pvPos,								// world position to create at, can be NULL (if so, will use particle source data)
	const Vector3Packed *pvVelUnitVector,					// velocity to create at, can be NULL (if so, will use particle source data)
	const float *pfVelMagnitude,								// velocity to create at, can be NULL (if so, will use particle source data)
	const Vector3Packed *pvAdditiveVel,						// additive velocity to use, can be NULL (if so, no additive velocity)
	const float fFirstAdvanceTime,							// value to use for particle's first advance. good default value is 0.0
	const float fScale, /*= 1.0f*/							// scale to apply to created particles
	const int nScaleRandomPct, /*= 0*/						// scale randomness
	const float fConstantAlpha, /*= 1.0f*/					// constant alpha
	const float fColorMult, /*= 1.0f*/						// color multiplier
	const u32 u32ForceFlags, /*= 0*/							// bits to force in ts_Particle.u32Flags on particle creation
	const int iSceneIndex, /*= -1*/							// which scene this effect is drawn in.  -1 = all scenes
	const CPlaneList *pPlaneList, /*= NULL*/			// restricting plane list
	const SplinePath *pPath, /*= NULL*/						// path for particle to follow
	const ts_ParticleEmitter *pFluxEmitter) /*= NULL*/	// pointer back to emitter for flux particles
{
	/* pre-load and reset one-shots */

	Graphics4x4 *pVelTransform = ParticleMgr.pEffectVelTransform;
	Vector3Packed *pvTarget = ParticleMgr.pvEffectTarget;
	ParticleMgr.pEffectVelTransform = NULL;
	ParticleMgr.pvEffectTarget = NULL;

	/* maybe disabled */

	if (ParticleMgr.bEnable == FALSE)
		return;

	/* maybe nothing to do */

	if ((fScale == 0.0f) || (fConstantAlpha == 0.0f))
		return;

	/* grab a free record. if full, go ahead and create all previously requested effects, then try again */

	ts_ParticleCreate *pParticleCreate = (ts_ParticleCreate *) SLinkList__GetAndRemoveHead(&ParticleMgr.CreateFreeList);
	if (pParticleCreate == NULL)
	{
#ifdef _DEBUG
		/* this is a non-critical assert, but if it hits it means that we are trying to create more */
		/* particles per frame through direct calls to ParticleEffect__Create than expected. we try to */
		/* collect all the particle creation for data cache reasons. calling ParticleMgr__CreatePendingEffects */
		/* while not in ParticleMgr__Advance defeats this. if this happens a lot, you might want to */
		/* call GameEngine__SetMaxParticleCreateRecords to increase the size of the pending list */

		if (ParticleMgr.bInAdvance == FALSE)
			ASSERT(FALSE);
#endif //_DEBUG
		ParticleMgr__CreatePendingEffects();
		pParticleCreate = (ts_ParticleCreate *) SLinkList__GetAndRemoveHead(&ParticleMgr.CreateFreeList);
		ASSERT(pParticleCreate != NULL);
	}

	/* record passed data for later call to Particle__Create. the pointer stuff is a bit awkward, and bloats */
	/* the ts_ParticleCreate record, but the alternative (previously, i was storing a known bad value in the */
	/* variables and then checking that) isn't too pretty either. in any case, i have to record to passed values, */
	/* i can't count on them being persistent */

	pParticleCreate->iSceneIndex = iSceneIndex;
	pParticleCreate->nDataIndex = nDataIndex;
	pParticleCreate->nEffectID = nEffectID;
	if (pvPos)
	{
		pParticleCreate->vPos = *pvPos;
		pParticleCreate->pvPos = &pParticleCreate->vPos;
	}
	else
		pParticleCreate->pvPos = NULL;
	if (pvVelUnitVector)
	{
		pParticleCreate->vVelUnitVector = *pvVelUnitVector;
		pParticleCreate->pvVelUnitVector = &pParticleCreate->vVelUnitVector;
		if (pvVelUnitVector->LengthSquared() == 0.0f)
		{
			ASSERT(false);	// this vector defines the direction at which the particles are ejected from the effect position, should be a unit vector
			pParticleCreate->pvVelUnitVector = NULL;
		}
	}
	else
		pParticleCreate->pvVelUnitVector = NULL;
	if (pfVelMagnitude)
	{
		pParticleCreate->fVelMagnitude = *pfVelMagnitude;
		pParticleCreate->pfVelMagnitude = &pParticleCreate->fVelMagnitude;
	}
	else
		pParticleCreate->pfVelMagnitude = NULL;
	if (pvAdditiveVel)
	{
		pParticleCreate->vAdditiveVel = *pvAdditiveVel;
		pParticleCreate->pvAdditiveVel = &pParticleCreate->vAdditiveVel;
	}
	else
		pParticleCreate->pvAdditiveVel = NULL;
	pParticleCreate->fFirstAdvanceTime = fFirstAdvanceTime;
	pParticleCreate->u32ForceFlags = u32ForceFlags;
	if (nScaleRandomPct != 0)
		pParticleCreate->fScale = Particle__fRandomValue(fScale, nScaleRandomPct);
	else
		pParticleCreate->fScale = fScale;
	pParticleCreate->fConstantAlpha = fConstantAlpha;
	pParticleCreate->fColorMult = fColorMult;
	pParticleCreate->pVelTransform = pVelTransform;
	pParticleCreate->pvTarget = pvTarget;
	pParticleCreate->pPlaneList = (CPlaneList*) pPlaneList;
	pParticleCreate->pFluxEmitter = (ts_ParticleEmitter *) pFluxEmitter;
	pParticleCreate->pPath = (SplinePath *) pPath;

	/* add to pending list */

	SLinkList__AddLinkToHead(&ParticleMgr.CreatePendingList, pParticleCreate);
}

/* create an owned particle-- these are particles that application wants to control directly. they still go through
Particle__Advance code, but application can directly set things like position, orientation, etc. first implemented
to support schools of fish. owned particles typically have a callback that they call when they die so that the
application knows that particle is gone (particles can die for unpredictable reasons like being stolen for higher
priority effects). owned particles are created directly, not through the effect interface */

t_Handle ParticleMgr__CreateOwnedParticle(
	const char *pParticleName,							// particle to create
	const Vector3Padded *pvPos,								// world position to create at, can be NULL (if so, will use particle source data)
	const Vector3Padded *pvVelUnitVector,					// velocity to create at, can be NULL (if so, will use particle source data)
	const float *pfVelMagnitude,						// velocity to create at, can be NULL (if so, will use particle source data)
	const Vector3Padded *pvAdditiveVel,						// additive velocity to use, can be NULL (if so, no additive velocity)
	const t_ParticleDeathCallback DeathCallback,	// callback to call at particle death (can be NULL)
	void *context,											// context within which to call DeathCallback
	const float fScale, /*= 1.0f*/					// scale factor to apply to particle size
	const float fConstantAlpha) /*= 1.0f*/			// overall alpha to multiply against alpha envelope
{
	/* when the particle manager is locked, it is supposed to ignore all outside requests for effects/particles. but i have */
	/* to honor requests for owned particles...i need a test case, so i'm putting this assert here */

	ASSERT(ParticleMgr.bLocked == false);	// if this hits, please tell ned (and then ignore it)

	/* pre-load and reset one-shots */

	Graphics4x4 *pVelTransform = ParticleMgr.pOwnedParticleVelTransform;
	Vector3Packed *pvTarget = ParticleMgr.pvOwnedParticleTarget;
	ParticleMgr.pOwnedParticleVelTransform = NULL;
	ParticleMgr.pvOwnedParticleTarget = NULL;

	/* find particle by name */

	int nDataIndex;
	int nParticleIndex;
	if (ParticleMgr__FindParticleByName(pParticleName, nDataIndex, nParticleIndex) == false)
		return(INVALID_HANDLE);	// not found

	/* point to source for this particle id */

	ts_ParticleSource *pParticleSource = (ts_ParticleSource *) GetRecordAddress(ParticleMgr.pParticleSourceList[nDataIndex], nParticleIndex);

#ifdef PRINT_PARTICLE_CREATION
	/* spit out owned particle's name */

	dbgPrint("Creating owned particle %s, %d active particles\n", pParticleName, ParticleMgr__GetActiveCount());
#endif //PRINT_PARTICLE_CREATION

	/* create particle. create particle with a small activation delay-- this will put the particle */
	/* into the pending list until the next time the particle manager advances. this fixes a problem */
	/* in running tak2 in makeenv mode, where the particle is created but the particle manager never */
	/* runs, so the particle stays on the pending list but then we try to free it from the active list */

	t_Handle Handle = Particle__Create(
		nDataIndex,					// which of possibly-multiply-loaded particle data files to use
		(Vector3Packed*)pvPos,						// world position to create at, can be NULL (if so, will use particle source data)
		(Vector3Packed*)pvVelUnitVector,			// velocity to create at, can be NULL (if so, will use particle source data)
		pfVelMagnitude,			// velocity to create at, can be NULL (if so, will use particle source data)
		(Vector3Packed*)pvAdditiveVel,				// additive velocity to use, can be NULL (if so, no additive velocity)
		0,								// forced bits for ts_Particle.u32Flags
		0.0f,							// first Particle__Advance time slice, 0.0 is a good value
		fScale,						// scale to apply to particle size
		fConstantAlpha,			// overall alpha to multiply against alpha envelope
		1.0f,							// overall value to multiply against color envelope
		0.01f,						// activation delay-- particle will go active when this counts down to 0
		pParticleSource,			// particle source data
		-1,							// all scenes
		pVelTransform,				// possible transform for calculated initial velocity
		pvTarget,					// possible target for particle to move toward
		NULL,							// restricting plane list
		NULL,							// path for particle to follow
		NULL);						// pointer to flux emitter for flux particles
	if (Handle == INVALID_HANDLE)
		return(INVALID_HANDLE);

	/* set death callback */

	ts_Particle *pParticle = PARTICLE_GET_RECORD(Handle);
	ASSERT(pParticle->Handle == Handle);	// it damn well should, this particle was just created
	pParticle->DeathCallback = DeathCallback;
	pParticle->callbackContext = context;

	/* success */

	return(Handle);
}

/* create all pending particle effects */

void ParticleMgr__CreatePendingEffects(void)
{
	ts_ParticleCreate *pParticleCreate;

//yuch--setup cache with particle source???
	pParticleCreate = (ts_ParticleCreate *) ParticleMgr.CreatePendingList.pHead;
	while (pParticleCreate)
	{
		ParticleEffect__Create(pParticleCreate);
		pParticleCreate = pParticleCreate->pNext;
	}

	/* reset lists */

	SLinkList__Init(&ParticleMgr.CreateFreeList, offsetof(ts_ParticleCreate, pNext));
	SLinkList__LinkBlock(&ParticleMgr.CreateFreeList, ParticleMgr.pParticleCreateAlloc, ParticleMgr.nMaxParticleCreateRecords, sizeof(ts_ParticleCreate));
	SLinkList__Init(&ParticleMgr.CreatePendingList, offsetof(ts_ParticleCreate, pNext));
}

/* update particle system */

void ParticleMgr__Advance(void)
{
	ts_Particle *pParticle;
	ts_Particle *pNextParticle;
	int i;
	u32 u32DistortionsToApply;

	/* are we paused? */

	if (ParticleMgr.bPaused)
		return;

	/* maybe disabled */

	if (ParticleMgr.bEnable == FALSE)
		return;

	/* get time advance for this frame */

	ParticleMgr.fFrameSec = g_timer.GetFrameSec();
	ParticleMgr.fOSFrameSec = g_timer.GetOSFrameSec();

	/* bail if no time has passed (paused) */

	if (Math::Equal(ParticleMgr.fFrameSec, 0.0f))
		return;

	/* signal that we are in routine */

#ifndef CONSUMER_BUILD
	ParticleMgr.bInAdvance = TRUE;
#endif //CONSUMER_BUILD

	/* let the emitters fire */

	ParticleMgr__AdvanceEmitters();

	/* create pending effects */

	ParticleMgr__CreatePendingEffects();

	/* build list of distortions that apply to the particles */
//yuch-- calculate bounding box for all active particles? will it help?

	u32DistortionsToApply = DistortionMgr__BuildApplyList(NULL, DISTORTION_AFFECTS_PARTICLES | FORCEFIELD_AFFECTS_PARTICLES);
	if (u32DistortionsToApply != 0)
		DistortionMgr__TransformDistortions(NULL, NULL);	// leave distortions in world space

	/* loop through all possibly-multiply-loaded particle data files */

	for (int nDataIndex = 0; nDataIndex < MAX_PARTICLE_DATA_FILES; nDataIndex++)
	{
		if (ParticleMgr.DBHandle[nDataIndex] == INVALID_HANDLE)
			continue;

		/* any pending particles that need to be added to active list? */

		pNextParticle = (ts_Particle *) ParticleMgr.PendingList[nDataIndex].pHead;
		while (pNextParticle)
		{
			/* have to pre-load since particle may be removed from list */

			pParticle = pNextParticle;
			pNextParticle = pNextParticle->pNext;

			/* advance time for this particle */

			float fAdvanceTime = ((pParticle->u32Flags & PARTICLE_USE_OS_TIMER) != 0) ? ParticleMgr.fOSFrameSec : ParticleMgr.fFrameSec;

			/* time to start? */

			ASSERT(pParticle->fActivationDelay >= 0.0f);	// should never be negative, that would mean it stayed on the pending list past it's activation time
			pParticle->fActivationDelay -= fAdvanceTime;
			if (pParticle->fActivationDelay <= 0.0f)
			{
				DLinkList__RemoveLink(&ParticleMgr.PendingList[nDataIndex], pParticle);

				// insert it
				Particle__SortIntoActive( pParticle, nDataIndex );

				pParticle->u32Flags |= PARTICLE_IN_VIEW_ANY_SCENE;

				/* may have to activate piggyback emitter */

				if (pParticle->PiggybackEmitterHandle != INVALID_HANDLE)
					ParticleEmitter__Activate(pParticle->PiggybackEmitterHandle);
			}
		}

		/* build list of particles to advance. this is necessary because particles may get stolen during advance process */
		/* (mainly due to spawning at life's end), and i don't want to advance new particles. worse things can happen, */
		/* too, like the chain of links pointing back to the beginning of the list and advancing particles twice */

		if (ParticleMgr.ActiveList[nDataIndex].pHead == NULL)
		{
#ifndef CONSUMER_BUILD
			ParticleMgr.bInAdvance = FALSE;	// signal that we are out of routine
#endif //CONSUMER_BUILD
			continue;	// quick exit
		}
		ParticleMgr.nAdvanceListSize = 0;
		pNextParticle = (ts_Particle *) ParticleMgr.ActiveList[nDataIndex].pHead;
		while (pNextParticle)
		{
			pParticle = pNextParticle;
			pNextParticle = pNextParticle->pNext;

			// TJC - this used to happen before new particles were created, but I moved it reduce the # of times
			// we walk the list of particles.  It could limit our max particle count by some, but I think thats the
			// only real drawback.
			/* remove particles that are not visible in any view. the PARTICLE_IN_VIEW_ANY_SCENE bit is set as part */
			/* of the ParticleMgr__PrepareRender/ParticleMgr__Render calls last frame */
			/* should it be killed? */
			if (((pParticle->u32Flags & PARTICLE_IN_VIEW_ANY_SCENE) == 0) &&
				 ((pParticle->pSource->u32Flags & PARTICLE_NO_VIEW_CULL) == 0))
			{
				Particle__Kill(pParticle);
				continue;
			}
			else
			{
				// TJC - also clear this flag for next frame...
				pParticle->u32Flags &= ~PARTICLE_IN_VIEW_ANY_SCENE;
				// TJC - and track Active Count in here.
#ifndef CONSUMER_BUILD
				ParticleMgr.u32ActiveCount++;
#endif //CONSUMER_BUILD
			}

			ParticleMgr.ppAdvanceList[ParticleMgr.nAdvanceListSize++] = pParticle;
			pParticle->u32Flags |= PARTICLE_NEEDS_ADVANCEMENT;	// will be cleared by alloc/dealloc if particle gets stolen
			pParticle = pParticle->pNext;
		}

		/* go through particles */

		for (i = 0; i < ParticleMgr.nAdvanceListSize; i++)
		{
			pParticle = ParticleMgr.ppAdvanceList[i];

			/* check advancement flag */

			if ((pParticle->u32Flags & PARTICLE_NEEDS_ADVANCEMENT) == 0)
				continue;

			/* advance particle */

			Particle__Advance(pParticle, u32DistortionsToApply);
		}
	}

	/* maybe adjusting lights */

	if (ParticleMgr.bAdjustLights)
	{
		ParticleMgr.fCurrentSeconds += ParticleMgr.fFrameSec;

		/* done with lead out? */

		if (ParticleMgr.fCurrentSeconds > ParticleMgr.fLeadOutSeconds)
			ParticleMgr.bAdjustLights = false;
	}

#ifndef CONSUMER_BUILD
	ParticleMgr.bInAdvance = FALSE;	// signal that we are out of routine
#endif //CONSUMER_BUILD
}

/* let emitters emit */

void ParticleMgr__AdvanceEmitters(void)
{
	/* loop through active emitters */

	ts_ParticleEmitter *pNextEmitter = (ts_ParticleEmitter *) ParticleMgr.EmitterActiveList.pHead;
	while (pNextEmitter)
	{
		/* record next emitter since ParticleEmitter__Advance can kill emitter */

		ts_ParticleEmitter *pEmitter = pNextEmitter;
		pNextEmitter = pNextEmitter->pNext;

		/* not if particle manager is locked, unless specially marked */

		if ((ParticleMgr.bLocked) && ((pEmitter->u32Flags & EMITTER_ADVANCES_WHILE_MANAGER_LOCKED) == 0))
			continue;

		/* advance if active */

		if (pEmitter->u32Flags & EMITTER_ACTIVE)
			ParticleEmitter__Advance(pEmitter);
	}
}

/* prepare for a render. ParticleMgr__Render is called once per scene, since the particles may be visible in more than
one scene. i want to kill particles that are out of all views, so mark particles as unrendered. each time a particle is
rendered, it's gets marked. after all scenes have been rendered, i can kill particles that weren't rendered in any scene */

void ParticleMgr__PrepareRender(void)
{
#ifndef CONSUMER_BUILD
	/* useful info */

	ParticleMgr.u32ActiveCount = 0;
	ParticleMgr.u32RenderedCount = 0;
#endif //CONSUMER_BUILD
}

/* render particles */

void ParticleMgr__Render(CScene *pScene)
{
	/* maybe no particles at all, from any particle data file */

	int nDataIndex;
	for (nDataIndex = 0; nDataIndex < MAX_PARTICLE_DATA_FILES; nDataIndex++)
	{
		/* data loaded? */

		if (ParticleMgr.DBHandle[nDataIndex] == INVALID_HANDLE)
			continue;

		/* any active particles? */

		if (ParticleMgr.ActiveList[nDataIndex].pHead != NULL)
			break;	// found at least one active particle
	}
	if (nDataIndex == MAX_PARTICLE_DATA_FILES)
		return;

	/* record fog state */

#ifndef CONSUMER_BUILD
	ParticleMgr.bFogEnabled = ((RenderState__Get(RS_FOG) && RenderMgr__FogEnabled()));
#else
	ParticleMgr.bFogEnabled = RenderState__Get(RS_FOG);
#endif
	ParticleMgr.fNearFog = pScene->d_sceneView.GetNearFog();
	ParticleMgr.fFarFog = pScene->d_sceneView.GetFarFog();
	ParticleMgr.fOneOverFogDelta = 1.0f / (ParticleMgr.fFarFog - ParticleMgr.fNearFog);

	/* far clip distance of current scene affects z-bias calculation */

	ParticleMgr.fZBiasFarClipAdjust = 100.0f / pScene->d_sceneView.GetFarClip();

	/* get the current scene index, so we can make sure each particle is rendering to the appropriate scene */

	int nSceneIndex = 0;
	CScene *pTestScene = SceneMgr::GetFirstScene();
	while ((pTestScene) && (pTestScene != pScene))
	{
		nSceneIndex++;
		pTestScene = SceneMgr::GetNextScene(pTestScene);
	}
	ASSERT(nSceneIndex < 4);	// we handle 4 render-in-this-scene-only indices-- the PARTICLE_ONLY_SCENE_<n> defines

	/* maybe adjust lights */

	if (ParticleMgr.bAdjustLights)
	{
		/* which phase? */

		float fMult = 1.0f;
		if (ParticleMgr.fCurrentSeconds < ParticleMgr.fLeadInSeconds)
		{
			/* lead in */

			fMult = ParticleMgr.fCurrentSeconds / ParticleMgr.fLeadInSeconds;
		}
		else if (ParticleMgr.fCurrentSeconds > ParticleMgr.fSustainSeconds)	// fSustainSeconds is set to fLeadInSeconds + fSustainSeconds
		{
			/* lead out */

			fMult = 1.0f - (ParticleMgr.fCurrentSeconds - ParticleMgr.fSustainSeconds) / (ParticleMgr.fLeadOutSeconds - ParticleMgr.fSustainSeconds);
		}

		/* multipliers */

		float fAmbientMult = 1.0f + ((ParticleMgr.fAmbientMult - 1.0f) * fMult);	// at 0.0, 1.0, at 1.0, fAmbientMult
		float fDirectionalMult = 1.0f + ((ParticleMgr.fDirectionalMult - 1.0f) * fMult);	// at 0.0, 1.0, at 1.0, fDirectionalMult

		/* adjust ambient */

		ts_fRGB NewAmbient;
		NewAmbient.R = g_lightSet.getAmbientLight()->R * fAmbientMult;
		NewAmbient.G = g_lightSet.getAmbientLight()->G * fAmbientMult;
		NewAmbient.B = g_lightSet.getAmbientLight()->B * fAmbientMult;
		NewAmbient.R = Math::Min(NewAmbient.R, 1.0f);
		NewAmbient.G = Math::Min(NewAmbient.G, 1.0f);
		NewAmbient.B = Math::Min(NewAmbient.B, 1.0f);

		/* and adjust lights */

		g_lightSet.adjustLights( NewAmbient, fDirectionalMult );
	}

	/* send lights for possibly lighted particles. lights have already been transformed into camera space, */
	/* so load identity matrix so they don't get transformed any further */

	MatrixStack__Mode( MS_MODELVIEW );
	MatrixStack__Identity();
	RenderState__Request( RS_SEND_LIGHTS, RS_TRUE );

	/* particle vertices are in world space */

	MatrixStack__Mode( MS_MODELVIEW );
	MatrixStack__Load( &g4WorldToCamera );

#ifdef PS2
	GSState__Clear();
#endif //PS2
	RenderState__ForceAll();	// on ps2, we aren't sure of the gs state at this point? some gs commands get baked into display lists, have to account for them
	RenderState__Default();

#ifdef GCN
	g_ShaderMgr.Reset( );
	
	TextureState__RequestUnit( 0);
	MatrixStack__Mode( MS_TEXTURE);
	MatrixStack__Push( );
	MatrixStack__Identity( );

#endif

	// switch to all positive dither matrix
#ifdef PS2
	GSState__Set(SCE_GS_DIMX, GSState.POSDIMX);
#endif

	/* we are rendering through draw arrays */

	RenderState__Request( RS_PRIMITIVE, RS_PRIMITIVE_DRAWARRAY );

	/* most particles are double sided */

	RenderState__Request( RS_CULL, RS_CULL_NONE );

#ifdef PS2
	ParticleMgr__PS2PrepRender();
#endif

	/* save projection matrix-- on some platforms it gets modified to implement z-bias */

	MatrixStack__Mode(MS_PROJECTION);
	MatrixStack__Push();

	/* particle z-bias is implemented by changing the projection matrix so that the calculated z-buffer values */
	/* take care of the bias. in effect, it is like drawing the particle closer to the camera than it actually */
	/* is placed, so that the z-buffer test takes care of the z-bias. this is only for particles that don't write */
	/* to the z-buffer, so that we don't get false z-buffer values. on ps2 we adjust a value in world-to-clip */
	/* matrix and pass it to particle rendering vu code to shove into the right place. see code at bottom of */
	/* ParticleMgr__PS2PrepRender */

#ifdef PS2
	ParticleMgr.mZBiasProjection = *(Matrix4x4 *) PS2Renderer::GetWorldToClip();
#else //PS2
	ParticleMgr.mZBiasProjection = *MatrixStack__Get(MS_PROJECTION);
#endif //PS2
	ParticleMgr.vZBiasProjectionRow3 = ParticleMgr.mZBiasProjection.GetRow3();
	ParticleMgr.fZBiasZShift = ParticleMgr.vZBiasProjectionRow3.z();

	/* set up pointer into data lists (e.g. vertex list). in immediate mode, this will always be reset to 0 once a list is */
	/* rendered. in retained mode (ps2 and gamecube), ParticleMgr__RenderList will update this. ps2 is double buffered, */
	/* gamecube is single-buffered */

	int nListIndex = 0;

#ifdef GCN
	ParticleMgr__GCNPrepGeomLists( nListIndex);
#endif
	/* loop through the loaded particle data files */

	for (int i = 0; i < MAX_PARTICLE_DATA_FILES; i++)
	{
		/* data loaded? */

		if (ParticleMgr.DBHandle[i] == INVALID_HANDLE)
			continue;

		/* render this list */

		ParticleMgr__RenderList(i, nSceneIndex);
	}

	/* restore dither matrix */
#ifdef PS2
	GSState__Set(SCE_GS_DIMX, GSState.DIMX);
#endif

	/* restore projection matrix */

	MatrixStack__Mode(MS_PROJECTION);
	MatrixStack__Pop();

#ifdef GCN
	TextureState__RequestUnit( 0);
	MatrixStack__Mode( MS_TEXTURE);
	MatrixStack__Pop( );
#endif

	/* color ends up in indeterminate state */

	RenderState__Force( RS_BASECOLOR );

	/* z-buffer back on */

	RenderState__Request( RS_ZWRITE, RS_TRUE );

	/* maybe restore lights */

	if (ParticleMgr.bAdjustLights) 
		g_lightSet.restoreLights();
}

// prepare to render a bunch of different particles organzied into sublists
void ParticleMgr__PrepDrawList()
{
#if defined( GCN )
	GCNParticle__SetupTEV( NO_BIND);
#elif defined( PS2 )
	ParticleMgr__PS2PrepDrawList();
#else
	DrawArray__VectorPointer( sizeof ( Vector3 ) / sizeof ( float ), DA_FLOAT, 0, ParticleMgr.pVertexList );
	DrawArray__Enable( DA_VECTOR );
	DrawArray__TexturePointer( 2, DA_FLOAT, 0, ParticleMgr.pUVList );
	DrawArray__Enable( DA_TEXTURE );
	DrawArray__ColorPointer( 4, DA_UBYTE, 0, ParticleMgr.pColorList );
	DrawArray__Enable( DA_COLOR );
#endif
}

// finish rendering a bunch of different particles
void ParticleMgr__FlushDrawList( int nDataIndex )
{
#ifdef PS2
	ParticleMgr__PS2FinishDrawList(nDataIndex);
#endif
}

// prepare to render a set of particles that Particle__Compare says are the same type.
int ParticleMgr__PrepDrawSubList( ts_ParticleSource* pParticleSource )
{
	int nMaxParticlesInSublist = PARTICLE_LIST_SIZE;

	// dont handle any flags that arent in PARTICLE_STATE_FLAGS within this routine.  its bad since the particles
	// arent sorted by them.
	u32 u32State = pParticleSource->u32Flags & PARTICLE_STATE_FLAGS;

#if defined( GCN )
	// have to set this to a reasonable value.  on GCN, it will vary
	// depending on how many entries in the main list have been used
	// so far.

	static Vector2 vParticleUV[4] = {Vector2(0.0f, 0.0f), Vector2(1.0f, 0.0f), Vector2(1.0f, 1.0f), Vector2(0.0f, 1.0f)};

	ParticleMgr.pVertexList = (Vector3 *)g_pFrameData->memAlloc( PARTICLE_LIST_SIZE * 4 * sizeof(Vector3));	// each particle contributes 4 vertices
	ParticleMgr.pUVList = vParticleUV;																								// each particle uses same set of UVs
	ParticleMgr.pColorList = (ts_bRGBA *)g_pFrameData->memAlloc( PARTICLE_LIST_SIZE * 1 * sizeof(ts_bRGBA));	// each particle contributes 1 color

	DrawArray__VectorPointer( sizeof (Vector3) / sizeof (float), DA_FLOAT, 0, ParticleMgr.pVertexList);
	DrawArray__TexturePointer( 2, DA_FLOAT, 0, ParticleMgr.pUVList);
	DrawArray__ColorPointer( 4, DA_UBYTE, 0, ParticleMgr.pColorList);

#elif defined( PS2 )
#elif defined( OPENGL )
	/* do we need normals for lighting? */

	ASSERT(PARTICLE_STATE_FLAGS & PARTICLE_LIGHTED);	// flag must be in state flags in order for us to check it, duh
	if ((u32State & PARTICLE_LIGHTED) && (RenderMgr__LightingOn()))
	{
		DrawArray__NormalPointer( DA_FLOAT, 0, ParticleMgr.pNormalList );
		DrawArray__Enable( DA_NORMAL );
		RenderState__Request( RS_LIGHTING, RS_TRUE );
		RenderState__Request( RS_TEXTURE_BRIGHTNESS, RenderState__ftoi( 2.0f ) );
		RenderState__Request( RS_CULL, RS_CULL_BACK );	// lighted particles get two quads each-- a front and back face
	}
	else
	{
		DrawArray__Disable( DA_NORMAL );
		RenderState__Request( RS_LIGHTING, RS_FALSE );
		RenderState__Request( RS_TEXTURE_BRIGHTNESS, RenderState__ftoi( 1.0f ) );
		RenderState__Request( RS_CULL, RS_CULL_NONE );	// unlighted particles are double-sided
	}

	/* set z-bias-- adjust the z-shift value of the projection matrix and load it up. doing z-bias this */
	/* way is pretty slippery-- its effect changes with distance to the camera and with far clip distance. */
	/* however, the z-bias gets sent down once with a group of particles, so we can't do a per-particle */
	/* distance to camera fix-up */

	float fZBias = CONVERT_S16_TO_FLOAT(pParticleSource->s16ZBias, MAX_P_ZBIAS);
	fZBias *= ParticleMgr.fZBiasFarClipAdjust;
	ParticleMgr.vZBiasProjectionRow3.z( ParticleMgr.fZBiasZShift - fZBias );
	ParticleMgr.mZBiasProjection.SetRow3(ParticleMgr.vZBiasProjectionRow3);
	MatrixStack__Mode(MS_PROJECTION);
	MatrixStack__Load(&ParticleMgr.mZBiasProjection);
#endif

	/* writing to the z-buffer is important for "solid" particles like leaves, but not writing */
	/* to the z-buffer is good for dust clouds and other mostly transparent particles where you */
	/* want an additive alpha effect */

	ASSERT(PARTICLE_STATE_FLAGS & PARTICLE_ZBUFFER_WRITE);	// flag must be in state flags in order for us to check it, duh
	if (u32State & PARTICLE_ZBUFFER_WRITE)
		RenderState__Request(RS_ZWRITE, RS_TRUE);
	else
		RenderState__Request(RS_ZWRITE, RS_FALSE);

	/* implement alpha ref to fight haloes */

	ASSERT(PARTICLE_STATE_FLAGS & PARTICLE_ALPHA_REF);	// flag must be in state flags in order for us to check it, duh
	
	//yuch-- this will be the new way-- alpha ref value gets multiplied by constant alpha
	//			RenderState__Request( RS_ALPHAREF, (u32State & PARTICLE_ALPHA_REF) ? (int) ((float) RS_ALPHA_REF_NO_HALO_VALUE * RenderState__GetConstantAlpha()) : RS_ALPHA_REF_DEFAULT_VALUE );
	RenderState__Request( RS_ALPHAREF, (u32State & PARTICLE_ALPHA_REF) ? RS_ALPHA_REF_NO_HALO_VALUE : RS_ALPHA_REF_DEFAULT_VALUE );

	/* blend function */

	RenderState__Request( RS_BLENDFUNC, (u32State & PARTICLE_ADDITIVE_RENDER) ? RS_BLENDFUNC_ADDITIVE : RS_BLENDFUNC_DEFAULT );

	return nMaxParticlesInSublist;
}

// finish rendering a set of particles that Particle__Compare says are the same type.
void ParticleMgr__FlushDrawSubList( ts_Particle* pFirstParticle, u32 nParticleCount )
{
#if defined( GCN )
	DCFlushRangeNoSync( (void *)OSRoundDown32B( ParticleMgr.pVertexList), OSRoundUp32B( PARTICLE_LIST_SIZE * 4 * sizeof(Vector3)) + 32);
	DCFlushRangeNoSync( (void *)OSRoundDown32B( ParticleMgr.pColorList), OSRoundUp32B( PARTICLE_LIST_SIZE * 1 * sizeof(ts_bRGBA)) + 32);

	GCNParticle__SetupTEV( pFirstParticle->u16CurrentBind);
	GCNParticle__Render( 0, nParticleCount * 4);

#elif defined( PS2 )

	s16 s16ZBias = pFirstParticle->pSource->s16ZBias;
	float fZBias = CONVERT_S16_TO_FLOAT(s16ZBias, MAX_P_ZBIAS);
	ParticleMgr__PS2SendDrawList(pFirstParticle->pSource->pTextureDatabase, nParticleCount, pFirstParticle->u16CurrentIndex, fZBias );

#else

//#ifndef CONSUMER_BUILD
		if (RenderMgr__WireframeOn())
			TextureState__RequestAll( 0, TS_TYPE_NONE, 0, 0, 0, 0, 0 );
		else
//#endif //CONSUMER_BUILD
			TextureState__RequestAll( 0, TS_TYPE_2D, pFirstParticle->pSource->pTextureDatabase->GetTexture(pFirstParticle->u16CurrentIndex), TS_METHOD_MODULATE, TS_WRAP_CLAMP, TS_WRAP_CLAMP, TS_FILTER_BILINEAR );
		RenderState__Begin();
		DrawArray__Render( DA_QUADS, 0, nParticleCount * 4 );
		RenderState__End();
#endif
}

void ParticleMgr__RenderList(int nDataIndex, int nSceneIndex)
{
	ts_Particle *pParticle;
	u32 nParticleCount;		// particle count for current group of particles
	ts_Particle *pFirstParticle;

	/* quick exit */

	if (ParticleMgr.ActiveList[nDataIndex].pHead == NULL)
		return;

	ParticleMgr__PrepDrawList();

	/* may need to download particle texture set */

	ParticleMgr__LockTextureSet(nDataIndex);

	Frustum__SetupToCheckFrustum(SceneMgr::GetCurrentSceneView()->GetViewFrustum()); 

	pParticle = (ts_Particle *) ParticleMgr.ActiveList[nDataIndex].pHead;	// start at head of particle list to look for unprocessed ones
	while (pParticle)
	{
		pFirstParticle = pParticle;

		nParticleCount = 0;

		u32 nMaxParticlesInSublist = ParticleMgr__PrepDrawSubList( pParticle->pSource );

		/* go through particles, starting at this particle, looking for other ones that match this texture */
		/* and state. if found, add to list */
		/* if not found, we are ready to switch to a new texture/state */

		while ( pParticle && 															// reached end?
				  Particle__Compare(pParticle,  pFirstParticle ) == 0 &&		// same type?
				  nParticleCount < nMaxParticlesInSublist)						// lists full?
		{
			/* draw if this particle is not culled */
			if (! Particle__IsCulled( pParticle, nDataIndex, nSceneIndex ) )
			{
				/* mark as processed */
		
//				pParticle->u32Flags |= PARTICLE_RENDERED;
		
				#ifndef CONSUMER_BUILD
				ParticleMgr.u32RenderedCount++;
				#endif //CONSUMER_BUILD
				
				ParticleMgr__AddParticle(pParticle, nParticleCount);
			}

			/* to next particle */

			pParticle = pParticle->pNext;
		}
		#ifdef _DEBUG
		if(pParticle)
		{
			ASSERTS(Particle__Compare(pParticle,  pFirstParticle ) >= 0, "Particles out of order");
		}
		#endif

		/* finish off this sub list */
		ParticleMgr__FlushDrawSubList(pFirstParticle, nParticleCount);
	}

	ParticleMgr__FlushDrawList( nDataIndex );

	// TJC - notify texturemgr we are done with the current dynamic textures
	ParticleMgr__UnLockTextureSet(nDataIndex);
}

bool Particle__IsCulled(ts_Particle* pParticle, int nDataIndex, int nSceneIndex)
{
	//Make sure this particle should be rendered in this scene
	if (pParticle->u32Flags & (PARTICLE_ONLY_SCENE_1 | PARTICLE_ONLY_SCENE_2 
											| PARTICLE_ONLY_SCENE_3 | PARTICLE_ONLY_SCENE_4))
	{
		if (!(pParticle->u32Flags & (PARTICLE_ONLY_SCENE_1 << nSceneIndex)))
		{
			//Particle is not supposed to be drawn for this scene, so consider it rendered
			return true;
		}

		// TJC - on the PS2, this pass is in microcode...
		// TJC - but its currently disabled in the microcde because of lack of a test case
//#ifndef PS2
		//Make sure the particle is not in the suppression box
		for (int iTmp=0; iTmp < MAX_SUPPRESSION_BOXES; iTmp++)
		{
			if (pSuppressionBox[iTmp] != NULL)
			{				
				if (IsInside(pSuppressionBox[iTmp]->GetBodyToWorld(), pSuppressionBox[iTmp]->GetBodyInWorld(), pParticle->vPos))
				{
					return true;
				}
			}
		}
//#endif //PS2
	}
	/* remove from consideration if out of view */

	GeneralBoundingSphere.Center = pParticle->vPos;
	GeneralBoundingSphere.Radius = Math::Max(pParticle->vSize.x(), pParticle->vSize.y());
	GeneralBoundingSphere.Radius += pParticle->fOrbitRadius;	// usually 0
	if (Frustum__IntersectsWorldSphere(&GeneralBoundingSphere))
		pParticle->u32Flags |= PARTICLE_IN_VIEW_ANY_SCENE;	// in view in at least one scene
	else
		return true;

	// consider invisible particles as rendered

	if ((pParticle->vSize.x() == 0.0f) || (pParticle->vSize.y() == 0.0f) || (pParticle->Color.A == 0))
		return true;

	return false;
}

/* for pigpen, return memory address of requested particle source record. this is so i can update
particle definition in real time from pigpen */

void *ParticleMgr__GetParticleSource(int nDataIndex, int nEntryIndex)
{
	ASSERT(ParticleMgr.DBHandle[nDataIndex] != INVALID_HANDLE);	// if this hits, we are requesting info from an unloaded particle data file
	return(GetRecordAddress(ParticleMgr.pParticleSourceList[nDataIndex], nEntryIndex));
}
void *ParticleMgr__GetEffectSource(int nDataIndex, int nEntryIndex)
{
	ASSERT(ParticleMgr.DBHandle[nDataIndex] != INVALID_HANDLE);	// if this hits, we are requesting info from an unloaded particle data file
	return(GetRecordAddress(ParticleMgr.pEffectList[nDataIndex], nEntryIndex));
}
void *ParticleMgr__GetEmitterSource(int nDataIndex, int nEntryIndex)
{
	ASSERT(ParticleMgr.DBHandle[nDataIndex] != INVALID_HANDLE);	// if this hits, we are requesting info from an unloaded particle data file
	return(GetRecordAddress(ParticleMgr.pEmitterList[nDataIndex], nEntryIndex));
}
void *ParticleMgr__GetSwooshSource(int nDataIndex, int nEntryIndex)
{
	ASSERT(ParticleMgr.DBHandle[nDataIndex] != INVALID_HANDLE);	// if this hits, we are requesting info from an unloaded particle data file
	return(GetRecordAddress(ParticleMgr.pSwooshSourceList[nDataIndex], nEntryIndex));
}
void *ParticleMgr__GetSwooshEmitterSource(int nDataIndex, int nEntryIndex)
{
	ASSERT(ParticleMgr.DBHandle[nDataIndex] != INVALID_HANDLE);	// if this hits, we are requesting info from an unloaded particle data file
	return(GetRecordAddress(ParticleMgr.pSwooshEmitterList[nDataIndex], nEntryIndex));
}

/* get database file handle-- for pigpen */

t_Handle ParticleMgr__GetDBHandle(int nDataIndex)
{
	return(ParticleMgr.DBHandle[nDataIndex]);
}

/* access to texture set-- for pigpen */

TextureArray *ParticleMgr__GetTextureArray(int nDataIndex)
{
	return(ParticleMgr.pTextureArray[nDataIndex]);
}

#ifndef CONSUMER_BUILD
/* statistics */
int ParticleMgr__GetActiveCount(void)
{
	return(ParticleMgr.u32ActiveCount);
}
int ParticleMgr__GetRenderedCount(void)
{
	return(ParticleMgr.u32RenderedCount);
}
#endif //CONSUMER_BUILD

/* return an unused reserved group id in the emitter set */

u16 ParticleMgr__RequestReservedEmitterGroupID(void)
{
	if (ParticleMgr.u16EmitterGroupID >= EMITTER_RESERVED_GROUP_MAX)
	{
		ParticleMgr.bEmitterGroupIDWrapped = TRUE;
		ParticleMgr.u16EmitterGroupID = EMITTER_RESERVED_GROUP_MIN;
	}
	while (ParticleMgr__EmitterGroupUsed(ParticleMgr.u16EmitterGroupID))
		ParticleMgr.u16EmitterGroupID++;
	return(ParticleMgr.u16EmitterGroupID++);
}

/* check if passed group id is used by any emitter */

bool ParticleMgr__EmitterGroupUsed(u16 u16Group)
{
	/* look for it among active emitters */

	ts_ParticleEmitter *pEmitter = (ts_ParticleEmitter *) ParticleMgr.EmitterActiveList.pHead;
	while (pEmitter)
	{
		if (pEmitter->u16Group == u16Group)
			return(TRUE);
		pEmitter = pEmitter->pNext;
	}
	return(FALSE);
}

/* adjust the lights during rendering of lighted particles. only affects real-time-lighted particles! */

void ParticleMgr__AdjustLights(float fAmbientMult, float fDirectionalMult, float fLeadInSeconds, float fSustainSeconds, float fLeadOutSeconds)
{
	/* nothing yet */

	ParticleMgr.bAdjustLights = false;
	ASSERT(fLeadInSeconds >= 0.0f);
	ASSERT(fSustainSeconds >= 0.0f);
	ASSERT(fLeadOutSeconds >= 0.0f);

	/* check for no action required */

	if ((fAmbientMult == 1.0f) && (fDirectionalMult == 1.0f))
		return;
	if ((fLeadInSeconds <= 0.0f) && (fSustainSeconds <= 0.0f) && (fLeadOutSeconds <= 0.0f))
		return;

	/* set up-- fCurrentSeconds get compared to fLeadInSeconds, then fSustainSeconds, then fLeadOutSeconds, */
	/* so add them together */

	ParticleMgr.bAdjustLights = true;
	ParticleMgr.fAmbientMult = fAmbientMult;
	ParticleMgr.fDirectionalMult = fDirectionalMult;
	ParticleMgr.fLeadInSeconds = fLeadInSeconds;
	ParticleMgr.fSustainSeconds = fLeadInSeconds + fSustainSeconds;
	ParticleMgr.fLeadOutSeconds = fLeadInSeconds + fSustainSeconds + fLeadOutSeconds;
	ParticleMgr.fCurrentSeconds = 0.0f;
}

/* one-shots */

void ParticleMgr__SetOneShotEmitterVelTransform(Graphics4x4 *pVelTransform)
{
	ParticleMgr.pEmitterVelTransform = pVelTransform;
}
void ParticleMgr__SetOneShotEmitterTarget(Vector3Packed *pvTarget)
{
	ParticleMgr.pvEmitterTarget = pvTarget;
}
void ParticleMgr__SetOneShotEffectVelTransform(Graphics4x4 *pVelTransform)
{
	ParticleMgr.pEffectVelTransform = pVelTransform;
}
void ParticleMgr__SetOneShotEffectTarget(Vector3Packed *pvTarget)
{
	ParticleMgr.pvEffectTarget = pvTarget;
}
void ParticleMgr__SetOneShotOwnedParticleVelTransform(Graphics4x4 *pVelTransform)
{
	ParticleMgr.pOwnedParticleVelTransform = pVelTransform;
}
void ParticleMgr__SetOneShotOwnedParticleEffectTarget(Vector3Packed *pvTarget)
{
	ParticleMgr.pvOwnedParticleTarget = pvTarget;
}

/*********************** particle routines ************************************/

#ifdef PRINT_PARTICLE_CREATION
void Particle__Create_Print(const int nDataIndex,
									 const ts_ParticleSource* pSource)
{
	/* spit out particle's name. have to find it in chunk so i can get particle index, then look up name in name chunk */

	int nParticlesInChunk = ParticleMgr.pParticleSourceList[nDataIndex]->usRecordCount;
	int nParticleIndex;
	for (nParticleIndex = 0; nParticleIndex < nParticlesInChunk; nParticleIndex++)
	{
		if (pSource == (ts_ParticleSource *) GetRecordAddress(ParticleMgr.pParticleSourceList[nDataIndex], nParticleIndex))
			break;
	}
	ASSERT(nParticleIndex < nParticlesInChunk);	// should always find it
	if (nParticleIndex < nParticlesInChunk)
	{

		ts_RecordList *pRecordList = (ts_RecordList *) dblGetChunkData(DB_PARTICLE_LIST, DEFAULT_CHUNK_KEY_STRING, ParticleMgr.DBHandle[nDataIndex]);
		if (pRecordList)
		{
			char *pRecordName = (char *) GetRecordAddress(pRecordList, nParticleIndex);
			dbgPrint("Creating particle %s, %d active particles\n", pRecordName, ParticleMgr__GetActiveCount());
		}
	}
}
#endif

static Vector3 Particle__Create_GeneratePosition(
	const ts_ParticleSource *pSource,				// particle source data
	const Vector3Packed *pvPos,						// world position to create at, can be NULL (if so, will use particle source data)
	const Vector3Packed *pvVelUnitVector)			// velocity to create at, can be NULL (if so, will use particle source data)
{
	Vector3 vParticlePos = pSource->vInitialPos;
	if (pvPos)
		vParticlePos += *pvPos;
	if (pSource->u16PosRandom != 0)
	{
		float fHalfCubeSize = CONVERT_U16_TO_FLOAT(pSource->u16PosRandom, MAX_P_POS_RANDOM);
		Vector3 vPosRandom;
		vPosRandom.x( Particle__fRandomValue(fHalfCubeSize, RANDOM_CENTER_ON_ZERO));
		vPosRandom.z( Particle__fRandomValue(fHalfCubeSize, RANDOM_CENTER_ON_ZERO));

		/* if we are restricting position to a plane, don't add random value to y-pos, and rotate according to velocity */
		/* unit vector. if no unit vector, assume ground plane */

		if (pSource->u32Flags & PARTICLE_RANDOM_POS_PLANE)
		{
			/* if no velocity, we're done-- just leave it in xz ground plane. also, if velocity is straight up or */
			/* straight down, leave it xz ground plane */

			vPosRandom.y(0.0f);
			if ((pvVelUnitVector) && (pvVelUnitVector->y() < 0.99f) && (pvVelUnitVector->y() > -0.99f))
			{
				/* we have a velocity vector. calculate transform matrix that will transform unit vector Y (0, 1, 0) */
				/* to the passed unit vector. we have the input vector pEmitter->pvUp, think of it as vA. then calculate */
				/* vB = Y - (vA * (vA dot Y)), and vC = vB x vA. then the transform matrix is: */
				/*
				/*		vB.x()	vB.y(), vB.z()
				/*		vA.x()	vA.y(), vA.z()
				/*		vC.x()	vC.y(), vC.z()
				/*
				/* vA dot Y = (vA.x(), vA.y(), vA.z()) dot (0, 1, 0) = vA.y() */

				Vector3 vB;
				Vector3 vC;

				vB = Vector3(0.0f, 1.0f, 0.0f) - *pvVelUnitVector * pvVelUnitVector->y();
				vB.Normalize();
				vC = Vector3::Cross(vB, *pvVelUnitVector);

				/* fill in transform matrix */								  
				DirCos3x3 mTransform;
				mTransform.e00 = vB.x();
				mTransform.e01 = vB.y();
				mTransform.e02 = vB.z();
				mTransform.e10 = pvVelUnitVector->x();
				mTransform.e11 = pvVelUnitVector->y();
				mTransform.e12 = pvVelUnitVector->z();
				mTransform.e20 = vC.x();
				mTransform.e21 = vC.y();
				mTransform.e22 = vC.z();

				/* and put rotation-plane vector through */

				vPosRandom = vPosRandom * mTransform;
			}
		}
		else
		{
			/* random ypos too */

			vPosRandom.y(Particle__fRandomValue(fHalfCubeSize, RANDOM_CENTER_ON_ZERO));
		}

		/* add calculated randomness into position */

		vParticlePos += vPosRandom;
	}
	return vParticlePos;
}

bool Particle__TestAgainstRestrictingPlanes(Vector3CRef vParticlePos, const CPlaneList* pPlaneList)
{
	for (int i = 0; i < pPlaneList->d_numPlanes; i++)
	{
		/* if particle is on outside of any plane, it gets rejected */

		if ( pPlaneList->d_planes[ i ].IsFacing( vParticlePos ) )
			return FALSE;
	}
	return TRUE;
}

bool Particle__GetWaterHeightAtPoint(Vector3CRef vParticlePos, float& result)
{
	/* check tyler's water system first */

	Segment Seg;
	Seg.start = vParticlePos;
	Seg.start.y(Seg.start.y() - 1.0f);
	Seg.end = vParticlePos;
	Seg.end.y(Seg.end.y() + 1.0f);
	Vector3 vPoint;
	int pool = g_waterMgr.Test(Seg, &vPoint);
	if (pool != -1)
		result = vPoint.y();
	else
	{
		/* check food's water */

		AnimatedHeightField::Collision collision;
		if (g_aniHFMgr.Test(Seg, collision))
			result = collision.intersection.y();
		else
			/* no water found to float on, don't create particle */
			return FALSE;
	}
	return TRUE;
}

/* create a particle */

t_Handle Particle__Create(
	const int nDataIndex,								// which of possibly-multiply-loaded particle data files to use
	const Vector3Packed *pvPos,						// world position to create at, can be NULL (if so, will use particle source data)
	const Vector3Packed *pvVelUnitVector,			// velocity to create at, can be NULL (if so, will use particle source data)
	const float *pfVelMagnitude,						// velocity to create at, can be NULL (if so, will use particle source data)
	const Vector3Packed *pvAdditiveVel,				// additive velocity to use, can be NULL (if so, no additive velocity)
	const u32 u32ForceFlags,							// forced bits for ts_Particle.u32Flags
	const float fFirstAdvanceTime,					// first Particle__Advance time slice, 0.0 is a good value
	const float fScale,									// scale to apply to particle size
	const float fConstantAlpha,						// overall alpha to multiply against alpha envelope
	const float fColorMult,								// overall value to multiply against color envelope
	const float fActivationDelay,						// activation delay-- particle will go active when this counts down to 0
	const ts_ParticleSource *pSource,				// particle source data
	const int iSceneIndex,								// which scene particle should appear in, -1 == all scenes
	const Graphics4x4 *pVelTransform,				// possible transform for particle's initial velocity
	const Vector3Packed *pvTarget,					// possible target for particle to move toward
	const CPlaneList* pPlaneList,					// restricting plane list
	const SplinePath *pPath,							// path for particle to follow
	const ts_ParticleEmitter *pFluxEmitter)		// pointer to flux emitter for flux particles
{
#ifdef PRINT_PARTICLE_CREATION
	Particle__Create_Print( nDataIndex, pSource );
#endif //PRINT_PARTICLE_CREATION

	/* grab a particle record. this always adds it to the pending list. if fActivationDelay is 0, then */
	/* it will be moved to the active list during the next ParticleMgr__Advance. I don't want particles */
	/* to render until they've been through an advance loop */

	ts_Particle *pParticle = ParticleMgr__AllocParticle(nDataIndex);
	if (pParticle == NULL)
		return(INVALID_HANDLE);
	pParticle->nDataIndex = nDataIndex;		// record source data file
	pParticle->u32Flags = u32ForceFlags;	// advanced use-- may want to force some flag values, like PARTICLE_NO_MOVE

	/* position. always start with pigpen-defined initial position. this is useful for raising effects off */
	/* the floor so they don't clipped by ground geometry, like dust clouds. calculate this first because */
	/* particle may be culled by plane list */
	Vector3 vParticlePos = Particle__Create_GeneratePosition( pSource, pvPos, pvVelUnitVector );

	/* test restricting planes */

	if ( pPlaneList && !Particle__TestAgainstRestrictingPlanes(vParticlePos, pPlaneList) )
	{
		ParticleMgr__FreeParticle( pParticle );
		return(INVALID_HANDLE);
	}

	/* particles that float on water don't get created unless there's water nearby to float on */

	if (pSource->u8CollisionFlags & PARTICLE_FLOAT_ON_WATER)
	{
		float height;
		if( ! Particle__GetWaterHeightAtPoint( vParticlePos, height ) )
		{
			ParticleMgr__FreeParticle( pParticle );
			return(INVALID_HANDLE);
		}
		vParticlePos.y( height );
	}

	/* particle has not been advanced */

	pParticle->u32Flags |= PARTICLE_FIRST_ADVANCE;
	
	//If the scene is specified, set flag to only display particle on selected scene
	if (iSceneIndex != -1)
		pParticle->u32Flags |= (PARTICLE_ONLY_SCENE_1 << iSceneIndex);

	/* record data we are using */

	pParticle->pSource = (ts_ParticleSource *) pSource;

	/* typically, no death callback (death callbacks are used for owned particles) */

	pParticle->DeathCallback = NULL;

	/* image */


	if(pSource->u16FirstIndex != pSource->u16LastIndex)
	{
		pParticle->u16CurrentIndex = ParticleRandom.InRange(pSource->u16FirstIndex, pSource->u16LastIndex);
		pParticle->u32Flags |= PARTICLE_NEEDS_TEXTURE_ANIM;
		pParticle->s8IndexInc = 1;
	}
	else
	{
		pParticle->u16CurrentIndex = pSource->u16FirstIndex;
		pParticle->s8IndexInc = 0;
	}


	/* animation speed */

//yuch-- synch one-shots to lifetime?
	if (pSource->u8FPS == 0)
		pParticle->u32Flags &= ~PARTICLE_NEEDS_TEXTURE_ANIM;	// just stay on current frame
	else
	{
		pParticle->fAnimDelay = 1.0f / (float) Particle__nRandomValue(pSource->u8FPS, pSource->u8FPSRandomPct);
		pParticle->fAnimTimer = pParticle->fAnimDelay;
	}

	/* activation delay */

	// TJC - dont set it if its 0, the default is a very small number, so this way we 
	// avoid confusing FreeParticle
	if(fActivationDelay != 0.0f)
		pParticle->fActivationDelay = fActivationDelay;

	/* lifetime */

//yuch-- will already have been converted to floats in ts_ParticleData record??? look for all conversion calls in this section
	pParticle->fLifetime = CONVERT_U8_TO_FLOAT(pSource->u8Lifetime, MAX_P_LIFETIME);
	pParticle->fLifetime = Particle__fRandomValue(pParticle->fLifetime, pSource->u8LifetimeRandomPct);
	pParticle->fOneOverOrigLifetime = 1.0f / pParticle->fLifetime;

	/* leash-- particle dies if it gets too far away from creation position. can be used in combination */
	/* with lifetime-- that is, if both are non-zero, particle will die whichever happens first */

	pParticle->fLeashSquared = CONVERT_U16_TO_FLOAT(pSource->u16Leash, MAX_P_LEASH);
	pParticle->fLeashSquared = Particle__fRandomValue(pParticle->fLeashSquared, pSource->u8LeashRandomPct);
	pParticle->fLeashSquared *= pParticle->fLeashSquared;
	if (pParticle->fLeashSquared != 0.0f)
	{
		ASSERT(pPath == NULL);															// pPath, pFluxEmitter and fLeashSquared are union'd
		ASSERT((pParticle->u32Flags & PARTICLE_FROM_FLUX_EMITTER) == 0);	// pPath, pFluxEmitter and fLeashSquared are union'd
		pParticle->u32Flags |= PARTICLE_LEASHED;
		pParticle->u32Flags &= ~PARTICLE_FROM_FLUX_EMITTER;					// make sure we have no conflict
		pParticle->u32Flags &= ~PARTICLE_FOLLOWS_PATH;							// make sure we have no conflict
	}

	/* path to follow */

	if (pPath)
	{
		ASSERT((pParticle->u32Flags & PARTICLE_LEASHED) == 0);				// pPath, pFluxEmitter and fLeashSquared are union'd
		ASSERT((pParticle->u32Flags & PARTICLE_FROM_FLUX_EMITTER) == 0);	// pPath, pFluxEmitter and fLeashSquared are union'd
		pParticle->pPath = (SplinePath *) pPath;
		pParticle->fPathDistance = 0.0f;
		pParticle->u32Flags |= PARTICLE_FOLLOWS_PATH;
		pParticle->u32Flags |= PARTICLE_NO_MOVE;									// path calculation sets position and velocity
		pParticle->u32Flags &= ~PARTICLE_LEASHED;									// make sure we have no conflict
		pParticle->u32Flags &= ~PARTICLE_FROM_FLUX_EMITTER;					// make sure we have no conflict

		/* random displacement from path-- always moves parallel to path, but at some random distance. can't */
		/* union vCreationPos with another Vector3Packed, so use a macro */

		pParticle->vPathDisplace.Clear();
		if (pSource->u16PosRandom != 0)
		{
			float fHalfCubeSize = CONVERT_U16_TO_FLOAT(pSource->u16PosRandom, MAX_P_POS_RANDOM);
			pParticle->vPathDisplace.X(Particle__fRandomValue(fHalfCubeSize, RANDOM_CENTER_ON_ZERO));
			pParticle->vPathDisplace.Z(Particle__fRandomValue(fHalfCubeSize, RANDOM_CENTER_ON_ZERO));
			if ((pSource->u32Flags & PARTICLE_RANDOM_POS_PLANE) == 0)
				pParticle->vPathDisplace.Y(Particle__fRandomValue(fHalfCubeSize, RANDOM_CENTER_ON_ZERO));
		}
	}

	/* pointer back to flux emitter */

	if (pParticle->u32Flags & PARTICLE_FROM_FLUX_EMITTER)
	{
		ASSERT((pParticle->u32Flags & PARTICLE_LEASHED) == 0);				// pPath, pFluxEmitter and fLeashSquared are union'd
		ASSERT(pPath == NULL);															// pPath, pFluxEmitter and fLeashSquared are union'd
		pParticle->pFluxEmitter = (ts_ParticleEmitter *) pFluxEmitter;
		pParticle->u32Flags &= ~PARTICLE_LEASHED;									// make sure we have no conflict
		pParticle->u32Flags &= ~PARTICLE_FOLLOWS_PATH;							// make sure we have no conflict
	}

	/* record value to use for particle's first advance. this handles the fact that particles */
	/* can be generated at sub-frame resolution to satisy min distance requirements or just frame rate */
	/* inaccuracy */

	pParticle->fFirstAdvanceTime = fFirstAdvanceTime;

	/* point to envelopes */

	ts_ParticleEnvelope *pEnvelope = pParticle->pSource->Envelope;
//yuch-- randomize envelopes. how? i don't want to have to copy envelopes into particles for randomizing there...
// may only be able to do some kind of "add this amount to envelope result before clipping" adjustment

	/* color */

	pParticle->fColorMult = fColorMult;
	pParticle->u32Flags |= PARTICLE_UPDATE_COLOR;	// assume we need to update all color components every frame
	if (pSource->u32Flags & PARTICLE_COLOR_RGB)
	{
		/* separate r, g, b envelopes */

		if ((pEnvelope[RED_ENVELOPE].u8Type == FLAT_ENVELOPE) || (pParticle->fLifetime == 0.0f))	// infinite life always uses first point
		{
			pParticle->Color.R = (int) (MAX_COLOR_VALUE * fColorMult * pEnvelope[RED_ENVELOPE].fSplinePoint0Y);
			pParticle->u32Flags &= ~PARTICLE_UPDATE_RED;
		}
		if ((pEnvelope[GREEN_ENVELOPE].u8Type == FLAT_ENVELOPE) || (pParticle->fLifetime == 0.0f))	// infinite life always uses first point
		{
			pParticle->Color.G = (int) (MAX_COLOR_VALUE * fColorMult * pEnvelope[GREEN_ENVELOPE].fSplinePoint0Y);
			pParticle->u32Flags &= ~PARTICLE_UPDATE_GREEN;
		}
		if ((pEnvelope[BLUE_ENVELOPE].u8Type == FLAT_ENVELOPE) || (pParticle->fLifetime == 0.0f))	// infinite life always uses first point
		{
			pParticle->Color.B = (int) (MAX_COLOR_VALUE * fColorMult * pEnvelope[BLUE_ENVELOPE].fSplinePoint0Y);
			pParticle->u32Flags &= ~PARTICLE_UPDATE_BLUE;
		}
	}
	else
	{
		/* single envelope that interpolates between two colors-- green and blue envelopes are unused */

		pParticle->u32Flags &= ~(PARTICLE_UPDATE_GREEN | PARTICLE_UPDATE_BLUE);
		if ((pEnvelope[RED_ENVELOPE].u8Type == FLAT_ENVELOPE) || (pParticle->fLifetime == 0.0f))	// infinite life always uses first point
		{
			Particle__InterpolateColor(pParticle, pEnvelope[RED_ENVELOPE].fSplinePoint0Y);
			pParticle->u32Flags &= ~PARTICLE_UPDATE_RED;
		}
	}

	/* alpha is separate */

	pParticle->fConstantAlpha = fConstantAlpha;
	if ((pEnvelope[ALPHA_ENVELOPE].u8Type == FLAT_ENVELOPE) || (pParticle->fLifetime == 0.0f))	// infinite life always uses first point
	{
		pParticle->Color.A = (int) (MAX_COLOR_VALUE * fConstantAlpha * pEnvelope[ALPHA_ENVELOPE].fSplinePoint0Y);
		pParticle->u32Flags &= ~PARTICLE_UPDATE_ALPHA;
	}

	/* record scale in case we need it later for spawning */

	pParticle->fScale = fScale;

	/* size */

	pParticle->u32Flags |= PARTICLE_UPDATE_SIZE;	// assume we need to
																// update size every
																// frame
	
	pParticle->vSize100.x(
		Particle__fRandomValue(
			CONVERT_U16_TO_FLOAT(pSource->u16XSize, MAX_P_SIZE),	// value at 100%
			pSource->u8XSizeRandomPct)
		* fScale 	// maybe scale it
		* 0.5f);	// * 0.5 since we add/subtract to center pos
	if ((pEnvelope[X_SIZE_ENVELOPE].u8Type == FLAT_ENVELOPE) || (pParticle->fLifetime == 0.0f))	// infinite life always uses first point
	{
		/* flat envelope, set to final value */

		pParticle->vSize.x( pParticle->vSize100.x() * pEnvelope[X_SIZE_ENVELOPE].fSplinePoint0Y );
		pParticle->u32Flags &= ~PARTICLE_UPDATE_X_SIZE;
	}

	/* y-size-- if aspect ratio is maintained throughout randomness, just use x-size. if */
	/* aspect ratio is generally maintained, pSource->u8YSizeEnvelope will */
	/* point to x-size envelope instead of y-size envelope (which is empty, anyway) */

	if (pSource->u32Flags & PARTICLE_RANDOM_SIZE_MAINTAIN_ASPECT_RATIO)
	{
		pParticle->vSize100.y( pParticle->vSize100.x() );
		pParticle->vSize.y( pParticle->vSize.x() );
		if ((pParticle->u32Flags & PARTICLE_UPDATE_X_SIZE) == 0)
			pParticle->u32Flags &= ~PARTICLE_UPDATE_Y_SIZE;
	}
	else
	{
		pParticle->vSize100.y(
			Particle__fRandomValue(
				CONVERT_U16_TO_FLOAT(pSource->u16YSize, MAX_P_SIZE), // value at 100%
				pSource->u8YSizeRandomPct)
			* fScale	// maybe scale it
			* 0.5f);	// * 0.5 since we add/subtract to center pos
		if ((pEnvelope[pSource->u8YSizeEnvelope].u8Type == FLAT_ENVELOPE) || (pParticle->fLifetime == 0.0f))	// infinite life always uses first point
		{
			/* flat envelope, set to final value */

			pParticle->vSize.y(pParticle->vSize100.y() * pEnvelope[pSource->u8YSizeEnvelope].fSplinePoint0Y);
			pParticle->u32Flags &= ~PARTICLE_UPDATE_Y_SIZE;
		}
	}

	/* record calculated position, and creation position for possible leash check */

	pParticle->vPos = vParticlePos;
	if (pParticle->u32Flags & (PARTICLE_LEASHED | PARTICLE_FROM_FLUX_EMITTER))
		pParticle->vCreationPos = vParticlePos;

	/* velocity */

	Vector3 vVelUnitVector;
	if (pvVelUnitVector)
		vVelUnitVector = *pvVelUnitVector;
	else
	{
		/* not overiding direction, grab unit vector from particle source data */

		vVelUnitVector.x( CONVERT_S8_TO_FLOAT(pSource->s8InitialVelX, 1.0f) );
		vVelUnitVector.y( CONVERT_S8_TO_FLOAT(pSource->s8InitialVelY, 1.0f) );
		vVelUnitVector.z( CONVERT_S8_TO_FLOAT(pSource->s8InitialVelZ, 1.0f) );
	}
	float fVelMagnitude;
	if (pfVelMagnitude)
		fVelMagnitude = *pfVelMagnitude;
	else
	{
		/* not overiding magnitude, use source data */

		fVelMagnitude = pSource->fInitialVel;
	}

	/* randomize initial magnitude */

	fVelMagnitude = Particle__fRandomValue(fVelMagnitude, pSource->u8VelRandomPct);

	/* randomize unit vector to fill cone. it's really two cones, one inside the other. the */
	/* randomization selects a degree of "unfolding" between the min and max values. if the */
	/* min value is 0, then we get a full cone. if the min and max value are equal, then we */
	/* always get that angle-- for example, you could use min = max = 90 to get spray in a */
	/* circle perpendicular to the initial velocity */

	if ((pSource->u8VelConeStart != 0) || (pSource->u8VelConeDelta != 0))
	{
		Graphics4x4 mTransform;
		Graphics4x4 mTransform2;
		float fRadians;
		Vector3 vB;
		Vector3 vC;
		static Vector3 vUnitVector100(1.0f, 0.0f, 0.0f);

		/* calculate randomizing matrix-- this will transform unit vector (1, 0, 0) into a cone centered on */
		/* the positive x-axis */

		fRadians = ParticleRandom.UpTo(Math::TwoPi);	// 0.0 to 2PI
		mTransform.SetXRotation(fRadians);
		fRadians = CONVERT_U8_TO_FLOAT(pSource->u8VelConeStart, MAX_P_VEL_CONE);
		if (pSource->u8VelConeDelta != 0)
		{
			float fDeltaRadians = CONVERT_U8_TO_FLOAT(pSource->u8VelConeDelta, MAX_P_VEL_CONE);
			fRadians += ParticleRandom.UpTo(fDeltaRadians);
		}
		mTransform.ReverseCatYRotation(fRadians);

		/* we have gimbal lock issues with input vector = (1, 0, 0) (vB ends up (0, 0, 0)) */
      if (Math::Equal(vVelUnitVector.x(), 1.0f))
			mTransform2.Identity();   // need to transform from (1, 0, 0) to (1, 0, 0)-- identity 
		else if (Math::Equal(vVelUnitVector.x(), -1.0f))
			mTransform2.SetYRotation(Math::Pi);      // transform from (1, 0, 0) to (-1, 0, 0), 180 degree y-rotation should do it
		else
		{
			/* calculate transform matrix that will transform unit vector X (1, 0, 0) to the passed */
			/* unit vector. we have the input vector vVelUnitVector, think of it as vA. then calculate */
			/* vB = X - (vA * (vA dot X)), and vC = vA x vB. then the transform matrix is: */
			/*
			/*                      vA.x()    vA.y(), vA.z()
			/*                      vB.x()    vB.y(), vB.z()
			/*                      vC.x()    vC.y(), vC.z()
			/*
			/* vA dot X = (vA.x(), vA.y(), vA.z()) dot (1, 0, 0) = vA.x() */
			
			vB = Vector3(1.0f, 0.0f, 0.0f) - vVelUnitVector * vVelUnitVector.x();
			vB.SafeNormalize();
			vC = Vector3::Cross(vVelUnitVector, vB);
			
			/* fill in transform matrix */
			mTransform2.Identity();
			mTransform2.r00 = vVelUnitVector.x();
			mTransform2.r01 = vVelUnitVector.y();
			mTransform2.r02 = vVelUnitVector.z();
			mTransform2.r10 = vB.x();
			mTransform2.r11 = vB.y();
			mTransform2.r12 = vB.z();
			mTransform2.r20 = vC.x();
			mTransform2.r21 = vC.y();
			mTransform2.r22 = vC.z();
		}

		/* and now put the unit vector (1, 0, 0) through both matrices-- the first */
		/* one randomizes it into a cone around the positive x-axis, the second transforms */
		/* it to match the passed velocity direction */

		vVelUnitVector = vUnitVector100 * mTransform;
		vVelUnitVector = vVelUnitVector * mTransform2;
	}

	/* rotate vel unit vector through transform matrix if any. this is to provide a way for the caller */
	/* to align the ejection velocity with an instance's orientation */

	if (pVelTransform)
		vVelUnitVector.FastBasisTransform(vVelUnitVector, *pVelTransform);

	/* set initial velocity, record possible target */

	pParticle->vVel = vVelUnitVector * fVelMagnitude;
	pParticle->fInitVelMagnitude = fVelMagnitude;	// i think this is right-- should be before additive velocity is added
	pParticle->pvTarget = (Vector3Packed*)pvTarget;// TJC - This discards a const qualifier!

	/* possibly add a velocity-- usually velocity of emitter */

	if (pvAdditiveVel)
		pParticle->vVel += *pvAdditiveVel;

	/* for bullet, transform into world space */

	if (pSource->u32Flags & PARTICLE_BULLET)
	{
//yuch-- this matrix should come from scene manager, but have to pick correct scene for player firing shot
		pParticle->vVel.FastBasisTransform(pParticle->vVel, g4CameraToWorld);

		/* and transform initial position too */

		pParticle->vPos = pParticle->vPos * g4CameraToWorld;
	}

	/* rotation */

	if (pSource->u32Flags & PARTICLE_INIT_ROT_RANDOM)
	{
		/* not all particle orientations use all rotation components (e.g. screen orientation */
		/* just uses z-rotation) but no harm in setting them */

		pParticle->vRotRadians.Set( Particle__fRandomValue(Math::TwoPi, 100),
											 Particle__fRandomValue(Math::TwoPi, 100),
											 Particle__fRandomValue(Math::TwoPi, 100));
		STRONG_CHECK_PARTICLE_ROTATION(pParticle->vRotRadians, x);
		STRONG_CHECK_PARTICLE_ROTATION(pParticle->vRotRadians, y);
		STRONG_CHECK_PARTICLE_ROTATION(pParticle->vRotRadians, z);
	}
	else
		memset(&pParticle->vRotRadians, 0, sizeof(Vector3));
	if (pSource->u8RotVelRandomPct == 0)
		pParticle->vRotVelRadians =  pSource->vInitialRotVelRadians;
	else
	{
		pParticle->vRotVelRadians.Set( Particle__fRandomValue(pSource->vInitialRotVelRadians.x(), pSource->u8RotVelRandomPct),
												 Particle__fRandomValue(pSource->vInitialRotVelRadians.y(), pSource->u8RotVelRandomPct),
												 Particle__fRandomValue(pSource->vInitialRotVelRadians.z(), pSource->u8RotVelRandomPct));
	}

	/* flags */

	if ((pParticle->vRotVelRadians.x() != 0.0f) || (pParticle->vRotVelRadians.y() != 0.0f) || (pParticle->vRotVelRadians.z() != 0.0f) || (pParticle->pSource->fRotAccelRadians > 0.0f))
		pParticle->u32Flags |= PARTICLE_HAS_ROTATION;
	if (pSource->u32Flags & PARTICLE_PERPENDICULAR_TO_VELOCITY)
	{
		/* can't have align-to-velocity set or it will trump PARTICLE_HAS_NORMAL */

		ASSERT((pSource->u32Flags & PARTICLE_SOURCE_ALIGN_TO_VELOCITY) == 0);	// pigpen should make sure this is cleared
		pParticle->vNormal = pParticle->vVel;
		pParticle->vNormal.SafeNormalize();
		pParticle->u32Flags |= PARTICLE_HAS_NORMAL;
	}
	if (pSource->u32Flags & PARTICLE_SOURCE_ALIGN_TO_VELOCITY)
		pParticle->u32Flags |= PARTICLE_ALIGN_TO_VELOCITY;

	/* see comment in Particle__Advance by test of PARTICLE_GROUND_SPECIAL_CHECK flag */

	if (((pSource->u32Flags & PARTICLE_ORIENT_MASK) == PARTICLE_ORIENT_GROUND) &&
		 (pSource->u32Flags & PARTICLE_SOURCE_ALIGN_TO_VELOCITY) &&
		 (pSource->fGravity == 0.0f))
	 {
		pParticle->u32Flags |= PARTICLE_GROUND_SPECIAL_CHECK;
	 }

	/* orbit */

	if ((pSource->u8OrbitRadius != 0) || (pSource->s8OrbitRadiusVel != 0))
	{
		pParticle->fOrbitRadius = CONVERT_U8_TO_FLOAT(pSource->u8OrbitRadius, MAX_P_ORBIT_RADIUS);
		pParticle->fOrbitRadius = Particle__fRandomValue(pParticle->fOrbitRadius, pSource->u8OrbitRadiusRandomPct);
		pParticle->fOrbitRadiusVel = CONVERT_S8_TO_FLOAT(pSource->s8OrbitRadiusVel, MAX_P_ORBIT_RADIUS_VEL);
		pParticle->fOrbitRadiusVel = Particle__fRandomValue(pParticle->fOrbitRadiusVel, pSource->u8OrbitRadiusVelRandomPct);
		pParticle->fOrbitRotRadians = 0.0f;
		if (pSource->u32Flags & PARTICLE_ORBIT_INIT_ROT_RANDOM)
			pParticle->fOrbitRotRadians = Particle__fRandomValue(Math::TwoPi, 100);
		pParticle->fOrbitRotVelRadians = CONVERT_S16_TO_FLOAT(pSource->s16OrbitRotVel, MAX_P_ORBIT_ROTVEL);
		pParticle->fOrbitRotVelRadians = Particle__fRandomValue(pParticle->fOrbitRotVelRadians, pSource->u8OrbitRotVelRandomPct);
	}
	else
	{
		pParticle->fOrbitRadius = 0.0f;
		pParticle->fOrbitRadiusVel = 0.0f;
		pParticle->fOrbitRotRadians = 0.0f;
		pParticle->fOrbitRotVelRadians = 0.0f;
	}

	/* floating particles don't move-- they just track water */

	if (pSource->u8CollisionFlags & PARTICLE_FLOAT_ON_WATER)
	{
		pParticle->u32Flags |= PARTICLE_NO_MOVE;
		pParticle->vVel.Clear();

		/* particle rotates to stay flat on water surface if normal is available */

		pParticle->vNormal = vUp;
		pParticle->u32Flags |= PARTICLE_HAS_NORMAL;
		ASSERT((pParticle->u32Flags & PARTICLE_ALIGN_TO_VELOCITY) == 0);	// trumps PARTICLE_HAS_NORMAL, pigpen should make sure it's clear
	}

	/* a very special case-- particles that are spawned underwater and then float back up to collide with the */
	/* the water (like bubbles) can check the water height now and then use the leash distance to kill the */
	/* particle at the proper time. this is good because water height tests are slow, and they're inaccurate */
	/* (on ths ps2, the noise function in the code does not match the noise function on the vu). so this saves */
	/* some calculation */

	if (pSource->u8CollisionFlags & PARTICLE_COLLIDE_WATER)
	{
		/* this is only reasonable if the particle is going to move more or less straight up */

		if ((pSource->fGravity >= 0.0f) && ((pParticle->vVel.LengthXZSquared() == 0.0f) || (pSource->fAccel < 0.0f)))
		{
			/* test water systems. i'm cutting and pasting this code too much! */

			Segment Seg;
			Seg.start = pParticle->vPos;
			Seg.end = pParticle->vPos;
			Seg.end.y(Seg.end.y() + 10.0f);	// check up to 10 meters directly above particle
			bool bFound = false;
			Vector3 vPoint;
			int pool = g_waterMgr.Test(Seg, &vPoint);
			if (pool != -1)
				bFound = true;
			else
			{
				/* check food's water */

				AnimatedHeightField::Collision collision;
				if (g_aniHFMgr.Test(Seg, collision))
				{
					vPoint = collision.intersection;
					bFound = true;
				}
			}

			/* if found, set leash */

			if (bFound)
			{
				float fLeashSquared = vPoint.Y() - pParticle->vPos.Y();
				fLeashSquared *= fLeashSquared;
				if (pParticle->u32Flags & PARTICLE_LEASHED)
				{
					/* don't override existing leash if shorter */

					pParticle->fLeashSquared = Math::Min(pParticle->fLeashSquared, fLeashSquared);
				}
				else
				{
					/* set leash */

					pParticle->u32Flags |= PARTICLE_LEASHED;
					pParticle->fLeashSquared = fLeashSquared;
				}

				/* skip water collision check-- leash takes care of it */

				pParticle->u32Flags |= PARTICLE_SKIP_WATER_COLLIDE;
			}
		}
	}

	/* maybe piggyback an emitter onto particle */

	pParticle->PiggybackEmitterHandle = INVALID_HANDLE;
	if (pSource->u16PiggybackEmitterIndex != NO_PIGGYBACK_EMITTER)
	{
		/* some settings depend on flags */

		Vector3Packed *pvEmitterVel = NULL;
		bool bStationary = true;
		if ((pParticle->u32Flags & PARTICLE_NO_MOVE) == 0)
		{
			/* particle moves, so emitter moves with it */

			pvEmitterVel = &pParticle->vVel;
			bStationary = false;
		}
		Vector3Packed *pvEjectVel = NULL;
		bool bEjectVelChanges = false;
		if ((pSource->u32Flags & PARTICLE_EJECT_MASK) == PARTICLE_EJECT_PARTICLE)
		{
			/* use particle's velocity as emitter ejection velocity */

			pvEjectVel = &pParticle->vVel;
			bEjectVelChanges = true;

			/* if no velocity, back to none */

			if (pvEjectVel->LengthSquared() == 0.0f)
			{
				pvEjectVel = NULL;
				bEjectVelChanges = false;
			}
		}

		/* create piggyback emitter */

		pParticle->PiggybackEmitterHandle = ParticleEmitter__CreateFromRecord(
			pParticle->nDataIndex,													//int nDataIndex
			pSource->u16PiggybackEmitterIndex,									//int nEmitterID
			&pParticle->vPos,															//Vector3 *pvEmitterPos
			pvEmitterVel,																//Vector3 *pvEmitterVel
			pvEjectVel,																	//Vector3 *pvEjectVel
			bEjectVelChanges,															//bool bEjectVelChanges
			(Vector3Packed*)&vUp,													//Vector3 *pvUp
			bStationary,																//bool bStationary
			EMITTER_DEFAULT_GROUP,													//u16 u16Group /*= EMITTER_DEFAULT_GROUP*/
			iSceneIndex);																//int iSceneIndex /*= -1 */

		/* a couple more things */

		if (pParticle->PiggybackEmitterHandle != INVALID_HANDLE)
		{
			/* point back to particle */

			ts_ParticleEmitter *pEmitter = PARTICLE_EMITTER_GET_RECORD(pParticle->PiggybackEmitterHandle);
			pEmitter->pParentParticle = pParticle;

			/* emitter starts out deactivated, activates when particle moves from pending to active list. this */
			/* handles not starting up the emitter if the particle has an activation delay */

			pEmitter->u32Flags &= ~EMITTER_ACTIVE;

			/* inherit use-os-timer flag */

			if (pParticle->u32Flags & PARTICLE_USE_OS_TIMER)
				pEmitter->u32Flags |= EMITTER_USE_OS_TIMER;

			/* emitters that piggyback onto an orbiting particle need some adjustment */

			if ((pParticle->fOrbitRadius != 0.0f) || (pParticle->fOrbitRadiusVel != 0.0f))
			{
				pEmitter->u32Flags |= EMITTER_PIGGYBACKS_ORBITER;
				pEmitter->pvPos = &pEmitter->vStationaryPos;	// emitter isn't stationary, but we'll shove calculated position in vStationaryPos
			}
		}
	}

	/* success */

	return(pParticle->Handle);
}

void Particle__RemoveAsGuess(ts_Particle* pParticle)
{
	Texture* texture = pParticle->pSource->pTextureDatabase->GetTexture(pParticle->u16CurrentIndex);

	// set initial search point
	if(texture->GetUserData() == pParticle)
	{
		if(pParticle->pPrev && Particle__Compare(pParticle, pParticle->pPrev) == 0)
			texture->SetUserData(pParticle->pPrev);
		else if(pParticle->pNext && Particle__Compare(pParticle, pParticle->pNext) == 0)
			texture->SetUserData(pParticle->pNext);
		else
			texture->SetUserData(NULL);
	}
}

/* add a particle into the active list in correct place */

int Particle__Compare(ts_Particle* pLHS, ts_Particle* pRHS)
{
	if ( pLHS->u16CurrentIndex < pRHS->u16CurrentIndex )
		return -1;
	else if ( pLHS->u16CurrentIndex > pRHS->u16CurrentIndex )
		return 1;

	if ( pLHS->pSource->pTextureDatabase < pRHS->pSource->pTextureDatabase )
		return -1;
	else if ( pLHS->pSource->pTextureDatabase > pRHS->pSource->pTextureDatabase )
		return 1;

	if ( pLHS->pSource->s16ZBias < pRHS->pSource->s16ZBias )
		return -1;
	else if ( pLHS->pSource->s16ZBias > pRHS->pSource->s16ZBias )
		return 1;

	u32 lhsFlags = pLHS->pSource->u32Flags & PARTICLE_STATE_FLAGS;
	u32 rhsFlags = pRHS->pSource->u32Flags & PARTICLE_STATE_FLAGS;
	if ( lhsFlags < rhsFlags )
		return -1;
	else if ( lhsFlags > rhsFlags )
		return 1;

	return 0;
}

void Particle__SortIntoActive(ts_Particle* pParticle, int nDataIndex)
{
	// find correct place to insert
	Texture* texture = pParticle->pSource->pTextureDatabase->GetTexture(pParticle->u16CurrentIndex);
	ts_Particle* guess = (ts_Particle*)texture->GetUserData();
	int searchDir;

	if(!guess)
	{
		ts_Particle* head = (ts_Particle*)ParticleMgr.ActiveList[nDataIndex].pHead;
		guess = head;
	}

	// in this case, the list is empty.
	if ( !guess )
	{
		DLinkList__AddLinkToHead(&ParticleMgr.ActiveList[nDataIndex], pParticle);
		return;
	}

	// check if we are > or < guess to determine search direction
	searchDir = Particle__Compare( guess, pParticle );
	if(searchDir == 0)
	{
		DLinkList__InsertLinkAfter( &ParticleMgr.ActiveList[nDataIndex], pParticle, guess );
		return;
	}

	// search for insertion point
	while ( guess )
	{
		if ( searchDir < 0 )
		{
			ts_Particle* guessNext = guess->pNext;
			if ( guessNext == NULL || Particle__Compare( guessNext,  pParticle ) >= 0 )
			{
				DLinkList__InsertLinkAfter( &ParticleMgr.ActiveList[nDataIndex], pParticle, guess );
				break;
			}
			else
			{
				guess = guessNext;
			}
		}
		else
		{
			ts_Particle* guessPrev = guess->pPrev;

			if ( guessPrev == NULL || Particle__Compare( guessPrev, pParticle ) <= 0 )
			{
				DLinkList__InsertLinkBefore(&ParticleMgr.ActiveList[nDataIndex], pParticle, guess);
				break;
			}
			else
			{
				guess = guessPrev;
			}
		}
	}

	// remember where we parked if we had to search.
	texture->SetUserData(pParticle);
}

/* pull a particle out of the active list and put it back in in the correct place. */

void Particle__Resort(ts_Particle* pParticle)
{
#ifdef CHECK_LIST_INTEGRITY
	ASSERT(DLinkList__InList(ParticleMgr.ActiveList[pParticle->nDataIndex], pParticle) == TRUE);
#endif //CHECK_LIST_INTEGRITY

	int nDataIndex = pParticle->nDataIndex;

	// remove particle
	DLinkList__RemoveLink( &ParticleMgr.ActiveList[nDataIndex], pParticle );

	// readd in sorted order
	Particle__SortIntoActive( pParticle, nDataIndex );
}


/* advance a particle */

void Particle__Advance(ts_Particle *pParticle, u32 u32DistortionsToApply)
{
	Vector3 vLastPos;
	Vector3 vCollisionPos;
	Vector3 vCollisionNormal;
	u32 u32CollisionMaterial;

	CSceneStats& stats = SceneMgr::GetStats();
	stats.IncParticlesUpdated(1);

	/* advance time to use for this particle */

	float fAdvanceTime = ((pParticle->u32Flags & PARTICLE_USE_OS_TIMER) != 0) ? ParticleMgr.fOSFrameSec : ParticleMgr.fFrameSec;

	/* for new particles, adjust fAdvanceTime to reflect when particle was actually created */

	if (pParticle->fFirstAdvanceTime >= 0.0f)
	{
		fAdvanceTime = pParticle->fFirstAdvanceTime;	// fAdvanceTime == 0.0f is legal
		pParticle->fFirstAdvanceTime = -1.0f;	// one-shot
	}

	/***************** do stuff that might kill particle first to avoid excess calculation *************/

	/* lifetime */

	if (pParticle->fLifetime != 0.0f)	// 0.0 means infinite
	{
		pParticle->fLifetime -= fAdvanceTime;
		if (pParticle->fLifetime <= 0.0f)
		{
			/* maybe spawn an effect at end of life */

			Particle__SpawnEffect(pParticle, pParticle->pSource->u16EndOfLifeSpawnIndex);
			Particle__Kill(pParticle);
			return;	// dead, nothing more to do
		}
	}

	/* leash */

	if ((pParticle->u32Flags & PARTICLE_LEASHED) && ((pParticle->vPos - pParticle->vCreationPos).LengthSquared() > pParticle->fLeashSquared))
	{
		/* maybe spawn an effect at end of life */

		Particle__SpawnEffect(pParticle, pParticle->pSource->u16EndOfLifeSpawnIndex);
		Particle__Kill(pParticle);
		return;	// dead, nothing more to do
	}

	/* flux emitter volume-- no need to check if particle was just created */

	if ((pParticle->u32Flags & (PARTICLE_FROM_FLUX_EMITTER | PARTICLE_FIRST_ADVANCE)) == PARTICLE_FROM_FLUX_EMITTER)
	{
		/* kill particle if it's outside of flux volume */

		if (BoundingBox__ContainsPoint(&pParticle->pFluxEmitter->FluxVolume, pParticle->vCreationPos) == false)
		{
			/* maybe spawn an effect at end of life */

			Particle__SpawnEffect(pParticle, pParticle->pSource->u16EndOfLifeSpawnIndex);
			Particle__Kill(pParticle);
			return;	// dead, nothing more to do
		}
	}

	/* texture animation */

	pParticle->fAnimTimer -= fAdvanceTime;
	if ((pParticle->u32Flags & PARTICLE_NEEDS_TEXTURE_ANIM) && (pParticle->fAnimTimer < 0.0f))
	{
		Particle__RemoveAsGuess(pParticle);

		while ((pParticle->u32Flags & PARTICLE_NEEDS_TEXTURE_ANIM) && (pParticle->fAnimTimer < 0.0f))
		{
			/* update animation frame */
	
			pParticle->u16CurrentIndex += pParticle->s8IndexInc;
	
			/* check for getting out of bounds */
	
			if (pParticle->u16CurrentIndex > pParticle->pSource->u16LastIndex)
			{
				switch (pParticle->pSource->u32Flags & PARTICLE_ANIM_MASK)
				{
				case PARTICLE_ANIM_LOOP:
					pParticle->u16CurrentIndex = pParticle->pSource->u16FirstIndex;
					break;
				case PARTICLE_ANIM_PING_PONG:
					pParticle->u16CurrentIndex = pParticle->pSource->u16LastIndex - 1;
					pParticle->s8IndexInc = -1;
					break;
				case PARTICLE_ANIM_ONE_SHOT:
					pParticle->u16CurrentIndex = pParticle->pSource->u16LastIndex;
					pParticle->u32Flags &= ~PARTICLE_NEEDS_TEXTURE_ANIM;
					break;
				}
			}
			else if (pParticle->u16CurrentIndex < pParticle->pSource->u16FirstIndex)
			{
				ASSERT((pParticle->pSource->u32Flags & PARTICLE_ANIM_MASK) == PARTICLE_ANIM_PING_PONG);
				pParticle->u16CurrentIndex = pParticle->pSource->u16FirstIndex + 1;
				pParticle->s8IndexInc = 1;
			}
	
			/* update frame delay */
	
			pParticle->fAnimTimer += pParticle->fAnimDelay;
	
			/* continuously random? */
	
			if (pParticle->pSource->u32Flags & PARTICLE_FPS_CONT_RANDOM)
				pParticle->fAnimDelay = 1.0f / (float) Particle__nRandomValue(pParticle->pSource->u8FPS, pParticle->pSource->u8FPSRandomPct);
		}

		/* readd particle into sorted list */

		Particle__Resort( pParticle );
	}

	/* size */

	if (pParticle->u32Flags & PARTICLE_UPDATE_SIZE)
	{
		if (pParticle->u32Flags & PARTICLE_UPDATE_X_SIZE)
			pParticle->vSize.x( pParticle->vSize100.x() * Particle__EvaluateEnvelope(pParticle, X_SIZE_ENVELOPE));
		if (pParticle->u32Flags & PARTICLE_UPDATE_Y_SIZE)
			pParticle->vSize.y( pParticle->vSize100.y() * Particle__EvaluateEnvelope(pParticle, pParticle->pSource->u8YSizeEnvelope));
	}

	/* color */

	if (pParticle->u32Flags & PARTICLE_UPDATE_COLOR)
	{
		if (pParticle->pSource->u32Flags & PARTICLE_COLOR_RGB)
		{
			/* separate r, g, b envelopes */

			if (pParticle->u32Flags & PARTICLE_UPDATE_RED)
				pParticle->Color.R = (int) (MAX_COLOR_VALUE * pParticle->fColorMult * Particle__EvaluateEnvelope(pParticle, RED_ENVELOPE));
			if (pParticle->u32Flags & PARTICLE_UPDATE_GREEN)
				pParticle->Color.G = (int) (MAX_COLOR_VALUE * pParticle->fColorMult * Particle__EvaluateEnvelope(pParticle, GREEN_ENVELOPE));
			if (pParticle->u32Flags & PARTICLE_UPDATE_BLUE)
				pParticle->Color.B = (int) (MAX_COLOR_VALUE * pParticle->fColorMult * Particle__EvaluateEnvelope(pParticle, BLUE_ENVELOPE));
		}
		else
		{
			/* single envelope that interpolates between two colors-- green and blue envelopes are unused */

			if (pParticle->u32Flags & PARTICLE_UPDATE_RED)
				Particle__InterpolateColor(pParticle, Particle__EvaluateEnvelope(pParticle, RED_ENVELOPE));
		}
		if (pParticle->u32Flags & PARTICLE_UPDATE_ALPHA)
			pParticle->Color.A = (int) (MAX_COLOR_VALUE * pParticle->fConstantAlpha * Particle__EvaluateEnvelope(pParticle, ALPHA_ENVELOPE));
	}

	/* maybe adjust velocity to move towards (possibly moving) target */

	if (pParticle->pvTarget)
	{
		ASSERT((pParticle->u32Flags & PARTICLE_NO_MOVE) == 0);

		/* calculate new velocity */

		pParticle->vVel = *pParticle->pvTarget - pParticle->vPos;
		pParticle->vVel.SafeNormalize();
		pParticle->vVel *= pParticle->fInitVelMagnitude;
	}

	/* movement/collision */

	if ((pParticle->u32Flags & PARTICLE_NO_MOVE) == 0)
	{
		/* position */

		vLastPos = pParticle->vPos;
		pParticle->vPos += pParticle->vVel * fAdvanceTime;

		/* collision */

		if ((ParticleMgr.CollisionTest) &&
			 ((pParticle->pSource->u8CollisionFlags & (PARTICLE_COLLIDE_WORLD | PARTICLE_COLLIDE_ACTORS)) != 0) &&
			 (vLastPos != pParticle->vPos))
		{
			Vector3 pos = pParticle->vPos;
			if (ParticleMgr.CollisionTest(&pos,																				// Vector3 *pvPos
													&vLastPos,																		// Vector3 *pvLastPos
													&vCollisionPos,																// Vector3 *pvCollisionPos
													&vCollisionNormal,															// Vector3 *pvCollisionNormal
													&u32CollisionMaterial,														// u32 *pu32CollisionMaterial
													(pParticle->pSource->u8CollisionFlags & PARTICLE_COLLIDE_WORLD) != 0, 		// bool testTerrain
													(pParticle->pSource->u8CollisionFlags & PARTICLE_COLLIDE_ACTORS) != 0))		// bool testDynamic
			{
				/* maybe spawn a new effect due to collision */

				Particle__SpawnEffect(pParticle, u32CollisionMaterial, (Vector3Packed*)&vCollisionPos, &vCollisionNormal);

				/* what happens to original particle? */

				switch(pParticle->pSource->u32Flags & PARTICLE_COLLIDE_MASK)
				{
				case PARTICLE_COLLIDE_DIE:
					Particle__Kill(pParticle);
					return;	// skip rest of particle advancement code for this particle
				case PARTICLE_COLLIDE_BOUNCE:
					/* bounce off surface with some energy gain or loss */

//yuch-- things get stuck and fall through floor? works better if you revert to last position. or better,
// pull it back off surface a couple of inches...
pParticle->vPos = vLastPos;
//						pParticle->vPos = vCollisionPos;
					pParticle->vVel = Particle__ReflectVector(pParticle->vVel, vCollisionNormal);
					pParticle->vVel *= CONVERT_U8_TO_FLOAT(pParticle->pSource->u8CollideBounceEnergy, MAX_P_BOUNCE);
					break;
				case PARTICLE_COLLIDE_STICK:
					/* stick in place */

					pParticle->vPos = vCollisionPos;
					pParticle->u32Flags |= PARTICLE_NO_MOVE;
					break;
				case PARTICLE_COLLIDE_CONTINUE:
					/* continue travelling along current velocity, with energy gain or loss */

					pParticle->vVel *= CONVERT_U8_TO_FLOAT(pParticle->pSource->u8CollideBounceEnergy, MAX_P_BOUNCE);
					break;
				}

				/* maybe stop rotating and animating */

				if (pParticle->pSource->u32Flags & PARTICLE_COLLIDE_STOP_ROTATING)
				{
					pParticle->vRotVelRadians.Clear();
					pParticle->u32Flags &= ~PARTICLE_HAS_ROTATION;
				}
				if (pParticle->pSource->u32Flags & PARTICLE_COLLIDE_STOP_ANIMATING)
					pParticle->u32Flags &= ~PARTICLE_NEEDS_TEXTURE_ANIM;
			}
		}

		/* water pool collisions */

		if ((pParticle->pSource->u8CollisionFlags & PARTICLE_COLLIDE_WATER) &&
			 ((pParticle->u32Flags & PARTICLE_SKIP_WATER_COLLIDE) == 0) &&
			 (vLastPos != pParticle->vPos))
		{
			/* i've seen particles pass through water, presumably because water surface is moving as particle is moving */
			/* too. so set segment start point to three times as far back as last particle position. that is, use */
			/* (last pos) - (2 * (current pos - last pos)), which reduces to (3 * last pos) - (2 * current pos) */

			Segment seg;
			seg.start = (3.0f * vLastPos) - (2.0f * pParticle->vPos);
			seg.end = pParticle->vPos;

			Vector3 point;
			int pool = g_waterMgr.Test(seg, &point);

			if (pool != -1)
			{
				// spawn tyler's water sprite effect if requested
				if (pParticle->pSource->u8CollisionFlags & PARTICLE_COLLIDE_CREATE_WATER_SPRITE)
					g_waterMgr.SpawnSpriteEffect(PARTICLE_WATER_POOL_EFFECT, pool, point, pParticle->vVel);

				// for now we always kill these particles when they collide
				Particle__SpawnEffect(pParticle, pParticle->pSource->u16EndOfLifeSpawnIndex);
				Particle__Kill(pParticle);
				return;
			}
			else
			{
				/* try food's water. at this point, just die if particle collides */

				AnimatedHeightField::Collision collision;
				if (g_aniHFMgr.Test(seg, collision))
				{
					Particle__SpawnEffect(pParticle, pParticle->pSource->u16EndOfLifeSpawnIndex);
					Particle__Kill(pParticle);
					return;
				}
			}
		}

		/* gravity and acceleration */

		if (pParticle->pSource->fGravity != 0.0f)
			pParticle->vVel.y(pParticle->vVel.y() + pParticle->pSource->fGravity * fAdvanceTime);
		if ((u32DistortionsToApply != 0) && ((pParticle->pSource->u32Flags & PARTICLE_NO_DISTORTION) == FALSE))
		{
			/* some particles are affected by most distortions but not forcefields. for example, dust clouds kicked up */
			/* by a player's footsteps shouldn't be pushed away by a forcefield around the player */
	
			int nCompatibilityFlags = DISTORTION_AFFECTS_PARTICLES;
			if (pParticle->pSource->u32Flags & PARTICLE_FORCEFIELDS_AFFECT)
				nCompatibilityFlags |= FORCEFIELD_AFFECTS_PARTICLES;
			Vector3 pos = pParticle->vPos;
			Vector3 vel = pParticle->vVel;
			DistortionMgr__ApplyDistortions(&pos, &vel, nCompatibilityFlags);
			pParticle->vPos = pos;
			pParticle->vVel = vel;
		}
		if (pParticle->pSource->fAccel != 0.0f)
		{
			float fAccel = pParticle->pSource->fAccel * fAdvanceTime;
			if (fAccel > 0.0f)
			{
				pParticle->vVel += fAccel;
			}
			else
			{
				/* friction/air resistance is proportional to current velocity. we clamp at */
				/* -0.9 because at low frame rates we can get fAccel == -1.0, which stops velocity */
				/* completely-- that's not what we want */

				if (fAccel <= -0.9f)
					fAccel = -0.9f;	// -1.0 == 0 resultant velocity-- that's not what we want
				pParticle->vVel += pParticle->vVel * fAccel;

				/* here's a very specific case! ground-orient align-to-velocity particles that have friction but */
				/* no gravity slow to a stop. Arctan2 gets squirrelly at very low values, but the align-to-velocity */
				/* records the last-known-good y-rotation that it calculates from the x/z velocities. so if the */
				/* y-velocity ever gets very close to 0, change the particle over to PARTICLE_WAS_ALIGN_TO_VELOCITY. */
				/* note that the particle may still be travelling in x/z, but since friction applies equally to */
				/* x/y/z, heading should never change */

				if ((pParticle->u32Flags & PARTICLE_GROUND_SPECIAL_CHECK) && (Math::Zero(pParticle->vVel.Y())))
				{
					pParticle->u32Flags &= ~PARTICLE_ALIGN_TO_VELOCITY;
					pParticle->u32Flags |= PARTICLE_WAS_ALIGN_TO_VELOCITY;
				}
			}
		}
	}

	/* float on water */

	if (pParticle->pSource->u8CollisionFlags & PARTICLE_FLOAT_ON_WATER)
	{
		Segment Seg;
		Seg.start = pParticle->vPos;
		Seg.start.y(Seg.start.y() - 1.0f);
		Seg.end = pParticle->vPos;
		Seg.end.y(Seg.end.y() + 1.0f);
		Vector3 vPoint;
		int pool = g_waterMgr.Test(Seg, &vPoint);
		if (pool != -1)
			pParticle->vPos.y(vPoint.y());
		else
		{
			/* check food's water */

			AnimatedHeightField::Collision collision;
			if (g_aniHFMgr.Test(Seg, collision))
			{
				pParticle->vPos.y(collision.intersection.y());
				pParticle->vNormal = collision.normal;	// tilt to follow water surface
			}
			else
			{
				/* no water found to float on, kill particle. shouldn't really happen, since particle isn't moving away from water */

				Particle__SpawnEffect(pParticle, pParticle->pSource->u16EndOfLifeSpawnIndex);
				Particle__Kill(pParticle);
				return;	// dead, nothing more to do
			}
		}
	}

	/* follow path */

	if (pParticle->u32Flags & PARTICLE_FOLLOWS_PATH)
	{
		/* move along path */

		ASSERT(pParticle->pPath);
		pParticle->fPathDistance += pParticle->fInitVelMagnitude * fAdvanceTime;
		if (pParticle->fPathDistance >= pParticle->pPath->Length())
		{
			/* reached end of path, time to die. maybe spawn an effect at end of life */

			Particle__SpawnEffect(pParticle, pParticle->pSource->u16EndOfLifeSpawnIndex);
			Particle__Kill(pParticle);
			return;	// dead, nothing more to do
		}

		/* take possible random displacement into account */

		pParticle->vPos = pParticle->pPath->PointFromDistance(pParticle->fPathDistance) + pParticle->vPathDisplace;

		/* set velocity as path tangent */

		pParticle->vVel = pParticle->pPath->TangentFromDistance(pParticle->fPathDistance);
		pParticle->vVel *= pParticle->fInitVelMagnitude;
	}

	/* rotation */

	if (pParticle->u32Flags & PARTICLE_HAS_ROTATION)
	{
		pParticle->vRotRadians += pParticle->vRotVelRadians * fAdvanceTime;
		STRONG_CHECK_PARTICLE_ROTATION(pParticle->vRotRadians,x);	// use strong check because if advance time gets large particle can spin a lot
		STRONG_CHECK_PARTICLE_ROTATION(pParticle->vRotRadians,y);
		STRONG_CHECK_PARTICLE_ROTATION(pParticle->vRotRadians,z);

		/* acceleration */

		if (pParticle->pSource->fRotAccelRadians != 0.0f)
		{
			float fRotAccelRadians = pParticle->pSource->fRotAccelRadians * fAdvanceTime;
			if (fRotAccelRadians > 0.0f)
			{
				pParticle->vRotVelRadians += fRotAccelRadians;
			}
			else
			{
				/* friction/air resistance is proportional to current velocity */

				pParticle->vRotVelRadians += pParticle->vRotVelRadians * fRotAccelRadians;
//yuch--stop once velocity below threshhold
			}
		}
	}

	/* perpendicular-align-to-velocity is supported by using the PARTICLE_HAS_NORMAL flag */

	if (pParticle->pSource->u32Flags & PARTICLE_PERPENDICULAR_TO_VELOCITY)
	{
		pParticle->vNormal = pParticle->vVel;
		pParticle->vNormal.SafeNormalize();
	}

	/* orbit */

	pParticle->fOrbitRadius += pParticle->fOrbitRadiusVel * fAdvanceTime;
	if (pParticle->fOrbitRadius < 0.0f)
	{
		pParticle->fOrbitRadius = 0.0f;
		pParticle->fOrbitRadiusVel = 0.0f;
	}
	pParticle->fOrbitRotRadians += pParticle->fOrbitRotVelRadians * fAdvanceTime;

	/* particle has been advanced at least once */

	pParticle->u32Flags &= ~PARTICLE_FIRST_ADVANCE;
}

/* routines to let user manipulate owned particles */

bool Particle__SetPosition(const t_Handle Handle, const Vector3 *pvPos)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_Particle *pParticle = PARTICLE_GET_RECORD(Handle);
	if (pParticle->Handle != Handle)
		return(FALSE);

	/* update */

	pParticle->vPos = *pvPos;
	return(TRUE);
}

/* retrieve particle's position */

Vector3 Particle__GetPosition(const t_Handle Handle)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_Particle *pParticle = PARTICLE_GET_RECORD(Handle);
	if (pParticle->Handle != Handle)
		return(vZero);

	/* update */

	return(pParticle->vPos);
}

bool Particle__SetVelocity(const t_Handle Handle, const Vector3 *pvVel)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_Particle *pParticle = PARTICLE_GET_RECORD(Handle);
	if (pParticle->Handle != Handle)
		return(FALSE);

	/* update */

	pParticle->vVel = *pvVel;
	return(TRUE);
}

/* retrieve particle's velocity */

Vector3 Particle__GetVelocity(const t_Handle Handle)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_Particle *pParticle = PARTICLE_GET_RECORD(Handle);
	if (pParticle->Handle != Handle)
		return(vZero);

	/* update */

	return(pParticle->vVel);
}

bool Particle__SetOrbitVelocity(const t_Handle Handle, const float fVel)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_Particle *pParticle = PARTICLE_GET_RECORD(Handle);
	if (pParticle->Handle != Handle)
		return(FALSE);

	/* update */

	pParticle->fOrbitRotVelRadians = fVel;
	
	return(TRUE);
}

float Particle__GetOrbitVelocity(const t_Handle Handle)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_Particle *pParticle = PARTICLE_GET_RECORD(Handle);
	if (pParticle->Handle != Handle)
		return(0.0f);

	/* update */

	return pParticle->fOrbitRotVelRadians;
}

bool Particle__SetScale(const t_Handle Handle, const float fScale)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_Particle *pParticle = PARTICLE_GET_RECORD(Handle);
	if (pParticle->Handle != Handle)
		return(FALSE);

	/* update */

	pParticle->vSize.x( pParticle->vSize100.x() * fScale );
	pParticle->vSize.y( pParticle->vSize100.y() * fScale );

	/* from now on, this particle ignores size envelope, if any. this is a bit weird, i know. but owned particles */
	/* typically have infinite lives, and so envelopes aren't very meaningful. we don't want to set the particle's */
	/* vSize100 variable because then subsequent calls to this routine would be screwed up. if we REALLY wanted to */
	/* support size envelopes here, we would need to add a new fOwnedScale variable to the particle record that */
	/* started at 1.0 and was set by this routine, and then used in the size envelope evaluation. the current fScale */
	/* variable records the initial scale at particle creation time, and isn't suitable for use here */

	pParticle->u32Flags &= ~PARTICLE_UPDATE_SIZE;
	return(TRUE);
}

bool Particle__SetRotation(const t_Handle Handle, const Vector3 *pvRotRadians)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_Particle *pParticle = PARTICLE_GET_RECORD(Handle);
	if (pParticle->Handle != Handle)
		return(FALSE);

	/* not if particle is align-to-velocity-- this overrides rotation (calculates it based on velocity) */

	if (pParticle->u32Flags & PARTICLE_ALIGN_TO_VELOCITY)
		return(false);

	/* update */

	pParticle->vRotRadians = *pvRotRadians;
	STRONG_CHECK_PARTICLE_ROTATION(pParticle->vRotRadians,x);
	STRONG_CHECK_PARTICLE_ROTATION(pParticle->vRotRadians,y);
	STRONG_CHECK_PARTICLE_ROTATION(pParticle->vRotRadians,z);

	/* particle now has rotation */

	pParticle->u32Flags |= PARTICLE_HAS_ROTATION;
	return(TRUE);
}

bool Particle__SetRotationVelocity(const t_Handle Handle, const Vector3 *pvRotVelRadians)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_Particle *pParticle = PARTICLE_GET_RECORD(Handle);
	if (pParticle->Handle != Handle)
		return(FALSE);

	/* not if particle is align-to-velocity-- this overrides rotation (calculates it based on velocity) */

	if (pParticle->u32Flags & PARTICLE_ALIGN_TO_VELOCITY)
		return(false);

	/* update */

	pParticle->vRotVelRadians = *pvRotVelRadians;

	/* particle now has rotation */

	pParticle->u32Flags |= PARTICLE_HAS_ROTATION;
	return(TRUE);
}

bool Particle__SetNormal(const t_Handle Handle, const Vector3 *pvNormal)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_Particle *pParticle = PARTICLE_GET_RECORD(Handle);
	if (pParticle->Handle != Handle)
		return(FALSE);

	/* not if particle is align-to-velocity-- this overrides rotation (calculates it based on velocity) */

	if (pParticle->u32Flags & PARTICLE_ALIGN_TO_VELOCITY)
		return(false);

	/* update */

	pParticle->vNormal = *pvNormal;

	ASSERT(Math::Equal(pvNormal->Length(), 1.0f, 0.1f));//ensure that it is more or less a unit normal

	/* particle now has normal */

	pParticle->u32Flags |= PARTICLE_HAS_NORMAL;
	return(TRUE);
}

/* directly set color. assumes color is not coming from envelopes. on ps2, all 4 components are subject to the
0..128 range instead of 0..255, not just the alpha channel */

bool Particle__SetColor(const t_Handle Handle, const ts_bRGBA Color)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_Particle *pParticle = PARTICLE_GET_RECORD(Handle);
	if (pParticle->Handle != Handle)
		return(FALSE);

	/* shouldn't be getting color from envelopes-- if so, will overwrite value passed here */

	ASSERT((pParticle->u32Flags & (PARTICLE_UPDATE_RED | PARTICLE_UPDATE_GREEN | PARTICLE_UPDATE_BLUE | PARTICLE_UPDATE_ALPHA)) == 0);
	pParticle->Color = Color;
	return(true);
}

#ifdef _DEBUG
/* owned particles can be useful for showing trails through the world (like in ballistic motion), but only if they
satisfy certain criteria */

bool Particle__MakeItGoodForTrails(const t_Handle Handle, const float fSeconds)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_Particle *pParticle = PARTICLE_GET_RECORD(Handle);
	if (pParticle->Handle != Handle)
		return(FALSE);

	/* reset lifetime to requested length */

	pParticle->fLifetime = fSeconds;
	pParticle->fOneOverOrigLifetime = 1.0f / pParticle->fLifetime;

	/* can't disappear when off-screen, no gravity, not affected by wind */

	pParticle->pSource->u32Flags |= PARTICLE_NO_VIEW_CULL;
	pParticle->pSource->fGravity = 0.0f;
	pParticle->pSource->u32Flags |= PARTICLE_NO_DISTORTION;
	return(TRUE);
}
#endif //_DEBUG

/* this version is for the game app killing particles created with ParticleMgr__CreateOwnedParticle. the bCallbackOK
parameter lets the caller decide whether the DeathCallback, is any, should fire. the DeathCallback is used to notify
the owner of the particle that the particle has died. typically, callers to this routine will be killing the owner
anyway, so they don't need the callback. so i believe the typical value should be FALSE. however, i want to force
the caller to make an informed choice, so i'm not making it an optional parameter */

bool Particle__Kill(const t_Handle Handle, const bool bCallbackOK)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_Particle *pParticle = PARTICLE_GET_RECORD(Handle);
	if (pParticle->Handle != Handle)
		return(FALSE);

	/* kill it, possibly clearing the callback first */

	if (bCallbackOK == FALSE)
		pParticle->DeathCallback = NULL;
	Particle__Kill(pParticle);
	return(TRUE);
}

/* return a particle to the free list */


void Particle__Kill(ts_Particle *pParticle)
{
	/* maybe kill piggybacked emitter */

	if (pParticle->PiggybackEmitterHandle != INVALID_HANDLE)
		ParticleEmitter__Kill(pParticle->PiggybackEmitterHandle);

	/* notify owner, if any */

	if (pParticle->DeathCallback)
		pParticle->DeathCallback(pParticle->callbackContext, pParticle->Handle);

	/* if it was the last added, nuke that */
	Particle__RemoveAsGuess(pParticle);

	/* move to free list */

	ParticleMgr__FreeParticle( pParticle );
}

Vector3 Particle__ReflectVector(Vector3CRef Vector, Vector3CRef Normal)
{
	//bounce a vector off the surface given by pNormal
	//this algorithm stolen from Rash, kbike.cpp

	float lenV = Vector.Length();

	Vector3 WorkVector = Vector;

	if (!Math::Zero(lenV))
	{
		Vector3 L = Vector;
		Vector3 workNormal = Normal;
		float lDotN;

		L *= (1.0f / -lenV);
		lDotN = Vector3::Dot(workNormal, L);	// L dot N

		workNormal *= (2.0f * lDotN);
		WorkVector = workNormal - L;
		WorkVector *= lenV;
	}
	return WorkVector;
}

void Particle__ReflectUnitVector(Vector3Packed &UnitVector, Vector3CRef Normal)
{
	//bounce a vector off the surface given by pNormal
	//this algorithm stolen from Rash, kbike.cpp

	Vector3 L = UnitVector;
	Vector3 workNormal = Normal;
	float lDotN;

	L *= -1.0f;
	lDotN = Vector3::Dot(workNormal, L);	// L dot N

	workNormal *= (2.0f * lDotN);
	UnitVector = workNormal - L;
}

/* spawn a particle effect from a particle */

void Particle__SpawnEffect(ts_Particle *pParticle, int nIndex, Vector3Packed *pvPos /*= NULL*/, Vector3 *pvCollisionNormal /*= NULL*/)
{
	/* particle may not spawn for this surface type */

	if (nIndex >= pParticle->pSource->u16SpawnEffectCount)
		return;
	if ((pParticle->pSource->SpawnEffect[nIndex].u8Flags & SPAWN_ACTIVE) == 0)
		return;

	/* if caller did not specify a position, use particle position */

	if (pvPos == NULL)
	{
		/* use particle position-- take orbiting particles into account */

		if (pParticle->fOrbitRadius == 0.0f)
			pvPos = &pParticle->vPos;
		else
			pvPos = Particle__CalculateOrbit(pParticle);
	}

	/* determine spawn velocity */

	Vector3Packed vUnitVel;
	Vector3Packed *pvUnitVel = NULL;
	float fVelMagnitude;
	float *pfVelMagnitude = NULL;
	u32 u32ForceFlags = 0;
	switch(pParticle->pSource->SpawnEffect[nIndex].u8Flags & SPAWN_MASK)
	{
	case SPAWN_DEFAULT:
		/* use particle source velocity data (that is, pigpen settings) */

		break;
	case SPAWN_STICK:
		/* set all created particles as no-move */

		u32ForceFlags = PARTICLE_NO_MOVE;
		break;
	case SPAWN_BOUNCE:
	case SPAWN_CONTINUE:
		/* need to base new velocity on spawning particle's velocity */

		vUnitVel = pParticle->vVel;
		vUnitVel.SafeNormalize();
		fVelMagnitude = pParticle->vVel.Length();
		pvUnitVel = &vUnitVel;
		pfVelMagnitude = &fVelMagnitude;

		/* which is it? */

		float fEnergy = CONVERT_U8_TO_FLOAT(pParticle->pSource->SpawnEffect[nIndex].u8BounceEnergy, MAX_P_BOUNCE);
		switch(pParticle->pSource->SpawnEffect[nIndex].u8Flags & SPAWN_MASK)
		{
		case SPAWN_BOUNCE:
			if (pvCollisionNormal)
				Particle__ReflectUnitVector(vUnitVel, *pvCollisionNormal);
			else
				vUnitVel *= -1.0f;	// if no collision, "bounce" means reverse velocity (this is for end-of-life spawn, just to provide some kind of functionality for bounce)
			fVelMagnitude *= fEnergy;
			break;
		case SPAWN_CONTINUE:
			/* continue travelling along current velocity, with energy gain or loss */

			fVelMagnitude *= fEnergy;
			break;
		}
		break;
	}

	/* spawn */

	ParticleMgr__CreateEffect(
		pParticle->nDataIndex,										// effect is in same data set as parent particle
		pParticle->pSource->SpawnEffect[nIndex].u16Effect,	// effect to create
		pvPos,															// world position to create at
		pvUnitVel,														// creation velocity unit direction vector, can be NULL
		pfVelMagnitude,												// creation velocity magnitude, can be NULL
		NULL,																// additive velocity, can be NULL
		0.0f,																// value for initial particle advance
		pParticle->fScale,											// scale to use
		0,																	// no randomness
		pParticle->fConstantAlpha,									// constant alpha-- spawned particles inherit parent's value
		pParticle->fColorMult,										// color multiplier-- spawned particles inherit parent's value
		u32ForceFlags);												// bits to force in ts_Particle.u32Flags on particle creation
}

/* return value of passed envelope at current t value. return value is 0.0 to 1.0 */

float Particle__EvaluateEnvelope(ts_Particle *pParticle, int nIndex)
{
	ts_ParticleEnvelope *pEnvelope = &pParticle->pSource->Envelope[nIndex];
	float fY;

	/* set fraction of lifetime passed for envelope evaluation. at lifetime near 0 we are at end of lifetime, so */
	/* fraction at that point should be 1 */

	ASSERT(pParticle->fLifetime != 0.0f);	// infinite-life particles shouldn't get here, they should pre-load envelope value
	float fT = 1.0f - (pParticle->fLifetime * pParticle->fOneOverOrigLifetime);

	/* what kind of envelope? */

	switch(pEnvelope->u8Type)
	{
	case SPLINE_ENVELOPE:
		{
			float fTSquared = fT * fT;
			float fOneMinusT = 1.0f - fT;
			float fOneMinusTSquared = fOneMinusT * fOneMinusT;

			/* the equation is:

			fY = (fOneMinusT * fOneMinusT * fOneMinusT * pEnvelope->fSplinePoint0Y) + 
				  (3.0f * fOneMinusT * fOneMinusT * fT * pEnvelope->SplinePoint1.y()) +
				  (3.0f * fOneMinusT * fT * fT * pEnvelope->SplinePoint2.y()) +
				  (fT * fT * fT * pEnvelope->fSplinePoint3Y);

			but pEnvelope->SplinePoint1.y and pEnvelope->SplinePoint2.y have already been multiplied by 3.0 in FixParticleSource */

			fY = (fOneMinusTSquared * fOneMinusT * pEnvelope->fSplinePoint0Y) + 
				  (fOneMinusTSquared * fT * pEnvelope->SplinePoint1.y()) +
				  (fOneMinusT * fTSquared * pEnvelope->SplinePoint2.y()) +
				  (fTSquared * fT * pEnvelope->fSplinePoint3Y);
		}
		break;
	case ADSR_ENVELOPE:
		if (fT < pEnvelope->SplinePoint1.x())
			fY = pEnvelope->fSplinePoint0Y + ((pEnvelope->SplinePoint1.y() - pEnvelope->fSplinePoint0Y) * fT / pEnvelope->SplinePoint1.x());
		else if (fT < pEnvelope->SplinePoint2.x())
			fY = pEnvelope->SplinePoint1.y() + ((pEnvelope->SplinePoint2.y() - pEnvelope->SplinePoint1.y()) * (fT - pEnvelope->SplinePoint1.x()) / (pEnvelope->SplinePoint2.x() - pEnvelope->SplinePoint1.x()));
		else
			fY = pEnvelope->SplinePoint2.y() + ((pEnvelope->fSplinePoint3Y - pEnvelope->SplinePoint2.y()) * (fT - pEnvelope->SplinePoint2.x()) / (1.0f - pEnvelope->SplinePoint2.x()));
		break;
	case STRAIGHT_ENVELOPE:
		fY = pEnvelope->fSplinePoint0Y + ((pEnvelope->fSplinePoint3Y - pEnvelope->fSplinePoint0Y) * fT);
		break;
	case FLAT_ENVELOPE:
		ASSERT(FALSE);	// should really be set once at particle creation; it's a waste to constantly evaluate a flat envelope
		return(pEnvelope->fSplinePoint0Y);
	default:
		ASSERT(FALSE);	// unknown envelope type! bad data???
		return(1.0f);
	}

	/* check against clip values */

	if (fY > pEnvelope->fCeilingY)
		return(pEnvelope->fCeilingY);
	if (fY < pEnvelope->fFloorY)
		return(pEnvelope->fFloorY);
	return(fY);
}

/* return a randomized value of the initial value, fValue +/- 0..fRandomValue */

float Particle__fRandomValue(float fValue, int nRandomPctValue)
{
	

	
	ASSERT((nRandomPctValue <= 100) || (nRandomPctValue == RANDOM_CENTER_ON_ZERO));
	if (nRandomPctValue != 0)
	{
		float fMinusOneToOne = ParticleRandom.NegPos();
		if (nRandomPctValue == RANDOM_CENTER_ON_ZERO)
			fValue = fValue * fMinusOneToOne;	// result is -fValue to fValue
		else if (fValue >= 0.0f)
		{
			fValue += fValue * ((float) nRandomPctValue / 100.0f) * fMinusOneToOne;
			if (fValue <= 0.0f)
				fValue = 0.001f;
		}
		else
		{
			fValue += fValue * ((float) nRandomPctValue / 100.0f) * fMinusOneToOne;
			if (fValue >= 0.0f)
				fValue = -0.001f;
		}
	}
	return(fValue);
}

/* return a randomized value of the initial value, nValue +/- 0..nRandomValue */

int Particle__nRandomValue(int nValue, int nRandomPctValue)
{
	ASSERT((nRandomPctValue <= 100) || (nRandomPctValue == RANDOM_CENTER_ON_ZERO));
	if (nRandomPctValue != 0)
	{
		float fMinusOneToOne = ParticleRandom.NegPos();
		if (nRandomPctValue == RANDOM_CENTER_ON_ZERO)
			nValue = (int) ((float) nValue * fMinusOneToOne);	// result is -fValue to fValue
		else
		{
			ASSERT(nValue >= 0);	// since we are clamping at 0, this routine is not useful for negative numbers, though that might be useful sometime
			nValue += (int) ((float) nValue * ((float) nRandomPctValue / 100.0f) * fMinusOneToOne);
			if (nValue < 0)
				nValue = 0;
		}
	}
	return(nValue);
}

/* interpolate between two colors to find current color. 1.0 is all first color, 0.0 is all second color.
this supports the color pair color control-- a single envelope controls interpolation between a start
and an end color */

void Particle__InterpolateColor(ts_Particle *pParticle, float fValue)
{
	ts_fRGB *pColor0 = &pParticle->pSource->Color[0];	// values are in 0.0..255.0 range
	ts_fRGB *pColor1 = &pParticle->pSource->Color[1];

	pParticle->Color.R = (int) (pParticle->fColorMult * (pColor1->R + (fValue * (pColor0->R - pColor1->R))));
	pParticle->Color.G = (int) (pParticle->fColorMult * (pColor1->G + (fValue * (pColor0->G - pColor1->G))));
	pParticle->Color.B = (int) (pParticle->fColorMult * (pColor1->B + (fValue * (pColor0->B - pColor1->B))));
}

/* calculate orbital position of particle. orbiting particles orbit around their own position at a given distance and speed */

Vector3Packed *Particle__CalculateOrbit(ts_Particle *pParticle)
{
	static Vector3 vOrbitPos;	// pointer to this gets returned
	float fCos;
	float fSin;
	Math::SinCos(pParticle->fOrbitRotRadians, fSin, fCos);

	/* what kind of orbit plane? */

	switch(pParticle->pSource->u32Flags & PARTICLE_ORBIT_PLANE_MASK)
	{
	case PARTICLE_ORBIT_ALIGN_TO_VEL:
		if ((pParticle->vVel.x() != 0.0f) || (pParticle->vVel.y() != 0.0f) || (pParticle->vVel.z() != 0.0f))
		{
			vOrbitPos.Set(pParticle->fOrbitRadius * fSin, 0.0f, pParticle->fOrbitRadius * -fCos);	// ok as is for velocity straight up
			Vector3 vVelUnitVector = pParticle->vVel;
			vVelUnitVector.Normalize();
			if (vVelUnitVector.y() < -0.99f)
			{
				/* velocity is straight down, adjust adjustment (can think of it as rotate 180 degrees about x-axis) */

				vOrbitPos.Set(pParticle->fOrbitRadius * -fSin, 0.0f, pParticle->fOrbitRadius * fCos);
			}
			else if (vVelUnitVector.y() < 0.99f)
			{
				/* we have a velocity vector. calculate transform matrix that will transform unit vector Y (0, 1, 0) */
				/* to the passed unit vector. we have the input vector vVelUnitVector, think of it as vA. then calculate */
				/* vB = Y - (vA * (vA dot Y)), and vC = vB x vA. then the transform matrix is: */
				/*
				/*		vB.x()	vB.y(), vB.z()
				/*		vA.x()	vA.y(), vA.z()
				/*		vC.x()	vC.y(), vC.z()
				/*
				/* vA dot Y = (vA.x(), vA.y(), vA.z()) dot (0, 1, 0) = vA.y() */

				Vector3 vB;
				Vector3 vC;
				vB = Vector3(0.0f, 1.0f, 0.0f) - vVelUnitVector * vVelUnitVector.y();
				vB.Normalize();
				vC = Vector3::Cross(vB, vVelUnitVector);

				/* fill in transform matrix */

				DirCos3x3 mTransform;
				mTransform.e00 = vB.x();
				mTransform.e01 = vB.y();
				mTransform.e02 = vB.z();
				mTransform.e10 = vVelUnitVector.x();
				mTransform.e11 = vVelUnitVector.y();
				mTransform.e12 = vVelUnitVector.z();
				mTransform.e20 = vC.x();
				mTransform.e21 = vC.y();
				mTransform.e22 = vC.z();

				/* and put rotation-plane vector through */

				vOrbitPos = vOrbitPos * mTransform;
			}

			/* and add to particle's world pos */

			vOrbitPos += pParticle->vPos;
			break;
		}

		/* if no velocity, fall through to PARTICLE_ORBIT_SCREEN */
	case PARTICLE_ORBIT_SCREEN:
		/* camera looks down negative z-axis in camera space, so set x/y components and pass through camera-to-world */
		/* matrix to get proper delta in world space */

		vOrbitPos.Set(pParticle->fOrbitRadius * fCos, pParticle->fOrbitRadius * fSin, 0.0f);	// this setting gives positive rotation velocity correct sense-- counter-clockwise
		vOrbitPos.FastBasisTransform(vOrbitPos, g4CameraToWorld);
		vOrbitPos += pParticle->vPos;
		break;
	case PARTICLE_ORBIT_GROUND:
		vOrbitPos.Set(pParticle->vPos.x() + pParticle->fOrbitRadius * fSin,
						  pParticle->vPos.y(),
						  pParticle->vPos.z() + pParticle->fOrbitRadius * fCos);
		break;
	}

	/* return result */

	return((Vector3Packed*)&vOrbitPos);
}

/*********************** emitter routines ************************************/

/* create an emitter:

	u32Type....................choose from list in g_ParticleMgr.h
	nEffectID..................type of effect to emit. NO_PARTICLE_EFFECT for temporarily inactive emitter.
										Use ParticleMgr__UpdateEmitter to change effect id
	pvEmitterPos...............points to a Vector3 that the emitter can read at any time to get its current
										position. even stationary emitters need a position
	pvEmitterVel...............if non-NULL, this is the emitter's velocity. for example, a tire emitter
										needs the bike's velocity to support some emitter types. the next two parameters
										use the emitter's velocity
	fEmitterVelMult............is used to adjust the emitted particle effect's velocity based on the emitter's
										velocity-- the faster the emitter, the faster the particles emitted. this is
										only currently functional for the EMITTER_TIRE_SIDE_SPRAY emitter type. in this
										case, pvEjectVel is ignored (ejection velocity is calculated from emitter velocity).
										if non-0, pvEmitterVel must be non-NULL
	fAdditiveEmitterVelMult....controls how much (if any) of the emitter's velocity is added to the created
										particles after they are created. 0.0 for no additive vel. if non-0, pvEmitterVel
										must be non-NULL
	pvEjectVel.................if non-NULL, effect is emitted with this velocity. if NULL, effect uses particle
										source data (defined in pigpen). for example, this would be useful for cannons
										firing cannonballs in different directions
	bEjectVelChanges...........should be TRUE if value pointed to by pvEjectVel changes over time (if it doesn't,
										i can pre-calculate unit velocity vector and velocity magnitude). value is ignored
										for tire spray emitters
	fDelay.....................delay in seconds between emissions
	fMinDistance...............minimum distance between particles. only valid is pvEmitterVel is non-NULL. this
										is the minimum distance in meters between particles. if an emitter is travelling
										fast, it will create out multiple effects per frame if necessary to satisfy
										this condition. value of 0 means skip min distance check
	bStationary................emitter never moves from its spot

	some examples:

	smokestack:	EMITTER_STANDARD,			// u32Type
					<smoke id>,					// nEffectID
					<smokestack position>	// pvEmitterPos
					NULL,							// pvEmitterVel-- it doesn't move
					0.0f,							// fEmitterVelMult
					0.0f,							// fAdditiveEmitterVelMult
					NULL,							// pvEjectVel-- use particle velocity data from pigpen
					FALSE,						// bEjectVelChanges
					<smoke rate>,				// fDelay
					0.0f,							// fMinDistance
					TRUE							// bStationary

	cannon:		EMITTER_STANDARD,			// u32Type
					<cannonball id>,			// nEffectID
					<muzzle position>			// pvEmitterPos
					NULL,							// pvEmitterVel-- it doesn't move
					0.0f,							// fEmitterVelMult
					0.0f,							// fAdditiveEmitterVelMult
					<calculated velocity>,	// pvEjectVel-- calculated cannonball's velocity, depends on cannon angle
					FALSE,						// bEjectVelChanges
					<smoke rate>,				// fDelay
					0.0f,							// fMinDistance
					TRUE							// bStationary

	litterbug:	EMITTER_STANDARD,			// u32Type
					<tin can id>,				// nEffectID
					<driver's window>			// pvEmitterPos
					<car velocity>,			// pvEmitterVel-- car velocity for additive velocity
					0.0f,							// fEmitterVelMult
					0.9f,							// fAdditiveEmitterVelMult-- almost match car's velocity
					<can velocity>,			// pvEjectVel-- calculated to throw the can out the driver's window
					TRUE,							// bEjectVelChanges-- his aim varies from throw to throw
					<can rate>,					// fDelay-- as fast as the driver can drink another one
					0.0f,							// fMinDistance
					FALSE							// bStationary

	tailpipe:	EMITTER_STANDARD,			// u32Type
					<exhaust id>,				// nEffectID
					<tailpipe position>		// pvEmitterPos
					<motorcycle velocity>,	// pvEmitterVel-- bike velocity for additive velocity
					0.0f,							// fEmitterVelMult
					0.7f,							// fAdditiveEmitterVelMult-- follows the bike some
					NULL,							// pvEjectVel-- use pigpen value-- presumably 0, with positive gravity
					FALSE,						// bEjectVelChanges
					<exhaust rate>,			// fDelay-- maybe something like 10
					2.0f,							// fMinDistance-- make sure they some out every 2 meters when bike is moving
					FALSE							// bStationary

	tire spray:	EMITTER_TIRE_SIDE_SPRAY,// u32Type
					<spray id>,					// nEffectID
					<tire contact position>	// pvEmitterPos
					<motorcycle velocity>,	// pvEmitterVel-- bike velocity for velocity mult, additive velocity
					0.2f,							// fEmitterVelMult-- velocity of spray increases as bike velocity increases
					0.2f,							// fAdditiveEmitterVelMult-- follows the bike some
					NULL,							// pvEjectVel-- ejection velocity is calculated for side spray emitter
					FALSE,						// bEjectVelChanges
					<spray rate>,				// fDelay-- maybe something like 10
					2.0f,							// fMinDistance-- make sure they some out every 2 meters when bike is moving
					FALSE							// bStationary
*/

t_Handle ParticleEmitter__Create(
	int nDataIndex,
	u32 u32Type,
	int nEffectID,
	const Vector3Packed *pvEmitterPos,
	const Vector3Packed *pvEmitterVel,
	float fEmitterVelMult,
	float fAdditiveEmitterVelMult,
	const Vector3Packed *pvEjectVel,
	bool bEjectVelChanges,
	float fDelay,
	int nDelayRandomPct,
	float fScale,
	int nScaleRandomPct,
	float fMinDistance,
	float fGoRadius,
	bool bStationary,
	float fLifetime, /*= 0.0f (forever)*/
	u16 u16Group, /*= EMITTER_DEFAULT_GROUP*/
	int iSceneIndex, /*= -1*/
	float fLOD1Radius, /*= MAX_E_LOD_RADIUS*/
	float fLOD2Radius, /*= MAX_E_LOD_RADIUS*/
	float fFluxVolumeHalfSize, /*= 0.0f*/
	float fInvisDistance, /*= 0.0f*/
	float fNearDistance, /*= 0.0f*/
	float fNearScale, /*= 1.0f*/
	float fFarDistance, /*= 0.0f*/
	float fFarScale) /*= 1.0f*/
{
	/* pre-load and reset one-shots */

	Graphics4x4 *pVelTransform = ParticleMgr.pEmitterVelTransform;
	Vector3Packed *pvTarget = ParticleMgr.pvEmitterTarget;
	ParticleMgr.pEmitterVelTransform = NULL;
	ParticleMgr.pvEmitterTarget = NULL;

	/* grab a record from free pool */

	ts_ParticleEmitter *pEmitter = ParticleMgr__AllocParticleEmitter(u16Group);
	if (pEmitter == NULL)
		return(INVALID_HANDLE);

	/* record data */

	pEmitter->nDataIndex = nDataIndex;
	ASSERT((u32Type == EMITTER_STANDARD) ||
			 (u32Type == EMITTER_ORBITER) ||
			 (u32Type == EMITTER_TIRE_BACK_SPRAY) ||
			 (u32Type == EMITTER_TIRE_SIDE_SPRAY) ||
			 (u32Type == EMITTER_TIRE_CIRCLE) ||
			 (u32Type == EMITTER_FLUX) ||
			 (u32Type == EMITTER_DISTANT));
	pEmitter->u32Flags &= ~EMITTER_TYPE_MASK;
	pEmitter->u32Flags |= (u16) u32Type;

	/* emitter has not been advanced */

	pEmitter->u32Flags |= EMITTER_FIRST_ADVANCE;

	/* if a scene is specified, pass through the correct flag */

	if (iSceneIndex != -1)
		pEmitter->u32Flags |= (EMITTER_ONLY_SCENE_1 << iSceneIndex);

	/* more data */

	pEmitter->u16EffectID = (u16) nEffectID;
	ASSERT(pvEmitterPos);	// always need a position to read, even for static emitters
	pEmitter->pvPos = pvEmitterPos;
	pEmitter->fTimerInc = fDelay;
	pEmitter->fTimer = ParticleRandom.UpTo(fDelay);	// start at some random value beween 0 and delay
	pEmitter->fMinDistance = fMinDistance;
	pEmitter->fGoRadius = fGoRadius;
	pEmitter->fDelay = fDelay;
	pEmitter->nDelayRandomPct = nDelayRandomPct;
	pEmitter->fScale = fScale;
	pEmitter->nScaleRandomPct = nScaleRandomPct;
	pEmitter->fConstantAlpha = 1.0f;									// use ParticleEmitter__SetConstantAlpha to change if desired
	pEmitter->pfConstantAlpha = &pEmitter->fConstantAlpha;	// use ParticleEmitter__SetConstantAlpha to change if desired
	pEmitter->fColorMult = 1.0f;										// use ParticleEmitter__SetColorMult to change if desired
	pEmitter->pfColorMult = &pEmitter->fColorMult;				// use ParticleEmitter__SetColorMult to change if desired
	pEmitter->pVelTransform = pVelTransform;
	pEmitter->pvTarget = pvTarget;
	pEmitter->pPlaneList = NULL;	// this gets set separately by ParticleEmitter__SetPlaneList
	pEmitter->pPath = NULL;			// this gets set by ParticleEmitter__SetParticlePath

	/* lifetime */

	pEmitter->fLifetime = fLifetime;

	/* lod-- if enabled, emitter emits at 100% of emitter rate when closer than near lod radius, 0% when farther than */
	/* far radius */

	pEmitter->fLOD1Radius = fLOD1Radius;
	pEmitter->fLOD2Radius = fLOD2Radius;
	if (fLOD1Radius >= MAX_E_LOD_RADIUS - 1.0f)	// we test for fLOD1Radius == MAX_E_LOD_RADIUS to see if it's off, make sure int-to-float conversion is accurate
		pEmitter->fLOD1Radius = MAX_E_LOD_RADIUS;
	if (fLOD2Radius >= MAX_E_LOD_RADIUS - 1.0f)	// we test for fLOD1Radius == MAX_E_LOD_RADIUS to see if it's off, make sure int-to-float conversion is accurate
		pEmitter->fLOD2Radius = MAX_E_LOD_RADIUS;

	/* flux emitter-specific data */

	pEmitter->fFluxVolumeHalfSize = fFluxVolumeHalfSize;
	pEmitter->fFluxCeiling = MAXFLOAT;

	/* EMITTER_DISTANT emitters don't show up at all near the camera, but only when they are farther away. they are */
	/* used to call attention to distant objects, then go away when that object gets near enough to see */

	pEmitter->fInvisDistance = fInvisDistance;	// distance at which emitter particles start to fade in, using fNearScale
	pEmitter->fNearDistance = fNearDistance;		// distance at which fade in is complete, still using fNearScale
	pEmitter->fNearScale = fNearScale;				// scale to use when distance from camera <= fNearDistance
	pEmitter->fFarDistance = fFarDistance;			// interpolate from fNearScale to fFarScale between fNearDistance and fFarDistance
	pEmitter->fFarScale = fFarScale;					// scale to use when distance from camera >= fFarDistance

	/* emitter is default active */

	ParticleEmitter__Activate(pEmitter->Handle);

	/* figure out how to use velocities */

	pEmitter->pvEjectVel = NULL;
	pEmitter->pvEmitterVel = NULL;
	switch(u32Type)
	{
	case EMITTER_STANDARD:
	case EMITTER_ORBITER:
	case EMITTER_DISTANT:
		if (pvEjectVel)
		{
			pEmitter->pvEjectVel = pvEjectVel;
			if (bEjectVelChanges)
				pEmitter->u32Flags |= EMITTER_EJECT_VEL_CHANGES;
			else
			{
				/* ejection velocity does not change, i can pre-calculate unit vector and magnitude */

				pEmitter->fVelMagnitude = pvEjectVel->Length();
				pEmitter->vVelUnitVector = *pvEjectVel;
				pEmitter->vVelUnitVector.SafeNormalize();
			}
		}
		if (pvEmitterVel)
		{
			pEmitter->pvEmitterVel = pvEmitterVel;
			pEmitter->fAdditiveVelMult = fAdditiveEmitterVelMult;
			pEmitter->fVelMult = 0.0f;	// not used in these emitters
		}
		break;
	case EMITTER_TIRE_CIRCLE:
		ASSERT(pvEjectVel == NULL);						// ejection velocity is calculated from emitter velocity
		ASSERT(pvEmitterVel);								// must have an emitter vel for this emitter type
		pEmitter->pvEmitterVel = pvEmitterVel;
		pEmitter->fVelMult = fEmitterVelMult;
		pEmitter->fAdditiveVelMult = fAdditiveEmitterVelMult;
		break;
	case EMITTER_TIRE_BACK_SPRAY:
	case EMITTER_TIRE_SIDE_SPRAY:
		ASSERT(pvEjectVel == NULL);						// ejection velocity is calculated from emitter velocity
		ASSERT(pvEmitterVel);								// must have an emitter vel for this emitter type
		pEmitter->pvEmitterVel = pvEmitterVel;
		ASSERT(fEmitterVelMult != 0.0f);					// doesn't make any sense for tire spray-- won't get any spray since emitter velocity will be multiplied by 0
		pEmitter->fVelMult = fEmitterVelMult;
		pEmitter->fAdditiveVelMult = fAdditiveEmitterVelMult;
		pEmitter->u32Flags |= EMITTER_ACTIVE_MOVE_ONLY;	// force for sprays
		break;
	case EMITTER_FLUX:
		/* flux emitters are quite different from the rest. they define a cubic volume within which particles are */
		/* created at a certain density. as the volume moves, new particles are created to fill the new space, and */
		/* old particles that are now outside the volume are killed. particles created by flux emitters can't move, */
		/* since if they travel outside the volume they will be killed and never replaced */

		ASSERT(pvEjectVel == NULL);								// no additional velocity
		ASSERT(pvEmitterVel == NULL);								// no emitter velocity
		ASSERT(fAdditiveEmitterVelMult == 0.0f);				// no additive velocity
		ASSERT(fEmitterVelMult == 0.0f);							// no velocity multiplier
		ASSERT(fMinDistance == 0.0f);								// no minimum distance between effects
		ASSERT(bStationary == false);								// doesn't make sense for flux emitters to be stationary
		ASSERT(pEmitter->fLOD1Radius == MAX_E_LOD_RADIUS);	// since emitter doesn't continually emit particles, lod makes no sense
		pEmitter->fAdditiveVelMult = 0.0f;
		pEmitter->fVelMult = 0.0f;
		bStationary = false;

		/* mark this flux emitter as new-- the first time this advances it needs to fill the whole volume */

		pEmitter->u32Flags |= EMITTER_FLUX_FIRST_ADVANCE;

		/* guarantee that first flux emitter advance will find movement */

		pEmitter->vLastFluxEmitterPos.x( MAXFLOAT );
		break;
	}

	/* stationary? note that being a stationary emitter means that the contents of pvPos will not change */

	if (bStationary)
	{
		pEmitter->vStationaryPos = *pEmitter->pvPos;
		pEmitter->pvPos = &pEmitter->vStationaryPos;
		pEmitter->u32Flags |= EMITTER_STATIONARY;
		pEmitter->u32Flags &= ~EMITTER_ACTIVE_MOVE_ONLY;	// no longer applies
		pEmitter->pvEmitterVel = NULL;								// no need to worry about emitter's velocity
		pEmitter->fMinDistance = 0.0f;								// no need to check this for stationary emitters
		pEmitter->fVelMult = 0.0f;										// no velocity multiplication for stationary emitter
		pEmitter->fAdditiveVelMult = 0.0f;							// no additive velocity multiplication for stationary emitter
	}

	/* success */

	return(pEmitter->Handle);
}

/* create an emitter from a pre-defined emitter record. pvUp is only meaningful if the emitter type is
EMITTER_ORBITER, and even then it can be NULL if you want (0, 1, 0) to be up vector. see notes above
for description of pvEmitterPos, pvEmitterVel, pvEjectVel */

t_Handle ParticleEmitter__CreateFromRecord(
	const char *pEmitterName,
	const Vector3Padded *pvEmitterPos,
	const Vector3Padded *pvEmitterVel,
	const Vector3Padded *pvEjectVel,
	bool bEjectVelChanges,
	const Vector3Padded *pvUp,
	bool bStationary,
	u16 u16Group, /*= EMITTER_DEFAULT_GROUP*/
	int iSceneIndex) /*= -1 */
{
	return(ParticleEmitter__CreateFromRecord(
				 pEmitterName,
				 (Vector3Packed*)pvEmitterPos,
				 (Vector3Packed*)pvEmitterVel,
				 (Vector3Packed*)pvEjectVel,
				 bEjectVelChanges,
				 (Vector3Packed*)pvUp,
				 bStationary,
				 u16Group, /*= EMITTER_DEFAULT_GROUP*/
				 iSceneIndex)); /*= -1 */
}

t_Handle ParticleEmitter__CreateFromRecord(
	const char *pEmitterName,
	const Vector3Packed *pvEmitterPos,
	const Vector3Packed *pvEmitterVel,
	const Vector3Packed *pvEjectVel,
	bool bEjectVelChanges,
	const Vector3Packed *pvUp,
	bool bStationary,
	u16 u16Group, /*= EMITTER_DEFAULT_GROUP*/
	int iSceneIndex) /*= -1 */
{
	/* find emitter by name */

	int nDataIndex;
	int nEmitterIndex;
	if (ParticleMgr__FindEmitterByName(pEmitterName, nDataIndex, nEmitterIndex) == false)
		return(INVALID_HANDLE);	// not found

	/* return result of create */

	return(ParticleEmitter__CreateFromRecord(
				nDataIndex,				// int nDataIndex
				nEmitterIndex,			// int nEmitterID
				pvEmitterPos,			// Vector3 *pvEmitterPos
				pvEmitterVel,			// Vector3 *pvEmitterVel
				pvEjectVel,				// Vector3 *pvEjectVel
				bEjectVelChanges,		// bool bEjectVelChanges
				pvUp,						// Vector3 *pvUp
				bStationary,			// bool bStationary
				u16Group,				// u16 u16Group = EMITTER_DEFAULT_GROUP
				iSceneIndex));			// int iSceneIndex = -1
}

/* create an emitter from a pre-defined emitter record. pvUp is only meaningful if the emitter type is
EMITTER_ORBITER, and even then it can be NULL if you want (0, 1, 0) to be up vector. see notes above
for description of pvEmitterPos, pvEmitterVel, pvEjectVel */

t_Handle ParticleEmitter__CreateFromRecord(
	int nDataIndex,
	int nEmitterID,
	const Vector3Packed *pvEmitterPos,
	const Vector3Packed *pvEmitterVel,
	const Vector3Packed *pvEjectVel,
	bool bEjectVelChanges,
	const Vector3Packed *pvUp,
	bool bStationary,
	u16 u16Group, /*= EMITTER_DEFAULT_GROUP*/
	int iSceneIndex) /*= -1 */
{
	t_Handle Handle;
	ts_EmitterSource *pEmitterSource;

	/* any particle data? */

	if (ParticleMgr.pEmitterList == NULL)
		return(INVALID_HANDLE);

	/* point to emitter record */

	pEmitterSource = (ts_EmitterSource *) GetRecordAddress(ParticleMgr.pEmitterList[nDataIndex], nEmitterID);

	/* lifetime */

	float fLifetime = CONVERT_U8_TO_FLOAT(pEmitterSource->u8Lifetime, MAX_E_LIFETIME);
	fLifetime = Particle__fRandomValue(fLifetime, pEmitterSource->u8LifetimeRandomPct);

	/* emission rate if emitter is marked as delay-continuously-random, pass base delay and random factor. if not */
	/* randomize here once for emitter's life */

	float fDelay = CONVERT_U16_TO_FLOAT(pEmitterSource->u16Delay, MAX_E_DELAY);
	int nDelayRandomPct = pEmitterSource->u8DelayRandomPct;
	if ((pEmitterSource->u8Flags & EMITTER_SOURCE_DELAY_CONT_RANDOM) == 0)
	{
		/* randomize once */

		fDelay = Particle__fRandomValue(fDelay, nDelayRandomPct);
		nDelayRandomPct = 0; // no more randomness
	}

	/* scale. if emitter is marked as scale-continuously-random, pass base scale and random factor. if not */
	/* randomize here once for emitter's life */

	ASSERT(pEmitterSource->u8Scale);	// if 0, all particles scaled to 0
	if (pEmitterSource->u8Scale == 0)
		return(INVALID_HANDLE);
	float fScale = CONVERT_U8_TO_FLOAT(pEmitterSource->u8Scale, MAX_E_SCALE);
	int nScaleRandomPct = pEmitterSource->u8ScaleRandomPct;
	if ((pEmitterSource->u8Flags & EMITTER_SOURCE_SCALE_CONT_RANDOM) == 0)
	{
		/* randomize once */

		fScale = Particle__fRandomValue(fScale, nScaleRandomPct);
		nScaleRandomPct = 0; // no more randomness
	}

	/* create it */

	Handle = ParticleEmitter__Create(
					nDataIndex,																						// int nDataIndex
					pEmitterSource->u8Type,																		// u32 u32Type
					pEmitterSource->u16EffectID,																// int nEffectID
					(Vector3Packed*)pvEmitterPos,																// Vector3 *pvEmitterPos
					pvEmitterVel,																					// Vector3 *pvEmitterVel
					(float) pEmitterSource->u8VelPct / 100.0f,											// float fEmitterVelMult
					(float) pEmitterSource->u8AdditiveVelPct / 100.0f,									// float fAdditiveEmitterVelMult
					pvEjectVel,																						// Vector3 *pvEjectVel
					bEjectVelChanges,																				// bool bEjectVelChanges
					fDelay,																							// float fDelay
					nDelayRandomPct,																				// int nDelayRandomPct
					fScale,																							// float fScale
					nScaleRandomPct,																				// int nScaleRandomPct
					CONVERT_U8_TO_FLOAT(pEmitterSource->u8MinDistance, MAX_E_MIN_DIST),			// float fMinDistance
					CONVERT_U16_TO_FLOAT(pEmitterSource->u16GoRadius, MAX_E_GO_RADIUS),			// float fGoRadius
					bStationary,																					// bool bStationary
					fLifetime,																						// float fLifetime = 0.0f (forever)
					u16Group,																						// u16 u16Group = EMITTER_DEFAULT_GROUP
					iSceneIndex,																					// int iSceneIndex = -1
					CONVERT_U16_TO_FLOAT(pEmitterSource->u16LOD1Radius, MAX_E_LOD_RADIUS),		// float fLOD1Radius = MAX_E_LOD_RADIUS
					CONVERT_U16_TO_FLOAT(pEmitterSource->u16LOD2Radius, MAX_E_LOD_RADIUS),		// float fLOD2Radius = MAX_E_LOD_RADIUS
					CONVERT_U8_TO_FLOAT(pEmitterSource->u8FluxHalfSize, MAX_E_FLUX_HALF_SIZE),	// float fFluxVolumeHalfSize = 0.0f
					CONVERT_U16_TO_FLOAT(pEmitterSource->u16InvisDistance, MAX_E_LOD_RADIUS),	// float fInvisDistance = 0.0f
					CONVERT_U16_TO_FLOAT(pEmitterSource->u16NearDistance, MAX_E_LOD_RADIUS),	// float fNearDistance = 0.0f
					CONVERT_U8_TO_FLOAT(pEmitterSource->u8NearScale, MAX_E_SCALE),					// float fNearScale = 1.0f
					CONVERT_U16_TO_FLOAT(pEmitterSource->u16FarDistance, MAX_E_LOD_RADIUS),		// float fFarDistance = 0.0f
					CONVERT_U8_TO_FLOAT(pEmitterSource->u8FarScale, MAX_E_SCALE)					// float fFarScale = 1.0f
					);
	if (Handle == INVALID_HANDLE)
		return(INVALID_HANDLE);

	/* resolve handle back to emitter */

	ts_ParticleEmitter *pEmitter = PARTICLE_EMITTER_GET_RECORD(Handle);

	/* additional flags from source */

	if ((pEmitter->pvEmitterVel) && (pEmitterSource->u8Flags & EMITTER_SOURCE_ACTIVE_MOVE_ONLY))
		pEmitter->u32Flags |= EMITTER_ACTIVE_MOVE_ONLY;

#ifdef PRINT_PARTICLE_CREATION
	/* fill in name pointer for debugging */

	ts_RecordList *pRecordList = (ts_RecordList *) dblGetChunkData(DB_EMITTER_LIST, DEFAULT_CHUNK_KEY_STRING, ParticleMgr.DBHandle[nDataIndex]);
	if (pRecordList)
		pEmitter->pName = (char *) GetRecordAddress(pRecordList, nEmitterID);
#endif //PRINT_PARTICLE_CREATION

	/* maybe finish off orbiter (tire burnout circle is just an orbiter on its side) */

	if ((pEmitterSource->u8Type == EMITTER_ORBITER) || (pEmitterSource->u8Type == EMITTER_TIRE_CIRCLE))
	{
		ParticleEmitter__FinishOrbiter(
			Handle, 
			CONVERT_U8_TO_FLOAT(pEmitterSource->u8Radius, MAX_E_RADIUS),
			CONVERT_S16_TO_FLOAT(pEmitterSource->s16AngularVelRadians, MAX_E_ANGULAR_VEL),
			pvUp);
	}

	/* return handle */

	return(Handle);
}

/* supply info for finishing orbiting emitter. radius is distance in meters that emit orbits from
position, angular velocity can be positive or negative, pvUp defines the plane the orbiter inhabits.
assumed (0, 1, 0) if NULL is passed */

bool ParticleEmitter__FinishOrbiter(t_Handle Handle, float fRadius, float fAngularVelRadians, const Vector3Packed *pvUp)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_ParticleEmitter *pEmitter = PARTICLE_EMITTER_GET_RECORD(Handle);
	if (pEmitter->Handle != Handle)
		return(FALSE);

	/* update emitter */

	pEmitter->fRadians = ParticleRandom.UpTo(Math::TwoPi);	// 0.0 to 2PI
	pEmitter->fRadius = fRadius;
	pEmitter->fAngularVelRadians = fAngularVelRadians;
	pEmitter->pvUp = pvUp;
	return(TRUE);
}

/* update a tire-type emitter. this routine has a very specific purpose-- to be able to swap emitters
on a motorcycle wheel without having to kill the existing emitter and create a new one. it basically
just swaps in the effect id, vel mult, additive velocity. for example, side spray emitters are basically
the same, but the emitter values may be subtly different, as well as the effect id. this routine should
be able to swap between EMITTER_TIRE_SIDE_SPRAY, EMITTER_TIRE_BACK_SPRAY, and EMITTER_TIRE_CIRCLE. none
of them have an ejection velocity, everything is calculated from the bike velocity */

bool ParticleEmitter__UpdateTireEmitter(t_Handle Handle, char *pEmitterName)
{
	ts_ParticleEmitter *pEmitter;
	ts_EmitterSource *pEmitterSource;

	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	pEmitter = PARTICLE_EMITTER_GET_RECORD(Handle);
	if (pEmitter->Handle != Handle)
		return(FALSE);

	/* don't update locked emitters */

	if (ParticleEmitter__IsLocked(Handle))
		return(FALSE);

	/* find new emitter by name */

	int nDataIndex;
	int nEmitterIndex;
	if (ParticleMgr__FindEmitterByName(pEmitterName, nDataIndex, nEmitterIndex) == false)
		return(false);	// not found

	/* check types */

//	ASSERT(((pEmitter->u32Flags & EMITTER_TYPE_MASK) == EMITTER_TIRE_SIDE_SPRAY) ||
//			 ((pEmitter->u32Flags & EMITTER_TYPE_MASK) == EMITTER_TIRE_BACK_SPRAY) ||
//			 ((pEmitter->u32Flags & EMITTER_TYPE_MASK) == EMITTER_TIRE_CIRCLE));

	/* point to emitter record */

	pEmitter->nDataIndex = nDataIndex;	// i guess it's ok to switch to emitting effects from another particle data file???
	pEmitterSource = (ts_EmitterSource *) GetRecordAddress(ParticleMgr.pEmitterList[pEmitter->nDataIndex], nEmitterIndex);
//	ASSERT((pEmitterSource->u8Type == EMITTER_TIRE_SIDE_SPRAY) ||
//			 (pEmitterSource->u8Type == EMITTER_TIRE_BACK_SPRAY) ||
//			 (pEmitterSource->u8Type == EMITTER_TIRE_CIRCLE));

	/* replace values */

	pEmitter->u32Flags &= ~EMITTER_TYPE_MASK;
	pEmitter->u32Flags |= (u16) pEmitterSource->u8Type;
	pEmitter->u16EffectID = pEmitterSource->u16EffectID;
	pEmitter->fVelMult = (float) pEmitterSource->u8VelPct / 100.0f,
	pEmitter->fAdditiveVelMult = (float) pEmitterSource->u8AdditiveVelPct / 100.0f,
	pEmitter->fMinDistance = CONVERT_U8_TO_FLOAT(pEmitterSource->u8MinDistance, MAX_E_MIN_DIST);
	pEmitter->fTimer = 0.0f;	// start right away
	pEmitter->fTimerInc = CONVERT_U16_TO_FLOAT(pEmitterSource->u16Delay, MAX_E_DELAY);

	/* circle isn't move only-- it smokes on burnout */

	if (pEmitterSource->u8Type == EMITTER_TIRE_CIRCLE)
		pEmitter->u32Flags &= ~EMITTER_ACTIVE_MOVE_ONLY;
	else
		pEmitter->u32Flags |= EMITTER_ACTIVE_MOVE_ONLY;

	/* default to active */

	ParticleEmitter__Activate(Handle);

	/* success */

	return(TRUE);
}

/* activate/deactivate an emitter */

bool ParticleEmitter__Activate(t_Handle Handle)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_ParticleEmitter *pEmitter = PARTICLE_EMITTER_GET_RECORD(Handle);
	if (pEmitter->Handle != Handle)
		return(FALSE);

	/* don't update locked emitters */

	if (ParticleEmitter__IsLocked(Handle))
		return(FALSE);

	/* activate */

	pEmitter->u32Flags |= EMITTER_ACTIVE;
	ParticleEmitter__MakeReady(pEmitter);
	return(TRUE);
}
bool ParticleEmitter__Deactivate(t_Handle Handle)
{
	ts_ParticleEmitter *pEmitter;

	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	pEmitter = PARTICLE_EMITTER_GET_RECORD(Handle);
	if (pEmitter->Handle != Handle)
		return(FALSE);

	/* don't update locked emitters */

	if (ParticleEmitter__IsLocked(Handle))
		return(FALSE);

	/* de-activate */

	pEmitter->u32Flags &= ~EMITTER_ACTIVE;
	return(TRUE);
}
bool ParticleEmitter__IsActive(t_Handle Handle)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_ParticleEmitter *pEmitter = PARTICLE_EMITTER_GET_RECORD(Handle);
	if (pEmitter->Handle != Handle)
		return(FALSE);

	/* return status */

	return((pEmitter->u32Flags & EMITTER_ACTIVE) != 0);
}

/* report whether particle emitter was in view frustum last scene */

bool ParticleEmitter__InScene(t_Handle Handle)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_ParticleEmitter *pEmitter = PARTICLE_EMITTER_GET_RECORD(Handle);
	if (pEmitter->Handle != Handle)
		return(false);

	/* return status */

	return((pEmitter->u32Flags & EMITTER_IN_VIEW_LAST_SCENE) != 0);
}

/* lock/unlock an emitter */

bool ParticleEmitter__Lock(t_Handle Handle)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_ParticleEmitter *pEmitter = PARTICLE_EMITTER_GET_RECORD(Handle);
	pEmitter = PARTICLE_EMITTER_GET_RECORD(Handle);
	if (pEmitter->Handle != Handle)
		return(FALSE);

	/* lock */

	pEmitter->u32Flags |= EMITTER_LOCKED;
	return(TRUE);
}
bool ParticleEmitter__Unlock(t_Handle Handle)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_ParticleEmitter *pEmitter = PARTICLE_EMITTER_GET_RECORD(Handle);
	if (pEmitter->Handle != Handle)
		return(FALSE);

	/* lock */

	pEmitter->u32Flags &= ~EMITTER_LOCKED;
	return(TRUE);
}
bool ParticleEmitter__IsLocked(t_Handle Handle)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_ParticleEmitter *pEmitter = PARTICLE_EMITTER_GET_RECORD(Handle);
	if (pEmitter->Handle != Handle)
		return(FALSE);
	
	/* return status */

	return((pEmitter->u32Flags & EMITTER_LOCKED) != 0);
}
bool ParticleEmitter__ChangeEffect(t_Handle Handle, char *pEffectName)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_ParticleEmitter *pEmitter = PARTICLE_EMITTER_GET_RECORD(Handle);
	if (pEmitter->Handle != Handle)
		return(FALSE);

	/* find new effect by name */

	int nDataIndex;
	int nEffectIndex;
	if (ParticleMgr__FindEffectByName(pEffectName, nDataIndex, nEffectIndex) == false)
		return(false);	// not found
	ASSERT(nDataIndex == pEmitter->nDataIndex);	// if this hits, we have found a same-name effect from a different particle data file-- not necessarily a disaster

	/* change effect */

	pEmitter->nDataIndex = nDataIndex;
	pEmitter->u16EffectID = nEffectIndex;
	return(TRUE);
}

/* is the emitter a tire type emitter? just for assertion */

#ifdef _DEBUG
bool ParticleEmitter__IsTireType(t_Handle Handle)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_ParticleEmitter *pEmitter = PARTICLE_EMITTER_GET_RECORD(Handle);
	if (pEmitter->Handle != Handle)
		return(FALSE);

	/* check type */

	u32 u32Type = pEmitter->u32Flags & EMITTER_TYPE_MASK;
	return((u32Type == EMITTER_TIRE_SIDE_SPRAY) ||
			 (u32Type == EMITTER_TIRE_BACK_SPRAY) ||
			 (u32Type == EMITTER_TIRE_CIRCLE));
}
#endif //_DEBUG

/* update an emitter's frame rate */

bool ParticleEmitter__UpdateDelay(t_Handle Handle, float fDelay, bool bPreserveOriginalDelay /*= false*/)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_ParticleEmitter *pEmitter = PARTICLE_EMITTER_GET_RECORD(Handle);
	if (pEmitter->Handle != Handle)
		return(FALSE);

	/* different for flux emitters */

	if ((pEmitter->u32Flags & EMITTER_TYPE_MASK) == EMITTER_FLUX)
	{
		ASSERT(bPreserveOriginalDelay == false);	// flux emitters shouldn't have lod data
		FluxEmitter__UpdateDensity(pEmitter, fDelay);
		return(TRUE);
	}

	/* calling as a result of LOD calculation does not update emitter's original effects per second value (since */
	/* it uses the original value as an input to its calculation), otherwise we do */

	if (bPreserveOriginalDelay == false)
		pEmitter->fDelay = fDelay;

	/* update delay */

	pEmitter->fTimerInc = fDelay;

	/* if current delay until next emit is longer than new inc, reduce it */

	if (pEmitter->fTimer > pEmitter->fTimerInc)
		pEmitter->fTimer = pEmitter->fTimerInc;
	return(TRUE);
}

/* clear emitter's timer so it fires immediately */

bool ParticleEmitter__ClearTimer(t_Handle Handle)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_ParticleEmitter *pEmitter = PARTICLE_EMITTER_GET_RECORD(Handle);
	if (pEmitter->Handle != Handle)
		return(FALSE);

	/* update effect id */

	pEmitter->fTimer = 0.0f;
	return(TRUE);
}

/* for pigpen, to suspend min distance enforcement when test emitter wraps */

bool ParticleEmitter__ResetLastPos(t_Handle Handle)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_ParticleEmitter *pEmitter = PARTICLE_EMITTER_GET_RECORD(Handle);
	if (pEmitter->Handle != Handle)
		return(FALSE);

	/* update effect id */

	ASSERT(pEmitter->pvPos);
	pEmitter->vLastPos = *pEmitter->pvPos;
	pEmitter->vLastEmitPos = *pEmitter->pvPos;
	return(TRUE);
}

/* call this to make sure emitter is ready to emit */

void ParticleEmitter__MakeReady(ts_ParticleEmitter *pEmitter)
{
	/* emitter should be ready when it gets advanced next. updating vLastPos and vLastEmitPos is important for */
	/* not creating a huge amount of particles for emitters with a min distance value */

	if (pEmitter->fTimer < (MAXFLOAT32 / 2.0f))	// don't reset if waiting forever (mostly for pigpen use)
		pEmitter->fTimer = 0.0f;	// should fire immediately if rate != 0 per second
	pEmitter->vLastPos = *pEmitter->pvPos;
	pEmitter->vLastEmitPos = *pEmitter->pvPos;
}

/* let emitter emit */

void ParticleEmitter__Advance(ts_ParticleEmitter *pEmitter)
{
	u32 u32Type = pEmitter->u32Flags & EMITTER_TYPE_MASK;
	float fInterpolate;
	float fVelMagnitude;
	float *pfVelMagnitude = NULL;
	Vector3 vNewEmitPos;
	Vector3Packed vInterpolatedPos;
	Vector3Packed vPos;
	Vector3Packed vVel;
	Vector3Packed vAdditiveVel;
	Vector3Packed *pvVel;
	Vector3Packed *pvAdditiveVel;
	Vector3Packed vSprayPosAdjust;
	Vector3Packed vSprayVelLeft;
	Vector3Packed vSprayVelRight;
	Vector3Packed vDelta;
	Graphics4x4 mOrbit;
	int iSceneIndex;

	if (pEmitter->u32Flags & EMITTER_ONLY_SCENE_1)
		iSceneIndex = 0;
	else if (pEmitter->u32Flags & EMITTER_ONLY_SCENE_2)
		iSceneIndex = 1;
	else if (pEmitter->u32Flags & EMITTER_ONLY_SCENE_3)
		iSceneIndex = 2;
	else if (pEmitter->u32Flags & EMITTER_ONLY_SCENE_4)
		iSceneIndex = 3;
	else
		iSceneIndex = -1;

	/* advance time to use for this emitter */

	float fAdvanceTime = ((pEmitter->u32Flags & EMITTER_USE_OS_TIMER) != 0) ? ParticleMgr.fOSFrameSec : ParticleMgr.fFrameSec;

	/* lifetime */

	if (pEmitter->fLifetime != 0.0f)	// 0.0 means infinite
	{
		pEmitter->fLifetime -= fAdvanceTime;
		if (pEmitter->fLifetime <= 0.0f)
		{
			ParticleEmitter__Kill(pEmitter->Handle);
			return;	// dead, nothing more to do
		}
	}

	/* advance timer */

	pEmitter->fTimer -= fAdvanceTime;

	/* flux emitters always update, since they are based on movement of emitter */

	if (u32Type == EMITTER_FLUX)
		pEmitter->fTimer = 0.0f;

	/* advance orbiter angle */

	pEmitter->fRadians += pEmitter->fAngularVelRadians * fAdvanceTime;

	/* particles created by flux emitters check this flag to see if they should check if they are outside the */
	/* flux volume and should die. since we may bail if emitter is out of view, always clear this here to avoid */
	/* unnecessary checks. if flux emitter advances, proper state is set in FluxEmitter__Advance */

	pEmitter->u32Flags &= ~EMITTER_FLUX_VOLUME_MOVED;

	/* emitters with valid lod and distant emitters both need distance to nearest camera */

	float fMinCameraDistance = MAXFLOAT32;
	if ((pEmitter->fLOD1Radius < MAX_E_LOD_RADIUS) || (u32Type == EMITTER_DISTANT))
	{
		CScene *pScene = SceneMgr::GetFirstScene();
		while (pScene)
		{
			if (pScene->d_type == SCENE_DISPLAY)
			{
				float fDistanceSquared = (*pEmitter->pvPos - pScene->d_cameraWorldPos).LengthSquared();
				if (fDistanceSquared < fMinCameraDistance)
					fMinCameraDistance = fDistanceSquared;
			}
			pScene = SceneMgr::GetNextScene(pScene);
		}
		fMinCameraDistance = Math::Sqrt(fMinCameraDistance);
	}

	/* check lod-- when enabled, emitter rate is at 100% closer than near radius, 0% at farther than far radius. */
	/* use distance to closest camera */

	if (pEmitter->fLOD1Radius < MAX_E_LOD_RADIUS)	// == MAX_E_LOD_RADIUS means not enabled 
	{
		/* calculate emitter rate we should use */

		float fDelay = pEmitter->fDelay;	// start with original value
		if (fMinCameraDistance >= pEmitter->fLOD2Radius)
			fDelay = MAX_E_DELAY;	// past far radius, turn off
		else if (fMinCameraDistance > pEmitter->fLOD1Radius)
		{
			/* for backward compatibility, and to limit interpolation range, convert delay (in seconds) to emissions */
			/* per second, and interpolate from that value at near radius to 0 at far radius. if i interpolate between */
			/* original delay and MAX_E_DELAY, emission rate drops off too steeply because MAX_E_DELAY is relatively large */

			float fEffectsPerSecond = 1.0f / fDelay;
			fEffectsPerSecond = fEffectsPerSecond * (pEmitter->fLOD2Radius - fMinCameraDistance) / (pEmitter->fLOD2Radius - pEmitter->fLOD1Radius);
			fDelay = 1.0f / fEffectsPerSecond;
			if (fDelay > MAX_E_DELAY)
				fDelay = MAX_E_DELAY;
		}

		/* update emitter rate-- adjusts fTimerInc, but passing true preserves original delay value */

		ParticleEmitter__UpdateDelay(pEmitter->Handle, fDelay, true);
	}

	/* for distant emitters, calculate a scale multiplier and an alpha multiplier based on distance. distance emitters */
	/* are inactive while closer than fInvisDistance, fade in between fInvisDistance and fNearDistance using fNearScale, */
	/* interpolate between fNearScale and fFarScale while between fNearDistance and fFarDistance, and use fFarScale if */
	/* beyond fFarDistance */

	float fConstantAlpha = *pEmitter->pfConstantAlpha;
	float fScaleMultiplier = 1.0f;
	if (u32Type == EMITTER_DISTANT)
	{
		if (fMinCameraDistance < pEmitter->fInvisDistance)
		{
			fConstantAlpha = 0.0f;
			ParticleEmitter__UpdateDelay(pEmitter->Handle, MAX_E_DELAY, true);	// emitter is essentially off
		}
		else
		{
			/* restore normal delay if necessary */

			if (pEmitter->fTimerInc == MAX_E_DELAY)
				ParticleEmitter__UpdateDelay(pEmitter->Handle, pEmitter->fDelay, true);

			if (fMinCameraDistance < pEmitter->fNearDistance)
			{
				/* emitter is fading in */

				fConstantAlpha *= (fMinCameraDistance - pEmitter->fInvisDistance) / (pEmitter->fNearDistance - pEmitter->fInvisDistance);
				fScaleMultiplier = pEmitter->fNearScale;
			}
			else if (fMinCameraDistance < pEmitter->fFarDistance)
			{
				/* emitter is between near and far distances-- interpolate scale */

				fScaleMultiplier = pEmitter->fNearScale + ((pEmitter->fFarScale - pEmitter->fNearScale) * (fMinCameraDistance - pEmitter->fNearDistance) / (pEmitter->fFarDistance - pEmitter->fNearDistance));
			}
			else
			{
				/* emitter is beyond far distance */

				fScaleMultiplier = pEmitter->fFarScale;
			}
		}
	}

	/* some emitters only function when they are moving */

#define MIN_EMIT_VELOCITY	1.0f	// meters per second, 1 m/s = 2.23 mph 
	if (pEmitter->u32Flags & EMITTER_ACTIVE_MOVE_ONLY)
	{
		if (pEmitter->pvEmitterVel->LengthSquared() < MIN_EMIT_VELOCITY * MIN_EMIT_VELOCITY)
		{
			ParticleEmitter__MakeReady(pEmitter);	// skip this advance, make it ready for next one
			return;
		}
	}

	/* emitters that are piggybacking on an orbiting particle need to calculate new position. put the */
	/* calculated position in vStationaryPos, pvPos has already been re-directed to point to it instead */
	/* of to the particle's position. need to calculate this before checking if in camera view so it's valid */

	if (pEmitter->u32Flags & EMITTER_PIGGYBACKS_ORBITER)
	{
		pEmitter->vStationaryPos = *Particle__CalculateOrbit(pEmitter->pParentParticle);

		/* if particle hasn't had a chance to advance yet, delay emitter */

		if (pEmitter->pParentParticle->u32Flags & PARTICLE_FIRST_ADVANCE)
		{
			ASSERT(pEmitter->u32Flags & EMITTER_FIRST_ADVANCE);	// shouldn't have had a chance to start yet
			return;
		}
	}

	/* only if in view of any of the active cameras */

	GeneralBoundingSphere.Center = *pEmitter->pvPos;
	GeneralBoundingSphere.Radius = pEmitter->fGoRadius;
	ts_Frustum *pViewFrustum = SceneMgr::GetFirstViewFrustum();
	bool bVisible = FALSE;
	while (pViewFrustum)
	{
		if (Frustum__IntersectsWorldSphere(pViewFrustum, &GeneralBoundingSphere))
		{
			bVisible = TRUE;
			pViewFrustum = NULL;	// stop the search
		}
		else
			pViewFrustum = SceneMgr::GetNextViewFrustum();	// not in this frustum, try the next one
	}
	if (bVisible)
		pEmitter->u32Flags |= EMITTER_IN_VIEW_LAST_SCENE;
	else
	{
		ParticleEmitter__MakeReady(pEmitter);	// skip this advance, make it ready for next one
		pEmitter->u32Flags &= ~EMITTER_IN_VIEW_LAST_SCENE;
		return;
	}

	/* if this is first time through advance, call ParticleEmitter__MakeReady to make sure that */
	/* last emit positions are valid-- position may not have been set yet when emitter was created */
	/* (that is, contents of pEmitter->pvPos may not have been initialized) */

	if (pEmitter->u32Flags & EMITTER_FIRST_ADVANCE)
	{
		ParticleEmitter__MakeReady(pEmitter);
		pEmitter->u32Flags &= ~EMITTER_FIRST_ADVANCE;
	}

	/* calculate new emit position-- usually just emitter position */

	switch(u32Type)
	{
		default:
			ASSERT(0);
			// and fall through to standard case
		case EMITTER_STANDARD:
		case EMITTER_TIRE_BACK_SPRAY:
		case EMITTER_TIRE_SIDE_SPRAY:
		case EMITTER_FLUX:
		case EMITTER_DISTANT:
			vNewEmitPos = *pEmitter->pvPos;
			break;
		case EMITTER_TIRE_CIRCLE:
		case EMITTER_ORBITER:
			/* calculate point on orbit. start with correct position in x/z plane */

			vNewEmitPos.Set(Math::Cos(pEmitter->fRadians),
								 0.0f,
								 Math::Sin(pEmitter->fRadians));

			/* if pvUp is straight up, we're done. if not, we have to do a coordinate transform */
			if (pEmitter->pvUp != NULL)
			{
				if (pEmitter->pvUp->y() < 0.999f)
				{
					Vector3 vB;
					Vector3 vC;

					/* calculate transform matrix that will transform unit vector Y (0, 1, 0) to the passed */
					/* unit vector. we have the input vector pEmitter->pvUp, think of it as vA. then calculate */
					/* vB = Y - (vA * (vA dot Y)), and vC = vB x vA. then the transform matrix is: */
					/*
					/*		vB.x()	vB.y(), vB.z()
					/*		vA.x()	vA.y(), vA.z()
					/*		vC.x()	vC.y(), vC.z()
					/*
					/* vA dot Y = (vA.x(), vA.y(), vA.z()) dot (0, 1, 0) = vA.y() */

					/* note that differs slightly from calculation of (1, 0, 0) to arbitrary vector used elsewhere */
					/* in this module. this is because we are working from the y-axis */

					vB = Vector3(0.0f, 1.0f, 0.0f) - *pEmitter->pvUp * pEmitter->pvUp->y();
					vB.Normalize();
					vC = Vector3::Cross(vB, *pEmitter->pvUp);

					/* fill in transform matrix */

					mOrbit.r00 = vB.x();
					mOrbit.r01 = vB.y();
					mOrbit.r02 = vB.z();
					mOrbit.r10 = pEmitter->pvUp->x();
					mOrbit.r11 = pEmitter->pvUp->y();
					mOrbit.r12 = pEmitter->pvUp->z();
					mOrbit.r20 = vC.x();
					mOrbit.r21 = vC.y();
					mOrbit.r22 = vC.z();

					/* and put rotation-plane vector through */

					vNewEmitPos.FastBasisTransform(vNewEmitPos, mOrbit);
				}
			}

			/* now adjust transformed unit vector by radius and add to emitter's position */

			vNewEmitPos *= pEmitter->fRadius;
			vNewEmitPos += *pEmitter->pvPos;
			break;
	}

	/* set timer inc to use this frame. this is the value to use to update fTimer if it is < 0.0-- the delay */
	/* between emissions */

	float fTimerInc = pEmitter->fTimerInc;	// may get overridden to enforce min distance
	if (pEmitter->nDelayRandomPct != 0)
		fTimerInc = Particle__fRandomValue(fTimerInc, pEmitter->nDelayRandomPct);

	/* have to create multiple effects to satisfy minimum distance restriction? this is a */
	/* way to make the particles emitted by high-velocity objects look beter. for example, */
	/* in road rash the bikes move fast. at 30 hz, the distance a bike travels from one */
	/* frame to another is large, and emitting one effect per frame looks too sparse. so */
	/* we want to guarantee that effects are emitted no further than <n> distance apart. */
	/* this gives a full look without having to crank the emit rate (which would create */
	/* hordes of particles at low speed, when the distance travelled per frame is small). */
	/* only valid if emitter has a velocity. */
	
	/* to accomplish this, set up to emulate an emit rate that is higher than the frame */
	/* rate so that the same code that looks for that case (below) will handle the minimum */
	/* distance case. effects will be emitted every <min distance> until the next effect */
	/* would be past the current position. this should give a nice stream of evenly-spaced */
	/* effects, though the path won't go exactly through each frame's emitter position */

	/* it used to be that LOD control was turned off for these emitters, because the minimum */
	/* distance would trump the reduced emission rate, but it's important to be able to LOD */
	/* these emitters out (ESPECIALLY these emitters, since they can produce a lot of particles) */
	/* so if we are beyond the first LOD radius, ignore the min distance. another possiblility */
	/* would be to adjust the min distance based on position between fLOD1Radius and fLOD2Radius, */
	/* but i'm not sure what kind of multiplier would be reasonable at fLOD2Radius...10? 100? */

	if ((pEmitter->fMinDistance != 0.0f) &&
		 ((pEmitter->fLOD1Radius == MAX_E_LOD_RADIUS) || (fMinCameraDistance < pEmitter->fLOD1Radius)))	// do not execute this code section if farther than valid fLOD1Radius
	{
		float fDistance;
		float fEmitDistance;
		float fEmitDistanceSquared;
		float fNewTimerInc;

		vDelta = vNewEmitPos - pEmitter->vLastEmitPos;
		fEmitDistanceSquared = vDelta.LengthSquared();
		if (fEmitDistanceSquared > pEmitter->fMinDistance * pEmitter->fMinDistance)
		{
			/* ok, we have to create at least one particle this frame to satisfy min distance. */
			/* we need to know distance from current position to last position */

			vDelta = vNewEmitPos - pEmitter->vLastPos;
			fDistance = vDelta.Length();
			fEmitDistance = Math::Sqrt(fEmitDistanceSquared);

			/* i think we can assume that last emission was last frame. this is certainly */
			/* true once we get into enforcing the minimum distance. it might not be true */
			/* when we just cross the boundary between not enforcing and enforcing, but it should */
			/* be close enough. over the last fAdvanceTime seconds we have travelled from vLastPos */
			/* to current vPos. note that vLastPos and vLastEmitPos are usually different-- */
			/* particle position is interpolated between current pos and vLastPos such that */
			/* particles are created at equal intervals (whether to satisfy min distance check */
			/* or just frame rate) */

			/* this looks like this:

				diagram 1

                    |<------- fAdvanceTime -------->|

            last
            emit   last                            new 
            pos    pos                             pos
             +------*-------+--------------+--------*
             |<--min dist-->|<--min dist-->|

			*/

			/* ned says: rob caught a bug 8/15/03 where fTimer was -Inf, caused by a divide by 0 here. here's */
			/* what i think happens: if the emitter reverses direction, you can get this situation for a frame:

				diagram 2

            |<------- fAdvanceTime ----->|

                               last
            last               emit      new 
            pos                pos       pos
            *-+----1---------+-*---------*
              |<--min dist-->|<--min dist-->|

			/* notice that the last emit position (which is the position where the emitter last actually created */
			/* an effect) is in between the last pos (which is last frame's new pos) and the new pos for this */
			/* frame. in this case, the distance between the new pos and the last emit pos is less than the */
			/* minimum distance between effects, so this section of code does not execute. that leaves the normal */
			/* fTimer and fTimerInc to execute, and a new effect can be created anywhere between the last pos */
			/* and the new pos (depending on the timer values). let's say that an effect is created at the number */
			/* "1" in the diagram 2. if the emitter keeps moving, this is fine-- next frame we'll be back */
			/* at the situation in diagram 1. however, if the emitter does not move next frame, we get this:

            diagram 3

                   last                  new
                   emit                  *** AND ***
                   pos                   last pos
            --+----*---------+-----------*
              |<--min dist-->|<--min dist-->|

			/* now we have trouble. the distance between the last emit pos and the new emit pos is greater */
			/* than the minimum distance, so this code section executes. but fDistance is 0 because the */
			/* emitter didn't move, so we get a divide by 0. so if this happens, fix the distance */
			
			if (Math::Zero(fDistance))
				fDistance = fEmitDistance;

			/* first, calculate how much time it takes to cover the min distance */

			fNewTimerInc = fAdvanceTime * (pEmitter->fMinDistance / fDistance);

			/* only proceed if new update rate is faster than emission-rate timing */

			if ((pEmitter->fTimer > 0.0f) || (fNewTimerInc < fTimerInc))
			{
				fTimerInc = fNewTimerInc;

				/* now set initial value of fTimer so that the first effect is emitted at */
				/* min distance from last emit position. first calculate time we would have */
				/* been at last emit position (assuming constant velocity), then move up by */
				/* time required to cover min distance */

				pEmitter->fTimer = (-fAdvanceTime * (fEmitDistance / fDistance)) + fTimerInc;
			}
		}
	}
	
	/* time to go? */

	if (pEmitter->fTimer > 0.0f)
	{
		/* keep fLastEmitTime accurate when we don't emit anything for possible min distance check */

		pEmitter->fLastEmitTime -= fAdvanceTime;
	}
	else
	{
		/* ok, we have an effect to generate. fTimer is typically < 0.0f, in which case we are generating it */
		/* in the past. if the emitter is moving, we want to generate at the correct spot, interpolating */
		/* between the current emitter position and the last emitter position. also, we may need to generate */
		/* multiple effects, depending on the how many iterations it takes for fTimer + fTimerInc to get */
		/* back above 0.0 */

		/* calculate velocity to use, if any */

		pvVel = NULL;
		if (pEmitter->pvEjectVel)
		{
			if (pEmitter->u32Flags & EMITTER_EJECT_VEL_CHANGES)
			{
				/* have to calculate unit vector and magnitude */

				vVel = *pEmitter->pvEjectVel;
				pvVel = &vVel;

				/* decompose into magnitude and direction */

				fVelMagnitude = vVel.Length();
				pfVelMagnitude = &fVelMagnitude;
				vVel.SafeNormalize();	// must be after vct3fLength call, obviously, so fVelMagnitude is correct
			}
			else
			{
				/* if emitter velocity doesn't change, use pre-calculated values */

				pvVel = &pEmitter->vVelUnitVector;
				pfVelMagnitude = &pEmitter->fVelMagnitude;
			}
		}
		else if (pEmitter->pvEmitterVel)
		{
			/* tire spray needs to calculate ejection velocity from emitter velocity */

			switch(u32Type)
			{
			case EMITTER_STANDARD:
			case EMITTER_ORBITER:
			case EMITTER_TIRE_CIRCLE:
			case EMITTER_FLUX:
			case EMITTER_DISTANT:
				break;
			case EMITTER_TIRE_BACK_SPRAY:
			case EMITTER_TIRE_SIDE_SPRAY:
				/* pvEmitterVel, fVelMult have been checked for validity during creation */

				vVel = *pEmitter->pvEmitterVel;
				vVel *= pEmitter->fVelMult;
				pvVel = &vVel;

				/* decompose into magnitude and direction */

				fVelMagnitude = vVel.Length();
				pfVelMagnitude = &fVelMagnitude;
				vVel.Normalize();	// must be after vct3fLength call, obviously, so fVelMagnitude is correct

				/* side or back for rest of calculation */

				ASSERT(pvVel);							// must have velocity to work with
				ASSERT(pfVelMagnitude);				// must have velocity to work with
				if (u32Type == EMITTER_TIRE_SIDE_SPRAY)
				{
					/* ok, we want to spray out to both sides of the tire. first rotate vel by 90 degrees */

					vSprayPosAdjust = *pvVel;
//djr: commented out (and following ASSERT) because vertical velocity will cause a divide by zero in Normalize().
//					vSprayPosAdjust.y(0.0f);	// treat ground as flat
					vSprayPosAdjust.Normalize();
					vSprayPosAdjust = vSprayPosAdjust * g3YRotate90;
					vSprayVelLeft = vSprayPosAdjust;

					/* move out half a tire width */

					vSprayPosAdjust.x(vSprayPosAdjust.x() * 0.1f);
//djr: commented out - see above
//					ASSERT(vSprayPosAdjust.y() == 0.0f);
					vSprayPosAdjust.z(vSprayPosAdjust.z() * 0.1f);

					/* spray velocity is rotated up 45 degrees from ground plane */

					vSprayVelLeft.y(vSprayVelLeft.y() + 0.7f);
					vSprayVelLeft.Normalize();
					vSprayVelRight.x( -vSprayVelLeft.x());
					vSprayVelRight.y( vSprayVelLeft.y());
					vSprayVelRight.z( -vSprayVelLeft.z());
				}
				else
				{
					/* back spray. cheesy first implementation-- just elevate it a little bit from */
					/* straight back. vVel is already normalized velocity vector */

					vVel = -vVel;
					vVel.y(vVel.y() + 0.25f);
					vVel.Normalize();
				}
				break;
			}
		}

		/* calculate additive velocity, if any */

		pvAdditiveVel = NULL;
		if ((pEmitter->fAdditiveVelMult != 0.0f) && (pEmitter->pvEmitterVel))
		{
			vAdditiveVel = *pEmitter->pvEmitterVel;
			vAdditiveVel *= pEmitter->fAdditiveVelMult;
			pvAdditiveVel = &vAdditiveVel;
		}

		/* for interpolation between current and last positions */

		vDelta = vNewEmitPos - pEmitter->vLastPos;

		/* now generate effects at required spots */

		while (pEmitter->fTimer <= 0.0f)
		{
			// rhn, hack fix ... if ftimer is -Inf (which I got 8/15/03), fix it up so it's not an eternal freeze
			if ( Math::IsNaN( pEmitter->fTimer ) || ( pEmitter->fTimer < -1000.0f ) || (fTimerInc <= 0.0f) )
			{
				ASSERTS( false, "particle timer frozen, releasing." );
				pEmitter->fTimer = 0.0f;
				fTimerInc = 0.01f;	// the line pEmitter->fTimer += fTimerInc; below will cause the loop to end
			}

#ifdef PRINT_PARTICLE_CREATION
			/* spit out emitter's name for debugging */

			if (pEmitter->pName)
				dbgPrint("Emitting from emitter %s, %d active particles\n", pEmitter->pName, ParticleMgr__GetActiveCount());
			else
				dbgPrint("Emitting from unnamed emitter, %d active particles\n", ParticleMgr__GetActiveCount());
#endif //PRINT_PARTICLE_CREATION

			/* interpolate distance. fTimer is between 0.0 and -fAdvanceTime. so take */
			/* (fTimer / fAdvanceTime) to get a 0.0 to -1.0 range. at 0.0 we want */
			/* current position. at -1.0 we want last position */
			
			fInterpolate = pEmitter->fTimer / fAdvanceTime;
			vInterpolatedPos = vNewEmitPos + (fInterpolate * vDelta);

			/* flag bits to force in particles created by effect that emitter emits */

			u32 u32ForceFlags = 0;
			if (pEmitter->u32Flags & EMITTER_USE_OS_TIMER)
				u32ForceFlags |= PARTICLE_USE_OS_TIMER;	// inherit use-os-timer flag

			/* handle emitter types */

			switch(pEmitter->u32Flags & EMITTER_TYPE_MASK)
			{
			case EMITTER_STANDARD:
			case EMITTER_TIRE_BACK_SPRAY:
			case EMITTER_TIRE_CIRCLE:
			case EMITTER_ORBITER:
			case EMITTER_DISTANT:
				ParticleMgr__SetOneShotEffectVelTransform(pEmitter->pVelTransform);	// one-shot
				ParticleMgr__SetOneShotEffectTarget(pEmitter->pvTarget);					// one-shot
				ParticleMgr__CreateEffect(
					pEmitter->nDataIndex,						// effect is in same data set as parent emitter
					pEmitter->u16EffectID,						// effect to create
					&vInterpolatedPos,							// world position to create at
					pvVel,											// creation velocity unit direction vector, can be NULL
					pfVelMagnitude,								// creation velocity magnitude, can be NULL
					pvAdditiveVel,									// additive velocity, can be NULL
					-pEmitter->fTimer, 							// value for initial particle advance
					pEmitter->fScale * fScaleMultiplier,	// scale to apply
					pEmitter->nScaleRandomPct,					// scale randomness
					fConstantAlpha,								// constant alpha
					pEmitter->fColorMult,						// color multiplier
					u32ForceFlags,									// bits to force in ts_Particle.u32Flags on particle creation
					iSceneIndex,									// which scenes to be rendered in, -1 means all scenes
					pEmitter->pPlaneList,						// restricting planes
					pEmitter->pPath,								// path for particles to follow
					NULL);											// pointer back to emitter for flux particles
				break;
			case EMITTER_FLUX:
				/* flux emitters are different... */

				FluxEmitter__Advance(pEmitter, iSceneIndex);
				break;
			case EMITTER_TIRE_SIDE_SPRAY:
				/* left side effect */

				vPos = vInterpolatedPos;
				vPos.x(vPos.x() + vSprayPosAdjust.x());
				vPos.z(vPos.z() + vSprayPosAdjust.z());
				ParticleMgr__SetOneShotEffectVelTransform(pEmitter->pVelTransform);	// one-shot
				ParticleMgr__SetOneShotEffectTarget(pEmitter->pvTarget);					// one-shot
				ParticleMgr__CreateEffect(
					pEmitter->nDataIndex,						// effect is in same data set as parent emitter
					pEmitter->u16EffectID,						// effect to create
					&vPos,											// world position to create at
					&vSprayVelLeft,								// creation velocity unit direction vector, can be NULL
					pfVelMagnitude,								// creation velocity magnitude, can be NULL
					pvAdditiveVel,									// additive velocity, can be NULL
					-pEmitter->fTimer, 							// value for initial particle advance
					pEmitter->fScale * fScaleMultiplier,	// scale to apply
					pEmitter->nScaleRandomPct,					// scale randomness
					fConstantAlpha,								// constant alpha
					pEmitter->fColorMult,						// color multiplier
					u32ForceFlags,									// bits to force in ts_Particle.u32Flags on particle creation
					iSceneIndex,									// which scenes to be rendered in, -1 means all scenes
					pEmitter->pPlaneList,						// restricting planes
					pEmitter->pPath,								// path for particles to follow
					NULL);											// pointer back to emitter for flux particles

				/* right side effect */

				vPos = vInterpolatedPos;
				vPos.x(vPos.x() - vSprayPosAdjust.x());
				vPos.z(vPos.z() - vSprayPosAdjust.z());
				ParticleMgr__SetOneShotEffectVelTransform(pEmitter->pVelTransform);	// one-shot
				ParticleMgr__SetOneShotEffectTarget(pEmitter->pvTarget);					// one-shot
				ParticleMgr__CreateEffect(
					pEmitter->nDataIndex,						// effect is in same data set as parent emitter
					pEmitter->u16EffectID,						// effect to create
					&vPos,											// world position to create at
					&vSprayVelRight,								// creation velocity unit direction vector, can be NULL
					pfVelMagnitude,								// creation velocity magnitude, can be NULL
					pvAdditiveVel,									// additive velocity, can be NULL
					-pEmitter->fTimer, 							// value for initial particle advance
					pEmitter->fScale * fScaleMultiplier,	// scale to apply
					pEmitter->nScaleRandomPct,					// scale randomness
					fConstantAlpha,								// constant alpha
					pEmitter->fColorMult,						// color multiplier
					u32ForceFlags,									// bits to force in ts_Particle.u32Flags on particle creation
					iSceneIndex,									// which scenes to be rendered in, -1 means all scenes
					pEmitter->pPlaneList,						// restricting planes
					pEmitter->pPath,								// path for particles to follow
					NULL);											// pointer back to emitter for flux particles

				/* done */

				break;
			}

			/* record last emission position and time, update timer */

			pEmitter->vLastEmitPos = vInterpolatedPos;
			pEmitter->fLastEmitTime = pEmitter->fTimer;
			pEmitter->fTimer += fTimerInc;
		}
//yuch-- randomness?
	}

	/* record last emitter position */

	pEmitter->vLastPos = vNewEmitPos;

	/* only lasting one more frame? */

	if (pEmitter->u32Flags & EMITTER_ADVANCE_ONE_MORE_FRAME)
		ParticleEmitter__Kill(pEmitter->Handle);
}

/* kill an emitter */

bool ParticleEmitter__Kill(t_Handle Handle)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_ParticleEmitter *pEmitter = PARTICLE_EMITTER_GET_RECORD(Handle);
	if (pEmitter->Handle != Handle)
		return(FALSE);

	/* clear assigned handle */

	pEmitter->Handle = INVALID_HANDLE;

	/* may need to free plane list */

	if (pEmitter->u32Flags & EMITTER_FREE_PLANE_LIST)
	{
		memFree(pEmitter->pPlaneList);
		pEmitter->pPlaneList = NULL;
	}

	/* move particle from active to free list */

	SLinkList__RemoveLink(&ParticleMgr.EmitterActiveList, pEmitter);
	SLinkList__AddLinkToHead(&ParticleMgr.EmitterFreeList, pEmitter);
	return(TRUE);
}

/* set scale info for an emitter */

bool ParticleEmitter__SetScale(t_Handle Handle, float fScale, int nRandomPct /*= 0*/)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_ParticleEmitter *pEmitter = PARTICLE_EMITTER_GET_RECORD(Handle);
	if (pEmitter->Handle != Handle)
		return(FALSE);

	/* set scale info */

	pEmitter->fScale = fScale;
	pEmitter->nScaleRandomPct = nRandomPct;
	return(TRUE);
}

/* control emitter's constant alpha */

bool ParticleEmitter__SetConstantAlpha(t_Handle Handle, float fConstantAlpha)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_ParticleEmitter *pEmitter = PARTICLE_EMITTER_GET_RECORD(Handle);
	if (pEmitter->Handle != Handle)
		return(false);

	/* set constant alpha-- use record's internal value */

	pEmitter->fConstantAlpha = fConstantAlpha;
	pEmitter->pfConstantAlpha = &pEmitter->fConstantAlpha;
	return(true);
}
bool ParticleEmitter__SetConstantAlpha(t_Handle Handle, float *pfConstantAlpha)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_ParticleEmitter *pEmitter = PARTICLE_EMITTER_GET_RECORD(Handle);
	if (pEmitter->Handle != Handle)
		return(false);

	/* set constant alpha-- point off to external value instead of internal value */

	pEmitter->pfConstantAlpha = pfConstantAlpha;
	return(true);
}

/* control emitter's color multiplier */

bool ParticleEmitter__SetColorMult(t_Handle Handle, float fColorMult)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_ParticleEmitter *pEmitter = PARTICLE_EMITTER_GET_RECORD(Handle);
	if (pEmitter->Handle != Handle)
		return(false);

	/* set color multiplier-- use record's internal value */

	pEmitter->fColorMult = fColorMult;
	pEmitter->pfColorMult = &pEmitter->fColorMult;
	return(true);
}
bool ParticleEmitter__SetColorMult(t_Handle Handle, float *pfColorMult)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_ParticleEmitter *pEmitter = PARTICLE_EMITTER_GET_RECORD(Handle);
	if (pEmitter->Handle != Handle)
		return(false);

	/* set color multiplier-- point off to external value instead of internal value */

	pEmitter->pfColorMult = pfColorMult;
	return(true);
}

/* set the position of an emitter */

bool ParticleEmitter__SetPosition(t_Handle Handle, Vector3CRef position )
{
	ASSERT( Handle != INVALID_HANDLE );
	ts_ParticleEmitter* pEmitter = PARTICLE_EMITTER_GET_RECORD( Handle );
	if ( pEmitter->Handle != Handle )
		return( false );

	pEmitter->vStationaryPos = position;
	pEmitter->pvPos = &pEmitter->vStationaryPos;

	return( true );
}

/* get emitter's current position */

Vector3 ParticleEmitter__GetPosition(t_Handle Handle)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_ParticleEmitter *pEmitter = PARTICLE_EMITTER_GET_RECORD(Handle);
	if (pEmitter->Handle != Handle)
		return(vZero);

	/* return position */

	return(*pEmitter->pvPos);
}

/* set lifetime info for an emitter */

bool ParticleEmitter__SetLifetime(t_Handle Handle, float fLifetime)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_ParticleEmitter *pEmitter = PARTICLE_EMITTER_GET_RECORD(Handle);
	if (pEmitter->Handle != Handle)
		return(false);

	/* set lifetime */

	pEmitter->fLifetime = fLifetime;
	return(true);
}

/* get the emitter's lifetime. emitters with a non-0 lifetime will die on their own */

float ParticleEmitter__GetLifetime(t_Handle Handle)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_ParticleEmitter *pEmitter = PARTICLE_EMITTER_GET_RECORD(Handle);
	if (pEmitter->Handle != Handle)
		return(0.0f);

	/* return lifetime */

	return(pEmitter->fLifetime);
}

/* set a plane list for an emitter. this defines a space outside of which particles will not be created. this is useful for
having an emitter that creates particles with randomized position, but wanting the shape of the space to be something other
than a axis-aligned box */

bool ParticleEmitter__SetPlaneList(t_Handle Handle, CPlaneList* pPlaneList, bool bMemFree /*= true*/)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_ParticleEmitter *pEmitter = PARTICLE_EMITTER_GET_RECORD(Handle);
	if (pEmitter->Handle != Handle)
		return(false);

	/* set plane list info */

	pEmitter->pPlaneList = pPlaneList;
	if (bMemFree)
		pEmitter->u32Flags |= EMITTER_FREE_PLANE_LIST;	// emitter is responsible for freeing plane list memory
	return(true);
}

/* velocity references the pointer passed in (so it had better stick around) */

bool ParticleEmitter__SetEjectVelPtr(t_Handle Handle, Vector3Packed *pEjectVel)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_ParticleEmitter *pEmitter = PARTICLE_EMITTER_GET_RECORD(Handle);

	if (pEmitter->Handle != Handle)
		return(false);

	if(pEjectVel == NULL)
		return(true);

	pEmitter->pvEjectVel = pEjectVel;
	pEmitter->u32Flags |= EMITTER_EJECT_VEL_CHANGES;

	return(true);
}

/* velocity is a copy of the vector passed in */

bool ParticleEmitter__SetEjectVel(t_Handle Handle, Vector3CRef ejectVel)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_ParticleEmitter *pEmitter = PARTICLE_EMITTER_GET_RECORD(Handle);

	if (pEmitter->Handle != Handle)
		return(false);

	// this is kind of wacked.  All of the below values are pre calculated when the
	// ejection velocity is non changing.  But the particle code looks at the pvEjectVel
	// pointer as being non null in order to use the static values. So I'm forcing an
	// odd non Null pointer value in here.
	/* ned says: nate's right, it's wacked. also make sure that the eject-vel-changes */
	/* flag is clear, otherwise the code will reference the pointer. at least i didn't */
	/* get the dreaded 0xCACACACA pointer */

	pEmitter->pvEjectVel = (Vector3Packed *)0xFAFAFAFA;
	ASSERT((pEmitter->u32Flags & EMITTER_EJECT_VEL_CHANGES) == 0);
	
	/* ejection velocity does not change, i can pre-calculate unit vector and magnitude */
	pEmitter->fVelMagnitude = ejectVel.Length();
	pEmitter->vVelUnitVector = ejectVel;
	pEmitter->vVelUnitVector.Normalize();

	return(true);
}

/* change size of flux emitter */

bool ParticleEmitter__SetFluxSize(t_Handle Handle, float fFluxSize)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_ParticleEmitter *pEmitter = PARTICLE_EMITTER_GET_RECORD(Handle);
	if (pEmitter->Handle != Handle)
		return(false);

	/* set ceiling */

	pEmitter->fFluxVolumeHalfSize = fFluxSize * 0.5f;
	return(true);
}

/* set the ceiling value for a flux emitter-- useful for confining flux particles to below water surface */

bool ParticleEmitter__SetFluxCeiling(t_Handle Handle, float fFluxCeiling)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_ParticleEmitter *pEmitter = PARTICLE_EMITTER_GET_RECORD(Handle);
	if (pEmitter->Handle != Handle)
		return(false);

	/* set ceiling */

	pEmitter->fFluxCeiling = fFluxCeiling;
	return(true);
}

/* emitter is allowed to advance and create effects even while particle manager is locked. this lets the loading screen
use particle emitters */

bool ParticleEmitter__AdvancesWhileManagerLocked(t_Handle Handle)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_ParticleEmitter *pEmitter = PARTICLE_EMITTER_GET_RECORD(Handle);
	if (pEmitter->Handle != Handle)
		return(false);

	/* mark status */

	pEmitter->u32Flags |= EMITTER_ADVANCES_WHILE_MANAGER_LOCKED;
	return(true);
}

/* make setting changes to freeze the particle emitter in place and have it advance one more frame. this routine
was written to fix a problem with emitters attached to missiles, and we need to assume that the emitter's owner
is going away, so any pointer to external data is no longer valid */

/*** this is a very specialized routine, use at your own risk ***/

bool ParticleEmitter__FreezeForOneMoreFrame(t_Handle Handle)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_ParticleEmitter *pEmitter = PARTICLE_EMITTER_GET_RECORD(Handle);
	if (pEmitter->Handle != Handle)
		return(false);

	/* lock it in place */

	pEmitter->vStationaryPos = *pEmitter->pvPos;
	pEmitter->pvPos = &pEmitter->vStationaryPos;	// point to stationary position

	/* velocity pointer, if any, can no longer be considered valid. since velocity pointer goes */
	/* away, we need to clear the only-emit-while-moving check */

	pEmitter->pvEmitterVel = NULL;
	pEmitter->u32Flags &= ~EMITTER_ACTIVE_MOVE_ONLY;

	/* other external pointers have to be assumed to be invalid */

	pEmitter->pvEjectVel = NULL;
	if (pEmitter->pfConstantAlpha != &pEmitter->fConstantAlpha)
		pEmitter->pfConstantAlpha = NULL;
	if (pEmitter->pfColorMult != &pEmitter->fColorMult)
		pEmitter->pfColorMult = NULL;
	pEmitter->pvUp = NULL;
	pEmitter->pVelTransform = NULL;
	pEmitter->pvTarget = NULL;
	pEmitter->pPlaneList = NULL;

	/* set flag to kill emitter after next advance */

	pEmitter->fLifetime = 0.0f;	// make sure it doesn't die before it has a chance to advance one more time
	pEmitter->u32Flags |= EMITTER_ADVANCE_ONE_MORE_FRAME;

	return(true);
}

/* add a spline path for the particles created by the emitter to follow. particles will always start at start of
path, and travel along it at speed dictated by magnitude of eject velocity. particle will die when it hits the
end of the path */

bool ParticleEmitter__SetParticlePath(t_Handle Handle, SplinePath *pPath)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_ParticleEmitter *pEmitter = PARTICLE_EMITTER_GET_RECORD(Handle);
	if (pEmitter->Handle != Handle)
		return(false);

	/* set path */

	pEmitter->pPath = pPath;
	return(true);
}
bool ParticleEmitter__HasParticlePath(t_Handle Handle)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_ParticleEmitter *pEmitter = PARTICLE_EMITTER_GET_RECORD(Handle);
	if (pEmitter->Handle != Handle)
		return(false);

	/* check value */

	return(pEmitter->pPath != NULL);
}

bool ParticleEmitter__UseOSTimer(t_Handle Handle, bool bUseOSTimer /*= true*/)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_ParticleEmitter *pEmitter = PARTICLE_EMITTER_GET_RECORD(Handle);
	if (pEmitter->Handle != Handle)
		return(false);

	/* set flag */

	if (bUseOSTimer)
		pEmitter->u32Flags |= EMITTER_USE_OS_TIMER;
	else
		pEmitter->u32Flags &= ~EMITTER_USE_OS_TIMER;
	return(true);
}

bool ParticleEmitter__SetLOD(t_Handle Handle, float fLOD1Radius, float fLOD2Radius)
{
	/* if handle does not match, do nothing */

	ASSERT(Handle != INVALID_HANDLE);
	ts_ParticleEmitter *pEmitter = PARTICLE_EMITTER_GET_RECORD(Handle);
	if (pEmitter->Handle != Handle)
		return(false);

	/* i've seen almost-but-not-quite values come through */

	if (Math::Equal(fLOD1Radius, MAX_E_LOD_RADIUS, 0.5f))
		fLOD1Radius = MAX_E_LOD_RADIUS;
	if (Math::Equal(fLOD2Radius, MAX_E_LOD_RADIUS, 0.5f))
		fLOD2Radius = MAX_E_LOD_RADIUS;

	/* set values */

	if (fLOD1Radius != MAX_E_LOD_RADIUS)
		pEmitter->fLOD1Radius = Math::Min(fLOD1Radius, MAX_E_LOD_RADIUS);	// passing in a big number is same as turning LOD off
	if (fLOD2Radius != MAX_E_LOD_RADIUS)
		pEmitter->fLOD2Radius = Math::Min(fLOD2Radius, MAX_E_LOD_RADIUS);
	ASSERTS(pEmitter->fLOD1Radius <= pEmitter->fLOD2Radius, "Particle emitter invalid LOD values");
	return(true);
}

/*********************** flux emitter routines ************************************/

/* handle final advancement of the flux emitter-- the part where it spits out effects */

void FluxEmitter__Advance(ts_ParticleEmitter *pEmitter, int iSceneIndex)
{
	/* has emitter moved? */

	if (*pEmitter->pvPos == pEmitter->vLastFluxEmitterPos)
	{
		ASSERT((pEmitter->u32Flags & EMITTER_FLUX_VOLUME_MOVED) == 0);	// gets cleared early on in ParticleEmitter__Advance
		return;
	}
	pEmitter->u32Flags |= EMITTER_FLUX_VOLUME_MOVED;	// emitter has moved, old particles need to check if they are still in volume

	/* calculate new volume(s) to create particles in. since flux volume is axis aligned, we can always cut new volume */
	/* into a maximum of 3 bounding boxes */

	Vector3 vSize(pEmitter->fFluxVolumeHalfSize, pEmitter->fFluxVolumeHalfSize, pEmitter->fFluxVolumeHalfSize);
	ts_BoundingBox NewFluxVolume;
	NewFluxVolume.Max = *pEmitter->pvPos + vSize;
	NewFluxVolume.Min = *pEmitter->pvPos - vSize;

	/* ceiling? flux emitters are typically used for bubbles suspended underwater */

	NewFluxVolume.Max.y( Math::Min(pEmitter->fFluxCeiling, NewFluxVolume.Max.y()) );
	NewFluxVolume.Min.y( Math::Min(pEmitter->fFluxCeiling, NewFluxVolume.Min.y()) );

	/* if this is the first time we have constructed the volume, then entire volume gets filled with particles */

	if (pEmitter->u32Flags & EMITTER_FLUX_FIRST_ADVANCE)
	{
		FluxEmitter__FillVolume(pEmitter, iSceneIndex, NewFluxVolume, 0.0f);
		pEmitter->u32Flags &= ~EMITTER_FLUX_FIRST_ADVANCE;
		pEmitter->vFluxFraction.Clear();
	}
	else
	{
		/* compare the new volume to the old volume, looking for places where it grew. since volume is axis-aligned, */
		/* at most we have 6 sub volumes to fill (and only if the volume grew in all 3 axes). typically, if the */
		/* volume stays the same size but moves in 3 axes, we have 3 new sub-volumes to fill */

		ts_BoundingBox WorkingVolume = NewFluxVolume;	// make a copy because we will be editing it as we go
		ts_BoundingBox SubVolume;

		/* first y */

		if (NewFluxVolume.Max.y() > pEmitter->FluxVolume.Max.y())
		{
			/* we want portion of new volume that is above old volume */

			SubVolume = WorkingVolume;
			// check to make sure old and new boxes overlap-- if not, just fill whole new volume in this direction
			SubVolume.Min.y(
				Math::Max(SubVolume.Min.y(), pEmitter->FluxVolume.Max.y()));
			pEmitter->vFluxFraction.y( FluxEmitter__FillVolume(pEmitter, iSceneIndex, SubVolume, pEmitter->vFluxFraction.y()));

			/* reduce top of working volume to top of old volume so that we don't fill it twice in subsequent checks */

			WorkingVolume.Max.y( pEmitter->FluxVolume.Max.y() );
		}
		if (NewFluxVolume.Min.y() < pEmitter->FluxVolume.Min.y())
		{
			/* we want portion of new volume that is below old volume */

			SubVolume = WorkingVolume;
			// check to make sure old and new boxes overlap-- if not, just fill whole new volume in this direction
			SubVolume.Max.y(
				Math::Min(SubVolume.Max.y(), pEmitter->FluxVolume.Min.y()));
			pEmitter->vFluxFraction.y( FluxEmitter__FillVolume(pEmitter, iSceneIndex, SubVolume, pEmitter->vFluxFraction.y()));

			/* reduce bottom of working volume to bottom of old volume so that we don't fill it twice in subsequent checks */

			WorkingVolume.Min.y(pEmitter->FluxVolume.Min.y());
		}

		/* then x. working volume has been adjusted so that we don't re-fill volume that is intersection of new x-volume */
		/* and new y-volume */

		if (NewFluxVolume.Max.x() > pEmitter->FluxVolume.Max.x())
		{
			/* we want portion of new volume that is to left of old volume (looking down positive z-axis) */

			SubVolume = WorkingVolume;
			// check to make sure old and new boxes overlap-- if not, just fill whole new volume in this direction
			SubVolume.Min.x(
				Math::Max(SubVolume.Min.x(), pEmitter->FluxVolume.Max.x()));
			pEmitter->vFluxFraction.x(FluxEmitter__FillVolume(pEmitter, iSceneIndex, SubVolume, pEmitter->vFluxFraction.x()));

			/* reduce left of working volume to left of old volume so that we don't fill it twice in subsequent checks */

			WorkingVolume.Max.x( pEmitter->FluxVolume.Max.x() );
		}
		if (NewFluxVolume.Min.x() < pEmitter->FluxVolume.Min.x())
		{
			/* we want portion of new volume that is to right of old volume (looking down positive z-axis) */

			SubVolume = WorkingVolume;
			// check to make sure old and new boxes overlap-- if not,
			// just fill whole new volume in this direction
			SubVolume.Max.x(
				Math::Min(pEmitter->FluxVolume.Min.x(), SubVolume.Max.x()));
			pEmitter->vFluxFraction.x( FluxEmitter__FillVolume(pEmitter, iSceneIndex, SubVolume, pEmitter->vFluxFraction.x()));

			/* reduce right of working volume to right of old volume so that we don't fill it twice in subsequent checks */

			WorkingVolume.Min.x( pEmitter->FluxVolume.Min.x() );
		}

		/* then z. working volume has been adjusted so that we don't re-fill volume that is intersection of new z-volume */
		/* and new x/y-volumes */

		if (NewFluxVolume.Max.z() > pEmitter->FluxVolume.Max.z())
		{
			/* we want portion of new volume that is in back of old volume (looking down positive z-axis) */

			SubVolume = WorkingVolume;
			// check to make sure old and new boxes overlap-- if not,
			// just fill whole new volume in this direction
			SubVolume.Min.z(
				Math::Max(pEmitter->FluxVolume.Max.z(), SubVolume.Min.z()));
			pEmitter->vFluxFraction.z( FluxEmitter__FillVolume(pEmitter, iSceneIndex, SubVolume, pEmitter->vFluxFraction.z()));

			/* reduce back of working volume to back of old volume so that we don't fill it twice in subsequent checks */

			WorkingVolume.Max.z( pEmitter->FluxVolume.Max.z() );
		}
		if (NewFluxVolume.Min.z() < pEmitter->FluxVolume.Min.z())
		{
			/* we want portion of new volume that is in front of old volume (looking down positive z-axis) */

			SubVolume = WorkingVolume;
			// check to make sure old and new boxes overlap-- if not,
			// just fill whole new volume in this direction
			SubVolume.Max.z(
				Math::Min(pEmitter->FluxVolume.Min.z(), SubVolume.Max.z()));
			pEmitter->vFluxFraction.z( FluxEmitter__FillVolume(pEmitter, iSceneIndex, SubVolume, pEmitter->vFluxFraction.z()) );

			/* reduce front of working volume to front of old volume so that we don't fill it twice in subsequent checks */

			WorkingVolume.Min.z( pEmitter->FluxVolume.Min.z());
		}
	}

	/* copy new volume into emiiter to be used for old particle checks */

	pEmitter->FluxVolume = NewFluxVolume;

	/* record last position */

	pEmitter->vLastFluxEmitterPos = *pEmitter->pvPos;
}

float FluxEmitter__FillVolume(ts_ParticleEmitter *pEmitter, int iSceneIndex, ts_BoundingBox &FluxVolume, float fFraction)
{
	/* what is the volume? */

	float fVolume = (FluxVolume.Max.x() - FluxVolume.Min.x()) * (FluxVolume.Max.y() - FluxVolume.Min.y()) * (FluxVolume.Max.z() - FluxVolume.Min.z());
	if (Math::Zero(fVolume))
		return(fFraction);	// nothing to do

	/* get size of volume */

	Vector3 vSize = FluxVolume.Max - FluxVolume.Min;

	/* fDelay (in seconds) used to be nEffectsPerSecond. i used nEffectsPerSecond as a density, since there is */
	/* a logical link between larger value == higher density. convert delay back into effects per second */

	float fEffectsPerSecond = 1.0f / pEmitter->fDelay;

	/* we use effects per second as a density (particles/10 cubic meters). since the volumes can be very small (paper- */
	/* thin), we keep track of fractional values and pass them back in next time for accurate results */

	float fEffects = (fVolume * fEffectsPerSecond * 0.1f) + fFraction;
	while (fEffects >= 1.0f)
	{
		fEffects -= 1.0f;

		/* pick a random spot within volume */

		Vector3Packed vPos = FluxVolume.Min;
		vPos.Set(ParticleRandom.Unit(), ParticleRandom.Unit(), ParticleRandom.Unit());
		vPos = vPos.Diag(vSize);

		/* flag bits to force in particles created by effect that emitter emits */

		u32 u32ForceFlags = PARTICLE_FROM_FLUX_EMITTER;
		if (pEmitter->u32Flags & EMITTER_USE_OS_TIMER)
			u32ForceFlags |= PARTICLE_USE_OS_TIMER;	// inherit use-os-timer flag

		/* create effect */

		ParticleMgr__CreateEffect(
			pEmitter->nDataIndex,			// effect is in same data set as parent emitter
			pEmitter->u16EffectID,			// effect to create
			&vPos,								// world position to create at
			NULL,									// creation velocity unit direction vector, can be NULL
			NULL,									// creation velocity magnitude, can be NULL
			NULL,									// additive velocity, can be NULL
			0.0f,					 				// value for initial particle advance
			pEmitter->fScale,					// scale to apply
			pEmitter->nScaleRandomPct,		// scale randomness
			1.0f,									// constant alpha
			1.0f,									// color multiplier
			u32ForceFlags,						// bits to force in ts_Particle.u32Flags on particle creation
			iSceneIndex,						// which scenes to be rendered in, -1 means all scenes
			NULL,									// restricting planes
			NULL,									// path for particles to follow
			pEmitter);							// point back to emitter for flux particles
	}

	/* return left over value */

	return(fEffects);
}

/* this is basically for pigpen, though might get used in game i suppose-- there will be a visual discontinuity as old
particles are killed */

void FluxEmitter__UpdateDensity(ts_ParticleEmitter *pEmitter, float fDelay)
{
	/* maybe nothing to do */

	if (fDelay == pEmitter->fDelay)
		return;

	/* kill all particles attached to this emitter */

	ts_Particle *pNextParticle = (ts_Particle *) ParticleMgr.ActiveList[pEmitter->nDataIndex].pHead;
	while (pNextParticle)
	{
		/* pre-load next in case current one gets killed */

		ts_Particle *pParticle = pNextParticle;
		pNextParticle = pParticle->pNext;

		/* if from this emitter, kill */

		if (pParticle->pFluxEmitter == pEmitter)
			Particle__Kill(pParticle);
	}

	/* update effects per second */

	pEmitter->fDelay = fDelay;

	/* re-mark this flux emitter as new-- the next time it advances it needs to fill the whole volume */

	pEmitter->u32Flags |= EMITTER_FLUX_FIRST_ADVANCE;

	/* guarantee that flux emitter advance will find movement */

	pEmitter->vLastFluxEmitterPos.x( MAXFLOAT );
}

/*********************** effect routines ************************************/

// pass an effect off to the particulate system
static void ParticleEffect__CreateInParticulateSystem(ts_ParticleCreate *pParticleCreate, ts_Channel *pChannel)
{
//	dbgPrint("Particulate Manager selected\n");
	Vector3 vel;
	Vector3 pos;
	ts_ParticleSource* source = pChannel->pParticleSource;

	// use whatever we can of the create structure and move along...
	if(pParticleCreate->pvPos)
		pos = *pParticleCreate->pvPos;
	else
		pos = source->vInitialPos;
	if(pParticleCreate->pvVelUnitVector)
		vel = *pParticleCreate->pvVelUnitVector;
	else
		vel.Set(CONVERT_S8_TO_FLOAT(source->s8InitialVelX, 1.0f),
				  CONVERT_S8_TO_FLOAT(source->s8InitialVelY, 1.0f),
				  CONVERT_S8_TO_FLOAT(source->s8InitialVelZ, 1.0f));
	
	float velmag;
	if(pParticleCreate->pfVelMagnitude)
		velmag = *pParticleCreate->pfVelMagnitude;
	else
		velmag = source->fInitialVel;

	Vector3 veladd(0.0f, 0.0f, 0.0f);
	if(pParticleCreate->pvAdditiveVel)
		veladd = *pParticleCreate->pvAdditiveVel;

	if (pParticleCreate->pVelTransform)
		vel.FastBasisTransform(vel, *pParticleCreate->pVelTransform);

	ParticleMgr.pParticulateSystem->CreateEffect(pChannel, pos, vel, velmag, veladd, pParticleCreate->fScale, pParticleCreate->fConstantAlpha, pParticleCreate->fColorMult);
}

static bool ParticleCreate__CompatibleWithParticulateSystem(ts_ParticleCreate *pParticleCreate)
{
	return pParticleCreate->iSceneIndex == -1 &&
		pParticleCreate->fFirstAdvanceTime == 0.0f &&
		(pParticleCreate->u32ForceFlags & (PARTICLE_USE_OS_TIMER |
													  PARTICLE_FOLLOWS_PATH |
													  PARTICLE_ALIGN_TO_VELOCITY |
													  PARTICLE_NEEDS_TEXTURE_ANIM |
													  PARTICLE_ONLY_SCENE_1 |
													  PARTICLE_ONLY_SCENE_1 |
													  PARTICLE_ONLY_SCENE_1 |
													  PARTICLE_ONLY_SCENE_4 |
													  PARTICLE_LEASHED |
													  PARTICLE_HAS_NORMAL |
													  PARTICLE_WAS_ALIGN_TO_VELOCITY |
													  PARTICLE_GROUND_SPECIAL_CHECK)) == 0 &&
		pParticleCreate->pvTarget == NULL &&
		pParticleCreate->pPlaneList == NULL &&
		pParticleCreate->pFluxEmitter == NULL &&
		pParticleCreate->pPath == NULL;
}

/* create an effect. this is the one that actually creates the particles */

bool ParticleEffect__Create(ts_ParticleCreate *pParticleCreate)
{
	int i;
	int j;
	ts_Effect *pEffect;
	ts_Channel *pChannel;

#ifdef PRINT_PARTICLE_CREATION
	/* spit out effect's name */

	ts_RecordList *pRecordList = (ts_RecordList *) dblGetChunkData(DB_EFFECT_LIST, DEFAULT_CHUNK_KEY_STRING, ParticleMgr.DBHandle[pParticleCreate->nDataIndex]);
	if (pRecordList)
	{
		char *pRecordName = (char *) GetRecordAddress(pRecordList, pParticleCreate->nEffectID);
		dbgPrint("Creating effect %s, %d active particles\n", pRecordName, ParticleMgr__GetActiveCount());
	}
#endif //PRINT_PARTICLE_CREATION

	pEffect = (ts_Effect *) GetRecordAddress(ParticleMgr.pEffectList[pParticleCreate->nDataIndex], pParticleCreate->nEffectID);


	/* a particle effect is one or more channels. each channel creates one or more particles, sounds, or events. */
	/* one channel can create only one kind of particle, sound, or event. that is, if a channel creates a */
	/* smoke particle, it can create some number of smoke particles, but smoke particles is all it can create */
	
	pChannel = pEffect->ChannelData;

	for (i = 0; i < (signed) pEffect->u32ChannelCount; i++, pChannel++)
	{
		/* sound is easy */
		if (pChannel->u8Flags & CHANNEL_IS_SOUND)
			continue;

#if defined(USE_PARTICULATE_MGR)
		/* if the effect is compatible with the particulate system, which is faster, create it in there */
		if(ParticleCreate__CompatibleWithParticulateSystem(pParticleCreate) && 
			ParticleMgr.pParticulateSystem->IsCompatible(pChannel))
		{
			ParticleEffect__CreateInParticulateSystem(pParticleCreate, pChannel);
		}
		else
#endif
		{
			int nEmitCount;
			float fActivationDelay = pChannel->fEmitDelay;	// delay before first particle is emitted
			float fSubsequentDelay = pChannel->fEmitTime / (float) pChannel->u8EmitCount;	// delay before each subsequent particle is emiitted
	
			/* how many particles to create? */
	
			nEmitCount = pChannel->u8EmitCount;

	//yuch-- randomize emit count
			for (j = 0; j < nEmitCount; j++)
			{
				// TJC - Added this code to speed up rediculous requests.
				// But I dont know if I should return TRUE or FALSE.  To stay
				// compatible with the old code, Im returning TRUE, even though
				// no particles may have been created. (Note also that this routine
				// can never return FALSE, and nothing ever pays attention to
				// the return code).  I was just going to use the return value from
				// Particle__Create, except that it may fail if we are just barely
				// beyond a suppression plane.
				if(ParticleMgr.FreeList.pHead == NULL)
					return(TRUE);

				/* create the particle */
	
				Particle__Create(
					pParticleCreate->nDataIndex,			// which of possibly-multiply-loaded particle data files to use
					(Vector3Packed*)pParticleCreate->pvPos,					// world position to create at, can be NULL (if so, will use particle source data)
					(Vector3Packed*)pParticleCreate->pvVelUnitVector,	// velocity to create at, can be NULL (if so, will use particle source data)
					pParticleCreate->pfVelMagnitude,		// velocity to create at, can be NULL (if so, will use particle source data)
					(Vector3Packed*)pParticleCreate->pvAdditiveVel,		// additive velocity to use, can be NULL (if so, no additive velocity)
					pParticleCreate->u32ForceFlags,		// forced bits for ts_Particle.u32Flags
					pParticleCreate->fFirstAdvanceTime,	// first Particle__Advance time slice, 0.0 is a good value
					pParticleCreate->fScale,				// scale to apply to particle size
					pParticleCreate->fConstantAlpha,		// overall alpha to multiply against alpha envelope
					pParticleCreate->fColorMult,			// overall value to multiply against color envelope
					fActivationDelay,							// activation delay-- particle will go active when this counts down to 0
					pChannel->pParticleSource,				// particle source data
					pParticleCreate->iSceneIndex,			// index of scene to draw particle in. -1 = all scenes
					pParticleCreate->pVelTransform,		// possible transform for calculated particle initial velocity
					pParticleCreate->pvTarget,				// possible target for particle to move toward
					pParticleCreate->pPlaneList,			// restricting plane list
					pParticleCreate->pPath,					// path for particle to follow
					pParticleCreate->pFluxEmitter);		// pointer to flux emitter for flux particles
	
				/* update delay */
	
				fActivationDelay += fSubsequentDelay;
	//yuch-- randomize subsequent delay on CHANNEL_EMIT_TIME_CONT_RANDOM flag
			}
		}
	}

	/* success */

	return(TRUE);
}

//Used to detect whether or not a particle is in the suppression box
bool IsInside(const DirCos3x3& i_mBoxOrientation, Vector3CRef i_vBoxPosition, Vector3CRef i_vPoint)
{
	sPlane a, b, c, d, e, f;
	Vector3 v;

	v = i_mBoxOrientation.GetRow0();
	a.p = i_vBoxPosition + v;
	a.n = -v;

	b.p = i_vBoxPosition - v;
	b.n = v;

	v = i_mBoxOrientation.GetRow1();
	c.p = i_vBoxPosition + v;
	c.n = -v;

	d.p = i_vBoxPosition - v;
	d.n = v;

	v = i_mBoxOrientation.GetRow2();
	e.p = i_vBoxPosition + v;
	e.n = -v;

	f.p = i_vBoxPosition - v;
	f.n = v;

	v = i_vPoint - a.p;

	if (Vector3::Dot(a.n, v) < 0.0f)
		return false;

	v = i_vPoint - b.p;

	if (Vector3::Dot(b.n, v) < 0.0f)
		return false;

	v = i_vPoint - c.p;

	if (Vector3::Dot(c.n, v) < 0.0f)
		return false;

	v = i_vPoint - d.p;

	if (Vector3::Dot(d.n, v) < 0.0f)
		return false;

	v = i_vPoint - e.p;

	if (Vector3::Dot(e.n, v) < 0.0f)
		return false;

	v = i_vPoint - f.p;

	if (Vector3::Dot(f.n, v) < 0.0f)
		return false;

	return true;
}



//////////////////////////////////////////////////////////////////////////////////
// Here lies most of the platform specific code broken out by platform
//////////////////////////////////////////////////////////////////////////////////

/////////////////////////
// DirectX
/////////////////////////

#if defined (DIRECTX)

static u32 ParticleMgr__AddParticle(ts_Particle * pParticle, u32 &nParticleCount)
{
	static Vector2 vParticleUV[4] = {Vector2(0.0f, 0.0f), Vector2(1.0f, 0.0f), Vector2(1.0f, 1.0f), Vector2(0.0f, 1.0f)};
	int nListIndex = nParticleCount*4;

	if( pParticle->pSource->u32Flags & PARTICLE_RENDER_FOR_DISTORT )
		return( 0);

	CSceneStats& stats = SceneMgr::GetStats();
	stats.IncParticlesDrawn(1);

	// basic particle vertices, normals, uv coords
	Vector3 vWorldCenter;
	Vector3 vParticleWorldVertex[4];
	Vector3 vNormal;
	u32 u32State = pParticle->pSource->u32Flags & PARTICLE_STATE_FLAGS;
	ParticleMgr__TransformParticle(pParticle, vWorldCenter, vParticleWorldVertex, vNormal, u32State);
	
	memcpy(&ParticleMgr.pVertexList[nListIndex], vParticleWorldVertex, 4 * sizeof(Vector3));
	memcpy(&ParticleMgr.pUVList[nListIndex], vParticleUV, 4 * sizeof(Vector2));
	ParticleMgr.pColorList[nListIndex + 0] = pParticle->Color;
	ParticleMgr.pColorList[nListIndex + 1] = pParticle->Color;
	ParticleMgr.pColorList[nListIndex + 2] = pParticle->Color;
	ParticleMgr.pColorList[nListIndex + 3] = pParticle->Color;

	// update list index, list vertex count 
	nParticleCount++;
	return 1;
}
#endif // DIRECTX

/////////////////////////
// GCN
/////////////////////////

#ifdef GCN

static u32 ParticleMgr__AddParticle(ts_Particle * pParticle, u32 &nParticleCount)
{
	static Vector2 vParticleUV[4] = {Vector2(0.0f, 0.0f), Vector2(1.0f, 0.0f), Vector2(1.0f, 1.0f), Vector2(0.0f, 1.0f)};
//	static Vector2 vParticleUV[4] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};
	int nListIndex = nParticleCount*4;

	if( pParticle->pSource->u32Flags & PARTICLE_RENDER_FOR_DISTORT )
		return( 0);

	CSceneStats& stats = SceneMgr::GetStats();
	stats.IncParticlesDrawn(1);

	Vector3		*pVertexList = ParticleMgr.pVertexList + nListIndex;
	Vector2		*pUVList = ParticleMgr.pUVList + nListIndex;
	ts_bRGBA		*pColorList = ParticleMgr.pColorList + nListIndex;

	// basic particle vertices, normals, uv coords
	Vector3 vWorldCenter;
	Vector3 vParticleWorldVertex[4];
	Vector3 vNormal;
	ParticleMgr__TransformParticle( pParticle, vWorldCenter, vParticleWorldVertex, vNormal, u32State);
	
	pVertexList[0] = vParticleWorldVertex[0];
	pVertexList[1] = vParticleWorldVertex[1];
	pVertexList[2] = vParticleWorldVertex[2];
	pVertexList[3] = vParticleWorldVertex[3];
	pUVList[0] = vParticleUV[0];
	pUVList[1] = vParticleUV[1];
	pUVList[2] = vParticleUV[2];
	pUVList[3] = vParticleUV[3];
	pColorList[0] = pParticle->Color;
	pColorList[1] = pParticle->Color;
	pColorList[2] = pParticle->Color;
	pColorList[3] = pParticle->Color;

	/* particles implement fogging by alpha'ing out over fog distance */
	/* z-camera distance comes out negative, make it positive. this calculation is just the */
	/* z-result of putting the instance's translation vector through the world-to-camera matrix */

	if (ParticleMgr.bFogEnabled)
	{
		Vector3 vCenter = 0.5f * (vParticleWorldVertex[0] + vParticleWorldVertex[2]);	// takes orbiters into account
		float fDistance = -((g4WorldToCamera.r02 * vCenter.x( )) +
								  (g4WorldToCamera.r12 * vCenter.y( )) +
								  (g4WorldToCamera.r22 * vCenter.z( )) +
								  g4WorldToCamera.tz);
		if (fDistance > ParticleMgr.fNearFog)
		{
			if (fDistance >= ParticleMgr.fFarFog)
				return(0);	// past far fog distance, bail

			/* i originally implemented a linear drop off from 1.0 at near fog to 0.0 at far fog, but we'd like */
			/* a squared drop-off. so calculate linear value from 0.0 at near, 1.0 at far, square it, and */
			/* subtract it from 1.0 */

			float fAlpha = (fDistance - ParticleMgr.fNearFog) * ParticleMgr.fOneOverFogDelta;
			fAlpha = 1.0f - (fAlpha * fAlpha);
			pColorList[0].A = (u8) ((float)pColorList[0].A * fAlpha);
			pColorList[1].A = pColorList[0].A;
			pColorList[2].A = pColorList[0].A;
			pColorList[3].A = pColorList[0].A;
		}
	}

	// update list index, list vertex count 
	nParticleCount++;
	return 1;
}
#endif

/////////////////////////
// PS2
/////////////////////////

#ifdef PS2

// the microcode expects particles as:
// W | R,    X  | G,    H  | B,    Y  | A
// Z | f1,   Ox | f2,   Oy | f3,   Oz | f4
//
// where the O parameters are additional orientation information (ie
// either the particles velocity or its arbitary rotation). and the f*
// parameters are flags.  W is width, H is height, XYZ are obvious,
// RGBA should be too.
//



extern unsigned int	Particle_DmaTag		__attribute__( ( section( ".vudata" )));
extern unsigned int	Particle_CodeStart	__attribute__( ( section( ".vudata" )));
extern unsigned int	Particle_CodeEnd		__attribute__( ( section( ".vudata" )));

#define MAX_PARTICLES_IN_ONE_HANDFULL	 ((MASTERLIST_QW_MAX_CHAIN_SIZE-2) / 2)

void ParticleMgr__PS2PrepRender(void)
{
	// since we have to call GSState__Send anyway, lets send down clamp and tex1 & prim via GSState
	GSState__Set(SCE_GS_TEX1_1, SCE_GS_SET_TEX1_1(0, 0, 1, 1, 1, 0, 0));
	GSState__Set(SCE_GS_CLAMP_1, SCE_GS_SET_CLAMP_1(1, 1, 0, 0, 0, 0));

	// activate desired gs state
	GSState__Send();

	// make sure it will be enough space for us
	ASSERT(MASTERLIST_QW_MAX_CHAIN_SIZE > 1 + INIT_QWC);

	// send up the microcode
	MasterList::AppendDMATAG(INIT_QWC + 1, TAGID_CALL, &Particle_DmaTag);
	MasterList::Append(createBASE(VU_BASE));
	MasterList::Append(createOFFSET(VU_OFFSET));
	MasterList::Append(createITOP(VU_ITOP));
	MasterList::Append(createNOP());
	MasterList::Append(createNOP());
	MasterList::Append(createUNPACK(0, V4_32, INIT_QWC));

	// geometry primitive, scene number, and fog constants. fog is implemented by alpha'ing out each particle
	// over the fog range. the fog constants are used to convert the particle's z-value in clip space into an
	// alpha multiplier to multiply against the particle's color alpha component. at the fog near plane, the
	// resulting multiplier should 1.0; at far it should be 0.0. the calculation is alpha = (z * scale) + translate,
	// then alpha is clamped to 0.0..1.0
	MasterList::Append4(
		SCE_GS_SET_PRIM(			/* prim */																		\
			SCE_GS_PRIM_TRIFAN,	/* prim(3)-- primitive type */											\
			1,							/* iip(1)-- shading method (0 = flat, 1 = gouraud) */				\
			1,							/* tme(1)-- texture mapping enable */									\
			0,							/* fge(1)-- fogging enable */												\
			1,							/* abe(1)-- alpha blending enable */									\
			0,							/* aa1(1)-- antialiasing enable */										\
			0,							/* fst(1)-- texture coord (0 = STQ, 1 = UV) */						\
			0,							/* ctxt(1)-- context */														\
			0));						/* fix(1)-- fragment value control(0 = normal, 1 = fixed) */
	MasterList::Append4(0); // scene #
	if (ParticleMgr.bFogEnabled)
	{
		/* i originally implemented a linear drop off from 1.0 at near fog to 0.0 at far fog, but we'd like */
		/* a squared drop-off. so calculate linear value from 0.0 at near, 1.0 at far, square it, and */
		/* subtract it from 1.0. this means that the fog constants have to be different */
#define ALPHA_SQUARED_DROPOFF	// if you change this you have to update the vu code as well!
#ifdef ALPHA_SQUARED_DROPOFF
		/* we want a line such that y = 0.0 at x = near fog, and y = 1.0 at x = far fog. doing some simple geometry, */
		/* we get y = (1 / (far - near)) * x - (near / (far - near)). you can confirm that plugging in */
		/* x = near and x = far produces the correct results */

		MasterList::Append(-ParticleMgr.fNearFog * ParticleMgr.fOneOverFogDelta);	// translate value
		MasterList::Append(ParticleMgr.fOneOverFogDelta);									// scale value
#else //ALPHA_SQUARED_DROPOFF
		/* we want a line such that y = 1.0 at x = near fog, and y = 0.0 at x = far fog. doing some simple geometry, */
		/* we get y = (-1 / (far - near)) * x + (1 + (near / (far - near))). you can confirm that plugging in */
		/* x = near and x = far produces the correct results */

		MasterList::Append(1.0f + (ParticleMgr.fNearFog * ParticleMgr.fOneOverFogDelta));	// translate value
		MasterList::Append(-ParticleMgr.fOneOverFogDelta);												// scale value
#endif //ALPHA_SQUARED_DROPOFF
	}
	else
	{
		/* these values will always produce a value of 1.0 for the alpha multiplier (y = 1 - (0 * x + 0) */

		MasterList::Append(0.0f);	// translate value
		MasterList::Append(0.0f);	// scale value
	}

	// 3 qw for light directions, 4 qw for light colors, 1 qw for ambient light color
	RenderState__SetParticleLights();

	// one unused quad word
	MasterList::Append16(0);

	// now there are some matrices we want to send up
	MasterList::AppendMtx(&g4CameraToWorld);
	MasterList::AppendMtx(&g4WorldToCamera);
	MasterList::AppendMtx(&PS2Renderer::GetWorldToClip());
	MasterList::AppendMtx(&PS2Renderer::GetClipToScreen());

	// now the suppression boxes...
	// box 0
	MasterList::Append16(0);
	MasterList::Append16(0);
	MasterList::Append16(0);
	MasterList::Append16(0);
	// box 1
	MasterList::Append16(0);
	MasterList::Append16(0);
	MasterList::Append16(0);
	MasterList::Append16(0);
	// box 2
	MasterList::Append16(0);
	MasterList::Append16(0);
	MasterList::Append16(0);
	MasterList::Append16(0);
	// box 3
	MasterList::Append16(0);
	MasterList::Append16(0);
	MasterList::Append16(0);
	MasterList::Append16(0);
	// box 4
	MasterList::Append16(0);
	MasterList::Append16(0);
	MasterList::Append16(0);
	MasterList::Append16(0);

	// Tell masterlist that it needs to fix base & offset and it will need to flush when things start back up...
	MasterList::SetFlushed(false);
	MasterList::SetBase(VU_BASE);
	MasterList::SetOffset(VU_OFFSET);
	PS2Renderer::JustTrashedVUInstructionMem((u_int)&Particle_CodeStart, (u_int)&Particle_CodeEnd - (u_int)&Particle_CodeStart);

#ifndef CDROM
	/* test assumptions on how to implement particle z-bias on ps2. do this once per program execution, will */
	/* fire the first time there is a particle to render, should get a good random condition. particle z-bias */
	/* is implemented by changing the z-buffer range so that pixels are rendered in the same place, but the */
	/* z-buffer thinks that the particle is closer to the camera, and the z-buffer test takes care of things. */
	/* on the pc, this is done by adjusting the tz element of the projection matrix. on the ps2, the particle */
	/* vu code is handed a "WorldToClip" matrix, which is 3 matrices multiplied together-- the world-to-camera, */
	/* the camera-to-view (what i would call the projection matrix), and a view-to-clip matrix that i don't */
	/* understand (it has to do with screen dimensions). it turns out that changing the projection matrix tz */
	/* element and re-multiplying the matrix just carries the adjustment straight through to the world-to-clip */
	/* matrix, so we can just pass a single adjusted tz element value down to the particle renderer and have */
	/* it put it in the correct spot in the matrix. i haven't written out all the math for how that element */
	/* is calculated. this section of code is intended to test the assumption */

	static bool bTested = false;
	if (bTested == false)
	{
		bTested = true;

		/* save projection matrix */

		MatrixStack__Mode(MS_PROJECTION);
		MatrixStack__Push();

		/* get current ps2 world-to-clip matrix. the tortured syntax to read the world-to-clip matrix */
		/* into the local variable is necessary to satisfy both CW and SN compilers. the declaration */
		/* is: const PS2Matrix4x4 &GetWorldToClip(), and we have to essentially override the const */ 

		PS2Matrix4x4 Matrix;
		Matrix = *(PS2Matrix4x4 *) PS2Renderer::GetWorldToClip();

		/* update projection matrix by a known amount to simulate pc implementation */

		Matrix4x4 mProjection = *MatrixStack__Get(MS_PROJECTION);
		Vector3 vRow3;
		vRow3 = mProjection.GetRow3();
		vRow3.z(vRow3.z() - 0.25f);
		mProjection.SetRow3(vRow3);
		MatrixStack__Load(&mProjection);

		/* get updated ps2 world-to-clip matrix */

		PS2Matrix4x4 NewMatrix;
		NewMatrix = *(PS2Matrix4x4 *) PS2Renderer::GetWorldToClip();

		/* restore projection matrix */

		MatrixStack__Pop();

		/* compare matrices */

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if ((i == 3) && (j == 2))
					ASSERTS(Math::Equal(NewMatrix[i][j], Matrix[i][j] - 0.25f, 0.0001f), "Bad ps2 z-bias implementation");
				else
					ASSERTS(Math::Equal(NewMatrix[i][j], Matrix[i][j], 0.0001f), "Bad ps2 z-bias implementation");
			}
		}
	}
#endif //CDROM
}

void ParticleMgr__PS2PrepDrawList()
{
	// this code has problems... for now we need to flush before and after any object using dynamic textures
	// for now I'll just do it here.. :)...
	MasterList::AppendDMATAG();
	MasterList::Append(createFLUSH());
	MasterList::Append(createMSCAL((u_int)&Particle_CodeStart / 8));
}

void ParticleMgr__StartDrawList()
{
	ASSERT(ParticleMgr.pCurrPos == NULL);

	// you have to set this the first time you start a drawlist!!
	ASSERT(ParticleMgr.nOffset != 0);

	ParticleMgr.pInitPos = ParticleMgr.pCurrPos = MasterList::StartMiscData();

	// enough space for the unpack below
	MasterList::Append8(ParticleMgr.pCurrPos, MasterList::createDMATAG());
	MasterList::Append(ParticleMgr.pCurrPos, createNOP());
	MasterList::Append(ParticleMgr.pCurrPos, createNOP());
	ParticleMgr.nQWC = 0;
}

void ParticleMgr__EndDrawList()
{
	ASSERT(ParticleMgr.pCurrPos);

	MasterList::Append8(ParticleMgr.pInitPos, MasterList::createDMATAG(ParticleMgr.nQWC));
	MasterList::Append(ParticleMgr.pInitPos, createNOP());
	MasterList::Append(ParticleMgr.pInitPos, createUNPACK(ParticleMgr.nOffset, V4_32, ParticleMgr.nQWC, true));

	MasterList::EndMiscData(ParticleMgr.pCurrPos);

	ParticleMgr.pCurrPos = NULL;

	ParticleMgr.nOffset += ParticleMgr.nQWC;
}

u32 ParticleMgr__AddParticle(ts_Particle * pParticle, u32 &nParticleCount)
{
	CSceneStats& stats = SceneMgr::GetStats();
	stats.IncParticlesDrawn(1);

	// deal with updateing the spr pointer
	if(ParticleMgr.pCurrPos == NULL)
	{
		ParticleMgr.nOffset = PARTICLE_BUFFER_HEAD;
		ParticleMgr__StartDrawList();
	}
	else if(ParticleMgr.nQWC + 2 >= MASTERLIST_QW_MAX_CHAIN_SIZE)
	{

		ParticleMgr__EndDrawList();
		ParticleMgr__StartDrawList();
	}

	ParticleMgr.nQWC += 2;

	/* handle orbiting particles here, there's no room to pass the necessary data to the vu */

	Vector3Packed *pvPos;
	if (pParticle->fOrbitRadius == 0.0f)
		pvPos = &pParticle->vPos;
	else
		pvPos = Particle__CalculateOrbit(pParticle);

	// generate the particle
	u32 flags1 = 0;
	u32 flags2 = 0;
	u32 flags3;
	flags3 = ((pParticle->pSource->u32Flags & PARTICLE_ORIENT_MASK) >> PARTICLE_ORIENT_SHIFT);
	flags3 |= ((pParticle->pSource->u32Flags & PARTICLE_LIGHTED) == PARTICLE_LIGHTED) << 2;
	flags3 |= ((pParticle->pSource->u32Flags & PARTICLE_RENDER_FOR_DISTORT) == PARTICLE_RENDER_FOR_DISTORT ) << 5;

	u32 a, b, c, d, e, f, g, h;
	
	a = ((Math::FloatAsInt(pParticle->vSize.x())) & 0xFFFFFF00) | (pParticle->Color.R);
	b = ((Math::FloatAsInt(pvPos->x()))  & 0xFFFFFF00) | (pParticle->Color.G);
	c = ((Math::FloatAsInt(pParticle->vSize.y())) & 0xFFFFFF00) | (pParticle->Color.B);
	d = ((Math::FloatAsInt(pvPos->y()))  & 0xFFFFFF00) | (pParticle->Color.A);

	e = (Math::FloatAsInt(pvPos->z())) & 0xFFFFFF00 | flags1;

	if(pParticle->u32Flags & PARTICLE_ALIGN_TO_VELOCITY)	// test ALIGN_TO_VELOCITY first, it trumps HAS_ROTATION
	{
		flags3 |= 1 << 4;
		f = (Math::FloatAsInt(pParticle->vVel.x())) & 0xFFFFFF00 | flags2;
		g = (Math::FloatAsInt(pParticle->vVel.y())) & 0xFFFFFF00 | flags3;
		h = Math::FloatAsInt(pParticle->vVel.z());
	}
	else if(pParticle->u32Flags & PARTICLE_HAS_NORMAL)
	{
		flags3 |= 1 << 3;	// mark as having a valid rotation

		/* now compute the rotation */

		float pitch = Math::ArcTan2(Math::Sqrt(pParticle->vNormal.x() * pParticle->vNormal.x() + pParticle->vNormal.z() * pParticle->vNormal.z()), pParticle->vNormal.y());
		if(pParticle->vNormal.y() >= 0.0f)
		{
			f = (Math::FloatAsInt(pitch)) & 0xFFFFFF00 | flags2;
			h = 0;
		}
		else
		{
			f = (Math::FloatAsInt(pitch - Math::Pi)) & 0xFFFFFF00 | flags2;
			h = Math::FloatAsInt(Math::Pi);
		}
		g = (Math::FloatAsInt(Math::ArcTan2(pParticle->vNormal.x(), pParticle->vNormal.z()))) & 0xFFFFFF00 | flags3;
	}
	else if(pParticle->u32Flags & PARTICLE_HAS_ROTATION)
	{
		flags3 |= 1 << 3;
		f = (Math::FloatAsInt(pParticle->vRotRadians.x())) & 0xFFFFFF00 | flags2;
		g = (Math::FloatAsInt(pParticle->vRotRadians.y())) & 0xFFFFFF00 | flags3;
		h = Math::FloatAsInt(pParticle->vRotRadians.z());
	}
	else
	{
		f = flags2;
		g = flags3;
		h = 0;
	}

	u_char* addr = (u_char*)ParticleMgr.pCurrPos;
	MasterList::Append4x4(addr, a, b, c, d);
	MasterList::Append4x4(addr, e, f, g, h);
	ParticleMgr.pCurrPos = addr;
	nParticleCount++;
	return 1;
}

void ParticleMgr__PS2SendDrawList(TextureDatabase* pDatabase, u32 nParticleCount, u16 u16Index, float fZBias)
{
	// maybe nothing to do
	if (nParticleCount == 0)
	{
		ASSERT(ParticleMgr.pCurrPos == NULL);
		return;
	}

	// send it off
	ParticleMgr__EndDrawList();

	// create dma tag that starts the vu running (it also uploads the header info to the vu [count, etc])
	MasterList::AppendDMATAG(PARTICLE_BUFFER_HEAD+1);
	MasterList::Append(createNOP());
	MasterList::Append(createUNPACK(0, V4_32, PARTICLE_BUFFER_HEAD, true));

	// these defines must be corrected if you change the code below
	// this chunk of code is really just to remind anyone who modifies below
#if PARTICLE_BUFFER_HEAD != 5
#error PARTICLE_BUFFER_HEAD must match the # of qwords put into the list by ParticleMgr__PS2SendDrawList()
#endif

	// we upload a bunch of settings every buffer here...
	u64 TEX0_1;

	Texture* tex;
	if ( u16Index == 0xfffd )
		tex = TextureMgr::GetBackBuffer();
	else if ( u16Index == 0xfffe )
		tex = TextureMgr::GetFrontBuffer();
	else if ( u16Index == 0xffff )
		tex = TextureMgr::GetWhiteTexture();
	else
		tex = pDatabase->GetTexture(u16Index);
	ASSERT_PTR(tex);

	tex->GetImage()->GetPlatformImage()->GetRegisterValues(&TEX0_1);

	u32 vWidth = tex->GetImage()->GetPlatformImage()->VirtualWidth();
	u32 vHeight = tex->GetImage()->GetPlatformImage()->VirtualHeight();
	u32 vWidHeight = (vWidth << 15) | vHeight;

	MasterList::Append(createGIFTAG(1, 5, 0xEEEE0, false));	// giftag that sends initialization registers to gs
	MasterList::Append4(nParticleCount);
	MasterList::Append4(GSState.TEST_1);	// TEST_1 only uses 32 bits, so there's room for particle count
	MasterList::Append8(TEX0_1);				// 64-bit gs register
	MasterList::Append8(GSState.ZBUF_1);	// 64-bit gs register
	MasterList::Append8(GSState.ALPHA_1);	// 64-bit gs register
	MasterList::Append(createGIFTAG(0, 3, 0x512, true));	// giftag for sending a single particle
	MasterList::AppendF((float)DisplayState.nScreenWidth);	// width of the render buffer
	MasterList::AppendF((float)DisplayState.nScreenHeight);	// height of the render buffer
	MasterList::AppendF(ParticleMgr.fZBiasZShift - fZBias);	// z-bias value to put into world-to-clip matrix (in z field)
	MasterList::Append4(vWidHeight);

	// fire off the microcode...

	// TJC - In Tak2, we'd get graphic glitches if we dont 
	// flush here..  why?  we didnt need it on Tak1... Guess 
	// I should figure it out by looking through the microcode, 
	// but Im lazy...
	// Probably a problem with a buffer overflow because
	// it appears that one drawlist is overwriting currently
	// rendering data.  Adding some slop space to the buffers
	// didnt help, nor did increasing BASE & munging with OFFSET
	// nor reducing the max number of particles sent up by 16.
	// Seems like its a bug in the math in the microcode for
	// buffer placement.  I dont really want to be messing around
	// with the microcode this late in the game, so I'll just
	// keep the flush for now...
	// TJC - I copied this change from the old engine to here.
	// in an attempt to correct a particle problem on TTL.
//	MasterList::Append4x4( createMSCNT(), createNOP(), createNOP(), createNOP() );
	MasterList::Append4x4( createMSCNT(), createNOP(), createNOP(), createFLUSH() );
}

void ParticleMgr__PS2FinishDrawList(u32 nDataIndex)
{
	// make sure we sent them all off...
	ASSERT(ParticleMgr.pCurrPos == NULL);

	// this code has problems... for now we need to flush before and after any object using dynamic textures
	// for now I'll just do it here.. :)...
	MasterList::AppendDMATAG();
	MasterList::Append(createFLUSH());
	MasterList::Append(createNOP());
}

#endif // PS2

void ParticleMgr__TransformParticle(ts_Particle* pParticle, Vector3 &vWorldCenter, Vector3 vParticleWorldVertex[4], Vector3 &vNormal, u32 u32State)
{
	Vector3 vCorner[2];
	Vector3 vTransformedCorner[2];
	Graphics4x4 mTransform;

	//yuch-- how can i optimize this? if we have a bunch of screen-oriented particles, can i just bake the
	// camera-to-world matrix into the modelview matrix? would it just become a translation matrix? for
	// ps2 rendering we want to collect particles into like groups for spliced tri-strips with adc bit--
	// don't transform until that stage? or just look for screen-orient particles with no rotation as a special
	// case-- should be the most common case, so wouldn't be any need for transform here...
	switch(pParticle->pSource->u32Flags & PARTICLE_ORIENT_MASK)
	{
	case PARTICLE_ORIENT_WALL:
		ASSERT(FALSE);	// not currently handled, so fall through...
	case PARTICLE_ORIENT_SCREEN:
		/* stand up straight, looking down world's negative z-axis */

		vCorner[0].Set(-pParticle->vSize.x(),	// upper left corner
							pParticle->vSize.y(),
							0.0f);
		vCorner[1].Set(pParticle->vSize.x(),	// upper right corner
							pParticle->vSize.y(),
							0.0f);

		/* then do camera-to-world transform so that after world-to-camera transform at render */
		/* time it ends up this way in camera space (standing up straight, looking down negative */
		/* z-axis, that is) */

		if (pParticle->u32Flags & PARTICLE_ALIGN_TO_VELOCITY)	// test ALIGN_TO_VELOCITY first, it trumps HAS_ROTATION
		{
			/* translate unit velocity vector to camera space, look at theta between */
			/* (1, 0, 0) and velocity vector in camera x/y plane to get rotation */

			float fRadians;
			Vector3 vVel;

			/* start with camera to world */

			memcpy(&mTransform, &g4CameraToWorld, sizeof(Graphics4x4));

			/* transform velocity vector into camera space */

			vVel.FastBasisTransform(pParticle->vVel, g4WorldToCamera);

			/* spin on z-axis to match rotation (tilt left/right) */

			fRadians = Math::ArcTan2(vVel.y(), vVel.x());
			if (fRadians != 0.0f)
				mTransform.ReverseCatZRotation(fRadians);

			/* and transform */

			mTransform.BasisTransformArray(vCorner, vTransformedCorner, 2);
		}
		else if (pParticle->u32Flags & PARTICLE_HAS_ROTATION)
		{
			memcpy(&mTransform, &g4CameraToWorld, sizeof(Graphics4x4));
			mTransform.ReverseCatZRotation(pParticle->vRotRadians.z());
			mTransform.BasisTransformArray(vCorner, vTransformedCorner, 2);
		}
		else
			g4CameraToWorld.BasisTransformArray(vCorner, vTransformedCorner, 2);
		break;
	case PARTICLE_ORIENT_FREE:
		/* set up lying flat in world space. if you look down so positive x-axis is right, */
		/* texture is correctly oriented */

		vCorner[0].Set(-pParticle->vSize.x(),	// upper left corner
							0.0f,
							-pParticle->vSize.y());
		vCorner[1].Set(pParticle->vSize.x(),	// upper right corner
							0.0f,
							-pParticle->vSize.y());

		/* align to velocity or rotate ? */

		if (pParticle->u32Flags & PARTICLE_ALIGN_TO_VELOCITY)	// test ALIGN_TO_VELOCITY first, it trumps HAS_ROTATION
		{
			/* any velocity? */

			if ((pParticle->vVel.x() == 0.0f) && (pParticle->vVel.y() == 0.0f) && (pParticle->vVel.z() == 0.0f))
				mTransform.Identity();
			else
			{
				/* wow, this took long enough, and involved much rotation transform flailing.
					here's what we want to do. here's the view looking down from above:

                          |
                          |
                     +---------+
                     |    |    |
                     |    |    |
               ------|->>>>>>>-|-------> +x
                     |    |    |
                     |    |    |
                     +---------+
                          |
                          |
                          v
                          +z

					the >>>> indicates the orientation of the particle texture. so first we want
					to rotate around the y-axis (spin in the ground plane) so that the texture
					orientation matches the velocity in the x-z plane (e.g. so the particle
					is pointing in the direction it is moving). note that the positive
					z-axis points down. a positive rotation angle rotates counterclockwise (we're
					used to the 2d case with the positive y-axis pointing up), so use -vel.z()
					to get the correct angle
				*/

				float fRadians = Math::ArcTan2(-pParticle->vVel.z(), pParticle->vVel.x());
				mTransform.SetYRotation(fRadians);

				/* ok, now we want to tilt the particle up to match it's trajectory. this is a
					rotation around the z-axis. looking at things from the side, we have:

                          y
                          ^
                          |
                          |
                          |
               ------+->>>>>>>-+------->x
                          |
                          |
                          |

					the particle is edge on to us. a positive rotation is still counterclockwise.
					if the particle velocity is up and to the right, the rotation angle we want
					is positive. the trajectory angle is defined by the length of the velocity
					vector in the x-z (ground) plane vs it's y-velocity
				*/

				fRadians = Math::ArcTan2(pParticle->vVel.y(), pParticle->vVel.LengthXZ());
				mTransform.ReverseCatZRotation(fRadians);

				/* ok, finally we want to spin the particle along it's center axis (the ">>>>>>"
					in the first diagram) according to it's velocity's relation to the camera. if
					the velocity is moving straight towards or away from the camera, i want the
					rotation to be 0. this leaves it in "ground orient" mode-- as if you had a ribbon
					lying flat on the floor and lifted the middle to form a trajectory arc. but if
					the velocity is perpendicular to the camera view, i want to rotate into
					"screen orient" mode so that we can see the particle as much as possible. to
					do this, rotate the original velocity into camera space and look at the x/z
					components. a 0 x-component means straight towards or away from camera,
					a 0 z-component means perpendicular to camera */

				Vector3 vVel;
				vVel.FastBasisTransform(pParticle->vVel, g4WorldToCamera);
				if (vVel.z() < 0.0f)
					vVel.z(-vVel.z());	// abs(vVel.z()) so rotation goes back to 0 (instead of going to 180) for motion away from camera
				fRadians = Math::ArcTan2(vVel.x(), vVel.z());

				/* a final refinement is to reduce the x-spin based on whether the camera is
					looking "level" or "down". for the down case, we want to always leave the
					particles in "ground orient" mode-- like you're looking down on the ribbon.
					this is pretty easy-- pass (0, 1, 0) through the camera matrix and the
					y-component will come back 1.0 for level camera and 0.0 for straight down
					(or up). passing (0, 1, 0) through the matrix just yields:

					Y = (Matrix.r01 * 0.0f) + (Matrix.r11 * 1.0f) + (Matrix.r21 * 0.0f);
					= Matrix.r11
				*/

// ned says: this is a reasonable refinement, but it isn't supported in ps2 vu code, so in the interests of having the pc
// and the ps2 look the same, don't do it
//				ASSERT((g4WorldToCamera.r11 >= 0.0f) && (g4WorldToCamera.r11 <= 1.0f));	// i think this is true (well, obviously, or i wouldn't make it an assert)
//				fRadians *= g4WorldToCamera.r11;

				/* add in the x-spin to the transform matrix and we're done */

				mTransform.ReverseCatXRotation(fRadians);
			}
		}
		else if (pParticle->u32Flags & PARTICLE_HAS_NORMAL)
		{
			/* use particle normal to build orientation matrix */

#if 1
			mTransform.BuildFromUnitVector(pParticle->vNormal, 1, 2, -1);
#else
			Vector3 rotation;
			rotation.x( Math::ArcTan2(pParticle->vNormal.LengthXZ(), pParticle->vNormal.y()));
			rotation.y( Math::ArcTan2(pParticle->vNormal.x(), pParticle->vNormal.z()));
			if(pParticle->vNormal.y() >= 0.0f)
				rotation.z(0.0f);
			else
			{
				rotation.x(rotation.x() - Math::Pi);
				rotation.z( Math::Pi);
			}
			mTransform.SetXYZRotation(rotation);
#endif

			/* if the normal is being used to orient a particle, then we can use the rotational values to perform a local rotation */

			if (pParticle->u32Flags & PARTICLE_HAS_ROTATION)
			{
				/* pre-multiply the transform with a local rotation transform */

				Graphics4x4 temp;
				temp.SetXYZRotation(pParticle->vRotRadians);
				mTransform = temp * mTransform;
			}

		}
		else if (pParticle->u32Flags & PARTICLE_HAS_ROTATION)
		{
			/* use particle rotation */

			mTransform.SetXYZRotation(pParticle->vRotRadians);
		}
		else
			mTransform.Identity();	// no rotation or align to velocity

		/* transform */

		mTransform.BasisTransformArray(vCorner, vTransformedCorner, 2);
		if (u32State & PARTICLE_LIGHTED)
			vNormal = vUp * mTransform;
		break;
	case PARTICLE_ORIENT_GROUND:
		/* set up lying flat in world space. if you look down so positive x-axis is right, */
		/* texture is correctly oriented */

		vCorner[0].Set(-pParticle->vSize.x(),	// upper left corner
							0.0f,
							-pParticle->vSize.y());
		vCorner[1].Set(pParticle->vSize.x(),	// upper right corner
							0.0f,
							-pParticle->vSize.y());
		if (pParticle->u32Flags & PARTICLE_ALIGN_TO_VELOCITY)	// test ALIGN_TO_VELOCITY first, it trumps HAS_ROTATION
		{
			/* any velocity? */

			if ((pParticle->vVel.x() == 0.0f) && (pParticle->vVel.y() == 0.0f) && (pParticle->vVel.z() == 0.0f))
				mTransform.SetYRotation(pParticle->vRotRadians.y());	// use last known good y-rotation, set below, so particle doesn't snap to identity when it stops
			else
			{
				/* this is just the first two steps of the free-orient align to velocity. spin it to match x/z velocity. */
				/* set it into particle's y-rotation as a possible last-known-good value */
			
				pParticle->vRotRadians.Y(Math::ArcTan2(-pParticle->vVel.z(), pParticle->vVel.x()));
				mTransform.SetYRotation(pParticle->vRotRadians.Y());

				/* tilt to match trajectory */

				float fRadians = Math::ArcTan2(pParticle->vVel.y(), pParticle->vVel.LengthXZ());
				mTransform.ReverseCatZRotation(fRadians);
			}
		}
		else if (pParticle->u32Flags & (PARTICLE_HAS_ROTATION | PARTICLE_WAS_ALIGN_TO_VELOCITY))
		{
			/* give it a spin. in the PARTICLE_WAS_ALIGN_TO_VELOCITY case, particle used to be align-to-velocity, but */
			/* has stopped-- just preserve last-known-good y-rotation so it doesn't snap to another angle */

			mTransform.SetYRotation(pParticle->vRotRadians.y());
		}
		else
			mTransform.Identity();	// no rotation or align to velocity
	
		/* transform */

		mTransform.BasisTransformArray(vCorner, vTransformedCorner, 2);
		if (u32State & PARTICLE_LIGHTED)
			vNormal = vUp * mTransform;
		break;
	}

	/* handle orbiting particles here. by doing this just before render we save a bit of space in ts_Particle record */

	Vector3Packed *pvPos;
	if (pParticle->fOrbitRadius == 0.0f)
		pvPos = &pParticle->vPos;
	else
		pvPos = Particle__CalculateOrbit(pParticle);

	/* calculate world coords of vertices */

	vWorldCenter = *pvPos;	// points to pParticle->vPos unless particle orbits
	vParticleWorldVertex[0] = *pvPos;
	vParticleWorldVertex[1] = *pvPos;
	vParticleWorldVertex[2] = *pvPos;
	vParticleWorldVertex[3] = *pvPos;
	vParticleWorldVertex[0].x(vParticleWorldVertex[0].x() + vTransformedCorner[0].x());	// upper left corner
	vParticleWorldVertex[0].y(vParticleWorldVertex[0].y() + vTransformedCorner[0].y());
	vParticleWorldVertex[0].z(vParticleWorldVertex[0].z() + vTransformedCorner[0].z());
	vParticleWorldVertex[1].x(vParticleWorldVertex[1].x() + vTransformedCorner[1].x());	// upper right corner
	vParticleWorldVertex[1].y(vParticleWorldVertex[1].y() + vTransformedCorner[1].y());
	vParticleWorldVertex[1].z(vParticleWorldVertex[1].z() + vTransformedCorner[1].z());
	vParticleWorldVertex[2].x(vParticleWorldVertex[2].x() - vTransformedCorner[0].x());	// lower right corner-- subtract upper left corner
	vParticleWorldVertex[2].y(vParticleWorldVertex[2].y() - vTransformedCorner[0].y());
	vParticleWorldVertex[2].z(vParticleWorldVertex[2].z() - vTransformedCorner[0].z());
	vParticleWorldVertex[3].x(vParticleWorldVertex[3].x() - vTransformedCorner[1].x());	// lower left corner-- subtract upper right corner
	vParticleWorldVertex[3].y(vParticleWorldVertex[3].y() - vTransformedCorner[1].y());
	vParticleWorldVertex[3].z(vParticleWorldVertex[3].z() - vTransformedCorner[1].z());
}

#ifdef GCN
void ParticleMgr__GCNPrepGeomLists( int &nListIndex)
{
	ParticleMgr.pVertexList = (Vector3 *)g_pFrameData->memAlloc( PARTICLE_LIST_SIZE * 4 * sizeof(Vector3));	// each particle contributes 4 vertices
	ParticleMgr.pUVList = (Vector2 *)g_pFrameData->memAlloc( PARTICLE_LIST_SIZE * 4 * sizeof(Vector2));		// each particle contributes 4 vertices
	ParticleMgr.pColorList = (ts_bRGBA *)g_pFrameData->memAlloc( PARTICLE_LIST_SIZE * 4 * sizeof(ts_bRGBA));	// each particle contributes 4 vertices

	DrawArray__VectorPointer( sizeof (Vector3) / sizeof (float), DA_FLOAT, 0, ParticleMgr.pVertexList);
	DrawArray__Enable( DA_VECTOR);
	DrawArray__TexturePointer( 2, DA_FLOAT, 0, ParticleMgr.pUVList);
	DrawArray__Enable( DA_TEXTURE);
	DrawArray__ColorPointer( 4, DA_UBYTE, 0, ParticleMgr.pColorList);
	DrawArray__Enable( DA_COLOR);

	nListIndex = 0;
}
#endif
