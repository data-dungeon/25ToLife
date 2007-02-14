///////////////////////////////////////////////////////////////////////////////////////
///  \file UseCaseEORConnect.hpp
///
///  \brief Defines the EORConnect class
///////////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDED_EORConnect_
#define _INCLUDED_EORConnect_

#include "UseCase.hpp"
#include "OSI/OSI.hpp"
#include "OSI/OSIchar.hpp"
#include "StateMachine.hpp"
#include "Singleton.hpp"

namespace OnlineUI
{
    class EORConnect : public UseCase, public Singleton<EORConnect>
    {
	    public:
		    enum State
            {
                CONNECTING,
				REQUEST_ACCOUNT_INFO,
                SAVE_ACCOUNT,
                SHUT_DOWN,
                NUM_STATES
            };
            
            /** \name UseCase public methods */
            //\{
            virtual bool update() { return _stateMachine.update(this); }
            //\}


            /** \name Mutators */
            //\{
            void setLoginName(const OSIchar *pName);
            void setLoginPassword(const OSIchar *pPassword);
            //\}

        protected:
	        friend Singleton<EORConnect>;
        
            /** \name Constructor */
            //\{
            EORConnect();
            //\}

            /** \name Destructor */
            //\{
            virtual ~EORConnect() { }
            //\}

            /** \name UseCase protected methods */
            //\{
            virtual bool				_init();
            virtual bool				_deinit();
            //\}

            /** \name State machine methods */
            //\{
            void _enterConnecting();
            bool _updateConnecting();

			void _enterRequestAccountInfo();
            bool _updateRequestAccountInfo();
            
            void _enterSave();
            bool _updateSave();

            bool _updateShutDown();
            //\}
    	
	    private:
            StateMachine<State, NUM_STATES, EORConnect> _stateMachine;  /// < State machine
            OSIchar _login[ OSI_PLAYER_NAME_LENGTH ];
            OSIchar _password[ OSI_PASSWORD_LENGTH ];

			bool _connecting;
    };
}

#endif //_INCLUDED_MULTIPLAYER_MENU_