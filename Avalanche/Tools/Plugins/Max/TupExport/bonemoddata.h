
// skin modifier local mod data from the bonedef source

#ifndef __BONEMODDATA__H
#define __BONEMODDATA__H

class VertexDistanceClass
{
public:
	float dist;
	float u;
    int SubCurveIds;
    int SubSegIds;
    Point3 Tangents;
    Point3 OPoints;

};

class LocalGizmoData;
class CSkinCallback;
class VertexListClass;
class ISkinEngine;
class WeightRestore;
class BonesDefMod;
class ExclusionListClass;

class BoneModDataREV4 : public LocalModData, public ISkinContextData {

	public:
		BOOL initialShadeVC, initialDisplayVC;

		float squashAmount;
		BitArray autoInteriorVerts;
		Tab<int> vecOwnerList;
//gizmo data
		Tab<LocalGizmoData*> gizmoData;
//vertex info
		// NS: flag that shows if the InitMesh TM has to be recomputed
		bool recompInitMeshTM;
        int VertexDataCount;
        float effect;

		// This data value is needed for the CSkinCallabck (Eval)
		TimeValue CurrentTime;
		
		// SkinCallback variables :
		DWORD		cookie;
		CSkinCallback *pSkinCallback;

		// The engine pointer
		ISkinEngine *pSE;

		Point3 hitPoint;
		BOOL isHit;

		Tab<VertexListClass*> VertexData;
		Matrix3 BaseTM,InverseBaseTM;

		// This is the node TM of the mesh at initialization time
		Matrix3 BaseNodeTM;
		ObjectState *hitState;
//		Mesh mesh;
//		PatchMesh patch;
		BOOL isMesh;
		BOOL isPatch;
		BOOL isSurf;
//selectio info
        BitArray selected;

		Tab<VertexDistanceClass> DistCache;
		int CurrentCachePiece;

		Tab<Matrix3> tmCacheToBoneSpace;
		Tab<Matrix3> tmCacheToObjectSpace;

		Tab<Point3> tempTableL1;
		Tab<Point3> tempTableL2;
		Tab<Point3> tempTableL1ObjectSpace;
		Tab<Point3> tempTableL2ObjectSpace;

		BOOL forceUpdate;
		Point3 localCenter;


		WeightRestore *wundo;
//6-18-99
		BOOL inputObjectIsNURBS;
		BonesDefMod *mod;  //ns
//watje 9-7-99  198721 
        BOOL reevaluate;
        BOOL justRedraw;

//		BOOL addGizmo;

		Tab<ExclusionListClass*> exclusionList;
		BOOL isExcluded(int boneID, int vertID);
		void ExcludeVerts(int boneID, Tab<int> exList);
		void IncludeVerts(int boneID, Tab<int> incList);
		void SelectExcludedVerts(int boneID);




		BoneModDataREV4(BonesDefMod *m);
		~BoneModDataREV4();
		LocalModData*	Clone();

		void InitSkinEngine();
		
		// From ISkinContextData (Skin API) ns
		virtual int GetNumPoints();
		virtual int GetNumAssignedBones(int pointIdx);
		virtual int GetAssignedBone(int pointIdx, int boneIdx);
		virtual float GetBoneWeight(int pointIdx, int boneIdx);

		virtual int GetSubCurveIndex(int pointIdx, int boneIdx);
		virtual int GetSubSegmentIndex(int pointIdx, int boneIdx);
		virtual float GetSubSegmentDistance(int pointIdx, int boneIdx);
		virtual Point3 GetTangent(int pointIdx, int boneIdx);
		virtual Point3 GetOPoint(int pointIdx, int boneIdx);
		
		Tab<BitArray*> namedSelSets;
};

class BoneModDataREV5 : public LocalModData, public ISkinContextData {

	public:

		BOOL nukeValidCache;
		BOOL initialShadeVC, initialDisplayVC;

		float squashAmount;
		BitArray autoInteriorVerts;
		Tab<int> vecOwnerList;
		BitArray validVerts;
//gizmo data
		Tab<LocalGizmoData*> gizmoData;
//vertex info
		// NS: flag that shows if the InitMesh TM has to be recomputed
		bool recompInitMeshTM;
        int VertexDataCount;
        float effect;

		// This data value is needed for the CSkinCallabck (Eval)
		TimeValue CurrentTime;
		
		// SkinCallback variables :
		DWORD		cookie;
		CSkinCallback *pSkinCallback;

		// The engine pointer
		ISkinEngine *pSE;

		Point3 hitPoint;
		BOOL isHit;

		Tab<VertexListClass*> VertexData;
		Matrix3 BaseTM,InverseBaseTM;
		Matrix3 baseNodeOffsetTM;

		Matrix3 gizmoRemoveDoubleOffset;
		Matrix3 gizmoPutBackDoubleOffset;


		// This is the node TM of the mesh at initialization time
		Matrix3 BaseNodeTM;
		ObjectState *hitState;
//		Mesh mesh;
//		PatchMesh patch;
		BOOL isMesh;
		BOOL isPatch;
		BOOL isSurf;
//selectio info
        BitArray selected;

		Tab<VertexDistanceClass> DistCache;
		int CurrentCachePiece;

		Tab<Matrix3> tmCacheToBoneSpace;
		Tab<Matrix3> tmCacheToObjectSpace;

		Tab<Point3> tempTableL1;
		Tab<Point3> tempTableL2;
		Tab<Point3> tempTableL1ObjectSpace;
		Tab<Point3> tempTableL2ObjectSpace;

		BOOL forceUpdate;
		Point3 localCenter;


		WeightRestore *wundo;
//6-18-99
		BOOL inputObjectIsNURBS;
		BonesDefMod *mod;  //ns
//watje 9-7-99  198721 
        BOOL reevaluate;
        BOOL justRedraw;

//		BOOL addGizmo;

		Tab<ExclusionListClass*> exclusionList;
		BOOL isExcluded(int boneID, int vertID);
		void ExcludeVerts(int boneID, Tab<int> exList,BOOL cleanUpVerts=TRUE);
		void IncludeVerts(int boneID, Tab<int> incList);
		void SelectExcludedVerts(int boneID);
		void CleanUpExclusionLists();

		BOOL forceRecomuteBaseNode;

		INode *meshNode;

		BOOL needTMsRebuilt;


		BoneModDataREV5(BonesDefMod *m);
		~BoneModDataREV5();
		LocalModData*	Clone();

		void InitSkinEngine();
		
		// From ISkinContextData (Skin API) ns
		virtual int GetNumPoints();
		virtual int GetNumAssignedBones(int pointIdx);
		virtual int GetAssignedBone(int pointIdx, int boneIdx);
		virtual float GetBoneWeight(int pointIdx, int boneIdx);

		virtual int GetSubCurveIndex(int pointIdx, int boneIdx);
		virtual int GetSubSegmentIndex(int pointIdx, int boneIdx);
		virtual float GetSubSegmentDistance(int pointIdx, int boneIdx);
		virtual Point3 GetTangent(int pointIdx, int boneIdx);
		virtual Point3 GetOPoint(int pointIdx, int boneIdx);
		
		Tab<BitArray*> namedSelSets;
};

#endif