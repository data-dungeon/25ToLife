///////////////////////////////////////////////////////////////////////////////////////
///  \file UIXFriends.hpp
///
///  \brief Defines the UIX friends class
///////////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDED_UIX_FRIENDS_
#define _INCLUDED_UIX_FRIENDS_

#include "../UseCase.hpp"
#include "OSI/OSI.hpp"
#include "../StateMachine.hpp"
#include "../Singleton.hpp"

///////////////////////////////////////////////////////////////////////////////////////
/// The UIXFriends class.  This class handles the presentation
/// of the UIX friends on XBox to the player.
///////////////////////////////////////////////////////////////////////////////////////
namespace OnlineUI
{
    class UIXFriends : public UseCase, public Singleton<UIXFriends>
    {
        public:
            enum State
            {
				NORMAL,
                NUM_STATES
            };

            /** \name Mutators */
            //\{
            void setAllowGameInvites(bool matchLobby) { _inMatchLobby = matchLobby; }
            //\}

            /** \name UseCase public methods */
            //\{
			virtual bool				update() { return _stateMachine.update( this ); }
            //\}

        protected:
            friend class Singleton<UIXFriends>;

            /** \name Constructor */
            //\{
            UIXFriends();
            //\}

            /** \name Destructor */
            //\{
            virtual ~UIXFriends() { }
            //\}

            /** \name UseCase protected methods */
            //\{
            virtual bool				_init();
            virtual bool				_deinit();
			
			bool _updateNormal();
            //\}

        private:
            bool _inMatchLobby;
            
            StateMachine<State, NUM_STATES, UIXFriends> _stateMachine;  /// < State machine
    };
}

#endif //_INCLUDED_UIX_FRIENDS_
