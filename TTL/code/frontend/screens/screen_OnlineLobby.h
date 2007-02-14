#ifndef SCREEN_ONLINELOBBY_H
#define SCREEN_ONLINELOBBY_H

#include "frontend/screens/screen_BaseClass.h"
#include "frontend/screens/screen_OnlineChat.h"
#include "main/PlayerProfile.h"

//=============================================================================
// ScreenOnlineLobby
//=============================================================================

#define LOBBY_MAX_PLAYERS	(8)

class ScreenOnlineLobby : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenOnlineLobby );

	public:
		typedef enum
		{
			STATUS_UNDEFINED,
			STATUS_READY,
			STATUS_NOTREADY,
			STATUS_CHAT,
			STATUS_SETTINGS,
			STATUS_IDLE,
		}
		STATUS;

		typedef enum
		{
			SPEAKERTYPE_NONE,
			SPEAKERTYPE_HEADSET,
			SPEAKERTYPE_TVSET,
			SPEAKERTYPE_DISABLED,
		}
		SPEAKERTYPE;

		typedef enum
		{
			FRIENDICON_NONE,
			FRIENDICON_SELF,
			FRIENDICON_FRIEND,
			FRIENDICON_SENT,
			FRIENDICON_RECEIVED,
			FRIENDICON_HOST,
		}
		FRIENDICON;

		ScreenOnlineLobby( void );
		virtual ~ScreenOnlineLobby();

		void OpenTeamSlot( int Team, int Slot );
		void CloseTeamSlot( int Team, int Slot );
		void ClearTeamSlot( int Team, int Slot );
		Sprite* GetTeamSlot( int Team, int Slot );
		Text* GetTextPlayerName( int Team, int Slot );
		Text* GetTextPlayerReady( int Team, int Slot );
		void SetPlayerName( int Team, int Slot, const char *pPlayerName );
		void ShowRatingIcon( int Team, int Slot, int Rank, int Rating );
		void ShowCountdownTimer( bool Show );
		void SetCountdownTimer( int N );

		// team 

		// status icons
		void SetPlayerStatus( int Team, int Slot, STATUS Status );
		Sprite* GetStatusIcon( STATUS Status );

		// speaker icons
		void SetSpeakerType( int Team, int Slot, SPEAKERTYPE SpeakerType, bool Bright );
		Sprite* GetSpeakerIcon( SPEAKERTYPE SpeakerType, bool Bright );

		// friend icons
		void SetFriendIcon( int Team, int Slot, FRIENDICON FriendIcon );
		Sprite* GetFriendIcon( FRIENDICON FriendIcon );

		void SetSessionParam( int ParamId, const char *pParamText );
		void SetGameType( const char* pGameType );
		void SetLevelName( const char* pLevelName );
		void SetTimeLimit( const char* pTimeLimit );
		void SetTimeLimit( int Minutes, int Seconds );
		void SetScoreLimit( const char* pScoreLimit );
		void SetScoreLimit( int Seconds );
		void ShowMenu( bool Show );
		void ShowLoadingText( bool Show );
		void SetLoadingText( const char* pText );
		void ShowSettingsChanged( bool Show );
		void ShowMatchInProgress( bool Show );
		void SetTimeRemaining( const char* pTimeText );
		void EnableAllButtons( bool ReadyEnabled, bool OthersEnabled );

		void SelectLoadout( int Slot );
		void SelectNextLoadout( void );
		void SelectPrevLoadout( void );

		int GetLocalPlayer( void );
		void ChangeTeam( void );
		void CheckButtonStatus( bool& IsPlayingMultiplayerOnline, bool& IsHostOfMultiplayerOnline );

		void  SetAwayTime( float AwayTime );
		float GetAwayTime( void );
		bool  IsPlayerIdle( void );


protected:
		virtual void OnInitialize( void );
		virtual void OnScreenIntro( void );
		virtual void OnScreenExit( void );
		virtual void OnScreenUpdate( void );
		virtual void OnCursorLeft( void );
		virtual void OnCursorRight( void );
		virtual void OnButtonCancel( void );
		virtual void OnButtonPressed( int Button );
		virtual void OnButtonReleased( int Button );
		virtual void OnCommand( int Command );
		virtual void OnCommandReady( void );
		virtual void OnCommandForceReady( void );
		virtual void OnCommandChangeTeam( void );
		virtual void OnCommandFriends( void );
		virtual void OnCommandPlayers( void );
		virtual void OnCommandInvite( void );
		virtual void OnDialogCommand( int Command );
		virtual void OnKeyboardInput( const char* pString );
		virtual void OnKeyboardCancel( void );

		void RefreshLobby( void );
		void RefreshChat( void );

		Sprite* m_pTeam1Group;
		Sprite* m_pTeam2Group;
		Sprite* m_pTeam1Sprites[LOBBY_MAX_PLAYERS];
		Sprite* m_pTeam2Sprites[LOBBY_MAX_PLAYERS];
		char m_szTextChat[512];
		int  m_lastVariant;
		float m_AwayTime;
		bool m_GameSettingsEnabled;
		int32 m_PopulateStartTime;
		enum EReadyState
		{
			NOT_READY,
			REVERSABLE_READY,
			IRREVERSABLE_READY
		} m_WeAreReady;

		ChatWindow m_Chat;

		CMessageOwner	m_messageOwner;
		REF_SEND_MESSAGE_HANDLER( LobbyScreenRefresh, ScreenOnlineLobby )	m_LobbyScreenRefreshHandler;
		REF_SEND_MESSAGE_HANDLER( LobbyStartCount, ScreenOnlineLobby )	m_LobbyStartCountHandler;
};

class ScreenLobbyMenu : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenLobbyMenu );

	public:
		void SetDescription( const char* pDescription );

	protected:
		virtual void OnInitialize( void );
		virtual void OnScreenIntro( void );
		virtual void OnButtonCancel( void );
		virtual void OnSaveGameResult( int Result );
		virtual void OnButtonSelect( Button* pButton, Sprite* pSprite );
};

#endif // SCREEN_ONLINELOBBY_H
