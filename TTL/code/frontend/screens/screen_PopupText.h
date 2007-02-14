#ifndef SCREEN_POPUP_TEXT_H
#define SCREEN_POPUP_TEXT_H

#include "frontend/screens/screen_BaseClass.h"

//=============================================================================
// ScreenPopupText
//=============================================================================

class ScreenPopupText : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenPopupText );

	public:

        /** \name Base class overrides */
        //\{
        virtual void OnInitialize( void );
        virtual void OnScreenIntro( void );
        //\}

        /** \name Mutators */
        //\{
        void SetTitleText(const char *pText);
        void SetMessageText(const char *pText);
        //\}

    private:
        char _title[250];
        char _text[500];
};

#endif // SCREEN_POPUP_TEXT_H