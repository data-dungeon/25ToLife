#ifndef SCREEN_MAINMENU_H
#define SCREEN_MAINMENU_H

#include "frontend/screens/screen_BaseClass.h"

//=============================================================================
// ScreenMainMenu
//=============================================================================

class ScreenMainMenu : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenMainMenu );

	public:
		ScreenMainMenu( void );
		virtual ~ScreenMainMenu();

		void SetDescription( const char* pDescription );
		void SignOut( void );

	enum eStates
	{
		STATE_IDLE = 0,
#if !defined(PS2)
		STATE_GET_NETWORK_CONFIG,
		STATE_START_CONNECTION,
		STATE_INITIATE_CONNECTION,
		STATE_WAIT_FOR_CONNECTION,
		STATE_CONNECTION_VALID,
		STATE_CONNECTION_FAILED,
#endif
		STATE_WAIT,
	};

	protected:
		virtual void OnInitialize( void );
		virtual void OnScreenIntro( void );
		virtual void OnScreenUpdate( void );
		virtual void OnButtonCancel( void );
		virtual void OnButtonPressed( int Button );
		virtual void OnPopupShow( void );
		virtual void OnPopupHide( void );
		virtual void OnDialogCommand( int Command );
		virtual void OnCommand( int Command );
		void OnCommandSinglePlayer( void );
#if !defined(PS2)
		void OnCommandMultiPlayerOnline( void );
		void OnCommandMultiPlayerNetwork( void );
#endif
		void OnCommandSignOut( void );

	#ifdef _XBOX
		void OnCommandFriends( void );
		void OnCommandAppearOffline( void );
		void OnCommandSignIn( void );

		bool m_XboxLiveEnabled;
	#endif

		int				m_state;
		int				m_modeSelected;
		unsigned int	m_connectionTimeOut;
};

//=============================================================================
// ScreenDifficulty
//=============================================================================

class ScreenDifficulty : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenDifficulty );

	protected:
		virtual void OnInitialize( void );
		virtual void OnScreenIntro( void );
		virtual void OnScreenExit( void );
		virtual void OnButtonCancel( void );
		virtual void OnCommand( int Command );
		virtual void OnPopupShow( void );
		virtual void OnPopupHide( void );
};


//=============================================================================
// ScreenLevelSelect
//=============================================================================
#ifndef DIRECTX_PC
class ScreenLevelSelect : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenLevelSelect );

	public:
		GuiListBox* GetListBox( void	);

		void LaunchSinglePlayer( int Level );

	protected:
		virtual void OnInitialize( void );
		virtual void OnScreenIntro( void );
		virtual void OnScreenExit( void );
		virtual void OnCursorUp( void );
		virtual void OnCursorDown( void );
		virtual void OnCursorUpdate( void );
		virtual void OnButtonOK( void );
		virtual void OnPopupShow( void );
		virtual void OnPopupHide( void );

		GuiListBox* m_pListBox;
};
#endif // DIRECTX_PC


#endif // SCREEN_MAINMENU_H