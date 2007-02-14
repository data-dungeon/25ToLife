#include "TTLPCH.h"
#include "OSI/OSI.hpp"
#include "frontend/inputdriver.h"
#include "screen_clan_notices.h"
#include "data/screens/Multiplayer.h"
#include "frontend/frontend.h"
#include "GameHelper/Translator.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the screen is first loaded
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ void ScreenClanNotices::OnInitialize(void)
{
    Screen* pScreen = m_pScreen;

    UseHighlighting(true);

	if(pScreen)
	{
	// Hide PS2 help bar on PC
#ifdef DIRECTX_PC
	GetSprite(ID_CLAN_HELPGROUP)->HideAll();
#endif
	}
	
	ScreenBaseClass::OnInitialize();

	SetHelpRect( ID_CLAN_HELPGROUP );

	GuiScrollingText* pScrollText = CreateScrollingText( ID_CLAN_NOTICES_FRAME );

	if( pScrollText )
	{
		Sprite* pSprite = pScrollText->SpriteHost();

		if( pSprite )
			pScrollText->SetWidth( pSprite->GetWidth() );

		pScrollText->InitControl( this );
		pScrollText->Init( 11 );
		pScrollText->InitItem(  0, GetTextSprite( ID_CLAN_NOTICES_ROW1 ) );
		pScrollText->InitItem(  1, GetTextSprite( ID_CLAN_NOTICES_ROW2 ) );
		pScrollText->InitItem(  2, GetTextSprite( ID_CLAN_NOTICES_ROW3 ) );
		pScrollText->InitItem(  3, GetTextSprite( ID_CLAN_NOTICES_ROW4 ) );
		pScrollText->InitItem(  4, GetTextSprite( ID_CLAN_NOTICES_ROW5 ) );
		pScrollText->InitItem(  5, GetTextSprite( ID_CLAN_NOTICES_ROW6 ) );
		pScrollText->InitItem(  6, GetTextSprite( ID_CLAN_NOTICES_ROW7 ) );
		pScrollText->InitItem(  7, GetTextSprite( ID_CLAN_NOTICES_ROW8 ) );
		pScrollText->InitItem(  8, GetTextSprite( ID_CLAN_NOTICES_ROW9 ) );
		pScrollText->InitItem(  9, GetTextSprite( ID_CLAN_NOTICES_ROW10 ) );
		pScrollText->InitItem(  10, GetTextSprite( ID_CLAN_NOTICES_ROW11 ) );
	}

	GuiScrollBar* pScrollBar = CreateScrollBar( ID_CLAN_NOTICES_SCROLLBAR );

	if( pScrollBar )
	{
		Sprite* pSprite = pScrollBar->SpriteHost();

		pScrollBar->InitPart( GuiScrollBar::Shaft, pSprite );
		pScrollBar->InitPart( GuiScrollBar::Thumb, GetSprite( ID_CLAN_NOTICES_THUMB, pSprite ) );
		pScrollBar->InitPart( GuiScrollBar::ArrowUp, GetSprite( ID_CLAN_NOTICES_ARROWUP, pSprite ) );
		pScrollBar->InitPart( GuiScrollBar::ArrowDown, GetSprite( ID_CLAN_NOTICES_ARROWDOWN, pSprite ) );

		pScrollBar->SetRange( 0.0f, 5.0f );
		pScrollBar->SetPosition( 0.0f );
		pScrollBar->SetSteps( 5 );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every time when the screen is displayed
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenClanNotices::OnScreenIntro( void )
{
    _status = RUNNING;

    ScreenBaseClass::OnScreenIntro();
}

void ScreenClanNotices::OnCursorUp( void )
{
	ScrollTextUp();
}

void ScreenClanNotices::OnCursorDown( void )
{
	ScrollTextDown();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the scrolling text has scrolled
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/
void ScreenClanNotices::ScrollTextUp( void )
{
	GuiScrollingText* pScrollText = GetScrollingText();

	if( pScrollText )
	{
		pScrollText->ScrollLine(-1);

		PlaySoundEvent(Screen::SOUNDEVENT_INVALID);
	}

	return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when the scrolling text has scrolled
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/
void ScreenClanNotices::ScrollTextDown( void )
{
	GuiScrollingText* pScrollText = GetScrollingText();

	if( pScrollText )
	{
		pScrollText->ScrollLine(1);

		PlaySoundEvent(Screen::SOUNDEVENT_INVALID);
	}

	return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called to set the notice text on screen
///
/// \param noticeList List of the notices to display
///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenClanNotices::SetNotices(OSIClanNoticeList *noticeList)
{
	GuiScrollingText* pScrollText = GetScrollingText();
	if( pScrollText == NULL) return;

	OSIchar buffer[ 2056 ];
	buffer[0] = 0;

	if( noticeList != NULL)
	{
		noticeList->begin();
		while( OSIClanNotice* notice = noticeList->getCurrent() )
		{
			OSIsnprintf( buffer, NUM_ARRAY_ELEMENTS(buffer), "%s%s: %s\n%s\n \n", buffer,
				g_translator.Translate( "From" ), notice->posterName, notice->noticeText
			);
			noticeList->next();
		}
	}

	pScrollText->SetText(buffer);
}