#include "TTLPCH.h"
#include "frontend/frontend.h"
#include "frontend/inputdriver.h"
#include "frontend/screens/screen_PopupText.h"
#include "data/screens/MultiplayerPopups.h"

/*virtual*/ void ScreenPopupText::OnInitialize(void)
{
    UseHighlighting(true);

	ScreenBaseClass::OnInitialize();
}

void ScreenPopupText::OnScreenIntro( void )
{
	SetText( ID_POPUP_TEXT_TITLE, _title );
	SetText( ID_POPUP_TEXT_MSG, _text );

    ScreenBaseClass::OnScreenIntro();
}

void ScreenPopupText::SetTitleText(const char *pText)
{
	strncpy(_title, pText, 250);
	
	SetText( ID_POPUP_TEXT_TITLE, _title );
}

void ScreenPopupText::SetMessageText(const char *pText)
{
	strncpy(_text, pText, 500);

	SetText( ID_POPUP_TEXT_MSG, _text );
}
