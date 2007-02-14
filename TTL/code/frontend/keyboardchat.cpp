#include "ttlpch.h"
#include "hud/stringex.h"
#include "frontend\KeyboardChat.h"
#include "main/Lobby.h"
#include "voice/NetZ/CVoiceChat.h"

KeyboardChat g_KeyboardChat;

KeyboardChat::KeyboardChat( void )
{
	ClearChat();
}

KeyboardChat::~KeyboardChat( void )
{
	Terminate();
}

void KeyboardChat::Initialize( void )
{
	ClearChat();

	m_KeyboardTextChatHandler.Initialize( KEYBOARD_TEXT_CHAT, MESSAGE_HANDLE_IF_MASTER, m_MessageOwner );
	m_TextChatHandler.Initialize( TEXT_CHAT, MESSAGE_HANDLE_IF_MASTER, m_MessageOwner );
	m_TextFilteredHandler.Initialize( FILTERED_TEXT_CHAT, MESSAGE_HANDLE_IF_MASTER, m_MessageOwner, this );
}

void KeyboardChat::Terminate( void )
{
	m_MessageOwner.UnregisterAll();

	ClearChat();
}

void KeyboardChat::SendMessage( const char* pChatMessage )
{
	// --- must check for voice banned accounts
	if( !g_lobby.IsLAN() && g_CVoiceChat.isVoiceChatBanned( ) )
		return;

	if( xstrlen(pChatMessage) > 0 )
	{
		const char* pAccountName = g_playerProfile.GetAccountName();

		if( !pAccountName )
			pAccountName = "Unnamed";

		const int maxlen = MaxSendChat();

		xstrncpyz( m_SendChat, pAccountName, maxlen );
		xstrncatz( m_SendChat, ": ", maxlen );
		xstrncatz( m_SendChat, pChatMessage, maxlen );


		if( g_lobby.IsLAN() )
		{
			// --- setup the STextChat structure now
			STextChat chatDataOut;
			chatDataOut.id = g_CVoiceChat.getUserId( );
			chatDataOut.flags = g_CVoiceChat.getUserFlags();
			chatDataOut.text = m_SendChat;
			g_messageDispatcher.SendMessageToSession( KEYBOARD_TEXT_CHAT, (void*)&chatDataOut, INVALID_OBJECT_HANDLE );
		}
		else
		{
			g_messageDispatcher.SendMessageToLocalHost( FILTER_TEXT_CHAT, (void*) m_SendChat, INVALID_OBJECT_HANDLE );
		}
	}

	return;
}

const char* KeyboardChat::GetChat( void )
{
	return m_Chat;
}

void KeyboardChat::AddChat( const char* pChat )
{
	if( pChat )
	{
		int len = strlen( pChat ) + 1;
		int maxchat = MaxChat();
		int endchat = EndChat();

		if( endchat + len < maxchat )
		{
			memcpy( &m_Chat[endchat], pChat, len );

			endchat += len;
		}
		else
		{
			int move = len - maxchat + endchat;
			int size = endchat - move;
			char* copyfrom = &m_Chat[move];
			char* copyto = &m_Chat[0];

			memmove( copyto, copyfrom, size );
			memcpy( &m_Chat[size], pChat, len );

			endchat = maxchat;
		}

		m_EndChat = endchat;
	}

	return;
}

void KeyboardChat::ClearChat( void )
{
	memset( m_Chat, 0, sizeof(m_Chat) );

	m_EndChat = 0;
}

int KeyboardChat::MaxChat( void )
{
	return sizeof(m_Chat)/sizeof(m_Chat[0]);
}

int KeyboardChat::EndChat( void )
{
	return m_EndChat;
}

int KeyboardChat::MaxSendChat( void )
{
	return sizeof(m_SendChat)/
			 sizeof(m_SendChat[0]);
}

const char* KeyboardChat::GetSendChat( void )
{
	return m_SendChat;
}

//==========================================================================
// KeyboardChat::TextChat::HandleMessage
//
//		This message hander responds to text chat ;)
//==========================================================================

void KeyboardChat::TextChat::HandleMessage( void* i_data, ActorHandle i_sender, bool i_posted )
{
	g_KeyboardChat.AddChat( (const char*) i_data );
}

//==========================================================================
// KeyboardChat::KeyBoardTextChat::HandleMessage
//
//		This message hander responds to text chat ;)
//==========================================================================

void KeyboardChat::KeyBoardTextChat::HandleMessage( void* i_data, ActorHandle i_sender, bool i_posted )
{
	STextChat *dataIn = ( STextChat * )i_data;

	if( !g_lobby.IsLAN() && ( g_CVoiceChat.isVoiceChatBanned( ) || g_CVoiceChat.isPlayerMuted( dataIn->id, dataIn->flags ) ) )
		return;

	g_KeyboardChat.AddChat( (const char*)dataIn->text );
}

//==========================================================================
// FrontEnd::TextFiltered::HandleMessage
//
//		This message handler responds to the text being filtered or not.
//
//		There is a message handler somewhere out in space that responds to a
//		previous request to filter some text.
//
//		If the filter is successful and  the text is allowed to be displayed,
//		we'll receive this message and i_data will be true.  We'll then
//    forward that message on to the session by sending the TextChat message.
//
//		If the filter fails and the message is not allowed because of
//		(presumably) offensive content, we'll receive this message and i_data
//		will be false.  In that case we won't actually do anything; we'll let
//		the chat window screen handlers respond to that message and decide
//		what to do (display a warning, usually).
//==========================================================================

void KeyboardChat::TextFiltered::HandleMessage( void* i_data, ActorHandle i_sender, bool i_posted )
{
	bool filtered = ( i_data == 0 ? false : true );

	if( !g_lobby.IsLAN() && g_CVoiceChat.isVoiceChatBanned( ) )
		return;

	if( !filtered )
	{
		// --- setup the STextChat structure now
		STextChat chatDataOut;
		chatDataOut.id = g_CVoiceChat.getUserId();
		chatDataOut.flags = g_CVoiceChat.getUserFlags();
		chatDataOut.text = ( char *)d_ref->GetSendChat();

		g_messageDispatcher.SendMessageToSession( KEYBOARD_TEXT_CHAT, (void*)&chatDataOut, INVALID_OBJECT_HANDLE );
	}

	return;
}