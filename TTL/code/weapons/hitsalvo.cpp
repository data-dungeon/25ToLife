/////////////////////////////////////////////////////////////////////////////
// a collection of simultaneous hits to an actor
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"
#include "weapons/hitsalvo.h"

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
HitSalvo::HitSalvo()
{
	m_weapon = NULL;
	m_tests = 0;
	m_hits = 0;
}

/////////////////////////////////////////////////////////////////////////////
// Add a hit
/////////////////////////////////////////////////////////////////////////////
bool HitSalvo::Add(
const CMunitionImpactSurface &impact)
{
	// Scan for an existing
	for (int h = 0; h < m_hits; h++)
	{
		// Find the right victim
		ActorHit &hit = m_hit[h];
		if (hit.impact[0].d_victim == impact.d_victim)
		{
			// Find a match (only for complex actors)
			bool complex = (impact.d_victim && impact.d_victim->ComplexBit(CStageEntity::USE_COMPLEX) != 0);
			if (complex)
			{
				for (int i = 0; i < hit.impacts; i++)
				{
					// Can we merge?
					if (hit.impact[i].d_surfaceMaterial == impact.d_surfaceMaterial)
					{
						// This isn't quite right, but close enough
						hit.impact[i].d_intersection = (hit.impact[i].d_intersection + impact.d_intersection) * 0.5f;
						hit.impact[i].d_surfaceNormal = (hit.impact[i].d_surfaceNormal + impact.d_surfaceNormal).Normalized();

						// This is right!
						hit.impact[i].d_energy += impact.d_energy;
						hit.impact[i].d_damage += impact.d_damage;
						hit.impact[i].d_salvoSize += 1;
						return true;
					}
				}
			}

			// No match, will it fit?
			ASSERT(hit.impacts < MAX_SALVO_SIZE);
			if (hit.impacts == MAX_SALVO_SIZE)
				return false;

			// Add a new one here
			hit.impact[hit.impacts++] = impact;
			return true;
		}
	}

	// Will it fit
	ASSERT(m_hits < MAX_SALVO_SIZE);
	if (m_hits == MAX_SALVO_SIZE)
		return false;

	// Totally new victim!
	ActorHit &hit = m_hit[m_hits++];
	hit.impact[0] = impact;
	hit.impacts = 1;

	// That was easy
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Send the hits
/////////////////////////////////////////////////////////////////////////////
void HitSalvo::Send(
ActorHandle shooter)
{
	for (int h = 0; h < m_hits; h++)
	{
		ActorHit &hit = m_hit[h];
		for (int i = 0; i < hit.impacts; i++)
			g_ttlMunition.SendMunitionImpact(shooter, hit.impact[i]);
	}
}
