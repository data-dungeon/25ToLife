#ifndef _SCREEN_TITLE_H_
#define _SCREEN_TITLE_H_

#include "frontend/texteffects.h"
#include "frontend/screens/screen_BaseClass.h"

enum	{
	TITLE_IDLE,
	TITLE_DETECT,
	TITLE_DETECT_RESULT,
};

enum {
	ATTRACT_WAITING,
	ATTRACT_RUNNING,
	ATTRACT_DEBOUNCE,
};

class ScreenTitle : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenTitle );

	protected:
		virtual void OnInitialize( void );
		virtual void OnScreenIntro( void );
		virtual void OnScreenExit( void );
		virtual void OnScreenUpdate(void);
		virtual float GetScreenIntroTime( void );
		virtual float GetScreenExitTime( void );
		virtual void OnButtonStart( void );
		virtual void OnButtonOK( void );
		virtual void OnButtonPressed( int Button );
		virtual void OnPopupShow( void );
		virtual void OnPopupHide( void );

#ifdef DIRECTX_PC
		virtual void OnButtonPressed( void );
#endif
		virtual void HandleInput( UIHandler* pHandler );

		int		m_MemCardState;	
		int		m_StartPressed;
		int		m_BlendMode;
		u32		m_attractModeTimer;
		int		m_attractState;
};

#endif // _SCREEN_TITLE_H_