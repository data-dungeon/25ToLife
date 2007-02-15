////////////////////////////////////////////////////////////////////////////
//
// DBLSwap
//
// Class to handle the swapping endians of the DBL data
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: DBLSwap.cpp $
 * 
 * *****************  Version 42  *****************
 * User: Food         Date: 8/11/03    Time: 2:19p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * 
 * *****************  Version 41  *****************
 * User: Todd Blackburn Date: 8/09/03    Time: 3:42p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * 
 * *****************  Version 40  *****************
 * User: Todd Blackburn Date: 6/27/03    Time: 4:30p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * 
 * *****************  Version 39  *****************
 * User: Dwain Skinner Date: 6/25/03    Time: 7:46a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * 
 * *****************  Version 38  *****************
 * User: Adam Clayton Date: 6/16/03    Time: 10:07a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * 
 * *****************  Version 37  *****************
 * User: Dwain Skinner Date: 6/12/03    Time: 3:55p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * 
 * *****************  Version 36  *****************
 * User: Nmartin      Date: 5/28/03    Time: 7:49p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * processing adam's data-- creates .cut and curve .dbl file
 * 
 * *****************  Version 35  *****************
 * User: Nmartin      Date: 5/19/03    Time: 8:43a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * start of cutscene filter
 * 
 * *****************  Version 34  *****************
 * User: Adam Clayton Date: 5/02/03    Time: 8:15a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * 
 * *****************  Version 33  *****************
 * User: Adam Clayton Date: 4/29/03    Time: 1:49p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * 
 * *****************  Version 31  *****************
 * User: Dwain Skinner Date: 4/18/03    Time: 4:47p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * 
 * *****************  Version 30  *****************
 * User: Food         Date: 4/17/03    Time: 1:06p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * 
 * *****************  Version 29  *****************
 * User: Dwain Skinner Date: 4/15/03    Time: 8:04p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * 
 * *****************  Version 28  *****************
 * User: Dwain Skinner Date: 4/15/03    Time: 2:09p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * 
 * *****************  Version 27  *****************
 * User: Dwain Skinner Date: 4/14/03    Time: 6:32p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * 
 * *****************  Version 26  *****************
 * User: Adam Clayton Date: 4/04/03    Time: 2:25p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * new CVisRegion structure
 * 
 * *****************  Version 25  *****************
 * User: Dwain Skinner Date: 4/04/03    Time: 11:23a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * 
 * *****************  Version 24  *****************
 * User: Dwain Skinner Date: 3/27/03    Time: 2:45p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * 
 * *****************  Version 23  *****************
 * User: Dwain Skinner Date: 3/25/03    Time: 1:25p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * 
 * *****************  Version 22  *****************
 * User: Adam Clayton Date: 3/12/03    Time: 11:48a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * update for streaming system
 * 
 * *****************  Version 21  *****************
 * User: Dwain Skinner Date: 2/27/03    Time: 12:11p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * 
 * *****************  Version 20  *****************
 * User: Dwain Skinner Date: 2/26/03    Time: 7:12p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * 
 * *****************  Version 19  *****************
 * User: Food         Date: 1/14/03    Time: 3:39p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * 
 * *****************  Version 18  *****************
 * User: Adam Clayton Date: 12/02/02   Time: 8:07a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * added swapendianinplace for bounding box
 * 
 * *****************  Version 17  *****************
 * User: Nmartin      Date: 11/27/02   Time: 2:11p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * endian swap mBoneBox for model bone
 * 
 * *****************  Version 16  *****************
 * User: Adam Clayton Date: 11/07/02   Time: 4:09p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * more fixes...
 * 
 * *****************  Version 15  *****************
 * User: Rob Nelson   Date: 9/16/02    Time: 3:49p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * 
 * *****************  Version 14  *****************
 * User: Adam Clayton Date: 8/13/02    Time: 10:00a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * ts_DBChunkHdr & ts_DBBaseChunkHdr sizes
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 7/08/02    Time: 3:58p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * Added Mesh Som classes
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 6/18/02    Time: 9:54a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * ts_EmitterSource changes
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 5/17/02    Time: 11:21a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * added Gamecube tristrip header
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 5/13/02    Time: 4:08p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * test swap issues
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 5/03/02    Time: 9:20a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * Emmitter update
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 5/01/02    Time: 12:10p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * particle size
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 4/29/02    Time: 3:57p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * Particle update
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 3/25/02    Time: 12:41p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * hybrid configuration
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 3/15/02    Time: 3:19p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * Bones Fix
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 2/20/02    Time: 2:50p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * Exceptions working properly
 * Log working with all plugins
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 2/12/02    Time: 5:11p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/07/02    Time: 5:17p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/DBLUtils
 * Added library for handling the dbl file format (taken from Import
 * Export)
 */

#include "DBLUtilsPch.h"
#include "DBLSwap.h"
#include <Math/Quatern.h>
#include <Game/Database/MeshSOM.h>
#include <Game/Database/VisRegion.h>
#include <audio/environment/envdbl.h>
#include <effects/halo/halodbl.h>
#include <Camera/StationaryCam/StationaryCamDbl.h>
#include <effects/water/waterdbl.h>
#include <GeomUtil/AnimCurve.h>
#include <Game/Managers/TextureMgr/TextureDbl.h>

bool DBLSwap::m_swapEndianActive=false; // defaults to off
u8 DBLSwap::m_bigEndian8Buff[8];

/******************* functions ***************************/

/* check size of record. we insist that the record is of the expected size. this is to prevent errors in the endian
swapping processing.

NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed so
that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue is
that the tools need to behave the same regardless of whose machine they are compiled on */

bool DBLSwap::CheckRecordSize(const char *pName, u32 u32HardwiredSize, u32 u32ActualSize)
{
	if (u32HardwiredSize != u32ActualSize)
	{
		char acMessage[200];
#if MMX_OK
		sprintf(acMessage, "DBLSwap::WriteRecord: %s; hard-wired size: %d; actual size: %d", pName, u32HardwiredSize, u32ActualSize);
#else //MMX_OK
		sprintf(acMessage, "DBLSwap::WriteRecord: MMX_OK error? %s; hard-wired size: %d; actual size: %d", pName, u32HardwiredSize, u32ActualSize);
#endif //MMX_OK
//		AddConditionalInfoToErrorLog(acMessage);
		assert(FALSE);
		return(FALSE);
	}
	return(TRUE);
}

/***************************************************************/
/* overloaded basic data type handlers-- switch value in place */

void DBLSwap::SwapEndianInPlace(u8 *pValue)
{
}
void DBLSwap::SwapEndianInPlace(s8 *pValue)
{
}
void DBLSwap::SwapEndianInPlace(bool *pValue)
{
}
void DBLSwap::SwapEndianInPlace(u16 *pValue)
{
	SwapEndian2(pValue);
}
void DBLSwap::SwapEndianInPlace(s16 *pValue)
{
	SwapEndian2(pValue);
}
void DBLSwap::SwapEndianInPlace(u32 *pValue)
{
	SwapEndian4(pValue);
}
void DBLSwap::SwapEndianInPlace(s32 *pValue)
{
	SwapEndian4(pValue);
}
void DBLSwap::SwapEndianInPlace(int *pValue)
{
	SwapEndian4(pValue);
}
void DBLSwap::SwapEndianInPlace(u64 *pValue)	// handles s64 too, since they are both declared as LARGE_INTEGER
{
	SwapEndian8(pValue);
}
void DBLSwap::SwapEndianInPlace(float *pValue)
{
	SwapEndian4(pValue);
}
void DBLSwap::SwapEndianInPlace(Vector2 *pValue)
{
	DBLSwap::SwapEndianInPlace(&pValue->_x);
	DBLSwap::SwapEndianInPlace(&pValue->_y);
}
void DBLSwap::SwapEndianInPlace(Vector3 *pValue)
{
	DBLSwap::SwapEndianInPlace(&pValue->_x);
	DBLSwap::SwapEndianInPlace(&pValue->_y);
	DBLSwap::SwapEndianInPlace(&pValue->_z);
}
void DBLSwap::SwapEndianInPlace(Vector4Packed *pValue)
{
	float temp;
	temp = pValue->x();	DBLSwap::SwapEndianInPlace(&temp); pValue->x(temp);
	temp = pValue->y();	DBLSwap::SwapEndianInPlace(&temp); pValue->y(temp);
	temp = pValue->z();	DBLSwap::SwapEndianInPlace(&temp); pValue->z(temp);
	temp = pValue->w();	DBLSwap::SwapEndianInPlace(&temp); pValue->w(temp);
}
void DBLSwap::SwapEndianInPlace(Vector4Aligned *pValue)
{
	float temp;
	temp = pValue->x();	DBLSwap::SwapEndianInPlace(&temp); pValue->x(temp);
	temp = pValue->y();	DBLSwap::SwapEndianInPlace(&temp); pValue->y(temp);
	temp = pValue->z();	DBLSwap::SwapEndianInPlace(&temp); pValue->z(temp);
	temp = pValue->w();	DBLSwap::SwapEndianInPlace(&temp); pValue->w(temp);
}
void DBLSwap::SwapEndianInPlace(DirCos3x3 *pValue)
{
	DBLSwap::SwapEndianInPlace(&pValue->e00);
	DBLSwap::SwapEndianInPlace(&pValue->e01);
	DBLSwap::SwapEndianInPlace(&pValue->e02);
	DBLSwap::SwapEndianInPlace(&pValue->e10);
	DBLSwap::SwapEndianInPlace(&pValue->e11);
	DBLSwap::SwapEndianInPlace(&pValue->e12);
	DBLSwap::SwapEndianInPlace(&pValue->e20);
	DBLSwap::SwapEndianInPlace(&pValue->e21);
	DBLSwap::SwapEndianInPlace(&pValue->e22);
}
void DBLSwap::SwapEndianInPlace(Graphics4x4 *pValue)
{
	DBLSwap::SwapEndianInPlace(&pValue->r00);
	DBLSwap::SwapEndianInPlace(&pValue->r01);
	DBLSwap::SwapEndianInPlace(&pValue->r02);
	DBLSwap::SwapEndianInPlace(&pValue->e03);
	DBLSwap::SwapEndianInPlace(&pValue->r10);
	DBLSwap::SwapEndianInPlace(&pValue->r11);
	DBLSwap::SwapEndianInPlace(&pValue->r12);
	DBLSwap::SwapEndianInPlace(&pValue->e13);
	DBLSwap::SwapEndianInPlace(&pValue->r20);
	DBLSwap::SwapEndianInPlace(&pValue->r21);
	DBLSwap::SwapEndianInPlace(&pValue->r22);
	DBLSwap::SwapEndianInPlace(&pValue->e23);
	DBLSwap::SwapEndianInPlace(&pValue->tx);
	DBLSwap::SwapEndianInPlace(&pValue->ty);
	DBLSwap::SwapEndianInPlace(&pValue->tz);
	DBLSwap::SwapEndianInPlace(&pValue->w);
}
void DBLSwap::SwapEndianInPlace(ts_bRGBA *pValue)
{
	/* nothing to do */
}
void DBLSwap::SwapEndianInPlace(ts_fRGB *pValue)
{
	DBLSwap::SwapEndianInPlace(&pValue->R);
	DBLSwap::SwapEndianInPlace(&pValue->G);
	DBLSwap::SwapEndianInPlace(&pValue->B);
}
void DBLSwap::SwapEndianInPlace(ts_Quatvw *pValue)
{
	DBLSwap::SwapEndianInPlace(&pValue->V);
	DBLSwap::SwapEndianInPlace(&pValue->W);
}
void DBLSwap::SwapEndianInPlace(ts_MaterialBitfield *pValue)
{
	/* size of bitfield may change */

	switch(sizeof(ts_MaterialBitfield))
	{
	case 1:
		break;
	case 2:
		SwapEndian2(pValue);
		break;
	case 4:
		SwapEndian4(pValue);
		break;
	default:
		assert(FALSE);
		break;
	}
}
void DBLSwap::SwapEndianInPlace(ts_BoundingBox *pValue)
{
	DBLSwap::SwapEndianInPlace(&pValue->Min);
	DBLSwap::SwapEndianInPlace(&pValue->Max);
}

/* a test routine to do a swap on all known types. this should be updated if any new types are added. this test serves two
purposes. first, it checks that the values we pass to DBLSwap::CheckRecordSize are correct, and it can be used to make
sure that the status of MMX_OK doesn't change the record size */

void DBLSwap::TestSwapEndian(void)
{
	{	int Test;										DBLSwap::SwapEndian(&Test);	}
	{	float Test;										DBLSwap::SwapEndian(&Test);	}
	{	u16 Test;										DBLSwap::SwapEndian(&Test);	}
	{	s16 Test;										DBLSwap::SwapEndian(&Test);	}
	{	u32 Test;										DBLSwap::SwapEndian(&Test);	}
	{	s32 Test;										DBLSwap::SwapEndian(&Test);	}
	{	u64 Test;										DBLSwap::SwapEndian(&Test);	}
	{	Vector2 Test;									DBLSwap::SwapEndian(&Test);	}
	{	Vector3 Test;									DBLSwap::SwapEndian(&Test);	}
	{	Vector4 Test;									DBLSwap::SwapEndian(&Test);	}
	{	ts_ModelBone Test;							DBLSwap::SwapEndian(&Test);	}
	{	ts_BoneInfo Test;								DBLSwap::SwapEndian(&Test);	}
	{	ts_GroupBoneMap Test;						DBLSwap::SwapEndian(&Test);	}
	{	ts_LeftRightMap Test;						DBLSwap::SwapEndian(&Test);	}
	{	ts_RecordList Test;							DBLSwap::SwapEndian(&Test);	}
	{	ts_DBFileHdr Test;							DBLSwap::SwapEndian(&Test);	}
	{	ts_DBBaseChunkHdr Test;						DBLSwap::SwapEndian(&Test);	}
	{	ts_DBChunkHdr Test;							DBLSwap::SwapEndian(&Test);	}
	{	ts_CollisionSOM Test;						DBLSwap::SwapEndian(&Test);	}
	{	ts_CollisionPoly Test; Test.u16VertexCount = 0; 	DBLSwap::SwapEndian(&Test);	}
	{	ts_bRGBA Test;									DBLSwap::SwapEndian(&Test);	}
	{	ts_PatchSOM Test;								DBLSwap::SwapEndian(&Test);	}
	{	ts_Patch Test;									DBLSwap::SwapEndian(&Test);	}
	{	ts_ExpansionTableHeader Test;				DBLSwap::SwapEndian(&Test);	}
	{	ts_Group Test;									DBLSwap::SwapEndian(&Test);	}
	{	CInstance Test;								DBLSwap::SwapEndian(&Test);	}
	{	ts_LOD Test;									DBLSwap::SwapEndian(&Test);	}
	{	ts_PieceRecord Test;							DBLSwap::SwapEndian(&Test);	}
	{	ts_TextureChunkHeader Test;				DBLSwap::SwapEndian(&Test);	}
	{	ts_AnimatedTextureChunkHeader Test;		DBLSwap::SwapEndian(&Test);	}
	{	ts_TextureChunkRecord Test;				DBLSwap::SwapEndian(&Test);	}
	{	ts_TextureContext Test;						DBLSwap::SwapEndian(&Test);	}
	{	ts_TextureLayer Test;						DBLSwap::SwapEndian(&Test);	}
	{	ts_TriStripHeader Test;						DBLSwap::SwapEndian(&Test);	}
	{	ts_GCNTriStripHeader Test;						DBLSwap::SwapEndian(&Test);	}
	{	ts_EmitterSource Test;						DBLSwap::SwapEndian(&Test);	}
	{	ts_Effect Test;								DBLSwap::SwapEndian(&Test);	}
	{	ts_Channel Test;								DBLSwap::SwapEndian(&Test);	}
	//{	ts_ParticleSource Test;						DBLSwap::SwapEndian(&Test);	}
	{	CSceneNode Test;								DBLSwap::SwapEndian(&Test);	}
	{	ts_BoundingBox Test;							DBLSwap::SwapEndian(&Test);	}
	{	ts_BoundingSphere Test;						DBLSwap::SwapEndian(&Test);	}
}

/***************************************************************/
/* overloaded record endian swappers-- return pointer to original */
/* data if not swapped, or to BigEndian record if swapped */

const int *DBLSwap::SwapEndian(const int *pData)
{
	static int BigEndianData;	// pointer to this may be returned, must be static

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace(&BigEndianData);

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

const float *DBLSwap::SwapEndian(const float *pData)
{
	static float BigEndianData;	// pointer to this may be returned, must be static

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace(&BigEndianData);

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

const u16 *DBLSwap::SwapEndian(const u16 *pData)
{
	static u16 BigEndianData;	// pointer to this may be returned, must be static

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace(&BigEndianData);

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

const s16 *DBLSwap::SwapEndian(const s16 *pData)
{
	static s16 BigEndianData;	// pointer to this may be returned, must be static

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace(&BigEndianData);

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

const u32 *DBLSwap::SwapEndian(const u32 *pData)
{
	static u32 BigEndianData;	// pointer to this may be returned, must be static

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace(&BigEndianData);

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

const s32 *DBLSwap::SwapEndian(const s32 *pData)
{
	static s32 BigEndianData;	// pointer to this may be returned, must be static

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace(&BigEndianData);

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

const u64 *DBLSwap::SwapEndian(const u64 *pData)
{
	static u64 BigEndianData;	// pointer to this may be returned, must be static

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace(&BigEndianData);

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

const Vector2 *DBLSwap::SwapEndian(const Vector2 *pData)
{
	static Vector2 BigEndianData;	// pointer to this may be returned, must be static

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace(&BigEndianData);

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

const Vector3 *DBLSwap::SwapEndian(const Vector3 *pData)
{
	static Vector3 BigEndianData;	// pointer to this may be returned, must be static

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace(&BigEndianData);

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

const Vector4 *DBLSwap::SwapEndian(const Vector4 *pData)
{
	static Vector4 BigEndianData;	// pointer to this may be returned, must be static

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace(&BigEndianData);

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

const ts_ModelBone *DBLSwap::SwapEndian(const ts_ModelBone *pData)
{
	static ts_ModelBone BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLSwap::CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("ts_ModelBone", 240, sizeof(BigEndianData)) == FALSE)
		return(NULL);

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace((u32 *) &BigEndianData.pChildren);	// convert pointer as u32 in case it contains offset to be fixed up at runtime
		DBLSwap::SwapEndianInPlace((u32 *) &BigEndianData.pSiblings);	// convert pointer as u32 in case it contains offset to be fixed up at runtime
		DBLSwap::SwapEndianInPlace(&BigEndianData.usStartVtxIdx);
		DBLSwap::SwapEndianInPlace(&BigEndianData.usNumVtxIdxs);
		DBLSwap::SwapEndianInPlace(&BigEndianData.usStartCtlVtxIdx);
		DBLSwap::SwapEndianInPlace(&BigEndianData.usNumCtlVtxIdxs);
		DBLSwap::SwapEndianInPlace(&BigEndianData.usStartNrmlIdx);
		DBLSwap::SwapEndianInPlace(&BigEndianData.usNumNrmlIdxs);
		DBLSwap::SwapEndianInPlace(&BigEndianData.boneGroupID);
		DBLSwap::SwapEndianInPlace(&BigEndianData.boneID);
		DBLSwap::SwapEndianInPlace(&BigEndianData.bOrientXYZ);
		DBLSwap::SwapEndianInPlace(&BigEndianData.ucOrientLikeValue);
		DBLSwap::SwapEndianInPlace(&BigEndianData.ucOrient2LikeValue);
		DBLSwap::SwapEndianInPlace(&BigEndianData.ucRollLikeValue);
		DBLSwap::SwapEndianInPlace(&BigEndianData.OrientBone);
		DBLSwap::SwapEndianInPlace(&BigEndianData.Orient2Bone);
		DBLSwap::SwapEndianInPlace(&BigEndianData.vPivot);
		DBLSwap::SwapEndianInPlace(&BigEndianData.vBoneEnd);
		// skip cBoneName string
		DBLSwap::SwapEndianInPlace((u32 *) &BigEndianData.pNextBoneInGroup);	// convert pointer as u32 in case it contains offset to be fixed up at runtime
		DBLSwap::SwapEndianInPlace(&BigEndianData.boneToModel);
		DBLSwap::SwapEndianInPlace(&BigEndianData.m_bonetostandard);
		DBLSwap::SwapEndianInPlace((u32 *) &BigEndianData.m_parent);				// convert pointer as u32 in case it contains offset to be fixed up at runtime
		DBLSwap::SwapEndianInPlace(&BigEndianData.fBoneLen);
		DBLSwap::SwapEndianInPlace(&BigEndianData.mBoneBox);

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

const ts_BoneInfo *DBLSwap::SwapEndian(const ts_BoneInfo *pData)
{
	static ts_BoneInfo BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLSwap::CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("ts_BoneInfo", 12, sizeof(BigEndianData)) == FALSE)
		return(NULL);

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace((u16 *) &BigEndianData.numGBEntries);	// convert pointer as u32 in case it contains offset to be fixed up at runtime
		DBLSwap::SwapEndianInPlace((u16 *) &BigEndianData.numLREntries);	// convert pointer as u32 in case it contains offset to be fixed up at runtime
		// skip cBoneName string
		DBLSwap::SwapEndianInPlace((u32 *) &BigEndianData.pGroupBoneMap);	// convert pointer as u32 in case it contains offset to be fixed up at runtime
		DBLSwap::SwapEndianInPlace((u32 *) &BigEndianData.pLeftRightMap);	// convert pointer as u32 in case it contains offset to be fixed up at runtime

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

const ts_GroupBoneMap *DBLSwap::SwapEndian(const ts_GroupBoneMap *pData)
{
	static ts_GroupBoneMap BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLSwap::CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("ts_GroupBoneMap", 17, sizeof(BigEndianData)) == FALSE)
		return(NULL);

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace((BoneGroupID *) &BigEndianData.boneGroupID);	// convert pointer as u32 in case it contains offset to be fixed up at runtime
		// skip cBoneName string

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

const ts_LeftRightMap *DBLSwap::SwapEndian(const ts_LeftRightMap *pData)
{
	static ts_LeftRightMap BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLSwap::CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("ts_LeftRightMap", 32, sizeof(BigEndianData)) == FALSE)
		return(NULL);

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		// skip cBoneName1 string
		// skip cBoneName2 string

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

const ts_RecordList *DBLSwap::SwapEndian(const ts_RecordList *pData)
{
	static ts_RecordList BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLSwap::CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("ts_RecordList", 8, sizeof(BigEndianData)) == FALSE)
		return(NULL);

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace(&BigEndianData.usRecordCount);
		DBLSwap::SwapEndianInPlace(&BigEndianData.usFlags);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32RecordSize);	// might be converting u32OffsetTable[0] instead depending on type of record list

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

const ts_DBFileHdr *DBLSwap::SwapEndian(const ts_DBFileHdr *pData)
{
	static ts_DBFileHdr BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLSwap::CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("ts_DBFileHdr", 32, sizeof(BigEndianData)) == FALSE)
		return(NULL);

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		// skip cID string
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32Version);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32FileFlags);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32Chunks);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32MaxChunkSize);

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

const ts_DBBaseChunkHdr *DBLSwap::SwapEndian(const ts_DBBaseChunkHdr *pData)
{
	static ts_DBBaseChunkHdr BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLSwap::CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("ts_DBBaseChunkHdr", 32, sizeof(BigEndianData)) == FALSE)
		return(NULL);

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace(&BigEndianData.u16Type);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u16Flags);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32Size);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u16Version);

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

const ts_DBChunkHdr *DBLSwap::SwapEndian(const ts_DBChunkHdr *pData)
{
	static ts_DBChunkHdr BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLSwap::CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("ts_DBChunkHdr", 64, sizeof(BigEndianData)) == FALSE)
		return(NULL);

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace(&BigEndianData.u16Type);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u16Flags);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32Size);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u16Version);
		// skip cName string

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

const ts_CollisionSOM *DBLSwap::SwapEndian(const ts_CollisionSOM *pData)
{
	static ts_CollisionSOM BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLSwap::CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("ts_CollisionSOM", 100, sizeof(BigEndianData)) == FALSE)
		return(NULL);

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace(&BigEndianData.dh.u8DataType);
		DBLSwap::SwapEndianInPlace(&BigEndianData.dh.u8Flags2);
		DBLSwap::SwapEndianInPlace(&BigEndianData.dh.u16Flags);
		DBLSwap::SwapEndianInPlace(&BigEndianData.BoundingSphere.Center);
		DBLSwap::SwapEndianInPlace(&BigEndianData.BoundingSphere.Radius);
		DBLSwap::SwapEndianInPlace(&BigEndianData.BoundingBox.Min);
		DBLSwap::SwapEndianInPlace(&BigEndianData.BoundingBox.Max);
		// skip Name string
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32VertexCount);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32VertexOffset);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32NormalCount);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32NormalOffset);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32PolyCount);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32PolyOffset);

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

const ts_CollisionPoly *DBLSwap::SwapEndian(const ts_CollisionPoly *pData)
{
	static ts_CollisionPoly BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLSwap::CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("ts_CollisionPoly", 36, sizeof(BigEndianData)) == FALSE)
		return(NULL);

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		for (int i = 0; i < (signed) pData->u16VertexCount; i++)
		{
			DBLSwap::SwapEndianInPlace(&BigEndianData.u16VertexIndex[i]);
			DBLSwap::SwapEndianInPlace(&BigEndianData.u16NormalIndex[i]);
		}
		DBLSwap::SwapEndianInPlace(&BigEndianData.u16VertexCount);
		DBLSwap::SwapEndianInPlace(&BigEndianData.MaterialBitfield);
		// skip SourcePatchName string

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

const ts_bRGBA *DBLSwap::SwapEndian(const ts_bRGBA *pData)
{
	/* nothing to do */

	return(pData);
}

const ts_PatchSOM *DBLSwap::SwapEndian(const ts_PatchSOM *pData)
{
	static ts_PatchSOM BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLSwap::CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("ts_PatchSOM", 148, sizeof(BigEndianData)) == FALSE)
		return(NULL);

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace(&BigEndianData.dh.u8DataType);
		DBLSwap::SwapEndianInPlace(&BigEndianData.dh.u8Flags2);
		DBLSwap::SwapEndianInPlace(&BigEndianData.dh.u16Flags);
		DBLSwap::SwapEndianInPlace(&BigEndianData.BoundingSphere.Center);
		DBLSwap::SwapEndianInPlace(&BigEndianData.BoundingSphere.Radius);
		DBLSwap::SwapEndianInPlace(&BigEndianData.BoundingBox.Min);
		DBLSwap::SwapEndianInPlace(&BigEndianData.BoundingBox.Max);
		DBLSwap::SwapEndianInPlace((u32 *) &BigEndianData.pBody);	// convert pointer as u32 in case it contains offset to be fixed up at runtime
		// skip Name string
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32VertexCount);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32VertexOffset);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32PatchCount);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32PatchOffset);
		DBLSwap::SwapEndianInPlace((u32 *) &BigEndianData.u32TextureSubSetIndex);	// convert pointer as u32 in case it contains offset to be fixed up at runtime
		DBLSwap::SwapEndianInPlace((u32 *) &BigEndianData.pTextureArray);	// convert pointer as u32 in case it contains offset to be fixed up at runtime
		DBLSwap::SwapEndianInPlace((u32 *) &BigEndianData.DisplayListRecord.pDisplayList);	// convert pointer as u32 in case it contains offset to be fixed up at runtime
		DBLSwap::SwapEndianInPlace(&BigEndianData.DisplayListRecord.u16PolyCount);
		DBLSwap::SwapEndianInPlace(&BigEndianData.DisplayListRecord.u16QuadWordCount);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32TriStripVertexColorCount);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32TriStripCount);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32TriStripOffset);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32ControlVertexCount);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32ControlVertexOffset);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32NormalCount);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32NormalOffset);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32UVCount);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32UVOffset);

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

const ts_Patch *DBLSwap::SwapEndian(const ts_Patch *pData)
{
	static ts_Patch BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLSwap::CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("ts_Patch", 112, sizeof(BigEndianData)) == FALSE)
		return(NULL);

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace(&BigEndianData.usFlags);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32TextureContextIndex);
		DBLSwap::SwapEndianInPlace(&BigEndianData.MaterialBitfield);
		for (int i = 0; i < 4; i++)
			DBLSwap::SwapEndianInPlace(&BigEndianData.usVertexIndex[i]);
		for (i = 0; i < 4; i++)
			DBLSwap::SwapEndianInPlace(&BigEndianData.usNormalIndex[i]);
		for (i = 0; i < 8; i++)
			DBLSwap::SwapEndianInPlace(&BigEndianData.usControlVertexIndex[i]);
		for (i = 0; i < MAX_SOM_TEXTURES_PER_PATCH; i++)
		{
			DBLSwap::SwapEndianInPlace(&BigEndianData.usUV0Index[i]);
			DBLSwap::SwapEndianInPlace(&BigEndianData.usUV1Index[i]);
			DBLSwap::SwapEndianInPlace(&BigEndianData.usUV2Index[i]);
			DBLSwap::SwapEndianInPlace(&BigEndianData.usUV3Index[i]);
		}
		DBLSwap::SwapEndianInPlace(&BigEndianData.ucBoundingBoxXIndex);
		DBLSwap::SwapEndianInPlace(&BigEndianData.ucBoundingBoxYIndex);
		DBLSwap::SwapEndianInPlace(&BigEndianData.ucBoundingBoxZIndex);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u8Opacity);
		for (i = 0; i < S_SHAPED_MAX_ENTRIES; i++)
			DBLSwap::SwapEndianInPlace(&BigEndianData.usSShapedNormalIndex[i]);
		// skip Name string

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

const ts_ExpansionTableHeader *DBLSwap::SwapEndian(const ts_ExpansionTableHeader *pData)
{
	static ts_ExpansionTableHeader BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLSwap::CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("ts_ExpansionTableHeader", 12, sizeof(BigEndianData)) == FALSE)
		return(NULL);

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace(&BigEndianData.u32ExpansionTableXSize);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32ExpansionTableYSize);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32ExpansionTableZSize);

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

const ts_Group *DBLSwap::SwapEndian(const ts_Group *pData)
{
	static ts_Group BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLSwap::CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("ts_Group", 64, sizeof(BigEndianData)) == FALSE)
		return(NULL);

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace(&BigEndianData.BoundingSphere.Center);
		DBLSwap::SwapEndianInPlace(&BigEndianData.BoundingSphere.Radius);
		DBLSwap::SwapEndianInPlace(&BigEndianData.BoundingBox.Min);
		DBLSwap::SwapEndianInPlace(&BigEndianData.BoundingBox.Max);
		DBLSwap::SwapEndianInPlace(&BigEndianData.usDispStatInst);
		DBLSwap::SwapEndianInPlace(&BigEndianData.usCollStatInst);
		DBLSwap::SwapEndianInPlace((u32 *) &BigEndianData.ppDispStatArray);	// convert pointer as u32 in case it contains offset to be fixed up at runtime
		DBLSwap::SwapEndianInPlace((u32 *) &BigEndianData.pDispDynList);		// convert pointer as u32 in case it contains offset to be fixed up at runtime
		DBLSwap::SwapEndianInPlace((u32 *) &BigEndianData.ppCollStatArray);	// convert pointer as u32 in case it contains offset to be fixed up at runtime
		DBLSwap::SwapEndianInPlace((u32 *) &BigEndianData.pCollDynList);		// convert pointer as u32 in case it contains offset to be fixed up at runtime
		DBLSwap::SwapEndianInPlace(&BigEndianData.u8Flags);
		// skip Pad

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

const CInstance *DBLSwap::SwapEndian(const CInstance *pData)
{
	if (DBLSwap::CheckRecordSize("CInstance", 224, sizeof(CInstance)) == FALSE)
		return NULL;

	if (GetSwapEndianActive())
	{
		// leave original data intact, since it may get referred to later
		static CInstance BigEndianData;
		memcpy( &BigEndianData, pData, sizeof(BigEndianData) );

		DBLSwap::SwapEndianInPlace( &BigEndianData.d_localToWorld );
		DBLSwap::SwapEndianInPlace( &BigEndianData.d_boundingSphere.Center );
		DBLSwap::SwapEndianInPlace( &BigEndianData.d_boundingSphere.Radius );
		DBLSwap::SwapEndianInPlace( &BigEndianData.d_boundingBox );
		DBLSwap::SwapEndianInPlace( &BigEndianData.d_flags );
		DBLSwap::SwapEndianInPlace( &BigEndianData.d_viewFlags );
		DBLSwap::SwapEndianInPlace( &BigEndianData.d_lightingBits );
		DBLSwap::SwapEndianInPlace( &BigEndianData.d_alpha );
		DBLSwap::SwapEndianInPlace( &BigEndianData.d_renderBucket );
		DBLSwap::SwapEndianInPlace( (int*)&BigEndianData.d_SOM );
//		DBLSwap::SwapEndianInPlace( &BigEndianData.d_dynamicObject );				// in-game only
		DBLSwap::SwapEndianInPlace( &BigEndianData.d_fadeOutDistance );
		DBLSwap::SwapEndianInPlace( &BigEndianData.d_lastRenderFrame );
//		DBLSwap::SwapEndianInPlace( &BigEndianData.d_textureAnimatorArray );		// in-game only
//		DBLSwap::SwapEndianInPlace( &BigEndianData.d_displayList );					// in-game only
		DBLSwap::SwapEndianInPlace( (int*)&BigEndianData.d_vertexColors );
		DBLSwap::SwapEndianInPlace( &BigEndianData.d_colorSet );
//		DBLSwap::SwapEndianInPlace( &BigEndianData.d_body );
//		DBLSwap::SwapEndianInPlace( &BigEndianData.d_curLightInfluences );		// in-game only
		DBLSwap::SwapEndianInPlace( &BigEndianData.d_attenuationScale );
//		DBLSwap::SwapEndianInPlace( &BigEndianData.d_owner );							// in-game only
		DBLSwap::SwapEndianInPlace( &BigEndianData.d_uniqueID );

		pData = &BigEndianData;
	}

	return pData;
}


//	static CInstance BigEndianData;	// pointer to this may be returned, must be static
//
//	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
//	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLSwap::CheckRecordSize), and make */
//	/* sure all fields are in the endian swapping list */
//	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
//	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
//	/* is that the tools need to behave the same regardless of whose machine they are compiled on */
//
//	if (DBLSwap::CheckRecordSize("CInstance", 176, sizeof(BigEndianData)) == FALSE)
//		return(NULL);
//
//	/* swap endianness if necessary */
//
//	if (GetSwapEndianActive())
//	{
//		/* leave original data intact, since it may get referred to again later (for example, */
//		/* headers are often written out twice, once as a placeholder, and again when complete) */
//
//		memcpy(&BigEndianData, pData, sizeof(BigEndianData));
//
//		/* convert each field, except for strings. u8 fields won't change, of course, but if */
//		/* they are expanded to u16 at some future time, conversion will still be correct */
//
//		DBLSwap::SwapEndianInPlace(&BigEndianData.dh.u8DataType);
//		DBLSwap::SwapEndianInPlace(&BigEndianData.dh.u8Flags2);
//		DBLSwap::SwapEndianInPlace(&BigEndianData.dh.u16Flags);
//		for (int i = 0; i < MAX_LODS; i++)
//			DBLSwap::SwapEndianInPlace(&BigEndianData.u32VertexColorsOffset[i]);
//		DBLSwap::SwapEndianInPlace((u32 *) &BigEndianData.pBody);	// convert pointer as u32 in case it contains offset to be fixed up at runtime
//		DBLSwap::SwapEndianInPlace(&BigEndianData.mLocalToWorld);
//		DBLSwap::SwapEndianInPlace(&BigEndianData.BoundingSphere.Center);
//		DBLSwap::SwapEndianInPlace(&BigEndianData.BoundingSphere.Radius);
//		DBLSwap::SwapEndianInPlace(&BigEndianData.BoundingBox.Min);
//		DBLSwap::SwapEndianInPlace(&BigEndianData.BoundingBox.Max);
//		DBLSwap::SwapEndianInPlace(&BigEndianData.u32SOMIndex);	// might be converting u32LODIndex depending on data
//		for (i = 0; i < MAX_LODS; i++)
//		{
//			DBLSwap::SwapEndianInPlace((u32 *) &BigEndianData.DisplayListRecord[i].pDisplayList);	// convert pointer as u32 in case it contains offset to be fixed up at runtime
//			DBLSwap::SwapEndianInPlace(&BigEndianData.DisplayListRecord[i].u16PolyCount);
//			DBLSwap::SwapEndianInPlace(&BigEndianData.DisplayListRecord[i].u16QuadWordCount);
//		}
//		DBLSwap::SwapEndianInPlace(&BigEndianData.fConstantAlpha);
//		DBLSwap::SwapEndianInPlace((u32 *) &BigEndianData.pTextureAnimatorArray);
//		DBLSwap::SwapEndianInPlace(&BigEndianData.u32InstancePart2Offset);
//		DBLSwap::SwapEndianInPlace(&BigEndianData.u16NumColorSets);
//		DBLSwap::SwapEndianInPlace(&BigEndianData.m_lightingBits);
//		DBLSwap::SwapEndianInPlace(&BigEndianData.m_attenuationScale);
//		
//		/* point to converted data */
//
//		pData = &BigEndianData;
//	}
//
//	/* return pointer to data */
//
//	return(pData);
//}

const ts_LOD *DBLSwap::SwapEndian(const ts_LOD *pData)
{
	static ts_LOD BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLSwap::CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("ts_LOD", 12, sizeof(BigEndianData)) == FALSE)
		return(NULL);

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		for (int i = 0; i < MAX_LODS; i++)
			DBLSwap::SwapEndianInPlace(&BigEndianData.u32SOMIndex[i]);
		for (i = 0; i < MAX_LODS - 1; i++)
			DBLSwap::SwapEndianInPlace(&BigEndianData.fSwitchRange[i]);

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

const ts_PieceRecord *DBLSwap::SwapEndian(const ts_PieceRecord *pData)
{
	static ts_PieceRecord BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLSwap::CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("ts_PieceRecord", 16, sizeof(BigEndianData)) == FALSE)
		return(NULL);

	/* swap endianness */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace(&BigEndianData.s16OffX);
		DBLSwap::SwapEndianInPlace(&BigEndianData.s16OffY);
		DBLSwap::SwapEndianInPlace(&BigEndianData.s16Width);
		DBLSwap::SwapEndianInPlace(&BigEndianData.s16Height);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u16TexIndex);
		DBLSwap::SwapEndianInPlace(&BigEndianData.s16HotSpotX);
		DBLSwap::SwapEndianInPlace(&BigEndianData.s16HotSpotY);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u16Pieces);

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

/******* note-- not suitable for cut-and-paste to create a new record type writer because of truncated swap ******/
const ts_TextureChunkHeader *DBLSwap::SwapEndian(const ts_TextureChunkHeader *pData)
{
	static ts_TextureChunkHeader BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLSwap::CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("ts_TextureChunkHdr", 56, sizeof(BigEndianData) - sizeof(ts_TextureChunkRecord)) == FALSE)
		return(NULL);

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace(&BigEndianData.u32TextureCount);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32Flags);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32VRAMInfoTableOffset);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32NumTexSubSets);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32PieceCount);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32PieceOffset);

		// skip Name string
		// skip TextureChunkRecord array[1]-- this routine only swaps the header portion of ts_TextureChunkHdr

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

#if 0
/* do a ps2 vram info chunk */
const ts_TextureChunkVRAMInfoRecord *DBLSwap::SwapEndian(const ts_TextureChunkVRAMInfoRecord *pData)
{
	static ts_TextureChunkVRAMInfoRecord BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLSwap::CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("ts_TextureChunkVRAMInfoRecord", 4, sizeof(BigEndianData)) == FALSE)
		return(NULL);

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace(&BigEndianData.u32VRAMBlockSize);

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}
#endif

/******* note-- not suitable for cut-and-paste to create a new record type writer because of truncated swap ******/
const ts_AnimatedTextureChunkHeader *DBLSwap::SwapEndian(const ts_AnimatedTextureChunkHeader *pData)
{
	static ts_AnimatedTextureChunkHeader BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLSwap::CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("ts_AnimatedTextureChunkHeader", 24, sizeof(BigEndianData) - sizeof(ts_TextureChunkRecord)) == FALSE)
		return(NULL);

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace((u32 *) &BigEndianData.pNext);	// convert pointer as u32 in case it contains offset to be fixed up at runtime
		DBLSwap::SwapEndianInPlace(&BigEndianData.s16Timer);
		DBLSwap::SwapEndianInPlace(&BigEndianData.s16TimerInit);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u16TextureIndex);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u16TextureCount);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32TextureSlot);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u16Format);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u16Type);
		// skip TextureChunkRecord array[1]-- this routine only swaps the header portion of ts_AnimatedTextureChunkHdr

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

const ts_TextureChunkRecord *DBLSwap::SwapEndian(const ts_TextureChunkRecord *pData)
{
	static ts_TextureChunkRecord BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLSwap::CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("ts_TextureChunkRecord", 72, sizeof(BigEndianData)) == FALSE)
		return(NULL);

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace(&BigEndianData.u32Flags);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32NextOffset);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u16MipmapDistance);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32TextureOffset);
		DBLSwap::SwapEndianInPlace(&BigEndianData.s16TextureWidth);
		DBLSwap::SwapEndianInPlace(&BigEndianData.s16TextureHeight);
		DBLSwap::SwapEndianInPlace(&BigEndianData.s16TextureBlock);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32PaletteOffset);
		DBLSwap::SwapEndianInPlace(&BigEndianData.s16PaletteWidth);
		DBLSwap::SwapEndianInPlace(&BigEndianData.s16PaletteHeight);
		DBLSwap::SwapEndianInPlace(&BigEndianData.s16PaletteBlock);
		DBLSwap::SwapEndianInPlace(&BigEndianData.s16ImageBlocks);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32SubSetNumber);

		// skip acTextureName string

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

/******* note-- not suitable for cut-and-paste to create a new record type writer because of truncated swap ******/
const ts_TextureContext *DBLSwap::SwapEndian(const ts_TextureContext *pData)
{
	static ts_TextureContext BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLSwap::CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("ts_TextureContext", 8, sizeof(BigEndianData) - sizeof(ts_TextureLayer)) == FALSE)
		return(NULL);

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndian(&BigEndianData.BaseColor);
		DBLSwap::SwapEndianInPlace(&BigEndianData.TextureCount);
		// skip TextureLayer array[1]-- this routine only swaps the header portion of ts_TextureContext

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

const ts_TextureLayer *DBLSwap::SwapEndian(const ts_TextureLayer *pData)
{
	static ts_TextureLayer BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLSwap::CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("ts_TextureLayer", 44, sizeof(BigEndianData)) == FALSE)
		return(NULL);

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace(&BigEndianData.s16TextureIndex);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u8Method);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u8Blend);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u8WrapU);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u8WrapV);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u8Filter);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u8Flags);
		// skip TextureName string

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

const ts_TriStripHeader *DBLSwap::SwapEndian(const ts_TriStripHeader *pData)
{
	static ts_TriStripHeader BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLSwap::CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("ts_TriStripHeader", 32, sizeof(BigEndianData)) == FALSE)
		return(NULL);

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace(&BigEndianData.u32NextOffset);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32TextureContextIndex);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32BoneIndex);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32ID);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u16PointCount);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u16SkipPointCount);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u8Opacity);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u8Flags);
		// skip Pad

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

const ts_GCNTriStripHeader *DBLSwap::SwapEndian(const ts_GCNTriStripHeader *pData)
{
	static ts_GCNTriStripHeader BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLSwap::CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("ts_GCNTriStripHeader", 32, sizeof(BigEndianData)) == FALSE)
		return(NULL);

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace(&BigEndianData.u32NextOffset);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32TextureContextIndex);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32BoneIndex);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32ID);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u32DisplayListSize);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u16VertexCount);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u16TriangleCount);
		// skip Pad

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

#if 0
const ts_LightInfo *DBLSwap::SwapEndian(const ts_LightInfo *pData)
{
	static ts_LightInfo BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLSwap::CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("ts_LightInfo", 144, sizeof(BigEndianData)) == FALSE)
		return(NULL);

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace(&BigEndianData.mLocalToWorld);
		DBLSwap::SwapEndianInPlace(&BigEndianData.ulColor);
		DBLSwap::SwapEndianInPlace(&BigEndianData.fIntensity);

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

const ts_NamedPointInfo *DBLSwap::SwapEndian(const ts_NamedPointInfo *pData)
{
	static ts_NamedPointInfo BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLSwap::CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("ts_NamedPointInfo", 128, sizeof(BigEndianData)) == FALSE)
		return(NULL);

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		// skip Name string
		DBLSwap::SwapEndianInPlace(&BigEndianData.mLocalToWorld);

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

const ts_SectorInfo *DBLSwap::SwapEndian(const ts_SectorInfo *pData)
{
	static ts_SectorInfo BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLSwap::CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("ts_SectorInfo", 120, sizeof(BigEndianData)) == FALSE)
		return(NULL);

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace(&BigEndianData.sectorIdent);
		DBLSwap::SwapEndianInPlace(&BigEndianData.overrideWorldLighting);
		DBLSwap::SwapEndianInPlace(&BigEndianData.sunDirection);
		DBLSwap::SwapEndianInPlace(&BigEndianData.sunColor);
		DBLSwap::SwapEndianInPlace(&BigEndianData.ambientColor);
		DBLSwap::SwapEndianInPlace(&BigEndianData.overrideSkyColor);
		DBLSwap::SwapEndianInPlace(&BigEndianData.skyColor);
		DBLSwap::SwapEndianInPlace(&BigEndianData.reflectionMap);
		DBLSwap::SwapEndianInPlace(&BigEndianData.clipFar);
		DBLSwap::SwapEndianInPlace(&BigEndianData.clipNear);
		assert(sizeof(INTERPOLATE) == 4);	//we assume that enum types are treated as ints-- all game compilers must have same setting
		DBLSwap::SwapEndianInPlace((int *) &BigEndianData.clipInterpolate);
		DBLSwap::SwapEndianInPlace(&BigEndianData.fogNear);
		DBLSwap::SwapEndianInPlace(&BigEndianData.fogFar);
		DBLSwap::SwapEndianInPlace(&BigEndianData.fogDensity);
		DBLSwap::SwapEndianInPlace(&BigEndianData.fogColor);
		assert(sizeof(FOGTYPE) == 4);	//we assume that enum types are treated as ints-- all game compilers must have same setting
		DBLSwap::SwapEndianInPlace((int *) &BigEndianData.fogType);
		assert(sizeof(INTERPOLATE) == 4);	//we assume that enum types are treated as ints-- all game compilers must have same setting
		DBLSwap::SwapEndianInPlace((int *) &BigEndianData.fogInterpolate);
		DBLSwap::SwapEndianInPlace(&BigEndianData.reverbLevel);
		DBLSwap::SwapEndianInPlace(&BigEndianData.reverbDepth);
		DBLSwap::SwapEndianInPlace(&BigEndianData.reverbDampening);

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

#endif

const ts_EmitterSource *DBLSwap::SwapEndian(const ts_EmitterSource *pData)
{
	static ts_EmitterSource BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLSwap::CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("ts_EmitterSource", 32, sizeof(BigEndianData)) == FALSE)
		return(NULL);

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace(&BigEndianData.u16EffectID);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u8Type);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u8Scale);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u8ScaleRandomPct);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u8MinDistance);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u8Radius);
		DBLSwap::SwapEndianInPlace(&BigEndianData.s16AngularVelRadians);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u16GoRadius);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u16LOD1Radius);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u16LOD1Radius);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u8VelPct);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u8AdditiveVelPct);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u8Lifetime);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u8LifetimeRandomPct);

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

/******* note-- not suitable for cut-and-paste to create a new record type writer because of truncated swap ******/
const ts_Effect *DBLSwap::SwapEndian(const ts_Effect *pData)
{
	static ts_Effect BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLSwap::CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("ts_Effect", 4, sizeof(BigEndianData) - sizeof(ts_Channel)) == FALSE)
		return(NULL);

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace(&BigEndianData.u32ChannelCount);
		// skip ts_Channel array[1]-- this routine only swaps the header portion of ts_Effect

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

const ts_Channel *DBLSwap::SwapEndian(const ts_Channel *pData)
{
	static ts_Channel BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLSwap::CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("ts_Channel", 16, sizeof(BigEndianData)) == FALSE)
		return(NULL);

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace(&BigEndianData.u32ParticleIndex);	// one member of union
		DBLSwap::SwapEndianInPlace(&BigEndianData.fEmitDelay);
		DBLSwap::SwapEndianInPlace(&BigEndianData.fEmitTime);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u8EmitCount);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u8Flags);

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

const ts_ParticleSource *DBLSwap::SwapEndian(const ts_ParticleSource *pData)
{
	static ts_ParticleSource BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLSwap::CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("ts_ParticleSource", 328, sizeof(BigEndianData)) == FALSE)
		return(NULL);

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		int i;
//		DBLSwap::SwapEndianInPlace((u32 *) &BigEndianData.pParticleData);	// convert pointer as u32 in case it contains offset to be fixed up at runtime

		DBLSwap::SwapEndianInPlace(&BigEndianData.u8FPS);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u8FPSRandomPct);

		DBLSwap::SwapEndianInPlace(&BigEndianData.u8Lifetime);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u8LifetimeRandomPct);

		DBLSwap::SwapEndianInPlace(&BigEndianData.s8InitialVelX);
		DBLSwap::SwapEndianInPlace(&BigEndianData.s8InitialVelY);
		DBLSwap::SwapEndianInPlace(&BigEndianData.s8InitialVelZ);

		DBLSwap::SwapEndianInPlace(&BigEndianData.u8CollideBounceEnergy);

		DBLSwap::SwapEndianInPlace(&BigEndianData.u16FirstIndex);	// one member of union
		DBLSwap::SwapEndianInPlace(&BigEndianData.u16LastIndex);		// one member of union

		DBLSwap::SwapEndianInPlace(&BigEndianData.u16XSize);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u16YSize);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u8XSizeRandomPct);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u8YSizeRandomPct);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u8YSizeEnvelope);

		DBLSwap::SwapEndianInPlace(&BigEndianData.u16Leash);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u8LeashRandomPct);

		DBLSwap::SwapEndianInPlace(&BigEndianData.vInitialPos);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u16PosRandom);

		DBLSwap::SwapEndianInPlace(&BigEndianData.fInitialVel);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u8VelRandomPct);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u8VelConeStart);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u8VelConeDelta);

		DBLSwap::SwapEndianInPlace(&BigEndianData.vInitialRotVelRadians);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u8RotVelRandomPct);

		DBLSwap::SwapEndianInPlace(&BigEndianData.fGravity);
		DBLSwap::SwapEndianInPlace(&BigEndianData.fAccel);
		DBLSwap::SwapEndianInPlace(&BigEndianData.fRotAccelRadians);

		DBLSwap::SwapEndianInPlace(&BigEndianData.u32Flags);

		for (i = 0; i < MAX_ENVELOPES; i++)
		{
			DBLSwap::SwapEndianInPlace(&BigEndianData.Envelope[i].fSplinePoint0Y);
			DBLSwap::SwapEndianInPlace(&BigEndianData.Envelope[i].SplinePoint1);
			DBLSwap::SwapEndianInPlace(&BigEndianData.Envelope[i].SplinePoint2);
			DBLSwap::SwapEndianInPlace(&BigEndianData.Envelope[i].fSplinePoint3Y);
			DBLSwap::SwapEndianInPlace(&BigEndianData.Envelope[i].fCeilingY);
			DBLSwap::SwapEndianInPlace(&BigEndianData.Envelope[i].fFloorY);
			DBLSwap::SwapEndianInPlace(&BigEndianData.Envelope[i].u8Type);
		}

		for (i = 0; i < COLOR_PAIR_COUNT; i++)
			DBLSwap::SwapEndianInPlace(&BigEndianData.Color[i]);

		/* orbit */

		DBLSwap::SwapEndianInPlace(&BigEndianData.u8OrbitRadius);
		DBLSwap::SwapEndianInPlace(&BigEndianData.s8OrbitRadiusVel);
		DBLSwap::SwapEndianInPlace(&BigEndianData.s16OrbitRotVel);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u8OrbitRadiusRandomPct);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u8OrbitRadiusVelRandomPct);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u8OrbitRotVelRandomPct);

		/* swap spawn effect count, but not SpawnEffect[1] array-- taht is done elsewhere wince it is a variable-length */
		/* array */

		DBLSwap::SwapEndianInPlace(&BigEndianData.u16SpawnEffectCount);
		DBLSwap::SwapEndianInPlace(&BigEndianData.u16EndOfLifeSpawnIndex);

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}


const CSceneNode *DBLSwap::SwapEndian(const CSceneNode *pData)
{
	static CSceneNode BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLSwap::CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("CSceneNode", 24, sizeof(BigEndianData)) == FALSE)
		return(NULL);

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		u32 tempData;
		tempData = (u32)BigEndianData.GetData();	SwapEndianInPlace(&tempData); BigEndianData.SetData((void *)tempData);
		tempData = (u32)BigEndianData.GetFirstChild(); SwapEndianInPlace(&tempData); BigEndianData.SetFirstChild((CSceneNode *)tempData);
		tempData = (u32)BigEndianData.GetNextSibling(); SwapEndianInPlace(&tempData); BigEndianData.SetNextSibling((CSceneNode *)tempData);
		tempData = (u32)BigEndianData.GetParent(); SwapEndianInPlace(&tempData); BigEndianData.SetParent((CSceneNode *)tempData);
		tempData = (u32)BigEndianData.GetNextRender(); SwapEndianInPlace(&tempData); BigEndianData.SetNextRender((CSceneNode *)tempData);
		tempData = (u32)BigEndianData.GetFlags(); SwapEndianInPlace(&tempData); BigEndianData.SetFlags(tempData);

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

const ts_BoundingBox *DBLSwap::SwapEndian(const ts_BoundingBox *pData)
{
	static ts_BoundingBox BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLSwap::CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("ts_BoundingBox", 24, sizeof(BigEndianData)) == FALSE)
		return(NULL);

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace(&BigEndianData.Max);
		DBLSwap::SwapEndianInPlace(&BigEndianData.Min);

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

const ts_BoundingSphere *DBLSwap::SwapEndian(const ts_BoundingSphere *pData)
{
	static ts_BoundingSphere BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLSwap::CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("ts_BoundingSphere", 16, sizeof(BigEndianData)) == FALSE)
		return(NULL);

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace(&BigEndianData.Center);
		DBLSwap::SwapEndianInPlace(&BigEndianData.Radius);

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}


const SMeshSOM *DBLSwap::SwapEndian(const SMeshSOM *pData)
{
	static SMeshSOM BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLUtil__CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("SMeshSOM", 88, sizeof(BigEndianData)) == FALSE)
		return(NULL);

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace(&BigEndianData.dh.u8DataType);
		DBLSwap::SwapEndianInPlace(&BigEndianData.dh.u8Flags2);
		DBLSwap::SwapEndianInPlace(&BigEndianData.dh.u16Flags);
		DBLSwap::SwapEndianInPlace(&BigEndianData.d_nodeOffset);
		DBLSwap::SwapEndianInPlace(&BigEndianData.d_constraintOffset);
		DBLSwap::SwapEndianInPlace(&BigEndianData.d_collisionFaceOffset);
		DBLSwap::SwapEndianInPlace(&BigEndianData.d_collisionEdgeOffset);
		DBLSwap::SwapEndianInPlace(&BigEndianData.d_collisionPointOffset);
		DBLSwap::SwapEndianInPlace(&BigEndianData.d_subMeshOffset);
		DBLSwap::SwapEndianInPlace(&BigEndianData.d_neighborOffset);
		DBLSwap::SwapEndianInPlace((u32 *) &BigEndianData.d_textureArray);
		DBLSwap::SwapEndianInPlace(&BigEndianData.d_textureSubSetIndex);
		DBLSwap::SwapEndianInPlace(&BigEndianData.d_numNode);
		DBLSwap::SwapEndianInPlace(&BigEndianData.d_numConstraint);
		DBLSwap::SwapEndianInPlace(&BigEndianData.d_numCollisionFace);
		DBLSwap::SwapEndianInPlace(&BigEndianData.d_numCollisionEdge);
		DBLSwap::SwapEndianInPlace(&BigEndianData.d_numCollisionPoint);
		DBLSwap::SwapEndianInPlace(&BigEndianData.d_numSubMesh);
		DBLSwap::SwapEndianInPlace(&BigEndianData.d_positionNode);
		DBLSwap::SwapEndianInPlace(&BigEndianData.d_dragNode);
		// skip d_name

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

const SMeshSOM::SNodeSOM *DBLSwap::SwapEndian(const SMeshSOM::SNodeSOM *pData)
{
	static SMeshSOM::SNodeSOM BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLUtil__CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("SNodeSOM", 28, sizeof(BigEndianData)) == FALSE)
		return(NULL);

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace(&BigEndianData.d_baseVertexInModel);
		DBLSwap::SwapEndianInPlace(&BigEndianData.d_numFrontSide);
		DBLSwap::SwapEndianInPlace(&BigEndianData.d_numBackSide);
		DBLSwap::SwapEndianInPlace(&BigEndianData.d_endNeighbor);
		DBLSwap::SwapEndianInPlace(&BigEndianData.d_thickness);
		DBLSwap::SwapEndianInPlace(&BigEndianData.d_masterBoneID);
		DBLSwap::SwapEndianInPlace(&BigEndianData.d_flag);

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

const SMeshSOM::SIndexPair *DBLSwap::SwapEndian(const SMeshSOM::SIndexPair *pData)
{
	static SMeshSOM::SIndexPair BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLUtil__CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("SIndexPair", 4, sizeof(BigEndianData)) == FALSE)
		return(NULL);

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace(&BigEndianData.d_index[0]);
		DBLSwap::SwapEndianInPlace(&BigEndianData.d_index[1]);

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

const SMeshSOM::SIndexTrio *DBLSwap::SwapEndian(const SMeshSOM::SIndexTrio *pData)
{
	static SMeshSOM::SIndexTrio BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLUtil__CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("SIndexTrio", 6, sizeof(BigEndianData)) == FALSE)
		return(NULL);

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace(&BigEndianData.d_index[0]);
		DBLSwap::SwapEndianInPlace(&BigEndianData.d_index[1]);
		DBLSwap::SwapEndianInPlace(&BigEndianData.d_index[2]);

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

const SMeshSOM::SSubMesh *DBLSwap::SwapEndian(const SMeshSOM::SSubMesh *pData)
{
	static SMeshSOM::SSubMesh BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLUtil__CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("SSubMesh", 24, sizeof(BigEndianData)) == FALSE)
		return(NULL);

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace(&BigEndianData.d_textureContextIndex);
		DBLSwap::SwapEndianInPlace(&BigEndianData.d_nodeIndexOffset);
		DBLSwap::SwapEndianInPlace(&BigEndianData.d_textureCoordOffset);
		DBLSwap::SwapEndianInPlace(&BigEndianData.d_triStripHeadOffset);
		DBLSwap::SwapEndianInPlace(&BigEndianData.d_triStripOffset);
		DBLSwap::SwapEndianInPlace(&BigEndianData.d_numVertex);
		DBLSwap::SwapEndianInPlace(&BigEndianData.d_numTriStrip);

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

/******* note-- not suitable for cut-and-paste to create a new record type writer because of truncated swap ******/
const CVisRegion* DBLSwap::SwapEndian( const CVisRegion* pData )
{
	static CVisRegion BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLUtil__CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("CVisRegion", 152, sizeof(CVisRegion) - sizeof(CVisRegion *)) == FALSE)
		return(NULL);

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace(&BigEndianData.GetBoundingBox()->Min);
		DBLSwap::SwapEndianInPlace(&BigEndianData.GetBoundingBox()->Max);

		u32 u32Value;
		u32Value = BigEndianData.GetSiblingIndex();
		DBLSwap::SwapEndianInPlace(&u32Value);
		BigEndianData.SetSiblingIndex(u32Value);

		u32Value = BigEndianData.GetPartitionID();
		DBLSwap::SwapEndianInPlace(&u32Value);
		BigEndianData.SetPartitionID(u32Value);

		DBLSwap::SwapEndianInPlace(&BigEndianData.GetOBBBoundingBox()->Min);
		DBLSwap::SwapEndianInPlace(&BigEndianData.GetOBBBoundingBox()->Max);
		DBLSwap::SwapEndianInPlace(BigEndianData.GetOBBCenter());
		DBLSwap::SwapEndianInPlace(BigEndianData.GetOBBOrientation());

		u32Value = BigEndianData.GetFlags();
		DBLSwap::SwapEndianInPlace(&u32Value);
		BigEndianData.SetFlags(u32Value);

		/* swap visible region count, but not array-- that is done elsewhere since it is variable-length */

		u32Value = BigEndianData.GetVisibleRegionCount();
		DBLSwap::SwapEndianInPlace(&u32Value);
		BigEndianData.SetVisibleRegionCount(u32Value);
		
		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}

const CLightRecord* DBLSwap::SwapEndian( const CLightRecord* pData )
{
	static CLightRecord BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLUtil__CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	// 144+64, 64 is a char array for the name of the light
	if (DBLSwap::CheckRecordSize("CLightRecord", 144+64, sizeof( CLightRecord ) ) == FALSE)
		return(NULL);

	/* swap endianness if necessary */
	if ( GetSwapEndianActive( ) )
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy( &BigEndianData, pData, sizeof( BigEndianData ) );

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		DBLSwap::SwapEndianInPlace(&BigEndianData.getColor( ) );
		DBLSwap::SwapEndianInPlace(&BigEndianData.getOrigColor( ) );
		DBLSwap::SwapEndianInPlace(&BigEndianData.getPosition( ) );
		DBLSwap::SwapEndianInPlace(&BigEndianData.getOrigPosition( ) );
		DBLSwap::SwapEndianInPlace(&BigEndianData.getDirection( ) );
		DBLSwap::SwapEndianInPlace(&BigEndianData.getType( ) );
		DBLSwap::SwapEndianInPlace(&BigEndianData.getConstantAttenuation( ) );
		DBLSwap::SwapEndianInPlace(&BigEndianData.getLinearAttenuation( ) );
		DBLSwap::SwapEndianInPlace(&BigEndianData.getQuadraticAttenuation( ) );
		DBLSwap::SwapEndianInPlace(&BigEndianData.getCubicAttenuation( ) );
		DBLSwap::SwapEndianInPlace(&BigEndianData.getIntensity( ) );
		DBLSwap::SwapEndianInPlace(&BigEndianData.getOrigIntensity( ) );
		DBLSwap::SwapEndianInPlace(&BigEndianData.getIntensityNoise( ) );
		DBLSwap::SwapEndianInPlace(&BigEndianData.getVolumeSphereRadius( ) );
		DBLSwap::SwapEndianInPlace(&BigEndianData.getVolumeCubeWidthHeight( ) );
		DBLSwap::SwapEndianInPlace(&BigEndianData.getLightFlags( ) );
		pData = &BigEndianData;
	}

	/* return pointer to data */

	return( pData );
}

const ts_EnvSoundSource *DBLSwap::SwapEndian(const ts_EnvSoundSource *pData)
{
	// Don't do a damned thing if we don't have to
	if (!GetSwapEndianActive())
		return pData;

	// God, this is retarded
	static ts_EnvSoundSource source;
	memcpy(&source, pData, sizeof(source));

	DBLSwap::SwapEndianInPlace(&source.flags);
	DBLSwap::SwapEndianInPlace(&source.position);
	DBLSwap::SwapEndianInPlace(&source.audibleDistance);
	DBLSwap::SwapEndianInPlace(&source.minTriggerTime);
	DBLSwap::SwapEndianInPlace(&source.maxTriggerTime);
	DBLSwap::SwapEndianInPlace(&source.volume);
	DBLSwap::SwapEndianInPlace(&source.pitch);
	DBLSwap::SwapEndianInPlace(&source.ambience);
	DBLSwap::SwapEndianInPlace(&source.presence);
	DBLSwap::SwapEndianInPlace(&source.dir);
	DBLSwap::SwapEndianInPlace(&source.minConeAngle);
	DBLSwap::SwapEndianInPlace(&source.maxConeAngle);

	return &source;
}

const ts_EnvHalo *DBLSwap::SwapEndian(const ts_EnvHalo *pData)
{
	// Don't do a damned thing if we don't have to
	if (!GetSwapEndianActive())
		return pData;

	// God, this is retarded
	static ts_EnvHalo source;
	memcpy(&source, pData, sizeof(source));

	DBLSwap::SwapEndianInPlace(&source.pos);
	DBLSwap::SwapEndianInPlace(&source.dir);
	DBLSwap::SwapEndianInPlace(&source.color);
	DBLSwap::SwapEndianInPlace(&source.size);
	DBLSwap::SwapEndianInPlace(&source.flags);
	DBLSwap::SwapEndianInPlace(&source.textureIndex);

	return &source;
}

const ts_EnvStationaryCam *DBLSwap::SwapEndian(const ts_EnvStationaryCam *pData)
{
	// Don't do a damned thing if we don't have to
	if (!GetSwapEndianActive())
		return pData;

	// God, this is retarded
	static ts_EnvStationaryCam source;
	memcpy(&source, pData, sizeof(source));

	DBLSwap::SwapEndianInPlace(&source.pos);
	DBLSwap::SwapEndianInPlace(&source.dir);
	DBLSwap::SwapEndianInPlace(&source.fov);
	DBLSwap::SwapEndianInPlace(&source.x);
	DBLSwap::SwapEndianInPlace(&source.y);
	DBLSwap::SwapEndianInPlace(&source.speed);

	return &source;
}

const ts_EnvSoundField *DBLSwap::SwapEndian(const ts_EnvSoundField *pData)
{
	// Don't do a damned thing if we don't have to
	if (!GetSwapEndianActive())
		return pData;

	// God, this is retarded
	static ts_EnvSoundField field;
	memcpy(&field, pData, sizeof(field));

	DBLSwap::SwapEndianInPlace(&field.flags);
	DBLSwap::SwapEndianInPlace(&field.extents);
	DBLSwap::SwapEndianInPlace(&field.position);
	DBLSwap::SwapEndianInPlace(&field.orientation);
	DBLSwap::SwapEndianInPlace(&field.effectVolume);
	DBLSwap::SwapEndianInPlace(&field.volume);
	DBLSwap::SwapEndianInPlace(&field.minRetriggerTime);
	DBLSwap::SwapEndianInPlace(&field.maxRetriggerTime);
	DBLSwap::SwapEndianInPlace(&field.fadeInTime);
	DBLSwap::SwapEndianInPlace(&field.fadeOutTime);
	DBLSwap::SwapEndianInPlace(&field.effectTaper);

	return &field;
}

const ts_Water *DBLSwap::SwapEndian(const ts_Water *pData)
{
	if (!GetSwapEndianActive())
		return pData;

	// wow.  returning a pointer to a static...not exactly the cleanest thing
	// in the world, now is it?
	static ts_Water water;
	memcpy(&water, pData, sizeof(water));

	DBLSwap::SwapEndianInPlace(&water.wrap);
	DBLSwap::SwapEndianInPlace(&water.pos);
	DBLSwap::SwapEndianInPlace(&water.size);
	DBLSwap::SwapEndianInPlace(&water.angle);

	return &water;
}

const ts_AnimCurve *DBLSwap::SwapEndian(const ts_AnimCurve *pData)
{
	static ts_AnimCurve BigEndianData;	// pointer to this may be returned, must be static

	/* we insist that the record is of the expected size. this is to prevent errors in the endian swapping processing. */
	/* if you get an error here, adjust the hard-wired size (the second parameter to DBLSwap::CheckRecordSize), and make */
	/* sure all fields are in the endian swapping list */
	/* NOTE: it is possible that this error occurs due to the status of MMX_OK. if so, the	data structure needs to be fixed */
	/* so that it behaves the same regardless of MMX_OK's state. this may require padding, and hand-aligning. the main issue */
	/* is that the tools need to behave the same regardless of whose machine they are compiled on */

	if (DBLSwap::CheckRecordSize("ts_AnimCurve", 60, sizeof(BigEndianData)) == FALSE)
		return(NULL);

	/* swap endianness if necessary */

	if (GetSwapEndianActive())
	{
		/* leave original data intact, since it may get referred to again later (for example, */
		/* headers are often written out twice, once as a placeholder, and again when complete) */

		memcpy(&BigEndianData, pData, sizeof(BigEndianData));

		/* convert each field, except for strings. u8 fields won't change, of course, but if */
		/* they are expanded to u16 at some future time, conversion will still be correct */

		// skip Name string
		DBLSwap::SwapEndianInPlace(&BigEndianData.nSplineCount);
		DBLSwap::SwapEndianInPlace(&BigEndianData.fStartTime);
		DBLSwap::SwapEndianInPlace(&BigEndianData.fEndTime);

		/* swap first 4 entries in SplineData, but rest are handled elsewhere, since it's a variably-sized array */

		for (int i = 0; i < 4; i++)
			DBLSwap::SwapEndianInPlace(&BigEndianData.SplineData[i]);

		/* point to converted data */

		pData = &BigEndianData;
	}

	/* return pointer to data */

	return(pData);
}


