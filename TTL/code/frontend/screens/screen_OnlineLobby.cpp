#include "TTLPCH.h"
#include "voice/netz/CVoiceChat.h"
#include "environ/ConfigMgr.h"
#include "frontend/frontend.h"
#include "frontend/inputdriver.h"
#include "frontend/keyboardchat.h"
#include "frontend/screens/screen_OnlineLobby.h"
#include "frontend/screens/screen_Popups.h"
#include "frontend/screens/screen_GameSetting.h"
#include "GameHelper/Translator.h"

#include "data/screens/NetworkGames.h"
#include "data/screens/OnlineScreens.h"
#include "Game/Managers/ResourceManager.h"

#include "frontend/savegame.h"
#include "main/PlayerDB.h"

#if defined( _XBOX )
#include "platform/xbox/xboxlive.h"
#include "frontend/UseCase/Xbox/UseCaseUIXPlayers.hpp"
#include "frontend/UseCase/Xbox/UseCaseUIXFriends.hpp"
#elif defined( PS2 ) || defined( DIRECTX_PC )
#include "frontend/UseCase/UseCasePlayers.hpp"
#include "frontend/UseCase/UseCaseFriends.hpp"
#include "frontend/UseCase/UseCaseInvitePlayerToMatch.hpp"
#endif

enum
{
	COMMAND_QUITGAME_CANCEL = 0x100,
	COMMAND_QUITGAME_OK,
	COMMAND_TEAMCHANGE_OK,
	COMMAND_TEAMCHANGE_CANCEL,
	COMMAND_INVALIDTEAMS_OK,
};

//=============================================================================
// ScreenOnlineLobby::ScreenOnlineLobby - The default constructor
//=============================================================================

ScreenOnlineLobby::ScreenOnlineLobby( void ) :
	m_messageOwner( "ScreenOnlineLobby" )
{
	m_pTeam1Group = 0;
	m_pTeam2Group = 0;
	m_GameSettingsEnabled = true;
	m_WeAreReady = NOT_READY;

	memset( m_pTeam1Sprites, 0, sizeof(m_pTeam1Sprites) );
	memset( m_pTeam2Sprites, 0, sizeof(m_pTeam2Sprites) );
}

//=============================================================================
// ScreenOnlineLobby::~ScreenOnlineLobby - The default destructor
//=============================================================================

ScreenOnlineLobby::~ScreenOnlineLobby( void )
{
}

//=============================================================================
// ScreenOnlineLobby::OpenTeamSlot
//
//		This function opens a slot on the specified team.
//=============================================================================

void ScreenOnlineLobby::OpenTeamSlot( int Team, int Slot )
{
	Sprite* pSprite = GetTeamSlot( Team, Slot );

	if( pSprite )
		pSprite->m_state.familycolor.a = 255;

	ClearTeamSlot( Team, Slot );
	SetPlayerName( Team, Slot, "Open" );
}

//=============================================================================
// ScreenOnlineLobby::CloseTeamSlot
//
//		This function closes a slot on the specified team.
//=============================================================================

void ScreenOnlineLobby::CloseTeamSlot( int Team, int Slot )
{
	Sprite* pSprite = GetTeamSlot( Team, Slot );

	if( pSprite )
		pSprite->m_state.familycolor.a = 0;

	return;
}

//=============================================================================
// ScreenOnlineLobby::ClearTeamSlot
//
//		Removes the specified player from the team and displays "Open" in that
//		slot.
//=============================================================================

void ScreenOnlineLobby::ClearTeamSlot( int Team, int Slot )
{
	SetPlayerName( Team, Slot, 0 );
	SetPlayerStatus( Team, Slot, STATUS_UNDEFINED );
	SetSpeakerType( Team, Slot, SPEAKERTYPE_NONE, false );
	SetFriendIcon( Team, Slot, FRIENDICON_NONE );
}

//=============================================================================
// ScreenOnlineLobby::GetTeamSlot
//
//		Returns a sprite pointer to the specified team slot.
//=============================================================================

Sprite* ScreenOnlineLobby::GetTeamSlot( int Team, int Slot )
{
	const int n = LOBBY_MAX_PLAYERS;

	if( Slot >= 0 && Slot < n )
	{
		if( g_referee.GetTeamingModel().IsTeamInGroupAlpha( Team ) )
			return m_pTeam1Sprites[Slot];

		if( g_referee.GetTeamingModel().IsTeamInGroupBeta( Team ) )
			return m_pTeam2Sprites[Slot];
	}

	return 0;
}

//=============================================================================
// ScreenOnlineLobby::GetTextPlayerName
//
//		Returns a pointer to the text which displays a name for the specified
//		player.
//=============================================================================

Text* ScreenOnlineLobby::GetTextPlayerName( int Team, int Slot )
{
	return GetTextSprite( ID_ONLINELOBBY_PLAYERNAME, GetTeamSlot( Team, Slot ) );
}

//=============================================================================
// ScreenOnlineLobby::SetPlayerName
//
//		Sets the name to display for the specified player.
//=============================================================================

void ScreenOnlineLobby::SetPlayerName( int Team, int Slot, const char* pPlayerName )
{
	Text* pText = GetTextPlayerName( Team, Slot );

	if( pText )
		pText->SetText( pPlayerName );

	return;
}

//=============================================================================
// ScreenOnlineLobby::ShowRatingIcon
//=============================================================================

void ScreenOnlineLobby::ShowRatingIcon( int Team, int Slot, int Rank, int Rating )
{
	Sprite* pRatingIcons;
	Sprite* pRankIcons;
	Sprite* pSourceIcon;
	Sprite* pIcon;

	bool Ranked = false;

	pIcon = GetSprite( ID_ONLINELOBBY_RATINGICON, GetTeamSlot( Team, Slot ) );

	if( pIcon )
	{
		pRankIcons = GetSprite( ID_ONLINELOBBY_RANKICONS );

		if( pRankIcons )
		{
			// go for a number rank
			pSourceIcon = pRankIcons->GetNthChild( Rank-1 );

			if( pSourceIcon )
			{
				float x, y;
				pIcon->GetPosition( x, y );
				pIcon->Copy( pSourceIcon );
				pIcon->SetPosition( x, y );
				Ranked = true;
			}
			else
			{
				pIcon->SetTextureSet( INVALID_HANDLE, 0 );
			}
		}

		if( !Ranked )
		{
			// try for a rating
			pRatingIcons = GetSprite( ID_ONLINELOBBY_RATINGICONS );

			if( pRatingIcons )
			{
				pSourceIcon = pRatingIcons->GetNthChild( Rating-1 );
				
				if( pSourceIcon )
				{
					float x, y;
					pIcon->GetPosition( x, y );
					pIcon->Copy( pSourceIcon );
					pIcon->SetPosition( x, y );
					Ranked = true;
				}
			}

			// if its not ranked or rated, then don't show it
			pIcon->Show( Ranked );
		}
	}

	return;
}

//=============================================================================
// ScreenOnlineLobby::ShowCountdownTimer
//=============================================================================

void ScreenOnlineLobby::ShowCountdownTimer( bool Show )
{
	ShowSprite( ID_ONLINELOBBY_TIMER, Show );
}

//=============================================================================
// ScreenOnlineLobby::SetCountdownTimer
//=============================================================================

void ScreenOnlineLobby::SetCountdownTimer( int N )
{
	Text* pText = GetTextSprite( ID_ONLINELOBBY_TIMER );

	if( pText )
		pText->Printf( "%d", N );

	return;
}

//=============================================================================
// ScreenLineLobby::SetPlayerStatus
//=============================================================================

void ScreenOnlineLobby::SetPlayerStatus( int Team, int Slot, STATUS Status )
{
	Sprite* pSlot = GetTeamSlot( Team, Slot );

	if( pSlot )
	{
		Sprite* pSourceIcon = GetStatusIcon( Status );
		Sprite* pStatusIcon = GetSprite( ID_ONLINELOBBY_STATUSICON, pSlot );

		if( pSourceIcon && pStatusIcon )
		{
			float x, y;
			pStatusIcon->GetPosition( x, y );
			pStatusIcon->Copy( pSourceIcon );
			pStatusIcon->SetPosition( x, y );
			pStatusIcon->Show();
		}
		else
		if( pStatusIcon )
		{
			pStatusIcon->Hide();
		}
	}

	return;
}

//=============================================================================
// ScreenOnlineLobby::GetStatusIcon
//=============================================================================

Sprite* ScreenOnlineLobby::GetStatusIcon( STATUS Status )
{
	int IconId = 0;

	switch( Status )
	{
	case STATUS_UNDEFINED:
		IconId = 0;
		break;

	case STATUS_READY:
		IconId = ID_ONLINELOBBY_ICONREADY;
		break;

	case STATUS_NOTREADY:
		IconId = ID_ONLINELOBBY_ICONNOTREADY;
		break;

	case STATUS_CHAT:
		IconId = ID_ONLINELOBBY_ICONCHAT;
		break;

	case STATUS_SETTINGS:
		IconId = ID_ONLINELOBBY_ICONOPTIONS;
		break;

	case STATUS_IDLE:
		IconId = ID_ONLINELOBBY_ICONIDLE;
		break;
	}

	return GetSprite( IconId );
}

//=============================================================================
// ScreenOnlineLobby::SetSpeakerType
//=============================================================================

void ScreenOnlineLobby::SetSpeakerType( int Team, int Slot, SPEAKERTYPE SpeakerType, bool Bright )
{
	Sprite* pSlot = GetTeamSlot( Team, Slot );

	if( pSlot )
	{
		Sprite* pSourceIcon = GetSpeakerIcon( SpeakerType, Bright );
		Sprite* pSpeakerIcon = GetSprite( ID_ONLINELOBBY_SPEAKERICON, pSlot );

		if( pSourceIcon && pSpeakerIcon )
		{
			float x, y;
			pSpeakerIcon->GetPosition( x, y );
			pSpeakerIcon->Copy( pSourceIcon );
			pSpeakerIcon->SetPosition( x, y );
			pSpeakerIcon->Show();
		}
		else
		if( pSpeakerIcon )
		{
			pSpeakerIcon->Hide();
		}
	}

	return;
}

//=============================================================================
// ScreenOnlineLobby::GetSpeakerIcon
//=============================================================================

Sprite* ScreenOnlineLobby::GetSpeakerIcon( SPEAKERTYPE SpeakerType, bool Bright )
{
	int GroupId = 0;
	int IconId = 0;

	if( Bright )
		GroupId = ID_ONLINELOBBY_SPEAKERICONS_BRIGHT;
	else
		GroupId = ID_ONLINELOBBY_SPEAKERICONS_DIM;

	switch( SpeakerType )
	{
	case SPEAKERTYPE_NONE:
		IconId = ID_ONLINELOBBY_ICONSPEAKEROFF;
		break;

	case SPEAKERTYPE_HEADSET:
		IconId = ID_ONLINELOBBY_ICONSPEAKER;
		break;

	case SPEAKERTYPE_TVSET:
		IconId = ID_ONLINELOBBY_ICONTVSPEAKER;
		break;

	case SPEAKERTYPE_DISABLED:
		IconId = ID_ONLINELOBBY_ICONNOSPEAKER;
		break;
	}

	return GetSprite( IconId, GetSprite( GroupId ) );
}

//=============================================================================
// ScreenOnlineLobby::SetFriendIcon
//=============================================================================

void ScreenOnlineLobby::SetFriendIcon( int Team, int Slot, FRIENDICON FriendIcon )
{
	Sprite* pSlot = GetTeamSlot( Team, Slot );

	if( pSlot )
	{
		Sprite* pSourceIcon = GetFriendIcon( FriendIcon );
		Sprite* pFriendIcon = GetSprite( ID_ONLINELOBBY_FRIENDICON, pSlot );

		if( pSourceIcon && pFriendIcon )
		{
			float x, y;
			pFriendIcon->GetPosition( x, y );
			pFriendIcon->Copy( pSourceIcon );
			pFriendIcon->SetPosition( x, y );
			pFriendIcon->Show();
		}
		else
		if( pFriendIcon )
		{
			pFriendIcon->Hide();
		}
	}

	return;
}

//=============================================================================
// ScreenOnlineLobby::GetFriendIcon
//=============================================================================

Sprite* ScreenOnlineLobby::GetFriendIcon( FRIENDICON FriendIcon )
{
	int IconId = 0;

	switch( FriendIcon )
	{
	case FRIENDICON_NONE:
		IconId = ID_ONLINELOBBY_ICONPLAYER;
		break;

	case FRIENDICON_SELF:
		IconId = ID_ONLINELOBBY_ICONSELF;
		break;

	case FRIENDICON_FRIEND:
		IconId = ID_ONLINELOBBY_ICONFRIEND;
		break;

	case FRIENDICON_SENT:
		IconId = ID_ONLINELOBBY_ICONFRIEND_SENT;
		break;

	case FRIENDICON_RECEIVED:
		IconId = ID_ONLINELOBBY_ICONFRIEND_RECEIVED;
		break;

	case FRIENDICON_HOST:
		IconId = ID_ONLINELOBBY_ICONHOST;
		break;
	}

	return GetSprite( IconId );
}

//=============================================================================
// ScreenOnlineLobby::SetSessionParam
//=============================================================================

void ScreenOnlineLobby::SetSessionParam( int ParamId, const char* pParamText )
{
	Text* pText = GetTextSprite( ID_ONLINELOBBY_PARAM, GetSprite( ParamId ) );

	if( pText )
		pText->SetText( pParamText );

	return;
}

//=============================================================================
// ScreenOnlineLobby::SetGameType
//=============================================================================

void ScreenOnlineLobby::SetGameType( const char* pGameType )
{
	SetSessionParam( ID_ONLINELOBBY_GAMETYPE, pGameType );
}

//=============================================================================
// ScreenOnlineLobby::SetLevelName
//=============================================================================

void ScreenOnlineLobby::SetLevelName( const char* pLevelName )
{
	SetSessionParam( ID_ONLINELOBBY_LEVELNAME, pLevelName );
}


//=============================================================================
// ScreenOnlineLobby::SetTimeLimit
//=============================================================================

void ScreenOnlineLobby::SetTimeLimit( const char* pTimeLimit )
{
	SetSessionParam( ID_ONLINELOBBY_TIMELIMIT, pTimeLimit );
}

//=============================================================================
// ScreenOnlineLobby::SetTimeLimit
//=============================================================================

void ScreenOnlineLobby::SetTimeLimit( int Minutes, int Seconds )
{
	Minutes+= Seconds / 60;
	Seconds = Seconds % 60;

	if( Seconds == 0 )
		SetTimeLimit( ::Printf( "%d Minutes", Minutes ) );
	else
		SetTimeLimit( ::Printf( "%d Min %d Sec", Minutes, Seconds ) );

	return;
}

//=============================================================================
// ScreenOnlineLobby::SetScoreLimit
//=============================================================================

void ScreenOnlineLobby::SetScoreLimit( const char* pScoreLimit )
{
	SetSessionParam( ID_ONLINELOBBY_SCORELIMIT, pScoreLimit );
}

//=============================================================================
// ScreenOnlineLobby::SetScoreLimit
//=============================================================================

void ScreenOnlineLobby::SetScoreLimit( int ScoreLimit )
{
	SetScoreLimit( ::Printf( "%d Points", ScoreLimit ) );
}

//=============================================================================
// ScreenOnlineLobby::ShowLoadingText
//=============================================================================

void ScreenOnlineLobby::ShowLoadingText( bool Show )
{
	Text* pText = GetTextSprite( ID_ONLINELOBBY_LOADINGTEXT );

	if( pText )
	{
		if( Show )
			pText->m_state.familycolor.a = 255;
		else
			pText->m_state.familycolor.a = 0;
	}

	return;
}

//=============================================================================
// ScreenOnlineLobby::SetLoadingText
//=============================================================================

void ScreenOnlineLobby::SetLoadingText( const char* pText )
{
	SetText( ID_ONLINELOBBY_LOADINGTEXT, pText );
}

//=============================================================================
// ScreenOnlineLobby::ShowSettingsChanged
//=============================================================================

void ScreenOnlineLobby::ShowSettingsChanged( bool Show)
{
	Sprite* pPopupBox = GetSprite( ID_ONLINELOBBY_POPUPBOX );

	if( pPopupBox )
	{
		if( Show )
		{
			pPopupBox->InitAnim();

			SpriteAnim& anim = *pPopupBox->m_anim;

			pPopupBox->m_anim->familycolor.a.SetFrame( 0.0f, 0.0f );
			pPopupBox->m_anim->familycolor.a.NextFrame( 0.2f, 1.0f );
			pPopupBox->m_anim->familycolor.a.NextFrame( 1.0f, 1.0f );
			pPopupBox->m_anim->familycolor.a.NextFrame( 2.4f, 0.0f );
			pPopupBox->m_anim->familycolor.a.TimeCurve( true );
			pPopupBox->m_anim->familycolor.a.Play( false );

			pPopupBox->m_anim->scale.x.SetFrame( 0.000f, 3.3f );
			pPopupBox->m_anim->scale.x.NextFrame( 0.200f, 1.15f );
			pPopupBox->m_anim->scale.x.NextFrame( 3.400f, 0.95f );
			pPopupBox->m_anim->scale.x.Deccelerate( true );
			pPopupBox->m_anim->scale.x.Play( false );

			pPopupBox->m_anim->scale.y.SetFrame( 0.000f, 3.3f );
			pPopupBox->m_anim->scale.y.NextFrame( 0.200f, 1.15f );
			pPopupBox->m_anim->scale.y.NextFrame( 3.400f, 0.95f );
			pPopupBox->m_anim->scale.y.Deccelerate( true );
			pPopupBox->m_anim->scale.y.Play( false );

			/*
			anim.scale.x.SetFrame( 0.000f, 1.2f );
			anim.scale.x.NextFrame( 0.800f, 1.3f );
			anim.scale.x.NextFrame( 1.000f, 0.5f );
			anim.scale.x.Deccelerate( true );
			anim.scale.x.Play( false );

			anim.scale.y.SetFrame( 0.000f, 1.2f );
			anim.scale.y.NextFrame( 0.800f, 1.3f );
			anim.scale.y.NextFrame( 1.000f, 0.5f );
			anim.scale.y.Deccelerate( true );
			anim.scale.y.Play( false );

			anim.familycolor.a.SetFrame( 0.000f, 0.0f );
			anim.familycolor.a.NextFrame( 0.100f, 1.0f );
			anim.familycolor.a.NextFrame( 1.500f, 1.0f );
			anim.familycolor.a.NextFrame( 0.200f, 0.0f );
			anim.familycolor.a.Deccelerate( true );
			anim.familycolor.a.Play( false );
			*/
		}
		else
		{
			pPopupBox->m_state.familycolor.a = 0;
		}
	}

	return;
}

void ScreenOnlineLobby::ShowMatchInProgress( bool Show )
{
	Sprite* pMatchRect = NULL;
	pMatchRect = GetSprite( ID_ONLINELOBBY_TIMEBOX );

	if( pMatchRect )
	{
		if( Show )
		{
			pMatchRect->m_state.familycolor.a = 255;
		}
		else
		{
			pMatchRect->m_state.familycolor.a = 0;
		}
	}

	return;
}

void ScreenOnlineLobby::SetTimeRemaining( const char* pTimeText )
{
	SetText( ID_ONLINELOBBY_TIMELEFT, pTimeText );
}

void ScreenOnlineLobby::EnableAllButtons( bool ReadyEnabled, bool OthersEnabled )
{
	bool IsMultiplayer = false;
	bool IsHost = false;
	CheckButtonStatus( IsMultiplayer, IsHost );

	bool IsFriendsEnabled = false;

#ifdef _XBOX
	if( OthersEnabled && IsMultiplayer )
		IsFriendsEnabled = true;
	else
	if( OthersEnabled && CXboxLive::AreWeLoggedIn() )
		IsFriendsEnabled = true;
	else
		IsFriendsEnabled = false;
#else
	IsFriendsEnabled = OthersEnabled && IsMultiplayer;
#endif

	m_GameSettingsEnabled = OthersEnabled;

	EnableButton( ID_ONLINELOBBY_READY, ReadyEnabled );
	EnableButton( ID_ONLINELOBBY_FORCEREADY, ReadyEnabled && IsHost );
	EnableButton( ID_ONLINELOBBY_SETTINGS, OthersEnabled );
	EnableButton( ID_ONLINELOBBY_CHANGETEAM, OthersEnabled );
	EnableButton( ID_ONLINELOBBY_TEXTCHAT, OthersEnabled );
	EnableButton( ID_ONLINELOBBY_FRIENDS, IsFriendsEnabled );
	EnableButton( ID_ONLINELOBBY_PLAYERS, OthersEnabled && IsMultiplayer );
#if defined PS2 || defined DIRECTX_PC
	EnableButton( ID_ONLINELOBBY_INVITE, OthersEnabled && IsHost );
#else
	EnableButton( ID_ONLINELOBBY_INVITE, false );
#endif

	ShowSprite( ID_ONLINELOBBY_HELPTEXT1, OthersEnabled );
	ShowSprite( ID_ONLINELOBBY_HELPTEXT2, ReadyEnabled || OthersEnabled );
	ShowSprite( ID_ONLINELOBBY_HELPTEXT3, OthersEnabled );
	ShowSprite( ID_ONLINELOBBY_HELPTEXT4, OthersEnabled );
}

//=============================================================================
// Determines what goup of buttons should be visible based on
// platform and online status
//=============================================================================

void ScreenOnlineLobby::CheckButtonStatus( bool& IsPlayingMultiplayerOnline, bool& IsHostOfMultiplayerOnline )
{
#if defined PS2 || defined _XBOX || defined DIRECTX_PC
	IsPlayingMultiplayerOnline = ( OSI::getSingletonPtr() != NULL );
#elif defined _DEBUG
	IsPlayingMultiplayerOnline = true;
#else
	IsPlayingMultiplayerOnline = false;
#endif

	//=========================================================
	// The friends and players menu is disabled when we're not
	// playing online (for example, when we're in system link).
	//=========================================================

#ifndef _XBOX
	ShowSprite( ID_ONLINELOBBY_FRIENDS, IsPlayingMultiplayerOnline );
	ShowSprite( ID_ONLINELOBBY_FRIENDSCURSOR, IsPlayingMultiplayerOnline );
#endif

	ShowSprite( ID_ONLINELOBBY_PLAYERS, IsPlayingMultiplayerOnline );
	ShowSprite( ID_ONLINELOBBY_PLAYERSCURSOR, IsPlayingMultiplayerOnline );

	IsHostOfMultiplayerOnline = g_session.IsMaster() && IsPlayingMultiplayerOnline;

#if defined PS2 || defined DIRECTX_PC
	ShowSprite( ID_ONLINELOBBY_INVITE, IsHostOfMultiplayerOnline );
	ShowSprite( ID_ONLINELOBBY_INVITECURSOR, IsHostOfMultiplayerOnline );
#else
	ShowSprite( ID_ONLINELOBBY_INVITE, false );
	ShowSprite( ID_ONLINELOBBY_INVITECURSOR, false );
#endif
	ShowSprite( ID_ONLINELOBBY_FORCEREADYCURSOR, IsHostOfMultiplayerOnline );
	ShowSprite( ID_ONLINELOBBY_FORCEREADY, IsHostOfMultiplayerOnline );
	Sprite* p25ToLife = GetSprite( ID_ONLINELOBBY_25TOLIFE );

	if( p25ToLife )
		p25ToLife->m_state.familycolor.a = IsHostOfMultiplayerOnline ? 0 : 255;
}

//=============================================================================
// ScreenOnlineLobby::GetLocalPlayer
//=============================================================================

int ScreenOnlineLobby::GetLocalPlayer( void )
{
	CPlayerAgent* pAgent = g_referee.GetLocalPlayerAgent();
	if( pAgent )
		return pAgent->Id();
	return 0;
}

//=============================================================================
// ScreenOnlineLobby::ChangeTeam
//=============================================================================

void ScreenOnlineLobby::ChangeTeam( void )
{
	CPlayerAgent* pAgent = g_referee.GetLocalPlayerAgent();
	if( pAgent )
	{
		Referee &ref = g_referee;
		const CTeamingModel &teaming = ref.GetTeamingModel();

		uint8 teamA = teaming.FindTeamInGroupAlpha();
		uint8 teamB = teaming.FindTeamInGroupBeta();

		if( pAgent->GetRequestedTeam() == teamA )
			pAgent->RequestTeam( teamB );
		else if( pAgent->GetRequestedTeam() == teamB )
			pAgent->RequestTeam( teamA );
	}

	return;
}

//==========================================================================
// ScreenOnlineLobby::SetAwayTime
//==========================================================================

void ScreenOnlineLobby::SetAwayTime( float AwayTime )
{
	m_AwayTime = AwayTime;
}

//==========================================================================
// ScreenOnlineLobby::GetAwayTime
//==========================================================================

float ScreenOnlineLobby::GetAwayTime( void )
{
	return m_AwayTime;
}

//==========================================================================
// ScreenOnlineLobby::IsPlayerIdle
//==========================================================================

bool ScreenOnlineLobby::IsPlayerIdle( void )
{
	if( m_AwayTime >= 120.0f )
		return true;

	return false;
}

//=============================================================================
// ScreenOnlineLobby::OnInitialize
//
//		Called once when the screen is initialzed, it gets pointers to all the
//		important stuff it needs later on.
//=============================================================================

void ScreenOnlineLobby::OnInitialize( void )
{
	ScreenBaseClass::OnInitialize();

	m_pTeam1Group = GetSprite( ID_ONLINELOBBY_TEAM1 );

	m_pTeam1Sprites[0] = GetSprite( ID_ONLINELOBBY_PLAYER1, m_pTeam1Group );
	m_pTeam1Sprites[1] = GetSprite( ID_ONLINELOBBY_PLAYER2, m_pTeam1Group );
	m_pTeam1Sprites[2] = GetSprite( ID_ONLINELOBBY_PLAYER3, m_pTeam1Group );
	m_pTeam1Sprites[3] = GetSprite( ID_ONLINELOBBY_PLAYER4, m_pTeam1Group );
	m_pTeam1Sprites[4] = GetSprite( ID_ONLINELOBBY_PLAYER5, m_pTeam1Group );
	m_pTeam1Sprites[5] = GetSprite( ID_ONLINELOBBY_PLAYER6, m_pTeam1Group );
	m_pTeam1Sprites[6] = GetSprite( ID_ONLINELOBBY_PLAYER7, m_pTeam1Group );
	m_pTeam1Sprites[7] = GetSprite( ID_ONLINELOBBY_PLAYER8, m_pTeam1Group );

	m_pTeam2Group = GetSprite( ID_ONLINELOBBY_TEAM2 );

	m_pTeam2Sprites[0] = GetSprite( ID_ONLINELOBBY_PLAYER1, m_pTeam2Group );
	m_pTeam2Sprites[1] = GetSprite( ID_ONLINELOBBY_PLAYER2, m_pTeam2Group );
	m_pTeam2Sprites[2] = GetSprite( ID_ONLINELOBBY_PLAYER3, m_pTeam2Group );
	m_pTeam2Sprites[3] = GetSprite( ID_ONLINELOBBY_PLAYER4, m_pTeam2Group );
	m_pTeam2Sprites[4] = GetSprite( ID_ONLINELOBBY_PLAYER5, m_pTeam2Group );
	m_pTeam2Sprites[5] = GetSprite( ID_ONLINELOBBY_PLAYER6, m_pTeam2Group );
	m_pTeam2Sprites[6] = GetSprite( ID_ONLINELOBBY_PLAYER7, m_pTeam2Group );
	m_pTeam2Sprites[7] = GetSprite( ID_ONLINELOBBY_PLAYER8, m_pTeam2Group );

	m_LobbyScreenRefreshHandler.Initialize( "RefereeAssignmentChanged", MESSAGE_HANDLE_IF_MASTER, m_messageOwner, this );
	m_LobbyStartCountHandler.Initialize( "BeginPopulating", MESSAGE_HANDLE_IF_MASTER, m_messageOwner, this );

	UseHighlighting( true );

	SetHelpRect( ID_ONLINELOBBY_HELPGROUP );
	SetInitialButton( ID_ONLINELOBBY_READY );
	SetNextButton( ID_ONLINELOBBY_SETTINGS );
	SetNextButton( ID_ONLINELOBBY_CHANGETEAM );
	SetNextButton( ID_ONLINELOBBY_TEXTCHAT );
	SetNextButton( ID_ONLINELOBBY_FRIENDS );
	SetNextButton( ID_ONLINELOBBY_PLAYERS );
	SetNextButton( ID_ONLINELOBBY_INVITE );
	SetNextButton( ID_ONLINELOBBY_FORCEREADY );

	SetCursor( ID_ONLINELOBBY_CURSOR );

	SetNextScreen( ID_ONLINELOBBY_SETTINGS, "SCREEN_ID_LOBBYMENU" );
	SetNextScreen( ID_ONLINELOBBY_TEXTCHAT, "SCREEN_ID_CHAT" );

	m_Chat.InitChatWindow( GetSprite(ID_ONLINELOBBY_CHATWINDOW), ID_ONLINELOBBY_CHATTEXT );

#ifdef DIRECTX_PC
	GetSprite(ID_ONLINELOBBY_HELPGROUP)->HideAll();
#endif
}

//=============================================================================
// ScreenOnlineLobby::OnScreenIntro
//=============================================================================

void ScreenOnlineLobby::OnScreenIntro( void )
{
	ScreenBaseClass::OnScreenIntro();

	int Slot;
	int n = LOBBY_MAX_PLAYERS;

	int alphaTeam = g_referee.GetTeamingModel().FindTeamInGroupAlpha();
	int betaTeam = g_referee.GetTeamingModel().FindTeamInGroupBeta();

	for( Slot = 0; Slot < n; Slot++ )
	{
		CloseTeamSlot( alphaTeam, Slot );
		CloseTeamSlot( betaTeam, Slot );
	}

	m_PopulateStartTime = g_session.GetSessionTimeMS();
	ShowCountdownTimer( false );
	ShowSettingsChanged( false );
	ShowMatchInProgress( false );
	ShowLoadingText( false );

	// initialize a big string for the chat xstring so we don't grow dynamically
	Text* pText = GetTextSprite( ID_ONLINELOBBY_CHATTEXT );
	if ( pText )
	{
		char buf[ 640 ];
		memset( buf, ' ', sizeof(buf) );
		buf[ sizeof(buf) - 1 ] = 0;
		pText->SetText( buf );
	}

	m_szTextChat[0] = 0;

	if( g_KeyboardChat.EndChat() == 0 )
		g_KeyboardChat.AddChat( "Welcome." );

	EnableAllButtons( true, true );

	SelectButton( ID_ONLINELOBBY_READY );
	m_WeAreReady = NOT_READY;

	// --- update the voice chat settings

	bool VoiceEnabled = g_GlobalProfile.VoiceParams.VoiceOutput != VOICEOUTPUT_DISABLED;
	bool VoiceThruSpeakers = g_GlobalProfile.VoiceParams.VoiceOutput == VOICEOUTPUT_SPEAKERS;

	g_CVoiceChat.enable( VoiceEnabled );
	g_CVoiceChat.channelVoiceThruSpeakers( VoiceThruSpeakers );
	g_CVoiceChat.setOutputVolume( g_GlobalProfile.VoiceParams.OutputVolume / 100.0f );
	g_CVoiceChat.refreshPlatformData( );

	// --- set our push to talk button up
#if defined( PS2 )
	g_CVoiceChat.SetPushToTalk( CB_STICK1 );
#elif defined( _XBOX )
	g_CVoiceChat.SetPushToTalk( CB_L2 );
#else
	g_CVoiceChat.SetPushToTalk( CB_STICK1 );
#endif

	SetAwayTime(0.0f);
}

//=============================================================================
// ScreenOnlineLobby::OnScreenExit
//=============================================================================

void ScreenOnlineLobby::OnScreenExit( void )
{
	if( 0 )
	{
		Sprite* pSprite = GetScreenSprite();

		if( pSprite )
		{
			SpriteAnim& anim = *pSprite->m_anim;

			anim.offset.y.Clear();
			anim.offset.y.SetFrame( 0.000f, pSprite->m_state.offset.y );
			anim.offset.y.SetFrame( 0.250f, pSprite->GetHeight() );
			anim.offset.y.Deccelerate( true );
			anim.offset.y.Play( false );
		}
	}
	else
	{
		ScreenBaseClass::OnScreenExit();
	}

	return;
}

//=============================================================================
// ScreenOnlineLobby::OnScreenUpdate
//=============================================================================

void ScreenOnlineLobby::OnScreenUpdate( void )
{
#ifdef DIRECTX_PC
	if (g_MenuCursor.IsActive()) {
		float x1, y1, x2, y2;	
		// check for back navigation
		if (g_MenuCursor.CursorButtonPressed(1))
		{
			g_MenuCursor.ForceButtonUp(1);
			//OnButtonCancel();
		}
		float x = g_MenuCursor.GetX();
		float y = g_MenuCursor.GetY();
		// functionality for list box based menus
		GuiListBox* listBox = GetListBox();
		if (listBox)
		{
			int items = listBox->GetItemCount();
			for (int c=0; c < items; ++c)
			{
				listBox->GetCursorRect(x1,y1,x2,y2);
				if ( ( x > x1 && x < x2 ) && ( y > y1 && y < y2 ) )
				{
					if ( g_MenuCursor.CursorButtonPressed(0) )
						SelectMenuItem();
					break;
				}
				//listBox->OnCursorDown();
				OnCursorDown();
			}
		}
		Button* button = GetDefaultButton();
		// functionality for button based menus
		if (!button)
		{
			if (g_MenuCursor.CursorButtonPressed(0))
				OnButtonStart();
		}
		// search through the buttons to see if we're over one
		// if so, check to see if we're clicking the mouse
		while(button) 
		{
			button->SpriteHost()->GetFamilyRect(x1, y1, x2, y2);
			if ( ( x > x1 && x < x2 ) && ( y > y1 && y < y2 ) )
			{
				if (!button->Enabled()) break;
				SelectButton(button);
				if ( g_MenuCursor.CursorButtonPressed(0) )
				{
						g_MenuCursor.ForceButtonUp(0);
						OnButtonOK(); 
				}
				break;
			}
			button = button->GetNextButton();
		}
	}
#endif
	float loadPct = g_resourceManager.GetLoadedPercentage();
	if ( loadPct < 100.0f )
	{
		char buf[ 32 ];
		sprintf( buf, "Loading Map: %2.0f%%", loadPct );
		SetLoadingText( buf );
		ShowLoadingText( true );
	}
	else
	{
		ShowLoadingText( false );
	}

	SetAwayTime( m_AwayTime + g_timer.GetOSFrameSec() );

	STATUS lobbyStatus = STATUS_NOTREADY;

	if( IsPlayerIdle() )
		lobbyStatus = STATUS_IDLE;

	CPlayerAgent* pPlayerAgent = g_referee.GetLocalPlayerAgent();

	if( pPlayerAgent && pPlayerAgent->GetLobbyStatus() != lobbyStatus )
		pPlayerAgent->SetLobbyStatus( lobbyStatus );

	m_Chat.RefreshChat();

	RefreshLobby();
}

//=============================================================================
// ScreenOnlineLobby::OnCursorLeft
//=============================================================================

void ScreenOnlineLobby::OnCursorLeft( void )
{
	OnCursorUp();
	OnCursorUp();
}

//=============================================================================
// ScreenOnlineLobby::OnCursorRight
//=============================================================================

void ScreenOnlineLobby::OnCursorRight( void )
{
	OnCursorDown();
	OnCursorDown();
}

//=============================================================================
// ScreenOnlineLobby::OnButtonCancel
//=============================================================================

void ScreenOnlineLobby::OnButtonCancel( void )
{
	if( g_referee.IsReady( g_referee.GetLocalPlayerAgentId() ) )
		return;

	ScreenPopupDialog* pDialog = g_FrontEnd.GetPopupDialog();

	if( pDialog )
	{
		pDialog->Clear();
		pDialog->SetTitle(0);
		pDialog->SetMessage( "Do you want to Quit?" );
		pDialog->AddMenuItem( "OK", COMMAND_QUITGAME_OK, 0 );
		pDialog->AddMenuItem( "Cancel", COMMAND_QUITGAME_CANCEL, 0 );
		pDialog->SelectMenuItem( COMMAND_QUITGAME_CANCEL );
	}

	ShowPopupDialog();
}

//=============================================================================
// ScreenOnlineLobby::OnButtonPressed
//=============================================================================

void ScreenOnlineLobby::OnButtonPressed( int Button )
{
	switch( Button )
	{	
	case FluffControlMapper::SQUARE:
	{
		if( m_GameSettingsEnabled )
		{
			CPlayerAgent* pPlayerAgent = g_referee.GetLocalPlayerAgent();
			if( pPlayerAgent )
				pPlayerAgent->SetLobbyStatus( STATUS_SETTINGS );
			GotoScreen( "SCREEN_ID_GAMESETTING" );
		}
		break;
	}
	default:
		ScreenBaseClass::OnButtonPressed( Button );
		break;
	}

	SetAwayTime(0.0f);
}

//=============================================================================
// ScreenOnlineLobby::OnButtonReleased
//=============================================================================

void ScreenOnlineLobby::OnButtonReleased( int Button )
{
	ScreenBaseClass::OnButtonReleased( Button );

	SetAwayTime(0.0f);
}


//=============================================================================
// ScreenOnlineLobby::OnCommand
//=============================================================================

void ScreenOnlineLobby::OnCommand( int Command )
{
	switch( Command )
	{
	case ID_ONLINELOBBY_READY:
		OnCommandReady();
		break;
	case ID_ONLINELOBBY_FORCEREADY:
		OnCommandForceReady();
		break;
	case ID_ONLINELOBBY_CHANGETEAM:
		OnCommandChangeTeam();
		break;
	case ID_ONLINELOBBY_FRIENDS:
		OnCommandFriends();
		break;
	case ID_ONLINELOBBY_PLAYERS:
		OnCommandPlayers();
		break;
	case ID_ONLINELOBBY_INVITE:
		OnCommandInvite();
		break;
	}

	return;
}


//=============================================================================
// ScreenOnlineLobby::OnCommandReady
//=============================================================================

void ScreenOnlineLobby::OnCommandReady( void )
{
	g_messageDispatcher.SendMessageToAll( "PlayerReadyUpToggle", NULL, INVALID_OBJECT_HANDLE );
}

//=============================================================================
// ScreenOnlineLobby::OnCommandReady
//=============================================================================

void ScreenOnlineLobby::OnCommandForceReady( void )
{
	g_messageDispatcher.SendMessageToAll( "PlayerForceReadyUp", NULL, INVALID_OBJECT_HANDLE );
}

//=============================================================================
// ScreenOnlineLobby::OnCommandChangeTeam
//=============================================================================

void ScreenOnlineLobby::OnCommandChangeTeam( void )
{
	ScreenPopupDialog* pDialog = GetPopupDialog();
	CPlayerAgent* pAgent = g_referee.GetLocalPlayerAgent();

	if( pDialog && pAgent )
	{
		Referee &ref = g_referee;
		const CTeamingModel &teaming = ref.GetTeamingModel();

		uint8 teamA = teaming.FindTeamInGroupAlpha();
		uint8 teamB = teaming.FindTeamInGroupBeta();
		uint8 desiredTeam = INVALID_TEAM;

		if( pAgent->GetRequestedTeam() == teamA )
			desiredTeam = teamB;
		else if( pAgent->GetRequestedTeam() == teamB )
			desiredTeam = teamA;

		if( g_referee.GetTeamingModel().TeamHasOpenSlot( desiredTeam ) )
		{
			pDialog->Clear();
			pDialog->SetTitle( "Changing Teams..." );
			pDialog->SetMessage( "Would you like to switch to the other team?" );
			pDialog->AddMenuItem( "OK", COMMAND_TEAMCHANGE_OK, 0 );
			pDialog->AddMenuItem( "Cancel", COMMAND_TEAMCHANGE_CANCEL, 0 );
		}
		else
		{
			pDialog->Clear();
			pDialog->SetTitle( "Team Full" );
			pDialog->SetMessage( "That team has no available slots!" );
			pDialog->AddMenuItem( "OK", COMMAND_TEAMCHANGE_CANCEL, 0 );
		}
	}

	ShowPopupDialog();
}

//=============================================================================
// ScreenOnlineLobby::OnCommandFriends
//=============================================================================

void ScreenOnlineLobby::OnCommandFriends( void )
{
	CPlayerAgent* pPlayerAgent = g_referee.GetLocalPlayerAgent();

	if( pPlayerAgent )
		pPlayerAgent->SetLobbyStatus( STATUS_SETTINGS );

#if defined( _XBOX )
	OnlineUI::UIXFriends::getSingletonPtr()->setAllowGameInvites( true );
	OnlineUI::UIXFriends::getSingletonPtr()->start();
#elif defined( PS2 ) || defined( DIRECTX_PC )
	OnlineUI::Friends::getSingletonPtr()->start();
	//OnlineUI::Friends::getSingletonPtr()->setDisplayPopup( true ); // display popup of options
#endif
}

//=============================================================================
// ScreenOnlineLobby::OnCommandPlayers
//=============================================================================

void ScreenOnlineLobby::OnCommandPlayers( void )
{
	CPlayerAgent* pPlayerAgent = g_referee.GetLocalPlayerAgent();

	if( pPlayerAgent )
		pPlayerAgent->SetLobbyStatus( STATUS_SETTINGS );

#if defined( _XBOX )
	OnlineUI::UIXPlayers::getSingletonPtr()->setShowCurrentPlayers(true);
	OnlineUI::UIXPlayers::getSingletonPtr()->start();
#elif defined( PS2 ) || defined( DIRECTX_PC )
	OnlineUI::Players::getSingletonPtr()->start(true);
	//OnlineUI::Players::getSingletonPtr()->setDisplayPopup( true ); // display popup of options
#endif // !_XBOX
}


//=============================================================================
// ScreenOnlineLobby::OnCommandInvite
//=============================================================================

void ScreenOnlineLobby::OnCommandInvite( void )
{
	CPlayerAgent* pPlayerAgent = g_referee.GetLocalPlayerAgent();

	if( pPlayerAgent )
		pPlayerAgent->SetLobbyStatus( STATUS_SETTINGS );

#if defined( PS2 ) || defined( DIRECTX_PC )
	OnlineUI::InvitePlayerToMatch::getSingletonPtr()->start();
#endif
}

//=============================================================================
// ScreenOnlineLobby::OnDialogCommand
//=============================================================================

void ScreenOnlineLobby::OnDialogCommand( int Command )
{
	switch( Command )
	{
	case COMMAND_QUITGAME_OK:
		g_messageDispatcher.SendMessageToAll("Quit", NULL, INVALID_OBJECT_HANDLE);
		break;

	case COMMAND_QUITGAME_CANCEL:
		EndScreen(0);
		break;

	case COMMAND_TEAMCHANGE_OK:
		EndScreen(0);
		ChangeTeam();
		SelectButton( ID_ONLINELOBBY_READY );
		break;

	case COMMAND_TEAMCHANGE_CANCEL:
		EndScreen(0);
		break;

	case COMMAND_INVALIDTEAMS_OK:
		EndScreen(0);
		break;
	}

	return;
}

//=============================================================================
// Handle keyboard input
//=============================================================================

void ScreenOnlineLobby::OnKeyboardInput( const char* pString )
{
	g_messageDispatcher.SendMessageToAll( "InviteByName", (void *)pString, INVALID_OBJECT_HANDLE );
}

//=============================================================================
// 
//=============================================================================

void ScreenOnlineLobby::OnKeyboardCancel( void )
{
	g_messageDispatcher.SendMessageToAll( "CancelInviteByName", NULL, INVALID_OBJECT_HANDLE );
}

//=============================================================================
// ScreenOnlineLobby::RefreshLobby
//=============================================================================

void ScreenOnlineLobby::RefreshLobby( void )
{
	Referee& ref = g_referee;
	Referee::EPhase refPhase = ref.GetPhase();
	CTeamingModel &teaming = ref.GetTeamingModel();
	CPlayerAgent* pAgent;
	int Team;
	int Alphas = 0;
	int Betas = 0;
	int Slot = 0;

	if( ( refPhase == Referee::POPULATING ) || ( refPhase == Referee::REPOPULATE ) )
	{
		ShowCountdownTimer( true );
		SetCountdownTimer( ( int ) ( 0.001f * ( float ) ( g_session.GetSessionTimeMS() - m_PopulateStartTime ) ) );
	}
	else
		ShowCountdownTimer( false );

	if( ref.IsGhost( ref.GetLocalPlayerAgentId() ) )
	{
		ShowMatchInProgress( true );

		int timeRem;
		if( refPhase == Referee::PLAYING )
			timeRem = ref.IsTimedPhase() ? g_referee.GetPhaseCountdown() : -1;
		else
		{
			const char* timeLimitString = g_configmgr.GetValue( "timeLimit" );
			timeRem = ( timeLimitString != NULL ) ? atoi( timeLimitString ) : -1;
		}

		if (timeRem < 0)
			SetTimeRemaining( "??:??" );
		else
			SetTimeRemaining( ::Printf( "%2d:%02d", timeRem / 60, timeRem % 60 ) );
	}
	else
		ShowMatchInProgress( false );

	EReadyState newReadyState;
	if( ref.IsReady( ref.GetLocalPlayerAgentId() ) )
	{
		uint8 team;
		if( ( refPhase == Referee::READY ) ||
				!g_referee.GetTeam( ref.GetLocalPlayerAgentId(), team ) )
			newReadyState = REVERSABLE_READY;
		else
			newReadyState = IRREVERSABLE_READY;
	}
	else
		newReadyState = NOT_READY;

	if( newReadyState != m_WeAreReady )
	{
		m_WeAreReady = newReadyState;
		switch( m_WeAreReady )
		{
		 case NOT_READY:
			EnableAllButtons( true, true );
			SelectButton( ID_ONLINELOBBY_READY );
			break;
		 case REVERSABLE_READY:
			EnableAllButtons( true, false );
			SelectButton( ID_ONLINELOBBY_READY );
			break;
		 case IRREVERSABLE_READY:
			EnableAllButtons( false, false );
			break;
		}
	}

	STATUS StatusIcon;
	FRIENDICON FriendIcon;
	SPEAKERTYPE SpeakerType;

	uint8 Player = INVALID_PARTICIPANT;
	while ( ( Player = ref.GetNextPlayer( Player ) ) != INVALID_PARTICIPANT )
	{
		pAgent = ref.GetPlayerAgent( Player );
		ASSERT_PTR( pAgent );
		Team = ref.GetTeam( Player );

		if( ref.GetTeamingModel().IsTeamInGroupAlpha( Team ) )
			Slot = Alphas++;
		else if( ref.GetTeamingModel().IsTeamInGroupBeta( Team ) )
			Slot = Betas++;
		else
		{
			ASSERT( false );
			continue;
		}

		OpenTeamSlot( Team, Slot );

		FriendIcon = FRIENDICON_NONE;
		StatusIcon = STATUS_NOTREADY;
		if( g_referee.IsParticipantVoiceReDirected(Player) )
			SpeakerType = SPEAKERTYPE_TVSET;
		else if( g_referee.IsParticipantHeadsetConnected(Player) )
			SpeakerType = SPEAKERTYPE_HEADSET;
		else
			SpeakerType = SPEAKERTYPE_DISABLED;

		if( ref.IsReady( Player ) )
			StatusIcon = STATUS_READY;
		else
			StatusIcon = (STATUS) pAgent->GetLobbyStatus();

		if( pAgent->IsSessionHost() )
			FriendIcon = FRIENDICON_HOST;
		else
		if( pAgent == ref.GetLocalPlayerAgent() )
			FriendIcon = FRIENDICON_SELF;
		else
		{
			// TODO: CHOOSE THE APPROPRIATE FRIEND ICON
			//       FRIENDICON_FRIEND
			//       FRIENDICON_SENT
			//       FRIENDICON_RECEIVED
		}

		SetPlayerName( Team, Slot, pAgent->GetDisplayName() );
		SetPlayerStatus( Team, Slot, StatusIcon );
		SetSpeakerType( Team, Slot, SpeakerType, g_referee.IsParticipantTalking(Player));
		SetFriendIcon( Team, Slot, FriendIcon );
		ShowRatingIcon( Team, Slot, pAgent->GetRank(), pAgent->GetStarRating() );
	}

	int alphaTeam = ref.GetTeamingModel().FindTeamInGroupAlpha();
	while ( Alphas < 8 )
	{
		Slot = Alphas++;
		CloseTeamSlot( alphaTeam, Slot );
	}
	int betaTeam = ref.GetTeamingModel().FindTeamInGroupBeta();
	while ( Betas < 8 )
	{
		Slot = Betas++;
		CloseTeamSlot( betaTeam, Slot );
	}

	int iOptionIndex;
	iOptionIndex = g_configmgr.GetOptionIndexByName( "gameModes" );
	if ( iOptionIndex >= 0 )
		SetGameType( g_configmgr.GetDisplayValue( iOptionIndex ) );

	iOptionIndex = g_configmgr.GetOptionIndexByName( "gameMap" );
	if ( iOptionIndex >= 0 )
		SetLevelName( g_configmgr.GetDisplayValue( iOptionIndex ) );

	iOptionIndex = g_configmgr.GetOptionIndexByName( "timeLimit" );
	if ( iOptionIndex >= 0 )
		SetTimeLimit( g_configmgr.GetDisplayValue( iOptionIndex ) );

	iOptionIndex = g_configmgr.GetOptionIndexByName( "scoreLimit" );
	if ( iOptionIndex >= 0 )
		SetScoreLimit( g_configmgr.GetDisplayValue( iOptionIndex ) );

	Text* pTeamName1 = GetTextSprite( ID_ONLINELOBBY_TEAMNAME, GetSprite( ID_ONLINELOBBY_TEAM2 ) );
	Text* pTeamName2 = GetTextSprite( ID_ONLINELOBBY_TEAMNAME, GetSprite( ID_ONLINELOBBY_TEAM1 ) );

	if( pTeamName1 )
		pTeamName1->SetText( teaming.GetTeamName(0) );

	if( pTeamName2 )
		pTeamName2->SetText( teaming.GetTeamName(1) );

	Text* pTeam1Wins = GetTextSprite( ID_ONLINELOBBY_WINS, GetSprite( ID_ONLINELOBBY_TEAM2 ) );
	Text* pTeam2Wins = GetTextSprite( ID_ONLINELOBBY_WINS, GetSprite( ID_ONLINELOBBY_TEAM1 ) );

	if( pTeam1Wins )
		pTeam1Wins->Printf( "%d Wins", g_referee.GetSessionTeamWins(0) );

	if( pTeam2Wins )
		pTeam2Wins->Printf( "%d Wins", g_referee.GetSessionTeamWins(1) );

	return;
}

void ScreenOnlineLobby::LobbyScreenRefresh::HandleMessage( void* i_data, ActorHandle i_sender, bool i_posted )
{
	// handle this message if you'd like to know when players connect/disconnect or change teams
}

void ScreenOnlineLobby::LobbyStartCount::HandleMessage( void* i_data, ActorHandle i_sender, bool i_posted )
{
	d_ref->m_PopulateStartTime = g_referee.GetCurrentPhaseStartTime();
}

void ScreenLobbyMenu::SetDescription( const char *pDescription )
{
	SetText( ID_LOBBYMENU_DESCRIPTION, pDescription );
}

void ScreenLobbyMenu::OnInitialize( void )
{
	ScreenBaseClass::OnInitialize();

	UseHighlighting( true );

	SetHelpRect( ID_LOBBYMENU_HELPRECT );
	SetInitialButton( ID_LOBBYMENU_GAMEMODE );
	SetNextButton( ID_LOBBYMENU_WEAPONS );
	SetNextButton( ID_LOBBYMENU_VOICESETTINGS );
	SetNextButton( ID_LOBBYMENU_OPTIONS );

	SetCursor( ID_LOBBYMENU_CURSOR );

	SetNextScreen( ID_LOBBYMENU_GAMEMODE, "SCREEN_ID_GAMESETTING" );
	SetNextScreen( ID_LOBBYMENU_WEAPONS, "SCREEN_ID_WEAPONS" );
	SetNextScreen( ID_LOBBYMENU_VOICESETTINGS, "SCREEN_ID_VOICESETTING" );
#ifdef DIRECTX_PC
	SetNextScreen( ID_LOBBYMENU_OPTIONS, "SCREEN_ID_OPTIONS_PC" );
	// hide the ps2 help stuff
	GetSprite(ID_LOBBYMENU_HELPRECT)->HideAll();
#else
	SetNextScreen( ID_LOBBYMENU_OPTIONS, "SCREEN_ID_OPTIONS" );
#endif
}

void ScreenLobbyMenu::OnScreenIntro( void )
{
	ScreenBaseClass::OnScreenIntro();

	Printf( ID_LOBBYMENU_PROFILENAME, g_translator.Translate( "Current Profile: %s" ), g_playerProfile.GetAccountName() );

	CPlayerAgent* pPlayerAgent = g_referee.GetLocalPlayerAgent();

	if( pPlayerAgent )
		pPlayerAgent->SetLobbyStatus( ScreenOnlineLobby::STATUS_SETTINGS );

	return;
}

void ScreenLobbyMenu::OnButtonCancel( void )
{
	SaveProfile();
}

void ScreenLobbyMenu::OnSaveGameResult( int Result )
{
	switch( Result )
	{
	case PR_FAILED:
		break;

	default:
		ScreenBaseClass::OnButtonCancel();
		break;
	}

	return;
}

void ScreenLobbyMenu::OnButtonSelect( Button* pButton, Sprite* pSprite )
{
	ScreenBaseClass::OnButtonSelect( pButton, pSprite );

	const char* pDescription = 0;

	if( pButton )
	{
		switch( pButton->GetButtonId() )
		{
		case ID_LOBBYMENU_GAMEMODE:
			pDescription = "View current game settings.";
			break;

		case ID_LOBBYMENU_WEAPONS:
			pDescription = "Create your loadout.";
			break;

		case ID_LOBBYMENU_VOICESETTINGS:
			pDescription = "Customize your voice parameters.";
			break;

		case ID_LOBBYMENU_OPTIONS:
			pDescription = "Change your game options, button configuration and analog stick settings.";
			break;
		}
	}
	
	SetDescription( pDescription );
}
