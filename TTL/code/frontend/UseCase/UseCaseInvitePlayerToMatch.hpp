///////////////////////////////////////////////////////////////////////////////////////
///  \file UseCaseInvitePlayerToMatch.hpp
///
///  \brief Defines the InvitePlayerToMatch class
///////////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDED_INVITE_PLAYER_TO_MATCH_
#define _INCLUDED_INVITE_PLAYER_TO_MATCH_

#include "UseCase.hpp"
#include "StateMachine.hpp"
#include "Singleton.hpp"

#include "OSI/OSI.hpp"

///////////////////////////////////////////////////////////////////////////////////////
/// The InvitePlayerToMatch class.  This class handles waiting on an OSI request.
///////////////////////////////////////////////////////////////////////////////////////
namespace OnlineUI
{
    class InvitePlayerToMatch : public UseCase, public Singleton<InvitePlayerToMatch>
    {
	    public:
		    enum State
            {
				LISTEN_FOR_MESSAGES,
				CHECK_NAME_WITH_CURRENT_PLAYERS,
				INVITE_PLAYER_BY_NAME,
				SHUT_DOWN,

                NUM_STATES
            };
            
            /** \name UseCase public methods */
            //\{
            virtual bool update() { return _stateMachine.update(this); }
            //\}

			CMessageOwner d_messageOwner;  /// < Loop can receive messages

        protected:
	        friend Singleton<InvitePlayerToMatch>;
        
            /** \name Constructor */
            //\{
            InvitePlayerToMatch();
            //\}

            /** \name Destructor */
            //\{
            virtual ~InvitePlayerToMatch() { }
            //\}

            /** \name UseCase protected methods */
            //\{
            virtual bool _init();
            virtual bool _deinit();
            //\}

            /** \name State Machine methods */
            //\{
			bool _updateListenForMessages();

			bool _updateCheckCurrentPlayers();

            void _enterInvitePlayerByName();
			bool _updateInvitePlayerByName();

			bool _updateShutDown();
			//\}
	    private:
			REF_SEND_MESSAGE_HANDLER(InviteByName, InvitePlayerToMatch) _inviteByNameHandler;
			REF_SEND_MESSAGE_HANDLER(CancelInviteByName, InvitePlayerToMatch) _cancelInviteByNameHandler;

            StateMachine<State, NUM_STATES, InvitePlayerToMatch> _stateMachine;  /// < State machine

			OSIchar _inviteeName[ OSI_PLAYER_NAME_LENGTH ];
    };
}

#endif //_INCLUDED_INVITE_PLAYER_TO_MATCH_