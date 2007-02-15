////////////////////////////////////////////////////////////////////////////
//
// JointManager
//
// Class to handle keeping track of the joint data for import
//
////////////////////////////////////////////////////////////////////////////

#ifndef JOINT_MANAGER
#define JOINT_MANAGER

class JointManager
{
public:
	class BoneConnections
	{
	public:
		int m_boneNodeRef; // node reference which bone are we connected to -1 if end bone
		MDagPath m_boneDagPath; // dag path to maya bone
		float m_boneLength; // used for building end bones
		TupArray<int> m_vertexIndices; // what vertices within the mesh we are using
		TupArray<float> m_weights; // the weights for each vertex
		Graphics4x4 m_boneBindingPoseMatrix; // the bone matrix for the binding pose
	};
	class SkinForMesh
	{
	public:
		bool m_bRigidBind;
		int m_meshNodeRef;
		int m_meshObjectRef;
		MDagPath m_meshDagPath;
		TupArray<BoneConnections> m_bones; // keep track of multiple bones for this mesh
	};

private:

	TupNodePool &m_tupNodePool;
	TupObjectPool &m_tupObjectPool;
	TupModifierPool &m_tupModifierPool;

	TupArray<SkinForMesh> m_skins;

	Keeper<int> m_boneNodes;
	TupArray<int> m_boneSkinIndices;
	TupArray<int> m_boneInSkinIndices;

public:
	JointManager(TupNodePool &tupNodePool,TupObjectPool &tupObjectPool,TupModifierPool &tupModifierPool);

	void ReadSkinData(void);
	bool BoneInSkin(int boneNodeRef,int &skinIndex,int &boneInSkinIndex) const;

	void AddEndBoneToSkin(int skinIndex,const BoneConnections &boneConnections);
	int GetSkinIndex(int meshObjectRef) const;
	int GetNumSkins(void) const; // return the size of the m_skins array
	int GetNumBones(int skinIndex) const;
	const SkinForMesh &JointManager::GetSkin(int skinIndex) const;
	const BoneConnections &GetBoneConnections(int skinIndex,int boneIndex) const;
	bool SetDagPathForBone(int nodeRef,const MDagPath &boneDagPath);
	void SetMeshDagPathForSkin(int skinIndex,const MDagPath &meshDagPath);

	void ConnectSkinAndJoints(void);

private:

	MObject CreateBindPose(void);
	bool BindSkin(const MDagPath &meshPath,const TupArray<MDagPath> &bones,const TupArray<MSelectionList> &vertexgroups);
	bool HookUpBoneToBindPose(const MDagPath &jointDagPath,MObject bindpose,MObject prevJointCluster,const MSelectionList &vertices,MObject &jointcluster,MObject &boneset);
	bool CreateJointClusterChain(const MDagPath &bonedagpath,const MSelectionList &vertices,MObject prevJointCluster,MObject &jointcluster,MObject &boneset);
	MObject CreateJointCluster(MObject bone);
	MObject CreateBoneSet(MObject bone,const MSelectionList &vertices);
	MObject CreateSkinPartition(void);

};

#endif
