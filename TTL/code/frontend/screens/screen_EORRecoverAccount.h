#ifndef SCREEN_EORRECOVERACCOUNT_H
#define SCREEN_EORRECOVERACCOUNT_H

#include "frontend/screens/screen_BaseClass.h"
#include "OSI/OSI.hpp"

//=============================================================================
// ScreenEORRecoverAccount
//=============================================================================

class ScreenEORRecoverAccount : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenEORRecoverAccount );

	public:
        enum ScreenStatus
        {
            RUNNING,
			ENTER_NAME,
			ENTER_PASSWORD,
            RECOVER,
            BACK
        };

        /** \name Base class overrides */
        //\{
        virtual void OnInitialize( void );
        virtual void OnScreenIntro( void );
		virtual void OnPrevScreen( Screen* pPrevScreen );
        virtual void OnButtonCancel() { _inUse = false; _status = BACK; }
        virtual void OnCommand( int Command );
        virtual void OnKeyboardInput( const char* keyboardString );
        virtual void OnCursorLeft( void );
		virtual void OnCursorRight( void );
        //\}

        /** \name Accessors */
        //\{
        ScreenStatus GetScreenStatus() { ScreenStatus current = _status; _status = RUNNING; return current; }
        const char *getName() { return _name; }
        const char *getPassword() { return _password; }
        bool getSavePassword() { return _savePassword; }
        //\}

        /** \name Mutators */
        //\{
        void setName(const char *pName);
        void setPassword(const char *pPassword);
        void setSavePassword(bool Save);
        //\}

    private:

        /// The text field selected for the virtual keyboard
        enum TextField
        {
            NONE,
            NAME,
            PASSWORD,
            CONFIRM,
            EMAIL,
            NUM_FIELDS
        };

        // Field values
        OSIchar _name[OSI_PLAYER_NAME_LENGTH ];
        OSIchar _password[OSI_PASSWORD_LENGTH ];
        bool _savePassword;
        bool _inUse;

        ScreenStatus _status;  /// < Last button selected on dialog
        TextField _textField;  /// < The text field last selected
};

#endif // SCREEN_EORRECOVERACCOUNT_H
