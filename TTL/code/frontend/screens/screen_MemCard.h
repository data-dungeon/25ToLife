#ifndef _SCREEN_MEMCARD_H_
#define _SCREEN_MEMCARD_H_

#include "frontend/screens/screen_BaseClass.h"

class ScreenMemCard : public ScreenBaseClass
{
		DECLARE_CLASSOBJECT( ScreenMemCard );

	public:
		void SetTitle( const char* pTitle );
		void SetMessage( const char* pMessage );
		int AddMenuItem( const char* pText, int id );
		void SelectMenuItem( int id );
		void SetResult( int Result );
		int GetResult( void );
		void Reset( void );

		GuiListBox* GetListBox( void );

	protected:
		virtual void OnInitialize( void );
		virtual void OnScreenIntro( void );
		virtual void OnScreenExit( void );
		virtual void OnCursorUp( void );
		virtual void OnCursorDown( void );
		virtual void OnButtonOK( void );
		virtual void OnButtonCancel( void );
		//virtual float GetScreenExitTime( void ) { return 0.0f; }

		int m_Result;
};

#endif // _SCREEN_MEMCARD_H_