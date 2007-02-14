/**********************************************/
/*                                            */
/* Plant.cpp                                  */
/* big juju   3/26/02                         */
/* ned martin  avalanche software             */
/* originally by bryant collard,              */
/* i just fit it into derived CSimple scheme  */
/* visual c++ 6.0                             */
/* plants                                     */
/*                                            */
/**********************************************/

#include "TTLPCH.h"

#include "components/Plant.h"
#include "components/PlantParam.h"

#include "CollideHelper/AabbEntity.h"
#include "stage/EntityCategory.h"
#include "stage/ActorCreator.h"
#include "CollideHelper/ObbNode.h"
#include "geomUtil/BoxExtents.h"
#include "Game/Database/lightset.h"

/******************* defines *****************************/

/******************* macros ******************************/

/******************* structures **************************/

/******************* externs *****************************/

/******************* global variables ********************/
float g_featherTimer = 5.0f;

/******************* local variables *********************/

/******************* local prototypes ********************/

/******************* functions ***************************/

// Define a creator and add it to the creator list.
STANDARD_ACTOR_CREATOR(CPlant, plant);

/* constructor */

CPlant::CPlant() : CProtagonist( s_plantClass )
{
	d_pParam = NULL;
	d_interactionHint = HINT_IMPOTENT;
}

/* destructor */

CPlant::~CPlant()
{
	/* clean up plant parameters */

	if (d_pParam)
		d_pParam->Unregister(this);
}

/* initialize */

bool CPlant::Initialize(void)
{
	d_enableTime = 0.0f;
	d_scale = 1.0f;
	
	CInstance *pInst = Instance( 0 );
	if ( pInst )
	{
		if ( pInst->Model() )
		{
			const char *pModelName = pInst->Model()->Name;
			if ((pModelName) && (pModelName[0] != 0))
				d_pParam = CPlantParam::Register(this, pModelName);
		}
	}
	if (d_pParam == NULL)
		d_pParam = CPlantParam::Register(this, "FlowersA");	// plant crashes without params, try to get something
	if (d_pParam == NULL)
		return(false);

	/* build the obb */

	CObbNode* obbNode = new CObbNode;

	ts_Bone* bone;
	if (boneGetBone(Instance(0)->Body(), &bone, "Trunk"))
	{
		float scaleX, scaleY, scaleZ, shearXY, shearXZ, shearYZ;
		Vector3 boxInActor, boxMin, boxMax;
		DirCos3x3 boxToActor;
		Cartesian4x4 actorTransform(d_bodyToWorld, d_bodyInWorld);
		Graphics4x4 boxTransform = bone->pModelBone->boneToModel *
				Instance()->Matrix() / actorTransform;
		boxTransform.Deconstruct(boxToActor, boxInActor, scaleX, scaleY,
				scaleZ, shearXY, shearXZ, shearYZ);
		Matrix3x3 scaleShear(scaleX, 0.0f, 0.0f, scaleY * shearXY,
				scaleY, 0.0f, scaleZ * shearXZ, scaleZ * shearYZ, scaleZ);
		CBoxExtents::FindExtents(bone->pModelBone->mBoneBox.Max,
				boxInActor / boxToActor, scaleShear, boxMin, boxMax);
		obbNode->SetCollisionToActor(boxToActor);
		obbNode->SetExtents(boxMin, boxMax);
		SetPositiveInclude(ACTOR_CATEGORY);
		d_boxParent = bone;
		d_hasTrunk = true;
	}
	else
	{
		d_boxParent = NULL;
		SetPositiveInclude(DEBUG_DRAW_CATEGORY);
		obbNode->SetExtents(Vector3(-0.2f, 0.0f, -0.2f), Vector3(0.2f, 1.0f, 0.2f));
		d_hasTrunk = false;
//		MoveToNonProtagonistCast();
	}

	SetCollisionNode(obbNode, SIMPLE, true);

	if (CProtagonist::Initialize() == false)
		return(false);

	/* plant interaction category */

	d_category |= PENETRABLE_CATEGORY;

	/* make plants wood surface */

//	SetSurfaceType("wood");
	
	/* set up the leaves as dynamic chains */

	if (d_chains)
	{
		CAnimatedChain* curr = d_chains->GetHead();
		if (d_boxParent == NULL)
			d_boxParent = curr->GetBone(0);
		while (curr != NULL)
		{
			Cartesian4x4 boxConstraintToParent;
			boxConstraintToParent.SetXYZRotation(d_pParam->d_boxRotation[1]);
			boxConstraintToParent.PokeTranslation(d_pParam->d_boxLocation[1]);
			curr->SetBoxConstraintParent(d_boxParent, boxConstraintToParent);
			curr->SetGravity(0.0f, -9.81f, 0.0f);
			curr->SetMass(0, 1.0f);
			curr->SetControlMask(0, CDynamicChain::LINK_ATTACH);
			curr = curr->GetNextChain();
		}
	}
#ifdef NO_OBB_CHECKS
	d_firstFrame = true;
#endif //NO_OBB_CHECKS
	d_hidden = false;
	d_disabled = false;
	d_touched = NULL;
	d_bPlantCheck = false;
	d_sfxTimer = 0.0f;

	// doesn't move, don't reassign vis regions
	SceneMgr::SetInstanceVisRegionStatic( Instance( 0 ), false );

	/* success */

	return(TRUE);
}

/* set up plant parameters */

void CPlant::SetParams()
{
	if (d_chains)
	{
		int i;
		CAnimatedChain *curr = d_chains->GetHead();
		while (curr != NULL)
		{
			curr->SetIterationRate(d_pParam->d_iterationRate[1]);
			curr->SetGlobalDamping(d_pParam->d_globalDamping[1]);
			curr->DrawBoxConstraint(d_pParam->d_boxDisplay[1]);
			Cartesian4x4 boxConstraintToParent;
			boxConstraintToParent.SetXYZRotation(d_pParam->d_boxRotation[1]);
			boxConstraintToParent.PokeTranslation(d_pParam->d_boxLocation[1]);
			curr->SetBoxConstraintParent(d_boxParent, boxConstraintToParent);
			curr->SetBoxConstraintExtent(d_pParam->d_boxExtent[1]);
			float scale = 1.0f / (float)(curr->GetNumLinks() - 1);
			float massFact = scale * (d_pParam->d_tipMass[1] - d_pParam->d_baseMass[1]);
			float naturalFrequencyFact = scale * (d_pParam->d_tipNaturalFrequency[1] - d_pParam->d_baseNaturalFrequency[1]);
			float dampingFactorFact = scale * (d_pParam->d_tipDampingFactor[1] - d_pParam->d_baseDampingFactor[1]);
			float dragCoefficientFact = scale * (d_pParam->d_tipDragCoefficient[1] - d_pParam->d_baseDragCoefficient[1]);
			for (i = 1; i < curr->GetNumLinks(); i++)
			{
				curr->SetMass(i, massFact * (float)(i - 1) + d_pParam->d_baseMass[1]);
				curr->SetTargetDynamics(i, naturalFrequencyFact *
						(float)(i - 1) + d_pParam->d_baseNaturalFrequency[1],
						dampingFactorFact * (float)(i - 1) +
						d_pParam->d_baseDampingFactor[1]);
				curr->SetDragCoefficient(i, dragCoefficientFact * (float)(i - 1) + d_pParam->d_baseDragCoefficient[1]);
				curr->SetControlMask(i, CDynamicChain::LINK_DRAG |
						(d_pParam->d_freeSwinging[1] ? CDynamicChain::LINK_GRAVITY :
						CDynamicChain::LINK_TARGET) |
						(d_pParam->d_boxActive[1] ? CDynamicChain::LINK_BOX : 0));
			}
			curr = curr->GetNextChain();
		}
		float constraintScale = d_pParam->d_constraintScale[1];
		if (d_enableTime > 0.0f)
			constraintScale *=
					1.0f - (d_enableTime / d_pParam->d_obbInflateTime[1]);
		d_chains->SetConstraintScale(constraintScale);
	}
}

/* override CActor.LoadDynamicLinkageParams */

void CPlant::LoadDynamicLinkageParams(void)
{
	if (d_pParam == NULL)
	{
		const char* modelName = Instance()->Model()->Name;
		if ((modelName) && (modelName[0] != 0))
			d_pParam = CPlantParam::Register(this, modelName);
	}

	if (d_pParam)
		SetParams();
}

/* support for turning off collision, or toggling plants */
#ifdef CONSUMER_BUILD
	#define PLANTS_ON() true
#else
	#define TOGGLE_PLANTS
	#ifdef TOGGLE_PLANTS
		extern bool gbl_plants;	//debug: toggle plants on/off for speed test
		#define PLANTS_ON() gbl_plants 
	#else
		#define PLANTS_ON() true
	#endif
#endif

#ifdef NO_OBB_CHECKS
void CPlant::RunAnimation(void)
{
/*
 *  Bypass RunAnimation in CProtagonist to stop obb checks for now.
 *  Passing in a pointer to a bool makes sure the CActor is called directly.
 */

#if 1

	d_touched = NULL;
	if (d_firstFrame)
	{
#if 1
		/* update light color from height check (color is embedded in collision data) if this is a dynamically lit object*/
		if (Instance()->DynamicallyLit())
		{
			/* color data is only included in CollisionSystem::WORLD types (terrain and static obb's). for example, */
			/* environmental actor could be standing on top of a crate, which as a dynamic object has no color. */
			/* so look for the WORLD type */

			/* since a plant presumably doesn't move, get and set it's color multiplier only once */

			SetBaseAmbientLight( g_lightSet.getAmbientLight() );
			SetLightColorSource(LIGHT_COLOR_SOURCE_TERRAIN);//to avoid asset in SetLightColor()
			CXZEntity entity;
			entity.SetPositiveInclude(WORLD_CATEGORY);
			entity.FindFloor(d_bodyInWorld.x(), d_bodyInWorld.y() + 1.0f, d_bodyInWorld.z());
			g_collisionSystem.ResetStack();
			CCollisionList* list = g_collisionSystem.Test(&entity);
			if (list != NULL)
				SetLightColor(CLineQuery::Color(entity.Line(), list));
			else
				SetLightColor(0xffff);	// no data, use white (100% of defined lights)

			SetLightColorSource(LIGHT_COLOR_SOURCE_MANUAL);//do we need to set this so the light multipler is not re-computed?
		}
		
		/* set the plant to fade out before we hide it */

//		SetFadeOutDistance(g_cameraLOD.GetLODDistance(CBaseLOD::LOW) - 5.0f); 

		/* since we set this to fade, we may get a draw order problem.  Make sure that if this object has no alpha
		or isn't already put in a draw first or last layer, then set it to draw last 1 so it fades to whatever is behind it*/
		if ( Instance()->AlphaSort() == false && Instance()->RenderBucket() == 0 )
		{
			Instance()->SetRenderBucket( 1 );
		}
#endif

		if (d_chains)
		{
			d_chains->SetDeltaSec(g_timer.GetFrameSec());

			if (DistortionMgr__BuildApplyList(NULL, DISTORTION_AFFECTS_CHAINS))
				DistortionMgr__TransformDistortions(NULL, NULL);
		}

		if (Instance()->GetMotionChannelTable())
		{
			Instance()->GetMotionChannelTable()->Process( g_timer.GetFrameSec(), true);
		}

		Instance()->UpdateBoundingVolumes();

		d_firstFrame = false;
	}
	else if (!PLANTS_ON() || d_hidden ||
			(g_cameraLOD.GetLOD(d_bodyInWorld) >= CBaseLOD::PERIPHERY))
	{
	 	Hide(true);
		d_disabled = true;
	}
	else
	{
		if (!Math::Equal(d_scale, 1.0f))
			d_disabled = true;
		else
		{
			if (d_disabled)
			{
				d_disabled = false;
				d_enableTime = d_pParam->d_obbInflateTime[1];
			}
/*
 *			Find the time since being enabled as a fraction of the total inflate
 *			time and scale the chains constraints by it.
 */
			if (d_enableTime > 0.0f)
			{
				d_enableTime -= g_timer.GetFrameSec();
				if (d_chains)
				{
					float constraintScale = d_pParam->d_constraintScale[1];
					if (d_enableTime > 0.0f)
						constraintScale *= 1.0f -
								(d_enableTime / d_pParam->d_obbInflateTime[1]);
					d_chains->SetConstraintScale(constraintScale);
				}
			}
		}

		if (Instance()->GetMotionChannelTable() && d_chains && InScene() &&
				(g_cameraLOD.GetLOD(d_bodyInWorld) <= CBaseLOD::FOCUS) &&
				!d_disabled)
		{
			// Test obb's
			Vector3 minExtent, maxExtent;
			ASSERT(d_chains);
			d_chains->GetExtents(minExtent, maxExtent, false);
			CAabbEntity entity;
			entity.Aabb().GetAABB().Set(minExtent, maxExtent);
			g_collisionSystem.ResetStack();
			entity.SetPositiveExclude(TERRAIN_CATEGORY | PENETRABLE_CATEGORY | PLANT_IGNORES_CATEGORY);
			entity.SetIgnoreEntity(this);
			CCollisionList* list = g_collisionSystem.Test(&entity);
			CObjectListArray<50> objectList;
			objectList.Add(list);
			d_chains->SetObjectList(&objectList);
			if (list)
			{
				CCollisionEvent* event = list->GetHeadEvent();
				while (event)
				{
					if (d_touched)
					{
//						if (event->GetTargetEntity()->IsA(Avatar::ClassType()))
						{
							d_touched = (CProtagonist*)(event->GetTargetEntity());
							break;
						}
					}
					else if (event->GetTargetEntity()->IsA(CProtagonist::ClassType()))
						d_touched = (CProtagonist*)(event->GetTargetEntity());
					event = event->GetNext();
				}
			}
			d_chains->SetDeltaSec(g_timer.GetFrameSec());

			if (DistortionMgr__BuildApplyList(NULL, DISTORTION_AFFECTS_CHAINS))
				DistortionMgr__TransformDistortions(NULL, NULL);

			// JMH - timed sfx call
			Math::MoveToZero( &d_sfxTimer, g_timer.GetFrameSec() );
			
			//If we're no longer being hit by anything, reset our "plant check"
			if ((d_bPlantCheck) && (!d_touched))
				d_bPlantCheck = false;
			
			if ( d_touched )
			{
				// get velocity of top of plant
				ASSERT(d_chains->GetHead()->GetNumLinks() > 0);
				Vector3 velocity = d_chains->GetHead()->GetVelocityInWorld(d_chains->GetHead()->GetNumLinks() - 1);
				
				if ( ( d_sfxTimer == 0.0f) && ( velocity.LengthSquared() > ( 0.2f * 0.2f ) ) )
				{
					float vol = velocity.Length() * 0.25f;
					if ( vol > 1.0f )
						vol = 1.0f;

					SoundEmitter *emitter = g_audio.GetEmitter("plants");
					if (emitter == NULL)
					{
						if (g_audio.Create3DGroup("Plants", 4))
						{
							g_audio.DestroyGroupWhenEmpty("Plants");
							emitter = g_audio.Create3DEmitter("plants", "Plants");
							if (emitter)
							{
								emitter->KillOnSoftFlush();
								((SoundEmitter3D*)emitter)->GetParams().SetAudibleDistance(40.0f);
							}
						}
					}
					if (emitter && (emitter->Type() == SoundEmitter::EMITTER3D))
					{
						SoundHandle handle = emitter->Cue(d_pParam->d_touchedSound);
						if ( handle )
						{
							((SoundEmitter3D*)emitter)->SetPosition(handle, d_bodyInWorld);
							emitter->SetVolume(handle, vol);
							emitter->Play(handle);
							delete handle;
						}
					}
					d_sfxTimer = 0.4f;
				}
			}

			Instance()->GetMotionChannelTable()->Process( g_timer.GetFrameSec(), true);

#if 0
			if (d_chains->GetHead()->IsObjectList())
			{
				int color = 0;
				CAnimatedChain* curr = d_chains->GetHead();
				while(curr)
				{
					curr->DrawConstraint(color);
					curr->DrawBoneBoxes(color++);
					curr = curr->GetNextChain();
				}
			}
#endif
		}
		Hide(false);
	}

#else //0/1

	if(d_firstFrame)
		Instance()->UpdateBoundingVolumes();
    d_firstFrame = false;

#endif //0/1
}
#endif //NO_OBB_CHECKS

void CPlant::ScaleActor(void)
{
	Graphics4x4& localToWorld = (Graphics4x4 &)Instance()->Matrix();

	localToWorld.r00 *= d_scale;
	localToWorld.r01 *= d_scale;
	localToWorld.r02 *= d_scale;

	localToWorld.r10 *= d_scale;
	localToWorld.r11 *= d_scale;
	localToWorld.r12 *= d_scale;

	localToWorld.r20 *= d_scale;
	localToWorld.r21 *= d_scale;
	localToWorld.r22 *= d_scale;
}

#define SELECTION_MS  1500

void CPlant::Draw(CCollisionEvent* i_event)
{
	if (d_hasTrunk)
		d_collisionNode->GetCollisionObject()->Draw(DrawUtility::RED);

	if (d_chains)
	{
		Vector3 minExtent, maxExtent;
		d_chains->GetExtents(minExtent, maxExtent, false);
		DrawUtility::Box(minExtent, maxExtent);

		uint chainCount = 0;
		CAnimatedChain* curr = d_chains->GetHead();
		while(curr)
		{
			chainCount++;
			curr = curr->GetNextChain();
		}
		if (chainCount == 0)
			return;

		uint selection = (g_timer.GetStartMS() / SELECTION_MS) %
				(2 * chainCount + 3);

		if (selection == 0)
		{
			uint color = 0;
			CAnimatedChain* curr = d_chains->GetHead();
			while(curr)
			{
				curr->DrawBoneBoxes(color++, false);
				curr = curr->GetNextChain();
			}
			return;
		}

		if (selection == 1)
		{
			uint color = 0;
			CAnimatedChain* curr = d_chains->GetHead();
			while(curr)
			{
				curr->DrawConstraint(color++, false);
				curr = curr->GetNextChain();
			}
			return;
		}

		if (selection == 2)
			return;

		selection -= 3;

		if ((selection & 0x1) == 0)
		{
			selection >>= 1;
			uint color = 0;
			CAnimatedChain* curr = d_chains->GetHead();
			while(curr)
			{
				if (selection == color)
				{
					curr->DrawBoneBoxes(color, true);
					return;
				}
				color++;
				curr = curr->GetNextChain();
			}
		}
		else
		{
			selection >>= 1;
			uint color = 0;
			CAnimatedChain* curr = d_chains->GetHead();
			while(curr)
			{
				if (selection == color)
				{
					curr->DrawConstraint(color, true);
					return;
				}
				color++;
				curr = curr->GetNextChain();
			}
		}
	}
}

const char* CPlant::GetName(void)
{
	return("Plant");
}

void CPlant::AppendDescription(char* io_msg)
{
}
