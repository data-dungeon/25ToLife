#include "TTLPCH.h"
#include "GameHelper/Translator.h"
#include "platform/wantpal.h"
#include "frontend/inputdriver.h"
#include "screen_EOA.h"
#include "frontend/frontend.h"
#include "data/screens/EOR.h"

/*virtual*/ void ScreenEOA::OnInitialize(void)
{
    UseHighlighting(true);
#ifndef DIRECTX_PC
	 SetHelpRect( ID_EOA_HELPRECT );
#endif
    ScreenBaseClass::OnInitialize();

#ifdef DIRECTX_PC
	// Hide PS2 help bar
	GetSprite(ID_EOA_DECLINE)->HideAll();
	GetSprite(ID_EOA_AGREE)->HideAll();
	GetSprite(ID_EOA_HELPICON)->HideAll();
	GetSprite(ID_EOA_HELPTEXT)->HideAll();

	// Set up buttons
	SetInitialButton(ID_EOA_ACCEPT);
	SetNextButton(ID_EOA_DECLINE2);
#else
	// Hide accept/decline
	GetSprite(ID_EOA_ACCEPT)->HideAll();
	GetSprite(ID_EOA_DECLINE2)->HideAll();
#endif

	GuiScrollingText* pScrollText = (GuiScrollingText*) AddInterface( ID_EOA_FRAME, GuiScrollingText::ClassnameStatic() );

	if( pScrollText )
	{
		Sprite* pSprite = pScrollText->SpriteHost();

		if( pSprite )
			pScrollText->SetWidth( pSprite->GetWidth() );

		pScrollText->InitControl( this );
		pScrollText->Init(11);
		pScrollText->InitItem(  0, GetTextSprite( ID_EOA_ROW1 ) );
		pScrollText->InitItem(  1, GetTextSprite( ID_EOA_ROW2 ) );
		pScrollText->InitItem(  2, GetTextSprite( ID_EOA_ROW3 ) );
		pScrollText->InitItem(  3, GetTextSprite( ID_EOA_ROW4 ) );
		pScrollText->InitItem(  4, GetTextSprite( ID_EOA_ROW5 ) );
		pScrollText->InitItem(  5, GetTextSprite( ID_EOA_ROW6 ) );
		pScrollText->InitItem(  6, GetTextSprite( ID_EOA_ROW7 ) );
		pScrollText->InitItem(  7, GetTextSprite( ID_EOA_ROW8 ) );
		pScrollText->InitItem(  8, GetTextSprite( ID_EOA_ROW9 ) );
		pScrollText->InitItem(  9, GetTextSprite( ID_EOA_ROW10) );
		pScrollText->InitItem( 10, GetTextSprite( ID_EOA_ROW11) );
	}

	GuiScrollBar* pScrollBar = CreateScrollBar( ID_EOA_SCROLLBAR );

	if( pScrollBar )
	{
		Sprite* pSprite = pScrollBar->SpriteHost();

		pScrollBar->InitPart( GuiScrollBar::Shaft, pSprite );
		pScrollBar->InitPart( GuiScrollBar::Thumb, GetSprite( ID_EOA_THUMB, pSprite ) );
		pScrollBar->InitPart( GuiScrollBar::ArrowUp, GetSprite( ID_EOA_ARROWUP, pSprite ) );
		pScrollBar->InitPart( GuiScrollBar::ArrowDown, GetSprite( ID_EOA_ARROWDOWN, pSprite ) );

		pScrollBar->SetRange( 0.0f, 10.0f );
		pScrollBar->SetPosition( 5.0f );
		pScrollBar->SetSteps( 10 );
	}

	OnCursorUpdate();
}

void ScreenEOA::LoadText( void )
{
	const char* pFileName = "eula/english - us/eoa.txt";
	const char* pLanguage = g_translator.GetLanguage();

	if( 0 == stricmp( pLanguage, "english" ) )
	{
		if( g_regionUSA )
			pFileName = "eula/english - us/eoa.txt";
		else
			pFileName = "eula/english - eu/eoa.txt";
	}
	else
	if( 0 == stricmp( pLanguage, "french" ) )
		pFileName = "eula/french/eoa.txt";
	else
	if( 0 == stricmp( pLanguage, "german" ) )
		pFileName = "eula/german/eoa.txt";
	else
	if( 0 == stricmp( pLanguage, "italian" ) )
		pFileName = "eula/italian/eoa.txt";
	else
	if( 0 == stricmp( pLanguage, "spanish" ) )
		pFileName = "eula/spanish/eoa.txt";

	GuiScrollingText* pScrollText = (GuiScrollingText*) GetInterface( ID_EOA_FRAME, GuiScrollingText::ClassnameStatic() );

	if( pScrollText && pScrollText->GetRows() == 0 )
	{
		const char* pstring = (char*) Media::Load( pFileName, 0, 0 );

		if( pstring )
			pScrollText->SetText( pstring );

		delete [] pstring;
	}

	return;
}

void ScreenEOA::OnScreenIntro( void )
{
    _status = RUNNING;

	 ScreenBaseClass::OnScreenIntro();

	 LoadText();

	 OnCursorUpdate();

	 // Deselect all buttons (DX only)
#ifdef DIRECTX_PC
	GetButton(ID_EOA_ACCEPT)->m_Selected = false;
	GetButton(ID_EOA_DECLINE2)->m_Selected = false;
#endif
}

void ScreenEOA::OnCursorUp( void )
{
	GuiScrollingText* pScrollText = (GuiScrollingText*) GetInterface( ID_EOA_FRAME, GuiScrollingText::ClassnameStatic() );

	if( pScrollText )
	{
		//		pScrollText->ScrollPage(-1);
		//		pScrollText->ScrollLine( 1);
		pScrollText->ScrollLine(-1);

		PlaySoundEvent(Screen::SOUNDEVENT_INVALID);
	}

	OnCursorUpdate();
}

void ScreenEOA::OnCursorDown( void )
{
	GuiScrollingText* pScrollText = (GuiScrollingText*) GetInterface( ID_EOA_FRAME, GuiScrollingText::ClassnameStatic() );

	if( pScrollText )
	{
		//		pScrollText->ScrollPage( 1);
		//		pScrollText->ScrollLine(-1);
		pScrollText->ScrollLine( 1);

		PlaySoundEvent(Screen::SOUNDEVENT_INVALID);
	}

	OnCursorUpdate();
}

void ScreenEOA::OnCursorUpdate( void )
{
	GuiScrollingText* pScrollText = GetScrollingText();

	if( pScrollText )
		Printf( ID_EOA_LINESOFTEXT, "Line %d of %d", pScrollText->GetCursorPos()+1, pScrollText->GetRows() );

	return;
}

void ScreenEOA::OnButtonOK( void )
{
#ifdef DIRECTX_PC
	if (GetCurrentButtonId() == ID_EOA_ACCEPT)
	{
		GetButton(ID_EOA_ACCEPT)->m_Selected = false;
		OnCommand(ID_EOA_AGREE);
		return;
	}
	else if (GetCurrentButtonId() == ID_EOA_DECLINE2)
	{
		GetButton(ID_EOA_DECLINE2)->m_Selected = false;
		OnCommand(ID_EOA_DECLINE);
		return;
	}
#else
	OnCommand( ID_EOA_AGREE );
#endif
}



void ScreenEOA::OnCommand( int Command )
{
    // Set the status flag
    switch (Command)
    {
        case ID_EOA_AGREE:
            _status = AGREE;
            break;

        case ID_EOA_DECLINE:
            _status = DISAGREE;
            break;
    }

    ScreenBaseClass::OnCommand(Command);
}
