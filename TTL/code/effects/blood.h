///////////////////////////////////////////////////////////////////////////////
// The class to make blood...
///////////////////////////////////////////////////////////////////////////////
#ifndef BLOOD_H
#define BLOOD_H

#include "EngineHelper/Singleton.h"
#include "Math/MathClass.h"
#include "Math/fastrand.h"
#include "CollideHelper/ObbNode.h"
#include "Effects/decal/adecal.h"

// Maximum bleed types
#define MAX_BLEEDTYPES 		3
#define MAX_DIETYPES 		2
#define MAX_IMPACTTYPES 	3

class BloodPool;

class Blood
{
public:
	Blood();
	~Blood();

	// Control if we are on or not
	void Enable(bool enabled)
		{ m_enabled = enabled; }
	bool Enabled()
		{ return m_enabled; }

	// Initialize everything
	bool Load(const char *fxDBL);

	// Is it time to bleed yet?
	// You keep lastbled in your entity and I update it. Init it to 0.0f
	bool TimeToBleed(float health, float &nextBleed);

	// Bleed (place a blood spatter on the ground underneath this point
	// Quantity goes [0-1] for the size of the pool
	void Bleed(CActor &bleeder, float health, const char *boneName = NULL);

	// A character has died, place a pool of blood underneath them.
	void DeathPool(CActor &corpse, const char *boneName);

	// A character has been hit by a shot, place blood on the wall behind
	void Impact(Vector3CRef impact, Vector3CRef dir, CActor &victim, float energy);

	// A character has been hit by a shot, place blood on the wall behind
	void ImpactBleed(CActor &bleeder, const char *boneName, float energy);

private:
	// Are we enabled
	bool			m_enabled;

	// Our internal sutff
	DecalSet		m_decalSet;

	// Params from the blood.var
	float			m_minUVT, m_maxUVT;
	float			m_colorIntensity, m_minColorIntensity, m_maxColorIntensity;
	int			m_bloodAlpha;

	int			m_bleedBaseIndex;
	int			m_bleedEmitters;
	int			m_bleedFXCount;
	float			m_bleedMinSizeFactor, m_bleedBaseSize;
	float			m_bleedSizeVaryMin, m_bleedSizeVaryMax;
	float			m_bleedAtHealth;
	float			m_bleedMinRate, m_bleedMaxRate;
	float			m_bleedVariance;
	float			m_bleedFilterT;
	float			m_bleedRadiusDivisor;
	float			m_bleedImpactRangeMin, m_bleedImpactRangeMax;
	float			m_bleedImpactRadius;
	float			m_bleedImpactEnergyDivisor;
	int			m_bleedImpactSpurtsMin, m_bleedImpactSpurtsMax;

	int			m_dieBaseIndex;
	int			m_dieEmitters;
	int			m_dieFXCount;
	float			m_dieBaseSize;
	float			m_dieFilterT;
	int			m_bloodyFootsteps;

	int			m_impactBaseIndex;
	int			m_impactEmitters;
	int			m_impactFXCount;
	float			m_impactBaseSize, m_impactMaxSize;
	float			m_impactEnergyDivisor;
	float			m_impactMaxRange;
	float			m_impactFilterT;
	float			m_impactCone;

	float			m_ps2FXMultiply;

	// Emitter for spatter & death
	AnimatedDecalEmitter	*m_bleedEmitter[MAX_BLEEDTYPES];
	AnimatedDecalEmitter	*m_dieEmitter[MAX_DIETYPES];
	AnimatedDecalEmitter	*m_impactEmitter[MAX_IMPACTTYPES];

	// Do a bleed
	void Bleed(Vector3CRef from, float size);

	// Emitter handiness
	int 						m_onBleed;
	AnimatedDecalEmitter	*NextBleed();
	int 						m_onDie;
	AnimatedDecalEmitter	*NextDie();
	int 						m_onImpact;
	AnimatedDecalEmitter	*NextImpact();

	// What is our material?
	char		m_material[12];
	u8			m_surfaceType;

	// My own little compressed collision result
	struct Collide
	{
		Vector3		point;
		Vector3		normal;
		float			d;
		u16			color;
		u8				material;
	};

	// Find the ground
	bool FindGround(Vector3CRef start, Collide &hit);

	// find an intersection w/the world
	bool FindWorld(Vector3CRef start, Vector3CRef dir, float maxRange, Collide &hit);

	// Some nice utilties
	inline float RandomTwist()
		{ return g_random.InRange(-Math::Pi, Math::Pi); }
	inline void StartUV(float uvT[4])
		{ 	for (int i = 0; i < 4; i++) uvT[i] = g_random.InRange(m_minUVT, m_maxUVT); }

	// Convert colors
	ts_bRGBA MakeColor(u16 color, u8 alpha = 255);
	ts_bRGBA MakeIntensity(u16 color, u8 alpha = 255);

	// Bone stuff
	Vector3 GetBoneInWorld(CActor &forWho, const char *boneName);

	// Blood pools in the world
	DoublyLinkedList<BloodPool>	m_bloodPool;
	void AddBloodPool(const Vector3 &pos, const Vector3 &normal, float size);
	void KillBloodPools();

	// A utility to create an emitter
	AnimatedDecalEmitter *CreateEmitter(DecalSet &decalSet, int textureIndex);

	// Checkpoint reset!!
	REF_BASIC_MESSAGE_HANDLER(CheckpointReset, MESSAGE_VALUE, Blood) m_checkpointReset;
};

class BloodPool : public CProtagonist
{
public:
	BloodPool() {}
	~BloodPool() {}

	bool Initialize(const Vector3 &pos, const Vector3 &normal, float size, int bloodyFootsteps);
	virtual void CollisionAccept(CStageEntity* i_entity, CCollisionEvent* i_event);

	// For the managed list
	BloodPool	*prev, *next;

private:
	CObbNode		d_obbNode;
	int			d_bloodyFootsteps;
};

// There can be only one!
DefineSingleton(Blood)
#define g_blood GetSingleton(Blood)

#endif
