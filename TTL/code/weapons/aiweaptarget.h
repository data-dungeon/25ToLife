/////////////////////////////////////////////////////////////////////////////
// AI Weapon target class!
/////////////////////////////////////////////////////////////////////////////
#ifndef AIWEAPTARGET_H
#define AIWEAPTARGET_H

#include "stage/cactor.h"

// The abstract base
class AIWeaponTarget
{
public:
	AIWeaponTarget() {}
	virtual ~AIWeaponTarget() {}

	// Get information about the target
	virtual Vector3 PositionInWorld() = 0;
	virtual Vector3 PositionInWorld(float deltaT) = 0;
	virtual Vector3 VelocityInWorld() = 0;

	// Simple versions
	virtual Vector3 PositionInWorldSimple()
		{ return PositionInWorld(); }
	virtual Vector3 PositionInWorldSimple(float deltaT)
		{ return PositionInWorld(deltaT); }

	// Called once per frame
	virtual void Update() {}
};

// A static target
class AIWeaponTargetPosition : public AIWeaponTarget
{
public:
	AIWeaponTargetPosition(const Vector3 &pos)
		{ Set(pos); }
	~AIWeaponTargetPosition() {}

	void Set(const Vector3 &pos)
		{ m_pos = pos; }

	// Get information about the target
 	Vector3 PositionInWorld()
		{ return	m_pos; }
	Vector3 PositionInWorld(float deltaT)
		{ return m_pos; }
	Vector3 VelocityInWorld()
		{ return Vector3(0.0f, 0.0f, 0.0f); }

private:
	Vector3	m_pos;
};

// An actor based target
class AIWeaponTargetActor : public AIWeaponTarget
{
public:
	// The constructors
	AIWeaponTargetActor(CActor &actor)
		{ m_actor = &actor; m_offset.Clear(); }
	AIWeaponTargetActor(CActor &actor, const Vector3 &offset)
		{ m_actor = &actor; m_offset = offset; }
	~AIWeaponTargetActor() {}

	void Set(CActor &actor)
		{ m_actor = &actor; m_offset.Clear(); }
	void Set(CActor &actor, const Vector3 &offset)
		{ m_actor = &actor; m_offset = offset; }

	// Get information about the target
 	Vector3 PositionInWorld()
		{ return	m_actor->GetBodyInWorld() + (m_offset * m_actor->GetBodyToWorld()); }
	Vector3 PositionInWorld(float deltaT)
		{ return PositionInWorld() + VelocityInWorld() * deltaT; }
	Vector3 VelocityInWorld()
		{ return m_actor->GetVelocityInWorld(); }

	// Simple versions
	Vector3 PositionInWorldSimple()
		{ return m_actor->GetBodyInWorld(); }
	Vector3 PositionInWorldSimple(float deltaT)
		{ return m_actor->GetBodyInWorld() + VelocityInWorld() * deltaT; }

private:
	CActor	*m_actor;
	Vector3 	m_offset;
};

// Need a bone based target, write one!

#endif
