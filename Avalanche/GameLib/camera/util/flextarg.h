///////////////////////////////////////////////////////////////////////////////
// the flexible target W2V controller
///////////////////////////////////////////////////////////////////////////////
#ifndef FLEXTARGET_H
#define FLEXTARGET_H

#include "camera/world2view.h"

// Forwards
class CActor;

//////////////////////////////////////////////////////////////////////////
/////////////////////?/////////////////////////////////////////////////////
class FlexTargetW2VC : public WorldToViewController
{
public:
	FlexTargetW2VC();
	virtual ~FlexTargetW2VC() {}

	// Follow this actor
	// useFocus controls where the point comes from, 
	// if false it is GetBodyInWorld else it is GetFocusInWorld
	void SetTarget(CActor *actor, bool useFocus = false);

	// Keeps pointer...look out
	void SetTarget(const Vector3 *target);

	// Get the target
	CActor *GetActorTarget();
	const Vector3 *GetPointTarget();

	// No target
	void ClearTarget();

	// What type of target is there?
	enum Type
	{
		NONE,
		POINT,
		ACTORBODY,
		ACTORFOCUS
	};
	Type TargetType();

	// Set an offset to the actor
	void SetOffset(const Vector3 &offset);

	// Return the focal point
	virtual Vector3 FocalPoint();

	// Return an approximate up
	virtual Vector3 Up();

protected:
	Type					m_type;
	CActor				*m_actor;
	const Vector3		*m_point;
	Vector3				m_offset;
};

#endif




