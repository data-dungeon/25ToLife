///////////////////////////////////////////////////////////////////////////////////////
///  \file UIXPlayers.hpp
///
///  \brief Defines the UIX Players class
///////////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDED_UIX_PLAYERS_
#define _INCLUDED_UIX_PLAYERS_

#include "../UseCase.hpp"
#include "OSI/OSI.hpp"
#include "../StateMachine.hpp"
#include "../Singleton.hpp"
#include <uix.h>

///////////////////////////////////////////////////////////////////////////////////////
/// The UIXPlayers class.  This class handles the presentation
/// of the UIX Players on XBox to the player.
///////////////////////////////////////////////////////////////////////////////////////
namespace OnlineUI
{
    class UIXPlayers : public UseCase, public Singleton<UIXPlayers>
    {
        public:
            enum State
            {
                NORMAL,
                NUM_STATES
            };

            /** \name Mutators */
            //\{
            void setShowCurrentPlayers(bool matchLobby) { _inMatchLobby = matchLobby; }
            //\}

            /** \name UseCase public methods */
            //\{
	        virtual bool				update() { return _stateMachine.update( this ); }
            //\}

        protected:
            friend class Singleton<UIXPlayers>;

            /** \name Constructor */
            //\{
            UIXPlayers();
            //\}

            /** \name Destructor */
            //\{
            virtual ~UIXPlayers() { }
            //\}

            /** \name UseCase protected methods */
            //\{
            virtual bool				_init();
            virtual bool				_deinit();

			bool _updateNormal();
			//\}

        private:
            bool _inMatchLobby;
            
            StateMachine<State, NUM_STATES, UIXPlayers> _stateMachine;  /// < State machine
    };
}

#endif //_INCLUDED_UIX_PLAYERS_
