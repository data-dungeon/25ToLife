//============================================================================
//=
//= WorldActorLoaded.cpp - creates actors specified in the world maya file
//=                   and initialized them.
//=
//============================================================================

#include "TTLPCH.h"

#include "environ/EnvInitMgr.h"
#include "environ/WorldActorLoader.h"
#include "environ/ActorCreator.h"
#include "InterfacesTTL/ParticipantIF.h"
#include "stage/ActorCreator.h"
#include "Script/ScriptVM.h"
#include "Script/ScriptSystem.h"
#include "EngineHelper/crc.h"
#include "main/TTL.h"

//============================================================================

#define BEGIN_BLOCK	"BEGIN OBJECTATTRIBUTES"
#define END_BLOCK		"END OBJECTATTRIBUTES"
#define EOL( c )     ( (c) == '\n' || (c) == '\r' )

#define DEFAULT_DISTRIBUTE_RATE  0.06f

#if defined(PS2)
#define DISABLE_MULTIPLAYER_CLOTH
#endif

//============================================================================

bool WorldActorLoader::s_actorsDisabled = false;
bool WorldActorLoader::s_combatantsDisabled = false;

//============================================================================

WorldActorLoader::WorldActorLoader()
{
	d_pEnvActorArray  = NULL;
	d_actorParameters = NULL;
	d_actorParameterCount = 0;
	d_loaded = false;
	d_crc = 0;
	d_distributedActorCount = 0;
	d_distributeRate = DEFAULT_DISTRIBUTE_RATE;
}

//============================================================================

WorldActorLoader::~WorldActorLoader()
{
	Terminate();
}

//============================================================================

void WorldActorLoader::Terminate()
{
	FreeActorParameters();
}

//============================================================================

bool WorldActorLoader::LoadActorParameters( CEnvironment* environment )
{
	const char* FileName;
	s32  size;
	char* buf;
	t_Error err;

	FileName = environment->WorldRoot( ".ap", NULL );
	buf = (char*) Media::Load( FileName, &size, &err );
	if ( !buf )
		return false;

	// prep for processing directives
	d_loaded = true;
	d_crc = (uint32)ComputeCRC(buf, size);
	d_distributedActorCount = 0;

	ParameterEntry* curEntry = NULL;

	// parse the parameter set text file
	char* p = buf;

	// skip to first token
	while ( p < buf + size && ( isspace( *p ) || EOL( *p ) ) ) p++;

	while ( p < buf + size )
	{
		if ( *p == '[' )
		{
			// add a key/value pair to the last entry that provides direction
			// while processing the entry
			DetermineProcessingDirective( curEntry );

			// create a new entry, link to list
			curEntry = new ParameterEntry;
			curEntry->parms = new ParameterSet;
			curEntry->next  = d_actorParameters;
			d_actorParameters = curEntry;
			d_actorParameterCount++;

			// get instance name
			char* instanceName = ++p;
			while ( p < buf + size && *p != ']' ) p++;
			ASSERT( *p == ']' );
			*p++ = 0;
			curEntry->parms->Set( "instance", instanceName );
		}
		else
		{
			// parse a key=value pair
			char* key = p;
			while ( p < buf + size && *p != '=' && !EOL( *p ) ) p++;
			ASSERT( *p == '=' );
			if ( *p == '=' )
			{
				*p++ = 0;
				char* value = p;
				while ( p < buf + size && !EOL( *p ) ) p++;
				ASSERT( EOL( *p ) );
				*p++ = 0;
				curEntry->parms->Set( key, value );
			}
		}

		// skip to next token
		while ( p < buf + size && ( isspace( *p ) || EOL( *p ) ) ) p++;
	}

	// compute the directive for the last entry.
	DetermineProcessingDirective( curEntry );

	memFree( buf );

	// compact all the parameter sets
	for ( ParameterEntry* n = d_actorParameters ; n ; n = n->next )
		n->parms->Compact();

	return true;
}

//============================================================================

void WorldActorLoader::DetermineProcessingDirective(ParameterEntry* entry )
{
	if ( entry != NULL )
	{
		const char* actorClass = entry->parms->GetString( "class" );

		// see if instance is an environmental non-actor
		int value = EnvInitMgr::GetTypeIndex( actorClass );
		if ( value >= 0 )
			value = -value - 1;

		// if not, see if it is an actor
		else if ( CActorCreator::Query( actorClass ) )
		{
			// set its id if distributed
			const char* net = entry->parms->GetString( "net" );
			if ( ( net == NULL ) || strcmp( net, "local" ) )
				value = d_distributedActorCount++;

			// flag it as not distributed
			else
				value = -MAXINT;
		}

		// this instance cannot be processed. note that the value used to flag
		// this condition is the same that ParameterSet returns when it cannot
		// find a key.
		else
			value = MAXINT;

		char valueString[ 12 ];
		sprintf( valueString, "%d", value );
		entry->parms->Set( "directive", valueString );
	}
}

//============================================================================

void WorldActorLoader::FreeActorParameters()
{
	// free actor parameters linked list
	ParameterEntry* next;
	ParameterEntry* n;
	for ( n = d_actorParameters ; n ; n = next )
	{
		next = n->next;
		delete n->parms;
		delete n;
	}
	d_actorParameters = NULL;
	d_actorParameterCount = 0;
	d_loaded = false;
	d_crc = 0;
	d_distributedActorCount = 0;
}

//============================================================================

void WorldActorLoader::ProcessInstances( int firstDistribute, int lastDistribute, int checkpoint /*=-1*/ )
{
#ifdef _DEBUG
	int lastDistributedActor = -1;
#endif
	d_distributeIndex = -1;
	CActor* actor;

	ParameterEntry* n;
	for ( n = d_actorParameters ; n ; n = n->next )
	{
		// get the directive
		int directive = n->parms->GetInt( "directive" );

		// this instance might be something other than an actor, like a sound emitter or particle emitter... 
		if ( ( directive < 0 ) && ( directive != -MAXINT ) )
		{
			if ( checkpoint < 0 )
			{
				EnvInitMgr::InitFromIndex( -1 - directive, *n->parms );
			}
			continue;
		}

		// move on if no actors are to be created, no directive is found, or
		// no action can be taken
		if ( s_actorsDisabled || ( directive == MAXINT ) )
		{
			continue;
		}

		// possibly remove cloth on multiplayer, too slow
#if defined(DISABLE_MULTIPLAYER_CLOTH)
		char* ai;
		if (g_ttl->IsMultiPlayer() && (ai = n->parms->GetString("ai")) != NULL && stricmp(ai, "cloth.sx") == 0)
		{
			continue;
		}
#endif //DISABLE_MULTIPLAYER_CLOTH

		// verify that distribution directives are monotonically decreasing then
		// move on if a distributed actor is not in range
		if ( directive != -MAXINT )
		{
#ifdef _DEBUG
			if (lastDistributedActor == -1)
				lastDistributedActor = directive + 1;
			ASSERT( directive == ( lastDistributedActor - 1 ) );
			lastDistributedActor = directive;
#endif
			if ( ( directive < firstDistribute ) || ( directive > lastDistribute ) )
			{
				continue;
			}
		}

		// skip if we're checkpoint masking and it's not our checkpoint, or if the guy is already created
		if ( checkpoint >= 0 )
		{
			if ( n->parms->GetInt( "cp" ) < checkpoint )
				continue;
			if ( g_stage.MasterCast().FindActorByInstanceName( n->parms->GetString( "instance" ) ) != NULL )
				continue;
		}

		// create our actor
		const char* actorClass = n->parms->GetString( "class" );
		ASSERT_PTR( actorClass );
		if ( s_combatantsDisabled && ( !stricmp( actorClass, "combatant" ) || 
			                          ( n->parms->GetString( "sp1/class" ) && !stricmp( n->parms->GetString( "sp1/class" ), "combatant" ) ) ) )
			continue;
		actor = CActorCreator::Create( actorClass );
		ASSERT_PTR( actor );

		// set the name (used for debugging and for basic identity checking)
#ifndef CDROM
		char name[64];
		const char *actorName = n->parms->GetString( "name" );
		const char *instanceName = n->parms->GetString( "instance" );
		if (instanceName)
		{
			strcpy(name, instanceName);
			if (actorName)	
			{	
				strcat(name, "|");
				strcat(name, actorName);	
			}
		}
		else if (actorName)
		{
			strcpy(name, actorName);
		}
		else
		{
			strcpy(name, "NotSetFromAP");
		}
		if (instanceName && strcmpi(instanceName, "lamp_glasstable01") == 0)
		{
			TRACE("Bad instance!!!!\n");
		}
		actor->SetName( name );
#else
		const char *actorName = n->parms->GetString( "name" );
		if (actorName)
			actor->SetName( actorName );
#endif
		actor->GetMessageOwner().SetOwnerName( "WorldActor", n->parms->GetString( "instance" ) );

		// copy the compacted version of the parameter set to the new actor
		actor->Parm().ShareFrom( n->parms );

		// set up the geometry
		bool validGeometry = ActorInitializer::InitGeometry( actor, ActorCreator::InitGameSpecificGeometry );

		// get actor location/orientation from world instance matrix
		bool validInstance = ActorInitializer::InitFromInstance( actor );

		// initialize for real
		if ( validGeometry && validInstance && actor->Initialize() )
		{
			// try to pull in visual details from instance (lighting, draw order, etc)
			ActorInitializer::InitVisualPropertiesFromInstance( actor );

			// No actors should receive shadows.
			actor->Instance()->SetReceiveShadows(false);

			// assign ids if the actor is a participant
			if ( actor->GetInterface(CParticipantIF::GetID()) != NULL && ValidSingleton(Referee) )
				g_referee.RegisterParticipant(actor);

			// put them in the stage
			if ( actor->IsA( CProtagonist::ClassType() ) && actor->Parm().GetInt( "interacts" ) != false )
				g_stage.AddProtagonist( (CProtagonist*) actor );
			else
				g_stage.AddNonProtagonist( actor );

			// delete automatically when level ends
			actor->SetDeleteWhenNoCast( true );

			// load AI script (won't run until all actors are loaded)
			ActorInitializer::LoadAIScript( actor, actor->Parm().GetString( "ai" ) );

			// add new actor to tracking array
			ts_EnvActor EnvActor;
			memset( &EnvActor, 0, sizeof( EnvActor ) );
			EnvActor.pActor = actor;
			d_pEnvActorArray->Add( EnvActor, -1 );		// -1 means add to end of array

			// note the first and last distributed actors
			if ( ( directive >= firstDistribute ) && ( directive <= lastDistribute ) )
			{
				d_distributeEnd = d_pEnvActorArray->Count();
				if ( d_distributeIndex == -1 )
					d_distributeIndex = d_distributeEnd - 1;
			}
		}
		else
		{
			// initialization failed
	 		delete actor;
			actor = NULL;
		}
	}
}

//============================================================================

void WorldActorLoader::StartActorScripts()
{
	int i;
	CActor* actor;

	for ( i = 0 ; i < d_pEnvActorArray->Count() ; i++ )
	{
		// run the first run on the AI script
		actor = d_pEnvActorArray->Get(i)->pActor;
		if ( actor->Script( AI_SCRIPT ) )
			actor->Script( AI_SCRIPT )->Run( 0.0f );
	}
}

//============================================================================

bool WorldActorLoader::DistributeActors()
{
	// figure out how many actors to create this time
	int count = (int)((float)(g_timer.GetOSTime() - d_distributeTime) *
			d_distributeRate);

	if (count > 0)
		d_distributeTime += (int)((float)count / d_distributeRate);

	for ( ; d_distributeIndex < d_distributeEnd ; d_distributeIndex++ )
	{
		CActor* actor = (*d_pEnvActorArray)[ d_distributeIndex ].pActor;
		int directive = actor->Parm().GetInt( "directive" );
		if ( ( directive >= 0 ) && ( directive != MAXINT ) )
		{
			if ( count <= 0 )
				return false;
			const char* net = actor->Parm().GetString( "net" );
#if 1
			actor->DistributeNetActor( ( net == NULL ) || strcmp( net, "nonpersistent" ) );
#else
			const char* actorName = actor->Parm().GetString( "name" );
			ASSERT_PTR( actorName );
			actor->DistributeNetActor( ( net == NULL ) || strcmp( net, "nonpersistent" ) );
			g_console.Echo("Distributing %s\n", actorName);
#endif
			count--;
		}
	}
	return true;
}

//============================================================================

void WorldActorLoader::RecreateActorsForCheckpoint( int checkpoint )
{
	ASSERT( checkpoint >= 0 );

	if ( d_actorParameterCount > 0 )
	{
		// create array to track all actors spawned from world database
		d_pEnvActorArray = new DynamicArray<ts_EnvActor>(d_actorParameterCount, 0);
		ASSERT( d_pEnvActorArray );

		// create all the actors specified by the world
		ProcessInstances( 0, MAXINT, checkpoint );

		// start the scripts for actors that run them
		StartActorScripts();

		// clean up
		delete d_pEnvActorArray;
		d_pEnvActorArray = NULL;
	}
}

//============================================================================

static void preloadGeom( const char* path, const char* name, const char* ext )
{
	if ( name && name[0] )
	{
		char filename[ 128 ];
		sprintf( filename, "%s%s%s", path, name, ext );
		g_loadSOM.Geometry( filename );
	}
}

//============================================================================

static void preloadNPCTexture( const char* textureSpec )
{
	if ( textureSpec && textureSpec[0] )
	{
		char* p = strchr( textureSpec, '/' );
		if ( p )
		{
			char swapTexture[ 64 ];
			char filename[ 64 ];

			int len = p - textureSpec;
			ASSERT( len + 6 < sizeof(swapTexture) );
			memcpy( swapTexture, textureSpec, len );
			swapTexture[ len ] = 0;
			sprintf( filename, "npc\\textures\\%s_%s.dbl", swapTexture, textureSpec + len + 1 );
			dblLoad( filename );
		}
	}
}

//============================================================================

bool WorldActorLoader::Load( CEnvironment* environment )
{
	// load world database
	bool success = LoadActorParameters( environment );

	if ( success )
	{
		// preload scripts and obvious model replacements
		for ( ParameterEntry* n = d_actorParameters ; n ; n = n->next )
		{
			const char* ai = n->parms->GetString( "ai" );
			if ( ai && ai[0] )
			{
				char fullFilename[ 128 ];
				sprintf( fullFilename, "scripts\\%s", ai );
				g_ScriptSystem.LoadExecutable( fullFilename );
			}
			preloadGeom( "", n->parms->GetString( "model_name" ), "" );
			preloadGeom( "npc/models/", n->parms->GetString( "npc_model" ), ".dbl" );
			preloadGeom( "npc/models/", n->parms->GetString( "npc_accmodel" ), ".dbl" );
			preloadNPCTexture( n->parms->GetString( "npc_texture" ) );
			preloadNPCTexture( n->parms->GetString( "npc_acctexture" ) );
		}
	}

	return success;
}

//============================================================================

void WorldActorLoader::Process()
{
	if ( d_actorParameterCount > 0 )
	{
		// create array to track all actors spawned from world database
		d_pEnvActorArray = new DynamicArray<ts_EnvActor>(d_actorParameterCount, 0);
		ASSERT( d_pEnvActorArray );

		// create all the actors specified by the world
		ProcessInstances( 0, MAXINT );

		// start the scripts for actors that run them
		StartActorScripts();

		// clean up
		delete d_pEnvActorArray;
		d_pEnvActorArray = NULL;
	}
}

//============================================================================

bool WorldActorLoader::Process( int firstDistribute, int lastDistribute )
{
	if ( d_actorParameterCount > 0 )
	{
		// create array to track all actors spawned from world database
		d_pEnvActorArray = new DynamicArray<ts_EnvActor>(d_actorParameterCount, 0);
		ASSERT( d_pEnvActorArray );

		// create all the actors specified by the world
		ProcessInstances( firstDistribute, lastDistribute );

		// start the scripts for actors that run them
		StartActorScripts();

		// setup for distribution
		if ( d_distributeIndex != -1 )
		{
			d_distributeTime = g_timer.GetOSTime();
			return false;
		}

		// clean up
		delete d_pEnvActorArray;
		d_pEnvActorArray = NULL;
	}
	return true;
}

//============================================================================

bool WorldActorLoader::Distribute()
{
	// distribute actors
	if ( d_pEnvActorArray != NULL )
	{
  		if ( !DistributeActors() )
  			return false;

		// clean up
		delete d_pEnvActorArray;
		d_pEnvActorArray = NULL;
	}
	return true;
}

//============================================================================

void WorldActorLoader::DisableActors()
{
	s_actorsDisabled = true;
}

//============================================================================

void WorldActorLoader::DisableCombatants()
{
	s_combatantsDisabled = true;
}

//============================================================================
