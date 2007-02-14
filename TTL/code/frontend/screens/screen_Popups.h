#ifndef _SCREEN_POPUPS_H_
#define _SCREEN_POPUPS_H_

#include "frontend/screens/screen_BasePopup.h"

class ScreenPopupDialog : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenPopupDialog );

	public:
		ScreenPopupDialog( void );

		void Clear( void );
		void SetDialogId( int DialogId );
		int GetDialogId( void );
		void SetTitle( const char* pTitle );
		void SetMessage( const char* pMessage );
		void SetCancelCommand( int CancelCommand );
		int GetCancelCommand( void );
		void ClearMenu( void );
		void AddMenuItem( const char* pText, int CommandId, void* pParam );
		void SetMenuItemText( const char* pText, int CommandId );
		void SelectMenuItem( int CommandId );
		int CurrentSelection( void );
		
	protected:
		virtual void OnInitialize( void );
		virtual void OnCursorUp( void );
		virtual void OnCursorDown( void );
		virtual void OnButtonOK( void );
		virtual void OnButtonCancel( void );
		virtual void OnScreenIntro( void );
		virtual void OnScreenExit( void );
		virtual void OnScreenUpdate( void );

		int m_DialogId;
		int m_CancelCommand;
};

class ScreenErrorDialog : public ScreenPopupDialog
{
	DECLARE_CLASSOBJECT( ScreenErrorDialog );

	protected:
		virtual void OnButtonCancel( void ) {} // eat it
};

class ScreenErrorMessage : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenErrorMessage );

	public:
		virtual void SetErrorMessage( const char* pMessage );

	protected:
		virtual void OnInitialize( void );
		virtual void OnButtonCancel( void );
};

class ScreenPleaseWait : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenPleaseWait );

	public:
		virtual void SetMessage( const char* pMessage );

	protected:
		virtual void OnInitialize( void );
		virtual void OnButtonCancel( void );
};

class ScreenConnectingToSession : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenConnectingToSession );

	public:
		ScreenConnectingToSession( void );
		virtual void SetMessage( const char* pMessage );
		virtual void SetSessionDescription( const char* pDescription );

	protected:
		virtual void OnButtonCancel( void );
		virtual void OnScreenIntro( void );
		virtual void OnScreenRefresh( void );

		uint32 m_StartTime;
};

#endif // _SCREEN_POPUPS_H_
