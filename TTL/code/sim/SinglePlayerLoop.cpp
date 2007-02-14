/********************************************************************
	$File: //depot/TTL/code/sim/SinglePlayerLoop.cpp $
	$Revision: #24 $
	$DateTime: 2005/07/13 15:35:08 $
	$Author: Ken_Harward $
	purpose: Implement the initial game loop.
*********************************************************************/

#include "TTLPCH.h"
#include "SinglePlayerLoop.h"
#include "frontend/frontend.h"
#ifdef NETZ
#include "Network/NetZ/NetZConnection.h"
#endif

#include "platform/MemoryCard/MemoryCard.h"

SLoopEntry CSinglePlayerLoop::s_stateArray[CSinglePlayerLoop::NUM_STATES] =
{
	"INITIALIZE", (LoopFunc)&CSinglePlayerLoop::Initialize,
	"RUN",        NULL,
	"TERMINATE",  (LoopFunc)&CSinglePlayerLoop::Terminate
};

/******************************************************************************/
/******************************************************************************/
CSinglePlayerLoop::CSinglePlayerLoop() :
	GameLoop(s_stateArray, UNINITIALIZED),
	d_messageOwner("SinglePlayerLoop")
{
}

/******************************************************************************/
/******************************************************************************/
int CSinglePlayerLoop::DetermineState(int i_state)
{
	// popup needed?
	if (g_MemCardManager->GetPopupRequest())
	{
		g_FrontEnd.ShowPopupCallBack();
		g_MemCardManager->SetPopupRequest(false);
	}

	// remove popup?
	if (g_MemCardManager->GetPopupClearRequest())
	{
		g_FrontEnd.HidePopupCallBack();
		g_MemCardManager->SetPopupClearRequest(false);
	}

	switch((ELoopStates)i_state)
	{
	 case UNINITIALIZED:
		if (d_requestExit)
			return(UNINITIALIZED);
		d_nextLoop = NO_NEXT;
		return(INITIALIZE);
	 case INITIALIZE:
	 case RUN:
		 if (d_requestExit || (d_nextLoop == INITIAL_NEXT))
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
void CSinglePlayerLoop::Initialize(void)
{
#ifdef NETZ
	CNetZConnection::TearDown();
#endif

	d_launchHandler.Initialize("SetupLaunch", MESSAGE_HANDLE_IF_MASTER,
			d_messageOwner, this);
	d_backupHandler.Initialize("SetupBackup", MESSAGE_HANDLE_IF_MASTER,
			d_messageOwner, this);
	d_acceptInvitationHandler.Initialize("AcceptInvitation",
			MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this);

	// Make sure there is no fade.
	if (RenderMgr__IsFadedOut() || RenderMgr__IsFadingOut())
		RenderMgr__FadeIn(FADE_TO_BLACK_INSTANT);

	g_FrontEnd.ClearScreenHistory();
	g_FrontEnd.GotoScreen("SCREEN_ID_DIFFICULTY");
}

/******************************************************************************/
/******************************************************************************/
void CSinglePlayerLoop::Terminate(void)
{
	d_messageOwner.UnregisterAll();
}

/******************************************************************************/
/******************************************************************************/
void CSinglePlayerLoop::CLaunch::HandleMessage(void* i_data,
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
void CSinglePlayerLoop::CBackup::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	d_ref->d_nextLoop = INITIAL_NEXT;
}

/******************************************************************************/
/******************************************************************************/
void CSinglePlayerLoop::CAcceptInvitation::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	d_ref->d_nextLoop = INITIAL_NEXT;
}
