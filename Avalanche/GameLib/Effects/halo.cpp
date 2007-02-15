//***************************************************************************w
// The halo
//***************************************************************************
#include "Effects/EffectsPCH.h"
#include "Math/sfilter.h"

// Our singleton
Halo *g_haloPtr = NULL;

// Define this for "fast" non-occluding halos
#define FAST_HALO

//***************************************************************************
//***************************************************************************
Halo::Halo()
{
	hTexture = INVALID_HANDLE;

	disableAll = false;

	ignoreEntity = NULL;
	cullDFactor = 0.6f;

	worldHalo  = NULL;
	worldHalos = 0;
}

//***************************************************************************
//***************************************************************************
Halo::~Halo()
{
	// Kill the world halo database
	if (worldHalo)
	{
		for (int i = 0; i < worldHalos; i++)
			worldHalo[i].sprite.Destroy();

		memFree(worldHalo);
		worldHalo = NULL;
		worldHalos = 0;
	}

	// Free all the dynamic halo's
	FreeAllDynamic();

	// Kill our sprite group
	if (hTexture != INVALID_HANDLE)
		SpriteMgr__KillSpriteGroup(HALO_GROUP);
}

//***************************************************************************
// Initialize/terminate the system
//***************************************************************************
void Halo::Initialize()
{
	ASSERT(!g_haloPtr);
	g_haloPtr = new Halo;
	ASSERT(g_haloPtr);

	g_ssEffects.AddTail(g_haloPtr);
}

//***************************************************************************
//***************************************************************************
void Halo::Terminate()
{
	if (!g_haloPtr)
		return;

	g_ssEffects.Remove(g_haloPtr);
	delete g_haloPtr;
	g_haloPtr = NULL;
}

//***************************************************************************
// Load the sprite DBL
//***************************************************************************
bool Halo::LoadSprites(
char *spriteDBL)
{
#ifndef FAST_HALO
	// duh
	if (spriteDBL == NULL)
		return(false);

	// Load the dataset
	hTexture = dblLoad(spriteDBL);
	if (hTexture == INVALID_HANDLE)
		return(false);
#endif

	return(true);
}

/////////////////////////////////////////////////////////////////////////////
// load halos from the world dbl
/////////////////////////////////////////////////////////////////////////////
bool Halo::LoadHalosFromDBL(
t_Handle worldHandle)
{
	// Do the halos
	ts_RecordList *recordList = (ts_RecordList *)dblGetChunkData(DB_HALO, DEFAULT_CHUNK_KEY_STRING, worldHandle);
	if (!recordList)
		return false;

	// setup array
	worldHalos = (int)recordList->usRecordCount;

	// create a list of halos
	worldHalo = (WorldHalo *)memAlloc(sizeof(WorldHalo) * recordList->usRecordCount);
	ASSERT(worldHalo);
	if (!worldHalo)
		return false;

	// Clear it!
	memset(worldHalo, '\0', sizeof(WorldHalo) * recordList->usRecordCount);

	for (int i = 0; i < worldHalos; i++)
	{
		ts_EnvHalo *source = (ts_EnvHalo *)GetRecordAddress(recordList, i);
		ASSERT(source);

		// Construct the element
		new (&worldHalo[i].sprite) HaloElement;

		// setup position and direction
		worldHalo[i].pos = source->pos;
		worldHalo[i].dir = source->dir;

		// setup flags (these don't seem to work from tool)
#ifdef FAST_HALO
		worldHalo[i].inWorld = true;
#else
		worldHalo[i].inWorld = false;
#endif
		worldHalo[i].hasDir = false;
		worldHalo[i].rotate = true;
		worldHalo[i].disabled = false;

		// setup size
		if (worldHalo[i].inWorld)
			worldHalo[i].size = source->size * 10.0f;
		else
			worldHalo[i].size = source->size;

		// setup colors
		worldHalo[i].color.R = source->color.R;
		worldHalo[i].color.G = source->color.G;
		worldHalo[i].color.B = source->color.B;
		worldHalo[i].color.A = source->color.A;

		// set texture index
		worldHalo[i].textureIndex = source->textureIndex;
	}

#ifdef PS2
	// The sprite stuff should do this
	for (int i = 0; i < worldHalos; i++)
	{
		worldHalo[i].color.R /= 2;
		worldHalo[i].color.G /= 2;
		worldHalo[i].color.B /= 2;
		//worldHalo[i].color.A /= 2;
	}
#endif

	// Setup the texture stuff
	if (hTexture != INVALID_HANDLE)
		SpriteMgr__SetTextureDbl(hTexture);

	// Create all the sprites/particles
	for (int i = 0; i < worldHalos; i++)
	{
		if (worldHalo[i].inWorld)
			worldHalo[i].sprite.Setup(worldHalo[i].textureIndex, worldHalo[i].color, &worldHalo[i].pos);
		else
			worldHalo[i].sprite.Setup(worldHalo[i].textureIndex, worldHalo[i].color);
	}

	// Assume we are not disabled
	disableAll = false;

	// It's loaded (we are now fat)
	return(true);
}

//***************************************************************************
// Add a dynamic halo, returns null on failure
// position and orienation are saved as POINTERS so make sure you remove
// the halo before deleting your object.
// If modelOFfset is non-null, it provides an offset in model coordinates of
// the halo.
// Dirtype states with direction the light is facing...if it is NONE then
// the light is omni-directional
//***************************************************************************
Halo::Handle Halo::Add(
Vector3 *position,
DirCos3x3 *orientation,
int spriteIndex,
const ts_bRGBA &color,
float size,
bool rotate,
bool trueSizing,
const Vector3 *modelOffset,
DIRTYPE dirType,
bool dynamicBlock,
CCollisionStackArray<CStageEntity*> *ignoreList)
{
	// Make sure they aren't on crack
	ASSERT(position);
	ASSERT(orientation);

	// Can't do it if they don't have a texture set
	ASSERT(hTexture != INVALID_HANDLE);
	if (hTexture == INVALID_HANDLE)
		return(NULL);

	// Allocate a new thingie
	DynamicHalo *halo = Allocate();

	// Fill it out
	halo->automaton = NULL;
	halo->pos = position;
	halo->o = orientation;
	if (!modelOffset)
		halo->hasModelOffset = false;
	else
	{
		halo->hasModelOffset = true;
		halo->modelOffset = *modelOffset;
	}
	halo->hidden = false;
	halo->color = color;
	halo->trueSizing = trueSizing;
	halo->size = size;
	halo->rotate = rotate;
	halo->dirType = dirType;
	halo->dynamicBlock = dynamicBlock;
	halo->ignoreList = ignoreList;

	// Setup the halo element
	SpriteMgr__SetTextureDbl(hTexture);
	halo->element.Setup(spriteIndex, halo->color);

	return(halo);
}

//***************************************************************************
// Add a haloAutomaton type halo
//***************************************************************************
Halo::Handle Halo::Add(
HaloAutomaton &automaton,
int spriteIndex,
const ts_bRGBA &color,
float size,
bool rotate,
bool trueSizing,
const Vector3 *modelOffset,
DIRTYPE dirType,
bool dynamicBlock,
CCollisionStackArray<CStageEntity*> *ignoreList)
{
	// Can't do it if they don't have a texture set
	ASSERT(hTexture != INVALID_HANDLE);
	if (hTexture == INVALID_HANDLE)
		return(NULL);

	// Allocate a new thingie
	DynamicHalo *halo = Allocate();

	// Bind it back into the automaton
	automaton.Bind(this, halo);

	// Fill it out
	halo->automaton = &automaton;
	halo->pos = automaton.Position();
	halo->o = automaton.Orientation();
	if (!modelOffset)
		halo->hasModelOffset = false;
	else
	{
		halo->hasModelOffset = true;
		halo->modelOffset = *modelOffset;
	}
	halo->hidden = false;
	halo->color = color;
	halo->trueSizing = trueSizing;
	halo->size = size;
	halo->rotate = rotate;
	halo->dirType = dirType;
	halo->dynamicBlock = dynamicBlock;
	halo->ignoreList = ignoreList;

	// Setup the halo element
	SpriteMgr__SetTextureDbl(hTexture);
	halo->element.Setup(spriteIndex, halo->color);

	return(halo);
}

//***************************************************************************
//***************************************************************************
void Halo::ChangeSize(
Handle haloHandle,
float newSize)
{
	ASSERT(haloHandle);
	DynamicHalo *halo = (DynamicHalo *)haloHandle;
	halo->size = newSize;
}

//***************************************************************************
//***************************************************************************
void Halo::ChangeColor(
Handle haloHandle,
const ts_bRGBA &newColor)
{
	ASSERT(haloHandle);
	DynamicHalo *halo = (DynamicHalo *)haloHandle;
	halo->color = newColor;
	halo->element.SetColor(halo->color);
}

//***************************************************************************
//***************************************************************************
void Halo::ChangeRotate(
Handle haloHandle,
bool newRotate)
{
	ASSERT(haloHandle);
	DynamicHalo *halo = (DynamicHalo *)haloHandle;
	halo->rotate = newRotate;
}

//***************************************************************************
//***************************************************************************
void Halo::ChangeModelOffset(
Handle haloHandle,
Vector3 *newModelOffset)
{
	ASSERT(haloHandle);
	DynamicHalo *halo = (DynamicHalo *)haloHandle;
	if (!newModelOffset)
		halo->hasModelOffset = false;
	else
	{
		halo->modelOffset = *newModelOffset;
		halo->hasModelOffset = true;
	}
}

//***************************************************************************
//***************************************************************************
void Halo::ChangeDirType(
Handle haloHandle,
DIRTYPE newDirType)
{
	ASSERT(haloHandle);
	DynamicHalo *halo = (DynamicHalo *)haloHandle;
	halo->dirType = newDirType;
}

//***************************************************************************
//***************************************************************************
void Halo::ChangeCollision(
Handle haloHandle,
bool newDynamicBlock,
CCollisionStackArray<CStageEntity*> *newIgnoreList)
{
	ASSERT(haloHandle);
	DynamicHalo *halo = (DynamicHalo *)haloHandle;
	halo->dynamicBlock = newDynamicBlock;
	if (halo->ignoreList != newIgnoreList)
		CCollisionStackItem::Delete(halo->ignoreList);
	halo->ignoreList = newIgnoreList;
}

//***************************************************************************
// Hide the halo
//***************************************************************************
void Halo::Hide(
Handle haloHandle,
bool hide)
{
	ASSERT(haloHandle);
	DynamicHalo *halo = (DynamicHalo *)haloHandle;
	halo->hidden = hide;
}

//***************************************************************************
//***************************************************************************
bool Halo::Hidden(Handle haloHandle)
{
	ASSERT(haloHandle);
	DynamicHalo *halo = (DynamicHalo *)haloHandle;
	return(halo->hidden);
}

//***************************************************************************
// Remove the halo once it is done
//***************************************************************************
bool Halo::Remove(
Handle haloHandle)
{
	if (!haloHandle)
		return(true);

	DynamicHalo *halo = (DynamicHalo *)haloHandle;
	Free(halo);
	return(true);
}

//***************************************************************************
// Allocate/free a dynmaic halo (puts it at tail of list)
//***************************************************************************
Halo::DynamicHalo *Halo::Allocate()
{
	DynamicHalo *halo = new DynamicHalo;
	if (!halo)
		return(NULL);

	// Insert it into the list
	dynamicList.AddTail(halo);

	// Give it back so they can fill it out
	return(halo);
}

//***************************************************************************
//***************************************************************************
void Halo::Free(
DynamicHalo *halo)
{
	if (halo)
	{
		halo->element.Destroy();
		CCollisionStackItem::Delete(halo->ignoreList);
		dynamicList.Unlink(halo);
		delete halo;
	}
}

//***************************************************************************
//***************************************************************************
void Halo::FreeAllDynamic()
{
	DynamicHalo *halo = dynamicList.Head();
	while (halo)
	{
		Free(halo);
		halo = dynamicList.Head();
	}
}

//***************************************************************************
// Find and disable a static halo
//***************************************************************************
bool Halo::FindAndEnable(
const AABB &insideOf,
bool enable)
{
	bool gotOne = false;
 	for (int i = 0; i < worldHalos; i++)
	{
		if (insideOf.PointIn(worldHalo[i].pos))
		{
			worldHalo[i].disabled = !enable;
			gotOne = true;
		}
	}

	return gotOne;
}

//***************************************************************************
// Draw
//***************************************************************************
void Halo::Update(
CameraScene *scene)
{
	// Anything to do?
	if (worldHalos == 0 && dynamicList.Head() == NULL)
		return;

	// time
	float deltaT = g_timer.GetFrameSec();

	// Get the current camera
	Camera &camera = g_cameraDirector.CurrentCamera(scene);
	Matrix4x4 m = camera.WorldToView();
	Vector3 cameraPos = camera.Eye();
	Vector3 cameraDir = camera.Dir();

	// Setup our clip distance
	float farClip = g_cameraDirector.GetCurrentProjection(scene).Yon();
	float cullDSquared = Math::Square(farClip * cullDFactor);

	// Update the world halo's
	for (int i = 0; i < worldHalos; i++)
	{
		Update(camera, cullDSquared, worldHalo[i]);
		worldHalo[i].sprite.Update(cameraPos, deltaT);
	}

	// Update the dynamic
	DynamicHalo *halo = dynamicList.Head();
	while (halo)
	{
		// If update fails, hide it
		Update(camera, cullDSquared, *halo);
		halo->element.Update(cameraPos, deltaT);

		// Next in list
		halo = halo->next;
	}
}

//***************************************************************************
// Update a static halo
//***************************************************************************
void Halo::Update(
Camera &camera,
float cullDSquared,
WorldHalo &halo)
{
	if (RenderMgr__IsFading() || disableAll)
	{
		halo.sprite.SetState(HaloElement::HIDDEN);
		return;
	}

	// Handle the two cases differently
	if (halo.inWorld)
	{
		// Is the halo on screen?
		Vector2 screen;
		float w;
		const Vector2 clipFactor(1.4f, 1.4f);
		if (!camera.ToScreen(halo.pos, screen, w, clipFactor))
			return;

		// What is our size?
		float screenSize = halo.size;
		if (halo.hasDir)
		{
			float dot = Vector3::Dot(halo.dir, camera.Dir());
			if (dot < 0.0f)
				screenSize *= -dot;
			else
			{
				halo.sprite.SetState(HaloElement::HIDDEN);
				return;
			}
		}

		// Scale by w
		screenSize *= 1.0f + ((w - hither) * invHitherYonDelta) * 2.0f;

		// Tell the halo all about this stuff
		halo.sprite.SetPosition(screen.x(), screen.y(), screenSize, aspect);

		// Figure out angle stuff
		if (halo.rotate)
		{
			float theta = Math::ArcTan2(screen.y() - 0.5f, screen.x() - 0.5f);
			halo.sprite.SetRotation(theta);
		}

		// Wee!
		if (!halo.disabled)
			halo.sprite.SetState(HaloElement::VISIBLE);
		else
			halo.sprite.SetStateSmart(HaloElement::FADEOUT);
	}
	else
	{
		// Is the halo on screen?
		Vector2 screen;
		float w;
		if (!camera.ToScreen(halo.pos, screen, w))
		{
			halo.sprite.SetState(HaloElement::HIDDEN);
			return;
		}

		// What is our size?
		float screenSize = halo.size;
		if (halo.hasDir)
		{
			float dot = Vector3::Dot(halo.dir, camera.Dir());
			if (dot < 0.0f)
				screenSize *= -dot;
			else
			{
				halo.sprite.SetState(HaloElement::HIDDEN);
				return;
			}
		}

		// Scale by w
		screenSize *= 1.0f - ((w - hither) * invHitherYonDelta);
		if (screenSize < 0.01f)
		{
			halo.sprite.SetState(HaloElement::HIDDEN);
			return;
		}
		else if (screenSize < halo.size * 0.2f)
			screenSize = halo.size * 0.2f;

		// Tell the halo all about this stuff
		halo.sprite.SetPosition(screen.x(), screen.y(), screenSize, aspect);

		// Figure out angle stuff
		if (halo.rotate)
		{
			float theta = Math::ArcTan2(screen.y() - 0.5f, screen.x() - 0.5f);
			halo.sprite.SetRotation(theta);
		}

		// do we care?
		if (halo.disabled || (halo.pos - camera.Eye()).LengthSquared() > cullDSquared)
		{
			halo.sprite.SetStateSmart(HaloElement::FADEOUT);
			return;
		}

		// this is nuts
		int index = (&halo - &worldHalo[0]) / sizeof(WorldHalo);
		if ((index & 1) != (g_timer.GetFrameCount() & 1))
			return;

		// In shadow?
		CLineEntity detectEntity;
		detectEntity.Line().SetOriginEnd(camera.Eye(), halo.pos);
		detectEntity.Line().SetMaxBoundary(0.98f);
		detectEntity.Line().SetFindMode(CLineCollisionObject::FIND_ANY);
		detectEntity.SetIgnoreEntity(ignoreEntity);
		g_collisionSystem.ResetStack();
		bool occluded = (g_collisionSystem.Test(&detectEntity) != NULL);
		if (occluded)
			halo.sprite.SetStateSmart(HaloElement::FADEOUT);
		else
			halo.sprite.SetStateSmart(HaloElement::FADEIN);
	}
}

//***************************************************************************
// Update a dynamic
//***************************************************************************
void Halo::Update(
Camera &camera,
float cullDSquared,
DynamicHalo &halo)
{
	// Does this halo have an automaton?
	if (halo.automaton)
		halo.automaton->Update();

	if (RenderMgr__IsFading() || disableAll)
	{
		halo.element.SetState(HaloElement::HIDDEN);
		return;
	}

	// Is the halo on screen?
	Vector3 worldPos = WorldPosition(halo);
	Vector2 screen;
	float w;
	if (!camera.ToScreen(worldPos, screen, w))
	{
		 halo.element.SetState(HaloElement::HIDDEN);
		return;
	}

	// What is our size?
	float size = halo.size;
	if (halo.dirType != NONE)
	{
		Vector3 dir = WorldDir(halo);
		float dot = Vector3::Dot(dir, camera.Dir());
		if (dot > 0.0f)
		{
			halo.element.SetState(HaloElement::HIDDEN);
			return;
		}
		size *= -dot;
	}

	// Scale by w
	if (halo.trueSizing)
		size *= 1.0f - ((w - hither) * invHitherYonDelta);
	else
		size *= 1.0f / w;

	// Too small to see?
	if (size < 0.005f)
	{
		halo.element.SetState(HaloElement::HIDDEN);
		return;
	}
	else if (!halo.trueSizing && size < halo.size * 0.2f)
		size = halo.size * 0.2f;

	// Tell the halo all about this stuff
	halo.element.SetPosition(screen.x(), screen.y(), size, aspect);

	// Figure out angle stuff
	float theta;
	if (halo.rotate)
		theta = Math::ArcTan2(screen.y() - 0.5f, screen.x() - 0.5f);
	else
		theta = 0.0f;
	halo.element.SetRotation(theta);

	// do we care?
	if (halo.hidden || (worldPos - camera.Eye()).LengthSquared() > cullDSquared)
	{
		halo.element.SetStateSmart(HaloElement::FADEOUT);
		return;
	}

	// In shadow?
	CLineEntity detectEntity;
	detectEntity.SetIgnoreList(halo.ignoreList);
	if (!halo.dynamicBlock)
		detectEntity.SetPositiveExclude(ALL_ACTOR_CATEGORIES);
	detectEntity.Line().SetOriginEnd(camera.Eye(), worldPos);
	detectEntity.Line().SetMaxBoundary(0.98f);
	detectEntity.Line().SetFindMode(CLineCollisionObject::FIND_ANY);
	detectEntity.SetIgnoreEntity(ignoreEntity);
	g_collisionSystem.ResetStack();
	bool occluded = (g_collisionSystem.Test(&detectEntity) != NULL);
	if (occluded)
		halo.element.SetStateSmart(HaloElement::FADEOUT);
	else
		halo.element.SetStateSmart(HaloElement::FADEIN);
}

//***************************************************************************
//***************************************************************************
inline Vector3 Halo::WorldPosition(
DynamicHalo &halo)
{
	// If there is no position offset, just return pos
	if (!halo.hasModelOffset)
		return(*halo.pos);
	else
	{
		// Plase Mr. Optimizer, take this out
		const DirCos3x3 &m = *halo.o;
		Vector3CRef p = *halo.pos;
		Vector3CRef o = halo.modelOffset;

		// simulate a minimal matrix multiply
		Vector3 w;
		w.Set( o.x() * m.e00 + o.y() * m.e10 + o.z() * m.e20 + p.x(),
				 o.x() * m.e01 + o.y() * m.e11 + o.z() * m.e21 + p.y(),
				 o.x() * m.e02 + o.y() * m.e12 + o.z() * m.e22 + p.z());
		return(w);
	}
}

//***************************************************************************
//***************************************************************************
inline Vector3 Halo::WorldDir(
DynamicHalo &halo)
{
	Vector3 dir;

	switch (halo.dirType)
	{
		case NONE:
			dir.Clear();
			break;
		case XAXIS:
			dir.Set(halo.o->e00, halo.o->e01, halo.o->e02);
			break;
		case YAXIS:
			dir.Set(halo.o->e10, halo.o->e11, halo.o->e12);
			break;
		case ZAXIS:
			dir.Set(halo.o->e20, halo.o->e21, halo.o->e22);
			break;
		case NEGXAXIS:
			dir.Set(halo.o->e00, halo.o->e01, halo.o->e02);
			dir.Negate();
			break;
		case NEGYAXIS:
			dir.Set(halo.o->e10, halo.o->e11, halo.o->e12);
			dir.Negate();
			break;
		case NEGZAXIS:
			dir.Set(halo.o->e00, halo.o->e01, halo.o->e02);
			dir.Negate();
			break;
	}

	return(dir);
}
