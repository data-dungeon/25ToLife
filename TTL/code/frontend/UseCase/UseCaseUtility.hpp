
#ifndef USECASE_UTILITY_INC
#define USECASE_UTILITY_INC

#include "frontend/frontend.h"
#include "frontend/screens/screen_wait.h"
#include "frontend/screens/screen_PopupText.h"
#include "UseCase.hpp"

namespace OnlineUI
{

const int MIN_ENTRY_LENGTH = 4;
const int MAX_ENTRY_LENGTH = 15;

inline void ShowPopupTextOnly( const char* const title, const char* const text )
{
	ScreenHandler* pScreenHandler = g_FrontEnd.GetScreenHandler( "SCREEN_ID_POPUP_TEXT" );

	if( pScreenHandler && 0 == strcmp( pScreenHandler->Classname(), ScreenPopupText::ClassnameStatic() ) )
	{
		ScreenPopupText* pPopup = reinterpret_cast<ScreenPopupText*>(pScreenHandler);
		pPopup->SetTitleText( title );
		pPopup->SetMessageText( text );

		g_FrontEnd.ShowPopup( "SCREEN_ID_POPUP_TEXT" );
	}
}

inline void EndPopupTextOnly( void )
{
	if( UseCase::IsScreenActive< ScreenPopupText >() )
	{
		g_FrontEnd.RemoveActivePopup();
	}
	g_FrontEnd.ClosePopup( "SCREEN_ID_POPUP_TEXT" );
}

} // namespace OnlineUI

#endif 