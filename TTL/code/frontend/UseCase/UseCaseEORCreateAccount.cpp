///////////////////////////////////////////////////////////////////////////////////////
///  \file UseCaseMainMenu.cpp
///
///  \brief Implementation of the MainMenu class, which is responsible 
/// for presenting the Main Menu and allowing the player to pick what
/// they want to do.
///////////////////////////////////////////////////////////////////////////////////////

#include "TTLPCH.h"

#include "UseCaseEORCreateAccount.hpp"
#include "UseCaseDisplayDialog.hpp"
#include "UseCaseWaitForOSIRequest.hpp"
#include "frontend/frontend.h"
#include "frontend/savegame.h"
#include "../screens/screen_EORCreateAccount1.h"
#include "../screens/screen_EORCreateAccount2.h"
#include "../screens/screen_wait.h"
#include "../screens/screen_CountryList.h"
#include "platform/MemoryCard/MemoryCard.h"
#include "UseCaseStrings.hpp"
#include "UseCaseYesNoDialog.hpp"
#include "EORAccount.hpp"
#include "UseCaseUtility.hpp"
#include "main/NetMonitor.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor: Initialize member variables
///////////////////////////////////////////////////////////////////////////////////////////////////
OnlineUI::EORCreateAccount::EORCreateAccount() : 
    _stateMachine(CREATE1)
{
    _stateMachine.setStateUpdate(CREATE1,         &OnlineUI::EORCreateAccount::_updateCreate1);
    _stateMachine.setStateUpdate(CREATE2,         &OnlineUI::EORCreateAccount::_updateCreate2);
    _stateMachine.setStateUpdate(WAIT_FOR_CREATE, &OnlineUI::EORCreateAccount::_updateWait);
    _stateMachine.setStateEnter(WAIT_FOR_CREATE,  &OnlineUI::EORCreateAccount::_enterWait);
    _stateMachine.setStateUpdate(VERIFY_CREATE,   &OnlineUI::EORCreateAccount::_updateVerifyCreate);
    _stateMachine.setStateEnter(VERIFY_CREATE,    &OnlineUI::EORCreateAccount::_enterVerifyCreate);
    _stateMachine.setStateEnter(SAVE_ACCOUNT,     &OnlineUI::EORCreateAccount::_enterSave);
    _stateMachine.setStateUpdate(SAVE_ACCOUNT,    &OnlineUI::EORCreateAccount::_updateSave);
    _stateMachine.setStateUpdate(SHUT_DOWN,       &OnlineUI::EORCreateAccount::_updateShutDown);

	_createdAccount = false;
	_accountOffset = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called before use case is started.  Show the first screen for the use case.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::EORCreateAccount::_init(void)
{
    g_FrontEnd.GotoScreen("SCREEN_ID_CREATE_5_00A");

    ScreenEORCreateAccount1 *pScreen = UseCase::GetActiveScreen<ScreenEORCreateAccount1>();
	if ( !pScreen ) return true ;

	_stateMachine.setCurrentState(this, CREATE1);
	pScreen->setName("");
    pScreen->setPassword("");
    pScreen->setPasswordConfirm("");
    pScreen->setSavePassword(true);
    pScreen->setEmail("");
    pScreen->setPromos(true);
    pScreen->setShareInfo(true);

    
    _promos = true;
    _shareInfo = true;
    _onlineName[0] = 0;
    _password[0] = 0;
    _passwordConfirm[0] = 0;
    _emailAddress[0] = 0;
    _errorString[0] = 0;

	_isFirstTimeThrough = true;
	_createdAccount = false;

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called after use case is ended.  Clean up the screens and other objects.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::EORCreateAccount::_deinit(void)
{
 	g_CNetMonitor.Enable( true );

	g_FrontEnd.RemoveAllPopups();
	if( UseCase::IsScreenActive< ScreenEORCreateAccount2 >() )
	{
		g_FrontEnd.EndScreen( 0 );
	}
	g_FrontEnd.PopScreenHistory( "SCREEN_ID_CREATE_5_00B" );

	if( UseCase::IsScreenActive< ScreenEORCreateAccount1 >() )
	{
		g_FrontEnd.EndScreen( 0 );
	}
	g_FrontEnd.PopScreenHistory( "SCREEN_ID_CREATE_5_00A" );
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the CREATE1 state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORCreateAccount::_updateCreate1()
{
    if (!g_FrontEnd.GetActiveScreen()) return true;

    // Make sure keyboard is not up
    if (UseCase::IsScreenActive<ScreenKeyboard>()) return true;

    ScreenEORCreateAccount1 *pScreen = UseCase::GetActiveScreen<ScreenEORCreateAccount1>();
	if ( !pScreen ) return true ;

    EORNewAccountErrorCodes errorCode;
    switch (pScreen->GetScreenStatus())
    {
		case ScreenEORCreateAccount1::ENTER_NAME:
			UseCase::ActivateKeyboardForNames( pScreen->getName() );
			break;
		case ScreenEORCreateAccount1::ENTER_PASSWORD:
			UseCase::ActivateKeyboardForPasswords( "" );
			break;
		case ScreenEORCreateAccount1::ENTER_CONFIRM_PASSWORD:
			UseCase::ActivateKeyboardForPasswords( "" );
			break;
		case ScreenEORCreateAccount1::ENTER_EMAIL:
			UseCase::ActivateKeyboardForEmail( pScreen->getEmail(), OSI_EMAIL_LENGTH );
			break;

        case ScreenEORCreateAccount1::BACK:
			stop();
            break;

        case ScreenEORCreateAccount1::CONTINUE:
            errorCode = _verifyScreen1();
            if (errorCode == ERROR_NONE)
            {
                _stateMachine.setCurrentState(this, CREATE2);
                g_FrontEnd.GotoScreen("SCREEN_ID_CREATE_5_00B");
				ScreenEORCreateAccount2* pScreen = UseCase::GetActiveScreen<ScreenEORCreateAccount2>();
				if ( !pScreen ) return true ;
				pScreen->setUsing( !_isFirstTimeThrough );
				_isFirstTimeThrough = false;
            }
            else
            {
                DisplayDialog::getSingletonPtr()->start("Invalid input", EORNewAccountErrorStringTable[errorCode]);
            }
            break;
    }

    return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the CREATE2 state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORCreateAccount::_updateCreate2()
{
    static int newCountry = -1;

    if (!g_FrontEnd.GetActiveScreen()) return true;

    // Make sure keyboard is not up
    if (UseCase::IsScreenActive<ScreenKeyboard>()) return true;

    // If the country list is up, see if we're done with it yet
    if (UseCase::IsScreenActive<ScreenCountryList>())
    {
        ScreenCountryList *pCountryList = UseCase::GetActiveScreen<ScreenCountryList>();

        switch (pCountryList->GetStatus())
        {
            case ScreenCountryList::DONE:
                newCountry = pCountryList->GetSelectedCountry();
                g_FrontEnd.EndPopup(0);
                break;

            case ScreenCountryList::CANCEL:
                g_FrontEnd.EndPopup(0);
                break;
        }

        return true;
    }

    ScreenEORCreateAccount2 *pScreen = UseCase::GetActiveScreen<ScreenEORCreateAccount2>();
	if ( !pScreen ) return true ;

    EORNewAccountErrorCodes errorCode;
	ScreenKeyboard* pKeyboard = NULL;
    char tempBuffer[500];

    // If we have a new country, set it
    if (newCountry != -1)
    {
        pScreen->setCountry(newCountry);
        newCountry = -1;
    }

    switch (pScreen->GetScreenStatus())
    {
		case ScreenEORCreateAccount2::ENTER_YEAR:
            if (pScreen->getYear() == 0)
            {
                tempBuffer[0] = 0;
            }
            else
            {
                sprintf(tempBuffer, "%.4d", pScreen->getYear());
            }
			pKeyboard = UseCase::ActivateKeyboard(tempBuffer, 4);
			// restrict keyboard to only numbers '0' - '9'
			if(pKeyboard != NULL) {
				pKeyboard->RestrictKeys(true, 0, '0'-1);
				pKeyboard->RestrictKeys(true, '9'+1, 255);
			}
			break;

		case ScreenEORCreateAccount2::ENTER_ZIP:
			UseCase::ActivateKeyboardForPostalCode( pScreen->getZip() );
			break;

        case ScreenEORCreateAccount2::COUNTRY:
            g_FrontEnd.ShowPopup("SCREEN_ID_COUNTRY_LIST");
            UseCase::GetActiveScreen<ScreenCountryList>()->SetSelectedCountry(pScreen->getCountry());
            break;

        case ScreenEORCreateAccount2::BACK:
            _stateMachine.setCurrentState(this, CREATE1);
            g_FrontEnd.EndScreen(0);
            break;

        case ScreenEORCreateAccount2::CREATE_ACCOUNT:
            errorCode = _verifyScreen2();
            if (errorCode == ERROR_NONE)
            {
                _stateMachine.setCurrentState(this, VERIFY_CREATE);
            }
            else
            {
                DisplayDialog::getSingletonPtr()->start("Invalid input", EORNewAccountErrorStringTable[errorCode]);
            }
            break;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////
/// Bring up a yes-no dialog to see if the user wants to send their sensitive
/// information over the network (TRC 1.11)
///////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::EORCreateAccount::_enterVerifyCreate()
{
    // Ask the user if they want to download the patch
    YesNoDialog::getSingletonPtr()->start("Warning", "You are about to send this information over "
                                                     "the network.  Are you sure?");
}

///////////////////////////////////////////////////////////////////////////////////////
/// Check to see if the user has verified sending their information over the
/// network.  If so, create the account.  If not, go back.
///////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORCreateAccount::_updateVerifyCreate()
{
    // Check return result from dialog
    if (YesNoDialog::YES == YesNoDialog::getSingletonPtr()->GetAnswer())
    {
        // Create the account
        _stateMachine.setCurrentState(this, WAIT_FOR_CREATE);
    }
    else
    {
        _stateMachine.setCurrentState(this, CREATE2);
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called when entering the WAIT_FOR_CREATE state.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::EORCreateAccount::_enterWait()
{
    // Build the OSIAccount to be passed to OSI
	_newAccount.flags = 0;
	_newAccount.birthdayDay = _birthDay;
	_newAccount.birthdayMonth = _birthMonth;
	_newAccount.birthdayYear = _birthYear;
    _newAccount.countryCode = EORAccount::getCountryCode(_country);
	OSIstrcpy(_newAccount.email, _emailAddress);
	OSIstrcpy(_newAccount.password, _password);
	OSIstrcpy(_newAccount.player.name, _onlineName);
	OSIstrncpy(_newAccount.postalCode, _zipCode, NUM_ARRAY_ELEMENTS(_zipCode));
	if ( _gender ) _newAccount.flags |= FLAG_GENDER;
	if ( _promos ) _newAccount.flags |= FLAG_PROMOS;
	if ( _shareInfo ) _newAccount.flags |= FLAG_SHARE_INFO;
	if ( _savePassword ) _newAccount.flags |= FLAG_SAVE_PWD;

 	g_CNetMonitor.Enable( false );
    bool success = OSI::getSingletonPtr()->requestCreateAccount(_newAccount);

    WaitForOSIRequest::getSingletonPtr()->start(success, "Creating Account", "Please wait");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the WAIT_FOR_CREATE state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORCreateAccount::_updateWait()
{
	OSIRequestStatus requestStatus = OSI::getSingletonPtr()->getLastRequestStatus();
 	g_CNetMonitor.Enable( true );

	if (requestStatus == STATUS_SUCCEEDED)
	{
		_createdAccount = true;
        _stateMachine.setCurrentState(this, SAVE_ACCOUNT);
		DisplayDialog::getSingletonPtr()->start( "Success", "Your account was successfully created on the server." );
	}
	else
	{
		_stateMachine.setCurrentState( this, CREATE2 );
	}

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////
/// Save the current account out to the profile group and to the memory card.
///////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::EORCreateAccount::_enterSave()
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

    // We should have found a profile, as the create screen should not have
    // been brought up if the profiles were full.
    ASSERT(profileNum < MAX_ONLINE_ACCOUNTS);

    // Fill out the profile
    g_GlobalProfile.OnlineAccounts[profileNum].InUse = true;
    OSIstrcpy(g_GlobalProfile.OnlineAccounts[profileNum].AccountName, _onlineName);
    if (_savePassword)
    {
        OSIstrcpy(g_GlobalProfile.OnlineAccounts[profileNum].Password, _password);
    }
    else
    {
        OSIstrcpy(g_GlobalProfile.OnlineAccounts[profileNum].Password, "");
    }
    g_GlobalProfile.OnlineAccounts[profileNum].SavePassword = _savePassword;
    g_GlobalProfile.OnlineAccounts[profileNum].EidosPartners = _shareInfo;
    g_GlobalProfile.OnlineAccounts[profileNum].EidosPromos = _promos;
    OSIstrcpy(g_GlobalProfile.OnlineAccounts[profileNum].Email, _emailAddress);

    g_GlobalProfile.OnlineAccounts[profileNum].BirthMonth = _birthMonth;
    g_GlobalProfile.OnlineAccounts[profileNum].BirthDay = _birthDay;
    g_GlobalProfile.OnlineAccounts[profileNum].BirthYear = _birthYear;
    g_GlobalProfile.OnlineAccounts[profileNum].Gender = _gender;
    g_GlobalProfile.OnlineAccounts[profileNum].Country = EORAccount::getCountryCode(_country);
    OSIstrncpy( g_GlobalProfile.OnlineAccounts[profileNum].ZipCode, _zipCode, NUM_ARRAY_ELEMENTS( _zipCode ) );

    _accountOffset = profileNum;

    EORAccount::saveAccounts();
}

///////////////////////////////////////////////////////////////////////////////////////
/// Wait until the memory card is done saving
///
/// \return True to continue updating use case, false to return to main menu
///////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EORCreateAccount::_updateSave()
{
    if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_IDLE)
	{
		g_MemCardManager->ResetLoadSaveBuffer();

		const int memCardPopupResult = g_MemCardManager->GetPopupExitResult();
		if (memCardPopupResult != PR_COMPLETED_OK)
		{
			// User has canceled save, or save failed
			// We need to send this info back to the use case so I'm going
			// to overload the Changed flag to indicate the account wasn't saved.
			// I don't like this, but at this point I don't care.
			g_GlobalProfile.OnlineAccounts[_accountOffset].Changed = true;

			char warningMsg[256] = {0};
#ifdef PS2
			sprintf( warningMsg, "Even though your account was not saved to the "
								 "memory card (8MB) (for PlayStation®2) it was still successfully created on the server.  "
								 "If you want to save it to the memory card (8MB) (for PlayStation®2) in the future "
								 "you will need to recover this account." );
#elif defined DIRECTX_PC
			sprintf( warningMsg, "Even though your account was not saved to disk "
								 "it was still successfully created on the server.  "
								 "If you want to use this account in the future "
								 "you will need to recover it." );
#endif
			DisplayDialog::getSingletonPtr()->start( "Warning", warningMsg );
		}
		else
		{
			g_GlobalProfile.OnlineAccounts[_accountOffset].Changed = false;
		}

		// Put password back in account so we can log in without prompting for password
		OSIstrcpy(g_GlobalProfile.OnlineAccounts[_accountOffset].Password, _password);
		g_GlobalProfile.CurrentAccount = _accountOffset;

        _stateMachine.setCurrentState( this, SHUT_DOWN );
	}

    return true;
}

bool OnlineUI::EORCreateAccount::_updateShutDown()
{
	stop();
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called to verify values on the first EOR create account screen.
///
/// \return The error code (ERROR_NONE for no error)
///////////////////////////////////////////////////////////////////////////////////////////////////
EORNewAccountErrorCodes OnlineUI::EORCreateAccount::_verifyScreen1()
{
    ScreenEORCreateAccount1 *pScreen = UseCase::GetActiveScreen<ScreenEORCreateAccount1>();
	if ( !pScreen ) return ERROR_NONE ;

    // Copy data over from screen
    OSIstrcpy(_onlineName, pScreen->getName());
    OSIstrcpy(_password, pScreen->getPassword());
    OSIstrcpy(_passwordConfirm, pScreen->getPasswordConfirm());
    OSIstrcpy(_emailAddress, pScreen->getEmail());
    _savePassword = pScreen->getSavePassword();
    _promos = pScreen->getPromos();
    _shareInfo = pScreen->getShareInfo();

    return EORAccount::validateScreen1(_onlineName, _password, _passwordConfirm, _emailAddress);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called to verify values on the second EOR create account screen.
///
/// \return The error code (ERROR_NONE for no error)
///////////////////////////////////////////////////////////////////////////////////////////////////
EORNewAccountErrorCodes OnlineUI::EORCreateAccount::_verifyScreen2()
{
    ScreenEORCreateAccount2 *pScreen = UseCase::GetActiveScreen<ScreenEORCreateAccount2>();
	if ( !pScreen ) return ERROR_NONE ;

    // Get data from screen
    _birthMonth = pScreen->getMonth();
    _birthDay = pScreen->getDay();
    _birthYear = pScreen->getYear();
    _gender = pScreen->isMale();
    _country = pScreen->getCountry();
    OSIstrncpy( _zipCode, pScreen->getZip(), NUM_ARRAY_ELEMENTS( _zipCode ) );

    // Validate data
    return EORAccount::validateScreen2(_birthMonth, _birthDay, _birthYear, _zipCode, EORAccount::getCountryCode(_country));
}