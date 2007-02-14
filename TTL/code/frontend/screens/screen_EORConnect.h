#ifndef SCREEN_EORCONNECT_H
#define SCREEN_EORCONNECT_H

#include "frontend/screens/screen_BaseClass.h"

//=============================================================================
// ScreenEORConnect
//=============================================================================

class ScreenEORConnect : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenEORConnect );

	public:
        enum ScreenStatus
        {
            RUNNING,
            BACK
        };

        /** \name Constructor */
        //\{
        ScreenEORConnect();
        //\}

        /** \name Base class overrides */
        //\{
        virtual void OnScreenIntro( void );
        virtual void OnScreenExit( void );
        virtual void OnButtonCancel();
        virtual void OnScreenUpdate( void );
        //\}

        /** \name Accessors */
        //\{
        ScreenStatus GetScreenStatus() { ScreenStatus current = _status; _status = RUNNING; return current; }
        bool CancelDnas() { return _cancelDnas; }
        //\}

    private:
        ScreenStatus _status;
        bool _cancelDnas;
};

#endif // SCREEN_CONNECT_H
