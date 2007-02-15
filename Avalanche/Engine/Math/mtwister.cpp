///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "Math/MathPCH.h"

#include "Math/MathClass.h"
#include "Math/MTwister.h"

///////////////////////////////////////////////////////////////////////////////
// initialize with a simple unsigned int
///////////////////////////////////////////////////////////////////////////////
MTwister::MTwister(
unsigned int oneSeed)
{
	Seed(oneSeed);
}

///////////////////////////////////////////////////////////////////////////////
// or an array
///////////////////////////////////////////////////////////////////////////////
MTwister::MTwister(
unsigned int *const bigSeed,
unsigned int seedLength)
{
	Seed(bigSeed, seedLength);
}

///////////////////////////////////////////////////////////////////////////////
// auto-initialize
///////////////////////////////////////////////////////////////////////////////
MTwister::MTwister()
{
	Seed();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
float MTwister::RandNorm(
float mean,
float variance)
{
	// Return a real number from a normal (Gaussian) distribution with given
	// mean and variance by Box-Muller method
	float r = Math::Sqrt(-2.0f * Math::Log(1.0f - RandExcludeLowHigh())) * variance;
	float phi = Math::TwoPi * RandExcludeHigh();
	return mean + r * Math::Cos(phi);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MTwister::Seed(
unsigned int oneSeed)
{
	// Seed the generator with a simple unsigned int
	Initialize(oneSeed);
	Reload();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MTwister::Seed(
unsigned int *const bigSeed,
const unsigned int seedLength)
{
	// Seed the generator with an array of unsigned int's
	// There are 2^19937-1 possible initial states.  This function allows
	// all of those to be accessed by providing at least 19937 bits (with a
	// default seed length of TWISTER_VECTOR_LENGTH = 624 unsigned int's).  Any bits above the lower 32
	// in each element are discarded.
	// Just call seed() if you want to get array from /dev/urandom
	Initialize(19650218UL);
	int i = 1;
	unsigned int j = 0;
	int k = (TWISTER_VECTOR_LENGTH > seedLength ? TWISTER_VECTOR_LENGTH : seedLength);
	for (; k; --k)
	{
		m_state[i] = m_state[i] ^ ((m_state[i - 1] ^ (m_state[i - 1] >> 30)) * 1664525UL);
		m_state[i] += ( bigSeed[j] & 0xffffffffUL ) + j;
		m_state[i] &= 0xffffffffUL;

		if (++i >= TWISTER_VECTOR_LENGTH)
		{
			m_state[0] = m_state[TWISTER_VECTOR_LENGTH - 1];
			i = 1;
		}

		if (++j >= seedLength)
			j = 0;
	}
	for (k = TWISTER_VECTOR_LENGTH - 1; k; --k)
	{
		m_state[i] = m_state[i] ^ ((m_state[i - 1] ^ (m_state[i - 1] >> 30)) * 1566083941UL);
		m_state[i] -= i;
		m_state[i] &= 0xffffffffUL;
		if (++i >= TWISTER_VECTOR_LENGTH)
		{
			m_state[0] = m_state[TWISTER_VECTOR_LENGTH - 1];
			i = 1;
		}
	}

	m_state[0] = 0x80000000UL;  // MSB is 1, assuring non-zero initial array
	Reload();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MTwister::Seed()
{
	// Was not successful, so use time() and clock() instead
	Seed(6181968L);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MTwister::Initialize(
unsigned int seed)
{
	// Initialize generator m_state with seed
	// See Knuth TAOCP Vol 2, 3rd Ed, p.106 for multiplier.
	// In previous versions, most significant bits (MSBs) of the seed affect
	// only MSBs of the m_state array.  Modified 9 Jan 2002 by Makoto Matsumoto.
	unsigned int *s = m_state;
	unsigned int *r = m_state;
	int i = 1;
	*s++ = seed & 0xffffffffUL;
	for (; i < TWISTER_VECTOR_LENGTH; ++i)
	{
		*s++ = (1812433253UL * (*r ^ (*r >> 30)) + i) & 0xffffffffUL;
		r++;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MTwister::Reload()
{
	// Generate TWISTER_VECTOR_LENGTH new values in m_state
	// Made clearer and faster by Matthew Bellew (matthew.bellew@home.com)
	register unsigned int *p = m_state;
	register int i;

	for (i = TWISTER_VECTOR_LENGTH - TWISTER_PERIOD; i--; ++p)
		*p = Twist(p[TWISTER_PERIOD], p[0], p[1]);

	for (i = TWISTER_PERIOD; --i; ++p)
		*p = Twist(p[TWISTER_PERIOD-TWISTER_VECTOR_LENGTH], p[0], p[1]);

	*p = Twist(p[TWISTER_PERIOD-TWISTER_VECTOR_LENGTH], p[0], m_state[0]);

	m_left = TWISTER_VECTOR_LENGTH;
	m_next = m_state;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MTwister::Save(
unsigned int *saveArray) const
{
	register unsigned int *sa = saveArray;
	register const unsigned int *s = m_state;
	register int i = TWISTER_VECTOR_LENGTH;
	for( ; i--; *sa++ = *s++ ) {}
	*sa = m_left;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MTwister::Load(
unsigned int *const loadArray)
{
	register unsigned int *s = m_state;
	register unsigned int *la = loadArray;
	register int i = TWISTER_VECTOR_LENGTH;
	for( ; i--; *s++ = *la++ ) {}
	m_left = *la;
	m_next = &m_state[TWISTER_VECTOR_LENGTH - m_left];
}

