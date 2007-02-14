///////////////////////////////////////////////////////////////////////////////////////
///  \file UIXLogin.hpp
///
///  \brief Defines the UIX Login class
///////////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDED_UIX_LOGIN_
#define _INCLUDED_UIX_LOGIN_

#include "../UseCase.hpp"
#include "OSI/OSI.hpp"
#include "../StateMachine.hpp"
#include "XboxUIXFontRenderer.hpp"
#include "XboxUIXAudioPlugin.hpp"
#include "../Singleton.hpp"

///////////////////////////////////////////////////////////////////////////////////////
/// The UIXLogin class.  This class handles the presentation
/// of the UIX login on XBox to the player.
///////////////////////////////////////////////////////////////////////////////////////
namespace OnlineUI
{
    class UIXLogin : public UseCase, public Singleton<UIXLogin>
    {
        public:
            enum State
            {
                WAITING_LOGIN,
                REQUEST_LOGOUT,
                WAITING_LOGOUT,
				VERIFY_REBOOT,
                NUM_STATES
            };

            /** \name UseCase public methods */
            //\{
	        virtual bool				update();
            //\}

        protected:
            friend Singleton<UIXLogin>;

            /** \name Constructor */
            //\{
            UIXLogin();
            //\}

            /** \name Destructor */
            //\{
            virtual ~UIXLogin() { }
            //\}

            /** \name UseCase protected methods */
            //\{
            virtual bool				_init();
            virtual bool				_deinit();
            //\}

            /** \name State machine update methods */
            //\{
            bool updateWaitingLogin();
            bool updateRequestLogout();
            bool updateWaitingLogout();
			void enterVerifyReboot();
			bool updateVerifyReboot();
            //\}

        private:
            /** \name Utility methods */
            //\{
            bool _startUIXLogin( XONLINE_LOGON_STATE* logonState = 0, bool allowSilent = true );
            //\}

            UIXFontRenderer*	_uixFontRenderer;
			UIXAudioPlugin*		_uixAudioPlugin;
			UIX_LOGON_TYPE		_lastAttemptedLoginType;
            
            StateMachine<State, NUM_STATES, UIXLogin> _stateMachine;  /// < State machine
    };
}

#endif //_INCLUDED_UIX_LOGIN_
