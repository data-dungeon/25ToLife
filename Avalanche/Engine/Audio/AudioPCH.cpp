//============================================================================
//=
//= AudioPCH.cpp - Precompiled header CPP
//=
//============================================================================

// codewarrior flag
#ifdef CW
#define CW_PCH
#endif

#include "Audio/AudioPCH.h"

#if defined(PS2) && defined(SN)

#include "Audio/audiosys.cpp"
#include "Audio/jukebox.cpp"
#include "Audio/listener.cpp"
#include "Audio/soundattr.cpp"
#include "Audio/soundbank.cpp"
#include "Audio/sounddriver.cpp"
#include "Audio/sstream.cpp"
#include "Audio/sstream3d.cpp"
#include "Audio/streamq.cpp"
#include "Audio/voiceq.cpp"
#include "audio/ps2/ass.cpp"
#include "audio/ps2/soundirx.cpp"
#include "audio/ps2/sbankps2.cpp"
#include "Audio/PS2/sdriver.cpp"
#include "Audio/PS2/sound.cpp"
#include "Audio/PS2/sounderror.cpp"
#include "Audio/PS2/soundstream.cpp"
#include "audio/ps2/soundlib.cpp"
#include "Audio/Emitter/e3dparam.cpp"
#include "Audio/Emitter/semit2d.cpp"
#include "Audio/Emitter/semit3d.cpp"
#include "Audio/Emitter/semitter.cpp"
#include "Audio/group/shandle.cpp"
#include "Audio/Environment/envstrig.cpp"
#include "Audio/Environment/semitenv.cpp"
#include "Audio/Math/doppler.cpp"
#include "Audio/Math/positional.cpp"
#include "Audio/Math/volume.cpp"
#include "Audio/Math/occlude.cpp"

#endif // defined(PS2) && defined(SN)
