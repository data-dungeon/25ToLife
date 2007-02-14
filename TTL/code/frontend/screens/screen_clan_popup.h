#ifndef _SCREEN_CLAN_POPUP_H_
#define _SCREEN_CLAN_POPUP_H_

#include "frontend/screens/screen_BaseClass.h"
#include "frontend/screens/screen_BasePopup.h"
#include "OSI/OSIClans.hpp"

class ScreenClanListPopup : public ScreenBasePopup
{
	DECLARE_CLASSOBJECT( ScreenClanListPopup );

	public:
        enum ScreenStatus
        {
            RUNNING,
            KICK,
            PROMOTE,
            DEMOTE,
			TRANSFER_LEADERSHIP,
            JOIN,
			SEND_FRIEND_INVITE,
            BACK
        };

        /** \name Accessors */
        //\{
		GuiListBox* GetListBox( void );
        ScreenStatus GetScreenStatus() { ScreenStatus current = _status; _status = RUNNING; return current; }
        //\}

        /** \name Mutators */
        //\{
        virtual void SetAccessLevel(const OSIClanRole &clanRole, const OSIClanMembership &selectedMember);
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

#endif // _SCREEN_CLAN_POPUP_H_