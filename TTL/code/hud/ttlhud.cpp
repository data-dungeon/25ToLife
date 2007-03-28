///////////////////////////////////////////////////////////////////////////////
// TTL hud singleton
///////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"
#include "GameHelper/TextChat.h"
#include "main/Lobby.h"
#include "hud/ihealth.h"
#include "hud/itimer.h"
#include "hud/iweap.h"
#include "hud/ilobbed.h"
#include "hud/ipanel/imultiline.h"
#include "hud/ireticle.h"
#include "hud/iradar.h"
#include "hud/iinteract.h"
#include "hud/iscope.h"
#include "hud/imarquee.h"
#include "hud/iplayernames.h"
#include "hud/ispawntickets.h"
#include "hud/italkie.h"
#include "hud/isong.h"
#include "hud/icheat.h"
#include "hud/imatspy.h"
#include "hud/iactorspy.h"
#include "hud/inetstat.h"
#include "hud/ipanel/isys.h"
#include "interfacesTTL/healthIF.h"
#include "frontend/frontend.h"
#include "environ/ConfigMgr.h"
#include "effects/lensflare.h"
#include "avatar/avatarfx.h"
#include "hud/ttlhud.h"
#include "Voice/NetZ/CVoiceChat.h"

DeclareSingleton(GameHUD);

const char						*GameHUD::s_font = NULL;
float								GameHUD::s_fontHeight = 28.0f;
const char						*GameHUD::s_smallFont = NULL;
float								GameHUD::s_smallFontHeight = 20.0f;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool GameHUD::Create()
{
	ASSERT(!ValidSingleton(GameHUD));
	NewSingleton(GameHUD);
	return ValidSingleton(GameHUD);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool GameHUD::Destroy()
{
	if (!ValidSingleton(GameHUD))
		return false;

	DeleteSingleton(GameHUD);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GameHUD::GameHUD() :
m_messageOwner("GameHUD")
{
	m_active = true;
	m_subjectHandle = (ActorHandle)INVALID_OBJECT_HANDLE;

	// Setup our message handlers
	d_munitionFiredHandler.Initialize(MUNITION_FIRED, MESSAGE_HANDLE_IF_MASTER, m_messageOwner, this);
	d_munitionImpactAckHandler.Initialize(MUNITION_IMPACT_ACK, MESSAGE_HANDLE_IF_MASTER, m_messageOwner, this);
	g_messageDispatcher.Unregister(&d_munitionImpactAckHandler);
	d_pulseHealthHandler.Initialize(GAMEHUD_PULSEHEALTH, MESSAGE_HANDLE_IF_MASTER, m_messageOwner, this);
	d_scopeOverlayHandler.Initialize(GAMEHUD_SCOPEOVERLAY, MESSAGE_HANDLE_IF_MASTER, m_messageOwner, this);
	d_showMessageHandler.Initialize(GAMEHUD_SHOWMESSAGE, MESSAGE_HANDLE_IF_MASTER, m_messageOwner, this);
	d_textChatHandler.Initialize("TextChat", MESSAGE_HANDLE_IF_MASTER, m_messageOwner, this);
	d_keyboardTextChatHandler.Initialize(KEYBOARD_TEXT_CHAT, MESSAGE_HANDLE_IF_MASTER, m_messageOwner, this);

	// We talk directly to some intruments
	m_reticle = NULL;
	m_radar = NULL;
	m_health = NULL;
	m_scope = NULL;
	m_marquee = NULL;
	m_chat = NULL;

	m_showOverride = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GameHUD::~GameHUD()
{
	g_instruments.Remove("AvatarHUD");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameHUD::SetSubject(
ActorHandle actor)
{
	if (actor == m_subjectHandle)
		return;

	// Remove the old snoopers
	if (m_subjectHandle)
	{
		g_messageDispatcher.Unregister(&d_munitionImpactAckHandler);
	}

	// Save it
	CCompActor *subject = CCompActor::FromHandle(actor);
	if (!subject)
		m_subjectHandle = (ActorHandle)INVALID_OBJECT_HANDLE;
	else
	{
		// Save it!
		m_subjectHandle = actor;

		// Setup the snooper
		g_messageDispatcher.Register(&d_munitionImpactAckHandler, MUNITION_IMPACT_ACK, subject->GetMessageOwner());
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool GameHUD::CreateCommonPanel()
{
  InstrumentPanel *panel = g_instruments.Create("AvatarHUD");
	if (!panel)
		return false;

	// Setup the transition for all the groups
	panel->SetDefaultTransition(0.33f);

	m_health = new HealthInstrument;
	ASSERT(m_health);
	if (m_health)
		panel->Add(InstrumentPanel::BOTTOMLEFT, InstrumentCluster::CENTER, "player", m_health);

	WeaponInstrument *weapon = new WeaponInstrument;
	ASSERT(weapon);
	if (weapon)
	{
		weapon->SetFont(s_smallFont, s_smallFontHeight);
		panel->Add(InstrumentPanel::BOTTOMRIGHT, InstrumentCluster::CENTER, "player", weapon);
	}

	LobbedInstrument *lobbed = new LobbedInstrument;
	ASSERT(lobbed);
	if (lobbed)
	{
		lobbed->SetFont(s_smallFont, s_smallFontHeight);;
		panel->Add(InstrumentPanel::BOTTOMRIGHT, InstrumentCluster::NORTH, "player", lobbed);
	}

	m_reticle = new ReticleInstrument;
	ASSERT(m_reticle);
	if (m_reticle)
		panel->Add(InstrumentPanel::CENTER, InstrumentCluster::CENTER, "player", m_reticle);

	InteractInstrument *interact = new InteractInstrument;
	ASSERT(interact);
	if (interact)
	{
		interact->SetFont(s_font, s_fontHeight);
		panel->Add(InstrumentPanel::BOTTOMCENTER, InstrumentCluster::CENTER, "player", interact);
	}

	m_scope = new ScopeInstrument;
	ASSERT(m_scope);
	if (m_scope)
	{
		m_scope->SetFont(s_smallFont, s_smallFontHeight);
		panel->Add(InstrumentPanel::CENTER, InstrumentCluster::CENTER, "player", m_scope);
	}

	m_marquee = new MarqueeInstrument;
	ASSERT(m_marquee);
	if (m_marquee)
		panel->Add(InstrumentPanel::TOPCENTER, InstrumentCluster::CENTER, "game", m_marquee);

	SongInstrument *song = new SongInstrument;
	ASSERT(song);
	if (song)
	{
		song->SetFont(s_smallFont, s_smallFontHeight);;
  		panel->Add(InstrumentPanel::BOTTOMLEFT, InstrumentCluster::EAST, "game", song);
	}

#if !defined(CONSUMER_BUILD)
	MaterialSpyInstrument *matSpy = new MaterialSpyInstrument;
	ASSERT(matSpy);
	if (matSpy)
	{
		matSpy->SetFont(s_smallFont, s_smallFontHeight);
		panel->Add(InstrumentPanel::CENTER, InstrumentCluster::CENTER, "player", matSpy);
	}
#endif

#if !defined(CONSUMER_BUILD)
	CActorSpyInstrument *actorSpy = new CActorSpyInstrument;
	ASSERT(actorSpy);
	if (actorSpy)
	{
		actorSpy->Initialize();
		actorSpy->SetFont(s_smallFont, s_smallFontHeight);
		panel->Add(InstrumentPanel::CENTER, InstrumentCluster::CENTER, "player", actorSpy);
	}
#endif

// Cheater in for consumer builds only
	CheaterInstrument *cheat = new CheaterInstrument;
	ASSERT(cheat);
	if (cheat)
	{
		cheat->SetFont(s_font, s_fontHeight);
		panel->Add(InstrumentPanel::BOTTOMCENTER, InstrumentCluster::CENTER, "game", cheat);
	}

	m_showOverride = true;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Create the appropriate panel
///////////////////////////////////////////////////////////////////////////////
bool GameHUD::CreateSPPanel()
{
	if (!CreateCommonPanel())
		return false;

	InstrumentPanel *panel = g_instruments.Find("AvatarHUD");
	ASSERT(panel);
	if (!panel)
		return false;

	if (g_gameEnvironment.BonusLevel() || g_GlobalProfile.GameSettings.Difficulty == 3)
		m_radar = NULL;
	else
	{
		m_radar = new SPRadarInstrument;
		ASSERT(m_radar);
		if (m_radar)
			panel->Add(InstrumentPanel::TOPLEFT, InstrumentCluster::CENTER, "player", m_radar);
	}

#if !defined(CONSUMER_BUILD)
	AISpyInstrument *aiSpy = new AISpyInstrument;
	ASSERT(aiSpy);
	if (aiSpy)
		panel->Add(InstrumentPanel::CENTER, InstrumentCluster::CENTER, "game", aiSpy);
#endif

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool GameHUD::CreateMPPanel()
{
	if (!CreateCommonPanel())
		return false;

	InstrumentPanel *panel = g_instruments.Find("AvatarHUD");
	ASSERT(panel);
	if (!panel)
		return false;

	m_radar = new MPRadarInstrument;
	ASSERT(m_radar);
	if (m_radar)
		panel->Add(InstrumentPanel::TOPRIGHT, InstrumentCluster::CENTER, "player", m_radar);

	TimerInstrument *timer = new TimerInstrument;
	ASSERT(timer);
	if (timer)
	{
		timer->SetFont(s_font, s_fontHeight);
		panel->Add(InstrumentPanel::TOPCENTER, InstrumentCluster::CENTER, "game", timer);
	}

	m_chat = new MultilineTextInstrument("chat", 5, 10.0f, s_smallFont, s_smallFontHeight);
	ASSERT(m_chat);
	if (m_chat)
	{
		Vector4 color(1.0f, 1.0f, 1.0f, 0.3f);
		m_chat->SetColor(color);
		m_chat->SetSpritePriority(99);
#ifdef _DEBUG
		m_chat->AddText("Welcome to 25 To Life.");
		m_chat->AddText("ILikeFood wants to kill you.");
		m_chat->AddText("You have no chance.");
		m_chat->AddText("By the way, the loadout you selected sux.");
		m_chat->AddText("And your custom character looks whack.");
#endif
		panel->Add(InstrumentPanel::TOPLEFT, InstrumentCluster::CENTER, "game", m_chat);
	}

	PlayerNamesInstrument *playerNames = new PlayerNamesInstrument;
	ASSERT(playerNames);
	if (playerNames)
		panel->Add(InstrumentPanel::CENTER, InstrumentCluster::CENTER, "player", playerNames);
#if defined(NETWORKING)
	NetStatInstrument *netStat = new NetStatInstrument;
	ASSERT(netStat);
	if (netStat)
		panel->Add(InstrumentPanel::TOPRIGHT, InstrumentCluster::CENTER, "game", netStat);
#endif
	TalkieInstrument *talkie = new TalkieInstrument;
	ASSERT(talkie);
	if (talkie)
		panel->Add(InstrumentPanel::TOPLEFT, InstrumentCluster::SOUTH, "game", talkie);

	SpawnTicketsInstrument *tickets = new SpawnTicketsInstrument;
	ASSERT(tickets);
	if(tickets)
	{
		tickets->SetFont(s_font, s_fontHeight);
		panel->Add(InstrumentPanel::TOPCENTER, InstrumentCluster::EAST, "game", tickets);
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool GameHUD::Activate(bool active)
{
	g_instruments.Activate("AvatarHUD", active);
	m_active = active;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool GameHUD::Activated()
{
	return m_active;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameHUD::Update()
{
	// Get our panel!
	InstrumentPanel *panel = g_instruments.Find("AvatarHUD");
	if (!panel)
		return;

	// Game hud is always on, unless we are
	bool showGameHUD = m_showOverride && !InFrontEnd() && !InCutscene() && !InError() && !InFade();
	bool showPlayerHUD = showGameHUD && !PlayerDead() && !NonPlayerCamera() && !EndOfMatch();
	float fade = showGameHUD ? -1.0f : 0.0f;

	panel->ShowGroup("game", showGameHUD, fade);
	panel->ShowGroup("player", showPlayerHUD, fade);

	// This is a crazy-assed hiazack, but the hud knows best
	if (g_lensFlarePtr)
	{
		if (showGameHUD) g_lensFlare.Show(); else	g_lensFlare.Hide();
	}
	if (!showPlayerHUD && ValidSingleton(AvatarFX))
		g_avatarFX.Reset();
}

///////////////////////////////////////////////////////////////////////////////
// Wee! everything else you just send a message
///////////////////////////////////////////////////////////////////////////////
bool GameHUD::SendMessage(
const char *message,
void *data,
ActorHandle sender)
{
	if (ValidSingleton(InstrumentSystem))
		g_messageDispatcher.SendMessageToOwner(message, data, sender, g_hud.GetMessageOwner());
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameHUD::MunitionFired::HandleMessage(
void *i_data,
ActorHandle i_sender,
bool i_posted)
{
	CActor *sender = CActor::FromHandle(i_sender);
	if (!sender || !d_ref->m_radar)
		return;

	CMunitionFired *fired = (CMunitionFired *)i_data;
	if (!fired->d_firstShot || fired->d_deflected)
		return;

	d_ref->m_radar->NotifyFired(sender, fired->d_shotOrigin);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameHUD::MunitionImpactAck::HandleMessage(
void *i_data,
ActorHandle i_sender,
bool i_posted)
{
	bool hit = (bool)(i_data != 0);
	CCompActor *subject = d_ref->GetSubject();
	if (!hit || !subject || !d_ref->m_reticle)
		return;

	CActor *victim = CActor::FromHandle(i_sender);
	if (victim)
	{
		// Try to get my actor's participant interface.
		CParticipantIF* victimIF = (CParticipantIF*)victim->GetInterface(CParticipantIF::GetID());
		CParticipantIF* actorIF = (CParticipantIF*)subject->GetInterface(CParticipantIF::GetID());
		if (victimIF && actorIF && actorIF->GetTeamId() != victimIF->GetTeamId())
			d_ref->m_reticle->RegisterHit();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameHUD::PulseHealth::HandleMessage(
void *i_data,
ActorHandle i_sender,
bool i_posted)
{
	if (!d_ref->m_health)
		return;

 	GameHUD_PulseHealth *msg = (GameHUD_PulseHealth *)i_data;
	ASSERT(msg);
	if (msg)
		d_ref->m_health->Pulse(msg->strength, msg->duration);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameHUD::ScopeOverlay::HandleMessage(
void *i_data,
ActorHandle i_sender,
bool i_posted)
{
	if (!d_ref->m_scope)
		return;

	// Is this us?
	if (i_sender == d_ref->m_subjectHandle)
	{
	 	bool draw = (i_data != 0);
		d_ref->m_scope->DrawOverlay(draw);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameHUD::ShowMessage::HandleMessage(
void *i_data,
ActorHandle i_sender,
bool i_posted)
{
	ASSERT(d_ref->m_marquee);
	if (!d_ref->m_marquee)
		return;

	GameHUD_ShowMessage *msg = (GameHUD_ShowMessage *)i_data;
	if (msg && msg->message)
		d_ref->m_marquee->ShowMessage(msg->message, msg->duration, msg->pauseIt, msg->override);
	else
		d_ref->m_marquee->HideMessage();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameHUD::TextChat::HandleMessage(
void *i_data,
ActorHandle i_sender,
bool i_posted)
{
	if (!d_ref->m_chat)
		return;
#if defined(NETWORKING)
	const char *textMessage = (const char *)i_data;
	if (textMessage)
		d_ref->m_chat->AddText(textMessage);
#endif
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameHUD::KeyboardTextChat::HandleMessage(
void *i_data,
ActorHandle i_sender,
bool i_posted)
{
	if (!d_ref->m_chat)
		return;
#if defined(NETWORKING)
	STextChat *dataIn = ( STextChat * )i_data;

	if( !g_lobby.IsLAN() && ( g_CVoiceChat.isVoiceChatBanned( ) || g_CVoiceChat.isPlayerMuted( dataIn->id, dataIn->flags ) ) )
		return;

	if( dataIn->text )
		d_ref->m_chat->AddText(dataIn->text);
#endif
}


///////////////////////////////////////////////////////////////////////////////
// All the stuff for the auto show-state
///////////////////////////////////////////////////////////////////////////////
bool GameHUD::InFrontEnd()
{
	return (g_FrontEnd.GetActiveScreen() != NULL) ||
				(g_FrontEnd.GetActiveScreen() != NULL) ||
				g_FrontEnd.ScoreBoardUp() ||
				/* ScreenUp("SCREEN_ID_SUMMARY") ||
				ScreenUp("SCREEN_ID_STATISTICS") || */
				(g_referee.GetPhase() != Referee::PLAYING);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool GameHUD::InCutscene()
{
	return g_FrontEnd.IsCinematicInProgress();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool GameHUD::InError()
{
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool GameHUD::InFade()
{
	return RenderMgr__IsFading();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool GameHUD::PlayerDead()
{
	CCompActor *player = GetSubject();
	if (!player)
		return true;

	// Get the health interface
	HealthIF *healthIF = (HealthIF *)player->GetInterface(HealthIF::GetID());
	if (!healthIF)
		return true;

	// Hello!
	return !healthIF->IsAlive();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool GameHUD::NonPlayerCamera()
{
	if (!g_shooterCamPtr)
		return false;

	return !g_shooterCam.Active() || g_shooterCam.LockedToTarget();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool GameHUD::EndOfMatch()
{
	// Wee!
	int time = g_referee.GetRemainingPhaseTime();
	return (g_referee.GetPhase() == Referee::PLAYING && (time >= 0 && time < 1000));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool GameHUD::ScreenUp(
const char *screenName)
{
	ScreenHandler *handler = g_FrontEnd.GetScreenHandler(screenName);
	return (handler && handler->GetVisible());
}
