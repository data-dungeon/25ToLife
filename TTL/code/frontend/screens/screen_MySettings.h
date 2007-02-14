#ifndef _SCREEN_MYSETTINGS_H_
#define _SCREEN_MYSETTINGS_H_

#include "frontend/screens/screen_BaseClass.h"

enum {
	MYSETTINGS_IDLE,
	MYSETTINGS_SAVE,
	MYSETTINGS_SAVE_RESULT,
};


class ScreenMySettings : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenMySettings );

	public:
		virtual void SetDescription( const char* pDescription );

	protected:
		virtual void OnInitialize( void );
		virtual void OnScreenIntro( void );
		virtual void OnButtonSelect( Button* pButton, Sprite* pSprite );
		virtual void OnButtonCancel( void );
		virtual void OnSaveGameResult( int Result );
		virtual void OnCommand( int Command );

		int	m_MemCardState;
		char	m_Profile[64];
};

#endif // _SCREEN_MYSETTINGS_H_
