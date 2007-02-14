#ifndef SCREEN_MULTIPLAYERMENU_H
#define SCREEN_MULTIPLAYERMENU_H

#include "frontend/screens/screen_BaseClass.h"

//=============================================================================
// ScreenMultiplayerMenu
//=============================================================================

class ScreenMultiplayerMenu : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenMultiplayerMenu );

	public:
        enum ScreenStatus
        {
            RUNNING,
            QUICKMATCH,
            FINDMATCH,
            HOSTMATCH,
            FRIENDS,
            PLAYERLIST,
            STATS,
            SETTINGS,
            CLAN,
            ONLINE_STATUS,
            LOGOUT
        };

        /** \name Base class overrides */
        //\{
        virtual void OnInitialize( void );
        virtual void OnScreenIntro( void );
        virtual void OnButtonCancel() { _status = LOGOUT; }
        virtual void OnCommand( int Command );
        //\}

        /** \name Accessors */
        //\{
        ScreenStatus GetScreenStatus() { ScreenStatus current = _status; _status = RUNNING; return current; }
        //\}

        /** \name Mutators */
        //\{
        void SetOnlineStatus(bool status);
        void SetOnlineName(const char *pName);
        void SetDescription(const char *pDescription);
		void SetMessageOfTheDay(const char* message);
        //\}


    private:
        ScreenStatus _status;
};

#endif // SCREEN_MULTIPLAYERMENU_H
