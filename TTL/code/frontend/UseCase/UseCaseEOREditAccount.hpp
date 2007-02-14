///////////////////////////////////////////////////////////////////////////////////////
///  \file UseCaseEOREditAccount.hpp
///
///  \brief Defines the EOREditAccount class
///////////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDED_EDIT_ACCOUNT_
#define _INCLUDED_EDIT_ACCOUNT_

#include "UseCase.hpp"
#include "OSI/OSI.hpp"
#include "StateMachine.hpp"
#include "Singleton.hpp"
#include "UseCaseStrings.hpp"
#include "frontend/savegame.h"

///////////////////////////////////////////////////////////////////////////////////////
/// The EOREditAccount class.  This class handles the presentation
/// of the multiplayer choices to the user, and starting the appropriate
/// game choice.
///////////////////////////////////////////////////////////////////////////////////////
namespace OnlineUI
{
    class EOREditAccount : public UseCase, public Singleton<EOREditAccount>
    {
	    public:
		    enum State
            {
                EDIT1,
                EDIT2,
                WAIT_FOR_SAVE,
                SAVE_ACCOUNT,
				SHUT_DOWN,
                NUM_STATES
            };
            
            /** \name UseCase public methods */
            //\{
            void start(ts_OnlineAccount *accountPtr);
            virtual bool update() { return _stateMachine.update(this); }
            //\}

        protected:
	        friend Singleton<EOREditAccount>;

            /** \name Constructor */
            //\{
            EOREditAccount();
            //\}

            /** \name Destructor */
            //\{
            virtual ~EOREditAccount() { }
            //\}

            /** \name UseCase protected methods */
            //\{
            virtual bool				_init();
            virtual bool				_deinit();
            //\}

            /** \name State machine methods */
            //\{
            bool _updateEdit1();
            bool _updateEdit2();
            void _enterWait();
            bool _updateWait();
            void _enterSave();
            bool _updateSave();
			bool _updateShutDown();
            //\}

            /** \name Utility methods */
            //\{
            EORNewAccountErrorCodes _verifyScreen1();
            EORNewAccountErrorCodes _verifyScreen2();
            void _populateScreen1();
            void _populateScreen2();
            //\}
    	
	    private:
            StateMachine<State, NUM_STATES, EOREditAccount> _stateMachine;  /// < State machine

            // Account values
            ts_OnlineAccount *_accountPtr;

			// Holded for the updated data
			OSIAccount _accountData;

            // Screen 1 values
            OSIchar _onlineName[ OSI_PLAYER_NAME_LENGTH ];
            OSIchar _oldPassword[ OSI_PASSWORD_LENGTH ];
            OSIchar _newPassword[ OSI_PASSWORD_LENGTH ];
            OSIchar _newPasswordConfirm[OSI_PASSWORD_LENGTH ];
            bool _savePassword;
            OSIchar _emailAddress[OSI_EMAIL_LENGTH ];
            bool _promos;
            bool _shareInfo;

            // Screen 2 values
            int _birthMonth;                   // Birthday month: 0 = January, 1 = Feburary, etc.
            unsigned char _birthDay;           // Birthday day (1-31)
            unsigned short _birthYear;         // Birthday year
            bool _gender;                      // Gender: male = true, female = false
            int _country;                      // Country the account holder is in
            char _zipCode[ OSI_ZIP_LENGTH ];   // ZipCode the account holder is in

            // Error return value
            OSIchar _errorString[ OSI_STRING_LENGTH_LONG ];

			bool _isFirstTimeThrough;
    };
}

#endif //_INCLUDED_EDIT_ACCOUNT_
