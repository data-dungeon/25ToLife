/********************************************************************
	$File: //depot/TTL/code/sim/InitialLoop.cpp $
	$Revision: #38 $
	$DateTime: 2005/07/22 14:49:47 $
	$Author: Ken_Harward $
	purpose: Implement the initial game loop.
*********************************************************************/
#include "TTLPCH.h"
#include "InitialLoop.h"
#include "frontend/frontend.h"
#include "fmv/fmv.h"

extern bool g_regionUSA;
extern bool g_PalModeDone;

bool CInitialLoop::s_skipFluff = false;
bool CInitialLoop::s_titleHasBeenShown = false;
bool CInitialLoop::s_preloadProfile = false;

SLoopEntry CInitialLoop::s_stateArray[CInitialLoop::NUM_STATES] =
{
	"INITIALIZE", (LoopFunc)&CInitialLoop::Initialize,
	"RUN",        NULL,
	"TERMINATE",  (LoopFunc)&CInitialLoop::Terminate
};

CInitialLoop::CInitialLoop() :
	GameLoop(s_stateArray, UNINITIALIZED),
	d_messageOwner("InitialLoop")
{
}

int CInitialLoop::DetermineState(int i_state)
{
	switch((ELoopStates)i_state)
	{
	 case UNINITIALIZED:
		return(INITIALIZE);
	 case INITIALIZE:
	 case RUN:
		if ((d_nextLoop != NO_NEXT) || d_requestExit)
			return(TERMINATE);
		return(RUN);
	 case TERMINATE:
		return(UNINITIALIZED);
	 default:
		// Let any externally defined states pass through.
		return(i_state);
	}

	return(UNINITIALIZED);
}

void CInitialLoop::Initialize(void)
{
	d_nextLoop = NO_NEXT;
	d_nextLoopHandler.Initialize("NextLoop", MESSAGE_HANDLE_IF_MASTER,
			d_messageOwner, this);
	d_acceptInvitationHandler.Initialize("AcceptInvitation",
			MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this);

	// Make sure there is no fade.
	if (RenderMgr__IsFadedOut() || RenderMgr__IsFadingOut())
		RenderMgr__FadeIn(FADE_TO_BLACK_INSTANT);

	// Don't let the front end back out on its own.
	g_FrontEnd.ClearScreenHistory();

	// We need to do this for debugging on the xbox
	if ((s_preloadProfile || s_skipFluff) && !g_input.ControllerLocked(0))
		g_input.LockPlayerToController( 0, 0 );

	// Put up the menu.
	if (!s_titleHasBeenShown && !s_preloadProfile)
	{
		s_titleHasBeenShown = true;

		if (!s_skipFluff)
		{
#if 0 // Stop the Pal 60 test
			if	(!g_regionUSA)
				g_FrontEnd.GotoScreen("SCREEN_ID_PALMODE");
			else
#endif
			{
				g_PalModeDone = true;
				g_FrontEnd.GotoScreen("SCREEN_ID_TITLE");
			}
		}
		else
		{
#ifdef DIRECTX_PC
				g_FrontEnd.GotoScreen("SCREEN_ID_ONLINEPROFILE_PC");
#else
				g_FrontEnd.GotoScreen("SCREEN_ID_ONLINEPROFILE");
#endif // DIRECTX_PC
		}
	}
	else
	{
		g_FrontEnd.GotoScreen( SCREEN_MAINMENU );
	}
}

void CInitialLoop::Terminate(void)
{
	d_messageOwner.UnregisterAll();
}

void CInitialLoop::CNextLoopHandler::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	char* loopName = (char*)i_data;
	if (!strcmp(loopName, "SinglePlayer"))
	{
		d_ref->d_nextLoop = SINGLEPLAYER_NEXT;
	}
	else if (!strcmp(loopName, "MultiPlayer_LAN"))
	{
		d_ref->d_nextLoop = SYSTEMLINK_NEXT;
	}
	else if (!strcmp(loopName, "MultiPlayer_Internet"))
	{
		d_ref->d_nextLoop = ONLINE_NEXT;
	}
	else if (!strcmp(loopName, "Developer"))
	{
		d_ref->d_nextLoop = DEVELOPER_NEXT;
	}
}

void CInitialLoop::CAcceptInvitationHandler::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	d_ref->d_nextLoop = ONLINE_NEXT;
}
