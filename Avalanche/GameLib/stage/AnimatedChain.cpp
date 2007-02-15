/*
**
**  File:   DynamicChain.h
**  Date:   April 19, 2001
**  By:     Bryant Collard
**  Desc:   Applies dynamics to chained bones in a model.
**
**  Copyright (c) 2001, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/stage/AnimatedChain.cpp $
**  $Revision: 31 $
**      $Date: 6/18/03 12:30p $
**    $Author: Bcollard $
**
*/

/******************* includes ****************************/

#include "stage/StagePCH.h"

///////////////////////////    CActorChains    //////////////////////////

/**********************************************************************
 *
 *  Class:       CActorChains
 *  Function:    CActorChains()
 *  Access:      public
 *  Description: Default constructor.
 *  Arguments:   None
 *  Return:      None
 *
 **********************************************************************/
CActorChains::CActorChains()
{
	d_head = NULL;
	d_motionChannelTable = NULL;
	d_motionChannel = NULL;
}

/**********************************************************************
 *
 *  Class:       CActorChains
 *  Function:    ~CActorChains()
 *  Access:      public
 *  Description: Destructor
 *  Arguments:   None
 *  Return:      None
 *
 **********************************************************************/
CActorChains::~CActorChains()
{
	delete d_head;
	if ( (d_motionChannelTable != NULL) && (d_motionChannel != NULL) )
	{
		d_motionChannelTable->RemoveChannel( &d_motionChannel );
	}
}

/**********************************************************************
 *
 *  Class:       CActorChains
 *  Function:    BuildChains (static)
 *  Access:      public
 *  Description: Builds a collection of chains for an actor.
 *  Arguments:
 *      i_actor - The actor.
 *  Return:      Pointer to a CActorChains object if any chains were
 *               found and successfully constructed; otherwise, NULL.
 *
 **********************************************************************/
CActorChains* CActorChains::BuildChains(CActorInstance* i_inst)
{
	ASSERT(i_inst);
	char name[MAX_BONENAME_LEN];
/*
 *	Declare a pointer to receive a collection of chains and set it to
 *	NULL to indicate that one has not been created yet.
 */
	CActorChains* chainList = NULL;
/*
 *	Find the body and the first bone. Loop through each bone.
 */
	ts_Body* body = i_inst->Body();
	if (!body)
		return(NULL);
	ts_ModelBone* bone = body->pMBones;
	if (!bone)
		return(NULL);
	for(int i = 0; i < body->usBones; i++, bone++)
	{
/*
 *		Look for an ! in the bone name which indicates that the bone
 *		is a member of a chain.
 */
		int j;
		for (j = 0; j < MAX_BONENAME_LEN; j++)
		{
			if (bone->cBoneName[j] == '!')
			{
/*
 *				Extract the name of the chain from the bone then build it if
 *				has not already been built.
 */
				strncpy (name, bone->cBoneName, j);
				name[j] = '\0';
				if ((chainList == NULL) || !(chainList->d_head->IsName(name)))
				{
/*
 *					Constuct a new chain and initialize it. Exit if the
 *					chain cannot be built.
 */
					MEM_SET_ONE_SHOT_NEW_NAME("CActorChains::BuildChains");
					CAnimatedChain* chain = new CAnimatedChain;
					if ((chain == NULL) || !chain->Initialize(i_inst, name))
					{
						ASSERT(false);
						delete chain;
						delete chainList;
						chainList = NULL;
						return(NULL);
					}
/*
 *					See if the chain is a partner of an existing chain and
 *					add it if it is.
 */
					CAnimatedChain* partner = NULL;
					if (chainList != NULL)
					{
						partner = chainList->d_head;
						while (partner != NULL)
						{
							if (partner->TryAddPartner(chain))
								break;
							partner = partner->GetNextPartnership();
						}
					}
/*
 *					The new chain is not a partner of an existing chain.
 */
					if (partner == NULL)
					{
/*
 *						Construct and initialize a collection of chains if
 *						one does not yet exist.
 */
						if (chainList == NULL)
						{
							MEM_SET_ONE_SHOT_NEW_NAME("CActorChains::BuildChains");
							chainList = new CActorChains;
							if (chainList == NULL)
							{
								ASSERT(false);
								delete chain;
								return(NULL);
							}
/*
 *							Start the collection with the new chain.
 */
							chainList->d_head = chain;
							chain->SetNext(NULL);
						}
						else
						{
/*
 *							Add the new chain to the collection.
 */
							BoneGroupID group = chain->GetBoneGroup();
							if (group < chainList->d_head->GetBoneGroup())
							{
								chain->SetNext(chainList->d_head);
								chainList->d_head = chain;
							}
							else
							{
								CAnimatedChain* last = chainList->d_head;
								CAnimatedChain* curr;
								while ((curr = last->GetNextPartnership()) != NULL)
								{
									if (group < curr->GetBoneGroup())
										break;
									last = curr;
								}
								chain->SetNext(curr);
								last->SetNext(chain);
							}
						}
					}
				}  // End building chain.
/*
 *				The bone that is a member of a chain has been processed.
 *				Move on to the next bone.
 */
				break;
			}  // End ! in name.
		}  // End looping through characters in the name.
	}  // End looping through bones.
/*
 *	Final setup.
 */
	if (chainList)
	{
/*
 *		Process each partnership.
 */
		BoneGroupBitMask boneGroupBitMask = 0;
		CAnimatedChain* partner = chainList->d_head;
		while (partner != NULL)
		{
/*
 *			Set the next pointer for every member of a partnership to point
 *			to the next partnership.
 */
			CAnimatedChain* curr = partner->GetPartner();
			while(curr)
			{
				curr->SetNext(partner->GetNextPartnership());
				curr = curr->GetPartner();
			}
/*
 *			Set the group bit for the partnership.
 */
			boneGroupBitMask |= 1 << partner->GetBoneGroup();
			partner = partner->GetNextPartnership();
		}
/*
 *	Hook up to the animation system.
 */
		chainList->RegisterAnimation(i_inst, boneGroupBitMask);
	}

	return(chainList);
}

void CActorChains::LoadParams(void)
{
	CAnimatedChain* curr = d_head;
	while(curr)
	{
		curr->LoadParams();
		curr = curr->GetNextChain();
	}
}

void CActorChains::RegisterAnimation(CActorInstance* i_inst,
		BoneGroupBitMask i_bitMask)
{
	if (i_bitMask)
	{
/*
 *		Make sure the animation system setup is clean.
 */
		if ((d_motionChannelTable != NULL) && (d_motionChannel != NULL))
			d_motionChannelTable->RemoveChannel( &d_motionChannel );
/*
 *		Register animation callback.
 */
		d_motionChannelTable = i_inst->GetMotionChannelTable();
		d_motionChannelTable->AddChannel( (BoneGroupAnimFunction*)&RunCallback, (void*)this, (u32)100, i_bitMask, NULL, &d_motionChannel );
	}
}

/**********************************************************************
 *
 *  Class:       CActorChains
 *  Function:    RunCallback (static)
 *  Access:      public
 *  Description: Passes the run callback through.
 *  Arguments:   None
 *  Return:      None
 *
 **********************************************************************/
void CActorChains::RunCallback(void* i_context, ts_Body* i_body,
		BoneGroupID i_boneGroup, ts_MotnInf* i_motionInfo)
{
	((CActorChains*)i_context)->Run(i_boneGroup, i_motionInfo);
}

/**********************************************************************
 *
 *  Class:       CActorChains
 *  Function:    Run
 *  Access:      public
 *  Description: Finds partnership then runs its chain motion.
 *  Arguments:
 *      i_bondGroup  - The bone group to run.
 *      i_motionInfo - Motion info.
 *  Return:      None
 *
 **********************************************************************/
void CActorChains::Run(BoneGroupID i_boneGroup, ts_MotnInf *i_motionInfo)
{
	CAnimatedChain* curr = d_head;
	while(curr)
	{
		if (i_boneGroup == curr->GetBoneGroup())
		{
			curr->Run(i_motionInfo);
			return;
		}
		curr = curr->GetNextPartnership();
	}
}

/**********************************************************************
 *
 *  Class:       CActorChains
 *  Function:    ResetChains
 *  Access:      public
 *  Description: Initializes particles in the chains.
 *  Arguments:   None
 *  Return:      None
 *
 **********************************************************************/
void CActorChains::ResetChains(void)
{
	CAnimatedChain* curr = d_head;
	while(curr)
	{
		curr->Reset();
		curr = curr->GetNextPartnership();
	}
}

/**********************************************************************
 *
 *  Class:       CActorChains
 *  Function:    DisableChains
 *  Access:      public
 *  Description: Turns chain animation off.
 *  Arguments:   None
 *  Return:      None
 *
 **********************************************************************/
void CActorChains::DisableChains(void)
{
	CAnimatedChain* curr = d_head;
	while(curr)
	{
		curr->SetDisable(true);
		curr = curr->GetNextChain();
	}
}

/**********************************************************************
 *
 *  Class:       CActorChains
 *  Function:    EnableChains
 *  Access:      public
 *  Description: Turns chain animation on.
 *  Arguments:   None
 *  Return:      None
 *
 **********************************************************************/
void CActorChains::EnableChains(void)
{
	CAnimatedChain* curr = d_head;
	while(curr)
	{
		curr->SetDisable(false);
		curr = curr->GetNextChain();
	}

	curr = d_head;
	while(curr)
	{
		curr->Reset();
		curr = curr->GetNextPartnership();
	}
}

/**********************************************************************
 *
 *  Class:       CActorChains
 *  Function:    SetDeltaSec
 *  Access:      public
 *  Description: Updates the time step.
 *  Arguments:
 *      i_deltaSec - The time step.
 *  Return:      None
 *
 **********************************************************************/
void CActorChains::SetDeltaSec(float i_deltaSec)
{
	CAnimatedChain* curr = d_head;
	while(curr)
	{
		curr->SetDeltaSec(i_deltaSec);
		curr = curr->GetNextChain();
	}
}

/**********************************************************************
 *
 *  Class:       CActorChains
 *  Function:    SetObjectList
 *  Access:      public
 *  Description: Propogates an object list.
 *  Arguments:
 *      i_objectList - The list.
 *  Return:      None
 *
 **********************************************************************/
void CActorChains::SetObjectList(CObjectList* i_objectList)
{
	CAnimatedChain* curr = d_head;
	while(curr)
	{
		curr->SetObjectList(i_objectList);
		curr = curr->GetNextChain();
	}
}

void CActorChains::SetConstraintScale(float i_scale)
{
	CAnimatedChain* curr = d_head;
	while(curr)
	{	
		curr->SetConstraintScale(i_scale);
		curr = curr->GetNextChain();
	}
}

void CActorChains::GetExtents(Vector3 &io_minExtent, Vector3 &io_maxExtent,
		bool i_compare)
{
	if (d_head)
	{
		d_head->GetPartnershipExtents(io_minExtent, io_maxExtent, i_compare);

		CAnimatedChain* curr = d_head->GetNextPartnership();
		while(curr)
		{
			curr->GetPartnershipExtents(io_minExtent, io_maxExtent, true);
			curr = curr->GetNextPartnership();
		}
	}
}

bool CActorChains::DidCollide(void)
{
	CAnimatedChain* curr = d_head;
	while(curr)
	{
		if (curr->DidCollide())
			return(true);
		curr = curr->GetNextChain();
	}
	return(false);
}


///////////////////////////    CAnimatedChain    //////////////////////////

/**********************************************************************
 *
 *  Class:       CAnimatedChain
 *  Function:    CAnimatedChain()
 *  Access:      public
 *  Description: Default constructor.
 *  Arguments:   None
 *  Return:      None
 *
 **********************************************************************/
CAnimatedChain::CAnimatedChain()
{
	d_next = NULL;
	d_currentDeltaSec = 0.0f;
}

/**********************************************************************
 *
 *  Class:       CAnimatedChain
 *  Function:    ~CAnimatedChain()
 *  Access:      public
 *  Description: Destructor
 *  Arguments:   None
 *  Return:      None
 *
 **********************************************************************/
CAnimatedChain::~CAnimatedChain()
{
	if (d_partner == NULL)
		delete d_next;
}

/**********************************************************************
 *
 *  Class:       CAnimatedChain
 *  Function:    Initialize
 *  Access:      public
 *  Description: Constructs a chain, given a name, from an actor.
 *  Arguments:
 *      i_actor - The actor.
 *      i_name  - The name.
 *  Return:      true for success, false for failure.
 *
 **********************************************************************/
bool CAnimatedChain::Initialize(CActorInstance* i_inst, char* i_name)
{
	if (!SetName(i_name))
		return(InitializeFail());
/*
 *	Find the the body and the first bone. Loop through each bone just
 *	counting chain members.
 */
	int i;
	ts_Body* body = i_inst->Body();
	ts_Bone* bone = body->pBones;
	int numLinks = 1;
	int len = strlen(i_name);
	for(i = 0; i < body->usBones; i++, bone++)
	{
		if (!strnicmp(bone->pModelBone->cBoneName, i_name, len) &&
				(bone->pModelBone->cBoneName[len] == '!'))
			numLinks++;
	}
	if (!SetNumLinks(numLinks))
		return(InitializeFail());
/*
 *	Get pointers to each real link (all but the first one) and to any control
 *	bones.
 */
	char name[MAX_BONENAME_LEN + 1];
	for (i = 1; i < numLinks; i++)
	{
		sprintf(name, "%s!%d", d_name, i);
		boneGetBone(body, &bone, name);
		if (!InitLink(i, bone))
			return(InitializeFail());

		sprintf(name, "%s#%d", d_name, i);
		if (boneGetBone(body, &(d_dynamicLink[i].d_control), name))
			d_dynamicLink[i].d_controlMask = LINK_TARGET;

		sprintf(name, "%s@%d", d_name, i);
		if (boneGetBone(body, &bone, name))
		{
			if (d_dynamicLink[i].d_control)
			{
				char msgStr[256];
				sprintf(msgStr, "Overspecified chain control bone for %s!%d\n",
						d_name, i);
				conPuts(msgStr);
				sprintf(msgStr,
						"    by including both bones %s#%d and %s@%d\n",
						d_name, i, d_name, i);
				conPuts(msgStr);
				ASSERT(false);
				return(InitializeFail());
			}
			d_dynamicLink[i].d_control = bone;
			d_dynamicLink[i].d_controlMask = LINK_ATTACH;
		}

		sprintf(name, "%s&%d", d_name, i);
		if (boneGetBone(body, &bone, name))
		{
			if (d_dynamicLink[i].d_control)
			{
				char msgStr[256];
				sprintf(msgStr, "Overspecified chain control bone for %s!%d\n",
						d_name, i);
				conPuts(msgStr);
				if (d_dynamicLink[i].d_controlMask == LINK_TARGET)
					sprintf(msgStr,
							"    by including both bones %s#%d and %s&%d\n",
							d_name, i, d_name, i);
				else
					sprintf(msgStr,
							"    by including both bones %s@%d and %s&%d\n",
							d_name, i, d_name, i);
				conPuts(msgStr);
				ASSERT(false);
				return(InitializeFail());
			}
			d_dynamicLink[i].d_control = bone;
			d_dynamicLink[i].d_controlMask = LINK_ATTACH | LINK_TARGET;
		}
	}
/*
 *	See if a control point is provided for the first particle.
 */
	sprintf(name, "%s@0", d_name);
	if (boneGetBone(body, &(d_dynamicLink[0].d_control), name))
		d_dynamicLink[0].d_controlMask = LINK_ATTACH;

	sprintf(name, "%s&0", d_name);
	if (boneGetBone(body, &bone, name))
	{
		if (d_dynamicLink[0].d_control)
		{
			char msgStr[256];
			sprintf(msgStr, "Overspecified control for start of chain %s\n",
					d_name);
			conPuts(msgStr);
			sprintf(msgStr, "    by including both bones %s@0 and %s&0\n", d_name,
					d_name);
			conPuts(msgStr);
			ASSERT(false);
			return(InitializeFail());
		}
		d_dynamicLink[0].d_control = bone;
		d_dynamicLink[0].d_controlMask = LINK_ATTACH | LINK_TARGET;
	}
/*
 *	Make sure all the links are in the same group and that the proper
 *	parent/child relationships exist.
 */
	BoneGroupID group = d_link[1].d_bone->pModelBone->boneGroupID;
	for (i = 2; i < numLinks; i++)
	{
		if (d_link[i].d_bone->pModelBone->boneGroupID != group)
		{
			char msgStr[256];
			sprintf(msgStr, "Chain bone %s!%d is in group %d but is "
					"expected to be in group %d.\n", d_name, i,
					d_link[i].d_bone->pModelBone->boneGroupID, group);
			conPuts(msgStr);
			ASSERT(false);
			return(InitializeFail());
		}
	}

	if (!CompleteInit(i_inst, &(i_inst->Matrix())))
		return(InitializeFail());

	d_disabled = false;
	return(true);
}

/**********************************************************************
 *
 *  Class:       CAnimatedChain
 *  Function:    IsName
 *  Access:      public
 *  Description: Determines if a name is the same as the chain name.
 *  Arguments:
 *      i_queryName - The test name.
 *  Return:      true if the names are the same, false otherwise.
 *
 **********************************************************************/
bool CAnimatedChain::IsName(char* i_queryName)
{
	if (CDynamicChain::IsName(i_queryName))
		return(true);

	return(d_next && d_next->IsName(i_queryName));
}

/**********************************************************************
 *
 *  Class:       CAnimatedChain
 *  Function:    GetBoneGroup
 *  Access:      public
 *  Description: Returns the bone group.
 *  Arguments:   None
 *  Return:      The bone group.
 *
 **********************************************************************/
BoneGroupID CAnimatedChain::GetBoneGroup(void)
{
	ASSERT(d_numLinks > 1);
	return (d_link[1].d_bone->pModelBone->boneGroupID);
}

/**********************************************************************
 *
 *  Class:       CAnimatedChain
 *  Function:    Run
 *  Access:      public
 *  Description: Runs chain motion for a partnership of chains.
 *  Arguments:
 *      i_motionInfo - Motion info.
 *  Return:      None
 *
 **********************************************************************/
void CAnimatedChain::Run(ts_MotnInf *i_motionInfo)
{
	if (d_disabled)
		return;

	STAGE_PROFILE_START(CStageProfile::CHAIN_SIM);
/*
 *	If the motion is being restarted or if no time has passed, sync the
 *	particles with the model and zero out velocities.
 */
	if (i_motionInfo->u32MotnFlags & MOTNINF_FLAG_NEW_UPDATE)
	{
		Reset();
		i_motionInfo->interpTime = 0.0f;
	}
/*
 *	Update the position of the particles.
 */
	else
	{
//		float deltaSec = i_motionInfo->currentTime;
		Update(d_currentDeltaSec, d_objectList);
	}
/*
 *	Reset the integration time.
 */
//	i_motionInfo->usCurrentTime = 0;
	STAGE_PROFILE_END(CStageProfile::CHAIN_SIM);
}
