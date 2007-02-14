#ifndef _KEYBOARD_CHAT_H_
#define _KEYBOARD_CHAT_H_

#include "GameHelper/TextChat.h"

class KeyboardChat
{
	public:
		KeyboardChat( void );
		~KeyboardChat( void );

	public:
		void Initialize( void );
		void Terminate( void );
		void SendMessage( const char* pMessage );
		const char* GetChat( void );
		void AddChat( const char* pChat );
		void ClearChat( void );
		int MaxChat( void );
		int EndChat( void );
		int MaxSendChat( void );
		const char* GetSendChat( void );

		char m_SendChat[512];
		char m_Chat[4096];
		int m_EndChat;

		CMessageOwner	m_MessageOwner;
		SEND_MESSAGE_HANDLER( KeyBoardTextChat ) m_KeyboardTextChatHandler;
		BASIC_MESSAGE_HANDLER( TextChat, MESSAGE_STRING ) m_TextChatHandler;
		REF_BASIC_MESSAGE_HANDLER( TextFiltered, MESSAGE_VALUE, KeyboardChat ) m_TextFilteredHandler;
};

extern KeyboardChat g_KeyboardChat;

#endif // _KEYBOARD_CHAT_H_