///////////////////////////////////////////////////////////////////////////////////////
///  \file UseCaseEORCreateAccount.hpp
///
///  \brief Defines the EORCreateAccount class
///////////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDED_CREATE_ACCOUNT_
#define _INCLUDED_CREATE_ACCOUNT_

#include "UseCase.hpp"
#include "OSI/OSI.hpp"
#include "StateMachine.hpp"
#include "Singleton.hpp"
#include "UseCaseStrings.hpp"

///////////////////////////////////////////////////////////////////////////////////////
/// The EORCreateAccount class.  This class handles the presentation
/// of the multiplayer choices to the user, and starting the appropriate
/// game choice.
///////////////////////////////////////////////////////////////////////////////////////
namespace OnlineUI
{
    class EORCreateAccount : public UseCase, public Singleton<EORCreateAccount>
    {
	    public:
		    enum State
            {
                CREATE1,
                CREATE2,
                VERIFY_CREATE,
                WAIT_FOR_CREATE,
                SAVE_ACCOUNT,
				SHUT_DOWN,
                NUM_STATES
            };
            
            /** \name UseCase public methods */
            //\{
            virtual bool				update() { return _stateMachine.update(this); }
			bool createdAccount() { bool temp = _createdAccount; _createdAccount = false; return temp; }
			int getAccountCreatedOffset() { return _accountOffset; }
            //\}

        protected:
	        friend Singleton<EORCreateAccount>;

            /** \name Constructor */
            //\{
            EORCreateAccount();
            //\}

            /** \name Destructor */
            //\{
            virtual ~EORCreateAccount() { }
            //\}

            /** \name UseCase protected methods */
            //\{
            virtual bool				_init();
            virtual bool				_deinit();
            //\}

            /** \name State machine methods */
            //\{
            bool _updateCreate1();
            bool _updateCreate2();
            void _enterWait();
            bool _updateWait();
            void _enterVerifyCreate();
            bool _updateVerifyCreate();
            void _enterSave();
            bool _updateSave();
			bool _updateShutDown();
            //\}

            /** \name Utility methods */
            //\{
            EORNewAccountErrorCodes _verifyScreen1();
            EORNewAccountErrorCodes _verifyScreen2();
            void _saveAccount();
            //\}
    	
	    private:
            StateMachine<State, NUM_STATES, EORCreateAccount> _stateMachine;  /// < State machine


			OSIAccount _newAccount;
            int _accountOffset;

            // Screen 1 values
            OSIchar _onlineName[ OSI_PLAYER_NAME_LENGTH ];
            OSIchar _password[ OSI_PASSWORD_LENGTH ];
            OSIchar _passwordConfirm[ OSI_PASSWORD_LENGTH ];
            bool _savePassword;
            OSIchar _emailAddress[ OSI_EMAIL_LENGTH ];
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
			bool _createdAccount;
    };
}

#endif //_INCLUDED_CREATE_ACCOUNT_