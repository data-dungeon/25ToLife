///////////////////////////////////////////////////////////////////////////////
// Simple wrapper for mixbins
///////////////////////////////////////////////////////////////////////////////
#include "Audio/AudioPCH.h"
#include "Audio/Math/volume.h"
#include "Audio/Xbox/xbmixbin.h"

float	XBMixBin51::m_nonPannedBackMix = 0.25f;
float	XBMixBin51::m_pannedBackMix = 0.2f;
float XBMixBin51::m_centerMix = 0.4f;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
XBMixBinStereo::XBMixBinStereo(const float &masterReverb) :
XBMixBin(masterReverb)
{
	// Setup the default mixbin
	m_monoSource.dwMixBinCount = sizeof(m_monoSourcePair) / sizeof(DSMIXBINVOLUMEPAIR);
	m_monoSource.lpMixBinVolumePairs = &m_monoSourcePair[0];

	// seutp the volume pairs
	m_monoSourcePair[FRONT_LEFT].dwMixBin = DSMIXBIN_FRONT_LEFT;
	m_monoSourcePair[FRONT_LEFT].lVolume = DSBVOLUME_MAX;
	m_monoSourcePair[FRONT_RIGHT].dwMixBin = DSMIXBIN_FRONT_RIGHT;
	m_monoSourcePair[FRONT_RIGHT].lVolume = DSBVOLUME_MAX;
	m_monoSourcePair[REVERB].dwMixBin = DSMIXBIN_I3DL2;
	m_monoSourcePair[REVERB].lVolume = DSBVOLUME_MIN;
	m_monoSourcePair[SUBWOOFER].dwMixBin = DSMIXBIN_LOW_FREQUENCY;
	m_monoSourcePair[SUBWOOFER].lVolume = DSBVOLUME_MAX;

	// Setup the temp mixbin
	m_tempMonoSource.dwMixBinCount = sizeof(m_tempMonoSourcePair) / sizeof(DSMIXBINVOLUMEPAIR);
	m_tempMonoSource.lpMixBinVolumePairs = &m_tempMonoSourcePair[0];
	for (DWORD i = 0; i < m_tempMonoSource.dwMixBinCount; i++)
		m_tempMonoSourcePair[i] = m_monoSourcePair[i];

	// Setup the default mixbin
	m_stereoSource.dwMixBinCount = sizeof(m_stereoSourcePair) / sizeof(DSMIXBINVOLUMEPAIR);
	m_stereoSource.lpMixBinVolumePairs = &m_stereoSourcePair[0];

	// seutp the volume pairs
	m_stereoSourcePair[FRONT_LEFT].dwMixBin = DSMIXBIN_FRONT_LEFT;
	m_stereoSourcePair[FRONT_LEFT].lVolume = DSBVOLUME_MAX;
	m_stereoSourcePair[FRONT_RIGHT].dwMixBin = DSMIXBIN_FRONT_RIGHT;
	m_stereoSourcePair[FRONT_RIGHT].lVolume = DSBVOLUME_MAX;

	// Setup the temp mixbin
	m_tempStereoSource.dwMixBinCount = sizeof(m_tempStereoSourcePair) / sizeof(DSMIXBINVOLUMEPAIR);
	m_tempStereoSource.lpMixBinVolumePairs = &m_tempStereoSourcePair[0];
	for (DWORD i = 0; i < m_tempStereoSource.dwMixBinCount; i++)
		m_tempStereoSourcePair[i] = m_stereoSourcePair[i];
}

///////////////////////////////////////////////////////////////////////////////
// Get specific mixbins (only good for one call)
///////////////////////////////////////////////////////////////////////////////
DSMIXBINS &XBMixBinStereo::Get(
float volume,
float wet,
float dry,
bool monoSource)
{
	if (monoSource)
	{
		LONG dBDry = SoundVolume::UnitToHectodB(dry);
		LONG dBWet = SoundVolume::UnitToHectodB(wet * m_masterReverb);

		m_tempMonoSourcePair[FRONT_LEFT].lVolume = dBDry;
		m_tempMonoSourcePair[FRONT_RIGHT].lVolume = dBDry;
		m_tempMonoSourcePair[REVERB].lVolume = dBWet;
		m_tempMonoSourcePair[SUBWOOFER].lVolume = dBDry;

		return m_tempMonoSource;
	}
	else
	{
		LONG dBDry = SoundVolume::UnitToHectodB(dry);

		m_tempStereoSourcePair[FRONT_LEFT].lVolume = dBDry;
		m_tempStereoSourcePair[FRONT_RIGHT].lVolume = dBDry;

		return m_tempStereoSource;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
DSMIXBINS &XBMixBinStereo::Get(
float volume,
float pan,
float wet,
float dry)
{
	// This is so wrong, but will work for now
	LONG pandB = SoundVolume::UnitPanToHectodB(pan);
	LONG dBL, dBR;
	if (pandB >= 0)
	{
		dBL = -pandB;
		dBR = 0;
	}
	else
	{
		dBL = 0;
		dBR = pandB;
	}

	float volumeL = SoundVolume::HectodBToUnit(dBL);
	float volumeR = SoundVolume::HectodBToUnit(dBR);
	LONG dBDryL = SoundVolume::UnitToHectodB(dry * volumeL);
	LONG dBDryR = SoundVolume::UnitToHectodB(dry * volumeR);
	LONG dBWet = SoundVolume::UnitToHectodB(wet * m_masterReverb);

	m_tempMonoSourcePair[FRONT_LEFT].lVolume = dBDryL;
	m_tempMonoSourcePair[FRONT_RIGHT].lVolume = dBDryR;
	m_tempMonoSourcePair[REVERB].lVolume = dBWet;
	m_tempMonoSourcePair[SUBWOOFER].lVolume = DSBVOLUME_MAX;

	return m_tempMonoSource;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
XBMixBin51::XBMixBin51(const float &masterReverb) :
XBMixBin(masterReverb)
{
	// Setup the positional mixbin
	m_monoSource.dwMixBinCount = sizeof(m_monoSourcePair) / sizeof(DSMIXBINVOLUMEPAIR);
	m_monoSource.lpMixBinVolumePairs = &m_monoSourcePair[0];

	// seutp the volume pairs
	m_monoSourcePair[FRONT_LEFT].dwMixBin = DSMIXBIN_FRONT_LEFT;
	m_monoSourcePair[FRONT_LEFT].lVolume = DSBVOLUME_MAX;
	m_monoSourcePair[BACK_LEFT].dwMixBin = DSMIXBIN_BACK_LEFT;
	m_monoSourcePair[BACK_LEFT].lVolume = DSBVOLUME_MAX;

	m_monoSourcePair[FRONT_RIGHT].dwMixBin = DSMIXBIN_FRONT_RIGHT;
	m_monoSourcePair[FRONT_RIGHT].lVolume = DSBVOLUME_MAX;
	m_monoSourcePair[BACK_RIGHT].dwMixBin = DSMIXBIN_BACK_RIGHT;
	m_monoSourcePair[BACK_RIGHT].lVolume = DSBVOLUME_MAX;

	m_monoSourcePair[REVERB].dwMixBin = DSMIXBIN_I3DL2;
	m_monoSourcePair[REVERB].lVolume = DSBVOLUME_MIN;

	m_monoSourcePair[CENTER].dwMixBin = DSMIXBIN_FRONT_CENTER;
	m_monoSourcePair[CENTER].lVolume = DSBVOLUME_MIN;

	m_monoSourcePair[SUBWOOFER].dwMixBin = DSMIXBIN_LOW_FREQUENCY;
	m_monoSourcePair[SUBWOOFER].lVolume = DSBVOLUME_MAX;

	// Setup the temp mixbin
	m_tempMonoSource.dwMixBinCount = sizeof(m_tempMonoSourcePair) / sizeof(DSMIXBINVOLUMEPAIR);
	m_tempMonoSource.lpMixBinVolumePairs = &m_tempMonoSourcePair[0];
	for (DWORD i = 0; i < m_tempMonoSource.dwMixBinCount; i++)
		m_tempMonoSourcePair[i] = m_monoSourcePair[i];

	// Setup the positional mixbin
	m_stereoSource.dwMixBinCount = sizeof(m_stereoSourcePair) / sizeof(DSMIXBINVOLUMEPAIR);
	m_stereoSource.lpMixBinVolumePairs = &m_stereoSourcePair[0];

	// seutp the volume pairs
	m_stereoSourcePair[FRONT_LEFT].dwMixBin = DSMIXBIN_FRONT_LEFT;
	m_stereoSourcePair[FRONT_LEFT].lVolume = DSBVOLUME_MAX;
	m_stereoSourcePair[BACK_LEFT].dwMixBin = DSMIXBIN_BACK_LEFT;
	m_stereoSourcePair[BACK_LEFT].lVolume = DSBVOLUME_MAX;

	m_stereoSourcePair[FRONT_RIGHT].dwMixBin = DSMIXBIN_FRONT_RIGHT;
	m_stereoSourcePair[FRONT_RIGHT].lVolume = DSBVOLUME_MAX;
	m_stereoSourcePair[BACK_RIGHT].dwMixBin = DSMIXBIN_BACK_RIGHT;
	m_stereoSourcePair[BACK_RIGHT].lVolume = DSBVOLUME_MAX;

	// Setup the temp mixbin
	m_tempStereoSource.dwMixBinCount = sizeof(m_tempStereoSourcePair) / sizeof(DSMIXBINVOLUMEPAIR);
	m_tempStereoSource.lpMixBinVolumePairs = &m_tempStereoSourcePair[0];
	for (DWORD i = 0; i < m_tempStereoSource.dwMixBinCount; i++)
		m_tempStereoSourcePair[i] = m_stereoSourcePair[i];
}

///////////////////////////////////////////////////////////////////////////////
// Get specific mixbins (only good for one call)
///////////////////////////////////////////////////////////////////////////////
DSMIXBINS &XBMixBin51::Get(
float volume,
float wet,
float dry,
bool monoSource)
{
	if (monoSource)
	{
		LONG dBDry = SoundVolume::UnitToHectodB(dry);
		LONG dBDryBack = SoundVolume::UnitToHectodB(dry * m_nonPannedBackMix);
		LONG dBCenter = SoundVolume::UnitToHectodB(dry * m_centerMix);
		LONG dBWet = SoundVolume::UnitToHectodB(wet * m_masterReverb);

		m_tempMonoSourcePair[FRONT_LEFT].lVolume = dBDry;
		m_tempMonoSourcePair[BACK_LEFT].lVolume = dBDryBack;
		m_tempMonoSourcePair[FRONT_RIGHT].lVolume = dBDry;
		m_tempMonoSourcePair[BACK_RIGHT].lVolume = dBDryBack;
		m_tempMonoSourcePair[REVERB].lVolume = dBWet;
		m_tempMonoSourcePair[CENTER].lVolume = dBCenter;
		m_tempMonoSourcePair[SUBWOOFER].lVolume = dBDry;

		return m_tempMonoSource;
	}
	else
	{
		LONG dBDry = SoundVolume::UnitToHectodB(dry);
		LONG dBDryBack = SoundVolume::UnitToHectodB(dry * m_nonPannedBackMix);
		LONG dBCenter = SoundVolume::UnitToHectodB(dry * m_centerMix);
	
		m_tempStereoSourcePair[FRONT_LEFT].lVolume = dBDry;
		m_tempStereoSourcePair[BACK_LEFT].lVolume = dBDryBack;
		m_tempStereoSourcePair[FRONT_RIGHT].lVolume = dBDry;
		m_tempStereoSourcePair[BACK_RIGHT].lVolume = dBDryBack;

		return m_tempStereoSource;
	}
}

///////////////////////////////////////////////////////////////////////////////
DSMIXBINS &XBMixBin51::Get(
float volume,
float pan,
float wet,
float dry)
{
	// This is so wrong, but will work for now
	LONG pandB = SoundVolume::UnitPanToHectodB(pan);
	LONG dBL, dBR;
	if (pandB >= 0)
	{
		dBL = -pandB;
		dBR = 0;
	}
	else
	{
		dBL = 0;
		dBR = pandB;
	}

	float volumeL = SoundVolume::HectodBToUnit(dBL);
	float volumeR = SoundVolume::HectodBToUnit(dBR);
	LONG dBDryL = SoundVolume::UnitToHectodB(dry * volumeL);
	LONG dBDryR = SoundVolume::UnitToHectodB(dry * volumeR);
	LONG dBDryLBack = SoundVolume::UnitToHectodB(dry * volumeL * m_pannedBackMix);
	LONG dBDryRBack = SoundVolume::UnitToHectodB(dry * volumeR * m_pannedBackMix);
	LONG dBWet = SoundVolume::UnitToHectodB(wet * m_masterReverb);
	LONG dBCenter = SoundVolume::UnitToHectodB(dry * m_centerMix * Math::Min(volumeL, volumeR));	
	LONG dBSub = SoundVolume::UnitToHectodB(dry);

	m_tempMonoSourcePair[FRONT_LEFT].lVolume = dBDryL;
	m_tempMonoSourcePair[BACK_LEFT].lVolume = dBDryLBack;
	m_tempMonoSourcePair[FRONT_RIGHT].lVolume = dBDryR;
	m_tempMonoSourcePair[BACK_RIGHT].lVolume = dBDryRBack;
	m_tempMonoSourcePair[REVERB].lVolume = dBWet;
	m_tempMonoSourcePair[CENTER].lVolume = dBCenter;
	m_tempMonoSourcePair[SUBWOOFER].lVolume = dBSub;

	return m_tempMonoSource;
}

