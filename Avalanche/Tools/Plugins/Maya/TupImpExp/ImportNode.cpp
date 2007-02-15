////////////////////////////////////////////////////////////////////////////
//
// ImportNode
//
// Tupperware import
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ImportNode.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 9/11/03    Time: 4:22p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * import and export of layers
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/13/03    Time: 4:52p
 * Updated in $/Avalanche/tools/Plugins/Maya/TupImpExp
 * import nodes and polymeshes
 */

#include "TupImpExpPch.h"
#include "TupImport.h"
#include "JointManager.h"

void TupImport::AddNote(MObject object,const TupString &noteString)
{
	MDGModifier dgModifier;
	MStatus status;
	MString command;
	MFnDependencyNode fnNode(object);
	MFnDagNode fnDagNode(object);
	MDagPath mDagPath;
	fnDagNode.getPath(mDagPath);
	TupString pathString = mDagPath.fullPathName().asChar();
	
	MString note(noteString);
	
	const char *debug = note.asChar();
	
	MPlug plug = fnNode.findPlug("notes",&status);
	if (!status)
	{
		command = MString("addAttr -dt \"string\" -ln \"notes\" -sn \"nts\" ") + mDagPath.fullPathName();
		MGlobal::executeCommand(command);
		
		plug = fnNode.findPlug("notes",&status);
		if (!status)
			return;
	}
	
	status = plug.setValue(note);
	if (!status)
		return;
	
	dgModifier.doIt();
}


void TupImport::ImportNodes(void)
{
	TupperwareAggregate *pRootNodeAgg = m_pNodePool->GetNode(m_rootNodeRef);
	assert(pRootNodeAgg);

	MStatus status;
	MItDag dagIterator( MItDag::kBreadthFirst, MFn::kInvalid, &status);
	
	MFnDagNode rootNode(dagIterator.item());
	
	MDagPath rootDagPath;
	status = dagIterator.getPath(rootDagPath);

	TupNode tupRootNode(pRootNodeAgg);
	int numChildren = tupRootNode.GetNumChildNodes();
	for (int childIndex=0;childIndex<numChildren;childIndex++)
	{
		int childNodeRef;
		tupRootNode.GetChildNodeReference(childIndex,childNodeRef);
		ImportNode(childNodeRef,m_rootNodeRef,tupRootNode,rootDagPath);
	}
}

void TupImport::ImportNode(int nodeRef,int parentNodeRef,TupNode &tupParentNode,MDagPath parentDagPath)
{
	TupperwareAggregate *pNodeAgg = m_pNodePool->GetNode(nodeRef);
	TupNode tupNode(pNodeAgg);

	// default object type is a polymesh
	TupObject::ObjectTypes objectType = TupObject::POLY_MESH;

	int objectRef;
	int baseObjectRef;
	tupNode.GetObjectReference(objectRef);
	if (objectRef!=-1)
	{
		baseObjectRef = TupObjectHelper::FindBaseObjectRef(*m_pObjectPool,objectRef);
		assert(baseObjectRef!=-1);
		TupperwareAggregate *pObjectAgg = m_pObjectPool->GetObjectAggregate(baseObjectRef);
		assert(pObjectAgg);
		TupObject tupObject(pObjectAgg);
		objectType = tupObject.GetObjectType();
	}

	TupString nodeName;
	char *pName;
	tupNode.GetName(&pName);
	nodeName = pName;

	MObject transformObject;

	switch (objectType)
	{
		case TupObject::BONE:
			{
				FixBoneName(nodeName);

				// build the joint
				MFnIkJoint mfnJoint;
				transformObject = mfnJoint.create(parentDagPath.node());
			}
			break;
		default:
			{
				MFnTransform transform;
				transformObject = transform.create(parentDagPath.node());
			}
			break;
	}

	MFnDagNode dagNode(transformObject);
	dagNode.setName(MString(nodeName));

	MDagPath dagPath;
	dagNode.getPath(dagPath);

	// if node is part of a skin set the dag path
	m_pJointManager->SetDagPathForBone(nodeRef,dagPath);

	ImportTransform(nodeRef,parentNodeRef,tupNode,tupParentNode,transformObject);

	char *pUserPropertyString;
	tupNode.GetUserProperties(&pUserPropertyString);
	if (pUserPropertyString)
	{
		TupString userPropertyString(pUserPropertyString);
		userPropertyString.Replace("\r\n","\r");
		userPropertyString.Replace("\n","\r");
		AddNote(transformObject,userPropertyString);
	}

	int layerRef;
	tupNode.GetLayerReference(layerRef);
	AddDisplayLayer(dagPath,layerRef);

	int flags;
	tupNode.GetFlags(flags);

	MGlobal::executeCommand (
		MString ("setAttr ") +  dagPath.fullPathName() + MString (".visibility ") +
		MString (" ") + ((flags & TupNode::FLAGS_HIDDEN) ? 0 : 1));

	if (objectRef!=-1)
	{
		ImportObject(objectRef,transformObject);
	}

	int numChildren = tupNode.GetNumChildNodes();
	for (int childIndex=0;childIndex<numChildren;childIndex++)
	{
		int childNodeRef;
		tupNode.GetChildNodeReference(childIndex,childNodeRef);
		ImportNode(childNodeRef,nodeRef,tupNode,dagPath);
	}
	if (objectType==TupObject::BONE)
	{
		AddEndBone(nodeRef,dagPath,baseObjectRef);
	}
}

void TupImport::FixBoneName(TupString &boneName)
{
	// replace # with ____
	boneName.Replace("#","____");
	// replace ! with ___
	boneName.Replace("!","___");
	// replace - with __
	boneName.Replace("-","__");
	// replace space with _
	boneName.Replace(" ","_");
}

void TupImport::AddEndBone(int boneNodeRef,const MDagPath &dagPath,int objectRef)
{
	const float SCALEFACTOR = 100.0f;
	TupperwareAggregate *pBoneObjectAgg = m_pObjectPool->GetObjectAggregate(objectRef);
	TupBone tupBoneNodeObjectBone(pBoneObjectAgg);
	float tupBoneLength;
	tupBoneNodeObjectBone.GetLength(tupBoneLength);
	tupBoneLength*=SCALEFACTOR;

	MMatrix tm = dagPath.inclusiveMatrix();
	Graphics4x4 matrix;
	ConvertMayaBoneMatrix(tm,matrix);
	Vector3 bonePos,xAxis;
	bonePos = matrix.GetTranslation();
	// get the x axis of the joint
	xAxis.Set(matrix.r00,matrix.r01,matrix.r02);
	xAxis.SafeNormalize();

	bool bEndJointFound = false;
	int bestChildIndex = -1;
	float bestChildWeight;
	float bestBoneLength;
	float bestBoneDotProduct;

	int numChildren = dagPath.childCount();
	// see if there is a child that will work as the end bone
	for (int childIndex = 0; childIndex < numChildren; childIndex++)
	{
		MObject childObject = dagPath.child(childIndex);
		MDagPath childPath = dagPath;
		childPath.push(childObject);
		// found a child joint
		if (childPath.apiType()==MFn::kJoint)
		{
			tm = childPath.inclusiveMatrix();
			ConvertMayaBoneMatrix(tm,matrix);
			Vector3 childBonePos(matrix.GetTranslation());
			Vector3 boneVector = childBonePos-bonePos;
			float boneLengthDifference = tupBoneLength - (boneVector.Length()*SCALEFACTOR);
			if (boneLengthDifference<0) boneLengthDifference = -boneLengthDifference;
			boneVector.SafeNormalize();
			float dotProduct = Vector3::Dot(boneVector,xAxis);
			float weight = 1.0f-dotProduct;
			if (weight<0) weight = - weight;
			weight *= boneLengthDifference;

			MFnDagNode dagNode(childPath.node());
			TupString childName(dagNode.name().asChar());
			// if the name does not match keep track of the one that is closest to the z axis
			if (bestChildIndex==-1 || weight<bestChildWeight)
			{
				bestChildIndex = childIndex;
				bestChildWeight = weight;
				bestBoneLength = boneLengthDifference;
				bestBoneDotProduct = dotProduct;
			}
		}
	}
	const float boneLengthTolerance = .1f;
	const float boneDotProductTolerance = .0001f;
	// if no child will work as the end bone
	if (bestChildIndex==-1 ||
		 bestBoneLength>boneLengthTolerance ||
		 bestBoneDotProduct<(1.0f-boneDotProductTolerance) ||
		 bestBoneDotProduct>(1.0f+boneDotProductTolerance))
	{
		MFnIkJoint mfnJoint;
		MFnDagNode boneNode(dagPath.node());
		MObject endBoneObject = mfnJoint.create(dagPath.node());
		MFnDagNode endBoneNode(endBoneObject);

		TupString endBoneName(boneNode.name().asChar());

		endBoneName += "_end";
		endBoneNode.setName(MString(endBoneName));

		MMatrix myMatrix;
		myMatrix.matrix[3][2] = tupBoneLength;

		MTransformationMatrix localMatrix(myMatrix);

		mfnJoint.set(localMatrix);

		int skinIndex;
		int boneInSkinIndex;
		if (m_pJointManager->BoneInSkin(boneNodeRef,skinIndex,boneInSkinIndex))
		{
			MDagPath endBonePath(dagPath);
			endBonePath.push(endBoneObject);
			JointManager::BoneConnections boneConnections;
			boneConnections.m_boneDagPath = endBonePath;
			boneConnections.m_boneLength = 1.0f;
			boneConnections.m_boneNodeRef = -1;
			MMatrix myTM = endBonePath.inclusiveMatrix();
			ConvertMayaBoneMatrix(myTM,boneConnections.m_boneBindingPoseMatrix);

			m_pJointManager->AddEndBoneToSkin(skinIndex,boneConnections);
		}
	}
}

void TupImport::ConvertMayaBoneMatrix(const MMatrix &srcMatrix,Graphics4x4 &dstMatrix)
{
	const float SCALEFACTOR = .01f;

	const float *pSrcMatrix = (const float *)&srcMatrix;

	dstMatrix.r00 = (float)srcMatrix.matrix[0][0];
	dstMatrix.r01 = (float)srcMatrix.matrix[0][1];
	dstMatrix.r02 = (float)srcMatrix.matrix[0][2];

	dstMatrix.r10 = (float)srcMatrix.matrix[1][0];
	dstMatrix.r11 = (float)srcMatrix.matrix[1][1];
	dstMatrix.r12 = (float)srcMatrix.matrix[1][2];

	dstMatrix.r20 = (float)srcMatrix.matrix[2][0];
	dstMatrix.r21 = (float)srcMatrix.matrix[2][1];
	dstMatrix.r22 = (float)srcMatrix.matrix[2][2];

	dstMatrix.tx  = (float)srcMatrix.matrix[3][0]*SCALEFACTOR;
	dstMatrix.ty  = (float)srcMatrix.matrix[3][1]*SCALEFACTOR;
	dstMatrix.tz  = (float)srcMatrix.matrix[3][2]*SCALEFACTOR;

	dstMatrix.u0  = dstMatrix.u1 = dstMatrix.u2 = 0.0f;
	dstMatrix.w = 1.0f;

#ifdef MAKE_Z_UP
	Graphics4x4 matrixYtoZ,matrixZtoY;

	matrixYtoZ.SetXRotation(-Math::Pi/2.0f);
	matrixZtoY.SetXRotation(Math::Pi/2.0f);

	// start with local(yup)->parent(yup) matrix
	// get local(yup)->parent(zup) matrix
	dstMatrix = matrixYtoZ*dstMatrix; 
	// get local(yup)->parent(yup) matrix
	dstMatrix *= matrixZtoY;

	Graphics4x4 rotateZ;
	rotateZ.SetZRotation(-Math::Pi/2.0f);
	dstMatrix = rotateZ * dstMatrix;
#else
	Graphics4x4 rotateY;
	rotateY.SetYRotation(-Math::Pi/2.0f);
	dstMatrix = rotateY * dstMatrix;
#endif
}
