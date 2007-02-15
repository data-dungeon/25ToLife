/***************************************************************************/
// The clustering system
/***************************************************************************/

/******************* includes ****************************/

#include "stage/StagePCH.h"

#include "EngineHelper/timer.h"
#include "stage/EntityCategory.h"


/* can be useful to print out the clusters */

//#define PRINT_CLUSTERS

#ifdef WIN32
//#define WRITE_FILE
#endif
#ifdef WRITE_FILE
#include <stdio.h>
#endif

/***************************************************************************/
// Initialize everything
/***************************************************************************/
ClusterSystem::ClusterSystem(void) :
clusterList(false),
clusterElementPool(0, 64),
scatter0(clusterElementPool),
scatter1(clusterElementPool)
{
}

/***************************************************************************/
// Make sure we run clean
/***************************************************************************/
ClusterSystem::~ClusterSystem(void)
{
}

/***************************************************************************/
// Initialization
/***************************************************************************/
void ClusterSystem::Initialize(void)
{
}

/***************************************************************************/
// Make sure we run clean
/***************************************************************************/
void ClusterSystem::Terminate(void)
{
	KillClusters();
	scatter0.Clear();
	scatter1.Clear();
}

/***************************************************************************/
// Add a protagonist
/***************************************************************************/
void ClusterSystem::Add(
CProtagonist *actor)
{
	// Make sure this actor has not already been added
	CCluster *cluster = clusterList.Head();
	while (cluster)
	{
		if (cluster->IsMember(actor))
			return;
		cluster = cluster->next;
	}

	if (scatter0.IsMember(actor) || scatter1.IsMember(actor))
		return;


	// Add them to the scatter, the next frame's ordering will take care of them
	// HACK bikes are in 0, others are in 1
	ClusterElement *ce;
	if (actor->Category().PositiveMatch(PROCESS_LAST_CATEGORY))
		ce = (ClusterElement *)scatter1.AddActor(actor);
	else
		ce = (ClusterElement *)scatter0.AddActor(actor);
	ASSERT(ce != NULL);

	// Register delete so element can be unlinked from clusterXZ
	ce->RegisterDeleteNotify((ClusterElement::MEMBERDELFN)DeleteClusterElementNotify, this);

	// Insert them into the two lists
	clusterXZ.InsertAscending(ce);

	// Let the actor know which cast member is managed by the cluster system
	actor->SetClusterElement(ce);
}

/***************************************************************************/
// Remove this actor from all the casts we currently are managing
/***************************************************************************/
void ClusterSystem::Remove(
CProtagonist *actor)
{
	// Tell the actor it is no longer managed by the cluster system. Must be
	// here because the actor might be deleted when removed from its cast.
	actor->SetClusterElement(NULL);

	// First do all the clusters
	CCluster *cluster = clusterList.Head();
	while (cluster)
	{
		if (cluster->RemoveActor(actor))
			break;
		cluster = cluster->next;
	}

	if (!cluster)
	{
		// Make sure they are not in the scatter
		// HACK bikes are in 0, others are in 1
		if (actor->Category().PositiveMatch(PROCESS_LAST_CATEGORY))
			scatter1.RemoveActor(actor);
		else
			scatter0.RemoveActor(actor);
	}
}

/***************************************************************************/
// Construct new dependencies, checking for errors
/***************************************************************************/
void ClusterSystem::UpdateActorDependencies(void)
{
	// Mark everyone as unprocessed
	ClusterElement* ce = clusterXZ.Head(0);
	while (ce)
	{
		ce->unprocessed = ClusterElement::UNPROCESSED;
		ce = ce->next[0];
	}

	// Find dependencies that will be valid. If you can start at the top
	// of the tree and find a node, it will be valid.
	ce = clusterXZ.Head(0);
	while (ce)
	{
		if (ce->IsProposedIndependent())
		{
			ASSERT(ce->GetProposedNextSibling() == NULL);
			ProcessValidDependency((CProtagonist*)ce->GetActor());
		}
		ce = ce->next[0];
	}

	// Assuming the current tree is valid, find the nodes that cannot be reached
	// with the new tree
	ce = clusterXZ.Head(0);
	while (ce)
	{
		if (ce->IsIndependent())
		{
			ASSERT(ce->GetNextSibling() == NULL);
			CheckValidDependency((CProtagonist*)ce->GetActor());
		}
		ce = ce->next[0];
	}

	// Now update dependencies
	ce = clusterXZ.Head(0);
	while (ce)
	{
		ce->UpdateDependentActors();
		ce = ce->next[0];
	}
}

/***************************************************************************/
/***************************************************************************/
void ClusterSystem::ProcessValidDependency(CProtagonist* i_actor)
{
	// Mark the node as processed
	ClusterElement* ce = i_actor->GetClusterElement();
	ASSERT(ce && ce->unprocessed);
	ce->unprocessed = ClusterElement::PROCESSED;

	if (ce->GetProposedNextSibling())
		ProcessValidDependency(ce->GetProposedNextSibling());
	if (ce->GetProposedChild())
		ProcessValidDependency(ce->GetProposedChild());
}

/***************************************************************************/
/***************************************************************************/
void ClusterSystem::CheckValidDependency(CProtagonist* i_actor)
{
	ClusterElement* ce = i_actor->GetClusterElement();
	ASSERT(ce);

	// The node cannot be reached so the new dependency is illegal
	if (ce->unprocessed)
	{
		// Assert if the current tree is not valid
		ASSERT(ce->IsChangeProposed());

		((CProtagonist*)ce->GetActor())->DependOnFailed();
		ce->CancelDependOn();
		ProcessValidDependency((CProtagonist*)ce->GetActor());
	}

	if (ce->GetNextSibling())
		CheckValidDependency(ce->GetNextSibling());
	if (ce->GetChild())
		CheckValidDependency(ce->GetChild());
}

/***************************************************************************/
// Build the casts based on collision proximity
/***************************************************************************/
void ClusterSystem::BuildClusters(
float timeStep)
{
	#ifdef CLUSTER_SYSTEM_STATS
	u32 t1Start = g_timer.GetOSTime();
	#endif

	// Update dependencies
	UpdateActorDependencies();

	// Update the speed hull for everyone
	UpdateCollisionHulls(timeStep);

	// Sort the lists
	#ifdef CLUSTER_SYSTEM_STATS
	SortSwapsThisFrame = clusterXZ.BubbleSortAscending();
	#else
	clusterXZ.BubbleSortAscending();
	#endif

	// Put everyone on the unprocessed list
	ClusterElementList unprocessedList;
	ClusterElement *ce = clusterXZ.Head(0);
	while (ce)
	{
		CActor *actor = ce->GetActor();
		ASSERT(actor);
		if (!actor->ActorInteract() && ce->IsNotInDependency())
		{
			// put them directly into the scatter
			ce->unprocessed = ClusterElement::PROCESSED;
			if (ce->cluster != NULL)
				TransferToScatter(ce);

			#ifdef CLUSTER_SYSTEM_STATS
			NonInteractiveElements++;
			#endif
		}
		else if (ce->IsIndependent())
		{
			// Put it in the unprocessed list.  fixed guys to the rear
			// so we avoid seeding them in collisions in BuildNeighbors
			if (actor->Category().PositiveMatch(FIXED_CATEGORY) && ce->IsNotInDependency())
				unprocessedList.AddTail(ce);
			else
			{
				unprocessedList.AddHead(ce);

				#ifdef CLUSTER_SYSTEM_STATS
				SeedElements++;
				#endif
			}

			ce->unprocessed = ClusterElement::UNPROCESSED;
		}
		else
		{
			ce->unprocessed = ClusterElement::DEPENDENT;

			#ifdef CLUSTER_SYSTEM_STATS
			DependantElements++;
			#endif
		}
		ce = ce->next[0];
	}

	// Traverse, building up a cluster list
	while ((ce = unprocessedList.Head()) != NULL &&
          (!ce->GetActor()->Category().PositiveMatch(FIXED_CATEGORY) ||
			 !ce->IsNotInDependency()))

	{
		// The current cluster we are building
		ClusterElementList currentCluster;

		// Build the neighbor list for the CE
		if (!BuildNeighbors(ce, unprocessedList, currentCluster))
		{
			// This is a scatter, add them to scatter unless they are already there
			if (ce->cluster != NULL)
				TransferToScatter(ce);
		}
		else
		{
			// We have a cluster list
			ASSERT(currentCluster.Head() != NULL);
			CCluster *bestCluster = BestCluster(currentCluster);

			// No cluster?
			if (!bestCluster)
			{
				bestCluster = AddCluster();
				if (!bestCluster)
					bestCluster = BestMergeCluster();
				ASSERT(bestCluster);
			}

			PopulateCluster(currentCluster, bestCluster);
		}

		// Garbage collect the clusters
		GarbageCollectClusters();
	}

	// All remaining are fixed, put them in scatter
	while ((ce = unprocessedList.Head()) != NULL)
	{
		ASSERT(ce->GetActor()->Category().PositiveMatch(FIXED_CATEGORY));
		ASSERT(ce->unprocessed == ClusterElement::UNPROCESSED);
		ASSERT(ce->GetChild() == NULL);
		ASSERT(ce->GetNextSibling() == NULL);
		ce->unprocessed = ClusterElement::PROCESSED;
		unprocessedList.Remove(ce);
		if (ce->cluster != NULL)
			TransferToScatter(ce);
	}

	#ifdef CLUSTER_SYSTEM_STATS
	u32 t1End = g_timer.GetOSTime();
	TimeBuildClusters = t1End - t1Start;
	#endif

#ifdef PRINT_CLUSTERS
	/* may want to print it out */

	static bool bPrinted = false;
	bool bPrint = !bPrinted;
//	if ((bPrint) && (RenderMgr__FogEnabled() == false))	// fires when user toggles fog, can be handy to get snapshot you want
	if (bPrint)
	{
		bPrinted = true;

#ifndef WIN32
#define FILE void
#define fopen(a, b) NULL
#define fprintf(a, ...)
#define fclose(a)
#endif //WIN32
		FILE *pFile = fopen("c:\\temp\\cluster.txt", "wt");

		CCast* cast = NULL;
		CCastMember* castMember;
		int castID, lastCastID;
		CActor* actor;
		lastCastID = -3;
		int nActorCount = 0;
		while (actor = GetNextDebugActor(cast, castMember, castID))
		{
			if (castID != lastCastID)
			{
				if (castID >= 0)
				{
					dbgPrint("cluster %d:\n", castID);
					if (pFile)
						fprintf(pFile, "cluster %d:\n", castID);
				}
				else if (castID == -1)
				{
					dbgPrint("scatter0:\n");
					if (pFile)
						fprintf(pFile, "scatter0:\n");
				}
				else
				{
					dbgPrint("scatter1:\n");
					if (pFile)
						fprintf(pFile, "scatter1:\n");
				}
				lastCastID = castID;
			}

			dbgPrint("   %d: Actor %s (%.2f, %.2f)", nActorCount, actor->InstanceName(), actor->GetBodyInWorld().x(), actor->GetBodyInWorld().z());
			if (pFile)
				fprintf(pFile, "   %d: Actor %s (%.2f, %.2f)", nActorCount, actor->InstanceName(), actor->GetBodyInWorld().x(), actor->GetBodyInWorld().z());
			nActorCount++;
			if (actor->IsA(CSimple::ClassType()))
			{
				dbgPrint(", active = %s",
						((CSimple *) actor)->IsEnvActive() ? "true" : "false");
				if (pFile)
					fprintf(pFile, ", active = %s",
						((CSimple *) actor)->IsEnvActive() ? "true" : "false");
			}
			dbgPrint("\n");
			if (pFile)
				fprintf(pFile, "\n");
		}
		if (pFile)
			fclose(pFile);
	}
#endif //PRINT_CLUSTERS

#ifdef WRITE_FILE
	bool writeFile = false;
	if (writeFile)
	{
		FILE* fp = fopen("cluster.txt", "w");

		CCast* cast;
		CCastMember* castMember;
		int castID;
		CActor* actor;

		int pickCast = -3;

		fprintf(fp, "Labels");
		cast = NULL;
		while (actor = GetNextDebugActor(cast, castMember, castID))
		{
			if ((pickCast > -3) && (pickCast != castID))
				continue;

			if (castID >= 0)
				fprintf(fp, "\tCluster%d", castID);
			else if (castID == -1)
				fprintf(fp, "\tScatter0", castID);
			else
				fprintf(fp, "\tScatter1", castID);

			fprintf(fp, "\t%s", actor->InstanceName());
		}
		fprintf(fp, "\n");

		fprintf(fp, "++(1)");
		cast = NULL;
		while (actor = GetNextDebugActor(cast, castMember, castID))
		{
			if ((pickCast > -3) && (pickCast != castID))
				continue;
			const XZRect& hull = ((CProtagonist*)actor)->CollisionHull();
			fprintf(fp, "\t%f\t%f", hull.MaxX(), hull.MaxZ());
		}
		fprintf(fp, "\n");

		fprintf(fp, "-+(1)");
		cast = NULL;
		while (actor = GetNextDebugActor(cast, castMember, castID))
		{
			if ((pickCast > -3) && (pickCast != castID))
				continue;
			const XZRect& hull = ((CProtagonist*)actor)->CollisionHull();
			fprintf(fp, "\t%f\t%f", hull.MinX(), hull.MaxZ());
		}
		fprintf(fp, "\n");

		fprintf(fp, "Center(1)");
		cast = NULL;
		while (actor = GetNextDebugActor(cast, castMember, castID))
		{
			if ((pickCast > -3) && (pickCast != castID))
				continue;
			fprintf(fp, "\t%f\t%f", actor->GetBodyInWorld().x(),
					actor->GetBodyInWorld().z());
		}
		fprintf(fp, "\n");

		fprintf(fp, "++(2)");
		cast = NULL;
		while (actor = GetNextDebugActor(cast, castMember, castID))
		{
			if ((pickCast > -3) && (pickCast != castID))
				continue;
			const XZRect& hull = ((CProtagonist*)actor)->CollisionHull();
			fprintf(fp, "\t%f\t%f", hull.MaxX(), hull.MaxZ());
		}
		fprintf(fp, "\n");

		fprintf(fp, "+-(1)");
		cast = NULL;
		while (actor = GetNextDebugActor(cast, castMember, castID))
		{
			if ((pickCast > -3) && (pickCast != castID))
				continue;
			const XZRect& hull = ((CProtagonist*)actor)->CollisionHull();
			fprintf(fp, "\t%f\t%f", hull.MaxX(), hull.MinZ());
		}
		fprintf(fp, "\n");

		fprintf(fp, "--(1)");
		cast = NULL;
		while (actor = GetNextDebugActor(cast, castMember, castID))
		{
			if ((pickCast > -3) && (pickCast != castID))
				continue;
			const XZRect& hull = ((CProtagonist*)actor)->CollisionHull();
			fprintf(fp, "\t%f\t%f", hull.MinX(), hull.MinZ());
		}
		fprintf(fp, "\n");

		fprintf(fp, "Center(2)");
		cast = NULL;
		while (actor = GetNextDebugActor(cast, castMember, castID))
		{
			if ((pickCast > -3) && (pickCast != castID))
				continue;
			fprintf(fp, "\t%f\t%f", (float)actor->GetBodyInWorld().x(),
					(float)actor->GetBodyInWorld().z());
		}
		fprintf(fp, "\n");

		fprintf(fp, "+-(2)");
		cast = NULL;
		while (actor = GetNextDebugActor(cast, castMember, castID))
		{
			if ((pickCast > -3) && (pickCast != castID))
				continue;
			const XZRect& hull = ((CProtagonist*)actor)->CollisionHull();
			fprintf(fp, "\t%f\t%f", hull.MaxX(), hull.MinZ());
		}
		fprintf(fp, "\n");

		fprintf(fp, "--(2)");
		cast = NULL;
		while (actor = GetNextDebugActor(cast, castMember, castID))
		{
			if ((pickCast > -3) && (pickCast != castID))
				continue;
			const XZRect& hull = ((CProtagonist*)actor)->CollisionHull();
			fprintf(fp, "\t%f\t%f", hull.MinX(), hull.MinZ());
		}
		fprintf(fp, "\n");

		fprintf(fp, "-+(2)");
		cast = NULL;
		while (actor = GetNextDebugActor(cast, castMember, castID))
		{
			if ((pickCast > -3) && (pickCast != castID))
				continue;
			const XZRect& hull = ((CProtagonist*)actor)->CollisionHull();
			fprintf(fp, "\t%f\t%f", hull.MinX(), hull.MaxZ());
		}
		fprintf(fp, "\n");

		fclose(fp);
	}
#endif //WRITE_FILE
}

/***************************************************************************/
/***************************************************************************/
CCluster *ClusterSystem::AddCluster(void)
{
	// Allocate a cluster from the pool
	void *clusterPtr = clusterPool.NewPtr();
	CCluster *cluster = new (clusterPtr) CCluster(clusterElementPool);

	if (cluster)
	{
		// Add it into the list
		clusterList.AddTail(cluster);

		// Record the frame
		cluster->usedFrame = g_timer.GetFrameCount();
	}

	// Give it back to them
	return(cluster);
}

/***************************************************************************/
/***************************************************************************/
void ClusterSystem::KillCluster(
CCluster *cluster)
{
	if (cluster)
	{
		// Take me out of the list
		clusterList.Unlink(cluster);

		// Delete me back into the pool
		clusterPool.Delete(cluster);
	}
}

/***************************************************************************/
/***************************************************************************/
void ClusterSystem::KillClusters(void)
{
	// Clear these
	CCluster *cluster = clusterList.Head();
	while (cluster)
	{
		CCluster *killMe = cluster;
		cluster = cluster->next;
		KillCluster(killMe);
	}
}

/***************************************************************************/
// Get rid of any clusters with 0 elements
/***************************************************************************/
void ClusterSystem::GarbageCollectClusters(void)
{
	// Clear these
	CCluster *cluster = clusterList.Head();
	while (cluster)
	{
		if (cluster->GetNextMember(NULL) != NULL)
			cluster = cluster->next;
		else
		{
			CCluster *killMe = cluster;
			cluster = cluster->next;
			KillCluster(killMe);
		}
	}
}

/***************************************************************************/
/***************************************************************************/
bool ClusterSystem::BuildNeighbors(
ClusterElement *parent,
ClusterElementList &unprocessedList,
ClusterElementList &clusterList)
{
	bool foundNeighbor = false;
	ClusterElement *test;

	// Add us
	ASSERT(parent->unprocessed);
	if (parent->unprocessed != ClusterElement::DEPENDENT)
	{
		unprocessedList.Remove(parent);
		clusterList.AddTail(parent);
	}
	else if (parent->GetNextSibling())
	{
		test = parent->GetNextSibling()->GetClusterElement();
		if (test->unprocessed)
		{
			parent->unprocessed = ClusterElement::PROCESSED;
			foundNeighbor = true;
			BuildNeighbors(test, unprocessedList, clusterList);
		}
	}
	parent->unprocessed = ClusterElement::PROCESSED;

	if (parent->GetChild())
	{
		test = parent->GetChild()->GetClusterElement();
		if (test->unprocessed)
		{
			foundNeighbor = true;
			BuildNeighbors(test, unprocessedList, clusterList);
		}
	}

	CActor *parentActor = parent->GetActor();

	// Go +x
	test = parent->next[0];
	while (test != NULL && parent->MaxX() > test->MinX())
	{
		if (test->unprocessed && parentActor->CanInteract(*(test->GetActor())) &&
				parent->OverlapsZ(test))
		{
			foundNeighbor = true;
			BuildNeighbors(test, unprocessedList, clusterList);
		}

		test = test->next[0];
	}

	// go +z
	test = parent->next[1];
	while (test != NULL && parent->MaxZ() > test->MinZ())
	{
		if (test->unprocessed && parentActor->CanInteract(*(test->GetActor())) &&
				parent->OverlapsX(test))
		{
			foundNeighbor = true;
			BuildNeighbors(test, unprocessedList, clusterList);
		}

		test = test->next[1];
	}

	// Go -x
	test = parent->prev[0];
	while (test != NULL && parent->MinX() < test->MaxX())
	{
		if (test->unprocessed && parentActor->CanInteract(*(test->GetActor())) &&
				parent->OverlapsZ(test))
		{
			foundNeighbor = true;
			BuildNeighbors(test, unprocessedList, clusterList);
		}

		test = test->prev[0];
	}

	// go -z
	test = parent->prev[1];
	while (test != NULL && parent->MinZ() < test->MaxZ())
	{
		if (test->unprocessed && parentActor->CanInteract(*(test->GetActor())) &&
				parent->OverlapsX(test))
		{
			foundNeighbor = true;
			BuildNeighbors(test, unprocessedList, clusterList);
		}

		test = test->prev[1];
	}

	return(foundNeighbor);
}

/***************************************************************************/
/***************************************************************************/
CCluster *ClusterSystem::BestCluster(
ClusterElementList &list)
{
	struct MembershipCount
	{
		CCluster		*cluster;
		uint			count;
	};
#define MAX_MEMBERSHIPS	32
	MembershipCount	membership[MAX_MEMBERSHIPS];
	uint memberships = 0;

	// Build up the memberships
	uint32 frame = g_timer.GetFrameCount();
	ClusterElement *ce = list.Head();
	while (ce)
	{
		// scatter or used?
		if ((ce->cluster != NULL) && (ce->cluster->usedFrame != frame))
		{
			// find this cluster in the memberships
			uint i;
			for (i = 0; i < memberships; i++)
				if (membership[i].cluster == ce->cluster)
					break;

			// Did we find it?
			if (i < memberships)
				++membership[i].count;
			else if (memberships < MAX_MEMBERSHIPS)
			{
				// Create a new membership
				membership[memberships].cluster = ce->cluster;
				membership[memberships].count = 1;
				++memberships;
			}
		}

		// Go to the next one
		ce = ce->processingNext;
	}

	// simple case
	if (memberships == 0)
		return(NULL);

	// Which is best cluster
	uint bestMembership = 0;
	for (uint i = 1; i < memberships; i++)
	{
		if (membership[i].count > membership[bestMembership].count)
			bestMembership = i;
	}

	// Record the frame
	membership[bestMembership].cluster->usedFrame = frame;

	// give it to 'em
	return(membership[bestMembership].cluster);
}

/***************************************************************************/
// Oh shit, there are no free clusters right now.  Merge with the least
// full cluster...
/***************************************************************************/
CCluster *ClusterSystem::BestMergeCluster(void)
{
	// assume the first is the best
	CCluster *best = clusterList.Head();
	if (!best)
		return(NULL);

	// find the fewest
	uint bestMembers = best->Members();
	CCluster *cluster = best->next;
	while (cluster)
	{
		// How many does this one have?
		uint members = cluster->Members();

		// Is it better?
		if (members < bestMembers)
		{
			bestMembers = members;
			best = cluster;
		}

		// next please
		cluster = cluster->next;
	}

	return(best);
}

/***************************************************************************/
/***************************************************************************/
void ClusterSystem::UnlinkClusterElement(
ClusterElement *clusterElement)
{
	clusterXZ.Unlink(clusterElement);
}

/***************************************************************************/
// Special notify for the removal of an actor
/***************************************************************************/
void ClusterSystem::DeleteClusterElementNotify(
ClusterSystem *clusterSystem,
ClusterElement *killMe)
{
	ASSERT(clusterSystem);
	ASSERT(killMe);

	// Get rid of it
	clusterSystem->UnlinkClusterElement(killMe);
}

/***************************************************************************/
// Populate the cluster sorted by update order. The actors that are in
// the group to be processed first (process first actors) and that are
// independent or that depend only on other process first actors (in
// dependency order) are updated first. Then, if there are process first
// actors that depend on process last actors, the direct ancestors of the
// process first actor and the actor itself are processed. Finally, the
// remaining process last actors that are independent or that depend only on
// other process last actors (in dependency order) are processed.
/***************************************************************************/
void ClusterSystem::PopulateCluster(ClusterElementList &i_srcList,
		CCluster* o_destCluster)
{
/*
 *	This will be a list of roots of dependency trees where the root should be
 *	an actor in the group to be processed last.
 */
	ClusterElementList processLastList;
	CProtagonist* actor;
	ClusterElement* prev = NULL;
/*
 *	Go through each member of the source list. (Note: the source list only
 *	contians independent actors.)
 */
	ClusterElement* curr;
	while (curr = i_srcList.PopHead())
	{
/*
 *		Identify dependency tree roots.
 */
		actor = (CProtagonist*)(curr->GetActor());
/*
 *		If the root of the tree should be processed last, add it to the list.
 */
		if (actor->Category().PositiveMatch(PROCESS_LAST_CATEGORY))
			processLastList.AddTail(curr);
/*
 *		The root is in the group to be processed first.
 */
		else
		{
/*
 *			Put the root actor in the cluster.
 */
			InsertInCluster(curr, prev, o_destCluster);
/*
 *			Descend the dependency tree (if any) and update the children. If
 *			any child is a member of the group to be processed last, it will
 *			not be moved to the cluster but will be added to the process last
 *			list. The tree will be, in effect, pruned at that point and the
 *			pruned branch will become a dependency tree in its own right,
 *			rooted by an actor in the group to be processed last.
 */
			if (curr->GetChild())
			{
				DescendUpdateFirst(curr->GetChild(), prev, o_destCluster,
						processLastList);
			}
		}
	}
/*
 *	The only actors in the source list that have not been processed are in trees
 *	rooted by actors in the group to be processed last. Walk through that list
 *	looking for any actors that are in the group to be processed first that
 *	depend on an actor in the group to be processed last.
 */
	curr = processLastList.Head();
	while (curr)
	{
/*
 *		Only trees with children can contain process first actors.
 */
		if (curr->GetChild())
		{
/*
 *			Look through the tree for process first actors. If one is found,
 *			its direct ancestors are updated and then its tree is updated just as
 *			the trees rooted in process first actors are updated above. In the
 *			process, trees rooted by process last actors may be found and added
 *			to the process last list. Also, siblings of the process first actor
 *			as well as siblings of it direct ancestors are either added to the
 *			process last list (if the sibling is a process last actor) or updated
 *			(if the sibling is a process first actor). Again, sub-trees of process
 *			first siblings may be added to the process last list.
 */
			if (DescendUpdateDependentFirst((CProtagonist*)(curr->GetActor()),
					curr->GetChild(), prev, o_destCluster, processLastList))
			{
/*
 *				If a process first actor is found, the current element and its
 *				children have either been updated or added to the process last
 *				list in new entries. Remove the current element.
 */
				ClusterElement* processed = curr;
				curr = processLastList.GetNextElement(curr);
				processLastList.Remove(processed);
				continue;
			}
		}
		curr = processLastList.GetNextElement(curr);
	}
/*
 *	The process last list now contains trees populated only by actors in the
 *	process last list. Update them.
 */
	while (curr = processLastList.PopHead())
	{
		actor = (CProtagonist*)(curr->GetActor());
		InsertInCluster(curr, prev, o_destCluster);
		if (curr->GetChild())
			DescendUpdateLast(curr->GetChild(), prev, o_destCluster);
	}
}

/***************************************************************************/
// Descend a dependency tree down to but not including a process last actor
/***************************************************************************/
void ClusterSystem::DescendUpdateFirst(CProtagonist* i_actor,
		ClusterElement* &io_prevElement, CCluster* o_destCluster,
		ClusterElementList &io_processLastList)
{
/*
 *	Get the cluster element and make sure it is valid.
 */
	ClusterElement* clusterElement = i_actor->GetClusterElement();
	ASSERT(clusterElement);
/*
 *	If the actor is in the group to be processed last, add it to the process last
 *	list.
 */
	if (i_actor->Category().PositiveMatch(PROCESS_LAST_CATEGORY))
		io_processLastList.AddTail(clusterElement);
/*
 *	The actor is in the group to be processed first.
 */
	else
	{
/*
 *		Move the actor to the cluster.
 */
		InsertInCluster(clusterElement, io_prevElement, o_destCluster);
/*
 *		Recursively process the children.
 */
		if (clusterElement->GetChild())
		{
			DescendUpdateFirst(clusterElement->GetChild(), io_prevElement,
					o_destCluster, io_processLastList);
		}
	}
/*
 *	Recursively call the siblings.
 */
	if (clusterElement->GetNextSibling())
	{
		DescendUpdateFirst(clusterElement->GetNextSibling(), io_prevElement,
				o_destCluster, io_processLastList);
	}
}

/***************************************************************************/
/***************************************************************************/
bool ClusterSystem::DescendUpdateDependentFirst(CProtagonist* i_root,
		CProtagonist* i_actor, ClusterElement* &io_prevElement,
		CCluster* o_destCluster, ClusterElementList &io_processLastList)
{
/*
 *	Get the cluster element and make sure it is valid.
 */
	ClusterElement* clusterElement = i_actor->GetClusterElement();
	ASSERT(clusterElement);
/*
 *	See if the actor is in the group to be processed first.
 */
	if (!i_actor->Category().PositiveMatch(PROCESS_LAST_CATEGORY))
	{
/*
 *		Process, in the correct order, the ancestors of the actor. Also, either
 *		process the siblings of the ancestors and actor or add them to the process
 *		last list.
 */
		AscendUpdateDependentFirst(i_root, clusterElement->GetMaster(), i_actor,
				io_prevElement, o_destCluster, io_processLastList);
/*
 *		Move the actor to the cluster.
 */
		InsertInCluster(clusterElement, io_prevElement, o_destCluster);
/*
 *		Descend the actor's children (its siblings have already been dealt with).
 */
		if (clusterElement->GetChild())
		{
			DescendUpdateFirst(clusterElement->GetChild(), io_prevElement,
					o_destCluster, io_processLastList);
		}
/*
 *		Indicate that a process first actor was found.
 */
		return(true);
	}
/*
 *	Process the actor's sibling tree and return true if a process first actor
 *	was found in it.
 */
	if (clusterElement->GetNextSibling() &&
			DescendUpdateDependentFirst(i_root, clusterElement->GetNextSibling(),
			io_prevElement, o_destCluster, io_processLastList))
		return(true);
/*
 *	Process children, if any.
 */
	if (clusterElement->GetChild())
	{
		return(DescendUpdateDependentFirst(i_root, clusterElement->GetChild(),
				io_prevElement, o_destCluster, io_processLastList));
	}
/*
 *	Indicate that no process first actor was found.
 */
	return(false);
}

/***************************************************************************/
// Work up to the top of the tree then descend along the line of ascent,
// pruning sibling branches along the way.
/***************************************************************************/
void ClusterSystem::AscendUpdateDependentFirst(CProtagonist* i_root,
		CProtagonist* i_actor, CProtagonist* i_child,
		ClusterElement* &io_prevElement, CCluster* o_destCluster,
		ClusterElementList &io_processLastList)
{
	ASSERT(i_actor);
	ClusterElement* clusterElement = i_actor->GetClusterElement();
	ASSERT(clusterElement);
/*
 *	Keep climbing if not at the root of the tree.
 */
	if (i_actor != i_root)
	{
		AscendUpdateDependentFirst(i_root, clusterElement->GetMaster(), i_actor,
				io_prevElement, o_destCluster, io_processLastList);
	}
/*
 *	Move the actor to the cluster.
 */
	InsertInCluster(clusterElement, io_prevElement, o_destCluster);
/*
 *	Deal with each child that is not the descendant of interest.
 */
	CProtagonist* currChild = clusterElement->GetChild();
	while (currChild)
	{
		clusterElement = currChild->GetClusterElement();
		ASSERT(clusterElement);
		if (currChild != i_child)
		{
/*
 *			If the child is a process last actor, add it to the list.
 */
			if (currChild->Category().PositiveMatch(PROCESS_LAST_CATEGORY))
				io_processLastList.AddTail(clusterElement);
/*
 *			If the child is a process fist actor, update the tree, pruning any
 *			process last branches and adding them to the list.
 */
			else
			{
				InsertInCluster(clusterElement, io_prevElement, o_destCluster);
				if (clusterElement->GetChild())
				{
					DescendUpdateFirst(clusterElement->GetChild(), io_prevElement,
							o_destCluster, io_processLastList);
				}
			}
		}
		currChild = clusterElement->GetNextSibling();
	}
}

/***************************************************************************/
// Descend a dependency tree (which will contain only process last actors).
/***************************************************************************/
void ClusterSystem::DescendUpdateLast(CProtagonist* i_actor,
		ClusterElement* &io_prevElement, CCluster* o_destCluster)
{
/*
 *	Get the cluster element and make sure it is valid.
 */
	ClusterElement* clusterElement = i_actor->GetClusterElement();
	ASSERT(clusterElement);
/*
 *	Move the actor to the cluster.
 */
	InsertInCluster(clusterElement, io_prevElement, o_destCluster);
/*
 *	Recursively call the siblings.
 */
	if (clusterElement->GetNextSibling())
	{
		DescendUpdateLast(clusterElement->GetNextSibling(), io_prevElement,
				o_destCluster);
	}
/*
 *	Recursively call the children.
 */
	if (clusterElement->GetChild())
	{
		DescendUpdateLast(clusterElement->GetChild(), io_prevElement,
				o_destCluster);
	}
}

CActor* ClusterSystem::GetNextDebugActor(CCast* &io_cast,
		CCastMember* &io_castMember, int &io_castID)
{
	if (io_cast == NULL)
	{
		io_cast = ClusterHead();
		if (io_cast)
			io_castID = 0;
		else
		{
			io_cast = &scatter0;
			io_castID = -1;
		}
		io_castMember = NULL;
	}

	io_castMember = io_cast->GetNextMember(io_castMember);
	while (io_castMember == NULL)
	{
		if (io_cast == &scatter1)
			return(NULL);
		else if (io_cast == &scatter0)
		{
			io_cast = &scatter1;
			io_castID = -2;
		}
		else
		{
			io_cast = ((CCluster*) io_cast)->next;
			if (io_cast)
				io_castID++;
			else
			{
				io_cast = &scatter0;
				io_castID = -1;
			}
		}
		io_castMember = io_cast->GetNextMember(io_castMember);
	}

	return(io_castMember->GetActor());
}
