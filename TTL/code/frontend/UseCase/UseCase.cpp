///////////////////////////////////////////////////////////////////////////////////////
///  \file UseCase.hpp
///  \brief Defines the base UseCase class
///  which provides common interface for running use cases.
///////////////////////////////////////////////////////////////////////////////////////

#include "TTLPCH.h"
#include "UseCase.hpp"
#include "Game/Managers/SpriteMgr.h"

#include "UseCaseClans.hpp"
#include "UseCaseDisplayDialog.hpp"

#ifdef PS2
#include "UseCaseEORConfig.hpp"
#endif

#if defined( PS2 ) || defined( DIRECTX_PC )
#include "UseCaseEOA.hpp"
#include "UseCaseEORConnect.hpp"
#include "UseCaseEORCreateAccount.hpp"
#include "UseCaseEORDeleteAccount.hpp"
#include "UseCaseEOREditAccount.hpp"
#include "UseCaseEORLogin.hpp"
#include "UseCaseEORPassword.hpp"
#include "UseCaseEORRecoverAccount.hpp"
#include "UseCaseFriends.hpp"
#include "UseCasePlayers.hpp"
#include "UseCaseInvitePlayerToMatch.hpp"
#endif 

#include "UseCaseFindMatch.hpp"
#include "UseCaseHostMatch.hpp"
#include "UseCaseJoinMatch.hpp"
#include "UseCaseMultiplayerMenu.hpp"
#include "UseCaseQuickMatch.hpp"
#include "UseCaseReadyUp.hpp"
#include "UseCaseStats.hpp"
#include "UseCaseYesNoDialog.hpp"
#include "UseCaseWaitForOSIRequest.hpp"

#ifdef _XBOX
#include "Xbox/UseCaseUIXFriends.hpp"
#include "Xbox/UseCaseUIXLogin.hpp"
#include "Xbox/UseCaseUIXPlayers.hpp"
#endif


//#include <typeinfo.h>  // To use this, RTTI needs to be enabled.

// Static data
OnlineUI::UseCaseStack OnlineUI::UseCase::_useCaseStack;
bool OnlineUI::UseCase::_allowUpdate = true;
bool OnlineUI::UseCase::_pendingNetworkErrorClose = false;

OnlineUI::UseCase::UseCase( void ):
	_running(false)
{
}

///////////////////////////////////////////////////////////////////////////////////////
/// getActiveUseCase
///////////////////////////////////////////////////////////////////////////////////////
OnlineUI::UseCase* OnlineUI::UseCase::getActiveUseCase( void )
{
    if (_useCaseStack.Items() == 0)
    {
        return 0;
    }
    else
    {
        // To use this code, RTTI needs to be enabled.
        /*OutputDebugString("Active use case is ");
        OutputDebugString(typeid(*_useCaseStack.Peek()).name());
        OutputDebugString("\n");*/
	    return _useCaseStack.Peek();
    }
}

///////////////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
	/*static*/ const char* OnlineUI::UseCase::getNameForUseCase( const UseCase* useCase )
	{
	#define RETURN_IF_USECASE_IS(_NAME_) if( useCase == _NAME_::getSingletonPtr() ) return #_NAME_

		RETURN_IF_USECASE_IS( Clans );
		RETURN_IF_USECASE_IS( DisplayDialog );

		#if defined(PS2) || defined(DIRECTX_PC)
		RETURN_IF_USECASE_IS( EOREOA );
#ifndef DIRECTX_PC
		RETURN_IF_USECASE_IS( EORConfig );
#endif
		RETURN_IF_USECASE_IS( EORConnect );
		RETURN_IF_USECASE_IS( EORCreateAccount );
		RETURN_IF_USECASE_IS( EORDeleteAccount );
		RETURN_IF_USECASE_IS( EORLogin );
		RETURN_IF_USECASE_IS( EORPassword );
		RETURN_IF_USECASE_IS( EOREditAccount );
		RETURN_IF_USECASE_IS( EORRecoverAccount );
		RETURN_IF_USECASE_IS( Friends );
		RETURN_IF_USECASE_IS( Players );
		RETURN_IF_USECASE_IS( InvitePlayerToMatch );
		#endif

		RETURN_IF_USECASE_IS( FindMatch );
		RETURN_IF_USECASE_IS( HostMatch );
		RETURN_IF_USECASE_IS( JoinMatch );
		RETURN_IF_USECASE_IS( MultiplayerMenu );
		RETURN_IF_USECASE_IS( QuickMatch );
		RETURN_IF_USECASE_IS( ReadyUp );
		RETURN_IF_USECASE_IS( Stats );
		RETURN_IF_USECASE_IS( YesNoDialog );
		RETURN_IF_USECASE_IS( WaitForOSIRequest );

		#ifdef _XBOX
		RETURN_IF_USECASE_IS( UIXFriends );
		RETURN_IF_USECASE_IS( UIXLogin );
		RETURN_IF_USECASE_IS( UIXPlayers );
		#endif

		return "UNKNOWN";

	#undef RETURN_IF_USECASE_IS
	}

	#ifdef _XBOX
	#define PRINT_DEBUG XBUtil_DebugPrint
	#else
	#define PRINT_DEBUG printf
	#endif

	#define USECASE_DEBUG_PREFIX "  [USECASE] "

	/*static*/ void OnlineUI::UseCase::dumpUseCaseStack()
	{
		PRINT_DEBUG( USECASE_DEBUG_PREFIX "STACK DUMP\n" );
		PRINT_DEBUG( USECASE_DEBUG_PREFIX "--- (front/top) ---\n" );
		int count = _useCaseStack.Items();
		while( --count >= 0 )
		{
			UseCase* usecase = _useCaseStack.Peek( count );
			PRINT_DEBUG( USECASE_DEBUG_PREFIX "--- %-2i, %s, %s\n", 
				count, 
				usecase->isRunning() ? "running" : "paused ",
				getNameForUseCase( usecase ) );
		}
		PRINT_DEBUG( USECASE_DEBUG_PREFIX "--- (back/bottom) ---\n" );
	}
#endif //_DEBUG

///////////////////////////////////////////////////////////////////////////////////////
/// Starts the use case and pushes it onto the stack.
///////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::UseCase::start( void )
{
	#ifdef _DEBUG
	PRINT_DEBUG( USECASE_DEBUG_PREFIX "(START) Calling UseCase::start on %s\n", getNameForUseCase( this ) );
	#endif // _DEBUG

	if ( !isRunning() )
	{
		setRunning( true );

		#ifdef _DEBUG
		PRINT_DEBUG( USECASE_DEBUG_PREFIX "(START) Calling UseCase::_init on %s\n", getNameForUseCase( this ) );
		#endif // _DEBUG
		const bool initSuccess = _init();
		if( initSuccess )
		{
            UseCase *useCasePtr;
            // Pause the current use case if running
            if (getActiveUseCase())
            {
				#ifdef _DEBUG
				PRINT_DEBUG( USECASE_DEBUG_PREFIX "(START) Calling UseCase::_onPause on %s\n", getNameForUseCase( getActiveUseCase() ) );
				#endif // _DEBUG
                getActiveUseCase()->_onPause();
            }

            useCasePtr = static_cast<UseCase *>(this);
			_useCaseStack.Push(useCasePtr);

			#ifdef _DEBUG
			PRINT_DEBUG( USECASE_DEBUG_PREFIX "(START) Calling UseCase::_onOpen on %s\n", getNameForUseCase( this ) );
			#endif // _DEBUG
			_onOpen();

			#ifdef _DEBUG
			PRINT_DEBUG( USECASE_DEBUG_PREFIX "(START) UseCase %s is on the stack\n", getNameForUseCase( this ) );
			dumpUseCaseStack();
			#endif // _DEBUG

		}
		else
		{
			setRunning( false );

			#ifdef _DEBUG
			ASSERTS( 0, "Usecase failed to start" );
			PRINT_DEBUG( USECASE_DEBUG_PREFIX "(START) UseCase %s failed to start\n", getNameForUseCase( this ) );
			dumpUseCaseStack();
			#endif // _DEBUG
		}

	}
	#ifdef _DEBUG
	else
	{
		PRINT_DEBUG( USECASE_DEBUG_PREFIX "(START) UseCase %s is already running\n", getNameForUseCase( this ) );
		ASSERTS( 0, "Tried to start a usecase that is already running" );
	}
	#endif // _DEBUG

}

///////////////////////////////////////////////////////////////////////////////////////
/// Stops the current use case and removes it from the
/// stack.
///////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::UseCase::stop( void )
{
	#ifdef _DEBUG
	PRINT_DEBUG( USECASE_DEBUG_PREFIX "(STOP) Calling UseCase::stop on %s\n", getNameForUseCase( this ) );
	#endif // _DEBUG

	if ( isRunning() )
	{
		#ifdef _DEBUG
		PRINT_DEBUG( USECASE_DEBUG_PREFIX "(STOP) Calling UseCase::_deinit on %s\n", getNameForUseCase( this ) );
		#endif // _DEBUG
		_deinit();
		setRunning( false );
	}
	#ifdef _DEBUG
	else
	{
		ASSERTS( 0, "Tried to stop a usecase that is not running" );
		PRINT_DEBUG( USECASE_DEBUG_PREFIX "(STOP) UseCase %s is not running\n", getNameForUseCase( this ) );
	}
	#endif // _DEBUG


	_useCaseStack.Pop();

	#ifdef _DEBUG
	PRINT_DEBUG( USECASE_DEBUG_PREFIX "(STOP) UseCase %s is off the stack\n", getNameForUseCase( this ) );
	dumpUseCaseStack();
	#endif // _DEBUG

    if (_useCaseStack.Items() > 0)
    {
	    UseCase* newTopUseCase = _useCaseStack.Peek();
	    if( newTopUseCase )
	    {
			#ifdef _DEBUG
			PRINT_DEBUG( USECASE_DEBUG_PREFIX "(STOP) Calling UseCase::_onOpen on %s\n", getNameForUseCase( newTopUseCase ) );
			#endif // _DEBUG
		    newTopUseCase->_onOpen();
	    }
    }
}

/////////////////////////////////////////////////////////////////////////////////////
/// Pops all use cases on the stack until it pops the
/// specified use case.  Passing any NULL will pop all uses case.
/////////////////////////////////////////////////////////////////////////////////////
void OnlineUI::UseCase::popThroughUseCase( UseCase* useCase )
{
	UseCase* topUseCase = getActiveUseCase();
	while ( topUseCase && (topUseCase != useCase) )
	{
		topUseCase->stop();
		topUseCase = getActiveUseCase();
	}
}


		

///////////////////////////////////////////////////////////////////////////////////////
/// Set up the keyboard and then display it
///
/// \param displayText default text to display when the keyboard is shown
/// \param maxLength max length the user can enter for the string (0 for no limit)
/// \param restrictChars string of characters to not allow ("" for no restrictions)
///
/// \return pointer to keyboard for additional manipulation if needed
///////////////////////////////////////////////////////////////////////////////////////
/*static*/
ScreenKeyboard* OnlineUI::UseCase::ActivateKeyboard( const char* displayText, int maxLength /*= 0*/, const char* restrictChars /*= ""*/ )
{
	ScreenKeyboard* pKeyboard = g_FrontEnd.GetKeyboard();

	if( pKeyboard )
	{
		pKeyboard->SetString( displayText );
		pKeyboard->SetMaxString( maxLength );
		pKeyboard->RestrictChars( true, restrictChars );
	}

	g_FrontEnd.ShowKeyboard();

	return pKeyboard;
}

///////////////////////////////////////////////////////////////////////////////////////
/// Set up the keyboard to handle input for a valid Eidos Name
///
/// \param displayText default text to display when the keyboard is shown
/// \param maxLength max length the user can enter for the string (0 for no limit)
///
/// \return pointer to keyboard for additional manipulation if needed
///////////////////////////////////////////////////////////////////////////////////////
/*static*/
ScreenKeyboard* OnlineUI::UseCase::ActivateKeyboardForNames( const char* displayText, int maxLength /*= 15*/ )
{
#ifdef _XBOX
	static const char allowedChars[] = " 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
#else //!_XBOX
	static const char allowedChars[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-_&@#$!?:.~*=[],ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÑÒÓÔÕÖØÙÚÛÜàáâãäåæçèéêëìíîïñòóôõöøùúûüß";
#endif

	ScreenKeyboard* pKeyboard = g_FrontEnd.GetKeyboard();

	if( pKeyboard )
	{
		pKeyboard->SetString( displayText );
		pKeyboard->SetMaxString( maxLength );
		pKeyboard->RestrictKeys( true, 0, 0xFFFF );
		pKeyboard->RestrictChars( false, allowedChars );
	}

	g_FrontEnd.ShowKeyboard();

	return pKeyboard;
}

///////////////////////////////////////////////////////////////////////////////////////
/// Set up the keyboard to handle input for a valid Eidos password.
///
/// \param displayText default text to display when the keyboard is shown
/// \param maxLength max length the user can enter for the string (0 for no limit)
///
/// \return pointer to keyboard for additional manipulation if needed
///////////////////////////////////////////////////////////////////////////////////////
/*static*/
ScreenKeyboard* OnlineUI::UseCase::ActivateKeyboardForPasswords( const char* displayText, int maxLength /*= 15*/ )
{
#ifdef _XBOX
	static const char allowedChars[] = " 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
#else //!_XBOX
	static const char allowedChars[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-_&@#$!?:.~*=[],";
#endif

	ScreenKeyboard* pKeyboard = g_FrontEnd.GetKeyboard();

	if( pKeyboard )
	{
		pKeyboard->SetString( displayText );
		pKeyboard->SetMaxString( maxLength );
		pKeyboard->RestrictKeys( true, 0, 0xFFFF );
		pKeyboard->RestrictChars( false, allowedChars );
		pKeyboard->SetPasswordMode( true );
	}

	g_FrontEnd.ShowKeyboard();

	return pKeyboard;
}

///////////////////////////////////////////////////////////////////////////////////////
/// Set up the keyboard to handle input for a valid Clan Name
///
/// \param displayText default text to display when the keyboard is shown
/// \param maxLength max length the user can enter for the string (0 for no limit)
///
/// \return pointer to keyboard for additional manipulation if needed
///////////////////////////////////////////////////////////////////////////////////////
/*static*/
ScreenKeyboard* OnlineUI::UseCase::ActivateKeyboardForClanNames( const char* displayText, int maxLength /*= 15*/ )
{
#ifdef _XBOX
	static const char allowedChars[] = " 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
#else //!_XBOX
	static const char allowedChars[] = " 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-_&@#$!?:.~*=[],ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÑÒÓÔÕÖØÙÚÛÜàáâãäåæçèéêëìíîïñòóôõöøùúûüß";
#endif

	ScreenKeyboard* pKeyboard = g_FrontEnd.GetKeyboard();

	if( pKeyboard )
	{
		pKeyboard->SetString( displayText );
		pKeyboard->SetMaxString( maxLength );
		pKeyboard->RestrictKeys( true, 0, 0xFFFF );
		pKeyboard->RestrictChars( false, allowedChars );
	}

	g_FrontEnd.ShowKeyboard();

	return pKeyboard;
}

///////////////////////////////////////////////////////////////////////////////////////
/// Set up the keyboard to handle input for a valid email address
///
/// \param displayText default text to display when the keyboard is shown
/// \param maxLength max length the user can enter for the string (0 for no limit)
///
/// \return pointer to keyboard for additional manipulation if needed
///////////////////////////////////////////////////////////////////////////////////////
/*static*/
ScreenKeyboard* OnlineUI::UseCase::ActivateKeyboardForEmail( const char* displayText, int maxLength /*= 0*/ )
{
	static const char allowedEmailChars[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_&'+*-.@=?^~";

	ScreenKeyboard* pKeyboard = g_FrontEnd.GetKeyboard();

	if( pKeyboard )
	{
		pKeyboard->SetString( displayText );
		pKeyboard->SetMaxString( maxLength );
		pKeyboard->RestrictKeys( true, 0, 0xFFFF );
		pKeyboard->RestrictChars( false, allowedEmailChars );
	}

	g_FrontEnd.ShowKeyboard();

	return pKeyboard;
}

///////////////////////////////////////////////////////////////////////////////////////
/// Set up the keyboard to handle input for a valid postal code
///
/// \param displayText default text to display when the keyboard is shown
/// \param maxLength max length the user can enter for the string (0 for no limit)
///
/// \return pointer to keyboard for additional manipulation if needed
///////////////////////////////////////////////////////////////////////////////////////
/*static*/
ScreenKeyboard* OnlineUI::UseCase::ActivateKeyboardForPostalCode( const char* displayText, int maxLength /*= 9*/ )
{
	static const char allowedEmailChars[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

	ScreenKeyboard* pKeyboard = g_FrontEnd.GetKeyboard();

	if( pKeyboard )
	{
		pKeyboard->SetString( displayText );
		pKeyboard->SetMaxString( maxLength );
		pKeyboard->RestrictKeys( true, 0, 0xFFFF );
		pKeyboard->RestrictChars( false, allowedEmailChars );
	}

	g_FrontEnd.ShowKeyboard();

	return pKeyboard;
}