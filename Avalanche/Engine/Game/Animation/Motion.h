/* --------------------------------------------------
 * File    : g_Motion.h
 * Created : Thu Mar  4 16:09:51 1999
 * Descript:
 * --------------------------------------------------*/
#ifndef __G_MOTION_H
#define __G_MOTION_H

/******************* includes *****************************/

/* system includes */

/* engine includes */

#include "Game/Animation/Bone.h"	// needed for BoneGroupID, BoneID typedefs
#include "Game/Animation/MotCtrl.h"

#ifdef GCN
#include "platform/GCN/GCNMWPort.h"
#endif	//GCN

/******************* forward declarations ****************/

struct ts_MotionHeader;
class CActorInstance;
class MotionChannel;

/******************* defines *****************************/

#define MAX_SHORT (32767)
#define MIN_SHORT (-32768)
#define MAX_USHORT (65535)

#define JIFFIES_PER_SECOND (300.0f)	//arbitrary...what's best?   system clock read in sec/1000.  PAL/NTSC common sec/300.  can't animate faster than sec/60 (except on an RGB monitor)
#define SECONDS_PER_JIFFY (1.0f/JIFFIES_PER_SECOND)
#define ANIMFRAMES_PER_SECOND	(30.0f)	
#define SECONDS_PER_ANIMFRAME	(1.0f/30.0f)	
#define JIFFIES_PER_ANIMFRAME	(JIFFIES_PER_SECOND / ANIMFRAMES_PER_SECOND)
#define ANIMFRAMES_PER_JIFFY (ANIMFRAMES_PER_SECOND / JIFFIES_PER_SECOND)
#define JIFFIES_PER_MILLISECOND (JIFFIES_PER_SECOND / 1000.0f)
#define SECONDS_TO_ANIMFRAMES(seconds) ((int)(seconds * ANIMFRAMES_PER_SECOND + 0.5f))	//round to the nearest frame
#define ANIMFRAMES_TO_SECONDS(frames) (((float)frames) * SECONDS_PER_ANIMFRAME)

#define MAX_JIFFIES (MAX_USHORT)	//max value a unsigned short can have 
#define SECONDS_TO_JIFFIES(seconds) ((int)(seconds * JIFFIES_PER_SECOND + 0.5f))	//round to the nearest jiffy
#define JIFFIES_TO_SECONDS(jiffies) (((float)jiffies) * SECONDS_PER_JIFFY)
#define MAX_INTERP_JIFFIES (MAX_SHORT)	//max value a short can have 

// the Motion Flags
#include "Game/Animation/MotnFlags.h"

/******************* macros ******************************/

/******************* structures **************************/

//#define BLEND_DENOMINATOR	(32767)	//blend ratio denominator (max short)
struct ts_MotnInf
{
	float	currentTime;		// Current time in current motion, starting from 0 at the beginning of the anim
	float	interpTime;			// if interpolating, positive value -> time left to interpolate over, zero or neg -> interpolating done
	float	blendRatio;			// if blending, the ratio to blend this motion into the existing motion
	float	animationRate;		// anim rate
	u32					u32MotnFlags;
	ts_MotionHeader*	pMotionHeader;			// Pointer to current motion
	ts_MotionHeader*	pNextMotionHeader;	// Pointer to next motion - the motion we're interpolating into
	CActorInstance*	inst;						// instance this motion is controlling
	MotionChannel*		currentChannel;		// active channel this is associated with
};

/******************* global variables ********************/


/******************* global prototypes *******************/

void CMotion__Animate(void *pContext, ts_Body *pBody, BoneGroupID boneGroupID, ts_MotnInf *pMotnInf, float secondsSinceLastUpdate);
void CMotion__UpdateSlavePositions(CActorInstance* master, CActorInstance* slave, Graphics4x4 &m);

bool CMotion__IsMotionDone( ts_MotnInf *pMotnInf );
bool CMotion__IsInterpDone( ts_MotnInf *pMotnInf );
bool CMotion__IsPaused( ts_MotnInf *pMotnInf );
void CMotion__Restart( ts_MotnInf *pMotnInf );
void CMotion__StartInterpolation( ts_MotnInf *pMotnInf, unsigned short usFrames );
void CMotion__AdvanceTimers(  ts_MotnInf *pMotnInf, float seconds );
void CMotion__SetCurrentFrame( ts_MotnInf *pMotnInf, unsigned short frame);
void CMotion__SetCurrentTime( ts_MotnInf *pMotnInf, float seconds);
u16 CMotion__GetCurrentFrame( ts_MotnInf *pMotnInf);
float CMotion__GetCurrentTime( ts_MotnInf *pMotnInf);
float CMotion__GetCurrentCompletion( ts_MotnInf *pMotnInf);
float CMotion__GetAnimTime( ts_MotionHeader *pMotionHeader );
u16 CMotion__GetAnimFrames( ts_MotionHeader *pMotionHeader );
void CMotion__SetProceduralPosition( ts_MotnInf *pMotnInf, float zeroToOne );
void CMotion__SetAnimationRate( ts_MotnInf *pMotnInf, float animRate );
float CMotion__GetAnimationRate( ts_MotnInf *pMotnInf);
float CMotion__ScaleTime( ts_MotnInf *pMotnInf, float seconds );
void CMotion__Restart( ts_MotnInf *pMotnInf );
void CMotion__Pause( ts_MotnInf *pMotnInf );

void CMotion__SetBlendRatio( ts_MotnInf *pMotnInf, float blendRatio );
float CMotion__GetBlendRatio( ts_MotnInf *pMotnInf);


void motKFMKeyFrames(ts_MotionHeader *pMotion, ts_Body *pBody, float currentTime, BoneGroupID boneGroupID);
void motInterpKFMKeyFrames(ts_MotionHeader *pMotion, ts_Body *pBody, float currentTime, BoneGroupID boneGroupID);

#endif __G_MOTION_H
