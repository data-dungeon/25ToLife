///////////////////////////////////////////////////////////////////////////////
// Simple wrapper for mixbins
///////////////////////////////////////////////////////////////////////////////
#ifndef XBMIXBIN_H
#define XBMIXBIN_H

class XBMixBin
{
public:
	XBMixBin(const float &masterReverb) : m_masterReverb(masterReverb) {}
	virtual ~XBMixBin() {}

	// Get the default mix bin
	virtual DSMIXBINS &ForMonoSource() = 0;
	virtual DSMIXBINS &ForStereoSource() = 0;

	// Get specific mixbins (only good for one call)
	virtual DSMIXBINS &Get(float volume, float wet, float dry, bool monoSource = true) = 0;

	// Don't call with a stereo source
	virtual DSMIXBINS &Get(float volume, float pan, float wet, float dry) = 0;

protected:
	const float &m_masterReverb;
};

class XBMixBinStereo : public XBMixBin
{
public:
	XBMixBinStereo(const float &masterReverb);
	~XBMixBinStereo() {}

	// Get the default mix bin
	DSMIXBINS &ForMonoSource()
		{ return m_monoSource; }
	DSMIXBINS &ForStereoSource()
		{ return m_stereoSource; }

	// Get specific mixbins (only good for one call)
	DSMIXBINS &Get(float volume, float wet, float dry, bool monoSource = true);

	// Don't call with a stereo source
	DSMIXBINS &Get(float volume, float pan, float wet, float dry);

private:
	enum SPEAKER_INDEX
	{
		FRONT_LEFT = 0,
		FRONT_RIGHT = 1,
		REVERB = 2,
		SUBWOOFER = 3
	};

	DSMIXBINS				m_monoSource;
	DSMIXBINVOLUMEPAIR   m_monoSourcePair[4];

	DSMIXBINS				m_tempMonoSource;
	DSMIXBINVOLUMEPAIR   m_tempMonoSourcePair[4];

	DSMIXBINS				m_stereoSource;
	DSMIXBINVOLUMEPAIR   m_stereoSourcePair[2];

	DSMIXBINS				m_tempStereoSource;
	DSMIXBINVOLUMEPAIR   m_tempStereoSourcePair[2];
};

class XBMixBin51 : public XBMixBin
{
public:
	XBMixBin51(const float &masterReverb);
	~XBMixBin51() {}

	// Get the default mix bin
	DSMIXBINS &ForMonoSource()
		{ return m_monoSource; }
	DSMIXBINS &ForStereoSource()
		{ return m_stereoSource; }

	// Get specific mixbins (only good for one call)
	DSMIXBINS &Get(float volume, float wet, float dry, bool monoSource = true);

	// Don't call with a stereo source
	DSMIXBINS &Get(float volume, float pan, float wet, float dry);

private:
	enum SPEAKER_INDEX
	{
		FRONT_LEFT = 0,
		BACK_LEFT = 1,
		FRONT_RIGHT = 2,
		BACK_RIGHT = 3,
		REVERB = 4,
		CENTER = 5,
		SUBWOOFER = 6
	};

	DSMIXBINS				m_monoSource;
	DSMIXBINVOLUMEPAIR   m_monoSourcePair[7];

	DSMIXBINS				m_tempMonoSource;
	DSMIXBINVOLUMEPAIR   m_tempMonoSourcePair[7];

	DSMIXBINS				m_stereoSource;
	DSMIXBINVOLUMEPAIR   m_stereoSourcePair[4];

	DSMIXBINS				m_tempStereoSource;
	DSMIXBINVOLUMEPAIR   m_tempStereoSourcePair[4];

	static float			m_nonPannedBackMix;
	static float			m_pannedBackMix;
	static float			m_centerMix;
};

#endif
