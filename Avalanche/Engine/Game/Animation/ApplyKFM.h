/******************* includes *****************************/

/* system includes */

/* engine includes */

#include "Game/Animation/Bone.h"				// needed for MAX_BONENAME_LEN

#if defined(__cplusplus)
extern "C" {
#endif

/******************* forward declarations ****************/

struct ts_BoneChannel;		// from MotCtrl.h
struct ts_BoneChannelItem;	// from MotCtrl.h

/******************* defines *****************************/

//data reduction: reduce anim value from float to short.  Here are macros to convert back and forth.
#define ANIMVALUE_TO_FLOAT(animvalue, animValToFloat) ((float)animvalue * animValToFloat)	
#define FLOAT_TO_ANIMVALUE(floatvalue, floatToAnimVal) ((short)(floatvalue * floatToAnimVal + 0.5f))

//data reduction: reduce the spline handles from vct2f's to vct2c's. Here are macros to convert back and forth.
#define MAX_ANIMVECT_VALUE (127.0f) //max value a signed char can have
#define MAX_ANIMVECT_FLOAT (8.0f) //max value that anim values can take on
#define ANIMVECT_TO_FLOAT(animvect) ((float)animvect * (MAX_ANIMVECT_FLOAT / MAX_ANIMVECT_VALUE))
#define FLOAT_TO_ANIMVECT(floatvalue) ((short)(floatvalue * (MAX_ANIMVECT_VALUE / MAX_ANIMVECT_FLOAT) + 0.5f))

/******************* macros ******************************/

/******************* structures **************************/

/******************* global variables ********************/

/******************* global prototypes *******************/

ts_BoneChannel *KFMFindChannelFromName( ts_MotionHeader *pMotionHeader, char cBoneName[MAX_BONENAME_LEN] );
void KFMComputeStandardToAction( ts_Bone *pBone, Matrix3x3 *m_bonetoparent);
bool KFMApplyBone( ts_BoneChannel *pMChannel, ts_Bone *pBone, Graphics4x4 *pMatrix, u16 jiffies, bool bXMirror );

#ifdef TOOLS
float InterpolateValue( ts_BoneChannelItem *pMChannelItem, const unsigned short usJiffies, unsigned int index, float animValToFloat);
float InterpolateValue2( ts_BoneChannelItem *pMChannelItem, const unsigned short usJiffies, unsigned int index, float animValToFloat);
#endif
#if defined(__cplusplus)
}
#endif


