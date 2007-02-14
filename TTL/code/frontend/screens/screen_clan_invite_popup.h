#ifndef _SCREEN_CLAN_INVITE_POPUP_H_
#define _SCREEN_CLAN_INVITE_POPUP_H_

#include "frontend/screens/screen_BaseClass.h"
#include "frontend/screens/screen_BasePopup.h"
#include "OSI/OSIClans.hpp"

class ScreenClanInvitePopup : public ScreenBasePopup
{
	DECLARE_CLASSOBJECT( ScreenClanInvitePopup );

	public:
        enum ScreenStatus
        {
            RUNNING,
            ACCEPT,
            DECLINE,
            BLOCK,
			CANCEL_SENT,
            BACK
        };

        /** \name Accessors */
        //\{
		GuiListBox* GetListBox( void );
        ScreenStatus GetScreenStatus() { ScreenStatus current = _status; _status = RUNNING; return current; }
        //\}

		/** \name Mutators */
        //\{
        virtual void SetAccessLevel(const OSIClanRole &clanRole);
        //\}

	protected:
        /** \name Base class override */
        //\{
		virtual void OnInitialize( void );
        virtual void OnCursorUp( void );
		virtual void OnCursorDown( void );
		virtual void OnButtonOK();
        virtual void OnButtonCancel() { _status = BACK; }
        virtual void OnScreenIntro( void );
        //\}

    private:
        ScreenStatus _status;
};

#endif // _SCREEN_CLAN_INVITE_POPUP_H_
