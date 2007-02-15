/*************************************/
/*                                   */
/*   PigpenPCH.h                     */
/*   new engine  05/01/03            */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   pre-compiled header             */
/*                                   */
/*************************************/

/* pre-compiled headers work by doing up-front work on a number of .h files, then having the results available when those
.h files are actually included. typically, there is a .cpp and .h files used to generate the pre-compiled header for a
project. in settings, the .cpp file is marked to "create precompiled header file." all other .cpp files are marked as
"use precompiled header file." choosing which .h files to include in a pre-compiled header is a balancing act between:

  wanting to include as many as possible because it improves performance
  not including rarely-include files since touching them will re-compile every file in the project

there is also a question about whether to include both .h files from inside and outside the project. for example, the
game project contains files like DBFile.h, and also depends on Layers include files. should the game pre-compiled header
include Layers include files and/or Game include files. i think this is also a balancing act, one we'll resolve through
trial and error

the implementation of pre-compiled headers for the different compilers is pretty messed up. in a nutshell...

under visual studio, you specify (in the project settings) the name of the .h file used to represent the pre-compiled
header stuff. the compiler looks for this include directly in the source, so it can't be inside an #if statement (stupid!).
so for non-visual-studio builds the .h file must be effectively empty

under codewarrior, we interpret the compiler commands to identify which file creates the pre-compiled header file. that
file gets the include file PREFIX_PS2 set as its prefix file, otherwise the .pch file is used as the prefix file to
implement pre-compiled headers. this means that we don't need to specifically include the .pch file in each module.
as a hack, since the .cpp file that creates the pre-compiled header is passed in as part of the link list, we also
create an empty .obj file for it at the same time as we create the pre-compiled header. this is done as part of the
xcl compile/link interpreter

finally, for no pre-compiled header (SN), the compiler falls through to the normal includes */

#ifndef __PIGPEN_PCH_H
#define __PIGPEN_PCH_H

/* this file must be effectively empty for non-visual-studio builds, except for when it is included by the codewarrior
.pch file that creates the pre-compiled header file */

/******************* includes ****************************/

/* from system-generated stdafx.h */

#if !defined(AFX_STDAFX_H__79558426_AB2A_11D3_8D36_00A0CC55BD72__INCLUDED_)
#define AFX_STDAFX_H__79558426_AB2A_11D3_8D36_00A0CC55BD72__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WINVER 0x501

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#endif //AFX_STDAFX_H__79558426_AB2A_11D3_8D36_00A0CC55BD72__INCLUDED_

/* system includes */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <io.h>
#include <float.h>	// for possible fpu exception enabling through _control87
#include <math.h>
#include <wingdi.h>	// DC_PEN

/* engine includes */

#include <Math/Vector.h>
#include <EngineHelper/Timer.h>
#include <Game/Database/DBFile.h>
#include <Game/Managers/ParticleDB.h>
#include <Game/Misc/GameEngine.h>

/* ImportExport includes */

#include "savestr.h"
#include "ExportVars.h"
#include "PluginError.h"
#include "CommonUtility.h"
#include "DBLUtil.h"
#include "DBLWrite.h"
#include "DBLSwap.h"
#include "TextureChunk.h"
#include "TextureInfo.h"
#include "TextureList.h"
#include "HashTable.h"
#include "ReadImage.h"

/* Pigpen includes */

#include "Doc.h"
#include "App.h"
#include "MainFrm.h"
#include "View.h"
#include "Preview.h"
#include "Sound.h"
#include "PigpenUtility.h"
#include "Export.h"
#include "Image.h"
#include "Graphic.h"
#include "Particle.h"
#include "Effect.h"
#include "Emitter.h"
#include "Swoosh.h"
#include "SwooshEmitter.h"
#include "Preview.h"
#include "resource.h"       // main symbols
#include "Envelope.h"
#include "Spawn.h"
#include "Surface.h"
#include "ParticleSwoosh.h"

#include "ImportDialog.h"
#include "EffectDialog.h"
#include "EmitterDialog.h"
#include "GraphicDialog.h"
#include "ParticleSheet.h"
#include "PageCollide.h"
#include "PageColor.h"
#include "PageDisplay.h"
#include "PageEnvelope.h"
#include "PageLifetime.h"
#include "PageMovement.h"
#include "PagePiggyback.h"
#include "PageSize.h"
#include "PageSwooshDisplay.h"
#include "SwooshSheet.h"
#include "SwooshEmitterDialog.h"
#include "ProgressDialog.h"
#include "SurfaceListDialog.h"
#include "PreviewDialog.h"

#endif // __PIGPEN_PCH_H
