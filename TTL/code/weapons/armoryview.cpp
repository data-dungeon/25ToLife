/////////////////////////////////////////////////////////////////////////////
// The armory is an API for enumerating and creating loadouts
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "weapons/armory.h"
#include "weapons/armoryview.h"

volatile ArmoryDependency g_armoryDependency;

DeclareSingleton(ArmoryView);

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
ArmoryView::ArmoryView()
{
	ClearRestrictions();
}

/////////////////////////////////////////////////////////////////////////////
// Setup the current restrictions
/////////////////////////////////////////////////////////////////////////////
void ArmoryView::ClearRestrictions()
{
	m_slotRestriction = 0;
	m_typeRestriction = 0;
	m_mode = Unknown;
 	m_allowSuperweapons = true;

	m_changed = true;
}

/////////////////////////////////////////////////////////////////////////////
// Have restrictions changed since you last called this?
/////////////////////////////////////////////////////////////////////////////
bool ArmoryView::RestrictionsChanged()
{
	if (!m_changed)
		return false;

	m_changed = false;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Setup the game mode
/////////////////////////////////////////////////////////////////////////////
void ArmoryView::SetGameMode(
GameMode mode)
{
	m_changed = (m_changed || (m_mode != mode));
	m_mode = mode;
}

/////////////////////////////////////////////////////////////////////////////
// Special restrictions
/////////////////////////////////////////////////////////////////////////////
void ArmoryView::RestrictSuperweapons()
{
	m_changed = (m_changed || !m_allowSuperweapons);
	m_allowSuperweapons = true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void ArmoryView::UnrestrictSuperweapons()
{
	m_changed = (m_changed || m_allowSuperweapons);
	m_allowSuperweapons = false;
}

/////////////////////////////////////////////////////////////////////////////
// Restrict based on types (see ttlweapon.h)
/////////////////////////////////////////////////////////////////////////////
void ArmoryView::RestrictWeaponType(
WeaponType weaponType)
{
	ASSERT(weaponType >= 0 && weaponType < 32);

	int old = m_typeRestriction;
	m_typeRestriction |= (1 << weaponType);
	m_changed = (m_changed || (old != m_typeRestriction));
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void ArmoryView::UnrestrictWeaponType(
WeaponType weaponType)
{
	ASSERT(weaponType >= 0 && weaponType < 32);

	int old = m_typeRestriction;
	m_typeRestriction &= ~(1 << weaponType);
	m_changed = (m_changed || (old != m_typeRestriction));
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void ArmoryView::RestrictSlot(
TTLWeapon::Slot slot)
{
	int old = m_slotRestriction;
	m_slotRestriction |= (1 << (int)slot);
	m_changed = (m_changed || (old != m_slotRestriction));
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void ArmoryView::UnrestrictSlot(
TTLWeapon::Slot slot)
{
	int old = m_slotRestriction;
	m_slotRestriction &= ~(1 << (int)slot);
	m_changed = (m_changed || (old != m_slotRestriction));
}

/////////////////////////////////////////////////////////////////////////////
// Get a default loadout
/////////////////////////////////////////////////////////////////////////////
const char *ArmoryView::GetDefaultLoadout(PlayerType playerType, int slot)
{
	const char *defaultCop[] =
	{
		"weapon0=baton; weapon1=glocklaser; weapon2=m4a1; weapon3=TearGas",
		"weapon0=asp; weapon1=deaglelaser; weapon2=moss590; weapon3=Concussion",
		"weapon0=tasermp; weapon1=glock2; weapon2=mp5; weapon3=Flash",
		"weapon0=stungun; weapon1=mp5kmp; weapon2=SwatSniper; weapon3=Smoke"
	};
	const int defaultCops = sizeof(defaultCop) / sizeof(const char *);
	const char *defaultGang[] =
	{
		"weapon0=bottle; weapon1=45compact; weapon2=ak74; weapon3=M61",
		"weapon0=bat; weapon1=doubletec9; weapon2=sawedoff; weapon3=Molotov",
		"weapon0=tireiron; weapon1=deagle; weapon2=dualmac10; weapon3=Pipebomb",
		"weapon0=bowie; weapon1=mac10; weapon2=M1A; weapon3=Smoke"
	};
	const int defaultGangs = sizeof(defaultGang) / sizeof(const char *);

	if (playerType == HumanCop || playerType == AICop)
		return defaultCop[slot % defaultCops];
	else
		return defaultGang[slot % defaultGangs];
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
int ArmoryView::Items(
PlayerType playerType,
TTLWeapon::Slot slot)
{
	int items = 0;

	const Armory::Description *desc = FindFirst(playerType, slot);
	while (desc)
	{
		if (IsLegal(playerType, *desc))
			++items;
		desc = FindNext(playerType, slot, desc);
	}

	return items;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
const Armory::Description *ArmoryView::FindFirst(
PlayerType playerType,
TTLWeapon::Slot slot)
{
	const Armory::Description *first = g_armory.Get(slot, 0);

	// Scan the list for a legal
	while (first && !IsLegal(playerType, *first))
		first = g_armory.Get(slot, first->Index() + 1);

	return first;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
const Armory::Description *ArmoryView::FindNext(
PlayerType playerType,
TTLWeapon::Slot slot,
const Armory::Description *current,
bool loop)
{
	// Doh!
	ASSERT(current);

	const Armory::Description *start = current;
	while (current)
	{
		// Are we at the end?
		int index = current->Index() + 1;
		if (index >= g_armory.Items(slot))
		{
			// IF we aren't looping, we are done
			if (loop)
				index = 0;
			else
				return NULL;
		}

		// Go forward
		current = g_armory.Get(slot, index);

		// have we wrapped without returning something valid?
		if (current == start)
			return NULL;

		// Scan the list for a legal
		if (IsLegal(playerType, *current))
			break;
	}

	// Give it to 'em
	return current;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
const Armory::Description *ArmoryView::FindPrevious(
PlayerType playerType,
TTLWeapon::Slot slot,
const Armory::Description *current,
bool loop)
{
	// Doh!
	ASSERT(current);

	const Armory::Description *start = current;
	while (current)
	{
		// Are we at the end?
		int index = current->Index() - 1;
		if (index < 0)
		{
			// IF we aren't looping, we are done
			if (loop)
				index = g_armory.Items(slot) - 1;
			else
				return NULL;
		}

		// Go forward
		current = g_armory.Get(slot, index);

		// have we wrapped without returning something valid?
		if (current == start)
			return NULL;

		// Scan the list for a legal
		if (IsLegal(playerType, *current))
			break;
	}

	// Give it to 'em
	return current;
}

/////////////////////////////////////////////////////////////////////////////
// Find this weapon (based on loadout name)
/////////////////////////////////////////////////////////////////////////////
const Armory::Description *ArmoryView::Find(
PlayerType playerType,
TTLWeapon::Slot slot,
const char *loadoutName)
{
	// Traverse
	const Armory::Description *desc = FindFirst(playerType, slot);
	while (desc)
	{
		// Is this it?
		if (strcmpi(loadoutName, desc->LoadoutName()) == 0)
			break;

		// Wee!
		desc = FindNext(playerType, slot, desc);
	}

	return desc;
}

/////////////////////////////////////////////////////////////////////////////
// Find this weapon (based on loadout name)
/////////////////////////////////////////////////////////////////////////////
const Armory::Description *ArmoryView::Find(
PlayerType playerType,
const char *loadoutName)
{
	const Armory::Description *desc = NULL;
	for (int s = 0; s < TTLWeapon::SLOTS; s++)
	{
		TTLWeapon::Slot slot = (TTLWeapon::Slot)s;

		desc = Find(playerType, slot, loadoutName);
		if (desc)
			break;
	}

	return desc;
}

/////////////////////////////////////////////////////////////////////////////
// find the default weapon for a slot (searches for best)
/////////////////////////////////////////////////////////////////////////////
const Armory::Description *ArmoryView::FindDefault(
PlayerType playerType,
TTLWeapon::Slot slot)
{
	// Try to find the default based on restrictions
	const Armory::Description *best = FindSlotDefault(playerType, slot);
	if (best)
		return best;

	// Now try each type
	const WeaponType searchType[] = { WEAPONTYPE_ASSAULT, WEAPONTYPE_SUB, WEAPONTYPE_SHOTGUN,
													WEAPONTYPE_LAUNCHER, WEAPONTYPE_SNIPER,
													WEAPONTYPE_LOBBED, WEAPONTYPE_PISTOL, WEAPONTYPE_NONE };
	int search = 0;
	while (searchType[search] != WEAPONTYPE_NONE)
	{
		best = FindTypeDefault(playerType, slot, searchType[search]);
		if (best)
			return best;

		++search;
	}

	// Nothing fits!
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// find the default weapon for a slot (can be null)
/////////////////////////////////////////////////////////////////////////////
const Armory::Description *ArmoryView::FindSlotDefault(
PlayerType playerType,
TTLWeapon::Slot slot)
{
	// Try to find the default based on restrictions
	const Armory::Description *desc = FindFirst(playerType, slot);
	if (!desc)
		return NULL;

	// First find the global default
	while (desc)
	{
		// Is this legal?
		if (desc->Properties().Test(TTLWeaponProperties::SLOTDEFAULT))
			break;

		desc = FindNext(playerType, slot, desc);
	}

	return desc;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
const Armory::Description *ArmoryView::FindTypeDefault(
PlayerType playerType,
TTLWeapon::Slot slot,
WeaponType type)
{
	// Try to find the default based on restrictions
	const Armory::Description *desc = FindFirst(playerType, slot);
	if (!desc)
		return NULL;

	// First find the global default
	while (desc)
	{
		// Is this legal?
		if (desc->Properties().Test(TTLWeaponProperties::TYPEDEFAULT))
			break;

		desc = FindNext(playerType, slot, desc);
	}

	return desc;
}

/////////////////////////////////////////////////////////////////////////////
// Setup the selected items (for loadout)
/////////////////////////////////////////////////////////////////////////////
void ArmoryView::ClearSelection(
PlayerType playerType)
{
	ArmorySelection *selection = GetSelection(playerType);
	if (!selection)
		return;

	// Do it!
	selection->Clear();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void ArmoryView::DefaultSelection(
PlayerType playerType,
bool emptiesOnly)
{
 	ArmorySelection *selection = GetSelection(playerType);
	if (!selection)
		return;

	// Wee!
	for (int s = 0; s < TTLWeapon::SLOTS; s++)
	{
		const TTLWeapon::Slot slot = (TTLWeapon::Slot)s;

		if (!emptiesOnly || !selection->Get(slot))
			selection->Set(slot, FindDefault(playerType, slot));
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void ArmoryView::SetSelection(
PlayerType playerType,
TTLWeapon::Slot slot,
const Armory::Description *desc)
{
 	ArmorySelection *selection = GetSelection(playerType);
	if (selection)
	 	selection->Set(slot, desc);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
const Armory::Description *ArmoryView::GetSelection(
PlayerType playerType,
TTLWeapon::Slot slot)
{
 	ArmorySelection *selection = GetSelection(playerType);
	if (selection)
 		return selection->Get(slot);
	else
		return NULL;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void ArmoryView::SetSelection(
PlayerType playerType,
WeaponLoadout &loadout,
bool forceValid)
{
	// Get the selection
 	ArmorySelection *selection = GetSelection(playerType);
	if (!selection)
		return;

	// Clear it
	selection->Clear();

	// Go through each slot
	for (int w = 0; w < loadout.Weapons(); w++)
	{
		const char *loadoutName = loadout.GetWeapon(w);
		if (loadoutName)
		{
			const Armory::Description *desc = Find(playerType, loadoutName);
			if (desc)
				selection->Set(*desc);
		}
	}

	// Fill empties?
	if (forceValid)
	{
		// Wee!
		for (int s = 0; s < TTLWeapon::SLOTS; s++)
		{
			const TTLWeapon::Slot slot = (TTLWeapon::Slot)s;

			if (!selection->Get(slot))
				selection->Set(slot, FindDefault(playerType, slot));
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void ArmoryView::CreateFromSelection(
PlayerType playerType,
WeaponLoadout &loadout)
{
	// Get the selection
 	ArmorySelection *selection = GetSelection(playerType);
	if (!selection)
		return;

	// nothing in here yet
	loadout.Clear();

	// Wee!
	for (int s = 0; s < TTLWeapon::SLOTS; s++)
	{
		const TTLWeapon::Slot slot = (TTLWeapon::Slot)s;

		const Armory::Description *desc = selection->Get(slot);
		if (desc)
			loadout.AddWeapon(desc->LoadoutName(), slot);
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool ArmoryView::Legalize(
PlayerType playerType,
WeaponLoadout &loadout)
{
	bool wasLegal = true;

	int weapons = loadout.Weapons();
	for (int w = 0; w < weapons; w++)
	{
		const char *weaponName = loadout.GetWeapon(w);
		if (!weaponName)
			continue;

		const Armory::Description *desc = g_armory.Find(weaponName);
		if (!desc)
			continue;

		// Is this legal?
		const bool legal = IsLegal(playerType, *desc);
		if (!legal)
		{
			const Armory::Description *replace = FindLegal(playerType, *desc);
			const char *replaceWith = replace ? replace->LoadoutName() : "none";

#if !defined(CDROM) && !defined(BEC)
			char type[32];
			if (playerType == AICop) strcpy(type, "AICop");
			else if (playerType == AICop) strcpy(type, "AICop");
			else if (playerType == HumanCop) strcpy(type, "HumanCop");
			else if (playerType == AIGangster) strcpy(type, "AIGang");
			else if (playerType == HumanGangster) strcpy(type, "HumanGang");
			else strcpy(type, "Unknown");
			char errorText[256];
			sprintf(errorText, "Illegal weapon for %s -> %s replaced with %s!", type, weaponName, replaceWith);
#if defined(WIN32) && defined(FOOD) && defined(_DEBUG)
			ASSERTS(legal, ("%s", errorText));
#else
			TRACE("%s\n", errorText);
#endif
#endif
			// sneak it in
			loadout.AddWeapon(replaceWith, w);
			wasLegal = false;
		}
	}

	return wasLegal;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
ArmorySelection *ArmoryView::GetSelection(
PlayerType playerType)
{
  // Selections only work for humans
	ASSERT(playerType == HumanCop || playerType == HumanGangster);

	if (playerType == HumanCop)
		return &m_copSelection;
	else if (playerType == HumanGangster)
		return &m_gangsterSelection;
	else
		return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// Is this description legal?
/////////////////////////////////////////////////////////////////////////////
bool ArmoryView::IsLegal(
PlayerType playerType,
const Armory::Description &desc)
{
	const TTLWeaponProperties &props = desc.Properties();

	// Is this an NPC only weapon?
	bool humanPlayer = (playerType == HumanCop || playerType == HumanGangster);
	if (humanPlayer && props.Test(TTLWeaponProperties::NPCONLY))
		return false;

	// Is this cop or gangster only?
	bool isCop = (playerType == HumanCop || playerType == AICop);
	if (isCop && !props.Test(TTLWeaponProperties::ALLOWCOPS))
		return false;
	bool isGangster = (playerType == HumanGangster  || playerType == AIGangster );
	if (isGangster && !props.Test(TTLWeaponProperties::ALLOWGANGSTER))
		return false;

	// Test the mode
	if (m_mode == Multiplayer && !props.Test(TTLWeaponProperties::ALLOWMULTIPLAYER))
		return false;

	// Are we allowed to have super weapons?
	if (!m_allowSuperweapons && props.Test(TTLWeaponProperties::SUPERWEAPON))
		return false;
	if (m_mode == Multiplayer && props.Test(TTLWeaponProperties::SUPERWEAPON))
		return false;

	// Test slot restructions
	int slotMask = 1 << (int)desc.Slot();
	if ((slotMask & m_slotRestriction) != 0)
		return false;

	// Test type restrictions
	int typeMask = 1 << (int)desc.Properties().m_weaponType;
	if ((typeMask & m_typeRestriction) != 0)
		return false;

	// Somehow we made it, this weapon is legal
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
const Armory::Description *ArmoryView::FindLegal(
PlayerType playerType,
const Armory::Description &basedOn)
{
	const TTLWeapon::Slot slot = (TTLWeapon::Slot)basedOn.Slot();
	const Armory::Description *replace = FindFirst(playerType, slot);
	while (replace)
	{
		if (replace->GetWeaponType() == basedOn.GetWeaponType())
			return replace;

		replace = FindNext(playerType, slot, replace);
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void ArmoryDependency::InitializeOnce()
{
	NewSingleton(Armory);
	NewSingleton(ArmoryView);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void ArmoryDependency::TerminateOnce()
{
	DeleteSingleton(ArmoryView);
	DeleteSingleton(Armory);
}


