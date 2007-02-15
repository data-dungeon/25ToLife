/*
**
**  File:   ObbBonesNode.cpp
**  Date:   July 12, 2004
**  By:     Bryant Collard
**  Desc:   A collision node made up of a parent OBB and child OBBs attached
**          to bones.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/CollideHelper/ObbBonesNode.cpp $
**  $Revision: #7 $
**  $DateTime: 2005/03/29 17:16:52 $
**    $Author: Rob $
**
*/

// Pre-compiled header
#include "CollideHelper/CollideHelperPCH.h"

// My header (always first)
#include "CollideHelper/ObbBonesNode.h"

// Engine includes //
#include "Game/Database/Som.h"     // For INVALID_MATERIALINDEX

// GameLib headers
#include "stage/protagonist.h"
#include "stage/SurfaceType.h"
#include "geomUtil/BoxExtents.h"
#include "EngineHelper/drawutil.h"

/////////////////////////////// CAttachedObbNode ///////////////////////////////

/******************************************************************************/
/******************************************************************************/
CAttachedObbNode::CAttachedObbNode()
{
	d_actor = NULL;
	d_bone = NULL;
	d_obbExtentInBone.Set(0.0f, 0.0f, 0.0f);
	d_obbToBone.Identity();
	d_obbInActor.Clear();
	d_obbToActor.Identity();
	d_obb.GetAABB().Set(d_obbExtentInBone.Negated(), d_obbExtentInBone);
	d_obb.UpdateRadius();
	d_scaleUsed = true;
	d_surfaceType = INVALID_MATERIALINDEX;
}

/******************************************************************************/
/******************************************************************************/
void CAttachedObbNode::TestExtents(CBoxExtents &io_boxExtents)
{
	io_boxExtents.TestExtentsInTarget(d_obb.GetConstAABB().maxV, d_obbInActor,
			d_obbToActor);
}

/******************************************************************************/
/******************************************************************************/
void CAttachedObbNode::Setup(CActor* i_actor, ts_Bone* i_bone)
{
	d_actor = i_actor;
	d_bone = i_bone;
	if (d_bone == NULL)
		return;

	d_obbExtentInBone = 0.5f * (i_bone->pModelBone->mBoneBox.Max -
			i_bone->pModelBone->mBoneBox.Min);
	d_obbToBone.Identity();
	d_obbToBone.PokeTranslation(i_bone->pModelBone->mBoneBox.Min +
			d_obbExtentInBone);

	if (!d_scaleUsed)
		SyncSize(Vector3(1.0f, 1.0f, 1.0f));

	SyncWithBone();
}

/******************************************************************************/
/******************************************************************************/
void CAttachedObbNode::SetObbInBone(Vector3CRef i_obbInBone)
{
	d_obbToBone.PokeTranslation(i_obbInBone);
	SyncWithBone();
}

/******************************************************************************/
/******************************************************************************/
void CAttachedObbNode::MoveObbInBone(Vector3CRef i_offsetInBone)
{
	d_obbToBone.CatTranslation(i_offsetInBone.X(), i_offsetInBone.Y(),
			i_offsetInBone.Z());
	SyncWithBone();
}

/******************************************************************************/
/******************************************************************************/
void CAttachedObbNode::SetObbToBone(const DirCos3x3 &i_obbToBone)
{
	d_obbToBone.PokeOrientation(i_obbToBone);
	SyncWithBone();
}

/******************************************************************************/
/******************************************************************************/
void CAttachedObbNode::RotateObbToBone(const DirCos3x3 &i_newToOldObb)
{
	d_obbToBone = i_newToOldObb * d_obbToBone;
	SyncWithBone();
}

/******************************************************************************/
/******************************************************************************/
void CAttachedObbNode::SetXSize(float i_size)
{
	Vector3 scale = ComputeScale();
	if (!Math::DivByZero(i_size, scale.X()))
	{
		d_obbExtentInBone.X(i_size / scale.X());
		SyncSize(scale);
	}
}

/******************************************************************************/
/******************************************************************************/
void CAttachedObbNode::SetYSize(float i_size)
{
	Vector3 scale = ComputeScale();
	if (!Math::DivByZero(i_size, scale.Y()))
	{
		d_obbExtentInBone.Y(i_size / scale.Y());
		SyncSize(scale);
	}
}

/******************************************************************************/
/******************************************************************************/
void CAttachedObbNode::SetZSize(float i_size)
{
	Vector3 scale = ComputeScale();
	if (!Math::DivByZero(i_size, scale.Z()))
	{
		d_obbExtentInBone.Z(i_size / scale.Z());
		SyncSize(scale);
	}
}

/******************************************************************************/
/******************************************************************************/
void CAttachedObbNode::SetSize(Vector3CRef i_size)
{
	Vector3 scale = ComputeScale();

	if (!Math::DivByZero(i_size.X(), scale.X()))
		d_obbExtentInBone.X(i_size.X() / scale.X());
	if (!Math::DivByZero(i_size.Y(), scale.Y()))
		d_obbExtentInBone.Y(i_size.Y() / scale.Y());
	if (!Math::DivByZero(i_size.Z(), scale.Z()))
		d_obbExtentInBone.Z(i_size.Z() / scale.Z());

	SyncSize(scale);
}

/******************************************************************************/
/******************************************************************************/
void CAttachedObbNode::SetXSizeInBone(float i_size)
{
	d_obbExtentInBone.X(i_size);
	SyncSize(ComputeScale());
}

/******************************************************************************/
/******************************************************************************/
void CAttachedObbNode::SetYSizeInBone(float i_size)
{
	d_obbExtentInBone.Y(i_size);
	SyncSize(ComputeScale());
}

/******************************************************************************/
/******************************************************************************/
void CAttachedObbNode::SetZSizeInBone(float i_size)
{
	d_obbExtentInBone.Z(i_size);
	SyncSize(ComputeScale());
}

/******************************************************************************/
/******************************************************************************/
void CAttachedObbNode::SetSizeInBone(Vector3CRef i_size)
{
	d_obbExtentInBone = i_size;
	SyncSize(ComputeScale());
}

/******************************************************************************/
/******************************************************************************/
void CAttachedObbNode::ScaleXSize(float i_scale)
{
	d_obbExtentInBone.X(i_scale * d_obbExtentInBone.X());
	float halfSize = i_scale * d_obb.GetAABB().maxV.X();
	d_obb.GetAABB().maxV.X(halfSize);
	d_obb.GetAABB().minV.X(-halfSize);
	d_obb.UpdateRadius();
}

/******************************************************************************/
/******************************************************************************/
void CAttachedObbNode::ScaleYSize(float i_scale)
{
	d_obbExtentInBone.Y(i_scale * d_obbExtentInBone.Y());
	float halfSize = i_scale * d_obb.GetAABB().maxV.Y();
	d_obb.GetAABB().maxV.Y(halfSize);
	d_obb.GetAABB().minV.Y(-halfSize);
	d_obb.UpdateRadius();
}

/******************************************************************************/
/******************************************************************************/
void CAttachedObbNode::ScaleZSize(float i_scale)
{
	d_obbExtentInBone.Z(i_scale * d_obbExtentInBone.Z());
	float halfSize = i_scale * d_obb.GetAABB().maxV.Z();
	d_obb.GetAABB().maxV.Z(halfSize);
	d_obb.GetAABB().minV.Z(-halfSize);
	d_obb.UpdateRadius();
}

/******************************************************************************/
/******************************************************************************/
void CAttachedObbNode::ScaleSize(Vector3CRef i_scale)
{
	d_obbExtentInBone = d_obbExtentInBone.Diag(i_scale);
	d_obb.GetAABB().maxV = d_obb.GetAABB().maxV.Diag(i_scale);
	d_obb.GetAABB().minV = d_obb.GetConstAABB().maxV.Negated();
	d_obb.UpdateRadius();
}

/******************************************************************************/
/******************************************************************************/
void CAttachedObbNode::SetSurfaceType(uint8 i_surfaceType)
{
	d_surfaceType = i_surfaceType;
}

/******************************************************************************/
/******************************************************************************/
void CAttachedObbNode::SetSurfaceName(const char* i_surfaceName)
{
	char name[256];
	strncpy(name, i_surfaceName, 256);
	int surfaceType = g_surfaceType.GetValidDynamicIndex(name, -1, 256);
	d_surfaceType = (surfaceType >= 0) ? surfaceType : INVALID_MATERIALINDEX;
}

/******************************************************************************/
/******************************************************************************/
void CAttachedObbNode::SyncWithBone(void)
{
	ASSERT_PTR(d_actor);

	if (d_bone != NULL)
	{
		// Find the bone to scaled actor transform.
		Graphics4x4 obbTransform;
		boneGetCurrentBoneToModelFromBone(d_bone, &obbTransform);

		// Pre-multiply to get the obb to scaled actor transform.
		obbTransform = d_obbToBone * obbTransform;

		if (d_scaleUsed)
		{
			// Add in any actor scale to get the obb to unscaled actor transform.
			Vector3 &scale = d_actor->Scale();
			obbTransform.CatScale(scale.X(), scale.Y(), scale.Z());

			// Deconstruct
			float scaleX, scaleY, scaleZ, shearXY, shearXZ, shearYZ;
			obbTransform.Deconstruct(d_obbToActor, d_obbInActor, scaleX, scaleY,
					scaleZ, shearXY, shearXZ, shearYZ);

			// Find the obb size ignoring any shear.
			d_obb.GetAABB().maxV.Set(scaleX * d_obbExtentInBone.X(),
					scaleY * d_obbExtentInBone.Y(), scaleZ * d_obbExtentInBone.Z());
			d_obb.GetAABB().minV = d_obb.GetConstAABB().maxV.Negated();
			d_obb.UpdateRadius();
		}
		else
		{
			d_obbInActor = obbTransform.GetTranslation();
			obbTransform.GetOrientation(d_obbToActor);
		}
	}

	SyncWithActor();
}

/******************************************************************************/
/******************************************************************************/
void CAttachedObbNode::SyncWithActor(void)
{
	d_obb.Position() = d_obbInActor * d_actor->GetBodyToWorld() +
			d_actor->GetBodyInWorld();
	d_obb.Orientation() = d_obbToActor * d_actor->GetBodyToWorld();
}

/******************************************************************************/
/******************************************************************************/
Vector3 CAttachedObbNode::ComputeScale(void)
{
	Vector3 scale;

	if (d_scaleUsed && (d_bone != NULL))
	{
		ASSERT_PTR(d_actor);

		// Find the bone to scaled actor transform.
		Graphics4x4 obbTransform;
		boneGetCurrentBoneToModelFromBone(d_bone, &obbTransform);

		// Pre-multiply to get the obb to scaled actor transform.
		obbTransform = d_obbToBone * obbTransform;

		// Add in any actor scale to get the obb to unscaled actor transform.
		Vector3 &scale = d_actor->Scale();
		obbTransform.CatScale(scale.X(), scale.Y(), scale.Z());

		// Deconstruct
		float scaleX, scaleY, scaleZ, shearXY, shearXZ, shearYZ;
		DirCos3x3 obbToActor;
		Vector3 obbInActor;
		obbTransform.Deconstruct(obbToActor, obbInActor, scaleX, scaleY,
				scaleZ, shearXY, shearXZ, shearYZ);
		scale.Set(scaleX, scaleY, scaleZ);
	}
	else
		scale.Set(1.0f, 1.0f, 1.0f);

	return(scale);
}

/******************************************************************************/
/******************************************************************************/
void CAttachedObbNode::SyncSize(Vector3CRef i_scale)
{
	d_obb.GetAABB().maxV.Set(i_scale.X() * d_obbExtentInBone.X(),
			i_scale.Y() * d_obbExtentInBone.Y(),
			i_scale.Z() * d_obbExtentInBone.Z());
	d_obb.GetAABB().minV = d_obb.GetConstAABB().maxV.Negated();
	d_obb.UpdateRadius();
}

//////////////////////////////// CObbBonesNode /////////////////////////////////

/******************************************************************************/
/******************************************************************************/
CObbBonesNode::CObbBonesNode()
{
	d_fitToAttached = false;
	d_scaleUsed = true;
	d_numAttached = 0;
	d_attached = NULL;
	d_currTest = d_numAttached;
	d_clampMin.Set(-MAXFLOAT, -MAXFLOAT, -MAXFLOAT);
	d_clampMax.Set(MAXFLOAT, MAXFLOAT, MAXFLOAT);
}

/******************************************************************************/
/******************************************************************************/
CObbBonesNode::~CObbBonesNode()
{
	delete[] d_attached;
}

/******************************************************************************/
/******************************************************************************/
void CObbBonesNode::EndObjectTest(ETestResult i_result, CCollisionPair* i_pair)
{
	if (i_result == INTERSECTION_IGNORED)
		d_currTest = -1;
	else
		d_currTest = d_numAttached;
}

/******************************************************************************/
/******************************************************************************/
CCollisionNode* CObbBonesNode::GetChild(void)
{
	if (++d_currTest < d_numAttached)
		return(&d_attached[d_currTest]);

	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
void CObbBonesNode::SyncWithActor(void)
{
	for (int i = 0; i < d_numAttached; i++)
		d_attached[i].SyncWithActor();
	CObbNode::SyncWithActor();
}

/******************************************************************************/
/******************************************************************************/
void CObbBonesNode::SyncWithAnimation(void)
{
	// Sync the attached OBBs to their bones.
	int i;
	for (i = 0; i < d_numAttached; i++)
		d_attached[i].SyncWithBone();

	// Fit the parent OBB to the attached OBBs.
	if (d_fitToAttached && (d_numAttached > 0))
	{
		CBoxExtents boxExtents;
		boxExtents.SetTargetToRef(Vector3(0.0f, 0.0f, 0.0f), d_obbToActor);
		for (i = 0; i < d_numAttached; i++)
			d_attached[i].TestExtents(boxExtents);

		Vector3 minExtent = Vector3::Max(d_clampMin,
				Vector3::Min(boxExtents.GetMinExtent(), d_clampMax));
		Vector3 maxExtent = Vector3::Min(d_clampMax,
				Vector3::Max(boxExtents.GetMaxExtent(), d_clampMin));

		Vector3 extent = 0.5f * (maxExtent - minExtent);
		d_obb.GetAABB().Set(extent.Negated(), extent);
		d_obb.UpdateRadius();
		d_obbInActor = minExtent + extent;
	}

	// Sync the parent OBB with the actor.
	CObbNode::SyncWithActor();
}

/******************************************************************************/
/******************************************************************************/
uint8 CObbBonesNode::Material(CCollisionPair* i_pair)
{
	if (i_pair != NULL)
	{
		CCollisionObject* object;
		if (i_pair->GetEvent()->GetQueryEntity() == (CStageEntity*)d_protagonist)
			object = i_pair->GetQueryObject();
		else
			object = i_pair->GetTargetObject();

		for (int i = 0; i < d_numAttached; i++)
		{
			if (d_attached[i].GetCollisionObject() == object)
				return(d_attached[i].Material());
		}
	}

	return(CObbNode::Material(i_pair));
}

/******************************************************************************/
/******************************************************************************/
void CObbBonesNode::Draw(CCollisionEvent* i_event)
{
	d_obb.Draw(DrawUtility::WHITE);

	if (i_event != NULL)
	{
		if (i_event->GetQueryEntity() == (CStageEntity*)d_protagonist)
		{
			CCollisionPair* pair = i_event->GetHeadPair();
			while (pair != NULL)
			{
				if (pair->GetQueryObject() != (CCollisionObject*)(&d_obb))
					pair->GetQueryObject()->Draw(DrawUtility::RED);
				pair = pair->GetNext();
			}
		}
		else
		{
			CCollisionPair* pair = i_event->GetHeadPair();
			while (pair != NULL)
			{
				if (pair->GetTargetObject() != (CCollisionObject*)(&d_obb))
					pair->GetTargetObject()->Draw(DrawUtility::RED);
				pair = pair->GetNext();
			}
		}
	}
}

/******************************************************************************/
/******************************************************************************/
void CObbBonesNode::SetCollisionInActor(Vector3CRef i_collisionInActor)
{
	if (!d_fitToAttached)
		CObbNode::SetCollisionInActor(i_collisionInActor);
}

/******************************************************************************/
/******************************************************************************/
void CObbBonesNode::MoveCollisionInActor(Vector3CRef i_offsetInActor)
{
	if (!d_fitToAttached)
		CObbNode::MoveCollisionInActor(i_offsetInActor);
}

/******************************************************************************/
/******************************************************************************/
void CObbBonesNode::SetXSize(float i_size)
{
	if (!d_fitToAttached)
		CObbNode::SetXSize(i_size);
}

/******************************************************************************/
/******************************************************************************/
void CObbBonesNode::SetYSize(float i_size)
{
	if (!d_fitToAttached)
		CObbNode::SetYSize(i_size);
}

/******************************************************************************/
/******************************************************************************/
void CObbBonesNode::SetZSize(float i_size)
{
	if (!d_fitToAttached)
		CObbNode::SetZSize(i_size);
}

/******************************************************************************/
/******************************************************************************/
void CObbBonesNode::SetSize(Vector3CRef i_size)
{
	if (!d_fitToAttached)
		CObbNode::SetSize(i_size);
}

/******************************************************************************/
/******************************************************************************/
void CObbBonesNode::ScaleXSize(float i_scale)
{
	if (!d_fitToAttached)
		CObbNode::ScaleXSize(i_scale);
}

/******************************************************************************/
/******************************************************************************/
void CObbBonesNode::ScaleYSize(float i_scale)
{
	if (!d_fitToAttached)
		CObbNode::ScaleYSize(i_scale);
}

/******************************************************************************/
/******************************************************************************/
void CObbBonesNode::ScaleZSize(float i_scale)
{
	if (!d_fitToAttached)
		CObbNode::ScaleZSize(i_scale);
}

/******************************************************************************/
/******************************************************************************/
void CObbBonesNode::ScaleSize(Vector3CRef i_scale)
{
	if (!d_fitToAttached)
		CObbNode::ScaleSize(i_scale);
}

/******************************************************************************/
/******************************************************************************/
int CObbBonesNode::SetupAttached(const char** i_boneNames, int i_numBones)
{
	ASSERT_PTR(d_protagonist);

	if (i_numBones < 0)
	{
		i_numBones = 0;
		while ((i_boneNames[i_numBones] != NULL) &&
				(i_boneNames[i_numBones][0] != '\0'))
			i_numBones++;
	}

	delete[] d_attached;
	d_numAttached = i_numBones;
	if (d_numAttached == 0)
		d_attached = NULL;
	else
	{
		d_attached = new CAttachedObbNode[d_numAttached];
		ASSERT_PTR(d_attached);
		for (int i = 0; i < d_numAttached; i++)
		{
			d_attached[i].SetScaleUsed(d_scaleUsed);
			d_attached[i].Setup(d_protagonist, GetBone(i_boneNames[i]));
		}
	}

	SyncWithAnimation();

	return(d_numAttached);
}

/******************************************************************************/
/******************************************************************************/
void CObbBonesNode::SetScaleUsed(bool i_scaleUsed)
{
	d_scaleUsed = i_scaleUsed;
	for (int i = 0; i < d_numAttached; i++)
		d_attached[i].SetScaleUsed(i_scaleUsed);
}

/******************************************************************************/
/******************************************************************************/
void CObbBonesNode::SetObbInBones(Vector3Packed* i_obbInBone, int i_stride)
{
	char *obbInBone = (char*)i_obbInBone;
	for (int i = 0; i < d_numAttached; i++)
	{
		d_attached[i].SetObbInBone(*(Vector3Packed*)obbInBone);
		obbInBone += i_stride;
	}
}

/******************************************************************************/
/******************************************************************************/
void CObbBonesNode::MoveObbInBones(Vector3Packed* i_offsetInBone, int i_stride)
{
	char *offsetInBone = (char*)i_offsetInBone;
	for (int i = 0; i < d_numAttached; i++)
	{
		d_attached[i].MoveObbInBone(*(Vector3Packed*)offsetInBone);
		offsetInBone += i_stride;
	}
}

/******************************************************************************/
/******************************************************************************/
void CObbBonesNode::SetObbToBones(DirCos3x3* i_obbToBone, int i_stride)
{
	char *obbToBone = (char*)i_obbToBone;
	for (int i = 0; i < d_numAttached; i++)
	{
		d_attached[i].SetObbToBone(*(DirCos3x3*)obbToBone);
		obbToBone += i_stride;
	}
}

/******************************************************************************/
/******************************************************************************/
void CObbBonesNode::RotateObbToBones(DirCos3x3* i_newToOldObb, int i_stride)
{
	char *newToOldObb = (char*)i_newToOldObb;
	for (int i = 0; i < d_numAttached; i++)
	{
		d_attached[i].RotateObbToBone(*(DirCos3x3*)newToOldObb);
		newToOldObb += i_stride;
	}
}

/******************************************************************************/
/******************************************************************************/
void CObbBonesNode::SetSizes(Vector3Packed* i_size, int i_stride)
{
	char *size = (char*)i_size;
	for (int i = 0; i < d_numAttached; i++)
	{
		d_attached[i].SetSize(*(Vector3Packed*)size);
		size += i_stride;
	}
}

/******************************************************************************/
/******************************************************************************/
void CObbBonesNode::SetSizeInBones(Vector3Packed* i_size, int i_stride)
{
	char *size = (char*)i_size;
	for (int i = 0; i < d_numAttached; i++)
	{
		d_attached[i].SetSizeInBone(*(Vector3Packed*)size);
		size += i_stride;
	}
}

/******************************************************************************/
/******************************************************************************/
void CObbBonesNode::ScaleSizes(Vector3Packed* i_scale, int i_stride)
{
	char *scale = (char*)i_scale;
	for (int i = 0; i < d_numAttached; i++)
	{
		d_attached[i].ScaleSize(*(Vector3Packed*)scale);
		scale += i_stride;
	}
}

/******************************************************************************/
/******************************************************************************/
void CObbBonesNode::SetSurfaceTypes(uint8 *i_surfaceType, int i_stride)
{
	char *surfaceType = (char*)i_surfaceType;
	for (int i = 0; i < d_numAttached; i++)
	{
		d_attached[i].SetSurfaceType(*(uint8*)surfaceType);
		surfaceType += i_stride;
	}
}

/******************************************************************************/
/******************************************************************************/
void CObbBonesNode::SetSurfaceNames(const char* i_name, int i_stride)
{
	char *name = (char*)i_name;
	for (int i = 0; i < d_numAttached; i++)
	{
		d_attached[i].SetSurfaceName((const char*)name);
		name += i_stride;
	}
}

/******************************************************************************/
/******************************************************************************/
CAttachedObbNode* CObbBonesNode::GetAttached(const char* i_name)
{
	if ((i_name == NULL) || (i_name[0] == '\0'))
		return(NULL);

	for (int i = 0; i < d_numAttached; i++)
	{
		ts_Bone* bone = d_attached[i].GetBone();
		if ((bone != NULL) && !stricmp(i_name, bone->pModelBone->cBoneName))
			return(&d_attached[i]);
	}

	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
CAttachedObbNode* CObbBonesNode::GetAttached(int i_index)
{
	if ((i_index >= 0) && (i_index < d_numAttached))
		return(&d_attached[i_index]);

	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
ts_Bone* CObbBonesNode::GetBone(const char* i_name)
{
	if ((i_name == NULL) || (i_name[0] == '\0'))
		return(NULL);

	for (int i = 0; i < MAX_ACTOR_INSTANCES; i++)
	{
		if ( d_protagonist->Instance( i ) )
		{
			ts_Body* body = d_protagonist->Instance( i )->Body();
			ts_Bone* bone;
			if (boneGetBone(body, &bone, i_name) != 0)
				return(bone);
		}
	}

	return(NULL);
}
