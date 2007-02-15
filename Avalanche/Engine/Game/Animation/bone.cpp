/******************* includes ****************************/

#include "Game/GamePCH.h"





void boneDirtyTree(ts_Bone *pBone);
void boneRunBones( ts_Bone *pBone);
void boneOrientLikeBone( ts_Bone *pBone);
void boneScaleMatrix(ts_Bone *pBone);
void boneIdentityTree(ts_Bone *pBone);


/* Set a bone and it's entire offspring tree to identity */

void boneSetBoneTreeIdentity(ts_Bone *pBone)
{
	boneIdentityTree( pBone );

	boneFinalRunBones( pBone->pBody );
}



void boneIdentityTree(ts_Bone *pBone)
{
	if(pBone && !pBone->bIdentity)
	{
		pBone->LocalBoneMatrix.Identity();
		pBone->bIdentity = true;
		pBone->bLocalMatrixChanged = TRUE;
	}

	if (pBone->pChildren)
		boneIdentityTree(pBone->pChildren);

	if (pBone->pSiblings)
		boneIdentityTree(pBone->pSiblings);
}



void boneDirtyLocalMatrix(ts_Bone *pBone)
{
	if (pBone)
	{
		pBone->bIdentity = false;

		if(!pBone->bLocalMatrixChanged)
		{
			pBone->bLocalMatrixChanged = TRUE;

			if (pBone->pChildren)
				boneDirtyTree(pBone->pChildren);
		}

		pBone->pBody->updatePending = true;
	}

	SceneMgr::GetStats().IncBonesComputed( 1 );
}

void boneDirtyTree(ts_Bone *pBone)
{
	if (!pBone->bLocalMatrixChanged)
	{
		pBone->bLocalMatrixChanged = TRUE;
		if (pBone->pChildren)
			boneDirtyTree(pBone->pChildren);
	}

	if (pBone->pSiblings)
		boneDirtyTree(pBone->pSiblings);
}

void boneSetVisibility(ts_Bone *pBone, bool bVisible)
{
	if (pBone)
	{
		uint nScale = (bVisible) ? SCALE_ONE : SCALE_ZERO;

		if (pBone->scale != nScale)
		{
			pBone->scale = nScale;
			if(nScale == SCALE_ONE)
				boneSetLocalMatrixToIdentity(pBone);
			if (!pBone->bLocalMatrixChanged)
				{
					pBone->bLocalMatrixChanged = TRUE;
					if (pBone->pChildren)
						boneDirtyTree(pBone->pChildren);
				}
		
			pBone->pBody->updatePending = true;
		}
	}
}

void boneSetScale(ts_Bone *pBone, float scale)
{
	ASSERT(pBone);
	//convert float to integer range
	ASSERT(scale >= 0 && scale < SCALE_MAX);

	uint nScale = (uint) (scale * SCALE_ONE);
	if(nScale > SCALE_MAX)
		nScale = SCALE_MAX;

	if (pBone && pBone->scale != nScale)
	{
		pBone->scale = nScale;
		if (!pBone->bLocalMatrixChanged)
		{
			pBone->bLocalMatrixChanged = TRUE;
			if (pBone->pChildren)
				boneDirtyTree(pBone->pChildren);
		}
		pBone->pBody->updatePending = true;
	}

}

void boneScaleMatrix(ts_Bone *pBone)
{
	Graphics4x4 m;
	float scale = pBone->scale * (1.0f / SCALE_ONE);
	m.SetScale(scale, scale, scale);
	m.PokeTranslation((1.0f - scale) * pBone->pModelBone->boneToModel.GetTranslation());

	pBone->Matrix.FastMultiply(m, pBone->Matrix);
	pBone->pBody->updatePending = true;
}

void boneSetLocalMatrix(ts_Bone *pBone, Graphics4x4 *mLocalBoneMatrix)
{
	if(pBone)
	{
		//memcpy( pBone->pLocalBoneMatrix, mLocalBoneMatrix, sizeof(Graphics4x4));
		pBone->LocalBoneMatrix = *mLocalBoneMatrix;
		pBone->bIdentity = false;

		if (!pBone->bLocalMatrixChanged)
		{
			pBone->bLocalMatrixChanged = TRUE;
			if (pBone->pChildren)
				boneDirtyTree(pBone->pChildren);
		}

		pBone->pBody->updatePending = true;
	}
}

void boneSetLocalMatrixToIdentity(ts_Bone *pBone)
{
	if(pBone && !pBone->bIdentity)
	{
		pBone->LocalBoneMatrix.Identity();
		pBone->bIdentity = true;

		if (!pBone->bLocalMatrixChanged)
		{
			pBone->bLocalMatrixChanged = TRUE;
			if (pBone->pChildren)
				boneDirtyTree(pBone->pChildren);
		}

		pBone->pBody->updatePending = true;

	}
}

void boneFinalRunBones( ts_Body *pBody)
{
	ASSERT(pBody);	// if we are looking for a bone, actor must have a body
	ts_Bone *pBone = pBody->pBones;
	if (pBone->bLocalMatrixChanged)
	{
		pBone->Matrix = pBone->LocalBoneMatrix;

		if(pBone->scale != SCALE_ONE)
			boneScaleMatrix(pBone);

		pBone->bLocalMatrixChanged = FALSE;
	}

	if( pBone->pChildren)
		boneRunBones( pBone->pChildren);

	pBody->updatePending = false;
}

void boneRunBones( ts_Bone *pBone)
{
#if 0 //def _DEBUG //verify that local matrices flagged as identity really are
	if( pBone->bIdentity)
	{
		ASSERT(Math::Zero(pBone->LocalBoneMatrix.e00 - 1.0f, 0.00001f));
		ASSERT(Math::Zero(pBone->LocalBoneMatrix.e01, 0.00001f));
		ASSERT(Math::Zero(pBone->LocalBoneMatrix.e02, 0.00001f));
		ASSERT(Math::Zero(pBone->LocalBoneMatrix.e10, 0.00001f));
		ASSERT(Math::Zero(pBone->LocalBoneMatrix.e11 - 1.0f, 0.00001f));
		ASSERT(Math::Zero(pBone->LocalBoneMatrix.e12, 0.00001f));
		ASSERT(Math::Zero(pBone->LocalBoneMatrix.e20, 0.00001f));
		ASSERT(Math::Zero(pBone->LocalBoneMatrix.e21, 0.00001f));
		ASSERT(Math::Zero(pBone->LocalBoneMatrix.e22 - 1.0f, 0.00001f));
		ASSERT(Math::Zero(pBone->LocalBoneMatrix.e30, 0.00001f));
		ASSERT(Math::Zero(pBone->LocalBoneMatrix.e31, 0.00001f));
		ASSERT(Math::Zero(pBone->LocalBoneMatrix.e32, 0.00001f));
	}
#endif
	
	if( pBone->pModelBone->OrientBone )
		boneOrientLikeBone(pBone);

	if (pBone->bLocalMatrixChanged)
	{
		pBone->Matrix.FastMultiply(pBone->LocalBoneMatrix, pBone->m_parent->Matrix);

		if(pBone->scale != SCALE_ONE)
			boneScaleMatrix(pBone);

		pBone->bLocalMatrixChanged = FALSE;
		SceneMgr::GetStats().IncBonesUpdated( 1 );
	}

	if( pBone->pChildren)
		boneRunBones( pBone->pChildren);

	if( pBone->pSiblings)
		boneRunBones( pBone->pSiblings);
}

void boneOrientLikeBone( ts_Bone *pBone)
{
	ts_Quatvw quatFrom;
	ts_Quatvw quatTo;
	ts_Quatvw quatOut;
	
	Cartesian4x4 m, m2;

	ts_ModelBone *pMBone = pBone->pModelBone;

	//get oriented-like bone's orientation in quat form
	qatFromMat( boneGetLocalMatrix(boneGetBonePtr(pBone->pBody, pBone->pModelBone->OrientBone)), &quatTo);
	
	if( pMBone->bOrientXYZ )
	{
		//Tyler has requested that we handle orienting one bone like another, but with different vaules for X, Y, and Z
		
		//interpolate around the individual axes
		quatOut.V.Set(pMBone->ucOrientX*0.01f*quatTo.V.x(),
						  pMBone->ucOrientY*0.01f*quatTo.V.y(),
						  pMBone->ucOrientZ*0.01f*quatTo.V.z());
		quatOut.W = 1.0f;
		
		ASSERT(pMBone->Orient2Bone == 0);//we only handle one OrientXYZ bone
	}
	else if( pMBone->ucOrientLikeValue )
		{
			//orient-like move a percetnage of orient-like bone about all 3 axes
			
			//reset all 3 axes
			qatIdentity( &quatFrom);
			
			//reduce them by the given percentage
			qatSlerpBetter(&quatFrom, &quatTo, pMBone->ucOrientLikeValue*0.01f, &quatOut);
			
			//Tyler has requested the ability to orient a bone like TWO bones...here do the other if it exists
			if( pMBone->ucOrient2LikeValue && pMBone->Orient2Bone)
			{
				ts_Quatvw quatOut1 = quatOut;
				ts_Quatvw quatOut2;
				
				//get the second orient-like bones orientation
				qatFromMat( boneGetLocalMatrix(boneGetBonePtr(pBone->pBody, pMBone->Orient2Bone)), &quatTo);
				
				//reset all 3 axes
				qatIdentity( &quatFrom);
				
				//reduce them by the given percentage
				qatSlerpBetter(&quatFrom, &quatTo, pMBone->ucOrient2LikeValue*0.01f, &quatOut2);
				
				// and combine the results.
				qatMul( &quatOut1, &quatOut2, &quatOut );
			}
		}
		else if( pMBone->ucRollLikeValue )
			{
				//roll-like: this bone should resemble it's parent in all respects except it's roll component should be x% of it's roll-like bone
				qatIdentity( &quatOut);	//set to inherit parent's orientation
				
				//interpolate around the Z-axis
				quatOut.V.x(pMBone->ucRollLikeValue*0.01f*quatTo.V.x());//djr: I'm not sure why the first quat value (v[0]) ends up rotating about z, but it does...

				ASSERT(pMBone->Orient2Bone == 0);//we only handle one roll-like bone
			}
	
	//and convert back to a matrix
	qatToRotMat(&quatOut, &m2);

	//apply this rotation: first translate to the origin...
	m.SetTranslation(pMBone->vPivot.x(), pMBone->vPivot.y(), pMBone->vPivot.z());
	
	//rotate...
	m2.FastMultiply(m2, m);
	
	//and translate back...
	Vector3 revTrans = -pMBone->vPivot;
	m2.ReverseCatTranslation(revTrans.x(), revTrans.y(), revTrans.z());

	//and voila, our new, orient-like local bone matrix
	boneSetLocalMatrix( pBone, &m2);

}

//run bones up to a specified EndBone, and return the concatenated matrix up to (and including?) that bone
Graphics4x4* boneRunBonesToBone( ts_Bone *pBone)
{
	ASSERT(pBone);

	if (pBone->bLocalMatrixChanged)
	{
		if (pBone->m_parent)
		{
			boneRunBonesToBone(pBone->m_parent);
			pBone->Matrix.FastMultiply(pBone->LocalBoneMatrix, pBone->m_parent->Matrix);
		}
		else
			 pBone->Matrix = pBone->LocalBoneMatrix;

		if(pBone->scale != SCALE_ONE)
			boneScaleMatrix(pBone);

		pBone->bLocalMatrixChanged = FALSE;

		SceneMgr::GetStats().IncBonesUpdated( 1 );
	}
	return(&pBone->Matrix);
}

void boneCopyLocalMatricies( ts_Body *pBody, Graphics4x4 *pMat, bool bExchange )
{
	ts_Bone *pBone = pBody->pBones;
	int i;

	//copy the local matricies to a save buffer.
	//if bExchange is true, exchange the matrices, otherwise copy to the save buffer
	for( i = 0; i < pBody->usBones; i++, pBone++, pMat++)
	{
		if( bExchange)
		{
			Graphics4x4 temp;

			temp = *pMat;
			*pMat = pBone->LocalBoneMatrix;
			pBone->LocalBoneMatrix = temp;
			pBone->bLocalMatrixChanged = TRUE;
		}
		else
			*pMat = pBone->LocalBoneMatrix;
	}
}

/*
 *	The LocalBoneMatrix is not the transformation from a bone's parent to the
 *	bone. It is, instead, multiplied by the parent's composite matrix to form
 *	a composite matrix for the bone. This composite matrix is used to move
 *	associated vertices with a bone. When the location of a vertex in model
 *	space is multiplied by the composite matrix, it results in a new location
 *	in model space whose position relative to the bone in its current pose is
 *	the same as the initial location relative to the bone in the design pose.
 *	If the transformation between a parent and the bone is desired, some
 *	computation needs to take place. If you work through the math, you find that
 *	the transformation from a parent to a bone is equal to the transformation
 *	from the bone in its design pose to the model times the LocalBoneMatrix times
 *	transformation from the model to the parent in its design pose.
 *
 *	Compute the bone to parent transformation.
 */
void boneComputeBoneToParent(ts_Bone *pBone, Graphics4x4 *pMat)
{
	// Multiply the transformation from the bone in its design pose by the
	// local bone matrix.
	pMat->FastMultiply(pBone->pModelBone->boneToModel, pBone->LocalBoneMatrix);

	// If the bone has a parent, multiply the resultant transformation by the
	// transformation from the model to the parent in its design pose.
	if ( pBone->m_parent )
		*pMat /= pBone->m_parent->pModelBone->boneToModel;
}

void boneComputeNeutralBoneToParent(ts_Bone *pBone, Cartesian4x4 *pMat)
{
	if ( pBone->m_parent )
		*pMat = pBone->pModelBone->boneToModel / pBone->m_parent->pModelBone->boneToModel;
	else
		*pMat = pBone->pModelBone->boneToModel;
}

/*
 *	If you solve for the LocalBoneMatrix, it is the transformation from the model
 *	to the bone in its design pose times the current transformation from the
 *	bone to its parent times the transformtation from the parent in its design
 *	pose to the model.
 *
 *	Set the bone to parent transformation.
 */
void boneSetBoneToParent(ts_Bone *pBone, Graphics4x4 *pMat)
{
	if(pBone)
	{
		// Find the transformation from the model to the bone in its design pose then
		// multiply it by the desired bone to parent matrix.
		pBone->LocalBoneMatrix = pBone->pModelBone->boneToModel.InvertTimes(*pMat);

		// If the bone has a parent, multiply the result by the transformation from
		// the parent in its design pose to the model.
		if ( pBone->m_parent )
			pBone->LocalBoneMatrix.FastMultiply(pBone->LocalBoneMatrix, pBone->m_parent->pModelBone->boneToModel);

		// The new local bone matrix is stored in LocalBoneMatrix. Mark the bone
		// as dirty.
		boneDirtyLocalMatrix( pBone );
	}
}

Cartesian4x4 *boneGetBoneToModel(ts_Bone *pBone)
{
	return (&pBone->pModelBone->boneToModel);
}

// Set the transformation from a bone to its design pose.
void boneSetBoneToNeutralBone(ts_Bone *pBone, Graphics4x4 *pMat)
{
	pBone->LocalBoneMatrix = pBone->pModelBone->boneToModel.InvertTimes(*pMat);
	pBone->LocalBoneMatrix.FastMultiply(pBone->LocalBoneMatrix, pBone->pModelBone->boneToModel);

	boneDirtyLocalMatrix( pBone );
}

// Find the transformation from the neutral bone to the model. The neutral bone
// state is where the bone would be if its relationship to its parent is the
// same as the relationship between the bone and its parent in their design
// poses.
void boneComputeCurrentNeutralBoneToModel(ts_Bone *pBone, Graphics4x4 *pMat)
{
	if ( pBone->m_parent )
		pMat->FastMultiply(pBone->pModelBone->boneToModel, *boneRunBonesToBone(pBone->m_parent));
	else
		*pMat = pBone->pModelBone->boneToModel;
}

int boneGetBone(ts_Body *pBody, ts_Bone **pBone, const char *cBoneName)
{
	if(pBody)	// if we are looking for a bone, actor must have a body
	{
		*pBone = pBody->pBones;

		int i;
		for(i = 0; i < pBody->usBones; i++, (*pBone)++)
		{
			//note that this does a CASE-INSENSITIVE string compare
			if( strnicmp(cBoneName, (*pBone)->pModelBone->cBoneName, sizeof((*pBone)->pModelBone->cBoneName)) == 0 )
			{
				return(1);
			}
		}
	}

	//can't find bone
	*pBone = NULL;
	return(0);
}


int boneGetBoneID(ts_Body *pBody, BoneID *boneID, const char *cBoneName)
{
	ASSERT(pBody);	// if we are looking for a bone, actor must have a body
	ts_ModelBone *pBone = pBody->pMBones;

	int i;
	for(i = 0; i < pBody->usBones; i++, pBone++)
	{
		//note that this does a CASE-INSENSITIVE string compare
		if( strnicmp(cBoneName, pBone->cBoneName, sizeof(pBone->cBoneName)) == 0 )
		{
			*boneID = pBone->boneID;
			return(1);
		}
	}

	// It's okay not to find a bone, we return a INVALID_BONEID...let them handle
	// unfound bones.
	// ASSERT( 0 );//let someone know that a bone they looked up wasn't found

	*boneID = INVALID_BONEID;
	return(0);
}


void boneGetMatrix(ts_Body *pBody, BoneID boneID, Graphics4x4 *mLocalBoneMatrix)
{
	ts_Bone *pBone = boneGetBonePtr(pBody, boneID);

	memcpy( mLocalBoneMatrix, boneGetLocalMatrix( pBone ), sizeof(Graphics4x4));
}

void boneSetMatrix(ts_Body *pBody, BoneID boneID, Graphics4x4 *mLocalBoneMatrix)
{
	ts_Bone *pBone = boneGetBonePtr(pBody, boneID);

	boneSetLocalMatrix( pBone, mLocalBoneMatrix);
}



void boneGetCompositeMatrix(ts_Body *pBody, BoneID boneID, Graphics4x4 *mCompositeBoneMatrix)
{
	ts_Bone *pEndBone = boneGetBonePtr(pBody, boneID);

	if(pEndBone)
	{
		*mCompositeBoneMatrix = *boneRunBonesToBone(pEndBone);
	}
}

void boneGetParentCompositeMatrix(ts_Body *pBody, BoneID boneID, Graphics4x4 *mCompositeBoneMatrix)
{
	ts_Bone *pEndBone = boneGetBonePtr(pBody, boneID);

	ASSERT( pEndBone );

	if(pEndBone && pEndBone->m_parent)
	{
		*mCompositeBoneMatrix = *boneRunBonesToBone(pEndBone->m_parent);
	}
	else 
	if (pEndBone)
	{
		*mCompositeBoneMatrix = *boneRunBonesToBone(pEndBone);
	}
}

void boneGetCurrentBoneToModel(ts_Body *pBody, BoneID boneID, Graphics4x4 *mCurrentBoneToModel)
{
	ts_Bone *pEndBone = boneGetBonePtr(pBody, boneID);

	if(pEndBone)
	{
		mCurrentBoneToModel->FastMultiply(pEndBone->pModelBone->boneToModel, *boneRunBonesToBone(pEndBone));
	}
}

void boneGetEndPosition(ts_Body *pBody, BoneID boneID, Vector3 *pBonePosition)
{
	ts_Bone *pEndBone = boneGetBonePtr(pBody, boneID);

	if(pEndBone)
		*pBonePosition = pEndBone->pModelBone->vBoneEnd * *boneRunBonesToBone(pEndBone);
}

void boneGetStartPosition(ts_Body *pBody, BoneID boneID, Vector3 *pBonePosition)
{
	ts_Bone *pEndBone = boneGetBonePtr(pBody, boneID);

	if(pEndBone)
		*pBonePosition = pEndBone->pModelBone->vPivot * *boneRunBonesToBone(pEndBone);
}

void boneGetInitialPosition(ts_Body *pBody, BoneID boneID, Vector3 *pBonePosition)
{
	ts_ModelBone *pEndBone = boneGetModelBone(pBody, boneID);

	if(pEndBone)
		*pBonePosition = pEndBone->vBoneEnd;
}

void boneGetInitialStartPosition(ts_Body *pBody, BoneID boneID, Vector3 *pBonePosition)
{
	ts_ModelBone *pEndBone = boneGetModelBone(pBody, boneID);

	if(pEndBone)
		*pBonePosition = pEndBone->vPivot;
}

void boneGetPositionFromBone(ts_Bone *pBone, Vector3 *pBonePosition)
{
	if(pBone)
		*pBonePosition = pBone->pModelBone->vBoneEnd * *boneRunBonesToBone(pBone);
}

void boneGetStartPositionFromBone(ts_Bone *pBone, Vector3 *pBonePosition)
{
	if(pBone)
		*pBonePosition = pBone->pModelBone->vPivot * *boneRunBonesToBone(pBone);
}

void boneGetInitialPositionFromBone(ts_Bone *pBone, Vector3 *pBonePosition)
{
	if(pBone)
		*pBonePosition = pBone->pModelBone->vBoneEnd;
}

void boneGetInitialStartPositionFromBone(ts_Bone *pBone, Vector3 *pBonePosition)
{
	if(pBone)
		*pBonePosition = pBone->pModelBone->vPivot;
}

void boneGetCurrentBoneToModelFromBone(ts_Bone *pBone, Graphics4x4 *mCurrentBoneToModel)
{
	mCurrentBoneToModel->FastMultiply(pBone->pModelBone->boneToModel, *boneRunBonesToBone(pBone));
}
