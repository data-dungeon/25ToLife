/////////////////////////////////////////////////////////////////////////////
// Cheats!
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "Network/Session.h"
#include "enginehelper/sfx.h"
#include "enginehelper/timer.h"
#include "platform/systemi.h"
#include "main/gameenv.h"
#include "frontend/savegame.h"

#ifndef CONSUMER_BUILD
const ControllerButton debugButtonSequence[] = { CB_LEFT, CB_RIGHT, CB_LEFT, CB_RIGHT, CB_UP, CB_DOWN, CB_UP, CB_DOWN, CB_O };
const int debugButtons = sizeof(debugButtonSequence) / sizeof(ControllerButton);
const ControllerButton freeCamButtonSequence[] = { CB_LEFT, CB_RIGHT, CB_LEFT, CB_RIGHT, CB_UP, CB_DOWN, CB_UP, CB_DOWN, CB_TRI };
const int freeCamButtons = sizeof(freeCamButtonSequence) / sizeof(ControllerButton);
#endif

const ControllerButton stealthButtonSequence[] = { CB_UP, CB_DOWN, CB_UP, CB_DOWN, CB_LEFT, CB_RIGHT, CB_O };
const int stealthButtons = sizeof(stealthButtonSequence) / sizeof(ControllerButton);

const ControllerButton godButtonSequence[] = { CB_UP, CB_DOWN, CB_UP, CB_DOWN, CB_LEFT, CB_RIGHT, CB_TRI };
const int godButtons = sizeof(godButtonSequence) / sizeof(ControllerButton);

const ControllerButton ammoButtonSequence[] = { CB_UP, CB_DOWN, CB_UP, CB_DOWN, CB_LEFT, CB_RIGHT, CB_SQUARE };
const int ammoButtons = sizeof(ammoButtonSequence) / sizeof(ControllerButton);

const ControllerButton unlockLevelsButtonSequence[] = { CB_UP, CB_DOWN, CB_UP, CB_DOWN, CB_LEFT, CB_RIGHT, CB_STICK2 };
const int unlockLevelsButtons = sizeof(unlockLevelsButtonSequence) / sizeof(ControllerButton);

const ControllerButton weaponMasterButtonSequence[] = { CB_UP, CB_DOWN, CB_UP, CB_DOWN, CB_LEFT, CB_RIGHT, CB_LEFT, CB_RIGHT, CB_R1 };
const int weaponMasterButtons = sizeof(weaponMasterButtonSequence) / sizeof(ControllerButton);

const ControllerButton noCheatHUDButtonSequence[] = { CB_UP, CB_DOWN, CB_UP, CB_DOWN, CB_LEFT, CB_RIGHT, CB_LEFT, CB_RIGHT, CB_SELECT };
const int noCheatHUDButtons = sizeof(noCheatHUDButtonSequence) / sizeof(ControllerButton);

const ControllerButton skipLevelButtonSequence[] = { CB_UP, CB_DOWN, CB_UP, CB_DOWN, CB_LEFT, CB_RIGHT, CB_STICK1 };
const int skipLevelButtons = sizeof(skipLevelButtonSequence) / sizeof(ControllerButton);

const ControllerButton fastTimeButtonSequence[] = { CB_UP, CB_DOWN, CB_UP, CB_DOWN, CB_LEFT, CB_RIGHT, CB_R1 };
const int fastTimeButtons = sizeof(fastTimeButtonSequence) / sizeof(ControllerButton);

const ControllerButton halfTimeButtonSequence[] = { CB_UP, CB_DOWN, CB_UP, CB_DOWN, CB_LEFT, CB_RIGHT, CB_R2 };
const int halfTimeButtons = sizeof(halfTimeButtonSequence) / sizeof(ControllerButton);

const ControllerButton quarterTimeButtonSequence[] = { CB_UP, CB_DOWN, CB_UP, CB_DOWN, CB_LEFT, CB_RIGHT, CB_L1 };
const int quarterTimeButtons = sizeof(quarterTimeButtonSequence) / sizeof(ControllerButton);

const ControllerButton eighthTimeButtonSequence[] = { CB_UP, CB_DOWN, CB_UP, CB_DOWN, CB_LEFT, CB_RIGHT, CB_L2 };
const int eighthTimeButtons = sizeof(eighthTimeButtonSequence) / sizeof(ControllerButton);

const ControllerButton wireButtonSequence[] = { CB_UP, CB_DOWN, CB_UP, CB_DOWN, CB_LEFT, CB_RIGHT, CB_LEFT, CB_RIGHT, CB_X };
const int wireButtons = sizeof(wireButtonSequence) / sizeof(ControllerButton);

const ControllerButton sprayButtonSequence[] = { CB_UP, CB_DOWN, CB_UP, CB_DOWN, CB_LEFT, CB_RIGHT, CB_LEFT, CB_RIGHT, CB_UP };
const int sprayButtons = sizeof(sprayButtonSequence) / sizeof(ControllerButton);

const ControllerButton sayStressButtonSequence[] = { CB_UP, CB_DOWN, CB_LEFT, CB_RIGHT, CB_UP, CB_DOWN, CB_UP, CB_DOWN, CB_UP, CB_UP };
const int sayStressButtons = sizeof(sayStressButtonSequence) / sizeof(ControllerButton);

const ControllerButton unlockAllButtonSequence[] = { CB_UP, CB_DOWN, CB_LEFT, CB_RIGHT, CB_UP, CB_DOWN, CB_X, CB_TRI, CB_SQUARE, CB_O };
const int unlockAllButtons = sizeof(unlockAllButtonSequence) / sizeof(ControllerButton);

Cheats::Info Cheats::s_cheat[] =
{
	/* "Stealth", 0, */ 		{ 0, false, NULL, stealthButtonSequence, stealthButtons },
	/* "God", 0,  */ 			{ 0, false, NULL, godButtonSequence, godButtons },
	/* "Ammo", 0, */ 			{ 0, false, NULL, ammoButtonSequence, ammoButtons },
	/* "Unlock Levels", */ 	{ CHEAT_DONTCOUNTASCHEAT | CHEAT_DONTRESET, false, NULL, unlockLevelsButtonSequence, unlockLevelsButtons },
	/* "Weapon Master", */ 	{ CHEAT_DONTCOUNTASCHEAT, false, NULL, weaponMasterButtonSequence, weaponMasterButtons },
	/* "No cheater */ 		{ CHEAT_DONTCOUNTASCHEAT, false, NULL, noCheatHUDButtonSequence, noCheatHUDButtons },
#ifndef CONSUMER_BUILD
	/* "Debug menu", */ 		{ CHEAT_DONTCOUNTASCHEAT | CHEAT_DONTRESET, false, NULL, debugButtonSequence, debugButtons },
	/* "Free Camera", */ 	{ CHEAT_DONTCOUNTASCHEAT | CHEAT_DONTRESET, false, NULL, freeCamButtonSequence, freeCamButtons },
#endif
	/* "Skip level", */ 		{ CHEAT_DONTCOUNTASCHEAT, false, Cheats::CheatEndLevel, skipLevelButtonSequence, skipLevelButtons },
	/* "Fast Times", */ 		{ CHEAT_DONTCOUNTASCHEAT, false, Cheats::CheatTimeFast, fastTimeButtonSequence, fastTimeButtons },
	/* "Half Times", */ 		{ CHEAT_DONTCOUNTASCHEAT, false, Cheats::CheatTimeSlow, halfTimeButtonSequence, halfTimeButtons },
	/* "Quarter Times", */ 	{ CHEAT_DONTCOUNTASCHEAT, false, Cheats::CheatTimeReallySlow, quarterTimeButtonSequence, quarterTimeButtons },
	/* "Eighth Times", */ 	{ CHEAT_DONTCOUNTASCHEAT, false, Cheats::CheatTimeReallyReallySlow, eighthTimeButtonSequence, eighthTimeButtons },
	/* "Wireframe", */		{ CHEAT_DONTCOUNTASCHEAT, false, Cheats::CheatWireframe, wireButtonSequence, wireButtons },
	/* "Spray", */ 			{ CHEAT_DONTCOUNTASCHEAT | CHEAT_DONTRESET, false, Cheats::CheatSpray, sprayButtonSequence, sprayButtons },
	/* "SayStress", */		{ CHEAT_DONTCOUNTASCHEAT, false, Cheats::CheatSayStress, sayStressButtonSequence, sayStressButtons },
	/* "Unlockall", */ 		{ CHEAT_DONTCOUNTASCHEAT | CHEAT_DONTRESET, false, Cheats::CheatUnlockAll, unlockAllButtonSequence, unlockAllButtons },
};
int Cheats::s_cheats = sizeof(s_cheat) / sizeof(Cheats::Info);
bool Cheats::s_externalCheat = false;

ControllerButton Cheats::s_buttonHistory[MAX_CHEAT_PRESSES];
int Cheats::s_buttons = 0;
uint Cheats::s_lastPress = 0;

bool Cheats::s_timeChanged = false;
CMasterTimer::ETimerSpeed Cheats::s_timeSpeed = CMasterTimer::ONE_X;

/////////////////////////////////////////////////////////////////////////////
// Reset the state
/////////////////////////////////////////////////////////////////////////////
void Cheats::Reset()
{
	for (int i = 0; i < s_cheats; i++)
	{
		if ((s_cheat[i].flags & CHEAT_DONTRESET) == 0)
			s_cheat[i].enabled = false;
	}
	s_externalCheat = false;

#ifndef CDROM
	Set(Cheats::DEBUGMENU, true);
	if (!ValidSingleton(CSession) || !g_session.IsNetworked())
		Set(Cheats::FREECAMERA, true);
	Set(Cheats::UNLOCKLEVELS, true);
#endif

	s_timeChanged = false;
	s_timeSpeed = CMasterTimer::ONE_X;

	ClearButtonSequence();
}

/////////////////////////////////////////////////////////////////////////////
// Set a cheat state
/////////////////////////////////////////////////////////////////////////////
void Cheats::Set(
Cheat cheat,
bool enable)
{
#ifdef CONSUMER_BUILD
	if (ValidSingleton(CSession) && g_session.IsNetworked())
		return;
#else
	if (ValidSingleton(CSession) && g_session.IsNetworked() && cheat != FREECAMERA)
		return;
#endif
	s_cheat[cheat].enabled = enable;
}

/////////////////////////////////////////////////////////////////////////////
// How many cheats are enabled?
/////////////////////////////////////////////////////////////////////////////
int Cheats::InUse()
{
	int count = s_externalCheat ? 1 : 0;
	for (int i = 0; i < s_cheats; i++)
	{
		if ((s_cheat[i].flags & CHEAT_DONTCOUNTASCHEAT) == 0 && s_cheat[i].enabled)
			++count;
	}

	return count;
}

/////////////////////////////////////////////////////////////////////////////
// Update!
/////////////////////////////////////////////////////////////////////////////
void Cheats::Daemon()
{
#ifdef CONSUMER_BUILD
	// Get out!
	if (ValidSingleton(CSession) && g_session.IsNetworked())
	{
		Reset();
		s_cheat[INFINITEAMMO].enabled = false; // cheating fag
		return;
	}
#endif

	// Read the contoller
	ReadController();

	// Keep resetting them
	const uint resetSequenceMS = 500;
	if (s_lastPress + resetSequenceMS < g_timer.GetOSEndMS())
	 	ClearButtonSequence();

	// Did they get one?
	for (int i = 0; i < s_cheats; i++)
	{
		if (ScanSequence(s_cheat[i].buttonSequence, s_cheat[i].buttons))
		{
			Toggle(i);
			ClearButtonSequence();
		}
	}

	// Apply the time cheat
	if (!ValidSingleton(CSession) || !g_session.IsNetworked())
	{
		if (!g_timer.IsPaused())
		{
			if (s_timeChanged || (g_timer.GetSpeed() != s_timeSpeed && s_timeSpeed != CMasterTimer::ONE_X))
			{
				g_messageDispatcher.SendMessageToAll("TimeSpeed", CMessageDispatcher::IntToData(s_timeSpeed), INVALID_OBJECT_HANDLE);
				s_timeChanged = false;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Cheats::ReadController()
{
#ifdef CONSUMER_BUILD
	if (ValidSingleton(CSession) && g_session.IsNetworked())
		return;
#endif

	if (!g_inputPtr)
		return;

	ControllerInputDriver *controller = NULL;
	if (g_input.ControllerLocked(0))
 		controller = g_input.GetController(0);
	else
		controller = g_input.GetUnlockedController(0);
	if (!controller)
		return;

	const ControllerButton careAbout[] = { CB_UP, CB_DOWN, CB_LEFT, CB_RIGHT, CB_SQUARE, CB_X, CB_O, CB_TRI, CB_L1, CB_R1, CB_L2, CB_R2, CB_START, CB_SELECT, CB_STICK1, CB_STICK2 };
	const int careAbouts = sizeof(careAbout) / sizeof(ControllerButton);
	for (int b = 0; b < careAbouts; b++)
	{
		if (controller->ButtonPressed(careAbout[b]))
		{
			AddButton(careAbout[b]);
			s_lastPress = g_timer.GetOSEndMS();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void DumpButtonString(const char *title,
const ControllerButton *buttonSequence,
int buttons)
{
#if 0//defined(FOOD) && defined(_DEBUG)
	TRACE(title);
	for (int b = 0; b < buttons; b++)
	{
		switch (buttonSequence[b])
		{
			case CB_UP:
				TRACE("CB_UP ");
				break;
			case CB_DOWN:
				TRACE("CB_DOWN ");
				break;
			case CB_LEFT:
				TRACE("CB_LEFT ");
				break;
			case CB_RIGHT:
				TRACE("CB_RIGHT ");
				break;
			case CB_SQUARE:
				TRACE("CB_SQUARE ");
				break;
			case CB_X:
				TRACE("CB_X ");
				break;
			case CB_O:
				TRACE("CB_O ");
				break;
			case CB_TRI:
				TRACE("CB_TRI ");
				break;
			case CB_L1:
				TRACE("CB_L1 ");
				break;
			case CB_R1:
				TRACE("CB_R1 ");
				break;
			case CB_L2:
				TRACE("CB_L2 ");
				break;
			case CB_R2:
				TRACE("CB_R2 ");
				break;
			case CB_START:
				TRACE("CB_START ");
				break;
			case CB_SELECT:
				TRACE("CB_SELECT ");
				break;
			case CB_STICK1:
				TRACE("CB_STICK1 ");
				break;
			case CB_STICK2:
				TRACE("CB_STICK2 ");
				break;
			default:
				TRACE("INVALID ");
				break;
		}
	}
	TRACE("\n");
#endif
}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Cheats::AddButton(ControllerButton button)
{
#ifdef CONSUMER_BUILD
	if (ValidSingleton(CSession) && g_session.IsNetworked())
		return;
#endif

	// Do we need to shif?
	if (s_buttons < MAX_CHEAT_PRESSES)
		s_buttonHistory[s_buttons++] = button;
	else
	{
		// Move them down
		memcpy(&s_buttonHistory[0], &s_buttonHistory[1], sizeof(ControllerButton) * (MAX_CHEAT_PRESSES - 1));
		s_buttonHistory[MAX_CHEAT_PRESSES - 1] = button;
	}

	DumpButtonString("Device: ", s_buttonHistory, s_buttons);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool Cheats::ScanSequence(
const ControllerButton *buttonSequence,
int buttons)
{
#ifdef CONSUMER_BUILD
	if (ValidSingleton(CSession) && g_session.IsNetworked())
		return false;
#endif

	if (!buttonSequence || buttons > s_buttons)
		return false;

	int start = s_buttons - buttons;
	for (int c = 0; c < buttons; c++)
	{
		if (s_buttonHistory[c + start] != buttonSequence[c])
			return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Set a cheat state
/////////////////////////////////////////////////////////////////////////////
void Cheats::Toggle(
int cheat)
{
#ifdef CONSUMER_BUILD
	if (ValidSingleton(CSession) && g_session.IsNetworked())
	{
		s_cheat[cheat].enabled = false;
		return;
	}
#else
	// Free cam is the only cheat allowed in network
	if (ValidSingleton(CSession) && g_session.IsNetworked() && (cheat != FREECAMERA))
		return;
#endif

	// You can't do this
	if (g_gameEnvironment.BonusLevel() && (cheat == STEALTH || cheat == INVULNERABLE))
		return;

	if (!s_cheat[cheat].cheatFunc)
		s_cheat[cheat].enabled = !s_cheat[cheat].enabled;
	else
		s_cheat[cheat].enabled = s_cheat[cheat].cheatFunc(cheat);

	// Play the sound
	if (s_cheat[cheat].enabled)
		Sfx::Play2D("cheat_on");
	else
		Sfx::Play2D("cheat_off");

#if defined(FOOD) && defined(_DEBUG)
	TRACE("Cheat %d changed to %d\n", cheat, s_cheat[cheat].enabled);
#endif
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool Cheats::CheatEndLevel(int cheat)
{
	if (ValidSingleton(CSession) && g_session.IsNetworked())
		return false;

	// Make sure we are in an SP level
	int currentLevel = g_gameEnvironment.Get();
	if (currentLevel < 0 || currentLevel > 11)
		return false;

	int nextLevel = g_gameEnvironment.GetByType(currentLevel + 1, CGameEnv::TYPE_SINGLEPLAYER);
	const char *wantLevel;
	if (g_gameEnvironment.EndOfGame() || nextLevel < 0)
		wantLevel = "quit";
	else
	{
		wantLevel = g_gameEnvironment.GetWorldFileName(nextLevel);
		Cheats::SignalExternalCheat();
	}
	g_messageDispatcher.SendMessageToAll("SwitchLevel", (void *)wantLevel, (ActorHandle)INVALID_OBJECT_HANDLE);

	// This never really "turns on"
	return false;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool Cheats::CheatTimeFast(int cheat)
{
	if (ValidSingleton(CSession) && g_session.IsNetworked())
		return false;

	const CMasterTimer::ETimerSpeed wantSpeed = CMasterTimer::TWO_X;
	if (s_timeSpeed == wantSpeed)
		s_timeSpeed = CMasterTimer::ONE_X;
	else
		s_timeSpeed = wantSpeed;
	s_timeChanged = true;

	return (s_timeSpeed != CMasterTimer::ONE_X);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool Cheats::CheatTimeSlow(int cheat)
{
	if (ValidSingleton(CSession) && g_session.IsNetworked())
		return false;

	const CMasterTimer::ETimerSpeed wantSpeed = CMasterTimer::ONE_HALF;
	if (s_timeSpeed == wantSpeed)
		s_timeSpeed = CMasterTimer::ONE_X;
	else
		s_timeSpeed = wantSpeed;
	s_timeChanged = true;

	return (s_timeSpeed != CMasterTimer::ONE_X);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool Cheats::CheatTimeReallySlow(int cheat)
{
	if (ValidSingleton(CSession) && g_session.IsNetworked())
		return false;

	const CMasterTimer::ETimerSpeed wantSpeed = CMasterTimer::ONE_FOURTH;
	if (s_timeSpeed == wantSpeed)
		s_timeSpeed = CMasterTimer::ONE_X;
	else
		s_timeSpeed = wantSpeed;
	s_timeChanged = true;

	return (s_timeSpeed != CMasterTimer::ONE_X);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool Cheats::CheatTimeReallyReallySlow(int cheat)
{
	if (ValidSingleton(CSession) && g_session.IsNetworked())
		return false;

	const CMasterTimer::ETimerSpeed wantSpeed = CMasterTimer::ONE_EIGHTH;
	if (s_timeSpeed == wantSpeed)
		s_timeSpeed = CMasterTimer::ONE_X;
	else
		s_timeSpeed = wantSpeed;
	s_timeChanged = true;

	return (s_timeSpeed != CMasterTimer::ONE_X);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool Cheats::CheatWireframe(int cheat)
{
#if !defined(CONSUMER_BUILD)
	RenderMgr__ToggleWireframe();
#endif
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
extern const char *g_overrideSound;
bool Cheats::CheatSpray(int cheat)
{
	if (!g_overrideSound)
		g_overrideSound = "spray_start";
	else
		g_overrideSound = NULL;

	return !g_overrideSound;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
extern bool g_sayStress;
bool Cheats::CheatSayStress(int cheat)
{
	g_sayStress = !g_sayStress;
	return g_sayStress;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool Cheats::CheatUnlockAll(int cheat)
{
	for (int l = 0; l < 12; l++)
		g_GlobalProfile.SetCompletedLevel(l, true);
	g_GlobalProfile.SetPreviousLevel(11);

	return true;
}

