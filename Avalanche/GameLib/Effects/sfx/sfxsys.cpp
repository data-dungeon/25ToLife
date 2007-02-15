///////////////////////////////////////////////////////////////////////////////
// Generic module to manage special FX
///////////////////////////////////////////////////////////////////////////////
#include "Effects/EffectsPCH.h"
#include "specialfx.h"

DeclareSingleton(SpecialFXSystem);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SpecialFXSystem::SpecialFXSystem() :
m_fxList(true)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SpecialFXSystem::~SpecialFXSystem()
{
}

///////////////////////////////////////////////////////////////////////////////
// Find it
///////////////////////////////////////////////////////////////////////////////
SpecialFX *SpecialFXSystem::FindFirstByName(
const char *name)
{
	SpecialFX *fx = m_fxList.Head();
	while (fx)
	{
		if (strcmpi(fx->Name(), name) == 0)
			return fx;

		fx = fx->next;
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Find it
///////////////////////////////////////////////////////////////////////////////
SpecialFX *SpecialFXSystem::FindNextByName(
const char *name,
SpecialFX *current)
{
	// oops
	ASSERT(current);
	if (!current)
		return NULL;

	SpecialFX *fx = current;
	do {
		fx = fx->next;
		if (!fx)
			break;
		}
	while (strcmpi(fx->Name(), name) != 0);

	return fx;
}

///////////////////////////////////////////////////////////////////////////////
// Find it
///////////////////////////////////////////////////////////////////////////////
SpecialFX *SpecialFXSystem::FindFirstByClass(
const char *className)
{
	SpecialFX *fx = m_fxList.Head();
	while (fx)
	{
		if (strcmp(fx->Class(), className) == 0)
			return fx;

		fx = fx->next;
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Find it
///////////////////////////////////////////////////////////////////////////////
SpecialFX *SpecialFXSystem::FindNextByClass(
const char *className,
SpecialFX *current)
{
	// oops
	ASSERT(current);
	if (!current)
		return NULL;

	SpecialFX *fx = current;
	do {
		fx = fx->next;
		if (!fx)
			break;
		}
	while (strcmp(fx->Class(), className) != 0);

	return fx;
}

///////////////////////////////////////////////////////////////////////////////
// Counting
///////////////////////////////////////////////////////////////////////////////
int SpecialFXSystem::CountByName(
const char *name)
{
	int found = 0;
	SpecialFX *fx = FindFirstByName(name);
	while (fx)
	{
		++found;
		fx = FindNextByName(name, fx);
	}

	return found;
}

///////////////////////////////////////////////////////////////////////////////
// Counting
///////////////////////////////////////////////////////////////////////////////
int SpecialFXSystem::CountByClass(
const char *className)
{
	int found = 0;
	SpecialFX *fx = FindFirstByClass(className);
	while (fx)
	{
		++found;
		fx = FindNextByClass(className, fx);
	}

	return found;
}

///////////////////////////////////////////////////////////////////////////////
// Delete all by name/class
///////////////////////////////////////////////////////////////////////////////
int SpecialFXSystem::DeleteAllByName(
const char *name)
{
	int killed = 0;
	SpecialFX *fx = FindFirstByName(name);
	while (fx)
	{
		SpecialFX *next = FindNextByName(name, fx);
		Delete(fx);
		++killed;
		fx = next;
	}

	return killed;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int SpecialFXSystem::DeleteAllByClass(const char *className)
{
	int killed = 0;
	SpecialFX *fx = FindFirstByClass(className);
	while (fx)
	{
		SpecialFX *next = FindNextByClass(className, fx);
		Delete(fx);
		++killed;
		fx = next;
	}

	return killed;
}

///////////////////////////////////////////////////////////////////////////////
// Delete the oldest by name/class
///////////////////////////////////////////////////////////////////////////////
bool SpecialFXSystem::DeleteOldestByName(const char *name)
{
	SpecialFX *oldest = NULL;
	SpecialFX *fx = FindFirstByName(name);
	while (fx)
	{
		if (!oldest || fx->Born() < oldest->Born())
			oldest = fx;

		fx = FindNextByName(name, fx);
	}

	// Not found!
	if (!oldest)
		return false;

	// Kill the oldest
	Delete(oldest);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Delete the oldest by name/class
///////////////////////////////////////////////////////////////////////////////
bool SpecialFXSystem::DeleteOldestByClass(const char *className)
{
	SpecialFX *oldest = NULL;
	SpecialFX *fx = FindFirstByClass(className);
	while (fx)
	{
		if (!oldest || fx->Born() < oldest->Born())
			oldest = fx;

		fx = FindNextByClass(className, fx);
	}

	// Not found!
	if (!oldest)
		return false;

	// Kill the oldest
	Delete(oldest);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Reset the system (deletes all ttl based fx)
///////////////////////////////////////////////////////////////////////////////
void SpecialFXSystem::Reset()
{
	// loop
	SpecialFX *fx = m_fxList.Head();
	while (fx)
	{
		// Who do we see next
		SpecialFX *next = fx->next;

		// Can we kill him?
		if (fx->TTLBased())
		{
			fx->OnRemove();
			m_fxList.Remove(fx);
		}

		// Next!
		fx = next;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Call this once per frame
///////////////////////////////////////////////////////////////////////////////
void SpecialFXSystem::Update()
{
	float now = g_timer.GetEndSec();
	float deltaT = g_timer.GetFrameSec();

	// loop
	SpecialFX *fx = m_fxList.Head();
	while (fx)
	{
		// Who do we see next
		SpecialFX *next = fx->next;

		// Time to die?
		if (!fx->TimeToDie(now))
			fx->Update(now, deltaT);
		else
		{
			fx->OnRemove();
			m_fxList.Remove(fx);
		}

		// Next!
		fx = next;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Manage FX
///////////////////////////////////////////////////////////////////////////////
bool SpecialFXSystem::Add(
SpecialFX *fx)
{
	if (!fx)
		return false;

	m_fxList.AddTail(fx);
	fx->OnAdd(g_timer.GetEndSec());
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool SpecialFXSystem::Delete(
SpecialFX *fx)
{
	if (!fx)
		return true;

	fx->OnRemove();
	m_fxList.Remove(fx);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool SpecialFXSystem::Unlink(
SpecialFX *fx)
{
	if (!fx)
		return true;

	fx->OnRemove();
	m_fxList.Unlink(fx);
	return true;
}


