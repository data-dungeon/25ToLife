////////////////////////////////////////////////////////////////////////////
//
// DBLSwap
//
// Class to handle the swapping endians of the DBL data
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: DBLWrite.h $
 * 
 * *****************  Version 12  *****************
 * User: Dwain Skinner Date: 6/12/03    Time: 3:55p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * 
 * *****************  Version 11  *****************
 * User: Nmartin      Date: 5/28/03    Time: 7:49p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * processing adam's data-- creates .cut and curve .dbl file
 * 
 * *****************  Version 9  *****************
 * User: Food         Date: 4/17/03    Time: 1:06p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * 
 * *****************  Version 8  *****************
 * User: Dwain Skinner Date: 2/26/03    Time: 7:10p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * 
 * *****************  Version 7  *****************
 * User: Rob Nelson   Date: 9/16/02    Time: 3:49p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 7/22/02    Time: 9:46a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * quat change
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 7/08/02    Time: 3:58p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * Added Mesh Som classes
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 5/17/02    Time: 11:21a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * added Gamecube tristrip header
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 2/20/02    Time: 2:50p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * Exceptions working properly
 * Log working with all plugins
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/07/02    Time: 5:17p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * Added library for handling the dbl file format (taken from Import
 * Export)
 */

#ifndef __DBLWRITE_H
#define __DBLWRITE_H

#ifndef P_BASETYPE_H
#include <Platform/BaseType.h>
#endif


class Vector2;
//class Vector3;
//class Vector4;
class DirCos3x3;
class Graphics4x4;
class CVisRegion;
class CLightRecord;

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


class DBLWrite
{
public:
	static void WriteRecord(const char *pData, FILE *pFile);
	static void WriteRecord(const int *pData, FILE *pFile);
	static void WriteRecord(const float *pData, FILE *pFile);
	static void WriteRecord(const u8 *pData, FILE *pFile);
	static void WriteRecord(const s8 *pData, FILE *pFile);
	static void WriteRecord(const u16 *pData, FILE *pFile);
	static void WriteRecord(const s16 *pData, FILE *pFile);
	static void WriteRecord(const u32 *pData, FILE *pFile);
	static void WriteRecord(const s32 *pData, FILE *pFile);
	static void WriteRecord(const u64 *pData, FILE *pFile);	// handles s64 too, since they are both typedef'd to LARGE_INTEGER
	static void WriteRecord(const Vector2 *pData, FILE *pFile);
	static void WriteRecord(const Vector3 *pData, FILE *pFile);
	static void WriteRecord(const Vector4 *pData, FILE *pFile);
	static void WriteRecord(const ts_ModelBone *pData, FILE *pFile);
	static void WriteRecord(const ts_BoneInfo *pData, FILE *pFile);
	static void WriteRecord(const ts_GroupBoneMap *pData, FILE *pFile);
	static void WriteRecord(const ts_LeftRightMap *pData, FILE *pFile);
	static void WriteRecord(const ts_RecordList *pData, FILE *pFile);
	static void WriteRecord(const ts_DBFileHdr *pData, FILE *pFile);
	static void WriteRecord(const ts_DBBaseChunkHdr *pData, FILE *pFile);
	static void WriteRecord(const ts_DBChunkHdr *pData, FILE *pFile);
	static void WriteRecord(const ts_CollisionSOM *pData, FILE *pFile);
	static void WriteRecord(const ts_CollisionPoly *pData, FILE *pFile);
	static void WriteRecord(const ts_bRGBA *pData, FILE *pFile);
	static void WriteRecord(const ts_PatchSOM *pData, FILE *pFile);
	static void WriteRecord(const ts_Patch *pData, FILE *pFile);
	static void WriteRecord(const ts_ExpansionTableHeader *pData, FILE *pFile);
	static void WriteRecord(const ts_Group *pData, FILE *pFile);
	static void WriteRecord(const CInstance *pData, FILE *pFile);
	static void WriteRecord(const ts_LOD *pData, FILE *pFile);
	static void WriteRecord(const ts_TextureChunkHeader *pData, FILE *pFile);
	static void WriteRecord(const ts_AnimatedTextureChunkHeader *pData, FILE *pFile);
	static void WriteRecord(const ts_TextureChunkRecord *pData, FILE *pFile);
	static void WriteRecord(const ts_TextureContext *pData, FILE *pFile);
	static void WriteRecord(const ts_TextureLayer *pData, FILE *pFile);
	static void WriteRecord(const ts_TriStripHeader *pData, FILE *pFile);
	static void WriteRecord(const ts_GCNTriStripHeader *pData, FILE *pFile);
//	static void WriteRecord(const ts_LightInfo *pData, FILE *pFile);
//	static void WriteRecord(const ts_NamedPointInfo *pData, FILE *pFile);
//	static void WriteRecord(const ts_SectorInfo *pData, FILE *pFile);
	static void WriteRecord(const ts_PieceRecord *pData, FILE *pFile);
#if 0
	static void WriteRecord(const ts_TextureChunkVRAMInfoRecord  *pData, FILE *pFile);
#endif
	static void WriteRecord(const CSceneNode *pData, FILE *pFile);
	static void WriteRecord(const ts_BoundingBox *pData, FILE *pFile);
	static void WriteRecord(const ts_BoundingSphere *pData, FILE *pFile);
	static void WriteRecord(const SMeshSOM *pData, FILE *pFile);
	static void WriteRecord(const SMeshSOM::SNodeSOM *pData, FILE *pFile);
	static void WriteRecord(const SMeshSOM::SIndexPair *pData, FILE *pFile);
	static void WriteRecord(const SMeshSOM::SIndexTrio *pData, FILE *pFile);
	static void WriteRecord(const SMeshSOM::SSubMesh *pData, FILE *pFile);
	static void WriteRecord( const CVisRegion* pData, FILE* pFile );
	static void WriteRecord( const CLightRecord* pData, FILE* pFile );
	static void WriteRecord(const ts_EnvSoundSource *pData, FILE *pFile);
	static void WriteRecord(const ts_EnvSoundField *pData, FILE *pFile);
	static void WriteRecord(const ts_EnvHalo *pData, FILE *pFile);
	static void WriteRecord(const ts_EnvStationaryCam *pData, FILE *pFile);
	static void WriteRecord(const ts_Water *pData, FILE *pFile);
	static void WriteRecord(const ts_AnimCurve *pData, FILE *pFile);

// write arrays

	static void WriteRecords(const char *pData, FILE *pFile, u32 u32Count);
	static void WriteRecords(const u16 *pData, FILE *pFile, u32 u32Count);
	static void WriteRecords(const u32 *pData, FILE *pFile, u32 u32Count);
	static void WriteRecords(const float *pData, FILE *pFile, u32 u32Count);
	static void WriteRecords(const Vector2 *pData, FILE *pFile, u32 u32Count);
	static void WriteRecords(const Vector3 *pData, FILE *pFile, u32 u32Count);
	static void WriteRecords(const Vector4 *pData, FILE *pFile, u32 u32Count);
	static void WriteRecords(const ts_bRGBA *pData, FILE *pFile, u32 u32Count);
	static void WriteRecords(const ts_ModelBone *pData, FILE *pFile, u32 u32Count);
	static void WriteRecords(const ts_CollisionPoly *pData, FILE *pFile, u32 u32Count);
	static void WriteRecords(const ts_TextureChunkRecord *pData, FILE *pFile, u32 u32Count);
	static void WriteRecords(const ts_TextureLayer *pData, FILE *pFile, u32 u32Count);
	static void WriteRecords(const ts_PieceRecord *pData, FILE *pFile, u32 u32Count);
#if 0
	static void WriteRecords(const ts_TextureChunkVRAMInfoRecord  *pData, FILE *pFile, u32 u32Count);
#endif
	static void WriteRecords(const SMeshSOM::SNodeSOM *pData, FILE *pFile, u32 u32Count);
	static void WriteRecords(const SMeshSOM::SIndexPair *pData, FILE *pFile, u32 u32Count);
	static void WriteRecords(const SMeshSOM::SIndexTrio *pData, FILE *pFile, u32 u32Count);
	static void WriteRecords(const SMeshSOM::SSubMesh *pData, FILE *pFile, u32 u32Count);

// generic data (strings, binary data, stuff that doesn't need to go through potential endian swap) 

	static void WriteData(const void *pData, u32 u32Length, FILE *pFile);

// smaller set of reading functions */

	static void ReadRecord(ts_DBBaseChunkHdr *pData, FILE *pFile);
	static void ReadData(void *pData, u32 u32Length, FILE *pFile);
};

#endif // __DBLWRITE_H
