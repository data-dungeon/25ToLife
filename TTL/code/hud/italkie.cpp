///////////////////////////////////////////////////////////////////////////////
// Simple text based instrument
///////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "voice/netz/cvoicechat.h"
#include "hud/italkie.h"
#include "NetZTTL/DOPlayerAgent.h"
#define FORCE_SIZE		64.0f

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TalkieInstrument::TalkieInstrument() :
SpriteInstrument("talkie")
{
	// Setup the sprites (no textures yet)
	m_talk.SetTextureDB("newhud/talkie.dbl");
	for (unsigned int t = 0; t < TALKIE_LAYERS; t++)
		m_sprite.AddChild(&m_talk.GetSprite(t));

	m_talkerTag = new TextInstrument("talkie");
	ASSERT( m_talkerTag );
	m_talkerTag->SetText(NULL);
	m_talkerTag->Show(false);
	m_talkerTag->SetRoot(m_sprite);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TalkieInstrument::~TalkieInstrument()
{
	if (m_talkerTag)
	{
		delete m_talkerTag;
		m_talkerTag = NULL;
	}
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Vector2 TalkieInstrument::Extents()
{
	return Vector2(FORCE_SIZE, FORCE_SIZE);
}

///////////////////////////////////////////////////////////////////////////////
// The update (called once per frame)
///////////////////////////////////////////////////////////////////////////////
void TalkieInstrument::Update(float dt)
{
	// Allows this to process
	if (m_talk.Process() == SSS_INITIALIZE)
		SetupSprites();

	// are we hidden?
	if (!m_hidden && m_active && m_talk.PeekState() == SSS_READY)
	{
		int state = (int)GetTalkState();
		for (int t = 0; t < TALKIE_LAYERS; t++)
		{
			if (state == t)
				m_talk.GetSprite(t).Show();
			else
				m_talk.GetSprite(t).Hide();
		}
	}
#if defined(NETWORKING)
	// Show gamer tag of whoever happens to be talking 
	m_talkerTag->Show(false);
	DOHandle handle = g_CVoiceChat.getWhosTalking();
	if (handle != INVALID_DOHANDLE)
	{
		ASSERT(handle.IsAKindOf(DOCLASSID(DOTalker)));
		DOTalker::Ref refTalker(handle);
		if (refTalker.IsValid())
		{
			DOPlayerAgent *pAgent = ( DOPlayerAgent * )*refTalker;
			if (pAgent)
			{
				const char *pName = pAgent->GetDisplayName();
				m_talkerTag->SetText(pName);
				m_talkerTag->Show(true);
				m_talkerTag->Place(0.0f, 64.0f);
			}
		}
	}
	m_talkerTag->Update(dt);
#endif
	SpriteInstrument::Update(dt);
}

///////////////////////////////////////////////////////////////////////////////
// Load the right sprite up
///////////////////////////////////////////////////////////////////////////////
void TalkieInstrument::SetupSprites()
{
	// Setup the layers
	for (unsigned int t = 0; t < TALKIE_LAYERS; t++)
	{
		Sprite &sprite = m_talk.GetSprite(t);
		sprite.SetOrigin(0.0f, 0.0f);
		sprite.SetSize(FORCE_SIZE, FORCE_SIZE);
		sprite.Hide();
	}
	m_talkerTag->Place(0.0f, 64.0f);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TalkieInstrument::TalkState TalkieInstrument::GetTalkState()
{
#if 1 && defined(NETWORKING)
	switch (g_CVoiceChat.getState())
	{
		case CVoiceChat::STATE_CHAT_TALKING:
			return TALK;
		case CVoiceChat::STATE_CHAT_DENY:
			return DENY;
		case CVoiceChat::STATE_CHAT_LISTEN:
			return LISTEN;
	}

	return IDLE;
#else
	return (TalkState)((g_timer.GetEndMS() / 400) % 4);
#endif
}

//#undef so the SN pch won't take a shit
#undef FORCE_SIZE
