////////////////////////////////////////////////////////////////////////////
//
// DuplicateFaces
//
// Class to handle the duplication of faces
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: DuplicateFaces.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 11:40a
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * moved tupmesh to tuptrimesh
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/20/02    Time: 3:11p
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * Duplicate Faces & Multitex Start
 */

////////////////////////////////////////////////////////////////////////////

#ifndef DUPLICATE_FACES_H
#define DUPLICATE_FACES_H

class DuplicateFaceManager
{
	TupObjectPool &m_tupObjectPool;
	TupModifierPool &m_tupModifierPool;

public:
	DuplicateFaceManager(TupObjectPool &tupObjectPool,TupModifierPool &tupModifierPool);

	void DuplicateFaces(int objectRef,const TupArray<int> &faceNumCopiesArray,TupArray< TupArray<int> > &newFacesArray);
private:
	// tri mesh
	void DuplicateMeshFaces(const TupArray<int> &faceNumCopiesArray,TupArray< TupArray<int> > &newFacesArray,TupTriMesh &tupTriMesh);

	void DuplicateModifierFaces(const TupArray<int> &faceNumCopiesArray,int modifierRef,TupperwareAggregate *pTupModifierContext,TupTriMesh &tupBaseMesh);
	// individual modifiers
	void DuplicateApplyVCModifierFaces(const TupArray<int> &faceNumCopiesArray,TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh);
	void DuplicateVertexPaintModifierFaces(const TupArray<int> &faceNumCopiesArray,TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh);
	void DuplicateHoldVCModifierFaces(const TupArray<int> &faceNumCopiesArray,TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh);
	void DuplicateFaceAttributeModifierFaces(const TupArray<int> &faceNumCopiesArray,TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh);
	void DuplicateSkinModifierFaces(const TupArray<int> &faceNumCopiesArray,TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh);
	void DuplicateClothModifierFaces(const TupArray<int> &faceNumCopiesArray,TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh);
};

#endif
