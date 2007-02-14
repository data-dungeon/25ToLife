//============================================================================
// about pickups...
// The pickup component handles only the generic functions of pickups.
// The script will determine if and when an actor picks it up.
// All gameplay should be in the script.
// Things like particle effects and sounds should be specified from the OBJATTR.CFG.
//
// Brian
//============================================================================
#ifndef PICKUP_H
#define PICKUP_H

#include "Components/Component.h"
#include "InterfacesTTL/ObjectiveIF.h"

class Pickup : public CActorComponent, public CObjectiveIF
{
  public:
	// Construct
	Pickup(CCompActor &i_actor);

	// CActorComponent overrides
	virtual const char *ComponentName() {return("Pickup");}
	virtual CInterface* GetInterface(uint i_id);
	virtual void SetupNetData(void);
	virtual void LoadNetData(void);
	virtual void Initialize();
	virtual void EndUpdate(float i_deltaSec);
	virtual void CollisionAccept(CStageEntity* i_entity, CCollisionEvent* i_event);

	// CObjectiveIF overrides
	virtual EResidence GetResidence(void);
	virtual EControl GetControl(void);
	virtual uint8 GetState(void);
	virtual uint32 GetTimeOfResidenceMS(void);
	virtual uint32 GetTimeOfControlMS(void);
	virtual uint32 GetTimeOfStateMS(void);
	virtual ActorHandle GetMostRecentHolder(void);

	void SetDropOnDeath(bool i_flag) {d_dropOnDeath = i_flag;}
	void SetDropOnButton(bool i_flag) {d_dropOnButton = i_flag;}
	void SetAttachableToActor(bool i_flag) {d_attachableToActor = i_flag;}
	void SetParticleEmitter(const char* i_name, Vector3CRef i_offset);

	virtual uint16 GetMasterDataSize(void);
	virtual bool HasMasterDataChangedSinceLastWrite(void);
	virtual void WriteMasterData(void* &io_metaDataBuffer, void* &io_dataBuffer);
	virtual void ReadMasterData(void* &io_metaDataBuffer, void* &io_dataBuffer);

  private:
	struct SState
	{
		uint8 d_state;
		uint32 d_holder;
		int32 d_residenceTime;
		int32 d_controlTime;
		Vector3Packed d_position;
		float d_heading;
	};

	bool ChangeState(uint8 i_state);
	void Acquire(ActorHandle i_holder);
	bool Release(bool i_place);
	void GoHome(ActorHandle i_sender);
	void Drop(void);
	void Hold(ActorHandle i_holder);
	void Capture(ActorHandle i_holder);
	void PlaceOnGround(CActor* i_dest);
	void StartEmitter(void);
	void StopEmitter(void);
	void Push(void);
	void Pull(void);

	// Configuration
	bool d_attachableToActor;
	bool d_dropOnDeath;
	bool d_dropOnButton;
	char d_emitterName[32];
	Vector3 d_emitterOffset;

	// Presence
	t_Handle d_emitterHandle;
	Vector3 d_emitterPosition;
	bool d_attached;

	// Home
	Vector3 d_homePosition;
	float d_homeHeading;
	bool d_homeChanged;

	// State
	uint8 d_state;
	ActorHandle d_holder;
	int32 d_residenceTime;
	int32 d_controlTime;

	REF_SEND_MESSAGE_HANDLER(PickupGoHome, Pickup) d_pickupGoHomeHandler;
	REF_SEND_MESSAGE_HANDLER(PickupDrop, Pickup) d_pickupDropHandler;
	REF_BASIC_MESSAGE_HANDLER(PickupHold, MESSAGE_ACTOR_HANDLE, Pickup)
		d_pickupHoldHandler;
	REF_SEND_MESSAGE_HANDLER(PickupCapture, Pickup) d_pickupCaptureHandler;
	REF_SEND_MESSAGE_HANDLER(NetDataReceived, Pickup) d_netDataReceivedHandler;
};

#endif // PICKUP_H
