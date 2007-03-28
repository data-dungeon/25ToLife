///////////////////////////////////////////////////////////////////////////////
// The marquee!!
///////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "game/managers/spritemgr.h"
#include "hud/imarquee.h"
#include "GameHelper/Translator.h"

#ifdef DIRECTX_PC
#include "platform/keyctrl.h"
#endif

#define BASE_TEXT_POSITION 	16.0f
#define MARQUEE_BORDERX			64.0f
#define MARQUEE_BORDERY			48.0f
#define MARQUEE_ALPHA			0.5f
#define MAX_TEXT_WIDTH			450.0f

const MarqueeInstrument::ButtonToFont MarqueeInstrument::s_buttonToFont[] =
{
#if !defined (_XBOX)
	{ CB_R1 , 15 },
	{ CB_R2 , 20 },
	{ CB_L1 , 18 },
	{ CB_L2 , 21 },
	{ CB_SQUARE , 19 },
	{ CB_O , 24 },
	{ CB_TRI , 16 },
	{ CB_X , 17 },
	{ CB_RIGHT , 26 },
	{ CB_LEFT , 25 },
	{ CB_UP , 22 },
	{ CB_DOWN , 23 },
	{ CB_STICK1, 22 },
	{ CB_STICK2, 23 },
	{ CB_START , 10 },
	{ CB_SELECT, 10 },
	{ CB_INVALID, 11 }
#else
	{ CB_R1 , 15 },
	{ CB_R2 , 20 },
	{ CB_L1 , 18 },
	{ CB_L2 , 21 },
	{ CB_SQUARE , 19 },
	{ CB_O , 24 },
	{ CB_TRI , 16 },
	{ CB_X , 17 },
	{ CB_RIGHT , 26 },
	{ CB_LEFT , 25 },
	{ CB_UP , 22 },
	{ CB_DOWN , 23 },
	{ CB_STICK1, 22 },
	{ CB_STICK2, 23 },
	{ CB_START , 10 },
	{ CB_SELECT, 10 },
	{ CB_INVALID, 11 }
#endif
};
int MarqueeInstrument::s_buttonToFonts = sizeof(MarqueeInstrument::s_buttonToFont) / sizeof(MarqueeInstrument::ButtonToFont);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MarqueeInstrument::MarqueeInstrument() :
SpriteInstrument("marquee")
{
	t_Handle handle = dblLoad("newhud/marquee.dbl");
	ASSERT(handle != INVALID_HANDLE);

	Font *font = Font::FindFont("GazzRegularB");
	ASSERT(font);

	m_sprite.AddChild(&m_text);
	m_text.SetOrigin(0.5f, 0.0f);
	m_text.SetPosition(0.0f, BASE_TEXT_POSITION);
	m_text.SetPriority(99);
	m_text.SetFont(font);

	m_sprite.AddChild(&m_skip);
	m_skip.SetOrigin(0.5f, 0.0f);
	m_skip.SetPosition(0.0f, BASE_TEXT_POSITION);
	m_skip.SetPriority(99);
	m_skip.SetFont(font);

	m_blackBox.SetTextureSet(handle, 0);
	m_blackBox.Reset();
	m_sprite.AddChild(&m_blackBox);
	m_blackBox.SetOrigin(0.5f, 0.0f);
	m_blackBox.SetPosition(0.0f, BASE_TEXT_POSITION);
	m_blackBox.SetPriority(0);
	m_blackBox.SetAlpha(MARQUEE_ALPHA);
	m_blackBox.Hide();

	m_sprite.SetPriority(100);

	m_timeOut = 0.0f;
	m_paused = false;
	m_override = false;
}

///////////////////////////////////////////////////////////////////////////////
// The update (called once per frame)
///////////////////////////////////////////////////////////////////////////////
void MarqueeInstrument::Update(float dt)
{
	if (m_timeOut > 0.0f)
	{
			m_timeOut -= dt;
		if	(m_timeOut <= 0.0f)
			HideMessage();
	}
	CheckForUnpause();

	// Hide/show
	if (!m_paused || m_hidden || !m_active)
		m_skip.HideAll();
	else
	{
		float height = m_text.ComputeTextHeight();
		m_skip.SetPosition(0.0f, BASE_TEXT_POSITION + height);
		m_skip.ShowAll();
	}

	char *text = m_text.GetText();
	if (!text || !text[0] || m_hidden || !m_active)
		m_blackBox.Hide();
	else
		m_blackBox.Show();

	// Do the rest of the wrok
	SpriteInstrument::Update(dt);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MarqueeInstrument::ShowMessage(
const char *message,
float duration,
bool pause,
bool override)
{
	if (!m_hidden && m_active && m_override && !override)
		return;

	if (!message || !message[0])
		HideMessage();
	else
	{
		// First reset this
		m_text.SetTextNoTranslate("");
		m_text.SetWordWrap(false);
		m_text.SetWidth(0.0f);

		// Translate it!
		char translated[512];

		// Make sure this string is current (based on control config)
		if (pause)
		{
			Translate("Press the \021 button to continue", translated, sizeof(translated) - 1);
			m_skip.SetTextNoTranslate(translated);
		}

		// Do the main message
		if (Translate(message, translated, sizeof(translated) - 1))
		{
			m_text.SetTextNoTranslate(translated);
			if (pause)
				RequestPause(true);
			m_timeOut = duration;
			m_override = override;
		}

		// Handle word wrapping
		m_textWidth = m_text.ComputeTextLength();
		if (m_textWidth > MAX_TEXT_WIDTH)
		{
			m_textWidth = MAX_TEXT_WIDTH;
			m_text.SetWidth(MAX_TEXT_WIDTH);
			m_text.SetWordWrap(true);
		}

		// Size the marquee
		float width = Math::Max(m_textWidth, m_skip.ComputeTextLength()) + 2.0f * MARQUEE_BORDERX;
		float height = m_text.ComputeTextHeight() + 2.0f * MARQUEE_BORDERY;
		if (pause)
			height += m_text.ComputeTextHeight();
		m_blackBox.SetSize(width, height);
		m_blackBox.SetPosition(0.0f, BASE_TEXT_POSITION - MARQUEE_BORDERY);
		m_blackBox.Show();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MarqueeInstrument::HideMessage()
{
	m_timeOut = 0.0f;
	m_skip.ClearText();
	m_text.ClearText();
	m_blackBox.Hide();
	if (m_paused)
		RequestPause(false);
	m_override = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MarqueeInstrument::RequestPause(
bool pause)
{
	if (!m_hidden && m_active)
	{
		SPause pauseMsg;
		pauseMsg.d_type = pause ? SPause::PAUSE : SPause::UNPAUSE;
		pauseMsg.d_menu = NULL;
		pauseMsg.d_affectAudio = true;
		g_messageDispatcher.PostMessageToAll("Pause", (void*)&pauseMsg, INVALID_OBJECT_HANDLE);
		m_paused = pause;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MarqueeInstrument::CheckForUnpause()
{
	if (m_paused)
	{
		bool unpause = false;

#ifdef DIRECTX_PC
		ControllerInputDriver *inputDriver = g_input.GetController(0);
		if (inputDriver && inputDriver->ButtonPressed(CB_X))
			unpause = true;
#else
		if (g_input.ControllerLocked(0))
		{
			ControllerInputDriver *c = g_input.GetController(0);
			if (c && c->ButtonPressed(CB_X))
				unpause = true;
		}
#endif

		if (unpause)
			HideMessage();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int MarqueeInstrument::Translate(
const char *source,
char *translated,
int maxLen)
{
	// language translation
	const char* text = g_translator.Translate(source);
	int	skip;
	char firstPass[ 256 ];
	const char* verbUC = "";
	const char* verbLC = "";
	ControllerButton cb;
   
	// button translation
	//char *dest = translated;
	char *dest = firstPass;
	int destLen = 0;
	while (*text && destLen < maxLen)
	{
		if (*text != '&')
			dest[destLen++] = *text++;
		else
		{
			// Find the button!
			AvatarControlMapper::DIGITALBUTTON button;
			
			// skip over verb keywords to process later. there is only one verb keyword right now so this is fairly hard coded
			// for simplicity.  These keywords will be processed on a second pass based on the type of button substituted
			// during button keyword replacement.
			skip = FindVerb(text + 1);
			if( skip > 0 )
			{
				strcpy( dest+destLen , "&Verb" );
				destLen += 5;
				text += skip+1;
				continue;
			}
			
			skip = FindAvatarButton(text + 1, button);
#ifdef DIRECTX_PC
// VERY HACKISH
			if (skip > 0)
			{
				// for pc, output the name of the key that performs the action
				ControllerInputDriver* controller = g_input.GetController( 0 );
				KeyControllerInputDriver* kcid = static_cast<KeyControllerInputDriver*>( controller );
				if (kcid)
				{
					text += skip + 1;
					const char * keyName = kcid->DigitalButtonToKey((int)button); 
					strcpy(dest+destLen, keyName);
					destLen+= strlen(keyName);
				}
				verbUC = g_translator.Translate("Press");
				verbLC = g_translator.Translate("press #LANG#A");
			}
#else
			if (skip > 0)
			{
				text += skip + 1;
				cb = ConvertButton(button);
#if defined ( _XBOX )
				switch( cb )
				{
					case CB_STICK1:
					case CB_STICK2:
						verbUC = g_translator.Translate("Click");
						verbLC = g_translator.Translate("click #LANG#A");
					break;

					case CB_L1:
					case CB_R1:
					case CB_L2:
					case CB_R2:
						verbUC = g_translator.Translate("Pull");
						verbLC = g_translator.Translate("pull #LANG#A");
					break;

					default:
						verbUC = g_translator.Translate("Press");
						verbLC = g_translator.Translate("press #LANG#A");
					break;
				}
#else
				verbUC = g_translator.Translate("Press");
				verbLC = g_translator.Translate("press #LANG#A");
#endif
				dest[destLen++] = ButtonToCharCode( cb );
			}
#endif
			else
			{
				ASSERTS( false, "Did not find button for message, tell Jeff G." );
				break;
			}

		}
	}
	dest[destLen] = '\0';

	// make a second pass to mark up verbs based on which button was placed
	int len;
	text = dest;
	dest = translated;
	destLen = 0;
   while (*text && destLen < maxLen)
	{
		if (*text != '&')
			dest[destLen++] = *text++;
		else
		{
			skip = FindVerb(text + 1);
			if( skip > 0 )
			{
				const char* translatedVerb = (destLen == 0) ? verbUC : verbLC;
				len = strlen( translatedVerb );
				strcpy( dest+destLen , translatedVerb );
				destLen += len;
				text += skip+1;
			}
		}
	}
	dest[destLen] = '\0';

	return strlen(translated);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
char MarqueeInstrument::ButtonToCharCode(
ControllerButton button)
{
	for (int i = 0; i < s_buttonToFonts; i++)
	{
		if (s_buttonToFont[i].button == button)
			return s_buttonToFont[i].charCode;
	}

	ASSERT(false);
	return 11;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int MarqueeInstrument::FindAvatarButton(
const char *name,
AvatarControlMapper::DIGITALBUTTON &button)
{
	for (int i = 0; i < AvatarControlMapper::DIGITALBUTTONS; i++)
	{
		const char *buttonName = AvatarControlMapper::GetActionName((AvatarControlMapper::DIGITALBUTTON)i);

		int len = strlen(buttonName);
		if (strncmp(name, buttonName, len) == 0)
		{
			button = (AvatarControlMapper::DIGITALBUTTON)i;
			return len;
		}
	}

	// Button not found
	return -1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int MarqueeInstrument::FindVerb( const char *name )
{
	int len = 4;
	if (strncmp(name, "Verb" , 4) == 0)
	{
		return len;
	}

	// Button not found
	return -1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ControllerButton MarqueeInstrument::ConvertButton(
AvatarControlMapper::DIGITALBUTTON button)
{
	AvatarControlMapper *mapper = g_players.GetMapper(0, false);
	if (!mapper)
		return CB_INVALID;
	return mapper->GetButton(button);
}
