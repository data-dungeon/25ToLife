#ifndef _SCREEN_ONLINECHAT_H_
#define _SCREEN_ONLINECHAT_H_

#include "frontend/texteffects.h"
#include "frontend/screens/screen_BaseClass.h"

class ChatWindow
{
	public:
		ChatWindow( void );
		~ChatWindow( void );

	public:
		bool InitChat( int n );
		void InitItem( int n, Text* pText );
		bool InitChatWindow( Sprite* pChatWindow, int TextId );
		void KillChat( void );
		void ClearChat( void );
		void SetText( int Row, const char* pText );
		int GetMaxChat( void );
		void RefreshChat( void );

	public:
		Text** m_pChatLines;
		int m_MaxChat;
};

class ScreenChatWindow : public ScreenBaseClass
{
	protected:
		virtual void OnInitialize( void );
		virtual void OnScreenIntro( void );
		virtual void OnScreenExit( void );
		virtual void OnScreenUpdate( void );
		virtual void OnKeyboardInput( const char* pString );
		virtual void OnKeyboardCancel( void );
		virtual void OnIllegalMessage( void );
		virtual void OnDialogCommand( int Command );
		virtual void OnDialogCancel( void );

		ChatWindow m_Chat;

		// chat related message handlers
		CMessageOwner	m_MessageOwner;
		REF_BASIC_MESSAGE_HANDLER( TextFiltered, MESSAGE_VALUE, ScreenChatWindow ) m_TextFilteredHandler;
};

class ScreenChatRoom : public ScreenChatWindow
{
	DECLARE_CLASSOBJECT( ScreenChatRoom );

	protected:
		virtual void OnInitialize( void );
};

class ScreenChatInGame : public ScreenChatWindow
{
	DECLARE_CLASSOBJECT( ScreenChatInGame );

	protected:
		virtual void OnInitialize( void );
		virtual void OnScreenIntro( void );
};

#endif // _SCREEN_ONLINECHAT_H_