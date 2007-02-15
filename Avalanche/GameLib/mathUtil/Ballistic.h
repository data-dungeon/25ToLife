/************************************/
/*                                  */
/* Ballistic.h                      */
/* gamelib  3/21/02                 */
/* ned martin  avalanche software   */
/* visual c++ 6.0                   */
/* math utility                     */
/*                                  */
/************************************/

/* a routine to help solve ballistic motion problems */

#ifndef __BALLISTIC_H
#define __BALLISTIC_H

#include "Math/Vector.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/******************* defines *****************************/

/* there are a couple of different initial velocity calculations to get to a target */

#define BALLISTIC_CALC_YVEL		0
#define BALLISTIC_CALC_MAGNITUDE	1

/******************* macros ******************************/

/******************* structures **************************/

/******************* global variables ********************/

/******************* global prototypes *******************/

/******************* class *******************************/

class CBallistic
{
// Constructor/destructor

public:

	CBallistic() { d_bOn = false; }
//	~CBallistic() {}	//djr: I don't know why, but if this empty destructor exists, then PS2 SN compiler DOESN'T call delete().  SO leave commented out till we can find out why...

// Implementation-- data

protected:

	bool		d_bOn;
	bool		d_bWaiting;				// don't advance until released by call to Go()
	bool		d_bTargeted;			// don't want friction or joystick control for when we are calculating x/z velocities to get to a specified target
	Vector3	d_vStartPos;
	Vector3	d_vStartVel;
	float		d_fGravity;				// should typically be negative
	float		d_fElapsedSeconds;
	float		d_fTargetSeconds;		// if targeted, ballistic motion will turn off once it reaches target (fElapsedSeconds >= fTargetSeconds)
	Vector3	d_vEndPos;				// if targeted, target position

// Implementation-- code

public:

	bool IsOn()							{ return(d_bOn); }
	void TurnOff()						{ d_bOn = false; }
	void Wait()							{ d_bWaiting = true; }
	void Go()							{ d_bWaiting = false; }
	bool IsTargeted()					{ return(d_bTargeted); }
	float GetElapsedSeconds(void)	{ return(d_fElapsedSeconds); }
	float GetTargetSeconds(void)	{ return(d_fTargetSeconds); }
	Vector3 &GetStartPos(void)		{ return(d_vStartPos); }
	Vector3 &GetStartVel(void)		{ return(d_vStartVel); }

	void Start(Vector3CRef vPos, Vector3& vVel, float fGravity);
	bool Start(Vector3CRef vPos, Vector3CRef vTarget, Vector3& vVel, float fGravity, int nCalcType = BALLISTIC_CALC_YVEL, bool bAllowRisingCollision = false);
	void AttemptUpdate(Vector3 &vPos, Vector3 &vVel, float fFriction, float fDeltaSec);
	void EndUpdate(float fDeltaSec);
	Vector3 &Evaluate(float fSeconds);

	/* helper routines that deal with ballistic motion */

	static bool WillReachHeight(float fStartY, float fTargetY, float fVelY, float fGravity);
	static float HeightAfterTime(float fStartY, float fVelY, float fGravity, float fSeconds);
	static float YVelocityToReachHeight(float fStartY, float fTargetY, float fGravity);

	/* calculation routines for Start to use */

	bool CalculateFromYVel(Vector3CRef vPos, Vector3CRef vTarget, Vector3& vVel, float fGravity, bool bAllowRisingCollision = false);
	bool CalculateFromMagnitude(Vector3CRef vPos, Vector3CRef vTarget, Vector3& vVel, float fGravity, bool bYVelOnly = false, bool bAllowRisingCollision = false);

// operations

public:

};



#ifdef __cplusplus
}
#endif //__cplusplus

#endif // __BALLISTIC_H
