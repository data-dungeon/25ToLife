/***************************************************************************/
/***************************************************************************/
#ifndef CLUSTERE_H
#define CLUSTERE_H

#include "platform/BaseType.h"
#include "stage/castmember.h"
#include "stage/protagonist.h"

class CActor;
class CCast;
class CCluster;

class ClusterElement : public CCastMember
{
private:
	// Manage dependent actors
	struct SDepend
	{
		CProtagonist*	d_master;
		CProtagonist*	d_prevSibling;
		CProtagonist*	d_nextSibling;
		CProtagonist*	d_child;
	};
	SDepend				d_depend;

public:
	// Call these and die
	ClusterElement(CActor* a_actor, CCast* a_cast);
	virtual ~ClusterElement();

	// Our xz linked lists
	ClusterElement		*prev[2];
	ClusterElement		*next[2];

	// Are we in a cluster (on the build pass)
	enum EProcessed
	{
		PROCESSED = 0,
		DEPENDENT,
		UNPROCESSED
	};
	EProcessed			unprocessed;

	// What is our current cluster (null for scatter)
	CCluster				*cluster;

	// Update local data
	XZRect				collisionHull;
	inline void Update();

	// Does our speed hull overlap this guy?
	inline bool Overlaps(ClusterElement *ce);
	inline bool OverlapsX(ClusterElement *ce);
	inline bool OverlapsZ(ClusterElement *ce);
	inline float MinX(void);
	inline float MaxX(void);
	inline float MinZ(void);
	inline float MaxZ(void);

	// operators for sorting
	inline int operator<(const ClusterElement &ce) const;
	inline int operator>(const ClusterElement &ce) const;

	// For sorting: int Compare(T &a, T &b, uint forN) returns:
	//	< 0: a < b
	//   0: a == b
	//	> 0: a > b
	inline static int Compare(ClusterElement &a, ClusterElement &b, uint forN);

	// Register a notify function
	typedef void (*MEMBERDELFN)(void *context, ClusterElement *member);
	void RegisterDeleteNotify(MEMBERDELFN fn, void *context)
		{ d_memberDeleteFn = fn; d_memberDeleteContext = context; }

	// Manage dependent actors
	CProtagonist::EDependOnResult DependOn(CProtagonist* i_master);
	void CancelDependOn(void);
	void UpdateDependentActors(void);
	bool IsNotInDependency(void)
		{ return((d_depend.d_master == NULL) && (d_depend.d_child == NULL)); }
	bool IsIndependent(void)
		{ return(d_depend.d_master == NULL); }
	bool IsProposedIndependent(void)
		{ return(d_newDepend.d_master == NULL); }
	CProtagonist* GetMaster(void)
		{ return(d_depend.d_master); }
	CProtagonist* GetNextSibling(void)
		{ return(d_depend.d_nextSibling); }
	CProtagonist* GetProposedNextSibling(void)
		{ return(d_newDepend.d_nextSibling); }
	CProtagonist* GetChild(void)
		{ return(d_depend.d_child); }
	CProtagonist* GetProposedChild(void)
		{ return(d_newDepend.d_child); }
	bool IsChangeProposed(void)
		{ return(d_depend.d_master != d_newDepend.d_master); }

	// Our processing list
	ClusterElement		*processingNext;
	ClusterElement		*processingPrev;


private:
	MEMBERDELFN			d_memberDeleteFn;
	void					*d_memberDeleteContext;

	SDepend				d_newDepend;

};

// Inlines
#include "stage/clustere.hpp"

#endif
