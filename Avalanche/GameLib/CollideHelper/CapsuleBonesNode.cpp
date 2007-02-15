// Pre-compiled header
#include "CollideHelper/CollideHelperPCH.h"

// My header (always first)
#include "CollideHelper/CapsuleBonesNode.h"

// Engine includes //
#include "Game/Database/Som.h"     // For INVALID_MATERIALINDEX

// GameLib headers
#include "stage/protagonist.h"
#include "stage/SurfaceType.h"
#include "geomUtil/BoxExtents.h"
#include "EngineHelper/drawutil.h"

//////////////////////////////// CCapsuleBonesNode /////////////////////////////////

/******************************************************************************/
/******************************************************************************/
CCapsuleBonesNode::CCapsuleBonesNode()
{
	d_fitToAttached = false;
	d_scaleUsed = true;
	d_numAttached = 0;
	d_attached = NULL;
	d_currTest = d_numAttached;
	minAxis = 0.0f;
	maxAxis = 0.0f;
	minRadius = 0.0f;
	maxRadius = 0.0f;
}

/******************************************************************************/
/******************************************************************************/
CCapsuleBonesNode::~CCapsuleBonesNode()
{
	delete[] d_attached;
}

/******************************************************************************/
/******************************************************************************/
void CCapsuleBonesNode::EndObjectTest(ETestResult i_result, CCollisionPair* i_pair)
{
	if (i_result == INTERSECTION_IGNORED)
		d_currTest = -1;
	else
		d_currTest = d_numAttached;
}

/******************************************************************************/
/******************************************************************************/
CCollisionNode* CCapsuleBonesNode::GetChild(void)
{
	if (++d_currTest < d_numAttached)
		return(&d_attached[d_currTest]);

	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
void CCapsuleBonesNode::SyncWithActor(void)
{
	for (int i = 0; i < d_numAttached; i++)
		d_attached[i].SyncWithActor();
	CapsuleNode::SyncWithActor();
}

/******************************************************************************/
/******************************************************************************/
void CCapsuleBonesNode::SyncWithAnimation(void)
{
	// Sync the attached OBBs to their bones.
	int i;
	for (i = 0; i < d_numAttached; i++)
		d_attached[i].SyncWithBone();

	// Fit the parent OBB to the attached OBBs.
	if (d_fitToAttached && (d_numAttached > 0))
	{
		CBoxExtents boxExtents;
		DirCos3x3 ident;
		ident.Identity();
		// I'm sure there's some optimisation we could do here because we're using
		// an identity matrix, but I don't have time to figure out how all this
		// extent stuff really works
		boxExtents.SetTargetToRef(Vector3(0.0f, 0.0f, 0.0f), ident);
		for (i = 0; i < d_numAttached; i++)
			d_attached[i].TestExtents(boxExtents);

		float a0, a1, r;
		Vector3 min, max, center;
		min = boxExtents.GetMinExtent();
		max = boxExtents.GetMaxExtent();
		center = (min + max) * 0.5f;
		
		Vector3 dir = d_protagonist->GetBodyToWorld().GetRow1();
		Vector3 origin = d_protagonist->GetBodyInWorld();

		if( d_fixedOffset )
			center = fixedOffset;

		Vector3 minRad = min - center;
		Vector3 maxRad = max - center;
		minRad -= dir * minRad.Dot( dir );
		maxRad -= dir * maxRad.Dot( dir );

		r = Math::Max( minRad.LengthSquared(), maxRad.LengthSquared() );
		r = Math::Sqrt( r );

		r = Math::Max( r, minRadius );
		r = Math::Min( r, maxRadius );

		a0 = min.Dot( dir );
		a1 = max.Dot( dir );
		
		float start, end;
		start = Math::Max( a0, minAxis );
		end = Math::Min( a1, maxAxis );

		start += r;
		end -= (r * 0.75f);
		if( end < start )
			end = start;

		d_originInActor.Set( center.x(), start, center.z() );
		d_endInActor.Set( center.x(), end, center.z() );
		d_radius = r;
	}

	// Sync the parent OBB with the actor.
	CapsuleNode::SyncWithActor();
}

/******************************************************************************/
/******************************************************************************/
uint8 CCapsuleBonesNode::Material(CCollisionPair* i_pair)
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

	return(CapsuleNode::Material(i_pair));
}

/******************************************************************************/
/******************************************************************************/
void CCapsuleBonesNode::Draw(CCollisionEvent* i_event)
{
	capsule.Draw(DrawUtility::WHITE);

	if (i_event != NULL)
	{
		if (i_event->GetQueryEntity() == (CStageEntity*)d_protagonist)
		{
			CCollisionPair* pair = i_event->GetHeadPair();
			while (pair != NULL)
			{
				if (pair->GetQueryObject() != (CCollisionObject*)(&capsule))
					pair->GetQueryObject()->Draw(DrawUtility::RED);
				pair = pair->GetNext();
			}
		}
		else
		{
			CCollisionPair* pair = i_event->GetHeadPair();
			while (pair != NULL)
			{
				if (pair->GetTargetObject() != (CCollisionObject*)(&capsule))
					pair->GetTargetObject()->Draw(DrawUtility::RED);
				pair = pair->GetNext();
			}
		}
	}
}

/******************************************************************************/
/******************************************************************************/
int CCapsuleBonesNode::SetupAttached(const char** i_boneNames, int i_numBones)
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
void CCapsuleBonesNode::SetScaleUsed(bool i_scaleUsed)
{
	d_scaleUsed = i_scaleUsed;
	for (int i = 0; i < d_numAttached; i++)
		d_attached[i].SetScaleUsed(i_scaleUsed);
}

/******************************************************************************/
/******************************************************************************/
void CCapsuleBonesNode::SetObbInBones(Vector3Packed* i_obbInBone, int i_stride)
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
void CCapsuleBonesNode::MoveObbInBones(Vector3Packed* i_offsetInBone, int i_stride)
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
void CCapsuleBonesNode::SetObbToBones(DirCos3x3* i_obbToBone, int i_stride)
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
void CCapsuleBonesNode::RotateObbToBones(DirCos3x3* i_newToOldObb, int i_stride)
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
void CCapsuleBonesNode::SetSizes(Vector3Packed* i_size, int i_stride)
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
void CCapsuleBonesNode::SetSizeInBones(Vector3Packed* i_size, int i_stride)
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
void CCapsuleBonesNode::ScaleSizes(Vector3Packed* i_scale, int i_stride)
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
void CCapsuleBonesNode::SetSurfaceTypes(uint8 *i_surfaceType, int i_stride)
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
void CCapsuleBonesNode::SetSurfaceNames(const char* i_name, int i_stride)
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
CAttachedObbNode* CCapsuleBonesNode::GetAttached(const char* i_name)
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
CAttachedObbNode* CCapsuleBonesNode::GetAttached(int i_index)
{
	if ((i_index >= 0) && (i_index < d_numAttached))
		return(&d_attached[i_index]);

	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
ts_Bone* CCapsuleBonesNode::GetBone(const char* i_name)
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
