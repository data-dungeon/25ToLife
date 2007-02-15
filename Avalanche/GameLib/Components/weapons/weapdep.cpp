/////////////////////////////////////////////////////////////////////////////
// Auto dependencies for weapon lib
// I'm sure there is a clever ass way to do this with a template,
// and I tried, but I'm in a hurry.
/////////////////////////////////////////////////////////////////////////////
#include "Components/ComponentsPCH.h"
#include "Components/weapons/weapdep.h"

// wee!
WeaponDependency *WeaponDependency::m_head = NULL;

/////////////////////////////////////////////////////////////////////////////
// Do the work
/////////////////////////////////////////////////////////////////////////////
void WeaponDependency::InitializeAllOnce(
Phase phase)
{
	WeaponDependency *ptr = m_head;
	while (ptr)
	{
		if (phase == ptr->WantPhase())
			ptr->InitializeOnce();
		ptr = ptr->m_next;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Do the work
/////////////////////////////////////////////////////////////////////////////
void WeaponDependency::TerminateAllOnce(
Phase phase)
{
	WeaponDependency *ptr = m_head;
	WeaponDependency *next;
	while (ptr)
	{
		next = ptr->m_next;
		if (phase == ptr->WantPhase())
			ptr->TerminateOnce();
		ptr = next;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Do the work
/////////////////////////////////////////////////////////////////////////////
void WeaponDependency::InitializeAllEvery(
Phase phase)
{
	WeaponDependency *ptr = m_head;
	while (ptr)
	{
		if (phase == ptr->WantPhase())
			ptr->InitializeEvery();
		ptr = ptr->m_next;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Do the work
/////////////////////////////////////////////////////////////////////////////
void WeaponDependency::TerminateAllEvery(
Phase phase)
{
	WeaponDependency *ptr = m_head;
	WeaponDependency *next;
	while (ptr)
	{
		next = ptr->m_next;
		if (phase == ptr->WantPhase())
			ptr->TerminateEvery();
		ptr = next;
	}
}









