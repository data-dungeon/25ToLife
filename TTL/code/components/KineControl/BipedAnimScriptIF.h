/*
**
**  File:   BipedAnimScriptIF.h
**  Date:   January 15, 2004
**  By:     Bryant Collard
**  Desc:   Provides the interface for the biped animation script.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/components/KineControl/BipedAnimScriptIF.h $
**  $Revision: #11 $
**  $DateTime: 2005/06/18 12:24:52 $
**    $Author: Bryce $
**
*/

#pragma once

#ifndef BIPED_ANIM_SCRIPT_IF_H
#define BIPED_ANIM_SCRIPT_IF_H

class ScriptVM;

class CBipedAnimScriptIF
{
  public:
	void RegisterInterface(ScriptVM* i_vm);
	static void RegisterCallbacks(void);

	virtual bool ScriptOnGround(void) {return(false);}
	virtual void ScriptFootprint(int foot) {}
	virtual bool ScriptLaunch(Vector3CRef i_vel) {return(false);}
	virtual bool ScriptAiming(void) {return(true);}
	virtual bool ScriptCrouching(void) {return(true);}
	virtual bool ScriptJumping(void) {return(false);}
	virtual int ScriptCurrentWeapon(void) {return(0);}
	virtual int ScriptCurrentWeaponReloadType(void) {return(0);}
	virtual void ScriptKick( void ) {}
	virtual bool ScriptOnLadder( void ) {return(false);}
	virtual void ScriptStartAim( void ) {}
	virtual void ScriptStopAim( void ) {}
	virtual void ScriptStartCrouch( void ) {}
	virtual void ScriptStopCrouch( void ) {}
	virtual void ScriptStartTagging( void ) {}
	virtual void ScriptStopTagging( void ) {}

  private:
	static CBipedAnimScriptIF* GetInterface(ScriptVM* i_vm);
	static void OnGround(ScriptVM* i_vm);
	static void Footprint(ScriptVM* i_vm);
	static void BipedLaunch(ScriptVM* i_vm);
	static void BipedKick(ScriptVM* i_vm);
	static void Aiming(ScriptVM* i_vm);
	static void Crouching(ScriptVM* i_vm);
	static void Jumping(ScriptVM* i_vm);
	static void CurrentWeapon(ScriptVM* i_vm);
	static void CurrentWeaponReloadType(ScriptVM* i_vm);
	static void OnLadder(ScriptVM* i_vm);
	static void StartAim(ScriptVM* i_vm);
	static void StopAim(ScriptVM* i_vm);
	static void StartCrouch(ScriptVM* i_vm);
	static void StopCrouch(ScriptVM* i_vm);
	static void StartTagging(ScriptVM* i_vm);
	static void StopTagging(ScriptVM* i_vm);
};

#endif // BIPED_ANIM_SCRIPT_IF_H
