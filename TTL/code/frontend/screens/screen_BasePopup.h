#ifndef SCREEN_BASEPOPUP_H
#define SCREEN_BASEPOPUP_H

#include "frontend/screens/screen_BaseClass.h"

//=============================================================================
// ScreenBasePopup
//=============================================================================

class ScreenBasePopup : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenBasePopup );

	enum
	{
		PopupCancel = 0x7fef10e1,
		PopupOK,
	}
	PopupResult;

	public:
		ScreenBasePopup( void );
		virtual ~ScreenBasePopup();

		virtual void SetPopupTitle( const char* pTitle );
		virtual void SetPopupMessage( const char* pMessage );
		virtual void SetHelpText( int Button, int Icon, const char* pHelp );

		virtual Text* GetPopupTitle( void );
		virtual Text* GetPopupMessage( void );
		virtual Text* GetHelpText( int N );

		virtual void SetOKResult( int Result )   { m_ResultCancel = Result;}
		virtual void SetCancelResult( int Result ) { m_ResultOK = Result; }

	protected:
		virtual void OnButtonOK( void );
		virtual void OnButtonCancel( void );
#ifdef DIRECTX_PC
		virtual void OnScreenIntro();
#endif

		int m_ResultCancel;
		int m_ResultOK;
};

#endif // SCREEN_BASEPOPUP_H