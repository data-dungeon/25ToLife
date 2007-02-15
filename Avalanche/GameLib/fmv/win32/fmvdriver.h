/////////////////////////////////////////////////////////////////////////////
// This is the driver for FMV video
/////////////////////////////////////////////////////////////////////////////
#ifndef FMVDRIVER_H
#define FMVDRIVER_H

#include "fmv/win32/bink/bink.h"
#include "fmv/fmvst.h"
#include <dsound.h>

extern void Display__ResetGraphics(void);

class FMVDriver
{
public:
	FMVDriver(void);
	~FMVDriver(void);

	// Some statics for how video is stored on this platform
	static const char *Directory(void)
		{ return("fmv\\"); }

	static const char *Extension(void)
		{ return("bik"); }

	// Make a full filename of what an FMV would be on this platform
	static void MakeFile(const char *fmv, char *fullFile);

	// Is this a valid/existing fmv for this driver?
	// Note, pass in fmv name, not the full path
	static bool Valid(const char *fmv);

	// Create the device
	bool Create(float initVolume);

	// Destroy the device
	void Destroy(void);

	// switching to/from the movie players video mode
	void InitMode(void) {}
	void CleanupMode(void) { Display__ResetGraphics(); }

	// Play an FMV
	typedef bool (*AbortFn)(void *context);
	bool Play(const char *fmv, FMVSoundTrack soundTrack, bool &aborted, AbortFn abort = NULL, void *abortContext = NULL);

private:
#if defined (DIRECTX)
	HBINK						m_Bink;							// Bink me bitch
	HBINKBUFFER				m_Bink_buffer;

	LPDIRECT3DDEVICE8		d3dDevice;			// The D3D rendering device
	LPDIRECTSOUND8			directSound;   	// Direct sound object
	LPDIRECT3DTEXTURE8	overlay[2];  		// Overlay textures
	LPDIRECT3DSURFACE8	surface[2];  		// Overlay Surfaces
	DWORD						surfaceWidth;
	DWORD						surfaceHeight;
	RECT 				  		srcRect;
	RECT 						destRect;

  	DWORD						currentSurface;	// Current surface
	D3DPRESENT_PARAMETERS	m_BinkParams;	// save them off
#endif

public:
	void	DestroyBink();
	void	PlayBink();
	void	ShowFrame(  HBINK bink, HBINKBUFFER bink_buffer);
};


#endif


















