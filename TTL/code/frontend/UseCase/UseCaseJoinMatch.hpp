///////////////////////////////////////////////////////////////////////////////////////
///  \file UseCaseFindMatch.hpp
///
///  \brief Defines the FindMatch class
///////////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDED_JOIN_MATCH_
#define _INCLUDED_JOIN_MATCH_

#include "UseCase.hpp"
#include "OSI/OSI.hpp"
#include "StateMachine.hpp"
#include "Singleton.hpp"

///////////////////////////////////////////////////////////////////////////////////////
/// The JoinMatch class.  This class handles joining a match and switching
/// from the OnlineSetupLoop to the NetworkLoop (or preparing for that).
/// 
/// This UseCase doesn't really have a UI element, it is simply a standardized
/// place for handling the join steps.  It stops itself upon successful join.
///////////////////////////////////////////////////////////////////////////////////////
namespace OnlineUI
{
    class JoinMatch : public UseCase, public Singleton<JoinMatch>
    {
	    public:
		    enum State
            {
				DECIDE_JOIN_METHOD,
				FIND_MATCH_OF_FRIEND,
                REQUEST_STATION_URLS_FOR_MATCH,
				REQUEST_JOIN_MATCH,
				BREAK_TO_NETWORK_LOOP,
                NUM_STATES
            };
            
            /** \name UseCase public methods */
            //\{
            virtual bool				update()							{ return _stateMachine.update(this); }
			OSIMatchID&					getMatchToJoin( void )				{ return _matchToJoin ; }
			void						setMatchToJoin( const OSIMatchID& matchID );
			void						setPlayerToJoin( const OSIPlayerID& playerID );
            //\}

            CMessageOwner d_messageOwner;  /// < Loop can receive messages

        protected:
            friend class Singleton<JoinMatch>;
            friend class SetupLaunch;

            /** \name Constructor */
            //\{
            JoinMatch();
            //\}

            /** \name Destructor */
            //\{
            virtual ~JoinMatch() { }
            //\}

            /** \name UseCase protected methods */
            //\{
            virtual bool	_init();
            virtual bool	_deinit();
            //\}

            /** \name State Machine methods */
            //\{
			void _enterDecideJoinMethod();

			void _enterFindMatchOfFriend();
			bool _updateFindMatchOfFriend();

			void _enterRequestStationURLSForMatch();
			bool _updateRequestStationURLSForMatch();

			void _enterRequestJoinMatch();
			bool _updateRequestJoinMatch();

			bool _updateBreakToNetworkLoop();
            //\}

            /** \name Utility methods */
            //\{
            //\}
   	
	    private:
			REF_SEND_MESSAGE_HANDLER(SetupLaunch, JoinMatch)	 _setupLaunchHandler;    /// < Launch match message handler
            StateMachine<State, NUM_STATES, JoinMatch>           _stateMachine;          /// < State machine

			enum JoinMethod
			{
				JOIN_PLAYER,
				JOIN_MATCH
			};
			
			JoinMethod		_joinMethod;
			OSIMatchID		_matchToJoin;
			OSIPlayerID		_playerToJoin;
    };
}

#endif //_INCLUDED_MULTIPLAYER_MENU_
