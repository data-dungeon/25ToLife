///////////////////////////////////////////////////////////////////////////////////////
///  \file UseCaseEORRecoverAccount.hpp
///
///  \brief Defines the EORRecoverAccount class
///////////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDED_RECOVER_ACCOUNT_
#define _INCLUDED_RECOVER_ACCOUNT_

#include "UseCase.hpp"
#include "OSI/OSI.hpp"
#include "StateMachine.hpp"
#include "Singleton.hpp"
#include "UseCaseStrings.hpp"

namespace OnlineUI
{
    class EORRecoverAccount : public UseCase, public Singleton<EORRecoverAccount>
    {
	    public:
		    enum State
            {
                RECOVER1,
                WAIT_FOR_RECOVER,
				SHUT_DOWN,
                NUM_STATES
            };
            
            /** \name UseCase public methods */
            //\{
            virtual bool	update()        { return _stateMachine.update(this); }
            void            clearAccount()  { _recoveredAccount = -1; } // Call after a successful recovery
			int             getAccountNum() { return _recoveredAccount; }  // Get the offset in the OnlineAccounts[] array with the account we recovered.
			const char*     getPassword()   { return _password; }
            //\}

        protected:
	        friend Singleton<EORRecoverAccount>;
        
            /** \name Constructor */
            //\{
            EORRecoverAccount();
            //\}

            /** \name Destructor */
            //\{
            virtual ~EORRecoverAccount() { }
            //\}

            /** \name UseCase protected methods */
            //\{
            virtual bool				_init();
            virtual bool				_deinit();
            //\}

            /** \name State machine methods */
            //\{
            bool _updateRecover1();
            bool _updateWait();
			bool _updateShutDown();
            //\}

            /** \name Utility methods */
            //\{
            EORNewAccountErrorCodes _verifyScreen1();
            void _recoverAccount();
            //\}
    	
	    private:
            StateMachine<State, NUM_STATES, EORRecoverAccount> _stateMachine;  /// < State machine

            // Screen 1 values
            OSIchar _onlineName[ OSI_PLAYER_NAME_LENGTH ];
            OSIchar _password[ OSI_PASSWORD_LENGTH ];
            bool _savePassword;

            // Error return value
            OSIchar _errorString[OSI_STRING_LENGTH_LONG ];

            // Offset in OnlineAccounts[] of the recovered account
            int _recoveredAccount;
    };
}

#endif //_INCLUDED_RECOVER_ACCOUNT_
