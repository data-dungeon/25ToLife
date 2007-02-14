///////////////////////////////////////////////////////////////////////////////////////
///  \file UseCaseMainMenu.cpp
///
///  \brief Implementation of the MainMenu class, which is responsible 
/// for presenting the Main Menu and allowing the player to pick what
/// they want to do.
///////////////////////////////////////////////////////////////////////////////////////

#include "TTLPCH.h"

#include "UseCaseEORDeleteAccount.hpp"
#include "frontend/frontend.h"
#include "../screens/screen_EORDeleteAccount.h"
#include "../screens/screen_wait.h"
#include "UseCaseYesNoDialog.hpp"
#include "UseCaseDisplayDialog.hpp"
#include "frontend/savegame.h"
#include "platform/MemoryCard/MemoryCard.h"
#include "EORAccount.hpp"
#include "UseCaseUtility.hpp"
#include "frontend/savegame.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor: Initialize member variables
///////////////////////////////////////////////////////////////////////////////////////////////////
OnlineUI::EORDeleteAccount::EORDeleteAccount() : 
    _stateMachine(ASK_TO_DELETE)
{
    _stateMachine.setStateEnter(ASK_TO_DELETE,     &OnlineUI::EORDeleteAccount::_enterAskToDelete);
    _stateMachine.setStateUpdate(ASK_TO_DELETE,    &OnlineUI::EORDeleteAccount::_updateAskToDelete);
#ifdef DIRECTX_PC
	_stateMachine.setStateEnter(DELETE_ACCT,      &OnlineUI::EORDeleteAccount::_enterDelete);
#else
    _stateMachine.setStateEnter(DELETE,           &OnlineUI::EORDeleteAccount::_enterDelete);
#endif
    _stateMachine.setStateEnter(SAVE_ACCOUNT,     &OnlineUI::EORDeleteAccount::_enterSave);
    _stateMachine.setStateUpdate(SAVE_ACCOUNT,    &OnlineUI::EORDeleteAccount::_updateSave);
	_stateMachine.setStateUpdate(SHUT_DOWN,       &OnlineUI::EORDeleteAccount::_updateShutDown);
}

void OnlineUI::EORDeleteAccount::start( ts_OnlineAccount* accountPtr )
{
	_accountPtr = accountPtr;

	UseCase::start();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called before use case is started.  Show the first screen for the use case.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::EORDeleteAccount::_init( void )
{
	_stateMachine.setCurrentState(this, ASK_TO_DELETE);

	_deletedAccount = false;
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called after use case is ended.  Clean up the screens and other objects.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::EORDeleteAccount::_deinit( void )
{
	g_FrontEnd.RemoveAllPopups(); // end any popups that might be up

	g_MemCardManager->SetUsePopups( true );
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////
/// Save the current account out to the profile group and to the memory card.
///////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::EORDeleteAccount::_enterAskToDelete()
{
	char dialogText[256];
	strcpy(dialogText, "Are you sure you want to delete the saved Twenty-Five to Life Account ");
	strcat(dialogText, _accountPtr->AccountName);
#ifdef PS2
	strcat(dialogText, " from the memory card (8MB) (for PlayStation®2)");
#endif
	strcat(dialogText, "?");
	YesNoDialog::getSingletonPtr()->start( "Delete Saved Eidos Online Account Profile", dialogText, false );   
}

///////////////////////////////////////////////////////////////////////////////////////
/// Wait until the memory card is done saving
///
/// \return True to continue updating use case, false to return to main menu
///////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORDeleteAccount::_updateAskToDelete()
{
	switch( YesNoDialog::getSingletonPtr()->GetAnswer() )
	{
	case YesNoDialog::YES:
#ifdef DIRECTX_PC
		_stateMachine.setCurrentState(this, DELETE_ACCT);
#else
		_stateMachine.setCurrentState(this, DELETE);
#endif
		ShowPopupTextOnly( "", "Deleting account, please wait..." );
		break;
	case YesNoDialog::NO:
	case YesNoDialog::CANCEL:
	default:
		_stateMachine.setCurrentState( this, SHUT_DOWN );
		break;
	}
return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when entering the DELETE state.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::EORDeleteAccount::_enterDelete()
{
    // Delete account locally
    _accountPtr->InUse = false;

	// Select the first account in our list
	for( int i = 0; i < MAX_ONLINE_ACCOUNTS; ++i )
	{
		if( g_GlobalProfile.OnlineAccounts[i].InUse )
		{
			g_GlobalProfile.CurrentAccount = i;
			break;
		}
	}

    // Save to memory card
    _stateMachine.setCurrentState(this, SAVE_ACCOUNT);
}

///////////////////////////////////////////////////////////////////////////////////////
/// Save the current account out to the profile group and to the memory card.
///////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::EORDeleteAccount::_enterSave()
{
#ifdef DIRECTX_PC
	g_MemCardManager->SetUsePopups(false);
#endif
    EORAccount::saveAccounts( false, false );
}

///////////////////////////////////////////////////////////////////////////////////////
/// Wait until the memory card is done saving
///
/// \return True to continue updating use case, false to return to main menu
///////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORDeleteAccount::_updateSave()
{
    if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_IDLE)
	{
		EndPopupTextOnly();
		g_MemCardManager->ResetLoadSaveBuffer();
		g_MemCardManager->SetUsePopups( true );

		const int memCardPopupResult = g_MemCardManager->GetPopupExitResult();
		if (memCardPopupResult != PR_COMPLETED_OK)
		{
			// User has canceled save, or save failed
			// Either way, don't delete the account and warn user
			_restoreDeletedAccount();

			char warningMsg[256] = {0};
#ifdef PS2
			sprintf( warningMsg, "The account you selected was not deleted from the memory card (8MB) (for PlayStation®2)" );
#elif defined DIRECTX_PC
			sprintf( warningMsg, "The account you selected was not successfully deleted from disk." );
#endif
			DisplayDialog::getSingletonPtr()->start( "Warning", warningMsg );
		}
		else
		{
			char successMsg[256] = {0};
#ifdef PS2
			sprintf( successMsg, "Account successfully deleted from the memory card (8MB) (for PlayStation®2)" );
#elif defined DIRECTX_PC
			sprintf( successMsg, "Account successfully deleted from disk." );
#endif
			DisplayDialog::getSingletonPtr()->start( "Success", successMsg );
			_deletedAccount = true;
		}

		_stateMachine.setCurrentState( this, SHUT_DOWN );
	}

    return true;
}

bool OnlineUI::EORDeleteAccount::_updateShutDown()
{
	stop();
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////
/// Deleting account from memory card failed, restore it back in memory
///////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::EORDeleteAccount::_restoreDeletedAccount()
{
	_accountPtr->InUse = true;

	// Select this account again
	for( int i = 0; i < MAX_ONLINE_ACCOUNTS; ++i )
	{
		if( _accountPtr == &g_GlobalProfile.OnlineAccounts[i] )
		{
			g_GlobalProfile.CurrentAccount = i;
			break;
		}
	}
}
