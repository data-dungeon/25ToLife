#include "TTLPCH.h"
#include "frontend/screens/screen_XboxClanFeedbackPopup.h"
#include "data/screens/Multiplayer.h"

#include "OSI/OSI.hpp"

//=============================================================================
// Enum and associated string for buttons in the popup
//=============================================================================
enum
{
	PPCHOICE_OFFENSIVE_ABBREV,
	PPCHOICE_OFFENSIVE_NAME,
	PPCHOICE_FEEDBACK_BACK,
};

//=============================================================================
// Get the list box associated with the popup
//=============================================================================
GuiListBox* ScreenXboxClanFeedbackPopup::GetListBox( void )
{
	return (GuiListBox*) GetInterface( ID_CLANFEEDBACK_POPUP_MENURECT, GuiListBox::ClassnameStatic() );
}


//=============================================================================
// Called when the screen is allocated
//=============================================================================
/* virtual */
void ScreenXboxClanFeedbackPopup::OnInitialize( void )
{
	GuiListBox* pListBox = CreateListBox( ID_CLANFEEDBACK_POPUP_MENURECT );

	if( pListBox )
	{
		pListBox->InitControl( this );
		pListBox->InitItem( GetTextSprite( ID_CLANFEEDBACK_POPUP_OFFENSIVE_ABBREV) );
		pListBox->InitItem( GetTextSprite( ID_CLANFEEDBACK_POPUP_OFFENSIVE_NAME ) );
		pListBox->InitCursor( GetSprite( ID_CLANFEEDBACK_POPUP_CURSOR ) );
	}

	pListBox->AddItem( "Offensive Abbreviation", PPCHOICE_OFFENSIVE_ABBREV, 0 );
	pListBox->AddItem( "Offensive Name", PPCHOICE_OFFENSIVE_NAME, 0 );
	pListBox->AddItem( "Back", PPCHOICE_FEEDBACK_BACK, 0 );

	ScreenBaseClass::OnInitialize();
}

//=============================================================================
// Called when the up button is pressed while the screen is active
//=============================================================================
/* virtual */
void ScreenXboxClanFeedbackPopup::OnCursorUp( void )
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
// Called when the down button is pressed while the screen is active
//=============================================================================
/* virtual */
void ScreenXboxClanFeedbackPopup::OnCursorDown( void )
{
	GuiListBox* pListBox = GetListBox();

	if( pListBox )
	{
		int OldPos = pListBox->CurrentItem();

		pListBox->OnCursorDown();

		int NewPos = pListBox->CurrentItem();

		if( OldPos != NewPos )
			PlaySoundEvent( Screen::SOUNDEVENT_NEXTBUTTON );
	}

	return;
}

//=============================================================================
// Called when the ok button is pressed while the screen is active
//=============================================================================
/* virtual */
void ScreenXboxClanFeedbackPopup::OnButtonOK( void )
{
    GuiListBox* pListBox = GetListBox();

	if(pListBox == NULL || pListBox->GetItemCount() == 0)
	{
		return;
	}

	switch( pListBox->GetItemId( pListBox->CurrentItem() ) )
	{
        case PPCHOICE_OFFENSIVE_ABBREV:
            _screenState = REPORT_OFFENSIVE_ABBREV;
            break;

        case PPCHOICE_OFFENSIVE_NAME:
            _screenState = REPORT_OFFENSIVE_NAME;
            break;

        case PPCHOICE_FEEDBACK_BACK:
            _screenState = BACK;
            break;

		default:
			ScreenBasePopup::OnButtonOK();
			break;
    }
}

