#ifndef SCREEN_EORCREATEACCOUNT1_H
#define SCREEN_EORCREATEACCOUNT1_H

#include "frontend/screens/screen_BaseClass.h"
#include "OSI/OSI.hpp"

//=============================================================================
// ScreenEORCreateAccount1
//=============================================================================

class ScreenEORCreateAccount1 : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenEORCreateAccount1 );

	public:
        enum ScreenStatus
        {
            RUNNING,
            LATEST_EIDOS,
            SHARE_INFO,
			ENTER_NAME,
			ENTER_PASSWORD,
			ENTER_CONFIRM_PASSWORD,
			ENTER_EMAIL,
            CONTINUE,
            BACK
        };

        /** \name Base class overrides */
        //\{
        virtual void OnInitialize( void );
        virtual void OnScreenIntro( void );
		virtual void OnPrevScreen( Screen* pPrevScreen );
        virtual void OnButtonCancel() { _inUse = false; _status = BACK; }
        virtual void OnCommand( int Command );
		  virtual void OnKeyboardInput( const char* pString );
        virtual void OnCursorLeft( void );
		virtual void OnCursorRight( void );
        //\}

        /** \name Accessors */
        //\{
        ScreenStatus GetScreenStatus() { ScreenStatus current = _status; _status = RUNNING; return current; }
        const char *getName() { return _name; }
        const char *getPassword() { return _password; }
        const char *getPasswordConfirm() { return _confirm; }
        bool getSavePassword() { return _savePassword; }
        const char *getEmail() { return _email; }
        bool getPromos() { return _promos; }
        bool getShareInfo() { return _shareInfo; }
        //\}

        /** \name Mutators */
        //\{
        void setName(const char *pName);
        void setPassword(const char *pPassword);
        void setPasswordConfirm(const char *pConfirm);
        void setSavePassword(bool Save);
        void setEmail(const char *pEmail);
        void setPromos(bool Promos);
        void setShareInfo(bool ShareInfo);
        void setUsing() { _inUse = true; }
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
        OSIchar _confirm[OSI_PASSWORD_LENGTH ];
        OSIchar _email[OSI_EMAIL_LENGTH ];
        bool _savePassword;
        bool _promos;
		bool _shareInfo;
		bool _inUse;

        ScreenStatus _status;  /// < Last button selected on dialog
        TextField _textField;  /// < The text field last selected
};

#endif // SCREEN_EORCREATEACCOUNT1_H
