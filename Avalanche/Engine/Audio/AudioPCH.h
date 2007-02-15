//============================================================================
//=
//= AudioPCH.h - Precompiled headers
//=
//============================================================================

#ifndef __AUDIO_PCH_H
#define __AUDIO_PCH_H

#if defined(WIN32) || defined(CW_PCH) || ( defined(PS2) && defined(SN) )

// system

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#if defined (WIN32) && !defined (_XBOX)
#include <process.h>
#include <windows.h>
#include <mmsystem.h>
#endif //WIN32

#if defined (_XBOX)
#include "Math/MathUndeprecate.h"
#include <xtl.h>
#include "Math/MathDeprecate.h"
#endif

#ifdef PS2
#include <eekernel.h>
#include <eetypes.h>
#include <sif.h>
#include <sifrpc.h>
#include <sifdev.h>
#include <malloc.h>
#include <sdrcmd.h>
#include <libdev.h>
#include <libpkt.h>
#include <libcdvd.h>
#include <libsdr.h>
#endif //PS2

#ifdef GCN
#include <Dolphin/os.h>
//#include <dolphin/dtk.h>
#endif //GCN

// engine

#include "platform/CompilerControl.h" // for named sections

#include "platform/BaseType.h"
#include "Math/MathClass.h"
#include "Layers/Media.h"
#include "Layers/Debug.h"
#include "Layers/Memory.h"
#include "Game/Database/DBFile.h"
#include "EngineHelper/timer.h"
#include "EngineHelper/hog.h"
#include "EngineHelper/pathfile.h"

#if defined(GCN) && defined(CW)
#include "platform/GCN/GCNMWPort.h"
#endif

#include "Audio/audiolib.h"
#include "Audio/audiosys.h"
#include "Audio/listener.h"
#include "Audio/soundattr.h"
#include "Audio/sstream.h"
#include "Audio/sstream3d.h"
#include "Audio/cstreami.h"
#include "Audio/voiceq.h"
#include "Audio/Math/doppler.h"
#include "Audio/Math/positional.h"
#include "Audio/Math/semitone.h"
#include "Audio/Math/volume.h"
#include "Audio/Math/occlude.h"
#include "Audio/group/schannel.h"
#include "Audio/group/schan2d.h"
#include "Audio/group/schan3d.h"
#include "Audio/group/sgroup.h"
#include "Audio/group/reclaim.h"
#include "Audio/group/shandle.h"
#include "Audio/group/sgroup2d.h"
#include "Audio/group/sgroup3d.h"
#include "Audio/group/sgalias.h"
#include "Audio/Emitter/e3dparam.h"
#include "Audio/Emitter/semitter.h"
#include "Audio/Environment/envstrig.h"
#include "Audio/Environment/semitenv.h"
#include "Audio/soundmodexport.h"

#if defined (WIN32) && !defined (_XBOX)
#include "Audio/Win32/sdriver.h"
#include "Audio/Win32/sound.h"
#include "Audio/Win32/soundstream.h"
#include "Audio/Win32/sounderror.h"
#include "Audio/Win32/sdrivercaps.h"
#include "Audio/Win32/sdriverenum.h"
#include "Audio/Win32/wavefile.h"
#ifdef DIRECTX_PC
#include "audio/win32/OggFile.h"
#endif //DIRECTX_PC
#endif //WIN32

#if defined (_XBOX)
#include "Audio/Xbox/sdriver.h"
#include "Audio/Xbox/sound.h"
#include "Audio/Xbox/soundstream.h"
#include "Audio/Xbox/sounderror.h"
#include "Audio/Xbox/sdrivercaps.h"
#include "Audio/Xbox/sdriverenum.h"
#include "Audio/Xbox/wmastream.h"
#endif //WIN32

#ifdef PS2
#include "Layers/mstream/mstream.h"
#include "Audio/PS2/sdriver.h"
#include "Audio/PS2/sound.h"
#include "Audio/PS2/sounderror.h"
#include "Audio/PS2/soundstream.h"
#include "Audio/PS2/streamlib.h"
#include "Audio/PS2/banklib.h"
#include "Audio/PS2/vag.h"
#endif //PS2

#ifdef GCN
#include "Audio/GCN/HWSoundStream.h"
#include "Audio/GCN/sbankgcn.h"
#include "Audio/GCN/sdriver.h"
#include "Audio/GCN/sound.h"
#include "Audio/GCN/sounderror.h"
#include "Audio/GCN/SWSoundStream.h"
#endif //GCN

#endif // defined(WIN32) || defined(CW_PCH)
#endif // __AUDIO_PCH_H
