//============================================================================
//=
//= LevelScript.cpp
//=    code to run a "master" script for a level/world
//=
//============================================================================

#include "TTLPCH.h"
#include "environ/LevelScript.h"
#include "Script/ScriptVM.h"
#include "Script/ScriptSystem.h"

//============================================================================

CLevelScript* CLevelScript::Load( const char* i_filename )
{
	char file[ 256 ];
	sprintf( file, "scripts\\%s.sx", i_filename );
	ScriptExecutable* exe = g_ScriptSystem.LoadExecutable( file );
	if ( exe )
		return new CLevelScript( exe );
	else
		return NULL;
}

//============================================================================

CLevelScript::CLevelScript( ScriptExecutable* exe ) :
	d_messageOwner( "LevelScript" )
{
	d_script = g_ScriptSystem.CreateVM( exe );
	d_script->RegisterMessageHandlers( &d_messageOwner );
}

//============================================================================

CLevelScript::~CLevelScript()
{
	if ( d_script != NULL )
	{
		SET_MESSAGE_DISPATCHER_CALL_LOCATION();
		g_ScriptSystem.DestroyVM( d_script );
		g_ScriptSystem.PurgeUnusedExecutables();
		d_script = NULL;
	}
}

//============================================================================

void CLevelScript::Run()
{
	if ( d_script )
		d_script->Run( g_timer.GetFrameSec() );
}

//============================================================================
