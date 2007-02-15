///////////////////////////////////////////////////////////////////////////////
// Simple sprite based instrument
///////////////////////////////////////////////////////////////////////////////
#ifndef ISPRITE_H
#define ISPRITE_H

#include "hud\sprite.h"
#include "hud\ipanel\instrument.h"

///////////////////////////////////////
///////////////////////////////////////
class SpriteInstrument : public Instrument
{
public:
	// Constructor
	SpriteInstrument(const char *name);
	virtual ~SpriteInstrument();

	// This is called by the group to set the overall parent
	virtual void SetRoot(Sprite &root)
		{ root.AddChild(&m_sprite); }

	// Create the sprite
	virtual bool SetupSprite(t_Handle textureSet, int index, int cell = 0, int cols = 1, int rows = 1);

	// Show/hide the instrument
	virtual bool Show(bool show = true);
	virtual bool Activate(bool activate = true);

	// For fading in and out
	virtual void SetBaseAlpha(float alpha)
		{ m_sprite.SetFamilyColor(1.0f, 1.0f, 1.0f, alpha); }
	virtual void SetBaseScale(float scale)
		{ m_sprite.SetScale(scale, scale); }
	virtual void SetSpritePriority(int priority)
		{ m_sprite.SetPriority(priority); }

	// Return the XY extent of this instrument (you write this!)
	virtual Vector2 Extents();

	// The parent calls this to set your position
	virtual void Place(float x, float y);

	// The update (called once per frame)
	virtual void Update(float dt) {}

protected:
	Sprite		m_sprite;
};

#endif
