//============================================================================
//=
//= FMVScriptFunc.cpp - 
//=    Script Interface routines to module "FMV"
//=
//============================================================================

#include "fmv/FMVPCH.h"

#include "Script/ScriptVM.h"
#include "Script/ScriptSystem.h"
#include "fmv/fmv.h"

//============================================================================

static void Script_PlayFMV( ScriptVM* vm )
{
	bool	AllowInterruptions = vm->PopBool();
	const char* Filename	= vm->PopString();

	FMVPlayer::Play( Filename, AllowInterruptions );
}
//============================================================================

void FMV_RegisterScriptFuncs()
{
	SCRIPT_REGISTER( PlayFMV );
}

//============================================================================
