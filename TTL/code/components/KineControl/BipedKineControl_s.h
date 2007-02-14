#ifndef BIPED_KINE_CONTROL_H
#define BIPED_KINE_CONTROL_H

enum
{
	Biped_Axis_Strafe = 0,
	Biped_Axis_Move,
	Biped_Axis_Turn,
	Biped_Axis_TurnRate,
	Biped_Axis_Count
};

enum
{
	Biped_Action_Shoot = 0,
	Biped_Action_Crouch,
	Biped_Action_Jump,
	Biped_Action_Die,
	Biped_Action_Aim,
	Biped_Action_Kick,
	Biped_Action_HandsUp,
	Biped_Action_Cower,
	Biped_Action_Count
};

enum
{
	Ladder_Axis_Climb = 0,
	Ladder_Axis_Count
};

enum
{
	Ladder_Action_Slide = 0,
	Ladder_Action_Count
};

enum
{
	LadderDismount_Use = 0,
	LadderDismount_Jump,
	LadderDismount_AtTop,
	LadderDismount_AtBottom,
	LadderDismount_SlideLand
};


enum
{
	Footprint_Left = 0,
	Footprint_Right
};

enum
{
	View_Axis_CameraTurnRate,
	View_Axis_CameraTurnRateTarget,
	View_Axis_CameraPitch,
	View_Axis_CameraPitchRate,
	View_Axis_CameraPitchRateTarget,
	View_Axis_Count
};

#ifndef __cplusplus
extern int OnGround();
extern void Footprint( int foot );
extern int BipedLaunch( float x, float y, float z );
extern void BipedKick();
extern int Aiming();
extern int Crouching();
extern int Jumping();
extern int CurrentWeapon();
extern int CurrentWeaponReloadType();
extern void StartAim();
extern void StopAim();
extern void StartCrouch();
extern void StopCrouch();
extern void StartTagging();
extern void StopTagging();
#endif

#endif // BIPED_KINE_CONTROL_H
