////////////////////////////////////////////////////////////////////////////
//
// ExportNode
//
// Class to handle the nodes
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportNode.cpp $
 * 
 * *****************  Version 11  *****************
 * User: Tony Jones   Date: 6/06/02    Time: 1:33p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Added nodes to the pools before filling them in
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 5/21/02    Time: 1:55p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Fixed due to changes in keeper
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 4/19/02    Time: 12:55p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * bones, skin modifier, skin modifier context
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 2/26/02    Time: 11:46a
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Added Node Pool and Node Referencing
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 1/04/02    Time: 1:22p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * User Property String carriage return problem
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 12/28/01   Time: 4:34p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * xrefs exporting
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 12/27/01   Time: 4:30p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * xrefs & derived objects (skeleton)
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 12/21/01   Time: 9:47a
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Made null maps, materials, object, etc use reference #-1
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 12/18/01   Time: 4:08p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Dialog Box working (doesn't do modifiers or xrefs yet)
 * Put transform back in (opps)
 * Export selected working
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/14/01   Time: 4:39p
 * Updated in $/Avalanche/tools/Plugins/Max/TupExport
 * Start of standard material exporting
 */

#include "TupExportPch.h"
#include "TupExport.h"

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

static void GetNodePathName(INode *pNode,TupString &pathName)
{
	if (!pNode->IsRootNode())
	{
		TupString newPathName(pNode->GetName());
		newPathName+="|";
		newPathName+=pathName;
		pathName = newPathName;
		GetNodePathName(pNode->GetParentNode(),pathName);
	}
	else
	{
		TupString newPathName;
		newPathName+="|";
		newPathName+=pathName;
		pathName = newPathName;
	}
}

void TupExport::AddNode(int parentNodeReference,int nodeReference,TupperwareAggregate *pNodeAgg,INode *pNode,bool exportSelected)
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

	TupString nodePathName(pNode->GetName());
	GetNodePathName(pNode->GetParentNode(),nodePathName);

	// object
	Object *pObject = pNode->GetObjectRef();
	int objectIndex = AddObject(pObject,nodePathName);
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

void TupExport::AddChildrenNodesToPools(INode *pNode,bool exportSelected)
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
