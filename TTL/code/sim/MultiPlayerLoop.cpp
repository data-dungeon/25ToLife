/********************************************************************
	$File: //depot/TTL/code/sim/MultiPlayerLoop.cpp $
	$Revision: #40 $
	$DateTime: 2005/05/03 16:18:03 $
	$Author: Bryant_Collard $
	purpose: Implement the initial game loop.
*********************************************************************/

#include "TTLPCH.h"
#include "sim/MultiPlayerLoop.h"
#include "frontend/frontend.h"
#include "main/PlayerProfile.h"
#include "Network/SessionSpec.h"
#include "GameHelper/ErrorStack.h"

SLoopEntry CMultiPlayerLoop::s_stateArray[CMultiPlayerLoop::NUM_STATES] =
{
	"INITIALIZE", (LoopFunc)&CMultiPlayerLoop::Initialize,
	"RUN",        NULL,
	"TERMINATE",  (LoopFunc)&CMultiPlayerLoop::Terminate
};

/******************************************************************************/
/******************************************************************************/
CMultiPlayerLoop::CMultiPlayerLoop() :
	GameLoop(s_stateArray, UNINITIALIZED),
	d_messageOwner("MultiPlayerLoop")
{
}

/******************************************************************************/
/******************************************************************************/
int CMultiPlayerLoop::DetermineState(int i_state)
{
	if (g_errorStack.IsFatal() && !d_lobby.SessionSearch().IsActive())
		d_nextLoop = INITIAL_NEXT;

	switch((ELoopStates)i_state)
	{
	 case UNINITIALIZED:
		if (d_requestExit)
			return(UNINITIALIZED);
		d_nextLoop = NO_NEXT;
		return(INITIALIZE);
	 case INITIALIZE:
	 case RUN:
		if (d_requestExit || (d_nextLoop == INITIAL_NEXT) ||
				(d_nextLoop == CUSTOMPLAYER_NEXT))
			return(TERMINATE);
		if (d_nextLoop == SIM_NEXT)
			return(WaitForTask(&RenderMgr__IsFadedOut, NULL, TERMINATE));
		return(RUN);
	 case TERMINATE:
		break;
	 default:
		if (d_requestExit || (d_nextLoop == INITIAL_NEXT))
			return(TERMINATE);
		return(i_state);
	}

	return(UNINITIALIZED);
}

/******************************************************************************/
/******************************************************************************/
void CMultiPlayerLoop::Initialize( void )
{
	// Set the lobby here so the front end can access it.
	CLobby::Start(d_lobby);

	// Register message handlers.
	d_backupHandler.Initialize("SetupBackup", MESSAGE_HANDLE_IF_MASTER,
			d_messageOwner, this);
	d_settingsHandler.Initialize("SetupSettings", MESSAGE_HANDLE_IF_MASTER,
			d_messageOwner, this);
	d_createHandler.Initialize("SetupCreateSession", MESSAGE_HANDLE_IF_MASTER,
			d_messageOwner, this);
	d_launchHandler.Initialize("SetupLaunch", MESSAGE_HANDLE_IF_MASTER,
			d_messageOwner, this);
	d_cancelSearchHandler.Initialize("CancelSearchResults",
			MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this);
	d_acceptInvitationHandler.Initialize("AcceptInvitation",
			MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this);

	// Make sure there is no fade.
	if (RenderMgr__IsFadedOut() || RenderMgr__IsFadingOut())
		RenderMgr__FadeIn(FADE_TO_BLACK_INSTANT);

	// Go to the first screen.
	g_FrontEnd.ClearScreenHistory();
	g_FrontEnd.GotoScreen("SCREEN_ID_ONLINEMENU");
}

/******************************************************************************/
/******************************************************************************/
void CMultiPlayerLoop::Terminate(void)
{
	// Clear the lobby if not simulating.
	if (d_nextLoop != SIM_NEXT)
		CLobby::Stop();

	g_errorStack.ClearAllNonExpiringErrors();
	d_messageOwner.UnregisterAll();
}

/******************************************************************************/
/******************************************************************************/
void CMultiPlayerLoop::CBackup::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	d_ref->d_nextLoop = INITIAL_NEXT;
}

/******************************************************************************/
/******************************************************************************/
void CMultiPlayerLoop::CSettings::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	if (d_ref->d_nextLoop == NO_NEXT)
		d_ref->d_nextLoop = CUSTOMPLAYER_NEXT;
}

/******************************************************************************/
/******************************************************************************/
void CMultiPlayerLoop::CCreate::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	if (d_ref->d_nextLoop == NO_NEXT)
	{
		const char* pSessionName = g_sessionSpec.GetName();

		//===========================================================================
		// NOTICE: The session name is entered by the user when a match is created.
		//         If the session name is invalid, then the account name is used.
		//===========================================================================

		if( !pSessionName || strlen(pSessionName) == 0 )
			g_sessionSpec.SetName(g_playerProfile.GetAccountName());

		d_ref->d_launchHandler.HandleMessage(i_data, i_sender, i_posted);
	}
}

/******************************************************************************/
/******************************************************************************/
void CMultiPlayerLoop::CLaunch::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	if (d_ref->d_nextLoop == NO_NEXT)
	{
		// Quiet the music.
		g_audio.SetVolume(Audio::MUSIC, 0.0f, Audio::smoothVolumeSlow);

		// Fade out.
		RenderMgr__FadeOut(FADE_TO_BLACK_FAST);

		// Play.
		d_ref->d_nextLoop = SIM_NEXT;
	}
}

/******************************************************************************/
/******************************************************************************/
void CMultiPlayerLoop::CCancelSearch::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	g_FrontEnd.GetActiveScreenHandler()->GotoPrevScreen();
}

/******************************************************************************/
/******************************************************************************/
void CMultiPlayerLoop::CAcceptInvitation::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	d_ref->d_nextLoop = INITIAL_NEXT;
}
