/*************************************/
/*                                   */
/*   AudioPCH.cpp                    */
/*   new engine  06/12/02            */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   pre-compiled header             */
/*                                   */
/*************************************/


/* system includes */

/* engine includes */

#include "AudioPCH.h"

/* For SN, we include all the source files from the projects here */
/* this is done to reduce compile times under SN (which currently */
/* does not support precompiled headers) */
#if defined(PS2) && defined(SN)
#include "audiosys.cpp"
#include "jukebox.cpp"
#include "listener.cpp"
#include "soundattr.cpp"
#include "soundbank.cpp"
#include "sounddriver.cpp"
#include "streamq.cpp"
#include "voiceq.cpp"
#include "sstream.cpp"
#include "sstream3d.cpp"
#include "cstreami.cpp"
#include "fader.cpp"
#include "PS2/sdriver.cpp"
#include "PS2/sound.cpp"
#include "PS2/sbankps2.cpp"
#include "PS2/sounderror.cpp"
#include "PS2/soundstream.cpp"
#include "PS2/streamlib.cpp"
#include "PS2/banklib.cpp"
#include "Emitter/e3dparam.cpp"
#include "Emitter/semit2d.cpp"
#include "Emitter/semit3d.cpp"
#include "Emitter/semits.cpp"
#include "Emitter/semitter.cpp"
#include "Environment/ambient.cpp"
#include "Environment/ambsound.cpp"
#include "Environment/envfield.cpp"
#include "Environment/envsource.cpp"
#include "Environment/envstrig.cpp"
#include "Environment/semitenv.cpp"
#include "group/reclaim.cpp"
#include "group/schan2d.cpp"
#include "group/schan3d.cpp"
#include "group/schannel.cpp"
#include "group/sgalias.cpp"
#include "group/sgroup.cpp"
#include "group/sgroup2d.cpp"
#include "group/sgroup3d.cpp"
#include "group/shandle.cpp"
#include "Math/doppler.cpp"
#include "Math/positional.cpp"
#include "Math/volume.cpp"
#include "Math/occlude.cpp"
#include "function/lfo.cpp"
#include "function/ramp.cpp"
#endif // defined(PS2) && defined(SN)

