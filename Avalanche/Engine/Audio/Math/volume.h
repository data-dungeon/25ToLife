/***************************************************************************/
// Simple class for converting between linear and dB sound volume
//
// The basis for all transforms is 0-255 for linear volume
// dB is always specified in 100th's of a dB (HectodB), with 0 being full
// volume and -10000 (-100dB) being minimum volume.
// Unit volumes are given from 0 to 1 (with 1 being full) and return dB
//
// Here is the equation dB = 10 dB * log I / I0 where I0 = 10^-12 w/m^2
/***************************************************************************/
#ifndef VOLUME_H
#define VOLUME_H

class SoundVolume
{
public:
	// Convert hectodB [-10000..0] to volume from [0..255]
	static int HectodBToLinear(int hectodB);

	// Convert volume from [0..255 to] [-10000..0] hectodB
	static int LinearToHectodB(int linear);

	// converts dB [-100..0] to volume [0..1]
	static float dBToUnit(float dB);

	// converts volume [0..1] to dB [-100..0]
	static float UnitTodB(float unit);

	// Special call that converts hectodB [-10000..0] to unit volume [0..1]
	static float HectodBToUnit(int hectodB);

	// Special call that converts unit volume [0..1] to hectodB [-10000..0]
	static int UnitToHectodB(float unit);

	// Convert a pan value [-1..1] to a hectodB
	static int UnitPanToHectodB(float pan);

	// Convert a hectodB pan value [-10000..10000] to unit pan [-1..1]
	static float HectodBToUnitPan(int hectodB);
};

#endif
