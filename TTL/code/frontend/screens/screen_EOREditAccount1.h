#ifndef SCREEN_EOREDITACCOUNT1_H
#define SCREEN_EOREDITACCOUNT1_H

#include "frontend/screens/screen_BaseClass.h"
#include "OSI/OSI.hpp"

//=============================================================================
// ScreenEOREditAccount1
//=============================================================================

class ScreenEOREditAccount1 : public ScreenBaseClass
{
	DECLARE_CLASSOBJECT( ScreenEOREditAccount1 );

	public:
        enum ScreenStatus
        {
            RUNNING,
            LATEST_EIDOS,
            SHARE_INFO,
			ENTER_NAME,
			ENTER_OLD_PASSWORD,
			ENTER_NEW_PASSWORD,
			ENTER_NEW_CONFIRM_PASSWORD,
			ENTER_EMAIL,
            CONTINUE,
            BACK
        };

        /** \name Base class overrides */
        //\{
        virtual void OnInitialize( void );
        virtual void OnScreenIntro( void );
		virtual void OnPrevScreen( Screen* pPrevScreen );
        virtual void OnButtonCancel() { _status = BACK; }
        virtual void OnCommand( int Command );
        virtual void OnKeyboardInput( const char* keyboardString );
        virtual void OnCursorLeft( void );
		virtual void OnCursorRight( void );
        //\}

        /** \name Accessors */
        //\{
        ScreenStatus GetScreenStatus() { ScreenStatus current = _status; _status = RUNNING; return current; }
        const char *getName() { return _name; }
		const char *getOldPassword() { return _oldPassword; }
        const char *getNewPassword() { return _password; }
        const char *getNewPasswordConfirm() { return _confirm; }
        bool getSavePassword() { return _savePassword; }
        const char *getEmail() { return _email; }
        bool getPromos() { return _promos; }
        bool getShareInfo() { return _shareInfo; }
        //\}

        /** \name Mutators */
        //\{
        void setName(const char *pName);
		void setOldPassword(const char *pOldPassword);
        void setNewPassword(const char *pPassword);
        void setNewPasswordConfirm(const char *pConfirm);
        void setSavePassword(bool Save);
        void setEmail(const char *pEmail);
        void setPromos(bool Promos);
        void setShareInfo(bool ShareInfo);
        //\}

    private:

        /// The text field selected for the virtual keyboard
        enum TextField
        {
            NONE,
            NAME,
			OLD_PASSWORD,
            NEW_PASSWORD,
            NEW_CONFIRM,
            EMAIL,
            NUM_FIELDS
        };

        // Field values
        OSIchar _name[OSI_PLAYER_NAME_LENGTH ];
		OSIchar _oldPassword[OSI_PASSWORD_LENGTH ];
        OSIchar _password[OSI_PASSWORD_LENGTH ];
        OSIchar _confirm[OSI_PASSWORD_LENGTH ];
        OSIchar _email[OSI_EMAIL_LENGTH ];
        bool _savePassword;
        bool _promos;
		bool _shareInfo;

        ScreenStatus _status;  /// < Last button selected on dialog
        TextField _textField;  /// < The text field last selected
};

#endif // SCREEN_EOREDITACCOUNT1_H
