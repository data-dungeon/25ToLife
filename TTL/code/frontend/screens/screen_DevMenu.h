#ifndef _SCREEN_DEVMENU_H_
#define _SCREEN_DEVMENU_H_

#include "frontend/screens/screen_BaseClass.h"

class ScreenDevMenu : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenDevMenu );

	public:
		ScreenDevMenu( void );
		virtual ~ScreenDevMenu( void );

		int m_GameEnv;

	protected:
		virtual void OnInitialize( void );
		virtual void OnScreenIntro( void );
		virtual void OnCursorLeft( void );
		virtual void OnCursorRight( void );
		virtual void OnButtonOK( void );
		virtual void OnButtonCancel( void );
		virtual void OnButtonSelect( Button* pButton, Sprite* pSprite );
		virtual void OnButtonUnselect( Button* pButton, Sprite* pSprite );

		void OnChangeOption( int Option, int N );
		void OnChangeLevel( int Direction );
		void OnChangeCharacter( int Direction );

		void SetLevelName( const char* pText );
		void SetCharacterName( const char* pText, int CharacterId );

		Text* GetLevelNameText( void );
		Text* GetCharacterNameText( void );
};

#endif // _SCREEN_DEVMENU_H_