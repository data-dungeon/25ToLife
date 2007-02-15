///////////////////////////////////////////////////////////////////////////////
// Shatter!
///////////////////////////////////////////////////////////////////////////////
#include "Effects/EffectsPCH.h"

#if defined(PS2)
#define VTXBUFFER_NEED_INLINES
#endif //
#include "Display/Common/VtxBuffer.h"

#include "GameHelper/ParamMgr.h"
#include "EngineHelper/sfx.h"
#include "Math/fastrand.h"
#include "Math/sfilter.h"
#include "Effects/shatter.h"

//#ifdef PS2
// Faster but requires additive alpha blending
#define FLATREFLECT
//#endif

// Our texture array for fast rendering
Matrix4x4 ShatterObject::s_texMatrix[8];

// This is hot!
DeclareSingleton(ShatterOverlord);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CParam<ShatterParams>::RegisterVariables()
{
	RegisterVariable(d_data.textureDBL, "textureDBL", "", sizeof(d_data.textureDBL) - 1);

	RegisterVariable(d_data.maxActive, "maxActive", 3, 1, 20);

	RegisterVariable(d_data.reclaim, "reclaim", true);

	RegisterVariable(d_data.startSound, "startSound", "", sizeof(d_data.startSound) - 1);

	// Base color of glass
	RegisterVariable(d_data.colorR, "colorR", 255, 0, 255);
	RegisterVariable(d_data.colorG, "colorG", 255, 0, 255);
	RegisterVariable(d_data.colorB, "colorB", 255, 0, 255);
	RegisterVariable(d_data.colorA, "colorA", 255, 0, 255);

	// Piece size override
	RegisterVariable(d_data.piecesW, "piecesW", 10, 3, 16);
	RegisterVariable(d_data.piecesH, "piecesH", 10, 3, 16);

	// Time to live for the whole effect
	RegisterVariable(d_data.timeToLive, "timeToLive", 10.0f, 0.0f, 100.0f);

	// Do we collide?
	RegisterVariable(d_data.collide, "collide", false);
	RegisterVariable(d_data.shardHitSound, "shardHitSound", "", sizeof(d_data.shardHitSound) - 1);

	// Gravity in m/s squared
	RegisterVariable(d_data.gravity, "gravity", -9.8f, -100.0f, 100.0f);

	// Time constant for alpha fade
	RegisterVariable(d_data.alphaStart, "alphaStart", 1.0f, 0.0f, 10.0f);
	RegisterVariable(d_data.alphaFadeRate, "alphaFadeRate", 0.5f, 0.0f, 10.0f);

	// Energy (speed squared) that pieces get marked as "atRest"
	RegisterVariable(d_data.atRestThresholdSquared, "atRestThresholdSquared", 1.0f, 0.0f, 100.0f);

	// Conversion factor for mapping speed m/s into sound volume
	RegisterVariable(d_data.speedToVolume, "speedToVolume", 0.5f, 0.0f, 10.0f);

	// Factor to reduce speed by during a bounce (less than 1)
	RegisterVariable(d_data.bounceSpeedFactor, "bounceSpeedFactor", 0.5f, 0.0f, 1.0f);

	// Factor to reduce rotation by during a bounce (less than 1)
	RegisterVariable(d_data.bounceRotationFactor, "bounceRotationFactor", -0.7f, -1.0f, 1.0f);

	// When triangles are split, where along the edge they split [0 - 1]
	RegisterVariable(d_data.splitMinT, "splitMinT", 0.2f, 0.0f, 1.0f);
	RegisterVariable(d_data.splitMaxT, "splitMaxT", 0.8f, 0.0f, 1.0f);

	// ground tweak
	RegisterVariable(d_data.groundTweak, "groundTweak", 0.1f, 0.0f, 1.0f);

	// Velocity ranges for parts
	RegisterVariable(d_data.topInPlaneVMin, "topInPlaneVMin", -0.5f, -10.0f, 10.0f);
	RegisterVariable(d_data.topInPlaneVMax, "topInPlaneVMax", 0.5f, -10.0f, 10.0f);

	RegisterVariable(d_data.bottomInPlaneVMin, "bottomInPlaneVMin", -0.5f, -10.0f, 10.0f);
	RegisterVariable(d_data.bottomInPlaneVMax, "bottomInPlaneVMax", 0.5f, -10.0f, 10.0f);

	RegisterVariable(d_data.rightInPlaneVMin, "rightInPlaneVMin", 0.0f, -10.0f, 10.0f);
	RegisterVariable(d_data.rightInPlaneVMax, "rightInPlaneVMax", 1.0f, -10.0f, 10.0f);

	RegisterVariable(d_data.leftInPlaneVMin, "leftInPlaneVMin", -1.0f, -10.0f, 10.0f);
	RegisterVariable(d_data.leftInPlaneVMax, "leftInPlaneVMax", 0.0f, -10.0f, 10.0f);

	RegisterVariable(d_data.inPlaneVMin, "inPlaneVMin", -1.0f, -10.0f, 10.0f);
	RegisterVariable(d_data.inPlaneVMax, "inPlaneVMax", 1.0f, -10.0f, 10.0f);

	RegisterVariable(d_data.normalPlaneVMin, "normalPlaneVMin", 0.2f, -10.0f, 10.0f);
	RegisterVariable(d_data.normalPlaneVMax, "normalPlaneVMax", 0.6f, -10.0f, 10.0f);

	RegisterVariable(d_data.rotVelMin, "rotVelMin", 2.0f, 0.0f, 100.0f);
	RegisterVariable(d_data.rotVelMax, "rotVelMax", 6.0f, 0.0f, 100.0f);

	// Hang time
	RegisterVariable(d_data.minHangTime, "minHangTime", 0.0f, 0.0f, 10.0f);
	RegisterVariable(d_data.maxHangTime, "maxHangTime", 1.0f, 0.0f, 10.0f);

	// Env map
	RegisterVariable(d_data.environmentMap, "environmentMap", false);
	RegisterVariable(d_data.environmentWrap, "environmentWrap", 1.0f, 1.0f, 10.0f);
	RegisterVariable(d_data.environmentWrapFlat, "environmentWrapFlat", 0.0f, 1.0f, 10.0f);

#ifndef PS2
	RegisterVariable(d_data.bumpMap, "bumpMap", false);
#else
	d_data.bumpMap = false;
#endif
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ShatterObject::ShatterObject(
const ShatterParams &params,
TextureArray &textureArray) :
m_params(params),
m_textureArray(textureArray)
{
	// clear vars
	m_instance = NULL;
	m_baseTexture = -1;
	m_envTexture = -1;
#ifndef PS2
	m_bumpTexture = -1;
#endif

	// create our shatter pieces
	m_shards = ((m_params.piecesW + m_params.piecesH) * 2) * 3;
	ASSERT(m_shards > 0);

	m_shard = new Shard[m_shards];
	ASSERT(m_shard);

	m_state = READY;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ShatterObject::~ShatterObject()
{
	// Get rid of all the info
	DestroyModel();

	// kill pieces
	delete [] m_shard;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ShatterObject::Start(
CProtagonist &actor,
const Vector3 *weakSpot,
int weakSpots)
{
	// Get the collision object for a simple collision configuration.
	CCollisionObject *collision = actor.GetCollisionObject();
	ASSERT(collision && collision->GetObjectType() == CCollisionObject::OBB_OBJECT);
	if (!collision || collision->GetObjectType() != CCollisionObject::OBB_OBJECT)
		return false;

	// Get the obb
	OBB *obb = (OBB *)collision;
	return Start(*obb, weakSpot, weakSpots);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ShatterObject::Start(
const OBB &obb,
const Vector3 *weakSpot,
int weakSpots)
{
	// What is the minimum axis
	Axis minAxis = MinimumAxis(obb);

	// Compute the corners
	Vector3 corner[4];
	ComputeCorners(obb, minAxis, corner);

	// Do it!
	return Start(corner, weakSpot, weakSpots);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ShatterObject::Start(
const Vector3 corner[4],
const Vector3 *weakSpot,
int weakSpots)
{
	// form the params for the new mesh
	m_topLeft = corner[0];
	m_topRight = corner[1];
	m_bottomRight = corner[2];
	m_bottomLeft = corner[3];

	// Compute the coordinate system
	m_right = (m_topLeft - m_topRight).Normalized();
	m_up = (m_topLeft - m_bottomLeft).Normalized();
	m_normal = Vector3::Cross(m_right, m_up);

	// Make sure we have the model
	if (!CreateModel())
		return false;

	// get width/height
	m_height = (m_topLeft - m_bottomLeft).Length();
	m_width = (m_topRight - m_topLeft).Length();

	// Setup stuff
	m_vertexColor.R = m_params.colorR;
	m_vertexColor.G = m_params.colorG;
	m_vertexColor.B = m_params.colorB;
#ifdef PS2
	m_vertexColor.A = m_params.colorA / 2;
#else
	m_vertexColor.A = m_params.colorA;
#endif

	// Create the weakspot array
	m_weakSpots = Math::Min(weakSpots, MAX_WEAKSPOTS);
	for (int w = 0; w < m_weakSpots; w++)
		m_weakSpot[w] = weakSpot[w];

	// create the shatter mesh
	CreateShards();

	// Seutp our state
	m_state = READY;
	Vector3 center = (corner[0] + corner[1] + corner[2] + corner[3]) / 4.0f;
	if (m_params.collide)
		m_ttl = m_params.timeToLive;
	else
	{
		// Figure out the time to hit the ground
		Vector3 up;
		if (m_up.Y() > 0.0f)
			up = m_up;
		else
			up = -m_up;
		Vector3 top = center + up * (m_height * 0.5f);
		float range = 0.5f * -m_params.gravity * Math::Square(m_params.timeToLive);
		float frontHeight = Height(top + m_normal * 0.5f, range);
		float backHeight = Height(top - m_normal * 0.5f, range);

		// We have two heights, pick the bigger of the two
		float height = Math::Max(frontHeight, backHeight);
		float timeToFall = Math::Sqrt(2.0f * height / -m_params.gravity) + m_params.maxHangTime;

		// Figure out the best TTL
		m_ttl = Math::Max(Math::Min(m_params.timeToLive, timeToFall), 1.0f);
	}

	// Kick it off
	m_state = SHATTERING;

	// Play the sound
	Sfx::PlayStationary(m_params.startSound, center);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// update all the triangles for the shattering
///////////////////////////////////////////////////////////////////////////////
void ShatterObject::Update(
float deltaT)
{
	// Are we done
	if (m_ttl < 0.0f && m_state != DONE)
		m_state = DONE;

	// Run the physics!
	if (m_state == SHATTERING && deltaT > 0.0f)
	{
		m_ttl -= deltaT;
		if (m_params.collide)
			RunPhysicsCollide(g_cameraDirector.CurrentCamera(), deltaT);
		else
			RunPhysicsSimple(g_cameraDirector.CurrentCamera(), deltaT);
	}
}

///////////////////////////////////////////////////////////////////////////////
// Render the Shattered  pieces
///////////////////////////////////////////////////////////////////////////////
void ShatterObject::Render(
const CInstance *instance,
const Graphics4x4 *localToCamera,
float zCamDist)
{
	// only render if we are shattering
	if (m_state == SHATTERING)
	{
		// Weee
		TextureMgr::Cache().Unlock();
		m_textureArray.Lock();

#ifndef PS2
		if (m_params.bumpMap)
		{
			// Fake a light for testing
			Camera &camera = g_cameraDirector.CurrentCamera();
			Vector3 light = camera.Dir();

			static float z = 0.5f;
			light.Z(z);
			light.Normalize();

			_RenderState__SetTFactor(light.X(), light.Y(), light.Z(), 0.0f);
		}
#endif

		// render it
		m_stripBuffer.Render(*localToCamera, s_texMatrix, 0);

		// Done
		m_textureArray.Unlock();
	}
}

///////////////////////////////////////////////////////////////////////////////
// Init the texture matrix
///////////////////////////////////////////////////////////////////////////////
void ShatterObject::InitializeStatics()
{
  	// init all matrices to identity
	for (int i = 0; i < 8; i++)
		s_texMatrix[i].Identity();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ShatterObject::CreateModel()
{
	// Already have?
	if (m_instance)
	{
		UpdateBounds();
		return true;
	}

	// Dummy up the istance
	m_instance = new CInstance;
	ASSERT(m_instance);
	if (!m_instance)
		return false;

	// clear the bounding box for now
	const Vector3 origin(0.0f, 0.0f, 0.0f);
	BoundingBox.Min = BoundingBox.Max = origin;
	BoundingSphere.Center = origin;
	BoundingSphere.Radius = 0.0f;

	// Make the instance
	m_instance->InitFromDynamicObject(this);
	m_instance->SetName("shatter");

	// Compute bounds
	UpdateBounds();

	// Render flags
	m_instance->SetAlphaSort(true);
	m_instance->SetZWriteDisabled(true);
	m_instance->SetReceiveShadows(false);
	m_instance->SetDynamicallyLit(false);
	m_instance->SetRenderBucket(INSTANCE_RENDER_BUCKET_MAX - 1);

	// Init the counts
	u32 vtxFlags = VtxBuffer::VERT_HAS_DIFFUSE | VtxBuffer::VERT_WRITEONLY;
	u32 texFlags[8] = { VtxBuffer::VERT_TEX_DIM2, 0, 0, 0, 0, 0, 0, 0 };

	int extraLayers = 0;
	if (m_params.environmentMap)
		++extraLayers;
#ifndef PS2
	if (m_params.bumpMap)
		++extraLayers;
#endif
	for (int l = 0; l < extraLayers; l++)
		texFlags[l + 1] = VtxBuffer::VERT_TEX_DIM2;

	m_vertexBuffer.Init(m_shards * 3, vtxFlags, texFlags);

	// init our strip buffer
	m_stripBuffer.Init();

 	// set number layers to render
	m_stripBuffer.SetTextureArray(&m_textureArray, 0);
#ifndef PS2
	m_stripBuffer.SetNumTexturePasses(1 + extraLayers);
#endif
	m_stripBuffer.SetVtxBuffer(&m_vertexBuffer);

	// Doh!
	int onPassTexture = 0;

	// Do the bump map first!
#ifndef PS2
	if (m_params.bumpMap)
	{
		int bumpMap = m_textureArray.TextureCount() - 2;
		m_bumpTexture = onPassTexture++;
		m_stripBuffer.SetPassTexture(m_bumpTexture, m_textureArray.GetTexture(bumpMap),
												TS_METHOD_DOT3,
												TS_WRAP_CLAMP,
												TS_WRAP_CLAMP,
												TS_FILTER_BILINEAR,
												RS_BLENDFUNC_DOT3);
	}
#endif

	// Set the base texture
	int baseTextures = m_textureArray.TextureCount() - extraLayers;
	ASSERT(baseTextures > 0);
	m_baseTexture = onPassTexture++;
	m_stripBuffer.SetPassTexture(m_baseTexture, m_textureArray.GetTexture(g_random.UpToExcludeHigh(baseTextures)),
											TS_METHOD_MODULATE,
											TS_WRAP_CLAMP,
											TS_WRAP_CLAMP,
											TS_FILTER_BILINEAR,
											RS_BLENDFUNC_DEFAULT);

	// Do they have an env map? (alwyas the last etexture
	if (m_params.environmentMap)
	{
		int envMap = m_textureArray.TextureCount() - 1;
		m_envTexture = onPassTexture++;
		m_stripBuffer.SetPassTexture(m_envTexture, m_textureArray.GetTexture(envMap),
#ifdef PS2
												TS_METHOD_MODULATE,
#else
												TS_METHOD_INTERPOLATE,
#endif
												TS_WRAP_CLAMP,
												TS_WRAP_CLAMP,
												TS_FILTER_BILINEAR,
#ifdef PS2
												RS_BLENDFUNC_DEFAULT);
#else
												RS_BLENDFUNC_DEFAULT);
#endif
	}

	// add him to the scene
	SceneMgr::AddInstance(m_instance);

	// glass doesn't move, so no vis region reassignment
	SceneMgr::SetInstanceVisRegionStatic(m_instance, false);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ShatterObject::UpdateBounds()
{
	ASSERT(m_instance);

	// setup the bounding data
	BoundingBox__Clear(&BoundingBox);
	BoundingBox__Include(&BoundingBox, m_topLeft + m_normal);
	BoundingBox__Include(&BoundingBox, m_topRight - m_normal);
	BoundingBox__Include(&BoundingBox, m_bottomLeft - m_normal);
	BoundingBox__Include(&BoundingBox, m_bottomRight + m_normal);
	BoundingSphere.Center = BoundingBox__GetCenter(&BoundingBox);
	BoundingSphere.Radius = (BoundingSphere.Center - BoundingBox.Max).Length();
	m_instance->UpdateBoundingVolumesAccurate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ShatterObject::DestroyModel()
{
	// remove the instance from scene
	if (m_instance)
	{
		SceneMgr::RemoveInstance(m_instance);
		delete m_instance;
		m_instance = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
// create shatter pieces
///////////////////////////////////////////////////////////////////////////////
bool ShatterObject::CreateShards()
{
	// setup the piece size
	float pieceWidth = m_width / (float)m_params.piecesW;
	float pieceHeight = m_height / (float)m_params.piecesH;
	m_pieceSize = 0.5f * (pieceWidth + pieceHeight) / 2.0f;

	// Setup the counts
	int onShard = 0;
	int onVertex = 0;

	// The root of all
	Vector3Packed root;
	if (Weakspots() == 0)
		root = (m_topLeft + m_bottomRight) * 0.5f;
	else
	{
		root.Clear();
		for (int w = 0; w < Weakspots(); w++)
			root += Weakspot(w);
		root /= (float)Weakspots();
	}

	// Do the work
	m_vertexBuffer.Lock();

	FragmentAlongEdge(TOP, m_topLeft, m_topRight, m_params.piecesW, root, onShard, onVertex);
	FragmentAlongEdge(RIGHT, m_topRight, m_bottomRight, m_params.piecesH, root, onShard, onVertex);
	FragmentAlongEdge(BOTTOM, m_bottomLeft, m_bottomRight, m_params.piecesW, root, onShard, onVertex);
	FragmentAlongEdge(LEFT, m_topLeft, m_bottomLeft, m_params.piecesH, root, onShard, onVertex);

	m_vertexBuffer.UnLock();

	// We are done
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// simple random velocity movement of shatter pieces
///////////////////////////////////////////////////////////////////////////////
void ShatterObject::RunPhysicsSimple(
const Camera &camera,
float deltaT)
{
	// take control of vtxbuff
	m_vertexBuffer.Lock();

	// adjust the positions
	int deadShards = 0;
	for (int i = 0; i < m_shards; i++)
	{
		Shard &shard = m_shard[i];

		// What is the color?
 		Math::MoveToZero(&shard.alpha, deltaT * m_params.alphaFadeRate);
 		if (Math::Zero(shard.alpha))
 			++deadShards;

#ifdef FLATREFLECT
		// Let's do some cheap environment mapping
		Vector2 uv;
		if (shard.hangTime > 0.0f)
			uv.Set(0.0f, 0.0f);
		else
		{
			Vector2 screen = camera.ToScreen(shard.pos) * m_params.environmentWrapFlat;
			uv.Set(Math::Frac(screen.X()), Math::Frac(screen.Y()));
		}
#endif
		u32 color = VtxBuffer::cvtRGBA(FastColor(shard.alpha));

		// loop thru verts for this tri
		int vertexBase = i * 3;
		for (int j = 0; j < 3; j++)
		{
			// Rotate a point around an arbitrary axis (axis should be normalized)
			const Vector3 v = shard.pos + RotatePointAroundAxis(shard.voffset[j], shard.rotAxis, shard.rot);

#ifndef FLATREFLECT
			// Let's do some cheap environment mapping
			Vector2 screen = camera.ToScreen(v) * m_params.environmentWrap;
			Vector2 uv(Math::Frac(screen.X()), Math::Frac(screen.Y()));
#endif

			// update the draw vertex
			m_vertexBuffer.Vertex(vertexBase + j).Set(v);
			if (m_params.environmentMap)
				m_vertexBuffer.TexCoordv2(m_envTexture, vertexBase + j) = uv;
 			m_vertexBuffer.Diffuse(vertexBase + j) = color;
		}

		// Just fall
		Vector3 forces = Vector3(0.0f, m_params.gravity, 0.0f);
		if (shard.hangTime > 0.0f)
			shard.hangTime -= deltaT;
		else
		{
			shard.velocity += forces * deltaT;
			shard.pos      += shard.velocity * deltaT;
			shard.rot		+= shard.rotVel * deltaT;
		}
	}

	// we are done with the vert buffer for now
	m_vertexBuffer.UnLock();

	// A simple check
	if (deadShards == m_shards)
		m_ttl = -1.0f;
}

///////////////////////////////////////////////////////////////////////////////
// simple random velocity movement of shatter pieces
///////////////////////////////////////////////////////////////////////////////
void ShatterObject::RunPhysicsCollide(
const Camera &camera,
float deltaT)
{
	// take control of vtxbuff
	m_vertexBuffer.Lock();

	// adjust the positions
	int deadShards = 0;
	for (int i = 0; i < m_shards; i++)
	{
		Shard &shard = m_shard[i];

		// What is the color?
		if (shard.atRest)
		{
			Math::MoveToZero(&shard.alpha, deltaT * m_params.alphaFadeRate);
			if (Math::Zero(shard.alpha))
				++deadShards;
		}

#ifdef FLATREFLECT
		// Let's do some cheap environment mapping
		Vector2 screen = camera.ToScreen(shard.pos) * m_params.environmentWrapFlat;
		Vector2 uv(Math::Frac(screen.X()), Math::Frac(screen.Y()));
#endif
		u32 color = VtxBuffer::cvtRGBA(FastColor(shard.alpha));

		// loop thru verts for this tri
		int vertexBase = i * 3;
		for (int j = 0; j < 3; j++)
		{
			// Rotate a point around an arbitrary axis (axis should be normalized)
			const Vector3 v = shard.pos + RotatePointAroundAxis(shard.voffset[j], shard.rotAxis, shard.rot);

#ifndef FLATREFLECT
			// Let's do some cheap environment mapping
			Vector2 screen = camera.ToScreen(v) * m_params.environmentWrap;
			Vector2 uv(Math::Frac(screen.X()), Math::Frac(screen.Y()));
#endif

			// update the draw vertex
			m_vertexBuffer.Vertex(vertexBase + j).Set(v);
			if (m_params.environmentMap)
				m_vertexBuffer.TexCoordv2(m_envTexture, vertexBase + j) = uv;

			if (shard.atRest)
				m_vertexBuffer.Diffuse(vertexBase + j) = color;
		}

	  	// update triangles position
		if (!shard.atRest)
		{
			Vector3 forces = Vector3( 0.0f, m_params.gravity, 0.0f );
			Vector3 lastPos = shard.pos;
			if (shard.hangTime > 0.0f)
				shard.hangTime -= deltaT;
			else
			{
				shard.velocity += forces * deltaT;
				shard.pos      += shard.velocity * deltaT;
				shard.rot		+= shard.rotVel * deltaT;
			}

			// What is our energy, are we at rest?
			if (shard.velocity.y() < 0.0f)
			{
				// if we hit ground (while moving down)
				Vector3 start = lastPos;
				start.Y(start.Y() + m_pieceSize);

				Vector3 end = shard.pos;
				end.Y(end.Y() - m_pieceSize);

				Vector3 groundPos, groundNormal;
				bool hit = FindGroundFast(start, end, groundPos, groundNormal);
				if (hit)
				{
					float penetrationDepth = Math::Max(0.0f, shard.pos.Y() - m_pieceSize - groundPos.Y());
					shard.pos.y(groundPos.y() + m_pieceSize + m_params.groundTweak);

					if (shard.velocity.LengthSquared() < m_params.atRestThresholdSquared)
						shard.atRest = true;
					else
					{
						shard.velocity.y(shard.velocity.y() - Math::Sqrt(2.0f * -m_params.gravity * penetrationDepth));
						float speed = shard.velocity.Length();
						float volume = Math::Clamp(speed * m_params.speedToVolume);
						if (volume > 0.01f)
							Sfx::PlayStationary(m_params.shardHitSound, shard.pos, volume, 0.0f, NULL);

						//	Compute reflection vector
						Vector3 v = -shard.velocity / speed;
						Vector3 r = groundNormal * (2.0f * Vector3::Dot(groundNormal, v)) - v;
						shard.velocity = r * (speed * m_params.bounceSpeedFactor);

						// Twiddle the rotation
						shard.rotVel *= m_params.bounceRotationFactor;
					}
				}
			}
		}
	}

	// we are done with the vert buffer for now
	m_vertexBuffer.UnLock();

	// A simple check
	if (deadShards == m_shards)
		m_ttl = -1.0f;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ShatterObject::FragmentAlongEdge(
EDGE edge,
const Vector3Packed &start,
const Vector3Packed &end,
int pieces,
Vector3Packed &root,
int &onShard,
int &onVertex)
{
	// Precompute a few things
	float pieceT = 1.0f / (float)pieces;
	Vector3Packed endMinusStart = end - start;

	Vector3Packed c = start;
	float t = pieceT;
	for (int v = 1; v <= pieces; v++, t += pieceT)
	{
		Vector3Packed b = c;
		c = start + (endMinusStart * t);
		SplitShard(root, b, c, edge, onShard, onVertex);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ShatterObject::SplitShard(
const Vector3Packed &a,
const Vector3Packed &b,
const Vector3Packed &c,
EDGE edge,
int &onShard,
int &onVertex)
{
	const float minT = m_params.splitMinT;
	const float maxT = m_params.splitMaxT;
	Vector3Packed ab = a + (b - a) * g_random.InRange(minT, maxT);
	Vector3Packed ac = a + (c - a) * g_random.InRange(minT, maxT);

	AddShard(a, ab, ac, NONE, onShard, onVertex);
	if (g_random.CoinToss())
	{
		AddShard(ab, b, ac, NONE, onShard, onVertex);
		AddShard(ac, b, c, edge, onShard, onVertex);
	}
	else
	{
		AddShard(ab, b, c, edge, onShard, onVertex);
		AddShard(ac, ab, c, NONE, onShard, onVertex);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ShatterObject::AddShard(
const Vector3Packed &a,
const Vector3Packed &b,
const Vector3Packed &c,
EDGE edge,
int &onShard,
int &onVertex)
{
	// Add the triangle
	AddTriangle(a, b, c, onVertex);

	// set up our positions
	m_shard[onShard].pos = (a + b + c) / 3.0f;
	m_shard[onShard].voffset[0] = a - m_shard[onShard].pos;
	m_shard[onShard].voffset[1] = b - m_shard[onShard].pos;
	m_shard[onShard].voffset[2] = c - m_shard[onShard].pos;

	// Compute the velocity
	switch (edge)
	{
		case TOP:
			m_shard[onShard].velocity = m_right * g_random.InRange(m_params.topInPlaneVMin, m_params.topInPlaneVMax);
			break;
		case BOTTOM:
			m_shard[onShard].velocity = m_right * g_random.InRange(m_params.bottomInPlaneVMin, m_params.bottomInPlaneVMax);
			break;
		case RIGHT:
			m_shard[onShard].velocity = m_right * g_random.InRange(m_params.rightInPlaneVMin, m_params.rightInPlaneVMax);
			break;
		case LEFT:
			m_shard[onShard].velocity = m_right * g_random.InRange(m_params.leftInPlaneVMin, m_params.leftInPlaneVMax);
			break;
		case NONE:
			m_shard[onShard].velocity = m_right * g_random.InRange(m_params.inPlaneVMin, m_params.inPlaneVMax);
			break;
	}
	m_shard[onShard].velocity += m_normal * (g_random.InRange(m_params.normalPlaneVMin, m_params.normalPlaneVMax) * g_random.Sign());

	// start with initial rotation of zero
	m_shard[onShard].rotAxis = g_random.UnitVector();
	m_shard[onShard].rot = 0.0f;
	m_shard[onShard].rotVel = g_random.NegPos() * g_random.InRange(m_params.rotVelMin, m_params.rotVelMax);

	// Initial state
	m_shard[onShard].atRest = false;

	if (edge != NONE)
		m_shard[onShard].hangTime = g_random.InRange(m_params.minHangTime, m_params.maxHangTime);
	else
		m_shard[onShard].hangTime = 0.0f;
	m_shard[onShard].alpha = m_params.alphaStart;

	++onShard;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ShatterObject::AddTriangle(
const Vector3Packed &a,
const Vector3Packed &b,
const Vector3Packed &c,
int &onVertex)
{
	// Make the strip
	u16 strip[3];

 	const Vector2 environment(0.0f, 0.0f);;

#if COLORDEBUG
	ts_bRGBA testColor[3] = { { 255, 0, 0, 255 }, { 0, 255, 0, 255 }, { 0, 0, 255, 255 } };
	int vertexColor = *((int *)&testColor[g_random.UpToExcludeHigh(3)]);
#else
	// Cast
	int vertexColor = *((int *)&m_vertexColor);
#endif

	// Add the vertices
	m_vertexBuffer.Vertex(onVertex).Set(a);

	m_vertexBuffer.TexCoordv2(m_baseTexture, onVertex).Set(0.0f, 0.0f);
	if (m_envTexture >= 0)
		m_vertexBuffer.TexCoordv2(m_envTexture, onVertex) = environment;
#ifndef PS2
	if (m_bumpTexture >= 0)
		m_vertexBuffer.TexCoordv2(m_bumpTexture, onVertex).Set(0.0f, 0.0f);
#endif

	m_vertexBuffer.Diffuse(onVertex) = VtxBuffer::cvtRGBA(vertexColor);
	strip[0] = onVertex++;

	m_vertexBuffer.Vertex(onVertex).Set(b);

	m_vertexBuffer.TexCoordv2(m_baseTexture, onVertex).Set(1.0f, 0.0f);
	if (m_envTexture >= 0)
		m_vertexBuffer.TexCoordv2(m_envTexture, onVertex) = environment;
#ifndef PS2
	if (m_bumpTexture >= 0)
		m_vertexBuffer.TexCoordv2(m_bumpTexture, onVertex).Set(1.0f, 0.0f);
#endif

	m_vertexBuffer.Diffuse(onVertex) = VtxBuffer::cvtRGBA(vertexColor);
	strip[1] = onVertex++;

	m_vertexBuffer.Vertex(onVertex).Set(c);

	m_vertexBuffer.TexCoordv2(m_baseTexture, onVertex).Set(0.0f, 1.0f);
	if (m_envTexture >= 0)
		m_vertexBuffer.TexCoordv2(m_envTexture, onVertex) = environment;
#ifndef PS2
	if (m_bumpTexture >= 0)
		m_vertexBuffer.TexCoordv2(m_bumpTexture, onVertex).Set(0.0f, 1.0f);
#endif

	m_vertexBuffer.Diffuse(onVertex) = VtxBuffer::cvtRGBA(vertexColor);
	strip[2] = onVertex++;

	// Add the strip
	m_stripBuffer.AddStrip(strip, 3);
}

///////////////////////////////////////////////////////////////////////////////
// Find the ground
///////////////////////////////////////////////////////////////////////////////
bool ShatterObject::FindGroundFast(
const Vector3 &start,
const Vector3 &end,
Vector3 &intersection,
Vector3 &normal)
{
	// Setup the query
	CLineEntity line;
	float l = (end - start).Length();
	line.Line().SetOriginEnd(start, end);
	line.Line().SetDetectionAbility(CLineCollisionObject::DETECT_FRONT_SIDE);
	line.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);

	// Do the query
	g_collisionSystem.ResetStack();
	CLineQuery test;
	if (!test.Test(line))
		return false;

	// Give them back the data
	intersection = test.intersection;
	normal = test.normal;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
float ShatterObject::Height(
const Vector3 &start,
float range)
{
	// Setup the query
	CLineEntity line;
	Vector3 down(0.0f, -1.0f, 0.0f);
	line.Line().SetOriginBasis(start, down);
	line.Line().SetMaxBoundary(range);
	line.Line().SetDetectionAbility(CLineCollisionObject::DETECT_FRONT_SIDE);
	line.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);

	// Do the query
	g_collisionSystem.ResetStack();
	CLineQuery test;
	if (!test.Test(line))
		return range;

	// Give them back the height
	return test.d;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ShatterObject::Axis ShatterObject::MinimumAxis(
const OBB &obb)
{
	const AABB &aabb = obb.GetConstAABB();
	const float x = aabb.maxV.X();
	const float y = aabb.maxV.Y();
	const float z = aabb.maxV.Z();

	if (x < y && x < z)
		return AXIS_X;
	else if (y < x && y < z)
		return AXIS_Y;
	else
		return AXIS_Z;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ShatterObject::ComputeCorners(
const OBB &obb,
Axis minAxis,
Vector3 corner[4])
{
	Vector3 inWorld[8];
	obb.Transform(inWorld);

	switch (minAxis)
	{
		case AXIS_X:
			corner[0] = (inWorld[0] + inWorld[1]) * 0.5f;
			corner[1] = (inWorld[2] + inWorld[3]) * 0.5f;
			corner[2] = (inWorld[6] + inWorld[7]) * 0.5f;
			corner[3] = (inWorld[4] + inWorld[5]) * 0.5f;
			break;

		case AXIS_Y:
			corner[0] = (inWorld[0] + inWorld[4]) * 0.5f;
			corner[1] = (inWorld[1] + inWorld[5]) * 0.5f;
			corner[2] = (inWorld[3] + inWorld[7]) * 0.5f;
			corner[3] = (inWorld[2] + inWorld[6]) * 0.5f;
			break;

		case AXIS_Z:
			corner[0] = (inWorld[0] + inWorld[2]) * 0.5f;
			corner[1] = (inWorld[1] + inWorld[3]) * 0.5f;
			corner[2] = (inWorld[5] + inWorld[7]) * 0.5f;
			corner[3] = (inWorld[4] + inWorld[6]) * 0.5f;
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ShatterOverlord::ShatterOverlord() :
m_type(true)
{
	// Help them out
	ShatterObject::InitializeStatics();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ShatterOverlord::~ShatterOverlord()
{
}

///////////////////////////////////////////////////////////////////////////////
// Create a shatter type
///////////////////////////////////////////////////////////////////////////////
bool ShatterOverlord::CreateType(
const char *typeName)
{
	// We!
	Type *type = new Type(typeName);
	ASSERT(type);
	if (!type)
		return false;

	// Is it valid?
	if (!type->Valid())
	{
		delete type;
		return false;
	}

	m_type.AddTail(type);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Get a type
///////////////////////////////////////////////////////////////////////////////
ShatterObject *ShatterOverlord::Create(
const char *typeName,
ts_bRGBA *colorOverride)
{
	// Find it
	Type *type = m_type.Head();
	while (type)
	{
		if (strcmpi(typeName, type->Name()) == 0)
			return type->GetObject();

		type = type->next;
	}

	// No luck, try to create (this is scary, it could hang)
	if (CreateType(typeName))
		return Create(typeName, colorOverride);
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
	// Update
///////////////////////////////////////////////////////////////////////////////
void ShatterOverlord::Update()
{
	// Do the updates
	float deltaT = g_timer.GetFrameSec();

	Type *type = m_type.Head();
	while (type)
	{
		type->Update(deltaT);
		type = type->next;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ShatterOverlord::Type::Type(const char *name)
{
	// save the name
	ASSERT(strlen(name) < sizeof(m_name));
	strcpy(m_name, name);

	// Load the params
	m_params = CParam<ShatterParams>::GetParam(name, "var\\shatter");
	ASSERT(m_params);

	m_textureDB = NULL;
	m_textureArray = NULL;
	if (m_params && m_params->textureDBL[0])
	{
		// Load the dataset
		t_Handle dblHandle = dblLoad(m_params->textureDBL);
		if (dblHandle != INVALID_HANDLE)
		{
			m_textureDB = dblGetTextures(dblHandle, DEFAULT_CHUNK_KEY_STRING);
			m_textureArray = new TextureArray();
			ASSERT(m_textureArray);
			if (m_textureArray)
				m_textureArray->AddEntireDatabase(m_textureDB);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ShatterOverlord::Type::~Type()
{
	delete m_textureArray;
}

///////////////////////////////////////////////////////////////////////////////
// Allocate one!
///////////////////////////////////////////////////////////////////////////////
ShatterObject *ShatterOverlord::Type::GetObject()
{
	if (!Valid())
		return NULL;

	// Are we full
	ShatterObject *shatter = NULL;
	if (m_active.Items() < m_params->maxActive)
		shatter = new ShatterObject(*m_params, *m_textureArray);
	else
	{
		// Get the oldest one
		shatter = m_active.Head();
		m_active.Unlink(shatter);
	}

	// Put it at the tail
	if (shatter)
		m_active.AddTail(shatter);

	// Weee!
	return shatter;
}

///////////////////////////////////////////////////////////////////////////////
// Update
///////////////////////////////////////////////////////////////////////////////
void ShatterOverlord::Type::Update(
float deltaT)
{
	ShatterObject *object = m_active.Head();
	while (object)
	{
		// Who is next
		ShatterObject *next = object->next;

		// Update this poor soul
		object->Update(deltaT);

		// Are they done?
		if (object->State() == ShatterObject::DONE)
			m_active.Remove(object);

		// Next!
		object = next;
	}
}

