///////////////////////////////////////////////////////////////////////////////
// Simple implementation of the Mersenne Twister for fast, good psuedo random
// numbers
///////////////////////////////////////////////////////////////////////////////
#ifndef MTWISTER_H
#define MTWISTER_H

#define TWISTER_VECTOR_LENGTH			624
#define SAVE_TWISTER_VECTOR_LENGTH 	(TWISTER_VECTOR_LENGTH + 1)
#define TWISTER_PERIOD					397

class MTwister
{
public:
	// initialize with a simple unsigned int
	MTwister(unsigned int oneSeed);

	// or an array
	MTwister(unsigned int *const bigSeed, unsigned int seedLength = TWISTER_VECTOR_LENGTH);

	// auto-initialize
	MTwister();

	// The max rand
	static inline unsigned int MaxInt()
		{ return ((unsigned int )0xffffffffUL - 1); }

	// real number in [0,1]
	float Rand();

	// real number in [0,n]
	float Rand(float n);

	// real number in [0,1)
	float RandExcludeHigh();

	// real number in [0,n)
	float RandExcludeHigh(float n);

	// real number in (0,1)
	float RandExcludeLowHigh();

	// real number in (0,n)
	float RandExludeLowHigh(float n);

	// integer in [0,2^32-1]
	unsigned int RandInt();

	// integer in [0,n] for n < MaxInt
	unsigned int RandInt(unsigned int n);

	// integer in [0,0x7fffffff]
	int RandSignedInt();

	// integer in [0,n] for n < MaxInt
	int RandSignedInt(int n);

	// Access to nonuniform random number distributions
	float RandNorm(float mean, float variance);

	// Re-seeding functions with same behavior as initializers
	void Seed(unsigned int oneSeed);
	void Seed(unsigned int *const bigSeed, const unsigned int seedLength = TWISTER_VECTOR_LENGTH);
	void Seed();

	// Saving and loading generator state
	// to array of size SAVE_STATE_VECTOR_LENGTH
	// from such array
	void Save(unsigned int *saveArray) const;
	void Load(unsigned int *const loadArray);

private:
	unsigned int 	m_state[TWISTER_VECTOR_LENGTH]; 	// internal state
	unsigned int 	*m_next;   		// next value to get from state
	int 				m_left;        // number of values left before reload needed

	void Initialize(unsigned int oneSeed);
	void Reload();

	unsigned int HiBit(unsigned int u) const
		{ return u & 0x80000000UL; }
	unsigned int LoBit(unsigned int u) const
		{ return u & 0x00000001UL; }
	unsigned int LoBits(unsigned int u) const
		{ return u & 0x7fffffffUL; }
	unsigned int MixBits(unsigned int u, unsigned int v) const
		{ return HiBit(u) | LoBits(v); }
	unsigned int Twist(unsigned int m, unsigned int s0, unsigned int s1 ) const
		{ return m ^ (MixBits(s0, s1) >> 1) ^ ((0 - LoBit(s1)) & 0x9908b0dfUL); }
};

#include "math/mtwister.hpp"

#endif
