//
// ScriptPCH_SN.cpp - faster compiles for the PS2
//

#ifdef CW
#define CW_PCH
#endif

#include "ScriptPCH.h"

// include the source for all the cpp files to reduce compile time
#if defined(PS2) && defined(SN)
#include "ScriptCodeGen.cpp"
#include "ScriptExecutable.cpp"
#include "ScriptLex.cpp"
#include "ScriptParse.cpp"
#include "ScriptParseTree.cpp"
#include "ScriptPreprocessor.cpp"
#include "ScriptSymbols.cpp"
#include "ScriptSystem.cpp"
#include "ScriptVM.cpp"
#include "ScriptDebug.cpp"
#endif
