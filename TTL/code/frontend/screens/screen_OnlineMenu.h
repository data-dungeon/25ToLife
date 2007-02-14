#ifndef SCREEN_ONLINEMENU_H
#define SCREEN_ONLINEMENU_H

#include "frontend/screens/screen_BaseClass.h"

// for the message-handling stuff
#include "GameHelper/Message.h"

enum {
	ONLINE_IDLE,
	ONLINE_SAVE,
	ONLINE_SAVE_RESULT,
};

class ScreenOnlineMenu : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenOnlineMenu );

	public:
		ScreenOnlineMenu( void );
		virtual ~ScreenOnlineMenu( void );

		void SetDescription( const char* pDescription );

	protected:
		virtual void OnInitialize( void );
		virtual void OnCommand( int Command );
		virtual void OnButtonCancel( void );
		virtual void OnButtonSelect( Button* pButton, Sprite* pSprite );
		virtual void OnScreenIntro( void );

		void OnCommandJoinSession( void );
		void OnCommandMySettings( void );
		CMessageOwner d_messageOwner;

		int	m_MemCardState;
		char	m_Profile[64];
};

#endif // SCREEN_ONLINEMENU_H
