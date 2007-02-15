/*************************************/
/*                                   */
/*   g_DataHelpers.h                 */
/*   big juju prototype  07/14/00    */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   data structure helper functions */
/*                                   */
/*************************************/

#ifndef __G_DATAHELPERS_H
#define __G_DATAHELPERS_H

/******************* includes ****************************/

#include "Layers/LayersStruct.h"		// low-level include
#include "Game/Animation/Bone.h"		// for BoneID typedef
#include "Game/Database/DBFile.h"	// for DEFAULT_CHUNK_KEY

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

/******************* forward declarations ****************/

struct ts_Geometry;			// in g_World.h
class CInstance;		// in g_World.h
struct ts_BoundingBox;		// in g_Bounds.h
struct ts_PatchSOM;			// in l_SOM.h

/******************* defines *****************************/

/******************* macros ******************************/

/******************* structures **************************/

/******************* global variables ********************/

/******************* global prototypes *******************/

bool GetBoneBox(ts_PatchSOM *pPatchSOM, BoneID boneID, ts_BoundingBox *pBox);
bool RemoveModelBoneData(ts_PatchSOM *pPatchSOM);
int GetTerrainCount(t_Handle Handle);
char *GetTerrainName(t_Handle Handle, int nIndex);
int GetNameCount(t_Handle Handle, int nChunkType, char *pChunkName = NULL);
char *GetName(t_Handle Handle, int nIndex, int nChunkType, char *pChunkName = NULL);

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // __G_DATAHELPERS_H

