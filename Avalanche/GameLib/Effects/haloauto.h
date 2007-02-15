/////////////////////////////////////////////////////////////////////////////
// A simple, subclassable haloautomaton
/////////////////////////////////////////////////////////////////////////////
#ifndef HALOAUTO_H
#define HALOAUTO_H

#include "Effects/halo.h"

class HaloAutomaton
{
public:
	// The automaton
	HaloAutomaton(void) { handle = NULL; }
	virtual ~HaloAutomaton(void) {}

	// You must implement this update
	virtual void Update(void) = 0;

	// This is called once we get added to the halo system by the halo system
	void Bind(Halo *parentHalo, Halo::Handle myHandle)
		{ parent = parentHalo; handle = myHandle; }
	Vector3 *Position(void)
		{ return(&pos); }
	DirCos3x3 *Orientation(void)
		{ return(&o); }

protected:
	// The handle of us (NULL if not active)
	Halo				*parent;
	Halo::Handle	handle;

	// Our position & orientation
	Vector3			pos;
	DirCos3x3		o;
};

#endif
