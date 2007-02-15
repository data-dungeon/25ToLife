////////////////////////////////////////////////////////////////////////////
//
// SkinManager
//
// Class to handle keeping track of the skin data
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: SkinManager.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 12/10/02   Time: 2:18p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * add bones that are not part of joint clusters
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/04/02   Time: 4:26p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * Skin and Bones exporting (matrices are not fixed yet)
 */

#ifndef SKIN_MANAGER
#define SKIN_MANAGER

class SkinManager
{
public:
	class BoneConnections
	{
	public:
		TupString m_boneName;
		int m_boneNodeRef; // which bone are we connected to
		TupArray<int> m_vertexIndices; // what vertices within the mesh we are using
		TupArray<float> m_weights; // the weights for each vertex
		Graphics4x4 m_boneMatrix; // the bone matrix
	};
	class SkinForMesh
	{
	public:
		TupArray<BoneConnections> m_bones; // keep track of multiple bones for this mesh
		Graphics4x4 m_meshMatrix; // the bone matrix
	};
	// the keeper keeps track of the unique objects that are in the m_skins array
	Keeper<int> m_uniqueObjects;
	TupArray<SkinForMesh> m_skins;

	void AddSkinToMesh(int objectPtr,const Graphics4x4 &meshMatrix,const BoneConnections &boneConnections);
	void AddFreeBoneToSkin(int skinIndex,const BoneConnections &boneConnections);
	int GetSkinIndex(int objectPtr) const; // return index in m_skins or -1 if not in keeper
	int GetNumSkins(void) const; // return the size of the m_skins array
	int GetNumBones(int skinIndex) const;
	const BoneConnections &GetBoneConnections(int skinIndex,int boneIndex) const;
	const Graphics4x4 &GetMeshMatrix(int skinIndex) const;
};

#endif
