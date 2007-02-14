#ifndef _SCREEN_GAMEPADSETTINGS_H_
#define _SCREEN_GAMEPADSETTINGS_H_

//=============================================================================
// ScreenButtonMap
//=============================================================================

class ScreenButtonMap : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenButtonMap );

	public:
		ScreenButtonMap( void );

		int GetActionId( DefaultControlMapper::DIGITALBUTTON Button );
		const char* GetActionText( ActionMap::ActionType Action );

	protected:
		virtual void OnInitialize( void );
		virtual void OnCursorUp( void );
		virtual void OnCursorDown( void );
		virtual void OnCursorLeft( void );
		virtual void OnCursorRight( void );
		virtual void OnCommand( int Command );
		virtual void OnButtonCancel( void );
		virtual void OnButtonSelect( Button* pButton, Sprite* pSprite );
		virtual void OnScreenIntro( void );
		virtual void OnScreenUpdate( void );

		ActionMap m_ActionMap;
};


//=============================================================================
// ScreenThumbSticks
//=============================================================================

class ScreenThumbSticks : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenThumbSticks );

	public:
		virtual void HideThumbStickImages( void );

	protected:
		virtual void OnInitialize( void );
		virtual void OnScreenIntro( void );
		virtual void OnScreenUpdate( void );
		virtual void OnCommand( int Command );
};


#endif // _SCREEN_GAMEPADSETTINGS_H_