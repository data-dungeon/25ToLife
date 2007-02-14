#include "TTLPCH.h"
#include "frontend/inputdriver.h"
#include "screen_UIX.h"
#include "data/screens/UIX.h"
#include "platform/xbox/xboxlive.h"
#include "main/TTL.h"

/*virtual*/ void ScreenUIX::OnScreenIntro( void )
{
}

//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл
// This is the implementation for the Xbox Live aware screens
// "SCREEN_ID_UIXFRIENDS" and "SCREEN_ID_UIXPLAYERS"
//ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл

//=============================================================================
// ScreenUIXFriends::OnScreenIntro
//
//		When this function is called, it should activate the UIX friends screen.
//=============================================================================

void ScreenUIXFriends::OnScreenIntro( void )
{
	ScreenBaseClass::OnScreenIntro();

#ifndef _DEBUG
	SetText( ID_UIXFRIENDS_TITLE, 0 );
#endif

	#if defined (_XBOX)
	if (CXboxLive::IsUIXStarted() == false)
	{
		GotoPrevScreen();
		return;
	}

	if (CXboxLive::AreWeLoggedIn())
		CXboxLive::StartFriendsList();
	else
		GotoPrevScreen();
	#endif
}

//=============================================================================
// ScreenUIXFriends::OnScreenUpdate
//
//		This function is called once per frame by the system, it should do
//		whatever updates to the UIX system as needed and check for an exit
//		condition.
//=============================================================================

void ScreenUIXFriends::OnScreenUpdate( void )
{
	ScreenBaseClass::OnScreenUpdate();

	#if defined (_XBOX)
	if (CXboxLive::CheckExitCondition( UIX_EXIT_FRIENDS_NORMAL_EXIT ))
	{
		GotoPrevScreen();
	}
	else
	if (CXboxLive::CheckExitCondition( UIX_EXIT_FRIENDS_JOIN_GAME ) )
	{
		// set flag to tell loop to jump to multiplayer
		g_messageDispatcher.SendMessageToAll("AcceptInvitation", NULL, INVALID_OBJECT_HANDLE );
		g_acceptingInvitation = true;
		GotoPrevScreen();
	}
	else
	if (CXboxLive::CheckExitCondition( UIX_EXIT_FRIENDS_JOIN_GAME_CROSS_TITLE ) )
	{
		// NEED TO TELL TTL TO REBOOT FOR ANOTHER GAME??
		GotoPrevScreen();
	}
	else

	#endif

	return;
}

//=============================================================================
// ScreenUIXFriends::OnScreenExit
//
//		This function is called when the screen is deactivated.  Ideally, you
//		want to terminate the UIX friends screen if its still active and any
//		cleanup if necessary.
//=============================================================================

void ScreenUIXFriends::OnScreenExit( void )
{
	ScreenBaseClass::OnScreenExit();
}

//=============================================================================
// ScreenUIXFriends::OnButtonCancel
//
//		We're goig to eat the button press so that UIX has control over the
//		screen flow.  UIX will signal when its time to leave.
//=============================================================================

void ScreenUIXFriends::OnButtonCancel( void )
{
	#ifndef _XBOX
		ScreenBaseClass::OnButtonCancel();
	#else
		// eat it
	#endif
}

//=============================================================================
// ScreenUIXPlayers::OnScreenIntro
//
//		When this function is called, it should activate the UIX players screen.
//=============================================================================

void ScreenUIXPlayers::OnScreenIntro( void )
{
	ScreenBaseClass::OnScreenIntro();

#ifndef _DEBUG
	SetText( ID_UIXPLAYERS_TITLE, 0 );
#endif

	#if defined (_XBOX)
	if (CXboxLive::IsUIXStarted() == false)
	{
		GotoPrevScreen();
		return;
	}

	if (CXboxLive::AreWeLoggedIn())
		CXboxLive::StartPlayerList();
	else
		GotoPrevScreen();
	#endif

}

//=============================================================================
// ScreenUIXPlayers::OnScreenUpdate
//
//		This function is called once per frame by the system, it should do
//		whatever updates to the UIX system as needed and check for an exit
//		condition.
//=============================================================================

void ScreenUIXPlayers::OnScreenUpdate( void )
{
	ScreenBaseClass::OnScreenUpdate();

	#if defined (_XBOX)
	if (CXboxLive::CheckExitCondition( UIX_EXIT_PLAYERS_NORMAL_EXIT ) )
	{
		GotoPrevScreen();
	}
	#endif
	return;
}

//=============================================================================
// ScreenUIXPlayers::OnScreenExit
//
//		This function is called when the screen is deactivated.  Ideally, you
//		want to terminate the UIX players screen if its still active and any
//		cleanup if necessary.
//=============================================================================

void ScreenUIXPlayers::OnScreenExit( void )
{
	ScreenBaseClass::OnScreenExit();
}

//=============================================================================
// ScreenUIXPlayers::OnButtonCancel
//
//		We're goig to eat the button press so that UIX has control over the
//		screen flow.  UIX will signal when its time to leave.
//=============================================================================

void ScreenUIXPlayers::OnButtonCancel( void )
{
	#ifndef _XBOX
		ScreenBaseClass::OnButtonCancel();
	#else
		// eat it
	#endif
}

//=============================================================================
// ScreenUIXLogin::OnScreenIntro
//
//		When this function is called, it should activate the UIX players screen.
//=============================================================================

void ScreenUIXLogin::OnScreenIntro( void )
{
	ScreenBaseClass::OnScreenIntro();

	#ifndef _DEBUG
		SetText( ID_UIXLOGIN_TITLE, 0 );
	#endif

	#if defined (_XBOX)
	if (CXboxLive::IsUIXStarted() == false)
		GotoPrevScreen();
	else
	{
		CXboxLive::ClearLastExitCode();
		CXboxLive::SetSilentLogin(false);		
		if( !CXboxLive::LoginUIX() )
			GotoPrevScreen();
	}
	#endif

}

//=============================================================================
// ScreenUIXLogin::OnScreenUpdate
//
//		This function is called once per frame by the system, it should do
//		whatever updates to the UIX system as needed and check for an exit
//		condition.
//=============================================================================

void ScreenUIXLogin::OnScreenUpdate( void )
{
 	ScreenBaseClass::OnScreenUpdate();

	#if defined (_XBOX)
	if (CXboxLive::CheckExitCondition(UIX_EXIT_LOGON_USER_EXIT) ||
		 CXboxLive::CheckExitCondition(UIX_EXIT_LOGON_SUCCESSFUL) )
	{
		GotoPrevScreen();
	}

	if(CXboxLive::CheckExitCondition( CXBOXLIVE_UIX_EXIT))
	{
		GotoPrevScreen();
	}


	#endif
	return;
}

//=============================================================================
// ScreenUIXLogin::OnScreenExit
//
//		This function is called when the screen is deactivated.  Ideally, you
//		want to terminate the UIX login screen if its still active and any
//		cleanup if necessary.
//=============================================================================

void ScreenUIXLogin::OnScreenExit( void )
{
	ScreenBaseClass::OnScreenExit();
}

//=============================================================================
// ScreenUIXLogin::OnButtonCancel
//
//		We're goig to eat the button press so that UIX has control over the
//		screen flow.  UIX will signal when its time to leave.
//=============================================================================

void ScreenUIXLogin::OnButtonCancel( void )
{
	#ifndef _XBOX
		ScreenBaseClass::OnButtonCancel();
	#else
		// eat it
	#endif
}