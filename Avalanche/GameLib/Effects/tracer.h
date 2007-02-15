///////////////////////////////////////////////////////////////////////////////
// Simple system for tracers
///////////////////////////////////////////////////////////////////////////////
#ifndef TRACER_H
#define TRACER_H

#include "Layers/LayersStruct.h"
#include "container/dllist.h"

class TracerManager;

// What does a tracer look like?
struct TracerModel
{
	int		verts;
	int 		textureIndex;

	float    headWidth, tailWidth;
	ts_bRGBA	headColor, tailColor;

	float		minLength, maxLength;
	float		minStartD, maxStartD;

	int		percent;

	float		textureWrap;
	float		wiggleScale;

	int		maxInstances;
	float		ttl;
};

// Some constants
#define UVOMEGA_MAX				16

// A tracer model and collection of instances
class Tracer : public GenericDynamicObject
{
public:
  virtual ~Tracer();

	// Create a new instance
	bool CreateInstanceRay(Vector3CRef muzzle, Vector3CRef dir);
	bool CreateInstanceChord(Vector3CRef muzzle, Vector3CRef hit);

	// The render
	virtual void Render(const CInstance *instance, const Graphics4x4 *localToCamera, float zCamDist);

protected:
	// Our owner
	TracerManager 				&m_parent;
	TracerModel					m_model;

	// Our geometry & textures
	VtxBuffer					m_vertexBuffer;
	StripBuffer	 				m_stripBuffer;
	CInstance					*m_instance;

	// Prep textures
	virtual void LockTextures();
	virtual void UnlockTextures();

	bool							m_geometryCreated;
	virtual bool CreateGeometry(int instances);
	void DestroyGeometry();

	// Instance management
	struct Instance
	{
		Vector3Packed			start;
		Vector3Packed			end;
		Vector3					dir;
		float						death;
		bool						inUse;
	};
	Instance		 	 		*m_instanceList;
	int						*m_uvOmega;
	int AllocateInstance();
	void FreeInstance(int instanceIndex);

	// Create an instance
	virtual void Create(int instanceIndex, const Vector3 &start, const Vector3 &end, const Vector3 &dir);

	// Update an instance for rendering
	virtual bool Update(const Camera &camera, int instanceIndex, float now);

	// Hide an instance
	void Hide(int instanceIndex);

	// utilities
	static inline int ConvertColor(const Vector4 &c)
		{ ts_bRGBA color; color.R = (u8)c.X(); color.G = (u8)c.Y(); color.B = (u8)c.Z(); color.A = (u8)c.W(); return *((int *)&color); }
	static inline Vector4 ConvertColor(const ts_bRGBA &c)
		{	Vector4 v((float)c.R, (float)c.G, (float)c.B, (float)c.A); return v; }
	static inline Vector2 Lerp(const Vector2 &a, const Vector2 &b, float t)
		{ return a + ((b - a ) * t); }
	static inline Vector3 Lerp(const Vector3 &a, const Vector3 &b, float t)
		{ return a + ((b - a ) * t); }
	static inline Vector4 Lerp(const Vector4 &a, const Vector4 &b, float t)
		{ Vector4 l(Lerp(a.X(), b.X(), t), Lerp(a.Y(), b.Y(), t), Lerp(a.Z(), b.Z(), t), Lerp(a.W(), b.W(), t)); return l; }
	static inline float Lerp(float a, float b, float t)
		{ return a + (b - a) * t; }

	// Some hacking
	static inline Vector2 RandomUVOffset(int omega, float scale)
		{ return s_offsetTable[omega & (UVOMEGA_MAX - 1)] * scale; }

	// Our texture array for fast rendering
	static Vector2 		s_offsetTable[UVOMEGA_MAX];
	static Matrix4x4 		s_texMatrix[8];
	static bool 			s_builtStatics;

	// Do it!
	Tracer(TracerManager &manager, TracerModel &model);
	friend class TracerManager;

public:
	// We appear in a list
	Tracer		  				*prev, *next;
};

class TracerManager
{
public:
	~TracerManager();

	// Initialize and terminate the system
	static bool Create(const char *textureDBL);
	static void Destroy();

	// Create a tracer model
	Tracer *New(TracerModel &init, bool distortion);

	// Access our information
	Texture *GetTexture(int index)
		{ return (m_textureDB ? m_textureDB->GetTexture(index) : NULL); }
	int Textures()
		{ return m_textures; }
	TextureArray *GetTextureArray()
		{ return m_textureArray; }

private:
	// Private constructor
	TracerManager(const char *textureDBL);

	// Our texture database
	t_Handle							m_texture;
	TextureDatabase				*m_textureDB;
	TextureArray					*m_textureArray;
	int								m_textures;
};

// We are going to offer a singleton for this
extern TracerManager	*g_tracerManagerPtr;
inline TracerManager &SafePtr(TracerManager *ptr)
{
	ASSERT(ptr);
	return(*ptr);
}
#define g_tracerManager SafePtr(g_tracerManagerPtr)

#endif
