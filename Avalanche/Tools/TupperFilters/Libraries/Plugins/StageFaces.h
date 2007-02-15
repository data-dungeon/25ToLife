////////////////////////////////////////////////////////////////////////////
//
// StageFaces
//
// Class to handle access to stage faces
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: StageFaces.h $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 11:03a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * moved tupmesh to tuptrimesh
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 1/24/03    Time: 4:59p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 1/21/03    Time: 3:59p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * added stage faces helper class
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERFILTERS_LIB_PLUGINS_STAGE_FACES_H
#define TUPPERFILTERS_LIB_PLUGINS_STAGE_FACES_H

class StageFaceManager
{
	int *m_pNextStageFaceIndices;
	int m_numFaces;
	StageFaceManager(void) {} // private
public:
	StageFaceManager(TupTriMesh &tupTriMesh);

	int GetNumStages(int faceIndex) const;
	int GetStageFace(int faceIndex,int stageIndex) const;
};

#endif
