#include "TTLPCH.h"

#include "UseCaseClans.hpp"
#include "UseCaseDisplayDialog.hpp"
#include "UseCaseYesNoDialog.hpp"
#include "UseCaseJoinMatch.hpp"
#include "UseCaseUtility.hpp"
#include "frontend/frontend.h"
#include "../screens/screen_clan_main.h"
#include "../screens/screen_clan_list.h"
#include "../screens/screen_clan_invite.h"
#include "../screens/screen_clan_create.h"
#include "../screens/screen_clan_popup.h"
#include "../screens/screen_clan_invite_popup.h"
#include "../screens/screen_clan_notices.h"
#include "UseCaseUtility.hpp"
#include "data/screens/Multiplayer.h"
#include "UseCaseUtility.hpp"
#include "UseCaseWaitForOSIRequest.hpp"
#include "GameHelper/Translator.h"
#include "main/PlayerProfile.h"

#ifdef _XBOX
#include "osi/xbox/osixbox.hpp"
#include "Xbox/UseCaseClanFeedback.hpp"
#endif //_XBOX

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Miscellaneous data used by the use case
///////////////////////////////////////////////////////////////////////////////////////////////////
static const char *askClanDisplayTitles[] = {
	"Confirm Clan Leave",
	"Decline Clan Invite",
	"Block Clan Invite",
	"Sent Clan Invite Cancelation",
	"Kick Soldier",
	"Demote Officer",
	"Promote Soldier",
	"Transfer Leadership"
};

static const char *askClanDisplayMessages[] = {
	"Are you sure you want to leave your current clan?",
	"Confirm declining invitation:\n%s",
	"Are you sure you want to permanently block\nClan invitations from %s?",
	"Confirm cancelation of sent clan invitation:\n%s",
	// Kick
	"Confirm removal of soldier from the clan:\n%s",
	// Demote
	"Confirm demotion of officer to soldier status:\n%s\n"
		"(Soldiers can be kicked out of the clan by officers and are unable to send clan invitations or post notices)",
	// Promote
	"Confirm promotion of soldier to officer status:\n%s\n"
		"(Officers are able to kick soldiers out of the clan, send clan invitations, and post notices)",
	// Transfer Leadership
	"Confirm transfer of leadership to officer:\n%s\n"
		"(You will become an officer and no longer be able to promote/demote or kick out officers)"
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor: Initialize member variables
///////////////////////////////////////////////////////////////////////////////////////////////////
OnlineUI::Clans::Clans() : 
    _stateMachine( OSI_GET_MY_CLAN_INFO ),
	_cleanUpScreens( true )
{
	_stateMachine.setStateEnter(	MENU_CLAN_MAIN,			&OnlineUI::Clans::_enterMenuMain);
	_stateMachine.setStateUpdate(	MENU_CLAN_MAIN,			&OnlineUI::Clans::_updateMenuMain);

    _stateMachine.setStateEnter(	MENU_CLAN_MEMBERS_START,	&OnlineUI::Clans::_enterMenuMemberList);
    _stateMachine.setStateUpdate(	MENU_CLAN_MEMBERS,		&OnlineUI::Clans::_updateMenuMemberList);

    _stateMachine.setStateEnter(	MENU_CLAN_CREATE_START,	&OnlineUI::Clans::_enterMenuCreate);
    _stateMachine.setStateUpdate(	MENU_CLAN_CREATE,		&OnlineUI::Clans::_updateMenuCreate);

    _stateMachine.setStateEnter(	MENU_CLAN_INVITATIONS_START,	&OnlineUI::Clans::_enterMenuInvitations);
    _stateMachine.setStateUpdate(	MENU_CLAN_INVITATIONS,	&OnlineUI::Clans::_updateMenuInvitations);

	_stateMachine.setStateEnter(	MENU_CLAN_NOTICES,	&OnlineUI::Clans::_enterMenuNotices);
	_stateMachine.setStateUpdate(	MENU_CLAN_NOTICES,	&OnlineUI::Clans::_updateMenuNotices);

	_stateMachine.setStateUpdate(	MENU_CLAN_MEMBER_MANAGEMENT, &OnlineUI::Clans::_updateMenuMemberManagement);

	_stateMachine.setStateUpdate(	MENU_CLAN_INVITATION_POPUP, &OnlineUI::Clans::_updateMenuInvitationPopup);

	_stateMachine.setStateEnter(	MENU_CLAN_ASK,	&OnlineUI::Clans::_enterMenuAsk);
	_stateMachine.setStateUpdate(	MENU_CLAN_ASK,	&OnlineUI::Clans::_updateMenuAsk);
    
	_stateMachine.setStateEnter(	OSI_POST_NOTICE,		&OnlineUI::Clans::_enterOSIClanPostNotice);
	_stateMachine.setStateUpdate(	OSI_POST_NOTICE,		&OnlineUI::Clans::_updateOSIClanPostNotice);

	_stateMachine.setStateEnter(	OSI_LEAVE,				&OnlineUI::Clans::_enterOSIClanLeave);
    _stateMachine.setStateUpdate(	OSI_LEAVE,				&OnlineUI::Clans::_updateOSIClanLeave);
	
	_stateMachine.setStateEnter(	OSI_INVITATION_SEND,	&OnlineUI::Clans::_enterOSIClanInviteSend);
    _stateMachine.setStateUpdate(	OSI_INVITATION_SEND,	&OnlineUI::Clans::_updateOSIClanInviteSend);

	_stateMachine.setStateEnter(	OSI_INVITATION_ACCEPT,	&OnlineUI::Clans::_enterOSIClanInviteAccept);
    _stateMachine.setStateUpdate(	OSI_INVITATION_ACCEPT,	&OnlineUI::Clans::_updateOSIClanInviteAccept);

	_stateMachine.setStateEnter(	OSI_INVITATION_DECLINE,	&OnlineUI::Clans::_enterOSIClanInviteDecline);
    _stateMachine.setStateUpdate(	OSI_INVITATION_DECLINE,	&OnlineUI::Clans::_updateOSIClanInviteDecline);

	_stateMachine.setStateEnter(	OSI_INVITATION_BLOCK,	&OnlineUI::Clans::_enterOSIClanInviteBlock);
    _stateMachine.setStateUpdate(	OSI_INVITATION_BLOCK,	&OnlineUI::Clans::_updateOSIClanInviteBlock);

	_stateMachine.setStateEnter(	OSI_INVITATION_CANCEL_SENT,	&OnlineUI::Clans::_enterOSIClanInviteCancelSent);
    _stateMachine.setStateUpdate(	OSI_INVITATION_CANCEL_SENT,	&OnlineUI::Clans::_updateOSIClanInviteCancelSent);
	
	_stateMachine.setStateEnter(	OSI_CREATE,				&OnlineUI::Clans::_enterOSIClanCreate);
	_stateMachine.setStateUpdate(	OSI_CREATE,				&OnlineUI::Clans::_updateOSIClanCreate);
	
	_stateMachine.setStateEnter(	OSI_GET_MY_CLAN_INFO,	&OnlineUI::Clans::_enterOSIClanGetMyInfo );
	_stateMachine.setStateUpdate(	OSI_GET_MY_CLAN_INFO,	&OnlineUI::Clans::_updateOSIClanGetMyInfo );
	
	_stateMachine.setStateEnter(	OSI_GET_NOTICES,		&OnlineUI::Clans::_enterOSIClanGetNoticeList );
	_stateMachine.setStateUpdate(	OSI_GET_NOTICES,		&OnlineUI::Clans::_updateOSIClanGetNoticeList );
	
	_stateMachine.setStateEnter(	OSI_GET_INVITATIONS,	&OnlineUI::Clans::_enterOSIClanGetInvitationList );
	_stateMachine.setStateUpdate(	OSI_GET_INVITATIONS,	&OnlineUI::Clans::_updateOSIClanGetInvitationList );

	_stateMachine.setStateEnter(	OSI_GET_MEMBER_LIST,	&OnlineUI::Clans::_enterOSIClanGetMemberList );
	_stateMachine.setStateUpdate(	OSI_GET_MEMBER_LIST,	&OnlineUI::Clans::_updateOSIClanGetMemberList );

	_stateMachine.setStateEnter(	OSI_DEMOTE_MEMBER,	&OnlineUI::Clans::_enterOSIClanDemoteMember );
	_stateMachine.setStateUpdate(	OSI_DEMOTE_MEMBER,	&OnlineUI::Clans::_updateOSIClanDemoteMember );

	_stateMachine.setStateEnter(	OSI_PROMOTE_MEMBER,	&OnlineUI::Clans::_enterOSIClanPromoteMember );
	_stateMachine.setStateUpdate(	OSI_PROMOTE_MEMBER,	&OnlineUI::Clans::_updateOSIClanPromoteMember );

	_stateMachine.setStateEnter(	OSI_KICK_MEMBER,	&OnlineUI::Clans::_enterOSIClanKickMember );
	_stateMachine.setStateUpdate(	OSI_KICK_MEMBER,	&OnlineUI::Clans::_updateOSIClanKickMember );

	_stateMachine.setStateEnter(	OSI_TRANSFER_LEADERSHIP,	&OnlineUI::Clans::_enterOSIClanTransferLeadership );
	_stateMachine.setStateUpdate(	OSI_TRANSFER_LEADERSHIP,	&OnlineUI::Clans::_updateOSIClanTransferLeadership );

	_stateMachine.setStateEnter(	OSI_SEND_FRIEND_INVITE,	&OnlineUI::Clans::_enterOSIClanSendFriendInvite );
	_stateMachine.setStateUpdate(	OSI_SEND_FRIEND_INVITE,	&OnlineUI::Clans::_updateOSIClanSendFriendInvite );

	#ifdef _XBOX
	_stateMachine.setStateUpdate(	OSI_GET_MEMBER_FRIENDS,	&OnlineUI::Clans::_updateOSIClanGetMemberFriends );

	_stateMachine.setStateEnter(	OSI_GET_MEMBER_LIST_FOR_SENT_INVITATIONS, &OnlineUI::Clans::_enterOSIClanGetMemberListForSentInvitations );
	_stateMachine.setStateUpdate(	OSI_GET_MEMBER_LIST_FOR_SENT_INVITATIONS, &OnlineUI::Clans::_updateOSIClanGetMemberListForSentInvitations );
	#endif //_XBOX

	_clanBuffer[ NUM_ARRAY_ELEMENTS( _clanBuffer ) - 1 ] = 0; // set last element of buffer to zero
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Wrapper function for raising errors, currently only calls a display dialog
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Clans::_raiseError(const OSIchar* pTitle, const char* pText)
{
	DisplayDialog::getSingletonPtr()->start(pTitle, pText);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called before use case is started.  Show the first screen for the use case.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Clans::_init( void )
{
	g_FrontEnd.GotoScreen("SCREEN_ID_CLAN_MAIN_5_60");
	_stateAfterClanInfoRefresh = MENU_CLAN_MAIN;
	_stateMachine.setCurrentState( this, OSI_GET_MY_CLAN_INFO );
	_cleanUpScreens = true ;

	ScreenClanMain* pScreen = reinterpret_cast<ScreenClanMain*>(g_FrontEnd.GetScreenHandler( "SCREEN_ID_CLAN_MAIN_5_60" ));
	if( pScreen )
	{
		pScreen->ClearScreen();
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called after use case is ended.  Clean up the screens and other objects.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Clans::_deinit( void )
{
	#ifdef _XBOX
	// stop the background match searching on the xbox
	OSIXbox* osix = static_cast< OSIXbox* >( OSI::getSingletonPtr() );
	osix->endMatchSearchesForClanMembershipList();
	#endif  // _XBOX

	//if ( _cleanUpScreens )
	{
		g_FrontEnd.RemoveAllPopups();

		// End potential overlay screens
		bool screenInviteActive = UseCase::IsScreenActive< ScreenClanInvite >() ;
		bool screenListActive = UseCase::IsScreenActive< ScreenClanList >() ;
		bool screenCreateActive = UseCase::IsScreenActive< ScreenClanCreate >() ;
		bool screenNoticesActive = UseCase::IsScreenActive< ScreenClanNotices >() ;
		if ( screenInviteActive || screenListActive || screenCreateActive || screenNoticesActive )
		{
			g_FrontEnd.EndScreen(0);
		}

		// Pop them just to be sure
		g_FrontEnd.PopScreenHistory( "SCREEN_ID_CLAN_INVITATIONS_5_62" );
		g_FrontEnd.PopScreenHistory( "SCREEN_ID_CLAN_LIST_5_61" );
		g_FrontEnd.PopScreenHistory( "SCREEN_ID_CLAN_CREATE_5_63" );
		g_FrontEnd.PopScreenHistory( "SCREEN_ID_CLAN_NOTICES" );

		// End the Main Screen
		if( UseCase::IsScreenActive< ScreenClanMain >() )
		{
			g_FrontEnd.EndScreen(0);
		}
		g_FrontEnd.PopScreenHistory( "SCREEN_ID_CLAN_MAIN_5_60" );
	}

	OSI::getSingletonPtr()->getClanMembershipList().clear();
	OSI::getSingletonPtr()->getClanInvitationList().clear();
	OSI::getSingletonPtr()->getClanSentInvitationList().clear();
	OSI::getSingletonPtr()->getClanNoticeList().clear();
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the MENU_MAIN state is entered.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Clans::_enterMenuMain()
{
	// Check on whether the player is in a clan or not and disable buttons accordingly.
	ScreenClanMain* pScreen = reinterpret_cast<ScreenClanMain*>(g_FrontEnd.GetScreenHandler( "SCREEN_ID_CLAN_MAIN_5_60" ));
	if ( !pScreen )
	{
		stop();
		return;
	}
	// set access level based on clan role of user
	pScreen->SetAccessLevel(OSI::getSingletonPtr()->getActivePlayer()->clanRole);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the MENU_CLAN_MAIN state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Clans::_updateMenuMain()
{
    ScreenClanMain *pScreen = UseCase::GetActiveScreen<ScreenClanMain>();
	if ( !pScreen ) return true ;

    switch (pScreen->GetScreenStatus())
    {
#ifdef _XBOX	
		case ScreenClanMain::FEEDBACK:
			XboxClanFeedback::getSingletonPtr()->start();
			break;
#endif

        case ScreenClanMain::MEMBERS:
			_stateAfterClanInfoRefresh = MENU_CLAN_MEMBERS_START;
            _stateMachine.setCurrentState(this, OSI_GET_MY_CLAN_INFO);
            break;

        case ScreenClanMain::CREATE:
            _stateMachine.setCurrentState(this, MENU_CLAN_CREATE_START);
            break;

        case ScreenClanMain::INVITATIONS:
			_stateAfterClanInfoRefresh = MENU_CLAN_INVITATIONS_START;
            _stateMachine.setCurrentState(this, OSI_GET_MY_CLAN_INFO);
            break;

        case ScreenClanMain::LEAVE:
			_askTitle = askClanDisplayTitles[0];
			_askMessage = askClanDisplayMessages[0];
			_askYes = OSI_LEAVE;
			_askNo = MENU_CLAN_MAIN;
            _stateMachine.setCurrentState(this, MENU_CLAN_ASK);
            break;

		case ScreenClanMain::VIEW_NOTICES:
			_stateAfterClanInfoRefresh = MENU_CLAN_NOTICES;
			_stateMachine.setCurrentState(this, OSI_GET_MY_CLAN_INFO);
            break;

        case ScreenClanMain::POST_NOTICE:
#ifdef _XBOX
			if( !XOnlineIsUserVoiceAllowed( OSI::getSingletonPtr()->getActivePlayer()->id.xBox.userFlags ) )
			{
   				ScreenPopupDialog* pDialog = pScreen->GetPopupDialog();
   				if( pDialog )
   				{
   					pDialog->Clear();
   					pDialog->SetMessage( "An Xbox Live account limitation does not allow you to use this feature." );
					pDialog->AddMenuItem( "OK", ScreenClanMain::COMMAND_DIALOG_OK, 0 );
   					pScreen->ShowPopupDialog();
   				}
				return true;
			}
#endif
			UseCase::ActivateKeyboard("", 80 ); // Multibyte, so 80 * 3 is 240, which is < 255 our table limit.
            break;

		case ScreenClanMain::NOTICE_POSTED:
			_clanNotice = pScreen->getEnteredClanNotice();
			_stateMachine.setCurrentState(this, OSI_POST_NOTICE);
			break;

        case ScreenClanMain::BACK:
            stop();
            break;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the MENU_CLAN_MEMBERS_START state is entered.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Clans::_enterMenuMemberList()
{
	g_FrontEnd.GotoScreen("SCREEN_ID_CLAN_LIST_5_61");
	_stateMachine.setCurrentState(this, OSI_GET_MEMBER_LIST);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the MENU_CLAN_MEMBERS state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Clans::_updateMenuMemberList()
{
    ScreenClanList *pScreen = UseCase::GetActiveScreen<ScreenClanList>();
	if ( !pScreen ) return true ;

	#ifdef _XBOX
	// Continue pumping the background match searching on the xbox.
	// The members in the clan member list will have their match info magically updated
	// by this function, and it's up to the UI to display the new information.
	OSIXbox* osix = static_cast< OSIXbox* >( OSI::getSingletonPtr() );
	const int memberUpdateCnt = osix->updateMatchSearchesForClanMembershipList();
	if( memberUpdateCnt > 0 )
	{
		pScreen->OnScreenRefresh();
	}
	#endif //_XBOX

    switch (pScreen->GetScreenStatus())
    {
        case ScreenClanList::MEMBER_SELECTED:
		{
			// Save off our currently selected member
			_clanMember = pScreen->GetSelectedMember();
			if( _clanMember == NULL )
				break;
			// Don't allow player to select himself
			if( _clanMember->playerID == OSI::getSingletonPtr()->getActivePlayer()->id )
				break;

			const OSIClanRole& role = OSI::getSingletonPtr()->getActivePlayer()->clanRole;

			// Bring up the member management popup for the clan member
            g_FrontEnd.ShowPopup("SCREEN_ID_CLAN_LIST_POPUP");
			ScreenClanListPopup *pPopup = UseCase::GetActiveScreen<ScreenClanListPopup>();
			if ( !pPopup ) return true ;

			pPopup->SetAccessLevel( role, *_clanMember );
			_stateMachine.setCurrentState(this, MENU_CLAN_MEMBER_MANAGEMENT);
            break;
		}

        case ScreenClanList::BACK:
			// Go back to the main menu
            g_FrontEnd.EndScreen(0); // get rid of member list screen
			// Do we need to refresh our clan data?
			_stateAfterClanInfoRefresh = MENU_CLAN_MAIN;
			_stateMachine.setCurrentState( this, OSI_GET_MY_CLAN_INFO );
            break;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the MENU_CLAN_CREATE_START state is entered.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Clans::_enterMenuCreate()
{
	g_FrontEnd.GotoScreen("SCREEN_ID_CLAN_CREATE_5_63");
	_stateMachine.setCurrentState(this, MENU_CLAN_CREATE);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the MENU_CLAN_CREATE state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Clans::_updateMenuCreate()
{
	// Make sure keyboard is not up
    if (UseCase::IsScreenActive<ScreenKeyboard>()) return true;

    ScreenClanCreate *pScreen = UseCase::GetActiveScreen<ScreenClanCreate>();
	if ( !pScreen ) return true ;

	char buffer[OSI_STRING_LENGTH_LONG];

    switch (pScreen->GetScreenStatus())
    {
        case ScreenClanCreate::ENTER_NAME:
			UseCase::ActivateKeyboardForClanNames("", OnlineUI::MAX_ENTRY_LENGTH);
            break;

        case ScreenClanCreate::ENTER_TAG:
			UseCase::ActivateKeyboardForNames("", OSIClanInfo::MAX_TAG_LENGTH-1);
            break;

		case ScreenClanCreate::INVALID_NAME_LENGTH:
			OSIsprintf(buffer, g_translator.Translate( "Clan name must be between %u and %u characters." ), OnlineUI::MIN_ENTRY_LENGTH, OnlineUI::MAX_ENTRY_LENGTH);
			_raiseError("Clan name invalid.", buffer);
			break;

		case ScreenClanCreate::INVALID_TAG_LENGTH:
			OSIsprintf(buffer, g_translator.Translate( "Clan abbreviation must be between %u and %u characters." ), 1, OSIClanInfo::MAX_TAG_LENGTH-1);
			_raiseError("Clan abbreviation invalid.", buffer);
			break;

        case ScreenClanCreate::DONE:
			_newClanName = pScreen->getEnteredName();
			if(NULL == _newClanName || "" == _newClanName) {
				// no name entered for the new clan
				_raiseError("No name specified", "You must specify a name before creating a clan.");
				break;
			}
			_newClanTag = pScreen->getEnteredTag();
			if(NULL == _newClanTag || "" == _newClanTag) {
				// no tag entered for the new clan
				_raiseError("No abbreviation specified", "You must specify an abbreviation before creating a clan.");
				break;
			}
			// verify that user is not in clan just prior to creating
			_stateAfterClanInfoRefresh = OSI_CREATE;
			_stateMachine.setCurrentState( this, OSI_GET_MY_CLAN_INFO );
            break;

        case ScreenClanCreate::BACK:
            // Go back to the main menu
			g_FrontEnd.EndScreen(0); // get rid of create screen
			_stateAfterClanInfoRefresh = MENU_CLAN_MAIN;
			// Do we need to refresh our clan data?
			_stateMachine.setCurrentState( this, OSI_GET_MY_CLAN_INFO );
            break;
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the MENU_CLAN_INVITATIONS_START state is entered.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Clans::_enterMenuInvitations()
{
	g_FrontEnd.GotoScreen("SCREEN_ID_CLAN_INVITATIONS_5_62");
	ScreenClanInvite *pScreen = UseCase::GetActiveScreen<ScreenClanInvite>();
	if ( !pScreen ) return ;

	pScreen->SetAccessLevel(OSI::getSingletonPtr()->getActivePlayer()->clanRole);
	pScreen->SetScreenRole( ScreenClanInvite::INBOX_MODE );

	_stateMachine.setCurrentState(this, OSI_GET_INVITATIONS);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the MENU_CLAN_INVITATIONS state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Clans::_updateMenuInvitations()
{
	// Make sure keyboard is not up
    if (UseCase::IsScreenActive<ScreenKeyboard>()) return true;

    ScreenClanInvite *pScreen = UseCase::GetActiveScreen<ScreenClanInvite>();
	if ( !pScreen ) return true ;

    switch (pScreen->GetScreenStatus())
    {
        case ScreenClanInvite::INVITE_SELECTED:
		{
            // Get the invite selected
			_clanInvite = pScreen->GetSelectedInvitation();
			if( pScreen->GetScreenRole() == ScreenClanInvite::INBOX_MODE )
			{
				g_FrontEnd.ShowPopup("SCREEN_ID_CLAN_INVITE_POPUP");
				ScreenClanInvitePopup *pPopup = UseCase::GetActiveScreen<ScreenClanInvitePopup>();
				if ( !pPopup ) return true ;

				pPopup->SetAccessLevel( OSI::getSingletonPtr()->getActivePlayer()->clanRole );
				_stateMachine.setCurrentState(this, MENU_CLAN_INVITATION_POPUP);
			}
			else
			{
				#ifndef _XBOX
				_askTitle = askClanDisplayTitles[3];
				OSIsnprintf( _clanBuffer, NUM_ARRAY_ELEMENTS( _clanBuffer )-1, g_translator.Translate( askClanDisplayMessages[3] ), _clanInvite->receiverName );
				_askMessage = _clanBuffer;
				_askYes = OSI_INVITATION_CANCEL_SENT;
				_askNo = MENU_CLAN_INVITATIONS;
				_stateMachine.setCurrentState(this, MENU_CLAN_ASK);
				#endif // _XBOX
			}
		}
            break;

        case ScreenClanInvite::INVITE_ENTER_INVITEE_NAME:
			UseCase::ActivateKeyboardForNames("", OnlineUI::MAX_ENTRY_LENGTH);
            break;

		case ScreenClanInvite::INVITE_INBOX:
			pScreen->SetScreenRole( ScreenClanInvite::INBOX_MODE );
			_stateMachine.setCurrentState( this, OSI_GET_INVITATIONS );
            break;

		case ScreenClanInvite::INVITE_SENT_INVITE_LIST:
			pScreen->SetScreenRole( ScreenClanInvite::SENT_INVITE_LIST_MODE );
			#ifdef _XBOX
			_stateMachine.setCurrentState( this, OSI_GET_MEMBER_LIST_FOR_SENT_INVITATIONS );
			#else
			_stateMachine.setCurrentState( this, OSI_GET_INVITATIONS );
			#endif// ! _XBOX
            break;

		case ScreenClanInvite::INVITE_SEND:
			_inviteeName = pScreen->GetEnteredName();
			ASSERT( _inviteeName != NULL );
			_stateMachine.setCurrentState(this, OSI_INVITATION_SEND);
            break;

        case ScreenClanInvite::BACK:
            // Go back to the main menu
            g_FrontEnd.EndScreen(0); // get rid of invitations screen
			_stateAfterClanInfoRefresh = MENU_CLAN_MAIN;
			// Do we need to refresh our clan data?
			_stateMachine.setCurrentState( this, OSI_GET_MY_CLAN_INFO );
            break;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the MENU_CLAN_NOTICES state is entered.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Clans::_enterMenuNotices()
{
	g_FrontEnd.GotoScreen("SCREEN_ID_CLAN_NOTICES");
	ScreenClanNotices *pScreen = UseCase::GetActiveScreen<ScreenClanNotices>();
	if ( !pScreen ) return ;

	pScreen->SetNotices( &OSI::getSingletonPtr()->getClanNoticeList() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the MENU_CLAN_NOTICES state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Clans::_updateMenuNotices()
{
    ScreenClanNotices *pScreen = UseCase::GetActiveScreen<ScreenClanNotices>();
	if ( !pScreen ) return true ;

    switch (pScreen->GetScreenStatus())
    {
        case ScreenClanNotices::BACK:
            // Go back to the main menu
			_stateAfterClanInfoRefresh = MENU_CLAN_MAIN;
            g_FrontEnd.EndScreen(0); // notices screen
			_stateMachine.setCurrentState( this, OSI_GET_MY_CLAN_INFO );
            break;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the MENU_CLAN_MEMBER_MANAGEMENT state.
///
/// \return true to continue use case processing
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Clans::_updateMenuMemberManagement()
{
	ScreenClanListPopup *pPopup = expectPopup<ScreenClanListPopup>();
	if ( !pPopup )
	{
		return true;
	}

	const ScreenClanListPopup::ScreenStatus status = pPopup->GetScreenStatus();
	switch( status )
	{
		case ScreenClanListPopup::JOIN:	
			if( _clanMember->currentMatchList && _clanMember->currentMatchList->count() > 0 )
			{
				_cleanUpScreens	= false ;
				_clanMember->currentMatchList->begin();
				JoinMatch::getSingletonPtr()->setMatchToJoin( _clanMember->currentMatchList->getCurrent()->id );
				stop(); // stop after using _clanMember variable to avoid crash.
				JoinMatch::getSingletonPtr()->start();
			}
			break;
		case ScreenClanListPopup::KICK:
			g_FrontEnd.EndPopup(1); // close the popup window
			_askTitle = askClanDisplayTitles[4];
			OSIsnprintf( _clanBuffer, NUM_ARRAY_ELEMENTS( _clanBuffer )-1, g_translator.Translate( askClanDisplayMessages[4] ), _clanMember->memberName );
			_askMessage = _clanBuffer;
			_askYes = OSI_KICK_MEMBER;
			_askNo = MENU_CLAN_MEMBERS;
			_stateMachine.setCurrentState(this, MENU_CLAN_ASK);
			break;
		case ScreenClanListPopup::PROMOTE:
			g_FrontEnd.EndPopup(1); // close the popup window
			_askTitle = askClanDisplayTitles[6];
			OSIsnprintf( _clanBuffer, NUM_ARRAY_ELEMENTS( _clanBuffer )-1, g_translator.Translate( askClanDisplayMessages[6] ), _clanMember->memberName );
			_askMessage = _clanBuffer;
			_askYes = OSI_PROMOTE_MEMBER;
			_askNo = MENU_CLAN_MEMBERS;
			_stateMachine.setCurrentState(this, MENU_CLAN_ASK);
			break;
		case ScreenClanListPopup::DEMOTE:
			g_FrontEnd.EndPopup(1); // close the popup window
			_askTitle = askClanDisplayTitles[5];
			OSIsnprintf( _clanBuffer, NUM_ARRAY_ELEMENTS( _clanBuffer )-1, g_translator.Translate( askClanDisplayMessages[5] ), _clanMember->memberName );
			_askMessage = _clanBuffer;
			_askYes = OSI_DEMOTE_MEMBER;
			_askNo = MENU_CLAN_MEMBERS;
			_stateMachine.setCurrentState(this, MENU_CLAN_ASK);
			break;
		case ScreenClanListPopup::TRANSFER_LEADERSHIP:
			g_FrontEnd.EndPopup(1); // close the popup window
			_askTitle = askClanDisplayTitles[7];
			OSIsnprintf( _clanBuffer, NUM_ARRAY_ELEMENTS( _clanBuffer )-1, g_translator.Translate( askClanDisplayMessages[7] ), _clanMember->memberName );
			_askMessage = _clanBuffer;
			_askYes = OSI_TRANSFER_LEADERSHIP;
			_askNo = MENU_CLAN_MEMBERS;
			_stateMachine.setCurrentState(this, MENU_CLAN_ASK);
			break;
		case ScreenClanListPopup::SEND_FRIEND_INVITE:
			g_FrontEnd.EndPopup(1); // close the popup window
			_stateMachine.setCurrentState(this, OSI_SEND_FRIEND_INVITE);
			break;
		case ScreenClanListPopup::BACK:
			g_FrontEnd.EndPopup(1); // close the popup window
			// go back to member list
			_stateMachine.setCurrentState(this, MENU_CLAN_MEMBERS);
			break;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the MENU_CLAN_INVITATION_POPUP state.
///
/// \return true to continue use case processing
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Clans::_updateMenuInvitationPopup()
{
	ScreenClanInvitePopup *pPopup = expectPopup<ScreenClanInvitePopup>();
	if ( !pPopup )
	{
		return true ;
	}

	OSIchar nameBuffer[ OSIClanInfo::MAX_FULL_NAME_LENGTH ];

	const ScreenClanInvitePopup::ScreenStatus status = pPopup->GetScreenStatus();
	switch( status )
	{
		case ScreenClanInvitePopup::ACCEPT:
			g_FrontEnd.EndPopup(1); // close the popup window
			_stateMachine.setCurrentState(this, OSI_INVITATION_ACCEPT);
			break;
		case ScreenClanInvitePopup::DECLINE:
			g_FrontEnd.EndPopup(1); // close the popup window
			_askTitle = askClanDisplayTitles[1];
			OSIsnprintf( _clanBuffer, NUM_ARRAY_ELEMENTS( _clanBuffer )-1, g_translator.Translate( askClanDisplayMessages[1] ),
				GetFullPlayerName( nameBuffer, _clanInvite->clanInfo.tag, _clanInvite->senderName ) );
			_askMessage = _clanBuffer;
			_askYes = OSI_INVITATION_DECLINE;
			_askNo = MENU_CLAN_INVITATIONS;
			_stateMachine.setCurrentState(this, MENU_CLAN_ASK);
			break;
		case ScreenClanInvitePopup::BLOCK:
			g_FrontEnd.EndPopup(1); // close the popup window
			_askTitle = askClanDisplayTitles[2];
			GetFullPlayerName( nameBuffer, _clanInvite->clanInfo.tag, _clanInvite->senderName );
			OSIsnprintf( _clanBuffer, NUM_ARRAY_ELEMENTS( _clanBuffer )-1, g_translator.Translate( askClanDisplayMessages[2] ), nameBuffer, nameBuffer );
			_askMessage = _clanBuffer;
			_askYes = OSI_INVITATION_BLOCK;
			_askNo = MENU_CLAN_INVITATIONS;
			_stateMachine.setCurrentState(this, MENU_CLAN_ASK);
			break;
		case ScreenClanInvitePopup::BACK:
			g_FrontEnd.EndPopup(1); // close the popup window
			// go back to invitation list
			_stateMachine.setCurrentState(this, MENU_CLAN_INVITATIONS);
			break;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the MENU_CLAN_ASK state is entered..
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Clans::_enterMenuAsk()
{
	YesNoDialog::getSingletonPtr()->start(_askTitle, _askMessage, false);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the MENU_CLAN_ASK state.
///
/// \return true to continue use case processing
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Clans::_updateMenuAsk()
{
	switch(YesNoDialog::getSingletonPtr()->GetAnswer()) 
	{
		case YesNoDialog::YES:
			_stateMachine.setCurrentState( this, _askYes );
			break;
		case YesNoDialog::NO:
		case YesNoDialog::CANCEL:
		default:
			_stateMachine.setCurrentState( this, _askNo );
			break;
	}
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the OSI_POST_NOTICE state is entered..
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Clans::_enterOSIClanPostNotice()
{
	OSIPlayer& player = OSI::getSingletonPtr()->getActiveAccount()->player;
	ASSERT( OSI_CLAN_ROLE_UNKNOWN != player.clanRole ); // can only be in this state if we are in a clan

	const bool requestOK = OSI::getSingletonPtr()->requestClanPostNotice( player.clanInfo.clanID, _clanNotice );

	WaitForOSIRequest::getSingletonPtr()->start( requestOK, "Posting Notice", "Please wait, posting clan notice..." );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the OSI_POST_NOTICE state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Clans::_updateOSIClanPostNotice()
{
	_checkOSIRequestStatus(OSI_GET_NOTICES, MENU_CLAN_MAIN);
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the OSI_INVITATION_SEND state is entered..
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Clans::_enterOSIClanInviteSend()
{
	OSIPlayer& player = OSI::getSingletonPtr()->getActiveAccount()->player;
	ASSERT( OSI_CLAN_ROLE_UNKNOWN != player.clanRole ); // can only be in this state if we are in a clan
	
	const bool requestOK = OSI::getSingletonPtr()->requestClanSendInvitation( player.clanInfo.clanID, _inviteeName );
	
	WaitForOSIRequest::getSingletonPtr()->start( requestOK, "Sending Invitation", "Please wait, sending invitation to player..." );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the OSI_INVITATION_SEND state.
///
/// \return true to continue use case processing
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Clans::_updateOSIClanInviteSend()
{
	if( _checkOSIRequestStatus(OSI_GET_INVITATIONS, MENU_CLAN_INVITATIONS) == STATUS_SUCCEEDED )
	{
		OSIsnprintf( _clanBuffer, NUM_ARRAY_ELEMENTS( _clanBuffer )-1, g_translator.Translate( "You have successfully invited %s to join your clan." ), _inviteeName );
		DisplayDialog::getSingletonPtr()->start( "Invite Sent", _clanBuffer );
	}
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the OSI_INVITATION_ACCEPT state is entered..
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Clans::_enterOSIClanInviteAccept()
{
	const bool requestOK = OSI::getSingletonPtr()->requestClanJoinByInvitation( *_clanInvite );
	
	WaitForOSIRequest::getSingletonPtr()->start( requestOK, "Accepting Invitation", "Please wait, accepting clan invitation..." );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the OSI_INVITATION_ACCEPT state.
///
/// \return true to continue use case processing
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Clans::_updateOSIClanInviteAccept()
{
	if( _checkOSIRequestStatus(OSI_GET_INVITATIONS, MENU_CLAN_INVITATIONS) == STATUS_SUCCEEDED )
	{
		OSIchar nameBuffer[ OSIClanInfo::MAX_FULL_NAME_LENGTH ];
		OSIsnprintf( _clanBuffer, NUM_ARRAY_ELEMENTS( _clanBuffer )-1, g_translator.Translate( "You have successfully joined %s." ),
			GetFullPlayerName( nameBuffer, _clanInvite->clanInfo.tag, _clanInvite->clanInfo.name ) );
		DisplayDialog::getSingletonPtr()->start( "Clan Welcome", _clanBuffer );
	}
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the OSI_INVITATION_DECLINE state is entered..
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Clans::_enterOSIClanInviteDecline()
{
	const bool requestOK = OSI::getSingletonPtr()->requestClanDenyInvitation( *_clanInvite );
	
	WaitForOSIRequest::getSingletonPtr()->start( requestOK, "Denying Invitation", "Please wait, denying clan invitation..." );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the OSI_INVITATION_DECLINE state.
///
/// \return true to continue use case processing
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Clans::_updateOSIClanInviteDecline()
{
	_checkOSIRequestStatus(OSI_GET_INVITATIONS, MENU_CLAN_INVITATIONS);
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the OSI_INVITATION_BLOCK state is entered..
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Clans::_enterOSIClanInviteBlock()
{
	const bool requestOK = OSI::getSingletonPtr()->requestClanBlockInvitation( *_clanInvite );
	
	WaitForOSIRequest::getSingletonPtr()->start( requestOK, "Blocking Invitation", "Please wait, blocking sender of clan invitation..." );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the OSI_INVITATION_BLOCK state.
///
/// \return true to continue use case processing
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Clans::_updateOSIClanInviteBlock()
{
	if( _checkOSIRequestStatus(OSI_GET_INVITATIONS, MENU_CLAN_INVITATIONS) == STATUS_SUCCEEDED )
	{
		OSIchar nameBuffer[ OSIClanInfo::MAX_FULL_NAME_LENGTH ];
		OSIsnprintf( _clanBuffer, NUM_ARRAY_ELEMENTS( _clanBuffer )-1, g_translator.Translate( "Player %s successfully blocked." ),
			GetFullPlayerName( nameBuffer, _clanInvite->clanInfo.tag, _clanInvite->senderName ) );
		DisplayDialog::getSingletonPtr()->start( askClanDisplayTitles[2], _clanBuffer );
	}
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the OSI_INVITATION_CANCEL_SENT state is entered..
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Clans::_enterOSIClanInviteCancelSent()
{
#ifdef _XBOX
	const bool requestOK = OSI::getSingletonPtr()->requestClanKickMember( _clanInvite->clanInfo.clanID, _clanInvite->receiverID );
#else
	const bool requestOK = OSI::getSingletonPtr()->requestClanCancelSentInvitation( *_clanInvite );
#endif // _XBOX
	
	WaitForOSIRequest::getSingletonPtr()->start( requestOK, "Canceling Invitation", "Please wait, canceling sent clan invitation..." );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the OSI_INVITATION_CANCEL_SENT state.
///
/// \return true to continue use case processing
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Clans::_updateOSIClanInviteCancelSent()
{
	#ifdef _XBOX
	_checkOSIRequestStatus(OSI_GET_MEMBER_LIST_FOR_SENT_INVITATIONS, MENU_CLAN_INVITATIONS);
	#else
	_checkOSIRequestStatus(OSI_GET_INVITATIONS, MENU_CLAN_INVITATIONS);
	#endif// ! _XBOX

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the OSI_LEAVE state is entered..
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Clans::_enterOSIClanLeave()
{
	OSIPlayer& player = OSI::getSingletonPtr()->getActiveAccount()->player;
	ASSERT( OSI_CLAN_ROLE_UNKNOWN != player.clanRole ); // can only be in this state if we are in a clan

	const bool requestOK = OSI::getSingletonPtr()->requestClanLeave( player.clanInfo.clanID );
	
	WaitForOSIRequest::getSingletonPtr()->start( requestOK, "One Moment", "Leaving current clan..." );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the OSI_LEAVE state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Clans::_updateOSIClanLeave()
{
	_stateAfterClanInfoRefresh = MENU_CLAN_MAIN;
	_checkOSIRequestStatus(OSI_GET_MY_CLAN_INFO, OSI_GET_MY_CLAN_INFO);
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the OSI_GET_MY_CLAN_INFO state is entered.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Clans::_enterOSIClanGetMyInfo()
{
	const bool requestOK = OSI::getSingletonPtr()->requestClanUpdateMyClanInformation();
	
	WaitForOSIRequest::getSingletonPtr()->start( requestOK, "One Moment", "Retrieving Clan Information" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the OSI_GET_MY_CLAN_INFO state.
/// Uses whatever happnes to be in OSI's clan ID list at [0].
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Clans::_updateOSIClanGetMyInfo()
{
	if( _checkOSIRequestStatus(OSI_GET_NOTICES, MENU_CLAN_MAIN) == STATUS_SUCCEEDED )
	{
		char fullname[OSI_PLAYER_FULL_NAME_LENGTH];
		OSI::getSingletonPtr()->getActivePlayer()->GetFullName( fullname );
		g_playerProfile.SetDisplayName( fullname );
	}

	ScreenClanMain* pScreen = reinterpret_cast<ScreenClanMain*>(g_FrontEnd.GetScreenHandler( "SCREEN_ID_CLAN_MAIN_5_60" ));
	if( pScreen )
	{
		pScreen->ClearScreen();
	}

	_refreshClanDataForMenuMain();

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the OSI_GET_NOTICES state is entered.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Clans::_enterOSIClanGetNoticeList()
{
	OSIPlayer& player = OSI::getSingletonPtr()->getActiveAccount()->player;
	if( OSI_CLAN_ROLE_UNKNOWN == player.clanRole )
	{
		_refreshClanDataForMenuMain();
		_stateMachine.setCurrentState( this, _stateAfterClanInfoRefresh );
		return;
	}

	const bool requestOK = OSI::getSingletonPtr()->requestClanNoticeList( player.clanInfo.clanID );
	
	WaitForOSIRequest::getSingletonPtr()->start( requestOK, "One Moment", "Retrieving Clan Notices" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the OSI_GET_NOTICES state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Clans::_updateOSIClanGetNoticeList()
{
	if( _checkOSIRequestStatus(_stateAfterClanInfoRefresh, _stateAfterClanInfoRefresh) == STATUS_SUCCEEDED )
	{
		_refreshClanNoticesForMenuMain();
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the OSI_CREATE state is entered.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Clans::_enterOSIClanCreate()
{
 	ASSERT( _newClanName );
	ASSERT( _newClanTag );

	if( OSI::getSingletonPtr()->getActivePlayer()->clanInfo.clanID.isValid() )
	{
        // Go back to the main menu
        g_FrontEnd.EndScreen(0); // end the create screen
		_stateMachine.setCurrentState( this, MENU_CLAN_MAIN );
		_refreshClanDataForMenuMain();
		return;
	}

	bool requestOK = OSI::getSingletonPtr()->requestClanCreate( _newClanTag, _newClanName );
	WaitForOSIRequest::getSingletonPtr()->start( requestOK, "One Moment", "Creating Clan" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the OSI_CREATE state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Clans::_updateOSIClanCreate()
{
	switch( _checkOSIRequestStatus(OSI_GET_MY_CLAN_INFO, MENU_CLAN_CREATE) )
	{
	case STATUS_SUCCEEDED:
		_stateAfterClanInfoRefresh = MENU_CLAN_MAIN;
		g_FrontEnd.EndScreen( 0 ); // get rid of create screen
		break;

	case STATUS_FAILED:
	{
		ScreenClanCreate *pScreen = UseCase::GetActiveScreen<ScreenClanCreate>();
		if ( !pScreen ) break;
		switch( OSI::getSingletonPtr()->getErrorCode() )
		{
		case OSIPS2_CLAN_CLAN_ABBR_TAKEN:
			pScreen->ClearTag();
			break;
		case OSIPS2_CLAN_CLAN_NAME_TAKEN:
			pScreen->ClearName();
			break;
		}
	}
		break;
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the OSI_GET_INVITATIONS state is entered.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Clans::_enterOSIClanGetInvitationList()
{
	const bool requestOK = OSI::getSingletonPtr()->requestClanInvitationList();
	
	WaitForOSIRequest::getSingletonPtr()->start( requestOK, "One Moment", "Retrieving Clan Invitations" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the OSI_GET_INVITATIONS state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Clans::_updateOSIClanGetInvitationList()
{
	_checkOSIRequestStatus(MENU_CLAN_INVITATIONS, MENU_CLAN_INVITATIONS);
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the OSI_GET_MEMBER_LIST state is entered.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Clans::_enterOSIClanGetMemberList()
{
	// make sure there is an entry in the ID list since that tells us which clan to get info for
	OSIPlayer& player = OSI::getSingletonPtr()->getActiveAccount()->player;
	ASSERT( OSI_CLAN_ROLE_UNKNOWN != player.clanRole ); // can only be in this state if we are in a clan

	const bool requestOK = OSI::getSingletonPtr()->requestClanMembershipList( player.clanInfo.clanID );

	WaitForOSIRequest::getSingletonPtr()->start( requestOK, "One Moment", "Retrieving Clan Member List" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the OSI_GET_INVITATIONS state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Clans::_updateOSIClanGetMemberList()
{
	_stateAfterClanInfoRefresh = MENU_CLAN_MAIN;
	if( STATUS_FAILED == _checkOSIRequestStatus(MENU_CLAN_MEMBERS, OSI_GET_MY_CLAN_INFO ) )
	{
		g_FrontEnd.EndScreen(0); // get rid of member list screen
		return true;
	}

	#ifdef _XBOX
	// On the xbox, clan member match information has to be retrieved asynch in the background
	// This is the spot where the clan member list is completely populated, so kick off the
	// match searching in the background.
	OSIXbox* osix = static_cast< OSIXbox* >( OSI::getSingletonPtr() );
	osix->endMatchSearchesForClanMembershipList();
	osix->beginMatchSearchesForClanMembershipList();

	// Also need to set the isfriend flag for people in the clanmember list.
	// This works a little differently than other OSI requests because the user doesn't need to know if
	// the friend list request failed or not - the game should continue as normal.
	const bool requestOK = osix->requestFriendList();
	if( requestOK )
	{
		WaitForOSIRequest::getSingletonPtr()->start( true, "One Moment", "Cross-referencing clan members and friends." );
		_stateMachine.setCurrentState( this, OSI_GET_MEMBER_FRIENDS );
	}

	#endif // _XBOX

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the OSI_KICK_MEMBER state is entered.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Clans::_enterOSIClanKickMember()
{
	OSIPlayer& player = OSI::getSingletonPtr()->getActiveAccount()->player;
	ASSERT( OSI_CLAN_ROLE_UNKNOWN != player.clanRole ); // can only be in this state if we are in a clan

	const bool requestOK = OSI::getSingletonPtr()->requestClanKickMember( player.clanInfo.clanID, _clanMember->playerID );
	
	WaitForOSIRequest::getSingletonPtr()->start( requestOK, "One Moment", "Removing selected member from clan." );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the OSI_KICK_MEMBER state.
///
/// \return true to continue use case processing
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Clans::_updateOSIClanKickMember()
{
	_checkOSIRequestStatus(OSI_GET_MEMBER_LIST, MENU_CLAN_MEMBERS);
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the OSI_PROMOTE_MEMBER state is entered.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Clans::_enterOSIClanPromoteMember()
{
	OSIPlayer& player = OSI::getSingletonPtr()->getActiveAccount()->player;
	ASSERT( OSI_CLAN_ROLE_UNKNOWN != player.clanRole ); // can only be in this state if we are in a clan

	const bool requestOK = OSI::getSingletonPtr()->requestClanPromoteMember( player.clanInfo.clanID, _clanMember->playerID );

	WaitForOSIRequest::getSingletonPtr()->start( requestOK, "One Moment", "Promoting selected clan member." );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the OSI_PROMOTE_MEMBER state.
///
/// \return true to continue use case processing
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Clans::_updateOSIClanPromoteMember()
{
	_checkOSIRequestStatus(OSI_GET_MEMBER_LIST, MENU_CLAN_MEMBERS);
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the OSI_DEMOTE_MEMBER state is entered.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Clans::_enterOSIClanDemoteMember()
{
	OSIPlayer& player = OSI::getSingletonPtr()->getActiveAccount()->player;
	ASSERT( OSI_CLAN_ROLE_UNKNOWN != player.clanRole ); // can only be in this state if we are in a clan

	const bool requestOK = OSI::getSingletonPtr()->requestClanDemoteMember( player.clanInfo.clanID, _clanMember->playerID );

	WaitForOSIRequest::getSingletonPtr()->start( requestOK, "One Moment", "Demoting selected clan member." );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the OSI_DEMOTE_MEMBER state.
///
/// \return true to continue use case processing
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Clans::_updateOSIClanDemoteMember()
{
	_checkOSIRequestStatus(OSI_GET_MEMBER_LIST, MENU_CLAN_MEMBERS);
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the OSI_TRANSFER_LEADERSHIP state is entered.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Clans::_enterOSIClanTransferLeadership()
{
	OSIPlayer& player = OSI::getSingletonPtr()->getActiveAccount()->player;
	ASSERT( OSI_CLAN_ROLE_UNKNOWN != player.clanRole ); // can only be in this state if we are in a clan

	const bool requestOK = OSI::getSingletonPtr()->requestClanTransferLeadership( player.clanInfo.clanID, _clanMember->playerID );

	WaitForOSIRequest::getSingletonPtr()->start( requestOK, "One Moment", "Transfering clan leadership." );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the OSI_TRANSFER_LEADERSHIP state.
///
/// \return true to continue use case processing
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Clans::_updateOSIClanTransferLeadership()
{
	_checkOSIRequestStatus(OSI_GET_MEMBER_LIST, MENU_CLAN_MEMBERS);
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called each time the OSI_SEND_FRIEND_INVITE state is entered.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Clans::_enterOSIClanSendFriendInvite()
{
	OSIPlayer& player = OSI::getSingletonPtr()->getActiveAccount()->player;
	ASSERT( OSI_CLAN_ROLE_UNKNOWN != player.clanRole ); // can only be in this state if we are in a clan

#ifdef _XBOX
	const bool requestOK = OSI::getSingletonPtr()->requestOfferMyFriendship( 
		const_cast< OSIPlayerID& >( _clanMember->playerID ) );
#else //!_XBOX
	const bool requestOK = OSI::getSingletonPtr()->requestOfferMyFriendship( _clanMember->memberName );
#endif

	WaitForOSIRequest::getSingletonPtr()->start( requestOK, "One Moment", "Sending friend invite." );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the OSI_SEND_FRIEND_INVITE state.
///
/// \return true to continue use case processing
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::Clans::_updateOSIClanSendFriendInvite()
{
	OSIRequestStatus requestStatus = _checkOSIRequestStatus(MENU_CLAN_MEMBERS, MENU_CLAN_MEMBERS);
	if( requestStatus == STATUS_SUCCEEDED )
	{
		OSIchar buffer[ OSI_STRING_LENGTH_LONG ];
		OSIchar nameBuffer[ OSIClanInfo::MAX_FULL_NAME_LENGTH ];
		OSIPlayer& player = OSI::getSingletonPtr()->getActiveAccount()->player;
		OSIsnprintf( buffer, OSI_STRING_LENGTH_LONG, g_translator.Translate( "You have successfully invited %s to be your friend." ),
			GetFullPlayerName( nameBuffer, player.clanInfo.tag, _clanMember->memberName ) );
		DisplayDialog::getSingletonPtr()->start( "Friend Invite", buffer );
	}
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Takes data from OSI's clan info and pushes it into the screen.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Clans::_refreshClanDataForMenuMain()
{
	ScreenClanMain *pScreen = UseCase::GetActiveScreen<ScreenClanMain>();
	if ( !pScreen ) return ;

	OSIPlayer& player = OSI::getSingletonPtr()->getActiveAccount()->player;
	if( OSI_CLAN_ROLE_UNKNOWN != player.clanRole )
	{
		OSIchar buffer[ OSI_STRING_LENGTH_LONG ];

		pScreen->GetTextSprite( ID_CLAN_NAME_TEXT )->SetTextNoTranslate( player.clanInfo.name );
		pScreen->GetTextSprite( ID_CLAN_ABBR_TEXT )->SetTextNoTranslate( player.clanInfo.tag );
		pScreen->GetTextSprite( ID_CLAN_ROLE_TEXT )->SetText( OSIClanRoleToStr(player.clanRole) );
		pScreen->GetTextSprite( ID_CLAN_LEADER_TEXT )->SetTextNoTranslate( player.clanInfo.leaderName );
		
		int rank = int(player.clanInfo.stats.getStat( OSIClanStats::CLANSTAT_RANK ));
		if( rank > 0 )	{ OSIsprintf( buffer, "%d", rank ); }
		else			{ OSIsprintf( buffer, "none" ); }

		pScreen->GetTextSprite( ID_CLAN_RANK_TEXT )->SetText( buffer );
		OSIsprintf( buffer, "%u", player.clanInfo.memberCount );
		pScreen->GetTextSprite( ID_CLAN_MEMBER_COUNT_TEXT )->SetText( buffer );
	}
	else
	{
		pScreen->GetTextSprite( ID_CLAN_NAME_TEXT )->SetText( "no clan" );
		pScreen->GetTextSprite( ID_CLAN_ABBR_TEXT )->SetText( "no clan"  );
		pScreen->GetTextSprite( ID_CLAN_ROLE_TEXT )->SetText( "no clan" );
		pScreen->GetTextSprite( ID_CLAN_LEADER_TEXT )->SetText( "no clan" );
		pScreen->GetTextSprite( ID_CLAN_RANK_TEXT )->SetText( "no clan" );
		pScreen->GetTextSprite( ID_CLAN_MEMBER_COUNT_TEXT )->SetText( "no clan" );
		pScreen->SetNotices( NULL );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Takes data from OSI's clan notices and pushes it into the screen.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::Clans::_refreshClanNoticesForMenuMain()
{
	OSIClanNoticeList& clanNoticeList = OSI::getSingletonPtr()->getClanNoticeList();
	ScreenClanMain *pScreen = UseCase::GetActiveScreen<ScreenClanMain>();
	if ( !pScreen )return ;

	pScreen->SetNotices( &clanNoticeList );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Refactoring common processing of an OSI request into this function.
/// If the calling states need to do something additional then this they can
/// check the return value and act accordingly.
///////////////////////////////////////////////////////////////////////////////////////////////////
OSIRequestStatus OnlineUI::Clans::_checkOSIRequestStatus(State successState, State failureState)
{
	switch( OSI::getSingletonPtr()->getLastRequestStatus() )
	{
		case STATUS_SUCCEEDED:
			_stateMachine.setCurrentState(this, successState);
			return STATUS_SUCCEEDED;

		case STATUS_FAILED:
			_stateMachine.setCurrentState(this, failureState);
			return STATUS_FAILED;

		case STATUS_WORKING:
		default:
			ASSERTS( 0, "Invalid OSI state" );
			return STATUS_WORKING;
	}
}

#ifdef _XBOX
///////////////////////////////////////////////////////////////////////////////////////////////////
void 
OnlineUI::Clans::
_enterOSIClanGetMemberListForSentInvitations()
{
	// make sure there is an entry in the ID list since that tells us which clan to get info for
	OSIPlayer& player = OSI::getSingletonPtr()->getActiveAccount()->player;
	if( OSI_CLAN_ROLE_UNKNOWN == player.clanRole )
	{
		_stateMachine.setCurrentState( this, MENU_CLAN_INVITATIONS );
		return;
	}

	const bool requestOK = OSI::getSingletonPtr()->requestClanMembershipList( player.clanInfo.clanID );

	WaitForOSIRequest::getSingletonPtr()->start( requestOK, "One Moment", "Retrieving Sent Invitations" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool 
OnlineUI::Clans::
_updateOSIClanGetMemberListForSentInvitations()
{
	_checkOSIRequestStatus(MENU_CLAN_INVITATIONS, MENU_CLAN_INVITATIONS);

	// On the xbox, clan member match information has to be retrieved asynch in the background
	// This is the spot where the clan member list is completely populated, so kick off the
	// match searching in the background.
	OSIXbox* osix = static_cast< OSIXbox* >( OSI::getSingletonPtr() );
	osix->endMatchSearchesForClanMembershipList();
	osix->beginMatchSearchesForClanMembershipList();

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool
OnlineUI::Clans::
_updateOSIClanGetMemberFriends()
{
	_checkOSIRequestStatus( MENU_CLAN_MEMBERS, MENU_CLAN_MEMBERS );

	OSIPlayerList& friends = OSI::getSingletonPtr()->getFriendList();
	OSIClanMembershipList& members = OSI::getSingletonPtr()->getClanMembershipList();
	for( friends.begin(); friends.getCurrent(); friends.next() )
	{
		OSIPlayer* f = friends.getCurrent();
		if( OSIClanMembership* memberfriend = members.findByPlayerID( f->id ) )
		{
			memberfriend->isFriend = true;
		}
	}
	return true;
}



#endif // _XBOX
