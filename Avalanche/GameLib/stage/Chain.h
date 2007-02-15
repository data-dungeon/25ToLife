/*
**
**  File:   Chain.h
**  Date:   December 17, 2002
**  By:     Bryant Collard
**  Desc:   Manages chained bones in a model.
**
**  Copyright (c) 2002, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/gamelibnew/stage/Chain.h $
**  $Revision: 7 $
**      $Date: 7/28/03 10:20p $
**    $Author: Rob Nelson $
**
*/

#ifndef CHAIN_H
#define CHAIN_H

#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "stage/cactor.h" // Just to get ENABLE_RECORD

struct ts_Bone;
struct ts_ModelBone;
struct ts_PatchSOM;

class CChain
{
  public:
	CChain();
	virtual ~CChain();
	virtual bool InitializeFail(void);
	bool SetName(char* i_name);
	virtual bool SetNumLinks(int i_numLinks);
	virtual bool InitLink(int i_link, ts_Bone* i_bone);
	virtual bool CompleteInit(CInstance* i_inst,
			Graphics4x4* i_modelToWorld);
	virtual bool IsName(char* i_queryName);
	bool TryAddPartner(CChain* i_queryChain);
	virtual void Reset(void);
	void AdjustBones(void);
	void SetReferenceOrientation(int i_link, const DirCos3x3 &i_refOrientation);
	const char* GetName(void) {return(d_name);}
	int GetNumLinks(void) const {return(d_numLinks);}
	ts_Bone* GetBone(int i_link) const;
	Vector3& GetParticleInWorld(int i_link) const;
	Vector3 &MinExtent(void) {return(d_minExtent);}
	Vector3 &MaxExtent(void) {return(d_maxExtent);}
	void GetPartnershipExtents (Vector3 &io_minExtent, Vector3 &io_maxExtent,
			bool i_compare = false);
	void GetLinkAxis(int i_link, Vector3 &o_axis);
	void GetLinkToWorld(int i_link, Graphics4x4 &o_linkToWorld);
	float GetNearestPoint(Vector3CRef i_testPoint, Vector3 &o_chainPoint,
			int &o_link, float &o_pctLinkDist) const;
	float GetCloserPoint(Vector3CRef i_testPoint, Vector3 &o_chainPoint,
			int &io_link, float &o_pctLinkDist) const;
	float GetScale(void) const {return(d_scale);}
	void DrawGeometry(CInstance* i_inst, int i_link);
	void DrawBoneBoxes(int i_startColor = 0, bool i_drawBone = false);
	void UpdateExtents(void);	// for entire chain

#ifdef ENABLE_RECORD
	bool CreateRecordBuffer(int i_numTicks);
	void DeleteRecordBuffer(void);
	void RecordState(int i_entry);
	void LoadState(int i_entry);
#endif

  protected:
	bool CompleteLinkInit(int i_link, ts_PatchSOM* i_model,
			Vector3 &o_extentInBone);
	void ResetParticles(void);
	void CompleteReset(void);
	void UpdateExtents(Vector3 &i_point, Vector3 &io_min, Vector3 &io_max);
	bool ComputeUniformParent(float &o_uniformScale,
			Graphics4x4 &o_invShearScaleParentToModel,
			DirCos3x3 &o_parentToWorldRotation,
			Vector3 &o_parentInWorld) const;
	void BuildLinkToParent(int i_link, DirCos3x3 &i_parentToWorld,
			DirCos3x3 &o_linkToParent) const;

	CChain* d_partner;
	char* d_name;
	int d_numLinks;
	struct SLink
	{
		ts_Bone* d_bone;
		DirCos3x3 d_refLinkToParent;
		Vector3 d_newParticleInWorld;
	} *d_link;
	Graphics4x4* d_modelToWorld;
	float d_maxRadius;
	float d_scale;
	Vector3 d_minExtent;
	Vector3 d_maxExtent;

  private:
	void ComputeBoneExtent(ts_PatchSOM* i_model, ts_ModelBone* i_bone,
			bool i_includeOrientLike, Vector3 &o_extentInBone);
	void ComputeOneBoneExtent(ts_ModelBone* i_bone, Vector3 &io_extent);

#ifdef ENABLE_RECORD
  protected:
	struct SChainRecordEntry
	{
		struct SRecordLink
		{
			Vector3 d_newParticleInWorld;
		} *d_link;
	} *d_chainRecordBuffer;
	int d_numRecordTicks;
#endif
};

#define CHAIN_HPP
#include "stage/Chain.hpp"

#endif // CHAIN_H
