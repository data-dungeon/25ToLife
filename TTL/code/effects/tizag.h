///////////////////////////////////////////////////////////////////////////////
// Simple system for tracers
///////////////////////////////////////////////////////////////////////////////
#ifndef TIZAG_H
#define TIZAG_H

#include "Layers/LayersStruct.h"
#include "effects/decal/decalset.h"

// A tracer model and collection of instances
class Tizag : public GenericDynamicObject
{
public:
	Tizag(DecalSet &decalSet, int textureIndex, int xPoints = 12, int yPoints = 8, bool useSceneMgr = true);
  ~Tizag();

	// Create a new instance (does a show)
	bool Setup(const Vector3 &pos,
					const Vector3 &normal,
					const Vector3 &up,
					float width, float height,
					float paintTime, float finalAlpha);

	// Show hide
	void Show(bool show = true)
		{ m_visible = false; }
	void Hide()
		{ Show(false); }
	bool Visible()
		{ return m_visible; }

	// Get at the instance
	CInstance *GetInstance()
		{ return m_instance; }

	// Set a new position!
	bool SetPosition(const Vector3 &pos);
	bool ShiftOffset(float newOffset);

	// The render
	void Render(const CInstance *instance, const Graphics4x4 *localToCamera, float zCamDist);

	bool IsFading()
		{ return !Math::Zero(m_fadeRate); }
	bool DoneFading()
		{ return (Visible() && !IsFading()); }

	// Change some stuff
	void ChangeAlphaTarget(float newTarget)
		{ m_fadeTarget = newTarget; }
	void ChangePaintTime(float newPaintTime);

protected:
	DecalSet 					&m_decalSet;
	int 							m_textureIndex;
	int							m_xPoints, m_yPoints;
	bool 							m_useSceneMgr;

	// Are overall state
	bool							m_visible;

	// Our geometry & textures
	VtxBuffer					m_vertexBuffer;
	StripBuffer	 				m_stripBuffer;
	CInstance					*m_instance;
	Vector3						m_normal;

	bool							m_geometryCreated;
	bool CreateGeometry();
	void DestroyGeometry();

	// The fade
	int							m_fadeX, m_fadeY;
	float							m_fadeAlpha;
	float							m_fadeTarget;
	float							m_fadeRate;
	void ClearFade();
	void StartFade(float fadeTime, float finalAlpha);
	void UpdateFade();

	// Utilities
	int VertexIndex(int x, int y);
	Vector2 GetUV(int x, int y);
	void SetVertexAlpha(int x, int y, float alpha)
		{ SetVertexAlpha(VertexIndex(x, y), alpha); }
	void SetVertexAlpha(int v, float alpha);

	// Our texture array for fast rendering
	static Matrix4x4 		s_texMatrix[8];
	static bool 			s_builtStatics;
};

#endif
