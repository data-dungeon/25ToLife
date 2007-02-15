/***************************************************************************/
/***************************************************************************/
#include "stage/protagonist.h"

/***************************************************************************/
// Upate and copy our local info
/***************************************************************************/
inline void ClusterElement::Update()
{
	CProtagonist *pa = (CProtagonist *)GetActor();
	ASSERT(pa);
	if (pa)
		collisionHull = pa->CollisionHull();
}

/***************************************************************************/
// Does our collision hull overlap this guy?
/***************************************************************************/
inline bool ClusterElement::Overlaps(
ClusterElement *ce)
{
	ASSERT(ce);
	return(collisionHull.Intersects(ce->collisionHull));
}

/***************************************************************************/
/***************************************************************************/
inline bool ClusterElement::OverlapsX(
ClusterElement *ce)
{
	ASSERT(ce);
	return(collisionHull.IntersectsX(ce->collisionHull));
}

/***************************************************************************/
/***************************************************************************/
inline bool ClusterElement::OverlapsZ(
ClusterElement *ce)
{
	ASSERT(ce);
	return(collisionHull.IntersectsZ(ce->collisionHull));
}

/***************************************************************************/
/***************************************************************************/
inline float ClusterElement::MinX(void)
{
	return(collisionHull.MinX());
}

/***************************************************************************/
/***************************************************************************/
inline float ClusterElement::MaxX(void)
{
	return(collisionHull.MaxX());
}

/***************************************************************************/
/***************************************************************************/
inline float ClusterElement::MinZ(void)
{
	return(collisionHull.MinZ());
}

/***************************************************************************/
/***************************************************************************/
inline float ClusterElement::MaxZ(void)
{
	return(collisionHull.MaxZ());
}

/***************************************************************************/
// operators for sorting
/***************************************************************************/
inline int ClusterElement::operator<(
const ClusterElement &ce) const
{
	return(collisionHull.CenterX() <= ce.collisionHull.CenterX());
}

/***************************************************************************/
// operators for sorting
/***************************************************************************/
inline int ClusterElement::operator>(
const ClusterElement &ce) const
{
	return(collisionHull.CenterX() > ce.collisionHull.CenterX());
}

/***************************************************************************/
// For sorting: int Compare(T &a, T &b, uint forN) returns:
//	< 0: a < b
//   0: a == b
//	> 0: a > b
/***************************************************************************/
inline int ClusterElement::Compare(
ClusterElement &a,
ClusterElement &b,
uint forN)
{
	// X or Z?
	float d;
	if (forN == 0)
		d = a.collisionHull.CenterX() - b.collisionHull.CenterX();
	else
		d = a.collisionHull.CenterZ() - b.collisionHull.CenterZ();

	// what an enormous pain in the ass
	if (d > 1e-4f)
		return(1);
	else if (d < -1e-4f)
		return(-1);
	else
		return(0);
}









