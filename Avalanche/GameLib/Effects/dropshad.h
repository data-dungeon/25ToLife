/////////////////////////////////////////////////////////////////////////////
// A noon drop-shadow that is very fast
// Rather than being an Actor that follows another actor, this is a manager
// that maintains all active drop shadows
/////////////////////////////////////////////////////////////////////////////

//        !!!!!!!!!!! Collision system mods !!!!!!!!!!!!!!!!!!!!
// The new collision system takes a list of entities to ignore instead of
// just one entity. Also, terrain and static objects are entities so if
// shadows are not to be drawn on them, they should be in the ignore list.
// This ignore list is built in "dynamic" memory. Typically, the list is
// built in a collision stack. Since the list here is to be persistant,
// the list should be built on the heap or on a special stack. When the
// associated shadow is removed, the list will be deleted.
// DO NOT send in a list built on the global collision stack. -bec

#ifndef DROPSHAD_H
#define DROPSHAD_H

#include "platform/BaseType.h"
#include "Math/Matrix.h"
#include "container/dllist.h"
#include "container/dynlist.h"
#include "collide/collsys.h"
#include "EngineHelper/Singleton.h"
#include "camera/camera.h"
#include "decal/staticdecal.h"

class DropShadow
{
public:
	DropShadow();
	~DropShadow();

	// Initialize the system
	bool Initialize(const char *textureDBL, int maxInstancesPerTexture = 64);

	// Shut down everything
	void Terminate();

	// Call this every frame for updates
	void Update(const Graphics4x4 &camera);

	// A shadow handle (You get this back from an add)
	typedef void *Handle;

	// Add a drop shadow that follows a caster position & 3x3 orienation (non-scaled)
	// The casterPositon and casterMatrix ARE READ FROM DIRECTLY DURING RUN-TIME,
	// remove the shadow before removing the object.
	// You must pass in the model handle returned by LoadModel, or if you pass in NULL
	// the LAST sucessful LoadModel is used.
	// Give a single radius
	// Set the disappear distance where the shadow automatically doesn't get drawn
	// The startYScanOffset is where the GetHeight call starts the scan from
	// heightTweak is the amount that is added above the collision point to the shadow pos
	// Returns a "handle" that allows you to hide/show it
	Handle Add(int textureIndex,
					float radius,
					Vector3 &casterPos, DirCos3x3 &casterMatrix,
					float startHeightAlphaFadeDistance, float endHeightAlphaFadeDistance,
					float disappearDistance = 0.0f, float startYScanOffset = 0.5f,
					float *masterAlpha = NULL,
					float heightTweak = 0.05f,
					bool shadeDynamic = false, CCollisionStackArray<CStageEntity*>* ignoreList = NULL);

	// Add a drop shadow that follows a caster position & 3x3 orienation (non-scaled)
	// The casterPositon and casterMatrix ARE READ FROM DIRECTLY DURING RUN-TIME,
	// remove the shadow before removing the object.
	// You must pass in the model handle returned by LoadModel, or if you pass in NULL
	// the LAST sucessful LoadModel is used.
	// Give an X and a Z radius, which are aligned to the XZ axes of the matrix
	// Set the disappear distance where the shadow automatically doesn't get drawn
	// The startYScanOffset is where the GetHeight call starts the scan from
	// Returns a "handle" that allows you to hide/show it
	Handle Add(int textureIndex,
					float radiusX, float radiusZ,
					Vector3 &casterPos, DirCos3x3 &casterMatrix,
					float startHeightAlphaFadeDistance, float endHeightAlphaFadeDistance,
					float disappearDistance = 0.0f, float startYScanOffset = 0.5f,
					float *masterAlpha = NULL,
					float heightTweak = 0.05f,
					bool shadeDynamic = false, CCollisionStackArray<CStageEntity*>* ignoreList = NULL);

	// Add a drop shadow that follows a caster position & 3x3 orienation (non-scaled)
	// The casterPositon and casterMatrix ARE READ FROM DIRECTLY DURING RUN-TIME,
	// remove the shadow before removing the object.
	// You must pass in the model handle returned by LoadModel, or if you pass in NULL
	// the LAST sucessful LoadModel is used.
	// Give a radius on each axis, allowing the shadow to follow the maximized projection
	// on the receiving plane.
	// Set the disappear distance where the shadow automatically doesn't get drawn
	// The startYScanOffset is where the GetHeight call starts the scan from
	// heightTweak is the amount that is added above the collision point to the shadow pos
	// heightTweak is the amount that is added above the collision point to the shadow pos
	// Returns a "handle" that allows you to hide/show it
	Handle Add(int textureIndex,
					const Vector3 &radius,
					Vector3 &casterPos, DirCos3x3 &casterMatrix,
					float startHeightAlphaFadeDistance, float endHeightAlphaFadeDistance,
					float disappearDistance = 0.0f, float startYScanOffset = 0.5f,
					float *masterAlpha = NULL,
					float heightTweak = 0.05f,
					bool shadeDynamic = false, CCollisionStackArray<CStageEntity*>* ignoreList = NULL);

	// After you've added a shadow, you can add in an offset (in model space)
	// that is added to the casterPos (in model space) every frame
	bool AddCasterOffset(Handle shadowHandle, Vector3CRef casterModelOffset);
	bool ClearCasterOffset(Handle shadowHandle);

	// After you've added a shadow, you can add who's the caster for debugging
	inline void SetCaster(Handle shadowHandle, CActor *caster);

	// Delete a shadow (handle is set to null automatically)
	bool Remove(Handle &shadow);

	// Hide/show a shadow
	bool Hide(Handle shadow);
	bool Show(Handle shadow);

	// Is the shadow hidden?
	bool Hidden(Handle shadow);

	// Change the size of the shadow, call the appropirate size call
	// based on how you created the shadow
	bool ChangeSize(Handle shadow, float radius);
	bool ChangeSize(Handle shadow, float radiusX, float radiusZ);
	bool ChangeSize(Handle shadow, Vector3CRef radius);

	// Set the base alpha for this object [0.0f 1.0f]
	bool SetBaseAlpha(Handle shadow, float baseAlpha);

	// alpha to target over time
	bool AlphaToTarget(Handle shadow, float alphaTarget, float seconds);

	// Set that this shadow is "fixed" at it's current location
	// This is a big speedup, if you set this to true...but then
	// that shadow won't move
	bool SetFixed(Handle shadow, bool fixed);

	// Setup some camera fading rules for alpha, note startFadeFactor goes
	// from [0-1], meaning is is a factor of disappearDistance
	bool EnableCameraAlphaFade(Handle shadow, float startFadeFactor);
	bool DisableCameraAlphaFade(Handle shadow);

	// Manually update the ground position
	bool CouldDraw(Handle shadow);
	bool SetGroundPosition(Handle shadow, const Vector3 &pos, const Vector3 &normal, u16 color = 0xffff);

	// Do coplanar check ( < 0.0f means never, 0.0f means always, > 0.0f check maxDraw)
	bool SetCoplanarCheck(Handle shadow, float dFactor);

private:
	// The decal support
	DecalSet										decalSet;
	DynList<SimpleStaticDecalEmitter>	emitterList;

	// We have a member function pointer for updating the data
	struct Shadow;
	typedef void (*BuildFunc)(Shadow &shadow, float cameraDSquared);

	// Are we fixed?
	typedef enum
	{
		MOVING,
		COMPUTEFIXED,
		FIXED
	} FIXEDSTATE;

	// Here is what an individual shadow looks like
	struct Shadow
	{
		// Our decal!
		SimpleStaticDecalEmitter	*emitter;
		StaticDecalHandle				handle;

		// Are we hidden
		bool					hidden;

		// How much do we tweak this bad boy up?
		float					heightTweak;

		// We store radius in a variety of ways
		Vector3				radius;

		// Pointers for the position and matrix
		Vector3 		  		*casterPos;
		DirCos3x3   		*casterO;
		bool					hasCasterOffset;
		Vector3				casterOffset;

		// Collision cache
		Vector3				cacheCollideStart;
		bool					cacheCollideHit;
		Vector3				cacheCollideIntersection;

		// Base alpha & color
		u8						shade;
		float					baseAlpha;
		float					fixedHeightAlpha;
		float					*masterAlpha;
		float					currentAlpha;

		// fade in/out support (ned added this)
		float					fadeAlpha;
		float					fadeAlphaVel;
		float					fadeAlphaTarget;

		// Our state, are we moving or fixed?
		FIXEDSTATE			fixed;

		// Uh oh, we lost the ground hide in n-frames
		Vector3				normal;
		int					hideInFrames;

		// What distance do we don't draw
		float					hideDistanceSquared;

		// stuff for alpha-fade
		float					startHeightAlphaFadeDistance;
		float					invHeightAlphaFadeRange;
		float					startCameraAlphaFadeDistance;
		float					invCameraAlphaFadeRange;

		// Where GetHeight starts from
		float					yScanOffset;

		// Cast shadows on the collision system's dynamic entities
		bool					shadeDynamic;

		// Manual update stuff
		uint					lastManualGroundUpdate;
		Vector3				manualGroundPos;
		Vector3				manualGroundNormal;
		u8						manualGroundShade;

		float					coplanarCheckDistanceSquared;
		bool					isCoplanar;

		// Collision ignore
		CCollisionStackArray<CStageEntity*>* ignoreList;

#ifdef _DEBUG
		// for debugging: the casting actor
		CActor				*caster;
#endif

		// Here is how we build the instance
		BuildFunc			buildFunc;

		// For the doubly linked list
		Shadow				*prev, *next;
	};

	// The list of current shadows
	DoublyLinkedList<Shadow>	shadowList;

	// Allocate and add a shadow to the list
	Shadow *Add(int textureIndex,
						Vector3 &casterPos, DirCos3x3 &casterMatrix,
						float startHeightAlphaFadeDistance, float endHeightAlphaFadeDistance,
						float disappearDistance, float startYScanOffset, float *masterAlpha, float heightTweak,
						bool shadeDynamic, CCollisionStackArray<CStageEntity*>* ignoreList);

	// Remove a shadow
	void Remove(Shadow *shadow);

	// Hide/show a shadow
	static void HideInstance(Shadow *shadow);
	static void ShowInstance(Shadow *shadow);

	// Function to see if a shadow is "in range of the camera"
	bool InRange(const Graphics4x4 &camera, const Shadow &shadow, float &cameraDSquared);

	// Here are the update functions, returns true if needs to be drawn
	static void BuildInstanceSimple(Shadow &shadow, float cameraDSquared = 0.0f);
	static void BuildInstanceXZ(Shadow &shadow, float cameraDSquared = 0.0f);
	static void BuildInstanceEllipsoid(Shadow &shadow, float cameraDSquared = 0.0f);

	// Collide!
	static bool FindGround(Shadow &shadow, Vector3 &intersection, bool &hasMoved);
	static bool CollideWithGround(Vector3CRef pos, float startHeight,
											Vector3 &intersection, Vector3 &normal,
											u8 &shade, bool shadeDynamic);
	static bool ComputeCoplanar(Shadow &shadow, float cameraDSquared, const Vector3 corner[4]);

	// Utilities for BuildInstance calls
	inline static void SetupWorldPosition(const Shadow &shadow, Vector3 &pos);
	inline static Vector3 ProjectOnNormalPlane(Vector3CRef normal, Vector3CRef v);
	inline static float AreaSquared(Vector3CRef a, Vector3CRef b);
	inline static void ComputeMajorMinorAxes(Vector3 projected[3], float area[3], Vector3 *&major, Vector3 *&minor);

	// Compute the alpha fade
	static inline float ComputeAlpha(Shadow &shadow, float intersectionY, float cameraDSquared);
	static inline float ComputeCameraAlpha(Shadow &shadow, float cameraDSquared);

	// Some utilities
	static inline void MakeCorners(const Vector3 &pos, const Vector3 &dir, const Vector3 &right, Vector3 corner[4]);
	static inline void MakeColors(u8 shade, float alpha, ts_bRGBA color[4]);

	static inline u8 ColorToShade(u16 color)
		{ return Math::Clamp(((((color >> 0) & 0x1f) << 3) + (((color >> 5) & 0x3f) << 2) + (((color >> 11) & 0x1f) << 3)) / 3, 0, 255); }
	static inline ts_bRGBA ShadeToColor(u8 shade, u8 alpha)
		{ ts_bRGBA c = { shade, shade, shade, alpha }; return c; }
};

// There can be only one!
DefineSingleton(DropShadow)
#define g_dropShadow	GetSingleton(DropShadow)


/////////////////////////////////////////////////////////////////////////////
// After you've added a shadow, you can add who's the caster for debugging
/////////////////////////////////////////////////////////////////////////////
inline void DropShadow::SetCaster(
Handle shadowHandle,
CActor *caster)
{
#ifdef _DEBUG
	((Shadow *)shadowHandle)->caster = caster;
#endif
}

#endif
