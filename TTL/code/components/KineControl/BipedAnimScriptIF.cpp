/*
**
**  File:   BipedAnimScriptIF.cpp
**  Date:   January 15, 2004
**  By:     Bryant Collard
**  Desc:   Provides the interface for the biped animation script.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/components/KineControl/BipedAnimScriptIF.cpp $
**  $Revision: #13 $
**  $DateTime: 2005/08/05 03:07:10 $
**    $Author: Rob $
**
*/

#include "TTLPCH.h"

#include "Script/ScriptVM.h"
#include "Script/ScriptSystem.h"
#include "components/KineControl/BipedAnimScriptIF.h"

#define REGISTER_CALLBACK(name) \
		g_ScriptSystem.RegisterFunction(#name, CBipedAnimScriptIF:: ## name)

/**********************************************************************/
/**********************************************************************/
void CBipedAnimScriptIF::RegisterInterface(ScriptVM* i_vm)
{
	i_vm->SetVar("interface", (int)this);
}

/**********************************************************************/
/**********************************************************************/
void CBipedAnimScriptIF::RegisterCallbacks(void)
{
	REGISTER_CALLBACK(OnGround);
	REGISTER_CALLBACK(Footprint);
	REGISTER_CALLBACK(BipedLaunch);
	REGISTER_CALLBACK(Aiming);
	REGISTER_CALLBACK(Crouching);
	REGISTER_CALLBACK(Jumping);
	REGISTER_CALLBACK(CurrentWeapon);
	REGISTER_CALLBACK(CurrentWeaponReloadType);
	REGISTER_CALLBACK(BipedKick);
	REGISTER_CALLBACK(OnLadder);
	REGISTER_CALLBACK(StartAim);
	REGISTER_CALLBACK(StopAim);
	REGISTER_CALLBACK(StartCrouch);
	REGISTER_CALLBACK(StopCrouch);
	REGISTER_CALLBACK(StartTagging);
	REGISTER_CALLBACK(StopTagging);
}

/**********************************************************************/
/**********************************************************************/
CBipedAnimScriptIF* CBipedAnimScriptIF::GetInterface(ScriptVM* i_vm)
{
	ASSERT_PTR( i_vm );

	CBipedAnimScriptIF* scriptIF =
		(CBipedAnimScriptIF*) i_vm->GetInt("interface");
	ASSERT_PTR( scriptIF );

	return scriptIF;
}

/**********************************************************************/
/**********************************************************************/
void CBipedAnimScriptIF::OnLadder(ScriptVM* i_vm)
{
	CBipedAnimScriptIF* scriptIF = GetInterface(i_vm);

	i_vm->Push(scriptIF->ScriptOnLadder());
}

/**********************************************************************/
/**********************************************************************/
void CBipedAnimScriptIF::OnGround(ScriptVM* i_vm)
{
	CBipedAnimScriptIF* scriptIF = GetInterface(i_vm);

	i_vm->Push(scriptIF->ScriptOnGround());
}

/**********************************************************************/
/**********************************************************************/
void CBipedAnimScriptIF::Footprint(ScriptVM* i_vm)
{
	CBipedAnimScriptIF* scriptIF = GetInterface(i_vm);
	int foot = i_vm->PopInt();

	scriptIF->ScriptFootprint( foot );
}

/**********************************************************************/
/**********************************************************************/
void CBipedAnimScriptIF::BipedLaunch(ScriptVM* i_vm)
{
	CBipedAnimScriptIF* scriptIF = GetInterface(i_vm);

	Vector3 vel = i_vm->PopVector3();
	i_vm->Push(scriptIF->ScriptLaunch(vel));
}

/**********************************************************************/
/**********************************************************************/
void CBipedAnimScriptIF::BipedKick(ScriptVM* i_vm)
{
	CBipedAnimScriptIF* scriptIF = GetInterface(i_vm);

	scriptIF->ScriptKick();
}

/**********************************************************************/
/**********************************************************************/
void CBipedAnimScriptIF::Aiming(ScriptVM* i_vm)
{
	CBipedAnimScriptIF* scriptIF = GetInterface(i_vm);

	i_vm->Push(scriptIF->ScriptAiming());
}

/**********************************************************************/
/**********************************************************************/
void CBipedAnimScriptIF::StartAim(ScriptVM* i_vm)
{
	CBipedAnimScriptIF* scriptIF = GetInterface(i_vm);

	scriptIF->ScriptStartAim();
}

/**********************************************************************/
/**********************************************************************/
void CBipedAnimScriptIF::StopAim(ScriptVM* i_vm)
{
	CBipedAnimScriptIF* scriptIF = GetInterface(i_vm);

	scriptIF->ScriptStopAim();
}

/**********************************************************************/
/**********************************************************************/
void CBipedAnimScriptIF::Crouching(ScriptVM* i_vm)
{
	CBipedAnimScriptIF* scriptIF = GetInterface(i_vm);

	i_vm->Push(scriptIF->ScriptCrouching());
}

/**********************************************************************/
/**********************************************************************/
void CBipedAnimScriptIF::StartCrouch(ScriptVM* i_vm)
{
	CBipedAnimScriptIF* scriptIF = GetInterface(i_vm);

	scriptIF->ScriptStartCrouch();
}

/**********************************************************************/
/**********************************************************************/
void CBipedAnimScriptIF::StopCrouch(ScriptVM* i_vm)
{
	CBipedAnimScriptIF* scriptIF = GetInterface(i_vm);

	scriptIF->ScriptStopCrouch();
}

/**********************************************************************/
/**********************************************************************/
void CBipedAnimScriptIF::StartTagging(ScriptVM* i_vm)
{
	CBipedAnimScriptIF* scriptIF = GetInterface(i_vm);

	scriptIF->ScriptStartTagging();
}

/**********************************************************************/
/**********************************************************************/
void CBipedAnimScriptIF::StopTagging(ScriptVM* i_vm)
{
	CBipedAnimScriptIF* scriptIF = GetInterface(i_vm);

	scriptIF->ScriptStopTagging();
}

/**********************************************************************/
/**********************************************************************/
void CBipedAnimScriptIF::Jumping(ScriptVM* i_vm)
{
	CBipedAnimScriptIF* scriptIF = GetInterface(i_vm);

	i_vm->Push(scriptIF->ScriptJumping());
}

/**********************************************************************/
/**********************************************************************/
void CBipedAnimScriptIF::CurrentWeapon(ScriptVM* i_vm)
{
	CBipedAnimScriptIF* scriptIF = GetInterface(i_vm);

	i_vm->Push(scriptIF->ScriptCurrentWeapon());
}

/**********************************************************************/
/**********************************************************************/
void CBipedAnimScriptIF::CurrentWeaponReloadType(ScriptVM* i_vm)
{
	CBipedAnimScriptIF* scriptIF = GetInterface(i_vm);

	i_vm->Push(scriptIF->ScriptCurrentWeaponReloadType());
}
