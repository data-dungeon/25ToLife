//============================================================================
//=
//= ModeScript.cpp - stuff to load and run the "GameMode" script
//=
//============================================================================

#include "TTLPCH.h"
#include "environ/ModeScript.h"
#include "Script/ScriptVM.h"
#include "Script/ScriptSystem.h"

//============================================================================

CModeScript::CModeScript() :
	d_tempMessageOwner( "ModeScript" )
{
	d_script = NULL;
	d_messageOwner = &d_tempMessageOwner;
	d_filename[ 0 ] = 0;
}

//============================================================================

CModeScript::~CModeScript()
{
	ClearScript();
}

//============================================================================

void CModeScript::ClearScript()
{
	if ( d_script != NULL )
	{
		SET_MESSAGE_DISPATCHER_CALL_LOCATION();
		g_ScriptSystem.DestroyVM( d_script );
		g_ScriptSystem.PurgeUnusedExecutables();
		d_script = NULL;
		d_messageOwner = &d_tempMessageOwner;
	}
	d_filename[ 0 ] = 0;
}

//============================================================================

void CModeScript::SetScript( const char* i_filename )
{
	ClearScript();

	strcpy( d_filename, i_filename );

//	// this thing fragments something fierce, so move it to the secondary heap.
//	t_Handle oldheap = memSetActiveHandle( LayersState.SecondaryMemoryHandle );

	// load script if it exists
	char file[ 256 ];
	sprintf( file, "scripts\\%s.sx", i_filename );
	ScriptExecutable* exe = g_ScriptSystem.LoadExecutable( file );

//	// restore heap
//	memSetActiveHandle( oldheap );

	if ( exe )
	{
		d_script = g_ScriptSystem.CreateVM( exe );
		d_script->RegisterMessageHandlers( d_messageOwner );
	}
}

//============================================================================

void CModeScript::Run()
{
	if ( d_script )
		d_script->Run( g_timer.GetFrameSec() );
}

//============================================================================

void CModeScript::SetMessageOwner( CMessageOwner* owner )
{
	ASSERT( d_script );
	if ( d_script )
	{
		SET_MESSAGE_DISPATCHER_CALL_LOCATION();
		d_script->RemoveAllMessageHandlers();
		d_script->RegisterMessageHandlers( owner );
	}
	d_messageOwner = owner;
}

//============================================================================
