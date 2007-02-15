////////////////////////////////////////////////////////////////////////////
//
// ExportBones
//
// Deal with the bones within a mesh
//
////////////////////////////////////////////////////////////////////////////

#ifndef EXPORT_BONES_H
#define EXPORT_BONES_H

class BoneHolder
{
public:

	Cartesian4x4 m_boneToModelMatrix;
	Graphics4x4 m_boneToStandardMatrix;
	int m_firstChildIndex;
	int m_nextSiblingIndex;
	int m_parentIndex;
	int m_index;
	int m_groupIndex;
	int m_nextBoneInGroupIndex;
	Vector3 m_pivotPoint;
	Vector3 m_boneEndPoint;
	float m_boneLength;
	TupString m_name;
	int m_boneRef;
	int m_simpleIndex;
	ts_BoundingBox m_boundingBox;
	int m_numAssignedVerts;

	bool m_bOrientXYZ;
	union
	{
		unsigned char m_orientLikeValue;
		unsigned char m_orientX;
	};

	union
	{
		unsigned char m_orient2LikeValue;
		unsigned char m_orientY;
	};

	union
	{
		unsigned char m_rollLikeValue;
		unsigned char m_orientZ;
	};
	int m_orientBone;
	int m_orient2Bone;

	unsigned char m_scaleLikeValue;
	bool m_bUnscaleBone;

	BoneHolder(void);
};

class BoneSimple
{
public:
	int m_boneRef;
	int m_holderIndex;
	Graphics4x4 m_boneTM;
	float m_boneLength;
};

class BoneGroupTable;

class BoneBuilder
{
	TupArray<BoneHolder> m_boneArray;
	TupArray<BoneSimple> m_simpleBoneArray;
	TupArray<int> m_boneVerts;
	BoneGroupTable &m_boneGroupTable;
	TupString m_modelName;

	TupTriMesh &m_tupTriMesh;
	TupNodePool &m_tupNodePool;
	TupObjectPool &m_tupObjectPool;
	int m_tupRootNodeRef;
	TupArray<int> m_parentBoneRefs;
	bool m_bAddedRootBone;
	bool m_bRemappedVertsToMaster;
	int m_errorCount;

public:

	BoneBuilder(TupTriMesh &tupMesh,TupNodePool &tupNodePool,TupObjectPool &tupObjectPool,BoneGroupTable &boneGroupTable,int tupRootNodeRef);
	int GetNumBones(void) const;
	void FillBoneStruct(int boneIndex,ts_ModelBone &boneStruct) const;
	const TupArray<int> &GetBoneVerts(void) const;
	const TupArray<BoneSimple> &GetSimpleBones(void) const;
	void RemapBonesToList(const TupArray<TupString> &masterBoneNames);
	void GetBoneNameList(TupArray<TupString> &masterNameStrings) const;
	bool RemappedToMaster(void) const;
	int GetErrorCount(void) const;

private:
	bool HasParentBone(int boneRef);
	void HookUpBone(Keeper<int> &boneKeeper,int boneKeeperIndex);
	void FixBoneName(TupString &boneName);
	void HandleOrientLike(int boneIndex);
	void HandleBonePivots(int boneIndex);
	void AddBoneInfoToLog(int boneIndex,int indentLevel);

	void HandleBoneStandardMatrix(int boneIndex);
	Graphics4x4 Rotate2V(const Vector3 &from,const Vector3 &to);
	Graphics4x4 RotateVRadians( float anglerad, const Vector3 &axis);

	int FindBone(const char *pBoneName);

	int GetBoneIndex(const char *pParentBoneName,const char *pMainBoneName);
	unsigned char GetPercent(const char *pParentBoneName,int percentIndex,const char *pMainBoneName);


};


#endif
