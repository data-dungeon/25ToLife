/////////////////////////////////////////////////////////////////////////////
// Simple footstep emitter and manager
/////////////////////////////////////////////////////////////////////////////
#ifndef FOOTSTEP_H
#define FOOTSTEP_H

#include "platform/BaseType.h"
#include "Math/Matrix.h"
#include "GameHelper/loadsom.h"
#include "container/dllist.h"
#include "container/pool.h"
#include "container/Darray.h"
#include "collide/collsys.h"
#include "EngineHelper/Singleton.h"
#include "decal/staticdecal.h"

// forward declarations
class FootstepSystem;

// The footstep emitter allows you to actually place footsteps
// on the ground
class FootstepEmitter
{
public:
	// Set the time that the footstep is at full alpha
	// and the time it takes to fade to zero
	void SetFadeEnvelopeTime(float newNoFadeTime, float newFadeTime)
		{ noFadeTime = newNoFadeTime; fadeTime = newFadeTime; }

	// Set the maximum number of footsteps that can exist in this emitter
	void SetMaxFootsteps(int newMaxFootsteps)
		{ maxFootsteps = newMaxFootsteps;}

	// Set the emitter flags
	void SetFadeSteps(bool newFadeSteps)
		{ fadeSteps = newFadeSteps; }

	// What is the base alpha of a footstep [0..1]
	void SetBaseAlpha(float newBaseAlpha)
		{ baseAlpha = newBaseAlpha; }

	// Set the current model index we are using, you are
	// allowed to set this as many times as you want over the lifetime
	// of this emitter
	void SetTextureIndex(int newTextureIndex)
		{ textureIndex = newTextureIndex; }

	// Set the scaling in xz
	void SetScaling(float newScaleX, float newScaleZ)
		{ scaleX = newScaleX; scaleZ = newScaleZ; }

	// Set the scan radius for finding terrain (leave this
	// alone if you arent having problems with the footsteps
	// mising the terrain)
	void SetScanHeight(float newScanHeight)
		{ scanHeight = newScanHeight; }

	// The the height tweak for this emitter (it's the value added above
	// the collision point)
	void SetHeightTweak(float newHeightTweak)
		{ heightTweak = newHeightTweak; }

	// Set the random amount of "jitter" you want applied to the angle
	// that the footstep is placed at...set in degrees
	void SetAngleJitter(float newAngleJitter)
		{ angleJitter = newAngleJitter; }

	// Place a footstep on the ground using collisions to determine
	// the best possible position for resting on the ground.
	// angle is specified in degrees, and 0 points the footstep along the
	// -z axis.
	// NOTE: ANGLE is given in WORLD COORDINATES, which means
	// this call isn't especially useful, try the next call.
	bool Place(Vector3CRef pos, float angle);

	// Place the footstep on the ground using a parent coordinate system
	// and an offset in model coordinates
	// Use the parent's direction vector as the direction
	bool Place(const Graphics4x4 &parent, Vector3CRef offset, bool flip = false);

	// Clear all footsteps
	void Clear();

	// Call destroy in the footstep system to delete this
	~FootstepEmitter();

private:
	// We must be created and destroyed within the footstep system
	FootstepEmitter(FootstepSystem &system);
	friend class FootstepSystem;

	// Call this every frame for updates
	void Update(const Graphics4x4 &camera);

	// Our parent
	FootstepSystem		&parent;

	// Our data members
	float 	fadeTime;
	float		noFadeTime;
	float		baseAlpha;
	int		textureIndex;
	float 	scaleX, scaleZ;
	float		scanHeight;
	float 	heightTweak;
	float		angleJitter;
	bool		fadeSteps;
	int		maxFootsteps;

	// Here is what an individual footstep looks like
	struct Footstep
	{
		// The decal!
		BornOnDecalEmitter  	*emitter;
		StaticDecalHandle	  	handle;

		// What is our alpha?
		u8							shade;
		float						alpha;

		// For the doubly linked list
		Footstep					*prev, *next;
	} ;

	// The list of footsteps associated with this emitter
	DoublyLinkedList<Footstep>	footsteps;
	Pool<Footstep>					pool;

	// Allocate a footstep
	bool AddFootstep(Vector3CRef pos, Vector3CRef right, Vector3CRef up, Vector3CRef dir, u8 shade);

	// Remove a footstep
	void RemoveFootstep(Footstep *footstep);

	// Set utilites
	static inline void MakeCorners(const Vector3 &pos, const Vector3 &dir, const Vector3 &right, Vector3 corner[4]);
	static inline void MakeColors(u8 shade, float alpha, ts_bRGBA color[4]);
	static inline void SetAlpha(float alpha, Footstep &footstep);

	// What is the born on date for this thingie?
	static inline float BornOn(Footstep &footstep)
		{ return (footstep.emitter && footstep.handle) ? footstep.emitter->BornOn(footstep.handle) : 0.0f; }

	// Return the height of the ground & normal at the specified point
	float CollideWithGround(Vector3CRef pos, Vector3 &normal, u8 &shade);
	inline u8 ColorToShade(u16 color)
		{ return Math::Clamp(((((color >> 0) & 0x1f) << 3) + (((color >> 5) & 0x3f) << 2) + (((color >> 11) & 0x1f) << 3)) / 3, 0, 255); }
	inline ts_bRGBA ShadeToColor(u8 shade, u8 alpha)
		{ ts_bRGBA c = { shade, shade, shade, alpha }; return c; }
public:
	// Ignore these, as we appear in a list
	FootstepEmitter		*prev, *next;
};

// the overall system for managing footsteps
class FootstepSystem
{
public:
	FootstepSystem();
	~FootstepSystem();

	// Initialize the system
	// The glossmap vector sets which footsteps get a glossmap
	// if glossmapvector != 0 it is assumed the last texture in the dbl is the glossmap
	bool Initialize(const char *textureDBL, int maxInstancesPerTexture = 128, int glossMapVector = 0, int bumpMapVector = 0);

	// Shut down everything
	void Terminate();

	// Call this every frame for updates
	void Update(const Graphics4x4 &camera);

	// Create an emitter, set startingModelIndex to the index returned by LoadModel
	FootstepEmitter *CreateEmitter(int startingTextureIndex = 0);

	// Destroy an emitter, also cleans up all footsteps.
	void DestroyEmitter(FootstepEmitter *emitter);

private:
	// The decal support
	DecalSet										decalSet;
	DynList<BornOnDecalEmitter>			decalEmitter;
	BornOnDecalEmitter *GetEmitter(int index)
		{ return (index < 0 || index >= decalEmitter.Count()) ? NULL : decalEmitter[index]; }

	// our list of emitters
	DoublyLinkedList<FootstepEmitter>	emitterList;

	// They are going to access the decals
	friend class FootstepEmitter;
};

// There can be only one!
DefineSingleton(FootstepSystem)
#define g_footsteps GetSingleton(FootstepSystem)

#endif
