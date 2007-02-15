/*
**
**  File:   Node.h
**  Date:   March 16, 2003
**  By:     Bryant Collard
**  Desc:   A collision object hierarchy.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/Node.h $
**  $Revision: 3 $
**      $Date: 4/25/03 7:24p $
**    $Author: Bcollard $
**
*/

#ifndef COLLISION_NODE_H
#define COLLISION_NODE_H

class CCollisionObject;
class CCollisionPair;
class CCollisionIntersection;

////////////////////////////// CCollisionNode //////////////////////////////

class CCollisionNode
{
  public:
	// Result code for the test of this node's collision object with another
	// node's collision object.
	enum ETestResult
	{
		NO_TEST,               // The other node has no collision object.
		NO_INTERSECTION,       // The objects do not intersect.
		INTERSECTION_IGNORED,  // The objects intersect but it was not added
                             // to the event.
		INTERSECTION_ACCEPTED  // The objects intersect and it was added to the
                             // event.
	};

	virtual ~CCollisionNode() {}

	// A collision node may contain a collision object. The specifics of the
	// object are set when the node is obtained. Requesting the object notifies
	// the the node that it is beginning a test between this node's collision
	// object with another node's collisio object.
	virtual CCollisionObject* BeginObjectTest(void) {return(NULL);}

	// Give the node the opportunity to prevent a detected intersection from
	// being added to the event.
	virtual bool IgnoreIntersection(CCollisionIntersection* i_intersection)
		{return(false);}

	// An object intersection test is complete. If a call to BeginObjectTest
	// returned NULL, no test could be attempted so a corresponding EndObjectTest
	// is not called. Otherwise, each BeginObjectTest call will have a
	// corresponding call to EndObjectTest.
	virtual void EndObjectTest (ETestResult i_result,
			CCollisionPair* i_pair = NULL) {}

	// Get the next child the node wants to serve. A call to BeginObjectTest
	// initiates child selection. If the node has a collision object, the
	// call to EndObjectTest can affect child selection. If the results of tests
	// of previous children affect the selection of subsequent children, that
	// must be handled in the implementation of the relevant CCollisionNodes.
	virtual CCollisionNode* GetChild(void) {return(NULL);}
};

//////////////////////////////// CSimpleNode ///////////////////////////////
class CSimpleNode : public CCollisionNode
{
  public:
	CSimpleNode() {d_object = NULL;}
	CSimpleNode(CCollisionObject* i_object) {d_object = i_object;}
	void Set(CCollisionObject* i_object) {d_object = i_object;}
	virtual CCollisionObject* BeginObjectTest(void) {return(d_object);}

  private:
	CCollisionObject* d_object;
};

#endif // COLLISION_NODE_H
