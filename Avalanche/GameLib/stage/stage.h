/***************************************************************************/
// The Stage
//
// The stage provides a container for all the casts in the game.  Typically
// there will be two casts: protagonists and non-protagonists.
//
// All actors are stored in a single list, called the masterCast.  The non-
// protagonists are all stored in a cast called nonProtaganistCast.
//
// Protagonists are all stored in a single cast called the protagonistCast.
// However, the stage also manages multiple casts of protagonists based on
// spatial separation.  Each frame the protagonists are sorted into:
//
// - A single scatter class for all protagonists not in collision proximity
//		to other protagonists called the scatterProtagonists.
// - A cluster cast for each set of protagonists is in collision proximity
// 	a cluster cast is created and added to a list of clusters called
//		the clusterProtagonists.
//
/***************************************************************************/
#ifndef STAGE_H
#define STAGE_H

#include "stage/protagonist.h"
#include "stage/clustersys.h"
#include "stage/BasicCastMemberPool.h"
#include "EngineHelper/Singleton.h"
#ifdef ENABLE_RECORD
#include "record.h"
#endif

class Stage
{
public:
	// Make destroy stuff (this is my most clever comment)
	Stage(void);
	~Stage(void);

	// Initialization
	void Initialize(void);
	void Terminate(void);

	// Handle adding/removing actors
	void AddActor(CActor *actor);
	void AddProtagonist(CProtagonist *actor);
	void AddNonProtagonist(CActor *actor);
	void RemoveActor(CActor *actor);

	// Activate/deactive actors
	void ActivateActor(CActor *actor);
	void DeactivateActor(CActor *actor);

	// Move between protagonist/non-protagonist
	void MoveToProtagonistCast(CProtagonist *actor);
	void MoveToNonProtagonistCast(CProtagonist *actor);

	// Collective changes
	void ChangeActor(CActor *actor, uint i_newInvolvement);

	void RemoveFromClusterSystem(CProtagonist *actor) { clusterSystem.Remove(actor); }
	void AddToClusterSystem(CProtagonist *actor) { clusterSystem.Add(actor); }
	void EnableOffscreensInClusters(bool enable = true) { clusterSystem.OffScreenScatter(!enable);}

	// The control/update/animation
	void AdvanceSimulation(void);

	// Get at the cast lists directly

	// Everyone
	CCast &MasterCast(void)
		{ return(masterCast); }

	// all non-protagonists
	CCast &NonProtagonistCast(void)
		{ return(nonProtagonistCast); }

	// all protagonists
	CCast &ProtagonistCast(void)
		{ return(protagonistCast); }

	// get at the cast member pool so temp casts can use it
	CCastMemberPool& CastMemberPool(void) {return(nonClusterPool);}

	bool ValidateRotations(float epsilon = 1e-7f) const;

	#ifdef CLUSTER_SYSTEM_STATS
	// for getting the stats out of the system.  I should make the stats
	// a separate object so that I dont have to expose the entire system,
	// but Im a bit lazy.
	ClusterSystem& GetClusterSystem(void) { return clusterSystem;}
	#endif

private:
	// Move members around
	void ProcessChanges(void);
	void ChangeActor(CActor* actor);

	// The cast member pool for non-cluster casts
	// Must be before casts so the pool is constructed before (not so
	// important) and destructed after (very important) them
	CBasicCastMemberPool			nonClusterPool;

	// The master list of all the actors
	CCast								masterCast;

	// The non-protagonists
	CCast								nonProtagonistCast;

	// The protagonists
	CCast								protagonistCast;

	// The active non-protagonists
	CCast								activeNonProtagonistCast;

	// The active protagonists
	CCast								activeProtagonistCast;

	// The inactive non-protagonists
	CCast								inactiveNonProtagonistCast;

	// The inactive protagonists
	CCast								inactiveProtagonistCast;

	// Waiting for added or transferred
	CCast								changeCast;

	// The cluster system
	ClusterSystem					clusterSystem;

	// Prevent changes to the casts during advance
	bool								inAdvance;

#ifdef ENABLE_RECORD
	CRecord							record;
#endif
};

// There can be only one!
DefineSingleton(Stage)
#define g_stage GetSingleton(Stage)

#endif
