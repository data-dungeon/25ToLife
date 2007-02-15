////////////////////////////////////////////////////////////////////////////
//
// Node
//
// Class to handle nodes
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: node.cpp $
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 9/05/03    Time: 4:35p
 * Updated in $/Avalanche/tools/Tupper3d
 * exporting layers
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 2/28/02    Time: 11:21a
 * Updated in $/Avalanche/tools/Tupper3d
 * Node Reference Stuff.  Moved code to use private functions (less space)
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 2/26/02    Time: 11:43a
 * Updated in $/Avalanche/tools/Tupper3d
 * Added the Node Pool and Node Referencing
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 12/28/01   Time: 4:35p
 * Updated in $/Avalanche/tools/Tupper3d
 * user properties and flags
 * 
 * *****************  Version 6  *****************
 * User: Shon         Date: 12/18/01   Time: 3:26p
 * Updated in $/Avalanche/tools/Tupper3d
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 12/10/01   Time: 4:20p
 * Updated in $/Avalanche/tools/Tupper3d
 * Materials - Textures Part 1
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 12/07/01   Time: 9:20a
 * Updated in $/Avalanche/tools/Tupper3d
 * GetTransform fix
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 12/06/01   Time: 4:38p
 * Updated in $/Avalanche/tools/Tupper3d
 * Get Methods
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/05/01   Time: 4:26p
 * Updated in $/Avalanche/tools/Tupper3d
 * Level A
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "node.h"

char *TupNode::m_keyNames[TupNode::NUM_CHILDREN_ATOMS] = 
{
	"Name", // NAME
	"ObjectReference", //OBJECT_REFERENCE
	"MaterialReference", // MATERIAL_REFERENCE
	"Transform", // TRANSFORM
	"ParentNodeReference", // PARENT_NODE_REFERENCE
	"Children", // CHILDREN
	"Flags", // FLAGS
	"UserProperties", // USER_PROPERTIES
	"WireframeColor", // WIREFRAME_COLOR
	"LayerReference", // LAYER_REFERENCE
};

TupNode::TupNode(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

void TupNode::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareAggregate *TupNode::GetBaseAggregate( void )
{
  return( m_pObject );
}

TupperwareScalar *TupNode::AddName(const char *name)
{
	TupperwareAtom *pAtom = m_cachedAtoms[NAME];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarString(m_keyNames[NAME],NULL,name);
	m_cachedAtoms[NAME] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupNode::AddObjectReference(int objectReference)
{
	return AddIntScalar(objectReference,OBJECT_REFERENCE);
}

TupperwareScalar *TupNode::AddMaterialReference(int materialReference)
{
	return AddIntScalar(materialReference,MATERIAL_REFERENCE);
}

TupperwareAggregate *TupNode::AddTransform(void)
{
	TupperwareAtom *pAtom = m_cachedAtoms[TRANSFORM];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);

	TupperwareAggregate *pTransformationAgg = m_pObject->AddAggregate("Transform");
	m_cachedAtoms[TRANSFORM] = (TupperwareAtom *)pTransformationAgg;
	return pTransformationAgg;
}

TupperwareScalar *TupNode::AddParentNodeReference(int parentNodeReference)
{
	return AddIntScalar(parentNodeReference,PARENT_NODE_REFERENCE);
}

TupperwareScalar *TupNode::AddChildNodeReference(int childNodeReference)
{
	TupperwareAtom *pAtom = m_cachedAtoms[CHILDREN];
	// delete if wrong type
	if (pAtom && pAtom->GetAtomType() != TupperwareAtom::ATOM_AGGREGATE)
	{
		m_pObject->DeleteAtom(pAtom);
		pAtom = NULL;
	}
	// Add if not there
	if (!pAtom)
	{
		pAtom = m_cachedAtoms[CHILDREN] = (TupperwareAtom *)m_pObject->AddAggregate(m_keyNames[CHILDREN]);
	}
	TupperwareAggregate *pChildrenAgg = (TupperwareAggregate *)pAtom;
	return pChildrenAgg->AddScalarInt("ChildNodeReference",NULL,childNodeReference);
}

TupperwareList *TupNode::AddWireframeColor(const float *color)
{
	return AddFloatList(color,3,WIREFRAME_COLOR);
}

TupperwareScalar *TupNode::AddUserProperties(const char *userPropertyString)
{
	TupperwareAtom *pAtom = m_cachedAtoms[USER_PROPERTIES];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarString(m_keyNames[USER_PROPERTIES],NULL,userPropertyString);
	m_cachedAtoms[USER_PROPERTIES] = (TupperwareAtom *)scalar;
	return scalar;

}

TupperwareScalar *TupNode::AddFlags(int flags)
{
	return AddIntScalar(flags,FLAGS);
}

TupperwareScalar *TupNode::AddLayerReference(int layerReference)
{
	return AddIntScalar(layerReference,LAYER_REFERENCE);
}

TupperwareScalar *TupNode::GetName(char **name)
{
	TupperwareAtom *pAtom = m_cachedAtoms[NAME];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *pScalar = (TupperwareScalar *)pAtom;
		if (pScalar->GetDataType() == TupperwareScalar::DATA_STRING)
		{
			*name = pScalar->GetAsString();
			return pScalar;
		}
	}
	*name = NULL;
	return NULL;
}

TupperwareScalar *TupNode::GetObjectReference(int &objectReference)
{
	TupperwareScalar *pScalar = GetIntScalar(objectReference,OBJECT_REFERENCE);
	if (!pScalar)
		objectReference = -1;
	return pScalar;
}

TupperwareScalar *TupNode::GetMaterialReference(int &materialReference)
{
	TupperwareScalar *pScalar = GetIntScalar(materialReference,MATERIAL_REFERENCE);
	if (!pScalar)
		materialReference = -1;
	return pScalar;
}

TupperwareAggregate *TupNode::GetTransform(void)
{
	TupperwareAtom *pAtom = m_cachedAtoms[TRANSFORM];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_AGGREGATE)
	{
		return (TupperwareAggregate *)pAtom;
	}
	return NULL;
}

TupperwareScalar *TupNode::GetParentNodeReference(int &parentNodeReference)
{
	TupperwareScalar *pScalar = GetIntScalar(parentNodeReference,PARENT_NODE_REFERENCE);
	if (!pScalar)
		parentNodeReference = -1;
	return pScalar;
}

// the name on the aggregate is the node reference
int TupNode::GetNodeReference(void)
{
	const char *name = m_pObject->GetName();
	return atoi(name);
}

int TupNode::GetNumChildNodes(void)
{
	int total = 0;
	TupperwareAtom *pAtom = m_cachedAtoms[CHILDREN];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_AGGREGATE)
	{
		TupperwareAggregate *pChildrenAgg = (TupperwareAggregate *)pAtom;
		if (pChildrenAgg->GetFirstChildAtom())
		{
			for (TupperwareIterator i = pChildrenAgg->FindByKey("ChildNodeReference"); ! i.Finished(); i.Next() ) 
			{
				// make sure the child is a aggregate
				TupperwareAtom *pMapAtom = i.GetAsAtom();
				if (pMapAtom && pMapAtom->GetAtomType() == TupperwareAtom::ATOM_SCALAR)
				{
					total++;
				}
			}

		}
	}
	return total;
}

TupperwareScalar *TupNode::GetChildNodeReference(int index,int &childNodeReference)
{
	int count = 0;
	TupperwareAtom *pAtom = m_cachedAtoms[CHILDREN];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_AGGREGATE)
	{
		TupperwareAggregate *pChildrenAgg = (TupperwareAggregate *)pAtom;
		if (pChildrenAgg->GetFirstChildAtom())
		{
			for (TupperwareIterator i = pChildrenAgg->FindByKey("ChildNodeReference"); ! i.Finished(); i.Next() ) 
			{
				// make sure the child is a aggregate
				TupperwareAtom *pNodeAtom = i.GetAsAtom();
				if (pNodeAtom && pNodeAtom->GetAtomType() == TupperwareAtom::ATOM_SCALAR)
				{
					if (index == count)
					{
						TupperwareScalar *pNodeScalar = (TupperwareScalar *)pNodeAtom;
						childNodeReference = pNodeScalar->GetAsInt();
						return pNodeScalar;
					}
					count++;
				}
			}

		}
	}
	childNodeReference = -1;
	return NULL;
}

TupperwareList *TupNode::GetWireframeColor(float **color)
{
	int numColors;
	return GetFloatList(color,numColors,WIREFRAME_COLOR);
}

TupperwareScalar *TupNode::GetUserProperties(char **userPropertyString)
{
	TupperwareAtom *pAtom = m_cachedAtoms[USER_PROPERTIES];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *pScalar = (TupperwareScalar *)pAtom;
		if (pScalar->GetDataType() == TupperwareScalar::DATA_STRING)
		{
			*userPropertyString = pScalar->GetAsString();
			return pScalar;
		}
	}
	*userPropertyString = NULL;
	return NULL;
}

TupperwareScalar *TupNode::GetFlags(int &flags)
{
	return GetIntScalar(flags,FLAGS);
}

TupperwareScalar *TupNode::GetLayerReference(int &layerReference)
{
	return GetIntScalar(layerReference,LAYER_REFERENCE);
}

bool TupNode::RemoveChildNodeReference(int childNodeRef)
{
	TupperwareAtom *pAtom = m_cachedAtoms[CHILDREN];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_AGGREGATE)
	{
		TupperwareAggregate *pChildrenAgg = (TupperwareAggregate *)pAtom;
		if (pChildrenAgg->GetFirstChildAtom())
		{
			for (TupperwareIterator i = pChildrenAgg->FindByKey("ChildNodeReference"); ! i.Finished(); i.Next() ) 
			{
				// make sure the child is a aggregate
				TupperwareAtom *pNodeAtom = i.GetAsAtom();
				if (pNodeAtom && pNodeAtom->GetAtomType() == TupperwareAtom::ATOM_SCALAR)
				{
					TupperwareScalar *pNodeScalar = (TupperwareScalar *)pNodeAtom;
					if (childNodeRef == pNodeScalar->GetAsInt())
					{
						pChildrenAgg->DeleteAtom(pNodeAtom);
						return true;
					}
				}
			}
		}
	}
	return false;
}


// Private Methods
TupperwareScalar *TupNode::AddIntScalar(int value,TupNode::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareList *TupNode::AddFloatList(const float *pFloats,int numFloats,TupNode::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareList *list = m_pObject->AddListFloat(m_keyNames[index],NULL,pFloats,numFloats);
	m_cachedAtoms[index] = (TupperwareAtom *)list;
	return list;
}

TupperwareList *TupNode::AddIntList(const int *pInts,int numInts,TupNode::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareList *list = m_pObject->AddListInt(m_keyNames[index],NULL,pInts,numInts);
	m_cachedAtoms[index] = list;
	return list;
}

TupperwareScalar *TupNode::GetIntScalar(int &value,TupNode::ChildrenAtoms index)
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

TupperwareList *TupNode::GetFloatList(float **pFloats,int &numFloats,TupNode::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_LIST)
	{
		TupperwareList *pList = (TupperwareList *)pAtom;
		if (pList->GetDataType()==TupperwareList::DATA_FLOAT)
		{
			*pFloats = pList->GetAsFloat();
			numFloats = pList->GetLength();
			return pList;
		}
	}
	*pFloats = NULL;
	numFloats = 0;
	return NULL;
}

TupperwareList *TupNode::GetIntList(int **pInts,int &numInts,TupNode::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_LIST)
	{
		TupperwareList *pList = (TupperwareList *)pAtom;
		if (pList->GetDataType()==TupperwareList::DATA_INT)
		{
			*pInts = pList->GetAsInt();
			numInts = pList->GetLength();
			return pList;
		}
	}
	*pInts = NULL;
	numInts = 0;
	return NULL;
}
