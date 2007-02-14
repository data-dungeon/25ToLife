/////////////////////////////////////////////////////////////////////////////
// The armoryview is an API for enumerating and creating loadouts with
// restrictions
/////////////////////////////////////////////////////////////////////////////
#ifndef ARMORYVIEW_H
#define ARMORYVIEW_H

#include "EngineHelper/Singleton.h"
#include "weapons/ttlweapon.h"
#include "Components/weapons/weapdep.h"
#include "weapons/armory.h"

class WeaponLoadout;

class ArmorySelection
{
public:
	inline ArmorySelection()
		{ Clear(); }

	// Clear the selection
	inline void Clear()
		{ for (int s = 0; s < TTLWeapon::SLOTS; s++) m_selected[s] = NULL; }

	// Set/clear a slot selection
	inline void Set(TTLWeapon::Slot slot, const Armory::Description *desc)
		{ m_selected[slot] = desc; }
	inline void Set(const Armory::Description &desc)
		{ m_selected[desc.Slot()] = &desc; }
	inline void Clear(TTLWeapon::Slot slot)
		{ Set(slot, NULL); }

	// Get the selection
	const Armory::Description *Get(TTLWeapon::Slot slot)
		{ return m_selected[slot]; }

private:
	// Our current selection
	const Armory::Description *m_selected[TTLWeapon::SLOTS];
};

class ArmoryView
{
public:
	ArmoryView();

	// Setup the current restrictions
	void ClearRestrictions();

	// Have restrictions changed since you last called this?
	bool RestrictionsChanged();

	// Setup the game mode
	enum GameMode
	{
		Unknown,
		SinglePlayer,
		Multiplayer
	};
	void SetGameMode(GameMode mode);

	// Special restrictions
	void RestrictSuperweapons();
	void UnrestrictSuperweapons();

	// Restrict based on types (see ttlweapon.h)
	void RestrictWeaponType(WeaponType weaponType);
	void UnrestrictWeaponType(WeaponType weaponType);

	// Restrict based on slot
	void RestrictSlot(TTLWeapon::Slot slot);
	void UnrestrictSlot(TTLWeapon::Slot slot);

	// Setup the player type
	enum PlayerType
	{
		HumanCop = 0x01,
		HumanGangster = 0x02,
		AICop = 0x11,
		AIGangster = 0x12
	};

	// Get a default loadout
	static const char *GetDefaultLoadout(PlayerType playerType, int slot);

	int Items(PlayerType playerType, TTLWeapon::Slot slot);
	const Armory::Description *FindFirst(PlayerType playerType, TTLWeapon::Slot slot);
	const Armory::Description *FindNext(PlayerType playerType, TTLWeapon::Slot slot, const Armory::Description *current, bool loop = false);
	const Armory::Description *FindPrevious(PlayerType playerType, TTLWeapon::Slot slot, const Armory::Description *current, bool loop = false);

	// Find this weapon (based on loadout name)
	const Armory::Description *Find(PlayerType playerType, TTLWeapon::Slot slot, const char *loadoutName);
	const Armory::Description *Find(PlayerType playerType, const char *loadoutName);

	// find the default weapon for a slot (searches for best)
	const Armory::Description *FindDefault(PlayerType playerType, TTLWeapon::Slot slot);

	// Find the default for a type/slot
	const Armory::Description *FindSlotDefault(PlayerType playerType, TTLWeapon::Slot slot);
	const Armory::Description *FindTypeDefault(PlayerType playerType, TTLWeapon::Slot slot, WeaponType type);

	// Setup the selected items (for loadout)
	void ClearSelection(PlayerType playerType);
	void DefaultSelection(PlayerType playerType, bool emptiesOnly = false);
	void SetSelection(PlayerType playerType, TTLWeapon::Slot slot, const Armory::Description *desc);
	const Armory::Description *GetSelection(PlayerType playerType, TTLWeapon::Slot slot);

	void SetSelection(PlayerType playerType, WeaponLoadout &loadout, bool forceValid = true);

	// Create the loadout from the current selection
	void CreateFromSelection(PlayerType playerType, WeaponLoadout &loadout);

	// Utility to make a param set weapon inventory legal
	bool Legalize(PlayerType playerType, WeaponLoadout &loadout);

private:
	// Our current setup
	bool				m_changed;
	GameMode			m_mode;
	bool				m_allowSuperweapons;
	int				m_slotRestriction;
	int				m_typeRestriction;

	// The current selection (We track both types)
	ArmorySelection m_copSelection;
	ArmorySelection m_gangsterSelection;
	ArmorySelection *GetSelection(PlayerType playerType);

	// Is this description legal given the restrictions and player type?
	bool IsLegal(PlayerType playerType, const Armory::Description &desc);
	const Armory::Description *FindLegal(PlayerType playerType, const Armory::Description &basedOn);
};

class ArmoryDependency : public WeaponDependency
{
public:
	ArmoryDependency() {}

	void InitializeOnce();
	void TerminateOnce();

	WeaponDependency::Phase WantPhase()
		{ return WeaponDependency::POSTINIT; }
};

DefineSingleton(ArmoryView)
#define g_armoryView GetSingleton(ArmoryView)

#endif
