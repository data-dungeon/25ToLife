///////////////////////////////////////////////////////////////////////////////////////////////////
/// \file UseCaseEORPassword.hpp
///
/// \brief The use case for the forgot EORPassword c
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _INCLUDED_EORPassword_
#define _INCLUDED_EORPassword_

#include "UseCase.hpp"
#include "OSI/OSI.hpp"
#include "StateMachine.hpp"
#include "Singleton.hpp"

namespace OnlineUI
{
    class EORPassword : public UseCase, public Singleton<EORPassword>
    {
        public:
            enum State
            {
                MAKE_REQUEST,
                SUCCESS,
                FAILURE,
                NUM_STATES
            };

            /** \name UseCase public methods */
            //\{
            virtual bool				update() { return _stateMachine.update(this); }
            //\}

        protected:
            friend class Singleton<EORPassword>;

            /** \name Constructor */
            //\{
            EORPassword();
            //\}

            /** \name Destructor */
            //\{
            virtual ~EORPassword() { }
            //\}

            /** \name UseCase protected methods */
            //\{
            virtual bool				_init();
            virtual bool				_deinit();
            //\}

            /** \name State machine methods */
            //\{
            void _enterSuccess();
            void _enterFailure();
            bool _updateMakeRequest();
            //\}

        private:
            StateMachine<State, NUM_STATES, EORPassword> _stateMachine;  /// < State machine
    };
}

#endif //_INCLUDED_EORPassword_
