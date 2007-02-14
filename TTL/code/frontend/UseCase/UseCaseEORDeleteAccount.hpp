///////////////////////////////////////////////////////////////////////////////////////
///  \file UseCaseEORDeleteAccount.hpp
///
///  \brief Defines the EORDeleteAccount class
///////////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDED_EOR_DELETE_ACCOUNT_
#define _INCLUDED_EOR_DELETE_ACCOUNT_

#include "UseCase.hpp"
#include "OSI/OSI.hpp"
#include "StateMachine.hpp"
#include "Singleton.hpp"

struct ts_OnlineAccount;

///////////////////////////////////////////////////////////////////////////////////////
/// The EORDeleteAccount class.  This class handles the presentation
/// of the multiplayer choices to the user, and starting the appropriate
/// game choice.
///////////////////////////////////////////////////////////////////////////////////////
namespace OnlineUI
{
    class EORDeleteAccount : public UseCase, public Singleton<EORDeleteAccount>
    {
	    public:
		    enum State
            {
				ASK_TO_DELETE,
#ifdef DIRECTX_PC
				DELETE_ACCT,
#else
				DELETE,
#endif
                SAVE_ACCOUNT,
				SHUT_DOWN,
                NUM_STATES
            };
            
            /** \name UseCase public methods */
            //\{
            virtual bool				update() { return _stateMachine.update(this); }
			void start( ts_OnlineAccount* accountPtr );
			bool deletedAccount() { bool temp = _deletedAccount; _deletedAccount = false; return temp; }
            //\}

        protected:
	        friend Singleton<EORDeleteAccount>;
        
            /** \name Constructor */
            //\{
            EORDeleteAccount();
            //\}

            /** \name Destructor */
            //\{
            virtual ~EORDeleteAccount() { }
            //\}

            /** \name UseCase protected methods */
            //\{
            virtual bool				_init();
            virtual bool				_deinit();
            //\}

            /** \name State machine methods */
            //\{
            bool _updateInit();
			void _enterAskToDelete();
			bool _updateAskToDelete();
            void _enterDelete();

            void _enterSave();
            bool _updateSave();

			bool _updateShutDown();
            //\}
    	
			void _restoreDeletedAccount();
	    private:
            StateMachine<State, NUM_STATES, EORDeleteAccount> _stateMachine;  /// < State machine
            ts_OnlineAccount* _accountPtr; ///< The account we're trying to delete
			bool _deletedAccount;
    };
}

#endif //_INCLUDED_EOR_DELETE_ACCOUNT_
