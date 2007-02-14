/*
**
**  File:   iactorspy.h
**  Date:   August 2, 2005
**  By:     Bryant Collard
**  Desc:   Instrument to display actor's names (and hosts if applicable).
**
**  Copyright (c) 2005, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/hud/iactorspy.cpp $
**  $Revision: #1 $
**  $DateTime: 2005/08/02 15:34:31 $
**    $Author: Bryant_Collard $
**
*/

// Precompiled header.
#include "TTLPCH.h"

// My header.
#include "hud/iactorspy.h"

// GameLib headers.
#include "Network/NetActor.h"

// TTL headers.
#include "weapons/ttlweapinv.h"
#include "weapons/ttlweapon.h"

/******************************************************************************/
/******************************************************************************/
CActorSpyInstrument::CActorSpyInstrument() : TextInstrument("actorspy")
{
	d_active = false;
}

/******************************************************************************/
/******************************************************************************/
void CActorSpyInstrument::Initialize(void)
{
	d_toggleActorSpyHandler.Initialize("ToggleActorSpy",
			MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this,
			&CActorSpyInstrument::ToggleActorSpy);
}

/******************************************************************************/
/******************************************************************************/
void CActorSpyInstrument::Update(float i_deltaSec)
{
	if (!d_active)
		SetText(NULL);
	else
	{
		CCompActor* actor = g_hud.GetSubject();
		TTLWeaponInventory* inventory = (actor == NULL) ? NULL :
				(TTLWeaponInventory*)actor->GetComponentByName("WeaponInventory");
		TTLWeapon* weapon = (inventory == NULL) ? NULL :
				(TTLWeapon*)inventory->GetSelected();
		CActor* target = (weapon == NULL) ? NULL :
				weapon->GetAimSim().TargetActor();
		const char* name = NULL;
		if (target != NULL)
		{
			if ((target->InstanceName() != NULL) &&
					(*target->InstanceName() != '\0'))
				name = target->InstanceName();
			else if ((target->Name() != NULL) && (*target->Name() != '\0'))
				name = target->Name();
		}
		if (name == NULL)
			SetText(NULL);
		else
		{
			char text[128];
			strcpy(text, name);
			CNetActor* netActor = target->GetNetActor();
			if (netActor != NULL)
			{
				const char* host = netActor->GetHostName();
				if (host != NULL)
				{
					strcat(text, " | ");
					strcat(text, host);
				}
			}
			SetText(text);
			m_text.SetColor(0.5f, 0.5f, 0.5f, 0.75f);
			m_text.SetPosition(m_pos.X(), m_pos.Y() - 32.0f);
		}
	}

	// Do the rest of the wrok
	TextInstrument::Update(i_deltaSec);
}

/******************************************************************************/
/******************************************************************************/
void CActorSpyInstrument::ToggleActorSpy(void* i_data, ActorHandle i_sender,
		bool i_posted)
{
	d_active = !d_active;
}
