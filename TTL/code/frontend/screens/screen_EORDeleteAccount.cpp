#include "TTLPCH.h"
#include "frontend/inputdriver.h"
#include "screen_EORDeleteAccount.h"
#include "data/screens/EOR.h"
#include "frontend/frontend.h"

/*virtual*/ void ScreenEORDeleteAccount::OnInitialize(void)
{
    Screen* pScreen = m_pScreen;

    UseHighlighting(true);

	if(pScreen)
	{
        Sprite* pSprite = GetSprite( ID_PROFILE_GROUP );

        if( pSprite )
		    pSprite->AddInterface( GuiListBox::ClassnameStatic(), 0 );

	    GuiListBox *pListBox = GetProfileListBox();

	    if( pListBox )
	    {
		    pListBox->InitControl( this );

		    pListBox->InitItem( GetTextSprite( ID_DELETE_PROFILE1 ) );
		    pListBox->InitItem( GetTextSprite( ID_DELETE_PROFILE2 ) );
		    pListBox->InitItem( GetTextSprite( ID_DELETE_PROFILE3 ) );
		    pListBox->InitItem( GetTextSprite( ID_DELETE_PROFILE4 ) );
		    pListBox->InitCursor( GetSprite( ID_DELETE_CURSOR ) );
	    }

		// Hide the PS2 help bar
#ifdef DIRECTX_PC
		GetSprite(ID_DELETE_HELPICON1)->HideAll();
		GetSprite(ID_DELETE_HELPICON2)->HideAll();
		GetSprite(ID_DELETE_HELPICON3)->HideAll();
		GetSprite(ID_DELETE_HELPTEXT1)->HideAll();
		GetSprite(ID_DELETE_HELPTEXT2)->HideAll();
		GetSprite(ID_DELETE_HELPTEXT3)->HideAll();

		SetInitialButton(ID_DELETE_BACK);
#else
		GetSprite(ID_DELETE_BACK)->HideAll();
#endif
	}

	ScreenBaseClass::OnInitialize();
}

void ScreenEORDeleteAccount::OnScreenIntro( void )
{
    _status = RUNNING;

#ifdef DIRECTX_PC
	GetButton(ID_DELETE_BACK)->m_Selected = false;
#endif
    ScreenBaseClass::OnScreenIntro();
}


//=============================================================================
// ScreenEORDeleteAccount::GetProfileListBox
//
//		Returns a pointer to the Network configuration list box.
//=============================================================================

GuiListBox* ScreenEORDeleteAccount::GetProfileListBox( void )
{
	GuiListBox* pListBox = 0;
	Sprite* pSprite = GetSprite( ID_PROFILE_GROUP );

	if( pSprite )
		pListBox = (GuiListBox*) pSprite->GetInterface( GuiListBox::ClassnameStatic() );

	return pListBox;
}

void ScreenEORDeleteAccount::OnCursorUp( void )
{
    GuiListBox* pListBox = GetProfileListBox();

	if( pListBox )
	{
		int OldPos = pListBox->CurrentItem();
			
		pListBox->OnCursorUp();

		int NewPos = pListBox->CurrentItem();

		if( OldPos != NewPos )
		{
#ifndef DIRECTX_PC
			PlaySoundEvent( Screen::SOUNDEVENT_PREVBUTTON );
#endif // !DIRECTX_PC

			OnCursorUpdate();
		}
	}
}

void ScreenEORDeleteAccount::OnCursorDown( void )
{
    GuiListBox* pListBox = GetProfileListBox();

	if( pListBox )
	{
		int OldPos = pListBox->CurrentItem();

		pListBox->OnCursorDown();

		int NewPos = pListBox->CurrentItem();

		if( OldPos != NewPos )
		{
#ifndef DIRECTX_PC
			PlaySoundEvent( Screen::SOUNDEVENT_NEXTBUTTON );
#endif // !DIRECTX_PC

			OnCursorUpdate();
		}
	}
}

void ScreenEORDeleteAccount::OnCursorUpdate( void )
{
    GuiListBox* pListBox = GetProfileListBox();

	if( pListBox )
	{
		GuiListBox::Item* pItem = pListBox->GetItem( pListBox->CurrentItem() );

		/*if( pItem )
			SetText( ID_ONLINEPROFILE_CURRENTPROFILE, pItem->GetName() );*/
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Remvoe all profiles from the screen.
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORDeleteAccount::ClearProfiles()
{
    GuiListBox* pListBox = GetProfileListBox();

	if( pListBox )
	{
        pListBox->ClearItems();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Add an account with an offset to the screen.
///
/// \param pName Name of the account to add
///
/// \param Offset The offset into the global account array of the account
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenEORDeleteAccount::AddProfile(const char *pName, int Offset)
{
    GuiListBox* pListBox = GetProfileListBox();

	if( pListBox )
	{
        pListBox->AddItem(pName, pListBox->GetItemCount() - 1, (void *)Offset);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Get the offset into the global account array of the currently selected account
///
/// \return The offset into the global account array of the currently selected account
///////////////////////////////////////////////////////////////////////////////////////////////////
int ScreenEORDeleteAccount::GetOffset()
{
    GuiListBox* pListBox = GetProfileListBox();

	if( pListBox )
	{
        return (int)pListBox->GetItem(pListBox->GetCursorPos())->GetParam();
    }

    return -1;
}

#ifdef DIRECTX_PC
void ScreenEORDeleteAccount::OnButtonOK( void )
{
	if (GetButton(ID_DELETE_BACK)->m_Selected)
	{
		GetButton(ID_DELETE_BACK)->m_Selected = false;
		OnButtonCancel();
	}
	else
	{
		_status = DONE;
	}

	ScreenBaseClass::OnButtonOK();
}
#endif
