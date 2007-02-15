///////////////////////////////////////////////////////////////////////////////
// System to build an array of all the instances in a world file
///////////////////////////////////////////////////////////////////////////////
#ifndef INSTLIST_H
#define INSTLIST_H

#include <Game/Database/Instance.h>
#include <container/darray.h>

class InstanceList
{
public:
	InstanceList(void);
	~InstanceList(void);

	// Build the list
	void Build(ts_Group *group, uint groups);

	// How many are there?
	int Count(void)
		{ return(instanceList.Count()); }

	// Get at the list
	CInstance **List(void)
		{ return(instanceList.GetPtr()); }

	// Get an entry (no range checking)
	CInstance *Get(int index)
		{ return(*instanceList.Get(index)); }

private:
	DynamicArray<CInstance *> instanceList;

	// Find a Instance in the list
	bool FindInstance(CInstance *instance);

	// Add a instance to the list
	void AddInstance(CInstance *instance)
		{ instanceList.Add(instance); }
};

#endif
