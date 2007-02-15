/***************************************************************************/
// Collision pairs
/***************************************************************************/

/******************* includes ****************************/

#include "CollideHelper/CollideHelperPCH.h"

// My header (always first) //
#include "CollideHelper/obbchild.h"

/***************************************************************************/
/***************************************************************************/
OBBChildList::OBBChildList(
OBB &parentOBB,
uint howManyChildren) :
parent(parentOBB)
{
	maxChildren = howManyChildren;
	children = maxChildren;
	testChild = maxChildren;
	child = new OBBChild[children];
	ASSERT(child);
}

/***************************************************************************/
/***************************************************************************/
OBBChildList::~OBBChildList(void)
{
	delete[] child;
}

/***************************************************************************/
// Debugging
/***************************************************************************/
void OBBChildList::Draw(
DrawUtility::COLOR color) const
{
	// Draw the parent
	parent.Draw(DrawUtility::WHITE);

	for (uint i = 0; i < children; i++)
	{
		OBB &childOBB = child[i].GetOBB();
		childOBB.Draw(color);
	}
}

/***************************************************************************/
/***************************************************************************/
CCollisionObject* OBBChildList::BeginObjectTest(void)
{
	return(&parent);
}

/***************************************************************************/
/***************************************************************************/
bool OBBChildList::IgnoreIntersection(CCollisionIntersection* i_intersection)
{
	return(true);
}

/***************************************************************************/
/***************************************************************************/
void OBBChildList::EndObjectTest (ETestResult i_result, CCollisionPair* i_pair)
{
	if (i_result == INTERSECTION_IGNORED)
		testChild = -1;
	else
		testChild = children;
}

/***************************************************************************/
/***************************************************************************/
CCollisionNode* OBBChildList::GetChild(void)
{
	if (++testChild < (int)children)
		return(&child[testChild]);

	return(NULL);
}

/***************************************************************************/
// Intersect a line with the child list. The index of the last child hit
// is returned. The line's find mode will determine which child is last. If
// a value less than 0 is returned, no hit occurred. The line to OBB
// intersection object is filled out for the detected hit.
/***************************************************************************/
int OBBChildList::Test(CLineCollisionObject &i_line,
		CLineToObbIntersection &o_intersection) const
{
	int found = -1;
	CLineToObbIntersection testIntersection;

	i_line.InitializeFindMode();
	testIntersection.Set(&i_line, &parent);
	if (testIntersection.IntersectionTest())
	{
		for (uint i = 0; i < children; i++)
		{
			testIntersection.Set(&i_line, &child[i].GetOBB());
			if (testIntersection.IntersectionTest())
			{
				o_intersection = testIntersection;
				found = (int)i;
			}
		}
	}
	i_line.TerminateFindMode();
	return(found);
}
