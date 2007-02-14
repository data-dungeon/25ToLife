/////////////////////////////////////////////////////////////////////////////
// Things that affect a combatant in the game
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "Components/ComponentCreator.h"
#include "components/affliction.h"
#include "components/onfire.h"

STANDARD_COMPONENT_CREATOR(OnFire, OnFire, COMPONENT_NEUTRAL_PRIORITY);

#ifdef PS2
const char *OnFire::s_boneNameSP[] =
{
	"leftarm", "leftforearm",
	"rightarm",	"rightforearm",
	"head", "leftleg", "rightleg"
};
const char *OnFire::s_boneNameMP[] =
{
	"leftforearm",
	"rightforearm",
	"head", "leftleg", "rightleg"
};
#else
const char *OnFire::s_boneNameSP[] =
{
	"leftshoulder", "leftarm", "leftforearm",	"lefthand",
	"rightshoulder", "rightarm", "rightforearm", "righthand",
	"head",
	"leftupleg", "leftleg",
	"rightupleg", "rightleg"
};
const char *OnFire::s_boneNameMP[] =
{
	"leftarm", "leftforearm",
	"rightarm",	"rightforearm",
	"head", "leftleg", "rightleg"
};
#endif
const int OnFire::s_bonesSP = sizeof(OnFire::s_boneNameSP) / sizeof(const char *);
const int OnFire::s_bonesMP = sizeof(OnFire::s_boneNameMP) / sizeof(const char *);

const OnFire::Effect OnFire::s_effect[OnFire::BURNTYPES] =
{
	{ "", NULL, false, Vector3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f },
	{ "FleshBurn", "onfire_loop", true, Vector3(255.0f / 255.0f, 228.0f / 255.0f, 103.0f / 255.0f), 0.2f, 1.0f },
	{ "Taser", "stun_long", true, Vector3(136.0f / 255.0f, 236.0f / 255.0f, 254.0f / 255.0f), 0.2f, 1.0f },
	{ "Taser", "stun_short", true, Vector3(136.0f / 255.0f, 236.0f / 255.0f, 254.0f / 255.0f), 0.8f, 1.0f }
};

/////////////////////////////////////////////////////////////////////////////
// Pass in a list (terminated with a NULL string)
/////////////////////////////////////////////////////////////////////////////
OnFire::OnFire(
CCompActor &owner) :
CActorComponent(owner)
{
	m_burnType = BURNTYPE_NONE;

	m_sound = NULL;
	m_bone = NULL;
	m_bones = 0;

	m_light = NULL;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
OnFire::~OnFire()
{
	QuenchBurn();
	delete [] m_bone;
}

/////////////////////////////////////////////////////////////////////////////
// Reset!
/////////////////////////////////////////////////////////////////////////////
void OnFire::Reset()
{
	QuenchBurn();
}

/////////////////////////////////////////////////////////////////////////////
// Update the OnFire
/////////////////////////////////////////////////////////////////////////////
void OnFire::EndUpdate(
float i_deltaSec)
{
	// Set our state
	Afflictions *afflictions = (Afflictions *)actor.GetComponentByName("Afflictions");
	bool onFire = (afflictions && afflictions->Afflicted(Afflictions::ONFIRE));
	bool stunned = (afflictions && afflictions->Afflicted(Afflictions::STUNNED));
	bool shocked = (afflictions && afflictions->Afflicted(Afflictions::SHOCK));

	if (onFire && m_burnType != BURNTYPE_FIRE)
		StartBurn(BURNTYPE_FIRE);
	else if (stunned && m_burnType != BURNTYPE_STUNNED)
		StartBurn(BURNTYPE_STUNNED);
	else if (shocked && m_burnType != BURNTYPE_SHOCK)
		StartBurn(BURNTYPE_SHOCK);
	else if (!onFire && !stunned && !shocked)
		QuenchBurn();
}

/////////////////////////////////////////////////////////////////////////////
// the animation
/////////////////////////////////////////////////////////////////////////////
void OnFire::RunAnimation()
{
	// Wee!
	if (m_burnType != BURNTYPE_NONE)
		UpdateBones();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void OnFire::StartBurn(
BurnType type)
{
	if (m_burnType == type)
		return;
	QuenchBurn();

	// Do we have any bones yet
	if (!m_bone && !CreateBones())
		return;

	// Sound?
	if (s_effect[type].soundName && s_effect[type].soundName[0])
		Sfx::Play(s_effect[type].soundName, actor, Sfx::ACTORPLAYPOS_DEFAULT, 1.0f, 0.0f, &m_sound);

	// Do a light?
	if (s_effect[type].wantLight)
	{
		SFXLight::Params params;
		params.priority = SFXLight::SFXLIGHT_HIGH_PRIORITY;
		params.type = CLightRecord::LIGHT_OMNI;
		params.color = s_effect[type].lightColor;
		params.attenuation = s_effect[type].lightAttenuation;
		params.intensity = s_effect[type].lightIntensity;
		params.ttl = -1.0f;
		m_light = SFXLight::Create("onfire", actor, NULL, params);
	}

	// Force an update
	UpdateBones();

	// Do it!
	for (int bone = 0; bone < m_bones; bone++)
	{
		// Create it
		if (m_bone[bone].bone != INVALID_BONEID)
			m_bone[bone].emitter = ParticleEmitter__CreateFromRecord(s_effect[type].emitterName,
																						&m_bone[bone].pos,
																						&m_velocity,
																						NULL, false, NULL, false);
	}

	m_burnType = type;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void OnFire::QuenchBurn()
{
	if (m_burnType == BURNTYPE_NONE)
		return;

	// Stop the sound
	Sfx::Stop(m_sound);

	if (m_light)
	{
		SFXLight::Destroy(m_light);
		m_light = NULL;
	}

	for (int bone = 0; bone < m_bones; bone++)
	{
		// Destroy it
		if (m_bone[bone].emitter != INVALID_HANDLE)
		{
			ParticleEmitter__Kill(m_bone[bone].emitter);
			m_bone[bone].emitter = INVALID_HANDLE;
		}
	}

	m_burnType = BURNTYPE_NONE;
}

/////////////////////////////////////////////////////////////////////////////
// The initialize
/////////////////////////////////////////////////////////////////////////////
bool OnFire::CreateBones()
{
	// Setup the bones
	const char **boneName;
	int bones;
	if (ValidSingleton(CSession) && g_session.IsNetworked())
	{
		boneName = s_boneNameMP;
		bones = s_bonesMP;
	}
	else
	{
		boneName = s_boneNameSP;
		bones = s_bonesSP;
	}

	// Make the array
	ASSERT(bones > 0);
	m_bone = new BurningBone[bones];
	ASSERT(m_bone);
	if (!m_bone)
		return false;
	m_bones = bones;

	// Query all the bones
	for (int bone = 0; bone < m_bones; bone++)
	{
		ASSERT(boneName[bone][0]);
		int ret = boneGetBoneID(actor.Instance()->Body(), &m_bone[bone].bone, (char *)boneName[bone]);
		ASSERT(ret >= 0);
		if (ret < 0)
			m_bone[bone].bone = INVALID_BONEID;
		m_bone[bone].emitter = INVALID_HANDLE;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void OnFire::UpdateBones()
{
	HealthIF *health = (HealthIF *)actor.GetInterface(HealthIF::GetID());
	if (health && health->IsAlive())
		m_velocity = actor.GetVelocityInWorld();
	else
		m_velocity.Clear();

	for (int bone = 0; bone < m_bones; bone++)
	{
		if (m_bone[bone].bone != INVALID_BONEID)
		{
			Graphics4x4 boneToModel;
	 		boneGetCurrentBoneToModel(actor.Instance()->Body(), m_bone[bone].bone, &boneToModel);
			Graphics4x4 boneToWorld = boneToModel * actor.Instance()->Matrix();

			m_bone[bone].pos = boneToWorld.GetRow3();
		}
	}
}
