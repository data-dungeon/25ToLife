///////////////////////////////////////////////////////////////////////////////
// Attempt at a generic shattering system
///////////////////////////////////////////////////////////////////////////////
#ifndef SHATTER_H
#define SHATTER_H

#include "Layers/LayersStruct.h"
#include "Game/Database/DynamicObject.h"
#include "EngineHelper/Singleton.h"
#include "container/dllist.h"

#define MAX_WEAKSPOTS 8

struct ShatterParams
{
	// texture dbl for shatter
	char		textureDBL[64];

	// Max active
	int		maxActive;

	// Do we reclaim
	bool		reclaim;

	// Pieces
	int		piecesW, piecesH;

	// Sound to play on start
	char		startSound[32];

	// Base color of shatter
	int		colorR, colorG, colorB, colorA;

	// Time to live for the whole effect
	float		timeToLive;

	// Do we collide?
	bool		collide;
	char		shardHitSound[32];

	// Gravity in m/s squared
	float		gravity;

	// Time constant for alpha fade
	float		alphaStart;
	float		alphaFadeRate;

	// Energy (speed squared) that pieces get marked as "atRest"
	float		atRestThresholdSquared;

	// Conversion factor for mapping speed m/s into sound volume
	float 	speedToVolume;

	// Factor to reduce speed by during a bounce (less than 1)
	float		bounceSpeedFactor;

	// Factor to reduce rotation by during a bounce (less than 1)
	float		bounceRotationFactor;

	// When triangles are split, where along the edge they split [0 - 1]
	float 	splitMinT, splitMaxT;

	// ground tweak
	float		groundTweak;

	// Velocity ranges for parts
	float 	topInPlaneVMin, topInPlaneVMax;
	float 	bottomInPlaneVMin, bottomInPlaneVMax;
	float 	rightInPlaneVMin, rightInPlaneVMax;
	float 	leftInPlaneVMin, leftInPlaneVMax;
	float		inPlaneVMin, inPlaneVMax;
	float		normalPlaneVMin, normalPlaneVMax;
	float		rotVelMin, rotVelMax;

	// Hang time
	float		minHangTime, maxHangTime;

	// environment mapping
	bool		environmentMap;
	float		environmentWrap;
	float		environmentWrapFlat;

	bool		bumpMap;
};

class ShatterObject : public GenericDynamicObject
{
public:
	ShatterObject(const ShatterParams &params, TextureArray &textureArray);
	~ShatterObject();

	// Shatter Funcs
	bool Start(CProtagonist &actor, const Vector3 *weakSpot = NULL, int weakSpots = 0);
	bool Start(const OBB &obb, const Vector3 *weakSpot = NULL, int weakSpots = 0);
	bool Start(const Vector3 corner[4], const Vector3 *weakSpot = NULL, int weakSpots = 0);

	// Update the shatter
	void Update(float deltaT);

	// funcs for rendering
	void Render(const CInstance *instance, const Graphics4x4 *localToCamera, float zCamDist);

	// Our current state
	enum STATE
	{
		READY,
		SHATTERING,
		DONE
	};
	STATE State()
		{ return m_state; }

	// Init the texture matrix
	static void InitializeStatics();

protected:
	// The params
	const ShatterParams		&m_params;
	TextureArray				&m_textureArray;
	int							m_baseTexture;
	int							m_envTexture;
#ifndef PS2
	int							m_bumpTexture;
#endif

	// our state
	STATE							m_state;
	float							m_ttl;

	// Create/destroy the model info
	bool CreateModel();
	void UpdateBounds();
	void DestroyModel();
	bool CreateShards();													// Create the mesh

	// update locations of shatter pieces
	void RunPhysicsSimple(const Camera &camera, float deltaT);
	void RunPhysicsCollide(const Camera &camera, float deltaT);

	// Info about the mesh
	Vector3						m_topLeft, m_bottomRight;
	Vector3						m_topRight, m_bottomLeft;
	float							m_width;								// dimensions
	float							m_height;
	float							m_pieceSize;						// average piece size
	Vector3						m_normal;							// Normal of pane
	Vector3						m_right;								// Right vector of pane
	Vector3						m_up;									// Up vector on pane

	// Shatter tri's
	struct Shard
	{
		Vector3		voffset[3];		// his vert offsets from center
		Vector3		pos;				// his center point

		Vector3		velocity;		// velocity of a piece

		Vector3		rotAxis;			// rotation axis
		float			rot, rotVel;	// rotation and rot velocity
		float			hangTime;		// Hang before falling
		float			alpha;
		bool			atRest;			// At rest
	};

	// The shattering pieces
	Shard							*m_shard;							// our shatter pieces
	int							m_shards;							// Num tri's for the shatter mesh
	enum EDGE
	{
		TOP, BOTTOM, RIGHT, LEFT, NONE
	};
	void FragmentAlongEdge(EDGE edge, const Vector3Packed &start, const Vector3Packed &end, int pieces, Vector3Packed &root, int &onShard, int &onVertex);
	void SplitShard(const Vector3Packed &a, const Vector3Packed &b, const Vector3Packed &c, EDGE edge, int &onShard, int &onVertex);
	void AddShard(const Vector3Packed &a, const Vector3Packed &b, const Vector3Packed &c, EDGE edge, int &onShard, int &onVertex);

	// Weak spot management
	Vector3Packed				m_weakSpot[MAX_WEAKSPOTS];
	int							m_weakSpots;
	inline Vector3Packed &Weakspot(int index)
		{ return m_weakSpot[index]; }
	inline int Weakspots()
		{ return m_weakSpots; }
	inline void DeleteWeakspot(int index)
		{ if (index != m_weakSpots - 1) m_weakSpot[index] = m_weakSpot[m_weakSpots - 1]; --m_weakSpots; }

	// The render data
	VtxBuffer					m_vertexBuffer;
	StripBuffer 				m_stripBuffer;
	CInstance					*m_instance;
	void AddTriangle(const Vector3Packed &a, const Vector3Packed &b, const Vector3Packed &c, int &onVertex);

	// Color stuff
	ts_bRGBA						m_vertexColor;
	inline int FastColor(float alpha)
		{  union { ts_bRGBA c; u32 i; } col; col.c = m_vertexColor; col.c.A = (unsigned char)(Math::Clamp(alpha) * 255.0f); return col.i; }

	// Find the ground
	bool FindGroundFast(const Vector3 &start, const Vector3 &end, Vector3 &intersection, Vector3 &normal);
	float Height(const Vector3 &pos, float range);

	// Utilities for determining shatter plane
	enum Axis
	{
		AXIS_X,
		AXIS_Y,
		AXIS_Z
	};
	Axis MinimumAxis(const OBB &obb);
	void ComputeCorners(const OBB &obb, Axis minAxis, Vector3 corner[4]);

	// Our texture array for fast rendering
	static Matrix4x4 			s_texMatrix[8];

public:
	ShatterObject				*prev, *next;
};

class ShatterOverlord
{
public:
	ShatterOverlord();
	~ShatterOverlord();

	// Create a shatter type
	bool CreateType(const char *typeName);

	// Get a type
	ShatterObject *Create(const char *typeName, ts_bRGBA *colorOverride = NULL);

	// Update
	void Update();

private:
 	class Type
	{
	public:
		Type(const char *paramfile);
		~Type();

		const char *Name() const
			{ return m_name; }

		bool Valid() const
			{ return (m_params != NULL && m_textureArray != NULL); }

		// Allocate one!
		ShatterObject *GetObject();

		// Update
		void Update(float deltaT);

	private:
		char										m_name[32];
		TextureDatabase						*m_textureDB;
		TextureArray							*m_textureArray;
		ShatterParams							*m_params;
		DoublyLinkedList<ShatterObject>  m_active;

	public:
		// We appear in a list
		Type 										*prev, *next;
	};

	// The list of types
	DoublyLinkedList<Type>  m_type;
};

// There can be only one!
DefineSingleton(ShatterOverlord)
#define g_shatter GetSingleton(ShatterOverlord)

#endif
