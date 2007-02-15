/////////////////////////////////////////////////////////////////////////////
// A noon drop-shadow that is very fast
// The shadow manager handles all the textures and the creation of shadows
/////////////////////////////////////////////////////////////////////////////
#include "Effects/EffectsPCH.h"

#include "EngineHelper/timer.h"
#include "camera/director.h"
#include "collideHelper/ccoplanar.h"

//#define DEBUG_COMPLEXSHADOW

// There can be only one!
DeclareSingleton(DropShadow);

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
DropShadow::DropShadow() :
emitterList(4, 4, true),
shadowList(false)
{
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
DropShadow::~DropShadow()
{
	// Make sure we call this
	Terminate();
}

/////////////////////////////////////////////////////////////////////////////
// Initialize the system
/////////////////////////////////////////////////////////////////////////////
bool DropShadow::Initialize(
const char *textureDBL,
int maxInstancesPerTexture)
{
	// Load it!
	if (!decalSet.Load(textureDBL))
		return false;

	// Create the emitters
	for (int i = 0; i < decalSet.Textures(); i++)
	{
		DecalTextureLayer textureLayer(i);
#ifndef PS2
		textureLayer.m_blend = RS_BLENDFUNC_SUBTRACTIVE;
#endif
		SimpleStaticDecalEmitter *emitter = new SimpleStaticDecalEmitter(decalSet, &textureLayer, 1);
		ASSERT(emitter);
		if (!emitter)
			continue;

		emitter->Setup(maxInstancesPerTexture, -6);
		emitterList.Add(emitter);
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Shut down everything
/////////////////////////////////////////////////////////////////////////////
void DropShadow::Terminate()
{
	// delete all the instances
	Shadow *shadow = shadowList.Head();
	while (shadow)
	{
		Remove(shadow);
		shadow = shadowList.Head();
	}
}

/////////////////////////////////////////////////////////////////////////////
// Call this every frame for updates
/////////////////////////////////////////////////////////////////////////////
void DropShadow::Update(
const Graphics4x4 &camera)
{
	// Traverse the list of shadows
	Shadow *s = shadowList.Head();
	while (s)
	{
		// Is this shadow hidden and in range?
		float cameraDSquared;
		bool hide = Hidden(s) || !InRange(camera, *s, cameraDSquared);

		// Hide it if hidden or out of range
		if (hide)
			HideInstance(s);
		else
		{
			// Update the matrix and show
			ShowInstance(s);
			s->buildFunc(*s, cameraDSquared);
		}

		// next one
		s = s->next;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Add a drop shadow that follows a position & 3x3 orienation (non-scaled)
/////////////////////////////////////////////////////////////////////////////
DropShadow::Handle DropShadow::Add(
int textureIndex,
float radius,
Vector3 &casterPos,
DirCos3x3 &casterMatrix,
float startHeightAlphaFadeDistance,
float endHeightAlphaFadeDistance,
float disappearDistance,
float startYScanOffset,
float *masterAlpha,
float heightTweak,
bool shadeDynamic,
CCollisionStackArray<CStageEntity*>* ignoreList)
{
	// Allocate a shadow
	Shadow *shadow = Add(textureIndex,
								casterPos, casterMatrix,
								startHeightAlphaFadeDistance, endHeightAlphaFadeDistance,
								disappearDistance, startYScanOffset, masterAlpha, heightTweak,
								shadeDynamic, ignoreList);
	if (!shadow)
		return NULL;

	// Fill out our specifics
	shadow->radius.Set(radius, radius, radius);
	shadow->buildFunc = BuildInstanceSimple;

	// Setup the initial frame
	BuildInstanceSimple(*shadow);

	// Return them an opaque handle
	return((Handle)shadow);
}

/////////////////////////////////////////////////////////////////////////////
// Add a drop shadow that follows a position & 3x3 orienation (non-scaled)
/////////////////////////////////////////////////////////////////////////////
DropShadow::Handle DropShadow::Add(
int textureIndex,
float radiusX,
float radiusZ,
Vector3 &casterPos,
DirCos3x3 &casterMatrix,
float startHeightAlphaFadeDistance,
float endHeightAlphaFadeDistance,
float disappearDistance,
float startYScanOffset,
float *masterAlpha,
float heightTweak,
bool shadeDynamic,
CCollisionStackArray<CStageEntity*>* ignoreList)
{
	// Allocate a shadow
	Shadow *shadow = Add(textureIndex,
								casterPos, casterMatrix,
								startHeightAlphaFadeDistance, endHeightAlphaFadeDistance,
								disappearDistance, startYScanOffset, masterAlpha, heightTweak,
								shadeDynamic, ignoreList);
	if (!shadow)
		return NULL;

	// Fill out our specifics
	shadow->radius.Set(radiusX, 0.0f, radiusZ);
	shadow->buildFunc = BuildInstanceXZ;

	// Setup the initial frame
	BuildInstanceXZ(*shadow);

	// Return them an opaque handle
	return((Handle)shadow);
}

/////////////////////////////////////////////////////////////////////////////
// Add a drop shadow that follows a position & 3x3 orienation (non-scaled)
/////////////////////////////////////////////////////////////////////////////
DropShadow::Handle DropShadow::Add(
int textureIndex,
const Vector3 &radius,
Vector3 &casterPos,
DirCos3x3 &casterMatrix,
float startHeightAlphaFadeDistance,
float endHeightAlphaFadeDistance,
float disappearDistance,
float startYScanOffset,
float *masterAlpha,
float heightTweak,
bool shadeDynamic,
CCollisionStackArray<CStageEntity*>* ignoreList)
{
	// Allocate a shadow
	Shadow *shadow = Add(textureIndex,
								casterPos, casterMatrix,
								startHeightAlphaFadeDistance, endHeightAlphaFadeDistance,
								disappearDistance, startYScanOffset, masterAlpha, heightTweak,
								shadeDynamic, ignoreList);
	if (!shadow)
		return NULL;

	// Fill out our specifics
	shadow->radius = radius;
	shadow->buildFunc = BuildInstanceEllipsoid;

	// Setup the initial frame
	BuildInstanceEllipsoid(*shadow);

	// Return them an opaque handle
	return((Handle)shadow);
}

/////////////////////////////////////////////////////////////////////////////
// After you've added a shadow, you can add in an offset (in model space)
// that is added to the casterPos (in model space) every frame
/////////////////////////////////////////////////////////////////////////////
bool DropShadow::AddCasterOffset(
Handle shadowHandle,
Vector3CRef casterModelOffset)
{
	// Valid shadow?
	if (!shadowHandle)
		return false;

	// Put in the caster offset
	Shadow *shadow = (Shadow *)shadowHandle;
	shadow->hasCasterOffset = true;
	shadow->casterOffset = casterModelOffset;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool DropShadow::ClearCasterOffset(
Handle shadowHandle)
{
	// Valid shadow?
	if (!shadowHandle)
		return false;

	// Put in the caster offset
	Shadow *shadow = (Shadow *)shadowHandle;
	shadow->hasCasterOffset = false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Delete a shadow (handle is set to null automatically)
/////////////////////////////////////////////////////////////////////////////
bool DropShadow::Remove(
Handle &shadow)
{
	// Valid shadow?
	if (shadow)
	{
		// Call the real remove
		Remove((Shadow *)shadow);
		shadow = NULL;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Hide/show a shadow
/////////////////////////////////////////////////////////////////////////////
bool DropShadow::Hide(
Handle shadow)
{
	// Valid shadow?
	if (!shadow)
		return false;

	// Hide it
	Shadow *s = (Shadow *)shadow;
	s->hidden = true;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Hide/show a shadow
/////////////////////////////////////////////////////////////////////////////
bool DropShadow::Show(
Handle shadow)
{
	// Valid shadow?
	if (!shadow)
		return false;

	// Show it
	Shadow *s = (Shadow *)shadow;
	s->hidden = false;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Is the shadow hidden?
/////////////////////////////////////////////////////////////////////////////
bool DropShadow::Hidden(
Handle shadow)
{
	// Valid shadow?
	if (!shadow)
		return true;
	else
	{
		Shadow *s = (Shadow *)shadow;
		return(s->hidden);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Change the size of the shadow, call the appropirate size call
// based on how you created the shadow
/////////////////////////////////////////////////////////////////////////////
bool DropShadow::ChangeSize(
Handle shadow,
float radius)
{
	if (!shadow)
		return false;
	Shadow *s = (Shadow *)shadow;
	s->radius.Set(radius, radius, radius);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool DropShadow::ChangeSize(
Handle shadow,
float radiusX,
float radiusZ)
{
	if (!shadow)
		return false;
	Shadow *s = (Shadow *)shadow;
	s->radius.Set(radiusX, 0.0f, radiusZ);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool DropShadow::ChangeSize(
Handle shadow,
Vector3CRef radius)
{
	if (!shadow)
		return false;
	Shadow *s = (Shadow *)shadow;
	s->radius = radius;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Set the base alpha for this object [0.0f 1.0f]
/////////////////////////////////////////////////////////////////////////////
bool DropShadow::SetBaseAlpha(
Handle shadow,
float baseAlpha)
{
	ASSERT(baseAlpha >= 0.0f && baseAlpha <= 1.0f);

	// duh
	if (!shadow)
		return false;

	// Get the shadow
	Shadow *s = (Shadow *)shadow;
	s->baseAlpha = baseAlpha;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// alpha to target over time
/////////////////////////////////////////////////////////////////////////////
bool DropShadow::AlphaToTarget(Handle shadow, float alphaTarget, float seconds)
{
	if (!shadow)
		return false;

	// Get the shadow
	Shadow *s = (Shadow *)shadow;

	// already there?
	if (s->fadeAlpha == alphaTarget)
		return true;

	// fade over time
	s->fadeAlphaTarget = alphaTarget;
	s->fadeAlphaVel = (s->fadeAlphaTarget - s->fadeAlpha) / seconds;

	// success
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Set that this shadow is "fixed" at it's current location
// This is a big speedup, if you set this to true...but then
// that shadow won't move
/////////////////////////////////////////////////////////////////////////////
bool DropShadow::SetFixed(
Handle shadow,
bool fixed)
{
	// duh
	if (!shadow)
		return false;

	// Get the shadow
	Shadow *s = (Shadow *)shadow;

	// If they "fixed" us, update the stuff one last time
	if (fixed)
		s->fixed = COMPUTEFIXED;
	else
		s->fixed = MOVING;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Setup some camera fading rules for alpha, note startFadeFactor goes
// from [0-1], meaning is is a factor of disappearDistance
/////////////////////////////////////////////////////////////////////////////
bool DropShadow::EnableCameraAlphaFade(
Handle shadow,
float startFadeFactor)
{
	ASSERT(startFadeFactor >= 0.0f && startFadeFactor <= 1.0f);

	// duh
	if (!shadow)
		return false;

	// Get the shadow
	Shadow *s = (Shadow *)shadow;

	// Can't fade if we don't have a final camera distance
	if (Math::Zero(s->hideDistanceSquared))
		return false;

	// Figure out the fade start
	float fadeEnd = Math::Sqrt(s->hideDistanceSquared);
	float fadeStart = startFadeFactor * fadeEnd;
	s->startCameraAlphaFadeDistance = fadeStart;
	s->invCameraAlphaFadeRange = 1.0f / (fadeEnd - fadeStart);;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool DropShadow::DisableCameraAlphaFade(
Handle shadow)
{
	// duh
	if (!shadow)
		return false;

	// Get the shadow
	Shadow *s = (Shadow *)shadow;

	// Set up the parameters
	s->startCameraAlphaFadeDistance = 0.0f;
	s->invCameraAlphaFadeRange = 0.0f;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Manually ipdate the ground position
/////////////////////////////////////////////////////////////////////////////
bool DropShadow::CouldDraw(Handle shadow)
{
	// duh
	if (!shadow)
		return false;

	// Get the shadow
	Shadow *s = (Shadow *)shadow;
	if (s->hidden)
		return false;

	const Graphics4x4 &camera = g_cameraDirector.GetCameraMatrix();
	float cameraDSquared;
	return InRange(camera, *s, cameraDSquared);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool DropShadow::SetGroundPosition(
Handle shadow,
const Vector3 &pos,
const Vector3 &normal,
u16 color)
{
	// duh
	if (!shadow)
		return false;

	// Get the shadow
	Shadow *s = (Shadow *)shadow;

	s->lastManualGroundUpdate = g_timer.GetFrameCount();
	s->manualGroundPos = pos;
	s->manualGroundNormal = normal;
	s->manualGroundShade = ColorToShade(color);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Do coplanar check ( < 0.0f means never, 0.0f means always, > 0.0f check maxDraw)
/////////////////////////////////////////////////////////////////////////////
bool DropShadow::SetCoplanarCheck(
Handle shadow,
float dFactor)
{
	// duh
	if (!shadow)
		return false;

	// Get the shadow
	Shadow *s = (Shadow *)shadow;

	if (Math::Zero(s->hideDistanceSquared) || Math::Zero(dFactor))
	{
		s->coplanarCheckDistanceSquared = 0.0f;
		s->isCoplanar = false;
	}
	else if (dFactor < 0.0f)
	{
		s->coplanarCheckDistanceSquared = -1.0f;
		s->isCoplanar = true;
	}
	else
	{
		s->coplanarCheckDistanceSquared = Math::Square(Math::Sqrt(s->hideDistanceSquared) * dFactor);
		s->isCoplanar = false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Allocate and add a shadow to the list
/////////////////////////////////////////////////////////////////////////////
DropShadow::Shadow *DropShadow::Add(
int textureIndex,
Vector3 &casterPos,
DirCos3x3 &casterMatrix,
float startHeightAlphaFadeDistance,
float endHeightAlphaFadeDistance,
float disappearDistance,
float startYScanOffset,
float *masterAlpha,
float heightTweak,
bool shadeDynamic,
CCollisionStackArray<CStageEntity*>* ignoreList)
{
	// What texture are they requesting?
	if (textureIndex < 0 || decalSet.Textures() == 0)
		return NULL;

	// Get a valid emitter
	int useTexture = textureIndex % decalSet.Textures();
	if (!emitterList[useTexture])
		return NULL;

	// Allocate a shadow
	MEM_SET_ALLOC_NAME("DropShadow shadow");
	Shadow *shadow = new Shadow;
	ASSERT(shadow);
	if (!shadow)
		return NULL;

	// Fill it out
	shadow->emitter = emitterList[useTexture];
	shadow->hidden = false;
	shadow->heightTweak = heightTweak;
	shadow->casterPos = &casterPos;
	shadow->casterO = &casterMatrix;
	shadow->fixed = MOVING;
	shadow->hideInFrames = 0;
	shadow->normal.Set(0.0f, 1.0f, 0.0f);
	shadow->baseAlpha = 1.0f;
	shadow->fadeAlpha = 1.0f;
	shadow->fadeAlphaVel = 0.0f;
	shadow->fadeAlphaTarget = 1.0f;
	shadow->fixedHeightAlpha = 1.0f;
	shadow->masterAlpha = masterAlpha;
	shadow->hasCasterOffset = false;
	shadow->hideDistanceSquared = disappearDistance * disappearDistance;
	if (endHeightAlphaFadeDistance > startHeightAlphaFadeDistance)
	{
		shadow->startHeightAlphaFadeDistance = startHeightAlphaFadeDistance;
		shadow->invHeightAlphaFadeRange = 1.0f / (endHeightAlphaFadeDistance - startHeightAlphaFadeDistance);
	}
	else
	{
		shadow->startHeightAlphaFadeDistance = 0.0f;
		shadow->invHeightAlphaFadeRange = 0.0f;
	}
	shadow->startCameraAlphaFadeDistance = 0.0f;
	shadow->invCameraAlphaFadeRange = 0.0f;
	shadow->yScanOffset = startYScanOffset;
	shadow->shadeDynamic = shadeDynamic;
	shadow->ignoreList = ignoreList;

	shadow->cacheCollideStart.Set(10000.0f, -100000.0f, 10000.0f);
	shadow->cacheCollideHit = false;
	shadow->cacheCollideIntersection = shadow->cacheCollideStart;

	shadow->lastManualGroundUpdate = 0;
	shadow->manualGroundPos.Clear();
	shadow->manualGroundNormal.Set(0.0f, 1.0f, 0.0f);
	shadow->manualGroundShade = 0;

	shadow->coplanarCheckDistanceSquared = -1.0f;
	shadow->isCoplanar = true;

	// Create the decal
	Vector3 corner[4];
	corner[0] = corner[1] = corner[2] = corner[3] = Vector3(0.0f, 0.0f, 0.0f);
	shadow->handle = shadow->emitter->Create(corner);
	ASSERT(shadow->handle);

	// Add it into the dyn list
	shadowList.AddTail(shadow);

	// return it so they can fill the rest out
	return shadow;
}

/////////////////////////////////////////////////////////////////////////////
// Delete a shadow (handle is set to null automatically)
/////////////////////////////////////////////////////////////////////////////
void DropShadow::Remove(
Shadow *shadow)
{
	// Remove it from the list
	shadowList.Unlink(shadow);

	// Tell the emitter
	if (shadow->emitter && shadow->handle)
		shadow->emitter->Destroy(shadow->handle);

	// Delete the ignore list, if any
	CCollisionStackItem::Delete(shadow->ignoreList);

	// Free it
	delete shadow;
}

/////////////////////////////////////////////////////////////////////////////
// Hide a shadow
/////////////////////////////////////////////////////////////////////////////
void DropShadow::HideInstance(
Shadow *shadow)
{
	if (shadow->emitter && shadow->handle)
		shadow->emitter->Hide(shadow->handle);
}

/////////////////////////////////////////////////////////////////////////////
// Show the shadow
/////////////////////////////////////////////////////////////////////////////
void DropShadow::ShowInstance(
Shadow *shadow)
{
	if (shadow->emitter && shadow->handle)
		shadow->emitter->Show(shadow->handle);
}

/////////////////////////////////////////////////////////////////////////////
// Function to see if a shadow is "in range of the camera"
/////////////////////////////////////////////////////////////////////////////
bool DropShadow::InRange(
const Graphics4x4 &camera,
const Shadow &shadow,
float &cameraDSquared)
{
	// Transform
	Vector3 inCamera = *shadow.casterPos * camera;
	cameraDSquared = Vector3::Dot(inCamera, inCamera);
	if (shadow.fixed == COMPUTEFIXED)
		return true;

	// are we in range?
	const bool inRange = Math::Zero(shadow.hideDistanceSquared) || cameraDSquared < shadow.hideDistanceSquared;
	if (!inRange)
		return false;

	const bool onScreen = (inCamera.Z() < 0.0f) && (Math::Abs(inCamera.Z()) > Math::Abs(inCamera.X())) && (Math::Abs(inCamera.Z()) > Math::Abs(inCamera.Y()));
	return onScreen;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void DropShadow::BuildInstanceSimple(
Shadow &shadow,
float cameraDSquared)
{
	// Anything to do?
	if (shadow.fixed == FIXED)
	{
		// Setup the alpha
		ts_bRGBA color[4];
		MakeColors(shadow.shade, ComputeCameraAlpha(shadow, cameraDSquared), color);
		shadow.emitter->Update(shadow.handle, NULL, NULL, color);
		return;
	}

	// If we didn't get a hit, just get out (it will follow at last m.ty)
	Vector3 intersection;
	bool hasMoved;
	if (FindGround(shadow, intersection, hasMoved))
	{
		// Set the position of the shadow and reset the hide filter
		intersection.Y(intersection.Y() + shadow.heightTweak);
		shadow.hideInFrames = 2;
	}
	else
	{
		// Decrement the hide count (using old m.ty)
		if (shadow.hideInFrames > 0)
			--shadow.hideInFrames;
		else
		{
			HideInstance(&shadow);
			return;
		}
	}

	// Build the matrix around the intersection
	const Vector3 &up = shadow.normal;

	// Use a reference dir
	Vector3 dir(0.0f, 0.0f, 1.0f);

	// compute the right
	Vector3 right = Vector3::Cross(dir, up);
	right.Normalize();

	// Re-fix the dir
	dir = Vector3::Cross(up, right);

	// Apply the scaling!!
	dir *= shadow.radius.z();
	right *= shadow.radius.x();

	// Yeah!
	Vector3 corner[4];
	MakeCorners(intersection, dir, right, corner);

	// Setup the alpha
	ts_bRGBA color[4];
	MakeColors(shadow.shade, ComputeAlpha(shadow, intersection.Y(), cameraDSquared), color);

	// Update it
	shadow.emitter->Update(shadow.handle, corner, NULL, color);

	// Should we hide?
	if (hasMoved)
		shadow.isCoplanar = ComputeCoplanar(shadow, cameraDSquared, corner);
	if (!shadow.isCoplanar)
		HideInstance(&shadow);

	// Fixed
	if (shadow.fixed == COMPUTEFIXED)
		shadow.fixed = FIXED;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void DropShadow::BuildInstanceXZ(
Shadow &shadow,
float cameraDSquared)
{
	// Anything to do?
	if (shadow.fixed == FIXED)
	{
		// Setup the alpha
		ts_bRGBA color[4];
		MakeColors(shadow.shade, ComputeCameraAlpha(shadow, cameraDSquared), color);
		shadow.emitter->Update(shadow.handle, NULL, NULL, color);
		return;
	}

	// If we didn't get a hit, just get out (it will follow at last m.ty)
	Vector3 intersection;
	bool hasMoved;
	if (FindGround(shadow, intersection, hasMoved))
	{
		// Set the position of the shadow and reset the hide filter
		intersection.Y(intersection.Y() + shadow.heightTweak);
		shadow.hideInFrames = 2;
	}
	else
	{
		// Decrement the hide count (using old m.ty)
		if (shadow.hideInFrames > 0)
			--shadow.hideInFrames;
		else
		{
			HideInstance(&shadow);
			return;
		}
	}

	// Build the matrix around the intersection
	Vector3 &up = shadow.normal;

	// Use the dir from the caster
	Vector3 dir;
	dir = shadow.casterO->GetRow2();

	// compute the right
	Vector3 right = Vector3::Cross(dir, up);
	right.SafeNormalize();

	// Re-fix the dir
	dir = Vector3::Cross(up, right);

	// Apply the scaling!!
	dir *= shadow.radius.z();
	right *= shadow.radius.x();

	// Yeah!
	Vector3 corner[4];
	MakeCorners(intersection, dir, right, corner);

	// Setup the alpha
	ts_bRGBA color[4];
	MakeColors(shadow.shade, ComputeAlpha(shadow, intersection.Y(), cameraDSquared), color);

	// Update it
	shadow.emitter->Update(shadow.handle, corner, NULL, color);

	// Should we hide?
	if (hasMoved)
		shadow.isCoplanar = ComputeCoplanar(shadow, cameraDSquared, corner);
	if (!shadow.isCoplanar)
		HideInstance(&shadow);

	// Fixed
	if (shadow.fixed == COMPUTEFIXED)
		shadow.fixed = FIXED;
}

/////////////////////////////////////////////////////////////////////////////
// Here are the update functions, returns true if needs to be drawn
/////////////////////////////////////////////////////////////////////////////
void DropShadow::BuildInstanceEllipsoid(
Shadow &shadow,
float cameraDSquared)
{
	// Anything to do?
	if (shadow.fixed == FIXED)
	{
		// Setup the alpha
		ts_bRGBA color[4];
		MakeColors(shadow.shade, ComputeCameraAlpha(shadow, cameraDSquared), color);
		shadow.emitter->Update(shadow.handle, NULL, NULL, color);
		return;
	}

	// If we didn't get a hit, just get out (it will follow at last m.ty)
	Vector3 intersection;
	bool hasMoved;
	if (FindGround(shadow, intersection, hasMoved))
	{
		// Set the position of the shadow and reset the hide filter
		intersection.Y(intersection.Y() + shadow.heightTweak);
		shadow.hideInFrames = 2;
	}
	else
	{
		// Decrement the hide count (using old m.ty)
		if (shadow.hideInFrames > 0)
			--shadow.hideInFrames;
		else
		{
			HideInstance(&shadow);
			return;
		}
	}

	// Get the three consitutient vectors
	Vector3 casterRight, casterUp, casterDir;
	casterRight = shadow.casterO->GetRow0();
	casterUp = shadow.casterO->GetRow1();
	casterDir = shadow.casterO->GetRow2();

	// Project each of the vectors onto the plane
	Vector3 projected[3];
	projected[0] = ProjectOnNormalPlane(shadow.normal, casterRight * shadow.radius.x());
	projected[1] = ProjectOnNormalPlane(shadow.normal, casterUp * shadow.radius.y());
	projected[2] = ProjectOnNormalPlane(shadow.normal, casterDir * shadow.radius.z());

#ifdef DEBUG_COMPLEXSHADOW
	DrawUtility::Arrow(pos, projected[0], DrawUtility::RED);
	DrawUtility::Arrow(pos, projected[1], DrawUtility::GREEN);
	DrawUtility::Arrow(pos, projected[2], DrawUtility::BLUE);
#endif

	// Which pair gives the shadow the most "area" on the plane
	float area[3];
	area[0] = AreaSquared(projected[0], projected[1]);	// p0 x p1
	area[1] = AreaSquared(projected[0], projected[2]); // p0 x p2
	area[2] = AreaSquared(projected[1], projected[2]); // p1 x p2

	// What are major and minor axes of the projection?
	Vector3 *major, *minor;
	ComputeMajorMinorAxes(projected, area, major, minor);

	if (Math::Zero((*major).LengthSquared() || (*minor).LengthSquared()))
	{
		HideInstance(&shadow);
		return;
	}

	// Build the matrix around the intersection
	Vector3 &up = shadow.normal;

	// major & minor are both in the plane defined by up,
	// so leave major alone, and figure out the the new right,
	// adjusting it out of minor
	Vector3 &dir = *major;
	Vector3 right = Vector3::Cross(dir, up);
	right.Normalize();
	right *= Math::Abs(Vector3::Dot(*minor, right));

#ifdef DEBUG_COMPLEXSHADOW
	Vector3 origin = pos + Vector3(0.0f, 0.5f, 0.0f);
	DrawUtility::Arrow(origin, right, DrawUtility::CYAN);
	DrawUtility::Arrow(origin, up, DrawUtility::MAGENTA);
	DrawUtility::Arrow(origin, dir, DrawUtility::YELLOW);
#endif

	// Yeah!
	Vector3 corner[4];
	MakeCorners(intersection, dir, right, corner);

	// Setup the alpha
	ts_bRGBA color[4];
	MakeColors(shadow.shade, ComputeAlpha(shadow, intersection.Y(), cameraDSquared), color);

	// Update it
	shadow.emitter->Update(shadow.handle, corner, NULL, color);

	// Should we hide?
	if (hasMoved)
		shadow.isCoplanar = ComputeCoplanar(shadow, cameraDSquared, corner);
	if (!shadow.isCoplanar)
		HideInstance(&shadow);

	// Fixed
	if (shadow.fixed == COMPUTEFIXED)
		shadow.fixed = FIXED;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool DropShadow::FindGround(
Shadow &shadow,
Vector3 &intersection,
bool &hasMoved)
{
	const float moveEpsilon = 0.00005f;

	// Do we need to collide?
	bool hit = false;
	if (shadow.lastManualGroundUpdate == g_timer.GetFrameCount())
	{
		hit = true;
		intersection = shadow.manualGroundPos;
		shadow.normal = shadow.manualGroundNormal;
		shadow.shade = shadow.manualGroundShade;

		hasMoved = ((shadow.cacheCollideIntersection - intersection).LengthSquared() > moveEpsilon);
		shadow.cacheCollideIntersection = intersection;
	}
	else
	{
		// Setup the position vector
		Vector3 pos;
		SetupWorldPosition(shadow, pos);
		if ((pos - shadow.cacheCollideStart).LengthSquared() < moveEpsilon)
		{
			hit = shadow.cacheCollideHit;
			intersection = shadow.cacheCollideIntersection;
			hasMoved = false;
		}
		else
		{
			hit = CollideWithGround(pos, shadow.yScanOffset, intersection, shadow.normal, shadow.shade, shadow.shadeDynamic);
			shadow.cacheCollideStart = pos;
			shadow.cacheCollideHit = hit;

			// Save it
			hasMoved = Math::Zero((shadow.cacheCollideIntersection - intersection).LengthSquared());
			shadow.cacheCollideIntersection = intersection;
		}
	}

	return hit;
}

/////////////////////////////////////////////////////////////////////////////
// Return the height of the ground & normal at the specified point
/////////////////////////////////////////////////////////////////////////////
bool DropShadow::CollideWithGround(
Vector3CRef pos,
float startHeight,
Vector3 &intersection,
Vector3 &normal,
u8 &shade,
bool shadeDynamic)
{
	// Where should it go?
	CLineEntity test;
	Vector3 start = pos;
	start.Y(start.Y() + startHeight);
	const Vector3 down(0.0f, -1.0f, 0.0f);
	test.Line().SetOriginBasis(start, down);
	test.Line().SetMaxBoundary(4.0f);
	test.Line().SetDetectionAbility(CLineCollisionObject::DETECT_FRONT_SIDE);
	test.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
	test.SetPositiveInclude(WORLD_CATEGORY);
	if (!shadeDynamic)
		test.SetPositiveExclude(ALL_ACTOR_CATEGORIES);

	// Do the query
	g_collisionSystem.ResetStack();
	CLineQuery query;
	if (!query.Test(test))
		return false;

	// Give them back the data
	intersection = query.intersection;
	normal = query.normal;
	shade = ColorToShade(query.color);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool DropShadow::ComputeCoplanar(
Shadow &shadow,
float cameraDSquared,
const Vector3 corner[4])
{
	// Should we hide?
	if (Math::Zero(shadow.coplanarCheckDistanceSquared) || cameraDSquared < shadow.coplanarCheckDistanceSquared)
	{
		const float depth = 0.03f;
		if (!CheckCoplanar::Check(corner, -shadow.normal, shadow.heightTweak - depth, shadow.heightTweak + depth))
			return false;
	}

	// We are coplanar
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Build the positon in the world based on wheter or not an offset was
// specified.
/////////////////////////////////////////////////////////////////////////////
inline void DropShadow::SetupWorldPosition(
const Shadow &shadow,
Vector3 &pos)
{
	if (!shadow.hasCasterOffset)
		pos = *shadow.casterPos;
	else
		pos = *shadow.casterPos + (shadow.casterOffset * *shadow.casterO);
}

/////////////////////////////////////////////////////////////////////////////
// Project v onto the plane defined by n
/////////////////////////////////////////////////////////////////////////////
inline Vector3 DropShadow::ProjectOnNormalPlane(
Vector3CRef normal,
Vector3CRef v)
{
	Vector3 p = v - normal * (Vector3::Dot(v, normal));
	return(p);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
inline float DropShadow::AreaSquared(
Vector3CRef a,
Vector3CRef b)
{
	Vector3 cross = Vector3::Cross(a, b);
	return(cross.LengthSquared());
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
inline void DropShadow::ComputeMajorMinorAxes(
Vector3 projected[3],
float area[3],
Vector3 *&major,
Vector3 *&minor)
{
	// p0 x p1
	if (area[0] > area[1] && area[0] > area[2])
	{
		// which is bigger?
		if (projected[0].LengthSquared() > projected[1].LengthSquared())
		{
			major = &projected[0];
			minor = &projected[1];
		}
		else
		{
			major = &projected[1];
			minor = &projected[0];
		}
	}
	// p0 x p2
	else if (area[1] > area[0] && area[1] > area[2])
	{
		// which is bigger?
		if (projected[0].LengthSquared() > projected[2].LengthSquared())
		{
			major = &projected[0];
			minor = &projected[2];
		}
		else
		{
			major = &projected[2];
			minor = &projected[0];
		}
	}
	// p1 x p2
	else
	{
		// which is bigger?
		if (projected[1].LengthSquared() > projected[2].LengthSquared())
		{
			major = &projected[1];
			minor = &projected[2];
		}
		else
		{
			major = &projected[2];
			minor = &projected[1];
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// Setup the alpha
/////////////////////////////////////////////////////////////////////////////
inline float DropShadow::ComputeAlpha(
Shadow &shadow,
float intersectionY,
float cameraDSquared)
{
	float alpha;

	// Compute alpha for height
	if (Math::Zero(shadow.invHeightAlphaFadeRange))
		alpha = 1.0f;
	else
	{
		float d = Math::Abs(intersectionY - shadow.casterPos->y());
		float a = 1.0f - ((d - shadow.startHeightAlphaFadeDistance) * shadow.invHeightAlphaFadeRange);
		alpha = Math::Clamp(a, 0.0f, 1.0f);
	}

	// Save the fixed height alpha for now
	if (shadow.fixed == COMPUTEFIXED)
		shadow.fixedHeightAlpha = alpha;

	// Is there alpha for camera distance?
	if (cameraDSquared > shadow.startCameraAlphaFadeDistance * shadow.startCameraAlphaFadeDistance)
	{
		float d = Math::Sqrt(cameraDSquared);
		float a = 1.0f - (d - shadow.startCameraAlphaFadeDistance) * shadow.invCameraAlphaFadeRange;
		ASSERT(a >= 0.0f && a <= 1.0f);
		alpha *= a;
	}

	// fading out or in?
	if (!Math::Zero(shadow.fadeAlphaVel))
	{
		shadow.fadeAlpha += shadow.fadeAlphaVel * g_timer.GetFrameSec();
		ShowInstance(&shadow);
		bool bDone = (shadow.fadeAlphaVel < 0.0f) ? shadow.fadeAlpha <= shadow.fadeAlphaTarget : shadow.fadeAlpha >= shadow.fadeAlphaTarget;
		if (bDone)
		{
			shadow.fadeAlphaVel = 0.0f;
			shadow.fadeAlpha = shadow.fadeAlphaTarget;
			if (Math::Zero(shadow.fadeAlpha))
				HideInstance(&shadow);
		}
	}
	alpha *= shadow.fadeAlpha;

	if (shadow.masterAlpha != NULL)
		alpha *= *shadow.masterAlpha;

	// Alpha!
	return shadow.baseAlpha * alpha;
}

/////////////////////////////////////////////////////////////////////////////
// Setup the alpha for just camera
/////////////////////////////////////////////////////////////////////////////
inline float DropShadow::ComputeCameraAlpha(
Shadow &shadow,
float cameraDSquared)
{
	// Is there alpha for camera distance?
	if (cameraDSquared < shadow.startCameraAlphaFadeDistance * shadow.startCameraAlphaFadeDistance)
		return shadow.baseAlpha * shadow.fixedHeightAlpha;
	else
	{
		float d = Math::Sqrt(cameraDSquared);
		float a = 1.0f - (d - shadow.startCameraAlphaFadeDistance) * shadow.invCameraAlphaFadeRange;
		if(a < 0)
			a = 0;
		return shadow.baseAlpha * shadow.fixedHeightAlpha * a;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Some utilities
/////////////////////////////////////////////////////////////////////////////
inline void DropShadow::MakeCorners(
const Vector3 &pos,
const Vector3 &dir,
const Vector3 &right,
Vector3 corner[4])
{
	corner[0] = pos - dir + right;
	corner[1] = pos + dir + right;
	corner[2] = pos - dir - right;
	corner[3] = pos + dir - right;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
inline void DropShadow::MakeColors(
u8 shade,
float alpha,
ts_bRGBA color[4])
{
#ifdef PS2
	color[0].R = color[0].G = color[0].B = shade;
#else
	color[0].R = color[0].G = color[0].B = 255;
#endif
	color[0].A = (u8)Math::Clamp(alpha * 255.0f, 0.0f, 255.0f);
	color[3] = color[2] = color[1] = color[0];
}

