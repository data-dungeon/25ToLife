/*
**
**  File:   Component.h
**  Date:   January 6, 2004
**  By:     Bryant Collard
**  Desc:   The behavior base class.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
*/

#ifndef COMPONENT_H
#define COMPONENT_H

#include "stage/protagonist.h"

class CCompActor;
class CStageEntity;
class CCollisionEvent;
class CInterface;
class CNetActor;

class CActorComponent
{
  public:
	// Construct, etc.
	CActorComponent(CCompActor &i_actor) : actor( i_actor ) { active = false; initialized = false; suspended = false; priority = 0; }
	virtual ~CActorComponent() {}
	CCompActor &GetActor(void) {return(actor);}

	// must override these
	virtual const char *ComponentName( void ) = 0;
	virtual const char *ComponentClass( void ) { return ""; }

	// called after the component is attached.  IT IS NOT INITIALIZED AT THIS POINT.
	virtual void Attach( void ) {}

	// if you override these, make sure that you either modify the active state
	// yourself, or call back to CActorComponent::De/Activate/IsActive, so that they
	// can handle tracking whether the component is active or not.
	virtual void Activate( void ) { active = true; suspended = false; }
	virtual void Deactivate( void ) { active = false; }
	virtual bool IsActive( void ) { return (active && !suspended); }

	virtual bool DefaultActive( void ) { return true; }
	
	// return true if you should be the "default" component in a class
	virtual bool DefaultInClass( void ) { return false; }

	virtual void Initialize( void ) {};
	// call this when the component is initialized.  done this way instead of in
	// Initialize() so that each individual component doesn't have to remember to
	// set initialized in their init function, and because actually calling Initialize
	// is centralized in CCompActor
	virtual void SetInitialized( void ) { initialized = true; }
	virtual bool IsInitialized( void ) { return initialized; }
	virtual void Reset( void ) {};

	// Continuous state changes
	virtual void BeginFrame(void) {}
	virtual void BeginUpdate(void) {}
	virtual void AttemptUpdate(float i_deltaSec) {}
	virtual void EndUpdate(float i_deltaSec) {}
	virtual void EndFrame(void) {}
	virtual void RunBones(void) {}
	virtual void RunAnimation(void) {}

	// Collision handling
	virtual bool CoupledCollisionDetect(CStageEntity* i_entity,
			CCollisionEvent* i_event, uint &o_myEffect, uint &o_otherEffect) 
		{return(false);}
	virtual uint CollisionDetect(CStageEntity* i_entity, CCollisionEvent* i_event, bool i_mateDisplaced)
		{return(CProtagonist::EFFECT_NONE);}
	virtual bool CoupledCollisionAccept(CStageEntity* i_entity, CCollisionEvent* i_event)
		{return(false);}
	virtual void CollisionAccept(CStageEntity* i_entity, CCollisionEvent* i_event) 
		{}
	virtual uint GetTailoredInteractionHint(CProtagonist* i_actor)
		{return(((CProtagonist*)&actor)->GetInteractionHint());}

	virtual void SyncActor( void ) {}

	// Get at interfaces
	virtual CInterface* GetInterface(uint i_id) {return(NULL);}

	// Allow components to pick a net actor
	virtual CNetActor* CreateNetActor(void) {return(NULL);}

	// Master data associated with component
	virtual uint16 GetMasterMetaDataSize(void) {return(0);}
	virtual bool HasMasterMetaDataChangedSinceLastWrite(void) {return(false);}
	virtual void WriteMasterMetaData(void* &io_buffer) {}
	virtual uint16 GetMasterDataSize(void) {return(0);}
	virtual bool HasMasterDataChangedSinceLastWrite(void)
		{return(GetMasterDataSize() > 0);}
	virtual void WriteMasterData(void* &io_metaDataBuffer, void* &io_dataBuffer) {}
	virtual void ReadMasterData(void* &io_metaDataBuffer, void* &io_dataBuffer) {}

	virtual void SetupNetData(void) {}
	virtual void LoadNetData(void) {}

	virtual void SetPriority( int i_priority ) { priority = i_priority; }
	virtual int GetPriority( void ) { return priority; }

	virtual void Suspend( void ) { if( IsActive() ){ suspended = true; active = false; } }
	virtual void Resume( void ) { suspended = false; active = true; }
	virtual bool Suspended( void ) { return suspended; }

protected:
	CCompActor &actor;

	int priority;
	bool active;
	bool initialized;
	bool suspended;
};

#endif // COMPONENT_H
