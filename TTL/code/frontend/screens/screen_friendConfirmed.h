#ifndef SCREEN_FRIENDCONFIRMED_H
#define SCREEN_FRIENDCONFIRMED_H

#include "frontend/screens/screen_BaseClass.h"

//=============================================================================
// ScreenFriendConfirmed
//=============================================================================

class ScreenFriendConfirmed : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenFriendConfirmed );

	public:
        enum ScreenStatus
        {
            RUNNING,
            JOIN,
            INVITE,
            REMOVE,
            BACK
        };

        /** \name Base class overrides */
        //\{
        virtual void OnInitialize( void );
        virtual void OnScreenIntro( void );
        virtual void OnButtonCancel() { _status = BACK; }
        virtual void OnCommand( int Command );
        //\}

        /** \name Accessors */
        //\{
        ScreenStatus GetScreenStatus() { ScreenStatus current = _status; _status = RUNNING; return current; }
        //\}

        /** \name Mutators */
        //\{
        void SetFriendName(const char *pName);
        //\}

    private:
        ScreenStatus _status;
};

#endif // SCREEN_FRIENDCONFIRMED_H
