///////////////////////////////////////////////////////////////////////////////
// TTL hud singleton
///////////////////////////////////////////////////////////////////////////////
#ifndef GAMEHUD_H
#define GAMEHUD_H

#include "EngineHelper/Singleton.h"
#include "GameHelper/Message.h"
#include "stage/ccompactor.h"

class ReticleInstrument;
class RadarInstrument;
class HealthInstrument;
class ScopeInstrument;
class MarqueeInstrument;
class MultilineTextInstrument;

// Messages we support
#define GAMEHUD_SCOPEOVERLAY		"HUDScopeOverlay"

#define GAMEHUD_PULSEHEALTH		"HUDPulseHealth"
struct GameHUD_PulseHealth
{
	float strength;
	float duration;
};

#define GAMEHUD_SHOWMESSAGE 		"HUDShowMessage"
struct GameHUD_ShowMessage
{
	const char *message;
	float			duration;
	bool			pauseIt;
	bool			override;
};

#define GAMEHUD_SHOWTEAMSTATUS  	"HUDShowTeamStatus"
#define GAMEHUD_SETRESPAWNTIMER	"HUDSetRespawnTimer"
struct GameHUD_SetRespawnTimer
{
	int 	team;
	int 	seconds;
};
#define GAMEHUD_SETTEAMSTATUS		"HUDSetTeamStatus"
struct GameHUD_SetTeamStatus
{
	int 	team;
	int 	seconds;
	int 	totalPlayers;
};

class GameHUD
{
public:
	// Manage the singleton
	static bool Create();
	static bool Destroy();

	GameHUD();
	~GameHUD();

	// Who are we watching?
	void SetSubject(CCompActor *actor)
		{ SetSubject(actor ? actor->GetHandle() : (ActorHandle)INVALID_OBJECT_HANDLE); }
	void SetSubject(ActorHandle actor);
	CCompActor *GetSubject() const
		{ return CCompActor::FromHandle(m_subjectHandle); }
	ActorHandle GetSubjectHandle() const
		{ return m_subjectHandle; }
	bool HaveSubject()
		{ return (m_subjectHandle != (ActorHandle)INVALID_OBJECT_HANDLE); }

	// Create the appropriate panel
	bool CreateSPPanel();
	bool CreateMPPanel();

	// Some direct calls...
	void ShowOverride(bool showOverride)
		{ m_showOverride = showOverride; }
	bool GetShowOverride(bool showOverride)
		{ return m_showOverride; }
	bool Activate(bool activate);
	bool Activated();

	// Call once per frame
	void Update();

	// Wee! everything else you just send a message
	static bool SendMessage(const char *message, void *data, ActorHandle sender = (ActorHandle)INVALID_OBJECT_HANDLE);
	CMessageOwner &GetMessageOwner()
		{ return m_messageOwner; }

private:
	bool 								m_active;
	ActorHandle						m_subjectHandle;
	CMessageOwner					m_messageOwner;
	ReticleInstrument				*m_reticle;
	RadarInstrument				*m_radar;
	HealthInstrument				*m_health;
	ScopeInstrument				*m_scope;
	MarqueeInstrument				*m_marquee;
	MultilineTextInstrument		*m_chat;

	REF_SEND_MESSAGE_HANDLER(MunitionFired, GameHUD) d_munitionFiredHandler;
	REF_BASIC_MESSAGE_HANDLER(MunitionImpactAck, MESSAGE_VALUE, GameHUD) d_munitionImpactAckHandler;
	REF_BASIC_MESSAGE_HANDLER(PulseHealth, MESSAGE_VALUE, GameHUD) d_pulseHealthHandler;
	REF_BASIC_MESSAGE_HANDLER(ScopeOverlay, MESSAGE_VALUE, GameHUD) d_scopeOverlayHandler;
	REF_SEND_MESSAGE_HANDLER(ShowMessage, GameHUD) d_showMessageHandler;
	REF_BASIC_MESSAGE_HANDLER(TextChat, MESSAGE_STRING, GameHUD) d_textChatHandler;
	REF_SEND_MESSAGE_HANDLER(KeyboardTextChat, GameHUD ) d_keyboardTextChatHandler;

	// Create the common panel
	bool CreateCommonPanel();

	// All the stuff for the auto show-state
	bool								m_showOverride;
	bool InFrontEnd();
	bool InCutscene();
	bool InError();
	bool InFade();
	bool PlayerDead();
	bool NonPlayerCamera();
	bool EndOfMatch();
	bool ScreenUp(const char *screenName);

	static const char							*s_font;
	static float								s_fontHeight;
	static const char							*s_smallFont;
	static float								s_smallFontHeight;
};

// There can be only one!
DefineSingleton(GameHUD)
#define g_hud GetSingleton(GameHUD)

#endif
