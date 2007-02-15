/************************************/
/*                                  */
/* Ballistic.cpp                    */
/* gamelib  3/21/02                 */
/* ned martin  avalanche software   */
/* visual c++ 6.0                   */
/* math utility                     */
/*                                  */
/************************************/

/* a routine to help solve ballistic motion problems */

/******************* includes ****************************/

#include "mathUtil/MathUtilPCH.h"



/******************* defines *****************************/

/******************* externs *****************************/

/******************* global variables ********************/

/******************* local variables *********************/

/******************* local prototypes ********************/

/******************* functions ***************************/

/* start ballistic motion for moving through air. as long as x-velocity is same as starting x-velocity, will use
ballistic motion to calculate x-position (e.g. pos = start pos + (vel * elapsed time)). otherwise we'll use
standard pos = pos + (vel * delta sec). same with z. y-position and velocity will be calculated via ballistic
motion calculation. this removes frame rate sensitivity of jumping */

void CBallistic::Start(Vector3CRef vPos, Vector3& vVel, float fGravity)
{
	/* fill in ballistic record */

	d_vStartPos = vPos;
	d_vEndPos.Clear();
	d_vStartVel = vVel;
	d_fElapsedSeconds = 0.0f;
	d_fGravity = fGravity;
	d_bTargeted = false;	// air friction, joystick control ok here
	d_bOn = true;
	d_bWaiting = false;
}

/* given a starting and ending location, and a starting velocity, calculate velocity such that ballistic object will
land at ending location. this assumes that object's motion will not be frame-rate-sensitive. if object does not have
enough initial velocity to reach the target, the routine will leave the initial velocity unchanged and return false */

bool CBallistic::Start(Vector3CRef vPos, Vector3CRef vTarget, Vector3& vVel, float fGravity, int nCalcType /*= BALLISTIC_CALC_YVEL*/, bool bAllowRisingCollision /*= false*/)
{
	switch(nCalcType)
	{
	case BALLISTIC_CALC_YVEL:
		return(CalculateFromYVel(vPos, vTarget, vVel, fGravity, bAllowRisingCollision));
	case BALLISTIC_CALC_MAGNITUDE:
		return(CalculateFromMagnitude(vPos, vTarget, vVel, fGravity, false, bAllowRisingCollision));
	}

	/* unhandled case */

	ASSERT(false);
	return(false);
}

/* ballistic calculation using initial y-velocity to determine time of travel and correct x/z velocity. if bThrowToTarget
if true, the calculation will use the hit-while-still-rising solution if available */

bool CBallistic::CalculateFromYVel(Vector3CRef vPos, Vector3CRef vTarget, Vector3& vVel, float fGravity, bool bAllowRisingCollision /*= false*/)
{
	/* thanks to bryant for deriving the equation. ballistic motion is a parabola, which is a quadratic equation, */
	/* so we can use the general solution of a quadratic equation to calculate the times, t1 and t2, that the */
	/* ballistic path will cross the end height:

		t1 = (vy - sqrt((vy * vy) - 2gh)) / g
		t2 = (vy + sqrt((vy * vy) - 2gh)) / g

	/* where vy is initial velocity y-component, g is gravity, and h is (end ypos - start ypos). there are a couple */
	/* of extra cases to consider. if h < 0, the starting point is above the ending point, and there will only be */
	/* one solution-- t2. if ((vy * vy) - 2gh) is negative, the sqrt is imaginary, and t can't be found. this means */
	/* that the object will never reach the end height-- not enough upward velocity */

	float fValue = (vVel.y() * vVel.y()) - (2.0f * -fGravity * (vTarget.y() - vPos.y()));	// input gravity should typically be negative, but equation uses positive value
	if (fValue < 0.0f)
	{
		/* will never reach the end height, not enough upward velocity */

		d_bOn = false;
		return(false);
	}

	/* solve for t2. this is the time it will take the object to reach the target height while going down */

	fValue = Math::Sqrt(fValue);
	float fTime = (vVel.y() + fValue) / -fGravity;
	ASSERT(fTime > 0.0f);

	/* if target is above starting position, we may want to consider using t1 instead. this is good for calculating */
	/* a throw velocity to a target, since it gets there faster and more directly */

	if ((bAllowRisingCollision) && (vTarget.y() > vPos.y()))
	{
		fTime = (vVel.y() - fValue) / -fGravity;
		ASSERT(fTime > 0.0f);
	}

	/* now we can calculate the x- and z-velocities that will get us there in the right amount of time */

	vVel.x( (vTarget.x() - vPos.x()) / fTime);
	vVel.z( (vTarget.z() - vPos.z()) / fTime);

	/* fill in ballistic record */

	d_vStartPos = vPos;
	d_vEndPos = vTarget;
	d_vStartVel = vVel;
	d_fElapsedSeconds = 0.0f;
	d_fGravity = fGravity;
	d_bTargeted = true;			// ignore air friction/joystick control so calculations are valid
	d_fTargetSeconds = fTime;	// turn off once we reach target

	/* success */

	d_bOn = true;
	d_bWaiting = false;
	return(true);
}

/* ballistic calculation using initial velocity magnitude to determine time of travel and correct velocity. sort of
like aiming a cannon with a known muzzle velocity */

bool CBallistic::CalculateFromMagnitude(Vector3CRef vPos, Vector3CRef vTarget, Vector3& vVel, float fGravity, bool bYVelOnly /*= false*/, bool bAllowRisingCollision /*= false*/)
{
	/* math is fun! in this case, we want to take an initial velocity magnitude (e.g. muzzle velocity) and calculate */
	/* the up/down angle a to aim to hit the target. thanks to bryant again for helping. this can be reduced to */
	/* a 2d problem by looking at it from the side:

            __
             /|
           /
         /	 theta
    y0 +---------


                                                         + y1 (target)

       |----------------------- d -----------------------|

	/* we have:
	
			m = initial velocity magnitude
			theta = desired angle
			vx = initial horizontal velocity = m * cos(theta)
			vy = initial vertical velocity = m * sin(theta)
			y0 = initial y-position
			y1 = target y-position
			dy = delta y-position = y1 - y0
			d = horizontal distance to target
			t = time to reach target = d / vx
			g = gravity

			standard ballistic y-position equation is y = y0 + (vy * t) + (g * t^2 / 2)

			plugging in, we have:

				y1 = y0 + (vy * d / vx) + ((g * d^2) / (2 * vx^2))

			noting that vx^2 + vy^2 = m^2, and so vy = sqrt(m^2 - vx^2), we have:

				y1 = y0 + (sqrt(m^2 - vx^2) * d / vx) + ((g * d^2) / (2 * vx^2))

			or:

				dy = (sqrt(m^2 - vx^2) * d / vx) + ((g * d^2) / (2 * vx^2))

			multiply through by vx^2:

				(dy * vx^2) = sqrt(m^2 - vx^2) * d * vx) + (g * d^2 / 2)

			move things around:

				(dy * vx^2) - (g * d^2 / 2) = sqrt(m^2 - vx^2) * d * vx)

			square both sides:

				(dy^2 * vx^4) - (dy * g * d^2 * vx^2) + (g^2 * d^4 / 4) = (m^2 * d^2 * vx^2) - (d^2 * vx^4)

			combine similar terms:

				((dy^2 + d^2) * vx^4) + ((-(m^2 * d^2) - (dy * g * d^2)) * vx^2) + (g^2 * d^4 / 4) = 0
				
			note that this a quadratic equation of vx^2-- it has vx^4 abd vx^2 terms, but no vx^3 or vx, so we can use
			the general solution of a quadratic equation:
			
				given a * x^2 + b * x + c = 0, x = (-b +/- sqrt(b^2 - 4ac)) / 2a
			
			in this case:
			
				a = dy^2 + d^2
				b = (-(m^2 * d^2) - (dy * g * d^2))
				c = g^2 * d^4 / 4

			let's go... */

	float m = vVel.Length();
	float dy = vTarget.y() - vPos.y();
	float d = (vPos - vTarget).LengthXZ();
	float a = (dy * dy) + (d * d);
	float b = -(m * m * d * d) - (dy * fGravity * d * d);
	float c = fGravity * fGravity * d * d * d * d / 4.0f;

	/* get sqrt. if value under square root is negative, target can't be reached */

	float fValue = (b * b) - (4.0f * a * c);
	if (fValue < 0.0f)
	{
		/* target can't be reached */

		d_bOn = false;
		return(false);
	}

	/* calculate the 2 values for vx^2. there are two possible values: a low-trajectory throw and a high-trajectory one. */
	/* the only time there is only one solution is if the target is at the extreme range of the initial velocity, where */
	/* the angle will be 45 degrees */

	fValue = Math::Sqrt(fValue);
	float fVX[2];
	fVX[0] = (-b + fValue) / (2.0f * a);
	fVX[1] = (-b - fValue) / (2.0f * a);

	/* figure out which one is best-- whichever takes the shortest time */

	float fBestSeconds = 1000.0f;
	float fBestVX = 0.0f;
	for (int i = 0; i < 2; i++)
	{
		/* the equation was quadratic in vx^2, get back to vx */

		if (fVX[i] < 0.0f)
			continue;	// ignore impossible cases
		fVX[i] = Math::Sqrt(fVX[i]);

		/* remember, time = distance / vx */

		float fSeconds = d / fVX[i];	// time it will take to travel horizontal distance
		if (fSeconds < fBestSeconds)
		{
			fBestSeconds = fSeconds;
			fBestVX = fVX[i];
		}
	}

	/* if neither valid, we fail-- i don't think this can happen, but we'll play it safe */

	if (fBestSeconds == 1000.0f)
	{
		/* target can't be reached */

		d_bOn = false;
		return(false);
	}

	/* remember, vx = m * cos(theta). in this situation, -90 < theta < 90, so cos(theta) is always positive */

	float fCosTheta = fBestVX / m;
	ASSERT(fCosTheta < 1.1f);
	if (fCosTheta > 1.0f)
		fCosTheta = 1.0f;	// account for precision errors

	/* we can calculate sin(theta) from cos(theta), but we need to determine its sign. we can do this by */
	/* looking at the end y-pos for both positive and negative values and check which is closer */

	float fSinTheta = Math::Sqrt(1.0f - (fCosTheta * fCosTheta));

	/* first try positive */

	vVel.y(m * fSinTheta);
	float fYPos = vPos.y() + (vVel.y() * fBestSeconds) + (fGravity * fBestSeconds * fBestSeconds / 2.0f);
	float fYDelta = Math::Abs(fYPos - vTarget.y());

	/* then negative */

	vVel.y(-vVel.y());
	fYPos = vPos.y() + (vVel.y() * fBestSeconds) + (fGravity * fBestSeconds * fBestSeconds / 2.0f);

	/* which is better? */

	if (Math::Abs(fYPos - vTarget.y()) > fYDelta)
		vVel.y(-vVel.y());	// positive was better

	/* may only be interested in y-velocity result of this calculation */

	if (bYVelOnly)
	{
		vVel.x(0.0f);
		vVel.z(0.0f);
		return(true);
	}

	/* finally, use CalculateFromYVel to set final x/z velocity, and account for precision errors */

#ifdef _DEBUG
	bool bSuccess = CalculateFromYVel(vPos, vTarget, vVel, fGravity, bAllowRisingCollision);
	ASSERT(bSuccess);	// this had better succeed, since we got all the way here in magnitude calculation
	return(bSuccess);
#else //_DEBUG
	return(CalculateFromYVel(vPos, vTarget, vVel, fGravity, bAllowRisingCollision));
#endif //_DEBUG
}

/* given starting values and elapsed time, calculate new position and velocity */

void CBallistic::AttemptUpdate(Vector3 &vPos, Vector3 &vVel, float fFriction, float fDeltaSec)
{
	ASSERT(d_bOn);	// shouldn't be calling this unless contents are valid

	/* maybe holding */

	if (d_bWaiting)
		return;

	/* maybe ignore friction */

	if (d_bTargeted)
		fFriction = 0.0f;

	/* apply friction. friction is a force, like gravity */

	if (fFriction)
	{
		vVel.x(vVel.x() - vVel.x() * fFriction * fDeltaSec);
		vVel.z(vVel.z() - vVel.z() * fFriction * fDeltaSec);
	}

	/* we handle case where x- or z-velocity has changed from start value. in this case, we use */
	/* pos = pos + (vel * delta sec) instead of pos = start pos + (vel * elapsed time) */

	float fSeconds = d_fElapsedSeconds + fDeltaSec;
	if (vVel.x() != d_vStartVel.x())
		vPos.x(vPos.x() + (vVel.x() * fDeltaSec));
	else
		vPos.x( d_vStartPos.x() + (d_vStartVel.x() * fSeconds));
	if (vVel.z() != d_vStartVel.z())
		vPos.z(vPos.z() + (vVel.z() * fDeltaSec));
	else
		vPos.z( d_vStartPos.z() + (d_vStartVel.z() * fSeconds));

	/* y-motion is always ballistic */

	vVel.y( d_vStartVel.y() + (d_fGravity * fSeconds));
	vPos.y( d_vStartPos.y() + (d_vStartVel.y() * fSeconds) + (d_fGravity * fSeconds * fSeconds * 0.5f));

	/* if targeted, and we have been moving longer than calculated time, place at target position */

	if ((d_bTargeted) && (fSeconds >= d_fTargetSeconds))
	{
//		ASSERT(vVel.x() == d_vStartVel.x());	// if this hits, we're not going to reach the target-- may not be critical
//		ASSERT(vVel.z() == d_vStartVel.z());	// if this hits, we're not going to reach the target-- may not be critical
		vPos = d_vEndPos;
	}
}

/* accept update by updating elapsed time */

void CBallistic::EndUpdate(float fDeltaSec)
{
	/* maybe holding */

	if (d_bWaiting)
		return;

	/* update elapsed time */

	d_fElapsedSeconds += fDeltaSec;

	/* if we are targeted, turn off once we reach the target */

	if ((d_bTargeted) && (d_fElapsedSeconds >= d_fTargetSeconds))
		d_bOn = false;
}

/* what will position be after <n> seconds? */

Vector3 &CBallistic::Evaluate(float fSeconds)
{
	static Vector3 vPos;
	ASSERT(d_bOn);

	Vector3 temp;
	temp = d_vStartPos + d_vStartVel * fSeconds;

	vPos.Set(temp.x(),
				temp.y() + (d_fGravity * fSeconds * fSeconds * 0.5f),
				temp.z());
	return(vPos);
}

/* will ballistic path reach a specified height? */
/*** static function ***/

bool CBallistic::WillReachHeight(float fStartY, float fTargetY, float fVelY, float fGravity)
{
	/* if already above height, answer is obvious */

	if (fStartY >= fTargetY)
		return(true);

	/* use equation from CBallistic::Start */

	float fValue = (fVelY * fVelY) - (2.0f * -fGravity * (fTargetY - fStartY));	// input gravity should typically be negative, but equation uses positive value
	return(fValue >= 0.0f);
}

/* what will height be after a specified time? */
/*** static function ***/

float CBallistic::HeightAfterTime(float fStartY, float fVelY, float fGravity, float fSeconds)
{
	return(fStartY + (fVelY * fSeconds) + (fGravity * fSeconds * fSeconds * 0.5f));
}

/* what is minimum y-velocity to reach target height? */
/*** static function ***/

float CBallistic::YVelocityToReachHeight(float fStartY, float fTargetY, float fGravity)
{
	/* if target is below start, just return 0 */

	float fDeltaY = fTargetY - fStartY;
	if (fDeltaY <= 0.0f)
		return(0.0f);

	/* thanks to bryant for deriving the equation. ballistic motion is a parabola, which is a quadratic equation, */
	/* so we can use the general solution of a quadratic equation to calculate the times, t1 and t2, that the */
	/* ballistic path will cross the end height:

		t1 = (vy - sqrt((vy * vy) - 2gh)) / g
		t2 = (vy + sqrt((vy * vy) - 2gh)) / g

	/* where vy is initial velocity y-component, g is gravity, and h is (end ypos - start ypos). the minimum velocity */
	/* is where the target is reached right at the top of the ballistic parabola, or where t1 == t2:

		(vy - sqrt((vy * vy) - 2gh)) / g = (vy + sqrt((vy * vy) - 2gh)) / g

	/* this is clearly where vy * vy == 2gh, so that the sqrt term is 0. so:

		vy = sqrt(2gh)

	/* easy */

	return(Math::Sqrt(2.0f * -fGravity * fDeltaY));	// input gravity should typically be negative, but equation uses positive value
}


