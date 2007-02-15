////////////////////////////////////////////////////////////////////////////
//
// DBLSwap
//
// Class to handle the swapping endians of the DBL data
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: DBLWrite.cpp $
 * 
 * *****************  Version 15  *****************
 * User: Nmartin      Date: 5/28/03    Time: 7:49p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * processing adam's data-- creates .cut and curve .dbl file
 * 
 * *****************  Version 13  *****************
 * User: Food         Date: 4/17/03    Time: 1:06p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 4/04/03    Time: 2:25p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * new CVisRegion structure
 * 
 * *****************  Version 11  *****************
 * User: Dwain Skinner Date: 2/26/03    Time: 7:10p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * 
 * *****************  Version 10  *****************
 * User: Nmartin      Date: 10/29/02   Time: 3:43p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * fix endian swap of CVisRegion data
 * 
 * *****************  Version 9  *****************
 * User: Nmartin      Date: 10/18/02   Time: 10:42a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * endian swap entries in vis region table
 * 
 * *****************  Version 8  *****************
 * User: Rob Nelson   Date: 9/16/02    Time: 3:49p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 8/21/02    Time: 4:17p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * DBL Debug Code
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 7/08/02    Time: 3:58p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * Added Mesh Som classes
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 5/17/02    Time: 11:21a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * added Gamecube tristrip header
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 2/20/02    Time: 4:31p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * Fixed Exception Problem
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

#include "DBLUtilsPch.h"
#include "DBLWrite.h"
#include "DBLSwap.h"
#include "DBLDebug.h"
#include "audio\environment\envdbl.h"
#include "effects\halo\halodbl.h"
#include "Camera\StationaryCam\StationaryCamDbl.h"
#include "effects\water\waterdbl.h"
#include <GeomUtil/AnimCurve.h>
#include <Game/Managers/TextureMgr/TextureDbl.h>

// one of these routines exists for each type of data record we want to write out 
void DBLWrite::WriteRecord(const char *pData, FILE *pFile)
{
	/* write a single record */
	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const char *) - fwrite error"));
}

void DBLWrite::WriteRecord(const int *pData, FILE *pFile)
{
	/* swap endianness if necessary */
	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const int *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const int *) - fwrite error"));
}

void DBLWrite::WriteRecord(const float *pData, FILE *pFile)
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const float *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const float *) - fwrite error"));
}

void DBLWrite::WriteRecord(const u8 *pData, FILE *pFile)
{
	/* write a single record */
	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const u8 *) - fwrite error"));
}

void DBLWrite::WriteRecord(const s8 *pData, FILE *pFile)
{
	/* write a single record */
	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const s8 *) - fwrite error"));
}

void DBLWrite::WriteRecord(const u16 *pData, FILE *pFile)
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const u16 *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const u16 *) - fwrite error"));
}

void DBLWrite::WriteRecord(const s16 *pData, FILE *pFile)
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const s16 *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const u16 *) - fwrite error"));
}

void DBLWrite::WriteRecord(const u32 *pData, FILE *pFile)
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const u32 *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const u32 *) - fwrite error"));
}

void DBLWrite::WriteRecord(const s32 *pData, FILE *pFile)
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const s32 *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const s32 *) - fwrite error"));
}

void DBLWrite::WriteRecord(const u64 *pData, FILE *pFile)	// handles s64 too, since they are both typedef'd to LARGE_INTEGER
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const u64 *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const u64 *) - fwrite error"));
}

void DBLWrite::WriteRecord(const Vector2 *pData, FILE *pFile)
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const Vector2 *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const Vector2 *) - fwrite error"));
}

void DBLWrite::WriteRecord(const Vector3 *pData, FILE *pFile)
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const Vector3 *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const Vector3 *) - fwrite error"));
}

void DBLWrite::WriteRecord(const Vector4 *pData, FILE *pFile)
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const Vector4 *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const Vector4 *) - fwrite error"));
}

void DBLWrite::WriteRecord(const ts_ModelBone *pData, FILE *pFile)
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_ModelBone *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_ModelBone *) - fwrite error"));
}

void DBLWrite::WriteRecord(const ts_BoneInfo *pData, FILE *pFile)
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_BoneInfo *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_BoneInfo *) - fwrite Error"));
}

void DBLWrite::WriteRecord(const ts_GroupBoneMap *pData, FILE *pFile)
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_GroupBoneMap *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_GroupBoneMap *) - fwrite Error"));
}

void DBLWrite::WriteRecord(const ts_LeftRightMap *pData, FILE *pFile)
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_LeftRightMap *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_LeftRightMap *) - fwrite Error"));
}

void DBLWrite::WriteRecord(const ts_RecordList *pData, FILE *pFile)
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_RecordList *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_RecordList *) - fwrite Error"));
}

void DBLWrite::WriteRecord(const ts_DBFileHdr *pData, FILE *pFile)
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_DBFileHdr *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_DBFileHdr *) - fwrite Error"));
}

void DBLWrite::WriteRecord(const ts_DBBaseChunkHdr *pData, FILE *pFile)
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_DBBaseChunkHdr *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_DBBaseChunkHdr *) - fwrite Error"));
}

void DBLWrite::WriteRecord(const ts_DBChunkHdr *pData, FILE *pFile)
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_DBChunkHdr *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_DBChunkHdr *) - fwrite Error"));
}

void DBLWrite::WriteRecord(const ts_CollisionSOM *pData, FILE *pFile)
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_CollisionSOM *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_CollisionSOM *) - fwrite Error"));
}

void DBLWrite::WriteRecord(const ts_CollisionPoly *pData, FILE *pFile)
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_CollisionPoly *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_CollisionPoly *) - fwrite Error"));
}

void DBLWrite::WriteRecord(const ts_bRGBA *pData, FILE *pFile)
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_bRGBA *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_bRGBA *) - fwrite Error"));
}

void DBLWrite::WriteRecord(const ts_PatchSOM *pData, FILE *pFile)
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_PatchSOM *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_PatchSOM *) - fwrite Error"));
}

void DBLWrite::WriteRecord(const ts_Patch *pData, FILE *pFile)
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_Patch *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_Patch *) - fwrite Error"));
}

void DBLWrite::WriteRecord(const ts_ExpansionTableHeader *pData, FILE *pFile)
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_ExpansionTableHeader *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_ExpansionTableHeader *) - fwrite Error"));
}

void DBLWrite::WriteRecord(const ts_Group *pData, FILE *pFile)
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_Group *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_Group *) - fwrite Error"));
}

void DBLWrite::WriteRecord(const CInstance *pData, FILE *pFile)
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const CInstance *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const CInstance *) - fwrite Error"));
}

void DBLWrite::WriteRecord(const ts_LOD *pData, FILE *pFile)
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_LOD *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_LOD *) - fwrite Error"));
}

void DBLWrite::WriteRecord(const ts_PieceRecord *pData, FILE *pFile)
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_PieceRecord *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_PieceRecord *) - fwrite Error"));
}

/******* note-- not suitable for cut-and-paste to create a new record type writer because of truncated output ******/
void DBLWrite::WriteRecord(const ts_TextureChunkHeader *pData, FILE *pFile)
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_TextureChunkHeader *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData) - sizeof(ts_TextureChunkRecord), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_TextureChunkHeader *) - fwrite Error"));
}

/******* note-- not suitable for cut-and-paste to create a new record type writer because of truncated output ******/
void DBLWrite::WriteRecord(const ts_AnimatedTextureChunkHeader *pData, FILE *pFile)
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_AnimatedTextureChunkHeader *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData) - sizeof(ts_TextureChunkRecord), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_AnimatedTextureChunkHeader *) - fwrite Error"));
}

void DBLWrite::WriteRecord(const ts_TextureChunkRecord *pData, FILE *pFile)
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_TextureChunkRecord *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_TextureChunkRecord *) - fwrite Error"));
}

/******* note-- not suitable for cut-and-paste to create a new record type writer because of truncated output ******/
void DBLWrite::WriteRecord(const ts_TextureContext *pData, FILE *pFile)
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_TextureContext *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData) - sizeof(ts_TextureLayer), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_TextureContext *) - fwrite Error"));
}

void DBLWrite::WriteRecord(const ts_TextureLayer *pData, FILE *pFile)
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_TextureLayer *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_TextureLayer *) - fwrite Error"));
}

void DBLWrite::WriteRecord(const ts_TriStripHeader *pData, FILE *pFile)
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_TriStripHeader *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_TriStripHeader *) - fwrite Error"));
}

void DBLWrite::WriteRecord(const ts_GCNTriStripHeader *pData, FILE *pFile)
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_GCNTriStripHeader *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_GCNTriStripHeader *) - fwrite Error"));
}

#if 0
void DBLWrite::WriteRecord(const ts_LightInfo *pData, FILE *pFile)
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_LightInfo *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_LightInfo *) - fwrite Error"));
}

void DBLWrite::WriteRecord(const ts_NamedPointInfo *pData, FILE *pFile)
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_NamedPointInfo *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_NamedPointInfo *) - fwrite Error"));
}

void DBLWrite::WriteRecord(const ts_SectorInfo *pData, FILE *pFile)
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_SectorInfo *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_SectorInfo *) - fwrite Error"));
}
#endif

#if 0
void DBLWrite::WriteRecord(const ts_TextureChunkVRAMInfoRecord  *pData, FILE *pFile)
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_TextureChunkVRAMInfoRecord *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_TextureChunkVRAMInfoRecord *) - fwrite Error"));
}
#endif

void DBLWrite::WriteRecord(const CSceneNode *pData, FILE *pFile)
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const CSceneNode *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const CSceneNode *) - fwrite Error"));
}

void DBLWrite::WriteRecord(const ts_BoundingBox *pData, FILE *pFile)
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_BoundingBox *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_BoundingBox *) - fwrite Error"));
}

void DBLWrite::WriteRecord(const ts_BoundingSphere *pData, FILE *pFile)
{
	/* swap endianness if necessary */

	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_BoundingSphere *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_BoundingSphere *) - fwrite Error"));
}

void DBLWrite::WriteRecord(const SMeshSOM *pData, FILE *pFile)
{
	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const SMeshSOM *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const SMeshSOM *) - fwrite Error"));
}

void DBLWrite::WriteRecord(const SMeshSOM::SNodeSOM *pData, FILE *pFile)
{
	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const SMeshSOM::SNodeSOM *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const SMeshSOM::SNodeSOM *) - fwrite Error"));
}

void DBLWrite::WriteRecord(const SMeshSOM::SIndexPair *pData, FILE *pFile)
{
	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const SMeshSOM::SIndexPair *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const SMeshSOM::SIndexPair *) - fwrite Error"));
}

void DBLWrite::WriteRecord(const SMeshSOM::SIndexTrio *pData, FILE *pFile)
{
	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const SMeshSOM::SIndexTrio *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const SMeshSOM::SIndexTrio *) - fwrite Error"));
}

void DBLWrite::WriteRecord(const SMeshSOM::SSubMesh *pData, FILE *pFile)
{
	pData = DBLSwap::SwapEndian(pData);
	if (pData == NULL)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const SMeshSOM::SSubMesh *) - SwapEndian Error"));

	/* write a single record */

	if (DBL_FILEWRITE(pData, sizeof(*pData), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const SMeshSOM::SSubMesh *) - fwrite Error"));
}

void DBLWrite::WriteRecord( const CVisRegion* pData, FILE* pFile )
{
	/* if SwapEndian swaps, it returns a pointer to the new data that is different than the input pointer-- it does not */
	/* write over the input data. however, it does not address the variable length array, so the pointer to the */
	/* swapped data does not include the contents of the array. since we have to write out the array, we need to */
	/* keep the old pointer around */
	CVisRegion *pInputData = (CVisRegion *) pData;

	/* swap endianness if necessary */
	pData = DBLSwap::SwapEndian( pData );
	if ( pData == NULL )
		throw(PluginException(ERROR_FATAL,"WriteRecord( const CVisRegion* pData, FILE* pFile ) - SwapEndian Error"));

	/* write a single record, not including variable length array */
	if (DBL_FILEWRITE(pData, sizeof(*pData) - sizeof(CVisRegion::VisRegionLinks), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord( const CVisRegion* pData, FILE* pFile ) - fwrite Error"));

	/* write the array-- from the input data, not from the possibly-swapped data */
	for (u32 i = 0; i < pInputData->GetVisibleRegionCount(); i++)
	{
		u32 u32Value = pInputData->GetVisibleRegionIndex(i);
		const u32 *pu32Value = DBLSwap::SwapEndian( &u32Value );
		if ( pu32Value == NULL )
			throw(PluginException(ERROR_FATAL,"WriteRecord( const CVisRegion* pData, FILE* pFile ) - SwapEndian Error"));
		if (DBL_FILEWRITE(pu32Value, sizeof(u32), 1,  pFile) != 1)
			throw(PluginException(ERROR_FATAL,"WriteRecord( const CVisRegion* pData, FILE* pFile ) - fwrite Error"));

		u32 u32Flags = pInputData->GetVisibleRegionFlags(i);
		const u32 *pu32Flags = DBLSwap::SwapEndian( &u32Flags );
		if ( pu32Flags == NULL )
			throw(PluginException(ERROR_FATAL,"WriteRecord( const CVisRegion* pData, FILE* pFile ) - SwapEndian Error"));
		if (DBL_FILEWRITE(pu32Flags, sizeof(u32), 1,  pFile) != 1)
			throw(PluginException(ERROR_FATAL,"WriteRecord( const CVisRegion* pData, FILE* pFile ) - fwrite Error"));
	}
}

void DBLWrite::WriteRecord( const CLightRecord* pData, FILE* pFile )
{
	pData = DBLSwap::SwapEndian( pData );

	if ( pData == NULL )
		throw(PluginException( ERROR_FATAL,"WriteRecord( const CLightRecord* pData, FILE* pFile ) - SwapEndian Error"));

	// --- write a single record
	if( DBL_FILEWRITE( pData, sizeof( *pData ), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord( const CLightRecord* pData, FILE* pFile ) - fwrite Error"));
}

void DBLWrite::WriteRecord(const ts_EnvSoundSource *pData, FILE *pFile)
{
	pData = DBLSwap::SwapEndian(pData);

	if (!pData)
		throw(PluginException( ERROR_FATAL,"WriteRecord(const ts_EnvSoundSource *pData, FILE *pFile) - SwapEndian Error"));

	// --- write a single record
	if (DBL_FILEWRITE(pData, sizeof(*pData), 1, pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_EnvSoundSource *pData, FILE *pFile) - fwrite Error"));
}

void DBLWrite::WriteRecord(const ts_EnvSoundField *pData, FILE *pFile)
{
	pData = DBLSwap::SwapEndian(pData);

	if (!pData)
		throw(PluginException( ERROR_FATAL,"WriteRecord(const ts_EnvSoundField *pData, FILE *pFile) - SwapEndian Error"));

	// --- write a single record
	if (DBL_FILEWRITE(pData, sizeof(*pData), 1, pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_EnvSoundField *pData, FILE *pFile) - fwrite Error"));
}

void DBLWrite::WriteRecord(const ts_EnvHalo *pData, FILE *pFile)
{
	pData = DBLSwap::SwapEndian(pData);

	if (!pData)
		throw(PluginException( ERROR_FATAL,"WriteRecord(const ts_EnvHalo *pData, FILE *pFile) - SwapEndian Error"));

	// --- write a single record
	if (DBL_FILEWRITE(pData, sizeof(*pData), 1, pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_EnvHalo *pData, FILE *pFile) - fwrite Error"));
}

void DBLWrite::WriteRecord(const ts_EnvStationaryCam *pData, FILE *pFile)
{
	pData = DBLSwap::SwapEndian(pData);

	if (!pData)
		throw(PluginException( ERROR_FATAL,"WriteRecord(const ts_EnvStaionaryCam *pData, FILE *pFile) - SwapEndian Error"));

	// --- write a single record
	if (DBL_FILEWRITE(pData, sizeof(*pData), 1, pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_EnvStationaryCam *pData, FILE *pFile) - fwrite Error"));
}

void DBLWrite::WriteRecord(const ts_Water *pData, FILE *pFile)
{
	pData = DBLSwap::SwapEndian(pData);

	if (!pData)
		throw(PluginException( ERROR_FATAL,"WriteRecord(const ts_Water *pData, FILE *pFile) - SwapEndian Error"));

	// --- write a single record
	if (DBL_FILEWRITE(pData, sizeof(*pData), 1, pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_Water *pData, FILE *pFile) - fwrite Error"));
}

void DBLWrite::WriteRecord(const ts_AnimCurve *pData, FILE *pFile)
{
	pData = DBLSwap::SwapEndian(pData);

	if (!pData)
		throw(PluginException( ERROR_FATAL,"WriteRecord(const ts_AnimCurve *pData, FILE *pFile) - SwapEndian Error"));

	// --- write a single record
	if (DBL_FILEWRITE(pData, sizeof(*pData), 1, pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteRecord(const ts_AnimCurve *pData, FILE *pFile) - fwrite Error"));
}

/***************************************************************************************************************/
/* write out multiple records-- one of these routines exists for each type of data record we want to write out */

void DBLWrite::WriteRecords(const u16 *pData, FILE *pFile, u32 u32Count)
{
	for (int i = 0; i < (signed) u32Count; i++, pData++)
	{
		try
		{
			DBLWrite::WriteRecord(pData, pFile);
		}
		catch (PluginException e)
		{
			throw(PluginException(ERROR_FATAL,"WriteRecords (const u16 *, Count = %d of %d) - WriteRecord Error\r\n  %s",i,u32Count,(const char *)(e)));
		}
	}
}

void DBLWrite::WriteRecords(const u32 *pData, FILE *pFile, u32 u32Count)
{
	for (int i = 0; i < (signed) u32Count; i++, pData++)
	{
		try
		{
			DBLWrite::WriteRecord(pData, pFile);
		}
		catch (PluginException e)
		{
			throw(PluginException(ERROR_FATAL,"WriteRecords (const u32 *, Count = %d of %d) - WriteRecord Error\r\n  %s",i,u32Count,(const char *)(e)));
		}
	}
}

void DBLWrite::WriteRecords(const float *pData, FILE *pFile, u32 u32Count)
{
	for (int i = 0; i < (signed) u32Count; i++, pData++)
	{
		try
		{
			DBLWrite::WriteRecord(pData, pFile);
		}
		catch (PluginException e)
		{
			throw(PluginException(ERROR_FATAL,"WriteRecords (const float *, Count = %d of %d) - WriteRecord Error\r\n  %s",i,u32Count,(const char *)(e)));
		}
	}
}
void DBLWrite::WriteRecords(const Vector2 *pData, FILE *pFile, u32 u32Count)
{
	for (int i = 0; i < (signed) u32Count; i++, pData++)
	{
		try
		{
			DBLWrite::WriteRecord(pData, pFile);
		}
		catch (PluginException e)
		{
			throw(PluginException(ERROR_FATAL,"WriteRecords (const Vector2 *, Count = %d of %d) - WriteRecord Error\r\n  %s",i,u32Count,(const char *)(e)));
		}
	}
}
void DBLWrite::WriteRecords(const Vector3 *pData, FILE *pFile, u32 u32Count)
{
	for (int i = 0; i < (signed) u32Count; i++, pData++)
	{
		try
		{
			DBLWrite::WriteRecord(pData, pFile);
		}
		catch (PluginException e)
		{
			throw(PluginException(ERROR_FATAL,"WriteRecords (const Vector3 *, Count = %d of %d) - WriteRecord Error\r\n  %s",i,u32Count,(const char *)(e)));
		}
	}
}
void DBLWrite::WriteRecords(const Vector4 *pData, FILE *pFile, u32 u32Count)
{
	for (int i = 0; i < (signed) u32Count; i++, pData++)
	{
		try
		{
			DBLWrite::WriteRecord(pData, pFile);
		}
		catch (PluginException e)
		{
			throw(PluginException(ERROR_FATAL,"WriteRecords (const Vector4 *, Count = %d of %d) - WriteRecord Error\r\n  %s",i,u32Count,(const char *)(e)));
		}
	}
}
void DBLWrite::WriteRecords(const ts_bRGBA *pData, FILE *pFile, u32 u32Count)
{
	for (int i = 0; i < (signed) u32Count; i++, pData++)
	{
		try
		{
			DBLWrite::WriteRecord(pData, pFile);
		}
		catch (PluginException e)
		{
			throw(PluginException(ERROR_FATAL,"WriteRecords (const ts_bRGBA *, Count = %d of %d) - WriteRecord Error\r\n  %s",i,u32Count,(const char *)(e)));
		}
	}
}
void DBLWrite::WriteRecords(const ts_ModelBone *pData, FILE *pFile, u32 u32Count)
{
	for (int i = 0; i < (signed) u32Count; i++, pData++)
	{
		try
		{
			DBLWrite::WriteRecord(pData, pFile);
		}
		catch (PluginException e)
		{
			throw(PluginException(ERROR_FATAL,"WriteRecords (const ts_ModelBone *, Count = %d of %d) - WriteRecord Error\r\n  %s",i,u32Count,(const char *)(e)));
		}
	}
}
void DBLWrite::WriteRecords(const ts_CollisionPoly *pData, FILE *pFile, u32 u32Count)
{
	for (int i = 0; i < (signed) u32Count; i++, pData++)
	{
		try
		{
			DBLWrite::WriteRecord(pData, pFile);
		}
		catch (PluginException e)
		{
			throw(PluginException(ERROR_FATAL,"WriteRecords (const ts_CollisionPoly *, Count = %d of %d) - WriteRecord Error\r\n  %s",i,u32Count,(const char *)(e)));
		}
	}
}
void DBLWrite::WriteRecords(const ts_TextureChunkRecord *pData, FILE *pFile, u32 u32Count)
{
	for (int i = 0; i < (signed) u32Count; i++, pData++)
	{
		try
		{
			DBLWrite::WriteRecord(pData, pFile);
		}
		catch (PluginException e)
		{
			throw(PluginException(ERROR_FATAL,"WriteRecords (const ts_TextureChunkRecord *, Count = %d of %d) - WriteRecord Error\r\n  %s",i,u32Count,(const char *)(e)));
		}
	}
}
void DBLWrite::WriteRecords(const ts_TextureLayer *pData, FILE *pFile, u32 u32Count)
{
	for (int i = 0; i < (signed) u32Count; i++, pData++)
	{
		try
		{
			DBLWrite::WriteRecord(pData, pFile);
		}
		catch (PluginException e)
		{
			throw(PluginException(ERROR_FATAL,"WriteRecords (const ts_TextureLayer *, Count = %d of %d) - WriteRecord Error\r\n  %s",i,u32Count,(const char *)(e)));
		}
	}
}
void DBLWrite::WriteRecords(const ts_PieceRecord *pData, FILE *pFile, u32 u32Count)
{
	for (int i = 0; i < (signed) u32Count; i++, pData++)
	{
		try
		{
			DBLWrite::WriteRecord(pData, pFile);
		}
		catch (PluginException e)
		{
			throw(PluginException(ERROR_FATAL,"WriteRecords (const ts_TextureLayer *, Count = %d of %d) - WriteRecord Error\r\n  %s",i,u32Count,(const char *)(e)));
		}
	}
}

#if 0
void DBLWrite::WriteRecords(const ts_TextureChunkVRAMInfoRecord  *pData, FILE *pFile, u32 u32Count)
{
	for (int i = 0; i < (signed) u32Count; i++, pData++)
	{
		try
		{
			DBLWrite::WriteRecord(pData, pFile);
		}
		catch (PluginException e)
		{
			throw(PluginException(ERROR_FATAL,"WriteRecords (const ts_TextureChunkVRAMInfoRecord *, Count = %d of %d) - WriteRecord Error\r\n  %s",i,u32Count,(const char *)(e)));
		}
	}
}
#endif

void DBLWrite::WriteRecords(const SMeshSOM::SNodeSOM *pData, FILE *pFile, u32 u32Count)
{
	for (int i = 0; i < (signed) u32Count; i++, pData++)
	{
		try
		{
			DBLWrite::WriteRecord(pData, pFile);
		}
		catch (PluginException e)
		{
			throw(PluginException(ERROR_FATAL,"WriteRecords (const SMeshSOM::SNodeSOM *, Count = %d of %d) - WriteRecord Error\r\n  %s",i,u32Count,(const char *)(e)));
		}
	}
}

void DBLWrite::WriteRecords(const SMeshSOM::SIndexPair *pData, FILE *pFile, u32 u32Count)
{
	for (int i = 0; i < (signed) u32Count; i++, pData++)
	{
		try
		{
			DBLWrite::WriteRecord(pData, pFile);
		}
		catch (PluginException e)
		{
			throw(PluginException(ERROR_FATAL,"WriteRecords (const SMeshSOM::SIndexPair *, Count = %d of %d) - WriteRecord Error\r\n  %s",i,u32Count,(const char *)(e)));
		}
	}
}

void DBLWrite::WriteRecords(const SMeshSOM::SIndexTrio *pData, FILE *pFile, u32 u32Count)
{
	for (int i = 0; i < (signed) u32Count; i++, pData++)
	{
		try
		{
			DBLWrite::WriteRecord(pData, pFile);
		}
		catch (PluginException e)
		{
			throw(PluginException(ERROR_FATAL,"WriteRecords (const SMeshSOM::SIndexTrio *, Count = %d of %d) - WriteRecord Error\r\n  %s",i,u32Count,(const char *)(e)));
		}
	}
}

void DBLWrite::WriteRecords(const SMeshSOM::SSubMesh *pData, FILE *pFile, u32 u32Count)
{
	for (int i = 0; i < (signed) u32Count; i++, pData++)
	{
		try
		{
			DBLWrite::WriteRecord(pData, pFile);
		}
		catch (PluginException e)
		{
			throw(PluginException(ERROR_FATAL,"WriteRecords (const SMeshSOM::SSubMesh *, Count = %d of %d) - WriteRecord Error\r\n  %s",i,u32Count,(const char *)(e)));
		}
	}
}

/* write out generic data */

void DBLWrite::WriteData(const void *pData, u32 u32Length, FILE *pFile)
{
	if (DBL_FILEWRITE(pData, u32Length, 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"WriteData - fwrite error"));
}

/***************************************************************************************************************/
/* and have to provide some read hooks too so that if we read data back in from a big-endian data file, it gets
converted back to little endian for use by ImportExport */

void DBLWrite::ReadRecord(ts_DBBaseChunkHdr *pData, FILE *pFile)
{
	/* read a single record */

	if (fread(pData, sizeof(ts_DBBaseChunkHdr), 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"ReadRecord - fread error"));

	/* swap endianness if necessary */

	const ts_DBBaseChunkHdr *pSwappedData = DBLSwap::SwapEndian(pData);
	if (pSwappedData != pData)	// if different, then it got swapped
		memcpy(pData, pSwappedData, sizeof(ts_DBBaseChunkHdr));

//	if (EXPORT_SWAP_ENDIAN)
//	{
//		/* convert each field, except for strings. u8 fields won't change, of course, but if */
//		/* they are expanded to u16 at some future time, conversion will still be correct */
//
//		DBLUtil__SwapEndianInPlace(&pData->usType);
//		DBLUtil__SwapEndianInPlace(&pData->usFlags);
//		DBLUtil__SwapEndianInPlace(&pData->u32Size);
//	}
}

/* read in generic data */

void DBLWrite::ReadData(void *pData, u32 u32Length, FILE *pFile)
{
	if (fread(pData, u32Length, 1,  pFile) != 1)
		throw(PluginException(ERROR_FATAL,"ReadData - fread error"));
}

