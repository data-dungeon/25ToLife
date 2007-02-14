///////////////////////////////////////////////////////////////////////////////
// Interact instrument
///////////////////////////////////////////////////////////////////////////////
#ifndef IINTERACT_H
#define IINTERACT_H

#include "hud\text.h"
#include "hud\ssprite.h"
#include "hud\ipanel\isprite.h"

///////////////////////////////////////
///////////////////////////////////////
class InteractInstrument : public SpriteInstrument
{
public:
	// Constructor
	InteractInstrument();
	~InteractInstrument() {}

	// Set the font!
	void SetFont(const char *font, float height = -1.0f)
		{	if (font) m_text.SetFont((char *)font); if (height > 0.0f) m_text.SetTextHeight(height); }

	// Give back our extents
	Vector2 Extents();

	// The update (called once per frame)
	void Update(float dt);

private:
	StreamedSprite		m_interact;
	Text					m_text;

	const char *GetInteractDBL(CCompActor *who);
	const char *GetInteractText(CCompActor *who);
};

#endif
