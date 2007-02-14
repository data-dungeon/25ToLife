#ifndef SCREEN_COUNTRYLIST_H
#define SCREEN_COUNTRYLIST_H

#include "frontend/screens/screen_BaseClass.h"

//=============================================================================
// ScreenCountryList
//=============================================================================

class ScreenCountryList : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenCountryList );

	public:
        enum ScreenStatus
        {
            RUNNING,
            DONE,
            CANCEL
        };

        /** \name Base class overrides */
        //\{
        virtual void OnInitialize( void );
        virtual void OnScreenIntro( void );
        virtual void OnButtonCancel() { _status = CANCEL; }
        virtual void OnButtonOK();
        virtual void OnCursorUp( void );
        virtual void OnCursorDown( void );
        //\}

        /** \name Accessors */
        //\{
        ScreenStatus GetStatus() { ScreenStatus current = _status; _status = RUNNING; return current; }
        int GetSelectedCountry() { return _selected; }
        //\}

        /** \name Mutators */
        //\{
        void SetSelectedCountry(int Country);
        //\}

    private:
        /** \name Utility methods */
        //\{
        GuiListBox *GetCountryListBox();
        //\}

        ScreenStatus _status;
        int _selected;
};

#endif // SCREEN_COUNTRYLIST_H
