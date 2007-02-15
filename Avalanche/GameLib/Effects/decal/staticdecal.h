///////////////////////////////////////////////////////////////////////////////
// Static decals require a handle and must be deleted.
///////////////////////////////////////////////////////////////////////////////
#ifndef STATICDECAL_H
#define STATICDECAL_H

#include "Effects/decal/decal.h"

// Define this out here so it's not so cumbersome
typedef void *StaticDecalHandle;

// Type safe invalid handle
#define INVALID_STATIC_DECAL_HANDLE ((StaticDecalHandle)NULL)

template <class ExtraData>
class StaticDecalEmitter : public DecalEmitter
{
public:
	// Construct it!
	inline StaticDecalEmitter(DecalSet &set, const DecalTextureLayer *layer, int layers);
	inline virtual ~StaticDecalEmitter();

	// Set the max number of decals
	inline virtual bool Setup(int maxDecals, int renderBucket, bool overbrighting = false);

	// Clear all the decals (be careful, invalidates handles)
	inline void Clear();

	// Create a decal
	inline virtual StaticDecalHandle Create(const Vector3 corner[4], const ts_bRGBA *color, const Vector2 *uv);

	// Set some defaults
	inline void SetDefaultColor(const ts_bRGBA &c);
	inline void SetDefaultColors(const ts_bRGBA c[4]);
	inline void SetDefaultUV(const Vector2 uv[4]);
	inline void UseDefaultUV();
	inline void GetDefaultUV(Vector2 uv[4])
		{ uv[0] = m_defaultUV[0]; uv[1] = m_defaultUV[1]; uv[2] = m_defaultUV[2]; uv[3] = m_defaultUV[3]; }

	// Create a decal using all defaults
	inline virtual StaticDecalHandle Create(const Vector3 corner[4]);

	// Free a decal!
	inline virtual bool Destroy(StaticDecalHandle handle);

	// Update stuff about the decal (pass in four of each, or NULL for no change)
	inline bool Update(StaticDecalHandle handle, const Vector3 *corner, const Vector2 *uv, const ts_bRGBA *color);

	// Hide/show the decal
	inline virtual bool Show(StaticDecalHandle handle, bool show = true);
	inline virtual bool Hide(StaticDecalHandle handle)
		{ return Show(handle, false); }
	inline bool Hidden(StaticDecalHandle handle);

	//////////////////////////////////////////////
	// Spanky utilities to generate creation data
	//////////////////////////////////////////////

	// generate an up if you don't know one
	inline static Vector3 GenerateUp(Vector3CRef normal);

 	// Create the corners from minimal info
	// Returns false if collide is set and the corners are not coplanar
	inline static bool CreateCorners(Vector3 corner[4],
			 		 							Vector3CRef point,
												Vector3CRef normal,
												Vector3CRef up,
												float width,
												float height,
												float heightTweak = 0.05f,
												DecalCoplanarCheck coplanarCheck = DECAL_COPLANARCHECK_NONE);

	// Manually check if you are coplanar
	inline static bool CheckCoplanar(const Vector3 corner[4],
				  								Vector3CRef normal,
												float scanDepth = 0.05f,
												DecalCoplanarCheck coplanarCheck = DECAL_COPLANARCHECK_CHEAP);

	// Build uv's from a rotation angle
	inline static void BuildRotatedUV(float radians, Vector2 uv[4]);

	// Handy utility for turning a single color into 4 (only good for one call, as a static is returned)
	inline static ts_bRGBA *ExpandColor(const ts_bRGBA color);

	// The render (don't touch this)
	inline void Render(const CInstance *instance, const Graphics4x4 *localToCamera, float zCamDist);

protected:
	enum DecalFlags
	{
		DECAL_ALLOCATED = 0x01,
		DECAL_HIDDEN = 0x02
	};

	// our list of decals
	struct PerDecal
	{
	 	int				flags;
		Vector3Packed	corner[4];
		ts_bRGBA			color[4];
		Vector2			uv[4];
		ExtraData		extra;
	};
	PerDecal				*m_decalList;

	// You write this, it gets called on successful create
	virtual void OnCreate(int index, PerDecal &decal) {}

	// The actual work!
	inline int Allocate();
	inline bool Free(int index);
	inline void Hide(int index);
	inline void Commit(int index);

	// Handle utilities
	inline int HandleToIndex(StaticDecalHandle handle)
		{ return (int)handle - 1; }
	inline StaticDecalHandle IndexToHandle(int index)
		{ return (StaticDecalHandle)(index + 1); }
	inline bool Allocated(int index)
		{ return (m_decalList[index].flags & DECAL_ALLOCATED) != 0; }
	inline bool Hidden(int index)
		{ return (m_decalList[index].flags & DECAL_HIDDEN) != 0; }

	// You write this, gets called per render
	virtual void Update(const CInstance &instance, const Graphics4x4 &localToCamera, float zCamDist) {}

	// Create the geometry
	VtxBuffer					m_vertexBuffer;
	StripBuffer	 				m_stripBuffer;
	bool CreateGeometry(bool overbrighting);

	// nice lock unlock
	int					m_locked;
	inline void Lock()
		{ if (m_locked++ == 0) m_vertexBuffer.Lock(); }
	inline void Unlock()
		{ ASSERT(m_locked > 0); if (--m_locked == 0) m_vertexBuffer.UnLock(); }

	// the defaults system
	ts_bRGBA	m_defaultColor[4];
	Vector2	m_defaultUV[4];
};

// A trick, a slight of hand!
struct SimpleDecalData { int dummy; };
class SimpleStaticDecalEmitter : public StaticDecalEmitter<SimpleDecalData>
{
public:
	inline SimpleStaticDecalEmitter(DecalSet &set, const DecalTextureLayer *layer, int layers) :
		StaticDecalEmitter<SimpleDecalData>(set, layer, layers) {}
	~SimpleStaticDecalEmitter() {}
};

// A sample implementation
struct DecalBorn { float timeBorn; };
class BornOnDecalEmitter : public StaticDecalEmitter<DecalBorn>
{
public:
	BornOnDecalEmitter(DecalSet &set, const DecalTextureLayer *layer, int layers);
	virtual ~BornOnDecalEmitter() {}

	// What is the born on date for this thingie?
	float BornOn(StaticDecalHandle handle);

protected:
	// You write this, it gets called on successful create
	virtual void OnCreate(int index, PerDecal &decal);
};

// Do the implementation!
#include "Effects/decal/staticdecal.hpp"

#endif
