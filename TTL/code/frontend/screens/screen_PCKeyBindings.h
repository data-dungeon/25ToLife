#ifndef SCREEN_PCKeyBindings_H
#define SCREEN_PCKeyBindings_H

#include "frontend/screens/screen_BaseClass.h"

//=============================================================================
// ScreenPCKeyBindings
//=============================================================================

class ScreenPCKeyBindings : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenPCKeyBindings );

	public:
        /** \name Base class overrides */
        //\{
        virtual void OnInitialize( void );
        virtual void OnScreenIntro( void );
        virtual void OnCommand( int Command );
		virtual void OnScreenUpdate( void );
		virtual void HandleInput(UIHandler *pUIHandler);
        //\}

    private:
		// does the job of updating the screen
		// to reflect currently pressed keys
		void UpdateCurrentKeys(); 
		void OnDialogCommand( int command );
		bool enterPressed;

};

#endif // SCREEN_PCKeyBindings_H
