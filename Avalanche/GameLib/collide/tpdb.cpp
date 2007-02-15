//***************************************************************************
// The triangle perimeter database
//***************************************************************************

/******************* includes ****************************/

#include "collide/CollidePCH.h"

// My header (always first) //
#include "collide/tpdb.h"

//***************************************************************************
// Load us
//***************************************************************************
TrianglePerimeterDatabase *TrianglePerimeterDatabase::Create(
t_Handle dblHandle)
{
	int dataSize;
	TrianglePerimeterDatabase *tpdb = NULL;

	if (dblHandle)
		tpdb = (TrianglePerimeterDatabase *)dblGetChunkData(DB_PDB,
			 																	DEFAULT_CHUNK_KEY_STRING,
																				dblHandle, &dataSize);

	// Did we get a chunk
	if (!tpdb || dataSize == 0)
		return(NULL);

	// Call the initialze
	tpdb->DiskFixup();

	// It's loaded (we are now fat)
	return(tpdb);
}

//***************************************************************************
//***************************************************************************
void TrianglePerimeterDatabase::Destroy(
TrianglePerimeterDatabase *killMe)
{
	if (killMe)
		memFree(killMe);
}

//***************************************************************************
// fixup everything
//***************************************************************************
void TrianglePerimeterDatabase::DiskFixup(void)
{
	// This would be bad
	ASSERT(version == TRIANGLEPERIMETER_VERSION);

	// Fixup all the pointers
	uint baseAddr = (uint)this;

	// Do the list
	ASSERT(list);
	list = (TrianglePerimeter **)((uint)list + baseAddr);

	// Initialize the elements
	for (uint i = 0; i < count; i++)
		list[i] = (TrianglePerimeter *)((uint)list[i] + baseAddr);
}

#ifdef _DEBUG
//***************************************************************************
//***************************************************************************
#ifdef GCN
bool TrianglePerimeterDatabase::Dump(
char *filename)
{
	return(false);
}
#else
bool TrianglePerimeterDatabase::Dump(
char *filename)
{
	FILE *fh = fopen(filename, "wt");
	if (!fh)
		return(false);

	for (uint i = 0; i < count; i++)
	{
		fprintf(fh, "Item: %d\n", i);
		list[i]->Dump(fh);
	}

	fclose(fh);
	return(true);
}
#endif //GCN
#endif //_DEBUG
