//
// MotnFlags.h
//
// Flags for ts_MotnInf.u32MotnFlags
// This file is included in the engine and in animscripts (so nothing but #defines!!).
//

// Motion flags 
#define MOTNINF_FLAG_NONE					0x00000000	//motion plays through to end and stops at last frame
#define MOTNINF_FLAG_PAUSE					0x00000001	//this motion is currently paused
#define MOTNINF_FLAG_INTERP				0x00000002	//this motion is interpolating from previous motion
#define MOTNINF_FLAG_ONESHOT				0x00000004	//end animation upon completion, otherwise assumed to "hold" on the last frame
#define MOTNINF_FLAG_MATRIXINTERP		0x00000008	//for the frames of this interpolation, assume that the pMFrames array is not valid, so compute from the bone's local matrix
#define MOTNINF_FLAG_MIRROR				0x00000010	//this animation is to be mirrored: left channels to right bones and vice versa.
#define MOTNINF_FLAG_NOINTERP				0x00000020	//this motion is to play immediately, don't interpolate into this motion
#define MOTNINF_FLAG_LOOP					0x00000040	//this motion is to loop to the beginning upon reaching the end (overrides ONESHOT)
#define MOTNINF_FLAG_BLENDINTERP			0x00000080	//allow the animation timer to advance while interpolating to new motion
#define MOTNINF_FLAG_MOTION2				0x00000100	//this motion is to play as a second, concurrent motion
#define MOTNINF_FLAG_PRIORITY_LOW		0x00000200	//this motion is to play with a lower priority
#define MOTNINF_FLAG_PRIORITY_HIGH		0x00000400	//this motion is to play with a higher priority
#define MOTNINF_FLAG_PROCEDURAL			0x00000800	//the app sets the motion timer - timers NOT automatically updated
#define MOTNINF_FLAG_BLEND					0x00001000	//this motion is to be blended into any existing motion
#define MOTNINF_FLAG_ENSURELASTFRAME	0x00002000	//when the motion timer advances past the end of this motion, make sure that the last frame is displayed (ie, don't step over last frame)
#define MOTNINF_FLAG_INTERPIFNOTSAME	0x00004000	//when the motion timer advances past the end of this motion, make sure that the last frame is displayed (ie, don't step over last frame)
#define MOTNINF_FLAG_NEW_UPDATE			0x00008000	//this motion was NOT applied last update and is being applied this update
#define MOTNINF_FLAG_MOTION3				0x00010000	//additional concurrent motion, for MotnInf[MOTION3]
#define MOTNINF_FLAG_MOTION4				0x00020000	//additional concurrent motion, for MotnInf[MOTION4]
#define MOTNINF_FLAG_IFNOTSAME         0x00040000	//starts animation only if it's not the currently playing animation

// PAB - added for driving multiple skeletons
#define MOTNINF_FLAG_PRIMARY_SKELETON		0x00080000	//applies motion to our primary skeleton (default)
#define MOTNINF_FLAG_SECONDARY_SKELETON	0x00100000	//applies motion to our secondary skeleton 
#define MOTNINF_FLAG_TERTIARY_SKELETON		0x00200000	//applies motion to our tertiary skeleton 

#define MOTNINF_FLAG_BLENDFULL				0x00400000   // additive animation blending.  Add this channel in addition to what's there already

/* a one-shot anim yields control of the motion channel when it finishes-- this is useful if you have a lower priority
motion channel waiting to re-take control after the animation finishes. MOTNINF_FLAG_NONE is also a one-shot animation
(i.e. it's not looped), but it retains control of the motion channel. i'm providing a slightly better mnemonic for it... */

#define MOTNINF_FLAG_ONESHOT_AND_HOLD	MOTNINF_FLAG_NONE

// These are really AnimProc flags, but need to be known in animscripts...so I'm putting them here in this handy .h file.
#define ANIM_START_FRAME_PROPORTIONAL	-1	//start next motion at the proportionally closest frame as the current motion 
#define ANIM_START_FRAME_LAST				-2	//start next motion at the last frame

// Indicies for motions running in the CActor
#define MOTION1								0		
#define MOTION2								1
#define MOTION3								2
#define MOTION4								3
#define MAX_MOTIONS							4

// Indicies for scripts running in the CActor
#define ANIM1_SCRIPT							0
#define ANIM2_SCRIPT							1
#define ANIM3_SCRIPT							2
#define ANIM4_SCRIPT							3
#define AI_SCRIPT								4
#define MAX_SCRIPTS							5

/* invalid action, for calls to Actor__LoadMotion and Actor__LoadAnimScript */

#define ACTION_INVALID						0xffff

/* for masking ts_Script.sAction */

#define ACTION_MASK							0xff00
#define SUB_ACTION_MASK						0x00ff
