///////////////////////////////////////////////////////////////////////////////
// System to build an array of all the soms in a world file
///////////////////////////////////////////////////////////////////////////////
#include "somlist.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SOMList::SOMList(void) :
somList(256, 16, true)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SOMList::~SOMList(void)
{
}

///////////////////////////////////////////////////////////////////////////////
// Build the list
///////////////////////////////////////////////////////////////////////////////
void SOMList::Build(
ts_Group *groupList,
uint groups)
{
	ts_Group *group = groupList;
	for (uint index = 0; index < groups; index++, group++)
		for (uint instanceCount = 0; instanceCount < group->usDispStatInst; instanceCount++)
		{
			// Get an alias to the instance
			CInstance *instance = group->ppDispStatArray[instanceCount];
			ts_CollisionSOM *som = GetSOM(*instance);

			// Add it!
			if (!FindSOM(som))
				AddSOM(som);
		}
}

///////////////////////////////////////////////////////////////////////////////
// Find a Instance in the list
///////////////////////////////////////////////////////////////////////////////
bool SOMList::FindSOM(
ts_CollisionSOM *som)
{
	int count = somList.Count();
	for (int i = 0; i < count; i++)
		if (somList[i] == som)
			return(true);

	return(false);
}

//***************************************************************************
//***************************************************************************
ts_CollisionSOM *SOMList::GetSOM(
CInstance &instance)
{
	// Get the pointer to the model
	ts_CollisionSOM *som = NULL;
	if ((instance.dh.u16Flags & INSTANCE_HAS_LOD) == 0)
	{
		// No LOD here
		ASSERT(instance.pSOM != NULL);
		som = (ts_CollisionSOM *)instance.pSOM;
	}
	else
	{
		// Get the highest LOD
		ASSERT(instance.pLOD != NULL);
		som = (ts_CollisionSOM *)instance.pLOD->pSOM[0];
	}

	return(som);
}


