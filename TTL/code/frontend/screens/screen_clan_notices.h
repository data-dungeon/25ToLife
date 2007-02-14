#ifndef SCREEN_CLANNOTICES_H
#define SCREEN_CLANNOTICES_H

#include "frontend/texteffects.h"
#include "frontend/screens/screen_BaseClass.h"

//=============================================================================
// ScreenClanNotices
//=============================================================================

class ScreenClanNotices : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenClanNotices );

	public:
        enum ScreenStatus
        {
            RUNNING,
            BACK
        };

        /** \name Base class overrides */
        //\{
        virtual void OnInitialize( void );
        virtual void OnScreenIntro( void );
        virtual void OnButtonCancel() { _status = BACK; }

		virtual void OnCursorUp( void );
        virtual void OnCursorDown( void );
		virtual void ScrollTextUp();
		virtual void ScrollTextDown();
        //\}

        /** \name Accessors */
        //\{
        ScreenStatus GetScreenStatus() { ScreenStatus current = _status; _status = RUNNING; return current; }
        //\}

		/** \name Mutators */
        //\{
		void SetNotices(OSIClanNoticeList *noticeList);
		//\}

    private:
        ScreenStatus _status;
};

#endif // SCREEN_CLANNOTICES_H
