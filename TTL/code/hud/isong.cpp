///////////////////////////////////////////////////////////////////////////////
// Simple text based instrument
///////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "hud/isong.h"

#define UP_TIME			8.0f
#define BASE_ALPHA		0.75f
#define FADE_RATE			(1.0f / 5.0f)
#define SPRITE_XOFFSET	4.0f

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SongInstrument::SongInstrument() :
TextInstrument("song")
{
	g_audio.jukebox.SetNewTrackCallback(NewSong, this);
	m_trackInfo = NULL;
	m_upTime = 0.0f;
	m_alpha = 0.0f;

	// Handle the sprite
	t_Handle handle = dblLoad("newhud/song.dbl");
	ASSERT(handle != INVALID_HANDLE);

	m_sprite.SetTextureSet(handle, 0);
	m_sprite.Reset();
	m_sprite.SetOrigin(1.0f, 0.5f);

	m_text.AddChild(&m_sprite);
	m_text.SetOrigin(0.0f, 1.0f);

	m_text.SetPriority(50);
	m_sprite.SetPriority(50);

	m_sprite.Hide();
	m_text.HideAll();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SongInstrument::~SongInstrument()
{
	g_audio.jukebox.ClearNewTrackCallback();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Vector2 SongInstrument::Extents()
{
	return Vector2(m_sprite.GetWidth() + m_text.ComputeTextLength(),
						Math::Max(m_sprite.GetHeight(), m_text.ComputeTextHeight()));
							
}

///////////////////////////////////////////////////////////////////////////////
// The update (called once per frame)
///////////////////////////////////////////////////////////////////////////////
void SongInstrument::Update(float dt)
{
	if (m_hidden || !m_active && Math::Zero(m_alpha))
		m_text.HideAll();
	else
	{
		Math::MoveToZero(&m_upTime, dt);
		if (Math::Zero(m_upTime))
			Math::MoveToZero(&m_alpha, dt * FADE_RATE);

		m_text.SetAlpha(m_alpha);
		m_sprite.SetAlpha(m_alpha);

		float pulse = 1.0f + 0.1f * Math::Cos(g_timer.GetEndSec() * 7.0f);
		m_sprite.SetScale(pulse, pulse);
	}

	// New track?
	if (m_trackInfo)
	{
		char song[256];

		sprintf(song, "%s\n%s", m_trackInfo->artist, m_trackInfo->song);

		SetText(song);

		float spriteWidth = m_sprite.GetWidth() + SPRITE_XOFFSET;	
		m_text.SetPosition(m_pos.X() + spriteWidth, m_pos.Y());		
		float height = m_text.ComputeTextHeight();
		m_sprite.SetPosition(-SPRITE_XOFFSET, height * 0.5f);
	
		m_text.ShowAll();
		m_sprite.Show();

		m_trackInfo = NULL;
		m_upTime = UP_TIME;
		m_alpha = BASE_ALPHA;
	}

	// Do the rest of the wrok
	TextInstrument::Update(dt);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SongInstrument::NewSong(
void *context,
Jukebox::TrackInfo *trackInfo)
{
	SongInstrument *songI = (SongInstrument *)context;
	ASSERT(songI);
	if (songI)
		songI->m_trackInfo = trackInfo;
}
