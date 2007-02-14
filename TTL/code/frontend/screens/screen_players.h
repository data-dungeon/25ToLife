#ifndef SCREEN_PLAYERS_H
#define SCREEN_PLAYERS_H

#include "frontend/screens/screen_BaseClass.h"

//=============================================================================
// ScreenPlayers
//=============================================================================

class ScreenPlayers : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenPlayers );

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
        virtual void OnCommand( int Command );
        //\}

        /** \name Accessors */
        //\{
        ScreenStatus GetScreenStatus() { ScreenStatus current = _status; _status = RUNNING; return current; }
        //\}

    private:
        ScreenStatus _status;
};

#endif // SCREEN_PLAYERS_H
