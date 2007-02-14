///////////////////////////////////////////////////////////////////////////////
// VOIP instrument
///////////////////////////////////////////////////////////////////////////////
#ifndef ITALKIE_H
#define ITALKIE_H

#include "hud\ipanel\isprite.h"
#include "hud\ssprite.h"
#include "hud\ipanel\itext.h"

///////////////////////////////////////
///////////////////////////////////////
class TalkieInstrument : public SpriteInstrument
{
public:
	// Constructor
	TalkieInstrument();
	~TalkieInstrument();

	// our extents
	Vector2 Extents();

	// The update (called once per frame)
	void Update(float dt);

private:
	// Load the right sprite up
	enum Layer
	{
		TALKIE_DENY = 0,
		TALKIE_LISTEN,
		TALKIE_TALK,
		TALKIE_LAYERS
	};
	StreamedSpriteArray<TALKIE_LAYERS> m_talk;
	TextInstrument *m_talkerTag;

	void SetupSprites();

	enum TalkState
	{
		DENY = 0,
		LISTEN,
		TALK,
		IDLE,
	};
	TalkState GetTalkState();
};

#endif
