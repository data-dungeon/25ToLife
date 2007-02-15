////////////////////////////////////////////////////////////////////////////
//
// DeleteFaces
//
// Class to handle the deleting of faces from the filter chain data
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef DELETE_FACES_H
#define DELETE_FACES_H

class DeleteFaceManager
{
	TupNodePool &m_tupNodePool;
	TupObjectPool &m_tupObjectPool;
	TupModifierPool &m_tupModifierPool;

public:
	DeleteFaceManager(TupNodePool &tupNodePool,TupObjectPool &tupObjectPool,TupModifierPool &tupModifierPool);

	// deletes faces from modifiers and mesh in chain (from node to mesh only)
	void DeleteNodeFaces(TupArray<int> &faceTagArray,int faceTagValue,int nodeRef);

	// deletes faces from mesh as well as all modifiers linked to this mesh (also can deal with staged faces)
	void DeleteMeshFaces(TupArray<int> &faceTagArray,int faceTagValue,int objectRef,TupTriMesh &tupBaseMesh);

private:
	// object
	void DeleteObjectFaces(TupArray<int> &faceTagArray,int faceTagValue,int objectRef,TupTriMesh &tupBaseMesh);
	// tri mesh
	void DeleteMeshFaces(TupArray<int> &faceTagArray,int faceTagValue,TupTriMesh &tupTriMesh);
	void DeleteMeshMapFaces(TupArray<int> &faceTagArray,int faceTagValue,TupperwareAggregate *pMeshMapAgg);

	void DeleteModifierFaces(TupArray<int> &faceTagArray,int faceTagValue,int modifierRef,TupperwareAggregate *pTupModifierContext,TupTriMesh &tupBaseMesh);
	// individual modifiers
	void DeleteApplyVCModifierFaces(TupArray<int> &faceTagArray,int faceTagValue,TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh);
	void DeleteVertexPaintModifierFaces(TupArray<int> &faceTagArray,int faceTagValue,TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh);
	void DeleteHoldVCModifierFaces(TupArray<int> &faceTagArray,int faceTagValue,TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh);
	void DeleteFaceAttributeModifierFaces(TupArray<int> &faceTagArray,int faceTagValue,TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh);
	void DeleteSkinModifierFaces(TupArray<int> &faceTagArray,int faceTagValue,TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh);
	void DeleteClothModifierFaces(TupArray<int> &faceTagArray,int faceTagValue,TupperwareAggregate *pTupModifierContext,TupModifier &tupModifier,TupTriMesh &tupBaseMesh);
};

#endif
