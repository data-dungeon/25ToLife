/***********************************/
/*                                 */
/* Preview.h                       */
/* Pigpen particle tool  12/13/00  */
/* ned martin  avalanche software  */
/* visual c++ 6.0                  */
/* preview particle effects        */
/*                                 */
/***********************************/

#ifndef __PREVIEW_H
#define __PREVIEW_H

/******************* includes ****************************/

#include "Doc.h"
#include "Effect.h"

/******************* defines *****************************/

/* file name for exporting effect for preview */

#define PREVIEW_FILE_NAME "PigPrevw.DBL"	// keep it 8.3 for best compatibility with engine
#define PREVIEW_LOG_NAME "PigPrevwExport.log"

/* default camera position */

#define DEFAULT_CAMERA_POS_X	0.0f
#define DEFAULT_CAMERA_POS_Y	2.0f
#define DEFAULT_CAMERA_POS_Z	20.0f

/* for more generic dialog processing, give access to addresses of data fields. data field id's are used */
/* by dialog control lists */

enum
{
	PREVIEW_FLAGS = 0,
	PREVIEW_DELAY,
	PREVIEW_YPOS,
	PREVIEW_EMITTER_VEL,
	PREVIEW_MODEL_XPOS,
	PREVIEW_MODEL_YPOS,
	PREVIEW_MODEL_ZPOS,
	PREVIEW_MODEL_YROT,
	PREVIEW_WIND_MPH,
};

/* flags for preview control via preview dialog */

#define PREVIEW_CAMERA_FOLLOWS	0x00000001
#define PREVIEW_GRAVITY				0x00000002
#define PREVIEW_PARTICLE_PATH		0x00000004

/******************* forward declarations ****************/

/******************* structures **************************/

/******************* global variables ********************/

/******************* global prototypes *******************/

void Preview__Create(CPigpenDoc *pDoc, CEmitter *pEmitter, CEffect *pEffect, CParticle *pParticle, CSwoosh *pSwoosh, CPigpenSwooshEmitter *pSwooshEmitter);
void Preview__SetBackgroundColor(COLORREF Color);
COLORREF Preview__GetBackgroundColor(void);
void Preview__SetCameraInitPos(void);
Vector3 *Preview__GetCameraPos(void);
void *Preview__GetDataPointer(int nDataID);
CString &Preview__GetStageFileAndStatus(void);
CString &Preview__GetEmitterModelFileAndStatus(void);
void Preview__UpdateDelay(void);
void Preview__UpdateEmitterVel(void);
void Preview__UpdateEmitterYPos(void);
void Preview__ClearEmitterVel(void);
void Preview__ToggleEmitterVelUnits(void);
void Preview__UpdateWindMPH(void);
void Preview__ResetEmitter(void);
void Preview__ResetCamera(void);
void Preview__DocClosed(CPigpenDoc *pDoc);
void Preview__DialogClosed(void);
void Preview__LoadNewStage(void);
void Preview__LoadNewEmitterModel(void);
void Preview__ClearEmitterModel(void);
void Preview__BuildEmitterModelTransform(bool bUpdateDoc);
bool Preview__EmitterVelIsMPH();

/* real-time update support, called from dialog boxes when changes are made */

void Preview__SetCollisionIndex(int nIndex);
void Preview__UpdateParticle(CParticle *pParticle);
void Preview__UpdateEffect(CEffect *pEffect);
void Preview__UpdateEmitter(CEmitter *pEmitter);
void Preview__UpdateSwoosh(CSwoosh *pSwoosh);
void Preview__UpdateSwooshEmitter(CPigpenSwooshEmitter *pSwooshEmitter);
void Preview__UpdateParticleOrSwoosh(CParticle *pParticle, CSwoosh *pSwoosh);	// one or the other

#endif // __PREVIEW_H
