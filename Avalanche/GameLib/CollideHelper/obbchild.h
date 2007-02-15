/***************************************************************************/
// A simple class to manage a list of OBB's
/***************************************************************************/
#ifndef OBBCHILD_H
#define OBBCHILD_H

#include "platform/BaseType.h"
#include "EngineHelper/drawutil.h"
#include "collide/Node.h"
#include "collide/obb.h"

class DirCos3x3;
class Cartesian4x4;
class CCollisionCull;
class CCollisionPair;
class CLineCollisionObject;
class CLineToObbIntersection;

class OBBChild : public CCollisionNode
{
public:
	inline OBBChild(void) {}
	inline OBBChild(OBB &initFrom) { obb = initFrom; }

	// Set the OBB
	inline void Set(OBB &initFrom)
		{ obb = initFrom; }

	// Get the OBB
	inline OBBInstance &GetOBB(void)
		{ return(obb); }

	// Set the transform for the obb
	inline void SetTransform(const DirCos3x3 &o, Vector3CRef p)
		{ obb.SetTransform(o, p); }
	inline void SetTransform(const Cartesian4x4 &m)
		{ obb.SetTransform(m); }
	inline void SetOffsetTransform(const Cartesian4x4 &m, Vector3CRef origin)
		{ obb.SetOffsetTransform(m, origin); }
	inline void SetOffsetTransform(const DirCos3x3 &o, Vector3CRef p, Vector3CRef origin)
		{ obb.SetOffsetTransform(o, p, origin); }

	// Apply a translation
	inline void ApplyTranslation(Vector3CRef t)
		{ obb.ApplyTranslation(t); }

	// Some handy operators
	inline OBBChild &operator=(const OBBInstance &o)
		{ obb = o; return(*this); }

	// Override CCollisionNode
	virtual CCollisionObject* BeginObjectTest(void) {return(&obb);}

private:
	OBBInstance		obb;
};

// This manages a list of Child OBB's
// Not intended to be dynamic in any way
class OBBChildList : public CCollisionNode
{
public:
	OBBChildList(OBB &parentOBB, uint howManyChildren);
	~OBBChildList(void);

	// Get the parent of the list
	inline OBB &Parent(void);

	// How many are there?
	inline uint Children(void);
	inline void SetChildren(uint newChildren);

	// Get a specific child
	inline OBBChild &operator[](uint index);

	// Setup from a list of OBB instances
	inline void Setup(OBBInstance *obb, uint count);

	// Apply a translation to all the children (not the parent)
	void ApplyChildTranslation(Vector3CRef t);

	// Debugging
	void Draw(DrawUtility::COLOR color = DrawUtility::GREEN) const;

	// Collision cull
	bool GetCollisionCull(CCollisionCull &o_cull);

	// Override CCollisionNode
	virtual CCollisionObject* BeginObjectTest(void);
	virtual bool IgnoreIntersection(CCollisionIntersection* i_intersection);
	virtual void EndObjectTest (ETestResult i_result,
			CCollisionPair* i_pair = NULL);
	virtual CCollisionNode* GetChild(void);

	// Intersect a line with the child list. The index of the last child hit
	// is returned. The line's find mode will determine which child is last. If
	// a value less than 0 is returned, no hit occurred. The line to OBB
	// intersection object is filled out for the detected hit.
	int Test(CLineCollisionObject &i_line,
			CLineToObbIntersection &o_intersection) const;

	// Given a pointer to an OBBInstance, find the child, if any.
	int FindChild(OBBInstance* i_obb);

private:
	OBB			&parent;
	OBBChild		*child;
	uint			children;
	uint			maxChildren;
	int			testChild;
};

// Inline implementations
#include "CollideHelper/obbchild.hpp"

#endif
