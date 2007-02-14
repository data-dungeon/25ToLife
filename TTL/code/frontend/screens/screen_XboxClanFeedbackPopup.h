#ifndef _SCREEN_FRIENDSPOPUP_H_
#define _SCREEN_FRIENDSPOPUP_H_

#include "frontend/screens/screen_BaseClass.h"
#include "frontend/screens/screen_BasePopup.h"
#include "OSI/OSIPlayer.hpp"

class ScreenXboxClanFeedbackPopup : public ScreenBasePopup
{
	DECLARE_CLASSOBJECT( ScreenXboxClanFeedbackPopup );

	public:
		enum ScreenState {
			RUNNING,
			REPORT_OFFENSIVE_ABBREV,
			REPORT_OFFENSIVE_NAME,
			BACK,
		};

		/** \name Accessors */
        //\{
		GuiListBox* GetListBox( void );
        ScreenState GetScreenState() { ScreenState current = _screenState; _screenState = RUNNING; return current; }
		//\}

	protected:
		/** \name Base class override */
        //\{
		virtual void OnInitialize( void );
		virtual void OnCursorUp( void );
		virtual void OnCursorDown( void );
		virtual void OnButtonOK( void );
		virtual void OnButtonCancel() { _screenState = BACK; }
		//\}

		ScreenState _screenState;
};

#endif // _SCREEN_FRIENDSPOPUP_H_