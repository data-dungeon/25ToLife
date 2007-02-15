/************************************/
/*                                  */
/* g_ParticleDB.h                   */
/* big juju prototype  1/5/00       */
/* ned martin  avalanche software   */
/* visual c++ 6.0                   */
/* structures for pigpen export     */
/*                                  */
/************************************/

/* this file is shared by the game engine and pigpen, and controls the data format of the particle chunks--
effects and particles. this data may be variable length. the data is used to fill in the ts_Particle record,
but does not match its structure */

#ifndef __G_PARTICLEDB_H
#define __G_PARTICLEDB_H

/******************* includes *****************************/

/* system includes */

/* engine includes */

#include "Layers/LayersStruct.h"			// low-level include
#include "Layers/DLinkList.h"				// low-level include

class Texture;
class TextureDatabase;

#if defined(__cplusplus)
extern "C" {
#endif

/******************* forward declarations ****************/

/******************* defines *****************************/

/* behavior envelopes used */

enum	{	X_SIZE_ENVELOPE = 0,
			Y_SIZE_ENVELOPE,
			RED_ENVELOPE,
			GREEN_ENVELOPE,
			BLUE_ENVELOPE,
			ALPHA_ENVELOPE,

			/* must be last */

			MAX_ENVELOPES
		};

/* for supporting envelope index choice as a set of radio buttons in envelope dialog */

#define ENVELOPE_INDEX_MASK		0xff	// has to be big enough to cover all envelope types, can be too big with no damage

/* envelope types */

#define SPLINE_ENVELOPE				0x00000000
#define ADSR_ENVELOPE				0x00000001
#define STRAIGHT_ENVELOPE			0x00000002
#define FLAT_ENVELOPE				0x00000003
#define ENVELOPE_TYPE_MASK			0x00000003	// for use by envelope dialog radio button handler

/* collision type bits */

#define PARTICLE_COLLIDE_WORLD					0x01
#define PARTICLE_COLLIDE_ACTORS					0x02
#define PARTICLE_COLLIDE_WATER					0x04
#define PARTICLE_COLLIDE_CREATE_WATER_SPRITE	0x08
#define PARTICLE_FLOAT_ON_WATER					0x10

/* max particle lifetime (0 means infinite), in seconds */

#define MAX_P_LIFETIME				30.0f

/* max particle leash length (0 means no leash), in meters */

#define MAX_P_LEASH					500.0f

/* max particle size, in meters */

#define MAX_P_SIZE					100.0f

/* max radius of random position sphere */

#define MAX_P_POS_RANDOM			10.0f

/* max bounce energy for collision */

#define MAX_P_BOUNCE					4.0f

/* max size for velocity cone */

#define MAX_P_VEL_CONE				Math::HalfPi

/* max particle orbit radius, in meters, and rotation speed, in radians/second */

#define MAX_P_ORBIT_RADIUS			10.0f
#define MAX_P_ORBIT_RADIUS_VEL	5.0f
#define MAX_P_ORBIT_ROTVEL			200.0f	// radians/second
#define MAX_P_ORBIT_ROTVEL_DEG	(MAX_P_ORBIT_ROTVEL * 360.0f / 6.28f)	// degrees/second

/* max particle z-bias for rendering */

#define MAX_P_ZBIAS					5.0f

/* max emitter delay, in seconds */

#define MIN_E_DELAY					(1.0f / 60.0f)	// every 60th of a second, i.e. 60 emissions per second
#define MAX_E_DELAY					100.0f	// bigger than necessary, but good for essentially turning off emitters past lod range

/* max emitter scale factor */

#define MAX_E_SCALE					10.0f

/* max min distance for emitter-- moving emitter can ensure that emitted effects are no more than min distance apart for visual continuity */

#define MAX_E_MIN_DIST				10.0f	// keep this fairly small to provide decent accuracy in 8-bit database value

/* activation distance for emitter-- defines a bounding sphere around emitter to be tested against scene view frustum(s) */

#define MAX_E_GO_RADIUS				1000.0f

/* each emitter has 2 lod radii-- near and far. emitter rate is 100% closer than near radius, 0% farther than far radius */

#define MAX_E_LOD_RADIUS			1000.0f

/* max radius for orbiting emitter */

#define MAX_E_RADIUS					50.0f

/* max angular velocity (plus/minus) for orbiting emitter, in radians */

#define MAX_E_ANGULAR_VEL			Math::Deg2Rad(10000.0f)

/* max emitter lifetime (0 means infinite), in seconds */

#define MAX_E_LIFETIME				30.0f

/* max size of flux emitter */

#define MAX_E_FLUX_HALF_SIZE		10.0f

/* randomness is done as a percentage of full value, which is handy since it lets you adjust magnitude
of 100% value without changing randomness value, it's also handy since the percentage can always be saved
out as a byte. but there are situations (like initial velocity) where you want to do 0.0 +/ 10.0. you can't
do this as a percentage. so add a special percentage that instead of using 100% value +/- random, do
0 +/- 100% value */

#define RANDOM_CENTER_ON_ZERO		255

/* flags (for ts_ParticleSource.u32SourceFlags) */

#define PARTICLE_PERPENDICULAR_TO_VELOCITY			0x00000001	// must be a free orient particle
//available														0x00000002
#define PARTICLE_BULLET										0x00000004
#define PARTICLE_STANDARD_GRAVITY						0x00000008
#define PARTICLE_FPS_CONT_RANDOM							0x00000010
#define PARTICLE_RANDOM_SIZE_MAINTAIN_ASPECT_RATIO	0x00000020	// maintain aspect ratio through random size adjustments
#define PARTICLE_INIT_ROT_RANDOM							0x00000040
#define PARTICLE_RANDOM_START_FRAME						0x00000080
//available														0x00000100
#define PARTICLE_SOURCE_ALIGN_TO_VELOCITY				0x00000200
#define PARTICLE_ZBUFFER_WRITE							0x00000400	// write to the z-buffer (use for opaque or mostly opaque images)
#define PARTICLE_LIGHTED									0x00000800	// particle needs to be lighted
#define PARTICLE_NO_DISTORTION							0x00001000	// particle is unaffected by distortions (like wind)
#define PARTICLE_NO_VIEW_CULL								0x00002000	// particle does not die once outside of view
#define PARTICLE_FORCEFIELDS_AFFECT						0x00004000	// particle is affected by forcefield distortions
#define PARTICLE_ALPHA_REF									0x00008000	// use high alpha ref value to fix haloes
#define PARTICLE_ADDITIVE_RENDER							0x00010000	// use additive blend function when rendering
#define PARTICLE_RANDOM_POS_PLANE						0x00020000	// if random pos is non-zero, restrict to emitter space xz plane
/*** these next two will move out of here into a per-collide-surface-type flags field, see comments in Pigpen\Particle.h ***/
#define PARTICLE_COLLIDE_STOP_ROTATING					0x00040000	// particle stops rotating when it collides
#define PARTICLE_COLLIDE_STOP_ANIMATING				0x00080000	// particle stops animating when it collides
/*** end ***/
#define PARTICLE_ORBIT_INIT_ROT_RANDOM					0x00100000	// random initial value for fOrbitRotRadians
#define PARTICLE_RENDER_FOR_DISTORT						0x00200000	// causes particle to render as a circular fan with the texcoords built for screen distortions

/*** highest available bit is 0x00200000 to leave room for bitfields */

/* bitfields (for ts_ParticleSource.u32SourceFlags). these are derived from the bitfields defined in
pigpen's Particle.h */

#define PARTICLE_ORIENT_SHIFT				22	// skip over particle flags above
#define PARTICLE_ORIENT_SCREEN			(0x00000000 << PARTICLE_ORIENT_SHIFT)
#define PARTICLE_ORIENT_FREE				(0x00000001 << PARTICLE_ORIENT_SHIFT)
#define PARTICLE_ORIENT_GROUND			(0x00000002 << PARTICLE_ORIENT_SHIFT)
#define PARTICLE_ORIENT_WALL				(0x00000003 << PARTICLE_ORIENT_SHIFT)
#define PARTICLE_ORIENT_MASK				(0x00000003 << PARTICLE_ORIENT_SHIFT)

#define PARTICLE_ANIM_SHIFT				(PARTICLE_ORIENT_SHIFT + 2)	// skip over orient mask
#define PARTICLE_ANIM_LOOP					(0x00000000 << PARTICLE_ANIM_SHIFT)
#define PARTICLE_ANIM_PING_PONG			(0x00000001 << PARTICLE_ANIM_SHIFT)
#define PARTICLE_ANIM_ONE_SHOT			(0x00000002 << PARTICLE_ANIM_SHIFT)
#define PARTICLE_ANIM_MASK					(0x00000003 << PARTICLE_ANIM_SHIFT)

#define PARTICLE_COLOR_SHIFT				(PARTICLE_ANIM_SHIFT + 2)	// skip over anim mask
#define PARTICLE_COLOR_PAIR				(0x00000000 << PARTICLE_COLOR_SHIFT)
#define PARTICLE_COLOR_RGB					(0x00000001 << PARTICLE_COLOR_SHIFT)	// can be used as a bit check since the bitfield is 1 bit wide
#define PARTICLE_COLOR_MASK				(0x00000001 << PARTICLE_COLOR_SHIFT)

#define PARTICLE_COLLIDE_SHIFT			(PARTICLE_COLOR_SHIFT + 1)	// skip over color mask
#define PARTICLE_COLLIDE_DIE				(0x000000000000 << PARTICLE_COLLIDE_SHIFT)
#define PARTICLE_COLLIDE_BOUNCE			(0x000000000001 << PARTICLE_COLLIDE_SHIFT)
#define PARTICLE_COLLIDE_STICK			(0x000000000002 << PARTICLE_COLLIDE_SHIFT)
#define PARTICLE_COLLIDE_CONTINUE		(0x000000000003 << PARTICLE_COLLIDE_SHIFT)
#define PARTICLE_COLLIDE_MASK				(0x000000000003 << PARTICLE_COLLIDE_SHIFT)

#define PARTICLE_EJECT_SHIFT				(PARTICLE_COLLIDE_SHIFT + 2)	// skip over collide mask
#define PARTICLE_EJECT_PARTICLE			(0x00000000 << PARTICLE_EJECT_SHIFT)
#define PARTICLE_EJECT_DEFAULT			(0x00000001 << PARTICLE_EJECT_SHIFT)
#define PARTICLE_EJECT_MASK				(0x00000001 << PARTICLE_EJECT_SHIFT)

#define PARTICLE_ORBIT_PLANE_SHIFT		(PARTICLE_EJECT_SHIFT + 1)	// skip over eject mask
#define PARTICLE_ORBIT_SCREEN				(0x00000000 << PARTICLE_ORBIT_PLANE_SHIFT)
#define PARTICLE_ORBIT_GROUND				(0x00000001 << PARTICLE_ORBIT_PLANE_SHIFT)
#define PARTICLE_ORBIT_ALIGN_TO_VEL		(0x00000002 << PARTICLE_ORBIT_PLANE_SHIFT)
#define PARTICLE_ORBIT_PLANE_MASK		(0x00000003 << PARTICLE_ORBIT_PLANE_SHIFT)

/*** bitfields currently take 10 bits ***/

/* flag bits for ts_SpawnEffect.u8Flags */

#define SPAWN_ACTIVE			0x01

/* bitfields for high bits of ts_SpawnEffect.u8Flags */

#define SPAWN_SHIFT			6	// skip over spawn flags
#define SPAWN_DEFAULT		(0x00 << SPAWN_SHIFT)
#define SPAWN_BOUNCE			(0x01 << SPAWN_SHIFT)
#define SPAWN_STICK			(0x02 << SPAWN_SHIFT)
#define SPAWN_CONTINUE		(0x03 << SPAWN_SHIFT)
#define SPAWN_MASK			(0x03 << SPAWN_SHIFT)

/* flag bits for ts_Channel.u8Flags, ts_ChannelEffect.ulFlags (in pigpen) */

#define CHANNEL_EMIT_TIME_CONT_RANDOM	0x01
#define CHANNEL_IS_SOUND					0x02

/* for checking particle data ref counts, to makse sure no particles are being killed without decrementing
the ref count. if that happens, we'll end up with a bunch of stranded particle data records */

#define CHECK_REF_COUNTS
#ifndef _DEBUG
#undef CHECK_REF_COUNTS
#endif //_DEBUG

/* flags for ts_EmitterSource.u8Flags-- emitter source data */

#define EMITTER_SOURCE_SCALE_CONT_RANDOM	0x01	// scale is continuously random-- emitter should randomize scale each time it emits an effect
#define EMITTER_SOURCE_DELAY_CONT_RANDOM	0x02	// delay is continuously random-- keep applying +/- to delay
#define EMITTER_SOURCE_ACTIVE_MOVE_ONLY	0x04	// active only when moving

/* flags for ts_ParticleEmitter.u32Flags. the types are the allowed u32Type parameters to 
ParticleMgr__CreateEmitter */

/* note: emitter types must be first-- don't shift them up */

#define EMITTER_STANDARD								0x00000000	// can be stationary or moving
#define EMITTER_ORBITER									0x00000001	// orbit a point at some angular velocity
#define EMITTER_TIRE_BACK_SPRAY						0x00000002	// spray that comes out behind a moving tire
#define EMITTER_TIRE_SIDE_SPRAY						0x00000003	// spray that comes out fromt sides of a moving tire
#define EMITTER_TIRE_CIRCLE							0x00000004	// outer circle of tire rubber
#define EMITTER_FLUX										0x00000005	// persistant particles in a volume of space
#define EMITTER_DISTANT									0x00000006	// only active when away from camera
// if you add more emitter types, update EmitterTypeString table in Pigpen\Emitter.cpp
#define EMITTER_TYPE_MASK								0x00000007	// type mask
#define EMITTER_EJECT_VEL_CHANGES					0x00000008	// whether eject velocity changes
#define EMITTER_STATIONARY								0x00000010	// stationary emitters can avoid some advancement calculations
#define EMITTER_ACTIVE_MOVE_ONLY						0x00000020	// emitter only emits when moving
#define EMITTER_ACTIVE									0x00000040	// emitter is currently active
#define EMITTER_LOCKED									0x00000080	// emitter will ignore updates until unlocked
#define EMITTER_FIRST_ADVANCE							0x00000100	// emitter has not been advanced yet
#define EMITTER_FREE_PLANE_LIST						0x00000200	// free pPlaneList memory when emitter dies
#define EMITTER_FLUX_VOLUME_MOVED					0x00000400	// flux emitter needs to create new particles when volume moves
#define EMITTER_FLUX_FIRST_ADVANCE					0x00000800	// flux emitter has not created any particles yet
/* these correspond with particle flags of the same name (in ParticleMgr.cpp), please leave them here */
#define EMITTER_ONLY_SCENE_1							0x00001000	// emitter will only emit particles in scene 1
#define EMITTER_ONLY_SCENE_2							0x00002000	// emitter will only emit particles in scene 2
#define EMITTER_ONLY_SCENE_3							0x00004000	// emitter will only emit particles in scene 3
#define EMITTER_ONLY_SCENE_4							0x00008000	// emitter will only emit particles in scene 4
/* end of corresponding flags */
#define EMITTER_PIGGYBACKS_ORBITER					0x00010000	// emitter is piggybacking an orbiting particle, need some position calculation
#define EMITTER_ADVANCES_WHILE_MANAGER_LOCKED	0x00020000	// emitter advances even while particle manager is locked
#define EMITTER_IN_VIEW_LAST_SCENE					0x00040000	// record result of view frustum check in ParticleEmitter__Advance
#define EMITTER_ADVANCE_ONE_MORE_FRAME				0x00080000	// emitter will advance one more frame and then die
#define EMITTER_USE_OS_TIMER							0x00100000	// use os timer, ignore slowdown of game timer (but still respect pause)

/* no piggyback emitter for particle */

#define NO_PIGGYBACK_EMITTER			0xffff

/* particle source contains a start and end color for color pair envelope processing */

#define COLOR_PAIR_COUNT				2

/* flags (for ts_SwooshSource.u32Flags) */

#define SWOOSH_FPS_CONT_RANDOM		0x00000001
#define SWOOSH_RANDOM_START_FRAME	0x00000002
#define SWOOSH_ADDITIVE_RENDER		0x00000004	// use additive blend function when rendering
#define SWOOSH_RENDER_FOR_DISTORT	0x00000008	// causes swoosh to generate texcoords for screen distortions (swoosh has extra verts down middle) *Unimplemented*

/* max swoosh point lifetime, in seconds, and lod distance in meters */

#define MAX_S_LIFETIME					10.0f
#define MAX_S_LOD							50.0f

/*** highest available bit is 0x10000000 to leave room for bitfields */

/* bitfields (for ts_SwooshSource.u32Flags). these are derived from the bitfields defined in
pigpen's Particle.h */

#define SWOOSH_ANIM_SHIFT				29
#define SWOOSH_ANIM_LOOP				(0x00000000 << SWOOSH_ANIM_SHIFT)
#define SWOOSH_ANIM_PING_PONG			(0x00000001 << SWOOSH_ANIM_SHIFT)
#define SWOOSH_ANIM_ONE_SHOT			(0x00000002 << SWOOSH_ANIM_SHIFT)
#define SWOOSH_ANIM_MASK				(0x00000003 << SWOOSH_ANIM_SHIFT)

#define SWOOSH_COLOR_SHIFT				(SWOOSH_ANIM_SHIFT + 2)	// skip over anim mask
#define SWOOSH_COLOR_PAIR				(0x00000000 << SWOOSH_COLOR_SHIFT)
#define SWOOSH_COLOR_RGB				(0x00000001 << SWOOSH_COLOR_SHIFT)	// can be used as a bit check since the bitfield is 1 bit wide
#define SWOOSH_COLOR_MASK				(0x00000001 << SWOOSH_COLOR_SHIFT)

/*** bitfields currently take 3 bits ***/

/* flags for CSwooshEmitter::u32Flags. the types are the allowed u32Type parameters to ParticleMgr__CreateSwooshEmitter */

/* note: types must be first-- don't shift them up */

#define SWOOSH_EMITTER_CYLINDER						0x00000000	// can be stationary or moving
#define SWOOSH_EMITTER_PREVIEW						0x00000001	// for previewing a swoosh directly-- 3 swooshes moving in circles at right angles
#define SWOOSH_EMITTER_TYPE_MASK						0x00000007	// mask for allowed types-- leave some room for expansion
#define SWOOSH_EMITTER_ACTIVE							0x00000008
#define SWOOSH_EMITTER_ALL_SWOOSHES_INACTIVE		0x00000010

#define SWOOSH_EMITTER_TYPE_COUNT					(SWOOSH_EMITTER_TYPE_MASK + 1)	// assumes mask is 1, 3, 7, 15, 31, etc

/* flags for ts_SwooshEmitterSource.u16Flags */

#define SWOOSH_EMITTER_SOURCE_CYLINDER				0x0000	// can be stationary or moving
#define SWOOSH_EMITTER_SOURCE_PREVIEW				0x0001	// for previewing a swoosh directly-- 3 swooshes moving in circles at right angles
#define SWOOSH_EMITTER_SOURCE_TYPE_MASK			0x0007	// mask for allowed types-- leave some room for expansion

/******************* macros ******************************/

/* for converting from float to unsigned integer and back maintaining maximum precision. for converting floats to int, */
/* if float is non-zero make sure int is non-zero, even if rounding would make it 0-- 0 often has a special meaning */

#define CONVERT_FLOAT_TO_U8(fValue, fMaxValue)		(fValue == 0.0f) ? 0 : Math::Max(1, ((u8) ((fValue / fMaxValue) * 255.0f)))		// convert to 0.0 to 1.0, then expand to integer range
#define CONVERT_U8_TO_FLOAT(nValue, fMaxValue)		(((float) nValue / 255.0f) * fMaxValue)	// convert to 0.0 to 1.0, then expand to float range
#define CONVERT_FLOAT_TO_S8(fValue, fMaxValue)		(fValue == 0.0f) ? 0 : (fValue < 0.0f) ? Math::Min(-1, ((s8) ((fValue / fMaxValue) * 127.0f))) : Math::Max(1, ((s8) ((fValue / fMaxValue) * 127.0f)))		// convert to -1.0 to 1.0, then expand to integer range
#define CONVERT_S8_TO_FLOAT(nValue, fMaxValue)		(((float) nValue / 127.0f) * fMaxValue)	// convert to -1.0 to 1.0, then expand to float range
#define CONVERT_FLOAT_TO_U16(fValue, fMaxValue)		(fValue == 0.0f) ? 0 : Math::Max(1, ((u16) ((fValue / fMaxValue) * 65535.0f)))	// convert to 0.0 to 1.0, then expand to integer range
#define CONVERT_U16_TO_FLOAT(nValue, fMaxValue)		(((float) nValue / 65535.0f) * fMaxValue)	// convert to 0.0 to 1.0, then expand to float range
#define CONVERT_FLOAT_TO_S16(fValue, fMaxValue)		(fValue == 0.0f) ? 0 : (fValue < 0.0f) ? Math::Min(-1, ((s16) ((fValue / fMaxValue) * 32767.0f))) : Math::Max(1, ((s16) ((fValue / fMaxValue) * 32767.0f)))		// convert to -1.0 to 1.0, then expand to integer range
#define CONVERT_S16_TO_FLOAT(nValue, fMaxValue)		(((float) nValue / 32767.0f) * fMaxValue)	// convert to -1.0 to 1.0, then expand to float range

/******************* structures **************************/

/* envelope that controls particle's behavior (like alpha) */

struct ts_ParticleEnvelope
{
//yuch-- this needs to be massively optimized-- can't have all these floats. SplinePoint1.x, SplinePoint2.x,
// fCeilingY and fFloorY are all 0.0 to 1.0 range, can probably use a u8, others may need a s16 (current
// range -2.375 to 3.375)
	float fSplinePoint0Y;		// x == 0.0
	Vector2 SplinePoint1;
	Vector2 SplinePoint2;
	float fSplinePoint3Y;		// x == 1.0
	float fCeilingY;
	float fFloorY;
	u8 u8Type;
};

/************************** source data (from .DBL file) ********************/

/* spawn effects. particles can spawn other effects upon collision or end of life */

struct ts_SpawnEffect
{
	/* effect to spawn */

	u16 u16Effect;

	/* flags */

	u8 u8Flags;

	/* bounce energy */

	u8 u8BounceEnergy;			// convert to float using using CONVERT_U8_TO_FLOAT with MAX_P_BOUNCE
};

/* individual particle source data, used to fill in ts_Particle record in CParticleEffect__Create */

struct ts_ParticleSource
{
	/* database containing the texture for these particles */

	TextureDatabase* pTextureDatabase;

	/* animation speed */

	u8 u8FPS;											// frames per second
	u8 u8FPSRandomPct;								// plus-or-minus, in percent

	/* lifetime */

	u8 u8Lifetime;										// convert to float using using CONVERT_U8_TO_FLOAT with MAX_P_LIFETIME
	u8 u8LifetimeRandomPct;							// plus or minus, in percent

	/* initial velocty unit vector */

	s8 s8InitialVelX;									// convert to float using using CONVERT_S8_TO_FLOAT with 1,0f
	s8 s8InitialVelY;									// convert to float using using CONVERT_S8_TO_FLOAT with 1,0f
	s8 s8InitialVelZ;									// convert to float using using CONVERT_S8_TO_FLOAT with 1,0f

	/* bounce energy */

	u8 u8CollideBounceEnergy;						// convert to float using using CONVERT_U8_TO_FLOAT with MAX_P_BOUNCE
	
	/* graphic's image indices */

	union
	{
		u16 u16FirstIndex;							// value in database
	};
	union
	{
		u16 u16LastIndex;								// value in database
	};

	/* size */

	u16 u16XSize;										// convert to float using using CONVERT_U16_TO_FLOAT with MAX_P_SIZE
	u16 u16YSize;										// convert to float using using CONVERT_U16_TO_FLOAT with MAX_P_SIZE
	u8 u8XSizeRandomPct;								// plus-or-minus, in percent
	u8 u8YSizeRandomPct;								// plus-or-minus, in percent
	u8 u8YSizeEnvelope;								// if maintaining aspect ratio, use x-size envelope for both x- and y-size

	/* collision detection */

	u8 u8CollisionFlags;

	/* rendering z-bias */

	s16 s16ZBias;										// convert to float using using CONVERT_S16_TO_FLOAT with MAX_P_ZBIAS

	/* leash length */

	u16 u16Leash;										// convert to float using using CONVERT_U16_TO_FLOAT with MAX_P_LEASH
	u8 u8LeashRandomPct;								// plus or minus, in percent

	/* initial position */

	Vector3Packed vInitialPos;								// initial position (for bullets)
	u16 u16PosRandom;									// position randomness (radius of sphere), convert to float using using CONVERT_U16_TO_FLOAT

	/* initial velocity */

	float fInitialVel;								// initial velocity magnitude (combined with unit vector)
	u8 u8VelRandomPct;								// randomness of initial velocity magnitude (plus/minus, in percent)
	u8 u8VelConeStart;								// size of random velocity cone in radians (min), convert to float using using CONVERT_U8_TO_FLOAT with MAX_P_VEL_CONE
	u8 u8VelConeDelta;								// size of random velocity cone in radians (max - min, always >= 0), convert to float using using CONVERT_U8_TO_FLOAT with MAX_P_VEL_CONE

	/* rotation velocity */

	Vector3Packed vInitialRotVelRadians;				// initial rotation velocity (radians/sec)
	u8 u8RotVelRandomPct;							// randomness of initial rotation velocity (plus/minus, in percent)

	/* gravity and acceleration */
	
	float fGravity;									// gravity
	float fAccel;										// acceleration (can be negative for friction/air resistance)
	float fRotAccelRadians;							// rotation acceleration (radians/sec^2)(can be negative for friction/air resistance)

	/* flags */

	u32 u32Flags;

	/* behavior envelopes */

	ts_ParticleEnvelope Envelope[MAX_ENVELOPES];

	/* colors for color pair color control */

	ts_fRGB Color[COLOR_PAIR_COUNT];	// values are 0.0..255.0, not 0.0..1.0

	/* piggyback emitter-- can have an emitter attached to a particle to spit stuff out as particle travels */

	u16 u16PiggybackEmitterIndex;

	/* orbit */

	u8 u8OrbitRadius;									// convert to float using using CONVERT_U8_TO_FLOAT with MAX_P_ORBIT_RADIUS
	s8 s8OrbitRadiusVel;								// convert to float using using CONVERT_S8_TO_FLOAT with MAX_P_ORBIT_RADIUS_VEL
	s16 s16OrbitRotVel;								// convert to float using using CONVERT_S16_TO_FLOAT with MAX_P_ORBIT_ROTVEL
	u8 u8OrbitRadiusRandomPct;						// plus-or-minus, in percent
	u8 u8OrbitRadiusVelRandomPct;					// plus-or-minus, in percent
	u8 u8OrbitRotVelRandomPct;						// plus-or-minus, in percent

	/* spawning. this is a variable length array, potentially one entry per collision surface type if particle */
	/* is marked for collision detection. even if particle is not marked for collision, end-of-life event may */
	/* spawn a new particle */

	u16 u16SpawnEffectCount;
	u16 u16EndOfLifeSpawnIndex;					// where in array special end-of-life spawn effect start
	ts_SpawnEffect SpawnEffect[1];

	/************ SpawnEffect must be last since it is variable-length array **************/

};

/* a channel can create one or more particles, sounds, or events (but only one type per channel) */

struct ts_Channel
{
	union
	{
		u32					u32ParticleIndex;	// value in database
		ts_ParticleSource	*pParticleSource;	// pointer after patching by FixParticleEffect
		u32					u32SoundIndex;		// value in database
	};
	float			fEmitDelay;				// delay before first particle is emitted
	float			fEmitTime;				// time to emit all particles, excluding initial delay
	u8				u8EmitCount;			// number of particles to emit
	u8				u8Flags;					// flags for channel
};

/* a particle effect consists of one or more channels */

struct ts_Effect
{
	u32			u32ChannelCount;
	ts_Channel	ChannelData[1];		// may be additional channels following first one
};

/* emitter database record */

struct ts_EmitterSource
{
	/* effect to emit, and type of emitter */

	u16 u16EffectID;
	u8 u8Type;

	/* effect scaling */

	u8 u8Scale;						// default is 1.0
	u8 u8ScaleRandomPct;			// plus-or-minus, in percent

	/* flags */

	u8 u8Flags;

	/* min distance between emitted particles */

	u8 u8MinDistance;				// convert to float using using CONVERT_U8_TO_FLOAT with MAX_E_MIN_DIST

	/* radius and angular velocity for orbiter */

	u8 u8Radius;					// convert to float using using CONVERT_U8_TO_FLOAT with MAX_E_RADIUS
	s16 s16AngularVelRadians;	// convert to float using using CONVERT_S16_TO_FLOAT with MAX_E_ANGULAR_VEL

	/* activation distance-- bounding sphere around emitter is tested against scene view frustum(s) */

	u16 u16GoRadius;				// convert to float using using CONVERT_U16_TO_FLOAT with MAX_E_GO_RADIUS

	/* LOD radii-- emitter will emit at 100% of emitter rate nearer than radius 1, at 0% of emitter rate at farther than radius 2 */

	u16 u16LOD1Radius;			// convert to float using using CONVERT_U16_TO_FLOAT with MAX_E_LOD_RADIUS
	u16 u16LOD2Radius;			// convert to float using using CONVERT_U16_TO_FLOAT with MAX_E_LOD_RADIUS

	/* emitter speed */

	u16 u16Delay;					// delay between effects, in seconds
	u8 u8DelayRandomPct;			// plus-or-minus, in percent

	/* velocity and additive velocity multipliers, expressed as percentages. see comments in */
	/* engine\game\ParticleMgr.c, routine ParticleEmitter__Create for details */

	u8 u8VelPct;
	u8 u8AdditiveVelPct;

	/* lifetime */

	u8 u8Lifetime;					// convert to float using using CONVERT_U8_TO_FLOAT with MAX_E_LIFETIME
	u8 u8LifetimeRandomPct;		// plus or minus, in percent

	/* flux emitter half size */

	u8 u8FluxHalfSize;

	/* variables to control emitters that don't show up until a certain distance away, then fade in at a minimum scale */
	/* then change scale to some maximum scale at a maximum distance. this is to support joe w.'s glint effect that */
	/* intended to draw attention to far away objects of interest, but to go away once you get close enough to see */
	/* the object */

	u16 u16InvisDistance;		// convert to float using using CONVERT_U16_TO_FLOAT with MAX_E_LOD_RADIUS
	u16 u16NearDistance;			// convert to float using using CONVERT_U16_TO_FLOAT with MAX_E_LOD_RADIUS
	u16 u16FarDistance;			// convert to float using using CONVERT_U16_TO_FLOAT with MAX_E_LOD_RADIUS
	u8 u8NearScale;				// convert to float using using CONVERT_U8_TO_FLOAT with MAX_E_SCALE
	u8 u8FarScale;					// convert to float using using CONVERT_U8_TO_FLOAT with MAX_E_SCALE
};

/* swoosh source data */

struct ts_SwooshSource
{
	/* animation speed */

	u8 u8FPS;								// frames per second
	u8 u8FPSRandomPct;					// plus-or-minus, in percent

	/* lifetime */

	u8 u8PointLifetime;					// convert to float using using CONVERT_U8_TO_FLOAT with MAX_S_LIFETIME
	u8 u8LOD;								// convert to float using using CONVERT_U8_TO_FLOAT with MAX_S_LOD

	/* edge 2 multipliers */

	u8 u8Edge2ColorMult;
	u8 u8Edge2AlphaMult;

	/* graphic's image indices */

	union {
		struct {
			u16 u16FirstIndex;
			u16 u16LastIndex;					// value in database
		};
		Texture* pTexture;
	};

	/* flags */

	u32 u32Flags;

	/* behavior envelopes */

	ts_ParticleEnvelope Envelope[MAX_ENVELOPES];

	/* colors for color pair color control */

	ts_bRGBA Color[COLOR_PAIR_COUNT];
};

/* swoosh emitter database record */

struct ts_SwooshEmitterSource
{
	/* swoosh to emit, and type of emitter */

	u16 u16SwooshID;
	u16 u16Flags;				// type, plus additional flags

	/* lifetime */

	float fLifetime;
	u8 u8LifetimeRandomPct;	// plus or minus, in percent

	/* lod distance-- for a swoosh emitter, this is when to deactivate all it's swooshes-- the swooshes */
	/* themselves will fade out over time based on their own lod value */

	float fLOD;

	/* cylinder-type specific */

	float	fCylinderRadius;
	float	fCylinderHeight;
	float	fCylinderSwooshYVel;
	float	fCylinderSwooshRotVel;
	float fCylinderDeltaTime;
	float fCylinderDeltaAngle;
	float fCylinderSwooshWidth;
	float fCylinderSwooshLife;
	u32 u32CylinderPointsOnCircle;
	u8 u8CylinderRadiusRandomPct;			// plus or minus, in percent
	u8 u8CylinderSwooshYVelRandomPct;	// plus or minus, in percent
	u8 u8CylinderSwooshWidthRandomPct;	// plus or minus, in percent
	u8 u8CylinderSwooshLifeRandomPct;	// plus or minus, in percent
};

/******************* global variables ********************/

/******************* global prototypes *******************/

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // __G_PARTICLEDB_H

					  
