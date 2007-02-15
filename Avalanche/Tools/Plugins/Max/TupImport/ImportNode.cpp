////////////////////////////////////////////////////////////////////////////
//
// ImportNode
//
// Class to handle the nodes
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ImportNode.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 10/24/02   Time: 4:29p
 * Updated in $/Avalanche/tools/Plugins/Max/TupImport
 * materials & maps
 *
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 10/23/02   Time: 9:54a
 * Created in $/Avalanche/tools/Plugins/Max/TupImport
 */ 

#include "TupImportPch.h"
#include "TupImport.h"

static void FixUserPropertyString(TSTR &string)
{
	int length = string.length();
	int badCharacterCount=0;
	for (int i=0;i<length;i++)
	{
		const char value = string.data()[i];
		if (value=='\r') badCharacterCount++;
	}
	if (badCharacterCount)
	{
		int newSize = (length-badCharacterCount)+1;
		char *newString = new char[newSize];
		int writeIndex = 0;
		for (int i=0;i<length;i++)
		{
			bool badCharacter = false;
			const char value = string.data()[i];
			if (value=='\r') badCharacter=true;
			if (!badCharacter)
			{
				newString[writeIndex] = value;
				writeIndex++;
			}
		}
		newString[writeIndex] = '\0';
		string = newString; // use this one
	}
}


void TupImport::AddNode(int parentNodeReference,int nodeReference,TupperwareAggregate *pNodeAgg,INode *pNode,bool exportSelected)
{
	TupNode tupNode(pNodeAgg);
	TSTR name = pNode->GetName();
	tupNode.AddName(name);
	tupNode.AddParentNodeReference(parentNodeReference);

	DWORD c = pNode->GetWireColor();
	Color wireframeColor;
	wireframeColor.r = GetRValue(c)/255.0f;
	wireframeColor.g = GetGValue(c)/255.0f;
	wireframeColor.b = GetBValue(c)/255.0f;
	tupNode.AddWireframeColor((float *)&wireframeColor);

	// transform
	AddTransform(tupNode,pNode);

	// object
	Object *pObject = pNode->GetObjectRef();
	int objectIndex = AddObject(pObject);
	tupNode.AddObjectReference(objectIndex);

	// material
	Mtl *pMaterial = pNode->GetMtl();
	int materialIndex = AddMaterial(pMaterial);
	tupNode.AddMaterialReference(materialIndex);

	// user properties
	TSTR userPropertyString;
	pNode->GetUserPropBuffer(userPropertyString);
	if (userPropertyString.Length())
	{
		FixUserPropertyString(userPropertyString);
		tupNode.AddUserProperties(userPropertyString);
	}

	// flags
	int flags = 0;
	if (pNode->IsHidden()) flags |= TupNode::FLAGS_HIDDEN;
	if (pNode->IsFrozen()) flags |= TupNode::FLAGS_FROZEN;
	if (pNode->ShowFrozenWithMtl()) flags |= TupNode::FLAGS_FROZEN_WITH_MATERIAL;
	if (pNode->GetBoxMode()) flags |= TupNode::FLAGS_BOX_DISPLAY;
	if (pNode->GetVertTicks()) flags |= TupNode::FLAGS_VERTICAL_TICKS_DISPLAY;
	if (pNode->GetAllEdges()) flags |= TupNode::FLAGS_EDGE_DISPLAY;
	if (pNode->GetBackCull()) flags |= TupNode::FLAGS_BACK_CULLED_DISPLAY;
	if (pNode->GetTrajectoryON()) flags |= TupNode::FLAGS_TRAJECTORY_DISPLAY;
	if (pNode->HasXRayMtl()) flags |= TupNode::FLAGS_XRAY_DISPLAY;
	tupNode.AddFlags(flags);

	// all my children...
	int numChildren = pNode->NumberOfChildren();
	if (numChildren)
	{
		for (int i=0;i<numChildren;i++)
		{
			INode *pChildNode = pNode->GetChildNode(i);
			if (!pChildNode)
				continue;
			// if this node is part of the selection and will be saved later don't do
			if(exportSelected && pChildNode->Selected() == FALSE)
				continue;

			TupperwareAggregate *pChildNodeAgg;
			int childReference = m_pNodePoolPtrs->GetIndex(pChildNode);
			// child should already be in the pools
			assert(childReference!=-1);
			pChildNodeAgg = m_pNodePool->GetNode(childReference);
			tupNode.AddChildNodeReference(childReference);
			AddNode(nodeReference,childReference,pChildNodeAgg,pChildNode,exportSelected);
		}
	}
}

void TupImport::AddChildrenNodesToPools(INode *pNode,bool exportSelected)
{
	// all my children...
	int numChildren = pNode->NumberOfChildren();
	if (numChildren)
	{
		for (int i=0;i<numChildren;i++)
		{
			INode *pChildNode = pNode->GetChildNode(i);
			if (!pChildNode)
				continue;
			// if this node is part of the selection and will be saved later don't do
			if(exportSelected && pChildNode->Selected() == FALSE)
				continue;

			TupperwareAggregate *pChildNodeAgg;
			int childReference = m_pNodePoolPtrs->GetIndex(pChildNode);
			// if this child is new to the pools
			assert(childReference==-1);
			childReference = m_pNodePoolPtrs->Add(pChildNode); // add to both pools
			pChildNodeAgg = m_pNodePool->AddNode(childReference);
			AddChildrenNodesToPools(pChildNode,exportSelected);
		}
	}
}
