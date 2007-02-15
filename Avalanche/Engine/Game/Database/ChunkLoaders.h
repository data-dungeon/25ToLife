/*************************************/
/*                                   */
/*   g_ChunkLoaders.h                */
/*   big juju prototype  02/24/00    */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   .DBL chunk loading routines     */
/*                                   */
/*************************************/

#ifndef __G_CHUNKLOADERS_H
#define __G_CHUNKLOADERS_H

/******************* includes ****************************/

/* system includes */

/* engine includes */

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

/******************* forward declarations ****************/

struct ts_ParticleSource;		// in Game/Managers/ParticleDB.h
struct ts_SwooshSource;			// in Game/Managers/ParticleDB.h
struct ts_Effect;					// in Game/Managers/ParticleDB.h
struct ts_DBChunkHdr;			// in Game/Database/DBFile.h
struct ts_RecordList;			// in Game/Database/DBFile.h
struct ts_PatchSOM;				// in Game/Database/SOM.h
struct ts_Body;					// in Game/Animation/Bone.h
struct ts_Bone;					// in Game/Animation/Bone.h
struct ts_ModelBone;				// in Game/Animation/Bone.h
struct ts_BoneGroupList;		// in Game/Animation/Bone.h

/******************* defines *****************************/

/******************* macros ******************************/

/******************* structures **************************/

/******************* global variables ********************/

/******************* global prototypes *******************/

bool LoadSingleSOM( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize);
bool LoadSOMArray( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize);
bool LoadSOMTextures( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize);
#ifdef WIN32	// only intended for use by terraintoola and obbworld
bool LoadCollisionSOMArray( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize);
#endif //WIN32
bool LoadMOT( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize);
bool LoadMOTNames( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize);
bool LoadTPG( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize);
bool LoadLODs( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize);
bool LoadInstances( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize);
bool LoadInstanceAttributes( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize);
bool LoadGroups( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize);
bool LoadPaths( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize);
bool LoadNamedPoints( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize);
bool LoadSoundSources( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize);
bool LoadHalo( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize);
bool LoadBloom( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize);
bool LoadStationaryCam( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize);
bool LoadSoundFields( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize);
bool LoadWater( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize);
bool LoadExpansionTables( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize);
bool LoadParticleEffects( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize);
bool LoadParticleSource( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize);
bool LoadParticleEmitters( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize);
bool LoadSwooshSource( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize);
bool LoadSwooshEmitters( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize);
bool LoadWorldEmitters(void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize);
bool LoadWorldDistortions(void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize);
bool ReadBoneData(ts_PatchSOM *pSOM, ts_ModelBone *pMBones);
bool LoadBoneInfo( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize);
bool LoadBoneGroupInfo( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize);
bool LoadSceneTree( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize);
bool LoadVisRegions( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize);
bool loadLightSet( void *pData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize );
#ifdef PS2
bool ForceDMAObject(ts_PatchSOM *pSOM);
#endif //PS2

/* these are called from Instance__FixDiskLoad, but don't belong in Instance.cpp */

void FixInstanceBones( ts_Bone *pRoot, ts_Bone *pBone, ts_Bone *pParent, ts_Body *pBody, ts_ModelBone *pMRoot, ts_ModelBone *pMBone, ts_ModelBone *pMParent);
void FixBoneGroupLists( ts_Bone *pBone, ts_BoneGroupList *pBoneGroupList);

/* these are used directly by pigpen */

void FixParticleEffect(ts_Effect *pEffectData, ts_RecordList *pParticleSourceList);
void FixParticleSource(ts_ParticleSource *pParticleSource);
void FixSwooshSource(ts_SwooshSource *pSwooshSource);



#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // __G_CHUNKLOADERS_H

