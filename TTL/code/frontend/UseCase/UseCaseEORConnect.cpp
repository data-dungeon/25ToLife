#include "TTLPCH.h"

#include "UseCaseEORConnect.hpp"
#include "frontend/frontend.h"
#include "UseCaseMultiplayerMenu.hpp"
#include "UseCaseDisplayDialog.hpp"
#include "UseCaseEORRecoverAccount.hpp"
#include "UseCaseWaitForOSIRequest.hpp"
#include "../screens/screen_wait.h"
#include "platform/MemoryCard/MemoryCard.h"
#include "frontend/savegame.h"
#include "EORAccount.hpp"
#include "UseCaseUtility.hpp"
#include "main/NetMonitor.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor: Initialize member variables
///////////////////////////////////////////////////////////////////////////////////////////////////
OnlineUI::EORConnect::EORConnect() : 
    _stateMachine(CONNECTING),
	_connecting( false )
{
    _stateMachine.setStateEnter(CONNECTING,       &OnlineUI::EORConnect::_enterConnecting);
    _stateMachine.setStateUpdate(CONNECTING,      &OnlineUI::EORConnect::_updateConnecting);
	_stateMachine.setStateEnter(REQUEST_ACCOUNT_INFO, &OnlineUI::EORConnect::_enterRequestAccountInfo);
	_stateMachine.setStateUpdate(REQUEST_ACCOUNT_INFO, &OnlineUI::EORConnect::_updateRequestAccountInfo);
    _stateMachine.setStateEnter(SAVE_ACCOUNT,     &OnlineUI::EORConnect::_enterSave);
    _stateMachine.setStateUpdate(SAVE_ACCOUNT,    &OnlineUI::EORConnect::_updateSave);
    _stateMachine.setStateUpdate(SHUT_DOWN,       &OnlineUI::EORConnect::_updateShutDown);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called before use case is started.  Show the first screen for the use case.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::EORConnect::_init( void )
{
	_stateMachine.setCurrentState(this, CONNECTING);

	_connecting = false;
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called after use case is ended.  Clean up the screens and other objects.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::EORConnect::_deinit( void )
{
	g_CNetMonitor.Enable( true );

	// If we're logging in with a recovered account and we were in the middle of connecting, invalidate it
	if( _connecting )
	{
		if (EORRecoverAccount::getSingletonPtr()->getAccountNum() != -1) {
			g_GlobalProfile.OnlineAccounts[EORRecoverAccount::getSingletonPtr()->getAccountNum()].InUse = false;
			EORRecoverAccount::getSingletonPtr()->clearAccount();
		}
	}
	_connecting = false;
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Set the name to use when logging in
///
/// \param pName The name to use when logging in
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::EORConnect::setLoginName(const OSIchar *pName)
{
    OSIstrncpy(_login, pName, EORAccount::NAME_BUFFER_SIZE);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Set the password to use when logging in
///
/// \param pPassword The password to use when logging in
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::EORConnect::setLoginPassword(const OSIchar *pPassword)
{
    OSIstrncpy(_password, pPassword, EORAccount::PASSWORD_BUFFER_SIZE);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when entering the CONNECTING state.  Fire off the OSI request to log in.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::EORConnect::_enterConnecting()
{
	g_CNetMonitor.Enable( false );
    bool success = OSI::getSingletonPtr()->requestLogin(_login, _password);

	if( success )
	{
		_connecting = true;
	}
    WaitForOSIRequest::getSingletonPtr()->start(success, "Logging In", "Please wait");
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the CONNECTING state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORConnect::_updateConnecting()
{
	g_CNetMonitor.Enable( true );
	OSIRequestStatus requestStatus = OSI::getSingletonPtr()->getLastRequestStatus();
	if ( requestStatus == STATUS_SUCCEEDED )
	{
		OSIPlayer* activePlayer = OSI::getSingletonPtr()->getActivePlayer();
		ASSERT( activePlayer );
		if( activePlayer )
		{
			char fullname[OSI_PLAYER_FULL_NAME_LENGTH];
			activePlayer->GetFullName( fullname );

			g_playerProfile.SetAccountName( activePlayer->name );
			g_playerProfile.SetDisplayName( fullname );
			g_playerProfile.SetRank( activePlayer->statRanking );
			g_playerProfile.SetRating( activePlayer->statRating );
		}

        // If we're logging in with a recovered account, get account info and then save to memory card
        if (EORRecoverAccount::getSingletonPtr()->getAccountNum() != -1)
        {
            _stateMachine.setCurrentState(this, REQUEST_ACCOUNT_INFO);
			return true;
        }
	}
	else
	{
		// If we were trying to login with a recovered account, invalidate it
        if (EORRecoverAccount::getSingletonPtr()->getAccountNum() != -1)
		{
			g_GlobalProfile.OnlineAccounts[EORRecoverAccount::getSingletonPtr()->getAccountNum()].InUse = false;
			EORRecoverAccount::getSingletonPtr()->clearAccount();
		}
	}

	_connecting = false;
	_stateMachine.setCurrentState( this, SHUT_DOWN );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when entering the REQUEST_ACCOUNT_INFO state.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::EORConnect::_enterRequestAccountInfo()
{
    bool success = OSI::getSingletonPtr()->requestAccountInfo();

    WaitForOSIRequest::getSingletonPtr()->start(success, "Retrieving Account Info", "Please wait...", false);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the REQUEST_ACCOUNT_INFO state.
///
/// \return true to continue use case processing
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORConnect::_updateRequestAccountInfo()
{
	OSIRequestStatus requestStatus = OSI::getSingletonPtr()->getLastRequestStatus();
	if ( requestStatus == STATUS_SUCCEEDED )
	{
		OSIAccount &osiAccount = OSI::getSingletonPtr()->getAccountInfo();
		ts_OnlineAccount &onlineAccount = g_GlobalProfile.OnlineAccounts[EORRecoverAccount::getSingletonPtr()->getAccountNum()];
		onlineAccount.BirthDay = osiAccount.birthdayDay;
		onlineAccount.BirthMonth = osiAccount.birthdayMonth;
		onlineAccount.BirthYear = osiAccount.birthdayYear;
		onlineAccount.Country = osiAccount.countryCode;
		onlineAccount.Gender = osiAccount.flags & 0x1 ? true : false;
		onlineAccount.EidosPromos = osiAccount.flags & 0x2 ? true : false;
		onlineAccount.EidosPartners = osiAccount.flags & 0x4 ? true : false;
		OSIstrncpy( onlineAccount.ZipCode, osiAccount.postalCode, NUM_ARRAY_ELEMENTS(onlineAccount.ZipCode) );
		OSIstrncpy( onlineAccount.Email, osiAccount.email, NUM_ARRAY_ELEMENTS(onlineAccount.Email) );
	}

	_stateMachine.setCurrentState( this, SAVE_ACCOUNT );
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////
/// Save the current account out to the profile group and to the memory card.
///////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::EORConnect::_enterSave()
{
    EORAccount::saveAccounts();
}

///////////////////////////////////////////////////////////////////////////////////////
/// Wait until the memory card is done saving
///
/// \return True to continue updating use case, false to return to main menu
///////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORConnect::_updateSave()
{
    if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_IDLE)
	{
		g_MemCardManager->ResetLoadSaveBuffer();

		const int memCardPopupResult = g_MemCardManager->GetPopupExitResult();
		if (memCardPopupResult != PR_COMPLETED_OK)
		{
			// User has canceled save, or save failed
			// Either way, kill the account and warn user
			g_GlobalProfile.OnlineAccounts[EORRecoverAccount::getSingletonPtr()->getAccountNum()].InUse = false;
			g_GlobalProfile.OnlineAccounts[EORRecoverAccount::getSingletonPtr()->getAccountNum()].Changed = false;

			char warningMsg[256] = {0};
#ifdef PS2
			sprintf( warningMsg, "Your account was not saved to the memory card (8MB) (for PlayStation®2).  "
								 "You will not be able to log in with this account later unless you "
								 "save it to the memory card (8MB) (for PlayStation®2)." );
#elif defined DIRECTX_PC
			sprintf( warningMsg, "Your account was not saved to disk.  You will not be "
								 "able to log in with this account later unless you save it." );
#endif
			DisplayDialog::getSingletonPtr()->start( "Warning", warningMsg );
		}

        // Clear account in EORRecoverAccount so we don't try to save again
        EORRecoverAccount::getSingletonPtr()->clearAccount();

		_connecting = false;
        _stateMachine.setCurrentState( this, SHUT_DOWN );
	}

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////
/// Reset to initial state
///////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORConnect::_updateShutDown()
{
    stop();
	// If we succesfully logged in, start up the Multiplayer Menu
	OSIRequestStatus requestStatus = OSI::getSingletonPtr()->getLastRequestStatus();
	if ( requestStatus == STATUS_SUCCEEDED )
	{
		MultiplayerMenu::getSingletonPtr()->start();
	}
	return true;
}
