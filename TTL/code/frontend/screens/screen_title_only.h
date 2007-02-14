#ifndef SCREEN_TITLE_ONLY_H
#define SCREEN_TITLE_ONLY_H

#include "frontend/screens/screen_BaseClass.h"

class ScreenTitleOnly : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenTitleOnly );

	public:
        /** \name Base class overrides */
        //\{
        virtual void OnInitialize( void );
        virtual void OnScreenIntro( void );
        virtual void OnButtonCancel() { }
        virtual void OnScreenUpdate( void );
        //\}

		void SetTitle( const char* title );

    private:
};

#endif // SCREEN_TITLE_ONLY_H
