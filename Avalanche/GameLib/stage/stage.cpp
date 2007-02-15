/***************************************************************************/
// The Stage
/***************************************************************************/

/******************* includes ****************************/

#include "stage/StagePCH.h"



#include "EngineHelper/timer.h"

#define ENABLE_ACTIVATION

// There can be only one!
DeclareSingleton(Stage);

/***************************************************************************/
// Initialize everything
/***************************************************************************/
Stage::Stage(void) :
nonClusterPool(0, 64),
masterCast(nonClusterPool),
nonProtagonistCast(nonClusterPool),
protagonistCast(nonClusterPool),
activeNonProtagonistCast(nonClusterPool),
activeProtagonistCast(nonClusterPool),
inactiveNonProtagonistCast(nonClusterPool),
inactiveProtagonistCast(nonClusterPool),
changeCast(nonClusterPool)
#ifdef ENABLE_RECORD
, record(masterCast)
#endif
{
	inAdvance = false;
}

/***************************************************************************/
// Make sure we run clean
/***************************************************************************/
Stage::~Stage(void)
{
}

/***************************************************************************/
// Initialization
/***************************************************************************/
void Stage::Initialize(void)
{
#ifdef ENABLE_RECORD
	record.Initialize();
#endif
	clusterSystem.Initialize();
	STAGE_PROFILE_INITIALIZE();
	CAST_PROFILE_INITIALIZE();
}

/***************************************************************************/
// Make sure we run clean
/***************************************************************************/
void Stage::Terminate(void)
{
	clusterSystem.Terminate();
	inactiveProtagonistCast.Clear();
	inactiveNonProtagonistCast.Clear();
	activeProtagonistCast.Clear();
	activeNonProtagonistCast.Clear();
	protagonistCast.Clear();
	nonProtagonistCast.Clear();
	masterCast.Clear();
	STAGE_PROFILE_TERMINATE();
	CAST_PROFILE_TERMINATE();
}

/***************************************************************************/
// Add an actor using its involvement settings.
/***************************************************************************/
void Stage::AddActor(
CActor *actor)
{
	ChangeActor(actor, actor->GetDesiredInvolvement());
}

/***************************************************************************/
// Add a non-protagonist
/***************************************************************************/
void Stage::AddNonProtagonist(
CActor *actor)
{
	ChangeActor(actor, CActor::INVOLVE_STAGE |
			(actor->GetDesiredInvolvement() & ~CActor::INVOLVE_PROTAGONIST));
}

/***************************************************************************/
// Add a protagonist
/***************************************************************************/
void Stage::AddProtagonist(
CProtagonist *actor)
{
	ChangeActor(actor, CActor::INVOLVE_STAGE |
			actor->GetDesiredInvolvement() | CActor::INVOLVE_PROTAGONIST);
}

/***************************************************************************/
// Remove this actor from all the casts we currently are managing
/***************************************************************************/
void Stage::RemoveActor(
CActor *actor)
{
	ChangeActor(actor, actor->GetDesiredInvolvement() & ~CActor::INVOLVE_STAGE);
}

/***************************************************************************/
// Activate an actor
/***************************************************************************/
void Stage::ActivateActor(
CActor *actor)
{
#ifdef ENABLE_ACTIVATION
	ChangeActor(actor, actor->GetDesiredInvolvement() | CActor::INVOLVE_ACTIVE);
#endif
}

/***************************************************************************/
// Activate an actor
/***************************************************************************/
void Stage::DeactivateActor(
CActor *actor)
{
#ifdef ENABLE_ACTIVATION
	ChangeActor(actor, actor->GetDesiredInvolvement() & ~CActor::INVOLVE_ACTIVE);
#endif
}

/***************************************************************************/
/***************************************************************************/
void Stage::MoveToProtagonistCast(CProtagonist *actor)
{
	ChangeActor(actor,
			actor->GetDesiredInvolvement() | CActor::INVOLVE_PROTAGONIST);
}

/***************************************************************************/
/***************************************************************************/
void Stage::MoveToNonProtagonistCast(CProtagonist *actor)
{
	ChangeActor(actor,
			actor->GetDesiredInvolvement() & ~CActor::INVOLVE_PROTAGONIST);
}

/***************************************************************************/
/***************************************************************************/
void Stage::ChangeActor(
CActor *actor,
uint i_newInvolvement)
{
	// Get the current involvement.
	uint involvement = actor->GetCurrentInvolvement();

	// Only need to take action if the current change is different than the
	// previously requested change.
	if (i_newInvolvement != involvement)
	{
		// If the actor is not in the stage and addition to the stage is not
		// requested, just update the actor's involvement; otherwise, request a
		// change.
		if (((i_newInvolvement | involvement) & CActor::INVOLVE_STAGE) == 0)
			actor->SetInvolvement(i_newInvolvement);
		else
		{
			actor->RequestInvolvement(i_newInvolvement);
			if (!inAdvance)
				ChangeActor(actor);
			else if (!changeCast.IsMember(actor))
				changeCast.AddActor(actor);
		}
	}
}

/***************************************************************************/
// Process change requests
/***************************************************************************/
void Stage::ProcessChanges(void)
{
	CCastMember* curr = NULL;
	while ((curr = changeCast.GetNextMember(curr)) != NULL)
		ChangeActor(curr->GetActor());
	changeCast.Clear();
}

/***************************************************************************/
/***************************************************************************/
void Stage::ChangeActor(CActor* actor)
{
	uint current = actor->GetCurrentInvolvement();
	uint desired = actor->GetDesiredInvolvement();

	// If the actor is not in the stage, this is an add.
	if ((current & CActor::INVOLVE_STAGE) == 0)
	{
		// Always add to the master list.
		masterCast.AddActor(actor);

		// Add a non-protagonist.
		if ((desired & CActor::INVOLVE_PROTAGONIST) == 0)
		{
			nonProtagonistCast.AddActor(actor);
			if ((desired & CActor::INVOLVE_ACTIVE) == 0)
				inactiveNonProtagonistCast.AddActor(actor);
			else
				activeNonProtagonistCast.AddActor(actor);
		}

		// Add a protagonist.
		else
		{
			ASSERT(actor->IsA(CProtagonist::ClassType()));
			CProtagonist* protagonist = (CProtagonist*)actor;
			protagonistCast.AddActor(protagonist);
			if ((desired & CActor::INVOLVE_ACTIVE) == 0)
				inactiveProtagonistCast.AddActor(protagonist);
			else
			{
				activeProtagonistCast.AddActor(protagonist);
				clusterSystem.Add(protagonist);
				g_collisionSystem.AddProtagonist(protagonist);
			}
		}
	}

	// If the desire is to not be in the stage, this is a delete.
	else if ((desired & CActor::INVOLVE_STAGE) == 0)
	{
		// Remove a non-protagonist.
		if ((current & CActor::INVOLVE_PROTAGONIST) == 0)
		{
			nonProtagonistCast.RemoveActor(actor);
			if ((current & CActor::INVOLVE_ACTIVE) == 0)
				inactiveNonProtagonistCast.RemoveActor(actor);
			else
				activeNonProtagonistCast.RemoveActor(actor);
		}

		// Remove a protagonist.
		else
		{
			ASSERT(actor->IsA(CProtagonist::ClassType()));
			CProtagonist* protagonist = (CProtagonist*)actor;
			protagonistCast.RemoveActor(protagonist);
			if ((current & CActor::INVOLVE_ACTIVE) == 0)
				inactiveProtagonistCast.RemoveActor(protagonist);
			else
			{
				activeProtagonistCast.RemoveActor(protagonist);
				clusterSystem.Remove(protagonist);
				// Remove us, if the collision system hasn't already died
				if (ValidSingleton(CollisionSystem))
					g_collisionSystem.RemoveProtagonist(protagonist);
			}
		}

		// Remove from the master.
		masterCast.RemoveActor(actor);
	}

	// Currently a non-protagonist.
	else if ((current & CActor::INVOLVE_PROTAGONIST) == 0)
	{
		// Remaining a non-protagonist.
		if ((desired & CActor::INVOLVE_PROTAGONIST) == 0)
		{
			// Must be changing active state.
			ASSERT((current & CActor::INVOLVE_ACTIVE) !=
					(desired & CActor::INVOLVE_ACTIVE));
			if ((desired & CActor::INVOLVE_ACTIVE) == 0)
				activeNonProtagonistCast.TransferActor(actor,
						&inactiveNonProtagonistCast);
			else
				inactiveNonProtagonistCast.TransferActor(actor,
						&activeNonProtagonistCast);
		}

		// Becoming a protagonist.
		else
		{
			ASSERT(actor->IsA(CProtagonist::ClassType()));
			CProtagonist* protagonist = (CProtagonist*)actor;
			nonProtagonistCast.TransferActor(protagonist, &protagonistCast);
			if ((desired & CActor::INVOLVE_ACTIVE) != 0)
			{
				g_stage.AddToClusterSystem(protagonist);
				g_collisionSystem.AddProtagonist(protagonist);
				if ((current & CActor::INVOLVE_ACTIVE) == 0)
					inactiveNonProtagonistCast.TransferActor(protagonist,
							&activeProtagonistCast);
				else
					activeNonProtagonistCast.TransferActor(protagonist,
							&activeProtagonistCast);
			}
			else if ((current & CActor::INVOLVE_ACTIVE) == 0)
				inactiveNonProtagonistCast.TransferActor(protagonist,
						&inactiveProtagonistCast);
			else
				activeNonProtagonistCast.TransferActor(protagonist,
						&inactiveProtagonistCast);
		}
	}

	// Currently a protagonist but becoming a non-protagonist.
	else if ((desired & CActor::INVOLVE_PROTAGONIST) == 0)
	{
		ASSERT(actor->IsA(CProtagonist::ClassType()));
		CProtagonist* protagonist = (CProtagonist*)actor;
		protagonistCast.TransferActor(protagonist, &nonProtagonistCast);
		if ((current & CActor::INVOLVE_ACTIVE) != 0)
		{
			clusterSystem.Remove(protagonist);
			g_collisionSystem.RemoveProtagonist(protagonist);
			if ((desired & CActor::INVOLVE_ACTIVE) == 0)
				activeProtagonistCast.TransferActor(protagonist,
						&inactiveNonProtagonistCast);
			else
				activeProtagonistCast.TransferActor(protagonist,
						&activeNonProtagonistCast);
		}
		else if ((desired & CActor::INVOLVE_ACTIVE) == 0)
			inactiveProtagonistCast.TransferActor(protagonist,
					&inactiveNonProtagonistCast);
		else
			inactiveProtagonistCast.TransferActor(protagonist,
					&activeNonProtagonistCast);
	}

	// Remaining a protagonist.
	else
	{
		// Must be changing active state.
		ASSERT((current & CActor::INVOLVE_ACTIVE) !=
				(desired & CActor::INVOLVE_ACTIVE));
		ASSERT(actor->IsA(CProtagonist::ClassType()));
		CProtagonist* protagonist = (CProtagonist*)actor;
		if ((desired & CActor::INVOLVE_ACTIVE) == 0)
			activeProtagonistCast.TransferActor(protagonist,
					&inactiveProtagonistCast);
		else
			inactiveProtagonistCast.TransferActor(protagonist,
					&activeProtagonistCast);
	}

	// Desired involvement is now current.
	actor->AcceptInvolvement();
}

/***************************************************************************/
// The control/update/animation
/***************************************************************************/
void Stage::AdvanceSimulation(void)
{
	STAGE_PROFILE_START(CStageProfile::ADVANCE);

	// Begin the frame for everyone in the world
	STAGE_PROFILE_START(CStageProfile::BEGIN_FRAME);
	inAdvance = true;
	masterCast.BeginFrame();
	STAGE_PROFILE_END(CStageProfile::BEGIN_FRAME);

#ifdef ENABLE_RECORD
	// Manage recording and load up the recorded state (if playing back)
	STAGE_PROFILE_START(CStageProfile::DEBUG);
	record.Run();
	STAGE_PROFILE_END(CStageProfile::DEBUG);
#endif

	// Build the list of macro/cluster casts
	// use 1/10th of a second for our cluster time step, this would be 
	// bad if the frame rate got lower.
	STAGE_PROFILE_START(CStageProfile::BUILD_CLUSTER);
	clusterSystem.BuildClusters(g_timer.GetFrameSec());
	STAGE_PROFILE_END(CStageProfile::BUILD_CLUSTER);

	STAGE_PROFILE_START(CStageProfile::PROTAGONIST);

	// Run the independent scattered
	STAGE_PROFILE_START(CStageProfile::FIRST_SCATTER);
	clusterSystem.Scatter(0).Advance();
	STAGE_PROFILE_END(CStageProfile::FIRST_SCATTER);

	// Now do all the clusters
	STAGE_PROFILE_START(CStageProfile::CLUSTER);
	CCluster *cluster = clusterSystem.ClusterHead();
	while (cluster)
	{
		cluster->Advance();
		cluster = cluster->next;
	}
	STAGE_PROFILE_END(CStageProfile::CLUSTER);

	// Run the dependent scattered
	STAGE_PROFILE_START(CStageProfile::LAST_SCATTER);
	clusterSystem.Scatter(1).Advance();
	STAGE_PROFILE_END(CStageProfile::LAST_SCATTER);

	STAGE_PROFILE_END(CStageProfile::PROTAGONIST);

	// Now let the non-protagonists run
	STAGE_PROFILE_START(CStageProfile::NON_PROTAGONIST);
	activeNonProtagonistCast.Advance();
	STAGE_PROFILE_END(CStageProfile::NON_PROTAGONIST);

	// End the frame for everyone in the world
	STAGE_PROFILE_START(CStageProfile::END_FRAME);
	masterCast.EndFrame();
	STAGE_PROFILE_END(CStageProfile::END_FRAME);

	// Run the animation
	STAGE_PROFILE_START(CStageProfile::ANIMATE);
#if 0
	masterCast.RunAnimation();
#else
	clusterSystem.Scatter(0).RunAnimation();
	cluster = clusterSystem.ClusterHead();
	while (cluster)
	{
		cluster->RunAnimation();
		cluster = cluster->next;
	}
	clusterSystem.Scatter(1).RunAnimation();
	inactiveProtagonistCast.RunAnimation();
	nonProtagonistCast.RunAnimation();
#endif
	STAGE_PROFILE_END(CStageProfile::ANIMATE);

	// Garbage collect
	STAGE_PROFILE_START(CStageProfile::GARBAGE);
	ProcessChanges();
	inAdvance = false;
	masterCast.GarbageCollect();
	STAGE_PROFILE_END(CStageProfile::GARBAGE);

	// Tell the collision system we have updated all the protagonists
	g_collisionSystem.ProtagonistsMoved();

	// Propagate state to the session (also clears discontinuity flags).
	masterCast.Propagate(g_timer.GetEndSec());

	STAGE_PROFILE_END(CStageProfile::ADVANCE);
}

/***************************************************************************/
/***************************************************************************/
bool Stage::ValidateRotations(float epsilon) const
{
	CCastMember* curr = masterCast.GetNextMember(NULL);
	while (curr)
	{
		ASSERT(curr->GetActor()->GetBodyToWorld().Validate(epsilon));
		curr = curr->GetNextActor();
	}

#if 0
	curr = protagonistCast.GetNextMember(NULL);
	while (curr)
	{
		ASSERT(((CProtagonist*)curr->GetActor())->Obb().Matrix()->Validate(epsilon));
		curr = curr->GetNextActor();
	}
#endif

	return(true);
}
