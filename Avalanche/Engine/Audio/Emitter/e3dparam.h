/***************************************************************************/
// Parameters for 3D sound
/***************************************************************************/
#ifndef E3DPARAM_H
#define E3DPARAM_H

class SoundOcclusionTest;

class SoundEmitter3DParams
{
public:
	SoundEmitter3DParams();

	// One of the few statics...but this is important
	typedef bool (*CookieCallback)(void *cookie);
	static void RegisterCookieCallback(CookieCallback callback)
		{ cookieCallback = callback; }
	static CookieCallback GetCookieCallback()
		{ return cookieCallback; }
	static inline bool VerifyCookie(void *cookie)
		{ return (!cookie || !cookieCallback || cookieCallback(cookie)); }

	// The wet/dry mix
	void SetWetDryMix(float wet, float dry);
	float GetWetMix();
	float GetDryMix();

	// Set the maximum audible distance (in meters)
	void SetAudibleDistance(float maximumAudibleDistance)
		{ audibleDistance = maximumAudibleDistance; }
	float AudibleDistance() const
		{ return audibleDistance; }

	// Pan control
	void SetLeftRightPanFactor(float factor)
		{ leftRightPanFactor = factor; }
	void SetForeAftPanFactor(float factor)
		{ foreAftPanFactor = factor; }
	float LeftRightPanFactor() const
		{ return leftRightPanFactor; }
	float ForeAftPanFactor() const
		{ return foreAftPanFactor; }

	// Doppler control
	void EnableDoppler(bool enable)
		{ doppler = enable; }
	inline bool DopplerEnabled() const
		{ return doppler; }

	void SetSpeedOfSound(float newSpeedOfSound);
	inline float SpeedOfSound() const
		{ return speedOfSound; }

	// How often do we upate in MS?
	void SetUpdateRate(uint ms);
	inline uint GetUpdateRate() const
		{ return updateRate; }

	// Setup the occlusion testing, if you pass in NULL no occlusion testing
	// is done
	void SetupOcclusionTest(SoundOcclusionTest *soundOcclusionTest);
	inline SoundOcclusionTest *GetOcclusionTest() const
		{ return occlusionTest; }

	// Setup the rates of testing (all sounds are tested when first played)
	// if the rate is set to 0ms the sound is never tested once it starts playing
	void SetupOcclusionTestRate(uint loopTestMS, uint oneShotTestMS = 0);
	inline uint LoopOcclusionTestMS() const
		{ return loopOcclusionTestMS; }
	inline uint OneShotOcclusionTestMS() const
		{ return oneShotOcclusionTestMS; }

	// Filters
	float VolumeOcclusionFilter() const
		{ return volumeOcclusionFilter; }
	float PanOcclusionFilter() const
		{ return panOcclusionFilter; }

	// Give them a copy
	SoundEmitter3DParams &operator=(const SoundEmitter3DParams &p);

private:
	static CookieCallback 	cookieCallback;

	// The wet and dry mix
	float							wetMix, dryMix;

	// The maximum audible distance (in meters)
	float							audibleDistance;

	// Panning control
	float							leftRightPanFactor;
	float							foreAftPanFactor;

	// Doppler
	bool							doppler;
	float							speedOfSound;

	// How often do we update?
	uint							updateRate;

	// What is our sound occlusion test
	SoundOcclusionTest		*occlusionTest;
	uint							loopOcclusionTestMS;
	uint 							oneShotOcclusionTestMS;
	float							volumeOcclusionFilter, panOcclusionFilter;
};

#endif
