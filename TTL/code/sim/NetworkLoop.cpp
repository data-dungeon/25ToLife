/*
**
**  File:   NetworkLoop.h
**  Date:   September 29, 2004
**  By:     Bryant Collard
**  Desc:   Gameplay loop for multi-player sessions.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
*/
#if defined(NETWORKING)
// Precompiled header.
#include "TTLPCH.h"

// My header
#include "sim/NetworkLoop.h"

// GameLib headers
#include "Network/Session.h"
#include "Network/SessionSpec.h"
#include "GameHelper/LogUtil.h"
#include "GameHelper/ErrorStack.h"

// TTL headers
#include "frontend/frontend.h"
#include "frontend/screens/screen_Popups.h"
#include "main/PlayerProfile.h"
#include "main/PlayerAgent.h"
#include "main/PlayerDB.h"
#include "main/CharacterLoader.h"
#include "main/Lobby.h"
#include "main/ErrorMonitor.h"
#include "sim/Referee.h"
#include "environ/ConfigMgr.h"
#include "environ/PlayerSpawnMgr.h"
#include "environ/ActorCreator.h"
#ifdef NETZ
#include "NetZTTL/TTLSession.h"
#endif

#include "frontend/savegame.h"
#include "Display/TextureUtil.h"

#include "weapons/armoryview.h"

#include "Voice/NetZ/CVoiceChat.h"

#include "frontend/UseCase/UseCaseReadyUp.hpp"
#include "OSI/OSI.hpp"

#ifdef _XBOX
#include "OSI/Xbox/OSIXbox.hpp"
#endif

#ifdef MASSIVE_AD
#include "Massive/MassiveClientDriver.h"
#endif

#define ALL_JOIN_ROUND

#if defined(PS2) && !defined(CONSUMER_BUILD)
//#define RAM_CLOBBER_DECOY
#endif

#if defined(RAM_CLOBBER_DECOY)
#include "EngineHelper/decoy.h"
CDecoy* g_decoy = NULL;
#endif //RAM_CLOBBER_DECOY

// Initialize function array.
SLoopEntry CNetworkLoop::s_stateArray[CNetworkLoop::NUM_STATES] =
{
	"OPEN_SESSION",            (LoopFunc)&CNetworkLoop::OpenSession,
	"WAIT_TO_OPEN_MATCH",      (LoopFunc)&CNetworkLoop::Wait,
	"WAIT_FOR_IDENTITY",       (LoopFunc)&CNetworkLoop::Wait,
	"WAIT_FOR_READY_OR_SETUP", (LoopFunc)&CNetworkLoop::Wait,
	"ENTER_READY",             (LoopFunc)&CNetworkLoop::EnterReady,
	"WAIT_FOR_POPULATE",       (LoopFunc)&CNetworkLoop::Wait,
	"WAIT_FOR_REPOPULATE",     (LoopFunc)&CNetworkLoop::Wait,
	"ENTER_JOIN",              (LoopFunc)&CNetworkLoop::Wait,
	"WAIT_TO_JOIN",            (LoopFunc)&CNetworkLoop::Wait,
	"WAIT_FOR_DISTRIBUTE",     (LoopFunc)&CNetworkLoop::Wait,
	"WAIT_FOR_LOCK_RESOLVE",   (LoopFunc)&CNetworkLoop::Wait,
	"PUBLISH",                 (LoopFunc)&CNetworkLoop::Publish,
	"WAIT_TO_RUN",             (LoopFunc)&CNetworkLoop::Wait,
	"ENTER_RUN",               (LoopFunc)&CNetworkLoop::EnterRun,
	"RUN",                     (LoopFunc)&CNetworkLoop::Run,
	"EXIT_RUN",                (LoopFunc)&CNetworkLoop::ExitRun,
	"UNPUBLISH",               (LoopFunc)&CNetworkLoop::Unpublish,
	"WAIT_FOR_EMIGRATION",     (LoopFunc)&CNetworkLoop::Wait,
	"ENTER_GHOST_READY",       (LoopFunc)&CNetworkLoop::EnterReady,
	"JOIN_GHOST_READY",        (LoopFunc)&CNetworkLoop::Wait,
	"WAIT_FOR_GHOST_READY",    (LoopFunc)&CNetworkLoop::Wait,
	"LEAVE_GHOST_READY",       (LoopFunc)&CNetworkLoop::Wait,
	"WAIT_FOR_GHOST_REPORT",   (LoopFunc)&CNetworkLoop::Wait,
	"START_GHOST",             (LoopFunc)&CNetworkLoop::StartGhost,
	"WAIT_TO_RUN_GHOST",       (LoopFunc)&CNetworkLoop::Wait,
	"ENTER_RUN_GHOST",         (LoopFunc)&CNetworkLoop::EnterRunGhost,
	"RUN_GHOST",               (LoopFunc)&CNetworkLoop::Run,
	"STOP_GHOST",              (LoopFunc)&CNetworkLoop::StopGhost,
	"LEAVE_ROUND",             (LoopFunc)&CNetworkLoop::Wait,
	"LEAVE_SESSION",           (LoopFunc)&CNetworkLoop::Wait,
	"DISABLE_SESSION",         (LoopFunc)&CNetworkLoop::DisableSession,
	"WAIT_TO_CLOSE_SESSION",   (LoopFunc)&CNetworkLoop::Wait,
	"WAIT_TO_CLOSE_MATCH",     (LoopFunc)&CNetworkLoop::Wait,
	"CLOSE_SESSION",           (LoopFunc)&CNetworkLoop::CloseSession,
	"RESET",                   (LoopFunc)&CNetworkLoop::Reset
};

/******************************************************************************/
/******************************************************************************/
static void LogMessage(char* i_label, char* i_action, const char* i_name)
{
	SLogMessage logMessage;
	logMessage.d_type = SLogMessage::LOG_ACTION;
	logMessage.d_label = i_label;
	logMessage.d_action = i_action;
	g_messageDispatcher.SendMessageToAll(i_name, &logMessage,
			INVALID_OBJECT_HANDLE);
}

/******************************************************************************/
/******************************************************************************/
CNetworkLoop::CNetworkLoop() : CTTLLoop(s_stateArray, UNINITIALIZED)
{
	// Reset owner name.
	d_messageOwner.SetOwnerName("NetworkLoop");
	d_controller = NULL;
}

/******************************************************************************/
/******************************************************************************/
int CNetworkLoop::DetermineState(int i_state)
{
	// update memcard state machine
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

	// Gracefully exit on fatal errors.
	if (g_errorStack.IsFatal())
		RequestExit();

	// Cache the referee phase.
	Referee::EPhase refereePhase = Referee::NUM_PHASES;
	if (ValidSingleton(Referee))
		refereePhase = g_referee.GetPhase();

	switch((ELoopStates)i_state)
	{
	 case UNINITIALIZED:
#ifdef NETZ
		CNetZConnection::InitializeLog();
#endif
		d_requestExit = false;
		d_loaded = false;
		d_lockState = LOCK_INACTIVE;
		g_lobby.Reset();
		d_currentScreen = NO_SCREEN;
		return(OPEN_SESSION);

	 case OPEN_SESSION:
		if (!d_stateSucceeded)
			return(WaitForTask(&IsErrorStackEmptyOfExpiring,
					(LoopFunc)&CNetworkLoop::RenderWait, RESET));
		if (d_requestExit)
			return(LEAVE_SESSION);
		return(WAIT_TO_OPEN_MATCH);

	 case WAIT_TO_OPEN_MATCH:
		if (d_requestExit)
			return(LEAVE_SESSION);
		if (g_lobby.IsRequestPending())
			return(WAIT_TO_OPEN_MATCH);
		g_lobby.OpenMatch();
		return(WAIT_FOR_IDENTITY);

	 case WAIT_FOR_IDENTITY:
		if (d_requestExit)
			return(LEAVE_SESSION);

		if (g_lobby.IsOrphanSession())
		{
			g_errorStack.SetError("Session", "Lost session host",
					CErrorStack::ERROR_FATAL, 5000);
			return(LEAVE_SESSION);
		}

		switch(d_localAgent->GetAssignment())
		{
		 case CPlayerAgent::WAITING:
			return(WAIT_FOR_IDENTITY);

		 case CPlayerAgent::ASSIGNED:
			d_localAgent->SetReady(g_referee.IsReady(d_localAgent->Id()));
			d_localAgent->SetMode(CPlayerAgent::UNLOADED);

			// --- handle voice chat setting
			if(refereePhase == Referee::READY || Referee::SETUP || Referee::REPORTING)
			{
				// --- DAS, go to all chat
				g_CVoiceChat.enableChatChannel(CVoiceChat::CHAT_CHANNEL_ALL, false);
				g_CVoiceChat.enableChatChannel(CVoiceChat::CHAT_CHANNEL_TEAM1, false);
				g_CVoiceChat.enableChatChannel(CVoiceChat::CHAT_CHANNEL_TEAM2, false);
				g_CVoiceChat.enableChatChannel(CVoiceChat::CHAT_CHANNEL_ALL, true);

				// --- set voice enable
				bool VoiceEnabled = g_GlobalProfile.VoiceParams.VoiceOutput != VOICEOUTPUT_DISABLED;
				g_CVoiceChat.enable(VoiceEnabled);
			}
			else if(refereePhase == Referee::POPULATING || Referee::REPOPULATE)
			{
				// --- DAS, no chat
				g_CVoiceChat.enableChatChannel(CVoiceChat::CHAT_CHANNEL_ALL, false);
				g_CVoiceChat.enableChatChannel(CVoiceChat::CHAT_CHANNEL_TEAM1, false);
				g_CVoiceChat.enableChatChannel(CVoiceChat::CHAT_CHANNEL_TEAM2, false);
			}
			else if(refereePhase == Referee::PLAYING)	// team chat
			{
				// enable team chat when in game
				int participantID = (g_referee.GetLocalPlayerAgent())->Id();
				bool blue = g_referee.GetTeamingModel().IsPlayerBlue(participantID);
				int teamID = (blue ? 1 : 2);
				g_CVoiceChat.enableChatChannel(CVoiceChat::CHAT_CHANNEL_ALL, false);
				g_CVoiceChat.enableChatChannel(CVoiceChat::CHAT_CHANNEL_TEAM1, false);
				g_CVoiceChat.enableChatChannel(CVoiceChat::CHAT_CHANNEL_TEAM2, false);
				g_CVoiceChat.enableChatChannel(((CVoiceChat::eChatChannels )teamID ), true);
				// --- set voice enable
				bool VoiceEnabled = g_GlobalProfile.VoiceParams.VoiceOutput != VOICEOUTPUT_DISABLED;
				g_CVoiceChat.enable(VoiceEnabled);
			}

			if ((refereePhase == Referee::READY) ||
					(refereePhase == Referee::SETUP))
				return(ENTER_READY);
			return(ENTER_GHOST_READY);

		 case CPlayerAgent::DENIED:
		 default:
			g_errorStack.SetError("Session", "Player slot unavailable",
					CErrorStack::ERROR_FATAL, 5000);
			return(LEAVE_SESSION);
		}

	 case WAIT_FOR_READY_OR_SETUP:
		if (d_requestExit)
			return(LEAVE_SESSION);
		if (g_lobby.IsOrphanSession())
		{
			g_errorStack.SetError("Session", "Lost session host",
					CErrorStack::ERROR_FATAL, 5000);
			return(LEAVE_SESSION);
		}
		if (g_referee.IsConsistent())
		{
			if (refereePhase == Referee::READY)
			{
				d_localAgent->SetReady(g_referee.IsReady(d_localAgent->Id()));
				return(ENTER_READY);
			}
			if ((refereePhase == Referee::SETUP) ||
					(refereePhase == Referee::POPULATING) ||
					(refereePhase == Referee::REPOPULATE))
			{
				d_localAgent->SetReady(g_referee.IsReady(d_localAgent->Id()));
				return(WAIT_FOR_POPULATE);
			}
		}
		return(WAIT_FOR_READY_OR_SETUP);

	 case ENTER_READY:
		d_localAgent->SetReady(false);
		// Fall through.

	 case WAIT_FOR_POPULATE:
		if (d_requestExit)
			return(LEAVE_SESSION);
		if (g_lobby.IsOrphanSession() &&	(refereePhase == Referee::READY))
		{
			g_errorStack.SetError("Session", "Lost session host",
					CErrorStack::ERROR_FATAL, 5000);
			return(LEAVE_SESSION);
		}
		if (refereePhase == Referee::REPORTING)
			return(WAIT_FOR_READY_OR_SETUP);
		if ((refereePhase == Referee::PLAYING) && g_referee.IsConsistent())
		{
			if (g_referee.IsGhost(d_localAgent->Id()))
					return(JOIN_GHOST_READY);
			return(WAIT_FOR_READY_OR_SETUP);
		}
		if (ProcessPreload())
		{
			d_localAgent->SetMode(CPlayerAgent::LOADED);
			if ((refereePhase == Referee::POPULATING) && g_referee.IsConsistent()
					&& g_referee.IsRoundOpen() && !g_lobby.IsRequestPending())
			{
				if (g_referee.IsGhost(d_localAgent->Id()))
					return(JOIN_GHOST_READY);

				LogMessage("Populate", "SrtLoad", "LogLock");
				if (!Load())
					return(LEAVE_SESSION);
				LogMessage("Populate", "EndLoad", "LogLock");

				g_lobby.JoinRound();
				return(ENTER_JOIN);
			}
		}
		else
			d_localAgent->SetMode(CPlayerAgent::UNLOADED);
		if ((refereePhase == Referee::READY) && (d_currentScreen != READY_SCREEN))
			GoToReadyScreen();
		return(WAIT_FOR_POPULATE);

	 case WAIT_FOR_REPOPULATE:
		if (d_requestExit)
			return(LEAVE_ROUND);
		if ((refereePhase != Referee::POPULATING) &&
				(refereePhase != Referee::REPOPULATE))
			return(WAIT_FOR_READY_OR_SETUP);
		if (ProcessPreload())
		{
			d_localAgent->SetMode(CPlayerAgent::LOADED);
			if (refereePhase == Referee::POPULATING)
			{
				if (g_referee.IsGhost(d_localAgent->Id()))
					return(WAIT_FOR_GHOST_READY);

				LogMessage("Populate", "SrtLoad", "LogLock");
				if (!Load())
					return(LEAVE_ROUND);
				LogMessage("Populate", "EndLoad", "LogLock");

				return(ENTER_JOIN);
			}
		}
		else
			d_localAgent->SetMode(CPlayerAgent::UNLOADED);
		return(WAIT_FOR_REPOPULATE);

	 case ENTER_JOIN:
		if (d_requestExit)
			return(LEAVE_ROUND);

		// Disable all voice while we are transitioning into the game
		g_CVoiceChat.enable(false);

		// Request a lock to populate.
		LogMessage("Lock", "Populate", "LogLock");
		LogMessage("Populate", "ReqLock", "LogLock");
		g_debugOutput.TSPrint(g_debugOutput.Category("LOCK"),
				"Request Populate Lock");
		d_lockState = LOCK_READY;
		if (!g_session.RequestLock((CSessionLockRequest*)this))
		{
			ASSERT(false);
			d_lockState = LOCK_INACTIVE;
			return(LEAVE_ROUND);
		}
		return(WAIT_TO_JOIN);

	 case WAIT_TO_JOIN:
		if (d_requestExit)
		{
			LogMessage("Unlock", "ExitPublish", "LogLock");
			d_lockState = LOCK_INACTIVE;
			g_debugOutput.TSPrint(g_debugOutput.Category("LOCK"),
					"Cancel Populate (ExitPublish) Lock");
			g_session.RequestUnlock((CSessionLockRequest*)this);
			return(LEAVE_ROUND);
		}
		if (refereePhase == Referee::REPOPULATE)
		{
			LogMessage("Unlock", "RePublish", "LogLock");
			d_lockState = LOCK_INACTIVE;
			g_debugOutput.TSPrint(g_debugOutput.Category("LOCK"),
					"Cancel Populate (RePublish) Lock");
			g_session.RequestUnlock((CSessionLockRequest*)this);
			Unload();
			return(WAIT_FOR_REPOPULATE);
		}
		if (refereePhase != Referee::POPULATING)
		{
			LogMessage("Unlock", "ConfusedPublish", "LogLock");
			d_lockState = LOCK_INACTIVE;
			g_debugOutput.TSPrint(g_debugOutput.Category("LOCK"),
					"Cancel Populate (ConfusedPublish) Lock");
			g_session.RequestUnlock((CSessionLockRequest*)this);
			Unload();
			return(WAIT_FOR_READY_OR_SETUP);
		}
		int start, end;
		if ((d_lockState == LOCK_ESTABLISHED) &&
				g_referee.CanLoadWorldActors(start, end))
		{
			char msg[80];
			sprintf(msg, "Process: %d - %d", start, end);
			LogMessage("Loop", msg, "LogEntry");
			LogMessage("Populate", "Start/Process", "LogLock");
			s_worldLoader.Process(start, end);
			LogMessage("Populate", "EndProcess", "LogLock");
			Display__SetIdleCallback(&PopulateWorld);
			return(WAIT_FOR_DISTRIBUTE);
		}
		return(WAIT_TO_JOIN);

	 case WAIT_FOR_DISTRIBUTE:
		if (d_requestExit)
		{
			g_session.RequestUnlock((CSessionLockRequest*)this);
			Display__SetIdleCallback(&ServiceConnection);
			return(LEAVE_ROUND);
		}
		if (s_worldLoader.Distribute())
		{
			LogMessage("Populate", "EndDist", "LogLock");
			LogMessage("Unlock", "Published", "LogLock");
			d_lockState = LOCK_INACTIVE;
			g_debugOutput.TSPrint(g_debugOutput.Category("LOCK"),
					"Cancel Populate (Finished) Lock");
			g_session.RequestUnlock((CSessionLockRequest*)this);
			Display__SetIdleCallback(&ServiceConnection);
			return(WAIT_FOR_LOCK_RESOLVE);
		}
		return(WAIT_FOR_DISTRIBUTE);

	 case WAIT_FOR_LOCK_RESOLVE:
		if (d_requestExit)
		{
			g_session.RequestUnlock((CSessionLockRequest*)this);
			Display__SetIdleCallback(&ServiceConnection);
			return(LEAVE_ROUND);
		}
		if (g_session.IsLockResolved())
		{
			LogMessage("Populate", "Resolved", "LogLock");
			return(PUBLISH);
		}
		return(WAIT_FOR_LOCK_RESOLVE);

	 case PUBLISH:
		LogMessage("Loop", "ProcessDone", "LogEntry");
		LogMessage("Populate", "Finish", "LogLock");

		if (!d_stateSucceeded)
			return(LEAVE_ROUND);

		d_localAgent->SetMode(CPlayerAgent::PUBLISHED);

		if (d_requestExit)
			return(UNPUBLISH);

		return(WAIT_TO_RUN);

	 case WAIT_TO_RUN:
		if (d_requestExit)
			return(UNPUBLISH);
		if (refereePhase == Referee::PLAYING)
			return(ENTER_RUN);
		if (refereePhase != Referee::POPULATING)
			return(UNPUBLISH);
		return(WAIT_TO_RUN);

	 case ENTER_RUN:
		LogMessage("Populate", "Run", "LogLock");
#if !defined(CONSUMER_BUILD)
		{
			int freeB = 0, freeH = 0, freeMax = 0, allocB = 0, allocH = 0;
			Layers__GetMemoryStats(&freeB, &freeH, &freeMax, &allocB, &allocH);
			g_debugOutput.RegisterCategory("Memory", "MEM");
			g_debugOutput.Print(false, g_debugOutput.Category("MEM"), "RUN %8d free, %8d largest, %4d handles", freeB, freeMax, freeH);
		}
#endif
#if defined(RAM_CLOBBER_DECOY)
		if (!g_decoy)
			g_decoy = new CDecoy(512, 20*1024, 100, 1500);
#endif //RAM_CLOBBER_DECOY

	 case RUN:
#if defined(RAM_CLOBBER_DECOY)
		g_decoy->Update();
#endif //RAM_CLOBBER_DECOY
		if ((d_requestExit || ((refereePhase != Referee::PLAYING) &&
				((refereePhase != Referee::REPORTING) ||
				g_referee.GetScoringModel().IsTabulated()))) &&
				!g_lobby.IsRequestPending())
			return(EXIT_RUN);
		return(RUN);

	 case EXIT_RUN:
#if defined(RAM_CLOBBER_DECOY)
		 delete g_decoy;
		 g_decoy = NULL;
#endif //RAM_CLOBBER_DECOY

		if (d_requestExit)
		{
			g_lobby.QuitRound();
			GoToLeavingScreen();
		}
		else if (refereePhase == Referee::REPORTING)
		{
			if (g_lobby.IsOrphanSession())
				g_lobby.QuitRound();
			else
				g_lobby.EndRound();
			GoToReportScreen();
		}
		else
			g_lobby.QuitRound();

#ifdef DIRECTX_PC
		// hide show the cursor if we're getting out of a loop
		g_MenuCursor.SetHidden(false);
#endif
		return(WaitForTask(&CLobby::RequestComplete,
				(LoopFunc)&CNetworkLoop::Wait, UNPUBLISH));

	 case UNPUBLISH:
		return(WAIT_FOR_EMIGRATION);

	 case WAIT_FOR_EMIGRATION:
		if (g_session.EmigrationPending())
			return(WAIT_FOR_EMIGRATION);
		if (d_requestExit)
			return(LEAVE_SESSION);
		d_localAgent->SetMode(CPlayerAgent::UNLOADED);
		if (refereePhase == Referee::REPOPULATE)
			return(WAIT_FOR_REPOPULATE);
		return(WAIT_FOR_READY_OR_SETUP);

	 case ENTER_GHOST_READY:
	 case JOIN_GHOST_READY:
#ifdef ALL_JOIN_ROUND
		if (d_requestExit)
			return(LEAVE_SESSION);
		if ((refereePhase == Referee::READY) || (refereePhase == Referee::SETUP))
		{
			d_localAgent->SetMode(CPlayerAgent::UNLOADED);
			return(WAIT_FOR_POPULATE);
		}
		if (refereePhase == Referee::REPORTING)
		{
			d_localAgent->SetMode(CPlayerAgent::UNLOADED);
			return(WAIT_FOR_GHOST_REPORT);
		}
		if (!g_lobby.IsRequestPending())
		{
			g_lobby.JoinRound();
			return(WAIT_FOR_GHOST_READY);
		}
		return(JOIN_GHOST_READY);
#endif

	 case WAIT_FOR_GHOST_READY:
		if (d_requestExit)
			return(LEAVE_ROUND);
		if ((refereePhase == Referee::READY) || (refereePhase == Referee::SETUP) ||
				(refereePhase == Referee::REPORTING))
		{
			d_localAgent->SetMode(CPlayerAgent::UNLOADED);
			return(LEAVE_GHOST_READY);
		}
		if (ProcessPreload())
		{
			d_localAgent->SetMode(CPlayerAgent::LOADED);
			if (g_referee.IsAccepted(d_localAgent->Id()) &&
					!g_lobby.IsRequestPending())
			{
				if (!Load())
					return(LEAVE_ROUND);
				return(START_GHOST);
			}
		}
		else
			d_localAgent->SetMode(CPlayerAgent::UNLOADED);
		if (d_currentScreen != READY_SCREEN)
			GoToReadyScreen();
		return(WAIT_FOR_GHOST_READY);

	 case LEAVE_GHOST_READY:
		if (!g_lobby.IsRequestPending())
		{
			if (refereePhase != Referee::REPORTING)
			{
#ifdef ALL_JOIN_ROUND
				g_lobby.QuitRound();
#endif
				return(WAIT_FOR_POPULATE);
			}
			if (g_referee.GetScoringModel().IsTabulated())
			{
#ifdef ALL_JOIN_ROUND
				if (g_lobby.IsOrphanSession())
					g_lobby.QuitRound();
				else
					g_lobby.EndRound();
#endif
				return(WAIT_FOR_GHOST_REPORT);
			}
		}
		return(LEAVE_GHOST_READY);

	 case WAIT_FOR_GHOST_REPORT:
		if (d_requestExit)
			return(LEAVE_SESSION);
		if (refereePhase != Referee::REPORTING)
			return(WAIT_FOR_POPULATE);
		if (g_referee.IsAccepted(d_localAgent->Id()))
		{
			GoToReportScreen();
			return(WAIT_FOR_READY_OR_SETUP);
		}
		if (d_currentScreen != READY_SCREEN)
			GoToReadyScreen();
		return(WAIT_FOR_GHOST_REPORT);

	 case START_GHOST:
		 if (d_requestExit)
			return(STOP_GHOST);
		if ((refereePhase == Referee::POPULATING) ||
				(refereePhase == Referee::REPOPULATE))
			return(WAIT_TO_RUN_GHOST);
		if (refereePhase == Referee::PLAYING)
			return(ENTER_RUN_GHOST);
		return(STOP_GHOST);

	 case WAIT_TO_RUN_GHOST:
		if (d_requestExit)
			return(STOP_GHOST);
		if (refereePhase == Referee::PLAYING)
			return(ENTER_RUN_GHOST);
		if ((refereePhase != Referee::POPULATING) &&
				(refereePhase != Referee::REPOPULATE))
			return(STOP_GHOST);
		return(WAIT_TO_RUN_GHOST);

	 case ENTER_RUN_GHOST:
		RenderMgr__FadeIn(FADE_TO_BLACK_SLOW);
#ifdef DIRECTX_PC
		g_MenuCursor.SetHidden(true);
#endif
	 case RUN_GHOST:
		if (d_requestExit || (refereePhase != Referee::PLAYING))
			return(STOP_GHOST);
		return(RUN_GHOST);

	 case STOP_GHOST:
#ifdef DIRECTX_PC
		g_MenuCursor.SetHidden(false);
#endif
		if (d_requestExit)
		{
#ifdef ALL_JOIN_ROUND
			g_lobby.QuitRound();
#endif
			GoToLeavingScreen();
		}
		else if (refereePhase == Referee::REPORTING)
		{
#ifdef ALL_JOIN_ROUND
			if (g_lobby.IsOrphanSession())
				g_lobby.QuitRound();
			else
				g_lobby.EndRound();
#endif
			GoToReportScreen();
		}
#ifdef ALL_JOIN_ROUND
		else
			g_lobby.QuitRound();
		return(WaitForTask(&CLobby::RequestComplete,
				(LoopFunc)&CNetworkLoop::Wait, WAIT_FOR_EMIGRATION));
#endif
		return(WAIT_FOR_EMIGRATION);

	 case LEAVE_ROUND:
		if (!g_lobby.IsRequestPending())
		{
			g_lobby.QuitRound();
			return(WaitForTask(&CLobby::RequestComplete,
					(LoopFunc)&CNetworkLoop::Wait, LEAVE_SESSION));
		}
		return(LEAVE_ROUND);

	 case LEAVE_SESSION:
		g_MemCardManager->SetCancelOperation(true);
		GoToLeavingScreen();
		return(WaitForTask((TestMemberFunc)&CNetworkLoop::IsDesiredScreenReady,
				(LoopFunc)&CNetworkLoop::Wait, DISABLE_SESSION));

	 case DISABLE_SESSION:
	 {
		g_CVoiceChat.enableChatChannel( CVoiceChat::CHAT_CHANNEL_ALL, false );
		g_CVoiceChat.enableChatChannel( CVoiceChat::CHAT_CHANNEL_TEAM1, false );
		g_CVoiceChat.enableChatChannel( CVoiceChat::CHAT_CHANNEL_TEAM2, false );
		g_CVoiceChat.enable( false );
		Unload();
		LogMessage("Lock", "Leave", "LogLock");
		d_lockState = LOCK_CLOSE_SESSION;
		g_debugOutput.TSPrint(g_debugOutput.Category("LOCK"),
				"Request Leave Lock");
		if (g_session.RequestLock((CSessionLockRequest*)this))
			return(WAIT_TO_CLOSE_SESSION);
		ASSERT(false);
		d_lockState = LOCK_INACTIVE;
		return(WaitForTask(&CGameEnv::ClearPreload,
				(LoopFunc)&CNetworkLoop::Wait, WAIT_TO_CLOSE_MATCH));
	 }

	 case WAIT_TO_CLOSE_SESSION:
		if ((d_lockState == LOCK_ESTABLISHED) && /*RenderMgr__IsFadedOut() &&*/
				g_gameEnvironment.ClearPreload())
		{
			d_lockState = LOCK_INACTIVE;
			return(WAIT_TO_CLOSE_MATCH);
		}
		return(WAIT_TO_CLOSE_SESSION);

	 case WAIT_TO_CLOSE_MATCH:
		if (g_lobby.IsRequestPending())
			return(WAIT_TO_CLOSE_MATCH);
		g_lobby.CloseMatch();
		return(WaitForTask(&CLobby::RequestComplete,
				(LoopFunc)&CNetworkLoop::Wait, CLOSE_SESSION));

	 case CLOSE_SESSION:
		return(WaitForTask((TestMemberFunc)&CNetworkLoop::ReadyToReset,
				(LoopFunc)&CNetworkLoop::RenderWait, RESET));

	 case RESET:
		g_lobby.Reset();
#ifdef NETZ
		CNetZConnection::TerminateLog();
#endif
		return(UNINITIALIZED);
	}

	// Let any externally defined states pass through.
	return(i_state);
}

/******************************************************************************/
/******************************************************************************/
void CNetworkLoop::LockEstablished(void)
{
	switch(d_lockState)
	{
	 case LOCK_INACTIVE:
		ASSERT(false);
		return;
	 case LOCK_READY:
		g_debugOutput.TSPrint(g_debugOutput.Category("LOCK"),
				"Establish Populate Lock");
		LogMessage("Established", "Ready", "LogLock");
		LogMessage("Populate", "EstLock", "LogLock");
		break;
	 case LOCK_CLOSE_SESSION:
		g_debugOutput.TSPrint(g_debugOutput.Category("LOCK"),
				"Establish Leave Lock");
		LogMessage("Established", "Close", "LogLock");
		break;
	}
	d_lockState = LOCK_ESTABLISHED;
}

/******************************************************************************/
/******************************************************************************/
void CNetworkLoop::Wait(void)
{
#ifndef CONSUMER_BUILD
	d_ttlDebug.InterceptControls();
#endif

	d_sessionMgr.Update();
	d_messageOwner.HandlePostedMessages();
	d_scenes.Run();

#ifndef CONSUMER_BUILD
	d_ttlDebug.Update();
#endif

	TTLRender();
}

/******************************************************************************/
/******************************************************************************/
void CNetworkLoop::RenderWait(void)
{
	TTLRender();
}

/******************************************************************************/
/******************************************************************************/
void CNetworkLoop::OpenSession(void)
{
	g_ControllerRemoved = 0;

	ActorCreator::SetCombatantLOD( COMBATANT_LOD_LOW );

	ParticleMgr__SetOptionalNameExtension( "_MP" );

	// enable/disable gamma on load
	if ( Math::Equal(g_GlobalProfile.GetGammaValue(),PROFILE_DEFAULT_GAMMA))
		TextureUtil::SetGammaOnLoadFlag(false);
	else
		TextureUtil::SetGammaOnLoadFlag(true);

	if( g_lobby.IsLAN() )
		g_FrontEnd.SetGameType( FrontEnd::GAMETYPE_NETWORKGAME );
	else
		g_FrontEnd.SetGameType( FrontEnd::GAMETYPE_ONLINEGAME );

	// Do TTL initialization
	BeginTTLInitialize();

#if 0
	// Put up a sceen to tell the user we know he is sitting there waiting.
	if (!g_lobby.IsLAN())
	{
		GoToSessionScreen();
		TTLRender();
	}
#endif

	// Setup the weapon restrictions for this mode
	g_armoryView.ClearRestrictions();
	g_armoryView.SetGameMode(ArmoryView::Multiplayer);
	g_armoryView.RestrictSuperweapons();

	// Try to open a session.
#ifdef NETZ
	SetSingleton(CSession, (CSession*)(new CTTLSession));
#endif
	d_stateSucceeded = d_sessionMgr.Open(g_lobby.IsLAN());
	if (!d_stateSucceeded)
	{
		TTLTerminate();
		return;
	}

	// Set a default lifetime for reporting.
	g_referee.SetReportingPhaseLifetime(15000);

	// Make sure a teaming model is loaded before any agents are added.
	g_referee.LoadTeamingModel();

	// Get the number of player slots from the config manager.
	g_referee.ConfigureSlots();

	// Initialize the character loader.
	CharacterLoader::Initialize();

	// Try to get the local agent.
	d_localAgent = g_referee.CreateLocalPlayerAgent();
	if (d_localAgent == NULL)
	{
		g_errorStack.SetError("Session", "Failed trying to create player agent",
				CErrorStack::ERROR_FATAL, 5000);
		d_sessionMgr.Close();
		CharacterLoader::Terminate();
		TTLTerminate();
		d_stateSucceeded = false;
		return;
	}

	// Enable the referee phase state machine.
	g_referee.EnablePhaseChanges();

	// Give the referee access to the world loader.
	g_referee.SetWorldLoader(&s_worldLoader);

	// Service the network when idle.
	Display__SetIdleCallback(&ServiceConnection);

	// Register message handlers.
	d_quitHandler.Initialize("Quit", MESSAGE_HANDLE_IF_MASTER,
			d_messageOwner, this);
	d_acceptInvitationHandler.Initialize("AcceptInvitation",
			MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this);

	// Setup debugging.
#ifndef CONSUMER_BUILD
	d_ttlDebug.Initialize(this);
#endif

#ifdef MASSIVE_AD
		//Load in Zone - Hard Coded for testing
		MassiveClientLoadZone(g_gameEnvironment.GetWorldFileName());
#endif
}

/******************************************************************************/
/******************************************************************************/
void CNetworkLoop::EnterReady(void)
{
	// Go to the ready up screen and fade in.
	GoToReadyScreen();
	RenderMgr__FadeIn(FADE_TO_BLACK_FAST);
}

/******************************************************************************/
/******************************************************************************/
void CNetworkLoop::Publish(void)
{
	// I won't believe it if this works
#ifdef DIRECTX_PC
	// Reset this screen hogging bitch.
	g_avatarFX.Reset();
	g_players.Destroy();
#endif

	// Create player.
	CPlayerProfile::EVariant variant =
			g_referee.GetTeamingModel().GetPlayerProfileVariant(d_localAgent->Id());
	g_players.CreateFromStrings(&d_scenes, d_localAgent->Id(),
			PlayerDB::CharacterInitString(g_playerProfile.GetCharacter(variant)),
			g_playerProfile.GetWeaponLoadout(variant));

	// Get the controller.
	d_controller = g_players.GetDevice(0, true);

	// Sync up dynamic collision entities.
	g_collisionSystem.ProtagonistsMoved();

	// Create our custom tags for this round.
	if (ValidSingleton(CustomTag))
		g_customTag.Initialize();
}

/******************************************************************************/
/******************************************************************************/
void CNetworkLoop::EnterRun(void)
{
	// Clear the ready-up screen.
	g_FrontEnd.HideScreen( "SCREEN_ID_NETWORKBK" );
	g_FrontEnd.GotoScreen(0);
	d_currentScreen = NO_SCREEN;

#ifdef DIRECTX_PC
	// Hide the cursor.
	g_MenuCursor.SetHidden(true);
	// so if we don't have a controller locked yet for whatever reason
	// make sure to do so
	//if (!g_input.ControllerLocked(0))
		//g_input.LockPlayerToController( 0, 0 );
#endif

	// Place the avatar.
	g_players.AttachHud(0);
	g_players.RespawnPlayer(0, true, true, true);

	// enable team chat when in game
	g_CVoiceChat.enableChatChannel( CVoiceChat::CHAT_CHANNEL_ALL, false );
	g_CVoiceChat.enableChatChannel( CVoiceChat::CHAT_CHANNEL_TEAM1, false );
	g_CVoiceChat.enableChatChannel( CVoiceChat::CHAT_CHANNEL_TEAM2, false );

	int participantID = ( g_referee.GetLocalPlayerAgent( ) )->Id( );
	if( participantID != INVALID_PARTICIPANT )
	{
		bool blue = g_referee.GetTeamingModel( ).IsPlayerBlue( participantID );
		int teamID = ( blue ? 1 : 2 );
		g_CVoiceChat.enableChatChannel( ( ( CVoiceChat::eChatChannels )teamID ), true );
	}

	// --- set voice enable
	bool VoiceEnabled = g_GlobalProfile.VoiceParams.VoiceOutput != VOICEOUTPUT_DISABLED;
	g_CVoiceChat.enable( VoiceEnabled );

	// An attempt to create the free camera is made in d_ttlDebug.Initialize()
	// but since that is called (in this loop) before the environment is loaded,
	// no free camera is created. Create one here.
	//
	// DO NOT CUT AND PASTE THIS CODE!!!
	//
	// Initialization of scenes should be rethought in the context of async load.
	GetScenes().CreateFreeCameras();

	// Start time.
	SetTimerSpeed(CMasterTimer::ONE_X);

	// Must be running to accept immigration.
	g_session.AcceptImmigration(true);

	// Fade in.
	RenderMgr__FadeIn(FADE_TO_BLACK_SLOW);
}

/******************************************************************************/
/******************************************************************************/
void CNetworkLoop::Run(void)
{
	BeginTTLRun();

#ifndef CONSUMER_BUILD
	d_ttlDebug.InterceptControls();
#endif

	InterceptControls();
	d_sessionMgr.Update();
	EndTTLRun();

#ifndef CONSUMER_BUILD
	d_ttlDebug.Update();
#endif

	TTLRender();
}

/******************************************************************************/
/******************************************************************************/
void CNetworkLoop::ExitRun(void)
{
	// Stop time.
	SetTimerSpeed(CMasterTimer::PAUSE);

	// Delete the hokey cameras created in EnterRun().
	//
	// DO NOT CUT AND PASTE THIS CODE!!!
	//
	// Initialization of scenes should be rethought in the context of async load.
	GetScenes().DeleteFreeCameras();

	// Reset this screen hogging bitch.
	g_avatarFX.Reset();

#ifdef DIRECTX_PC
	g_MenuCursor.SetHidden(false);
#endif

#if 0
	// --- DAS, go to all chat after game over
	g_CVoiceChat.enableChatChannel( CVoiceChat::CHAT_CHANNEL_TEAM1, false );
	g_CVoiceChat.enableChatChannel( CVoiceChat::CHAT_CHANNEL_TEAM2, false );
	g_CVoiceChat.enableChatChannel( CVoiceChat::CHAT_CHANNEL_ALL, false );
	g_CVoiceChat.enableChatChannel( CVoiceChat::CHAT_CHANNEL_ALL, true );

	// --- set voice enable
	bool VoiceEnabled = g_GlobalProfile.VoiceParams.VoiceOutput != VOICEOUTPUT_DISABLED;
	g_CVoiceChat.enable( VoiceEnabled );
#endif

	// Stop accepting immigration.
	g_session.AcceptImmigration(false);

	d_sessionMgr.Update();
}

/******************************************************************************/
/******************************************************************************/
void CNetworkLoop::Unpublish(void)
{
	d_controller = NULL;

	d_localAgent->SetRating(g_lobby.GetLocalPlayerRating());
	d_localAgent->SetRank(g_lobby.GetLocalPlayerRank());

	g_players.Destroy();
	Unload();
}

/******************************************************************************/
/******************************************************************************/
void CNetworkLoop::EnterRunGhost(void)
{
	// Clear the ready-up screen.
	g_FrontEnd.HideScreen( "SCREEN_ID_NETWORKBK" );
	g_FrontEnd.GotoScreen(0);
	d_currentScreen = NO_SCREEN;
	RenderMgr__FadeIn(FADE_TO_BLACK_SLOW);

	// Start time.
	SetTimerSpeed(CMasterTimer::ONE_X);

	// Must be running to accept immigration.
	g_session.AcceptImmigration(true);
}

/******************************************************************************/
/******************************************************************************/
void CNetworkLoop::StartGhost(void)
{
	// Build a camera group and add it to the scene.
	CameraGroup* group = new CameraGroup( "GhostGroup" );
	ASSERT_PTR(group);
	d_scenes.Scene( 0 )->AddGroup( group );

	// Build a camera and add it to the camera group.
	// Build a control mapper
	AvatarControlMapper* control = new AvatarControlMapper;
	ASSERT_PTR( control );
	g_controlFocus.Add( control, 0 );
	control->Initialize();  // Must be added to the control focus list first
	g_controlFocus.Push( control );
	control->SetupFromProfile();
	GhostCamera *camera = new GhostCamera( "GhostCam" , *control );
	ASSERT_PTR( camera );
	camera->setParticipantID( g_referee.GetLocalPlayerAgent()->Id() );
	group->AddTail( camera );
	group->Select( camera );

	// Push the group.
	d_scenes.Scene( 0 )->PushGroup( group );

	// Store the controller.
	d_controller = control->GetDevice();
}

/******************************************************************************/
/******************************************************************************/
void CNetworkLoop::StopGhost(void)
{
	// Stop time.
	SetTimerSpeed(CMasterTimer::PAUSE);

	d_controller = NULL;

	CameraGroup* group = d_scenes.Scene( 0 )->FindGroup( "GhostGroup" );
	CameraGroup* anotherGroup = d_scenes.Scene( 0 )->PopGroup("GhostGroup");
	ASSERT( group == anotherGroup );
	d_scenes.Scene( 0 )->RemoveGroup( group );
	g_controlFocus.Pop("Avatar", 0);
	g_controlFocus.Remove("Avatar", 0);

	// Stop accepting immigration.
	g_session.AcceptImmigration(false);

	Unload();
}

/******************************************************************************/
/******************************************************************************/
void CNetworkLoop::DisableSession(void)
{
	// Disable the referee phase state machine. This stops the state machine from
	// advancing until the referee master migrates.
	g_referee.DisablePhaseChanges();

	g_referee.DestroyLocalPlayerAgent();
}

/******************************************************************************/
/******************************************************************************/
void CNetworkLoop::CloseSession(void)
{
	Display__SetIdleCallback(NULL);
	d_sessionMgr.Close();

	CharacterLoader::Terminate();

#ifndef CONSUMER_BUILD
	d_ttlDebug.Terminate();
#endif

	TTLTerminate();
	g_errorMonitor.EnableFatalDismiss(true);
}

/******************************************************************************/
/******************************************************************************/
void CNetworkLoop::Reset(void)
{
	TTLReset();
}

/******************************************************************************/
/******************************************************************************/
bool CNetworkLoop::ProcessPreload(void)
{
	if ( !g_gameEnvironment.ProcessPreload() )
		return false;
	if ( !CharacterLoader::AllPlayersLoaded() )
		return false;
	return true;
}

/******************************************************************************/
/******************************************************************************/
bool CNetworkLoop::Load(void)
{
	if (!d_loaded)
	{
		// Tags
		NewSingleton(CustomTag);
		NewSingleton(ModeTag);
		g_tag.Init();

		// Load files associated with the environment.
		bool success = TTLLoadEnvironment(1);
		ASSERT(success);

		// World extents
		AABB worldBox = g_collisionSystem.WorldBox();
		d_localAgent->SetLevel(s_worldLoader.GetCRC(),
				s_worldLoader.GetDistributedActorCount(),
				worldBox.minV, worldBox.maxV, 5.0f);

		// Load munitions.
		NewSingleton(TTLMunition);

		// Finish initialization.
		EndTTLInitialize();
		SetTimerSpeed(CMasterTimer::PAUSE);

		g_session.EnablePopulate(true);
		CharacterLoader::PauseLoads();
		d_loaded = true;
	}
	return(true);
}

/******************************************************************************/
/******************************************************************************/
void CNetworkLoop::Unload(void)
{
	if (d_loaded)
	{
		DeleteSingleton(TTLMunition);

		// Cleanup the tags (before unload environment)
		DeleteSingleton(CustomTag);
		DeleteSingleton(ModeTag);

		TTLUnloadEnvironment();
		d_localAgent = g_referee.GetLocalPlayerAgent();
		if (d_localAgent)
			d_localAgent->ClearLevel();
		g_messageDispatcher.Purge();
		g_session.Reopen();

		g_session.EnablePopulate(false);
		CharacterLoader::ResumeLoads();
		d_loaded = false;
	}
}

/******************************************************************************/
/******************************************************************************/
bool CNetworkLoop::InterceptControls(void)
{
	// don't intercei[t controls while the frontend has input focus
	if( g_FrontEnd.GetActiveScreen() )
		return false;

#ifdef _XBOX // don't intercept controls when UIX is taking input focus
	OSIXbox* const osix = (OSIXbox*) OSI::getSingletonPtr();
	if( osix && osix->uixIsProcessingInput() ) 
	{ 
		return false; 
	}
#endif // _XBOX

	// lock out menu on controller pulled
#if defined (_XBOX) || defined (PS2)
	if (g_ControllerRemoved == 1)
		return false;
#endif

	if (d_controller != NULL)
	{
#ifndef DIRECTX_PC
		if (d_controller->ButtonPressed(CB_START))
#else
#ifdef _DEBUG
		if (d_controller->ButtonPressed(CB_START))
#else // _DEBUG
		if (d_controller->ButtonPressed(CB_ESCAPE) &&
			// test against the screen name to prevent us from going back in
			!g_FrontEnd.GetActiveScreenHandler())
#endif // _DEBUG
#endif // _DIRECTX_PC
		{
			g_FrontEnd.ShowScoreBoard( false );

			if( OSI::isCreated() )
			{
#ifdef DIRECTX_PC
				g_FrontEnd.GotoScreen("SCREEN_ID_STARTMENUONLINE_PC");
#else
				g_FrontEnd.GotoScreen("SCREEN_ID_STARTMENUONLINE");
#endif
			}
			else
			{
#ifdef DIRECTX_PC
				g_FrontEnd.GotoScreen("SCREEN_ID_STARTMENUNETWORK_PC");
#else
			#ifdef _XBOX
				g_FrontEnd.GotoScreen("SCREEN_ID_STARTMENUNETWORK_XBOX" );
			#else
				g_FrontEnd.GotoScreen("SCREEN_ID_STARTMENUNETWORK" );
			#endif
#endif
			}
			return(true);
		}
	
		if (d_controller->ButtonPressed(CB_SELECT))
		{
			g_FrontEnd.ShowScoreBoard( true );
			return(true);
		}
		
		if (d_controller->ButtonReleased(CB_SELECT))
		{
			g_FrontEnd.ShowScoreBoard( false );
			return(true);
		}
	}
	return(false);
}

/******************************************************************************/
/******************************************************************************/
void CNetworkLoop::RequestExit(void)
{
	g_messageDispatcher.SendMessageToAll("ExitRequested", NULL,
			INVALID_OBJECT_HANDLE);
	if (ValidSingleton(Referee))
		g_referee.LocalExitting();
	CTTLLoop::RequestExit();
}

/******************************************************************************/
/******************************************************************************/
void CNetworkLoop::CQuit::HandleMessage(void* i_data, ActorHandle i_sender,
		bool i_posted)
{
	d_ref->RequestExit();
}

/******************************************************************************/
/******************************************************************************/
void CNetworkLoop::CAcceptInvitation::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	d_ref->RequestExit();
}

/******************************************************************************/
/******************************************************************************/
void CNetworkLoop::ServiceConnection(void)
{
	if (ValidSingleton(CConnection))
	{
		g_connection.Service();
	}
}

/******************************************************************************/
/******************************************************************************/
void CNetworkLoop::PopulateWorld(void)
{
	s_worldLoader.Distribute();
	if (ValidSingleton(CConnection))
	{
		g_connection.Service();
	}
}

/******************************************************************************/
/******************************************************************************/
bool CNetworkLoop::IsDesiredScreenReady(void)
{
	return(g_FrontEnd.IsScreenReadyForInput());
}

/******************************************************************************/
/******************************************************************************/
bool CNetworkLoop::ReadyToReset(void)
{
	return(g_errorStack.IsEmptyOfExpiring() &&
			g_FrontEnd.IsActiveScreenReadyToExit());
}

/******************************************************************************/
/******************************************************************************/
void CNetworkLoop::GoToSessionScreen(void)
{
	ScreenConnectingToSession* screen = (ScreenConnectingToSession*)
			g_FrontEnd.GetScreenHandler("SCREEN_ID_CONNECTINGTOSESSION",
			ScreenConnectingToSession::ClassnameStatic());
	if (screen)
	{
		if (!ValidSingleton(CSessionSpec))
		{
			screen->SetMessage("Opening Session, Please Wait...");
			screen->SetSessionDescription("");
		}
		else if (g_sessionSpec.GetName() != NULL)
		{
			screen->SetMessage("Creating Session, Please Wait...");
			screen->SetSessionDescription(g_sessionSpec.GetName());
		}
		else
		{
			screen->SetMessage("Joining Session, Please Wait...");
			screen->SetSessionDescription("");
		}
	}
	g_FrontEnd.GotoScreen("SCREEN_ID_CONNECTINGTOSESSION");
	d_currentScreen = SESSION_SCREEN;
}

/******************************************************************************/
/******************************************************************************/
void CNetworkLoop::GoToReadyScreen(void)
{
	g_FrontEnd.ShowScreen( "SCREEN_ID_NETWORKBK" );
	g_FrontEnd.GotoScreen("SCREEN_ID_ONLINELOBBY");
	d_currentScreen = READY_SCREEN;
}

/******************************************************************************/
/******************************************************************************/
void CNetworkLoop::GoToLeavingScreen(void)
{
#if defined(PS2) || defined(_XBOX)
	// Ensure use case stack is empty to the ReadyUp UseCase.
	if ((d_currentScreen == NO_SCREEN) || (d_currentScreen == READY_SCREEN))
	{
		while(OnlineUI::UseCase::getActiveUseCase() &&
				(OnlineUI::UseCase::getActiveUseCase() !=
				OnlineUI::ReadyUp::getSingletonPtr()))
		{
			OnlineUI::UseCase::getActiveUseCase()->stop();
		}
	}
#endif

	if (d_currentScreen != LEAVING_SCREEN)
	{
		g_FrontEnd.GotoScreen("SCREEN_ID_QUITGAME");
		d_currentScreen = LEAVING_SCREEN;
	}
}

/******************************************************************************/
/******************************************************************************/
void CNetworkLoop::GoToReportScreen(void)
{
#if defined(PS2) || defined(_XBOX)
	// Ensure use case stack is empty to the ReadyUp UseCase.
	if ((d_currentScreen == NO_SCREEN) || (d_currentScreen == READY_SCREEN))
	{
		while(OnlineUI::UseCase::getActiveUseCase() &&
				(OnlineUI::UseCase::getActiveUseCase() !=
				OnlineUI::ReadyUp::getSingletonPtr()))
		{
			OnlineUI::UseCase::getActiveUseCase()->stop();
		}
	}
#endif

	if (d_currentScreen != REPORTING_SCREEN)
	{
		g_FrontEnd.GotoScoreBoard();
		d_currentScreen = REPORTING_SCREEN;
	}
}
#endif