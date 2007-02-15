/////////////////////////////////////////////////////////////////////////////
// Simple footstep emitter and manager
/////////////////////////////////////////////////////////////////////////////
#include "Effects/EffectsPCH.h"
#include "EngineHelper/timer.h"

// There can be only one!
DeclareSingleton(FootstepSystem);

/////////////////////////////////////////////////////////////////////////////
// We must be created and destroyed within the footstep system
/////////////////////////////////////////////////////////////////////////////
FootstepEmitter::FootstepEmitter(
FootstepSystem &system) :
parent(system),
footsteps(false),
pool(32, 16)
{
	// Setup some reasonable defaults
	fadeTime = 3.0f;
	noFadeTime = 6.0f;
	baseAlpha = 1.0f;
	textureIndex = 0;
	scaleX = 0.13f;
	scaleZ = 0.17f;
	scanHeight = 1.0f;
	heightTweak = 0.003f;
	angleJitter = 0.0f;
	fadeSteps = false;
	maxFootsteps = 16;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
FootstepEmitter::~FootstepEmitter()
{
	Clear();
}

/////////////////////////////////////////////////////////////////////////////
// Place a footstep on the ground using collisions to determine
// the best possible position for resting on the ground.
// angle is specified in degrees, and 0 points the footstep along the
// -z axis.
/////////////////////////////////////////////////////////////////////////////
bool FootstepEmitter::Place(
Vector3CRef pos,
float angle)
{
	// Twiddle the angle
	angle += g_random.NegPos() * angleJitter;

	// Collide with the ground
	Vector3 up;
	Vector3 hit;
	u8 shade;
	hit.Set(pos.x(),
			  CollideWithGround(pos, up, shade) + heightTweak,
			  pos.z());

	// Setup the orientation matrix
	float s, c;
	Math::SinCos(Math::Deg2Rad(angle), s, c);
	Vector3 dir;
	dir.Set(s, 0.0f, c);
	Vector3 right = Vector3::Cross(up, dir);
	Vector3 fixDir = Vector3::Cross(right, up);

	return(AddFootstep(hit, right, up, fixDir, shade));
}

/////////////////////////////////////////////////////////////////////////////
// Place the footstep on the ground using a parent coordinate system
// and an offset in model coordinates
// Use the parent's direction vector as the direction
/////////////////////////////////////////////////////////////////////////////
bool FootstepEmitter::Place(
const Graphics4x4 &parent,
Vector3CRef offset,
bool flip)
{
	// Make a position in world coordinates
	Vector3 pos = offset * parent;

	// What is the dir?
	Vector3 dir;
	dir = parent.GetRow2();

	// Do they want to twiddle the dir?
	if (!Math::Zero(angleJitter))
	{
		// Convert back into an angle
		float angle = Math::ArcTan2(dir.z(), dir.x());
		angle += g_random.NegPos() * Math::Deg2Rad(angleJitter);

		float s, c;
		Math::SinCos(angle, s, c);
		dir.Set(c, 0.0f, s);
	}

	// Collide with the ground
	Vector3 up;
	u8 shade;
	pos.y(CollideWithGround(pos, up, shade) + heightTweak);

	// Setup the orientation matrix
	Vector3 right = Vector3::Cross(up, dir);
	Vector3 fixDir = Vector3::Cross(right, up);

	if (flip)
		right *= -1.0f;
	return(AddFootstep(pos, right, up, fixDir, shade));
}

/////////////////////////////////////////////////////////////////////////////
// Clear all footsteps
/////////////////////////////////////////////////////////////////////////////
void FootstepEmitter::Clear()
{
	Footstep *footstep = footsteps.Head();
	while (footstep)
	{
		// Kill this footstep
		Footstep *killMe = footstep;
		footstep = footstep->next;
		RemoveFootstep(killMe);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Call this every frame for updates
/////////////////////////////////////////////////////////////////////////////
void FootstepEmitter::Update(
const Graphics4x4 &camera)
{
	Footstep *footstep = footsteps.Head();

	// fade steps that have exceeded our max
	if (footstep && footsteps.Items() > maxFootsteps)
	{
		// fade kill all extra footsteps
		int kill = footsteps.Items() - maxFootsteps;
		float deltaSec = g_timer.GetFrameSec();
		for (int i = 0; i < kill; i++)
		{
			// Adjust the alpha based on age
			Math::MoveToZero(&footstep->alpha, deltaSec);
			if (!Math::Zero(footstep->alpha))
			{
				SetAlpha(footstep->alpha, *footstep);
				footstep = footstep->next;
			}
			else
			{
				// Kill this footstep
				Footstep *killMe = footstep;
				footstep = footstep->next;
				RemoveFootstep(killMe);
			}
		}
	}

	// if our flags indicate we want to fade steps over time, then do this:
	if (fadeSteps)
	{
		footstep = footsteps.Head();

		// Go through each footstep
		while (footstep)
		{
			// What it the age of this footstep?
			float age = g_timer.GetEndSec() - BornOn(*footstep);

			// Adjust the alpha based on age
			if (age > noFadeTime && age < (noFadeTime + fadeTime))
			{
				// Make the alpha fall off with age
				float ratio = (1.0f - (age - noFadeTime) / fadeTime);
				SetAlpha(baseAlpha * ratio, *footstep);
			}

			if (age < (noFadeTime + fadeTime))
				footstep = footstep->next;
			else
			{
				// Kill this footstep
				Footstep *killMe = footstep;
				footstep = footstep->next;
				RemoveFootstep(killMe);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// Allocate a footstep
/////////////////////////////////////////////////////////////////////////////
bool FootstepEmitter::AddFootstep(
Vector3CRef pos,
Vector3CRef right,
Vector3CRef up,
Vector3CRef dir,
u8 shade)
{
	// Is there an emitter here
	BornOnDecalEmitter *decalEmitter = parent.GetEmitter(textureIndex);
	if (!decalEmitter)
		return false;

	// Allocate a foostep out of the pool
	Footstep *footstep = pool.New();
	if (!footstep)
		return false;

	// Create it!
	footstep->emitter = decalEmitter;

	// Setup all this crapola
	Vector3 corner[4];
	MakeCorners(pos, dir * scaleZ, right * scaleX, corner);

	ts_bRGBA color[4];
	footstep->shade = shade;
	footstep->alpha = baseAlpha;
	MakeColors(shade, baseAlpha, color);

	Vector2 uv[4];
	footstep->emitter->GetDefaultUV(uv);

	// allocate it
	footstep->handle = footstep->emitter->Create(corner, color, uv);

	// try again
	if (!footstep->handle)
	{
		RemoveFootstep(footsteps.Head());
		footstep->handle = footstep->emitter->Create(corner, color, uv);
	}
	if (!footstep->handle)
	{
		pool.Delete(footstep);
		return false;
	}

	// Add it to the list
	footsteps.AddTail(footstep);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Remove a footstep
/////////////////////////////////////////////////////////////////////////////
void FootstepEmitter::RemoveFootstep(
Footstep *footstep)
{
	if (!footstep)
		return;

	// Unlink it from the list
	footsteps.Unlink(footstep);

	// Tell the emitter
	if (footstep->emitter && footstep->handle)
		footstep->emitter->Destroy(footstep->handle);

	// Put it back in the pool
	pool.Delete(footstep);
}

/////////////////////////////////////////////////////////////////////////////
// Return the height of the ground & normal at the specified point
/////////////////////////////////////////////////////////////////////////////
float FootstepEmitter::CollideWithGround(
Vector3CRef pos,
Vector3 &normal,
u8 &shade)
{
#if 0
	// Collide with the world
	g_collisionSystem.ResetStack();
	CXZEntity xzEntity;
	xzEntity.SetPositiveInclude(WORLD_CATEGORY);
	xzEntity.FindFloor(pos.x(), pos.y() + scanHeight, pos.z());

	// Did we hit?
	if (g_collisionSystem.Test(&xzEntity) == NULL)
	{
		normal.Set(0.0f, 1.0f, 0.0f);
		return(pos.y());
	}

	// give them back the normal and the height
	CLineQuery::Normal(xzEntity.Line(), normal);
	return(xzEntity.YVal());
#else
	// Where should it go?
	CLineEntity test;
	Vector3 start = pos;
	start.Y(start.Y() + scanHeight);
	const Vector3 down(0.0f, -1.0f, 0.0f);
	test.Line().SetOriginBasis(start, down);
	test.Line().SetMaxBoundary(scanHeight * 2.0f);
	test.Line().SetDetectionAbility(CLineCollisionObject::DETECT_FRONT_SIDE);
	test.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
	test.SetPositiveInclude(WORLD_CATEGORY);

	// Do the query
	g_collisionSystem.ResetStack();
	CLineQuery query;
	if (!query.Test(test))
	{
		normal.Set(0.0f, 1.0f, 0.0f);
		shade = 0;
		return(pos.y());
	}

	// Give them back the data
	normal = query.normal;
	shade = ColorToShade(query.color);
	return query.intersection.Y();
#endif
}

/////////////////////////////////////////////////////////////////////////////
// Some utilities
/////////////////////////////////////////////////////////////////////////////
inline void FootstepEmitter::MakeCorners(
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
inline void FootstepEmitter::MakeColors(
u8 shade,
float alpha,
ts_bRGBA color[4])
{
	color[0].R = color[0].G = color[0].B = shade;
	color[0].A = (u8)Math::Clamp(alpha * 255.0f, 0.0f, 255.0f);
	color[3] = color[2] = color[1] = color[0];
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
inline void FootstepEmitter::SetAlpha(
float alpha,
Footstep &footstep)
{
	ts_bRGBA color[4];
	footstep.alpha = alpha;
	MakeColors(footstep.shade, alpha, color);
	if (footstep.emitter && footstep.handle)
		footstep.emitter->Update(footstep.handle, NULL, NULL, color);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
FootstepSystem::FootstepSystem() :
decalEmitter(4, 4, true),
emitterList(true)
{
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
FootstepSystem::~FootstepSystem()
{
}

/////////////////////////////////////////////////////////////////////////////
// Initialize the system
/////////////////////////////////////////////////////////////////////////////
bool FootstepSystem::Initialize(
const char *textureDBL,
int maxInstancesPerTexture,
int glossMapVector,
int bumpMapVector)
{
	// Load it!
	if (!decalSet.Load(textureDBL))
		return false;

#ifdef PS2
	// Doh!
	glossMapVector = 0;
	bumpMapVector = 0;
#endif

	// Are we going to have a gloss map?
	int textures = decalSet.Textures();
	if (glossMapVector != 0)
		--textures;

	// Create the emitters
	for (int i = 0; i < textures; i++)
	{
		// Do we have a gloass map
		bool hasGlossMap = ((glossMapVector & (1 << i)) != 0);
		bool hasBumpMap = ((bumpMapVector & (1 << i)) != 0);

		DecalTextureLayer textureLayer[3];
		int textureLayers;
		if (hasGlossMap && hasBumpMap)
		{
			textureLayer[0].DefaultBump(i + decalSet.Textures() / 2);
			textureLayer[1].DefaultGloss(decalSet.Textures() - 1);
			textureLayer[2].DefaultPostBump(i);
			textureLayers = 3;
		}
		else if (hasBumpMap)
		{
			textureLayer[0].DefaultBump(i + decalSet.Textures() / 2);
			textureLayer[1].DefaultPostBump(i);
			textureLayers = 2;
		}
		else if (hasGlossMap)
		{
			textureLayer[0].DefaultGloss(decalSet.Textures() - 1);
			textureLayer[1].Default(i);
			textureLayers = 2;
		}
		else
		{
			textureLayer[0].Default(i);
			textureLayers = 1;
		}

		BornOnDecalEmitter *emitter = new BornOnDecalEmitter(decalSet, textureLayer, textureLayers);
		ASSERT(emitter);
		if (!emitter)
			continue;

		emitter->Setup(maxInstancesPerTexture, -4);
		decalEmitter.Add(emitter);
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Shut down everything
/////////////////////////////////////////////////////////////////////////////
void FootstepSystem::Terminate()
{
	FootstepEmitter *emitter = emitterList.Head();
	while (emitter)
	{
		FootstepEmitter *killMe = emitter;
		emitter = emitter->next;
		emitterList.Unlink(killMe);
		delete killMe;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Call this every frame for updates
/////////////////////////////////////////////////////////////////////////////
void FootstepSystem::Update(
const Graphics4x4 &camera)
{
	FootstepEmitter *emitter = emitterList.Head();
	while (emitter)
	{
		emitter->Update(camera);
		emitter = emitter->next;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Create an emitter, set startingModelIndex to the index returned by LoadModel
/////////////////////////////////////////////////////////////////////////////
FootstepEmitter *FootstepSystem::CreateEmitter(
int startingTextureIndex)
{
	// Create the enitter
	FootstepEmitter *emitter = new FootstepEmitter(*this);
	if (!emitter)
		return(NULL);

	// Add it to the list
	emitterList.AddTail(emitter);

	// Setup the intial model
	emitter->SetTextureIndex(startingTextureIndex);
	return(emitter);
}

/////////////////////////////////////////////////////////////////////////////
// Destroy an emitter, also cleans up all footsteps.
/////////////////////////////////////////////////////////////////////////////
void FootstepSystem::DestroyEmitter(
FootstepEmitter *emitter)
{
	if (emitter)
	{
		emitterList.Unlink(emitter);
		delete emitter;
	}
}

