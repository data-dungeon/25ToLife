#ifndef _SCREEN_TAGEDITOR_H_
#define _SCREEN_TAGEDITOR_H_

#include "frontend/texteffects.h"
#include "frontend/screens/screen_BaseClass.h"

class ScreenTagEditor : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenTagEditor );

	public:
		typedef enum
		{
			MENUMODE_MAIN,
			MENUMODE_COLORS,
			MENUMODE_PICKER,
			MENUMODE_KEYBOARD,
		}
		MENUMODE;

		ScreenTagEditor( void );
		virtual ~ScreenTagEditor( void );

		void ShowMainMenu( bool Show );
		void ShowColorsMenu( bool Show );
		void ShowColorPicker( bool Show );
		void ShowKeyboard( bool Show );
		bool HasChanged( void );
		void CancelChanges( void );
		void InitPlayerTags( void );
		void UpdateTagString( void );

		GuiProgressBar* m_pRedBar;
		GuiProgressBar* m_pGreenBar;
		GuiProgressBar* m_pBlueBar;

		Text m_RenderText;
		int m_ColorIndex;

		xstring m_OriginalTag;
		xstring m_TagString;
		float m_TagRed[3];
		float m_TagGreen[3];
		float m_TagBlue[3];
		int m_DialogType;

		MENUMODE m_MenuMode;
		
	protected:
		virtual void OnInitialize( void );
		virtual void OnScreenIntro( void );
		virtual void OnScreenUpdate( void );
		virtual void OnScreenRefresh( void );
		virtual void OnPopupHide( void );
		virtual void OnCommand( int Command );
		virtual void OnCursorUp( void );
		virtual void OnCursorDown( void );
		virtual void OnCursorLeft( void );
		virtual void OnCursorRight( void );
		virtual void OnCursorMovement( int ButtonId, int N );
		virtual void OnButtonOK( void );
		virtual void OnButtonCancel( void );
		virtual void OnButtonStart( void );
		virtual void OnKeyboardInput( const char* pString );
		virtual void OnDialogCommand( int Command );

		void OnCommandText( void );
		void OnCommandColors( void );
		void OnCommandDone( void );
		void OnCommandColorPicker( int n );
		void OnIllegalTag( void );

		// handler for naughty text
		CMessageOwner	m_MessageOwner;
		REF_BASIC_MESSAGE_HANDLER( TextFiltered, MESSAGE_VALUE, ScreenTagEditor ) m_TextFilteredHandler;
};

#endif // _SCREEN_TAGEDITOR_H_