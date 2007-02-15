/*************************************/
/*                                   */
/*   ReflectionMgr.h                 */
/*   new engine  12/3/01             */
/*   ned martin, avalanche software  */
/*   visual c++ 6.0                  */
/*   reflection manager              */
/*                                   */
/*************************************/

#ifndef __REFLECTIONMGR_H
#define __REFLECTIONMGR_H

/******************* includes ****************************/

#include "Layers/LayersStruct.h"	// low-level include

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

/******************* forward declarations ****************/

class CInstance;	// in World.h
class CScene;		// in SceneMgr.h

/******************* defines *****************************/

/* define for mirror development-- adds a mirror to center of world */

//#define ADD_TEST_MIRROR

/******************* macros ******************************/

/******************* structures **************************/

/******************* global variables ********************/

/******************* global prototypes *******************/

void		ReflectionMgr__Init(int nMaxReflections);
void		ReflectionMgr__Reset(void);
void		ReflectionMgr__Shutdown(void);
void		ReflectionMgr__Enable(void);
void		ReflectionMgr__Disable(void);
#ifndef CONSUMER_BUILD
bool		ReflectionMgr__IsEmpty(void);
#endif //CONSUMER_BUILD

t_Handle ReflectionMgr__RegisterReflection(CInstance *pInst, Texture* pTexture);

void		ReflectionMgr__KillReflection(t_Handle ReflectionHandle);
void		ReflectionMgr__KillAllReflections(void);
void		ReflectionMgr__CreateReflections(CScene *pScene);
#ifdef ADD_TEST_MIRROR
void		ReflectionMgr__AddTestMirror(void);
#endif //ADD_TEST_MIRROR


#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // __REFLECTIONMGR_H
