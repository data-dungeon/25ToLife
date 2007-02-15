/////////////////////////////////////////////////////////////////////////////
// This is the driver for FMV video
/////////////////////////////////////////////////////////////////////////////
#ifndef FMVDRIVER_H
#define FMVDRIVER_H

// Forwards
typedef void *MPEGSESSION;

class FMVDriver
{
public:
	FMVDriver(void);
	~FMVDriver(void);

	// Some statics for how video is stored on this platform
	static const char *Directory(void)
		{ return("fmv\\"); }
	static const char *Extension(void)
		{ return("pss"); }

	// Make a full filename of what an FMV would be on this platform
	static void MakeFile(const char *fmv, char *fullFile);

	// Is this a valid/existing fmv for this driver?
	// Note, pass in fmv name, not the full path
	static bool Valid(const char *fmv);

	// Create the device
	bool Create(float volume, bool preserveVRAM);

	// Destroy the device
	void Destroy(void);

	// Play an FMV
	typedef bool (*AbortFn)(void *context);
	bool Play(const char *fmv, FMVSoundTrack soundTrack, bool &aborted, AbortFn abort = NULL, void *abortContext = NULL);

private:
	// Our MPEG Session
	MPEGSESSION	session;

	// Save off the scratch pad (Nice API engine guys)
	void *scratchPadSave;
	bool SaveScratchPad();
	void RestoreScratchPad();

	void *vramSave;
	bool SaveVRAM();
	void RestoreVRAM();
};

#endif
