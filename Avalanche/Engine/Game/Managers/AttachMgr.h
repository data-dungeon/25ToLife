/*************************************/
/*                                   */
/*   AttachMgr.h                     */
/*   big juju   07/09/02             */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   things attached to actor bones  */
/*                                   */
/*************************************/

/* attaching objects attach to an actor at some bone location. they may be instances (like arrows)
or emitters (like fire), or maybe other things, like actors. attachers record which bone of which
actor they are attached to, and get updated after all actors have run their animation so that the
bone locations are correct, and they get rendered in the correct place */

#ifndef __ATTACHMGR_H
#define __ATTACHMGR_H

/******************* includes ****************************/

/* system includes */

/* engine includes */
#include "Layers/LayersStruct.h"

/******************* forward declarations ****************/

class CProtagonist;
struct ts_PatchSOM;		// from Game/Database/SOM.h

/******************* defines *****************************/

/******************* macros ******************************/

/******************* structures **************************/

/******************* global variables ********************/

/******************* global prototypes *******************/

int AttachMgr__Init(int nMaxAttachers);
void AttachMgr__Reset();
void AttachMgr__Shutdown();
void AttachMgr__KillProtagonistAttachers(CProtagonist *pProtagonist);
void AttachMgr__EnableProtagonistAttachers(CProtagonist *pProtagonist, bool bEnable);
void AttachMgr__KillAllAttachers();
void AttachMgr__Advance();
#ifndef CONSUMER_BUILD
void AttachMgr__ShowAttachers();
#endif //CONSUMER_BUILD

/* create attachers */

t_Handle AttachMgr__CreateAttacher(ts_PatchSOM *pSOM, CProtagonist *pProtagonist, ts_Bone *pBone, float fDistanceAlongBone, Vector3CRef vRotation, const float fLifetime = 0.0f);
t_Handle AttachMgr__CreateAttacher(ts_PatchSOM *pSOM, CProtagonist *pProtagonist, Vector3CRef vCollidePos, Vector3CRef vCollideVel, const float fLifetime = 0.0f);
t_Handle AttachMgr__CreateAttacher(char *pEmitterName, CProtagonist *pProtagonist, Vector3CRef vCollidePos, Vector3CRef vCollideVel, const float fLifetime = 0.0f);
t_Handle AttachMgr__CreateAttacher( char *pEmitterName, CProtagonist *pProtagonist, ts_Bone *pBone, const float fLifetime = 0.0f);
t_Handle AttachMgr__CreateAttacher( float fForceFieldRadius, CProtagonist *pProtagonist, ts_Bone *pBone, const float fLifetime = 0.0f);

/************************ individual attacher manipulation *******************************/

void Attacher__Kill(t_Handle Handle);
void Attacher__Enable(t_Handle Handle, bool bEnable);
t_Handle Attacher__GetEmitterHandle(t_Handle Handle);
t_Handle Attacher__GetForceFieldHandle(t_Handle Handle);


#endif // __ATTACHMGR_H
