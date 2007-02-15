/***************************************************************************/
/***************************************************************************/

/******************* includes ****************************/

#include "stage/StagePCH.h"

/***************************************************************************/
/***************************************************************************/
ClusterElement::ClusterElement(CActor* a_actor, CCast* a_cast) :
CCastMember(a_actor, a_cast)
{
	collisionHull.Clear();
	processingNext = processingPrev = NULL;
	cluster = NULL;
	d_memberDeleteFn = NULL;
	d_memberDeleteContext = NULL;

	// Initialize dependent actor stuff
	d_depend.d_master = NULL;
	d_depend.d_prevSibling = NULL;
	d_depend.d_nextSibling = NULL;
	d_depend.d_child = NULL;
	d_newDepend = d_depend;
}

ClusterElement::~ClusterElement()
{
	// Call the delete notify
	if (d_memberDeleteFn != NULL)
		d_memberDeleteFn(d_memberDeleteContext, this);

	// Clean up dependent actor stuff

	// Remove this from the proposed dependency tree
	if (d_newDepend.d_prevSibling)
	{
		d_newDepend.d_prevSibling->GetClusterElement()->d_newDepend.d_nextSibling
				= d_newDepend.d_nextSibling;
	}
	else if (d_newDepend.d_master)
	{
		d_newDepend.d_master->GetClusterElement()->d_newDepend.d_child =
				d_newDepend.d_nextSibling;
	}

	if (d_newDepend.d_nextSibling)
	{
		d_newDepend.d_nextSibling->GetClusterElement()->d_newDepend.d_prevSibling
				= d_newDepend.d_prevSibling;
	}

	while (d_newDepend.d_child)
	{
		// Record the current head of the child list and get the next one
		ClusterElement* childElement = d_newDepend.d_child->GetClusterElement();
		d_newDepend.d_child = childElement->d_newDepend.d_nextSibling;

		// Free the child
		childElement->d_newDepend.d_prevSibling = NULL;
		childElement->d_newDepend.d_nextSibling = NULL;
		childElement->d_newDepend.d_master = NULL;
	}

	// Remove this from the current dependency tree
	if (d_depend.d_prevSibling)
	{
		d_depend.d_prevSibling->GetClusterElement()->d_depend.d_nextSibling =
				d_depend.d_nextSibling;
	}
	else if (d_depend.d_master)
	{
		d_depend.d_master->GetClusterElement()->d_depend.d_child =
				d_depend.d_nextSibling;
	}

	if (d_depend.d_nextSibling)
	{
		d_depend.d_nextSibling->GetClusterElement()->d_depend.d_prevSibling =
				d_depend.d_prevSibling;
	}

	while (d_depend.d_child)
	{
		// Tell the child that its master is going away
		d_depend.d_child->MasterDeleteNotify(d_depend.d_master);

		// Record the current head of the child list and get the next one
		ClusterElement* childElement = d_depend.d_child->GetClusterElement();
		d_depend.d_child = childElement->d_depend.d_nextSibling;

		// Free the child
		childElement->d_depend.d_prevSibling = NULL;
		childElement->d_depend.d_nextSibling = NULL;
		childElement->d_depend.d_master = NULL;
	}
}

CProtagonist::EDependOnResult ClusterElement::DependOn(CProtagonist* i_master)
{
	// Make sure the new master can accept dependencies
	if (i_master && (i_master->GetClusterElement() == NULL))
		return(CProtagonist::DEPEND_ON_NO_MASTER_CLUSTER_ELEMENT);

	// Is there a dependency change?
	if (i_master == d_newDepend.d_master)
		return(CProtagonist::DEPEND_ON_NO_CHANGE);

	// Remove me from the current master
	if (d_newDepend.d_master)
	{
		if (d_newDepend.d_prevSibling)
			d_newDepend.d_prevSibling->GetClusterElement(
					)->d_newDepend.d_nextSibling = d_newDepend.d_nextSibling;
		else
			d_newDepend.d_master->GetClusterElement()->d_newDepend.d_child =
					d_newDepend.d_nextSibling;

		if (d_newDepend.d_nextSibling)
			d_newDepend.d_nextSibling->GetClusterElement(
					)->d_newDepend.d_prevSibling = d_newDepend.d_prevSibling;

		d_newDepend.d_prevSibling = NULL;
		d_newDepend.d_nextSibling = NULL;
	}

	// Add me to the new master
	if (i_master)
	{
		d_newDepend.d_nextSibling =
				i_master->GetClusterElement()->d_newDepend.d_child;
		i_master->GetClusterElement()->d_newDepend.d_child =
				(CProtagonist*)GetActor();
		if (d_newDepend.d_nextSibling)
		{
			d_newDepend.d_nextSibling->GetClusterElement(
					)->d_newDepend.d_prevSibling = (CProtagonist*)GetActor();
		}
#if _DEBUG
		if (i_master->Category().PositiveMatch(PROCESS_LAST_CATEGORY) &&
				!GetActor()->Category().PositiveMatch(PROCESS_LAST_CATEGORY))
			conPuts("WARNING: Process first group actor depends on "
					"process last group actor\n");
#endif

	}

	// Check for an unprocessed proposed change
	if (d_depend.d_master != d_newDepend.d_master)
		((CProtagonist*)GetActor())->DependOnChanged(i_master);

	// Set my new master
	d_newDepend.d_master = i_master;

	return(CProtagonist::DEPEND_ON_PROPOSED);
}

void ClusterElement::CancelDependOn(void)
{
	DependOn(d_depend.d_master);
}

void ClusterElement::UpdateDependentActors(void)
{
	// Is there a dependency change?
	bool change = (d_newDepend.d_master != d_depend.d_master);

	// Copy the dependencies
	d_depend = d_newDepend;

	// Let the actor know it worked
	if (change)
		((CProtagonist*)GetActor())->DependOnSucceeded();
}
