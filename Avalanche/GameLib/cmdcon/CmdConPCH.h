//============================================================================
//= CmdConPCH.h - Pre-Compiled header file for project CmdCon.
//============================================================================

#ifndef __CMDCON_PCH_H
#define __CMDCON_PCH_H

#if defined(WIN32) || defined(CW_PCH) || ( defined(PS2) && defined(SN) )

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>

#include "Math/Vector.h"
#include "EngineHelper/filestream.h"
#include "Math/MathClass.h"
#include "Layers/Debug.h"
#include "Layers/Memory.h"

#if defined(GCN) && defined(CW)
#include "platform/GCN/GCNMWPort.h"
#endif

#include "cmdcon/cmdcon.h"
#include "cmdcon/convar.h"
#include "cmdcon/vcontext.h"
#include "GameHelper/Message.h"

#endif // defined(WIN32) || defined(CW_PCH)

#endif // __CMDCON_PCH_H
