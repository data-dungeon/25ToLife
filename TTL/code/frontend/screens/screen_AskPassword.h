#ifndef _SCREEN_ASK_PASSWORD_POPUP_H_
#define _SCREEN_ASK_PASSWORD_POPUP_H_

#include "frontend/screens/screen_BaseClass.h"
#include "frontend/screens/screen_BasePopup.h"

class ScreenAskPasswordPopup : public ScreenBasePopup
{
	DECLARE_CLASSOBJECT( ScreenAskPasswordPopup );

	public:
        enum ScreenStatus
        {
            RUNNING,
            ENTER_PASSWORD,
            BACK
        };

        /** \name Accessors */
        //\{
        ScreenStatus GetScreenStatus() { ScreenStatus current = _status; _status = RUNNING; return current; }
        //\}

        /** \name Mutators */
        //\{
        
        //\}

	protected:
        /** \name Base class override */
        //\{
		virtual void OnInitialize( void );
        virtual void OnCommand(int Command);
		virtual void OnButtonOK() { OnCommand( m_pScreen->GetCurrentButton()->GetButtonId() ); }
        virtual void OnButtonCancel() { _status = BACK; }
        virtual void OnScreenIntro( void );
        //\}

    private:
        ScreenStatus _status;
};

#endif // _SCREEN_CLAN_POPUP_H_