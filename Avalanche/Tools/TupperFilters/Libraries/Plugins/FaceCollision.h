////////////////////////////////////////////////////////////////////////////
//
// FaceCollision
//
// Class to handle face collision data
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: FaceCollision.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 9/25/02    Time: 11:33a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * new collision attribute system (using image names)
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERFILTERS_LIB_PLUGINS_FACE_COLLISION_H
#define TUPPERFILTERS_LIB_PLUGINS_FACE_COLLISION_H

class TupNode;
class TupObjectPool;
class TupModifierPool;
class TupMaterialPool;
class TupMapPool;
class TupImagePool;
class TupConfigRoot;

class FaceCollisionManager
{
public:
	struct Attributes
	{
		bool m_bCollision;
		int m_surfaceType;
		int m_obbValue;
		bool operator==(const Attributes& s) const;
	};

	struct AttributeStage
	{
		bool m_bDefaultDisabled;
		bool m_bDefaultCollision;
		bool m_bDisabled;
		bool m_bUseMesh;
		int m_surfaceType;
		int m_obbValue;
	};

private:
	TupObjectPool &m_tupObjectPool;
	TupModifierPool &m_tupModifierPool;
	TupMaterialPool &m_tupMaterialPool;
	TupMapPool &m_tupMapPool;
	TupImagePool &m_tupImagePool;

	TupConfigRoot &m_tupNodeTreeRoot;
	TupConfigRoot &m_tupFaceTreeRoot;

	TupArray<AttributeStage> m_materialStages;

	bool m_bTerrainMode;

public:
	FaceCollisionManager(TupConfigRoot &tupNodeTreeRoot,TupConfigRoot &tupFaceTreeRoot,
						 TupObjectPool &tupObjectPool,TupModifierPool &tupModifierPool,TupMaterialPool &tupMaterialPool,TupMapPool &tupMapPool,TupImagePool &tupImagePool,bool bTerrainMode);

	void BuildFaceCollisionAttributes(TupNode &tupNode,TupArray<Attributes> &faceAttributeArray,TupArray<int> &faceAttributeIndices);
private:
	void BuildFaceAttributeStageData(TupperwareAggregate *pFaceAttributesAgg,TupArray<AttributeStage> &faceStageArray,char ***pAttributeVerts,int &numAttributeVerts,int **pAttributeIndices);
	void BuildStageFromAttributes(AttributeStage &stage,TupConfigRoot &tupConfigRoot,TupKeyValue &tupAttributes);
	void BuildNodeMaterialStageData(int materialRef,TupArray<AttributeStage> &materialStageArray);
	void BuildMaterialStageData(void);
	void BuildImageAttributes(TupArray<AttributeStage> &imageStages);
};

#endif
