///////////////////////////////////////////////////////////////////////////////////////
///  \file UseCaseClans.hpp
///
///  \brief Defines the Clans class
///////////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDED_CLANS_
#define _INCLUDED_CLANS_

#include "UseCase.hpp"
#include "OSI/OSI.hpp"
#include "StateMachine.hpp"
#include "Singleton.hpp"

///////////////////////////////////////////////////////////////////////////////////////
/// The Clans class.  This class handles the presentation
/// of the multiplayer choices to the user, and starting the appropriate
/// game choice.
///////////////////////////////////////////////////////////////////////////////////////
namespace OnlineUI
{
    class Clans : public UseCase, public Singleton<Clans>
    {
	    public:

			// To prepare for displaying the main clan screen:
			// 1. Get clan ID list for player
			// 2. Get clan info for clan ID
			// 3. Get clan notices for clan ID

		    enum State
            {
				OSI_CREATE,				// this transitions to OSI_GET_MY_CLAN_INFO
				OSI_GET_MY_CLAN_INFO,	// this transitions to OSI_GET_NOTICES
				OSI_GET_NOTICES,		// this transitions to MENU_CLAN_MAIN
				OSI_GET_INVITATIONS,	// this transitions to MENU_CLAN_INVITATIONS
				OSI_GET_MEMBER_LIST,	// get the current players clan membership list from OSI
				OSI_INVITATION_SEND,	// send an invitation to the player specified
				OSI_INVITATION_ACCEPT,	// accept the highlighted invitiation
				OSI_INVITATION_DECLINE,	// decline the highlighted invitation
				OSI_INVITATION_BLOCK,	// block the highlighted invitation sender from sending me invitations
				OSI_INVITATION_CANCEL_SENT, // cancel a sent invitation
                OSI_LEAVE,				// this actually performs the leaving of the clan
				OSI_POST_NOTICE,		// post a notice to players clan
				OSI_KICK_MEMBER,		// kick a clan member
				OSI_PROMOTE_MEMBER,		// promote a clan member
				OSI_DEMOTE_MEMBER,		// demote a clan member
				OSI_TRANSFER_LEADERSHIP,// transfer clan leadership to an officer.
				OSI_SEND_FRIEND_INVITE, // send a friend invite to the selected clan member

				#ifdef _XBOX
				OSI_GET_MEMBER_LIST_FOR_SENT_INVITATIONS,
				OSI_GET_MEMBER_FRIENDS,	// update the clan member list's isFriend flag for each member
				#endif // _XBOX
                
				MENU_CLAN_MAIN,
				MENU_CLAN_CREATE_START,	// this starts the actual screen
				MENU_CLAN_CREATE,		// this handles the screen states
				MENU_CLAN_MEMBERS_START,
                MENU_CLAN_MEMBERS,
				MENU_CLAN_INVITATIONS_START,
                MENU_CLAN_INVITATIONS,
				MENU_CLAN_NOTICES,				// view all clan notices
				MENU_CLAN_MEMBER_MANAGEMENT,	// member management options when a user is selected in the clan member list
				MENU_CLAN_INVITATION_POPUP,		// invitation popup so user can choose what to do with a clan invitation
				MENU_CLAN_ASK,					// ask's the player for yes/no input

				OSI_DIALOG_ERROR,

                NUM_STATES
            };
            
            /** \name UseCase public methods */
            //\{
            virtual bool				update() { return _stateMachine.update(this); }
            //\}

        protected:
	        friend Singleton<Clans>;
        
            /** \name Constructor */
            //\{
            Clans();
            //\}

            /** \name Destructor */
            //\{
            virtual ~Clans() { }
            //\}

            /** \name UseCase protected methods */
            //\{
            virtual bool				_init();
            virtual bool				_deinit();
            //\}

            /** \name State Machine methods */
            //\{
			void _enterMenuMain();
            bool _updateMenuMain();

            void _enterMenuMemberList();
			bool _updateMenuMemberList();
            
			void _enterMenuCreate();
			bool _updateMenuCreate();

			void _enterMenuInvitations();
            bool _updateMenuInvitations();

			void _enterMenuNotices();
			bool _updateMenuNotices();

			bool _updateMenuMemberManagement();
			bool _updateMenuInvitationPopup();
			
			void _enterMenuAsk();
			bool _updateMenuAsk();
			
			void _enterOSIClanPostNotice();
			bool _updateOSIClanPostNotice();
			
			void _enterOSIClanLeave();
            bool _updateOSIClanLeave();

			void _enterOSIClanInviteSend();
			bool _updateOSIClanInviteSend();

			void _enterOSIClanInviteAccept();
			bool _updateOSIClanInviteAccept();

			void _enterOSIClanInviteDecline();
			bool _updateOSIClanInviteDecline();

			void _enterOSIClanInviteBlock();
			bool _updateOSIClanInviteBlock();

			void _enterOSIClanInviteCancelSent();
			bool _updateOSIClanInviteCancelSent();

			void _enterOSIClanCreate();
			bool _updateOSIClanCreate();

			void _enterOSIClanGetMyInfo();
			bool _updateOSIClanGetMyInfo();
			
			void _enterOSIClanGetNoticeList();
			bool _updateOSIClanGetNoticeList();
			
			void _enterOSIClanGetInvitationList();
			bool _updateOSIClanGetInvitationList();

			void _enterOSIClanGetMemberList();
			bool _updateOSIClanGetMemberList();

			void _enterOSIClanPromoteMember();
			bool _updateOSIClanPromoteMember();

			void _enterOSIClanDemoteMember();
			bool _updateOSIClanDemoteMember();

			void _enterOSIClanKickMember();
			bool _updateOSIClanKickMember();

			void _enterOSIClanTransferLeadership();
			bool _updateOSIClanTransferLeadership();

			void _enterOSIClanSendFriendInvite();
			bool _updateOSIClanSendFriendInvite();

			#ifdef _XBOX
			void _enterOSIClanGetMemberListForSentInvitations();
			bool _updateOSIClanGetMemberListForSentInvitations();

			bool _updateOSIClanGetMemberFriends();
			#endif //_XBOX
            //\}

			void _refreshClanDataForMenuMain();
			void _refreshClanNoticesForMenuMain();

    		void _raiseError(const OSIchar* pTitle, const char* pText);
	    private:
            StateMachine<State, NUM_STATES, Clans> _stateMachine;  /// < State machine

			State _stateAfterClanInfoRefresh;

			// Data used by the use case that is on the screen or entered in some way
			const OSIchar* _newClanName;
			const OSIchar* _newClanTag;

			const OSIchar* _inviteeName;

			const OSIchar* _clanNotice;

			const OSIClanInvitation *_clanInvite;
			const OSIClanMembership *_clanMember;

			// Data for the ask state that displays a yes/no dialog with this data
			const OSIchar* _askTitle;
			const OSIchar* _askMessage;
			State _askYes, _askNo;

			char _clanBuffer[ OSI_STRING_LENGTH_LONG ];

			bool _cleanUpScreens ;
		private:
			OSIRequestStatus _checkOSIRequestStatus(State successState, State failureState);
    };
}

#endif //_INCLUDED_CLANS_