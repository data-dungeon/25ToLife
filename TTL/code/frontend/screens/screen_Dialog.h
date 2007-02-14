#ifndef SCREEN_DIALOG_H
#define SCREEN_DIALOG_H

#include "frontend/screens/screen_BaseClass.h"

//=============================================================================
// ScreenDialog
//=============================================================================

class ScreenDialog : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenDialog );

	public:
        enum ScreenStatus
        {
            RUNNING,
            DONE
        };

        /** \name Base class overrides */
        //\{
        virtual void OnInitialize( void );
        virtual void OnScreenIntro( void );
        virtual void OnButtonCancel() { _status = DONE; }
        virtual void OnCommand( int Command );
        //\}

        /** \name Accessors */
        //\{
        ScreenStatus GetStatus() { ScreenStatus current = _status; _status = RUNNING; return current; }
        //\}

        /** \name Mutators */
        //\{
        void SetTitleText(const char *pText);
        void SetDialogText(const char *pText);
        //\}

    private:
        ScreenStatus _status;
        char _title[250];
        char _text[500];
};

#endif // SCREEN_DIALOG_H