#ifndef _SCREEN_XBOXLIVE_H_
#define _SCREEN_XBOXLIVE_H_

#include "hud/animator.h"
#include "frontend/screens/screen_BaseClass.h"

class ScreenXboxLiveLogin : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenXboxLiveLogin );

	public:
		void SetXboxLiveGamerTag( const char* pGamerTag );
		void SetXboxLiveStatusMessage( const char* pMessage );
		void SetXboxLiveStatusMessagef( const char* fmt, ... );

	protected:
		virtual void OnInitialize( void );
		virtual void OnTerminate( void );
		virtual void OnScreenIntro( void );
		virtual void OnScreenExit( void );
		virtual void OnScreenRefresh( void );

		float m_ScreenSizeShouldBe;
		float m_ScreenSizeWantsToBe;		
		Animator m_ScreenSizeAnim;
};

#endif // _SCREEN_XBOXLIVE_H_