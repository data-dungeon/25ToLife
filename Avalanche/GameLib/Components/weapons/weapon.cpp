/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#include "Components/ComponentsPCH.h"

#include "Game/Database/lightset.h"
#include "EngineHelper/sfx.h"
#include "GameHelper/SharedBody.h"
#include "Components/weapons/weapon.h"
#include "Components/weapons/weaplib.h"
#include "Components/weapons/sightlib.h"
#include "Components/weapons/projlib.h"

// Define this to see the weapon properties
#if defined(FOOD) &&	defined(_DEBUG)
//#define DEBUG_WEAPONVECTORS
#endif

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
WeaponProperties::WeaponProperties(
WeaponCreator &creator,
int index) : VarObjProperties<Weapon>(creator, index)
{
	m_projectile[0] = '\0';
	m_aimSight[0] = '\0';
	m_displayName[0] = '\0';
	m_icon[0] = '\0';

	// The models
	for (int m = 0; m < WEAPON_MAX_MODELS; m++)
	{
		// Zero this sucka out
		memset(&m_model[m], '\0', sizeof(m_model[m]));
	}
	m_models = 0;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
WeaponProperties::~WeaponProperties()
{
}

/////////////////////////////////////////////////////////////////////////////
// For parsing the data
/////////////////////////////////////////////////////////////////////////////
void WeaponProperties::RegisterContext(
const char *context)
{
	// Call the parent
	VarObjProperties<Weapon>::RegisterContext(context);

	// Our classification
	g_console.CreateVar(context, "type", m_type, sizeof(m_type) - 1);

	// Create the context
	g_console.CreateVar(context, "projectile", m_projectile, sizeof(m_projectile) - 1);
	g_console.CreateVar(context, "aimSight", m_aimSight, sizeof(m_aimSight) - 1);
	g_console.CreateVar(context, "displayName", m_displayName, sizeof(m_displayName) - 1);
	g_console.CreateVar(context, "icon", m_icon, sizeof(m_icon) - 1);

	// The models
	for (int m = 0; m < WEAPON_MAX_MODELS; m++)
	{
		char var[64];

		sprintf(var, "model%d_name", m);
		g_console.CreateVar(context, var, m_model[m].name, sizeof(m_model[m].name) - 1);

		sprintf(var, "model%d_geometry", m);
		g_console.CreateVar(context, var, m_model[m].geometry);

		sprintf(var, "model%d_attachBone", m);
		g_console.CreateVar(context, var, m_model[m].attachBone, sizeof(m_model[m].attachBone) - 1);

		sprintf(var, "model%d_muzzle", m);
		g_console.CreateVar(context, var, m_model[m].weaponAttachPoint[BONE_MUZZLE]);

		sprintf(var, "model%d_breech", m);
		g_console.CreateVar(context, var, m_model[m].weaponAttachPoint[BONE_BREECH]);

		sprintf(var, "model%d_sight", m);
		g_console.CreateVar(context, var, m_model[m].weaponAttachPoint[BONE_SIGHT]);
	}
}

/////////////////////////////////////////////////////////////////////////////
// After the script is executed on the context
/////////////////////////////////////////////////////////////////////////////
void WeaponProperties::PostLoadSetup()
{
	VarObjProperties<Weapon>::PostLoadSetup();

	// This bugs me so much
	const char *weaponsDir = "weapons\\";
	const int dirLen = strlen(weaponsDir);
	for (int m = 0; m < WEAPON_MAX_MODELS; m++)
	{
		if (FixupModelName(m_model[m].name))
			++m_models;
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool WeaponProperties::FixupModelName(
char *model,
int size)
{
	// This bugs me so much
 	const int modelLen = strlen(model);
	if (!modelLen)
		return false;

	const char *weaponsDir = "weapons\\";
	const int dirLen = strlen(weaponsDir);

	ASSERT(modelLen + dirLen < size - 1);
	memmove(&model[dirLen], model, modelLen + 1);
	memcpy(model, weaponsDir, dirLen);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Weapon::Weapon(const WeaponProperties &properties) :
m_properties(properties),
m_aimSim(g_aimSimProps)
{
	// No owner yet
	m_owner = (ActorHandle)INVALID_OBJECT_HANDLE;

	// Setup the stuff about the weapon
	m_aimSim.SetWeapon(m_properties.GetWeaponAimProps(), m_properties.GetStances());

	// Create our aim sight
	m_aimSight = g_aimSightLib.CreateInstance(m_properties.m_aimSight);
	if (m_aimSight)
		m_aimSight->SetWeapon(this);

	// Create our projectile
	m_projectile = g_projectileLib.CreateInstance(m_properties.m_projectile);

	for (int m = 0; m < WEAPON_MAX_MODELS; m++)
	{
		m_dblHandle[m] = INVALID_HANDLE;
		memset(&m_geometry[m], '\0', sizeof(ts_Geometry));
		for (int b = 0; b < WEAPONBONES; b++)
			m_attachPoint[m][b] = INVALID_ATTACH_POINT;
	}

	/// No really load the models
	LoadModels();

	// Clear out the transforms per model
	for (int w = 0; w < Models(); w++)
	{
  		m_transform[w].boneID = INVALID_BONEID;
  		m_transform[w].boneToWorld.Identity();

		// Ready the sub-transforms
		for (int s = 0; s < WEAPONBONES; s++)
			m_subTransform[w][s].Identity();
	}
	m_bonesBound = false;

	// Get ready to go
	Reset();
}

/////////////////////////////////////////////////////////////////////////////
// Clean up
/////////////////////////////////////////////////////////////////////////////
Weapon::~Weapon()
{
	delete m_aimSight;
	delete m_projectile;

	// Tell the world!
	if (m_owner != (ActorHandle)INVALID_OBJECT_HANDLE)
		g_messageDispatcher.SendMessageToAll("WeaponDied", this, m_owner);
}

/////////////////////////////////////////////////////////////////////////////
// What is our name?
/////////////////////////////////////////////////////////////////////////////
const char *Weapon::Name() const
{
	return m_properties.GetName();
}

/////////////////////////////////////////////////////////////////////////////
// What is our name?
/////////////////////////////////////////////////////////////////////////////
const char *Weapon::DisplayName() const
{
	return m_properties.m_displayName;
}

/////////////////////////////////////////////////////////////////////////////
// What is our aimsight, this is public.
/////////////////////////////////////////////////////////////////////////////
AimSight *Weapon::GetSight()
{
	return m_aimSight;
}

/////////////////////////////////////////////////////////////////////////////
// Get our projectile
/////////////////////////////////////////////////////////////////////////////
Projectile *Weapon::GetProjectile()
{
	return m_projectile;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Weapon::SetOwner(
CActor *owner)
{
	// Setup the owner
	m_owner = CActor::ToHandle(owner);

	m_aimSim.SetOwner(owner);

	if (m_aimSight)
		m_aimSight->SetOwner(owner);

	if (m_projectile)
		m_projectile->SetOwner(owner);

	m_bonesBound = false;
}

/////////////////////////////////////////////////////////////////////////////
// Ask for a specific subclass, returns NULL if not here
/////////////////////////////////////////////////////////////////////////////
Weapon *Weapon::GetInterface(
const char *type)
{
	if (strcmpi(type, m_properties.GetClass()) == 0)
		return this;
	else
		return NULL;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
const WeaponModel &Weapon::GetModel(
int index)
{
	ASSERT(index >= 0);
	ASSERT(index < Models());

	// Give it to them
	return m_properties.m_model[index];
}

/////////////////////////////////////////////////////////////////////////////
// Attach/detach from an actor
/////////////////////////////////////////////////////////////////////////////
bool Weapon::Attach(CActor &actor)
{
	for (int m = 0; m < Models(); m++)
	{
		if (m_dblHandle[m] == INVALID_HANDLE)
			continue;

		const WeaponModel &model = GetModel(m);
		const int baseBone = model.geometry;
		actor.SetGeometry(baseBone, &m_geometry[m], 0);
		actor.SetGeometryAttachBone(baseBone, model.attachBone);
		actor.SetSkeletonIndex(SECONDARY_SKELETON, baseBone);
		actor.InitializeGeometry(baseBone);

		CInstance* inst = actor.Instance(baseBone);
		if (inst)
		{
			inst->SetReceiveShadows(false);
			inst->SetDynamicallyLit(true);
		}
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool Weapon::Detach(CActor &actor)
{
	for (int m = 0; m < Models(); m++)
	{
		const WeaponModel &model = GetModel(m);
		actor.DetachGeometry(model.geometry);
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
// Reset the state of this weapon
/////////////////////////////////////////////////////////////////////////////
void Weapon::Reset()
{
	m_aimSim.Reset();

	if (m_aimSight)
		m_aimSight->Reset();

	m_isHidden = false;
}

/////////////////////////////////////////////////////////////////////////////
// Activate/deactivate this weapon
/////////////////////////////////////////////////////////////////////////////
void Weapon::Activate()
{
	if (m_aimSight)
		m_aimSight->Activate(*this);
	if (m_projectile)
		m_projectile->Activate(*this);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Weapon::Deactivate()
{
	if (m_aimSight)
		m_aimSight->Deactivate(*this);
	if (m_projectile)
		m_projectile->Deactivate(*this);
}

/////////////////////////////////////////////////////////////////////////////
// Call this once per frame
/////////////////////////////////////////////////////////////////////////////
void Weapon::Update()
{
	// Update the transforms
	UpdateTransforms();

	// Update the sight and projectile
	if (m_aimSight)
		m_aimSight->Update(*this);
	if (m_projectile)
		m_projectile->Update(*this);

	// Update the aiming sim
	int activeModel = GetActiveModel();
	m_aimSim.SetMuzzle(GetBoneInWorld(activeModel, BONE_MUZZLE), GetBoneDir(activeModel, BONE_MUZZLE));
	m_aimSim.Update();
}

/////////////////////////////////////////////////////////////////////////////
// Load our model!
/////////////////////////////////////////////////////////////////////////////
bool Weapon::LoadModels()
{
	// The fixed list
	const char *s_weaponBoneLookup[WEAPONBONES] =
	{
		"muzzle",
		"breach",
		"sight"
	};

	// Load the models
	for (int m = 0; m < Models(); m++)
	{
		const WeaponModel &model = GetModel(m);

		// load the lbl
		char modelDBL[32];
		strcpy(modelDBL, model.name);
		strcat(modelDBL, ".dbl");
		m_dblHandle[m] = dblLoad(modelDBL);
		ASSERT(m_dblHandle[m] != INVALID_HANDLE);
		if (m_dblHandle[m] == INVALID_HANDLE)
			continue;

		// Get the geometry
		dblGetGeometry(&m_geometry[m], m_dblHandle[m]);
		ASSERT(m_geometry[m].pSOM || m_geometry[m].pLOD);

		// Now setup the attach points
		int attachPoints = m_attachPoints[m].Bind(m_dblHandle[m]);
#if !defined(BEC)
		ASSERT(attachPoints > 0);
#endif
		for (int p = 0; p < WEAPONBONES; p++)
		{
			if (m_properties.m_model[m].weaponAttachPoint[p])
			{
				m_attachPoint[m][p] = m_attachPoints[m].Find(s_weaponBoneLookup[p]);
#ifdef _DEBUG
				if (m_attachPoint[m][p] == INVALID_ATTACH_POINT)
				{
					TRACE("Trying to bind bone \"%s\" in gun \"%s\" (%d) with no luck!\n", s_weaponBoneLookup[p], Name(), m);
				}
#endif
				ASSERT(m_attachPoint[m][p] != INVALID_ATTACH_POINT);
			}
		}
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Try to resolve the bones
/////////////////////////////////////////////////////////////////////////////
void Weapon::BindBones()
{
	CActor *owner = CActor::FromHandle(m_owner);
	if (m_bonesBound || !owner || !owner->Instance() || !owner->Instance()->Body())
		return;

	m_bonesBound = true;
	for (int m = 0; m < Models(); m++)
	{
		// Get the main transfomm
		const WeaponModel &model = GetModel(m);
		if (m_transform[m].boneID == INVALID_BONEID)
		{
			int ret = boneGetBoneID(owner->Instance()->Body(), &m_transform[m].boneID, (char *)model.attachBone);
			ASSERT(ret > 0);
			if (!ret)
				m_bonesBound = false;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Weapon::Show()
{
	CActor *owner = CActor::FromHandle(m_owner);
	if (!owner)
		return;

	for (int m = 0; m < Models(); m++)
	{
		CInstance* inst = owner->Instance(m_properties.m_model[m].geometry);
		if (inst)
			inst->Hide(false);
	}

	m_isHidden = false;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Weapon::HideLock( bool hide )
{
	CActor *owner = CActor::FromHandle(m_owner);
	if(!owner)
		return;

	for (int m = 0; m < Models(); m++)
	{
		CInstance* inst = owner->Instance(m_properties.m_model[m].geometry);
		if (inst)
		{
			if( hide || m_isHidden )
				inst->Hide( true );
			else
				inst->Hide( false );
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Weapon::Hide()
{
	CActor *owner = CActor::FromHandle(m_owner);
	if(!owner)
		return;

	for (int m = 0; m < Models(); m++)
	{
		CInstance* inst = owner->Instance(m_properties.m_model[m].geometry);
		if (inst)
			inst->Hide(true);
	}

	m_isHidden = true;
}

/////////////////////////////////////////////////////////////////////////////
// Update the transforms
/////////////////////////////////////////////////////////////////////////////
void Weapon::UpdateTransforms()
{
	// Can't do anything without an owner
	CActor *owner = CActor::FromHandle(m_owner);
	if (!owner)
		return;

	// Bind the bones
	BindBones();

	// Loop through the weapon bones
	for (int m = 0; m < Models(); m++)
	{
		const WeaponModel &model = m_properties.m_model[m];

		if (m_transform[m].boneID == INVALID_BONEID)
			m_transform[m].boneToWorld = owner->Instance()->Matrix();
		else
		{
			// Do the bone
			Graphics4x4 boneToModel;
		 	boneGetCurrentBoneToModel(owner->Instance()->Body(), m_transform[m].boneID, &boneToModel);
			m_transform[m].boneToWorld = boneToModel * owner->Instance()->Matrix();
		}

		for (int s = 0; s < WEAPONBONES; s++)
		{
			if (m_attachPoint[m][s] == INVALID_ATTACH_POINT)
				m_subTransform[m][s] = m_transform[m].boneToWorld;
			else
				m_subTransform[m][s] = m_attachPoints[m].PeekMatrix(m_attachPoint[m][s]) * m_transform[m].boneToWorld;
		}

#ifdef DEBUG_WEAPONVECTORS
		if (m_bonesBound)
		{
			// Draw everything
			Vector3 origin = Vector3(0.0f, 0.0f, 0.0f) * m_transform[m].boneToWorld;
			DrawUtility::Normal(origin, m_transform[m].boneToWorld.GetRow0(), DrawUtility::RED);
			DrawUtility::Normal(origin, m_transform[m].boneToWorld.GetRow1(), DrawUtility::GREEN);
			DrawUtility::Normal(origin, m_transform[m].boneToWorld.GetRow2(), DrawUtility::BLUE);

			Vector3 muzzleEnd = GetBoneInWorld(m, BONE_MUZZLE);
			DrawUtility::Point(muzzleEnd, DrawUtility::YELLOW, 0.1f);
			Vector3 muzzleDir = GetBoneDir(m, BONE_MUZZLE);
			DrawUtility::Normal(muzzleEnd, muzzleDir, DrawUtility::MAGENTA);

			Vector3 breech = GetBoneInWorld(m, BONE_BREECH);
			DrawUtility::Point(breech, DrawUtility::CYAN, 0.1f);
			Vector3 eject = GetBoneDir(m, BONE_BREECH);
			DrawUtility::Normal(breech, eject, DrawUtility::WHITE);
		}
#endif
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Vector3 Weapon::GetWeaponInWorld(
int model)
{
	ASSERT(model >= 0 && model < Models());
	return m_transform[model].boneToWorld.GetRow3();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
const Graphics4x4 &Weapon::GetWeaponToWorld(
int model)
{
	ASSERT(model >= 0 && model < Models());
	return m_transform[model].boneToWorld;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Vector3 Weapon::GetWeaponDir(int model)
{
	ASSERT(model >= 0 && model < Models());
	return m_transform[model].boneToWorld.GetRow2();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Graphics4x4 Weapon::GetWeaponPlacement(
int model)
{
	Graphics4x4 placement;

	CActor *owner = CActor::FromHandle(m_owner);
	if (!owner)
		placement.Identity();
	else
	{
		const WeaponModel &weaponModel = Weapon::GetModel(model);
		CInstance *instance = owner->Instance(weaponModel.geometry);
		ASSERT(instance);
		placement = instance->Matrix();
	}

	return placement;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Vector3 Weapon::GetWeaponVelocity(
int model)
{
	Vector3 vel( 0.0f, 0.0f, 0.0f );
	CActor *owner = CActor::FromHandle(m_owner);
	return ( owner ? owner->GetVelocityInWorld() : vel );
}

/////////////////////////////////////////////////////////////////////////////
// Get some info about the gun
/////////////////////////////////////////////////////////////////////////////
Vector3 Weapon::GetBoneInWorld(
int model,
WeaponBone forBone)
{
	ASSERT(model >= 0 && model < Models());
	ASSERT(forBone != WEAPONBONES);

	return m_subTransform[model][forBone].GetRow3();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
const Graphics4x4 &Weapon::GetBoneToWorld(
int model,
WeaponBone forBone)
{
	ASSERT(model >= 0 && model < Models());
	ASSERT(forBone != WEAPONBONES);

	return m_subTransform[model][forBone];
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
DirCos3x3 Weapon::GetBoneOrientation(
int model,
WeaponBone forBone)
{
	ASSERT(model >= 0 && model < Models());
	ASSERT(forBone != WEAPONBONES);

	Cartesian4x4 &m = (Cartesian4x4 &)m_subTransform[model][forBone];
	DirCos3x3 o;
	o = m;
	return o;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Vector3 Weapon::GetBoneDir(
int model,
WeaponBone forBone)
{
	ASSERT(model >= 0 && model < Models());
	ASSERT(forBone != WEAPONBONES);
	return m_subTransform[model][forBone].GetRow2();
}

