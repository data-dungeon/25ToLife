/////////////////////////////////////////////////////////////////////////////
// a collection of simultaneous hits to an actor
/////////////////////////////////////////////////////////////////////////////
#ifndef HITSALVO_H
#define HITSALVO_H

#include "Environ/Munition.h"

// What is the maximum number of projectiles we will spawn?
#define MAX_SALVO_SIZE	24

class HitSalvo
{
public:
	HitSalvo();

	// Max hits?
	static int MaxHits()
		{ return MAX_SALVO_SIZE; }

	// What is our base weapon
	void SetWeapon(Weapon *weapon)
		{ m_weapon = weapon; }
	Weapon *GetWeapon()
		{ return m_weapon; }

	// Increment the test count
	void IncrementTest()
		{ ++m_tests; }
	int Tests()
		{ return m_tests; }

	// Add a hit
	bool Add(const CMunitionImpactSurface &impact);

	// Send the hits
	void Send(ActorHandle shooter);

private:
	// Our data
	class ActorHit
	{
		public:
			ActorHit() { impacts = 0; }

		 	CMunitionImpactSurface	impact[MAX_SALVO_SIZE];
		 	int							impacts;
	};

	int			m_tests;
	Weapon		*m_weapon;

	ActorHit		m_hit[MAX_SALVO_SIZE];
	int			m_hits;
};

#endif
