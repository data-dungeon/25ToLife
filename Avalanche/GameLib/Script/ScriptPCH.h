//===========================================================================
//=
//= ScriptPCH.h
//=
//===========================================================================

#ifndef _SCRIPT_PCH_H
#define _SCRIPT_PCH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "EngineHelper/Hash.h"

#if defined( WIN32_TOOLS_DEBUG ) || defined( WIN32_TOOLS_RELEASE )
#include "ScriptAssert.h"
#else
#include "Layers/Assert/Assert.h"
#include "Layers/Debug.h"
#define printf dbgPrint
#endif

#include "ScriptTypes.h"
#include "ScriptError.h"
#include "ScriptUtil.h"
#include "ScriptSymbols.h"
#include "ScriptLex.h"
#include "ScriptPreprocessor.h"
#include "ScriptParseTree.h"
#include "ScriptParse.h"
#include "ScriptExecutable.h"
#include "ScriptCodeGen.h"
#include "ScriptVarList.h"
#include "ScriptVM.h"
#include "ScriptSystem.h"
#include "ScriptDebug.h"

#endif //_SCRIPT_PCH_H
