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
**   $Archive: /Avalanche/gamelibnew/stage/Chain.cpp $
**  $Revision: 11 $
**      $Date: 7/28/03 10:20p $
**    $Author: Rob Nelson $
**
*/

/******************* includes ****************************/

/** Pre-Compiled header **/
#include "stage/StagePCH.h"
#include "cmdcon/cmdcon.h"

/** Includes used that are in pre-compiled header **/
#if 0

/* My header (always first) */
#include "stage/Chain.h"

/* system includes */
#include <string.h>
#include <stdio.h>

/* engine includes */
#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Layers/Assert/Assert.h"
#include "Layers/Debug.h"
#include "Game/Animation/Bone.h"
#include "Game/Database/Som.h"
#include "Display/Common/PatchDisp.h"
#include "Layers/Timing.h"

#include "EngineHelper/drawutil.h"

#if defined(GCN) && defined(CW)
#include "platform/GCN/GCNMWPort.h"
#endif

#endif

/**********************************************************************
 *
 *	Class:       CChain
 *	Function:    CChain()
 *	Access:      public
 *	Description: Default constructor.
 *	Arguments:   None
 *	Return:      None
 *
 **********************************************************************/
CChain::CChain()
{
	d_partner = NULL;
	d_name = NULL;
	d_numLinks = 0;
	d_link = NULL;
#ifdef ENABLE_RECORD
	d_chainRecordBuffer = 0;
#endif
}

/**********************************************************************
 *
 *	Class:       CChain
 *	Function:    ~CChain()
 *	Access:      public
 *	Description: Destructor
 *	Arguments:   None
 *	Return:      None
 *
 **********************************************************************/
CChain::~CChain()
{
	delete[] d_link;
	delete[] d_name;
	delete d_partner;
#ifdef ENABLE_RECORD
	DeleteRecordBuffer();
#endif
}

bool CChain::InitializeFail(void)
{
	delete[] d_name;
	delete[] d_link;
	d_name = NULL;
	d_link = NULL;
	d_numLinks = 0;
	return(false);
}

bool CChain::SetName(char* i_name)
{
	MEM_SET_ONE_SHOT_NEW_NAME("CChain::SetName");
	d_name = new char[strlen(i_name) + 1];
	if (d_name == NULL)
	{
		ASSERT(false);
		return(false);
	}
	strcpy(d_name, i_name);
	return(true);
}

bool CChain::SetNumLinks(int i_numLinks)
{
	if (i_numLinks <= 1)
	{
		ASSERT(false);
		return(false);
	}
	d_numLinks = i_numLinks;
	MEM_SET_ONE_SHOT_NEW_NAME("CChain::SetNumLinks");
	d_link = new SLink[d_numLinks];
	if (d_link == NULL)
	{
		ASSERT(false);
		return(false);
	}
	return(true);
}

bool CChain::InitLink(int i_link, ts_Bone* i_bone)
{
	Cartesian4x4 boneToParent;
	if ((i_link < 1) || (i_link >= d_numLinks))
	{
		ASSERT(false);
		return(false);
	}
	if (i_bone == NULL)
	{
		char msgStr[256];
		sprintf(msgStr, "Cannot find bone %d (of %d) in chain %s\n", i_link,
				d_numLinks - 1, d_name);
		conPuts(msgStr);
		g_console.Notify(msgStr);
		ASSERT(false);
		return(false);
	}
	ts_ModelBone *mbone = i_bone->pModelBone;
	d_link[i_link].d_bone = i_bone;
	boneComputeNeutralBoneToParent(i_bone, &boneToParent);
	boneToParent.GetOrientation(d_link[i_link].d_refLinkToParent);

	if (i_link == 1)
	{
		d_link[0].d_bone = d_link[1].d_bone->m_parent;
	}
	return(true);
}

bool CChain::CompleteInit(CInstance* i_inst, Graphics4x4* i_modelToWorld)
{
	Vector3 extentInBone;
	ts_PatchSOM* model = i_inst->Model();
	d_maxRadius = 0.0f;
	for (int i = 1; i < d_numLinks; i++)
	{
		if (!CompleteLinkInit(i, model, extentInBone))
			return(false);
	}
	d_maxRadius = Math::Sqrt(d_maxRadius);
	d_scale = 1.0f;
/*
 *	Place in initial configuration.
 */
	d_modelToWorld = i_modelToWorld;
	Reset();

	return(true);
}

bool CChain::CompleteLinkInit(int i_link, ts_PatchSOM* i_model,
		Vector3 &o_extentInBone)
{
/*
 *	Make sure the bones have the correct parent/child relationship.
 */
	if (d_link[i_link].d_bone->m_parent != d_link[i_link - 1].d_bone)
	{
		char msgStr[256];
		sprintf(msgStr, "Bone %d of chain %s is not the parent of "
				"bone %d.\n", i_link - 1, d_name, i_link);
		conPuts(msgStr);
		g_consolePtr->Notify(msgStr);
		ASSERT(false);
		return(false);
	}
/*
 *	Compute node extents.
 */
	ComputeBoneExtent(i_model, d_link[i_link].d_bone->pModelBone, false,
			o_extentInBone);
	float extentRadiusSquared = o_extentInBone.LengthSquared();
	if (extentRadiusSquared > d_maxRadius)
		d_maxRadius = extentRadiusSquared;

	return(true);
}

/**********************************************************************
 *
 *	Class:       CChain
 *	Function:    IsName
 *	Access:      public
 *	Description: Determines if a name is the same as the chain name.
 *	Arguments:
 *		i_queryName - The test name.
 *	Return:      true if the names are the same, false otherwise.
 *
 **********************************************************************/
bool CChain::IsName(char* i_queryName)
{
	ASSERT(d_name && d_name[0] && i_queryName && i_queryName[0]);

	if (!stricmp(d_name, i_queryName))
		return(true);

	if (d_partner && d_partner->IsName(i_queryName))
		return(true);

	return(false);
}

/**********************************************************************
 *
 *	Class:       CChain
 *	Function:    TryAddPartner
 *	Access:      public
 *	Description: Determines if the given chain is a partner of this
 *	             chain. Chains are partners if they belong to the
 *	             same bone group.
 *	Arguments:
 *		i_queryChain - The test chain.
 *	Return:      true if the chains are partners, false otherwise.
 *
 **********************************************************************/
bool CChain::TryAddPartner(CChain* i_queryChain)
{
	if (d_link && i_queryChain && i_queryChain->d_link &&
			(d_link[1].d_bone->pModelBone->boneGroupID ==
			i_queryChain->d_link[1].d_bone->pModelBone->boneGroupID))
	{
		i_queryChain->d_partner = d_partner;
		d_partner = i_queryChain;
		return(true);
	}
	return(false);
}

/**********************************************************************
 *
 *	Class:       CChain
 *	Function:    Reset
 *	Access:      public
 *	Description: Places particles in their "pose" positions.
 *	Arguments:   None
 *	Return:      None
 *
 **********************************************************************/
void CChain::Reset(void)
{
/*
 *	Bail if the chain is not visible.
 */
	if (!boneGetVisibility(d_link[1].d_bone))
		return;

	ResetParticles();
	CompleteReset();
}

void CChain::ResetParticles(void)
{
/*
 *	A single coordinate system is used for the chain and it must be uniformly
 *	scaled from the world. Find a matrix that will back out any scale and shear
 *	and a scale to approximate the possibly non-uniform scale backed out.
 */
	Graphics4x4 parentToModelTransform;
	DirCos3x3 parentToWorld;
	Vector3 parentInWorld;
	if (!ComputeUniformParent(d_scale, parentToModelTransform, parentToWorld,
			parentInWorld))
		return;
/*
 *	Compute a local matrix for the first link that will back out any shear and
 *	scale applied to any ancestor bones or to the model itself, apply a uniform
 *	scale that approximates the possibly non-uniform scale that was backed out,
 *	and orient the bone in the design pose.
 */
	Graphics4x4 firstLocalMatrix;
	firstLocalMatrix.SetScale(d_scale, d_scale, d_scale);
	firstLocalMatrix.PokeTranslation((1.0f - d_scale) *
			d_link[0].d_bone->pModelBone->boneToModel.GetTranslation());
	firstLocalMatrix /= d_link[0].d_bone->pModelBone->boneToModel;
	firstLocalMatrix *= parentToModelTransform;
	boneSetLocalMatrix(d_link[1].d_bone, &firstLocalMatrix);
/*
 *	Compute the composite matrix up to the first link of the chain and transform
 *	it into the world. This picks up the just set local matrix as well as any
 *	scale programatically set for the bone.
 */
	Graphics4x4 compositeModelToWorld;
	compositeModelToWorld = (*boneRunBonesToBone(d_link[1].d_bone)) *
			(*d_modelToWorld);
/*
 *	Apply any scale in the first link to the total chain scale.
 */
	d_scale *= (float)d_link[1].d_bone->scale / (float)SCALE_ONE;
/*
 *	Place the first two particles.
 */
	d_link[0].d_newParticleInWorld = d_link[1].d_bone->pModelBone->vPivot *
			compositeModelToWorld;
	d_minExtent = d_link[0].d_newParticleInWorld;
	d_maxExtent = d_link[0].d_newParticleInWorld;
	d_link[1].d_newParticleInWorld = d_link[1].d_bone->pModelBone->vBoneEnd *
			compositeModelToWorld;
	UpdateExtents(d_link[1].d_newParticleInWorld, d_minExtent, d_maxExtent);
/*
 *	Place the rest of the particles.
 */
	for (int i = 2; i < d_numLinks; i++)
	{
		boneSetScale(d_link[i].d_bone, 1.0f);
		boneSetLocalMatrixToIdentity(d_link[i].d_bone);
		d_link[i].d_newParticleInWorld = d_link[i].d_bone->pModelBone->vBoneEnd *
				compositeModelToWorld;
		UpdateExtents(d_link[i].d_newParticleInWorld, d_minExtent,
				d_maxExtent);
	}
}

void CChain::CompleteReset(void)
{
	d_minExtent -= d_maxRadius * d_scale;
	d_maxExtent += d_maxRadius * d_scale;

	if (d_partner)
		d_partner->Reset();
}

void CChain::AdjustBones(void)
{
/*
 *	Only adjust if the chain is visible.
 */
	if (boneGetVisibility(d_link[1].d_bone))
	{
/*
 *		Find terms needed to back out shear and non-uniform scale and apply an
 *		approximate uniform scale. Only adjust if non-zero scale is found.
 */
		Graphics4x4 parentToModelTransform;
		DirCos3x3 parentToWorld;
		Vector3 parentInWorld;
		if (ComputeUniformParent(d_scale, parentToModelTransform, parentToWorld,
				parentInWorld))
		{
/*
 *			Find the rotation from the first link to its parent.
 */
			DirCos3x3 linkToParent;
			BuildLinkToParent(1, parentToWorld, linkToParent);
/*
 *			Build a local matrix that will back out shear and non-uniform scale,
 *			apply approximate uniform scale and orient the first link with the
 *			first two particles. Set the matrix.
 */
			Graphics4x4 linkTransform;
			linkTransform.Set(d_scale * linkToParent);
			linkTransform.PokeTranslation(
					(d_link[0].d_newParticleInWorld - parentInWorld) / parentToWorld);
			linkTransform = d_link[1].d_bone->pModelBone->boneToModel.InvertTimes(
					linkTransform * parentToModelTransform);
			boneSetLocalMatrix(d_link[1].d_bone, &linkTransform);
/*
 *			Include any scale programatically applied to the first link. Find the
 *			inverse of the combined scale for convenience. (Note that a visible
 *			first link implies that its scale is not zero.)
 */
			d_scale *= (float)d_link[1].d_bone->scale / (float)SCALE_ONE;
			float uniformScaleInv = 1.0f / d_scale;
/*
 *			Process each remaining link.
 */
			for (int i = 2; i < d_numLinks; i++)
			{
/*
 *				Scaling is allowed only on the first link.
 */
				ASSERT(d_link[i].d_bone->scale == SCALE_ONE);
/*
 *				Find the next parent to world rotation.
 */
				parentToWorld = linkToParent * parentToWorld;
/*
 *				Build the link to parent transform and set the bone's local matrix.
 */
				BuildLinkToParent(i, parentToWorld, linkToParent);
				linkTransform.Set(linkToParent);
				linkTransform.PokeTranslation(uniformScaleInv *
						(d_link[i - 1].d_newParticleInWorld -
						d_link[i - 2].d_newParticleInWorld) / parentToWorld);
				boneSetBoneToParent(d_link[i].d_bone, &linkTransform);
			}

#if 0
			Graphics4x4 draw;
			for (int j = 1; j < d_numLinks; j++)
			{
#if 0
				draw = (*boneRunBonesToBone(d_link[j].d_bone)) * (*d_modelToWorld);
				DrawUtility::Line(d_link[j].d_bone->pModelBone->vPivot * draw,
						d_link[j].d_bone->pModelBone->vBoneEnd * draw,
						(DrawUtility::COLOR)(j % 7));
#else
				DrawUtility::Line(d_link[j - 1].d_newParticleInWorld,
						d_link[j].d_newParticleInWorld, (DrawUtility::COLOR)(j % 7));
#endif
			}
#endif
		}
	}

	if (d_partner)
		d_partner->AdjustBones();
}

bool CChain::ComputeUniformParent(float &o_uniformScale,
		Graphics4x4 &o_invShearScaleParentToModel,
		DirCos3x3 &o_parentToWorldRotation,
		Vector3 &o_parentInWorld) const
{
/*
 *	Build the transformation from the first link's parent to the world and
 *	deconstruct it. Bail if there is any zero scale.
 */
	Graphics4x4 parentTransform;
	float scaleX, scaleY, scaleZ, shearXY, shearXZ, shearYZ;
	parentTransform = d_link[0].d_bone->pModelBone->boneToModel *
			(*boneRunBonesToBone(d_link[0].d_bone)) * (*d_modelToWorld);
	parentTransform.Deconstruct(o_parentToWorldRotation, o_parentInWorld,
			scaleX, scaleY, scaleZ, shearXY, shearXZ, shearYZ);
	if (Math::DivByZero(1.0f, scaleX) ||
			Math::DivByZero(1.0f, scaleY) ||
			Math::DivByZero(1.0f, scaleZ))
		return(false);
/*
 *	Find the inverse of the shear and scale.
 */
	Matrix3x3 invShearScale;
	invShearScale.SetScale(1.0f / Math::Abs(scaleX), 1.0f / Math::Abs(scaleY),
			1.0f / Math::Abs(scaleZ));
	invShearScale.e10 = -shearXY * invShearScale.e00;
	invShearScale.e20 = (shearXY * shearYZ - shearXZ) * invShearScale.e00;
	invShearScale.e21 = -shearYZ * invShearScale.e11;
/*
 *	Build a matrix that backs out shear and scale.
 */
	o_invShearScaleParentToModel.Set(invShearScale);
	o_invShearScaleParentToModel *= d_link[0].d_bone->pModelBone->boneToModel;
/*
 *	Set a value for the uniform scale that approximates the possibly non-uniform
 *	scale. In this case, it is the average of the three scale terms but it could
 *	be anything.
 */
	o_uniformScale = (Math::Abs(scaleX) + Math::Abs(scaleY) +
			Math::Abs(scaleZ)) / 3.0f;

	return(true);
}

void CChain::BuildLinkToParent(int i_link, DirCos3x3 &i_parentToWorld,
		DirCos3x3 &o_linkToParent) const
{
	ASSERT((i_link > 0) && (i_link <= d_numLinks));
/*
 *	Compute the reference to world matrix.
 */
	DirCos3x3 refToWorld;
	refToWorld = d_link[i_link].d_refLinkToParent * i_parentToWorld;
/*
 *	Build the link to reference matrix.
 */
	DirCos3x3 linkToRef;
	Vector3 xAxis, yAxis, zAxis;
	zAxis = (d_link[i_link].d_newParticleInWorld -
			d_link[i_link - 1].d_newParticleInWorld) / refToWorld;
	if (!zAxis.SafeNormalize())
	{
		zAxis.Set(0.0f, 0.0f, 1.0f);
		yAxis.Set(0.0f, 1.0f, 0.0f);
	}
	else if (zAxis.x() > 0.99999f)
	{
		zAxis.Set(1.0f, 0.0f, 0.0f);
		yAxis.Set(0.0f, 1.0f, 0.0f);
	}
	else if (zAxis.x() < -0.99999f)
	{
		zAxis.Set(-1.0f, 0.0f, 0.0f);
		yAxis.Set(0.0f, 1.0f, 0.0f);
	}
	else
	{
		yAxis.Set(0.0f, zAxis.z(), -zAxis.y());
		yAxis.Normalize();
	}
	xAxis = yAxis.Cross(zAxis);
	linkToRef.SetRow0(xAxis);
	linkToRef.SetRow1(yAxis);
	linkToRef.SetRow2(zAxis);
/*
 *	Build the link to parent transformation.
 */
	o_linkToParent = linkToRef * d_link[i_link].d_refLinkToParent;
}

#define MIN_BONE_EXTENT 0.005f

void CChain::ComputeBoneExtent(ts_PatchSOM* i_model,
		ts_ModelBone* i_bone, bool i_includeOrientLike,
		Vector3 &o_extentInBone)
{
	ASSERT ((i_model != NULL) && (i_bone != NULL));

	o_extentInBone = i_bone->vBoneEnd / i_bone->boneToModel;
	if (o_extentInBone.X() < 0.0f)
		o_extentInBone.X(-o_extentInBone.X());
	if (o_extentInBone.Y() < 0.0f)
		o_extentInBone.Y(-o_extentInBone.Y());
	if (o_extentInBone.Z() < 0.0f)
		o_extentInBone.Z(-o_extentInBone.Z());

	ComputeOneBoneExtent(i_bone, o_extentInBone);

	if (i_includeOrientLike)
	{
		int i;
		ts_ModelBone* bone = i_model->pBody->pMBones;
		for (i = 0; i < i_model->pBody->usBones; i++, bone++)
		{
			if (bone->OrientBone == i_bone->boneID)
				ComputeOneBoneExtent(bone, o_extentInBone);
		}
	}

	if (o_extentInBone.X() < (float)MIN_BONE_EXTENT)
		o_extentInBone.X( (float)MIN_BONE_EXTENT);
	if (o_extentInBone.Y() < (float)MIN_BONE_EXTENT)
		o_extentInBone.Y( (float)MIN_BONE_EXTENT);
	if (o_extentInBone.Z() < (float)MIN_BONE_EXTENT)
		o_extentInBone.Z( (float)MIN_BONE_EXTENT);
}

void CChain::ComputeOneBoneExtent(ts_ModelBone* i_bone, Vector3 &io_extent)
{
	if (i_bone->mBoneBox.Max.X() > io_extent.X())
		io_extent.X( i_bone->mBoneBox.Max.X());
	if (-i_bone->mBoneBox.Min.X() > io_extent.X())
		io_extent.X( -i_bone->mBoneBox.Min.X());

	if (i_bone->mBoneBox.Max.Y() > io_extent.Y())
		io_extent.Y( i_bone->mBoneBox.Max.Y());
	if (-i_bone->mBoneBox.Min.Y() > io_extent.Y())
		io_extent.Y( -i_bone->mBoneBox.Min.Y());

	if (i_bone->mBoneBox.Max.Z() > io_extent.Z())
		io_extent.Z( i_bone->mBoneBox.Max.Z());
	if (-i_bone->mBoneBox.Min.Z() > io_extent.Z())
		io_extent.Z( -i_bone->mBoneBox.Min.Z());
}

void CChain::GetLinkAxis(int i_link, Vector3 &o_axis)
{
	if ((i_link >= 0) && (i_link < d_numLinks))
	{
		Graphics4x4 boneToWorld;
		boneToWorld = d_link[i_link].d_bone->pModelBone->boneToModel *
				(*boneRunBonesToBone(d_link[i_link].d_bone)) *
				(*d_modelToWorld);
		o_axis = boneToWorld.GetRow2();
		if (o_axis.SafeNormalize())
			return;
	}
	o_axis.Set(0.0f, 0.0f, 1.0f);
}

void CChain::GetLinkToWorld(int i_link, Graphics4x4 &o_linkToWorld)
{
	if ((i_link > 0) && (i_link < d_numLinks))
	{
		o_linkToWorld = d_link[i_link].d_bone->pModelBone->boneToModel *
				(*boneRunBonesToBone(d_link[i_link].d_bone)) *
				(*d_modelToWorld);
		o_linkToWorld.PokeTranslation(d_link[i_link - 1].d_newParticleInWorld);
	}
	else
		o_linkToWorld.Identity();
}

void CChain::DrawGeometry(CInstance* i_inst, int i_link)
{
	if ((i_link <= 0) || (i_link >= d_numLinks))
		return;

	DrawUtility::Line(d_link[i_link - 1].d_newParticleInWorld,
			d_link[i_link].d_newParticleInWorld,
			(DrawUtility::COLOR)(i_link % 7));

#ifdef WIN32
	Graphics4x4 linkToWorld;
	linkToWorld = *boneRunBonesToBone(d_link[i_link].d_bone) * *d_modelToWorld;

	BoneID linkBoneID = d_link[i_link].d_bone->boneID;

	ts_PatchSOM* model = i_inst->Model();
	ts_TriStripHeader* triStrip = model->pTriStripBuffer;
	while (triStrip)
	{
		if ((BoneID)(triStrip->u32BoneIndex & 0xff) == linkBoneID)
			DrawUtility::Tristrip(linkToWorld, triStrip);
		triStrip = triStrip->pNext;
	}
#endif
}

void CChain::DrawBoneBoxes(int i_startColor, bool i_drawBone)
{
	for (int i = 1; i < d_numLinks; i++)
	{
		DrawUtility::COLOR color = (DrawUtility::COLOR)(i_startColor % 7);
		ts_ModelBone *mbone = d_link[i].d_bone->pModelBone;
		Graphics4x4 boneToWorld = mbone->boneToModel *
				(*boneRunBonesToBone(d_link[i].d_bone)) *
				(*d_modelToWorld);
		boneToWorld.PokeTranslation(d_link[i - 1].d_newParticleInWorld);
		DrawUtility::Box(mbone->mBoneBox.Min, mbone->mBoneBox.Max,
				boneToWorld, color);
		if (i_drawBone)
		{
			DrawUtility::Line(d_link[i - 1].d_newParticleInWorld,
					d_link[i].d_newParticleInWorld, color);
			DrawUtility::Point(d_link[i - 1].d_newParticleInWorld, color, 0.05f);
			DrawUtility::Point(d_link[i].d_newParticleInWorld, color, 0.05f);
		}
		i_startColor++;
	}
}

float CChain::GetNearestPoint(Vector3CRef i_testPoint,
		Vector3 &o_chainPoint, int &o_link, float &o_pctLinkDist) const
{
/*
 *	Start by picking the chain end as the proposed nearest point.
 */
	o_link = d_numLinks - 1;
	o_pctLinkDist = 1.0f;
	o_chainPoint = d_link[o_link].d_newParticleInWorld;
	float minDistSq = (i_testPoint - o_chainPoint).LengthSquared();
/*
 *	See if a closer point is associated with the remaining links.
 */
	for (int i = 1; i < d_numLinks; i++)
	{
/*
 *		Find vectors relative to the link start point and the projection of
 *		the vector to the test point onto the link scaled by the link length.
 */
		int s = i - 1;
		Vector3 pointDelta = i_testPoint - d_link[s].d_newParticleInWorld;
		Vector3 linkDelta = d_link[i].d_newParticleInWorld -
				d_link[s].d_newParticleInWorld;
		float scaledProjection = pointDelta.Dot(linkDelta);
/*
 *		If the projection is negative, the start point is the closest point on the
 *		link. See if it is closer than the current proposed nearest point.
 */
		if (scaledProjection <= 0.0f)
		{
/*
 *			If the previous link is the current proposed closest link, this link's
 *			start point (previous link's end point) will not be closer.
 */
			if (o_link != s)
			{
				float distSq = pointDelta.LengthSquared();
				if (distSq < minDistSq)
				{
					minDistSq = distSq;
					o_link = i;
					o_pctLinkDist = 0.0f;
					o_chainPoint = d_link[s].d_newParticleInWorld;
				}
			}
		}
		else
		{
/*
 *			Find the link length squared. If it is less than the scaled projection,
 *			the end point is the closest point on the link. If it is the closest
 *			point, it will be detected when the next link is processed (or was
 *			used as the initial proposed point if this is the last link). If the
 *			link length squared is greater than the scaled projection, the closest
 *			point falls on the link.
 */
			float linkLengthSq = linkDelta.LengthSquared();
			if (scaledProjection < linkLengthSq)
			{
				float pointFraction = scaledProjection / linkLengthSq;
				float distSq = pointDelta.LengthSquared() -
						pointFraction * scaledProjection;
				if (distSq < minDistSq)
				{
					minDistSq = distSq;
					o_link = i;
					o_pctLinkDist = pointFraction;
					o_chainPoint = d_link[s].d_newParticleInWorld +
							pointFraction * linkDelta;
				}
			}
		}
	}
	return(minDistSq);
}

float CChain::GetCloserPoint(Vector3CRef i_testPoint,
		Vector3 &o_chainPoint, int &io_link, float &o_pctLinkDist) const
{
/*
 *	If no starting link for the search is given, just find the nearest point.
 */
	if ((io_link < 0) || (io_link >= d_numLinks))
	{
		return(GetNearestPoint(i_testPoint, o_chainPoint, io_link,
				o_pctLinkDist));
	}
/*
 *	Starting a search at link 0 is the same as starting at link 1.
 */
	int homeLink = (io_link == 0) ? 1 : io_link;
/*
 *	Find the distance to the starting link.
 */
	int s = homeLink - 1;
	Vector3 pointDelta = i_testPoint - d_link[s].d_newParticleInWorld;
	Vector3 linkDelta = d_link[homeLink].d_newParticleInWorld -
			d_link[s].d_newParticleInWorld;
	float scaledProjection = pointDelta.Dot(linkDelta);
	float minDistSq;
	if (scaledProjection <= 0.0f)
	{
		minDistSq = pointDelta.LengthSquared();
		o_pctLinkDist = 0.0f;
		o_chainPoint = d_link[s].d_newParticleInWorld;
	}
	else
	{
		o_pctLinkDist = scaledProjection / linkDelta.LengthSquared();
		if (o_pctLinkDist > 1.0f)
			o_pctLinkDist = 1.0f;
		minDistSq = pointDelta.LengthSquared() - o_pctLinkDist * scaledProjection;
		o_chainPoint = d_link[s].d_newParticleInWorld + o_pctLinkDist * linkDelta;
	}
/*
 *	Search forward in the chain.
 */
	int i;
	for (i = homeLink + 1; i < d_numLinks; i++)
	{
/*
 *		Find vectors relative to the link start point and the projection of
 *		the vector to the test point onto the link scaled by the link length.
 */
		s = i - 1;
		pointDelta = i_testPoint - d_link[s].d_newParticleInWorld;
		linkDelta = d_link[i].d_newParticleInWorld -
				d_link[s].d_newParticleInWorld;
		scaledProjection = pointDelta.Dot(linkDelta);
/*
 *		If the projection is negative, the start point is the closest point on the
 *		link and the search can stop.
 */
		if (scaledProjection <= 0.0f)
			break;
/*
 *		Find the point on the link that is the closest to the point. Clamp if
 *		it is beyond the end point.
 */
		float pointFraction = scaledProjection / linkDelta.LengthSquared();
		if (pointFraction > 1.0f)
			pointFraction = 1.0f;
		float distSq = pointDelta.LengthSquared() -
				pointFraction * scaledProjection;
/*
 *		If there is no point on this link that is closer than what has been found,
 *		stop searching; otherwise, save the new closest point info.
 */
		if (distSq >= minDistSq)
			break;
		minDistSq = distSq;
		io_link = i;
		o_pctLinkDist = pointFraction;
		o_chainPoint = d_link[s].d_newParticleInWorld + pointFraction * linkDelta;
	}
/*
 *	Now search backward.
 */
	for (i = homeLink - 1; i > 0; i--)
	{
/*
 *		Find vectors relative to the link start point and the projection of
 *		the vector to the test point onto the link scaled by the link length.
 */
		s = i - 1;
		pointDelta = i_testPoint - d_link[s].d_newParticleInWorld;
		linkDelta = d_link[i].d_newParticleInWorld -
				d_link[s].d_newParticleInWorld;
		scaledProjection = pointDelta.Dot(linkDelta);
/*
 *		If the projection is negative, the start point is the closest point on the
 *		link. See if it is closer.
 */
		if (scaledProjection <= 0.0f)
		{
			float distSq = pointDelta.LengthSquared();
			if (distSq >= minDistSq)
				break;
			minDistSq = distSq;
			o_pctLinkDist = 0.0f;
			o_chainPoint = d_link[s].d_newParticleInWorld;
		}
/*
 *		The closest point is on the link or beyond the end point.
 */
		else
		{
/*
 *			If closest point is beyond the end point, stop searching.
 */
			float linkLengthSq = linkDelta.LengthSquared();
			if (scaledProjection > linkLengthSq)
				break;
/*
 *			Find the point on the link that is the closest to the point.
 */
			float pointFraction = scaledProjection / linkLengthSq;
			float distSq = pointDelta.LengthSquared() -
				pointFraction * scaledProjection;
/*
 *			If there is no point on this link that is closer than what has been
 *			found, stop searching; otherwise, save the new closest point info.
 */
			if (distSq >= minDistSq)
				break;
			minDistSq = distSq;
			io_link = i;
			o_pctLinkDist = pointFraction;
			o_chainPoint = d_link[s].d_newParticleInWorld +
					pointFraction * linkDelta;
		}
	}

	return(minDistSq);
}

#ifdef ENABLE_RECORD
bool CChain::CreateRecordBuffer(int i_numTicks)
{
	if (d_chainRecordBuffer)
		return (false);
	d_chainRecordBuffer = new SChainRecordEntry[i_numTicks];
	if (d_chainRecordBuffer == NULL)
		return (false);
	int i;
	for (i = 0; i < i_numTicks; i++)
	{
		d_chainRecordBuffer[i].d_link =
				new SChainRecordEntry::SRecordLink[d_numLinks];
		if (d_chainRecordBuffer[i].d_link == NULL)
			break;
	}
	if (i < i_numTicks)
	{
		for (i--; i >= 0; i--)
			delete[] d_chainRecordBuffer[i].d_link;
		delete[] d_chainRecordBuffer;
		d_chainRecordBuffer = NULL;
		return (false);
	}
	d_numRecordTicks = i_numTicks;
	return (true);
}

void CChain::DeleteRecordBuffer(void)
{
	if (d_chainRecordBuffer == NULL)
		return;
	for (int i = 0; i < d_numRecordTicks; i++)
		delete[] d_chainRecordBuffer[i].d_link;
	delete[] d_chainRecordBuffer;
	d_chainRecordBuffer = 0;
}

void CChain::RecordState(int i_entry)
{
	if (d_chainRecordBuffer)
	{
		for (int i = 0; i < d_numLinks; i++)
		{
			d_chainRecordBuffer[i_entry].d_link[i].d_newParticleInWorld =
					d_link[i].d_newParticleInWorld;
		}
	}
}

void CChain::LoadState(int i_entry)
{
	if (d_chainRecordBuffer)
	{
		for (int i = 0; i < d_numLinks; i++)
		{
			d_link[i].d_newParticleInWorld = d_chainRecordBuffer[i_entry
					].d_link[i].d_newParticleInWorld;
		}
	}
}
#endif
