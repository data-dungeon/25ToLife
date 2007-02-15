/***************************************************************************/
// The primary audio library include file
/***************************************************************************/
#ifndef AUDIOLIB_H
#define AUDIOLIB_H

// The master sound driver
#include "Audio/sounddriver.h"

// Platform specific includes
#if defined(WIN32)

#ifndef _XBOX
#include "Audio/Win32/sound.h"
#include "Audio/Win32/sdriver.h"
#include "Audio/Win32/sdrivercaps.h"
#include "Audio/Win32/sounderror.h"
#include "Audio/Win32/soundstream.h"
#else
#include "Audio/Xbox/sound.h"
#include "Audio/Xbox/sdriver.h"
#include "Audio/Xbox/sdrivercaps.h"
#include "Audio/Xbox/sounderror.h"
#include "Audio/Xbox/soundstream.h"
#endif

#elif defined (PS2)

#include "Audio/PS2/sound.h"
#include "Audio/PS2/sdriver.h"
#include "Audio/PS2/sounderror.h"
#include "Audio/PS2/soundstream.h"

#elif defined (GCN)

#include "Audio/GCN/sound.h"
#include "Audio/GCN/sdriver.h"
#include "Audio/GCN/sounderror.h"
#include "Audio/GCN/soundstream.h"

#else

#pragma message("WARNING: platform undefined!\n")

#endif

// The general includes
#include "Audio/soundattr.h"
#include "Audio/soundbank.h"
#include "Audio/Emitter/semitter.h"
#include "Audio/Emitter/semit2d.h"
#include "Audio/Emitter/semit3d.h"
#include "Audio/Emitter/semits.h"
#include "Audio/Environment/semitenv.h"
#include "Audio/group/shandle.h"
#include "Audio/cstreami.h"
#include "Audio/audiosys.h"

#endif
