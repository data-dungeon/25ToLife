#ifndef SCREEN_SANDBOX_H
#define SCREEN_SANDBOX_H

#include "frontend/screens/screen_BaseClass.h"

//=============================================================================
// ScreenSandbox
//=============================================================================

class ScreenSandbox : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenSandbox );

	public:
        enum ScreenStatus
        {
            RUNNING,
            TTL1,
            TTL2,
            TTL3,
            BACK
        };

        /** \name Base class overrides */
        //\{
        virtual void OnInitialize( void );
        virtual void OnScreenIntro( void );
        virtual void OnButtonCancel() { _status = BACK; }
        virtual void OnCommand( int Command );
        //virtual void OnButtonSelect( void );
        //\}

        /** \name Accessors */
        //\{
        ScreenStatus GetScreenStatus() { ScreenStatus current = _status; _status = RUNNING; return current; }
        //\}

    private:
        ScreenStatus _status;
};

#endif // SCREEN_SANDBOX_H
