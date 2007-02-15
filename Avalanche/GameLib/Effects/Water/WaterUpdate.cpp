//
// Author: Tyler Colbert
// Purpose: This is a subset of the Water class.  See its header for details... This
// particluar portion is the Update() routine as well as the Add/Rem functions...
//

#include "Effects/EffectsPCH.h"

#include "Effects/Water/Water.h"
#include "Effects/Water/WaterAffector.h"
#include "Effects/Water/WaterSprite.h"

void Water::Update(float deltaTime, Graphics4x4& cameraMatrix)
{
	// calculate the eye point...
	// this block should be render time, not update time... ick..
	{
		// What is the camera position in world coordinates
		Vector3 temp;
		temp = cameraMatrix.GetTranslation();
		Vector3 worldCameraPos = temp.InvBasisTransform(cameraMatrix);
		worldCameraPos.Negate();
		// find us
		Vector3 modelPos;
		modelPos = mInstance->Matrix().GetTranslation();
		// bulid eye point
		mEyePoint.FastMultiply(worldCameraPos, mWorldToLocal);
		mEyePoint.x( mEyePoint.x() * mXScale);
		mEyePoint.z( mEyePoint.z() * mZScale);
//		mEyePoint = (worldCameraPos - modelPos).InvBasisTransform(mInstance->mLocalToWorldBasis);
	}

	// figure current time...
	// this is ICKY!  I shouldnt do this...
	mT += deltaTime;

	// TJC -  update water bounding box position (incase the water
	// height changed)
//   GetBoundingBox().BoundingSphere.Center.y(mPool->CurrentSurfaceHeight());
   GetBoundingBox().Min.y(mPool->CurrentSurfaceHeight()-1.0f);
   GetBoundingBox().Max.y(mPool->CurrentSurfaceHeight()+1.0f);
	mInstance->UpdateBoundingVolumesAccurate();

	// run some physics
#ifdef DEBUG_WATER
	if(!Water_Dbg.mDisablePhysics)
#endif
	if(mPhysicsEnabled)
		RunPhysics(deltaTime);

	// update layer opacities...
	WaterLayerModes* layer = mLayerModes;
	for(int i = 0; i < mNumLayers; i++, layer++)
	{
		if(layer->mOpacityOverTime != 0.0f)
		{
			layer->mOpacity += Math::Inverse(layer->mOpacityOverTime) * deltaTime;
			if(layer->mOpacity < 0.0f)
			{
				layer->mOpacity = -layer->mOpacity;
				layer->mOpacityOverTime = -layer->mOpacityOverTime;
			}
			else if(layer->mOpacity > 1.0f)
			{
				layer->mOpacity = 2.0f - layer->mOpacity;
				layer->mOpacityOverTime = -layer->mOpacityOverTime;
			}
		}

		// in case watch turned off fading...
#ifdef ENABLE_WATCHCLIENT
		else
			layer->mOpacity = 1.0f;
#endif
	}
}

// void Water::UpdateAffectorStates(void)
// {
// 	int affidx = mAffectors.Count();
// 	while(affidx--)
// 	{
// 		WaterAffector* affector = mAffectors[affidx];
// 		OBBWorld obb(affector->BoundingBox());

// 		// things we try to find out..
// 		bool xzOverlap = false;
// 		bool vtxAbove = false;
// 		bool vtxBelow = false;

// 		// copy state to prior
// 		affector->mPriorState = affector->mState;
		
// 		// we need to test rays down from the obb against the water to determine intersection.
// 		// test ray from each corner of the obb (TODO: 4 if obb is not rotated from vertical)

// 		// for each one, check down...
// 		for(int i = 0; i < 8; i++)
// 		{
// 			Vector3 vtx;
// 			vtx.FastMultiply(obb.GetWorldVertex(i), mWorldToLocal);

// 			// see if the vertex overlaps us in xz
// 			if(vtx.x < 0.0f || vtx.x > 1.0f || vtx.z < 0.0f || vtx.z > 1.0f)
// 				continue;

// 			xzOverlap = true;

// 			// see what the height is below it...
// 			float height = GetHeightLocal(vtx.x, vtx.z);
			
// 			if(vtx.y > height)
// 				vtxAbove = true;
// 			else
// 				vtxBelow = true;
// 		}

// 		// update mState...
// 		if(xzOverlap)
// 		{
// 			if(vtxAbove)
// 			{
// 				if(vtxBelow)
// 				{
// 					affector->mState = WaterAffector::INTERSECTS_WATER;
// 				}
// 				else
// 				{
// 					affector->mState = WaterAffector::ABOVE_WATER;
// 				}
// 			}
// 			else
// 			{
// 				affector->mState = WaterAffector::UNDER_WATER;
// 			}
// 		}
// 		else
// 		{
// 			affector->mState = WaterAffector::OUT_OF_WATER;
// 		}
// 	}
// }


float Water::BumpSegmentHeight(Bump* bump, float x, float z)
{
	// find the point one the line closest to the passed point...
	Vector3 p, pdir, dir, start, end, closest;

	p.Set(x, 0.0f, z);
	start = bump->mReferencePoint;
	end = bump->mData.mSegment.mEndPoint;

	dir = end - start;
	pdir = p - start;

	float d = dir.Length();
	dir.Normalize();
	float t = dir.Dot(pdir);

	if(t < 0) closest = start;
	else if(t > d) closest = end;
	else closest = start + dir * t;

	// determine the distance between the two points.
	return (p - closest).LengthXZ();
}

float Water::BumpSemiCircleHeight(Bump* bump, float x, float z)
{
	// find the point on the circle closest to the passed point...
	Vector3 closest;
	Vector3 p;
	p.Set(x, 0.0f, z);

	Vector3 qc = p - Vector3(bump->mReferencePoint);
	qc.Normalize();
	closest = ((Vector3)bump->mReferencePoint) + bump->mData.mSemiCircle.mRadius * qc;

	// clamp the point to the semicircles arc
	Vector3 up, right;
	up.Set(0.0f, 0.0f, 1.0f);
	right.Set(1.0f, 0.0f, 0.0f);
	
	float angle = Math::ArcCos(right.Dot(qc)); // cos(upangle)

	// reflect to whole circle...
	if(up.Dot(qc) < 0.0f)
		angle = Math::TwoPi - angle;

	// make sure its > than startangle
	if(angle < bump->mData.mSemiCircle.mStartAngle)
		angle += Math::TwoPi;

	// if its > endangle, then were outside the arc
	if(angle > bump->mData.mSemiCircle.mEndAngle)
	{
		// outside the arc.. clamp to arc..
		if(angle - bump->mData.mSemiCircle.mEndAngle > angle - bump->mData.mSemiCircle.mStartAngle - Math::TwoPi)
			angle = bump->mData.mSemiCircle.mEndAngle;
		else
			angle = bump->mData.mSemiCircle.mStartAngle;

		// recalc the point...
		closest.Set(Math::Sin(angle), 0.0f, Math::Cos(angle));
	}

	// determine the distance between the two points.
	return (p - closest).LengthXZ();
}

float Water::BumpHeight(float x, float z)
{
	// find the average of all the heights of contributing bump objects
	float totalHeight = 0.0f;
	int numContribuors = 0;

	// loop through the bump objects..
	for(int i = 0; i < mBumps.Count(); i++)
	{
		Bump* b = &mBumps[i];
		float dist = 0.0f;

		// handle the object appropriately..
		switch(b->type)
		{
		case BUMP_SEGMENT:
			dist = BumpSegmentHeight(b, x, z);
			break;
		case BUMP_SEMICIRCLE:
			dist = BumpSemiCircleHeight(b, x, z);
			break;
		}
		if(dist < b->mRange)
		{
			float height;

			// build height...
			dist = (b->mRange - dist) / b->mRange; // 0 - 1
			dist *= Math::HalfPi; // 0 - Pi/2
			height = Math::Sin(dist); // 1 - 0
			height *= b->mHeight; // mHeight - 0

			// contribute height
			totalHeight += height;
			numContribuors++;
		}
	}

	if(numContribuors == 0)
		return 0.0f;

	return totalHeight / numContribuors;
}
