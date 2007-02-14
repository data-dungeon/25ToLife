///////////////////////////////////////////////////////////////////////////////
// The marquee!!
///////////////////////////////////////////////////////////////////////////////
#ifndef IMARQUEE_H
#define IMARQUEE_H

#include "hud\text.h"
#include "hud\ipanel\isprite.h"
#include "avatar\avcontrolmap.h"

///////////////////////////////////////
///////////////////////////////////////
class MarqueeInstrument : public SpriteInstrument
{
public:
	// Constructor
	MarqueeInstrument();
	~MarqueeInstrument() {}

	// Overried extents
	Vector2 Extents()
		{ return Vector2(0.0f, 0.0f); }

	// The update (called once per frame)
	void Update(float dt);

	// Twiddle the message
	void ShowMessage(const char *message, float duration, bool pause, bool override);
	void HideMessage();

	// Translate the message
	static	int Translate(const char *source, char *translated, int maxLen);

private:
	Text			m_text;
	Text			m_skip;
	Sprite		m_blackBox;
	float			m_textWidth;
	float			m_timeOut;
	bool			m_paused;
	bool			m_override;

	// Check for an unpause
	void RequestPause(bool pause);
	void CheckForUnpause();


	// Some translation stuff
	struct ButtonToFont
	{
		ControllerButton 	button;
		char					charCode;
	};
	static const ButtonToFont	s_buttonToFont[];
	static int 						s_buttonToFonts;
	static char ButtonToCharCode(ControllerButton button);

	static int FindAvatarButton(const char *name, AvatarControlMapper::DIGITALBUTTON &button);
	static int FindVerb( const char *name );
	static ControllerButton ConvertButton(AvatarControlMapper::DIGITALBUTTON button);
};

#endif
