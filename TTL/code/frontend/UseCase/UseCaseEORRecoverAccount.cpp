#include "TTLPCH.h"

#include "UseCaseEORRecoverAccount.hpp"
#include "UseCaseDisplayDialog.hpp"
#include "frontend/frontend.h"
#include "frontend/savegame.h"
#include "../screens/screen_EORRecoverAccount.h"
#include "../screens/screen_EORCreateAccount2.h"
#include "../screens/screen_wait.h"
#include "platform/MemoryCard/MemoryCard.h"
#include "UseCaseStrings.hpp"
#include "data/screens/EOR.h"
#include "EORAccount.hpp"
#include "UseCaseUtility.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor: Initialize member variables
///////////////////////////////////////////////////////////////////////////////////////////////////
OnlineUI::EORRecoverAccount::EORRecoverAccount() : 
    _stateMachine(RECOVER1),
    _recoveredAccount(-1)
{
    _stateMachine.setStateUpdate(RECOVER1,         &OnlineUI::EORRecoverAccount::_updateRecover1);
    _stateMachine.setStateUpdate(WAIT_FOR_RECOVER, &OnlineUI::EORRecoverAccount::_updateWait);
	_stateMachine.setStateUpdate(SHUT_DOWN,        &OnlineUI::EORRecoverAccount::_updateShutDown);

}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called before use case is started.  Show the first screen for the use case.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::EORRecoverAccount::_init(void)
{
    _recoveredAccount = -1;
    g_FrontEnd.GotoScreen("SCREEN_ID_RECOVER");

    ScreenEORRecoverAccount *pScreen = UseCase::GetActiveScreen<ScreenEORRecoverAccount>();
	if ( !pScreen ) return true ;

	// Reset member variables
    _onlineName[0] = 0;
    _password[0] = 0;
    _errorString[0] = 0;

	pScreen->setName("");
    pScreen->setPassword("");
    pScreen->setSavePassword(true);

	_stateMachine.setCurrentState(this, RECOVER1);

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called after use case is ended.  Clean up the screens and other objects.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::EORRecoverAccount::_deinit(void)
{
	g_FrontEnd.RemoveAllPopups(); // end memory card popup
    if( UseCase::IsScreenActive< ScreenEORRecoverAccount >() )
	{
		g_FrontEnd.EndScreen(0);
	}
	g_FrontEnd.PopScreenHistory( "SCREEN_ID_RECOVER" );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the RECOVER1 state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORRecoverAccount::_updateRecover1()
{
    if (!g_FrontEnd.GetActiveScreen()) return true;

    // Make sure keyboard is not up
    if (UseCase::IsScreenActive<ScreenKeyboard>()) return true;

    ScreenEORRecoverAccount *pScreen = UseCase::GetActiveScreen<ScreenEORRecoverAccount>();
	if ( !pScreen ) return true ;

    EORNewAccountErrorCodes errorCode;

    switch (pScreen->GetScreenStatus())
    {
		case ScreenEORRecoverAccount::ENTER_NAME:
			UseCase::ActivateKeyboardForNames( pScreen->getName() );
			break;
		case ScreenEORRecoverAccount::ENTER_PASSWORD:
			UseCase::ActivateKeyboardForPasswords( "" );
			break;

        case ScreenEORRecoverAccount::BACK:
            stop();
            break;

        case ScreenEORRecoverAccount::RECOVER:

            errorCode = _verifyScreen1();
            if (errorCode != ERROR_NONE)
            {
                DisplayDialog::getSingletonPtr()->start("Invalid input", EORNewAccountErrorStringTable[errorCode]);
                return true;
            }
            else
            {
                // Make sure the account isn't already on the memory card
                for (int i = 0; i < MAX_ONLINE_ACCOUNTS; ++i)
                {
                    if (g_GlobalProfile.OnlineAccounts[i].InUse)
                    {
                        if (strcmp(g_GlobalProfile.OnlineAccounts[i].AccountName, _onlineName) == 0)
                        {
                            DisplayDialog::getSingletonPtr()->start("Recover Account Failed",
								"The account name specified already exists in the loaded profile.");
                            return true;
                        }
                    }
                }

                // Recover the account
                _stateMachine.setCurrentState(this, WAIT_FOR_RECOVER);
            }
            break;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the WAIT_FOR_RECOVER state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORRecoverAccount::_updateWait()
{
    // Save the account and stop
    _recoverAccount();
	_stateMachine.setCurrentState( this, SHUT_DOWN );

	DisplayDialog::getSingletonPtr()->start( "Recovery attempt", "Your account data will now be used to attempt to login.  After a successful login the recovery process will be complete." );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called to verify values on the first EOR create account screen.
///
/// \return The error code (ERROR_NONE for no error)
///////////////////////////////////////////////////////////////////////////////////////////////////
EORNewAccountErrorCodes OnlineUI::EORRecoverAccount::_verifyScreen1()
{
    ScreenEORRecoverAccount *pScreen = UseCase::GetActiveScreen<ScreenEORRecoverAccount>();
	if ( !pScreen ) return ERROR_NONE ;

    // Copy data over from screen
    OSIstrcpy(_onlineName, pScreen->getName());
    OSIstrcpy(_password, pScreen->getPassword());
    _savePassword = pScreen->getSavePassword();

    // Put a correct email address in, since we don't force the user to recover one.
    return EORAccount::validateScreen1(_onlineName, _password, _password, "email@example.com");
}

///////////////////////////////////////////////////////////////////////////////////////
/// Recover the current account out to the profile group but not the memory card.
///////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::EORRecoverAccount::_recoverAccount()
{
    int profileNum = MAX_ONLINE_ACCOUNTS;
    int i;

    // Find the first unused profile
    for (i = 0; i < MAX_ONLINE_ACCOUNTS; ++i)
    {
        if (!g_GlobalProfile.OnlineAccounts[i].InUse)
        {
            profileNum = i;
            break;
        }
    }

    // We should have found a profile, as the recovery screen should not have
    // been brought up if the profiles were full.
    ASSERT(profileNum < MAX_ONLINE_ACCOUNTS);

    // Fill out the profile
    OSIstrcpy(g_GlobalProfile.OnlineAccounts[profileNum].AccountName, _onlineName);
    if (_savePassword)
    {
        OSIstrcpy(g_GlobalProfile.OnlineAccounts[profileNum].Password, _password);
    }
    else
    {
        OSIstrcpy(g_GlobalProfile.OnlineAccounts[profileNum].Password, "");
    }
    OSIstrcpy(g_GlobalProfile.OnlineAccounts[profileNum].Email, "");
    g_GlobalProfile.OnlineAccounts[profileNum].InUse = true;
    g_GlobalProfile.OnlineAccounts[profileNum].SavePassword = _savePassword;

    // This information comes from the server after connecting now, set some defaults for now...
    g_GlobalProfile.OnlineAccounts[profileNum].EidosPartners = true;
    g_GlobalProfile.OnlineAccounts[profileNum].EidosPromos = true;
    g_GlobalProfile.OnlineAccounts[profileNum].BirthMonth = EORAccount::JANUARY;
    g_GlobalProfile.OnlineAccounts[profileNum].BirthDay = 1;
    g_GlobalProfile.OnlineAccounts[profileNum].BirthYear = 1987;
    g_GlobalProfile.OnlineAccounts[profileNum].Gender = true;
	g_GlobalProfile.OnlineAccounts[profileNum].Country = EORAccount::getCountryCode( EORAccount::UNITED_STATES );
    g_GlobalProfile.OnlineAccounts[profileNum].ZipCode[0] = 0;
    g_GlobalProfile.OnlineAccounts[profileNum].Changed = false;

    _recoveredAccount = profileNum;
	g_GlobalProfile.CurrentAccount = _recoveredAccount;
}

///////////////////////////////////////////////////////////////////////////////////////
/// Stop the use case.
///////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORRecoverAccount::_updateShutDown()
{
	stop();
	return true;
}
