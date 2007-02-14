///////////////////////////////////////////////////////////////////////////////////////
///  \file UseCaseFriends.hpp
///
///  \brief Defines the Friends class
///////////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDED_FRIENDS_
#define _INCLUDED_FRIENDS_

#include "UseCase.hpp"
#include "OSI/OSI.hpp"
#include "StateMachine.hpp"
#include "Singleton.hpp"
#include "frontend/screens/screen_FriendsList.h"


///////////////////////////////////////////////////////////////////////////////////////
/// The Friends class.  This class handles the presentation
/// of the multiplayer choices to the user, and starting the appropriate
/// game choice.
///////////////////////////////////////////////////////////////////////////////////////
namespace OnlineUI
{
    class Friends : public UseCase, public Singleton<Friends>
    {
	    public:
		    enum State
            {
                DISPLAY_FRIENDS_MENU,
				DISPLAY_FRIENDS_LIST_MENU_START,
				DISPLAY_FRIENDS_LIST_MENU,
				DISPLAY_FRIENDS_MENU_POPUP,
				DISPLAY_ASK_MENU,
				REQUEST_CLAN_INFO,
				REQUEST_FRIENDS_LIST,
				REQUEST_ADD_FRIEND,
				REQUEST_ACCEPT_FRIENDSHIP,
				REQUEST_DENY_FRIENDSHIP,
				REQUEST_BLOCK_FRIENDSHIP,
				REQUEST_CANCEL_FRIEND_REQUEST,
				REQUEST_END_FRIENDSHIP,
				REQUEST_INVITE_FRIEND_TO_MATCH,
				REQUEST_JOIN_MATCH_OF_FRIEND,
				REQUEST_SEND_CLAN_INVITE,
                NUM_STATES
            };
            
            /** \name UseCase public methods */
            //\{
            virtual bool				update() { return _stateMachine.update(this); }
            //\}

        protected:
            friend class Singleton<Friends>;

            /** \name Constructor/Destructor */
            //\{
										Friends();
            virtual						~Friends() { }
            //\}


            /** \name UseCase protected methods */
            //\{
            virtual bool				_init();
            virtual bool				_deinit();
            //\}

            /** \name Update methods */
            //\{
            bool						_updateDisplayFriendsMenu( void );

			void						_enterDisplayFriendsListMenuStart( void );
			void						_enterDisplayFriendsListMenu( void );
			bool						_updateDisplayFriendsListMenu( void );

			void						_enterDisplayFriendsMenuPopup( void );
			bool						_updateDisplayFriendsMenuPopup( void );

			void						_enterDisplayAskMenu( void );
			bool						_updateDisplayAskMenu( void );

			void						_enterRequestClanInfo( void );
			bool						_updateRequestClanInfo( void );

			void						_enterRequestFriendsList( void );
			bool						_updateRequestFriendsList( void );

			void						_enterRequestAddFriend( void );
			bool						_updateRequestAddFriend( void );

			void						_enterRequestAcceptFriendship( void );
			bool						_updateRequestAcceptFriendship( void );

			void						_enterRequestDenyFriendship( void );
			bool						_updateRequestDenyFriendship( void );

			void						_enterRequestBlockFriendship( void );
			bool						_updateRequestBlockFriendship( void );

			void						_enterRequestCancelFriendRequest( void );
			bool						_updateRequestCancelFriendRequest( void );

			void						_enterRequestEndFriendship( void );
			bool						_updateRequestEndFriendship( void );

			void						_enterRequestInviteFriendToMatch( void );
			bool						_updateRequestInviteFriendToMatch( void );

			void						_enterRequestSendClanInvite( void );
			bool						_updateRequestSendClanInvite( void );

			bool						_updateRequestJoinMatchOfFriend( void );
            //\}
    	
	    private:
            StateMachine<State, NUM_STATES, Friends> _stateMachine;  /// < State machine
			OSIchar						_newFriendName[ OSI_PLAYER_NAME_LENGTH ];
			OSIPlayerID					_newFriendID ;
			OSIPlayer*					_selectedPlayer ;

			ScreenFriendsList::DisplayMode _friendsListDisplayMode;
			State						_stateAfterRequestingFriendsList;

			const OSIchar*				_askTitle;
			const OSIchar*				_askMessage;
			State						_askYes, _askNo;
			bool						_cleanUpScreens ;
    };
}

#endif //_INCLUDED_MULTIPLAYER_MENU_