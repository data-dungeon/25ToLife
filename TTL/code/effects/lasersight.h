///////////////////////////////////////////////////////////////////////////////
// Simple system for laser s
///////////////////////////////////////////////////////////////////////////////
#ifndef LASER_H
#define LASER_H

#include "Layers/LayersStruct.h"

class LaserRender;

// What does a tracer look like?
struct LaserRenderParameters
{
	float    rayStartRadius, rayEndRadius;
	float    dotRadius;

	ts_bRGBA	colorStart, colorEnd;
	ts_bRGBA	colorMiss;
	ts_bRGBA	colorDot;

	float		maxRange;
};

// A tracer model and collection of instances
class LaserRender : public GenericDynamicObject
{
public:
	LaserRender(const char *textureFile, LaserRenderParameters &params, int maxInstances = 16);
	~LaserRender();

	// Create an instance of this
	typedef void *Handle;
	Handle CreateInstance();
	void DestroyInstance(Handle handle);

	// Set an overall rangefactor for this laser
	void SetRangeFactor(float rangeFactor)
		{ m_rangeFactor = rangeFactor; }

	// Update the position of this bad boy (automatically ensures it is shown)
	void Update(Handle handle, CActor *shooter, Vector3CRef laserOrigin, Vector3CRef startTrace, Vector3CRef dir, bool startIsOrigin = true, float inflate = 1.0f, bool forceMiss = false);

	// Update the position of this bad boy (automatically ensures it is shown)
	void Update(Handle handle, CActor *shooter, Vector3CRef laserOrigin, Vector3CRef laserEnd, float inflate = 1.0f);

	// Hide this fool
	void Hide(Handle handle);

	// The render
	void Render(const CInstance *instance, const Graphics4x4 *localToCamera, float zCamDist);

private:
	// Our texture database
	t_Handle						m_texture;
	TextureDatabase			*m_textureDB;
	TextureArray				*m_textureArray;
	int							m_textures;

	bool LoadTextures(const char *textureDBL);
	inline Texture *GetTexture(int index)
		{ return (m_textureDB ? m_textureDB->GetTexture(index) : NULL); }
	inline int Textures()
		{ return m_textures; }
	inline TextureArray *GetTextureArray()
		{ return m_textureArray; }

	// Our settings
	int							m_maxInstances;
	int							m_instances;
	float							m_rayRadiusStart, m_rayRadiusEnd;
	float							m_dotRadius;
	int							m_colorStart, m_colorEnd;
	int							m_colorDot;
	int							m_colorMiss;
	float							m_maxRange;
	float							m_rangeFactor;

	// Our geometry
	VtxBuffer					m_rayVertex;
	StripBuffer	 				m_rayStrip;
	VtxBuffer					m_dotVertex;
	StripBuffer	 				m_dotStrip;

	// nice lock unlock
	bool						m_locked;
	inline void Lock()
		{ if (!m_locked) { m_locked = true; m_rayVertex.Lock(); m_dotVertex.Lock(); } }
	void Unlock()
		{ if (m_locked) { m_locked = false; m_rayVertex.UnLock(); m_dotVertex.UnLock(); } }

	CInstance					*m_instance;
	int							m_textureLayers;
	bool BuildGeometry();

	bool							m_addedToScene;
	void EnsureSceneBullshit();

	// Instance management
	struct Instance
	{
		Handle				handle;
		float					inflate;
		Vector3Packed		start;
		Vector3Packed		end;
		Vector3				dir;
		bool					hidden;
		bool					hit;
	};
	Instance		 	 		*m_instanceList;

	// Update an instance for rendering
	bool Update(int instanceIndex, Vector3CRef cameraDir);

	// Hide an instance
	void Hide(int instanceIndex);

	// Hide just the dot (does a lock)
	void HideDot(int instanceIndex);
	void PlaceDot(int instanceIndex, const Vector3Packed &center, const Vector3Packed &up, const Vector3Packed &right);

	// Our texture array for fast rendering
	static Matrix4x4 s_texMatrix[8];
};

#endif
