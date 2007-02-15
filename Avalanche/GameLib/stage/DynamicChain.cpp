/*
**
**  File:   DynamicChain.cpp
**  Date:   April 19, 2001
**  By:     Bryant Collard
**  Desc:   Applies dynamics to chained bones in a model.
**
**  Copyright (c) 2001, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/stage/DynamicChain.cpp $
**  $Revision: #113 $
**  $DateTime: 2005/06/28 17:02:02 $
**    $Author: Bryant_Collard $
**
*/

/******************* includes ****************************/

/** Pre-Compiled header **/
#include "stage/StagePCH.h"

#include "CollideHelper/ObjectList.h"

/** Includes used that are in pre-compiled header **/
#if 0

/* My header (always first) */
#include "stage/DynamicChain.h"

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
#include "Game/Managers/DistortionMgr.h"
#include "Layers/Timing.h"
#include "collide/obb.h"

#include "EngineHelper/drawutil.h"

#if defined(GCN) && defined(CW)
#include "platform/GCN/GCNMWPort.h"
#endif

#endif

/** Includes not in pre-compiled header **/
#include "EngineHelper/timer.h"
#include "GameHelper/ParamMgr.h"

const int CDynamicChain::LINK_ATTACH   = 0x01;
const int CDynamicChain::LINK_TARGET   = 0x02;
const int CDynamicChain::LINK_GRAVITY  = 0x04;
const int CDynamicChain::LINK_DRAG     = 0x08;
const int CDynamicChain::LINK_BOX      = 0x10;
const int CDynamicChain::LINK_NO_OBB   = 0x20;

Vector3 CDynamicChain::s_startInOBB;

/**********************************************************************
 *
 *	Class:       CDynamicChain
 *	Function:    CDynamicChain()
 *	Access:      public
 *	Description: Default constructor.
 *	Arguments:   None
 *	Return:      None
 *
 **********************************************************************/
CDynamicChain::CDynamicChain()
{
	d_dynamicPartner = NULL;
	d_dynamicLink = NULL;
	d_customConstraint = NULL;
#ifdef ENABLE_RECORD
	d_dynamicChainRecordBuffer = 0;
#endif
}

/**********************************************************************
 *
 *	Class:       CDynamicChain
 *	Function:    ~CDynamicChain()
 *	Access:      public
 *	Description: Destructor
 *	Arguments:   None
 *	Return:      None
 *
 **********************************************************************/
CDynamicChain::~CDynamicChain()
{
	delete[] d_dynamicLink;
#ifdef ENABLE_RECORD
	DeleteRecordBuffer();
#endif
}

bool CDynamicChain::InitializeFail(void)
{
	delete[] d_dynamicLink;
	d_dynamicLink = NULL;
	return(CChain::InitializeFail());
}

bool CDynamicChain::SetNumLinks(int i_numLinks)
{
	if (CChain::SetNumLinks(i_numLinks))
	{
		MEM_SET_ONE_SHOT_NEW_NAME("CDynChain::SetNumLinks");
		d_dynamicLink = new SDynamicLink[d_numLinks];
		if (d_dynamicLink == NULL)
		{
			delete[] d_link;
			d_link = NULL;
			ASSERT(false);
			return(false);
		}
	}
	return(true);
}

bool CDynamicChain::InitLink(int i_link, ts_Bone* i_bone)
{
	if (CChain::InitLink(i_link, i_bone))
	{
		ts_ModelBone *mbone = i_bone->pModelBone;
		d_dynamicLink[i_link].d_controlMask = 0;
		d_dynamicLink[i_link].d_control = NULL;
		d_dynamicLink[i_link].d_lengthSquared = mbone->fBoneLen * mbone->fBoneLen;
		d_dynamicLink[i_link].d_attachPoint = mbone->vBoneEnd;
		d_dynamicLink[i_link].d_externalForce.Clear();
		d_dynamicLink[i_link].d_baseInvMass = 1.0f;

		if (i_link == 1)
		{
			d_dynamicLink[0].d_controlMask = 0;
			d_dynamicLink[0].d_control = NULL;
			d_dynamicLink[0].d_attachPoint =
					d_link[1].d_bone->pModelBone->vPivot;
			d_dynamicLink[0].d_externalForce.Clear();
			d_dynamicLink[0].d_baseInvMass = 1.0f;
		}
	}
	return(true);
}

bool CDynamicChain::CompleteInit(CInstance* i_inst,
		Graphics4x4* i_modelToWorld)
{
/*
 *	If the first bone (second particle) has a parent and no attachment for the
 *	first particle is specified, the first particle is attached to its parent.
 */
	if ((d_dynamicLink[0].d_control == NULL) && (d_link[0].d_bone != NULL))
	{
		d_dynamicLink[0].d_control = d_link[0].d_bone;
		d_dynamicLink[0].d_controlMask = LINK_ATTACH;
	}
/*
 *	Compute the extents of the first bone.
 */
	Vector3 extentInBone;
	ts_PatchSOM* model = i_inst->Model();
	d_maxRadius = 0.0f;
	if (!CompleteLinkInit(1, model, extentInBone))
		return(false);
	d_dynamicLink[0].d_constraintRadius = Math::Sqrt(extentInBone.x() *
			extentInBone.x() + extentInBone.y() * extentInBone.y());
	d_dynamicLink[1].d_constraintRadius = d_dynamicLink[0].d_constraintRadius;
/*
 *	Finish the rest of the bones.
 */
	for (int i = 2; i < d_numLinks; i++)
	{
		if (!CompleteLinkInit(i, model, extentInBone))
			return(false);
		d_dynamicLink[i].d_constraintRadius = Math::Sqrt(extentInBone.x() *
				extentInBone.x() + extentInBone.y() * extentInBone.y());
		d_dynamicLink[i - 1].d_constraintRadius +=
				d_dynamicLink[i].d_constraintRadius;
		d_dynamicLink[i - 1].d_constraintRadius *= 0.5f;
	}
	d_maxRadius = Math::Sqrt(d_maxRadius);
/*
 *	Initialize global parameters.
 */
	d_iterationRate = 300.0f;
	d_globalDamping = 1.0f;
/*
 *	Initialize box constraint info.
 */
	d_drawBoxConstraint = false;
	d_interactiveBoxConstraint = false;
	d_boxConstraintParentName[0] = '\0';
	d_boxConstraintInParent.Clear();
	d_boxConstraintRotation.Clear();
	d_boxConstraintParent = NULL;
	d_boxConstraintToParent.Identity();
	d_boxConstraintExtent.Set(1.0f, 1.0f, 1.0f);
	d_boxConstraintToWorld.Identity();
	d_worldToBoxConstraint.Identity();
	d_constraintScale = 1.0f;
/*
 *	Place in initial configuration.
 */
	d_modelToWorld = i_modelToWorld;
	Reset();

	return(true);
}

/**********************************************************************
 *
 *	Class:       CDynamicChain
 *	Function:    LoadParams
 *	Access:      public
 *	Description: Fills in parameters.
 *	Arguments:   None.
 *	Return:      None.
 *
 **********************************************************************/
void CDynamicChain::LoadParams(void)
{
	CParam<CDynamicChainData>* context =
			(CParam<CDynamicChainData>*) g_paramMgr.FindContext(d_name,
			sizeof(CDynamicChainData));

	if (context == NULL)
	{
		MEM_SET_ONE_SHOT_NEW_NAME("CDynChain::LoadParams");
		context = new CParam<CDynamicChainData>;
		ASSERT(context);
		CDynamicChainData &data = context->GetData();
		data.d_refChain = this;
		context->Init(d_name, "var");
		g_paramMgr.AddContext(context);

		data.d_numLinks = d_numLinks;
		data.d_dynamicLink = new CDynamicChainData::SLinkData[d_numLinks];
		for (int i = 0; i < d_numLinks; i++)
		{
			data.d_dynamicLink[i].d_controlMask = d_dynamicLink[i].d_controlMask;
			data.d_dynamicLink[i].d_baseInvMass = d_dynamicLink[i].d_baseInvMass;
			data.d_dynamicLink[i].d_targetNaturalFrequency =
					d_dynamicLink[i].d_targetNaturalFrequency;
			data.d_dynamicLink[i].d_targetDampingFactor =
					d_dynamicLink[i].d_targetDampingFactor;
			data.d_dynamicLink[i].d_dragCoefficient =
					d_dynamicLink[i].d_dragCoefficient;
		}
		data.d_iterationRate = d_iterationRate;
		data.d_globalDamping = d_globalDamping;
		data.d_gravity = d_gravity;
		data.d_interactiveBoxConstraint = d_interactiveBoxConstraint;
		strcpy(data.d_boxConstraintParentName, d_boxConstraintParentName);
		data.d_boxConstraintInParent = d_boxConstraintInParent;
		data.d_boxConstraintRotation = d_boxConstraintRotation;
		data.d_boxConstraintExtent = d_boxConstraintExtent;
		data.d_constraintScale = d_constraintScale;
	}
	else
	{
		CDynamicChainData* srcChain = &(context->GetData());
		ASSERT(srcChain && (d_numLinks == srcChain->d_numLinks));

		for (int i = 0; i < d_numLinks; i++)
		{
			d_dynamicLink[i].d_controlMask =
					srcChain->d_dynamicLink[i].d_controlMask;
			d_dynamicLink[i].d_baseInvMass =
					srcChain->d_dynamicLink[i].d_baseInvMass;
			d_dynamicLink[i].d_targetNaturalFrequency =
					srcChain->d_dynamicLink[i].d_targetNaturalFrequency;
			d_dynamicLink[i].d_targetDampingFactor =
					srcChain->d_dynamicLink[i].d_targetDampingFactor;
			d_dynamicLink[i].d_dragCoefficient =
					srcChain->d_dynamicLink[i].d_dragCoefficient;
		}
		d_iterationRate = srcChain->d_iterationRate;
		d_globalDamping = srcChain->d_globalDamping;
		d_gravity = srcChain->d_gravity;

		d_interactiveBoxConstraint = srcChain->d_interactiveBoxConstraint;
		strcpy(d_boxConstraintParentName, srcChain->d_boxConstraintParentName);
		d_boxConstraintInParent = srcChain->d_boxConstraintInParent;
		d_boxConstraintRotation = srcChain->d_boxConstraintRotation;
		d_boxConstraintExtent = srcChain->d_boxConstraintExtent;
		d_constraintScale = srcChain->d_constraintScale;
	}
	
	BoneID id;
	if (boneGetBoneID(d_link[0].d_bone->pBody, &id, d_boxConstraintParentName))
	{
		d_boxConstraintParent = boneGetBonePtr(d_link[0].d_bone->pBody, id);
		d_boxConstraintToParent.SetXYZRotation(d_boxConstraintRotation);
		d_boxConstraintToParent.PokeTranslation(d_boxConstraintInParent);
	}
	else
	{
		d_boxConstraintParent = NULL;
		d_boxConstraintToWorld.SetXYZRotation(d_boxConstraintRotation);
		d_boxConstraintToWorld.PokeTranslation(d_boxConstraintInParent);
		(Cartesian4x4*)(&d_boxConstraintToWorld)->Invert(d_worldToBoxConstraint);
	}
}

/**********************************************************************
 *
 *	Class:       CParam<CDynamicChainData>
 *	Function:    RegisterVariables
 *	Access:      public
 *	Description: Fills in parameters via ParamMgr.
 *	Arguments:   None.
 *	Return:      None.
 *
 **********************************************************************/
void CParam<CDynamicChainData>::RegisterVariables(void)
{
	char varName[100];

	delete[] d_data.d_dynamicLink;
	if (d_data.d_refChain != NULL)
	{
		CDynamicChain* chain = d_data.d_refChain;
		for (int i = 0; i < chain->GetNumLinks(); i++)
		{
			sprintf(varName, "ControlMask%d", i);
			if (!chain->d_dynamicLink[i].d_controlMask)
			{
				RegisterVariable(chain->d_dynamicLink[i].d_controlMask, varName,
						CDynamicChain::LINK_GRAVITY, 0x0, 0x1F);
			}
			else
			{
				RegisterVariable(chain->d_dynamicLink[i].d_controlMask, varName,
						chain->d_dynamicLink[i].d_controlMask, 0x0, 0x1F);
			}
			sprintf(varName, "Mass%d", i);
			RegisterVariableInv(chain->d_dynamicLink[i].d_baseInvMass, varName,
					1.0f, 0.0f, 100.0f);
			sprintf(varName, "NaturalFrequency%d", i);
			RegisterVariable(chain->d_dynamicLink[i].d_targetNaturalFrequency,
					varName, 6.0f, 0.0f, 50.0f);
			sprintf(varName, "DampingFactor%d", i);
			RegisterVariable(chain->d_dynamicLink[i].d_targetDampingFactor,
					varName, 1.0f, 0.0f, 3.0f);
			sprintf(varName, "DragCoefficient%d", i);
			RegisterVariable(chain->d_dynamicLink[i].d_dragCoefficient, varName,
					0.0f, 0.0f, 5.0f);
		}
		RegisterVariable(chain->d_iterationRate, "IterationRate",
				300.0f, 0.0f, 3000.0f);
		RegisterVariable(chain->d_globalDamping, "GlobalDamping",
				0.97f, 0.0f, 1.0f);
		RegisterVariable((Vector3Packed*)&chain->d_gravity, "Gravity", 0.0f, -9.81f, 0.0f,
				-50.0f, -50.0f, -50.0f, 50.0f, 50.0f, 50.0f);
		RegisterVariable(chain->d_interactiveBoxConstraint, "InteractiveBox",
				false);
		RegisterVariable(chain->d_boxConstraintParentName, "BoxParent", "",
				MAX_BONENAME_LEN);
		RegisterVariable((Vector3Packed*)&chain->d_boxConstraintInParent, "BoxLocation",
				0.0f, 0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f);
		RegisterVariable((Vector3Packed*)&chain->d_boxConstraintRotation, "BoxOrientation",
				0.0f, 0.0f, 0.0f, -1.58f, -1.58f, -1.58f, 1.58f, 1.58f, 1.58f);
		RegisterVariable((Vector3Packed*)&chain->d_boxConstraintExtent, "BoxSize",
				1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f);
		RegisterVariable(chain->d_constraintScale, "ConstraintScale",
				1.0f, 0.0f, 2.0f);
	}
	else
	{
		d_data.d_numLinks = 0;
		d_data.d_dynamicLink = NULL;
		RegisterVariable(d_data.d_iterationRate, "IterationRate",
				300.0f, 0.0f, 3000.0f);
		RegisterVariable(d_data.d_globalDamping, "GlobalDamping",
				0.97f, 0.0f, 1.0f);
		RegisterVariable((Vector3Packed*)&d_data.d_gravity, "Gravity",
				0.0f, -9.81f, 0.0f, -50.0f, -50.0f, -50.0f, 50.0f, 50.0f, 50.0f);
		RegisterVariable(d_data.d_interactiveBoxConstraint, "InteractiveBox",
				false);
		RegisterVariable(d_data.d_boxConstraintParentName, "BoxParent", "",
				MAX_BONENAME_LEN);
		RegisterVariable((Vector3Packed*)&d_data.d_boxConstraintInParent, "BoxLocation",
				0.0f, 0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f);
		RegisterVariable((Vector3Packed*)&d_data.d_boxConstraintRotation, "BoxOrientation",
				0.0f, 0.0f, 0.0f, -1.58f, -1.58f, -1.58f, 1.58f, 1.58f, 1.58f);
		RegisterVariable((Vector3Packed*)&d_data.d_boxConstraintExtent, "BoxSize",
				1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f);
		RegisterVariable(d_data.d_constraintScale, "ConstraintScale",
				1.0f, 0.0f, 2.0f);
	}
}

/**********************************************************************
 *
 *	Class:       CDynamicChain
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
bool CDynamicChain::TryAddPartner(CDynamicChain* i_queryChain)
{
	if (CChain::TryAddPartner((CChain*)i_queryChain))
	{
		i_queryChain->d_dynamicPartner = d_dynamicPartner;
		d_dynamicPartner = i_queryChain;
		return(true);
	}
	return(false);
}

/**********************************************************************
 *
 *	Class:       CDynamicChain
 *	Function:    Reset
 *	Access:      public
 *	Description: Places particles in their "pose" positions.
 *	Arguments:   None
 *	Return:      None
 *
 **********************************************************************/
void CDynamicChain::Reset(void)
{
/*
 *	Bail if the chain is not visible.
 */
	if (!boneGetVisibility(d_link[1].d_bone))
		return;

	ResetParticles();
/*
 *	Place the first particle.
 */
	d_dynamicLink[0].d_particleInWorld = d_link[0].d_newParticleInWorld;
	d_dynamicLink[0].d_velocityInWorld.Clear();
/*
 *	Place the rest of the particles.
 */
	for (int i = 1; i < d_numLinks; i++)
	{
		d_dynamicLink[i].d_particleInWorld = d_link[i].d_newParticleInWorld;
		d_dynamicLink[i].d_velocityInWorld.Clear();
		d_dynamicLink[i].d_targetInWorld = d_dynamicLink[i].d_particleInWorld;
		d_dynamicLink[i].d_newTargetInWorld = d_dynamicLink[i].d_targetInWorld;
	}
/*
 *	Reset the last time step.
 */
	d_deltaSec = 0.0f;
/*
 *	Reset collisions
 */
	d_collided = false;

	CompleteReset();
}

/**********************************************************************
 *
 *	Class:       CDynamicChain
 *	Function:    Update
 *	Access:      public
 *	Description: Updates the configuration of the chain.
 *	Arguments:
 *		i_deltaSec   - Time step.
 *		i_objectList - List of object constraints.
 *	Return:      None
 *
 **********************************************************************/
void CDynamicChain::Update(float i_deltaSec, CObjectList* i_objectList)
{
	PrepUpdate();
	TryUpdate(i_deltaSec, i_objectList);
	EndUpdate();
}

void CDynamicChain::PrepUpdate(void)
{
/*
 *	Compute velocity and record new positions.
 */
	int i;
	if (!Math::DivByZero(d_globalDamping, d_deltaSec))
	{
		float dampedDeltaSecInv = d_globalDamping / d_deltaSec;
		for (i = 0; i < d_numLinks; i++)
		{
			d_dynamicLink[i].d_velocityInWorld = dampedDeltaSecInv *
					(d_link[i].d_newParticleInWorld -
					d_dynamicLink[i].d_particleInWorld);
			d_dynamicLink[i].d_particleInWorld = d_link[i].d_newParticleInWorld;
			d_dynamicLink[i].d_targetInWorld = d_dynamicLink[i].d_newTargetInWorld;
		}
	}
/*
 *	Compute a average position and velocity for all particles affected by
 *	aerodynamic drag. Determine if the box constraint is active.
 */
	Vector3 averagePosition(0.0f, 0.0f, 0.0f);
	Vector3 averageVelocity(0.0f, 0.0f, 0.0f);
	int numDrag = 0;
	d_boxConstraintActive = false;
	for (i = 0; i < d_numLinks; i++)
	{
		if (d_dynamicLink[i].d_controlMask & LINK_DRAG)
		{
			averagePosition += d_dynamicLink[i].d_particleInWorld;
			averageVelocity += d_dynamicLink[i].d_velocityInWorld;
			numDrag++;
		}
		d_boxConstraintActive |=
				((d_dynamicLink[i].d_controlMask & LINK_BOX) != 0);
	}
/*
 *	If there are particles affected by drag, get the local wind velocity
 *	vector. Also, compute an average relative wind speed.
 */
	d_windVelocity.Clear();
	d_relativeWindSpeed = 0.0f;
	if (numDrag > 0)
	{
		float invCount = 1.0f / (float)numDrag;
		averagePosition *= invCount;
		averageVelocity *= invCount;
		DistortionMgr__ApplyDistortions(&averagePosition, &d_windVelocity,
				DISTORTION_AFFECTS_CHAINS);
		if (!Math::DivByZero(1.0f, g_timer.GetFrameSec()))
			d_windVelocity *= (float)(1.0f / g_timer.GetFrameSec());
		else
			d_windVelocity.Clear();
		d_relativeWindSpeed = (d_windVelocity - averageVelocity).Length();
	}
/*
 *	Compute the constraint box.
 */
	if (d_boxConstraintActive && d_interactiveBoxConstraint)
	{
		BoneID id;
		if (boneGetBoneID(d_link[0].d_bone->pBody, &id,
				d_boxConstraintParentName))
		{
			d_boxConstraintParent = boneGetBonePtr(d_link[0].d_bone->pBody, id);
			d_boxConstraintToParent.SetXYZRotation(d_boxConstraintRotation);
			d_boxConstraintToParent.PokeTranslation(d_boxConstraintInParent);
		}
		else
		{
			d_boxConstraintParent = NULL;
			d_boxConstraintToWorld.SetXYZRotation(d_boxConstraintRotation);
			d_boxConstraintToWorld.PokeTranslation(d_boxConstraintInParent);
			(Cartesian4x4*)(&d_boxConstraintToWorld)->Invert(
					d_worldToBoxConstraint);
		}
	}
/*
 *	Make sure the first particle does not target based on orientation.
 */
	if (d_dynamicLink[0].d_controlMask & LINK_TARGET)
		d_dynamicLink[0].d_controlMask |= LINK_ATTACH;
/*
 *	Find the last particle that needs orientation information to compute
 *	the target.
 */
	for (d_lastOrient = d_numLinks - 1; d_lastOrient > 0; d_lastOrient--)
	{
		if (TargetBase(d_lastOrient))
			break;
	}

	if (d_dynamicPartner)
		d_dynamicPartner->PrepUpdate();
}

void CDynamicChain::TryUpdate(float i_deltaSec, CObjectList* i_objectList)
{
/*
 *	Record the time step.
 */
	d_deltaSec = i_deltaSec;
/*
 *	Only update if the chain is visible.
 */
	if (boneGetVisibility(d_link[1].d_bone))
	{
/*
 *		Compute the first parent to world matrix if necessary.
 */
		DirCos3x3 parentToWorld;
		Graphics4x4 temp;
		if (d_lastOrient > 0)
		{
			Vector3 parentInWorld;
			float scaleX, scaleY, scaleZ, shearXY, shearXZ, shearYZ;
			temp = d_link[0].d_bone->pModelBone->boneToModel *
					(*boneRunBonesToBone(d_link[0].d_bone)) * (*d_modelToWorld);
			temp.Deconstruct(parentToWorld, parentInWorld,
					scaleX, scaleY, scaleZ, shearXY, shearXZ, shearYZ);
		}
/*
 *		Update positions of each link.
 */
		DirCos3x3 refToWorld;
		DirCos3x3 linkToRef;
		Vector3 xAxis, yAxis, zAxis;
		Vector3 target;
		int i;
		for (i = 0; i < d_numLinks; i++)
		{
/*
 *			Compute the reference to world matrix if it will be needed.
 */
			if ((i > 0) && (i <= d_lastOrient))
				refToWorld = d_link[i].d_refLinkToParent * parentToWorld;
/*
 *			Deal with the case where the particle is rigidly attached.
 */
			if (RigidAttach(i))
			{
				d_dynamicLink[i].d_currInvMass = 0.0f;
				if (d_dynamicLink[i].d_control)
				{
					d_link[i].d_newParticleInWorld = (d_dynamicLink[i].d_attachPoint *
							(*boneRunBonesToBone(d_dynamicLink[i].d_control))) *
							(*d_modelToWorld);
				}
				else
					d_link[i].d_newParticleInWorld = d_dynamicLink[i].d_attachPoint;
				d_dynamicLink[i].d_newTargetInWorld = d_link[i].d_newParticleInWorld;
			}
/*
 *			The particle is not rigidly attached.
 */
			else
			{
/*
 *				Construct the explict integration part.
 */
				d_dynamicLink[i].d_currInvMass = d_dynamicLink[i].d_baseInvMass;
				d_link[i].d_newParticleInWorld = d_dynamicLink[i].d_externalForce;
				if (d_dynamicLink[i].d_controlMask & LINK_DRAG)
					d_link[i].d_newParticleInWorld +=
							d_dynamicLink[i].d_dragCoefficient * d_relativeWindSpeed *
							(d_windVelocity - d_dynamicLink[i].d_velocityInWorld);
				d_link[i].d_newParticleInWorld *= d_dynamicLink[i].d_currInvMass;
				if (d_dynamicLink[i].d_controlMask & LINK_GRAVITY)
					d_link[i].d_newParticleInWorld += d_gravity;
				d_link[i].d_newParticleInWorld = d_dynamicLink[i].d_velocityInWorld +
						d_link[i].d_newParticleInWorld * d_deltaSec;
/*
 *				If the particle is not being drawn to a target, integrate.
 */
				if (!(d_dynamicLink[i].d_controlMask & LINK_TARGET))
				{
					d_link[i].d_newParticleInWorld *= d_deltaSec;
					d_link[i].d_newParticleInWorld +=
							d_dynamicLink[i].d_particleInWorld;
					d_dynamicLink[i].d_newTargetInWorld =
							d_link[i].d_newParticleInWorld;
				}
/*
 *				The particle is being drawn to a target.
 */
				else
				{
/*
 *					If the particle is being drawn to a specific point, find it.
 */
					if (d_dynamicLink[i].d_controlMask & LINK_ATTACH)
					{
						if (d_dynamicLink[i].d_control)
						{
							target = (d_dynamicLink[i].d_attachPoint *
									(*boneRunBonesToBone(d_dynamicLink[i].d_control))) *
									(*d_modelToWorld);
						}
						else
							target = d_dynamicLink[i].d_attachPoint;
					}
/*
 *					The target point is determined by relative orientations.
 *					Note that particle 0 cannot go here.
 */
					else
					{
						if (d_dynamicLink[i].d_control)
						{
							boneComputeBoneToParent(d_dynamicLink[i].d_control, &temp);
							temp.GetOrientation(d_link[i].d_refLinkToParent);
						}
						target = refToWorld.GetRow2();
						target = d_scale * d_link[i].d_bone->pModelBone->fBoneLen *
								target + d_link[i - 1].d_newParticleInWorld;
					}
/*
 *					Add in the implicit part and integrate.
 */
					float dampingTerm = 2.0f * d_dynamicLink[i].d_targetDampingFactor
							* d_dynamicLink[i].d_targetNaturalFrequency;
					float dynamicTerm = d_deltaSec * (dampingTerm +
							d_deltaSec * d_dynamicLink[i].d_targetNaturalFrequency *
							d_dynamicLink[i].d_targetNaturalFrequency);
					d_link[i].d_newParticleInWorld += dampingTerm *
							(d_dynamicLink[i].d_particleInWorld  -
							d_dynamicLink[i].d_targetInWorld);
					d_link[i].d_newParticleInWorld *= d_deltaSec;
					d_link[i].d_newParticleInWorld +=
							d_dynamicLink[i].d_particleInWorld + dynamicTerm * target;
					d_link[i].d_newParticleInWorld *= 1.0f / (1.0f + dynamicTerm);
					d_dynamicLink[i].d_newTargetInWorld = target;
				}
			}
/*
 *			Update the parent to world matrix if necessary.
 */
			if ((i > 0) && (i < d_lastOrient))
			{
/*
 *				Build the link to reference matrix.
 */
				zAxis = (d_dynamicLink[i].d_particleInWorld -
						d_dynamicLink[i - 1].d_particleInWorld) / refToWorld;
				if (!zAxis.SafeNormalize())
					zAxis.Set(0.0f, 0.0f, 1.0f);
				if (zAxis.x() > 0.99999f)
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
/*
 *				Compute the new parent to world transformation.
 */
				linkToRef.SetRow0(xAxis);
				linkToRef.SetRow1(yAxis);
				linkToRef.SetRow2(zAxis);

				parentToWorld = linkToRef * refToWorld;
			}
		}
/*
 *		Enforce position constraints.
 */
		EnforceConstraints(i_objectList, (int)(d_iterationRate * d_deltaSec) + 1);
	}

	if (d_dynamicPartner)
		d_dynamicPartner->TryUpdate(d_deltaSec, i_objectList);
}

void CDynamicChain::EndUpdate(void)
{
	ClearExternalForces();
	AdjustBones();
	EndUpdateDebug();
}

void CDynamicChain::EndUpdateDebug(void)
{
#if 0
	for (i = 1; i < d_numLinks; i++)
	{
		DrawUtility::Line(d_dynamicLink[i - 1].d_newTargetInWorld,
				d_dynamicLink[i].d_newTargetInWorld, (DrawUtility::COLOR)(i % 7));
	}
#endif

	if (d_interactiveBoxConstraint || d_drawBoxConstraint)
	{
		DrawUtility::Box(d_boxConstraintExtent, -d_boxConstraintExtent,
				d_boxConstraintToWorld, DrawUtility::WHITE);
	}

	if (d_dynamicPartner)
		d_dynamicPartner->EndUpdateDebug();
}

void CDynamicChain::EnforceConstraints(CObjectList* i_objectList,
		int i_iterations)
{
	int i, j;
	Vector3 linkDelta;
	float scaleSq = d_scale * d_scale;
	for (j = 0; j < i_iterations; j++)
	{
		for (i = 1; i < d_numLinks; i++)
		{
			linkDelta = d_link[i].d_newParticleInWorld -
					d_link[i - 1].d_newParticleInWorld;
			float deltaScale = linkDelta.LengthSquared();
			float linkLengthSquared = scaleSq * d_dynamicLink[i].d_lengthSquared;
			float denom = (deltaScale + linkLengthSquared) *
					(d_dynamicLink[i - 1].d_currInvMass +
					d_dynamicLink[i].d_currInvMass);
			deltaScale -= linkLengthSquared;
			if (Math::DivByZero(deltaScale, denom))
				continue;
			deltaScale /= denom;
			linkDelta *= deltaScale;
			d_link[i - 1].d_newParticleInWorld +=
					d_dynamicLink[i - 1].d_currInvMass * linkDelta;
			d_link[i].d_newParticleInWorld -= d_dynamicLink[i].d_currInvMass *
					linkDelta;
		}
	}

	d_collided = false;
	if (i_objectList)
	{
		OBB* obb = (OBB*)(i_objectList->GetHead(CCollisionObject::OBB_OBJECT));
		while (obb)
		{
			if (ConstrainToObb(0))
			{
				if (!ConstrainToObb(1))
				{
					Vector3 extent;
					ComputeConstraintInfo(obb, d_link[0].d_newParticleInWorld,
							d_scale * d_dynamicLink[0].d_constraintRadius,
							s_startInOBB, extent);
					if (InsideOBB(s_startInOBB, extent))
					{
						MoveToFace(FindClosestFace(s_startInOBB, extent),
								obb, s_startInOBB, extent,
								d_link[0].d_newParticleInWorld);
						d_collided = true;
					}
				}
				else
					s_startInOBB = (d_link[0].d_newParticleInWorld -
							(*(obb->Center()))) / (*(obb->Matrix()));
			}

			for (i = 1; i < d_numLinks; i++)
				EnforceOBBConstraint(i, obb);

			obb = (OBB*)(i_objectList->GetNext(CCollisionObject::OBB_OBJECT));
		}
	}
/*
 *	Enforce box constraint.
 */
	if (d_boxConstraintActive)
	{
		if (d_boxConstraintParent)
		{
			d_boxConstraintToWorld = d_boxConstraintToParent *
					d_boxConstraintParent->pModelBone->boneToModel *
					(*boneRunBonesToBone(d_boxConstraintParent)) * (*d_modelToWorld);
			d_boxConstraintToWorld.Invert(d_worldToBoxConstraint);
		}
		for (i = 0; i < d_numLinks; i++)
		{
			if (!RigidAttach(i) && (d_dynamicLink[i].d_controlMask & LINK_BOX))
			{
				Vector3 particleInBox = d_link[i].d_newParticleInWorld *
						d_worldToBoxConstraint;

				if ((float)particleInBox.x() > (float)d_boxConstraintExtent.x())
					particleInBox.x( d_boxConstraintExtent.x());
				else if (particleInBox.x() < -d_boxConstraintExtent.x())
					particleInBox.x( -d_boxConstraintExtent.x());
				if (particleInBox.y() > d_boxConstraintExtent.y())
					particleInBox.y( d_boxConstraintExtent.y());
				else if (particleInBox.y() < -d_boxConstraintExtent.y())
					particleInBox.y( -d_boxConstraintExtent.y());
				if (particleInBox.z() > d_boxConstraintExtent.z())
					particleInBox.z( d_boxConstraintExtent.z());
				else if (particleInBox.z() < -d_boxConstraintExtent.z())
					particleInBox.z( -d_boxConstraintExtent.z());
				d_link[i].d_newParticleInWorld = particleInBox *
						d_boxConstraintToWorld;
			}
		}
	}
/*
 *	Enforce custom constraint.
 */
	if (d_customConstraint != NULL)
		d_customConstraint->ApplyConstraint();
/*
 *	Build extents.
 */
	d_minExtent = d_dynamicLink[0].d_particleInWorld;
	d_maxExtent = d_dynamicLink[0].d_particleInWorld;
	for (i = 1; i < d_numLinks; i++)
		UpdateExtents(d_dynamicLink[i].d_particleInWorld, d_minExtent,
				d_maxExtent);
	d_minExtent -= d_maxRadius * d_scale;
	d_maxExtent += d_maxRadius * d_scale;
}

void CDynamicChain::EnforceOBBConstraint(int i_link, OBB* i_obb)
{
	if (!ConstrainToObb(i_link - 1))
	{
		if (ConstrainToObb(i_link))
		{
			if ((i_link == (d_numLinks - 1)) || !ConstrainToObb(i_link + 1))
			{
				Vector3 extent;
				ComputeConstraintInfo(i_obb,
						d_link[i_link].d_newParticleInWorld,
						d_scale * d_dynamicLink[i_link].d_constraintRadius,
						s_startInOBB, extent);
				if (InsideOBB(s_startInOBB, extent))
				{
					MoveToFace(FindClosestFace(s_startInOBB, extent),
							i_obb, s_startInOBB, extent,
							d_link[i_link].d_newParticleInWorld);
					d_collided = true;
				}
			}
			else
				s_startInOBB = (d_link[i_link].d_newParticleInWorld -
						(*(i_obb->Center()))) / (*(i_obb->Matrix()));
		}
		return;
	}

	if (!ConstrainToObb(i_link))
		return;

	Vector3 endInOBB, center, size, corner;
	ComputeLinkConstraintInfo(i_obb, i_link, endInOBB, center, size, corner);

	float depthSquared;
	Vector3 crossAxisDisplacement;
	switch(ResolveOBBConstraint(i_obb, center, size, corner, depthSquared,
			crossAxisDisplacement))
	{
	 case NO_INTERSECTION:
		s_startInOBB = endInOBB;
		return;
	 case POS_X_FACE:
		if (s_startInOBB.x() < corner.x())
			s_startInOBB.x( corner.x());
		if (endInOBB.x() < corner.x())
			endInOBB.x( corner.x());
		break;
	 case NEG_X_FACE:
		if (s_startInOBB.x() > -corner.x())
			s_startInOBB.x( -corner.x());
		if (endInOBB.x() > -corner.x())
			endInOBB.x( -corner.x());
		break;
	 case POS_Y_FACE:
		if (s_startInOBB.y() < corner.y())
			s_startInOBB.y( corner.y());
		if (endInOBB.y() < corner.y())
			endInOBB.y( corner.y());
		break;
	 case NEG_Y_FACE:
		if (s_startInOBB.y() > -corner.y())
			s_startInOBB.y( -corner.y());
		if (endInOBB.y() > -corner.y())
			endInOBB.y( -corner.y());
		break;
	 case POS_Z_FACE:
		if (s_startInOBB.z() < corner.z())
			s_startInOBB.z( corner.z());
		if (endInOBB.z() < corner.z())
			endInOBB.z( corner.z());
		break;
	 case NEG_Z_FACE:
		if (s_startInOBB.z() > -corner.z())
			s_startInOBB.z( -corner.z());
		if (endInOBB.z() > -corner.z())
			endInOBB.z( -corner.z());
		break;
	 case CROSS_AXIS_FACE:
		s_startInOBB += crossAxisDisplacement;
		endInOBB += crossAxisDisplacement;
		break;
	}

	d_link[i_link - 1].d_newParticleInWorld = s_startInOBB *
			(*(i_obb->Matrix())) + (*(i_obb->Center()));
	d_link[i_link].d_newParticleInWorld = endInOBB *
			(*(i_obb->Matrix())) + (*(i_obb->Center()));
	s_startInOBB = endInOBB;

	d_collided = true;
}


void CDynamicChain::EnforceOBBConstraint(int i_link, CObjectList* i_objectList)
{
	if (RigidAttach(i_link))
		return;

	Vector3 pointInOBB;
	Vector3 extent;
	if (i_objectList)
	{
		OBB* obb = (OBB*)(i_objectList->GetHead(CCollisionObject::OBB_OBJECT));
		while (obb)
		{
			ComputeConstraintInfo(obb, d_link[i_link].d_newParticleInWorld,
					d_scale * d_dynamicLink[i_link].d_constraintRadius,
					pointInOBB, extent);
			if (InsideOBB(pointInOBB, extent))
			{
				MoveToFace(FindClosestFace(pointInOBB, extent), obb, pointInOBB,
						extent, d_link[i_link].d_newParticleInWorld);
				d_collided = true;
			}
			obb = (OBB*)(i_objectList->GetNext(CCollisionObject::OBB_OBJECT));
		}
	}
}

CDynamicChain::ESeparatingPlane CDynamicChain::ResolveOBBConstraint(OBB* i_obb,
		Vector3 &i_linkCenter, Vector3 &i_linkSize, Vector3 &i_obbCorner,
		float &o_depthSquared, Vector3 &o_crossAxisDisplacement)
{
	Vector3 absSize;
	Vector3 crossAxis;
	Vector3 scaledOverlap;
	float overlap[6];

	absSize = Vector3::Abs(i_linkSize);
	overlap[0] = i_obbCorner.x() + absSize.x() - Math::Abs(i_linkCenter.x());
	if (overlap[0] < 0.0f)
		return(NO_INTERSECTION);

	overlap[1] = i_obbCorner.y() + absSize.y() - Math::Abs(i_linkCenter.y());
	if (overlap[1] < 0.0f)
		return(NO_INTERSECTION);

	overlap[2] = i_obbCorner.z() + absSize.z() - Math::Abs(i_linkCenter.z());
	if (overlap[2] < 0.0f)
		return(NO_INTERSECTION);

	crossAxis.x( i_linkCenter.z() * i_linkSize.y() - i_linkCenter.y() * i_linkSize.z());
	overlap[3] = i_obbCorner.y() * absSize.z() + i_obbCorner.z() * absSize.y() -
			Math::Abs(crossAxis.x());
	if (overlap[3] < 0.0f)
		return(NO_INTERSECTION);

	crossAxis.y( i_linkCenter.x() * i_linkSize.z() - i_linkCenter.z() * i_linkSize.x());
	overlap[4] = i_obbCorner.x() * absSize.z() + i_obbCorner.z() * absSize.x() -
			Math::Abs(crossAxis.y());
	if (overlap[4] < 0.0f)
		return(NO_INTERSECTION);

	crossAxis.z( i_linkCenter.y() * i_linkSize.x() - i_linkCenter.x() * i_linkSize.y());
	overlap[5] = i_obbCorner.x() * absSize.y() + i_obbCorner.y() * absSize.x() -
			Math::Abs(crossAxis.z());
	if (overlap[5] < 0.0f)
		return(NO_INTERSECTION);

	int minIndex;
	if (overlap[0] < overlap[1])
	{
		if (overlap[0] < overlap[2])
			minIndex = 0;
		else
			minIndex = 2;
	}
	else if (overlap[1] < overlap[2])
		minIndex = 1;
	else
		minIndex = 2;

	o_depthSquared = overlap[minIndex] * overlap[minIndex];
	float distSq;

	scaledOverlap.x(overlap[3] / (i_linkSize.y() * i_linkSize.y() +
			i_linkSize.z() * i_linkSize.z()));
	distSq = overlap[3] * scaledOverlap.x();
	if (distSq < o_depthSquared)
	{
		o_depthSquared = distSq;
		minIndex = 3;
	}

	scaledOverlap.y( overlap[4] / (i_linkSize.x() * i_linkSize.x() +
			i_linkSize.z() * i_linkSize.z()));
	distSq = overlap[4] * scaledOverlap.y();
	if (distSq < o_depthSquared)
	{
		o_depthSquared = distSq;
		minIndex = 4;
	}

	scaledOverlap.z( overlap[5] / (i_linkSize.x() * i_linkSize.x() +
			i_linkSize.y() * i_linkSize.y()));
	distSq = overlap[5] * scaledOverlap.z();
	if (distSq < o_depthSquared)
	{
		o_depthSquared = distSq;
		minIndex = 5;
	}

	switch (minIndex)
	{
	case 0:
		if (i_linkCenter.x() >= 0.0f)
			return(POS_X_FACE);
		return(NEG_X_FACE);
	case 1:
		if (i_linkCenter.y() >= 0.0f)
			return(POS_Y_FACE);
		return(NEG_Y_FACE);
	case 2:
		if (i_linkCenter.z() >= 0.0f)
			return(POS_Z_FACE);
		return(NEG_Z_FACE);
	case 3:
		o_crossAxisDisplacement.x( 0.0f);
		if (crossAxis.x() >= 0.0f)
		{
			o_crossAxisDisplacement.y( -scaledOverlap.x() * i_linkSize.z());
			o_crossAxisDisplacement.z( scaledOverlap.x() * i_linkSize.y());
		}
		else
		{
			o_crossAxisDisplacement.y( scaledOverlap.x() * i_linkSize.z());
			o_crossAxisDisplacement.z( -scaledOverlap.x() * i_linkSize.y());
		}
		break;
	case 4:
		o_crossAxisDisplacement.y( 0.0f);
		if (crossAxis.y() >= 0.0f)
		{
			o_crossAxisDisplacement.x( scaledOverlap.y() * i_linkSize.z());
			o_crossAxisDisplacement.z( -scaledOverlap.y() * i_linkSize.z());
		}
		else
		{
			o_crossAxisDisplacement.x( -scaledOverlap.y() * i_linkSize.z());
			o_crossAxisDisplacement.z( scaledOverlap.y() * i_linkSize.z());
		}
		break;
	default:  // case 5:
		o_crossAxisDisplacement.z( 0.0f);
		if (crossAxis.z() >= 0.0f)
		{
			o_crossAxisDisplacement.x( -scaledOverlap.z() * i_linkSize.y());
			o_crossAxisDisplacement.y( scaledOverlap.z() * i_linkSize.x());
		}
		else
		{
			o_crossAxisDisplacement.x( scaledOverlap.z() * i_linkSize.y());
			o_crossAxisDisplacement.y( -scaledOverlap.z() * i_linkSize.x());
		}
		break;
	}
	return(CROSS_AXIS_FACE);
}

void CDynamicChain::ComputeConstraintInfo(OBB* i_obb, Vector3 &i_point,
		float i_constraintRadius, Vector3 &o_pointInOBB, Vector3 &o_extent)
{
	o_pointInOBB = (i_point - (*(i_obb->Center()))) / (*(i_obb->Matrix()));
	o_extent = d_constraintScale * i_obb->GetAABB().maxV;
	float constraintRadius = d_constraintScale * i_constraintRadius;
	o_extent += constraintRadius;
}

void CDynamicChain::ComputeLinkConstraintInfo(OBB* i_obb, int i_link,
		Vector3 &o_endInOBB, Vector3 &o_linkCenter, Vector3 &o_linkSize,
		Vector3 &o_obbCorner)
{
	o_endInOBB = (d_link[i_link].d_newParticleInWorld -
			(*(i_obb->Center()))) / (*(i_obb->Matrix()));
	o_linkCenter = 0.5f * (s_startInOBB + o_endInOBB);
	o_linkSize = o_endInOBB - o_linkCenter;

	// Expand the obb.
	o_obbCorner = d_constraintScale * i_obb->GetAABB().maxV;
	float constraintRadius = d_constraintScale *
			d_scale * d_dynamicLink[i_link].d_constraintRadius;
	o_obbCorner += constraintRadius;
}

bool CDynamicChain::InsideOBB(Vector3 &i_point, Vector3 &i_extent)
{
	return((i_point.x() > -i_extent.x()) && (i_point.y() > -i_extent.y()) &&
			(i_point.z() > -i_extent.z()) && (i_point.x() < i_extent.x()) &&
			(i_point.y() < i_extent.y()) && (i_point.z() < i_extent.z()));
}

int CDynamicChain::FindPenetrateFace(Vector3 &i_start, Vector3 &i_end,
		Vector3 &i_extent)
{
	Vector3 point = i_start;
	float f, fInv;

	if (point.x() > i_extent.x())
	{
		if (i_end.x() >= i_extent.x())
			return(-1);
		f = (i_extent.x() - point.x()) / (i_end.x() - point.x());
		fInv = 1.0f - f;
		point.Set(i_extent.x(), fInv * point.y() + f * i_end.y(),
				fInv * point.z() + f * i_end.z());
	}
	else if (point.x() < -i_extent.x())
	{
		if (i_end.x() <= -i_extent.x())
			return(-1);
		f = (-i_extent.x() - point.x()) / (i_end.x() - point.x());
		fInv = 1.0f - f;
		point.Set(-i_extent.x(), fInv * point.y() + f * i_end.y(),
				fInv * point.z() + f * i_end.z());
	}

	if (point.y() > i_extent.y())
	{
		if (i_end.y() >= i_extent.y())
			return(-1);
		f = (i_extent.y() - point.y()) / (i_end.y() - point.y());
		fInv = 1.0f - f;
		point.Set(fInv * point.x() + f * i_end.x(), i_extent.y(),
				fInv * point.z() + f * i_end.z());
	}
	else if (point.y() < -i_extent.y())
	{
		if (i_end.y() <= -i_extent.y())
			return(-1);
		f = (-i_extent.y() - point.y()) / (i_end.y() - point.y());
		fInv = 1.0f - f;
		point.Set(fInv * point.x() + f * i_end.x(), -i_extent.y(),
				fInv * point.z() + f * i_end.z());
	}

	if (point.z() > i_extent.z())
	{
		if (i_end.z() >= i_extent.z())
			return(-1);
		return(5);
	}
	else if (point.z() < -i_extent.z())
	{
		if (i_end.z() <= -i_extent.z())
			return(-1);
		return(2);
	}

	if (point.y() > 0)
	{
		if (point.x() > 0)
		{
			if (point.y() > point.x())
				return(4);
			return(3);
		}
		else if (point.y() > -point.x())
			return(4);
		return(0);
	}
	else if (point.x() > 0)
	{
		if (-point.y() > point.x())
			return(1);
		return(3);
	}
	else if (-point.y() > -point.x())
		return(1);
	return(0);
}

int CDynamicChain::FindClosestFace(Vector3 &i_point, Vector3 &i_extent)
{
	Vector3 extent = i_point + i_extent;
	int closestFace = 0;
	float closestDist = extent.x();
	if (extent.y() < closestDist)
	{
		closestFace = 1;
		closestDist = extent.y();
	}
	if (extent.z() < closestDist)
	{
		closestFace = 2;
		closestDist = extent.z();
	}
	
	extent = i_extent - i_point;
	if (extent.x() < closestDist)
	{
		closestFace = 3;
		closestDist = extent.x();
	}
	if (extent.y() < closestDist)
	{
		closestFace = 4;
		closestDist = extent.y();
	}
	if (extent.z() < closestDist)
		closestFace = 5;

	return(closestFace);
}

void CDynamicChain::MoveToFace(int i_face, OBB* i_obb, Vector3 &io_pointInOBB,
		Vector3 &i_extent, Vector3 &o_pointInWorld)
{
	switch (i_face)
	{
	case 0:
		io_pointInOBB.x( -i_extent.x());
		break;
	case 1:
		io_pointInOBB.y( -i_extent.y());
		break;
	case 2:
		io_pointInOBB.z( -i_extent.z());
		break;
	case 3:
		io_pointInOBB.x( i_extent.x());
		break;
	case 4:
		io_pointInOBB.y( i_extent.y());
		break;
	case 5:
		io_pointInOBB.z( i_extent.z());
		break;
	default:
		ASSERT(false);
	}
	o_pointInWorld = io_pointInOBB * (*(i_obb->Matrix())) +
			(*(i_obb->Center()));
}

int CDynamicChain::ConstrainToPoint(Vector3CRef i_point, int i_desiredNode,
		int i_iterations)
{
	ASSERT(i_desiredNode < d_numLinks);
/*
 *	Find a distance larger than the maximum possible distance between
 *	the endpoints of the chain if the chain were streached out. Then find
 *	the distance between the point and the first node. If it is larger than
 *	the largest possible chain length, bail.
 */
	float minDistSq = d_numLinks * d_maxRadius;
	minDistSq *= minDistSq;
	Vector3 delta = i_point - d_link[0].d_newParticleInWorld;
	float distSq = delta.Dot(delta);
	if (distSq > minDistSq)
		return(-1);
/*
 *	If a specific node has not been specified, find the closest node.
 */
	int i;
	int closestNode = i_desiredNode;
	int minNode = closestNode;
	if (closestNode < 0)
	{
		if (!RigidAttach(0))
		{
			minDistSq = distSq;
			closestNode = 0;
		}
		for (i = 1; i < d_numLinks; i++)
		{
			if (RigidAttach(i))
				continue;
			delta = i_point - d_link[i].d_newParticleInWorld;
			distSq = delta.Dot(delta);
			if (distSq < minDistSq)
			{
				minDistSq = distSq;
				closestNode = i;
			}
		}
/*
 *		Bail if no closest node was found. The only way this should be able to
 *		happen is if all links are rigidly attached. (I don't know why you would
 *		want to do that.)
 */
		if (closestNode < 0)
			return(-1);
/*
 *		Impose the constraint.
 */
		minNode = closestNode;
		if (!ImposePointConstraint(i_point, closestNode, minNode))
			return(-1);
	}
/*
 *	If the specified node is rigidly attached, find the closest one that is not.
 */
	else if (RigidAttach(closestNode))
	{
		int prevNode;
		for (prevNode = closestNode - 1; prevNode >= 0; prevNode--)
		{
			if (!RigidAttach(prevNode))
				break;
		}

		int nextNode;
		for (nextNode = closestNode + 1; nextNode < d_numLinks; nextNode++)
		{
			if (!RigidAttach(nextNode))
				break;
		}

		if (prevNode < 0)
		{
			if (nextNode >= d_numLinks)
				return(-1);
			closestNode = nextNode;
		}
		else if (nextNode >= d_numLinks)
			closestNode = prevNode;
		else if ((closestNode - prevNode) <= (nextNode - closestNode))
			closestNode = prevNode;
		else
			closestNode = nextNode;
	}
/*
 *	Iterate.
 */
	for (i = 0; i < i_iterations; i++)
	{
/*
 *		Try to find a closer node but try to avoid "popping" to a new section
 *		of the chain.
 */
		delta = i_point - d_link[closestNode].d_newParticleInWorld;
		minDistSq = delta.Dot(delta);
		int lastNode = closestNode;
		int j;
		for (j = lastNode - 1; j >= 0; j--)
		{
			if (RigidAttach(j))
				break;
			delta = i_point - d_link[j].d_newParticleInWorld;
			distSq = delta.Dot(delta);
			if (distSq > minDistSq)
				break;
			minDistSq = distSq;
			closestNode = j;
		}
		for (j = lastNode + 1; j < d_numLinks; j++)
		{
			if (RigidAttach(j))
				break;
			delta = i_point - d_link[j].d_newParticleInWorld;
			distSq = delta.Dot(delta);
			if (distSq > minDistSq)
				break;
			minDistSq = distSq;
			closestNode = j;
		}
/*
 *		Impose the constraint.
 */
		if (!ImposePointConstraint(i_point, closestNode, minNode))
			return(-1);
	}

	return(closestNode);
}

bool CDynamicChain::ImposePointConstraint(Vector3CRef i_point,
		int i_closestNode, int &io_minNode)
{
/*
 *	Find the fractional distances from the middle of the links before and
 *	after the node.
 */
	Vector3 offsetPrev, offsetNext, linkDeltaPrev, linkDeltaNext;
	float gPrev, gNext;
	if ((i_closestNode == 0) || RigidAttach(i_closestNode - 1))
		gPrev = 1.0f;
	else
	{
		offsetPrev = i_point - d_link[i_closestNode - 1].d_newParticleInWorld;
		linkDeltaPrev = d_link[i_closestNode].d_newParticleInWorld -
				d_link[i_closestNode - 1].d_newParticleInWorld;
		gPrev = (offsetPrev.Dot(linkDeltaPrev) /
				linkDeltaPrev.Dot(linkDeltaPrev)) - 0.5f;
/*
 *		If the closest node is the last one, don't stretch the last link.
 */
		if ((i_closestNode == (d_numLinks - 1)) && (gPrev > 0.5f))
			return(false);
	}

	if ((i_closestNode == (d_numLinks - 1)) || RigidAttach(i_closestNode + 1))
		gNext = -1.0f;
	else
	{
		offsetNext = i_point - d_link[i_closestNode].d_newParticleInWorld;
		linkDeltaNext = d_link[i_closestNode + 1].d_newParticleInWorld -
				d_link[i_closestNode].d_newParticleInWorld;
		gNext = (offsetNext.Dot(linkDeltaNext) /
				linkDeltaNext.Dot(linkDeltaNext)) - 0.5f;
/*
 *		If the closest node is the first one, don't stretch the first link.
 */
		if ((i_closestNode == 0) && (gNext < -0.5f))
			return(false);
	}
/*
 *	Select the link with the smallest g. If the closest point to the selected
 *	link does not fall on the link, just move the node; otherwise, move
 *	both ends of the link.
 */
	Vector3 linkDelta;
	float gPrevAbs = Math::Abs(gPrev);
	float gNextAbs = Math::Abs(gNext);
	if ((gPrevAbs > 0.5f) && (gNextAbs > 0.5f))
		d_link[i_closestNode].d_newParticleInWorld = i_point;
	else
	{
		float h;
		if (gPrevAbs < gNextAbs)
		{
			h = gPrev - 0.5f;
			linkDelta = ((2.0f * gPrev) * offsetPrev) -
					((2.0f * h * (gPrev + 1.0f)) * linkDeltaPrev);
			i_closestNode--;
		}
		else
		{
			h = gNext - 0.5f;
			linkDelta = ((2.0f * gNext) * offsetNext) -
					((2.0f * h * (gNext + 1.0f)) * linkDeltaNext);
		}
		d_link[i_closestNode + 1].d_newParticleInWorld = i_point - h * linkDelta;
		d_link[i_closestNode].d_newParticleInWorld =
				d_link[i_closestNode + 1].d_newParticleInWorld  - linkDelta;
	}
/*
 *	Perform over-relaxation on the link that changed and subsequent links.
 */
	if (i_closestNode < io_minNode)
		io_minNode = i_closestNode;
	if (io_minNode < 1)
		io_minNode = 1;
	float scaleSq = d_scale * d_scale;
	for (int i = io_minNode; i < d_numLinks; i++)
	{
		linkDelta = d_link[i].d_newParticleInWorld -
				d_link[i - 1].d_newParticleInWorld;
		float deltaScale = linkDelta.LengthSquared();
		float linkLengthSquared = scaleSq * d_dynamicLink[i].d_lengthSquared;
		float denom = (deltaScale + linkLengthSquared) *
				(d_dynamicLink[i - 1].d_currInvMass +
				d_dynamicLink[i].d_currInvMass);
		deltaScale -= linkLengthSquared;
		if (Math::DivByZero(deltaScale, denom))
			continue;
		deltaScale /= denom;
		linkDelta *= deltaScale;
		d_link[i - 1].d_newParticleInWorld += d_dynamicLink[i - 1].d_currInvMass *
				linkDelta;
		d_link[i].d_newParticleInWorld -= d_dynamicLink[i].d_currInvMass *
				linkDelta;
	}
	io_minNode--;

	return(true);
}

/**********************************************************************
 *
 *	Class:       CDynamicChain
 *	Function:    ConstrainToHole
 *	Access:      public
 *	Description: Constrains the chain to either lie outside an OBB or
 *	             inside a hole passing through the center of the OBB
 *	             aligned with the OBB y-axis.
 *	Arguments:
 *		i_obb        - The OBB containing the hole.
 *		i_xHalfWidth - The half width of the hole in the OBB x-direction.
 *		i_zHalfWidth - The half width of the hole in the OBB z-direction.
 *		i_iterations - The number of times to apply the constraint.
 *	Return:      None
 *
 **********************************************************************/
void CDynamicChain::ConstrainToHole(OBB* i_obb, float i_xHalfWidth,
		float i_zHalfWidth, int i_iterations)
{
	for (int i = 0; i < i_iterations; i++)
	{
/*
 *		If the start of the first link can move, make sure it is dealt with.
 */
		if (!RigidAttach(0))
		{
/*
 *			If the end of the first link cannot move, impose the constraint on
 *			the start of the link.
 */
			if (RigidAttach(1))
			{
				ConstrainNodeToHole(0, i_obb, i_xHalfWidth, i_zHalfWidth,
						s_startInOBB);
			}
/*
 *			If the end of the first link can move, transform the start of the
 *			link into OBB space so the constraint can be imposed on the whole
 *			link below.
 */
			else
			{
				s_startInOBB = (d_link[0].d_newParticleInWorld -
						(*(i_obb->Center()))) / (*(i_obb->Matrix()));
			}
		}
/*
 *		Impose the constraint on each of the links.
 */
		for (i = 1; i < d_numLinks; i++)
			ConstrainLinkToHole(i, i_obb, i_xHalfWidth, i_zHalfWidth);
	}
}

/**********************************************************************
 **********************************************************************/
void CDynamicChain::ConstrainNodeToHole(int i_node, OBB* i_obb,
		float i_xHalfWidth, float i_zHalfWidth, Vector3 &o_nodeInOBB)
{
/*
 *	Compute info so the problem can be solved in terms of a line segment against
 *	an axis-aligned box centered at the origin.
 */
	Vector3 extent;
	ComputeConstraintInfo(i_obb, d_link[i_node].d_newParticleInWorld, d_scale *
			d_dynamicLink[i_node].d_constraintRadius, o_nodeInOBB, extent);
/*
 *	Find the distance from the node to each outside face of the box. If the node
 *	is outside the box, return.
 */
	Vector3 distVector[2];
	float dist[6];
	distVector[0] = o_nodeInOBB + extent;
	distVector[1] = extent - o_nodeInOBB;
	int closestFace = 0;
	dist[0] = distVector[0].x();
	dist[1] = distVector[0].y();
	dist[2] = distVector[0].z();
	dist[3] = distVector[1].x();
	dist[4] = distVector[1].y();
	dist[5] = distVector[1].z();
	for (int i = 1; i < 6; i++)
	{
		if (dist[i] <= 0.0f)
			return;
		if (dist[i] < dist[closestFace])
			closestFace = i;
	}
/*
 *	Shrink the size of the hole by the constraint radius of the node. Only try
 *	to constrain to the hole if the hole is big enough for the node.
 */
	float constraintRadius = d_constraintScale * d_scale *
			d_dynamicLink[i_node].d_constraintRadius;
	i_xHalfWidth -= constraintRadius;
	i_zHalfWidth -= constraintRadius;
	if ((i_xHalfWidth >= 0.0f) && (i_zHalfWidth >= 0.0f))
	{
/*
 *		Find the distance to the hole along the x-axis.
 */
		float xDist, zDist, holeDist;
		if (o_nodeInOBB.x() >= 0.0f)
			xDist = o_nodeInOBB.x() - i_xHalfWidth;
		else
			xDist = i_xHalfWidth - o_nodeInOBB.x();
/*
 *		Find the distance to the hole along the z-axis.
 */
		if (o_nodeInOBB.z() >= 0.0f)
			zDist = o_nodeInOBB.z() - i_zHalfWidth;
		else
			zDist = i_zHalfWidth - o_nodeInOBB.z();
/*
 *		The bigger of the two distances is the minimum movement required to
 *		constrain the node to the hole.
 */
		if (xDist > zDist)
			holeDist = xDist;
		else
			holeDist = zDist;
/*
 *		See if the node is closer to the hole than a face of the OBB.
 */
		if (holeDist <= dist[closestFace])
		{
/*
 *			Move the node along the x-axis to lie in the hole if necessary.
 */
			if (o_nodeInOBB.x() > i_xHalfWidth)
				o_nodeInOBB.x( i_xHalfWidth);
			else if (o_nodeInOBB.x() < -i_xHalfWidth)
				o_nodeInOBB.x( -i_xHalfWidth);
/*
 *			Move the node along the z-axis to lie in the hole if necessary.
 */
			if (o_nodeInOBB.z() > i_zHalfWidth)
				o_nodeInOBB.z( i_zHalfWidth);
			else if (o_nodeInOBB.z() < -i_zHalfWidth)
				o_nodeInOBB.z( -i_zHalfWidth);
/*
 *			The node is now in the hole so return.
 */
			return;
		}
	}
/*
 *	Move the node to the closest face of the OBB.
 */
	MoveToFace(closestFace, i_obb, o_nodeInOBB, extent,
			d_link[i_node].d_newParticleInWorld);
}

/**********************************************************************
 **********************************************************************/
void CDynamicChain::ConstrainLinkToHole(int i_link, OBB* i_obb,
		float i_xHalfWidth, float i_zHalfWidth)
{
/*
 *	If the end of the link cannot move, return. The constraint was imposed
 *	on the start of the link (if it could move) when the previous link was
 *	processed.
 */
	if (RigidAttach(i_link))
		return;
/*
 *	If the start of this link cannot move, make sure the end is dealt with
 *	then return.
 */
	if (RigidAttach(i_link - 1))
	{
/*
 *		If this is the last link or the end of the next link cannot move, impose
 *		the constraint on the end of this link.
 */
		if ((i_link == (d_numLinks - 1)) ||	 RigidAttach(i_link + 1))
		{
			ConstrainNodeToHole(i_link, i_obb, i_xHalfWidth, i_zHalfWidth,
					s_startInOBB);
		}
/*
 *		If the constraint can be imposed on the next link, transform the end of
 *		this link into OBB space in preparation.
 */
		else
		{
			s_startInOBB = (d_link[i_link].d_newParticleInWorld -
					(*(i_obb->Center()))) / (*(i_obb->Matrix()));
		}
		return;
	}
/*
 *	Compute info so the problem can be solved in terms of a line segment against
 *	an axis-aligned box centered at the origin.
 */
	Vector3 endInOBB, center, size, corner;
	ComputeLinkConstraintInfo(i_obb, i_link, endInOBB, center, size, corner);
/*
 *	Resolving the hole constraint requires the y-component of the size vector
 *	to be positive. Its orientation is not important otherwise so negating the
 *	vector is fine.
 */
	if (size.y() < 0.0f)
		size.Negate();
/*
 *	Collide the line segment with the OBB. If it does not intersect the box,
 *	return.
 */
	float depthSquared;
	Vector3 crossAxisDisplacement;
	ESeparatingPlane separatingPlane = ResolveOBBConstraint(i_obb, center, size,
			corner, depthSquared, crossAxisDisplacement);
	if (separatingPlane == NO_INTERSECTION)
	{
		s_startInOBB = endInOBB;
		return;
	}
/*
 *	Shrink the size of the hole by the constraint radius of the link. Only try
 *	to constrain to the hole if the hole is big enough for the link.
 */
	float constraintRadius = d_constraintScale * d_scale *
			d_dynamicLink[i_link].d_constraintRadius;
	i_xHalfWidth -= constraintRadius;
	i_zHalfWidth -= constraintRadius;
	if ((i_xHalfWidth >= 0.0f) && (i_zHalfWidth >= 0.0f))
	{
/*
 *		Set up to resolve the hole constraint.
 */
		static const uint AXIS_MASK  = 0x1;
		static const uint X_AXIS     = 0x0;
		static const uint Z_AXIS     = 0x1;
		static const uint SIDE_MASK  = 0x2;
		static const uint POS_SIDE   = 0x0;
		static const uint NEG_SIDE   = 0x2;
		static const uint PLANE_MASK = 0x4;
		static const uint WALL_PLANE = 0x0;
		static const uint EDGE_PLANE = 0x4;
		float overlap[8];
		float absSizeX = Math::Abs(size.x());
		float absSizeZ = Math::Abs(size.z());
/*
 *		Find the the distance to the faces of the hole.
 */
		float distTerm = absSizeX - i_xHalfWidth;
		overlap[X_AXIS | POS_SIDE | WALL_PLANE] = distTerm + center.x();
		overlap[X_AXIS | NEG_SIDE | WALL_PLANE] = distTerm - center.x();
		distTerm = absSizeZ - i_zHalfWidth;
		overlap[Z_AXIS | POS_SIDE | WALL_PLANE] = distTerm + center.z();
		overlap[Z_AXIS | NEG_SIDE | WALL_PLANE] = distTerm - center.z();
/*
 *		Compute the square of the distance to the face if the face is penetrated.
 */
		uint i, j;
		bool inHole = true;
		for (i = 0; i < 4; i++)
		{
			if (overlap[i] > 0.0f)
			{
				overlap[i] *= overlap[i];
				inHole = false;
			}
		}
/*
 *		If the link lies fully in the extended hole, return.
 */
		if (inHole)
		{
			s_startInOBB = endInOBB;
			return;
		}
/*
 *		Find a scaled distance from the link to planes defined by cross product
 *		of the top and bottom edges of the x-walls (the hole z-axis) and the link.
 */
		distTerm = corner.y() * absSizeX - i_xHalfWidth * size.y();
		float crossTerm = size.y() * center.x() - size.x() * center.y();
		overlap[X_AXIS | POS_SIDE | EDGE_PLANE] = distTerm + crossTerm;
		overlap[X_AXIS | NEG_SIDE | EDGE_PLANE] = distTerm - crossTerm;
/*
 *		Find a scaled distance from the link to planes defined by cross product
 *		of the top and bottom edges of the z-walls (the hole x-axis) and the link.
 */
		distTerm = corner.y() * absSizeZ - i_zHalfWidth * size.y();
		crossTerm = size.y() * center.z() - size.z() * center.y();
		overlap[Z_AXIS | POS_SIDE | EDGE_PLANE] = distTerm + crossTerm;
		overlap[Z_AXIS | NEG_SIDE | EDGE_PLANE] = distTerm - crossTerm;
/*
 *		If the the planes do not separate the link and the "outside" of the hole,
 *		find the square of the distance as well as the distance divided by the
 *		length of the unscaled separating axis.
 */
		float scaledOverlap[4];
		float sizeYSq = size.y() * size.y();
		float axisLengthSq[2];
		axisLengthSq[X_AXIS] = size.x() * size.x() + sizeYSq;
		axisLengthSq[Z_AXIS] = size.x() * size.x() + sizeYSq;
		for (i = 0; i < 4; i++)
		{
			j = i | EDGE_PLANE;
			if (overlap[j] > 0.0f)
			{
				scaledOverlap[i] = overlap[j] / axisLengthSq[i & AXIS_MASK];
				overlap[j] *= scaledOverlap[i];
			}
		}
/*
 *		For each wall of the hole, find the plane that requires the smallest
 *		displacement to not violate the wall. Then, find the largest of these
 *		wall displacements. This will be used as the measure of the adjustment
 *		needed to make the link fit in the hole.
 *
 */
		float maxDistSq = 0.0f;
		uint bestPlane[4];
		for (i = 0; i < 4; i++)
		{
			j = i | EDGE_PLANE;
			if (overlap[i] < overlap[j])
			{
				bestPlane[i] = WALL_PLANE;
				if (overlap[i] > maxDistSq)
					maxDistSq = overlap[i];
			}
			else
			{
				bestPlane[i] = EDGE_PLANE;
				if (overlap[j] > maxDistSq)
					maxDistSq = overlap[j];
			}
		}
/*
 *		If less displacement is required to fit the link in the hole than to
 *		move it out of the OBB, fit the link in the hole and return.
 */
		if (maxDistSq < depthSquared)
		{
/*
 *			Fit the link in the hole for each axis.
 */
			float startX = s_startInOBB.x();
			float startZ = s_startInOBB.z();
			float endX = endInOBB.x();
			float endZ = endInOBB.z();
			float sizeX = size.x();
			float sizeZ = size.z();

			float* startHorizontal = &startX;
			float* endHorizontal = &endX;
			float* halfWidth = &i_xHalfWidth;
			float* horizontalSize = &sizeX;
			for (uint axis = 0; axis < 2; axis++)
			{
/*
 *				Find the side that requires the largest displacement.
 */
				uint side = (overlap[axis | POS_SIDE | bestPlane[axis | POS_SIDE]] >
						overlap[axis | NEG_SIDE | bestPlane[axis | NEG_SIDE]]) ?
						POS_SIDE : NEG_SIDE;
/*
 *				See if displacement is required.
 */
				if (overlap[axis | side | bestPlane[axis | side]] > 0.0f)
				{
/*
 *					Flip signs if using the negative side. Can now proceed as if
 *					the fit starts on the positive side.
 */
					if (side == NEG_SIDE)
					{
						*startHorizontal = -(*startHorizontal);
						*endHorizontal = -(*endHorizontal);
					}
/*
 *					Fit to the wall plane.
 */
					if (bestPlane[axis | side] == WALL_PLANE)
					{
						if (*startHorizontal > *halfWidth)
							*startHorizontal = *halfWidth;
						if (*endHorizontal > *halfWidth)
							*endHorizontal = *halfWidth;
					}
/*
 *					Fit to the edge plane.
 */
					else
					{
						float horizontalDisplacement = -size.y() *
								scaledOverlap[axis | side];
						float verticalDisplacement;
						if (side == POS_SIDE)
						{
							verticalDisplacement = (*horizontalSize) *
									scaledOverlap[axis | side];
						}
						else
						{
							verticalDisplacement = -(*horizontalSize) *
									scaledOverlap[axis | side];
						}
						*startHorizontal += horizontalDisplacement;
						s_startInOBB.y(s_startInOBB.y() + verticalDisplacement);
						*endHorizontal += horizontalDisplacement;
						endInOBB.y(endInOBB.y() + verticalDisplacement);
					}
/*
 *					Now fit the opposite side. At this point it is known that at
 *					least one end of the link touches or extends beyond the
 *					"positive" wall. Identify the ends by the side they are nearest.
 */
					float *positiveHorizontal, *negativeHorizontal;
					float positiveVertical, negativeVertical;
					if (*startHorizontal > *endHorizontal)
					{
						positiveHorizontal = startHorizontal;
						positiveVertical = s_startInOBB.y();
						negativeHorizontal = endHorizontal;
						negativeVertical = endInOBB.y();
					}
					else
					{
						positiveHorizontal = endHorizontal;
						positiveVertical = endInOBB.y();
						negativeHorizontal = startHorizontal;
						negativeVertical = s_startInOBB.y();
					}
/*
 *					See if the link extends beyond the "negative" wall.
 */
					if (*negativeHorizontal < -(*halfWidth))
					{
/*
 *						Find the y-position of the point on the link that touches the
 *						"positive" wall.
 */
						float pivotY;
						if (positiveVertical > corner.y())
							pivotY = corner.y();
						else if (positiveVertical < -corner.y())
							pivotY = -corner.y();
						else
							pivotY = positiveVertical;
/*
 *						Find half the y-distance between the point on the link that
 *						touches the "negative" wall and the pivot;
 */
						float adjustSlope;
						if (negativeVertical > corner.y())
							adjustSlope = 0.5f * (pivotY - corner.y());
						else if (negativeVertical < -corner.y())
							adjustSlope = 0.5f * (pivotY + corner.y());
						else
							adjustSlope = 0.5f * (pivotY - negativeVertical);
/*
 *						If this distance is zero, the link is horizontal. Just move
 *						the "negative" end to the wall.
 */
						if (Math::DivByZero(*halfWidth, adjustSlope))
							*negativeHorizontal = -(*halfWidth);
/*
 *						The link is not horizontal.
 */
						else
						{
/*
 *							Find the ratio of the width of the hole to the pivot to
 *							negative touch distance.
 */
							adjustSlope = *halfWidth / adjustSlope;
/*
 *							Find the "negative" horizontal position needed to make
 *							the link touch the "negative" wall.
 */
							float proposedHorizontal = *halfWidth +
									(negativeVertical - pivotY) * adjustSlope;
/*
 *							Move the endpoints horizontally to enforce the constraint
 *							if the "negative" end will move closer to the middle of
 *							the hole. This will pivot and shrink the link.
 */
							if (proposedHorizontal > *negativeHorizontal)
							{
								*negativeHorizontal = proposedHorizontal;
								*positiveHorizontal = *halfWidth +
										(positiveVertical - pivotY) * adjustSlope;
							}
						}
					}
/*
 *					Flip signs back if using the negative side.
 */
					if (side == NEG_SIDE)
					{
						*startHorizontal = -(*startHorizontal);
						*endHorizontal = -(*endHorizontal);
					}
				}
/*
 *				Prep to do the z-axis.
 */
				startHorizontal = &startZ;
				endHorizontal = &endZ;
				halfWidth = &i_zHalfWidth;
				horizontalSize = &sizeZ;
			}

			s_startInOBB.x( startX);
			s_startInOBB.z( startZ);
			endInOBB.x( endX);
			endInOBB.z( endZ);
			size.x( sizeX);
			size.z( sizeZ);
/*
 *			Transform the new endpoints into world space.
 */
			d_link[i_link - 1].d_newParticleInWorld = s_startInOBB *
					(*(i_obb->Matrix())) + (*(i_obb->Center()));
			d_link[i_link].d_newParticleInWorld = endInOBB *
					(*(i_obb->Matrix())) + (*(i_obb->Center()));
/*
 *			Prep for the next link and return.
 */
			s_startInOBB = endInOBB;
			return;
		}
	}
/*
 *	Move the link out of the OBB.
 */
	switch(separatingPlane)
	{
	 case POS_X_FACE:
		if (s_startInOBB.x() < corner.x())
			s_startInOBB.x( corner.x());
		if (endInOBB.x() < corner.x())
			endInOBB.x( corner.x());
		break;
	 case NEG_X_FACE:
		if (s_startInOBB.x() > -corner.x())
			s_startInOBB.x( -corner.x());
		if (endInOBB.x() > -corner.x())
			endInOBB.x( -corner.x());
		break;
	 case POS_Y_FACE:
		if (s_startInOBB.y() < corner.y())
			s_startInOBB.y( corner.y());
		if (endInOBB.y() < corner.y())
			endInOBB.y( corner.y());
		break;
	 case NEG_Y_FACE:
		if (s_startInOBB.y() > -corner.y())
			s_startInOBB.y( -corner.y());
		if (endInOBB.y() > -corner.y())
			endInOBB.y( -corner.y());
		break;
	 case POS_Z_FACE:
		if (s_startInOBB.z() < corner.z())
			s_startInOBB.z( corner.z());
		if (endInOBB.z() < corner.z())
			endInOBB.z( corner.z());
		break;
	 case NEG_Z_FACE:
		if (s_startInOBB.z() > -corner.z())
			s_startInOBB.z( -corner.z());
		if (endInOBB.z() > -corner.z())
			endInOBB.z( -corner.z());
		break;
	 case CROSS_AXIS_FACE:
		s_startInOBB += crossAxisDisplacement;
		endInOBB += crossAxisDisplacement;
		break;
	}
/*
 *	Transform the new endpoints into world space and prep for the next link.
 */
	d_link[i_link - 1].d_newParticleInWorld = s_startInOBB *
			(*(i_obb->Matrix())) + (*(i_obb->Center()));
	d_link[i_link].d_newParticleInWorld = endInOBB *
			(*(i_obb->Matrix())) + (*(i_obb->Center()));
	s_startInOBB = endInOBB;
}

void CDynamicChain::DrawConstraint(int i_startColor, bool i_drawBone)
{
	int i;
#if 0
	if (!RigidAttach(0))
	{
		DrawUtility::Sphere(d_link[0].d_newParticleInWorld,
				(DrawUtility::COLOR)(i_startColor % 7), d_constraintScale *
				d_scale * d_dynamicLink[0].d_constraintRadius);
		i_startColor++;
	}

	for (i = 1; i < d_numLinks; i++)
	{
		DrawUtility::COLOR color = (DrawUtility::COLOR)(i_startColor % 7);
		if (!RigidAttach(i))
			DrawUtility::Sphere(d_link[i].d_newParticleInWorld, color, d_scale *
					d_constraintScale * d_dynamicLink[i].d_constraintRadius);
		if (i_drawBone)
		{
			DrawUtility::Line(d_link[i - 1].d_newParticleInWorld,
					d_link[i].d_newParticleInWorld, color);
			DrawUtility::Point(d_link[i - 1].d_newParticleInWorld, color, 0.05f);
			DrawUtility::Point(d_link[i].d_newParticleInWorld, color, 0.05f);
		}
		i_startColor++;
	}
#else
	if (!RigidAttach(0) && RigidAttach(1))
	{
		DrawUtility::Sphere(d_link[0].d_newParticleInWorld,
				(DrawUtility::COLOR)(i_startColor % 7), d_constraintScale *
				d_scale * d_dynamicLink[0].d_constraintRadius);
		i_startColor++;
	}

	for (i = 1; i < d_numLinks; i++)
	{
		DrawUtility::COLOR color = (DrawUtility::COLOR)(i_startColor % 7);
		bool drawSomething = false;

		if (RigidAttach(i - 1))
		{
			if (!RigidAttach(i) && ((i == (d_numLinks - 1)) || RigidAttach(i + 1)))
			{
				DrawUtility::Sphere(d_link[i].d_newParticleInWorld,
						(DrawUtility::COLOR)(i_startColor % 7), d_constraintScale *
						d_scale * d_dynamicLink[i].d_constraintRadius);
				drawSomething = true;
			}
		}
		else if (!RigidAttach(i))
		{
			DrawUtility::Lozenge(d_link[i - 1].d_newParticleInWorld,
					d_link[i].d_newParticleInWorld, d_scale *
					d_constraintScale * d_dynamicLink[i].d_constraintRadius, color);
			DrawUtility::Lozenge(d_link[i - 1].d_newParticleInWorld,
					d_link[i].d_newParticleInWorld, d_scale *
					1.414f * d_constraintScale * d_dynamicLink[i].d_constraintRadius,
					(DrawUtility::COLOR)((uint)color + 8));
			drawSomething = true;
		}

		if (i_drawBone)
		{
			DrawUtility::Line(d_link[i - 1].d_newParticleInWorld,
					d_link[i].d_newParticleInWorld, color);
			DrawUtility::Point(d_link[i - 1].d_newParticleInWorld, color, 0.05f);
			DrawUtility::Point(d_link[i].d_newParticleInWorld, color, 0.05f);
			drawSomething = true;
		}

		if (drawSomething)
			i_startColor++;
	}
#endif
}

void CDynamicChain::SetBoxConstraintParent(ts_Bone* i_parent,
		Cartesian4x4 &i_boxToParent)
{
	d_boxConstraintParent = i_parent;
	if (d_boxConstraintParent)
		d_boxConstraintToParent = i_boxToParent;
	else
	{
		d_boxConstraintToWorld = i_boxToParent;
		i_boxToParent.Invert(d_worldToBoxConstraint);
	}
}

void CDynamicChain::SetLocations(Vector3 *i_particleInWorld, int i_iterations)
{
	int i;
	for (i = 0; i < d_numLinks; i++)
	{
		if (RigidAttach(i))
		{
			d_dynamicLink[i].d_currInvMass = 0.0f;
			if (d_dynamicLink[i].d_control)
			{
				d_link[i].d_newParticleInWorld = (d_dynamicLink[i].d_attachPoint *
						(*boneRunBonesToBone(d_dynamicLink[i].d_control))) *
						(*d_modelToWorld);
			}
			else
				d_link[i].d_newParticleInWorld = d_dynamicLink[i].d_attachPoint;
		}
		else
		{
			d_dynamicLink[i].d_currInvMass = d_dynamicLink[i].d_baseInvMass;
			d_link[i].d_newParticleInWorld = i_particleInWorld[i];
		}
	}

	d_boxConstraintActive = false;
	EnforceConstraints(NULL, i_iterations);
	
	for (i = 0; i < d_numLinks; i++)
	{
		d_dynamicLink[i].d_particleInWorld = d_link[i].d_newParticleInWorld;
		d_dynamicLink[i].d_targetInWorld = d_dynamicLink[i].d_particleInWorld;
		d_dynamicLink[i].d_newTargetInWorld = d_dynamicLink[i].d_targetInWorld;
		d_dynamicLink[i].d_velocityInWorld.Clear();
	}

	AdjustBones();
}

#ifdef ENABLE_RECORD
bool CDynamicChain::CreateRecordBuffer(int i_numTicks)
{
	if (d_dynamicChainRecordBuffer)
		return (false);

	if (!CChain::CreateRecordBuffer(i_numTicks))
		return(false);

	d_dynamicChainRecordBuffer = new SDynamicChainRecordEntry[i_numTicks];
	if (d_dynamicChainRecordBuffer == NULL)
	{
		CChain::DeleteRecordBuffer();
		return (false);
	}

	int i;
	for (i = 0; i < i_numTicks; i++)
	{
		d_dynamicChainRecordBuffer[i].d_dynamicLink =
				new SDynamicChainRecordEntry::SRecordDynamicLink[d_numLinks];
		if (d_dynamicChainRecordBuffer[i].d_dynamicLink == NULL)
			break;
	}
	if (i < i_numTicks)
	{
		for (i--; i >= 0; i--)
			delete[] d_dynamicChainRecordBuffer[i].d_dynamicLink;
		delete[] d_dynamicChainRecordBuffer;
		d_dynamicChainRecordBuffer = NULL;
		CChain::DeleteRecordBuffer();
		return (false);
	}
	return (true);
}

void CDynamicChain::DeleteRecordBuffer(void)
{
	if (d_dynamicChainRecordBuffer == NULL)
		return;
	for (int i = 0; i < d_numRecordTicks; i++)
		delete[] d_dynamicChainRecordBuffer[i].d_dynamicLink;
	delete[] d_dynamicChainRecordBuffer;
	d_dynamicChainRecordBuffer = 0;
}

void CDynamicChain::RecordState(int i_entry)
{
	if (d_dynamicChainRecordBuffer)
	{
		for (int i = 0; i < d_numLinks; i++)
		{
			d_dynamicChainRecordBuffer[i_entry].d_dynamicLink[i].d_particleInWorld
					= d_dynamicLink[i].d_particleInWorld;
			d_dynamicChainRecordBuffer[i_entry].d_dynamicLink[i].d_newTargetInWorld
					= d_dynamicLink[i].d_newTargetInWorld;
		}
		d_dynamicChainRecordBuffer[i_entry].d_deltaSec = d_deltaSec;
	}
}

void CDynamicChain::LoadState(int i_entry)
{
	if (d_dynamicChainRecordBuffer)
	{
		for (int i = 0; i < d_numLinks; i++)
		{
			d_dynamicLink[i].d_particleInWorld = d_dynamicChainRecordBuffer[
					i_entry].d_dynamicLink[i].d_particleInWorld;
			d_dynamicLink[i].d_newTargetInWorld = d_dynamicChainRecordBuffer[
					i_entry].d_dynamicLink[i].d_newTargetInWorld;
		}
		d_deltaSec = d_dynamicChainRecordBuffer[i_entry].d_deltaSec;
	}
}
#endif
