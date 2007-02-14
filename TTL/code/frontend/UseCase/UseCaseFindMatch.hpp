///////////////////////////////////////////////////////////////////////////////////////
///  \file UseCaseFindMatch.hpp
///
///  \brief Defines the FindMatch class
///////////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDED_FIND_MATCH_
#define _INCLUDED_FIND_MATCH_

#include "UseCase.hpp"
#include "OSI/OSI.hpp"
#include "StateMachine.hpp"
#include "Singleton.hpp"
#include "frontend/UseCase/GameSettingConfig.hpp"

///////////////////////////////////////////////////////////////////////////////////////
/// The FindMatch class.  This class handles the presentation
/// of the multiplayer choices to the user, and starting the appropriate
/// game choice.
///////////////////////////////////////////////////////////////////////////////////////
namespace OnlineUI
{
    class FindMatch : public UseCase, public Singleton<FindMatch>
    {
	    public:
		    enum State
            {
				LISTEN_FOR_MESSAGES,
				FIND_MATCHES,
				JOIN_SELECTED_MATCH,
                SHUT_DOWN,
                NUM_STATES
            };
            
            /** \name UseCase public methods */
            //\{
            virtual bool				update()						{ return _stateMachine.update(this); }
			virtual void				joinMatch( int matchIndex );
            //\}

            CMessageOwner d_messageOwner;  /// < Loop can receive messages

        protected:
            friend class Singleton<FindMatch>;
            friend class CreatingMatch;
            friend class CancelCreate;
            friend class CancelResults;
            friend class GameRules;
            friend class SessionSelect;

            /** \name Constructor */
            //\{
            FindMatch();
            //\}

            /** \name Destructor */
            //\{
            virtual ~FindMatch() { }
            //\}

            /** \name UseCase protected methods */
            //\{
            virtual bool				_init();
            virtual bool				_deinit();
            //\}

            /** \name State Machine methods */
            //\{
            bool _updateListenForMessages();
			void _enterFindMatches();
			bool _updateFindMatches();
			bool _updateJoinSelectedMatch();
            bool _updateShutDown();
            //\}

            /** \name Utility methods */
            //\{
            void _createGameSettings();
            //\}
    	
	    private:
            StateMachine<State, NUM_STATES, FindMatch>           _stateMachine;          /// < State machine

			OSIMatchOptions				                         _matchOptions;
			OSIMatchList*				                         _matchList;
            REF_SEND_MESSAGE_HANDLER(ConfigureMatch, FindMatch)  _configureMatchHandler; /// < Match configuration message handler
            REF_SEND_MESSAGE_HANDLER(CancelConfig, FindMatch)    _cancelConfigHandler;   /// < Configuration screen cancel message handler
            REF_SEND_MESSAGE_HANDLER(CancelResults, FindMatch)   _cancelResultsHandler;  /// < Search results screen cancel message handler
            REF_SEND_MESSAGE_HANDLER(SessionSelect, FindMatch)   _sessionSelectHandler;  /// < Session selected message handler
            int                                                  _nSession;              /// < Storage for selected session
            bool                                                 _refresh;               /// < Refresh search results or new search results?

            // Game settings values
			StringType*		_mapNamePtr;
            EnumType*		_gameTypePtr;
			bool			_cleanUpScreens ;
    };
}

#endif //_INCLUDED_MULTIPLAYER_MENU_
