/*************************************/
/*                                   */
/*   g_GameEngine.h                  */
/*   new engine  04/11/00            */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   game engine state and init      */
/*                                   */
/*************************************/

#ifndef __G_GAMEENGINE_H
#define __G_GAMEENGINE_H

/******************* includes ****************************/

/* system includes */

/* engine includes */

#include "platform/BaseType.h"	// low-level include

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

/******************* forward declarations ****************/

/******************* defines *****************************/

/******************* macros ******************************/

/******************* structures **************************/

/******************* global variables ********************/

/******************* global prototypes *******************/

bool GameEngine__Init(void);
bool GameEngine__IsRunning(void);
void GameEngine__Reset(void);
void GameEngine__Shutdown(void);
#ifdef DIRECTX_PC
void GameEngine__Advance(bool updateInput = false);
#else
void GameEngine__Advance(void);
#endif
void GameEngine__SetMaxScenes(int nMaxScenes);
void GameEngine__SetMaxSceneInstances(int nMaxSceneInstances);
void GameEngine__SetMaxSceneNodes(int nMaxScenes);
void GameEngine__SetMaxShadows(int nMaxShadows);
void GameEngine__SetMaxReflections(int nMaxReflections);
void GameEngine__SetMaxLights(int nMaxLights);
void GameEngine__SetMaxSprites(int nMaxSprites);
void GameEngine__SetMaxParticles(int nMaxParticles);
void GameEngine__SetMaxParticleEmitters(int nMaxParticles);
void GameEngine__SetMaxParticleCreateRecords(int nMaxParticleCreateRecords);
void GameEngine__SetMaxDistortions(int nMaxDistortions);
void GameEngine__SetMaxAttachers(int nMaxAttachers);
void GameEngine__SetMaxSwooshes(int nMaxSwooshes);
void GameEngine__SetMaxSwooshEmitters(int nMaxSwooshEmitters);
void GameEngine__SetMaxDataOwners(int nMaxDataOwners);
void GameEngine__SetMaxDataRecords(int nMaxDataRecords);
void GameEngine__SetMaxTextureSets(int nMaxTextureSets);
void GameEngine__SetMaxPoolMatrices(int nMaxPoolMatrices);
void GameEngine__SetMaxPoolFrustums(int nMaxPoolFrustums);
void GameEngine__SetMaxPoolInstances(int nMaxPoolInstances);
void GameEngine__SetMaxCustomChunkLoaders(int nMaxCustomChunkLoaders);

#ifndef CDROM
void GameEngine__StatsToConsole();
#endif //CDROM

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // __G_GAMEENGINE_H
