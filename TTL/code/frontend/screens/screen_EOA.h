#ifndef SCREEN_EOA_H
#define SCREEN_EOA_H

#include "frontend/screens/screen_BaseClass.h"

//=============================================================================
// ScreenEOA
//=============================================================================

class ScreenEOA : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenEOA );

	public:
        enum ScreenStatus
        {
            RUNNING,
            AGREE,
            DISAGREE,
            BACK
        };

		  void LoadText( void );

        /** \name Base class overrides */
        //\{
        virtual void OnInitialize( void );
        virtual void OnScreenIntro( void );
		  virtual void OnButtonOK( void );
        virtual void OnButtonCancel() { _status = BACK; }
        virtual void OnCommand( int Command );
        virtual void OnCursorUp( void );
        virtual void OnCursorDown( void );
		  virtual void OnCursorUpdate( void );
        virtual void OnCursorLeft() { SelectPrevButton(); }
        virtual void OnCursorRight() { SelectNextButton(); }
        //\}

        /** \name Accessors */
        //\{
        ScreenStatus GetScreenStatus() { ScreenStatus current = _status; _status = RUNNING; return current; }
        //\}

    private:
        ScreenStatus _status;
};

#endif // SCREEN_EOA_H