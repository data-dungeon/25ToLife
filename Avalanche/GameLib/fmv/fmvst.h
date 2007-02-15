/////////////////////////////////////////////////////////////////////////////
// Simple class that allows you to play an FMV
//
// Platform must provide an FMV driver
/////////////////////////////////////////////////////////////////////////////
#ifndef FMVST_H
#define FMVST_H

// Set the soundtrack
enum FMVSoundTrack
{
	FMVST_DEFAULT = 0, // Typically english
	FMVST_FRENCH,
	FMVST_ITALIAN,
	FMVST_SPANISH,
	FMVST_GERMAN,
	FMVST_JAPANESE
};

#endif
