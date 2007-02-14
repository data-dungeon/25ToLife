#ifndef SCREEN_CREATESESSION_H
#define SCREEN_CREATESESSION_H

#include "frontend\screens\screen_BaseClass.h"

//=============================================================================
// ScreenCreateSession
//=============================================================================

class ScreenCreateSession : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenCreateSession );

	public:
		ScreenCreateSession( void );
		virtual ~ScreenCreateSession();

		void SetValidateMatchName( bool validateMatchName ) { _validateMatchName = validateMatchName; }
		void SetMatchName( const char* matchName );
	protected:
		virtual void OnInitialize( void );
		virtual void OnPrevScreen( Screen* pPrevScreen );
		virtual void OnScreenIntro( void );
		virtual void OnScreenExit( void );
		virtual void OnKeyboardInput( const char* pString );
		virtual void OnKeyboardCancel( void );
		virtual void OnScreenUpdate( void );
#ifdef DIRECTX_PC
		void OnButtonPressed( int Button );
#endif

private:
		bool _validateMatchName;
		char _matchName[ 16 ];
};

#endif // SCREEN_CREATESESSION_H
