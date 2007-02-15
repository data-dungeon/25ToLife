////////////////////////////////////////////////////////////////////////////
//
// Scene
//
// Class to handle the scene
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: scene.cpp $
 * 
 * *****************  Version 21  *****************
 * User: Adam Clayton Date: 9/05/03    Time: 4:35p
 * Updated in $/Avalanche/tools/Tupper3d
 * exporting layers
 * 
 * *****************  Version 20  *****************
 * User: Adam Clayton Date: 7/14/03    Time: 1:12p
 * Updated in $/Avalanche/tools/Tupper3d
 * add source file name
 * 
 * *****************  Version 19  *****************
 * User: Adam Clayton Date: 7/03/03    Time: 10:08a
 * Updated in $/Avalanche/tools/Tupper3d
 * added map attributes
 * 
 * *****************  Version 18  *****************
 * User: Adam Clayton Date: 6/27/03    Time: 2:02p
 * Updated in $/Avalanche/tools/Tupper3d
 * map attributes export
 * 
 * *****************  Version 17  *****************
 * User: Adam Clayton Date: 6/17/03    Time: 2:57p
 * Updated in $/Avalanche/tools/Tupper3d
 * add audio clips
 * 
 * *****************  Version 16  *****************
 * User: Adam Clayton Date: 6/05/03    Time: 12:02p
 * Updated in $/Avalanche/tools/Tupper3d
 * motion path exporting
 * 
 * *****************  Version 15  *****************
 * User: Adam Clayton Date: 6/02/03    Time: 10:34a
 * Updated in $/Avalanche/tools/Tupper3d
 * animation globals
 * 
 * *****************  Version 14  *****************
 * User: Adam Clayton Date: 5/30/03    Time: 10:23a
 * Updated in $/Avalanche/tools/Tupper3d
 * add units
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 5/15/03    Time: 4:19p
 * Updated in $/Avalanche/tools/Tupper3d
 * first phase of animation export
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 5/15/03    Time: 2:22p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added Animation Wrapper Classes
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 7/03/02    Time: 9:52a
 * Updated in $/Avalanche/tools/Tupper3d
 * added exporter date and time
 * added version number
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 4/08/02    Time: 4:26p
 * Updated in $/Avalanche/tools/Tupper3d
 * attribute tree changes
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 4/04/02    Time: 3:27p
 * Updated in $/Avalanche/tools/Tupper3d
 * Attribute Tree Additions
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 2/26/02    Time: 11:43a
 * Updated in $/Avalanche/tools/Tupper3d
 * Added the Node Pool and Node Referencing
 * 
 * *****************  Version 6  *****************
 * User: Shon         Date: 12/18/01   Time: 3:26p
 * Updated in $/Avalanche/tools/Tupper3d
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 12/12/01   Time: 4:34p
 * Updated in $/Avalanche/tools/Tupper3d
 * Material - Map stuff
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 12/10/01   Time: 4:25p
 * Updated in $/Avalanche/tools/Tupper3d
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 12/06/01   Time: 1:51p
 * Updated in $/Avalanche/tools/Tupper3d
 * Add and Get functions complete
 * Added new files for handling various parts of the graphics format
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/05/01   Time: 4:26p
 * Updated in $/Avalanche/tools/Tupper3d
 * Level A
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "scene.h"

char *TupScene::m_keyNames[TupScene::NUM_CHILDREN_ATOMS] = 
{
	"RootNodeReference", // ROOT_NODE_REFERENCE
	"NodePool", // NODE_POOL
	"ObjectPool", // OBJECT_POOL
	"ModifierPool", // MODIFIER_POOL
	"MaterialPool", // MATERIAL_POOL
	"MapPool", // MAP_POOL
	"ImagePool", // IMAGE_POOL
	"AnimSetPool", // ANIM_SET_POOL
	"ConnectionPool", // CONNECTION_POOL
	"AudioPool", // AUDIO_POOL
	"LayerPool", // LAYER_POOL
	"AnimGlobals", // ANIIM_GLOBALS
	"NodeAttributeTree", // NODE_ATTRIBUTE_TREE
	"FaceAttributeTree", // FACE_ATTRIBUTE_TREE
	"MapAttributeTree", // MAP_ATTRIBUTE_TREE
	"PathPointAttributeTree", // PATH_POINT_ATTRIBUTE_TREE
	"ExporterDate", // EXPORTER_DATE
	"ExporterTime", // EXPORTER_TIME
	"Version", // VERSION
	"UnitFlags", // UNIT_FLAGS
	"SourceFileName", // SOURCE_FILE_NAME
	"DefaultLayerReference", // DEFAULT_LAYER_REFERENCE
};

TupScene::TupScene(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

void TupScene::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareAggregate *TupScene::GetBaseAggregate( void )
{
  return( m_pObject );
}

const char *TupScene::GetSceneVersion(void)
{
	// this is the current database version
	// change this value to enforce a re-export of the tup files
	// this only needs to be done when something has changed that will not work with an older version of the database
	return "1.0.0.0"; 
}

TupperwareScalar *TupScene::AddRootNodeReference(int nodeReferene)
{
	TupperwareAtom *pAtom = m_cachedAtoms[ROOT_NODE_REFERENCE];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[ROOT_NODE_REFERENCE],NULL,nodeReferene);
	m_cachedAtoms[ROOT_NODE_REFERENCE] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareAggregate *TupScene::AddNodePool(void)
{
	return AddAggregate(TupScene::NODE_POOL);
}

TupperwareAggregate *TupScene::AddObjectPool(void)
{
	return AddAggregate(TupScene::OBJECT_POOL);
}

TupperwareAggregate *TupScene::AddModifierPool(void)
{
	return AddAggregate(TupScene::MODIFIER_POOL);
}

TupperwareAggregate *TupScene::AddMaterialPool(void)
{
	return AddAggregate(TupScene::MATERIAL_POOL);
}

TupperwareAggregate *TupScene::AddMapPool(void)
{
	return AddAggregate(TupScene::MAP_POOL);
}

TupperwareAggregate *TupScene::AddImagePool(void)
{
	return AddAggregate(TupScene::IMAGE_POOL);
}

TupperwareAggregate *TupScene::AddAnimSetPool(void)
{
	return AddAggregate(TupScene::ANIM_SET_POOL);
}

TupperwareAggregate *TupScene::AddConnectionPool(void)
{
	return AddAggregate(TupScene::CONNECTION_POOL);
}

TupperwareAggregate *TupScene::AddAudioPool(void)
{
	return AddAggregate(TupScene::AUDIO_POOL);
}

TupperwareAggregate *TupScene::AddLayerPool(void)
{
	return AddAggregate(TupScene::LAYER_POOL);
}

TupperwareAggregate *TupScene::AddAnimGlobals(void)
{
	return AddAggregate(TupScene::ANIM_GLOBALS);
}

TupperwareAggregate *TupScene::AddNodeAttributeTree(TupperwareAggregate *pTreeAgg)
{
	TupperwareAggregate *pNodeAttributeTreeAgg = AddAggregate(TupScene::NODE_ATTRIBUTE_TREE);
	for (TupperwareIterator i = pTreeAgg->FindAll(); !i.Finished(); i.Next()) 
	{
		pNodeAttributeTreeAgg->Copy(i.GetAsAtom());
	}
	return pNodeAttributeTreeAgg;
}

TupperwareAggregate *TupScene::AddFaceAttributeTree(TupperwareAggregate *pTreeAgg)
{
	TupperwareAggregate *pFaceAttributeTreeAgg = AddAggregate(TupScene::FACE_ATTRIBUTE_TREE);
	for (TupperwareIterator i = pTreeAgg->FindAll(); !i.Finished(); i.Next()) 
	{
		pFaceAttributeTreeAgg->Copy(i.GetAsAtom());
	}
	return pFaceAttributeTreeAgg;
}

TupperwareAggregate *TupScene::AddMapAttributeTree(TupperwareAggregate *pTreeAgg)
{
	TupperwareAggregate *pMapAttributeTreeAgg = AddAggregate(TupScene::MAP_ATTRIBUTE_TREE);
	for (TupperwareIterator i = pTreeAgg->FindAll(); !i.Finished(); i.Next()) 
	{
		pMapAttributeTreeAgg->Copy(i.GetAsAtom());
	}
	return pMapAttributeTreeAgg;
}

TupperwareAggregate *TupScene::AddPathPointAttributeTree(TupperwareAggregate *pTreeAgg)
{
	TupperwareAggregate *pPathAttributeTreeAgg = AddAggregate(TupScene::PATH_POINT_ATTRIBUTE_TREE);
	for (TupperwareIterator i = pTreeAgg->FindAll(); !i.Finished(); i.Next()) 
	{
		pPathAttributeTreeAgg->Copy(i.GetAsAtom());
	}
	return pPathAttributeTreeAgg;
}

TupperwareScalar *TupScene::AddExporterDate(const char *pDateString)
{
	return AddStringScalar(pDateString,TupScene::EXPORTER_DATE);
}

TupperwareScalar *TupScene::AddExporterTime(const char *pTimeString)
{
	return AddStringScalar(pTimeString,TupScene::EXPORTER_TIME);
}

TupperwareScalar *TupScene::AddVersion(const char *pVersionString)
{
	return AddStringScalar(pVersionString,TupScene::VERSION);
}

TupperwareScalar *TupScene::AddUnitFlags(int flags)
{
	return AddIntScalar(flags,TupScene::UNIT_FLAGS);
}

TupperwareScalar *TupScene::AddSourceFileName(const char *pSourceFileName)
{
	return AddStringScalar(pSourceFileName,TupScene::SOURCE_FILE_NAME);
}

TupperwareScalar *TupScene::GetRootNodeReference(int &nodeReference)
{
	TupperwareAtom *pAtom = m_cachedAtoms[ROOT_NODE_REFERENCE];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *pScalar = (TupperwareScalar *)pAtom;
		if (pScalar->GetDataType() == TupperwareScalar::DATA_INT)
		{
			nodeReference = pScalar->GetAsInt();
			return pScalar;
		}
	}
	nodeReference = -1;
	return NULL;
}

TupperwareScalar *TupScene::AddDefaultLayerReference(int layerRef)
{
	return AddIntScalar(layerRef,TupScene::DEFAULT_LAYER_REFERENCE);
}

TupperwareAggregate *TupScene::GetNodePool(void)
{
	return GetAggregate(TupScene::NODE_POOL);
}

TupperwareAggregate *TupScene::GetObjectPool(void)
{
	return GetAggregate(TupScene::OBJECT_POOL);
}

TupperwareAggregate *TupScene::GetModifierPool(void)
{
	return GetAggregate(TupScene::MODIFIER_POOL);
}

TupperwareAggregate *TupScene::GetMaterialPool(void)
{
	return GetAggregate(TupScene::MATERIAL_POOL);
}

TupperwareAggregate *TupScene::GetMapPool(void)
{
	return GetAggregate(TupScene::MAP_POOL);
}

TupperwareAggregate *TupScene::GetImagePool(void)
{
	return GetAggregate(TupScene::IMAGE_POOL);
}

TupperwareAggregate *TupScene::GetAnimSetPool(void)
{
	return GetAggregate(TupScene::ANIM_SET_POOL);
}

TupperwareAggregate *TupScene::GetConnectionPool(void)
{
	return GetAggregate(TupScene::CONNECTION_POOL);
}

TupperwareAggregate *TupScene::GetAudioPool(void)
{
	return GetAggregate(TupScene::AUDIO_POOL);
}

TupperwareAggregate *TupScene::GetLayerPool(void)
{
	return GetAggregate(TupScene::LAYER_POOL);
}

TupperwareAggregate *TupScene::GetAnimGlobals(void)
{
	return GetAggregate(TupScene::ANIM_GLOBALS);
}

TupperwareAggregate *TupScene::GetNodeAttributeTree(void)
{
	return GetAggregate(TupScene::NODE_ATTRIBUTE_TREE);
}

TupperwareAggregate *TupScene::GetFaceAttributeTree(void)
{
	return GetAggregate(TupScene::FACE_ATTRIBUTE_TREE);
}

TupperwareAggregate *TupScene::GetMapAttributeTree(void)
{
	return GetAggregate(TupScene::MAP_ATTRIBUTE_TREE);
}

TupperwareAggregate *TupScene::GetPathPointAttributeTree(void)
{
	return GetAggregate(TupScene::PATH_POINT_ATTRIBUTE_TREE);
}

TupperwareScalar *TupScene::GetExporterDate(char **pDateString)
{
	return GetStringScalar(pDateString,TupScene::EXPORTER_DATE);
}

TupperwareScalar *TupScene::GetExporterTime(char **pTimeString)
{
	return GetStringScalar(pTimeString,TupScene::EXPORTER_TIME);
}

TupperwareScalar *TupScene::GetVersion(char **pVersionString)
{
	return GetStringScalar(pVersionString,TupScene::VERSION);
}

TupperwareScalar *TupScene::GetUnitFlags(int &flags)
{
	return GetIntScalar(flags,TupScene::UNIT_FLAGS);
}

TupperwareScalar *TupScene::GetSourceFileName(char **pSourceFileName)
{
	return GetStringScalar(pSourceFileName,TupScene::SOURCE_FILE_NAME);
}

TupperwareScalar *TupScene::GetDefaultLayerReference(int &layerRef)
{
	return GetIntScalar(layerRef,TupScene::DEFAULT_LAYER_REFERENCE);
}

// private methods

TupperwareAggregate *TupScene::AddAggregate(TupScene::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareAggregate *pAgg = m_pObject->AddAggregate(m_keyNames[index],NULL);
	m_cachedAtoms[index] = (TupperwareAtom *)pAgg;
	return pAgg;
}

TupperwareScalar *TupScene::AddStringScalar(const char *pString,TupScene::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *pScalar = m_pObject->AddScalarString(m_keyNames[index],NULL,pString);
	m_cachedAtoms[index] = (TupperwareAtom *)pScalar;
	return pScalar;
}

TupperwareAggregate *TupScene::GetAggregate(TupScene::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_AGGREGATE)
	{
		return (TupperwareAggregate *)pAtom;
	}
	return NULL;
}

TupperwareScalar *TupScene::GetStringScalar(char **pString,TupScene::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *pScalar = (TupperwareScalar *) pAtom;
		if (pScalar->GetDataType() == TupperwareScalar::DATA_STRING)
		{
			*pString = pScalar->GetAsString();
			return pScalar;
		}
	}
	*pString = NULL;
	return NULL;
}

TupperwareScalar *TupScene::AddIntScalar(int value,TupScene::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupScene::GetIntScalar(int &value,TupScene::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *pScalar = (TupperwareScalar *)pAtom;
		if (pScalar->GetDataType()==TupperwareScalar::DATA_INT)
		{
			value = pScalar->GetAsInt();
			return pScalar;
		}
	}
	value = 0;
	return NULL;
}

