///////////////////////////////////////////////////////////////////////////////
// Targeting reticle
///////////////////////////////////////////////////////////////////////////////
#ifndef IRETICLE_H
#define IRETICLE_H

#include "hud\ipanel\isprite.h"

class TTLWeapon;

class ReticlePip
{
public:
	ReticlePip(Sprite &parent, t_Handle dblHandle, int &baseTexture);
	virtual ~ReticlePip() {}

	// Set the min/max offsets
	void SetMinMaxOffset(float min, float max)
		{ m_minOffset = min; m_maxOffset = max; }

	void SetSqueeze(float min, float max)
		{ m_minSqueeze = min; m_maxSqueeze = max; }

	// You write these!
	virtual void Hide(float dt) = 0;
	virtual void Update(TTLWeapon *weapon, const Vector4 &color, float dt, float scale) = 0;

protected:
	float		m_minOffset, m_maxOffset;
	float		m_minSqueeze, m_maxSqueeze;

	float ComputeOffset(TTLWeapon *weapon);
};

///////////////////////////////////////
///////////////////////////////////////
class ReticleInstrument : public SpriteInstrument
{
public:
	// Constructor
	ReticleInstrument();
	~ReticleInstrument();

	// Our extents
	Vector2 Extents()
		{ return Vector2(0.0f, 0.0f); }

	// The update (called once per frame)
	void Update(float dt);

	// Register a hit
	void RegisterHit();

private:
	// The reticles we have loaded
	enum ReticleType
	{
		BOTHMELEE = 0,
		COPNORMAL,
		COPSHOTGUN,
		GANGNORMAL,
		GANGSHOTGUN,
		RETICLETYPES
	};
	ReticlePip			*m_pip[RETICLETYPES];
	void UpdatePips(CCompActor *me, TTLWeapon *weapon, float dt, float scale);
	bool IsGang(CCompActor *who);

	// The hit indicator
	Sprite				m_hit[4];
	float					m_hitStrength;
	static Vector2		s_hitDir[4];

	// Get the current weapon
	TTLWeapon *GetCurrentWeapon(CCompActor *actor);
	bool IsShotgun(TTLWeapon *weapon);
	bool IsMelee(TTLWeapon *weapon);

	// Handle targeting
 	enum ReticleColor
	{
		TARGET_FRIENDLY = 0,
		TARGET_ENEMY,
		TARGET_NEUTRAL,
		TARGET_NONE
	};
	ReticleColor HandleTarget(CCompActor *actor, TTLWeapon *weapon, bool &onHead);
	Vector4 GetPipColor(ReticleColor color);
	Vector4				m_currentColor;
	bool					m_onHead;
	float					m_scale, m_scaleTarget;
};

///////////////////////////////////////
///////////////////////////////////////
class ReticlePipSimple: public ReticlePip
{
public:
	ReticlePipSimple(Sprite &parent, t_Handle dblHandle, int &baseTexture, float scale = 1.0f);
	~ReticlePipSimple() {}

	// You write these!
	void Hide(float dt);
	void Update(TTLWeapon *weapon, const Vector4 &color, float dt, float scale);

private:
	Sprite				m_dot;
	float					m_baseScale;
};

///////////////////////////////////////
///////////////////////////////////////
class ReticlePipPlus : public ReticlePip
{
public:
	ReticlePipPlus(Sprite &parent, t_Handle dblHandle, int &baseTexture);
	~ReticlePipPlus() {}

	// You write these!
	void Hide(float dt);
	void Update(TTLWeapon *weapon, const Vector4 &color, float dt, float scale);

private:
	Sprite				m_dot;
	Sprite				m_pip[4];

	static Vector2 	s_pipDir[4];
};

///////////////////////////////////////
///////////////////////////////////////
class ReticlePipX : public ReticlePip
{
public:
	ReticlePipX(Sprite &parent, t_Handle dblHandle, int &baseTexture);
	~ReticlePipX() {}

	// You write these!
	void Hide(float dt);
	void Update(TTLWeapon *weapon, const Vector4 &color, float dt, float scale);

private:
	Sprite				m_dot;
	Sprite				m_pip[4];

	static Vector2 	s_pipDir[4];
};

///////////////////////////////////////
///////////////////////////////////////
class ReticlePipScale: public ReticlePip
{
public:
	ReticlePipScale(Sprite &parent, t_Handle dblHandle, int &baseTexture, int circle);
	~ReticlePipScale() {}

	void SetScale(float min, float max)
		{ m_minScale = min; m_maxScale = max; }

	// You write these!
	void Hide(float dt);
	void Update(TTLWeapon *weapon, const Vector4 &color, float dt, float scale);

private:
	float					m_minScale, m_maxScale;
	Sprite				m_inner;
	Sprite				m_outer;

	float ComputeScale(TTLWeapon *weapon);
};

#endif
