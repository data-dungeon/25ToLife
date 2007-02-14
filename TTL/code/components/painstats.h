//============================================================================
//=
//= PainStats - keeps track of what pain texture stage we have on body parts
//=
//============================================================================
#ifndef _PAIN_STATS_H
#define _PAIN_STATS_H

#include "Components/Component.h"
#include "effects/pain_decal.h"

#define MAX_YARDSALE_PARTS	3

struct PainStatsParams
{
	float		heavyDamage;
	float		criticalDamage;
	float		decapitationDamage;
};

//============================================================================
class CPainStats: public CActorComponent
{
public:
	// component base stuff
	CPainStats(CCompActor &i_actor);
	~CPainStats() {}

	void	Initialize();
	const char *ComponentName()
		{ return "PainStats"; }

	// Reset everything
	void Reset();

	// Wee!
	void EndUpdate(float i_deltaSec);

	// Describe a hit!
	enum Direction
	{
		DIRECTION_INVALID = -1,
		DIRECTION_NONE = 0,
		DIRECTION_FORWARD,
		DIRECTION_BACKWARD,
		DIRECTION_LEFT,
		DIRECTION_RIGHT
	};

	enum Location
	{
		LOCATION_HEAD = 0,
		LOCATION_BODY,
		LOCATION_RIGHTSHOULDER,
		LOCATION_LEFTSHOULDER,
		LOCATION_RIGHTLEG,
		LOCATION_LEFTLEG
	};

	// This should only be called on the master
	bool HandleHit(Location location, Direction direction, int munitionID, float damage);

	// Call this all you want on the master!
	static bool HandleHit(CCompActor &actor, Location location, Direction direction, int munitionID, float damage);

	// This is needed to update the duplicas' pain state
	int GetState();
	void SetState(int i_state);

	// For debugging these are public, but don't call them
	void YardSaleIncrease();
	void UpThePain(CPainDecal::Zone zone, int amount = 1);
	void Decapitate(bool decapitate);
	bool Decapitated()
		{ return m_decapitated; }
	void SetWeaponDropped()
	{ m_droppedWeapon = DWS_DROPPED; }

private:
	// Our params
	PainStatsParams	*m_params;

	// our pain state
	bool	m_wantToDecapitate;
	bool	m_decapitated;
	bool	m_showingWeapon;
	int 	m_painLevel[CPainDecal::PAINZONES];
	int 	m_yardSaleSOMIndex[MAX_YARDSALE_PARTS];
	int		m_yardSaleLevel;

	// Track our health
	float	m_health;

	// Our dropped weapon state
	enum DroppedWeaponState
	{
		DWS_PENDING,
		DWS_DROPPED,
		DWS_NOTDROPPED
	};
	DroppedWeaponState m_droppedWeapon;

	// Message!
	int FormulateState();
	void DecimateState(int state);

	// Apply the stae
	void ApplyState();

	// Health utilities
	float GetHealthPercentage();
	float GetHitPoints();
	bool Alive();

	// Yardsale information
	void BuildYardsaleParts();
	void ApplyYardSale();
	void YardSaleEject(int part);
	void YardSaleReattach(int part);

	// Dropping the weapon
	void DropWeapons();
	void HideWeapons();
	void ShowWeapons();
};

struct DroppedWeaponParams
{
	int					dropPercent;
	Vector3Packed		omegaMin, omegaMax;
	char					material[32];
	float					mass;
	Vector3Packed		inertiaTensor;
	float					ttl;
	int					ignoreMs;
	float					updateHz;
	char					bounceSound[32];
	float					bounceDivisor;
	float					alphaFadeRate;
};

class WeaponActor : public CCompActor
{
public:
	WeaponActor(const DroppedWeaponParams &params);
	~WeaponActor();

	const char* GetClassName()
		{ return("WeapAct");}
	uint8 Material(CCollisionPair* i_pair)
		{ return d_surfaceType; }

	struct InitialState
	{
		Graphics4x4 placement;
		Vector3		velocity;
		Vector3		omega;
	};
	bool Initialize(CActor *ignore, const InitialState &initialState);

	// Get the updates
	void EndUpdate(float i_deltaSec);

	// Accept a collision.
	void CollisionAccept(CStageEntity* i_entity, CCollisionEvent* i_event);

private:
	// weee
	const DroppedWeaponParams	&d_params;

	// What are we?
	uint8						d_surfaceType;

	// When do we die?
	float						d_die;

	// We have to disable the ignore at somepoint
	uint32						d_clearIgnore;

	// fade stuff
	bool						d_alphaFading;
	float						d_alpha;

	// Sound stuff
	SoundHandle					d_impactSound;
};

#endif //_PAIN_STATS_H
