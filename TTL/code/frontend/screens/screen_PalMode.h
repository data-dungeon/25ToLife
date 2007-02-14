#ifndef _SCREEN_PALMODE_H_
#define _SCREEN_PALMODE_H_

#include "frontend/screens/screen_BaseClass.h"

class ScreenPalMode : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenPalMode );

	protected:
		virtual void OnInitialize( void );
		virtual void OnCommand( int Command );
		void OnCommandTestPal60( void );
		void OnCommandUsePal60( void );
		void OnCommandUsePal50( void );
};


class ScreenTestPal60 : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenTestPal60 );

	protected:
		virtual void OnScreenIntro( void );
		virtual void OnScreenUpdate( void );
		virtual void OnButtonPressed( int Button );

		uint32 m_TimeOut;
};

#endif // _SCREEN_PALMODE_H_
