///////////////////////////////////////////////////////////////////////////////
// Generic module to manage special FX
///////////////////////////////////////////////////////////////////////////////
#ifndef SFXSYS_H
#define SFXSYS_H

#include "container/dllist.h"
#include "EngineHelper/Singleton.h"
#include "Effects/sfx/specialfx.h"

class SpecialFXSystem
{
public:
	// Startup and shutdown the singleton
	SpecialFXSystem();
	~SpecialFXSystem();

	// Find it by name
	SpecialFX *FindFirstByName(const char *name);
	SpecialFX *FindNextByName(const char *name, SpecialFX *current);

	// Find it by class
	SpecialFX *FindFirstByClass(const char *className);
	SpecialFX *FindNextByClass(const char *className, SpecialFX *current);

	// Counting
	int CountByName(const char *name);
	int CountByClass(const char *className);

	// Delete all by name/class
	int DeleteAllByName(const char *name);
	int DeleteAllByClass(const char *className);

	// Delete the oldest by name/class
	bool DeleteOldestByName(const char *name);
	bool DeleteOldestByClass(const char *className);

	// Reset the system (deletes all ttl based fx)
	void Reset();

	// Call this once per frame (not viewport intelligent)
	void Update();

	// Manage FX (sub classes should probably help with this)
	bool Add(SpecialFX *fx);
	bool Delete(SpecialFX *fx);
	bool Unlink(SpecialFX *fx);

	// Walk the list (be careful)
	SpecialFX *Head()
		{ return m_fxList.Head(); }

private:
	DoublyLinkedList<SpecialFX>	m_fxList;
};

// There can be only one!
DefineSingleton(SpecialFXSystem)
#define g_sfx GetSingleton(SpecialFXSystem)

#endif
