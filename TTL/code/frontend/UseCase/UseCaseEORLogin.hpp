///////////////////////////////////////////////////////////////////////////////////////
///  \file UseCaseEORLogin.hpp
///
///  \brief Defines the EORLogin class
///////////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDED_EOR_LOGIN_
#define _INCLUDED_EOR_LOGIN_

#include "UseCase.hpp"
#include "OSI/OSI.hpp"
#include "StateMachine.hpp"
#include "Singleton.hpp"
#include "frontend/savegame.h"

///////////////////////////////////////////////////////////////////////////////////////
/// The EORLogin class.  This class handles the presentation
/// of the multiplayer choices to the user, and starting the appropriate
/// game choice.
///////////////////////////////////////////////////////////////////////////////////////
namespace OnlineUI
{
    class EORLogin : public UseCase, public Singleton<EORLogin>
    {
	    public:
		    enum State
            {
                RETRIEVING_ACCOUNT_LIST,
				UPDATE_ACCOUNTS,
		        MENU_LOGIN,
				MENU_LOGIN_UPDATE,
				LOGIN_WITH_CURRENT_ACCOUNT,
				ASK_FOR_PASSWORD,
                RECOVER_ACCOUNT,
                STARTUP_STATE,
                DELETE_ACCOUNT,
				ASK_TO_DISCO,
				SHUT_DOWN,
                NUM_STATES
            };
            
            /** \name UseCase public methods */
            //\{
            virtual bool				update() { return _stateMachine.update(this); }
            //\}

        protected:
	        friend Singleton<EORLogin>;
        
            /** \name Constructor */
            //\{
            EORLogin();
            //\}

            /** \name Destructor */
            //\{
            virtual ~EORLogin() { }
            //\}

            /** \name UseCase protected methods */
            //\{
            virtual bool				_init();
            virtual bool				_deinit();
			virtual void				_onOpen();
            //\}

            /** \name State Machine methods */
            //\{
            void _enterMenuLogin();
		    bool _updateMenuLogin();

			bool _updateLoginWithCurrentAccount();

			void _enterAskPassword();
			bool _updateAskPassword();
		    
            bool _updateAccountInfo();
            bool _updateRecoverAccount();

            void _enterDelete();
            bool _updateDelete();

			void _enterAskToDisco();
            bool _updateAskToDisco();

			bool _updateShutDown();
            //\}

            /** \name Utility methods */
            //\{
			void _checkPassword();
            void _login();
            //\}
    	
	    private:
            StateMachine<State, NUM_STATES, EORLogin> _stateMachine;  /// < State machine
            const char* _password;                                    /// < Account password either entered or saved
			bool _justCreatedAccount;						  /// < Was an account just succesfully created?
    };
}

#endif //_INCLUDED_EOR_LOGIN_
