////////////////////////////////////////////////////////////////////////////
//
// ExportBones
//
// Deal with the bones within a mesh
//
////////////////////////////////////////////////////////////////////////////

#include "ExportPch.h"
#include "ExportBones.h"
#include "BoneGroupTable.h"

// BoneHolder methods
BoneHolder::BoneHolder(void)
{
	m_boneToModelMatrix.Identity();
	m_boneToStandardMatrix.Identity();
	m_firstChildIndex = -1;
	m_nextSiblingIndex = -1;
	m_parentIndex = -1;
	m_index = -1;
	m_groupIndex = -1;
	m_nextBoneInGroupIndex = -1;
	m_pivotPoint.Set(0.0f,0.0f,0.0f);
	m_boneEndPoint.Set(0.0f,0.0f,1.0f);
	m_boneLength = 1.0f;
	m_name = "Unused";
	m_boneRef = -1;
	m_simpleIndex = -1;
	m_boundingBox.Min.Set(MAXFLOAT32,MAXFLOAT32,MAXFLOAT32);
	m_boundingBox.Max.Set(MINFLOAT32,MINFLOAT32,MINFLOAT32);
	m_bOrientXYZ = false;
	m_orientLikeValue = 0;
	m_orient2LikeValue = 0;
	m_rollLikeValue = 0;
	m_orientBone = -1;
	m_orient2Bone = -1;
	m_numAssignedVerts = 0;
	m_scaleLikeValue = 0;
	m_bUnscaleBone = false;
};


BoneBuilder::BoneBuilder(TupTriMesh &tupTriMesh,TupNodePool &tupNodePool,TupObjectPool &tupObjectPool,BoneGroupTable &boneGroupTable,int tupRootNodeRef) : 
	m_tupTriMesh(tupTriMesh),
	m_tupNodePool(tupNodePool),
	m_tupObjectPool(tupObjectPool),
	m_boneGroupTable(boneGroupTable),
	m_tupRootNodeRef(tupRootNodeRef),
	m_bAddedRootBone(false),
	m_bRemappedVertsToMaster(false),
	m_errorCount(0)
{
	TupperwareAggregate *pMeshAgg = m_tupTriMesh.GetBaseAggregate();
	// if the model name has been added (from the attributes in the annotate filter) is has highest priority
	TupperwareScalar *pModelNameScalar = pMeshAgg->FindScalarByKey("Annotate_ModelName");
	if (pModelNameScalar)
	{
		m_modelName = pModelNameScalar->GetAsString();
	}
	else // if modelname is not there then look to see the maya object name (saved only from Maya)
	{
		TupperwareScalar *pMayaObjectNameScalar = pMeshAgg->FindScalarByKey("MayaObjectName");
		if (pMayaObjectNameScalar)
		{
			m_modelName = pMayaObjectNameScalar->GetAsString();
			int foundIndex =  m_modelName.ReverseFind('|');
			// if we find a | then get the characters to the right
			if (foundIndex!=-1)
			{
				TupString tempName = m_modelName.Mid(foundIndex+1);
				m_modelName = tempName;
			}
		}
		else
		{
			// default to None if not on the mesh
			m_modelName = "None";
		}
	}

	TupperwareAggregate *pBoneMeshAgg = pMeshAgg->FindAggregateByKey("Annotate_BoneMeshData");
	// if there is no bones on this mesh then kick out
	if (!pBoneMeshAgg)
		return;

	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Model %s bone information",(const char *)m_modelName);
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Index GroupID NumVerts Name");

	// go through the bones and grab what we need and add to the simple array
	int numBones = pBoneMeshAgg->FindScalarByKey("NumBones")->GetAsInt();
	for (int boneIndex = 0;boneIndex<numBones;boneIndex++)
	{
		BoneSimple simpleEntry;

		char boneString[40];
		itoa(boneIndex,boneString,10);
		TupperwareAggregate *pBoneAgg = pBoneMeshAgg->FindAggregateByKeyAndName("Bone",boneString);
		assert(pBoneAgg);
		simpleEntry.m_boneRef = pBoneAgg->FindScalarByKey("BoneRef")->GetAsInt();
		simpleEntry.m_boneLength = pBoneAgg->FindScalarByKey("BoneLength")->GetAsFloat();
		TupperwareList *pBoneTMList = pBoneAgg->FindListByKey("BoneModelTM");
		assert(pBoneTMList->GetLength()==16);
		simpleEntry.m_boneTM = *(Graphics4x4 *)pBoneTMList->GetAsFloat();
		m_simpleBoneArray.Add(simpleEntry);

		if (!HasParentBone(simpleEntry.m_boneRef))
		{
			m_parentBoneRefs.Add(simpleEntry.m_boneRef);
		}
	}
	// get which bones the vertices use
	pBoneMeshAgg->FindListByKey("MeshBoneVerts")->GetAsInt(m_boneVerts);
	// go through the bones and add all other bones that are in the node tree
	Keeper<int> boneKeeper;

	int numParentBones = m_parentBoneRefs.GetSize();
	// if we have more than one parent
	if (numParentBones!=1)
	{
		m_bAddedRootBone = true; // use later
		boneKeeper.Add(m_tupRootNodeRef); // make a new root bone
	}
	// add the parent bones
	for (boneIndex=0;boneIndex<numParentBones;boneIndex++)
	{
		boneKeeper.Add(m_parentBoneRefs[boneIndex]);
	}

	// add the other bones
	for (boneIndex=0;boneIndex<numBones;boneIndex++)
	{
		// add my bone
		boneKeeper.Add(m_simpleBoneArray[boneIndex].m_boneRef);
	}

	// TODO change the order of the bones in the keeper

	int numKeeperBones = boneKeeper.GetSize();
	m_boneArray.SetSize(numKeeperBones);

	// set up the relationships between the bones
	for (boneIndex=0;boneIndex<numKeeperBones;boneIndex++)
	{
		HookUpBone(boneKeeper,boneIndex);
	}
	// set up the groups that the bones belong to
	for (boneIndex=0;boneIndex<numKeeperBones;boneIndex++)
	{
		FixBoneName(m_boneArray[boneIndex].m_name);
		const char *boneName = m_boneArray[boneIndex].m_name;
		int invalidBoneGroup = m_boneGroupTable.IsValidModelChannel(boneName);
		int boneGroup = m_boneGroupTable.GetBoneGroup(boneName);
		// if an invalid model channel or can't find in bone group table
		if (invalidBoneGroup!=0 || boneGroup==-1) 
		{
			boneGroup = invalidBoneGroup; // ignore bone group and use this one (0 if not invalid)
		}
		m_boneArray[boneIndex].m_groupIndex = boneGroup;
		if (boneGroup==-1)
		{
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
				"Warning - Invalid bone group of -1 on bone %s",m_boneArray[boneIndex].m_name);
		}
	}
	// set up the next bone group indices
	for (boneIndex=0;boneIndex<numKeeperBones;boneIndex++)
	{
		int groupIndex = m_boneArray[boneIndex].m_groupIndex;
		// find the next one in the list
		for (int otherBoneIndex=boneIndex+1;otherBoneIndex<numKeeperBones;otherBoneIndex++)
		{
			// if we found one then mark it as the next for the bone
			if (m_boneArray[otherBoneIndex].m_groupIndex == groupIndex)
			{
				m_boneArray[boneIndex].m_nextBoneInGroupIndex = otherBoneIndex;
				break; // continue checking the other bones
			}
		}
	}
	// go through and set up the simple bones to reference the holder array
	// as well as set up some stuff
	int numSimpleBones = m_simpleBoneArray.GetSize();
	for (boneIndex = 0;boneIndex<numSimpleBones;boneIndex++)
	{
		BoneSimple &boneSimple = m_simpleBoneArray[boneIndex];
		// get which bone this is within the holder (and keeper)
		int boneKeeperIndex = boneKeeper.GetIndex(boneSimple.m_boneRef);
		assert(boneKeeperIndex!=-1);
		boneSimple.m_holderIndex = boneKeeperIndex;

		BoneHolder &boneHolder = m_boneArray[boneKeeperIndex];
		boneHolder.m_simpleIndex = boneIndex; // index into simple array

		// Build a bone to model matrix that keeps the z-direction and only
		// translates and rotates. Start by getting the unit z-axis after
		// rotating +90 degrees in y.
		Vector3 zAxis = boneSimple.m_boneTM.GetRow0();
		zAxis.Normalize();

		// The new x-axis is perpendicular to the z-axis and the old y-axis.
		Vector3 xAxis = boneSimple.m_boneTM.GetRow1().Cross(zAxis).Normalized();

		// Fill in the matrix.
		boneHolder.m_boneToModelMatrix.SetTranslation(boneSimple.m_boneTM.GetTranslation());
		boneHolder.m_boneToModelMatrix.SetRow0(xAxis);
		boneHolder.m_boneToModelMatrix.SetRow1(zAxis.Cross(xAxis));
		boneHolder.m_boneToModelMatrix.SetRow2(zAxis);

		// set the bone length
		boneHolder.m_boneLength = boneSimple.m_boneLength;
	}
	// set up the bounding boxes of the bones
	Vector3 *positionVerts;
	int numVerts;
	m_tupTriMesh.GetPositionVerts((float **)&positionVerts,numVerts);
	assert(m_boneVerts.GetSize() == numVerts);
	for (int vertIndex=0;vertIndex<numVerts;vertIndex++)
	{
		int boneIndex = m_boneVerts[vertIndex];
		if (boneIndex!=-1)
		{
			int keeperIndex = m_simpleBoneArray[boneIndex].m_holderIndex;
			assert(keeperIndex!=-1);
			BoneHolder &boneHolder = m_boneArray[keeperIndex];
			// transform model vert into bone space
			Vector3 transFormedVert = positionVerts[vertIndex] / boneHolder.m_boneToModelMatrix;
			Bounding::AddToBoundingBox(boneHolder.m_boundingBox.Min,boneHolder.m_boundingBox.Max,transFormedVert);
		}
	}
	// reset any bounding boxes that were not set to 0
	for (boneIndex = 0;boneIndex<numKeeperBones;boneIndex++)
	{
		if (m_boneArray[boneIndex].m_boundingBox.Min.x() == MAXFLOAT32)
		{
			m_boneArray[boneIndex].m_boundingBox.Min.Set(0.0f,0.0f,0.0f);
			m_boneArray[boneIndex].m_boundingBox.Max.Set(0.0f,0.0f,0.0f);
		}
	}
	// remap the bone verts from a simple index to a holder index
	for (vertIndex=0;vertIndex<numVerts;vertIndex++)
	{
		// this gives us the correct bone # for the builder
		int boneIndex = m_boneVerts[vertIndex];
		if (boneIndex!=-1)
		{
			int keeperIndex = m_simpleBoneArray[boneIndex].m_holderIndex;
			assert(keeperIndex!=-1);
			BoneHolder &boneHolder = m_boneArray[keeperIndex];
			boneHolder.m_numAssignedVerts++;
			m_boneVerts[vertIndex] = keeperIndex;
		}
		else
		{
			m_boneVerts[vertIndex] = 0; // set to root one if not pointing to any
		}
	}

	// set up the orient like data
	for (boneIndex = 0;boneIndex<numKeeperBones;boneIndex++)
	{
		HandleOrientLike(boneIndex);
	}
	if (m_errorCount)
	{
		throw(PluginException(ERROR_FATAL,"Error processing orient like bones - process stopped"));
	}

	// set up the bone pivot end and length
	for (boneIndex = 0;boneIndex<numKeeperBones;boneIndex++)
	{
		HandleBonePivots(boneIndex);
	}
	// set up the bone standard matricies
	for (boneIndex = 0;boneIndex<numKeeperBones;boneIndex++)
	{
		HandleBoneStandardMatrix(boneIndex);
	}
	// save bone stuff to log 
	AddBoneInfoToLog(0,0);
	int totalVerts = 0;
	for (boneIndex = 0;boneIndex<numKeeperBones;boneIndex++)
	{
		BoneHolder &boneHolder = m_boneArray[boneIndex];
		totalVerts += boneHolder.m_numAssignedVerts;
	}
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Total Bones: %d\r\nTotal vertices assigned to bones: %d",numKeeperBones,totalVerts);
}

// return true if has a parent bone
bool BoneBuilder::HasParentBone(int boneRef)
{
	TupperwareAggregate *pNodeAgg = m_tupNodePool.GetNode(boneRef);
	TupNode tupNode(pNodeAgg);
	int parentRef;
	tupNode.GetParentNodeReference(parentRef);
	// if my parent is the root of the scene
	if (parentRef==m_tupRootNodeRef)
		return false;

	TupperwareAggregate *pParentNodeAgg = m_tupNodePool.GetNode(parentRef);
	TupNode tupParentNode(pParentNodeAgg);
	int parentObjectRef;
	tupParentNode.GetObjectReference(parentObjectRef);
	// if parent has no object
	if (parentObjectRef==-1)
		return false;
	TupperwareAggregate *pParentObjectAgg = m_tupObjectPool.GetObjectAggregate(parentObjectRef);
	TupObject tupObject(pParentObjectAgg);
	return (tupObject.GetObjectType()==TupObject::BONE); // return true if a bone false otherwise
}

void BoneBuilder::HookUpBone(Keeper<int> &boneKeeper,int boneKeeperIndex)
{
	BoneHolder &boneHolder = m_boneArray[boneKeeperIndex];

	boneHolder.m_index = boneKeeperIndex;

	int boneRef = boneKeeper[boneKeeperIndex];
	TupperwareAggregate *pNodeAgg = m_tupNodePool.GetNode(boneRef);
	assert(pNodeAgg);
	TupNode tupNode(pNodeAgg);
	char *pName;
	tupNode.GetName(&pName);
	boneHolder.m_name = pName;
	boneHolder.m_boneRef = boneRef;

	// we added a root bone as we are on this bone
	if (m_bAddedRootBone && boneKeeperIndex==0)
	{
		boneHolder.m_parentIndex = -1; // no parent
		boneHolder.m_name = m_modelName; // use the model name

		// do my children (currently the refs of the parent bones)
		int numChildren = m_parentBoneRefs.GetSize();
		int nextChildKeeperIndex = -1; // last one gets this
		for (int childIndex=numChildren-1;childIndex>=0;childIndex--)
		{
			int childRef = m_parentBoneRefs[childIndex];
			assert(childRef!=-1);
			int childKeeperIndex = boneKeeper.GetIndex(childRef);
			assert(childKeeperIndex!=-1); // must be in the keeper
			BoneHolder &childBoneHolder = m_boneArray[childKeeperIndex];
			// my sibling was the last one done
			childBoneHolder.m_nextSiblingIndex = nextChildKeeperIndex;
			HookUpBone(boneKeeper,childKeeperIndex);
			nextChildKeeperIndex = childKeeperIndex;
		}
		// the first child gets its index put on the parent
		boneHolder.m_firstChildIndex = nextChildKeeperIndex;
	}
	else // did not add in a root bone
	{
		if (boneKeeperIndex == 0) // this is the root bone that was there
		{
			boneHolder.m_parentIndex = -1; // no parent
		}
		else
		{
			int parentRef;
			tupNode.GetParentNodeReference(parentRef);
			assert(parentRef!=-1);
			int parentKeeperIndex = boneKeeper.GetIndex(parentRef);
			// if parent is not in the keeper then point to 0
			if (parentKeeperIndex==-1)
				boneHolder.m_parentIndex = 0;
			else
				boneHolder.m_parentIndex = parentKeeperIndex;
		}
		// do my children
		int numChildren = tupNode.GetNumChildNodes();
		int nextChildKeeperIndex = -1; // last one gets this
		for (int childIndex=numChildren-1;childIndex>=0;childIndex--)
		{
			int childRef;
			tupNode.GetChildNodeReference(childIndex,childRef);
			assert(childRef!=-1);
			int childKeeperIndex = boneKeeper.GetIndex(childRef);
			if (childKeeperIndex!=-1) // must be in the keeper
			{
				BoneHolder &childBoneHolder = m_boneArray[childKeeperIndex];
				// my sibling was the last one done
				childBoneHolder.m_nextSiblingIndex = nextChildKeeperIndex;
				HookUpBone(boneKeeper,childKeeperIndex);
				nextChildKeeperIndex = childKeeperIndex;
			}
		}
		// the first child gets its index put on the parent
		boneHolder.m_firstChildIndex = nextChildKeeperIndex;
	}
}

void BoneBuilder::FixBoneName(TupString &boneName)
{
	boneName.MakeLower();
	TupString startName = boneName;
	TupString fixName = boneName;
	// try to get rid of stuff that causes trouble with the name parser
	fixName.TrimLeft();
	fixName.TrimRight();
	fixName.Replace("    "," ");
	fixName.Replace("   "," ");
	fixName.Replace("  "," ");
	fixName.Replace("- ","-");
	fixName.Replace(" -","-");
	if (startName!=fixName)
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
			"Warning - bone \"%s\" was changed to \"%s\"",(const char *)startName,(const char *)fixName);
	}
	boneName = fixName;
}


// function to strip the -100-100... from the end of the bone name
// and return the index of the bone found
int BoneBuilder::GetBoneIndex(const char *pParentBoneName,const char *pMainBoneName) 
{
	if (pParentBoneName == NULL)
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
			"Warning - parent bone name not specified with %s",pMainBoneName);
		m_errorCount++;
		return -1;
	}

	TupString boneName = pParentBoneName;
	if (boneName.GetLength()==0)
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
			"Warning - parent bone name not specified with %s",pMainBoneName);
		m_errorCount++;
		return -1;
	}

	int firstDash = boneName.Find('-');
	if (firstDash!=-1)
	{
		boneName = boneName.Left(firstDash);
	}
	int boneIndex = FindBone(boneName);
	if (boneIndex==-1)
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
			"Warning - cannot find parent bone name %s from %s",(const char *)boneName,pMainBoneName);
		m_errorCount++;
	}
	return boneIndex;
}

// given the parent bone name extract the percent (# based on percent index)
// and return the value
unsigned char BoneBuilder::GetPercent(const char *pParentBoneName,int percentIndex,const char *pMainBoneName)
{
	if (pParentBoneName == NULL)
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
			"Warning - parent bone name not specified with %s",pMainBoneName);
		m_errorCount++;
		return 0;
	}

	TupString boneName = pParentBoneName;
	if (boneName.GetLength()==0)
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
			"Warning - parent bone name not specified with %s",pMainBoneName);
		m_errorCount++;
		return 0;
	}

	int firstDash = boneName.Find('-');
	if (firstDash==-1)
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
			"Warning - no percents specified with %s",pMainBoneName);
		m_errorCount++;
		return 0;
	}

	// start with first percent
	boneName = boneName.Mid(firstDash+1);
	int index = percentIndex;
	while(index)
	{
		int nextDash = boneName.Find('-');
		if (nextDash==-1)
		{
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
				"Warning - cannot find percent #%d in %s",percentIndex+1,pMainBoneName);
			m_errorCount++;
			return 0;
		}
		boneName = boneName.Mid(nextDash+1);
		index--;
	}
	// strip off other characters so all we have is the percent
	int lastDash = boneName.Find('-');
	if (lastDash!=-1)
	{
		boneName = boneName.Left(lastDash);
	}
	int percent = atoi(boneName);
	if (percent<0||percent>100)
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_RED|LogFlags::FONT_STYLE_BOLD,
			"Error - Invalid percent %d value %s in %s Value needs to be >=0 and <=100",percent,percentIndex+1,pMainBoneName);
		m_errorCount++;
		return 0;
	}
	return (unsigned char)percent;
}

void BoneBuilder::HandleOrientLike(int boneIndex)
{
	BoneHolder &boneHolder = m_boneArray[boneIndex];
	TupString boneName = boneHolder.m_name;

	char cBoneType[256], cBoneName1[256], cBoneName2[256];
	int numScan;

	cBoneName1[0] = '\0';
	cBoneName2[0] = '\0';

	// handle the following types:
	// ol bonename-100 [bonename-100] (orient like another bone or two)
	// rl bonename-100 (roll like another bone)
	// olxyz bonename-100-100-100 (orient like another bone but provide % for x,y,z)
	// sl bonename-100 (scale like another bone)
	// olsl bonename-50-100 (orient and scale like another bone) (first is orient like, second is scale like) 
	// slol bonename-50-100 (scale and orient like another bone) (first is scale like, second is orient like)
	// rlsl bonename-50-100 (roll and scale like another bone) (first is roll like, second is scale like)
	// slrl bonename-50-100 (scale and roll like another bone) (first is scale like, second is roll like)
	// olxyzsl bonename-100-100-100-100 (orient like another bone but provide % for x,y,z and scale like (last one))
	// slolxyz bonename-100-100-100-100 (orient like another bone but provide % for x,y,z and scale like (first one))
	// unscale bonename (undo the scale from another bone)
	numScan = sscanf(boneName, "%s%s%s", cBoneType, cBoneName1, cBoneName2);

	if( numScan < 1 )
	{
		// bone name needs to be at least one string
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_RED|LogFlags::FONT_STYLE_BOLD,
			"Error - Invalid Bone Name \"%s\"",(const char *)boneName);
		m_errorCount++;
		return;
	}

	// ol bonename-100 [bonename-100] (orient like another bone or two)
	if (_strcmpi(cBoneType,"ol")==0)
	{
		boneHolder.m_orientBone = GetBoneIndex(cBoneName1,boneName);
		boneHolder.m_orientLikeValue = GetPercent(cBoneName1,0,boneName);
		if (numScan > 2)
		{
			boneHolder.m_orient2Bone = GetBoneIndex(cBoneName2,boneName);
			boneHolder.m_orient2LikeValue = GetPercent(cBoneName2,0,boneName);
		}
	}
	// rl bonename-100 (roll like another bone)
	else if (_strcmpi(cBoneType,"rl")==0)
	{
		boneHolder.m_orientBone = GetBoneIndex(cBoneName1,boneName);
		boneHolder.m_rollLikeValue = GetPercent(cBoneName1,0,boneName);
	}
	// olxyz bonename-100-100-100 (orient like another bone but provide % for x,y,z)
	else if (_strcmpi(cBoneType,"olxyz")==0)
	{
		boneHolder.m_bOrientXYZ = true;
		boneHolder.m_orientBone = GetBoneIndex(cBoneName1,boneName);
		boneHolder.m_orientX = GetPercent(cBoneName1,0,boneName);
		boneHolder.m_orientY = GetPercent(cBoneName1,1,boneName);
		boneHolder.m_orientZ = GetPercent(cBoneName1,2,boneName);
	}
	// sl bonename-100 (scale like another bone)
	else if (_strcmpi(cBoneType,"sl")==0)
	{
		boneHolder.m_orientBone = GetBoneIndex(cBoneName1,boneName);
		boneHolder.m_scaleLikeValue = GetPercent(cBoneName1,0,boneName);
	}
	// olsl bonename-50-100 (orient and scale like another bone) (first is orient like, second is scale like) 
	else if (_strcmpi(cBoneType,"olsl")==0)
	{
		boneHolder.m_orientBone = GetBoneIndex(cBoneName1,boneName);
		boneHolder.m_orientLikeValue = GetPercent(cBoneName1,0,boneName);
		boneHolder.m_scaleLikeValue = GetPercent(cBoneName1,1,boneName);
	}
	// slol bonename-50-100 (scale and orient like another bone) (first is scale like, second is orient like)
	else if (_strcmpi(cBoneType,"slol")==0)
	{
		boneHolder.m_orientBone = GetBoneIndex(cBoneName1,boneName);
		boneHolder.m_scaleLikeValue = GetPercent(cBoneName1,0,boneName);
		boneHolder.m_orientLikeValue = GetPercent(cBoneName1,1,boneName);
	}
	// rlsl bonename-50-100 (roll and scale like another bone) (first is roll like, second is scale like)
	else if (_strcmpi(cBoneType,"rlsl")==0)
	{
		boneHolder.m_orientBone = GetBoneIndex(cBoneName1,boneName);
		boneHolder.m_rollLikeValue = GetPercent(cBoneName1,0,boneName);
		boneHolder.m_scaleLikeValue = GetPercent(cBoneName1,1,boneName);
	}
	// slrl bonename-50-100 (scale and roll like another bone) (first is scale like, second is roll like)
	else if (_strcmpi(cBoneType,"slrl")==0)
	{
		boneHolder.m_orientBone = GetBoneIndex(cBoneName1,boneName);
		boneHolder.m_scaleLikeValue = GetPercent(cBoneName1,0,boneName);
		boneHolder.m_rollLikeValue = GetPercent(cBoneName1,1,boneName);
	}
	// olxyzsl bonename-100-100-100-100 (orient like another bone but provide % for x,y,z and scale like (last one))
	else if (_strcmpi(cBoneType,"olxyzsl")==0)
	{
		boneHolder.m_bOrientXYZ = true;
		boneHolder.m_orientBone = GetBoneIndex(cBoneName1,boneName);
		boneHolder.m_orientX = GetPercent(cBoneName1,0,boneName);
		boneHolder.m_orientY = GetPercent(cBoneName1,1,boneName);
		boneHolder.m_orientZ = GetPercent(cBoneName1,2,boneName);
		boneHolder.m_scaleLikeValue = GetPercent(cBoneName1,3,boneName);
	}
	// slolxyz bonename-100-100-100-100 (orient like another bone but provide % for x,y,z and scale like (first one))
	else if (_strcmpi(cBoneType,"slolxyz")==0)
	{
		boneHolder.m_bOrientXYZ = true;
		boneHolder.m_orientBone = GetBoneIndex(cBoneName1,boneName);
		boneHolder.m_scaleLikeValue = GetPercent(cBoneName1,0,boneName);
		boneHolder.m_orientX = GetPercent(cBoneName1,1,boneName);
		boneHolder.m_orientY = GetPercent(cBoneName1,2,boneName);
		boneHolder.m_orientZ = GetPercent(cBoneName1,3,boneName);
	}
	// unscale bonename (undo the scale from another bone)
	else if (_strcmpi(cBoneType,"unscale")==0)
	{
		boneHolder.m_bUnscaleBone = true;
		boneHolder.m_orientBone = GetBoneIndex(cBoneName1,boneName);
	}
}

void BoneBuilder::HandleBonePivots(int boneIndex)
{
	BoneHolder &boneHolder = m_boneArray[boneIndex];

	boneHolder.m_pivotPoint = boneHolder.m_boneToModelMatrix.Row(3); // translation is the pivot

	// end point = pivot + (z-axis * length)
	boneHolder.m_boneEndPoint = boneHolder.m_pivotPoint + (boneHolder.m_boneToModelMatrix.Row(2) * boneHolder.m_boneLength);
}

void BoneBuilder::HandleBoneStandardMatrix(int boneIndex)
{
	BoneHolder &boneHolder = m_boneArray[boneIndex];
	Vector3 rollPosition = boneHolder.m_pivotPoint + boneHolder.m_boneToModelMatrix.Row(1);
	Vector3 ZAxis;
	ZAxis.Set(0.0f,0.0f,1.0f);
	boneHolder.m_boneToStandardMatrix = Rotate2V(boneHolder.m_boneEndPoint-boneHolder.m_pivotPoint,ZAxis);

	Vector3 rollVector = rollPosition-boneHolder.m_pivotPoint;
	rollVector = rollVector * boneHolder.m_boneToStandardMatrix;
	float roll = (float)Math::ArcTan2( -rollVector.x( ), rollVector.y( ) );
	if ( roll )
	{
		Graphics4x4 matrixRotZ;
		matrixRotZ.SetZRotation(-roll);
		boneHolder.m_boneToStandardMatrix = boneHolder.m_boneToStandardMatrix * matrixRotZ;
	}
	Vector3 translation = (-boneHolder.m_pivotPoint) * boneHolder.m_boneToStandardMatrix;

	boneHolder.m_boneToStandardMatrix.PokeTranslation(translation);
}

void BoneBuilder::AddBoneInfoToLog(int boneIndex,int indentLevel)
{
	BoneHolder &boneHolder = m_boneArray[boneIndex];
	TupString indentString;
	for (int indentIndex=1;indentIndex<indentLevel;indentIndex++)
	{
		indentString += "| ";
	}
	if (indentLevel!=0)
		indentString += "|-";
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW," %3d     %3d     %4d  %s%s",
						boneIndex,boneHolder.m_groupIndex,boneHolder.m_numAssignedVerts,(const char *)indentString,(const char *)boneHolder.m_name);

	// do my children
	if (boneHolder.m_firstChildIndex!=-1)
	{
		AddBoneInfoToLog(boneHolder.m_firstChildIndex,indentLevel+1);
	}
	// do my siblings
	if (boneHolder.m_nextSiblingIndex!=-1)
	{
		AddBoneInfoToLog(boneHolder.m_nextSiblingIndex,indentLevel);
	}
}


Graphics4x4 BoneBuilder::Rotate2V(const Vector3 &from,const Vector3 &to)
{
	Graphics4x4 matrix;
	float numerator = Vector3::Dot(from,to);
	float denominator = from.Length() * to.Length();
	float radangle;
	Vector3 axis,xAxis,yAxis;
	xAxis.Set(1.0f,0.0f,0.0f);
	yAxis.Set(0.0f,1.0f,0.0f);

	if ( Math::Abs(numerator - denominator) < .00001) // SAME DIR
	{
		matrix.Identity();
		return matrix;
	}
	if ( Math::Abs(numerator + denominator) < .00001 )  // OPPOSITE DIR
	{
		radangle = 3.141519f;
		if (Math::Abs(from.Normalized().x( ) ) != 1.0f ||
			Math::Abs(from.Normalized().y( ) ) != 0.0f ||
			Math::Abs(from.Normalized().z( ) ) != 0.0f)
		{
			axis = Vector3::Cross(from,xAxis);
		}
		else
		{
			axis = Vector3::Cross(from,yAxis);
		}
	}
	else 
	{
		float ratio = numerator / denominator;
		if (ratio >= 1.0f)
		{
			matrix.Identity();
			return matrix;
		}
		radangle = Math::ArcCos( ratio );
		axis = Vector3::Cross(from,to);
	}
	return RotateVRadians( radangle, axis );
}

Graphics4x4 BoneBuilder::RotateVRadians( float anglerad, const Vector3 &axis )
{
	float c = Math::Cos(anglerad);
	float s = Math::Sin(anglerad);
	float t = 1.0f - c;
	Vector3 v = axis;
         
	v.Normalize();
#if 1
	return Graphics4x4(t*v.x( )*v.x( )+c,		t*v.x( )*v.y( )+s*v.z( ),	t*v.x( )*v.z( )-s*v.y( ),
					   t*v.x( )*v.y( )-s*v.z( ), t*v.y( )*v.y( )+c,		t*v.y( )*v.z( )+s*v.x( ),
					   t*v.x( )*v.z( )+s*v.y( ),	t*v.y( )*v.z( )-s*v.x( ),	t*v.z( )*v.z( )+c,
					   0.0f,0.0f,0.0f);
#else
	return Graphics4x4(t*v.x(  *v.x( )+c,		t*v.x( )*v.y( )-s*v.z( ),	t*v.x( )*v.z( )+s*v.y( ),
					   t*v.x( )*v.y( )+s*v.z( ), t*v.y( )*v.y( )+c,		t*v.y( )*v.z( )-s*v.x( ),
					   t*v.x( )*v.z( )-s*v.y( ),	t*v.y( )*v.z( )+s*v.x( ),	t*v.z( )*v.z( )+c,
					   0.0f,0.0f,0.0f);
#endif
}


int BoneBuilder::FindBone(const char *pBoneName)
{
	TupString boneName(pBoneName);
	boneName.MakeLower();
	int numBones = m_boneArray.GetSize();
	for (int boneIndex=0;boneIndex<numBones;boneIndex++)
	{
		if (m_boneArray[boneIndex].m_name == boneName)
			return boneIndex;
	}
	return -1;
}


int BoneBuilder::GetNumBones(void) const
{
	return m_boneArray.GetSize();
}

void BoneBuilder::FillBoneStruct(int boneIndex,ts_ModelBone &boneStruct) const
{
	const BoneHolder &boneHolder = m_boneArray[boneIndex];
	memset(&boneStruct,0,sizeof(boneStruct));

	boneStruct.boneToModel = boneHolder.m_boneToModelMatrix;
	boneStruct.m_bonetostandard = boneHolder.m_boneToStandardMatrix;
	// set index only when index is !=-1 (default is NULL)
	if (boneHolder.m_firstChildIndex!=-1)
		boneStruct.pChildren = (ts_ModelBone *)boneHolder.m_firstChildIndex;
	// set index only when index is !=-1 (default is NULL)
	if (boneHolder.m_nextSiblingIndex!=-1)
		boneStruct.pSiblings = (ts_ModelBone *)boneHolder.m_nextSiblingIndex;
	boneStruct.vPivot = boneHolder.m_pivotPoint;
	boneStruct.vBoneEnd = boneHolder.m_boneEndPoint;
	boneStruct.fBoneLen = boneHolder.m_boneLength;
	strncpy(boneStruct.cBoneName,boneHolder.m_name,MAX_BONENAME_LEN);
	boneStruct.cBoneName[MAX_BONENAME_LEN-1] = '\0';
	// set index only when index is !=-1 (default is NULL)
	if (boneHolder.m_nextBoneInGroupIndex!=-1)
		boneStruct.pNextBoneInGroup = (ts_ModelBone *)boneHolder.m_nextBoneInGroupIndex;
	// set index only when index is !=-1 (default is NULL)
	if (boneHolder.m_parentIndex!=-1)
		boneStruct.m_parent = (ts_ModelBone *)boneHolder.m_parentIndex;

	boneStruct.boneGroupID = boneHolder.m_groupIndex;
	// if the first one put the total in the ID
	if (boneIndex==0)
		boneStruct.boneID = m_boneArray.GetSize();
	else
		boneStruct.boneID = boneHolder.m_index;
	if (boneHolder.m_bOrientXYZ)
	{
		boneStruct.bOrientXYZ = TRUE;
		boneStruct.ucOrientX = boneHolder.m_orientX;
		boneStruct.ucOrientY = boneHolder.m_orientY;
		boneStruct.ucOrientZ = boneHolder.m_orientZ;
	}
	else
	{
		boneStruct.bOrientXYZ = FALSE;
		boneStruct.ucOrientLikeValue = boneHolder.m_orientLikeValue;
		boneStruct.ucOrient2LikeValue = boneHolder.m_orient2LikeValue;
		boneStruct.ucRollLikeValue = boneHolder.m_rollLikeValue;
	}
	// set index only when index is !=-1 (default is NULL)
	if (boneHolder.m_orientBone!=-1)
		boneStruct.OrientBone = boneHolder.m_orientBone;
	// set index only when index is !=-1 (default is NULL)
	if (boneHolder.m_orient2Bone!=-1)
		boneStruct.Orient2Bone = boneHolder.m_orient2Bone;

	// scale like and unscale values
	boneStruct.ucScaleLikeValue = boneHolder.m_scaleLikeValue;
	if (boneHolder.m_bUnscaleBone)
	{
		boneStruct.bUnscaleBone = TRUE;
	}

	boneStruct.mBoneBox = boneHolder.m_boundingBox;
}

const TupArray<int> &BoneBuilder::GetBoneVerts(void) const
{
	return m_boneVerts;
}

const TupArray<BoneSimple> &BoneBuilder::GetSimpleBones(void) const
{
	return m_simpleBoneArray;
}

// given a list of bones in a certain order remap the bone builder data to be in this order
void BoneBuilder::RemapBonesToList(const TupArray<TupString> &masterBoneNames)
{
	Keeper<TupString> masterBoneKeeper;
	TupArray<int> masterBoneIndices;

	int numMasterBoneNames = masterBoneNames.GetSize();
	int nameIndex;
	for (nameIndex=0;nameIndex<numMasterBoneNames;nameIndex++)
	{
		int boneNamesSize = masterBoneKeeper.GetSize();
		int newIndex = masterBoneKeeper.Add(masterBoneNames[nameIndex]);
		if (newIndex==boneNamesSize)
		{
			masterBoneIndices.Add(nameIndex);
		}
		else
		{
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
				"Warning - Duplicate bone name in master bone name list %s",
				(const char *)masterBoneNames[nameIndex]);
		}
	}
	int numSlaveBones = m_boneArray.GetSize();
	TupArray<int> slaveToMasterTable; // remap table from slave to master indexes
	TupArray<bool> validSlavesTable; // which slave bones that were mapped correctly to the 
	TupArray<int> numSlaveVertsRemappedTable; // how many verts were remapped

	slaveToMasterTable.SetSize(numSlaveBones);
	validSlavesTable.SetSize(numSlaveBones);
	numSlaveVertsRemappedTable.SetSize(numSlaveBones);

	// first bone maps the same
	slaveToMasterTable[0] = 0;
	validSlavesTable[0] = true;
	numSlaveVertsRemappedTable[0] = 0;
	int boneIndex;
	for (boneIndex=1;boneIndex<numSlaveBones;boneIndex++)
	{
		numSlaveVertsRemappedTable[boneIndex] = 0;

		BoneHolder &boneHolder = m_boneArray[boneIndex];
		int keeperIndex = masterBoneKeeper.GetIndex(boneHolder.m_name);
		if (keeperIndex==-1)
		{
			slaveToMasterTable[boneIndex]=0;
			validSlavesTable[boneIndex] = false;
		}
		else
		{
			slaveToMasterTable[boneIndex] = masterBoneIndices[keeperIndex];
			validSlavesTable[boneIndex] = true;
		}
	}
	// remap the vertices to point to the master bone instead of the slave
	int numVerts = m_boneVerts.GetSize();
	for (int vertIndex=0;vertIndex<numVerts;vertIndex++)
	{
		int boneIndex = m_boneVerts[vertIndex];
		numSlaveVertsRemappedTable[boneIndex]++;
		int newBoneIndex = slaveToMasterTable[boneIndex];
		m_boneVerts[vertIndex] = newBoneIndex;
	}

	PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Mesh vert bone assignments remapped to master bone list");
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"From    To  #Verts   Bone Name");
	int numProblemBones = 0;
	for (boneIndex=0;boneIndex<numSlaveBones;boneIndex++)
	{
		int numVerts = numSlaveVertsRemappedTable[boneIndex];
		if (numVerts)
		{
			if (!validSlavesTable[boneIndex])
			{
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_RED|LogFlags::FONT_STYLE_BOLD,
					"Error - bone %s not found in master bone list Remmapping to root",
					(const char *)m_boneArray[boneIndex].m_name);
				numProblemBones++;
			}
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED," %4d  %4d  %4d     %s",
				boneIndex,slaveToMasterTable[boneIndex],numVerts,(const char *)m_boneArray[boneIndex].m_name);
		}
	}
	if (numProblemBones)
	{
		throw(PluginException(ERROR_FATAL,"Error remapping to master bone list"));
	}

	m_bRemappedVertsToMaster = true;
}

void BoneBuilder::GetBoneNameList(TupArray<TupString> &masterNameStrings) const
{
	masterNameStrings.RemoveAll();
	int numBones = m_boneArray.GetSize();
	for (int boneIndex=0;boneIndex<numBones;boneIndex++)
	{
		const BoneHolder &boneHolder = m_boneArray[boneIndex];
		masterNameStrings.Add(boneHolder.m_name);
	}
}

bool BoneBuilder::RemappedToMaster(void) const
{
	return m_bRemappedVertsToMaster;
}