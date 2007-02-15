////////////////////////////////////////////////////////////////////////////
//
// ExportScene
//
// Tupperware importer/ exporter for Maya
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportScene.cpp $
 * 
 * *****************  Version 25  *****************
 * User: Adam Clayton Date: 10/08/03   Time: 12:48p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * smooth binding exporting
 * 
 * *****************  Version 24  *****************
 * User: Adam Clayton Date: 9/11/03    Time: 4:22p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * import and export of layers
 * 
 * *****************  Version 23  *****************
 * User: Adam Clayton Date: 9/05/03    Time: 4:35p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * exporting layers
 * 
 * *****************  Version 22  *****************
 * User: Adam Clayton Date: 8/18/03    Time: 3:56p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * fix map channel on uv set chooser
 * 
 * *****************  Version 21  *****************
 * User: Adam Clayton Date: 8/12/03    Time: 5:28p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * move around files
 * animation curve save out less data on child joints
 * 
 * *****************  Version 20  *****************
 * User: Adam Clayton Date: 7/14/03    Time: 1:12p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * add source file name
 * 
 * *****************  Version 19  *****************
 * User: Adam Clayton Date: 7/01/03    Time: 4:24p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * map attributes
 * y up 
 * 
 * *****************  Version 18  *****************
 * User: Adam Clayton Date: 6/27/03    Time: 2:02p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * map attributes export
 * 
 * *****************  Version 17  *****************
 * User: Adam Clayton Date: 6/17/03    Time: 2:57p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * vector3 issues
 * assert issues
 * add audio clips
 * 
 * *****************  Version 16  *****************
 * User: Adam Clayton Date: 6/05/03    Time: 12:02p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * motion path exporting
 * 
 * *****************  Version 15  *****************
 * User: Adam Clayton Date: 6/02/03    Time: 10:34a
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * animation globals
 * 
 * *****************  Version 14  *****************
 * User: Adam Clayton Date: 5/30/03    Time: 10:21a
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * add units to scene
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 5/15/03    Time: 4:19p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * first phase of animation export
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 4/16/03    Time: 4:26p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * new consolidation code
 * start of getting prebound mesh
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 12/12/02   Time: 8:55a
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * fix mesh instance matrix to use delta matrix from root bone
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
 * User: Adam Clayton Date: 12/05/02   Time: 3:48p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * bone pose exporting 
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 12/04/02   Time: 4:26p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * Skin and Bones exporting (matrices are not fixed yet)
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 11/15/02   Time: 4:30p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * vertex color modifiers
 * able to disable object consolidation
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 11/14/02   Time: 12:55p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * attribute fixes
 * consolidate based on name (ignore numbers on end)
 * save material from mesh to use on other instances
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 11/13/02   Time: 4:21p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * first pass of materials, maps, images, and attributes
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 11/07/02   Time: 3:58p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * node, mesh output working
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 11/06/02   Time: 3:02p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * first pass of mesh stuff
 */

#include "TupImpExpPch.h"
#include "TupExport.h"
#include "SkinManager.h"
#include "MeshManager.h"
#include "TupAnimExport.h"

class TupExportAddShaders : public MeshManager::AddShadersInterface
{
	TupExport &m_parent;
public:
	TupExportAddShaders(TupExport &parent) : m_parent(parent) {}
	int AddShaders(const MDagPath &shapePath,const MObjectArray &shaderArray,MIntArray &shaderIndices)
	{
		m_parent.m_shapePath = shapePath;
		return m_parent.AddMaterial(shaderArray,shaderIndices);
	}
};


int TupExport::ExportScene(const TCHAR *outputFilename)
{
   BuildScene(outputFilename);

   // save the tree
   TupperwareLoadSaveManager::FormatType formatType = m_bBinaryOutput ? TupperwareLoadSaveManager::BINARY : TupperwareLoadSaveManager::ASCII;
   return TupperwareLoadSaveManager::Write(formatType,outputFilename,m_pSceneAgg);
}

void TupExport::BuildScene(const TCHAR *outputFilename)
{
	// update dependency graph (make sure the face attributes stick)
	MGlobal::executeCommand(MString("dgdirty -a"));

	// create a new scene
	m_pSceneAgg = new TupperwareAggregate;
	m_pTupScene = new TupScene(m_pSceneAgg);

	// create the node pool
	TupperwareAggregate *pNodePoolAgg = m_pTupScene->AddNodePool();
	m_pNodePool = new TupNodePool(pNodePoolAgg);

	// create the object pool
	TupperwareAggregate *pObjectPoolAgg = m_pTupScene->AddObjectPool();
	m_pObjectPool = new TupObjectPool(pObjectPoolAgg);

	// create the modifier pool
	TupperwareAggregate *pModifierPoolAgg = m_pTupScene->AddModifierPool();
	m_pModifierPool = new TupModifierPool(pModifierPoolAgg);

	// create the material pool
	TupperwareAggregate *pMaterialPoolAgg = m_pTupScene->AddMaterialPool();
	m_pMaterialPool = new TupMaterialPool(pMaterialPoolAgg);

	// create the map pool
	TupperwareAggregate *pMapPoolAgg = m_pTupScene->AddMapPool();
	m_pMapPool = new TupMapPool(pMapPoolAgg);

	// create the image pool
	TupperwareAggregate *pImagePoolAgg = m_pTupScene->AddImagePool();
	m_pImagePool = new TupImagePool(pImagePoolAgg);

	// create the animation set pool
	TupperwareAggregate *pAnimSetPoolAgg = m_pTupScene->AddAnimSetPool();
	m_pAnimSetPool = new TupAnimSetPool(pAnimSetPoolAgg);
	m_pCurrentTupAnimChannelPool = NULL;

	// create the connection pool
	TupperwareAggregate *pConnectionPoolAgg = m_pTupScene->AddConnectionPool();
	m_pConnectionPool = new TupConnectionPool(pConnectionPoolAgg);

	// create the audio pool
	TupperwareAggregate *pAudioPoolAgg = m_pTupScene->AddAudioPool();
	m_pAudioPool = new TupAudioPool(pAudioPoolAgg);

	// create the layer pool
	TupperwareAggregate *pLayerPoolAgg = m_pTupScene->AddLayerPool();
	m_pLayerPool = new TupLayerPool(pLayerPoolAgg);

	// create new keepers
	m_pNodePoolDagPaths = new Keeper<MDagPath>;
	if (m_bExportSelected)
		m_pExportDagPaths = new Keeper<MDagPath>;

	m_pUniqueObjectPtrs = new Keeper<int>;
	m_pUniqueObjectRefs = new TupArray<UniqueObjectRefs>;

	m_pLayerNames = new Keeper<TupString>;
	m_pLayers = new TupArray<LayerData>;

	m_pMaterialCRCValues = new TupArray<unsigned long>;
	m_pMapCRCValues = new TupArray<unsigned long>;

	m_pImagePoolStrings = new Keeper<TupString>;

	m_pSkinManager = new SkinManager;
	m_pAddShaders = new TupExportAddShaders(*this);
	m_pMeshManager = new MeshManager;
	m_pMeshManager->SetAddShadersInterface(m_pAddShaders);

#ifdef IMBED_ATTRIBUTE_CFG
	// save these for later use (images especially)
//	m_sourceFilename = sourceFilename;
	m_outputFilename = outputFilename;

	char programPathBufferTemp[_MAX_PATH];
	char programPathBuffer[_MAX_PATH];
	GetModuleFileName(NULL,programPathBufferTemp,_MAX_PATH);
	FilePath::GetPath(programPathBufferTemp ,programPathBuffer);
	if (programPathBuffer[strlen(programPathBuffer)-1]!='\\')
		strcat(programPathBuffer,"\\");

	// Create the path to the node attribute config file:
	TupString tstrFilename(programPathBuffer);
	tstrFilename += "plug-ins\\ObjAttr.cfg";

	// Now read in the config file and create the tupperware tree:
	TupperwareAggregate *pNodeAttributeTreeAgg;
	if (TupperwareLoadSaveManager::Read(tstrFilename,&pNodeAttributeTreeAgg))
	{
		// add to scene
		m_pTupScene->AddNodeAttributeTree(pNodeAttributeTreeAgg);
	}

	// Create the path to the default config file:
	tstrFilename = programPathBuffer;
	tstrFilename += "plug-ins\\FaceAttr.cfg";

	// Now read in the config file and create the tupperware tree:
	TupperwareAggregate *pFaceAttributeTreeAgg;
	if (TupperwareLoadSaveManager::Read(tstrFilename,&pFaceAttributeTreeAgg))
	{
		// add to scene
		m_pTupScene->AddFaceAttributeTree(pFaceAttributeTreeAgg);
	}

	// Create the path to the default config file:
	tstrFilename = programPathBuffer;
	tstrFilename += "plug-ins\\MapAttr.cfg";
	
	// Now read in the config file and create the tupperware tree:
	TupperwareAggregate *pMapAttributeTreeAgg;
	if (TupperwareLoadSaveManager::Read(tstrFilename,&pMapAttributeTreeAgg))
	{
		// add to scene
		m_pTupScene->AddMapAttributeTree(pMapAttributeTreeAgg);
	}
#endif

	// add in the date, time and version
	m_pTupScene->AddExporterDate(g_pExporterDate);
	m_pTupScene->AddExporterTime(g_pExporterTime);
	m_pTupScene->AddVersion(TupScene::GetSceneVersion());
   TupExport::AddUnits(m_pTupScene);

	TupString sourceFileName = MFileIO::currentFile().asChar();
	m_pTupScene->AddSourceFileName(sourceFileName);

	// create the root node
	MStatus status;
	MItDag dagIterator( MItDag::kBreadthFirst, MFn::kInvalid, &status);

	MFnDagNode rootNode(dagIterator.item());

	MDagPath rootDagPath;
	status = dagIterator.getPath(rootDagPath);
	TupperwareAggregate *pTupRootNodeAgg;
	GetLayers();
	if (m_bExportSelected)
	{
		AddSelectedDagPathsForExport();
	}
	SetLayerReferenceValues();
	AddLayerPool();

	m_rootNodeRef = m_pNodePoolDagPaths->Add(rootDagPath); // add to both pools
	pTupRootNodeAgg = m_pNodePool->AddNode(m_rootNodeRef);

	m_pTupScene->AddRootNodeReference(m_rootNodeRef);

	TupNode tupRootNode(pTupRootNodeAgg);
	MFnDagNode dagNode(rootDagPath.node());

	tupRootNode.AddName(dagNode.name().asChar());
	tupRootNode.AddParentNodeReference(-1); // root has -1 as its parent reference
	AddNodesToPools(rootNode);
	BuildSkinData();
	AddNodesToScene(rootNode,tupRootNode,m_rootNodeRef);
   TupAnimExport::AddAnimationGlobals(m_pTupScene);
   TupAnimExport tupAnimExport(m_bExportSelected,m_pExportDagPaths);
	tupAnimExport.AddAnimation(rootDagPath,m_pAnimSetPool,m_pNodePoolDagPaths);
	AddConnections();
	AddAudio();

   int nNumNodes = m_pNodePool->GetNumNodes();
   char number[32];
   MString sceneinfo = "Info: ";
   itoa(nNumNodes,number,10);
   sceneinfo += number;
   sceneinfo += " nodes exported.\n";
   MGlobal::displayInfo(sceneinfo);
}

static const char *invalidNodeNamesFull[] = 
{
	"groundPlane_transform",
	"persp",
	"top",
	"front",
	"side",
	NULL
};

static const char *invalidNodeNamesPart[] = 
{
	"References",
	NULL
};

bool TupExport::ValidNodeName(const char *pName)
{
	int index=0;
	while (invalidNodeNamesFull[index])
	{
		if (!strcmp(pName,invalidNodeNamesFull[index]))
		{
			return false;
		}
		index++;
	}
	index=0;
	while (invalidNodeNamesPart[index])
	{
		// if a portion of the name is found
		if (strstr(pName,invalidNodeNamesPart[index]))
		{
			return false;
		}
		index++;
	}
	return true;
}

void TupExport::AddNodesToPools(MFnDagNode &rootNode)
{
	int numChildren = rootNode.childCount();

	int childIndex;
	for (childIndex = 0; childIndex < numChildren; childIndex++)
	{
		MFnDagNode childNode(rootNode.child(childIndex));
		MDagPath childPath;
		childNode.getPath(childPath);
		if (ValidNode(childPath) && ValidNodeName(childNode.name().asChar()))
		{
			TupperwareAggregate *pChildNodeAgg;
			int childReference = m_pNodePoolDagPaths->GetIndex(childPath);
			// this child better be new to the pools
			assert(childReference==-1);
			childReference = m_pNodePoolDagPaths->Add(childPath); // add to both pools
			pChildNodeAgg = m_pNodePool->AddNode(childReference);
			AddChildrenNodesToPools(childPath);
		}
	}
}

void TupExport::AddNodesToScene(MFnDagNode &rootNode,TupNode &tupRootNode,int rootIndex)
{
	int numChildren = rootNode.childCount();

	int childIndex;
	for (childIndex = 0; childIndex < numChildren; childIndex++)
	{
		MFnDagNode childNode(rootNode.child(childIndex));
		MDagPath childPath;
		childNode.getPath(childPath);
		if (ValidNode(childPath) && ValidNodeName(childNode.name().asChar()))
		{
			TupperwareAggregate *pChildNodeAgg;
			int childReference = m_pNodePoolDagPaths->GetIndex(childPath);
			// this child better be new to the pools
			assert(childReference!=-1);
			pChildNodeAgg = m_pNodePool->GetNode(childReference); // add to both pools
			tupRootNode.AddChildNodeReference(childReference);
			AddNode(rootIndex,childReference,pChildNodeAgg,childPath);
		}
	}
}

//
// Given a jointCluster, return the joint that drives it
//
MObject TupExport::GetJointForCluster(MObject& jointCluster)
{
	MObject result;
	MFnDependencyNode fnNode(jointCluster);
	MObject attrJoint = fnNode.attribute("matrix");
	MPlug pJointPlug(jointCluster,attrJoint);
	MPlugArray conns;
	if (pJointPlug.connectedTo(conns,true,false)) 
	{
		result = conns[0].node();
	}
	return result;
}

//
// Given a jointCluster, return the joint that drives it
//
MMatrix TupExport::GetBindPoseMatrixForJoint(MObject& jointNode)
{
	MMatrix matrix;

	MStatus status;
	MFnDependencyNode fnJoint(jointNode);
	MObject aBindPose = fnJoint.attribute("bindPose",&status);

	if (MS::kSuccess == status) 
	{
		unsigned connLength = 0;
		MPlugArray connPlugs;
		MPlug pBindPose(jointNode,aBindPose);
		pBindPose.connectedTo(connPlugs,false,true);
		connLength = connPlugs.length();
		for (unsigned ii = 0; ii < connLength; ++ii) 
		{
			if (connPlugs[ii].node().apiType() == MFn::kDagPose) 
			{
				MObject aMember = connPlugs[ii].attribute();
				MFnAttribute fnAttr(aMember);
				if (fnAttr.name() == "worldMatrix") 
				{
					unsigned jointIndex = connPlugs[ii].logicalIndex();
					MObject jointObject = connPlugs[ii].node();

					MFnDependencyNode nDagPose(jointObject);

					// construct plugs for this joints world and local matrices
					//
					MObject aWorldMatrix = nDagPose.attribute("worldMatrix");
					MPlug pWorldMatrix(jointObject,aWorldMatrix);
					pWorldMatrix.selectAncestorLogicalIndex(jointIndex,aWorldMatrix);

					MObject worldMatrix;
					MStatus status = pWorldMatrix.getValue(worldMatrix);
					if (MS::kSuccess == status) 
					{
						MFnMatrixData dMatrix(worldMatrix);
						MMatrix wMatrix = dMatrix.matrix(&status);
						if (MS::kSuccess == status) 
						{
							matrix = wMatrix;
						}
					}
				}
			}
		}
	}
	return matrix;
}

void TupExport::BuildSkinData(void)
{
	MStatus stat;

	// count the processed jointClusters
	//
	unsigned int jcCount = 0;

	// Iterate through graph and search for jointCluster nodes
	//
	MItDependencyNodes jcIter( MFn::kJointCluster);
	for ( ; !jcIter.isDone(); jcIter.next() ) 
	{
		MObject object = jcIter.item();
		MFnWeightGeometryFilter jointCluster(object);

		// get the joint driving this cluster
		//
		MObject joint = GetJointForCluster(object);
		if (joint.isNull()) 
		{
			continue;
		}

		MMatrix bPMatrix = GetBindPoseMatrixForJoint(joint);
		Graphics4x4 bindingPoseMatrix;
		ConvertMayaBoneMatrix(bPMatrix,bindingPoseMatrix);

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

		// print out the name of joint and the number of associated skins
		//
		MFnDependencyNode fnJoint(joint);
		MDagPath jointNodePath;
		MFnDagNode fnJointNode(joint);
		fnJointNode.getPath(jointNodePath);
		TupString jointName(fnJointNode.name().asChar());

		int jointNodeRef = m_pNodePoolDagPaths->GetIndex(jointNodePath);
		if (jointNodeRef==-1)
			continue;

		for (size_t kk = 0; kk < clusterSetList.length(); ++kk) 
		{
			MObject joint;
			MDagPath skinPath;
			MObject components;
			MFloatArray weights;

			clusterSetList.getDagPath(kk,skinPath,components);
			jointCluster.getWeights(skinPath,components,weights);

			MFnDependencyNode fnSkinNode(skinPath.node());
			int skinObjectPtr = (int)MayaHelper::GetMPtrBase(fnSkinNode);
			MMatrix tm = skinPath.inclusiveMatrix();
			Graphics4x4 skinMatrix;
			ConvertMayaMatrix(tm,skinMatrix);

			SkinManager::BoneConnections boneConnections;
			boneConnections.m_boneMatrix = bindingPoseMatrix;
			boneConnections.m_boneName = jointName;
			boneConnections.m_boneNodeRef = jointNodeRef;

			// loop through the components
			MItGeometry gIter(skinPath,components);
			for (int componentIndex = 0; !gIter.isDone() && componentIndex <(int) weights.length(); gIter.next(),componentIndex++) 
			{
				boneConnections.m_vertexIndices.Add(gIter.index());
				boneConnections.m_weights.Add(weights[componentIndex]);
			}
			// if we have any verts then add
			if (boneConnections.m_vertexIndices.GetSize())
			{
				m_pSkinManager->AddSkinToMesh(skinObjectPtr,skinMatrix,boneConnections);
			}
		}
	}
	// Iterate through graph and search for skincluster nodes
	//
	MItDependencyNodes scIter( MFn::kSkinClusterFilter);
	for ( ; !scIter.isDone(); scIter.next() ) 
	{
		MObject object = scIter.item();
		MFnSkinCluster skinCluster(object);

		// get the joints that are hooked to the mesh
		MDagPathArray jointPaths;
		int numJoints = skinCluster.influenceObjects(jointPaths,&stat);
		for (int jointIndex=0;jointIndex<numJoints;jointIndex++)
		{
			SkinManager::BoneConnections boneConnections;
			MFnDagNode fnJointNode(jointPaths[jointIndex]);
			boneConnections.m_boneName = fnJointNode.name().asChar();
			int jointNodeRef = m_pNodePoolDagPaths->GetIndex(jointPaths[jointIndex]);
			if (jointNodeRef==-1)
				continue;
			boneConnections.m_boneNodeRef = jointNodeRef;

			MMatrix bPMatrix = GetBindPoseMatrixForJoint(jointPaths[jointIndex].node());
			Graphics4x4 bindingPoseMatrix;
			ConvertMayaBoneMatrix(bPMatrix,bindingPoseMatrix);
			boneConnections.m_boneMatrix = bindingPoseMatrix;

			MSelectionList componentsAffected;
			MFloatArray weightsForComponents;
			stat = skinCluster.getPointsAffectedByInfluence(jointPaths[jointIndex],componentsAffected,weightsForComponents);
			int numComponents = componentsAffected.length();
			int numWeights = weightsForComponents.length();
			int weightIndex = 0; // keep track of which weight we are on
			int meshObjectPtr;
			Graphics4x4 meshMatrix;
			for (int componentIndex=0;componentIndex<numComponents;componentIndex++)
			{
				MDagPath componentPath;
				MObject component;
				stat = componentsAffected.getDagPath(componentIndex,componentPath,component);

				MFnDependencyNode fnMeshNode(componentPath.node());
				meshObjectPtr = (int)MayaHelper::GetMPtrBase(fnMeshNode);
				MMatrix tm = componentPath.inclusiveMatrix();
				ConvertMayaMatrix(tm,meshMatrix);

				if (component.apiType()==MFn::kMeshVertComponent)
				{
					MItMeshVertex vertexIter(componentPath,component);
					for (;!vertexIter.isDone();vertexIter.next())
					{
						int vertexIndex = vertexIter.index(&stat);
						boneConnections.m_vertexIndices.Add(vertexIndex);
						boneConnections.m_weights.Add(weightsForComponents[weightIndex]);
						weightIndex++;
					}
				}
			}
			// if we have any verts then add
			if (boneConnections.m_vertexIndices.GetSize())
			{
				m_pSkinManager->AddSkinToMesh(meshObjectPtr,meshMatrix,boneConnections);
			}
		}
	}
	AddExtraBones();
}

// this code is used to add any other bones that are part of the skeleton but are not connected to the mesh
void TupExport::AddExtraBones(void)
{
	int numSkins = m_pSkinManager->GetNumSkins();
	for (int skinIndex=0;skinIndex<numSkins;skinIndex++)
	{
		Keeper<int> attachedBones;
		Keeper<int> freeBones;
		int numBones = m_pSkinManager->GetNumBones(skinIndex);
		// go through all the bones that are already in the list
		int boneIndex;
		for (boneIndex=0;boneIndex<numBones;boneIndex++)
		{
			const SkinManager::BoneConnections &boneConnections = m_pSkinManager->GetBoneConnections(skinIndex,boneIndex);
			attachedBones.Add(boneConnections.m_boneNodeRef);
		}
		int numAttachedBones = attachedBones.GetSize();
		for (boneIndex=0;boneIndex<numAttachedBones;boneIndex++)
		{
			AddRelatedBones(attachedBones[boneIndex],attachedBones,freeBones);
		}
		int numFreeBones = freeBones.GetSize();
		for (boneIndex=0;boneIndex<numFreeBones;boneIndex++)
		{
			MDagPath dagPath = (*m_pNodePoolDagPaths)[freeBones[boneIndex]];
			MObject joint = dagPath.node();
			MMatrix bPMatrix = GetBindPoseMatrixForJoint(joint);
			Graphics4x4 bindingPoseMatrix;
			ConvertMayaBoneMatrix(bPMatrix,bindingPoseMatrix);
			MFnDagNode fnJointNode(joint);
			TupString jointName(fnJointNode.name().asChar());

			SkinManager::BoneConnections boneConnections;
			boneConnections.m_boneMatrix = bindingPoseMatrix;
			boneConnections.m_boneName = jointName;
			boneConnections.m_boneNodeRef = freeBones[boneIndex];
			// no vertices or weights
			m_pSkinManager->AddFreeBoneToSkin(skinIndex,boneConnections);
		}
	}
}
	
void TupExport::AddRelatedBones(int boneRef,const Keeper<int> &attachedBones,Keeper<int> &freeBones)
{
	MDagPath mDagPath = (*m_pNodePoolDagPaths)[boneRef];
	MDagPath parentDagPath = mDagPath;
	parentDagPath.pop();
	int parentNodeRef = m_pNodePoolDagPaths->GetIndex(parentDagPath);
	assert(parentNodeRef!=-1);
	if (parentNodeRef!=m_rootNodeRef && 
		 attachedBones.GetIndex(parentNodeRef)==-1 && 
		 freeBones.GetIndex(parentNodeRef)==-1 &&
		 parentDagPath.apiType() == MFn::kJoint)
	{
		freeBones.Add(parentNodeRef);
		AddRelatedBones(parentNodeRef,attachedBones,freeBones);
	}
	int numChildren = mDagPath.childCount();
	for (int childIndex = 0; childIndex < numChildren; childIndex++)
	{
		MObject childObject = mDagPath.child(childIndex);
		MDagPath childPath = mDagPath;
		childPath.push(childObject);
		
		if (childPath.apiType() == MFn::kJoint)
		{
			int childNodeRef = m_pNodePoolDagPaths->GetIndex(childPath);
			if (childNodeRef!=-1 && attachedBones.GetIndex(childNodeRef)==-1 && freeBones.GetIndex(childNodeRef)==-1)
			{
				freeBones.Add(childNodeRef);
				AddRelatedBones(childNodeRef,attachedBones,freeBones);
			}
		}
	}
}
	
void TupExport::AddUnits(TupScene *pTupScene)
{
	int unitFlags = 0;
   unitFlags |= TupScene::TIME_UNIT_SECONDS;// we convert to this
	unitFlags |= TupScene::LINEAR_UNIT_METER; // we convert to this
	unitFlags |= TupScene::ANGULAR_UNIT_RADIANS; // we convert to this
#ifdef MAKE_Z_UP
	unitFlags |= TupScene::UP_AXIS_Z; // we convert to z up
#else
	unitFlags |= TupScene::UP_AXIS_Y; // use native format
#endif
	pTupScene->AddUnitFlags(unitFlags);
}
