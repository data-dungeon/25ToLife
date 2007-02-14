///////////////////////////////////////////////////////////////////////////////////////
///  \file EOA.hpp
///
///  \brief Defines the EOA class
///////////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDED_EOA_
#define _INCLUDED_EOA_

#include "UseCase.hpp"
#include "OSI/OSI.hpp"
#include "StateMachine.hpp"
#include "Singleton.hpp"

///////////////////////////////////////////////////////////////////////////////////////
/// The EOA class.  This class handles the presentation
/// of the Eidos Online Agreement to the player.
///////////////////////////////////////////////////////////////////////////////////////
namespace OnlineUI
{
    class EOREOA : public UseCase, public Singleton<EOREOA>
    {
        public:
            enum State
            {
                INIT,
				DNS_LOOKUP,
				ASK_TO_DISCO,
                NUM_STATES
            };

            /** \name UseCase public methods */
            //\{
            virtual bool				update() { return _stateMachine.update(this); }
            //\}

        protected:
            friend class Singleton<EOREOA>;

            /** \name Constructor */
            //\{
            EOREOA();
            //\}

            /** \name Destructor */
            //\{
            virtual ~EOREOA() { }
            //\}

            /** \name UseCase protected methods */
            //\{
            virtual bool				_init();
            virtual bool				_deinit();
            //\}

            /** \name State machine methods */
            //\{
            bool _updateInit();

			void _enterDNSLookup();
			bool _updateDNSLookup();

			void _enterAskToDisco();
			bool _updateAskToDisco();
            //\}

        private:
            StateMachine<State, NUM_STATES, EOREOA> _stateMachine;  /// < State machine
    };
}

#endif //_INCLUDED_EOA_
