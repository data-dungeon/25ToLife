/***************************************************************************/
// Sound and Sound Instance stuff
/***************************************************************************/

/******************* includes ****************************/

#include "Audio/AudioPCH.h"


/***************************************************************************/
// Convert from a -10000 to 0 dB to a linear value
/***************************************************************************/
int SoundVolume::HectodBToLinear(
int hectodB)
{
	return (int)(255.0f * dBToUnit((float)hectodB * 0.01f) + 0.5f);
}

/***************************************************************************/
// Convert from a 0 - 255 to -10000 to 0 dB
/***************************************************************************/
int SoundVolume::LinearToHectodB(
int linear)
{
	return (int)(100.0f * UnitTodB(linear / 255.0f));
}

/***************************************************************************/
// converts dB [-100..0] to volume [0..1]
/***************************************************************************/
float SoundVolume::dBToUnit(
float dB)
{
	// Convert out of db
	dB = Math::Clamp(dB, -100.0f, 0.0f);
	return Math::Clamp(Math::Pow(10.0f, 3.0f * dB / 100.0f));
}

/***************************************************************************/
 // converts volume [0..1] to dB [-100..0]
/***************************************************************************/
float SoundVolume::UnitTodB(
float unit)
{
	unit = Math::Clamp(unit);
	if (unit > 0.0f)
		return Math::Clamp(Math::Log10(Math::Pow(unit, 1.0f / 3.0f)) * 100.0f, -100.0f, 0.0f);
	else
		return -100.0f;
}

/***************************************************************************/
// Special call that converts hectodB [-10000..0] to unit volume [0..1]
/***************************************************************************/
float SoundVolume::HectodBToUnit(
int hectodB)
{
	return SoundVolume::dBToUnit((float)hectodB * 0.01f);
}

/***************************************************************************/
// Special call that converts unit volume [0..1] to hectodB [-10000..0]
/***************************************************************************/
int SoundVolume::UnitToHectodB(
float unit)
{
	return (int)(SoundVolume::UnitTodB(unit) * 100.0f);
}

/***************************************************************************/
// Convert a pan value [-1..1] to a hectodB
/***************************************************************************/
int SoundVolume::UnitPanToHectodB(
float pan)
{
	pan = Math::Clamp(pan, -1.0f, 1.0f);
	float dB;
	if (pan >= 0.0f)
		dB = -UnitTodB(1.0f - pan);
	else
		dB = UnitTodB(1.0f + pan);
	return (int)(Math::Clamp(dB, -100.0f, 100.0f) * 100.0f);
}

/***************************************************************************/
// Convert a hectodB pan value [-10000..10000] to unit pan [-1..1]
/***************************************************************************/
float SoundVolume::HectodBToUnitPan(
int hectodB)
{
	// Convert out of db
	float dB = Math::Clamp((float)hectodB * 0.01f, -100.0f, 100.0f);
	if (dB > 0.0f)
		return dBToUnit(dB);
	else
		return -dBToUnit(-dB);
}
