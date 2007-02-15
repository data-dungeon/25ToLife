/**********************************************************************
 *<
	FILE: bones.cpp

	DESCRIPTION:  Bone implementation

	HISTORY: created November 11 1994

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

#ifndef __BONES__H
#define __BONES__H

class BoneObj : public SimpleObject2 {
	public:
		static IObjParam *ip;

		BoneObj(BOOL loading=FALSE);
		
		// Animatable methods		
		void DeleteThis() {delete this;}
		Class_ID ClassID() {return BONE_OBJ_CLASSID;}		
		void BeginEditParams(IObjParam  *ip, ULONG flags, Animatable *prev);
		void EndEditParams(IObjParam *ip, ULONG flags, Animatable *next);		
		RefTargetHandle Clone(RemapDir& remap = NoRemap());
		SvGraphNodeReference SvTraverseAnimGraph(IGraphObjectManager *gom, Animatable *owner, int id, DWORD flags); //Added by AF (12/01/2000)
		
		// ReferenceMaker methods:
		void RescaleWorldUnits(float);

		// From Object
#if 0
		int CanConvertToType(Class_ID obtype);
		Object* ConvertToType(TimeValue t, Class_ID obtype);
		void GetCollapseTypes(Tab<Class_ID> &clist, Tab<TSTR*> &nlist);
#endif
		CreateMouseCallBack* GetCreateMouseCallBack() {return NULL;}		
		TCHAR *GetObjectName();
		BOOL HasUVW();
		void SetGenUVW(BOOL sw);
		int	NumParamBlocks() {return 1;}
		IParamBlock2* GetParamBlock(int i) {return pblock2;}
		IParamBlock2* GetParamBlockByID(BlockID id) {return (pblock2->ID() == id) ? pblock2 : NULL;}
		
		// From SimpleObject
		void BuildMesh(TimeValue t);
		BOOL OKtoDisplay(TimeValue t);
		void InvalidateUI();

		void BuildFin(
			float size, float startTaper, float endTaper, 
			DWORD i0, DWORD i1, DWORD i2, DWORD i3, DWORD &curVert, DWORD &curFace);		
	};


IObjParam *BoneObj::ip = NULL;

#define PBLOCK_REF_NO	 0

// block IDs
enum { boneobj_params, };

// boneobj_params IDs
enum { 
	boneobj_width, boneobj_height, boneobj_taper, boneobj_length,
	boneobj_sidefins, boneobj_sidefins_size, boneobj_sidefins_starttaper, boneobj_sidefins_endtaper,
	boneobj_frontfin, boneobj_frontfin_size, boneobj_frontfin_starttaper, boneobj_frontfin_endtaper,
	boneobj_backfin,  boneobj_backfin_size,  boneobj_backfin_starttaper,  boneobj_backfin_endtaper,
	boneobj_genmap };

#endif