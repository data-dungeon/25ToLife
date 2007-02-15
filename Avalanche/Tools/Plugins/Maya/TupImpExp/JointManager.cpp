////////////////////////////////////////////////////////////////////////////
//
// JointManager
//
// Class to handle keeping track of the skin data for import
//
////////////////////////////////////////////////////////////////////////////

#include "TupImpExpPch.h"
#include "JointManager.h"

JointManager::JointManager(TupNodePool &tupNodePool,TupObjectPool &tupObjectPool,TupModifierPool &tupModifierPool) :
	m_tupNodePool(tupNodePool),
	m_tupObjectPool(tupObjectPool),
	m_tupModifierPool(tupModifierPool)
{
}

void JointManager::ReadSkinData(void)
{
	int numNodes = m_tupNodePool.GetNumNodes();
	for (int nodeIndex=0;nodeIndex<numNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = m_tupNodePool.GetNode(nodeIndex);
		if (!pNodeAgg)
			continue;

		TupNode tupNode(pNodeAgg);
		int objectRef;
		tupNode.GetObjectReference(objectRef);
		if (objectRef==-1)
			continue;

		int baseObjectRef = TupObjectHelper::FindBaseObjectRef(m_tupObjectPool,objectRef);
		if (baseObjectRef==-1)
			continue;

		TupperwareAggregate *pBaseObjectAgg = m_tupObjectPool.GetObjectAggregate(baseObjectRef);
		if (!pBaseObjectAgg)
			continue;

		TupObject tupObject(pBaseObjectAgg);
		if (tupObject.GetObjectType()!=TupObject::POLY_MESH)
			continue;

		TupPolyMesh tupPolyMesh(pBaseObjectAgg);
		Vector3 *pPositionVerts;
		int numVerts;
		tupPolyMesh.GetPositionVerts((float **)&pPositionVerts,numVerts);
		if (!numVerts)
			continue;

		TupperwareAggregate *pSkinModifierAgg;
		TupperwareAggregate *pSkinModifierContextAgg;
		if (TupObjectHelper::FindSkin(m_tupObjectPool,objectRef,m_tupModifierPool,
											&pSkinModifierAgg,&pSkinModifierContextAgg))
		{
			TupSkinModifier tupSkinModifier(pSkinModifierAgg);
			TupSkinModifierContext tupSkinModifierContext(pSkinModifierContextAgg);

			int numBones = tupSkinModifier.GetNumBones();
			if (!numBones)
				continue;

//			float *pFloatList;
//			tupSkinModifierContext.GetBaseTM(&pFloatList);
//			Graphics4x4 *pBaseMatrix = (Graphics4x4 *)pFloatList;
//			Graphics4x4 invBaseMatrix;
//			pBaseMatrix->Invert(invBaseMatrix);

			SkinForMesh skinForMeshTemp;
			m_skins.Add(skinForMeshTemp);
			SkinForMesh &skinForMesh = m_skins[m_skins.GetSize()-1];
			skinForMesh.m_bRigidBind = true; // default to rigid bind
			skinForMesh.m_meshNodeRef = nodeIndex;
			skinForMesh.m_meshObjectRef = baseObjectRef;

			skinForMesh.m_bones.SetSize(numBones);
			for (int boneIndex=0;boneIndex<numBones;boneIndex++)
			{
				BoneConnections &boneConnections = skinForMesh.m_bones[boneIndex];

				TupperwareAggregate *pModifierBoneAgg = tupSkinModifier.GetBone(boneIndex);
				TupSkinModifierBone tupBone(pModifierBoneAgg);
				int boneReference;
				tupBone.GetNodeReference(boneReference);
				boneConnections.m_boneNodeRef = boneReference;

				// update keeper to get to the skin data from the bones
				int oldKeeperIndex = m_boneNodes.GetSize();
				int keeperIndex = m_boneNodes.Add(boneReference);
				if (keeperIndex==oldKeeperIndex)
				{
					m_boneSkinIndices.Add(m_skins.GetSize()-1);
					m_boneInSkinIndices.Add(boneIndex);
				}

				TupperwareAggregate *pBoneNodeAgg = m_tupNodePool.GetNode(boneReference);
				assert(pBoneNodeAgg);
				TupNode boneNode(pBoneNodeAgg);

				// get the length of the bone
				float boneLength = 1.0f;
				int boneNodeObjectRef;
				boneNode.GetObjectReference(boneNodeObjectRef);
				assert(boneNodeObjectRef!=-1);
				TupperwareAggregate *pBoneNodeObjectAgg = m_tupObjectPool.GetObjectAggregate(boneNodeObjectRef);
				assert(pBoneNodeObjectAgg);
				TupObject tupBoneNodeObject(pBoneNodeObjectAgg);
				if (tupBoneNodeObject.GetObjectType()==TupObject::BONE)
				{
					TupBone tupBoneNodeObjectBone(pBoneNodeObjectAgg);
					tupBoneNodeObjectBone.GetLength(boneLength);
				}
				boneConnections.m_boneLength = boneLength;

				// get the matrix for the bone
				float *pFloatList;
				tupBone.GetBoneInitObjectTM(&pFloatList);
				boneConnections.m_boneBindingPoseMatrix = *((Graphics4x4 *)pFloatList);
			}
			TupperwareAggregate *pVertexPoolAgg = tupSkinModifierContext.GetVertexPool();
			TupSkinModifierContextVertexPool tupSkinModifierContextVertexPool(pVertexPoolAgg);
			int numBoneVerts = tupSkinModifierContextVertexPool.GetNumVertices();
			for (int vertexIndex = 0;vertexIndex<numVerts;vertexIndex++)
			{
				if (vertexIndex<numBoneVerts)
				{
					// get the bones and weights for this vertex
					TupperwareAggregate *pBoneVertexAgg = tupSkinModifierContextVertexPool.GetVertex(vertexIndex);
               // if missing vertex in skinmodifier context, aggregate will be NULL
               // below will exit this scenario gracefully, but the engine isnt handling this scenario, so better to catch it earlier
//               if (!pBoneVertexAgg)
 //                 continue;
					TupSkinModifierContextVertex tupSkinModifierContextVertex(pBoneVertexAgg);
					int nBones;
					int *pBones;
					int nWeights;
					float *pWeights;
					tupSkinModifierContextVertex.GetBones(&pBones,nBones);
					tupSkinModifierContextVertex.GetWeights(&pWeights,nWeights);
					assert(nBones==nWeights);
					// if more than one bone then not rigid bind
					if (nBones>1)
						skinForMesh.m_bRigidBind = false;
					// go through list of bones and weights and find the one that has the most weight
					for (int weightIndex=0;weightIndex<nWeights;weightIndex++)
					{
						int boneIndex = pBones[weightIndex];
						float weight = pWeights[weightIndex];
						assert(boneIndex<numBones);
						skinForMesh.m_bones[boneIndex].m_vertexIndices.Add(vertexIndex);
						skinForMesh.m_bones[boneIndex].m_weights.Add(weight);
					}
				}
			}
		}
	}
}

bool JointManager::BoneInSkin(int boneNodeRef,int &skinIndex,int &boneInSkinIndex) const
{
	int boneIndex = m_boneNodes.GetIndex(boneNodeRef);
	if (boneIndex!=-1)
	{
		skinIndex = m_boneSkinIndices[boneIndex];
		boneInSkinIndex = m_boneInSkinIndices[boneIndex];
		return true;
	}
	return false;
}

void JointManager::AddEndBoneToSkin(int skinIndex,const JointManager::BoneConnections &boneConnections)
{
	m_skins[skinIndex].m_bones.Add(boneConnections);
}

int JointManager::GetSkinIndex(int meshObjectRef) const
{
	int numSkins = m_skins.GetSize();
	for (int skinIndex=0;skinIndex<numSkins;skinIndex++)
	{
		if (m_skins[skinIndex].m_meshObjectRef==meshObjectRef)
			return skinIndex;
	}
	return -1;
}

int JointManager::GetNumSkins(void) const
{
	return m_skins.GetSize();
}

int JointManager::GetNumBones(int skinIndex) const
{
	assert(skinIndex>=0&&skinIndex<m_skins.GetSize());
	const SkinForMesh &skin = m_skins[skinIndex];
	return skin.m_bones.GetSize();
}

const JointManager::SkinForMesh &JointManager::GetSkin(int skinIndex) const
{
	assert(skinIndex>=0&&skinIndex<m_skins.GetSize());
	return m_skins[skinIndex];
}

const JointManager::BoneConnections &JointManager::GetBoneConnections(int skinIndex,int boneIndex) const
{
	assert(skinIndex>=0&&skinIndex<m_skins.GetSize());
	const SkinForMesh &skin = m_skins[skinIndex];
	assert(boneIndex>=0&&boneIndex<skin.m_bones.GetSize());
	return skin.m_bones[boneIndex];
}

bool JointManager::SetDagPathForBone(int nodeRef,const MDagPath &boneDagPath)
{
	int skinIndex;
	int boneInSkinIndex;
	if (BoneInSkin(nodeRef,skinIndex,boneInSkinIndex))
	{
		m_skins[skinIndex].m_bones[boneInSkinIndex].m_boneDagPath = boneDagPath;
		return true;
	}
	return false;
}

void JointManager::SetMeshDagPathForSkin(int skinIndex,const MDagPath &meshDagPath)
{
	m_skins[skinIndex].m_meshDagPath = meshDagPath;
}

MObject JointManager::CreateBindPose(void)
{
	MStatus status;
	MFnDependencyNode newdepnode;
	MDGModifier dgModifier;

	MObject newbondPose = newdepnode.create("dagPose","bindPose",&status);
	if (!status)
		return MObject::kNullObj;
	return newbondPose;
}

void JointManager::ConnectSkinAndJoints(void)
{
	int numSkins = m_skins.GetSize();
	for (int skinIndex=0;skinIndex<numSkins;skinIndex++)
	{
		SkinForMesh &skinForMesh = m_skins[skinIndex];
		TupArray<MDagPath> bones;
		TupArray<MSelectionList> vertexGroups;
		int numBones = skinForMesh.m_bones.GetSize();
		for (int boneIndex=0;boneIndex<numBones;boneIndex++)
		{
			BoneConnections &boneConnections = skinForMesh.m_bones[boneIndex];
			bones.Add(boneConnections.m_boneDagPath);

			MSelectionList vertices;

			int numVertices = boneConnections.m_vertexIndices.GetSize();
			for (int vertexIndex=0; vertexIndex<numVertices; vertexIndex++)
			{
				MItMeshVertex vit(skinForMesh.m_meshDagPath,MObject::kNullObj);
				int prev;
				vit.setIndex(boneConnections.m_vertexIndices[vertexIndex],prev);
				vertices.add(skinForMesh.m_meshDagPath,vit.vertex());
			}
			vertexGroups.Add(vertices);
		}
		BindSkin(skinForMesh.m_meshDagPath,bones,vertexGroups);
	}
}

bool JointManager::BindSkin(const MDagPath &meshPath,const TupArray<MDagPath> &bones,const TupArray<MSelectionList> &vertexGroups)
{
	if (bones.GetSize()==0)
		return false;
	if (bones.GetSize()!=vertexGroups.GetSize())
		return false;

	MStatus status;
	MDGModifier dgModifier;

	bool firstjointClusterflag = true;
	MObject firstjointCluster;

	MObject prevJointCluster = MObject::kNullObj;

	MObjectArray boneSetArray,jointClusterarray,groupIDArray;
	boneSetArray.clear();
	jointClusterarray.clear();
	groupIDArray.clear();

	MObject bondPose = CreateBindPose();
	if (bondPose == MObject::kNullObj)
		return false;

	// need mesh extended transform
	MObject shape = meshPath.node(&status);
	if (!status)
		return false;

	MDagPath parentPath(meshPath);
	parentPath.pop();

	MFnDependencyNode shapedepnode(shape);
	MString modelname = shapedepnode.name();

	MFnMesh fnmesh(meshPath);

	MObject copyshape = fnmesh.copy(shape,parentPath.node(),&status);
	if (!status)
		return false;

	dgModifier.renameNode(copyshape,modelname + "Orig");
	dgModifier.doIt();

	//	parent copy shape under original objects transform
	MDagModifier dagmodifier;
	dagmodifier.reparentNode(copyshape,parentPath.node());
	dagmodifier.doIt();

	MFnDagNode dagnode(copyshape);
	dagnode.setIntermediateObject(true);

	MFnDependencyNode copyshapedepnode(copyshape);

	MPlug shapeoutmeshplug = copyshapedepnode.findPlug("outMesh",&status);
	if (!status)
		return false;

	MPlug meshinplug = shapedepnode.findPlug("inMesh",&status);
	if (!status)
		return false;

	status = dgModifier.connect(shapeoutmeshplug,meshinplug);
	dgModifier.doIt();
	if (!status)
		return false;

	//	create bones and much more
	Keeper<int> boneSetKeeper;
	TupArray<int> boneSetjointCluster;
	for (int i = 0; i < bones.GetSize(); i++)
	{
		MObject bone = bones[i].node();
		MString fullpathname = bones[i].fullPathName();
		const char *debug = fullpathname.asChar();

		if (bone == MObject::kNullObj)
			continue;

		const MSelectionList *vertices = &vertexGroups[i];

//		if (vertices->length()==0)
//			continue;

		MObject jointCluster,boneset;

		if (!HookUpBoneToBindPose(bones[i],bondPose,prevJointCluster,*vertices,jointCluster,boneset))
			return false;

		//			store boneset pointer in map
		MFnDependencyNode bonesetdepnode(boneset);

		boneSetArray.append(boneset);
		jointClusterarray.append(jointCluster);

		int oldBoneSetKeeperSize = boneSetKeeper.GetSize();
		int boneSetKeeperIndex = boneSetKeeper.Add((int)MayaHelper::GetMPtrBase(bonesetdepnode));
		if (boneSetKeeperIndex==oldBoneSetKeeperSize)
		{
			boneSetjointCluster.Add(jointClusterarray.length()-1);
		}

		if (firstjointClusterflag)
		{
			firstjointCluster = jointCluster;
			firstjointClusterflag = false;
		}	

		prevJointCluster = jointCluster;
	}

	// connect joint cluster chain just before in mesh
	MPlugArray connected;
	meshinplug.connectedTo(connected,true,false,&status);
	if (connected.length()==0)
		return false;
	MPlug nodeplug_connected_to_inmesh = connected[0];

	dgModifier.disconnect(nodeplug_connected_to_inmesh,meshinplug);
	dgModifier.doIt();

	MFnDependencyNode firstjointClusterdepnode(jointClusterarray[0]);

	MPlug jointClusterinput = firstjointClusterdepnode.findPlug("input",&status);
	if (!status)
		return false;

	int nextindex = jointClusterinput.numConnectedElements(&status);

	MPlug jointClusterinputplug = jointClusterinput.elementByLogicalIndex(nextindex,&status);
	if (!status)
		return false;

	MPlug inputgeometryplug = jointClusterinputplug.child(0,&status);

	status = dgModifier.connect(nodeplug_connected_to_inmesh,inputgeometryplug);
	dgModifier.doIt();
	if (!status)
		return false;

	MFnDependencyNode lastjointClusterdepnode(jointClusterarray[jointClusterarray.length()-1]);
	MPlug lastjointClusteroutputgeom = lastjointClusterdepnode.findPlug("outputGeometry",&status);
	if (!status)
		return false;

	nextindex = lastjointClusteroutputgeom.numConnectedElements(&status);

	MPlug lastjointClusteroutputgeom2 = lastjointClusteroutputgeom.elementByLogicalIndex(nextindex,&status);
	if (!status)
		return false;

	dgModifier.connect(lastjointClusteroutputgeom2,meshinplug);
	dgModifier.doIt();
	if (!status)
		return false;

	//	create skin partition
	MObject skinpartition = CreateSkinPartition();
	if (skinpartition == MObject::kNullObj)
		return false;

	MFnDependencyNode partitionDepNode(skinpartition);

	MPlug partitionsetsplug = partitionDepNode.findPlug("sets",&status);
	if (!status)
		return false;

	for (i = 0; i < (int)boneSetArray.length(); i++)
	{
		MFnDependencyNode bonesetdepnode(boneSetArray[i]);
		MPlug bonesetpartitionplug = bonesetdepnode.findPlug("partition",&status);
		if (!status)
			return false;

		int nextindex = partitionsetsplug.evaluateNumElements(&status);

		MPlug partitionsetplug = partitionsetsplug.elementByLogicalIndex(nextindex,&status);
		if (!status)
			return false;

		status = dgModifier.connect(bonesetpartitionplug,partitionsetplug);
		dgModifier.doIt();
		if (!status)
			return false;
	}

	//	assign group parts to jointClusters

	MPlug instobjgroupsplug = shapedepnode.findPlug("instObjGroups",&status);
	if (!status)
		return false;

	int numelements = instobjgroupsplug.evaluateNumElements(&status);

	for (i = 0; i < numelements; i++)
	{
		MPlug instobjgroupsplug2 = instobjgroupsplug.elementByPhysicalIndex(i,&status);

		MPlug instobjgroupsplug3 = instobjgroupsplug2.child(0,&status);
		if (!status)
			return false;

		MString instobjgroupsplugname = instobjgroupsplug2.name();
		const char *debug0 = instobjgroupsplugname.asChar();

		int numelements2 = instobjgroupsplug3.evaluateNumElements(&status);

		for (int j = 0; j < numelements2; j++)
		{
			MPlug instobjgroupplug = instobjgroupsplug3.elementByPhysicalIndex(j);

			MPlugArray connected2;
			instobjgroupplug.connectedTo(connected2,false,true,&status);
			for (int k = 0; k < (int)connected2.length(); k++)
			{
				MObject setobject = connected2[k].node();

				MFnDependencyNode setdepnode(setobject);
				const char *setname = setdepnode.name().asChar();

				int boneSetKeeperIndex = boneSetKeeper.GetIndex((int)MayaHelper::GetMPtrBase(setdepnode));

				if (boneSetKeeperIndex!=-1)
				{
					//	get objectgroupid child
					MPlug instobjgroupidplug = instobjgroupplug.child(1,&status);
					if (!status)
						return false;

					MPlugArray connected;
					instobjgroupidplug.connectedTo(connected,true,false,&status);
					if (!status)
						return false;

					if (connected.length()==0)
						return false;

					MObject groupidnode = connected[0].node();

					MFnDependencyNode groupiddepnode(groupidnode);

					MPlug groupidplug = groupiddepnode.findPlug("groupId",&status);

					int index = boneSetjointCluster[boneSetKeeperIndex];

					MObject jointCluster = jointClusterarray[index];

					MFnDependencyNode jointClusterdepnode(jointCluster);

					MPlug jointClusterinputplug = jointClusterdepnode.findPlug("input",&status);
					if (!status)
						return false;

					MPlug jointClusterinputplug2 = jointClusterinputplug.elementByLogicalIndex(0,&status);
					if (!status)
						return false;

					MPlug jointClusterinputchildplug = jointClusterinputplug2.child(1,&status);
					if (!status)
						return false;

					MString groupidplugname = groupidplug.name();
					MString jointClusterinputchildplugname = jointClusterinputchildplug.name();

					const char *debugA = groupidplugname.asChar();
					const char *debugB = jointClusterinputchildplugname.asChar();

					dgModifier.connect(groupidplug,jointClusterinputchildplug);
					if (!status)
						return false;

					dgModifier.doIt();
				}
			}	
		}
	}

	MObject rootbone = bones[0].node();
	MFnDependencyNode rootbonedepnode(rootbone);

	MGlobal::executeCommand(MString("bindSkin -enable 1 ") + rootbonedepnode.name());
	dgModifier.doIt();

	return true;
}

bool JointManager::HookUpBoneToBindPose(const MDagPath &jointDagPath,MObject bondPose,MObject prevJointCluster,const MSelectionList &vertices,MObject &jointCluster,MObject &boneset)
{
	MStatus status;
	MDGModifier dgModifier;

	MObject joint = jointDagPath.node();
	MString bonefullpathname = jointDagPath.fullPathName();
	const char *fullpathname = bonefullpathname.asChar();

	MDagPath dagpath;
	MFnDagNode dagnode(joint);
	dagnode.getPath(dagpath);

	MFnIkJoint fnjoint(joint);
	MTransformationMatrix xformMatrix = fnjoint.transformation(&status);

	MMatrix localtoworld = dagpath.inclusiveMatrix();

	MMatrix localtoparent = fnjoint.transformationMatrix();

	if (bondPose != MObject::kNullObj)
	{
		if(!CreateJointClusterChain(jointDagPath,vertices,prevJointCluster,jointCluster,boneset))
		{	
			MGlobal::displayWarning("Couldn't create jointCluster chain.");
			return false;
		}

		//		get message, members and bondPose plugs
		MFnDependencyNode jointdepnode(joint);
		MPlug messageplug = jointdepnode.findPlug("message",&status);
		if (!status)
			return false;
		MPlug bondPoseplug = jointdepnode.findPlug("bindPose",&status);
		if (!status)
			return false;

		MFnDependencyNode bondPosedepnode(bondPose);

		MPlug membersplug = bondPosedepnode.findPlug("members",&status);
		if (!status)
			return false;

		int nextindex = membersplug.evaluateNumElements(&status);

		MPlug membersplug2 = membersplug.elementByLogicalIndex(nextindex,&status);
		if (!status)
			return false;

		//		get world matrix plug
		MPlug worldmatrixplug = bondPosedepnode.findPlug("worldMatrix",&status);
		if (!status)
			return false;

		nextindex = worldmatrixplug.evaluateNumElements(&status);

		MPlug worldmatrixplug2;

		worldmatrixplug2 = worldmatrixplug.elementByLogicalIndex(nextindex,&status);
		if (!status)
			return false;

		MString plugname = worldmatrixplug2.name();
		const char *debug = plugname.asChar();

		//		get xform matrix plug
		MPlug xformmatrixplug = bondPosedepnode.findPlug("xformMatrix",&status);
		if (!status)
			return false;

		MPlug xformmatrixplug2 = xformmatrixplug.elementByLogicalIndex(nextindex,&status);
		if (!status)
			return false;

		/////////////////////////////////////////////////////////////////////////////////
		//		Set joint bind pose local to world

		MFnMatrixData dMatrix;
		MObject localtoworldmatrixobject = dMatrix.create(localtoworld,&status);
		if (!status)
			return false;

		status = bondPoseplug.setValue(localtoworldmatrixobject);
		if (!status)
			return false;
		/////////////////////////////////////////////////////////////////////////////////		

		//		connect bondPose plug on joint to worldmatrix plug on bondPose
		status = dgModifier.connect(bondPoseplug,worldmatrixplug2);
		if (!status)
			return false;

		//		connect joint message to bondPose members	
		status = dgModifier.connect(messageplug,membersplug2);
		if (!status)
			return false;

		dgModifier.doIt();

		//		SET bondPose "xformMatrix"
		MFnMatrixData dMatrix2;
		MObject xformmatrixobject = dMatrix2.create(xformMatrix,&status);
		if (!status)
			return false;

		status = xformmatrixplug2.setValue(xformmatrixobject);
		if (!status)
			return false;

		//		dgModifier.doIt();

		//		SET jointCluster "weightCompensationMatrix"
		MFnDependencyNode jointClusterdepnode(jointCluster);
		MPlug weightedCompensationMatrixPlug = jointClusterdepnode.findPlug("weightedCompensationMatrix",&status);

		status = weightedCompensationMatrixPlug.setValue(xformmatrixobject);
		if (!status)
			return false;

		//		SET jointCluster "geomMatrix"
		MPlug geomMatrixPlug = jointClusterdepnode.findPlug("geomMatrix",&status);

		nextindex = geomMatrixPlug.evaluateNumElements(&status);

		MPlug geomMatrixPlug2 = geomMatrixPlug.elementByLogicalIndex(nextindex,&status);
		if (!status)
			return false;

		MFnMatrixData dMatrix3;
		MMatrix ident;
		ident.setToIdentity();
		MObject geommatrixobject = dMatrix3.create(ident,&status);

		status = geomMatrixPlug2.setValue(geommatrixobject);
		if (!status)
			return false;

		//		SET jointcluser "bindPreMatrix"
		MPlug bindPreMatrixPlug = jointClusterdepnode.findPlug("bindPreMatrix",&status);

		MMatrix localtoworldinverse = localtoworld.inverse();
		MObject localtoworldinverseobject = dMatrix.create(localtoworldinverse,&status);

		status = bindPreMatrixPlug.setValue(localtoworldinverseobject);
		if (!status)
			return false;

		//		dgModifier.doIt();

		return true;
	}
	return false;
}

// create joint cluster node, boneset, clusterhandle, link them all up
bool JointManager::CreateJointClusterChain(const MDagPath &bonedagpath,const MSelectionList &vertices,
														MObject prevJointCluster,
														MObject &jointCluster,
														MObject &boneset)
{
	MStatus status;
	MDGModifier dgModifier;
	MDagModifier dagmodifier;

	MObject bone = bonedagpath.node();
	MString bonefullpathname = bonedagpath.fullPathName();
	const char *fullpathname = bonefullpathname.asChar();

	//	Create Joint Cluster
	jointCluster = CreateJointCluster(bone);
	if (jointCluster==MObject::kNullObj)
		return false;

	//	Connect bone.worldMatrix to jointCluster.matrix
	MFnDependencyNode bonedepnode(bone);

	MPlug srcplug = bonedepnode.findPlug("worldMatrix",&status);
	if (!status)
		return false;

	int nextindex = srcplug.evaluateNumElements(&status);

	MPlug plug;
	if (nextindex > 0)
	{
		plug = srcplug.elementByPhysicalIndex(0,&status);
		if (!status)
			return false;
	}
	else
	{
		plug = srcplug.elementByLogicalIndex(nextindex,&status);
		if (!status)
			return false;
	}

	MFnDependencyNode jointClusterdepnode(jointCluster);
	MPlug dstplug = jointClusterdepnode.findPlug("matrix",&status);
	if (!status)
		return false;

	MString plugname = plug.name();
	MString dstplugname = dstplug.name();

	const char *debug = plugname.asChar();
	const char *debug0 = dstplugname.asChar();

	status = dgModifier.connect(plug,dstplug);
	if (!status)
		return false;

	//	Create Bone Set
	boneset = CreateBoneSet(bone,vertices);
	if (boneset==MObject::kNullObj)
		return false;

	//	Connect jointCluster.message to boneSet.usedBy
	MPlug srcplug2 = jointClusterdepnode.findPlug("message",&status);
	if (!status)
		return false;

	MFnDependencyNode bonesetdepnode(boneset);

	MPlug dstplug2 = bonesetdepnode.findPlug("usedBy",&status);
	if (!status)
		return false;

	nextindex = dstplug2.numConnectedElements(&status);

	MPlug plug2 = dstplug2.elementByLogicalIndex(nextindex,&status);
	if (!status)
		return false;

	status = dgModifier.connect(srcplug2,plug2);
	dgModifier.doIt();

	MFnDependencyNode newdepnode;

	//	Create ClusterHandle
	MObject jointClusterhandle = dagmodifier.createNode("clusterHandle",bone,&status);
	dagmodifier.doIt();
	if (!status)
		return false;

	MFnDagNode dagnode(jointClusterhandle);
	dagnode.setIntermediateObject(true);

	//	Connect ClusterHandle.clusterTransforms to jointCluster.clusterXForms
	MFnDependencyNode clusterhandledepnode(jointClusterhandle);

	MPlug clusterhandletransform = clusterhandledepnode.findPlug("clusterTransforms",&status);
	if (!status)
		return false;

	nextindex = clusterhandletransform.numConnectedElements(&status);

	MPlug clusterhandletransform2 = clusterhandletransform.elementByLogicalIndex(nextindex,&status);
	if (!status)
		return false;

	MPlug clusterxform = jointClusterdepnode.findPlug("clusterXforms",&status);
	if (!status)
		return false;

	status = dgModifier.connect(clusterhandletransform2,clusterxform);
	dgModifier.doIt();

	//	Connect jointCluster.input.inputGeometry to previous jointCluster.outputGeometry
	if (prevJointCluster != MObject::kNullObj)
	{
		MFnDependencyNode prevjointClusterdepnode(prevJointCluster);

		const char *debug = prevjointClusterdepnode.name().asChar();

		const char *debug0 = jointClusterdepnode.name().asChar();

		MPlug prejointClusteroutputgeom = prevjointClusterdepnode.findPlug("outputGeometry",&status);
		if (!status)
			return false;

		nextindex = prejointClusteroutputgeom.numConnectedElements(&status);

		MPlug prejointClusteroutputgeom2 = prejointClusteroutputgeom.elementByLogicalIndex(nextindex,&status);
		if (!status)
			return false;

		MString prejointClusteroutputgeomname = prejointClusteroutputgeom2.name();
		const char *debug4 = prejointClusteroutputgeomname.asChar();

		MPlug jointClusterinput = jointClusterdepnode.findPlug("input",&status);
		if (!status)
			return false;

		nextindex = jointClusterinput.numConnectedElements(&status);

		MPlug jointClusterinputplug = jointClusterinput.elementByLogicalIndex(nextindex,&status);
		if (!status)
			return false;

		MPlug inputgeometryplug = jointClusterinputplug.child(0,&status);

		MString inputgeometryplugname = inputgeometryplug.name();
		const char *debug3 = inputgeometryplugname.asChar();

		status = dgModifier.connect(prejointClusteroutputgeom2,inputgeometryplug);
		dgModifier.doIt();
		if (!status)
			return false;	
	}

	return true;
}

//	create joint cluster node
MObject JointManager::CreateJointCluster(MObject bone)
{	
	MStatus status;
	MDGModifier dgModifier;

	MFnDependencyNode bonedepnode(bone);

	MFnDependencyNode newdepnode;

	MObject newjointCluster = newdepnode.create("jointCluster",bonedepnode.name() + "Cluster",&status);
	if (!status)
		return MObject::kNullObj;

	return newjointCluster;
}

// create bone set
MObject JointManager::CreateBoneSet(MObject bone,const MSelectionList &vertices)
{
	MStatus status;
	MFnSet mfnboneset;
	MDGModifier dgModifier;

	int numverticesinset = vertices.length();

	MObject bonesetobject = mfnboneset.create(vertices,MFnSet::kVerticesOnly,false,&status);

	if (!status)
		return MObject::kNullObj;

	MFnDependencyNode depnode(bone);

	dgModifier.renameNode(bonesetobject,depnode.name() + MString("Set"));
	dgModifier.doIt();

	return bonesetobject;
}

MObject JointManager::CreateSkinPartition(void)
{
	MStatus status;
	MFnDependencyNode newdepnode;
	MDGModifier dgModifier;

	MObject newskinpartition = newdepnode.create("partition","skinPartition",&status);
	if (!status)
		return MObject::kNullObj;

	return newskinpartition;
}
