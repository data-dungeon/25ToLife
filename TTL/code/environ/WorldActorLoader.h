//============================================================================
//=
//= WorldActorLoader.h - handles creating actors specified in world database
//=
//============================================================================

#ifndef _WORLDACTORLOADER_H
#define _WORLDACTORLOADER_H

//============================================================================

//#include "Layers/Assert/Assert.h"
#include "container/Darray.h"

//============================================================================

class TupperwareAggregate;
class TupConfigRoot;
class TupAccessInstancePool;
class ParameterSet;
class CEnvironment;

//============================================================================

struct ts_EnvActor
{
	CActor* pActor;
};

//============================================================================

class WorldActorLoader
{
public:
	WorldActorLoader();
	~WorldActorLoader();

	// Load the parameter set file.
	bool Load( CEnvironment* environment );

	// Create actors reqardless of distribution directive but don't set them up
	// for distribution.
	void Process();

	// Create actors, only creating and preparing for distribution the
	// distributable actors in the given range. Returns true if processing is
	// complete, i.e. there is no need to call Distribute(). Note that
	// Distribute() can be called regardless of return value.
	bool Process( int firstDistribute, int lastDistribute );

	// Distribute the created actors that have been prepared for distribution.
	// Returns true when distribution is complete.
	bool Distribute();

	// Unload the parameter set file.
	void Terminate();

	bool IsLoaded() const { return d_loaded; }
	uint32 GetCRC() const { return d_crc; }
	int GetDistributedActorCount() const { return d_distributedActorCount; }
	void SetDistributeRate(float i_distributeRate = 1.0e8f) { d_distributeRate = i_distributeRate; }

	// Reset fallen actors to reset for checkpoint
	void RecreateActorsForCheckpoint( int checkpoint );

	// Debug, disable creating actors or some types of actors
	static void DisableActors();
	static void DisableCombatants();

private:
	bool LoadActorParameters( CEnvironment* environment );
	void FreeActorParameters();

	void ProcessInstances( int firstDistribute, int lastDistribute, int checkpoint = -1 );
	void StartActorScripts();
	bool DistributeActors();

protected:
	// use an expandable array to track environmental actors during creation/initialization */
	DynamicArray<ts_EnvActor>* d_pEnvActorArray;

	// lists of all the parameters
	struct ParameterEntry
	{
		ParameterSet*		parms;
		ParameterEntry*	next;
	};
	ParameterEntry*		d_actorParameters;
	int						d_actorParameterCount;

	void DetermineProcessingDirective( ParameterEntry* entry );
	bool						d_loaded;
	uint32					d_crc;
	int						d_distributedActorCount;
	int						d_distributeIndex;
	int						d_distributeEnd;
	uint32					d_distributeTime;
	float						d_distributeRate;

	static bool				s_actorsDisabled;
	static bool				s_combatantsDisabled;
};

//============================================================================

#endif //_WORLDACTORLOADER_H
