#include "TTLPCH.h"
#include "components/Pickup.h"
#include "Components/ComponentCreator.h"
#include "InterfacesTTL/ParticipantIF.h"
#include "GameHelper/SharedBody.h"
#include "EngineHelper/PackBuffer.h"

// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(Pickup, Pickup, COMPONENT_NEUTRAL_PRIORITY);

// How much does the pickup affect movement
#define PICKUP_INCUMBERANCE 0.15f

// Possible states
#define AT_HOME ((uint8)CObjectiveIF::HOME    | (uint8)CObjectiveIF::AVAILABLE)
#define CARRIED ((uint8)CObjectiveIF::TRANSIT | (uint8)CObjectiveIF::CLAIMED)
#define LOST    ((uint8)CObjectiveIF::DROPPED | (uint8)CObjectiveIF::AVAILABLE)
#define CAPTURE ((uint8)CObjectiveIF::DROPPED | (uint8)CObjectiveIF::OWNED)

//============================================================================
Pickup::Pickup( CCompActor &i_actor ) :
CActorComponent( i_actor )
{
	// Configuration
	d_attachableToActor = false;
	d_dropOnDeath = false;
	d_dropOnButton = false;
	d_emitterName[0] = '\0';
	d_emitterOffset.Clear();

	// Presence
	d_emitterHandle = INVALID_HANDLE;
	d_emitterPosition.Clear();
	d_attached = false;

	// Home
	d_homePosition.Clear();
	d_homeHeading = 0.0f;
	d_homeChanged = false;

	// State
	d_state = AT_HOME;
	d_holder.MakeInvalid();
	d_residenceTime = 0;
	d_controlTime = 0;
}

//============================================================================
CInterface* Pickup::GetInterface(uint i_id)
{
	if (i_id == CObjectiveIF::GetID())
		return((CObjectiveIF*)this);
	return(NULL);
}

//============================================================================
void Pickup::SetupNetData(void)
{
	actor.RequestNetDataSlot("Pickup", sizeof(SState));
	if (IsInitialized())
		Push();
}

//============================================================================
void Pickup::LoadNetData(void)
{
	if (IsInitialized())
		Pull();
}

//============================================================================
void Pickup::Initialize()
{
	// Handle messages from associated script. The script will run on all
	// stations but the message handlers make sure only the master will react.
	d_pickupGoHomeHandler.Initialize("PickupGoHome", MESSAGE_HANDLE_IF_MASTER,
			actor.GetMessageOwner(), this);
	d_pickupDropHandler.Initialize("PickupDrop", MESSAGE_HANDLE_IF_MASTER,
			actor.GetMessageOwner(), this);
	d_pickupHoldHandler.Initialize("PickupHold", MESSAGE_HANDLE_IF_MASTER,
			actor.GetMessageOwner(), this);
	d_pickupCaptureHandler.Initialize("PickupCapture", MESSAGE_HANDLE_IF_MASTER,
			actor.GetMessageOwner(), this);
	d_netDataReceivedHandler.Initialize("NetDataReceived",
			MESSAGE_HANDLE_IF_DUPLICA, actor.GetMessageOwner(), this);

	actor.SetStateDistributed(true);
	if (actor.IsNetMaster())
	{
		// Establish home.
		d_homePosition = actor.GetBodyInWorld();
		d_homeHeading = Math::ArcTan2(actor.GetBodyToWorld().e20,
				actor.GetBodyToWorld().e22);
		d_homeChanged = true;

		// Set state.
		d_state = AT_HOME;
		d_holder.MakeInvalid();
		d_residenceTime = g_session.GetSessionTimeMS();
		d_controlTime = d_residenceTime;

		// Set presence.
		actor.Hide(false);
		actor.EnableInteraction(true);
		actor.Place(d_homePosition, d_homeHeading);
		StartEmitter();
		d_attached = false;

		// Distribute.
		Push();
	}
	else
		Pull();
}

//============================================================================
void Pickup::EndUpdate(float i_deltaSec)
{
	if ((d_state == CARRIED) && actor.IsNetMaster())
	{
		CActor* holdingActor = CActor::FromHandle(d_holder);
		if (!holdingActor)
			GoHome(INVALID_OBJECT_HANDLE);
		else if (d_dropOnDeath)
		{
			CParticipantIF* partIF = CParticipantIF::GetInterface(d_holder);
			if ((partIF != NULL) && !partIF->GetAlive())
				Drop();
		}
	}
}

//============================================================================
void Pickup::CollisionAccept(CStageEntity* i_entity, CCollisionEvent* i_event)
{
	if (i_entity->IsA(CActor::ClassType()))
		g_messageDispatcher.SendMessageToActor("PickupCollidedWithActor", NULL,
				CActor::ToHandle((CActor*)i_entity), actor.GetHandle());
}

//============================================================================
CObjectiveIF::EResidence Pickup::GetResidence(void)
{
	return((EResidence)(d_state & RESIDENCE_MASK));
}

//============================================================================
CObjectiveIF::EControl Pickup::GetControl(void)
{
	return((EControl)(d_state & CONTROL_MASK));
}

//============================================================================
uint8 Pickup::GetState(void)
{
	return(d_state);
}

//============================================================================
uint32 Pickup::GetTimeOfResidenceMS(void)
{
	return(d_residenceTime);
}

//============================================================================
uint32 Pickup::GetTimeOfControlMS(void)
{
	return(d_controlTime);
}

//============================================================================
uint32 Pickup::GetTimeOfStateMS(void)
{
	return(Math::Max(d_residenceTime, d_controlTime));
}

//============================================================================
ActorHandle Pickup::GetMostRecentHolder(void)
{
	return(d_holder);
}

//============================================================================
bool Pickup::ChangeState(uint8 i_state)
{
	if (i_state != d_state)
	{
		uint32 transitionTime = g_session.GetSessionTimeMS();
		if ((i_state & RESIDENCE_MASK) != (d_state & RESIDENCE_MASK))
			d_residenceTime = transitionTime;
		if ((i_state & CONTROL_MASK) != (d_state & CONTROL_MASK))
			d_controlTime = transitionTime;
		d_state = i_state;
		return(true);
	}
	return(false);
}

//============================================================================
void Pickup::Acquire(ActorHandle i_holder)
{
	CCompActor* holdingActor = CCompActor::FromHandle(i_holder);
	if (holdingActor && (!d_attached || (i_holder != d_holder)))
	{
		actor.Hide(true);
		actor.EnableInteraction(false);

		if (d_attachableToActor)
		{
			if (d_attached)
			{
				CCompActor* lastActor = CCompActor::FromHandle(d_holder);
				if (lastActor)
				{
					SharedBody* body =
							(SharedBody*)lastActor->Instance()->DynamicModel();
					if (body)
						body->RemoveSOM(body->FindSOMIndexByName("loot"));

					Property *prop = lastActor->QueryProperty("incumberance");
					if (prop)
						prop->Set(0.0f);
				}
			}

			// Hardcode money bag geometry
			static const char* geomName = "avatars\\pieces\\p56.dbl";
			ts_Geometry* geom = g_loadSOM.Geometry(geomName);
			ASSERT(geom && geom->pSOM);
			if (geom && geom->pSOM)
			{
				SharedBody* body =
						(SharedBody*)holdingActor->Instance()->DynamicModel();
				if (body)
					body->AddSOM(geom->pSOM, INVALID_HANDLE, "loot");
			}

			Property *prop = holdingActor->QueryProperty("incumberance");
			if (prop)
				prop->Set(PICKUP_INCUMBERANCE);
		}
		d_attached = true;
	}
}

//============================================================================
bool Pickup::Release(bool i_place)
{
	if (d_attached)
	{
		d_attached = false;

		actor.Hide(false);
		actor.EnableInteraction(true);

		CCompActor* holdingActor = CCompActor::FromHandle(d_holder);
		if (holdingActor != NULL)
		{
			if (d_attachableToActor)
			{
				SharedBody* body =
						(SharedBody*)holdingActor->Instance()->DynamicModel();
				if (body)
					body->RemoveSOM(body->FindSOMIndexByName("loot"));

				Property *prop = holdingActor->QueryProperty("incumberance");
				if (prop)
					prop->Set(0.0f);
			}
			if (i_place)
			{
				PlaceOnGround(holdingActor);
				return(true);
			}
		}
	}
	return(false);
}

//============================================================================
void Pickup::GoHome(ActorHandle i_sender)
{
	if (ChangeState(AT_HOME))
	{
		Release(false);
		d_holder = i_sender;
		actor.Place(d_homePosition, d_homeHeading);
		StopEmitter();
		StartEmitter();
		if (d_holder.Valid())
			g_messageDispatcher.SendMessageToAll("ObjectiveReturned", d_holder,
					actor.GetHandle());
		Push();
		// We only care about the holder for initial reporting purposes.
		d_holder = INVALID_OBJECT_HANDLE;
	}
}

//============================================================================
void Pickup::Drop(void)
{
	if (Release(true))
	{
		ChangeState(LOST);
		StartEmitter();
		g_messageDispatcher.SendMessageToAll("ObjectiveDropped", d_holder,
				actor.GetHandle());
		Push();
	}
	else
		GoHome(INVALID_OBJECT_HANDLE);
}

//============================================================================
void Pickup::Hold(ActorHandle i_holder)
{
	Acquire(i_holder);
	if (d_attached && ChangeState(CARRIED))
	{
		d_holder = i_holder;
		StopEmitter();
		g_messageDispatcher.SendMessageToAll("ObjectiveTaken", d_holder,
				actor.GetHandle());
		Push();
	}
}

//============================================================================
void Pickup::Capture(ActorHandle i_dest)
{
	if (ChangeState(CAPTURE))
	{
		CActor* dest = CActor::FromHandle(i_dest);
		if (dest == NULL)
		{
			if (d_attached && !Release(true))
				actor.Place(d_homePosition, d_homeHeading);
		}
		else
		{
			Release(false);
			PlaceOnGround(dest);
		}
		StopEmitter();
		if (d_holder.Valid())
			g_messageDispatcher.SendMessageToAll("ObjectiveCaptured", d_holder,
					actor.GetHandle());
		Push();
	}
}

//============================================================================
void Pickup::PlaceOnGround(CActor* i_dest)
{
	Vector3 dir = i_dest->GetBodyToWorld().GetRow2();
	Vector3 origin = i_dest->GetBodyInWorld();
	origin.Y(origin.Y() + 0.1f);
	CLineEntity line;
	line.Line().SetOriginBasis(origin, Vector3(0.0f, -10.0f, 0.0f));
	line.Line().SetDetectionAbility(CLineCollisionObject::DETECT_FRONT_SIDE);
	line.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
	line.SetPositiveInclude(WORLD_CATEGORY);
	CLineQuery query;
	g_collisionSystem.ResetStack();
	if (query.Test(line))
		actor.Place(query.intersection, Math::ArcTan2(dir.Z(), dir.X()));
	else
		actor.Place(i_dest->GetBodyInWorld(), Math::ArcTan2(dir.Z(), dir.X()));
}

//============================================================================
void Pickup::Push(void)
{
	SState* state = (SState*)actor.OpenNetDataSlot("Pickup");
	if (state != NULL)
	{
		state->d_state = d_state;
		state->d_holder = g_session.ToSessionID(d_holder);
		state->d_residenceTime = d_residenceTime;
		state->d_controlTime = d_controlTime;
		state->d_position = actor.GetBodyInWorld();
		state->d_heading = Math::ArcTan2(actor.GetBodyToWorld().e20,
				actor.GetBodyToWorld().e22);
		actor.SendNetData();
	}
}

//============================================================================
void Pickup::Pull(void)
{
	SState* state = (SState*)actor.ReadNetDataSlot("Pickup");
	if (state != NULL)
	{
		ActorHandle holder = g_session.FromSessionIDToHandle(state->d_holder);
		if (state->d_state == CARRIED)
		{
			if (!holder.Valid())
				GoHome(INVALID_OBJECT_HANDLE);
			else
			{
				Hold(holder);
				d_residenceTime = state->d_residenceTime;
				d_controlTime = state->d_controlTime;
			}
		}
		else
		{
			Release(false);
			actor.Place(state->d_position, state->d_heading);
			d_holder = holder;
			d_residenceTime = state->d_residenceTime;
			d_controlTime = state->d_controlTime;
			if (state->d_state != d_state)
			{
				d_state = state->d_state;
				if (d_state == LOST)
				{
					StartEmitter();
					if (d_holder.Valid())
						g_messageDispatcher.SendMessageToAll("ObjectiveDropped",
								d_holder, actor.GetHandle());
				}
				else if (d_state == CAPTURE)
				{
					if (d_holder.Valid())
					{
						StopEmitter();
						g_messageDispatcher.SendMessageToAll("ObjectiveCaptured",
								d_holder, actor.GetHandle());
					}
					else
					{
						d_state = LOST;
						StartEmitter();
					}
				}
				else
				{
					StopEmitter();
					StartEmitter();
					if (d_holder.Valid())
						g_messageDispatcher.SendMessageToAll("ObjectiveReturned",
								d_holder, actor.GetHandle());
					// We only care about the holder for initial reporting purposes.
					d_holder = INVALID_OBJECT_HANDLE;
				}
			}
		}
	}
}

//============================================================================
void Pickup::PickupGoHome::HandleMessage(void* i_data, ActorHandle i_sender,
		bool i_posted)
{
	d_ref->GoHome((ActorHandle)i_data);
}

//============================================================================
void Pickup::PickupDrop::HandleMessage(void* i_data, ActorHandle i_sender,
		bool i_posted)
{
	if (d_ref->d_state == CARRIED)
		d_ref->Drop();
}

//============================================================================
void Pickup::PickupHold::HandleMessage(void* i_data, ActorHandle i_sender,
		bool i_posted)
{
	if (d_ref->d_state != CAPTURE)
		d_ref->Hold((ActorHandle)i_data);
}

//============================================================================
void Pickup::PickupCapture::HandleMessage(void* i_data, ActorHandle i_sender,
		bool i_posted)
{
	d_ref->Capture((ActorHandle)i_data);
}

//============================================================================
void Pickup::NetDataReceived::HandleMessage(void* i_data, ActorHandle i_sender,
		bool i_posted)
{
	d_ref->Pull();
}

//============================================================================
void Pickup::SetParticleEmitter(const char* i_name, Vector3CRef i_offset)
{
	if (i_name != NULL)
	{
		ASSERT(strlen(i_name) < sizeof(d_emitterName));
		strncpy(d_emitterName, i_name, sizeof(d_emitterName));
		d_emitterName[sizeof(d_emitterName) - 1] = '\0';
	}
	else
		d_emitterName[0] = '\0';

	d_emitterOffset = i_offset;

	if (IsInitialized() && !d_holder.Valid())
	{
		StopEmitter();
		StartEmitter();
	}
}

//============================================================================
void Pickup::StartEmitter(void)
{
	if (d_emitterName[0] != '\0')
	{
		d_emitterPosition = actor.GetBodyInWorld() + d_emitterOffset;
		d_emitterHandle = ParticleEmitter__CreateFromRecord(d_emitterName,
				&d_emitterPosition, NULL, NULL, false, NULL, true);
	}
}

//============================================================================
void Pickup::StopEmitter(void)
{
	if (d_emitterHandle != INVALID_HANDLE)
	{
		ParticleEmitter__Kill(d_emitterHandle);
		d_emitterHandle = INVALID_HANDLE;
	}
}

//============================================================================
struct SPickupMasterData
{
	Vector3Packed d_homePosition;
	float d_homeHeading;
};

//============================================================================
uint16 Pickup::GetMasterDataSize(void)
{
	return(sizeof(SPickupMasterData));
}

//============================================================================
bool Pickup::HasMasterDataChangedSinceLastWrite(void)
{
	return(d_homeChanged);
}

//============================================================================
void Pickup::WriteMasterData(void* &io_metaDataBuffer, void* &io_dataBuffer)
{
	SPickupMasterData masterData;
	masterData.d_homePosition = d_homePosition;
	masterData.d_homeHeading = d_homeHeading;
	io_dataBuffer = PackBuffer::Pack(io_dataBuffer, masterData);
	d_homeChanged = false;
}

//============================================================================
void Pickup::ReadMasterData(void* &io_metaDataBuffer, void* &io_dataBuffer)
{
	SPickupMasterData masterData;
	io_dataBuffer = PackBuffer::Unpack(io_dataBuffer, masterData);
	d_homePosition = masterData.d_homePosition;
	d_homeHeading = masterData.d_homeHeading;
}
//============================================================================
