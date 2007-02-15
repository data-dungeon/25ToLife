#ifndef __KINECONTROL_H_INCLUDED__
#define __KINECONTROL_H_INCLUDED__

/*************************************************

Kinematic Controller and Kinematic Driver Base Abstract Classes
----------------------------------------

A Kinematic Controller defines how an actor moves around.  Each individual type of
movement requires a separate controller, for example, BipedKineControl vs TankKineControl.

The kinematic controller does no controller polling on its own.  It should be fed by a
KinematicDriver, which may read controls directly, or may be a collection of higher-level
functions that "fake" controller input for the KinematicController.

KinematicController is loosely based on (and should replace) Bryant's locomotion modes.  I'm trying 
to make the concept more generalized, and applicable to all actors, not just avatars.  In addition, 
I wanted to create a concrete location for all the types of things that environmental actors might 
want to do as larger steps than just feeding control stick positions.  So, instead of the actor 
class containing things like RotateToAngle, those functions, wherever necessary, could be contained 
in a specialized kinematic driver.

Due to those changes, there are some fairly large conceptual differences, which is why I've created 
a new class instead of just modifying the locomotion mode classes.  In addition, there is really no
equivalent to the locomotion itself.  Each KinematicController is basically the equivalent of a "mode",
and there are no sub-modes.  In looking through the different locomotion types, I didn't really
see anything there that couldn't be handled just as well by activating (or deactivating) a component, 
so I decided not to add the extra complexity by having an extra layer of the top, and if some sort of
sub-mode is necessary later on, it shouldn't be too hard to add.

KinematicController also encompasses the funcionality previously contained in CAvatarControl, providing a
"logical" controller base with axes and actions.  For example, a BipedKinematicController might 
have the following axes and actions:

Axis_Strafe, Axis_Move, Axis_Turn, Axis_Pitch
Action_Shoot, Action_Jump, Action_Crouch

While a vehicle might have the following:
Axis_Move, Axis_Turn, Axis_TurretRotate, Axis_TurretPitch
Action_Shoot

I am combining the functionality of the locomotion and avatarcontrol classes primarily because 
it doesn't seem that there would ever be any good reason to divide the two.  If you have a logical 
controller that can do certain actor-type-specific actions, then there's no reason to divide that 
from code that actually performs those specific actions.

A KinematicController MAY have a KinematicDriver feeding it, or it may be fed through whatever
means the actor containing it desires.  The intended usage for KineControl and Driver is that when
an actor is created, it creates (and contains) a KinematicController.  If the actor wants to attach
a KinematicDriver, it may do so.  For example, an actor intended to be controlled
by a player would attach a KinematicDriver that read the controller stick assigned to that player.
Or, if it wanted an environmental driver that had higher level functions such as MoveToPos
and RotateToAngle, it could attach that driver instead.  Or it could attach no driver at all, and
handle updating the KinematicController itself through some other method.  Note that although the
functionality of KinematicController and KinematicDriver are intimately linked (a driver written for
a biped controller would not work on a vehicle controller), they are separated because it is very
reasonable to believe that you might want different types of drivers to apply to the same controller.
At the very least, you'd want the ability to attach a physical controller to a kinematic one.

Typically, you will want to define a kinematic controller and all the applicable driver types in 
the same file.  A driver that takes raw joystick controls and converts them to a biped kinematic 
controller logical controls won't be of much use to any other kinematic controller, so it makes
sense to keep the all the inter-related information local.

Note that there will very likely be platform-specific information and mapping in kinematic drivers
that read and translate physical controller input.  The only way to avoid that would be to try
to abstract the controller another level, which is what was attempted in Juju, but since the 
controllers have differing numbers of buttons, there's not really a good reason to do this.  At some
point you HAVE to translate from physical controls to logical controls, so there might as well only
be one translation instead of two.  Doing it this way also gives you much more flexibility in defining
how the real controls map to the logical ones.

The KinematicDriver would also be the logical place to perform any sort of user-defined remapping,
as well.  Since the KinematicController axes and actions are all defined by enums, a simple look-up
table should suffice.

NOTE: KinematicDriver is no longer a distinct class of its own.  It didn't add anything that 
CActorComponent didn't already add, so drivers should be derived directly from CActorComponent.
Drivers should be registered with a lower priority than their controllers, so that they get run
first.
**************************************************/

#include "Components/Component.h"

class CCompActor;
class CStageEntity;
class CCollisionEvent;
class CCollisionNode;

class AxisRange
{
public:
	AxisRange( float i_max = 1.0f );
	AxisRange( float i_min, float i_max, bool i_centerAtZero = true );
	float DeNormalize( float f );
	float Min( void ) { if( mirrored ) return -Max(); return min; }
	float Max( void ) { return max; }
	float Clamp( float in ) { return Math::Clamp( in, min, max ); }

	// these need to be public so they can be hooked to console inputs.
	float min;
	float max;

private:
	bool mirrored;
	bool centerAtZero;
};

class KinematicController : public CActorComponent
{
public:
	KinematicController(CCompActor &i_actor) : CActorComponent(i_actor) {}
	virtual ~KinematicController() {}

	virtual int Name( void ) { return 0; }

	// SetAxisNormalized should take a position from -1.0 to 1.0 and 
	// normalize it into the range, while SetAxis should set the axis
	// position directly, capping it to the range.  Normally you won't
	// need to override SetAxisNormalized, since the KinematicController
	// version does the normalization on its own
	virtual float GetAxis( int axis ) { return 0.0f; }
	virtual void SetAxis( int axis, float position ) {}
	virtual void SetAxisNormalized( int axis, float position );
	virtual AxisRange GetAxisRange( int axis ) { return AxisRange(); }
	virtual void SetAxisRange( int axis, const AxisRange &range ) {}
	virtual void StartAction( int action ) {};
	virtual void EndAction( int action ) {};
	virtual bool ActionInProgress( int action ) { return false; }

	virtual void VelocityDumped( Vector3CRef velChange ) {};

protected:
	int mode;
};

#endif //__KINECONTROL_H_INCLUDED__
