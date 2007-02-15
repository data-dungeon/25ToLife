/***************************************************************************/
// The clustering system
/***************************************************************************/
#ifndef CLUSTERSYS_H
#define CLUSTERSYS_H

#include "container/pool.h"
#include "container/dllist.h"
#include "container/nllist.h"

#include "stage/protagonist.h"
#include "stage/cluster.h"
#include "stage/scatter.h"
#include "stage/clustere.h"
#include "stage/ClusterElementPool.h"

//#ifndef CDROM
#define CLUSTER_SYSTEM_STATS
//#endif

// forwards
class ClusterElementList;

class ClusterSystem
{
public:
	// Make destroy stuff (this is my most clever comment)
	ClusterSystem();
	~ClusterSystem(void);

	// Initialization
	void Initialize(void);
	void Terminate(void);

	// Force all off screen objects into the scatter	
	void OffScreenScatter(bool enableOffscreenScatter);

	// Handle adding/removing actors
	void Add(CProtagonist *actor);
	void Remove(CProtagonist *actor);

	// Build the casts based on collision proximity
	void BuildClusters(float timeStep);

	// The scatter cast (2 is a cheesy hack!)
	// 0 is the bikes
	// 1 is EVERYTHING else
	// when the pilot becomes a real protagonist, this can go away
	CScatter &Scatter(uint which)
		{ if (which == 0) return(scatter0); return(scatter1); }

	// Get the head of the cluster cast
	CCluster *ClusterHead(void)
		{ return(clusterList.Head()); }

	#ifdef CLUSTER_SYSTEM_STATS
	int GetSwapsThisFrame()
		{ int retval = SortSwapsThisFrame; SortSwapsThisFrame = 0; return retval; }
	int GetNonInteractiveElements()
		{ int retval = NonInteractiveElements; NonInteractiveElements = 0; return retval; }
	int GetSeedElements()
		{ int retval = SeedElements; SeedElements = 0; return retval; }
	int GetDependantElements()
		{ int retval = DependantElements; DependantElements = 0; return retval; }
	uint32 GetTimeBuildClusters()
		{ uint32 retval = TimeBuildClusters; TimeBuildClusters = 0; return retval; }
	#endif
private:
	// The cast member pool
	// Must be before casts so the pool is constructed before (not so
	// important) and destructed after (very important) them
	CClusterElementPool			clusterElementPool;

	// The clutster elements
	// Must be before casts so elements can be unlinked as casts are destructed
	NLinkedList<ClusterElement, 2>	clusterXZ;

	// The protagonists not in collision proximity
	CScatter							scatter0;
	CScatter							scatter1;

	// The list of clusters of protagonists in the world
	DoublyLinkedList<CCluster>	clusterList;

	// number of swaps required for XZ bubble sort 
	// last time BuildClusters was called.
	#ifdef CLUSTER_SYSTEM_STATS
	int SortSwapsThisFrame;
	int NonInteractiveElements, SeedElements, DependantElements;
	uint32 TimeBuildClusters;
	#endif

	// A pool for all the cluster casts
	Pool<CCluster>					clusterPool;
	CCluster *AddCluster(void);
	void KillCluster(CCluster *cluster);
	void KillClusters(void);
	void GarbageCollectClusters(void);

	// Utilties
	void UpdateActorDependencies(void);
	void ProcessValidDependency(CProtagonist* i_actor);
	void CheckValidDependency(CProtagonist* i_actor);
	inline void UpdateCollisionHulls(float vFactor);
	inline void TransferCluster(ClusterElement *element, CCluster *cluster);
	inline void InsertInCluster(ClusterElement *element, ClusterElement* &prevElement, CCluster *cluster);
	inline void TransferToScatter(ClusterElement *element);
	bool BuildNeighbors(ClusterElement *parent, ClusterElementList &unprocessedList, ClusterElementList &clusterList);

	void PopulateCluster(ClusterElementList &i_srcList, CCluster* o_destCluster);
	void DescendUpdateFirst(CProtagonist* i_actor,
			ClusterElement* &io_prevElement, CCluster* o_destCluster,
			ClusterElementList &io_processLastList);
	bool DescendUpdateDependentFirst(CProtagonist* i_root, CProtagonist* i_actor,
			ClusterElement* &io_prevElement, CCluster* o_destCluster,
			ClusterElementList &io_processLastList);
	void AscendUpdateDependentFirst(CProtagonist* i_root, CProtagonist* i_actor,
			CProtagonist* i_child, ClusterElement* &io_prevElement,
			CCluster* o_destCluster, ClusterElementList &io_processLastList);
	void DescendUpdateLast(CProtagonist* i_actor,
			ClusterElement* &io_prevElement, CCluster* o_destCluster);

	CCluster *BestCluster(ClusterElementList &list);
	CCluster *BestMergeCluster(void);

	// Clean up when a cluster element is destroyed
	void UnlinkClusterElement(ClusterElement *clusterElement);
	static void DeleteClusterElementNotify(ClusterSystem *clusterSystem, ClusterElement *killMe);

	// For debugging
	CActor* GetNextDebugActor(CCast* &io_cast, CCastMember* &io_castMember,
			int &io_castID);
};

// Inline implementations
#include "stage/clustersys.hpp"

#endif
