#include "TTLPCH.h"
#include "environ/ConfigMgr.h"
#include "frontend/inputdriver.h"
#include "frontend/screens/screen_Popups.h"
#include "frontend/screens/screen_MultiplayerGameSettings.h"
//#ifdef DIRECTX_PC
//#include "data/screens/pcscreens.h"
//#else
#include "data/screens/Multiplayer.h"
//#endif
#include "OSI/OSI.hpp"
#include "GameHelper/Translator.h"

//=============================================================================
// ScreenMultiplayerGameSettings::GetListBox
//=============================================================================

ScreenMultiplayerGameSettings::ScreenMultiplayerGameSettings()
{
}

ScreenMultiplayerGameSettings::~ScreenMultiplayerGameSettings()
{
}

GuiListBox* ScreenMultiplayerGameSettings::GetListBox(void)
{
	return (GuiListBox*)GetInterface(ID_SEARCH_MENU, GuiListBox::ClassnameStatic());
}

void ScreenMultiplayerGameSettings::ShowCursorIcon( int n, bool Show )
{
	int CursorIcon = 0;

	switch( n )
	{
	case 0:
		CursorIcon = ID_SEARCH_LOWCURSOR1;
		break;
	case 1:
		CursorIcon = ID_SEARCH_LOWCURSOR2;
		break;
	case 2:
		CursorIcon = ID_SEARCH_LOWCURSOR3;
		break;
	case 3:
		CursorIcon = ID_SEARCH_LOWCURSOR4;
		break;
	}

	if( CursorIcon )
	{
		Sprite* pCursorIcon = GetSprite( CursorIcon );

		if( pCursorIcon )
			pCursorIcon->Show( Show );
	}

	return;
}

//=============================================================================
// ScreenMultiplayerGameSettings::OnInitialize
//=============================================================================

void ScreenMultiplayerGameSettings::OnInitialize(void)
{
	ScreenBaseClass::OnInitialize();

	SetHelpRect( ID_SEARCH_HELPRECT );

	GuiListBox* pListBox = CreateListBox(ID_SEARCH_MENU);

	if (pListBox)
	{
		pListBox->InitControl(this);

		Sprite* pMenu = GetSprite(ID_SEARCH_MENU);

		pListBox->InitItem(GetTextSprite(ID_SEARCH_LABEL, GetSprite(ID_SEARCH_SETTING1, pMenu)));
		pListBox->InitItem(GetTextSprite(ID_SEARCH_LABEL, GetSprite(ID_SEARCH_SETTING2, pMenu)));
		pListBox->InitItem(GetTextSprite(ID_SEARCH_LABEL, GetSprite(ID_SEARCH_SETTING3, pMenu)));
		pListBox->InitItem(GetTextSprite(ID_SEARCH_LABEL, GetSprite(ID_SEARCH_SETTING4, pMenu)));
		pListBox->InitCursor(GetSprite(ID_SEARCH_CURSOR));
	}
	
    if (m_pScreen && OSI::getSingletonPtr())
    {
#ifndef DIRECTX_PC
		OSIchar name[ OSI_PLAYER_FULL_NAME_LENGTH ];
        Printf(ID_SEARCH_CURRENTPROFILE, g_translator.Translate( "Current Profile: %s" ), OSI::getSingletonPtr()->getActivePlayer()->GetFullName(name) );
#endif
	 }    

	GuiScrollBar* pScrollBar = CreateScrollBar( ID_SEARCH_SCROLLBAR );

	if (pScrollBar)
	{
		Sprite* pSprite = pScrollBar->SpriteHost();

		pScrollBar->InitPart(GuiScrollBar::Shaft, pSprite);
		pScrollBar->InitPart(GuiScrollBar::Thumb, GetSprite(ID_SEARCH_SBTHUMB, pSprite));
		pScrollBar->InitPart(GuiScrollBar::ArrowUp, GetSprite(ID_SEARCH_SBARROWUP, pSprite));
		pScrollBar->InitPart(GuiScrollBar::ArrowDown, GetSprite(ID_SEARCH_SBARROWDOWN, pSprite));

		pScrollBar->SetRange(0.0f, 10.0f);
		pScrollBar->SetPosition(5.0f);
		pScrollBar->SetSteps(10);
	}

	m_InitMenu = true;


#ifdef _XBOX
	// Dan Silver - Testers are saying "OptiMatch" does NOT have space and the M is capitalized. So I changed it.
	// See bug 7778.
	SetText( ID_SEARCH_TITLE, "OptiMatch" );
#endif

// Hide PS2 help bar on PC
#ifdef DIRECTX_PC
	GetSprite(ID_SEARCH_HELPRECT)->HideAll();
#endif
}

void ScreenMultiplayerGameSettings::OnScreenIntro( void )
{
	char buff[ 256 ];
	OSIsnprintf( buff, 256, "%s: %s", g_translator.Translate( "Online name" ), OSI::getSingletonPtr()->getActiveAccount()->player.name );
	buff[ 255 ] = 0;
	GetTextSprite( ID_SEARCH_CURRENTPROFILE )->SetTextNoTranslate( buff );

	ScreenBaseClass::OnScreenIntro();
#ifdef DIRECTX_PC
	StartButton.SetText("Search");
	StartButton.SetPosition( 100, 420 );
	StartButton.Show(true);
#endif
}

void ScreenMultiplayerGameSettings::SetToAny()
{
	GuiListBox* pListBox = GetListBox();
	int i = 0;
	int count = _values.count();
	for ( i=0; i<count; i++ )
	{
		const char* firstValue = _values[i]->getStringValue();
		// Try to find the "Any" value
		while( strnicmp( _values[i]->getStringValue(), "Any", 4 ) != 0 )
		{
			_values[i]->prevValue();
			if( firstValue == _values[i]->getStringValue() )
				break;
		}
	}
}

void ScreenMultiplayerGameSettings::OnPrevScreen( Screen *pPrevScreen )
{
	for ( _values.begin(); _values.getCurrent() != NULL; _values.next() )
	{
		ParamType* type = _values.getCurrent();
		delete type ;
	}
	_values.clear();
}

//=============================================================================
// ScreenMultiplayerGameSettings::OnCursorUp
//=============================================================================

void ScreenMultiplayerGameSettings::OnCursorUp(void)
{
	GuiListBox* pListBox = GetListBox();

	if (pListBox)
	{
		int OldPos = pListBox->CurrentItem();

		pListBox->OnCursorUp();

		int NewPos = pListBox->CurrentItem();

		if (OldPos != NewPos)
        {
			PlaySoundEvent(Screen::SOUNDEVENT_PREVBUTTON);
        }
	}
}

//=============================================================================
// ScreenMultiplayerGameSettings::OnCursorDown
//=============================================================================

void ScreenMultiplayerGameSettings::OnCursorDown(void)
{
	GuiListBox* pListBox = GetListBox();

	if (pListBox)
	{
		int OldPos = pListBox->CurrentItem();

		pListBox->OnCursorDown();

		int NewPos = pListBox->CurrentItem();

#ifndef DIRECTX_PC
		if (OldPos != NewPos)
        {
			PlaySoundEvent(Screen::SOUNDEVENT_PREVBUTTON);
        }
#endif
	}
}

//=============================================================================
// ScreenMultiplayerGameSettings::OnCursorLeft
//=============================================================================

void ScreenMultiplayerGameSettings::OnCursorLeft(void)
{
	OnButtonCycle(-1);
}

//=============================================================================
// ScreenMultiplayerGameSettings::OnCursorRight
//=============================================================================

void ScreenMultiplayerGameSettings::OnCursorRight(void)
{
	OnButtonCycle(1);
}

//=============================================================================
// ScreenMultiplayerGameSettings::OnButtonOK
//=============================================================================

void ScreenMultiplayerGameSettings::OnButtonOK(void)
{
	//EndScreen(0);
    g_messageDispatcher.SendMessageToAll("SearchForMatches", (void *)false, INVALID_OBJECT_HANDLE);
}

//=============================================================================
// ScreenMultiplayerGameSettings::OnButtonCancel
//=============================================================================

void ScreenMultiplayerGameSettings::OnButtonCancel(void)
{
	//EndScreen(0);
    // Message for Use Cases to catch cancelation of screen
    g_messageDispatcher.SendMessageToAll("CancelConfigMatch", NULL, INVALID_OBJECT_HANDLE);
}

//=============================================================================
// ScreenMultiplayerGameSettings::OnButtonCycle
//=============================================================================

void ScreenMultiplayerGameSettings::OnButtonCycle(int N)
{
	GuiListBox* pListBox = GetListBox();

	if (pListBox)
	{
		int i = pListBox->CurrentSelection();

        if (i == -1)
        {
			PlaySoundEvent(Screen::SOUNDEVENT_INVALID);
            return;
        }

		if (N > 0)
        {
            _values[i]->nextValue();
        }
		else if (N < 0)
        {
			_values[i]->prevValue();
        }

		// the option list may have completely changed by altering this option
		m_InitMenu = true;

        // Rebuild the options list
        RebuildOptionList();
	}
}

//=============================================================================
// ScreenMultiplayerGameSettings::OnButtonPressed
//=============================================================================

void ScreenMultiplayerGameSettings::OnButtonPressed( int Button )
{
	switch( Button )
	{
	case FluffControlMapper::SQUARE:
		SetToAny();
		break;

	default:
		ScreenBaseClass::OnButtonPressed( Button );
		break;
	}

	return;
}

//=============================================================================
// ScreenMultiplayerGameSettings::OnScreenUpdate
//=============================================================================

void ScreenMultiplayerGameSettings::OnScreenUpdate(void)
{
	RebuildOptionList();
#ifdef DIRECTX_PC
	if (g_MenuCursor.IsActive()) {
		float x1, y1, x2, y2;	
		// check for back navigation
		if (g_MenuCursor.CursorButtonPressed(1))
		{
			g_MenuCursor.ForceButtonUp(1);
			OnButtonCancel();
		}
		float x = g_MenuCursor.GetX();
		float y = g_MenuCursor.GetY();
		Button* button = GetDefaultButton();
		// functionality for button based menus
		if (!button)
		{
			if (g_MenuCursor.CursorButtonPressed(0))
				OnButtonStart();
		}
		// search through the buttons to see if we're over one
		// if so, check to see if we're clicking the mouse
		GuiListBox* listBox = GetListBox();
		if (listBox)
		{
			int items = listBox->GetItemCount();
			for (int c=0; c < items; ++c)
			{
				listBox->GetCursorRect(x1,y1,x2,y2);
				// HACK HACK HACK
				// expand this out a little bit for the delete
				// button for the profile listbox. ugh
				x1 -= 32;
				if ( ( x > x1 && x < x2 ) && ( y > y1 && y < y2 ) )
				{
					if ( g_MenuCursor.CursorButtonPressed(0) )
					{
						OnCursorRight();
						return;
					}
					break;
				}
				//listBox->OnCursorDown();
				OnCursorDown();
			}
		}
	}
#endif
	ScreenBaseClass::OnScreenUpdate();
}

//=============================================================================
// ScreenMultiplayerGameSettings::RebuildOptionList
//=============================================================================

void ScreenMultiplayerGameSettings::RebuildOptionList(void)
{
	GuiListBox* pListBox = GetListBox();
	GuiListBox::Item* pItem;
	int i, n;

	if (pListBox)
	{
		if (m_InitMenu)
		{
			const char* pItemName;
			int CursorPos;

			// remember what option was selected
			xstring szCurSel;
			szCurSel.set(pListBox->GetItemText(pListBox->CurrentItem()));

			// get the cursor pos
			CursorPos = pListBox->GetCursorPos();

			// clear existing items
			pListBox->ClearItems();

			// add new items to the list box
			_values.begin();

            i = 0;
            while (_values.getCurrent())
            {
                pListBox->AddItem(_values.getCurrent()->getName(), i, 0);
                _values.next();
                ++i;
            }

			n = pListBox->GetItemCount();

			// try and re-select the old option
			for (i = 0; i < n; i++)
			{
				pItemName = pListBox->GetItemText(i);

				if (szCurSel.get() && pItemName)
				{
					if (0 == strcmp(szCurSel.get(), pItemName))
					{
						pListBox->SelectIndex(i);
						pListBox->SetCursorPos(CursorPos);
						break;
					}
				}
			}

			m_InitMenu = false;
		}

		// sync up the item labels
		n = pListBox->GetItemDisplayCount();

		pItem = pListBox->GetItem(pListBox->GetTopItem());

		Sprite* pParent;
		Text* pLabel;
		Text* pParam;

      for( i = 0; i < n; i++ )
		{
			pParent = 0;
			pLabel = 0;
			pParam = 0;

			pLabel = pListBox->GetDisplayItem(i);

			if( pLabel )
				pParent = pLabel->Parent();

			if( pParent )
				pParam = GetTextSprite( ID_SEARCH_PARAM, pParent );

			if( pParam )
				pParam->SetText(0);
		}

		i = 0;

		_values.begin();

		while (_values.getCurrent())
		{
			pParent = 0;
			pLabel = 0;
			pParam = 0;

			pLabel = pListBox->GetDisplayItem(i);

			if (pLabel)
				{
				pParent = pLabel->Parent();
				}

			if (pParent)
				{
				pParam = GetTextSprite(ID_SEARCH_PARAM, pParent);
				}

			if (pParam)
			{
				if (pItem && _values.count())
					{
						ParamType *pCurrent = _values.getCurrent();
					pParam->SetText(pCurrent->getStringValue());

						pItem = pItem->Next();
						_values.next();
						++i;
					}
				else
					{
						break;
					}
			}
		}

		// update the scroll bar
		int MaxItems   = pListBox->GetItemCount();
		int CursorPos  = pListBox->GetCursorPos();

		// hide/show cursor icons

		for( i = 0; i < pListBox->GetItemDisplayCount(); i++ )
		{
			if( i < MaxItems )
				ShowCursorIcon(i, true);
			else
				ShowCursorIcon(i, false);
		}

		// Set the description
      if (CursorPos < _values.count())
      {
          m_pScreen->GetTextSprite(ID_SEARCH_DESCRIPTION)->SetText(_values[CursorPos]->getDescription());
      }

	}
}

//#ifdef DIRECTX_PC
//void ScreenMultiplayerGameSettings::OnCommand(int Command)
//{
//	switch (Command)
//	{
//		case ID_SEARCH_CONTINUE:
//			OnButtonOK();
//			break;
//
//		case ID_SEARCH_BACK:
//			OnButtonCancel();
//			break;
//	}
//}
//#endif
