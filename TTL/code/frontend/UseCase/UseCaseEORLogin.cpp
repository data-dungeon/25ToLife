#include "TTLPCH.h"

#include "UseCaseEORLogin.hpp"
#include "UseCaseEORConnect.hpp"
#include "UseCaseEORCreateAccount.hpp"
#include "UseCaseEORDeleteAccount.hpp"
#include "UseCaseEOREditAccount.hpp"
#include "UseCaseDisplayDialog.hpp"
#include "UseCaseYesNoDialog.hpp"
#include "UseCaseEORPassword.hpp"
#include "UseCaseEORRecoverAccount.hpp"
#include "EORAccount.hpp"
#include "../screens/screen_EORLogin.h"
#include "../screens/screen_wait.h"
#include "../screens/screen_AskPassword.h"
#include "frontend/frontend.h"
#include "frontend/savegame.h"
#include "platform/MemoryCard/MemoryCard.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor: Initialize member variables
///////////////////////////////////////////////////////////////////////////////////////////////////
OnlineUI::EORLogin::EORLogin() : 
    _stateMachine(MENU_LOGIN)
{
	_stateMachine.setStateUpdate(UPDATE_ACCOUNTS,     &OnlineUI::EORLogin::_updateAccountInfo);
    _stateMachine.setStateEnter(MENU_LOGIN,           &OnlineUI::EORLogin::_enterMenuLogin);
	_stateMachine.setStateUpdate(MENU_LOGIN_UPDATE,   &OnlineUI::EORLogin::_updateMenuLogin);
	_stateMachine.setStateUpdate(LOGIN_WITH_CURRENT_ACCOUNT, &OnlineUI::EORLogin::_updateLoginWithCurrentAccount);
	_stateMachine.setStateEnter(ASK_FOR_PASSWORD,     &OnlineUI::EORLogin::_enterAskPassword);
	_stateMachine.setStateUpdate(ASK_FOR_PASSWORD,    &OnlineUI::EORLogin::_updateAskPassword);
    _stateMachine.setStateUpdate(RECOVER_ACCOUNT,     &OnlineUI::EORLogin::_updateRecoverAccount);
    _stateMachine.setStateEnter(DELETE_ACCOUNT,       &OnlineUI::EORLogin::_enterDelete);
	_stateMachine.setStateUpdate(DELETE_ACCOUNT,      &OnlineUI::EORLogin::_updateDelete);
	_stateMachine.setStateEnter(ASK_TO_DISCO,       &OnlineUI::EORLogin::_enterAskToDisco);
	_stateMachine.setStateUpdate(ASK_TO_DISCO,      &OnlineUI::EORLogin::_updateAskToDisco);
	_stateMachine.setStateUpdate(SHUT_DOWN,		      &OnlineUI::EORLogin::_updateShutDown);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called before use case is started.  Do nothing.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::EORLogin::_init( void )
{
	g_FrontEnd.GotoScreen("SCREEN_ID_EOR_LOGIN_4_00");
	_stateMachine.setCurrentState( this, MENU_LOGIN );
    return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called after use case is ended.  Clean up the screens and other objects.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::EORLogin::_deinit( void )
{
    // Get rid of the screen
	g_FrontEnd.RemoveAllPopups();
	if( UseCase::IsScreenActive< ScreenEORLogin >() )
	{
		g_FrontEnd.EndScreen(0);
	}
	g_FrontEnd.PopScreenHistory( "SCREEN_ID_EOR_LOGIN_4_00" );

    return true;
}

/*virtual*/ void OnlineUI::EORLogin::_onOpen( void )
{
    ScreenEORLogin *pScreen = expectScreen<ScreenEORLogin>();
	if ( pScreen )
	{
		pScreen->Reset();
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when entering the MENU_LOGIN state.  Update screen account information.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::EORLogin::_enterMenuLogin()
{
	_justCreatedAccount = EORCreateAccount::getSingletonPtr()->createdAccount();
	if( _justCreatedAccount )
	{
		// We just created an account, login with it right away
		_stateMachine.setCurrentState( this, LOGIN_WITH_CURRENT_ACCOUNT );
		return;
	}
	_stateMachine.setCurrentState( this, MENU_LOGIN_UPDATE );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the MENU_LOGIN_UPDATE state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORLogin::_updateMenuLogin()
{
    ScreenEORLogin *pScreen = expectScreen<ScreenEORLogin>();
	if ( !pScreen )
		return true ;

    switch (pScreen->GetScreenStatus())
    {
        case ScreenEORLogin::LOGIN:
			_checkPassword();
            break;

        case ScreenEORLogin::CREATE_ACCOUNT:
            EORCreateAccount::getSingletonPtr()->start();
            _stateMachine.setCurrentState(this, UPDATE_ACCOUNTS);
            break;

        case ScreenEORLogin::EDIT_ACCOUNT:
            EOREditAccount::getSingletonPtr()->start(&g_GlobalProfile.OnlineAccounts[g_GlobalProfile.CurrentAccount]);
            _stateMachine.setCurrentState(this, UPDATE_ACCOUNTS);
            break;

        case ScreenEORLogin::RECOVER_ACCOUNT:
            EORRecoverAccount::getSingletonPtr()->start();
            _stateMachine.setCurrentState(this, RECOVER_ACCOUNT);
            break;

        case ScreenEORLogin::DELETE_ACT:
            _stateMachine.setCurrentState(this, DELETE_ACCOUNT);
            break;

		case ScreenEORLogin::PASSWORD_ENTERED:
			_password = g_FrontEnd.GetKeyboard()->GetString();
    		_login();
			break;

        case ScreenEORLogin::BACK:
#ifdef DIRECTX_PC
            _stateMachine.setCurrentState(this, SHUT_DOWN);
#else
			_stateMachine.setCurrentState(this, ASK_TO_DISCO);
#endif
            break;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Log in with the current account
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORLogin::_updateLoginWithCurrentAccount()
{
	_checkPassword();
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Stub state to force restart of login state so accounts are refreshed.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORLogin::_updateAccountInfo()
{
	_stateMachine.setCurrentState(this, MENU_LOGIN);
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Ask the user for a password before logging in
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::EORLogin::_enterAskPassword()
{
	g_FrontEnd.ShowPopup("SCREEN_ID_ASKPWD_POPUP");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Get the recovered account and log in
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORLogin::_updateAskPassword()
{
	ScreenAskPasswordPopup *pPopup = expectPopup<ScreenAskPasswordPopup>();
	if ( !pPopup )
	{
		return true ;
	}

	switch(pPopup->GetScreenStatus()) 
	{
		case ScreenAskPasswordPopup::ENTER_PASSWORD:
			g_FrontEnd.EndPopup(0); // kill off ask popup
            UseCase::ActivateKeyboardForPasswords( "" );
			_stateMachine.setCurrentState(this, MENU_LOGIN);
			break;
		
		case ScreenAskPasswordPopup::BACK:
			g_FrontEnd.EndPopup(0);
			_stateMachine.setCurrentState(this, MENU_LOGIN);
			break;
	}
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Get the recovered account and log in
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORLogin::_updateRecoverAccount()
{
    _stateMachine.setCurrentState(this, MENU_LOGIN);

    // If we're recovering an account, select that one and log in with it
    if (EORRecoverAccount::getSingletonPtr()->getAccountNum() != -1)
    {
		g_GlobalProfile.CurrentAccount = EORRecoverAccount::getSingletonPtr()->getAccountNum();
        _password = EORRecoverAccount::getSingletonPtr()->getPassword();

        _login();
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Check if the user has saved his password or not
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::EORLogin::_checkPassword()
{
	ts_OnlineAccount* account = &g_GlobalProfile.OnlineAccounts[g_GlobalProfile.CurrentAccount];

	if(account->Password[0] == 0) 
	{
		_stateMachine.setCurrentState(this, ASK_FOR_PASSWORD);
	}
	else 
	{
		_password = account->Password;
		_login();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Do everything needed to log in
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::EORLogin::_login()
{
	ts_OnlineAccount* account = &g_GlobalProfile.OnlineAccounts[g_GlobalProfile.CurrentAccount];

	_stateMachine.setCurrentState(this, UPDATE_ACCOUNTS);

	// Was this a just created account and was it saved to memory card? overriding changed flag
	if( _justCreatedAccount && account->Changed )
	{
		// Account was not saved to memory card, kill it in system memory now
		account->InUse = false;
		account->Changed = false;
		_justCreatedAccount = false;
	}

    EORConnect::getSingletonPtr()->setLoginName(account->AccountName);
    EORConnect::getSingletonPtr()->setLoginPassword(_password);
	// Clear out password if they chose not to save it
	if( !account->SavePassword )
	{
		account->Password[0] = 0;
	}

	if( ScreenEORLogin *pScreen = UseCase::GetActiveScreen< ScreenEORLogin >() )
	{
		// Clear any action that the user may have made to the screen
		pScreen->GetScreenStatus();
	}

    EORConnect::getSingletonPtr()->start();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::EORLogin::_enterDelete()
{
#ifndef DIRECTX_PC
	g_MemCardManager->DetectCardWithResult(g_MemCardManager->GetPort());
#endif // DIRECTX_PC
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORLogin::_updateDelete()
{
    if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_IDLE)
	{
		g_MemCardManager->ResetLoadSaveBuffer();

        MEMCARD_RESULT result = (MEMCARD_RESULT)g_MemCardManager->GetResult();

        if (result == MEMCARD_RESULT_OK)
        {
			EORDeleteAccount::getSingletonPtr()->start( &g_GlobalProfile.OnlineAccounts[g_GlobalProfile.CurrentAccount] );
            _stateMachine.setCurrentState(this, UPDATE_ACCOUNTS);
        }
        else
        {
            DisplayDialog::getSingletonPtr()->start("Delete Account Failed", "You must have a memory card (8MB) (for PlayStation®2) inserted to delete accounts.");
            _stateMachine.setCurrentState(this, MENU_LOGIN);
        }

		g_MemCardManager->ResetLoadSaveBuffer();
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when entering the ASK_TO_DISCO state.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::EORLogin::_enterAskToDisco()
{
	YesNoDialog::getSingletonPtr()->start( "", "Are you sure you want to disconnect from the network?", false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the ASK_TO_DISCO state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORLogin::_updateAskToDisco()
{
	switch( YesNoDialog::getSingletonPtr()->GetAnswer() )
	{
	case YesNoDialog::YES:
		stop();
		break;
	case YesNoDialog::NO:
	case YesNoDialog::CANCEL:
	default:
		_stateMachine.setCurrentState( this, MENU_LOGIN_UPDATE );
		break;
	}

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORLogin::_updateShutDown()
{
	stop();
	return true;
}