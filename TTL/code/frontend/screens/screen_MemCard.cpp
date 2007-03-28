#include "TTLPCH.h"
#include "frontend/screens/screen_MemCard.h"
#include "data/screens/Popups.h"
#include "platform/MemoryCard/MemoryCard.h"

//=============================================================================
// ScreenMemCard::SetTitle
//=============================================================================

void ScreenMemCard::SetTitle( const char* pTitle )
{
	SetText( ID_MEMCARD_TITLE, pTitle );
}

//=============================================================================
// ScreenMemCard::SetMessage
//=============================================================================

void ScreenMemCard::SetMessage( const char* pMessage )
{
	SetText( ID_MEMCARD_MESSAGE, pMessage );
}

//=============================================================================
// ScreenMemCard::AddMenuItem
//=============================================================================

int ScreenMemCard::AddMenuItem( const char* pText, int id )
{
	GuiListBox* pListBox = GetListBox();

	int index = -1;

	if( pListBox )
	{
		index = pListBox->GetItemCount();
		pListBox->AddItem( pText, id, 0 );
	}

	return index;
}

//=============================================================================
// ScreenMemCard::SelectMenuItem()
//=============================================================================

void ScreenMemCard::SelectMenuItem( int id )
{
	GuiListBox* pListBox = GetListBox();

	if( pListBox )
		pListBox->SelectItem( id );

	return;
}

//=============================================================================
// ScreenMemCard::SetResult()
//=============================================================================

void ScreenMemCard::SetResult( int Result )
{
	m_Result = Result;
}

//=============================================================================
// ScreenMemCard::GetResult()
//=============================================================================

int ScreenMemCard::GetResult( void )
{
	return m_Result;
}

//=============================================================================
// ScreenMemCard::Reset()
//=============================================================================

void ScreenMemCard::Reset( void )
{
	GuiListBox* pListBox = GetListBox();

	if( pListBox )
		pListBox->ClearItems();

	SetTitle(0);
	SetMessage(0);

	return;
}

//=============================================================================
// ScreenMemCard::GetListBox
//=============================================================================

GuiListBox* ScreenMemCard::GetListBox( void )
{
	return (GuiListBox*) GetInterface( ID_MEMCARD_MENU, GuiListBox::ClassnameStatic() );
}

//=============================================================================
// ScreenMemCard::OnInitialize
//=============================================================================

void ScreenMemCard::OnInitialize( void )
{
	ScreenBaseClass::OnInitialize();

	GuiListBox* pListBox = CreateListBox( ID_MEMCARD_MENU );

	if( pListBox )
	{
		pListBox->InitControl( this );

		pListBox->InitItem( GetTextSprite( ID_MEMCARD_ITEM1 ) );
		pListBox->InitItem( GetTextSprite( ID_MEMCARD_ITEM2 ) );
		pListBox->InitItem( GetTextSprite( ID_MEMCARD_ITEM3 ) );
		pListBox->InitCursor( GetSprite( ID_MEMCARD_CURSOR ) );
	}

	m_Result = MC_NONE;
	return;
}

//=============================================================================
// ScreenMemCard::OnScreenIntro
//=============================================================================

void ScreenMemCard::OnScreenIntro( void )
{
	ScreenBaseClass::OnScreenIntro();
}

//=============================================================================
// ScreenMemCard::OnScreenExit
//=============================================================================

void ScreenMemCard::OnScreenExit( void )
{
	ScreenBaseClass::OnScreenExit();
}

//=============================================================================
// ScreenMemCard::OnCursorUp
//=============================================================================

void ScreenMemCard::OnCursorUp( void )
{
	GuiListBox* pListBox = GetListBox();

	if( pListBox )
	{
		int OldPos = pListBox->CurrentItem();

		pListBox->OnCursorUp();

		int NewPos = pListBox->CurrentItem();

		if( OldPos != NewPos )
			PlaySoundEvent( Screen::SOUNDEVENT_PREVBUTTON );
	}

	return;
}

//=============================================================================
// ScreenMemCard::OnCursorDown
//=============================================================================

void ScreenMemCard::OnCursorDown( void )
{
	GuiListBox* pListBox = GetListBox();

	if( pListBox )
	{
		int OldPos = pListBox->CurrentItem();

		pListBox->OnCursorDown();

		int NewPos = pListBox->CurrentItem();

#ifndef DIRECTX_PC
		if( OldPos != NewPos )
			PlaySoundEvent( Screen::SOUNDEVENT_NEXTBUTTON );
#endif
	}

	return;
}

//=============================================================================
// ScreenMemCard::OnButtonOK
//=============================================================================

void ScreenMemCard::OnButtonOK( void )
{
	GuiListBox* pListBox = GetListBox();

	if( pListBox && pListBox->GetItemCount() > 0 )
	{
		PlaySoundEvent( Screen::SOUNDEVENT_NEXTBUTTON );

		m_Result = pListBox->CurrentSelection();
//		EndScreen( m_Result );
	}

	return;

}

//=============================================================================
// ScreenMemCard::OnButtonCancel
//=============================================================================

void ScreenMemCard::OnButtonCancel( void )
{
	// eat it
}
