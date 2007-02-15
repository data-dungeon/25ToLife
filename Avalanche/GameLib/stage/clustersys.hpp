/***************************************************************************/
// Inlines for the clustering system
/***************************************************************************/

/***************************************************************************/
/***************************************************************************/
inline void ClusterSystem::UpdateCollisionHulls(
float timeStep)
{
	ClusterElement *ce = clusterXZ.Head(0);
	while (ce)
	{
		CProtagonist *actor = (CProtagonist *)ce->GetActor();
		ASSERT(actor);
		actor->UpdateCollisionHull(timeStep);
		ce->Update();
		ce = ce->next[0];
	}
}

/***************************************************************************/
/***************************************************************************/
inline void ClusterSystem::TransferCluster(
ClusterElement *element,
CCluster *cluster)
{
	element->ChangeCast(cluster);
	element->cluster = cluster;
}

/***************************************************************************/
/***************************************************************************/
inline void ClusterSystem::InsertInCluster(
ClusterElement *element,
ClusterElement* &prevElement,
CCluster *cluster)
{
	if (element->cluster != cluster)
	{
		element->ChangeCast(cluster, prevElement);
		element->cluster = cluster;
	}
	else if (element->GetPrevActor() != (CCastMember*)prevElement)
		element->ChangeOrder(prevElement);
	prevElement = element;
}

/***************************************************************************/
/***************************************************************************/
inline void ClusterSystem::TransferToScatter(
ClusterElement *element)
{
	CProtagonist *p = (CProtagonist *)element->GetActor();
	if (p->Category().PositiveMatch(PROCESS_LAST_CATEGORY))
		element->ChangeCast(&scatter1);
	else
		element->ChangeCast(&scatter0);
	element->cluster = NULL;
}
