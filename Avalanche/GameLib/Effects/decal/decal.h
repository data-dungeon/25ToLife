///////////////////////////////////////////////////////////////////////////////
// Implementations of decal stuff
///////////////////////////////////////////////////////////////////////////////
#ifndef DECAL_H
#define DECAL_H

#if defined(PS2)
#define VTXBUFFER_NEED_INLINES
#endif

#include "Display/Common/VtxBuffer.h"
#include "Display/Common/StripBuffer.h"
#include "Effects/decal/decalemit.h"

// This is outside the class because it's a template and the
// namespace is a pain in the ass
enum DecalCoplanarCheck
{
	DECAL_COPLANARCHECK_NONE,
	DECAL_COPLANARCHECK_CHEAP,
	DECAL_COPLANARCHECK_FULL
};

template <class PerDecal>
class AutomaticDecalEmitter : public DecalEmitter
{
public:
	// Construct it!
	inline AutomaticDecalEmitter(DecalSet &set, const DecalTextureLayer *layer, int layers);
	inline ~AutomaticDecalEmitter();

	// Set it up!
	inline bool Setup(int maxDecals, int renderBucket, bool overbrighting = false);

	// Clear all instances
	inline virtual void Clear();

	// Will this decal work?
	inline bool CheckCoplanar(Vector3CRef point, Vector3CRef normal, Vector3CRef up,
								float halfWidth, float halfHeight, float heightTweak,
								DecalCoplanarCheck check = DECAL_COPLANARCHECK_CHEAP);

	// Create a decal
	inline bool Create(Vector3CRef point, Vector3CRef normal, Vector3CRef up,
								float halfWidth, float halfHeight, float heightTweak,
								ts_bRGBA color[4], Vector2 uv[4], DecalCoplanarCheck check = DECAL_COPLANARCHECK_NONE);

	// Set some defaults
	inline void SetDefaultHeightTweak(float heightTweak);
	inline void SetDefaultColor(const ts_bRGBA &c);
	inline void SetDefaultColors(const ts_bRGBA c[4]);
	inline void UseDefaultUV();
	inline void SetDefaultUV(const Vector2 uv[4]);
	inline void GetDefaultUV(Vector2 uv[4])
		{ DefaultUV(uv); }

	// Create a decal using all defaults
	inline bool Create(Vector3CRef point, Vector3CRef normal, Vector3CRef up,
								float halfWidth, float halfHeight);

	// Create a decal using all defaults and computed up
	inline bool Create(Vector3CRef point, Vector3CRef normal, float halfWidth, float halfHeight);

	// get an up if you don't know one
	inline Vector3 GenerateUp(Vector3CRef normal);

	// The render (don't touch this)
	inline void Render(const CInstance *instance, const Graphics4x4 *localToCamera, float zCamDist);

protected:
	// our list of decals
	PerDecal		*m_decalList;
	int			m_head, m_tail;

	// Are we checking for complanar contact
	inline bool CheckCoplanar(const Vector3Packed corner[4],
											Vector3CRef normal,
											float heightTweak,
											DecalCoplanarCheck check);

	inline int NextDecal(int onDecal)
		{ if (++onDecal == m_maxDecals) onDecal = 0; return onDecal; }
	inline int AllocateDecal();
	inline void FreeDecal();

	// You write this, it gets called on successful create
	virtual inline void OnCreate(int decal,
											Vector3CRef point, Vector3CRef normal, Vector3CRef up,
											float halfWidth, float halfHeight, float heightTweak,
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

	// Setup a quad
	inline void SetupQuad(int quad, const Vector3Packed v[4], const Vector2 uv[4], const ts_bRGBA color[4]);

	// Hide an instance
	inline void Hide(int decal);

	// the defaults system
	float 	m_defaultHeightTweak;
	ts_bRGBA	m_defaultColor[4];
	Vector2	m_defaultUV[4];
};

// A really Automatic version of the decal emitter with TTL
class TTLDecal { public: float death; };
class TTLDecalEmitter : public AutomaticDecalEmitter<TTLDecal>
{
public:
	TTLDecalEmitter(DecalSet &set, const DecalTextureLayer *layer, int layers, float ttl);
	virtual ~TTLDecalEmitter() {}

protected:
	// Our ttl
	float m_ttl;

	// You write this, it gets called on successful create
	virtual void OnCreate(int decal,
									Vector3CRef center, Vector3CRef normal, Vector3CRef up,
									float halfWidth, float halfHeight, float heightTweak,
									ts_bRGBA color[4], Vector2 uv[4]);

	// Our update
	virtual void Update(const CInstance &instance, const Graphics4x4 &localToCamera, float zCamDist);

private:
	inline bool Dead(float now, const TTLDecal &decal)
		{ return (!Math::Zero(decal.death) && now >= decal.death); }
};

// Include the implementations
#include "Effects/decal/decal.hpp"

#endif
