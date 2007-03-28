#if defined(NETWORKING)
#include "TTLPCH.h"

#if defined(PS2)
#include <libcdvd.h>
#endif

#include "UseCaseEOREditAccount.hpp"
#include "UseCaseDisplayDialog.hpp"
#include "UseCaseWaitForOSIRequest.hpp"
#include "frontend/frontend.h"
#include "frontend/savegame.h"
#include "../screens/screen_EOREditAccount1.h"
#include "../screens/screen_EOREditAccount2.h"
#include "../screens/screen_wait.h"
#include "../screens/screen_CountryList.h"
#include "platform/MemoryCard/MemoryCard.h"
#include "UseCaseStrings.hpp"
#include "data/screens/EOR.h"
#include "EORAccount.hpp"
#include "UseCaseUtility.hpp"
#include "main/NetMonitor.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor: Initialize member variables
///////////////////////////////////////////////////////////////////////////////////////////////////
OnlineUI::EOREditAccount::EOREditAccount() : 
    _stateMachine(EDIT1),
    _promos(true),
    _shareInfo(true)
{
    _stateMachine.setStateUpdate(EDIT1,         &OnlineUI::EOREditAccount::_updateEdit1);
    _stateMachine.setStateUpdate(EDIT2,         &OnlineUI::EOREditAccount::_updateEdit2);
    _stateMachine.setStateEnter(WAIT_FOR_SAVE,  &OnlineUI::EOREditAccount::_enterWait);
    _stateMachine.setStateUpdate(WAIT_FOR_SAVE, &OnlineUI::EOREditAccount::_updateWait);
    _stateMachine.setStateEnter(SAVE_ACCOUNT,   &OnlineUI::EOREditAccount::_enterSave);
    _stateMachine.setStateUpdate(SAVE_ACCOUNT,  &OnlineUI::EOREditAccount::_updateSave);
	_stateMachine.setStateUpdate(SHUT_DOWN,     &OnlineUI::EOREditAccount::_updateShutDown);

    _onlineName[0] = 0;
	_oldPassword[0] = 0;
    _newPassword[0] = 0;
    _newPasswordConfirm[0] = 0;
    _emailAddress[0] = 0;
    _errorString[0] = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Show the dialog and use the given account information
///
/// \param account The account to be edited
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::EOREditAccount::start(ts_OnlineAccount *accountPtr)
{
    _accountPtr = accountPtr;

	_isFirstTimeThrough = true;

    UseCase::start();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called before use case is started.  Show the first screen for the use case.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::EOREditAccount::_init(void)
{
    g_FrontEnd.GotoScreen("SCREEN_ID_EDIT_A");

    // Set data on the screen
    _populateScreen1();

	_stateMachine.setCurrentState(this, EDIT1);

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called after use case is ended.  Clean up the screens and other objects.
///
/// \return true if succeeded, false otherwise
///////////////////////////////////////////////////////////////////////////////////////////////////
/*virtual*/ bool OnlineUI::EOREditAccount::_deinit(void)
{
 	g_CNetMonitor.Enable( true );

	g_FrontEnd.RemoveAllPopups();
	if( UseCase::IsScreenActive< ScreenEOREditAccount2 >() )
	{
		g_FrontEnd.EndScreen(0);
	}
	g_FrontEnd.PopScreenHistory( "SCREEN_ID_EDIT_B" );

	if( UseCase::IsScreenActive< ScreenEOREditAccount1 >() )
	{
		g_FrontEnd.EndScreen(0);
	}
	g_FrontEnd.PopScreenHistory( "SCREEN_ID_EDIT_A" );
	
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the EDIT1 state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EOREditAccount::_updateEdit1()
{
    if (!g_FrontEnd.GetActiveScreenHandler()) return true;

    // Make sure keyboard is not up
    if (UseCase::IsScreenActive<ScreenKeyboard>()) return true;

    ScreenEOREditAccount1 *pScreen = UseCase::GetActiveScreen<ScreenEOREditAccount1>();
	if ( !pScreen ) return true ;

    EORNewAccountErrorCodes errorCode;

    switch (pScreen->GetScreenStatus())
    {
		case ScreenEOREditAccount1::ENTER_NAME:
			UseCase::ActivateKeyboardForNames( pScreen->getName() );
			break;
		case ScreenEOREditAccount1::ENTER_OLD_PASSWORD:
			UseCase::ActivateKeyboardForPasswords( "" );
			break;
		case ScreenEOREditAccount1::ENTER_NEW_PASSWORD:
			UseCase::ActivateKeyboardForPasswords( "" );
			break;
		case ScreenEOREditAccount1::ENTER_NEW_CONFIRM_PASSWORD:
			UseCase::ActivateKeyboardForPasswords( "" );
			break;
		case ScreenEOREditAccount1::ENTER_EMAIL:
			UseCase::ActivateKeyboardForEmail( pScreen->getEmail(), OSI_EMAIL_LENGTH );
			break;

        case ScreenEOREditAccount1::BACK:
            _stateMachine.setCurrentState( this, SHUT_DOWN );
            break;

        case ScreenEOREditAccount1::CONTINUE:
            errorCode = _verifyScreen1();
            if (errorCode == ERROR_NONE)
            {
                _stateMachine.setCurrentState(this, EDIT2);
                g_FrontEnd.GotoScreen("SCREEN_ID_EDIT_B");

                // Set data on the screen
                _populateScreen2();
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
/// Called every frame to update the use case when in the EDIT2 state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EOREditAccount::_updateEdit2()
{
    static int newCountry = -1;

    if (!g_FrontEnd.GetActiveScreenHandler()) return true;

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

    ScreenEOREditAccount2 *pScreen = UseCase::GetActiveScreen<ScreenEOREditAccount2>();
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
		case ScreenEOREditAccount2::ENTER_YEAR:
            sprintf(tempBuffer, "%.4d", pScreen->getYear());
			pKeyboard = UseCase::ActivateKeyboard(tempBuffer, 4);
			// restrict keyboard to only numbers '0' - '9'
			if(pKeyboard != NULL) {
				pKeyboard->RestrictKeys(true, 0, '0'-1);
				pKeyboard->RestrictKeys(true, '9'+1, 255);
			}
			break;

		case ScreenEOREditAccount2::ENTER_ZIP:
			UseCase::ActivateKeyboardForPostalCode( pScreen->getZip() );
			break;

        case ScreenEOREditAccount2::COUNTRY:
            g_FrontEnd.ShowPopup("SCREEN_ID_COUNTRY_LIST");
            UseCase::GetActiveScreen<ScreenCountryList>()->SetSelectedCountry(EORAccount::getCountryOffset(_accountPtr->Country));
            break;

        case ScreenEOREditAccount2::BACK:
            {
                _stateMachine.setCurrentState(this, EDIT1);
                g_FrontEnd.EndScreen(0);
            }
            break;

        case ScreenEOREditAccount2::SAVE_ACCOUNT:
            errorCode = _verifyScreen2();
            if (errorCode == ERROR_NONE)
            {
                _stateMachine.setCurrentState(this, WAIT_FOR_SAVE);
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
/// Called when entering the WAIT_FOR_SAVE state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::EOREditAccount::_enterWait()
{
    // Build the OSIAccount to be passed to OSI
	_accountData.flags = 0;
	_accountData.birthdayDay = _birthDay;
	_accountData.birthdayMonth = _birthMonth;
	_accountData.birthdayYear = _birthYear;
	_accountData.countryCode = EORAccount::getCountryCode(_country);
	OSIstrcpy(_accountData.email, _emailAddress);
	if( _newPassword[0] )
		OSIstrcpy(_accountData.password, _newPassword);
	else
		OSIstrcpy(_accountData.password, _oldPassword);
	OSIstrcpy(_accountData.player.name, _onlineName);
	OSIstrncpy(_accountData.postalCode, _zipCode, NUM_ARRAY_ELEMENTS(_zipCode));
	if ( _gender ) _accountData.flags |= FLAG_GENDER;
	if ( _promos ) _accountData.flags |= FLAG_PROMOS;
	if ( _shareInfo ) _accountData.flags |= FLAG_SHARE_INFO;
	if ( _savePassword ) _accountData.flags |= FLAG_SAVE_PWD;

 	g_CNetMonitor.Enable( false );
    bool success = OSI::getSingletonPtr()->requestEditAccount(_accountPtr->AccountName, _oldPassword, _accountData);

    WaitForOSIRequest::getSingletonPtr()->start(success, "Editing Account", "Please wait");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called every frame to update the use case when in the WAIT_FOR_SAVE state.
///
/// \return true to continue use case processing, false to return to application main menu.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EOREditAccount::_updateWait()
{
 	g_CNetMonitor.Enable( true );
	OSIRequestStatus requestStatus = OSI::getSingletonPtr()->getLastRequestStatus();
	if (requestStatus == STATUS_SUCCEEDED)
	{
        _stateMachine.setCurrentState(this, SAVE_ACCOUNT);
		return true;
	}
	else
	{
		_stateMachine.setCurrentState(this, SHUT_DOWN);
		return true;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called to verify values on the first EOR create account screen.
///
/// \return The error code (ERROR_NONE for no error)
///////////////////////////////////////////////////////////////////////////////////////////////////
EORNewAccountErrorCodes OnlineUI::EOREditAccount::_verifyScreen1()
{
    ScreenEOREditAccount1 *pScreen = UseCase::GetActiveScreen<ScreenEOREditAccount1>();
	if ( !pScreen ) return ERROR_NONE ;

    // Copy data over from screen
    OSIstrcpy(_onlineName, pScreen->getName());
	OSIstrcpy(_oldPassword, pScreen->getOldPassword());
    OSIstrcpy(_newPassword, pScreen->getNewPassword());
    OSIstrcpy(_newPasswordConfirm, pScreen->getNewPasswordConfirm());
    OSIstrcpy(_emailAddress, pScreen->getEmail());
    _savePassword = pScreen->getSavePassword();
    _promos = pScreen->getPromos();
    _shareInfo = pScreen->getShareInfo();

    return EORAccount::validateEditScreen1(_onlineName, _oldPassword, _newPassword, _newPasswordConfirm, _emailAddress);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Called to verify values on the second EOR create account screen.
///
/// \return The error code (ERROR_NONE for no error)
///////////////////////////////////////////////////////////////////////////////////////////////////
EORNewAccountErrorCodes OnlineUI::EOREditAccount::_verifyScreen2()
{
    ScreenEOREditAccount2 *pScreen = UseCase::GetActiveScreen<ScreenEOREditAccount2>();
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

///////////////////////////////////////////////////////////////////////////////////////
/// Set all of the elements in edit account screen 1 to their proper values
///////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::EOREditAccount::_populateScreen1()
{
    // Set data on screen 1
    ScreenEOREditAccount1 *pScreen1 = UseCase::GetActiveScreen<ScreenEOREditAccount1>();
	if ( !pScreen1 ) return ;

    pScreen1->setName(_accountPtr->AccountName);
    pScreen1->setOldPassword(_accountPtr->Password);
	pScreen1->setNewPassword("");
    pScreen1->setNewPasswordConfirm("");
    pScreen1->setSavePassword(_accountPtr->SavePassword);
    pScreen1->setEmail(_accountPtr->Email);
    pScreen1->setPromos(_accountPtr->EidosPromos);
    pScreen1->setShareInfo(_accountPtr->EidosPartners);
}

///////////////////////////////////////////////////////////////////////////////////////
/// Set all of the elements in edit account screen 2 to their proper values
///////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::EOREditAccount::_populateScreen2()
{
    // Set data on the screen
    ScreenEOREditAccount2 *pScreen2 = UseCase::GetActiveScreen<ScreenEOREditAccount2>();
	if ( !pScreen2 ) return ;

	if( _isFirstTimeThrough )
	{
		pScreen2->setMale(_accountPtr->Gender);
		pScreen2->setMonth(_accountPtr->BirthMonth);
		pScreen2->setCountry(EORAccount::getCountryOffset(_accountPtr->Country));
		pScreen2->setDay(_accountPtr->BirthDay);
		pScreen2->setYear(_accountPtr->BirthYear);
		pScreen2->setZip(_accountPtr->ZipCode);
		_isFirstTimeThrough = false;
	}
}

///////////////////////////////////////////////////////////////////////////////////////
/// Save the current account out to the profile group and to the memory card.
///////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::EOREditAccount::_enterSave()
{
    // Fill out our account
    _accountPtr->InUse = true;
    OSIstrcpy(_accountPtr->AccountName, _onlineName);
    if (_savePassword)
    {
		if( _newPassword[0] )
			OSIstrcpy(_accountPtr->Password, _newPassword);
		else
			OSIstrcpy(_accountPtr->Password, _oldPassword);
    }
    else
    {
        OSIstrcpy(_accountPtr->Password, "");
    }
    _accountPtr->EidosPartners = _shareInfo;
    _accountPtr->EidosPromos = _promos;
    _accountPtr->BirthDay = _birthDay;
    _accountPtr->BirthMonth = _birthMonth;
    _accountPtr->BirthYear = _birthYear;
    _accountPtr->Country = EORAccount::getCountryCode(_country);
    _accountPtr->Gender = _gender;
    _accountPtr->InUse = true;
    OSIstrncpy( _accountPtr->ZipCode, _zipCode, NUM_ARRAY_ELEMENTS( _zipCode ) );
    _accountPtr->Changed = true;
    _accountPtr->SavePassword = _savePassword;
    OSIstrcpy(_accountPtr->Email, _emailAddress);

    EORAccount::saveAccounts();
}

///////////////////////////////////////////////////////////////////////////////////////
/// Wait until the memory card is done saving
///
/// \return True to continue updating use case, false to return to main menu
///////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EOREditAccount::_updateSave()
{
    if (g_MemCardManager->GetState() == g_MemCardManager->CMEMORYCARD_IDLE)
	{
		g_MemCardManager->ResetLoadSaveBuffer();

		const int memCardPopupResult = g_MemCardManager->GetPopupExitResult();
		if (memCardPopupResult != PR_COMPLETED_OK)
		{
			// User has canceled save or save failed, warn user
			_accountPtr->InUse = true;
			_accountPtr->Changed = false;

			char warningMsg[512] = {0};
#ifdef PS2
			sprintf( warningMsg, "Your account which you edited was not saved to "
								 "the memory card (8MB) (for PlayStation®2) but your changes "
								 "were saved on the server.  This may cause problems and it is "
								 "highly recommended you save your changes to the memory "
								 "card (8MB) (for PlayStation®2).  Please edit your account again "
								 "to save it to the memory card (8MB) (for PlayStation®2)." );
#elif defined DIRECTX_PC
			sprintf( warningMsg, "Your account which you edited was not saved to "
								 "disk but your changes were saved on the server.  "
								 "This may cause problems and it is highly recommended "
								 "you save your changes.  Please edit your account again "
								 "to save it." );
#endif
			DisplayDialog::getSingletonPtr()->start( "Warning", warningMsg );
		}
		else
		{
			DisplayDialog::getSingletonPtr()->start( "Success", "Your account was successfully edited." );
		}

        _stateMachine.setCurrentState( this, SHUT_DOWN );
	}

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////
/// Shut down this use case
///
/// \return True to continue updating use case
///////////////////////////////////////////////////////////////////////////////////////
bool OnlineUI::EOREditAccount::_updateShutDown()
{
	stop();
	return true;
}
#endif