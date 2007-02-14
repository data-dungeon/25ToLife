///////////////////////////////////////////////////////////////////////////////
// Song instrument
///////////////////////////////////////////////////////////////////////////////
#ifndef ISONG_H
#define ISONG_H

#include "hud\ipanel\itext.h"
#include "audio/audiosys.h"

///////////////////////////////////////
///////////////////////////////////////
class SongInstrument : public TextInstrument
{
public:
	// Constructor
	SongInstrument();
	~SongInstrument();

	// The extents!
	Vector2 Extents();

	// The update (called once per frame)
	void Update(float dt);

private:
	Jukebox::TrackInfo	*m_trackInfo;
	float						m_upTime, m_alpha;
	Sprite					m_sprite;

	static void NewSong(void *context, Jukebox::TrackInfo *trackInfo);
};

#endif
