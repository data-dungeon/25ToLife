/***************************************************************************/
// Parameters for 3D sound
/***************************************************************************/
#include "Audio/AudioPCH.h"

// Default wet/dry mix
#define DEFAULT_WET_MIX			1.0f
#define DEFAULT_DRY_MIX			1.0f

// how many times per second does a sound update?
#define SOUND3D_UPDATE_RATE	30
#define DEFAULT_UPDATEMS 		(1000 / SOUND3D_UPDATE_RATE)

// How often do we update occlusions? (in ms)
#define ONESHOT_OCCLUSION_RATE	0
#define LOOP_OCCLUSION_RATE		200

// The occlusion filters
#define VOLUMEOCCLUSIONFILTER 	0.15f
#define PANOCCLUSIONFILTER 		0.1f

// The magical cookie callback
SoundEmitter3DParams::CookieCallback SoundEmitter3DParams::cookieCallback = NULL;

/***************************************************************************/
// Setup reasonable defaults
/***************************************************************************/
SoundEmitter3DParams::SoundEmitter3DParams()
{
	// mixing
	wetMix = DEFAULT_WET_MIX;
	dryMix = DEFAULT_DRY_MIX;

	// Volume attenuation
	audibleDistance = DEFAULT_AUDIBLEDISTANCE;

	// Panning control
	leftRightPanFactor = DEFAULT_PANFACTOR;
	foreAftPanFactor = DEFAULT_PANFACTOR;

	// Doppler
	doppler = false;
	speedOfSound = DEFAULT_SPEEDOFSOUND;

	// How often do we update?
	updateRate = DEFAULT_UPDATEMS;

	// Occlusion stuff
	occlusionTest = NULL;
	oneShotOcclusionTestMS = ONESHOT_OCCLUSION_RATE;
	loopOcclusionTestMS = LOOP_OCCLUSION_RATE;

	// The occlusion filters
	volumeOcclusionFilter = VOLUMEOCCLUSIONFILTER;
	panOcclusionFilter = PANOCCLUSIONFILTER;
};

/***************************************************************************/
// The wet/dry mix
/***************************************************************************/
void SoundEmitter3DParams::SetWetDryMix(
float wet,
float dry)
{
	wetMix = wet;
	dryMix = dry;
}

/***************************************************************************/
/***************************************************************************/
float SoundEmitter3DParams::GetWetMix()
{
	return wetMix;
}

/***************************************************************************/
/***************************************************************************/
float SoundEmitter3DParams::GetDryMix()
{
	return dryMix;
}

/***************************************************************************/
/***************************************************************************/
void SoundEmitter3DParams::SetUpdateRate(
uint ms)
{
	updateRate = (ms > 0 ? ms : 1);
}

/***************************************************************************/
// Setup the occlusion testing, if you pass in NULL no occlusion testing
// is done
/***************************************************************************/
void SoundEmitter3DParams::SetupOcclusionTest(
SoundOcclusionTest *soundOcclustionTest)
{
	occlusionTest = soundOcclustionTest;
}

/***************************************************************************/
// Setup the rates of testing (all sounds are tested when first played)
// if the rate is set to 0ms the sound is never tested once it starts playing
/***************************************************************************/
void SoundEmitter3DParams::SetupOcclusionTestRate(
uint loopTestMS,
uint oneShotTestMS)
{
	loopOcclusionTestMS = loopTestMS;
	oneShotOcclusionTestMS = oneShotTestMS;
}

/***************************************************************************/
// Give them a copy
/***************************************************************************/
SoundEmitter3DParams &SoundEmitter3DParams::operator=(
const SoundEmitter3DParams &p)
{
	wetMix = p.wetMix;
	dryMix = p.dryMix;
	audibleDistance = p.audibleDistance;
	leftRightPanFactor = p.leftRightPanFactor;
	foreAftPanFactor = p.foreAftPanFactor;
	doppler = p.doppler;
	speedOfSound = p.speedOfSound;
	updateRate = p.updateRate;
	occlusionTest = p.occlusionTest;
	loopOcclusionTestMS = p.loopOcclusionTestMS;
	oneShotOcclusionTestMS = p.oneShotOcclusionTestMS;

	return *this;
}

