/////////////////////////////////////////////////////////////////////////////
// Weapon inventory
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"
#include "Components/ComponentCreator.h"
#include "Components/Property.h"

#include "weapons/zoomsight.h"
#include "weapons/ttlweapon.h"
#include "Weapons/lobbed.h"
#include "interfaces/viewif.h"
#include "weapons/ttlweapinv.h"

// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(TTLWeaponInventory, WeaponInventory, -9999);

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
TTLWeaponInventory::TTLWeaponInventory(CCompActor &owner) :
WeaponInventory(owner)
{
	actor.AddProperty("zoomLevel", Property_int);
	actor.SetProperty("zoomLevel", 0);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void TTLWeaponInventory::Initialize()
{
	m_weaponFireHandler.Initialize(MUNITION_FIREDFX,
												MESSAGE_HANDLE_IF_MASTER | MESSAGE_HANDLE_IF_DUPLICA,
												actor.GetMessageOwner(), this);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool TTLWeaponInventory::SelectNext()
{
	int current = GetSelectedIndex();
	int next = GetNextIndex();
	if (current == next || next == -1)
		return false;
	else
		return Select(next);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool TTLWeaponInventory::SelectPrevious()
{
	int current = GetSelectedIndex();
	int prev = GetPreviousIndex();
	if (current == prev || prev == -1)
		return false;
	else
		return Select(prev);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool TTLWeaponInventory::SelectBest()
{
	TTLWeapon *primary = (TTLWeapon *)Get(2);
	TTLWeapon *secondary = (TTLWeapon *)Get(1);

	if (primary && primary->HasAmmo())
	{
		Select(2);
		return true;
	}
	else if (secondary && secondary->HasAmmo())
	{
		Select(1);
		return true;
	}
	else if (primary)
	{
		Select(2);
		return true;
	}
	else if (secondary)
	{
		Select(1);
		return true;
	}


	return false;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
int TTLWeaponInventory::GetNextIndex()
{
	int index = GetSelectedIndex();
	int tries = Count() - 1;
	int end = GetLobbed() ? Count() - 1 : Count();
	int best = -1;
	while (tries-- > 0)
	{
		// who do we select?
		if (index < 1 || ++index >= end)
			index = 1;

		// Check it
		TTLWeapon *weapon = (TTLWeapon *)Get(index);
		if (weapon)
			best = index;
		if (weapon)//&& weapon->HasAmmo())
			return index;
	}

	return best;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
int TTLWeaponInventory::GetPreviousIndex()
{
	int index = GetSelectedIndex();
	int tries = Count();
	int end = GetLobbed() ? Count() - 2 : Count() - 1;
	int best = -1;
	while (tries-- > 0)
	{
		// who do we select?
		if (--index < 1)
			index = end;

		// Check it
		TTLWeapon *weapon = (TTLWeapon *)Get(index);
		if (weapon)
			best = index;
		if (weapon)// && weapon->HasAmmo())
			return index;
	}

	return best;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool TTLWeaponInventory::Select(
int index)
{
	// Deactivate current zoom sight
	TTLWeapon *lastWeapon = (TTLWeapon *)GetSelected();

	// Call the parent
	if (!WeaponInventory::Select(index))
		return false;

	// Ooops
	TTLWeapon *weapon = (TTLWeapon *)GetSelected();
	if (lastWeapon == weapon)
		return true;

	// Deselect
	g_messageDispatcher.SendMessageToActor("WeaponDeselected", lastWeapon, actor.GetHandle(), actor.GetHandle());

	// Monkey with the zoom sight
	if (lastWeapon)
	{
		ZoomSight *zoomSight = lastWeapon->GetZoomSight();
		if (zoomSight)
			zoomSight->SetZoomProperty(NULL);
	}

	Property *zoomProperty = actor.GetProperty("zoomLevel");
	if (weapon)
	{
		ZoomSight *zoomSight = weapon->GetZoomSight();
		if (zoomSight)
			zoomSight->SetZoomProperty(zoomProperty);
		else if (zoomProperty)
			zoomProperty->Set(0);
	}
	else
	{
		// Reset the zoom property if there is no zoom sight.
		if (zoomProperty)
			zoomProperty->Set(0);
	}

	// Damn
	Lobbed *lobbed = GetLobbed();
	if (lobbed)
		lobbed->Cancel();

	// let our owner know his weapon has changed, so he can pass the message through to
	// any components that care
	g_messageDispatcher.SendMessageToActor("WeaponSelected", weapon, actor.GetHandle(), actor.GetHandle());

	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void TTLWeaponInventory::Reset()
{
	// Deselect
	g_messageDispatcher.SendMessageToActor("WeaponDeselected", GetSelected(), actor.GetHandle(), actor.GetHandle());

	// Call the parent
	WeaponInventory::Reset();

	// let our owner know his weapon has changed, so he can pass the message through to
	// any components that care
	g_messageDispatcher.SendMessageToActor("WeaponSelected", GetSelected(), actor.GetHandle(), actor.GetHandle());
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void TTLWeaponInventory::ReArm()
{
	for (int i = 0; i < Count(); i++)
	{
		TTLWeapon *weapon = (TTLWeapon *)Get(i);
		if (weapon)
			weapon->FullAmmo(weapon->GetAmmoType());
	}
}

/////////////////////////////////////////////////////////////////////////////
// Do this once per frame
/////////////////////////////////////////////////////////////////////////////
void TTLWeaponInventory::RunAnimation()
{
	// Update the lobbed if it is not selected
	TTLWeapon *weapon = (TTLWeapon *)GetSelected();
	Lobbed *lobbed = GetLobbed();

	// Make sure we update this information
	bool isShooter = (g_shooterCamPtr && g_shooterCam.GetFollow() == &actor);
	bool shooterActive = (&g_cameraDirector.CurrentCamera() == g_shooterCamPtr);
	if (isShooter && weapon)
	{
		// Is the shooter active?
		if (!shooterActive)
		{
			weapon->ClearAim();
			if (lobbed)
				lobbed->ClearAim();
		}
		else
		{
			// We are active and aiming!
			bool scoped = g_shooterCam.FirstPersonLock();
			if (scoped)
			{
				weapon->GetAimSim().SetAimType(AimingSimulation::CAT_SIGHTLINE);
		  		weapon->AimCameraShake(true);
			}
			else
			{
				weapon->GetAimSim().RestoreAimType();
				weapon->AimCameraShake(false);
			}

			Vector3 aimOrigin, aimDir;
			g_shooterCam.AimOriginDir(aimOrigin, aimDir);
			weapon->SetAim(aimOrigin, aimDir);
			if (lobbed)
				lobbed->SetAim(aimOrigin, aimDir);
		}
	}

	// Update the aim point
	ViewIF *viewIF = (ViewIF *)actor.GetInterface(ViewIF::GetID());
	if (viewIF)
	{
		Vector3 direction;
		if (!isShooter || !shooterActive || !weapon)
			viewIF->GetDirection(direction);
		else
		{
			Vector3 target, origin;

			AimingSimulation &aimSim = weapon->GetAimSim();
			origin = actor.GetFocusInWorld();
			target = aimSim.FilteredAimPoint();
			direction = target - origin;
			direction.SafeNormalize();
#if 0
			DrawUtility::Sphere(target, DrawUtility::YELLOW, 0.25f);
			DrawUtility::Sphere(origin, DrawUtility::WHITE, 0.25f);
			DrawUtility::Normal(origin, direction, DrawUtility::WHITE, 2.0f);
#endif
		}
		direction = direction / actor.GetBodyToWorld();
		actor.SetProperty( "aimDirection", direction );
	}

	// We might have changed FOV
	if (isShooter)
	{
		ZoomSight *aimSight = weapon ? weapon->GetZoomSight() : NULL;
		if (!aimSight)
			g_shooterCam.RestoreFOV(0.0f);
		else if (aimSight->ZoomLevel() == 0)
			g_shooterCam.RestoreFOV(aimSight->FOVFilter());
		else
			g_shooterCam.SetFOVFactor(aimSight->FOV(), aimSight->FOVFilter());

		// Tell them what is going on
		if (weapon)
			weapon->GetAimSim().SetAiming(aimSight && (aimSight->ZoomLevel() != 0));
	}

	// Call the parent (updates the main weapon)
	WeaponInventory::RunAnimation();

	// Get everyoene to update
	if (lobbed)
	{
		if (weapon != lobbed)
			lobbed->Update();

		// Kill the lobbed weapon if it gets empty
		if (isShooter && !lobbed->HasAmmo() && lobbed->GetState() == Weapon::NOTREADY)
			Remove(3);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Get the current lobbed weapon (left hand), could be NULL
/////////////////////////////////////////////////////////////////////////////
Lobbed *TTLWeaponInventory::GetLobbed()
{
	TTLWeapon *weapon = (TTLWeapon *)Get(3);
	if (!weapon || weapon->GetWeaponClass() != WEAPONCLASS_LOBBED)
		return NULL;
	else
		return (Lobbed *)weapon;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void TTLWeaponInventory::WeaponFired(
const	CMunitionFired &fire)
{
	// handle the fire!
	for (int i = 0; i < Count(); i++)
	{
		TTLWeapon *weapon = (TTLWeapon *)Get(i);
		if (weapon && fire.d_weaponID == weapon->GetID())
		{
			weapon->DryFire(fire, i == GetSelectedIndex());
			break;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void TTLWeaponInventory::WeaponFireHandler::HandleMessage(
void *i_data,
ActorHandle i_sender,
bool i_posted)
{
	CMunitionFired *fire = (CMunitionFired *)i_data;
	ASSERT_PTR(fire);
	d_ref->WeaponFired(*fire);
}
