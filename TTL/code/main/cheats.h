/////////////////////////////////////////////////////////////////////////////
// Cheats!
/////////////////////////////////////////////////////////////////////////////
#ifndef CHEATS_H
#define CHEATS_H

#include "main/Players.h"
#include "platform/ctroller.h"

#define MAX_CHEAT_PRESSES	10

enum CheatFlags
{
	CHEAT_DONTCOUNTASCHEAT = 0x1,
	CHEAT_DONTRESET = 0x2
};

class Cheats
{
public:
	enum Cheat
	{
		STEALTH,
		INVULNERABLE,
		INFINITEAMMO,
		UNLOCKLEVELS,
		WEAPONMASTER,
		NOCHEATERHUD,
#ifndef CONSUMER_BUILD
		DEBUGMENU,
		FREECAMERA
#endif
	};

	// Is this cheat enabled
	static inline bool Enabled(Cheat cheat)
	{
		return s_cheat[cheat].enabled;
	}

	// Is this the main player?
	static inline bool Enabled(Cheat cheat, CActor *who)
	{
		if (!ValidSingleton(CPlayers) || (CCompActor *)who != g_players.GetPlayer(0))
			return false;
		else
			return s_cheat[cheat].enabled;
	}

	// Is this the main player?
	static inline bool Enabled(Cheat cheat, ActorHandle who)
	{
		if (!ValidSingleton(CPlayers) || CCompActor::FromHandle(who) != g_players.GetPlayer(0))
			return false;
		else
			return s_cheat[cheat].enabled;
	}

	// Reset the state
	static void Reset();

	// Set a cheat state
	static void Set(Cheat cheat, bool enable);

	// How many cheats are there?
	static int Count()
		{ return s_cheats; }

	// How many cheats are enabled?
	static int InUse();

	// Did they cheat?
	static void SignalExternalCheat()
		{ s_externalCheat = true; }

	// Update!
	static void Daemon();

private:
	typedef bool (*CheatFunc)(int info);

	struct Info
	{
		int							flags;
		bool							enabled;
		CheatFunc					cheatFunc;
		const ControllerButton 	*buttonSequence;
		const int					buttons;
	};

	static Info	s_cheat[];
	static int	s_cheats;
	static bool	s_externalCheat;

	// Button stuff
	static ControllerButton	s_buttonHistory[MAX_CHEAT_PRESSES];
	static int s_buttons;
	static uint s_lastPress;

	static void ReadController();
	static void AddButton(ControllerButton button);
	static void ClearButtonSequence()
		{ s_buttons = 0; }
	static bool ScanSequence(const ControllerButton *buttonSquence, int buttons);
	static void Toggle(int cheat);

	static bool CheatEndLevel(int cheat);

	static CMasterTimer::ETimerSpeed s_timeSpeed;
	static bool s_timeChanged;
	static bool CheatTimeFast(int cheat);
	static bool CheatTimeSlow(int cheat);
	static bool CheatTimeReallySlow(int cheat);
	static bool CheatTimeReallyReallySlow(int cheat);

	static bool CheatWireframe(int cheat);
	static bool CheatSpray(int cheat);
	static bool CheatSayStress(int cheat);

	static bool CheatUnlockAll(int cheat);
};

#endif
