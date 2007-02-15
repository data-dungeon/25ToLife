/***************************************************************************/
// What is the frequency Kenneth?
//
// A utility class for converting between cents, semitones and frequency
//
// For reference:
//
// +1 octave doubles the frequency.
// -1 octave halves the frequency.
// An octave is divided into 12 semitones.
// A semitone consists of 100 cents.
//
// Reading the code below gives the formula for modifying a frequency by a
// number of cents/semitones:
//
// f' = f * 2^(shiftSemitones / 12) or
// f' = f * 2^(shiftCents / 1200)
//
// The difference between two frequencies a and b in semitones:
//
// diff = 12 * log10(b / a) / log10(2)
//
// in cents:
//
// diff = 1200 * log10(b / a) / log10(2)
/***************************************************************************/
#ifndef SEMITONE_H
#define SEMITONE_H

#include "platform/BaseType.h"
#include "Math/MathClass.h"

// 1 / log10(2.0f)
#define INVLOG2	3.321928094f

class SemitoneMath
{
public:
	// Modify a given frequency by the passed semitones
	static inline int ModifyFrequencyBySemitones(int originalFrequency, float shiftSemitones)
		{ return (int)((float)originalFrequency * Math::Pow(2.0f, shiftSemitones / 12.0f)); }

	// Modify a given frequency by the passed cents
	static inline int ModifyFrequencyByCents(int originalFrequency, float shiftCents)
		{ return (int)((float)originalFrequency * Math::Pow(2.0f, shiftCents / 1200.0f)); }

	// Modify a given frequency by the passed semitones
	static inline float ModifyFrequencyBySemitones(float originalFrequency, float shiftSemitones)
		{ return (originalFrequency * Math::Pow(2.0f, shiftSemitones / 12.0f)); }

	// Modify a given frequency by the passed cents
	static inline float ModifyFrequencyByCents(float originalFrequency, float shiftCents)
		{ return (originalFrequency * Math::Pow(2.0f, shiftCents / 1200.0f)); }

	// How many cents different are these two frequencies?
	static inline float CentDifference(int freqA, int freqB)
		{ return (1200.0f * Math::Log10((float)freqB / (float)freqA) * INVLOG2); }

	// How many semitones different are these two frequencies?
	static inline float SemitoneDifference(int freqA, int freqB)
		{ return (12.0f * Math::Log10((float)freqB / (float)freqA) * INVLOG2); }

	// How many cents different are these two frequencies?
	static inline float CentDifference(float freqA, float freqB)
		{ return (1200.0f * Math::Log10(freqB / freqA) * INVLOG2); }

	// How many semitones different are these two frequencies?
	static inline float SemitoneDifference(float freqA, float freqB)
		{ return (12.0f * Math::Log10(freqB / freqA) * INVLOG2); }
};

#endif
