//============================================================================
//=
//= InstantMover.cpp - A component to handle rudamentary things such as rotating,
//=                  scaling, point to point movement, etc.
//=
//============================================================================

#include "Components/ComponentsPCH.h"
#include "Components/ComponentCreator.h"
#include "Components/InstantMover.h"
#include "mathUtil/Interpolate.h"

#include "Script/ScriptSystem.h"

// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(CInstantMover, InstantMover, COMPONENT_NEUTRAL_PRIORITY);

#define TRANSLATE_X         0x01
#define TRANSLATE_Y         0x02
#define TRANSLATE_Z         0x04
#define ROTATE_X            0x08
#define ROTATE_Y            0x10
#define ROTATE_Z            0x20
#define TRANSLATE_VELOCITY  0x40
#define ROTATE_VELOCITY     0x80

//============================================================================

CInstantMover::CInstantMover(CCompActor& i_actor) : CActorComponent(i_actor)
{
	d_flags = 0;
}

//============================================================================

CInstantMover::~CInstantMover()
{
}

//============================================================================

CInterface* CInstantMover::GetInterface(uint i_id)
{
	if (i_id == CBasicMoverIF::GetID())
		return((CInterface*)this);
	return(NULL);
}

//============================================================================

void CInstantMover::BeginUpdate(void)
{
	d_beginBodyInWorld = actor.GetBodyInWorld();
	d_beginBodyToWorld = actor.GetBodyToWorld();
}

//============================================================================

void CInstantMover::AttemptUpdate(float i_deltaSec)
{
	if ((d_flags & TRANSLATE_X) != 0)
		actor.GetBodyInWorld().X(d_bodyInWorld.X());

	if ((d_flags & TRANSLATE_Y) != 0)
		actor.GetBodyInWorld().Y(d_bodyInWorld.Y());

	if ((d_flags & TRANSLATE_Z) != 0)
		actor.GetBodyInWorld().Z(d_bodyInWorld.Z());

	if ((d_flags & (ROTATE_X | ROTATE_Y | ROTATE_Z)) != 0)
	{
		Vector3 euler;
		euler.X(Math::ArcSin(-d_beginBodyToWorld.e21));
		euler.Y(Math::ArcTan2(d_beginBodyToWorld.e20,
				d_beginBodyToWorld.e22));
		euler.Z(Math::ArcTan2(d_beginBodyToWorld.e01,
				d_beginBodyToWorld.e11));

		if ((d_flags & ROTATE_X) != 0)
			euler.X(d_euler.X());

		if ((d_flags & ROTATE_Y) != 0)
			euler.Y(d_euler.Y());

		if ((d_flags & ROTATE_Z) != 0)
			euler.Z(d_euler.Z());

		actor.GetBodyToWorld().SetZRotation(euler.Z());
		actor.GetBodyToWorld().CatXRotation(-euler.X());
		actor.GetBodyToWorld().CatYRotation(euler.Y());
	}

	if ((d_flags & TRANSLATE_VELOCITY) != 0)
		actor.GetVelocityInWorld() = d_velocityInWorld;

	if ((d_flags & ROTATE_VELOCITY) != 0)
		actor.GetAngVelocityInBody() = d_omegaInBody;
}

//============================================================================

void CInstantMover::EndUpdate(float i_deltaSec)
{
	if (d_flags != 0)
	{
		actor.Placed();
		d_flags = 0;
	}
}

//============================================================================

void CInstantMover::MoveTo(Vector3CRef pos, float duration)
{
	d_bodyInWorld = pos;
	d_flags |= TRANSLATE_X | TRANSLATE_Y | TRANSLATE_Z;
}

//============================================================================

void CInstantMover::MoveToX(float pos, float duration)
{
	d_bodyInWorld.X(pos);
	d_flags |= TRANSLATE_X;
}

//============================================================================

void CInstantMover::MoveToY(float pos, float duration)
{
	d_bodyInWorld.Y(pos);
	d_flags |= TRANSLATE_Y;
}

//============================================================================

void CInstantMover::MoveToZ(float pos, float duration)
{
	d_bodyInWorld.Z(pos);
	d_flags |= TRANSLATE_Z;
}

//============================================================================

void CInstantMover::MoveRel(Vector3CRef pos, float duration)
{
	Vector3 world = (pos * d_beginBodyToWorld) + d_beginBodyInWorld;
	MoveTo(world, duration);
}

//============================================================================

void CInstantMover::MoveRelX(float pos, float duration)
{
	Vector3 rel = Vector3(pos, 0.0f, 0.0f);
	Vector3 world = (rel * d_beginBodyToWorld) + d_beginBodyInWorld;
	MoveTo(world, duration);
}

//============================================================================

void CInstantMover::MoveRelY(float pos, float duration)
{
	Vector3 rel = Vector3(0.0f, pos, 0.0f);
	Vector3 world = (rel * d_beginBodyToWorld) + d_beginBodyInWorld;
	MoveTo(world, duration);
}

//============================================================================

void CInstantMover::MoveRelZ(float pos, float duration)
{
	Vector3 rel = Vector3(0.0f, 0.0f, pos);
	Vector3 world = (rel * d_beginBodyToWorld) + d_beginBodyInWorld;
	MoveTo(world, duration);
}

//============================================================================

void CInstantMover::RotateTo(Vector3CRef rot, float duration)
{
	d_euler = rot;
	d_flags |= ROTATE_X | ROTATE_Y | ROTATE_Z;
}

//============================================================================

void CInstantMover::RotateToX(float rot, float duration)
{
	d_euler.X(rot);
	d_flags |= ROTATE_X;
}

//============================================================================

void CInstantMover::RotateToY(float rot, float duration)
{
	d_euler.Y(rot);
	d_flags |= ROTATE_Y;
}

//============================================================================

void CInstantMover::RotateToZ(float rot, float duration)
{
	d_euler.Z(rot);
	d_flags |= ROTATE_Z;
}

//============================================================================

void CInstantMover::RotateRel(Vector3CRef rot, float duration)
{
	d_euler.X(Math::ArcSin(-d_beginBodyToWorld.e21));
	d_euler.Y(Math::ArcTan2(d_beginBodyToWorld.e20,
			d_beginBodyToWorld.e22));
	d_euler.Z(Math::ArcTan2(d_beginBodyToWorld.e01,
			d_beginBodyToWorld.e11));
	d_euler += rot;
	d_flags |= ROTATE_X | ROTATE_Y | ROTATE_Z;
}

//============================================================================

void CInstantMover::RotateRelX(float rot, float duration)
{
	d_euler.X(Math::ArcSin(-d_beginBodyToWorld.e21) + rot);
	d_flags |= ROTATE_X;
}

//============================================================================

void CInstantMover::RotateRelY(float rot, float duration)
{
	d_euler.Y(Math::ArcTan2(d_beginBodyToWorld.e20,
			d_beginBodyToWorld.e22) + rot);
	d_flags |= ROTATE_Y;
}	

//============================================================================

void CInstantMover::RotateRelZ(float rot, float duration)
{
	d_euler.Z(Math::ArcTan2(d_beginBodyToWorld.e01,
			d_beginBodyToWorld.e11) + rot);
	d_flags |= ROTATE_Z;
}

//============================================================================

void CInstantMover::MoveAtConstantVelocity(Vector3CRef vel)
{
	d_velocityInWorld = vel;
	d_flags |= TRANSLATE_VELOCITY;
}

//============================================================================

void CInstantMover::RotateAtConstantVelocity(Vector3CRef vel)
{
	d_omegaInBody = vel;
	d_flags |= ROTATE_VELOCITY;
}

//============================================================================
