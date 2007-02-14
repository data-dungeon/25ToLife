#ifndef SCREEN_WAIT_H
#define SCREEN_WAIT_H

#include "frontend/screens/screen_BaseClass.h"

//=============================================================================
// ScreenWait
//=============================================================================

class ScreenWait : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenWait );

	public:
        /** \name Base class overrides */
        //\{
        virtual void OnInitialize( void );
        virtual void OnScreenIntro( void );
        virtual void OnButtonCancel() { }
        virtual void OnScreenUpdate( void );
        //\}

    private:
        int _currentPos;
};

#endif // SCREEN_WAIT_H
