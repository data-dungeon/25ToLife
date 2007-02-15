////////////////////////////////////////////////////////////////////////////
//
// MakeDummyMesh
//
// Class to build a dummy mesh object
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: MakeDummyMesh.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 10/02/02   Time: 4:09p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * added make dummy mesh class
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERFILTERS_LIB_PLUGINS_MAKE_DUMMY_MESH_H
#define TUPPERFILTERS_LIB_PLUGINS_MAKE_DUMMY_MESH_H

class MakeDummyMesh
{
public:
	static void AddToAgg(TupperwareAggregate *pMeshAgg);
};

#endif