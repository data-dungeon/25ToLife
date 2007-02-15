///////////////////////////////////////////////////////////////////////////////
// System to build an array of all the SOM's in a world file
///////////////////////////////////////////////////////////////////////////////
#ifndef SOMLIST_H
#define SOMLIST_H

#include <Layers/som.h>
#include <Game/Database/Instance.h>
#include <container/darray.h>

class SOMList
{
public:
	SOMList(void);
	~SOMList(void);

	// Build the list
	void Build(ts_Group *group, uint groups);

	// How many are there?
	int Count(void)
		{ return(somList.Count()); }

	// Get at the list
	ts_CollisionSOM **List(void)
		{ return(somList.GetPtr()); }

	// Get an entry (no range checking)
	ts_CollisionSOM *Get(int index)
		{ return(*somList.Get(index)); }

private:
	DynamicArray<ts_CollisionSOM *> somList;

	// Find a SOM in the list
	bool FindSOM(ts_CollisionSOM *som);

	// Add a som to the list
	void AddSOM(ts_CollisionSOM *som)
		{ somList.Add(som); }

	// Get the som from the instance
	ts_CollisionSOM *GetSOM(CInstance &instance);
};

#endif
