/*
**
**  File:   BipedModel.h
**  Date:   February 6, 2004
**  By:     Bryant Collard
**  Desc:   Manages a biped model.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/components/KineControl/BipedModel.h $
**  $Revision: #36 $
**  $DateTime: 2005/07/19 14:03:24 $
**    $Author: Travis_Nixon $
**
*/

#pragma once

#ifndef BIPED_MODEL_H
#define BIPED_MODEL_H

#include "Components/Component.h"
#include "components/KineControl/BipedAnimScriptIF.h"
#include "Components/InteractionMonitor.h"

class Weapon;

class BipedModel : public CActorComponent, public CBipedAnimScriptIF, public InteractionIF
{
  public:
	BipedModel(CCompActor &i_actor);

	virtual const char* ComponentName(void) {return("BipedModel");}

	virtual void Initialize(void);
	virtual bool DefaultActive(void) {return(false);}
	virtual void Activate(void);
	virtual void Reset(void);
	virtual void SyncActor(void);
	virtual void BeginFrame( void );
	virtual void AttemptUpdate(float i_deltaSec);
	virtual void EndUpdate(float i_deltaSec);
	virtual void EndFrame(void);
	virtual void RunBones(void);
	virtual void RunAnimation(void);

	CInterface *GetInterface( uint i_id );

	InteractionMonitor::InteractionType QueryInteractionType( CCompActor *whoWantsToKnow );

	// Weapons
	void DeselectWeapon(Weapon* i_weapon);
	void SelectWeapon(Weapon* i_weapon);

	// Self terminating modes.
	bool Jumping(void);
	bool Kicking(void);

	// Commanded modes.
	bool Aiming(void) {return(d_mode[MODE_AIM].d_active || d_mode[MODE_AIMSNAP].d_active);}
	bool Crouching(void) {return(d_mode[MODE_CROUCH].d_active);}
	bool Cowering(void) {return(d_mode[MODE_COWER].d_active);}
	bool HandsUp(void) {return(d_mode[MODE_HANDS_UP].d_active);}
	bool Dead(void) {return(d_mode[MODE_DIE].d_active);}
	bool OnLadder(void) {return(d_mode[MODE_LADDER].d_active);}
	bool LadderSlide(void) {return(d_mode[MODE_LADDERSLIDE].d_active);}
	bool Tagging(void) {return(d_mode[MODE_TAG].d_active);}

	// Manage commanded modes.
	void SetAimTime(float i_time = -1.0f)
		{CommandSetTime(MODE_AIM, i_time); CommandSetTime(MODE_AIMSNAP, i_time);}
	void StartAim(float i_time = -1.0f)
		{CommandStart(MODE_AIM, i_time);}
	void StopAim(bool i_allowTimeout = false)
		{CommandStop(MODE_AIM, i_allowTimeout);}
	void StartAimSnap(float i_time = -1.0f)
		{CommandStart(MODE_AIMSNAP, i_time);}
	void StopAimSnap(bool i_allowTimeout = false)
		{CommandStop(MODE_AIMSNAP, i_allowTimeout);}
	void StartCrouch(float i_time = -1.0f)
		{CommandStart(MODE_CROUCH, i_time);}
	void StopCrouch(bool i_allowTimeout = false)
		{CommandStop(MODE_CROUCH, i_allowTimeout);}
	void StartCower(float i_time = -1.0f)
		{CommandStart(MODE_COWER, i_time);}
	void StopCower(bool i_allowTimeout = false)
		{CommandStop(MODE_COWER, i_allowTimeout);}
	void StartHandsUp(float i_time = -1.0f)
		{CommandStart(MODE_HANDS_UP, i_time);}
	void StopHandsUp(bool i_allowTimeout = false)
		{CommandStop(MODE_HANDS_UP, i_allowTimeout);}
	void StartDie(float i_time = -1.0f)
		{CommandStart(MODE_DIE, i_time);}
	void StopDie(bool i_allowTimeout = false)
		{CommandStop(MODE_DIE, i_allowTimeout);}
	void StartLadder(void)
		{CommandStart(MODE_LADDER, -1.0f);}
	void StopLadder(void)
		{CommandStop(MODE_LADDER, false);}
	void StartLadderSlide(void)
		{CommandStart(MODE_LADDERSLIDE, -1.0f);}
	void StopLadderSlide(void)
		{CommandStop(MODE_LADDERSLIDE, false);}
	void StartTagging(float i_time = -1.0f)
		{CommandStart(MODE_TAG, i_time);}
	void StopTagging(bool i_allowTimeout = false)
		{CommandStop(MODE_TAG, i_allowTimeout);}

	// Anim script interface
	virtual bool ScriptLadderSlide( void ) {return(LadderSlide());}
	virtual bool ScriptOnLadder( void ) {return(OnLadder());}
	virtual bool ScriptOnGround(void);
	virtual void ScriptFootprint(int foot);
	virtual bool ScriptLaunch(Vector3CRef i_vel);
	virtual bool ScriptAiming(void) {return(Aiming());}
	virtual bool ScriptCrouching(void) {return(Crouching());}
	virtual bool ScriptJumping(void) {return(Jumping());}
	virtual int ScriptCurrentWeapon(void);
	virtual int ScriptCurrentWeaponReloadType(void);
	virtual void ScriptKick(void);
	virtual void ScriptStartAim( void ) {StartAim();}
	virtual void ScriptStopAim( void ) {StopAim();}
	virtual void ScriptStartCrouch( void ) {StartCrouch();}
	virtual void ScriptStopCrouch( void ) {StopCrouch();}
	virtual void ScriptStartTagging( void ) {StartTagging();}
	virtual void ScriptStopTagging( void ) {StopTagging();}

  private:
	void UpdateAnimationState(void);
	void UpdateSecondaryAnimationState(void);

	enum EMode
	{
		MODE_AIM,
		MODE_AIMSNAP,
		MODE_CROUCH,
		MODE_COWER,
		MODE_HANDS_UP,
		MODE_DIE,
		MODE_LADDER,
		MODE_LADDERSLIDE,
		MODE_TAG,
		NUM_MODES
	};

	enum EOverride
	{
		OVERRIDE_NONE,
		OVERRIDE_ACTIVE,
		OVERRIDE_INACTIVE
	};

	struct
	{
		bool d_active;
		bool d_desired;
		bool d_commanded;
		float d_time;
	} d_mode[NUM_MODES];

	void CommandSetTime(EMode i_mode, float i_time);
	void CommandStart(EMode i_mode, float i_time);
	void CommandStop(EMode i_mode, bool i_allowTimeout);
	EOverride OverrideExternal(EMode i_mode);
	EOverride OverrideInternal(EMode i_mode);
	void StartMode(EMode i_mode);
	void StopMode(EMode i_mode);

	bool d_onlyForwardMotions;

	Weapon* d_weapon;

	ts_Bone* rootBone;
	ts_Bone* hipsBone;
	ts_Bone* headBone;

	Vector3 ladderStartPosition;
	float cullRadius;

	// message handling
	REF_BASIC_MESSAGE_HANDLER(WeaponSelectedHandler, MESSAGE_VALUE, BipedModel)
		d_weaponSelectedHandler;
	REF_BASIC_MESSAGE_HANDLER(WeaponDeselectedHandler, MESSAGE_VALUE, BipedModel)
		d_weaponDeselectedHandler;
	REF_SEND_MESSAGE_HANDLER(DestructHandler, BipedModel) d_destructHandler;
	REF_SEND_MESSAGE_HANDLER(DeathHandler, BipedModel) d_deathHandler;
	REF_SEND_MESSAGE_HANDLER(HostageHandler, BipedModel) d_hostageHandler;
	REF_BASIC_MESSAGE_HANDLER(HostageTakerHandler, MESSAGE_ACTOR_HANDLE, BipedModel) d_hostageTakerHandler;
};

#endif // BIPED_MODEL_H
