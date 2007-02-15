////////////////////////////////////////////////////////////////////////////
//
// DBLSwap
//
// Class to handle the swapping endians of the DBL data
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: DBLSwap.h $
 * 
 * *****************  Version 14  *****************
 * User: Dwain Skinner Date: 6/12/03    Time: 3:55p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * 
 * *****************  Version 13  *****************
 * User: Nmartin      Date: 5/28/03    Time: 7:49p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * processing adam's data-- creates .cut and curve .dbl file
 * 
 * *****************  Version 12  *****************
 * User: Nmartin      Date: 5/19/03    Time: 8:43a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * start of cutscene filter
 * 
 * *****************  Version 10  *****************
 * User: Food         Date: 4/17/03    Time: 1:06p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * 
 * *****************  Version 9  *****************
 * User: Dwain Skinner Date: 2/26/03    Time: 7:10p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 12/02/02   Time: 8:07a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * added swapendianinplace for bounding box
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 11/07/02   Time: 4:09p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * more fixes...
 * 
 * *****************  Version 6  *****************
 * User: Rob Nelson   Date: 9/16/02    Time: 3:49p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 7/22/02    Time: 9:46a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * quat change
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 7/08/02    Time: 3:58p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * Added Mesh Som classes
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 5/17/02    Time: 11:21a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * added Gamecube tristrip header
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/07/02    Time: 5:17p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * Added library for handling the dbl file format (taken from Import
 * Export)
 */

#ifndef __DBLSWAP_H
#define __DBLSWAP_H

#ifndef P_BASETYPE_H
#include <Platform/BaseType.h>
#endif

class Vector2;
//class Vector3;
//class Vector4;
class DirCos3x3;
class Graphics4x4;
class CVisRegion;

struct ts_Quatvw;
struct ts_bRGBA;
struct ts_MaterialBitfield;

struct ts_ModelBone;
struct ts_BoneInfo;
struct ts_GroupBoneMap;
struct ts_LeftRightMap;
struct ts_RecordList;
struct ts_DBFileHdr;
struct ts_DBBaseChunkHdr;
struct ts_DBChunkHdr;
struct ts_CollisionSOM;
struct ts_CollisionPoly;
struct ts_PatchSOM;
struct ts_Patch;
struct ts_ExpansionTableHeader;
struct ts_Group;
class CInstance;
struct ts_LOD;
struct ts_PieceRecord;
struct ts_TextureChunkHeader;
struct ts_AnimatedTextureChunkHeader;
struct ts_TextureChunkRecord;
struct ts_TextureContext;
struct ts_TextureLayer;
struct ts_TriStripHeader;
struct ts_GCNTriStripHeader;
//struct ts_LightInfo;
//struct ts_NamedPointInfo;
//struct ts_SectorInfo;
struct ts_EmitterSource;
struct ts_Effect;
struct ts_Channel;
struct ts_ParticleSource;

#if 0
struct ts_TextureChunkVRAMInfoRecord;
#endif

class CSceneNode;
struct ts_BoundingBox;
struct ts_BoundingSphere;
struct ts_EnvSoundSource;
struct ts_EnvSoundField;
struct ts_EnvHalo;
struct ts_EnvStationaryCam;
struct ts_Water;
struct ts_AnimCurve;

#ifndef MESH_SOM_H
#include <Game/Database/MeshSOM.h>
#endif

class DBLSwap
{
	static bool m_swapEndianActive;
	static u8 m_bigEndian8Buff[8];
public:
	static bool GetSwapEndianActive(void);
	static void SetSwapEndianActive(bool swapEndianActive);
	static void SwapEndian2(void *pValue);
	static void SwapEndian4(void *pValue);
	static void SwapEndian8(void *pValue);


	static void TestSwapEndian(void);

	static void SwapEndianInPlace(u8 *pValue);
	static void SwapEndianInPlace(s8 *pValue);
	static void SwapEndianInPlace(bool *pValue);
	static void SwapEndianInPlace(u16 *pValue);
	static void SwapEndianInPlace(s16 *pValue);
	static void SwapEndianInPlace(u32 *pValue);
	static void SwapEndianInPlace(s32 *pValue);
	static void SwapEndianInPlace(int *pValue);
	static void SwapEndianInPlace(u64 *pValue);
	static void SwapEndianInPlace(float *pValue);
	static void SwapEndianInPlace(Vector2 *pValue);
	static void SwapEndianInPlace(Vector3 *pValue);
	static void SwapEndianInPlace(Vector4Packed *pValue);
	static void SwapEndianInPlace(Vector4Aligned *pValue);
	static void SwapEndianInPlace(DirCos3x3 *pValue);
	static void SwapEndianInPlace(Graphics4x4 *pValue);
	static void SwapEndianInPlace(ts_Quatvw *pValue);
	static void SwapEndianInPlace(ts_bRGBA *pValue);
	static void SwapEndianInPlace(ts_fRGB *pValue);
	static void SwapEndianInPlace(ts_MaterialBitfield *pValue);
	static void SwapEndianInPlace(ts_BoundingBox *pValue);

	static const int *SwapEndian(const int *pData);
	static const float *SwapEndian(const float *pData);
	static const u16 *SwapEndian(const u16 *pData);
	static const s16 *SwapEndian(const s16 *pData);
	static const u32 *SwapEndian(const u32 *pData);
	static const s32 *SwapEndian(const s32 *pData);
	static const u64 *SwapEndian(const u64 *pData);
	static const Vector2 *SwapEndian(const Vector2 *pData);
	static const Vector3 *SwapEndian(const Vector3 *pData);
	static const Vector4 *SwapEndian(const Vector4 *pData);
	static const ts_ModelBone *SwapEndian(const ts_ModelBone *pData);
	static const ts_BoneInfo *SwapEndian(const ts_BoneInfo *pData);
	static const ts_GroupBoneMap *SwapEndian(const ts_GroupBoneMap *pData);
	static const ts_LeftRightMap *SwapEndian(const ts_LeftRightMap *pData);
	static const ts_RecordList *SwapEndian(const ts_RecordList *pData);
	static const ts_DBFileHdr *SwapEndian(const ts_DBFileHdr *pData);
	static const ts_DBBaseChunkHdr *SwapEndian(const ts_DBBaseChunkHdr *pData);
	static const ts_DBChunkHdr *SwapEndian(const ts_DBChunkHdr *pData);
	static const ts_CollisionSOM *SwapEndian(const ts_CollisionSOM *pData);
	static const ts_CollisionPoly *SwapEndian(const ts_CollisionPoly *pData);
	static const ts_bRGBA *SwapEndian(const ts_bRGBA *pData);
	static const ts_PatchSOM *SwapEndian(const ts_PatchSOM *pData);
	static const ts_Patch *SwapEndian(const ts_Patch *pData);
	static const ts_ExpansionTableHeader *SwapEndian(const ts_ExpansionTableHeader *pData);
	static const ts_Group *SwapEndian(const ts_Group *pData);
	static const CInstance *SwapEndian(const CInstance *pData);
	static const ts_LOD *SwapEndian(const ts_LOD *pData);
	static const ts_PieceRecord *SwapEndian(const ts_PieceRecord *pData);
	static const ts_TextureChunkHeader *SwapEndian(const ts_TextureChunkHeader *pData);
	static const ts_AnimatedTextureChunkHeader *SwapEndian(const ts_AnimatedTextureChunkHeader *pData);
	static const ts_TextureChunkRecord *SwapEndian(const ts_TextureChunkRecord *pData);
	static const ts_TextureContext *SwapEndian(const ts_TextureContext *pData);
	static const ts_TextureLayer *SwapEndian(const ts_TextureLayer *pData);
	static const ts_TriStripHeader *SwapEndian(const ts_TriStripHeader *pData);
	static const ts_GCNTriStripHeader *SwapEndian(const ts_GCNTriStripHeader *pData);
//	static const ts_LightInfo *SwapEndian(const ts_LightInfo *pData);
//	static const ts_NamedPointInfo *SwapEndian(const ts_NamedPointInfo *pData);
//	static const ts_SectorInfo *SwapEndian(const ts_SectorInfo *pData);
	static const ts_EmitterSource *SwapEndian(const ts_EmitterSource *pData);
	static const ts_Effect *SwapEndian(const ts_Effect *pData);
	static const ts_Channel *SwapEndian(const ts_Channel *pData);
	static const ts_ParticleSource *SwapEndian(const ts_ParticleSource *pData);
#if 0
	static const ts_TextureChunkVRAMInfoRecord *SwapEndian(const ts_TextureChunkVRAMInfoRecord *pData);
#endif
	static const CSceneNode *SwapEndian(const CSceneNode *pData);
	static const ts_BoundingBox *SwapEndian(const ts_BoundingBox *pData);
	static const ts_BoundingSphere *SwapEndian(const ts_BoundingSphere *pData);
	static const SMeshSOM *SwapEndian(const SMeshSOM *pData);
	static const SMeshSOM::SNodeSOM *SwapEndian(const SMeshSOM::SNodeSOM *pData);
	static const SMeshSOM::SIndexPair *SwapEndian(const SMeshSOM::SIndexPair *pData);
	static const SMeshSOM::SIndexTrio *SwapEndian(const SMeshSOM::SIndexTrio *pData);
	static const SMeshSOM::SSubMesh *SwapEndian(const SMeshSOM::SSubMesh *pData);
	static const CVisRegion* SwapEndian( const CVisRegion* pData );
	static const CLightRecord* SwapEndian( const CLightRecord* pData );
	static const ts_EnvSoundSource *SwapEndian(const ts_EnvSoundSource *pData);
	static const ts_EnvSoundField *SwapEndian(const ts_EnvSoundField *pData);
	static const ts_EnvHalo *SwapEndian(const ts_EnvHalo *pData);
	static const ts_EnvStationaryCam *SwapEndian(const ts_EnvStationaryCam *pData);
	static const ts_Water *SwapEndian(const ts_Water *pData);
	static const ts_AnimCurve *DBLSwap::SwapEndian(const ts_AnimCurve *pData);

private:
	static bool CheckRecordSize(const char *pName, u32 u32HardwiredSize, u32 u32ActualSize);

};

#include "DBLSwap.hpp"

#endif // __DBLSWAP_H
