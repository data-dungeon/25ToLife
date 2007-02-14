#include "TTLPCH.h"
#include "frontend/screens/screen_Popups.h"
#include "data/screens/Popups.h"

//=============================================================================
// ScreenPopupDialog::ScreenPopupDialog
//=============================================================================

ScreenPopupDialog::ScreenPopupDialog( void )
{
	m_DialogId = 0;
}

//=============================================================================
// ScreenPopupDialog::SetDialogId
//=============================================================================

void ScreenPopupDialog::SetDialogId( int DialogId )
{
	m_DialogId = DialogId;
}

//=============================================================================
// ScreenPopupDialog::GetDialogId
//=============================================================================

int ScreenPopupDialog::GetDialogId( void )
{
	return m_DialogId;
}

//=============================================================================
// ScreenPopupDialog::SetCancelCommand
//=============================================================================

void ScreenPopupDialog::SetCancelCommand( int CancelCommand )
{
	m_CancelCommand = CancelCommand;
}

//=============================================================================
// ScreenPopupDialog::GetCancelCommand
//=============================================================================

int ScreenPopupDialog::GetCancelCommand( void )
{
	return m_CancelCommand;
}

//=============================================================================
// ScreenPopupDialog::Clear
//=============================================================================

void ScreenPopupDialog::Clear( void )
{
	SetDialogId(0);
	SetTitle(0);
	SetMessage(0);
	ClearMenu();
}

//=============================================================================
// ScreenPopupDialog::SetTitle
//=============================================================================

void ScreenPopupDialog::SetTitle( const char* pTitle )
{
	SetText( ID_POPUP_TITLE, pTitle );
}

//=============================================================================
// ScreenPopupDialog::SetMessage
//=============================================================================

void ScreenPopupDialog::SetMessage( const char* pMessage )
{
	SetText( ID_POPUP_MESSAGE, pMessage );
}

//=============================================================================
// ScreenPopupDialog::ClearMenu
//=============================================================================

void ScreenPopupDialog::ClearMenu( void )
{
	GuiListBox* pListBox = GetListBox();

	if( pListBox )
		pListBox->ClearItems();

	return;
}

//=============================================================================
// ScreenPopupDialog::AddMenuItem
//=============================================================================

void ScreenPopupDialog::AddMenuItem( const char* pText, int CommandId, void* pParam )
{
	GuiListBox* pListBox = GetListBox();

	if( pListBox )
		pListBox->AddItem( pText, CommandId, pParam );

	return;
}

//=============================================================================
// ScreenPopupDialog::SetMenuItemText
//=============================================================================

void ScreenPopupDialog::SetMenuItemText( const char* pText, int CommandId )
{
	GuiListBox* pListBox = GetListBox();

	if( pListBox )
	{
		GuiListBox::Item* pItem = pListBox->FindItem( CommandId );
		if( pItem )
			pItem->SetName( pText );
	}

	return;
}

//=============================================================================
// ScreenPopupDialog::SelectMenuItem
//=============================================================================

void ScreenPopupDialog::SelectMenuItem( int CommandId )
{
	GuiListBox* pListBox = GetListBox();

	if( pListBox )
		pListBox->SelectItem( CommandId );

	return;
}

//=============================================================================
// ScreenPopupDialog::CurrentSelection
//=============================================================================

int ScreenPopupDialog::CurrentSelection( void )
{
	GuiListBox* pListBox = GetListBox();

	if( !pListBox )
		return 0;

	return pListBox->CurrentSelection();
}

//=============================================================================
// ScreenPopupDialog::OnInitialize
//=============================================================================

void ScreenPopupDialog::OnInitialize( void )
{
	ScreenBaseClass::OnInitialize();

	GuiListBox* pListBox = CreateListBox( ID_POPUP_MENU );

	if( pListBox )
	{
		pListBox->InitControl( this );
		pListBox->InitItem( GetTextSprite( ID_POPUP_MENU1 ) );
		pListBox->InitItem( GetTextSprite( ID_POPUP_MENU2 ) );
		pListBox->InitItem( GetTextSprite( ID_POPUP_MENU3 ) );
		pListBox->InitCursor( GetSprite( ID_POPUP_CURSOR ) );
	}

	GuiScrollBar* pScrollBar = CreateScrollBar( ID_POPUP_SCROLLBAR );

	if( pScrollBar )
	{
		pScrollBar->InitControl( this );
		pScrollBar->InitPart( GuiScrollBar::Shaft,     GetSprite( ID_POPUP_SCROLLBAR ) );
		pScrollBar->InitPart( GuiScrollBar::Thumb,     GetSprite( ID_POPUP_SBTHUMB ) );
		pScrollBar->InitPart( GuiScrollBar::ArrowUp,   GetSprite( ID_POPUP_SBARROWUP ) );
		pScrollBar->InitPart( GuiScrollBar::ArrowDown, GetSprite( ID_POPUP_SBARROWDOWN ) );
	}

	Clear();
}

//=============================================================================
// ScreenPopupDialog::OnCursorUp
//=============================================================================

void ScreenPopupDialog::OnCursorUp( void )
{
	GuiListBox* pListBox = GetListBox();

	if( pListBox )
	{
		int OldSel = pListBox->CurrentItem();

		pListBox->OnCursorUp();

		int NewSel = pListBox->CurrentItem();

		if( NewSel != OldSel )
			PlaySoundEvent( Screen::SOUNDEVENT_PREVBUTTON );
	}

	return;
}

//=============================================================================
// ScreenPopupDialog::OnCursorDown
//=============================================================================

void ScreenPopupDialog::OnCursorDown( void )
{
	GuiListBox* pListBox = GetListBox();

	if( pListBox )
	{
		int OldSel = pListBox->CurrentItem();

		pListBox->OnCursorDown();

		int NewSel = pListBox->CurrentItem();

#ifndef DIRECTX_PC
		if( NewSel != OldSel )
			PlaySoundEvent( Screen::SOUNDEVENT_NEXTBUTTON );
#endif
	}

	return;
}

//=============================================================================
// ScreenPopupDialog::OnButtonOK
//=============================================================================

void ScreenPopupDialog::OnButtonOK( void )
{
	ScreenBaseClass::OnButtonOK();

	int CurSel = CurrentSelection();

	// send the event message
	g_messageDispatcher.SendMessageToAll( "DialogCommand", (void*) CurSel, INVALID_OBJECT_HANDLE );

	// play the sound
	PlaySoundEvent( Screen::SOUNDEVENT_SELECT );
}

//=============================================================================
// ScreenPopupDialog::OnButtonCancel
//=============================================================================

void ScreenPopupDialog::OnButtonCancel( void )
{
	ScreenBaseClass::OnButtonCancel();

	if( m_CancelCommand )
		g_messageDispatcher.SendMessageToAll( "DialogCommand", (void*) m_CancelCommand, INVALID_OBJECT_HANDLE );

	// send the event message
	g_messageDispatcher.SendMessageToAll( "DialogCancel", 0, INVALID_OBJECT_HANDLE );
}

//=============================================================================
// ScreenPopupDialog::OnScreenIntro
//=============================================================================

void ScreenPopupDialog::OnScreenIntro( void )
{
	ScreenBaseClass::OnScreenIntro();
#ifdef DIRECTX_PC
	BackButton.Show(false);
	QuitButton.Show(false);
#endif
}

//=============================================================================
// ScreenPopupDialog::OnScreenExit
//=============================================================================

void ScreenPopupDialog::OnScreenExit( void )
{
	ScreenBaseClass::OnScreenExit();
}

//=============================================================================
// ScreenPopupDialog::OnScreenUpdate
//=============================================================================

void ScreenPopupDialog::OnScreenUpdate( void )
{
	ScreenBaseClass::OnScreenUpdate();
}

//=============================================================================
// ScreenErrorMessage::SetErrorMessage
//=============================================================================

void ScreenErrorMessage::SetErrorMessage( const char* pErrorMessage )
{
	SetText( ID_ERRORMESSAGE_MESSAGE, pErrorMessage );
}


//=============================================================================
// ScreenErrorMessage::OnInitialize
//=============================================================================

void ScreenErrorMessage::OnInitialize( void )
{
	ScreenBaseClass::OnInitialize();

	SetErrorMessage( "A SERIOUS ERROR OCCURED." );
}

//=============================================================================
// ScreenErrorMessage::OnButtonCancel
//=============================================================================

void ScreenErrorMessage::OnButtonCancel( void )
{
	// eat it
}

//=============================================================================
// ScreenPleaseWait::SetMessage
//=============================================================================

void ScreenPleaseWait::SetMessage( const char* pMessage )
{
	SetText( ID_PLEASEWAIT_MESSAGE, pMessage );
}

//=============================================================================
// ScreenPleaseWait::OnInitialize
//=============================================================================

void ScreenPleaseWait::OnInitialize( void )
{
	ScreenBaseClass::OnInitialize();

	Sprite* pBusyIcon = GetSprite( ID_PLEASEWAIT_BUSYICON );

	if( pBusyIcon )
	{
		pBusyIcon->InitAnim();

		SpriteAnim &anim = *pBusyIcon->m_anim;

		anim.rotation.z.SetFrame( 0.000f, 0.0f );
		anim.rotation.z.SetFrame( 0.250f, 360.0f );
		anim.rotation.z.Play( true );
	}

	return;
}

//=============================================================================
// ScreenPleaseWait::OnButtonCancel
//=============================================================================

void ScreenPleaseWait::OnButtonCancel( void )
{
	// eat it
}

//=============================================================================
// ScreenConnectingToSession::ScreenConnectingToSession
//=============================================================================

ScreenConnectingToSession::ScreenConnectingToSession( void )
{
	m_StartTime = 0;
}

//=============================================================================
// ScreenConnectingToSession::SetMessage
//=============================================================================

void ScreenConnectingToSession::SetMessage( const char* pMessage )
{
	SetText( ID_CONNECTINGTOSESSION_MESSAGE, pMessage );
}

//=============================================================================
// ScreenConnectingToSession::SetSessionDescription
//=============================================================================

void ScreenConnectingToSession::SetSessionDescription( const char* pDescription )
{
	SetText( ID_CONNECTINGTOSESSION_DESCRIPTION, pDescription );
}

//=============================================================================
// ScreenConnectingToSession::OnButtonCancel
//=============================================================================

void ScreenConnectingToSession::OnButtonCancel( void )
{
	// eat it
}


//=============================================================================
// ScreenConnectingToSession::OnScreenIntro
//=============================================================================

void ScreenConnectingToSession::OnScreenIntro( void )
{
	ScreenBaseClass::OnScreenIntro();

	m_StartTime = g_timer.GetOSTime();
}

//=============================================================================
// ScreenConnectingToSession::OnScreenRefresh
//=============================================================================

void ScreenConnectingToSession::OnScreenRefresh( void )
{
	int Seconds = (g_timer.GetOSTime() - m_StartTime) / 1000;
	
	int M = Seconds / 60;
	int S = Seconds % 60;

	Printf( ID_CONNECTINGTOSESSION_TIME, "Time: %02d:%02d", M, S );
	ShowSprite( ID_CONNECTINGTOSESSION_TIME, false );
}