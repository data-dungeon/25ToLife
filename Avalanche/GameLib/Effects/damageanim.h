///////////////////////////////////////////////////////////////////////////////
// Damage texture animator
///////////////////////////////////////////////////////////////////////////////
#ifndef DAMAGEANIM_H
#define DAMAGEANIM_H

#include "Layers/LayersStruct.h"

// forwards
class TextureAnimator;

#define MAX_DAMAGEANIM_STAGES		4

class DamageAnimator
{
public:
	DamageAnimator();

	// Initialize
	void SetInstance(CInstance &instance);

	// Set a UV Scale for a stage
	void SetUVScale(float uScale, float vScale);
	void SetUVScale(int stage, float uScale, float vScale);

	// What stage we on? -1 means uninitialized
	int Stage()
		{ return m_stage; }

	// Set the stage (clamps to max stage)
	void SetStage(int stage);

	// Advance to the next stage (if possible)
	void AdvanceStage()
		{ SetStage(m_stage + 1); }

	// Reset the state
	void Reset();

protected:
	CInstance 					*m_instance;
	int							m_stage;
	Vector2						m_uvScale[MAX_DAMAGEANIM_STAGES];

	// Call this once per frame
	void Update(int wantStage);

	inline int TextureAnimators();
	inline TextureAnimator *GetTextureAnimator(int index);;
};

#endif
