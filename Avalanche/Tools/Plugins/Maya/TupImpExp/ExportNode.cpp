////////////////////////////////////////////////////////////////////////////
//
// ExportNode
//
// Tupperware importer/ exporter for Maya
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportNode.cpp $
 * 
 * *****************  Version 19  *****************
 * User: Adam Clayton Date: 9/11/03    Time: 4:22p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * import and export of layers
 * 
 * *****************  Version 18  *****************
 * User: Adam Clayton Date: 9/05/03    Time: 4:35p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * exporting layers
 * 
 * *****************  Version 17  *****************
 * User: Adam Clayton Date: 8/12/03    Time: 5:28p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * move around files
 * animation curve save out less data on child joints
 * 
 * *****************  Version 16  *****************
 * User: Adam Clayton Date: 5/15/03    Time: 4:19p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * first phase of animation export
 * 
 * *****************  Version 15  *****************
 * User: Adam Clayton Date: 3/06/03    Time: 9:34a
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * don't use the attribute plugs to set the key/values
 * 
 * *****************  Version 14  *****************
 * User: Adam Clayton Date: 1/07/03    Time: 4:17p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * check for intermediate objects
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 12/18/02   Time: 12:13p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * exporting face attribute modifiers
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 12/16/02   Time: 1:41p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * back out
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 12/16/02   Time: 10:50a
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * newline problem with maya 4.0
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 12/10/02   Time: 2:18p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * add bones that are not part of joint clusters
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 12/09/02   Time: 1:57p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * bone matrix output correct
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 12/05/02   Time: 4:17p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * fix bone name
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 12/05/02   Time: 3:48p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * bone pose exporting 
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 12/04/02   Time: 4:26p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * Skin and Bones exporting (matrices are not fixed yet)
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 11/20/02   Time: 3:12p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * object attributes fixes
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 11/14/02   Time: 12:55p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * attribute fixes
 * consolidate based on name (ignore numbers on end)
 * save material from mesh to use on other instances
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 11/13/02   Time: 4:21p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * first pass of materials, maps, images, and attributes
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 11/07/02   Time: 3:58p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * node, mesh output working
 */

#include "TupImpExpPch.h"
#include "TupExport.h"
#include "../../libraries/MTools/MTools.h"
#include "UnknownObject.h"

void TupExport::AddNode(int parentNodeReference,int nodeReference,TupperwareAggregate *pNodeAgg,MDagPath mDagPath)
{
	MStatus status;
	MFnDagNode dagNode(mDagPath.node());

	TupNode tupNode(pNodeAgg);
	TupString nodeName(dagNode.name().asChar());
	if (mDagPath.apiType() == MFn::kJoint)
	{
		FixBoneName(nodeName);
	}
	tupNode.AddName(nodeName);

	tupNode.AddParentNodeReference(parentNodeReference);
	float wireframeColor[3];
	wireframeColor[0] = 1.0f;
	wireframeColor[1] = 1.0f;
	wireframeColor[2] = 1.0f;
	tupNode.AddWireframeColor(wireframeColor);

	// transform
	AddTransform(tupNode,mDagPath);

	int numChildren = mDagPath.childCount();
	int materialReference = -1;

	bool bAddedObject = false;
	int childIndex;
	// find an object for the transform node (child)
	if (mDagPath.apiType() == MFn::kTransform)
	{
		for (childIndex = 0; childIndex < numChildren; childIndex++)
		{
			MObject childObject = mDagPath.child(childIndex);
			MDagPath childPath = mDagPath;
			childPath.push(childObject);

			if (ValidObject(childPath))
			{
				int objectIndex = AddObject(childPath,materialReference);
				tupNode.AddObjectReference(objectIndex);
				bAddedObject = true;
				break;
			}
		}
	}
	// if a lookat pass myself as an object
	else if (mDagPath.apiType() == MFn::kLookAt)
	{
		int objectIndex = AddObject(mDagPath,materialReference);
		tupNode.AddObjectReference(objectIndex);
		bAddedObject = true;
	}
	// if a joint pass myself as an object
	else if (mDagPath.apiType() == MFn::kJoint)
	{
		int objectIndex = AddObject(mDagPath,materialReference);
		tupNode.AddObjectReference(objectIndex);
		bAddedObject = true;
	}

	if (!bAddedObject)
	{
		int objectIndex = -1;
		tupNode.AddObjectReference(objectIndex);
	}

	// material
	tupNode.AddMaterialReference(materialReference);

	// user properties
	TupString noteString;
	MPlug notesPlug = dagNode.findPlug("notes",&status);
	if (status)
	{
		MString tempString;
		notesPlug.getValue(tempString);	
		noteString = tempString.asChar();
		noteString.Replace("\r\n","\n");
		noteString.Replace("\r","\n");
	}
	if (noteString.GetLength())
	{
		tupNode.AddUserProperties(noteString);
	}

	int layerRef = m_pLayerNames->GetIndex(TupString("defaultLayer"));
	MPlug layerPlug = dagNode.findPlug("drawOverride",&status);
	if (status)
	{
		MPlugArray conns;
		if (layerPlug.connectedTo(conns,true,false)) 
		{
			if (conns.length()!=0)
			{
				MObject layerObject = conns[0].node();
				if (layerObject.apiType() == MFn::kDisplayLayer)
				{
					MFnDependencyNode layerNode(layerObject);
					layerRef = m_pLayerNames->GetIndex(TupString(layerNode.name().asChar()));
				}
			}
		}
	}
	if (layerRef!=-1)
	{
		layerRef = (*m_pLayers)[layerRef].m_reference; // get remapped value (usually due to selection)
	}
	tupNode.AddLayerReference(layerRef);


	// flags
	int flags = 0;

	if (!MayaHelper::GetBoolAttrib(dagNode,"visibility"))
	{
		flags |= TupNode::FLAGS_HIDDEN;
	}

	tupNode.AddFlags(flags);


	// all my children...
	for (childIndex = 0; childIndex < numChildren; childIndex++)
	{
		MObject childObject = mDagPath.child(childIndex);
		MDagPath childPath = mDagPath;
		childPath.push(childObject);
		if (ValidNode(childPath))
		{
			int childReference = m_pNodePoolDagPaths->GetIndex(childPath);
			// child should already be in the pools
			assert(childReference!=-1);
			tupNode.AddChildNodeReference(childReference);
			TupperwareAggregate *pChildNodeAgg = m_pNodePool->GetNode(childReference);
			AddNode(nodeReference,childReference,pChildNodeAgg,childPath);
		}
	}
}

void TupExport::AddChildrenNodesToPools(MDagPath mDagPath)
{
	int numChildren = mDagPath.childCount();
	// all my childern...
	int childIndex;
	for (childIndex = 0; childIndex < numChildren; childIndex++)
	{
		MObject childObject = mDagPath.child(childIndex);
		MDagPath childPath = mDagPath;
		childPath.push(childObject);
		if (ValidNode(childPath))
		{
			TupperwareAggregate *pChildNodeAgg;
			int childReference = m_pNodePoolDagPaths->GetIndex(childPath);
			// child should not be in the pools
			assert(childReference==-1);
			childReference = m_pNodePoolDagPaths->Add(childPath);
			pChildNodeAgg = m_pNodePool->AddNode(childReference);
			AddChildrenNodesToPools(childPath);
		}
	}
}

bool TupExport::ValidNode(MDagPath &mDagPath)
{
	// make sure it is not an intermediate object
	MFnDagNode mDagNode(mDagPath);
	if (mDagNode.isIntermediateObject())
		return false;

	if (m_bExportSelected)
	{
		// if not in the list of nodes to output
		if (m_pExportDagPaths->GetIndex(mDagPath)==-1)
			return false;
	}

	MFn::Type type = mDagPath.apiType();
	if (type == MFn::kTransform)
	{
		return true;
	}
	else if (type == MFn::kLookAt)
	{
		return true;
	}
	else if (type == MFn::kJoint)
	{
		int numChildren = mDagPath.childCount();
		// find an child joint (if there is one we are a valid joint)
		for (int childIndex = 0; childIndex < numChildren; childIndex++)
		{
			MObject childObject = mDagPath.child(childIndex);
			MDagPath childPath = mDagPath;
			childPath.push(childObject);
			// found a child joint
			if (childPath.apiType()==MFn::kJoint)
			{
				return true;
			}
		}
	}
	return false;
}

bool TupExport::ValidObject(MDagPath &mDagPath)
{
	// make sure it is not an intermediate object
	MFnDagNode mDagNode(mDagPath);
	if (mDagNode.isIntermediateObject())
		return false;
	MFn::Type type = mDagPath.apiType();
	switch (type)
	{
		case MFn::kMesh:
		case MFn::kNurbsCurve:
		case MFn::kPointLight:
		case MFn::kDirectionalLight:
		case MFn::kSpotLight:
		case MFn::kAreaLight:
		case MFn::kAmbientLight:
		case MFn::kCamera:
		case MFn::kLocator:
			return true;
      default:
			return false;
	}
}

void TupExport::FixBoneName(TupString &boneName)
{
	// replace _____ with $ later with _
	boneName.Replace("_____","$");
	// replace ____ with #
	boneName.Replace("____","#");
	// replace ___ with !
	boneName.Replace("___","!");
	// replace __ with -
	boneName.Replace("__","-");
	// replace _ with space
	boneName.Replace("_"," ");
	// replace $ with $ later with _
	boneName.Replace("$","_");
}

// get the selected objects and add them to the export dag paths
void TupExport::AddSelectedDagPathsForExport(void)
{
	MStatus status;
	MSelectionList slist;
	MGlobal::getActiveSelectionList( slist );
	MItSelectionList iter( slist );
	
	// Selection list loop
	for ( ; !iter.isDone(); iter.next() )
	{	 
		MDagPath mDagPath;
		// get the selected node
		status = iter.getDagPath(mDagPath);

		AddSelectedDagPathForExport(mDagPath,true,true);
	}
	AddConnectedNodes();
}

void TupExport::AddSelectedDagPathForExport(const MDagPath &mDagPath,bool bAddParents,bool bAddChildren)
{
	m_pExportDagPaths->Add(mDagPath);
	AddSelectedNodeToLayer(mDagPath);

	if (bAddParents)
	{
		// add all my parents
		MDagPath parentPath(mDagPath);
		while (parentPath.pop()==MStatus::kSuccess)
		{
			m_pExportDagPaths->Add(parentPath);
			AddSelectedNodeToLayer(parentPath);
		}
	}

	if (bAddChildren)
	{
		int numChildren = mDagPath.childCount();
		int childIndex;
		for (childIndex = 0; childIndex < numChildren; childIndex++)
		{
			MObject childObject = mDagPath.child(childIndex);
			MDagPath childPath = mDagPath;
			childPath.push(childObject);
			
			AddSelectedDagPathForExport(childPath,false,true);
		}
	}
}

void TupExport::AddConnectedNodes(void)
{
	MStatus stat;
	
	// count the processed jointClusters
	//
	unsigned int jcCount = 0;
	
	// Iterate through graph and search for jointCluster nodes
	//
	MItDependencyNodes iter( MFn::kJointCluster);
	for ( ; !iter.isDone(); iter.next() ) 
	{
		MObject object = iter.item();
		MFnWeightGeometryFilter jointCluster(object);
		
		// get the joint driving this cluster
		//
		MObject joint = GetJointForCluster(object);
		if (joint.isNull()) 
		{
			continue;
		}
		
		MObject deformSet = jointCluster.deformerSet(&stat);
		if (MS::kSuccess != stat)
		{
			continue;
		}
		
		MFnSet setFn(deformSet, &stat);	//need the fn to get the members
		if (MS::kSuccess != stat)
		{
			continue;
		}
		
		MSelectionList clusterSetList;
		
		//get all the members
		//
		stat = setFn.getMembers(clusterSetList, true);
		if (MS::kSuccess != stat)
		{
			continue;
		}
		
		MFnDependencyNode fnJoint(joint);
		MDagPath jointNodePath;
		MFnDagNode fnJointNode(joint);
		fnJointNode.getPath(jointNodePath);
		
		for (size_t kk = 0; kk < clusterSetList.length(); ++kk) 
		{
			MObject joint;
			MDagPath skinPath;
			MObject components;
			MFloatArray weights;
			
			clusterSetList.getDagPath(kk,skinPath,components);

			// is the mesh going to be exported?
			if (m_pExportDagPaths->GetIndex(skinPath)!=-1)
			{
				AddSelectedDagPathForExport(jointNodePath,true,true);
			}
		}
	}
}
