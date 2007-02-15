///////////////////////////////////////////////////////////////////////////////
// Implementations of decal stuff
///////////////////////////////////////////////////////////////////////////////
#ifndef QUADDECAL_H
#define QUADDECAL_H

#if defined(PS2)
#define VTXBUFFER_NEED_INLINES
#endif

#include "Display/Common/VtxBuffer.h"
#include "Display/Common/StripBuffer.h"
#include "Effects/decal/decalemit.h"

template <class PerDecal>
class QuadDecalEmitter : public DecalEmitter
{
public:
	// Construct it!
	inline QuadDecalEmitter(DecalSet &set, const DecalTextureLayer *layer, int layers);
	inline ~QuadDecalEmitter();

	// Set it up!
	inline bool Setup(int maxDecals, int renderBucket, bool overbrighting = false);

	// Clear all instances
	inline virtual void Clear();

	// Create a decal
	inline bool Create(Vector3CRef center, Vector3CRef right, Vector3CRef up,
								float halfWidth, float halfHeight,
								ts_bRGBA color[4], Vector2 uv[4]);

	// Set some defaults
	inline void SetDefaultColor(const ts_bRGBA &c);
	inline void SetDefaultColors(const ts_bRGBA c[4]);
	inline void UseDefaultUV();
	inline void SetDefaultUV(const Vector2 uv[4]);
	inline void GetDefaultUV(Vector2 uv[4])
		{ DefaultUV(uv); }

	// Create a decal using all defaults
	inline bool Create(Vector3CRef center, Vector3CRef right, Vector3CRef up,
								float halfWidth, float halfHeight);

	// Create a decal using some defaults
	inline bool Create(Vector3CRef center, Vector3CRef right, Vector3CRef up,
								float halfWidth, float halfHeight, const ts_bRGBA &color);

	// The render (don't touch this)
	inline void Render(const CInstance *instance, const Graphics4x4 *localToCamera, float zCamDist);

protected:
	// our list of decals
	PerDecal		*m_decalList;
	int			m_head, m_tail;

	inline int NextDecal(int onDecal)
		{ if (++onDecal == m_maxDecals) onDecal = 0; return onDecal; }
	inline int AllocateDecal();
	inline void FreeDecal();

	// You write this, it gets called on successful create
	virtual inline void OnCreate(int decal,
											Vector3CRef center,
											Vector3CRef right, Vector3CRef up,
											float halfWidth, float halfHeight,
											ts_bRGBA color[4], Vector2 uv[4]) {}

	// You write this, gets called per render
	virtual inline void Update(const CInstance &instance, const Graphics4x4 &localToCamera, float zCamDist) {}

	// Create the geometry
	VtxBuffer					m_vertexBuffer;
	StripBuffer	 				m_stripBuffer;
	bool CreateGeometry(bool overbrighting);

	// nice lock unlock
	bool						m_locked;
	inline void Lock()
		{ if (!m_locked) { m_locked = true; m_vertexBuffer.Lock(); } }
	inline void Unlock()
		{ if (m_locked) { m_locked = false; m_vertexBuffer.UnLock(); } }

	// Hide an instance
	inline void Hide(int decal);

	// the defaults system
	ts_bRGBA	m_defaultColor[4];
	Vector2	m_defaultUV[4];
};

// A really Automatic version of the decal emitter with TTL
class SimpleQuadData { public: float dummy; };
class SimpleQuadDecalEmitter : public QuadDecalEmitter<SimpleQuadData>
{
public:
	SimpleQuadDecalEmitter(DecalSet &set, const DecalTextureLayer *layer, int layers) :
		QuadDecalEmitter<SimpleQuadData>(set, layer, layers) {}
	virtual ~SimpleQuadDecalEmitter() {}
};

// Include the implementations
#include "Effects/decal/quaddecal.hpp"

#endif
