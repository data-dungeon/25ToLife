////////////////////////////////////////////////////////////////////////////
//
// SkinManager
//
// Class to handle keeping track of the skin data
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: SkinManager.cpp $
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

#include "TupImpExpPch.h"
#include "SkinManager.h"

void SkinManager::AddSkinToMesh(int objectPtr,const Graphics4x4 &meshMatrix,const SkinManager::BoneConnections &boneConnections)
{
	int keeperIndex = m_uniqueObjects.GetIndex(objectPtr);
	if (keeperIndex!=-1) // if in the keeper
	{
		m_skins[keeperIndex].m_bones.Add(boneConnections);
	}
	else // if not in the keeper
	{
		SkinForMesh newSkinForMesh;
		keeperIndex = m_uniqueObjects.Add(objectPtr); // add new pointer to keeper
		m_skins.Add(newSkinForMesh); // add new entry here (these should be in sync)
		assert(keeperIndex!=-1);
		assert(m_skins.GetSize()==m_uniqueObjects.GetSize());
		m_skins[keeperIndex].m_bones.Add(boneConnections);
		m_skins[keeperIndex].m_meshMatrix = meshMatrix;
	}
}

void SkinManager::AddFreeBoneToSkin(int skinIndex,const BoneConnections &boneConnections)
{
	m_skins[skinIndex].m_bones.Add(boneConnections);
}

int SkinManager::GetSkinIndex(int objectPtr) const
{
	return m_uniqueObjects.GetIndex(objectPtr);
}

int SkinManager::GetNumSkins(void) const
{
	return m_skins.GetSize();
}

int SkinManager::GetNumBones(int skinIndex) const
{
	assert(skinIndex>=0&&skinIndex<m_skins.GetSize());
	const SkinForMesh &skin = m_skins[skinIndex];
	return skin.m_bones.GetSize();
}

const SkinManager::BoneConnections &SkinManager::GetBoneConnections(int skinIndex,int boneIndex) const
{
	assert(skinIndex>=0&&skinIndex<m_skins.GetSize());
	const SkinForMesh &skin = m_skins[skinIndex];
	assert(boneIndex>=0&&boneIndex<skin.m_bones.GetSize());
	return skin.m_bones[boneIndex];
}

const Graphics4x4 &SkinManager::GetMeshMatrix(int skinIndex) const
{
	assert(skinIndex>=0&&skinIndex<m_skins.GetSize());
	const SkinForMesh &skin = m_skins[skinIndex];
	return skin.m_meshMatrix;
}

