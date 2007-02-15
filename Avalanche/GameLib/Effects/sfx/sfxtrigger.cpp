///////////////////////////////////////////////////////////////////////////////
// SpecialFX particle emitter!
///////////////////////////////////////////////////////////////////////////////
#include "Effects/EffectsPCH.h"

#include "Effects/sfx/sfxsys.h"
#include "Effects/sfx/sfxtrigger.h"
#include "stage/stage.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SFXTriggerBox::~SFXTriggerBox()
{
	SFXTriggerBoxActor *triggerActor = GetActor();
	if (triggerActor)
		triggerActor->MarkForDeath();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SFXTriggerBox *SFXTriggerBox::Create(
const char *message,
const Vector3 &pos,
const Vector3 &dimensions,
float ttl)
{
	// Extreme case
	if (!ValidSingleton(SpecialFXSystem))
		return false;

	// Create the object
	CSticky sticky;
	sticky.SetInWorld(NULL, pos);
	SFXTriggerBox *sfx = new SFXTriggerBox(g_sfx, sticky, message, dimensions, ttl);
	if (!sfx)
		return NULL;

	g_sfx.Add(sfx);
	return sfx;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SFXTriggerBox::SFXTriggerBox(
SpecialFXSystem &sfx,
const CSticky &sticky,
const char *message,
const Vector3 &dimensions,
float ttl) :
SpecialFXSticky(sfx, message, ttl)
{
	m_sticky = sticky;

	m_triggerBox = (ActorHandle)INVALID_OBJECT_HANDLE;
	SFXTriggerBoxActor *triggerActor = new SFXTriggerBoxActor;
	if (triggerActor)
	{
		// Gawd this is such a hack!
		ts_Geometry *geometry = g_loadSOM.Geometry("objects\\health_pkup.dbl");
		if (!geometry)
		{
			delete triggerActor;
			triggerActor = NULL;
		}
		else
		{
			triggerActor->SetGeometry(0, geometry);
			triggerActor->Initialize(message, Sticky().GetLocation(), dimensions);
			g_stage.AddProtagonist(triggerActor);
			triggerActor->SetDeleteWhenNoCast(true);
			m_triggerBox = triggerActor->GetHandle();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// Move it!
///////////////////////////////////////////////////////////////////////////////
void SFXTriggerBox::MoveTo(
const Vector3 &newPos)
{
	SFXTriggerBoxActor *triggerActor = GetActor();
	if (triggerActor)
		triggerActor->MoveTo(newPos);

	SpecialFXSticky::MoveTo(newPos);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool SFXTriggerBox::Inside(const Vector3 &pos)
{
	SFXTriggerBoxActor *triggerActor = GetActor();
	if (triggerActor)
		return triggerActor->Inside(pos);

	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SFXTriggerBoxActor::SFXTriggerBoxActor()
{
	m_message[0] = '\0';
	m_radius = 0.0f;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool SFXTriggerBoxActor::Initialize(
const char *message,
const Vector3 &pos,
const Vector3 &dimensions)
{
	CProtagonist::Initialize();
	Category() |= NO_SUPPORT_CATEGORY | FIXED_CATEGORY;
	SetPositiveInclude(PLAYER_CATEGORY | AI_CATEGORY | DEBUG_DRAW_CATEGORY);
	d_interactionHint = HINT_IMPOTENT | HINT_JUGGERNAUT;

	// Save the message
	ASSERT(strlen(message) < sizeof(m_message));
	strcpy(m_message, message);

	// Setup the stuff
	DirCos3x3 o;
	o.Identity();
	Place(pos, o);

	// Make an obb!
	// Setup collision
	SetCollisionNode(&m_obbNode, SIMPLE, false);
	m_obbNode.SetExtents(-dimensions, dimensions);
	m_radius = dimensions.Length();

	// we'll never render, so remove from scene tree
	SceneMgr::RemoveInstance(Instance(0));
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SFXTriggerBoxActor::CollisionAccept(
CStageEntity* i_entity,
CCollisionEvent* i_event)
{
	// Anything to do?
	if (m_message[0] != '\0')
	{
		ASSERT(i_entity->IsA(CActor::ClassType()));
		CActor *actor = (CActor *)i_entity;
		float d = (GetBodyInWorld() - actor->GetBodyInWorld()).Length();
		float intensity = 1.0f - Math::Clamp(d / m_radius, 0.0f, 0.9f);
		g_messageDispatcher.SendMessageToActor(m_message,
																g_messageDispatcher.FloatToData(intensity),
																GetHandle(), actor->GetHandle());
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SFXTriggerBoxActor::MoveTo(
const Vector3 &newPos)
{
	// Setup the stuff
	DirCos3x3 o;
	o.Identity();
	Place(newPos, o);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool SFXTriggerBoxActor::Inside(
const Vector3 &pos)
{
	OBBInstance *obb = (OBBInstance * )m_obbNode.GetCollisionObject();
	ASSERT(obb);
	if (obb)
		return obb->PointInBox(pos);

	return false;
}

