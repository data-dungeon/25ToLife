#ifndef SCREEN_YESNODIALOG_H
#define SCREEN_YESNODIALOG_H

#include "frontend/screens/screen_BaseClass.h"

//=============================================================================
// ScreenYesNoDialog
//=============================================================================

class ScreenYesNoDialog : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenYesNoDialog );

	public:
        enum ScreenStatus
        {
            RUNNING,
            YES,
            NO,
			CANCEL
        };

        /** \name Base class overrides */
        //\{
        virtual void OnInitialize( void );
        virtual void OnScreenIntro( void );
        virtual void OnButtonCancel() { _status = CANCEL; }
        virtual void OnCommand( int Command );
        //\}

        /** \name Accessors */
        //\{
        ScreenStatus GetScreenStatus() { ScreenStatus current = _status; _status = RUNNING; return current; }
        //\}

        /** \name Mutators */
        //\{
        void SetTitleText(const char *pText);
        void SetDialogText(const char *pText);
		void SetDefaultButtonToYes(bool yesButton);
        //\}

    private:
        ScreenStatus _status;
        char _title[250];
        char _text[1000];
};

#endif // SCREEN_YESNODIALOG_H
