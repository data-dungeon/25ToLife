/////////////////////////////////////////////////////////////////////////////
// a physics based leaf!!
/////////////////////////////////////////////////////////////////////////////
#ifndef LEAF_H
#define LEAF_H

#include "Effects/simplerb.h"
#include "GameHelper/loadsom.h"

class LeafModel
{
public:
	// The geometry pointer for this leaf
	ts_Geometry			geometry;

	// How much do we tweak this bad boy up?
	float					heightTweak;

	// Arbitrary spin impulse
	float					spinImpulse;

	// Our properties for this type of leaf
	RigidBodyProps		props;

	// For the doubly linked list
	LeafModel			*prev, *next;
};

class Leaf : public RigidBody
{
public:
	// We don't do much here
	Leaf(void) {}

	// Setup everything
	void Setup(LeafModel *leafModel, Graphics4x4 *matrix);

	// What is our parent model?	
	inline LeafModel *Model(void)
		{ return(model); }

	// we have an application d 
	inline float GetD(void)
		{ return(d); }
	inline void SetD(float newD)
		{ d = newD; }

private:
	// Our parent
	LeafModel			*model;
	float					d;
};

#endif