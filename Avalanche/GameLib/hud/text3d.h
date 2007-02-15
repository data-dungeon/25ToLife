#ifndef __TEXT3D_H__
#define __TEXT3D_H__

// TJC - this is all really a hack to make this work.  Its FAR from
// the correct way to handle it.  The correct way to handle it
// is probably allow the Sprite manager to have some sprites that
// ZTest and some that dont...  But Im lazy. :)

#include "hud/text.h"
#include "Game/Database/DynamicObject.h"
#include "Display/Common/StripBuffer.h"
#include "Display/Common/VtxBuffer.h"

class Text3D : public Text, public InstancedDynamicObject
{
	u32 m_lastHandles;
	StripBuffer *m_strip;
	VtxBuffer *m_vertex;

	void AllocStripAndVtxBuffers(int handles);
	virtual void Render(const CInstance* pInstance, const Graphics4x4* pmLocalToCamera, float fZCamDist);
	Texture* GetDaTexture();

	Vector3 m_position;
	Vector3 m_scale;
	void RegenMatrix()
	{
//		float x,y;
//		GetScreenPosition(x, y);
		mInstance->Matrix().SetTranslation(0.0f, 0.0f, 0.0f); //x * -0.01f, y * -0.01f, 0.0f);
		mInstance->Matrix().CatScale(m_scale.x(), m_scale.y(), m_scale.z());
		mInstance->Matrix().CatTranslation(m_position.x(), m_position.y(), m_position.z());
	}

	bool IsShadowHandleIndex(int index)
	{
		return (index >= GetNumHandles());
	}
	int GetTotalHandles() {
		if(m_state.shadow.enabled)
			return m_Text.length() * 2;
		return m_Text.length();
	}
	t_Handle GetTotalHandle(int index)
	{
		int length = m_Text.length();
		if(m_state.shadow.enabled && index >= length)
		{
			index -= length;
			return GetShadowHandle(index);
		}
		return GetHandle(index);
	}

public:
	//////////////////////////////////////////////////////////////////////////
	// Rendering functions

	Text3D();
	~Text3D();

	void SetWorldPosition(Vector3CRef pos) { m_position = pos; RegenMatrix(); }
	void SetWorldScale(float scale) { m_scale.Set(scale, scale, scale); RegenMatrix(); }

	virtual void Draw(void);
};

#endif // __TEXT3D_H__

