#ifndef SCREEN_FRIEND_REQUESTED_H
#define SCREEN_FRIEND_REQUESTED_H

#include "frontend/screens/screen_BaseClass.h"

//=============================================================================
// ScreenFriendRequested
//=============================================================================

class ScreenFriendRequested : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenFriendRequested );

	public:
        enum ScreenStatus
        {
            RUNNING,
            YES,
            NO,
            BLOCK,
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
        void SetFriendName(const char *pFriendName);
        //\}

    private:
        ScreenStatus _status;
};

#endif // SCREEN_FRIEND_REQUESTED_H
