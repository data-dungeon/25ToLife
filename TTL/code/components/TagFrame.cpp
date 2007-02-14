/*
**
**  File:   TagFrame.cpp
**  Date:   July 14, 2005
**  By:     Bryant Collard
**  Desc:   Component associated with tag frames.
**
**  Copyright (c) 2005, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/components/TagFrame.cpp $
**  $Revision: #13 $
**  $DateTime: 2005/08/26 17:16:33 $
**    $Author: Dwain_Skinner $
**
*/

// Pre-compiled header
#include "TTLPCH.h"

// My header
#include "components/TagFrame.h"

// GameLib headers
#include "Components/ComponentCreator.h"
#include "Effects/decal/staticdecal.h"

// TTL headers
#include "environ/PlayerSpawnMgr.h"
#include "sim/TeamingModel.h"

// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(CTagFrame, TagFrame, COMPONENT_NEUTRAL_PRIORITY);

#define INTERACTION_COS_ANGLE     0.7f
#define INTERACTION_BOX_DEPTH     2.0f
#define INTERACTION_WIDTH_BUFFER  0.0f
#define INTERACTION_HEIGHT_BUFFER 1.0f
#define TAG_TIME                  5000
#define COOLDOWN_TIME             500
#define TAG_OFFSET                0.025f

#define ACTIVE_ROLE(r) ((r == CTeamingModel::BLUE) || (r == CTeamingModel::RED))

/******************************************************************************/
/******************************************************************************/
CTagFrame::CTagFrame(CCompActor &i_actor) : CActorComponent(i_actor)
{
	d_assumeSpawnRole = false;
	d_decalRole = CTeamingModel::INVALID_ROLE;

	d_currentTag = NULL;
	d_desiredTag = NULL;
	d_control = AVAILABLE;
	d_holder.MakeInvalid();
	d_controlTime = 0;
	d_role = CTeamingModel::INVALID_ROLE;
}

/******************************************************************************/
/******************************************************************************/
CTagFrame::~CTagFrame()
{
	if (d_currentTag)
	{
		delete d_currentTag;
		d_currentTag = NULL;
	}
	if (d_desiredTag)
	{
		delete d_desiredTag;
		d_desiredTag = NULL;
	}
}

/******************************************************************************/
/******************************************************************************/
CInterface* CTagFrame::GetInterface(uint i_id)
{
	if (i_id == CObjectiveIF::GetID())
		return((CObjectiveIF*)this);
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
void CTagFrame::SetupNetData(void)
{
	actor.RequestNetDataSlot("TagFrame", sizeof(SState));
	if (IsInitialized())
		Push();
}

/******************************************************************************/
/******************************************************************************/
void CTagFrame::LoadNetData(void)
{
	if (IsInitialized())
		Pull();
}

/******************************************************************************/
/******************************************************************************/
void CTagFrame::Initialize(void)
{
	d_simpleInteractionHandler.Initialize("SimpleInteraction",
			MESSAGE_HANDLE_IF_MASTER | MESSAGE_ROUTE_TO_MASTER,
			actor.GetMessageOwner(), this);
	d_stopInteractionHandler.Initialize("StopInteraction",
			MESSAGE_HANDLE_IF_MASTER | MESSAGE_ROUTE_TO_MASTER,
			actor.GetMessageOwner(), this);
	d_netDataReceivedHandler.Initialize("NetDataReceived",
			MESSAGE_HANDLE_IF_DUPLICA, actor.GetMessageOwner(), this);

	d_decalRole = CTeamingModel::INVALID_ROLE;

	actor.SetStateDistributed(true);
	if (actor.IsNetMaster())
	{
		// Set state.
		if (d_assumeSpawnRole)
			d_role = g_PlayerSpawnMgr.FindRoleOfNearestSpawnPoint(
					actor.GetBodyInWorld());
		else
			d_role = CTeamingModel::AMBIENT;
		if (ACTIVE_ROLE(d_role))
			d_control = OWNED;
		else
		{
			d_role = CTeamingModel::AMBIENT;
			d_control = AVAILABLE;
		}
		d_holder.MakeInvalid();
		d_controlTime = g_session.GetSessionTimeMS();
		SyncRole();

		// Distribute.
		Push();
	}
	else
		Pull();
}

/******************************************************************************/
// Only called in sp mode when reseting for checkpoint.
/******************************************************************************/
void CTagFrame::Reset(void)
{
	// Clear out any player tags.
	if (d_decalRole != CTeamingModel::INVALID_ROLE)
	{
		if (d_currentTag)
		{
			delete d_currentTag;
			d_currentTag = NULL;
		}
		if (d_desiredTag)
		{
			delete d_desiredTag;
			d_desiredTag = NULL;
		}

		Vector3 &pos = actor.GetBodyInWorld();
		Vector3 normal = actor.GetBodyToWorld().GetRow2();
		Vector3 up = actor.GetBodyToWorld().GetRow1();
		float width, height;
		COrientedCollisionIF* collisionIF =
			COrientedCollisionIF::GetInterface(&actor);
		if (collisionIF != NULL)
		{
			width = collisionIF->GetXSize();
			height = collisionIF->GetYSize();
		}
		else
		{
			width = 0.1f;
			height = 0.1f;
		}
		d_currentTag = g_tag.CreateTag(CTeamingModel::AMBIENT, pos, normal, up, width, height, 0.001f);
		d_currentTag->GetInstance( )->SetRenderBucket(INSTANCE_RENDER_BUCKET_MAX - 2);

	}

	d_assumeSpawnRole = false;
	d_decalRole = CTeamingModel::INVALID_ROLE;

	d_control = AVAILABLE;
	d_holder.MakeInvalid();
	d_controlTime = 0;
	d_role = CTeamingModel::INVALID_ROLE;
}

/******************************************************************************/
/******************************************************************************/
void CTagFrame::EndUpdate(float i_deltaSec)
{
	if (actor.IsNetMaster() && (d_control == CLAIMED))
	{
		CParticipantIF* partIF = CParticipantIF::GetInterface(d_holder);
		if ((partIF == NULL) || !partIF->GetAlive() || !InPosition(d_holder))
			StopInteraction();
		else if (g_session.GetSessionTimeMS() >= d_controlTime)
			CompleteInteraction();
	}
}

/******************************************************************************/
/******************************************************************************/
CObjectiveIF::EControl CTagFrame::GetControl(void)
{
	return(d_control);
}

/******************************************************************************/
/******************************************************************************/
uint32 CTagFrame::GetTimeOfControlMS(void)
{
	return(d_controlTime);
}

/******************************************************************************/
/******************************************************************************/
uint32 CTagFrame::GetTimeOfStateMS(void)
{
	return(d_controlTime);
}

/******************************************************************************/
/******************************************************************************/
ActorHandle CTagFrame::GetMostRecentHolder(void)
{
	return(d_holder);
}

/******************************************************************************/
/******************************************************************************/
bool CTagFrame::CanInteract(ActorHandle i_handle) const
{
	uint8 id = CParticipantIF::GetParticipantIdFromActor(i_handle);
	return(g_referee.IsParticipant(id) &&
			((d_control == AVAILABLE) || ((d_control == OWNED) &&
			(g_referee.GetTeamingModel().GetPlayerRole(id) != d_role))) &&
			(g_session.GetSessionTimeMS() >= d_controlTime) &&
			InPosition(i_handle));
}

/******************************************************************************/
/******************************************************************************/
bool CTagFrame::InPosition(ActorHandle i_query) const
{
	CActor* query = CActor::FromHandle(i_query);
	ASSERT_PTR(query);

	// Is the actor looking at the tag?
	if (actor.GetBodyToWorld().GetRow2().Dot(query->GetBodyToWorld().GetRow2()) <
			-INTERACTION_COS_ANGLE)
	{
		Vector3 minExtent, maxExtent, tagInWorld;
		DirCos3x3 tagToWorld;
		COrientedCollisionIF* collisionIF =
				COrientedCollisionIF::GetInterface(&actor);
		if (collisionIF != NULL)
		{
			float halfWidth = 0.5f * collisionIF->GetXSize() +
					INTERACTION_WIDTH_BUFFER;
			float halfHeight = 0.5f * collisionIF->GetYSize() +
					INTERACTION_HEIGHT_BUFFER;
			minExtent.Set(-halfWidth, -halfHeight, 0.5f * collisionIF->GetZSize());
			maxExtent.Set(halfWidth, halfHeight,
					minExtent.Z() + INTERACTION_BOX_DEPTH);
			tagInWorld = collisionIF->GetCollisionInWorld();
			tagToWorld = collisionIF->GetCollisionToWorld();
		}
		else
		{
			minExtent.Set(-INTERACTION_WIDTH_BUFFER, -INTERACTION_HEIGHT_BUFFER, 0.0f);
			maxExtent.Set(INTERACTION_WIDTH_BUFFER, INTERACTION_HEIGHT_BUFFER,
					INTERACTION_BOX_DEPTH);
			tagInWorld = actor.GetBodyInWorld();
			tagToWorld = actor.GetBodyToWorld();
		}
		Vector3 queryInTag = (query->GetFocusInWorld() - tagInWorld) /
				tagToWorld;
		return(queryInTag.AllXYZLessOrEqual(maxExtent) &&
				minExtent.AllXYZLessOrEqual(queryInTag));
	}
	return(false);
}

/******************************************************************************/
// Note: Only called on master.
/******************************************************************************/
void CTagFrame::StartInteraction(ActorHandle i_handle)
{
	if (CanInteract(i_handle))
	{
		d_control = CLAIMED;
		d_holder = i_handle;
		d_controlTime = g_session.GetSessionTimeMS() + TAG_TIME;
		SyncRole();
		Push();
		g_messageDispatcher.SendMessageToActor("StartTagging", NULL,
				actor.GetHandle(), actor.GetHandle());
		g_messageDispatcher.SendMessageToActor("StartTagging", NULL,
				actor.GetHandle(), d_holder);
	}
}

/******************************************************************************/
// Note: Only called on master.
/******************************************************************************/
void CTagFrame::StopInteraction(void)
{
	if (d_control == CLAIMED)
	{
		d_control = ACTIVE_ROLE(d_role) ? OWNED : AVAILABLE;
		d_controlTime = g_session.GetSessionTimeMS() + COOLDOWN_TIME;
		SyncRole();
		Push();
		g_messageDispatcher.SendMessageToActor("StopTagging", NULL,
				actor.GetHandle(), actor.GetHandle());
		g_messageDispatcher.SendMessageToActor("StopTagging", NULL,
				actor.GetHandle(), d_holder);
	}
}

/******************************************************************************/
// Note: Only called on master.
/******************************************************************************/
void CTagFrame::CompleteInteraction(void)
{
	if (d_control == CLAIMED)
	{
		// Delete the old one.
		if (d_currentTag)
		{
			delete d_currentTag;
			d_currentTag = NULL;
		}

		// Set our new tag.
		d_currentTag = d_desiredTag;
		if (d_currentTag)
		{
			d_currentTag->ShiftOffset(-TAG_OFFSET);
			d_currentTag->GetInstance( )->SetRenderBucket(INSTANCE_RENDER_BUCKET_MAX - 2);
		}
		d_desiredTag = NULL;

		if (d_holder.Valid())
			d_role = g_referee.GetTeamingModel().GetPlayerRole(
					CParticipantIF::GetParticipantIdFromActor(d_holder));
		else
			d_role = CTeamingModel::AMBIENT;

		if (ACTIVE_ROLE(d_role))
			d_control = OWNED;
		else
		{
			d_role = CTeamingModel::AMBIENT;
			d_control = AVAILABLE;
		}

		d_controlTime = g_session.GetSessionTimeMS();
		SyncRole();
		Push();
		g_messageDispatcher.SendMessageToActor("StopTagging", NULL,
				actor.GetHandle(), actor.GetHandle());
		g_messageDispatcher.SendMessageToActor("StopTagging", NULL,
				actor.GetHandle(), d_holder);
		g_messageDispatcher.SendMessageToAll("RecordTagOwnage",
				(void*)CParticipantIF::GetParticipantIdFromActor(d_holder),
				actor.GetHandle());
		g_messageDispatcher.SendMessageToAll("PlayTagOwnageDialog",
				(void*)CParticipantIF::GetParticipantIdFromActor(d_holder),
				actor.GetHandle());
	}
}

/******************************************************************************/
/******************************************************************************/
void CTagFrame::SyncRole(void)
{
	// Find the desired decal role.
	CTeamingModel::ERole desiredDecalRole;
	if (d_control != CLAIMED)
		desiredDecalRole = d_role;
	else if (d_holder.Valid())
	{
		desiredDecalRole = g_referee.GetTeamingModel().GetPlayerRole(
				CParticipantIF::GetParticipantIdFromActor(d_holder));
		if (!ACTIVE_ROLE(desiredDecalRole))
			desiredDecalRole = CTeamingModel::AMBIENT;
	}
	else
		desiredDecalRole = CTeamingModel::AMBIENT;

	// Sync the decal role.
	if (desiredDecalRole != d_decalRole)
	{
		// Sync decal.
		if (ValidSingleton(ModeTag))
		{
			// Compute the transition time.
			int deltaTime = d_controlTime - g_session.GetSessionTimeMS();
			if (deltaTime < 1)
				deltaTime = 1;
			float deltaSec = 0.001f * (float)deltaTime;

			Vector3 &pos = actor.GetBodyInWorld();
			Vector3 normal = actor.GetBodyToWorld().GetRow2();
			Vector3 up = actor.GetBodyToWorld().GetRow1();
			float width, height;
			COrientedCollisionIF* collisionIF =
					COrientedCollisionIF::GetInterface(&actor);
			if (collisionIF != NULL)
			{
				width = collisionIF->GetXSize();
				height = collisionIF->GetYSize();
			}
			else
			{
				width = 0.1f;
				height = 0.1f;
			}

			if (d_desiredTag)
			{
				delete d_desiredTag;
				d_desiredTag = NULL;
			}

			// Create our starting or desired tag
			if (d_control == CLAIMED || !d_currentTag)
			{
				if (desiredDecalRole == CTeamingModel::BLUE)
					d_desiredTag = g_tag.CreateTag(CTeamingModel::BLUE, pos, normal, up, width, height, TAG_TIME / 1000.0f);
				else if (desiredDecalRole == CTeamingModel::RED)
					d_desiredTag = g_tag.CreateTag(CTeamingModel::RED, pos, normal, up, width, height, TAG_TIME / 1000.0f);
				else
					d_desiredTag = g_tag.CreateTag(CTeamingModel::AMBIENT, pos, normal, up, width, height, TAG_TIME / 1000.0f);
				d_desiredTag->ShiftOffset(TAG_OFFSET);
				d_desiredTag->GetInstance( )->SetRenderBucket(INSTANCE_RENDER_BUCKET_MAX - 1);
			}

			// Set our starting tag.
			if (d_currentTag == NULL)
			{
				d_currentTag = d_desiredTag;
				d_currentTag->ShiftOffset(-TAG_OFFSET);
				d_desiredTag = NULL;
				d_currentTag->GetInstance( )->SetRenderBucket(INSTANCE_RENDER_BUCKET_MAX - 2);
			}
			d_decalRole = desiredDecalRole;
		}
	}

 	// Sync spawn point role.
	if (d_assumeSpawnRole && (d_control != CLAIMED))
		g_PlayerSpawnMgr.ControlSpawn(actor.GetBodyInWorld(), d_role);
}

/******************************************************************************/
/******************************************************************************/
void CTagFrame::Push(void)
{
	SState* state = (SState*)actor.OpenNetDataSlot("TagFrame");
	if (state != NULL)
	{
		state->d_control = (uint8)d_control;
		state->d_holder = g_session.ToSessionID(d_holder);
		state->d_controlTime = d_controlTime;
		state->d_role = (uint8)d_role;
		actor.SendNetData();
	}
}

/******************************************************************************/
// Note: Only called on duplica.
/******************************************************************************/
void CTagFrame::Pull(void)
{
	SState* state = (SState*)actor.ReadNetDataSlot("TagFrame");
	if (state != NULL)
	{
		d_holder = g_session.FromSessionIDToHandle(state->d_holder);
		if (d_control != (EControl)state->d_control)
		{
			if ((EControl)state->d_control == CLAIMED)
			{
				g_messageDispatcher.SendMessageToActor("StartTagging", NULL,
						actor.GetHandle(), actor.GetHandle());
				g_messageDispatcher.SendMessageToActor("StartTagging", NULL,
						actor.GetHandle(), d_holder);
			}
			else if (d_control == CLAIMED)
			{
				g_messageDispatcher.SendMessageToActor("StopTagging", NULL,
						actor.GetHandle(), actor.GetHandle());
				g_messageDispatcher.SendMessageToActor("StopTagging", NULL,
						actor.GetHandle(), d_holder);
			}
			d_control = (EControl)state->d_control;
		}
		d_controlTime = state->d_controlTime;
		// Handle role change.
		if (d_role != (CTeamingModel::ERole)state->d_role)
		{
			d_role = (CTeamingModel::ERole)state->d_role;
			// Avatar completed interaction.
			if (d_currentTag && d_desiredTag)
			{
				delete d_currentTag;
				d_currentTag = d_desiredTag;
				d_currentTag->ShiftOffset(-TAG_OFFSET);
				d_desiredTag = NULL;
				d_currentTag->GetInstance( )->SetRenderBucket(INSTANCE_RENDER_BUCKET_MAX - 2);
			}
		}
		SyncRole();
	}
}

/******************************************************************************/
/******************************************************************************/
void CTagFrame::SimpleInteraction::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	d_ref->StartInteraction(i_sender);
}

/******************************************************************************/
/******************************************************************************/
void CTagFrame::StopInteraction::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	d_ref->StopInteraction();
}

/******************************************************************************/
/******************************************************************************/
void CTagFrame::NetDataReceived::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	d_ref->Pull();
}
