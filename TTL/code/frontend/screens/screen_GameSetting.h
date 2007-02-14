#ifndef _SCREEN_GAMESETTING_H_
#define _SCREEN_GAMESETTING_H_

#include "frontend/screens/screen_BaseClass.h"

class ScreenGameSetting : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenGameSetting );

	public:
		ScreenGameSetting();
		GuiListBox* GetListBox( void );
		void SetReadOnly( bool ReadOnly );
		bool GetReadOnly( void );
		void GetCursorPos( int n, float& x, float &y );
		void UseDefaultSettings( void );

	protected:
		virtual void OnInitialize( void );
		virtual void OnScreenIntro( void );
		virtual void OnScreenExit( void );
		virtual void OnCursorUp( void );
		virtual void OnCursorDown( void );
		virtual void OnCursorLeft( void );
		virtual void OnCursorRight( void );
		virtual void OnButtonOK( void );
		virtual void OnButtonCancel( void );
		virtual void OnButtonPressed( int Button );
		virtual void OnButtonCycle( int N );
		virtual void OnScreenUpdate( void );
		virtual void OnDialogCommand( int Command );
		virtual void OnPopupShow( void );
		virtual void OnPopupHide( void );

		virtual void RebuildOptionList( void );
		
		bool m_InitMenu;
		bool m_ReadOnly;

		CMessageOwner	m_messageOwner;
		REF_SEND_MESSAGE_HANDLER( GameOptionsChanged, ScreenGameSetting )	m_GameOptionsChangedHandler;
};

#endif // _SCREEN_GAMESETTING_H_
