///////////////////////////////////////////////////////////////////////////////
// Sniper scope instrument
///////////////////////////////////////////////////////////////////////////////
#ifndef ISCOPE_H
#define ISCOPE_H

#include "hud\text.h"
#include "hud\ssprite.h"
#include "hud\ipanel\isprite.h"

///////////////////////////////////////
///////////////////////////////////////
class ScopeInstrument : public SpriteInstrument
{
public:
	// Constructor
	ScopeInstrument();
	~ScopeInstrument() {}

	// Set the font!
	void SetFont(const char *font, float height = -1.0f)
		{	if (font) m_range.SetFont((char *)font); if (height > 0.0f) m_range.SetTextHeight(height); }

	// Give back our extents
	Vector2 Extents()
		{ return Vector2(0.0f, 0.0f) ; }

	// The update (called once per frame)
	void Update(float dt);

	// Turn on / off
	void DrawOverlay(bool drawIt)
		{ m_on = drawIt; }

private:
	Sprite							m_overlay[4];
	Sprite							m_vHatch[8];
	Sprite							m_hHatch[10];
	Text								m_range;
	bool								m_on;
	float Range(CCompActor *who);
	bool Dead(CCompActor *who);
};

#endif
