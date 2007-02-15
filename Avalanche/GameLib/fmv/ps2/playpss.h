///////////////////////////////////////////////////////////////////////////////
// Adaptated from the EZMPEGStr stuff
///////////////////////////////////////////////////////////////////////////////
#ifndef PLAYPSS_H
#define PLAYPSS_H

#include <libmpeg.h>
#include "fmv/ps2/fmvdriver.h"

// Set these up
//#define PAL
#define MEDIA_CD

#include "fmv/ps2/playpssi.h"

// The meat
typedef void *MPEGSESSION;
MPEGSESSION StartMpegSession(float volume);
void EndMpegSession(MPEGSESSION session);

// The actual play function
void PlayMpegFile(MPEGSESSION session, char *filename, FMVSoundTrack soundTrack, bool &aborted, FMVDriver::AbortFn abort, void *abortContext);

#endif
