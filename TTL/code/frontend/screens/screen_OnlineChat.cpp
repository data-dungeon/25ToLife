#include "TTLPCH.h"
#include "frontend/frontend.h"
#include "frontend/keyboardchat.h"
#include "frontend/screens/screen_OnlineLobby.h"
#include "frontend/screens/screen_OnlineChat.h"
#include "data/screens/NetworkGames.h"

#ifdef _XBOX
#include "osi/xbox/osixbox.hpp"
#endif

enum
{
	COMMAND_CHAT_OK = 0x100,
};

ChatWindow::ChatWindow( void )
{
	m_pChatLines = 0;
	m_MaxChat = 0;
}

ChatWindow::~ChatWindow( void )
{
	KillChat();
}

bool ChatWindow::InitChat( int n )
{
	KillChat();

	m_pChatLines = new Text*[n];

	if( !m_pChatLines )
		return false;

	for( int i = 0; i < n; i++ )
		m_pChatLines[i] = 0;

	m_MaxChat = n;

	return true;
}

void ChatWindow::InitItem(	int n, Text* pText )
{
	if( m_pChatLines && n < m_MaxChat )
		m_pChatLines[n] = pText;

	if( pText )
		pText->SetText(0);

	return;
}

bool ChatWindow::InitChatWindow( Sprite* pChatWindow, int TextId )
{
	if( !pChatWindow )
		return false;

	int n = pChatWindow->GetNumChildren( TextId );

	if( n < 1 )
		return false;

	if( !InitChat(n) )
		return false;

	Text* pChatRow = (Text*) pChatWindow->Child( TextId );

	for( int i = 0; i < n; i++ )
	{
		if( pChatRow )
		{
			InitItem( i, pChatRow );
			pChatRow = pChatRow->Next( TextId );
		}
	}

	return true;
}

void ChatWindow::KillChat( void )
{
	delete [] m_pChatLines;
	m_pChatLines = 0;
}

void ChatWindow::ClearChat( void )
{
	Text* pChatLine;
	int n = m_MaxChat;
	int i;

	for( i = 0; i < n; i++ )
	{
		pChatLine = m_pChatLines[i];

		if( pChatLine )
			pChatLine->SetText("");
	}

	return;
}

void ChatWindow::SetText( int Row, const char* pText )
{
	if( m_pChatLines && Row < m_MaxChat )
	{
		if( m_pChatLines[Row] )
			m_pChatLines[Row]->SetText( pText );
	}

	return;
}

int ChatWindow::GetMaxChat( void )
{
	return m_MaxChat;
}

void ChatWindow::RefreshChat( void )
{
	const char* chat = g_KeyboardChat.GetChat();
	int endchat = g_KeyboardChat.EndChat() - 1;
	int maxchat = GetMaxChat();
	int start;
	int c, ch;
	int row;

	row = 0;

	start = endchat;

	for( c = endchat-1; c >= 0; c-- )
	{
		ch = chat[c];
				
		if( ch == 0 || c == 0 )
		{
			row++;

			if( c == 0 )
			{
				start = c;
				break;
			}

			if( row >= maxchat )
			{
				start = c+1;
				break;
			}
		}
	}

	row = 0;

	for( c = start; c <= endchat; c++ )
	{
		ch = chat[c];

		if( ch == 0 )
		{
			SetText( row++, &chat[start] );

			start = c+1;
		}
	}

	for( row = row; row < maxchat; row++ )
		SetText( row, 0 );

	return;
}

//=============================================================================
// ScreenChatWindow::OnInitialize
//=============================================================================

void ScreenChatWindow::OnInitialize( void )
{
	ScreenBaseClass::OnInitialize();

	m_Chat.InitChatWindow( GetSprite( ID_CHAT_CHATWINDOW ), ID_CHAT_TEXTLINE );

	//==========================================================================
	// This message handler will respond to messages that were filtered because
	// of offensive content.
	//==========================================================================

	m_TextFilteredHandler.Initialize( "TextFiltered", MESSAGE_HANDLE_IF_MASTER, m_MessageOwner, this );
}

//=============================================================================
// ScreenChatWindow::OnScreenIntro
//=============================================================================

void ScreenChatWindow::OnScreenIntro( void )
{
	ScreenBaseClass::OnScreenIntro();

	ScreenKeyboard* pKeyboard = GetKeyboard();

	if( pKeyboard )
	{
		pKeyboard->Clear();
		pKeyboard->SetMaxString( 60 );
		pKeyboard->SetAutoHide( false );
	}

	CPlayerAgent* pPlayerAgent = g_referee.GetLocalPlayerAgent();

	if( pPlayerAgent )
		pPlayerAgent->SetLobbyStatus( ScreenOnlineLobby::STATUS_CHAT );

	ShowKeyboard();
}

//=============================================================================
// ScreenChatWindow::OnScreenExit
//=============================================================================

void ScreenChatWindow::OnScreenExit( void )
{
	ScreenBaseClass::OnScreenExit();

	ScreenKeyboard* pKeyboard = GetKeyboard();

	if( pKeyboard )
		pKeyboard->SetAutoHide( true );

	return;
}

//=============================================================================
// ScreenChatWindow::OnScreenUpdate
//=============================================================================

void ScreenChatWindow::OnScreenUpdate( void )
{
	ScreenBaseClass::OnScreenUpdate();

	m_Chat.RefreshChat();
}

//=============================================================================
// ScreenChatWindow::OnKeyboardInput
//=============================================================================

void ScreenChatWindow::OnKeyboardInput( const char* pString )
{
	g_KeyboardChat.SendMessage( pString );

	ScreenKeyboard* pKeyboard = GetKeyboard();

	if( pKeyboard )
		pKeyboard->Clear();

	return;
}

//=============================================================================
// ScreenChatWindow::OnKeyboardCancel
//=============================================================================

void ScreenChatWindow::OnKeyboardCancel( void )
{
	EndScreen(0);
}

void ScreenChatWindow::OnIllegalMessage( void )
{
	ScreenPopupDialog* pDialog = GetPopupDialog();

	if( pDialog )
	{
		pDialog->Clear();
#ifdef _XBOX
		OSIXbox* osix = (OSIXbox*) OSI::getSingletonPtr();
		if( osix && osix->isCreated() )
		{
			pDialog->SetMessage( osix->getErrorString() );
		}
		else
#endif
		pDialog->SetMessage( "The text entered contains offensive content." );
		pDialog->AddMenuItem( "OK", COMMAND_CHAT_OK, 0 );
		ShowPopupDialog();
	}

	return;
}

void ScreenChatWindow::OnDialogCommand( int Command )
{
	switch( Command )
	{
	case COMMAND_CHAT_OK:
		g_FrontEnd.RemoveActivePopup();
		ShowKeyboard();
		break;
	}

	return;
}

void ScreenChatWindow::OnDialogCancel( void )
{
	ShowKeyboard();
}

//==========================================================================
// FrontEnd::TextFiltered::HandleMessage
//
//		This message handler responds to the text being filtered or not
//==========================================================================

void ScreenChatWindow::TextFiltered::HandleMessage( void* i_data, ActorHandle i_sender, bool i_posted )
{
	bool filtered = i_data == 0 ? false : true;

	if( filtered && d_ref->GetActive() )
		d_ref->OnIllegalMessage();

	return;
}

//=============================================================================
// ScreenChatRoom::OnInitialize
//=============================================================================

void ScreenChatRoom::OnInitialize( void )
{
	ScreenChatWindow::OnInitialize();
}

//=============================================================================
// ScreenChatInGame::OnInitialize
//=============================================================================

void ScreenChatInGame::OnInitialize( void )
{
	ScreenChatWindow::OnInitialize();

	m_Chat.InitChatWindow( GetSprite( ID_CHATINGAME_FRAME ), ID_CHATINGAME_TEXTLINE );
}

//=============================================================================
// ScreenChatInGame::OnScreenIntro
//=============================================================================

void ScreenChatInGame::OnScreenIntro( void )
{
	ScreenChatWindow::OnScreenIntro();

	SetText( ID_CHATINGAME_PROFILENAME, g_playerProfile.GetAccountName() );
}
