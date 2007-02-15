///////////////////////////////////////////////////////////////////////////////
// SpecialFX trigger box
///////////////////////////////////////////////////////////////////////////////
#ifndef SFXTRIGGER_H
#define SFXTRIGGER_H

#include "Effects/sfx/specialfx.h"
#include "CollideHelper/ObbNode.h"

class SFXTriggerBoxActor;

class SFXTriggerBox : public SpecialFXSticky
{
public:
	// Auto destructs after ttl has expired
	SFXTriggerBox(SpecialFXSystem &sfx,
						const CSticky &sticky,
						const char *message,
						const Vector3 &dimensions,
						float ttl);
	~SFXTriggerBox();

	// give back the subclass
	const char *Class()
		{ return "trigger"; }

	// Move it!
	void MoveTo(const Vector3 &newPos);

	// Is this point inside of us?
	bool Inside(const Vector3 &pos);

	// are we still active?
	bool Active()
		{ return (GetActor() != NULL); }

	// All in one create and add
	static SFXTriggerBox *Create(const char *message,
											const Vector3 &pos,
											const Vector3 &dimensions,
											float ttl);

protected:
	// Our update!
	void Update(float now, float deltaT) {}

	// our stuff
	ActorHandle				m_triggerBox;
	SFXTriggerBoxActor *GetActor()
		{ return (SFXTriggerBoxActor *)CProtagonist::FromHandle(m_triggerBox); }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SFXTriggerBoxActor : public CProtagonist
{
public:
	SFXTriggerBoxActor();
	~SFXTriggerBoxActor() {}

	bool Initialize(const char *message, const Vector3 &pos, const Vector3 &dimensions);
	virtual void CollisionAccept(CStageEntity* i_entity, CCollisionEvent* i_event);

	void MoveTo(const Vector3 &newPos);
	bool Inside(const Vector3 &pos);

private:
	char 			m_message[32];
	CObbNode		m_obbNode;
	float			m_radius;
};


#endif

