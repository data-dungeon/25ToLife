///////////////////////////////////////////////////////////////////////////////
// A decal system

//		A "DecalEmitter" is a subordinate to a DecalSet, and contains a tri strip
//		and vertex buffer used to rendering decals. It provides functionality
//		for instantiating decals and updating them over time. The base class
//		for a DecalEmitter is abstract: the intent is you could do whatever you
//		want including:
//			- animating different attributes of the decal
//			- Make decals of different numbers of vertices
//			- Make custom placement functions
//			- Make camera releative updates.
///////////////////////////////////////////////////////////////////////////////
#ifndef DECALEMIT_H
#define DECALEMIT_H

#include "Layers/LayersStruct.h"
#include "Game/Managers/RenderMgr.h"
#include "Effects/decal/decalset.h"

class StripBuffer;

// Set this up to do multiple layers
class DecalTextureLayer
{
public:
	DecalTextureLayer() {}
	DecalTextureLayer(int index) { Default(index); }

	inline void Default(int index = 0)
	{
		m_index = index;
		m_method = TS_METHOD_MODULATE;
		m_blend = RS_BLENDFUNC_DEFAULT;
		m_clampU = true;
		m_clampV = true;
		m_filter = TS_FILTER_BILINEAR;
	}
	void DefaultGloss(int index);
	void DefaultPostBump(int index);
	void DefaultBump(int index);

	inline uint8 WrapU() const
		{ return m_clampU ? TS_WRAP_CLAMP : TS_WRAP_REPEAT; }
	inline uint8 WrapV() const
		{ return m_clampV ? TS_WRAP_CLAMP : TS_WRAP_REPEAT; }

	// All of these are twiddleable
	int	m_index;		// Index in texture set
	uint8	m_method;	// TS_METHOD_MODULATE, TS_METHOD_GLOSS, etc
	uint8	m_blend;		// RS_BLENDFUNC_DEFAULT, RS_BLENDFUNC_GLOSS, etc
	bool	m_clampU, m_clampV;
	uint8	m_filter;	// TS_FILTER_POINTSAMPLE, TS_FILTER_BILINEAR, etc

	inline DecalTextureLayer &operator=(const DecalTextureLayer &i)
	{
		m_index = i.m_index;
		m_method = i.m_method;
		m_blend = i.m_blend;
		m_clampU = i.m_clampU;
		m_clampV = i.m_clampV;
		m_filter = i.m_filter;
		return *this;
	}
};
#define MAX_DECAL_TEXTURE_LAYERS	8

// A decal model and collection of instances
class DecalEmitter : public GenericDynamicObject
{
public:
	// Construct it!
	DecalEmitter(DecalSet &set, const DecalTextureLayer *layer, int layers);
	virtual ~DecalEmitter();

	// Set it up!
	virtual bool Setup(int maxDecals, int renderBucket, bool overbrighting = false);

	// Max decals we support?
	virtual int MaxDecals()
		{ return m_maxDecals; }

	// How many decals do we have to draw?
	virtual int UsedDecals()
		{ return m_decals; }

	// How many decals are free?
	virtual int FreeDecals()
		{ return m_maxDecals - m_decals; }

	// Clear all instances
	virtual void Clear() = 0;

	// The render (don't touch this)
	virtual void Render(const CInstance *instance, const Graphics4x4 *localToCamera, float zCamDist) = 0;

protected:
	// Our owner
	DecalSet 					&m_set;
	int							m_decals, m_maxDecals;

	// The instance data
	CInstance					*m_instance;
	virtual bool CreateInstance(int renderBucket);
	virtual void DestroyInstance();

	// The scene manager is painful, we must ensure our instance is in scene
	// just before we render the first time
	bool m_addedToScene;
	void EnsureSceneBullshit();

	// Texture information
	DecalTextureLayer			m_textureLayer[MAX_DECAL_TEXTURE_LAYERS];
	int							m_textureLayers;
	u32 							m_texFlags[MAX_DECAL_TEXTURE_LAYERS];
	Matrix4x4 					m_texMatrix[MAX_DECAL_TEXTURE_LAYERS];
	bool							m_hasBump;
	void							SetTFactor();

	// Utility to setup passes on a strip
	void SetupStripTextures(StripBuffer &strip, bool overbright);

	// Create and destory the geometry
	virtual bool CreateGeometry(bool overbrighting) = 0;
	virtual void DestroyGeometry() {}

	// Useful utility
	inline int Color(const ts_bRGBA &c)
		{ return (*((int *)&c)); }

	// Useful texture coords
	static void DefaultUV(Vector2 uv[4]);
};

#endif
