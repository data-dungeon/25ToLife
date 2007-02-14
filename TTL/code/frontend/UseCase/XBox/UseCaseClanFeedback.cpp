#include "TTLPCH.h"
#include "../../screens/screen_XboxClanFeedback.h"
#include "../../screens/screen_XboxClanFeedbackPopup.h"
#include "frontend/frontend.h"
#include "UseCaseClanFeedback.hpp"
#include "../UseCaseDisplayDialog.hpp"
#include "OSI/Xbox/OSIXbox.hpp"
#include "OSI/Xbox/OSIXboxClansUtility.hpp"
#include "../UseCaseWaitForOSIRequest.hpp"

using OnlineUI::XboxClanFeedback;

XboxClanFeedback::XboxClanFeedback()
: _stateMachine( PROCESS_GET_CLAN_INFO ),
  _selectedClan( 0 )
{
	_stateMachine.setStateEnter( PROCESS_GET_CLAN_INFO, &OnlineUI::XboxClanFeedback::_enterProcessGetClanInfo );
	_stateMachine.setStateUpdate( PROCESS_GET_CLAN_INFO, &OnlineUI::XboxClanFeedback::_updateProcessGetClanInfo  );

	_stateMachine.setStateEnter( DISPLAY_CLANS, &OnlineUI::XboxClanFeedback::_enterDisplayClans );
	_stateMachine.setStateUpdate( DISPLAY_CLANS, &OnlineUI::XboxClanFeedback::_updateDisplayClans );

	_stateMachine.setStateEnter( DISPLAY_FEEDBACK_DIALOG, &OnlineUI::XboxClanFeedback::_enterDisplayFeedbackDialog );
	_stateMachine.setStateUpdate( DISPLAY_FEEDBACK_DIALOG, &OnlineUI::XboxClanFeedback::_updateDisplayFeedbackDialog );

	_stateMachine.setStateEnter( PROCESS_SEND_FEEDBACK, &OnlineUI::XboxClanFeedback::_enterProcessSendFeedback );
	_stateMachine.setStateUpdate( PROCESS_SEND_FEEDBACK, &OnlineUI::XboxClanFeedback::_updateProcessSendFeedback );
}

XboxClanFeedback::~XboxClanFeedback( void )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// UseCase methods
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
// Called when UseCase is start()-ed.
bool XboxClanFeedback::_init()
{
	_selectedClan = 0;

	g_FrontEnd.GotoScreen( "SCREEN_ID_XBOX_CLAN_FEEDBACK" );

	// setCurrentState ignores the call if newstate == currentstate, which means that if we switch to 
	// the first state again after having just shut down in it, the entry function won't get called.
	// So switch to the null state just to get the entry function to call properly.
	_stateMachine.setCurrentState( this, RESET );
	_stateMachine.update( this );

	_stateMachine.setCurrentState( this, PROCESS_GET_CLAN_INFO );
	return true;
}

//-------------------------------------------------------------------------------------------------
// Called when UseCase is stop()-ed.
bool XboxClanFeedback::_deinit()
{
	g_FrontEnd.RemoveAllPopups();
	if( UseCase::IsScreenActive< ScreenXboxClanFeedback >() )
		g_FrontEnd.EndScreen(0);
	g_FrontEnd.PopScreenHistory( "SCREEN_ID_XBOX_CLAN_FEEDBACK" );
	return true;
}

//-------------------------------------------------------------------------------------------------
// Called when UseCase goes from being buried to being at top of stack.
void XboxClanFeedback::_onOpen()
{
}

//-------------------------------------------------------------------------------------------------
// Called when UseCase goes from being at top of stack to being buried.
void XboxClanFeedback::_onPause()
{
}

//-------------------------------------------------------------------------------------------------
// Called every frame that UseCase is at top of stack.
bool XboxClanFeedback::update()
{
	_stateMachine.update( this );
	return true;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
// State handler methods
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
// Process: Get clan info for recent players.
void XboxClanFeedback::_enterProcessGetClanInfo()
{
	OSI* osi = OSI::getSingletonPtr();
	OSIPlayerList& playerList = osi->getRecentPlayerList();
	
	/*
	OSIPlayer testPlayer;
	strcpy( testPlayer.name, "ritual akuma" );
	testPlayer.id.xBox.userId = 0x00095FB1FB1FB75D;
	testPlayer.id.xBox.userFlags = 0;
	playerList.push_back( testPlayer );

	strcpy( testPlayer.name, "ritualz" );
	testPlayer.id.xBox.userId = 0x0009715F15F16643;
	testPlayer.id.xBox.userFlags = 0;
	playerList.push_back( testPlayer );
	*/

	if( playerList.count() == 0 )
	{
		stop();
		DisplayDialog::getSingletonPtr()->start( "Clan Feedback", "You haven't played with any people from other clans during this session." );
		return;
	}

	ClansAdapter::OSICreateSingleton();
	ClansAdapter* clanIO = ClansAdapter::OSIGetSingleton();
	clanIO->BeginDownloadClanInfo( &playerList );
}

bool XboxClanFeedback::_updateProcessGetClanInfo()
{
	// Process clan info downloads.
	ClansAdapter* clanIO = ClansAdapter::OSIGetSingleton();
	if( !clanIO->IsDownloadClanInfoComplete() )
	{
		clanIO->ContinueDownloadClanInfo();
		return true;
	}
	clanIO->FinishDownloadClanInfo();
	ClansAdapter::OSIDestroySingleton();

	// players in the recent player list now have valid clanInfo
	OSIPlayerList& playerList = OSI::getSingletonPtr()->getRecentPlayerList();
	bool noClans = true;
	OSIPlayer* player = 0;
	for( playerList.begin(); player = playerList.getCurrent(); playerList.next() )
	{
		if( (player->clanInfo.clanID != OSI::getSingletonPtr()->getActivePlayer()->clanInfo.clanID)
			&& (player->clanInfo.memberCount > 0) )
		{
			noClans = false;
			break;
		}
	}

	if( noClans )
	{
		stop();
		DisplayDialog::getSingletonPtr()->start( "Clan Feedback", "You haven't played with any people from other clans during this session." );
		return true;
	}

	_stateMachine.setCurrentState( this, DISPLAY_CLANS );

	return true;
}

//-------------------------------------------------------------------------------------------------
// Display: Clan list.
void XboxClanFeedback::_enterDisplayClans()
{
	ScreenXboxClanFeedback *pScreen = UseCase::GetActiveScreen< ScreenXboxClanFeedback >();
	if( !pScreen ) return;
	pScreen->PopulateFromPlayerList( OSI::getSingletonPtr()->getRecentPlayerList() );
}

bool XboxClanFeedback::_updateDisplayClans()
{
    if (!g_FrontEnd.GetActiveScreen()) return true;

    ScreenXboxClanFeedback* pScreen = UseCase::GetActiveScreen< ScreenXboxClanFeedback >();
	if ( !pScreen ) return true ;

    switch (pScreen->GetScreenStatus())
    {
		case ScreenXboxClanFeedback::CLAN_SELECTED:
		{
			OSIPlayer* p = OSI::getSingletonPtr()->getRecentPlayerList()[ pScreen->GetSelectedClan() ];
			if( p && p->clanInfo.memberCount > 0 )
			{
				_selectedClan = &p->clanInfo;
				_stateMachine.setCurrentState( this, DISPLAY_FEEDBACK_DIALOG );
			}
			else
			{
				_selectedClan = 0;
				_stateMachine.setCurrentState( this, DISPLAY_CLANS );
			}
			break;
		}
		
        // Back button hit or back option selected
        case ScreenXboxClanFeedback::BACK:
            stop();
            break;
	};
	return true;
}


//-------------------------------------------------------------------------------------------------
// Display: Feedback selection dialog.
void XboxClanFeedback::_enterDisplayFeedbackDialog()
{
	g_FrontEnd.ShowPopup( "SCREEN_ID_XBOX_CLAN_FEEDBACK_POPUP" );

}

bool XboxClanFeedback::_updateDisplayFeedbackDialog()
{
	ScreenXboxClanFeedbackPopup *pScreen = expectPopup< ScreenXboxClanFeedbackPopup >();
	if ( !pScreen ) return true ;

    switch (pScreen->GetScreenState())
	{
	case ScreenXboxClanFeedbackPopup::REPORT_OFFENSIVE_ABBREV:
		g_FrontEnd.EndPopup( 1 );
		_feedbackType = XONLINE_FEEDBACK_NEG_TEAM_NAME;
        _stateMachine.setCurrentState( this, PROCESS_SEND_FEEDBACK );
		break;

	case ScreenXboxClanFeedbackPopup::REPORT_OFFENSIVE_NAME:
		g_FrontEnd.EndPopup( 1 );
		_feedbackType = XONLINE_FEEDBACK_NEG_TEAM_DESCRIPTION;
        _stateMachine.setCurrentState( this, PROCESS_SEND_FEEDBACK );
		break;

	case ScreenXboxClanFeedbackPopup::BACK:
		g_FrontEnd.EndPopup( 0 );
        _stateMachine.setCurrentState( this, DISPLAY_CLANS );
        break;
	};

	return true;
}


//-------------------------------------------------------------------------------------------------
// Process: Send feedback.
void XboxClanFeedback::_enterProcessSendFeedback()
{
	if( !_selectedClan )
	{
		return;
	}

	OSIXbox* osix = (OSIXbox*) OSI::getSingletonPtr();
	XUID xuid;
	OSIClanID_to_XUID( _selectedClan->clanID, xuid );
	const bool requestOK = osix->requestSendFeedback( xuid, (XONLINE_FEEDBACK_TYPE) _feedbackType );
	WaitForOSIRequest::getSingletonPtr()->start( requestOK, "Clan Feedback", "Sending feedback" );
}

bool XboxClanFeedback::_updateProcessSendFeedback()
{
	_stateMachine.setCurrentState( this, DISPLAY_CLANS );
	return true;
}
