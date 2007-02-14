///////////////////////////////////////////////////////////////////////////////
// Interact instrument
///////////////////////////////////////////////////////////////////////////////
#ifndef ILOBBED_H
#define ILOBBED_H

#include "hud\text.h"
#include "hud\ssprite.h"
#include "hud\ipanel\isprite.h"

class Lobbed;

#define HUD_SHOW_LOBBED	5

///////////////////////////////////////
///////////////////////////////////////
class LobbedInstrument : public SpriteInstrument
{
public:
	// Constructor
	LobbedInstrument();
	~LobbedInstrument() {}

	// Set the font!
	void SetFont(const char *font, float height = -1.0f)
		{	if (font) m_text.SetFont((char *)font); if (height > 0.0f) m_text.SetTextHeight(height); }

	// Fix our extents
	Vector2 Extents();

	// The update (called once per frame)
	void Update(float dt);

private:
	int												m_lastLobbed;
	StreamedSpriteArray<HUD_SHOW_LOBBED>	m_lobbed;
	Text												m_text;
	Sprite											m_throwStrength;

	Lobbed *GetLobbed(CCompActor *who);
	void HandleWeaponChange(Lobbed *lobbed);
	void UpdateThrowStrength(Lobbed *lobbed);
	void Setup(float textWidth);
	bool GetDBLName(Lobbed *lobbed, char *dblName);
};

#endif
