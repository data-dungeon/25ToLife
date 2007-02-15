#include <libvu0.h>
#include <math.h>
#include "VUDataClass.h"
#include "VUCodeDefs.h"
#include "DMAObject.h"
#include "DDMAObject.h"
#include "VUCode.h"
#include "VUCodeManager.h"
#include "VUCodeSet.h"

#include "MasterList.h"
#include "PS2Renderer.h"
#include "DDMAObject.h"

#include <eestruct.h>

#include "GIFTag.h"
#include "GIFInline.h"
using namespace GIFInline;

#define VU_SPACE_RESERVED_FOR_TEXTURE_PACKET 7
unsigned int			VUDataClass::d_VUMemBase = 32;

VUCodeManager *		VUDataClass::p_CodeManager = 0;
TextureDataCallback* VUDataClass::p_TextureDataCallback = 0;

PS2MaterialInfo		VUDataClass::d_DefaultMaterial = { 0.8f };

VUCodeSetup				VUDataClass::CachedVUSetup = { 0 };

VUDataClass::LightingInfo VUDataClass::d_LightingInfo;

static inline void my_sceVu0ScaleVector(PS2Vector4 v0, const PS2Vector4 v1, float t)
{
	asm __volatile__("
		lqc2    vf4,0x0(%1)
		mfc1    $8,%2
		qmtc2    $8,vf5
		vmulx.xyzw	vf6,vf4,vf5
		sqc2    vf6,0x0(%0)
		": : "r" (v0) , "r" (v1), "f" (t):"$8");
}
static inline void my_sceVu0Normalize(PS2Vector4 v0, const PS2Vector4 v1)
{
	asm __volatile__("
		lqc2    vf4,0x0(%1)
		vmul.xyz vf5,vf4,vf4
		vaddy.x vf5,vf5,vf5
		vaddz.x vf5,vf5,vf5
		
		vsqrt Q,vf5x
		vwaitq
		vaddq.x vf5x,vf0x,Q
		vdiv    Q,vf0w,vf5x
		vsub.xyzw vf6,vf0,vf0           #vf6.xyzw=0;
	vwaitq
		
		vmulq.xyz  vf6,vf4,Q
		sqc2    vf6,0x0(%0)
		": : "r" (v0) , "r" (v1));
}
static inline void my_sceVu0NormalLightVector(PS2Matrix4x4 m, const PS2Vector4 l0, u_int row )
{
	PS2Vector4  t;
	my_sceVu0ScaleVector(t, l0, -1); my_sceVu0Normalize(m[row], t);
}
static inline void my_sceVu0CopyVector(sceVu0FVECTOR v0, const sceVu0FVECTOR v1)
{
	asm __volatile__("
		lq    $6,0x0(%1)
		sq    $6,0x0(%0)
		": : "r" (v0) , "r" (v1):"$6");
}
static inline void my_sceVu0TransposeMatrix(PS2Matrix4x4 m0, const PS2Matrix4x4 m1)
{
	asm __volatile__("
		lq $8,0x0000(%1)
		lq $9,0x0010(%1)
		lq $10,0x0020(%1)
		lq $11,0x0030(%1)
		
		pextlw     $12,$9,$8
		pextuw     $13,$9,$8
		pextlw     $14,$11,$10
		pextuw     $15,$11,$10
		
		pcpyld     $8,$14,$12
		pcpyud     $9,$12,$14
		pcpyld     $10,$15,$13
		pcpyud     $11,$13,$15
		
		sq $8,0x0000(%0)
		sq $9,0x0010(%0)
		sq $10,0x0020(%0)
		sq $11,0x0030(%0)
		": : "r" (m0) , "r" (m1));
}
static inline void my_sceVu0NormalLightMatrix(PS2Matrix4x4 m, const PS2Vector4 l0, const PS2Vector4 l1, const PS2Vector4 l2)
{
	PS2Vector4  t;
	my_sceVu0ScaleVector(t, l0, -1); my_sceVu0Normalize(m[0], t);
	my_sceVu0ScaleVector(t, l1, -1); my_sceVu0Normalize(m[1], t);
	my_sceVu0ScaleVector(t, l2, -1); my_sceVu0Normalize(m[2], t);
	
	m[3][0] = m[3][1] = m[3][2] = 0.0f;
	m[3][3] = 1.0f;
	
	my_sceVu0TransposeMatrix(m, m);
}


static inline void my_sceVu0LightColorMatrix(PS2Matrix4x4 m, const PS2Vector4 c0, const PS2Vector4 c1, const PS2Vector4 c2, const PS2Vector4 a)
{
	my_sceVu0CopyVector(m[0], c0);
	my_sceVu0CopyVector(m[1], c1);
	my_sceVu0CopyVector(m[2], c2);
	my_sceVu0CopyVector(m[3],  a);
}

void VUDataClass::prepEnvMappingMatrix( const VURenderInfo & rndInfo, const PS2Matrix4x4 & Mtx1 )
{
	PS2Matrix4x4					EnvTemp;
	const PS2Matrix4x4 *			pEnvMtx = 0;

	switch( rndInfo.mEnvData.mSpace )
	{
	case ENVMAP_SPACE_CAMERA:
		switch( rndInfo.drawFlags & LOCAL_MATRIX_MASK )
		{
		case LOCAL_TO_WORLD:
			sceVu0MulMatrix( EnvTemp, *(PS2Matrix4x4 *) &rndInfo.mWorldToCamera, *(PS2Matrix4x4 *) & Mtx1 );
			pEnvMtx = & EnvTemp;
			break;
		case LOCAL_TO_CAMERA:
			pEnvMtx = & Mtx1;
			break;
		default:
			// unknown input matrix type specified.
			RENDER_ASSERT( FALSE );
			break;
		}
		break;

	case ENVMAP_SPACE_VIEW:
		switch( rndInfo.drawFlags & LOCAL_MATRIX_MASK)
		{
		case LOCAL_TO_WORLD:
			sceVu0MulMatrix( EnvTemp, *(PS2Matrix4x4 *) &rndInfo.mWorldToView, *(PS2Matrix4x4 *) & Mtx1 );
			pEnvMtx = & EnvTemp;
			break;
		case LOCAL_TO_CAMERA:
			sceVu0MulMatrix( EnvTemp, *(PS2Matrix4x4 *) &rndInfo.mCameraToView, *(PS2Matrix4x4 *) & Mtx1 );
			pEnvMtx = & EnvTemp;
			break;
		default:
			// unknown input matrix type specified.
			RENDER_ASSERT( FALSE );
			break;
		}
		break;
	case ENVMAP_SPACE_WORLD:
		switch( rndInfo.drawFlags & LOCAL_MATRIX_MASK )
		{
		case LOCAL_TO_WORLD:
			pEnvMtx = & Mtx1;
			break;
		case LOCAL_TO_CAMERA:
			sceVu0MulMatrix( EnvTemp, *(PS2Matrix4x4 *) &rndInfo.mCameraToWorld, *(PS2Matrix4x4 *) & Mtx1 );
			pEnvMtx = (PS2Matrix4x4 *) &EnvTemp;
			break;
		default:
			// unknown input matrix type specified.
			RENDER_ASSERT( FALSE );
			break;
		}
	break;
			
	case ENVMAP_SPACE_MAP:
		ASSERT( rndInfo.mEnvData.mWorldToMap );

		switch( rndInfo.drawFlags & LOCAL_MATRIX_MASK )
		{
		case LOCAL_TO_WORLD:
			sceVu0MulMatrix( EnvTemp, *(PS2Matrix4x4 *) &rndInfo.mEnvData.mWorldToMap, *(PS2Matrix4x4 *) & Mtx1 );
			pEnvMtx = & Mtx1;
			break;
		case LOCAL_TO_CAMERA:
			PS2Matrix4x4			EnvWorld;
			sceVu0MulMatrix( EnvWorld, *(PS2Matrix4x4 *) &rndInfo.mCameraToWorld, *(PS2Matrix4x4 *) & Mtx1 );
			sceVu0MulMatrix( EnvTemp, *(PS2Matrix4x4 *) &rndInfo.mEnvData.mWorldToMap, EnvWorld );
			pEnvMtx = & EnvTemp;
			break;
		default:
			// unknown input matrix type specified.
			RENDER_ASSERT( FALSE );
			break;
		}
		break;
	default:
	case ENVMAP_SPACE_NOCALC:
		pEnvMtx = 0;
		break;
	}

	if( pEnvMtx )
		PS2Renderer::Normalize( *(PS2Matrix4x4 *) rndInfo.mEnvData.mFinalTransform, *pEnvMtx );

	if( rndInfo.mEnvData.mPostTransform )
		sceVu0MulMatrix( *(PS2Matrix4x4 *) rndInfo.mEnvData.mFinalTransform, *(PS2Matrix4x4 *) rndInfo.mEnvData.mPostTransform, *(PS2Matrix4x4 *) rndInfo.mEnvData.mFinalTransform );
}

void VUDataClass::prepLightingInfo( const VURenderInfo & rndInfo )
{
	MasterList::ClearMem( &d_LightingInfo, sizeof(d_LightingInfo) >> 4);

	my_sceVu0CopyVector( d_LightingInfo.DirLightColors[ 3 ], rndInfo.mLights.mTotalAmbientLight );
	d_LightingInfo.DirLightColors[ 3 ][ 3 ] = rndInfo.alphaMultiplier * 128.0f;

	for( int i = 0; i < MAX_LIGHTS; i++ )
	{
		DBGASSERT( d_LightingInfo.numDirectionalLights <= 3 );
		DBGASSERT( d_LightingInfo.numPointLights <= 3);
		
		if( rndInfo.mLights.mLight[ i ].mEnabled )
		{
			switch( rndInfo.mLights.mLight[ i ].mType )
			{
			case LIGHT_TYPE_DIRECTIONAL:
				if( d_LightingInfo.numDirectionalLights <= 2 )
				{
					my_sceVu0NormalLightVector( d_LightingInfo.DirLightMtx, rndInfo.mLights.mLight[ i ].mCurVec.mCurrentDirection, d_LightingInfo.numDirectionalLights );
					my_sceVu0CopyVector( d_LightingInfo.DirLightColors[ d_LightingInfo.numDirectionalLights ], rndInfo.mLights.mLight[ i ].mDiffuseColor );
					d_LightingInfo.numDirectionalLights++;
				}
				break;
				
			case LIGHT_TYPE_POINT:
				if( d_LightingInfo.numPointLights <= 2 )
				{
					my_sceVu0CopyVector( d_LightingInfo.PointLightMtx[ d_LightingInfo.numPointLights ], rndInfo.mLights.mLight[ i ].mCurVec.mCurrentPosition );
					my_sceVu0CopyVector( d_LightingInfo.PointLightColors[ d_LightingInfo.numPointLights ], rndInfo.mLights.mLight[ i ].mDiffuseColor );
					d_LightingInfo.numPointLights++;
				}
				break;
			}
		}
	}
	
}

#define NUM_REGS_IN_BASE_GSPACKET 6
void VUDataClass::appendGSSettings(u_int bind, PS2MaterialInfo* matInfo)
{
	u_long128 * pPacket = (u_long128*)(*p_TextureDataCallback)(bind, true, matInfo);

	unsigned int * pEVData = ((unsigned int *) pPacket) + 2;

	if( matInfo && matInfo->fEnvironmentMapOpacity != 0.0f)
	{
		*pEVData++ |= 0x100;			
		*((float *) pEVData) = matInfo->fEnvironmentMapOpacity;
	}

	MasterList::Append( GIFInline::createGIFTAG(NUM_REGS_IN_BASE_GSPACKET, 1, 0xE, false));
	MasterList::CopyToMasterList(pPacket, NUM_REGS_IN_BASE_GSPACKET);
}

void VUDataClass::addToMasterList_DTTable( void *pTableData, void *pDMAData)
{
	unsigned int 	numEntries = *((unsigned int *) pTableData)++;
	DTInfo *			pTable = (DTInfo *) pTableData;
	
	for( unsigned int i = 0; i < numEntries; i++)
	{
		PS2MaterialInfo		matInfo = d_DefaultMaterial;
	
		u_int bind = pTable[i].uiTexID;
		
		bind &= 0x7FFFFFFF;
		
		unsigned int numQWs = NUM_REGS_IN_BASE_GSPACKET + 1;

		MasterList::AppendDMATAG( numQWs, TAGID_CNT);
		MasterList::Append( createNOP() );
		MasterList::Append( createUNPACK( 2, V4_32, numQWs, 1 ));

		appendGSSettings(bind, &matInfo);

		MasterList::AppendDMATAG( 0, TAGID_CALL, (void *) (pTable[i].uiBlockOffset + (unsigned int) pDMAData));
		MasterList::Append( createNOP( ) );
		MasterList::Append( createNOP( ) );
	}
}

void VUDataClass::setStandardReqFlags_Static( VURenderInfo & rndInfo)
{
	if( rndInfo.drawFlags & MICROCODE_CLIP)
	{
		rndInfo.requirementsFlags |= REQ_M1_SPACE_DONT_CARE | REQ_M1M2_SPACE_CLIP;
		rndInfo.validMatricies |= MATRIX_CLIP_TO_SCREEN;
	}
	else if( rndInfo.drawFlags & MICROCODE_CULL)
	{
		rndInfo.requirementsFlags |= REQ_M1_SPACE_DONT_CARE | REQ_M1M2_SPACE_CULL;
		rndInfo.validMatricies |= MATRIX_CULL_TO_SCREEN;
	}
	else
	{
		rndInfo.requirementsFlags |= REQ_M1_SPACE_DONT_CARE | REQ_M1M2_SPACE_SCREEN;
	}

}

	VUCodeEntry * VUDataClass::selectBaseCode( VUCodeSet &BaseCodeSet, unsigned int &renderFlags)
	{
	VUCodeEntry * 		pBaseCode = 0;
	
		if( renderFlags & MICROCODE_CLIP)
		{
			if( BaseCodeSet.pClip)
			{
				pBaseCode = BaseCodeSet.pClip;
			}
			else if( BaseCodeSet.pCull)
			{
				renderFlags &= (~MICROCODE_CLIP);
				renderFlags |= MICROCODE_CULL;
				
				pBaseCode = BaseCodeSet.pCull;
			}
			
		}
		else if( renderFlags & MICROCODE_CULL)
		{ 
			if( BaseCodeSet.pCull)
			{
				pBaseCode = BaseCodeSet.pCull;
			}
			else if( BaseCodeSet.pClip)
			{
				renderFlags &= (~MICROCODE_CULL);
				renderFlags |= MICROCODE_CLIP;
				
				pBaseCode = BaseCodeSet.pClip;
			}
		}
		else
		{
			if( BaseCodeSet.pXfrm)
			{
				pBaseCode = BaseCodeSet.pXfrm;
			}
			else if( BaseCodeSet.pCull)
			{
				pBaseCode = BaseCodeSet.pCull;
				renderFlags |= MICROCODE_CULL;
			}
			else
			{
				pBaseCode = BaseCodeSet.pClip;
				renderFlags |= MICROCODE_CLIP;
			}
		}
		
		return pBaseCode;
	}
volatile unsigned int temp = 1;

void VUDataClass::selectVUCode_Static( VUCodeSet ** pCodeSetArray, VUCodeEntry ** pMiscArray, VURenderInfo & rndInfo, VUCodeSetup & ActiveCode)
{
	bool						bMultiPass;
	
	unsigned int 			renderFlags = rndInfo.drawFlags;
	unsigned int 			idxBaseCodeSet = 0;

	renderFlags = rndInfo.drawFlags;
	
	ActiveCode.numExtraPasses = 0;

	ActiveCode.sendFlagsAll = 0;
	ActiveCode.infoFlagsAll = 0;
	ActiveCode.reqFlagsAll = 0;
	
	if( renderFlags & MICROCODE_MASK_SECOND_RENDER )
	{
		bMultiPass = false;			

		if( renderFlags & MICROCODE_SHADOWED)
			idxBaseCodeSet = CSET_SHADOWED;
		else if( renderFlags & MICROCODE_VERTEX_TEXTURE_PROJECTION)
			idxBaseCodeSet = CSET_VERTEX_TEXTURE_PROJECTION;

	} 
	else
	{
		if( renderFlags & MICROCODE_SHADOWER )
		{
			bMultiPass = false;

			idxBaseCodeSet = CSET_SHADOWER;
		}
		else 
		{		
			bMultiPass = renderFlags & MICROCODE_MASK_MULTIPASS;

			if( renderFlags & (MICROCODE_POINT_LIGHTS | MICROCODE_DIRECTIONAL_LIGHTS))
			{
				if( (renderFlags & (MICROCODE_POINT_LIGHTS | MICROCODE_DIRECTIONAL_LIGHTS)) == (MICROCODE_POINT_LIGHTS | MICROCODE_DIRECTIONAL_LIGHTS))
					idxBaseCodeSet = CSET_ALIGHT;
				else if (renderFlags & MICROCODE_POINT_LIGHTS)
					idxBaseCodeSet = CSET_PLIGHT;
				else
					idxBaseCodeSet = CSET_DLIGHT;
				
				rndInfo.lightSpaceRequested = LIGHT_SPACE_MODEL;			
			}
			else if( (renderFlags & MICROCODE_FOG)  ||  (renderFlags & MICROCODE_TEXTURE_MTX)  ||  rndInfo.alphaMultiplier != 1.0f)
				idxBaseCodeSet = CSET_ALL;
			else
				idxBaseCodeSet = CSET_BASIC;
	
			if( bMultiPass )
				idxBaseCodeSet++;
		}

	}

	VUCodeEntry * pBaseCode = selectBaseCode( *(pCodeSetArray[idxBaseCodeSet]), renderFlags);
	
	
	RENDER_ASSERT( pBaseCode);
	
	rndInfo.drawFlags = renderFlags;
	
	if( pBaseCode->pParent)
	{
		ActiveCode.pBasePass = pBaseCode->pParent;
		
		ActiveCode.sendFlagsAll |= pBaseCode->pParent->sendFlags;
		ActiveCode.infoFlagsAll |= pBaseCode->pParent->infoFlags;
		ActiveCode.reqFlagsAll |= pBaseCode->pParent->reqFlags;
	
		ActiveCode.pExtraPasses[ActiveCode.numExtraPasses++] = pBaseCode;

		ActiveCode.sendFlagsAll |= pBaseCode->sendFlags;
		ActiveCode.infoFlagsAll |= pBaseCode->infoFlags & 0xffff;
		ActiveCode.reqFlagsAll |= pBaseCode->reqFlags;
	}
	else
	{
		ActiveCode.pBasePass = pBaseCode;

		ActiveCode.sendFlagsAll |= pBaseCode->sendFlags;
		ActiveCode.infoFlagsAll |= pBaseCode->infoFlags;
		ActiveCode.reqFlagsAll |= pBaseCode->reqFlags;
	}

	
	if( bMultiPass )
	{
		if( renderFlags & MICROCODE_ENV_MAP )
		{
			unsigned int vuIdx;
			switch( rndInfo.mEnvData.mType )
			{
			default:
			case ENVMAP_TYPE_STD_ENV_MAP:
				vuIdx = CENT_ENVMAP;
				break;
			case ENVMAP_TYPE_STD_SPHERE_MAP:
				vuIdx = CENT_SPHEREMAP;
				break;
			case ENVMAP_TYPE_REF_ENV_MAP:
				vuIdx = CENT_ENVREFMAP;
				break;
			case ENVMAP_TYPE_REF_SPHERE_MAP:
				vuIdx = CENT_SPHEREREFMAP;
				break;
			}

			ActiveCode.sendFlagsAll |= pMiscArray[vuIdx]->sendFlags;
			ActiveCode.infoFlagsAll |= pMiscArray[vuIdx]->infoFlags & 0xffff;
			ActiveCode.reqFlagsAll  |= pMiscArray[vuIdx]->reqFlags;
			
			ActiveCode.pExtraPasses[ActiveCode.numExtraPasses++] = pMiscArray[vuIdx];
		}
	}
	
	rndInfo.requirementsFlags |= ActiveCode.reqFlagsAll;
	
}
		
extern VUClipperSet StdClipper;
			
void VUDataClass::basicPrepDMA( DDMAObject &Model, const PS2Matrix4x4 & Mtx1, const PS2Matrix4x4 & Mtx2, const VURenderInfo & rndInfo, const VUCodeSetup & Setup)
{
	unsigned int					vuAddrBasePass;
	unsigned int					vuAddrClipperSetup, vuAddrClipperClip;
	unsigned int					vuAddrExtraPasses[SETUP_MAX_EXTRA_PASSES];

	RENDER_ASSERT( p_CodeManager);	
	
	if( Setup.sendFlagsAll & VU_SEND_BLOCK_ENV_MAP )
		prepEnvMappingMatrix( rndInfo, Mtx1 );

	if( Setup.sendFlagsAll & (VU_SEND_DATA_PLIGHTS | VU_SEND_DATA_DLIGHTS) )
	{
		prepLightingInfo( rndInfo );

		// --- need to pass down the scale that was applied to the Model
		if( d_LightingInfo.numPointLights )
		{
			d_LightingInfo.PointLightColors[ 3 ][ 0 ] = sqrtf( (Mtx1[0][0] * Mtx1[0][0])+(Mtx1[0][1] * Mtx1[0][1])+(Mtx1[0][2] * Mtx1[0][2]));
			d_LightingInfo.PointLightColors[ 3 ][ 1 ] = sqrtf( (Mtx1[1][0] * Mtx1[1][0])+(Mtx1[1][1] * Mtx1[1][1])+(Mtx1[1][2] * Mtx1[1][2]));
			d_LightingInfo.PointLightColors[ 3 ][ 2 ] = sqrtf( (Mtx1[2][0] * Mtx1[2][0])+(Mtx1[2][1] * Mtx1[2][1])+(Mtx1[2][2] * Mtx1[2][2]));
			d_LightingInfo.PointLightColors[ 3 ][ 3 ] = *( ( float * )&d_LightingInfo.numPointLights );
		}
		
		my_sceVu0TransposeMatrix( d_LightingInfo.DirLightMtx, d_LightingInfo.DirLightMtx );		
	}
	
	VUCodeEntry * pBasePass = Setup.pBasePass;

#ifdef OBJ_DEBUG
	static unsigned int f = MasterList::GetFrameNumber( );
	if( f == MasterList::GetFrameNumber( ) ) 
		return;

	objCount++;
	
	if( (objNoDrawID != 0)  &&  (objCount == objNoDrawID) )
		return;
		
	if( (objDrawID != 0)  &&  (objCount != objDrawID) )
		return;
		
	if( (objNoDrawAddr != 0)  &&  (objNoDrawAddr == (unsigned int) &Model))
		return;

	if( (objDrawAddr != 0)  &&  (objDrawAddr != (unsigned int) &Model))
		return;
		
#endif

// ------------ calculate BASE and OFFSET registers --------------------------
	unsigned int modelVUMemBase = (Model.d_Flags >> 24) & 0xff;
	unsigned int myVUMemBase, myVUMemOffset;
	
	RENDER_ASSERT( modelVUMemBase >= d_VUMemBase);
	if( Setup.infoFlagsAll & VU_USES_MODEL_BASE_OFFSET)
	{
		myVUMemBase = modelVUMemBase;	
		myVUMemOffset = (1024 - myVUMemBase) / 2;
	} else {
		myVUMemBase = d_VUMemBase;
		myVUMemOffset = d_VUMemOffset;
	}
	
	bool baseOffsetChanged = !MasterList::IsBaseOffsetSame( myVUMemBase, myVUMemOffset );

// ----------- pre-calc some DMATag information ------------------------------
	bool bAppendFlushA = false;

	unsigned int qwc = baseOffsetChanged ? 2 : 1;
	unsigned int unpack = 9;

	unsigned int lclDrawFlags = rndInfo.drawFlags;

	unsigned int baseSendFlags = Setup.sendFlagsAll & VU_SEND_DATA_BASE_MASK;

// ------------ process the VUCode Entries -----------------------------------
	unsigned int 		vuAddr;
	bool 					bExtraCode = ((pBasePass->infoFlags & VU_USES_CLIPPER_MASK)  ||  Setup.numExtraPasses) ? true : false;

	// --- base Pass ----------------------------------------------------
	p_CodeManager->loadManagedCode( pBasePass->pFunc->code( ), vuAddr, true, bExtraCode ? false : true);
	vuAddrBasePass = vuAddr + pBasePass->pFunc->offset( );
	
	if( pBasePass->infoFlags & VU_USES_STD_CLIPPER)
	{
		p_CodeManager->loadManagedCode( StdClipper.Code, vuAddr, true, Setup.numExtraPasses ? false : true);
	
		if( StdClipper.pSetup)
			vuAddrClipperSetup = vuAddr + StdClipper.pSetup->offset( );
	
		if( StdClipper.pClip)
			vuAddrClipperClip	= vuAddr + StdClipper.pClip->offset( );
	}

	if( baseSendFlags & VU_SEND_MTX_CLIP_SCREEN )
		unpack += 2;
	
	if( baseSendFlags & VU_SEND_MTX_TEXTURE_4x4 )
		unpack += 4;

	if( baseSendFlags & VU_SEND_MTX_TEXTURE_3x4 )
		unpack += 4;
			
	if( baseSendFlags & VU_SEND_DATA_FOG )	
		unpack++;
		
	if( baseSendFlags & VU_SEND_BLOCK_ALL )
		unpack += 3;
	else if( baseSendFlags & VU_SEND_BLOCK_VTP )
		unpack += 5;
	else if( baseSendFlags & VU_SEND_BLOCK_SHADOW_MAP )
		unpack += 5;

	// --- extra Passes -------------------------------------------------
	for( unsigned int pass = 0; pass < Setup.numExtraPasses; pass++)
	{
		VUCodeEntry * pPass = Setup.pExtraPasses[pass];
		
		p_CodeManager->loadManagedCode( pPass->pFunc->code( ), vuAddr, true, pass == (Setup.numExtraPasses - 1) ? true : false);
		vuAddrExtraPasses[pass] = vuAddr + pPass->pFunc->offset( );

		if( pPass->infoFlags & VU_USES_CORE)
			unpack++;

		if( pPass->sendFlags & VU_SEND_DATA_LIGHTS )
		{
			unpack++;		
		
			if( pPass->sendFlags & VU_SEND_DATA_PLIGHTS )
				unpack += 7;
		
			if( pPass->sendFlags & VU_SEND_DATA_DLIGHTS )
				unpack += 6;

			if( pPass->sendFlags & VU_SEND_DATA_ALIGHT )
				unpack += 1;
		}

		if( pPass->sendFlags & VU_SEND_BLOCK_ENV_MAP )	
		{
			unpack += 6 + 1 + NUM_REGS_IN_BASE_GSPACKET;// + 1; 
//			bAppendFlushA = true;
		}
	}
	
// ------------ build dma tag ------------------------------------------------
	qwc += unpack;

	if( Model.d_Flags & OBJ_FLAG_DYNAMIC_TEXTURE )
		MasterList::AppendDMATAG( qwc, TAGID_CNT, 0 );
	else
		MasterList::AppendDMATAG( qwc, TAGID_CALL, Model.getDMAPtr( ) );

	MasterList::Append( createNOP( ) );

// ------------ add BASE / OFFSET register, padding, and UNPACK --------------
	if(baseOffsetChanged)
	{
		if( !MasterList::GetFlushed( ) )
		{
			MasterList::Append( createFLUSHA( ) );
			MasterList::SetFlushed( true );
		}
		else
		{
			MasterList::Append( createNOP( ) );
		}
		
		MasterList::Append( createBASE( myVUMemBase) );
		MasterList::Append( createOFFSET( myVUMemOffset) );
		MasterList::Append( createNOP( ) );

		MasterList::SetBase( myVUMemBase );
		MasterList::SetOffset( myVUMemOffset );
	}

	MasterList::Append( createUNPACK( 0, V4_32, unpack, 1 ));

// ------------ add Mtxs -----------------------------------------------------
	MasterList::AppendMtx( Mtx2);
	MasterList::AppendMtx( Mtx1);

// ------------ add base Pass Data -------------------------------------------
	if( baseSendFlags & VU_SEND_DATA_FOG )
	{
		float 			fogScale, fogConst;
		
		if( rndInfo.fogFar == 0.0f)
		{
			fogScale = 0.0f;
			fogConst = 255.0f;
		}
		else
		{
			// TJC  - fix floating point rounding error.  As the
			// GS takes fog values as 8.4, there are 4 bits of error
			// that the equation can absorb, so we generate fog values
			// from 0.125 - 254.875  to avoid overflowing/underflowing
			// the fog value.

			fogScale = 254.875f / (rndInfo.fogFar - rndInfo.fogNear);
			fogConst = rndInfo.fogFar * fogScale + 0.125f;
			
			//printf("%x %x ", *(int*)&rndInfo.fogNear, *(int*)&rndInfo.fogFar);
			//printf("%x %f\n", *(int*)&fogConst, fogConst);
			//(*(int*)&fogConst)+=1;
			//printf("%x %f\n", *(int*)&fogConst, fogConst);
		}

		// FogNear, FogFar, FogScale, FogConst

		MasterList::Append4xF( rndInfo.fogNear,
										rndInfo.fogFar,
										fogScale,
										fogConst );
	}


// ------------ add Block Data -----------------------------------------------	
	if( baseSendFlags & VU_SEND_BLOCK_ALL )
	{
		const 			PS2Matrix4x4 & texMtx = rndInfo.mTexture;
		
		// TexMtx 1
		MasterList::Append4xF( texMtx[0][0],
									  texMtx[0][1],
									  0.0f,
									  ( baseSendFlags & VU_SEND_BLOCK_ALL_ALPHA_EARLY ) ? rndInfo.alphaMultiplier : 0.0f );

		// TexMtx 2
		MasterList::Append4xF( texMtx[1][0],
									  texMtx[1][1],
									  0.0f,
									  0.0f );
		
		// TexMtx 3
		MasterList::Append4xF( texMtx[2][0],
			                    texMtx[2][1],
									  texMtx[2][2],
									  ( baseSendFlags & VU_SEND_BLOCK_ALL_ALPHA_EARLY ) ? 0.0f : rndInfo.alphaMultiplier );
	}
	else if( baseSendFlags & VU_SEND_BLOCK_SHADOW_MAP)
	{
		sceVu0FMATRIX  *pTexMtx = (PS2Matrix4x4 *) &rndInfo.mTexture;
		
		// TexMtx 1, 2, 3, & 4
		MasterList::AppendMtx( *pTexMtx);
		
		// Shadow Color
		MasterList::Append4( rndInfo.ShadowR);	
		MasterList::Append4( rndInfo.ShadowG);
		MasterList::Append4( rndInfo.ShadowB);
		MasterList::AppendF( rndInfo.alphaMultiplier );
	}
	else if( baseSendFlags & VU_SEND_BLOCK_VTP)
	{
		PS2Matrix4x4	ClipTexMtx;
		sceVu0FMATRIX  *pTexMtx = (PS2Matrix4x4 *) &rndInfo.mTexture;

		if(lclDrawFlags & MICROCODE_CLIP)
		{ // we need texMtx = Clip->Texture
			PS2Matrix4x4	LocalClipMtx, ClipLocalMtx;
			
			sceVu0MulMatrix( LocalClipMtx, *(PS2Matrix4x4 *) &Mtx2, *(PS2Matrix4x4 *) &Mtx1); 	// Local->Clip
			PS2Renderer::Invert( ClipLocalMtx, LocalClipMtx); 												// Clip->Local
			sceVu0MulMatrix( ClipTexMtx, *(PS2Matrix4x4 *) &rndInfo.mTexture, ClipLocalMtx); 	// Clip->Texture
			
			pTexMtx = &ClipTexMtx;
		}
		
		// TexMtx 1, 2, 3, & 4
		MasterList::AppendMtx( *pTexMtx);

		// Vertex Color
		MasterList::Append4( rndInfo.ShadowR);	
		MasterList::Append4( rndInfo.ShadowG);
		MasterList::Append4( rndInfo.ShadowB);		
		MasterList::AppendF( rndInfo.alphaMultiplier );
	}
		
	const PS2Matrix4x4 * viewMtx;
	
	if( baseSendFlags & VU_SEND_MTX_CLIP_SCREEN )
	{
		if(lclDrawFlags & MICROCODE_CLIP)
			viewMtx = &rndInfo.mClipToScreen;
		else
			viewMtx = &rndInfo.mCullToScreen;

		MasterList::AppendF( (*viewMtx)[3][0] );
		MasterList::AppendF( (*viewMtx)[3][1] );
		MasterList::Append4( vuAddrClipperSetup );
		MasterList::Append4( vuAddrClipperClip );

		MasterList::Append4xF( 	(*viewMtx)[0][0],
										(*viewMtx)[1][1],
										(*viewMtx)[2][2],
										(*viewMtx)[3][2] );
	}

// ------------ add extra Pass Data -------------------------------------------
	for( unsigned int pass = 0; pass < Setup.numExtraPasses; pass++)
	{
		VUCodeEntry * pPass = Setup.pExtraPasses[pass];

		unsigned int passSendFlags = pPass->sendFlags & VU_SEND_DATA_PASS_MASK;
		
		if( pPass->infoFlags & VU_USES_CORE)
		{
			MasterList::Append4x4( vuAddrExtraPasses[pass],
											0,
											0,
											0 );
		}
			
	// ------------ add Lights ---------------------------------------------
		if( passSendFlags & VU_SEND_DATA_LIGHTS )
		{
			if( passSendFlags & VU_SEND_DATA_PLIGHTS )
			{
				u_long128* dl = (u_long128*) MasterList::CurrentDMAPointer();
				dl[0] = *( ( u_long128 * ) &d_LightingInfo.PointLightMtx[ 0 ][ 0 ] );
				dl[1] = *( ( u_long128 * ) &d_LightingInfo.PointLightMtx[ 1 ][ 0 ] );
				dl[2] = *( ( u_long128 * ) &d_LightingInfo.PointLightMtx[ 2 ][ 0 ] );
				dl[3] = *( ( u_long128 * ) &d_LightingInfo.PointLightColors[ 0 ][ 0 ] );
				dl[4] = *( ( u_long128 * ) &d_LightingInfo.PointLightColors[ 1 ][ 0 ] );
				dl[5] = *( ( u_long128 * ) &d_LightingInfo.PointLightColors[ 2 ][ 0 ] );
				dl[6] = *( ( u_long128 * ) &d_LightingInfo.PointLightColors[ 3 ][ 0 ] ); // actually the scale factor
				MasterList::CurrentDMAPointer() = (u_char*) &dl[7];
			}

			if( passSendFlags & VU_SEND_DATA_DLIGHTS )
			{
				u_long128* dl = (u_long128*) MasterList::CurrentDMAPointer();
				dl[0] = *( ( u_long128 * ) &d_LightingInfo.DirLightMtx[ 0 ][ 0 ] );
				dl[1] = *( ( u_long128 * ) &d_LightingInfo.DirLightMtx[ 1 ][ 0 ] );
				dl[2] = *( ( u_long128 * ) &d_LightingInfo.DirLightMtx[ 2 ][ 0 ] );
				dl[3] = *( ( u_long128 * ) &d_LightingInfo.DirLightColors[ 0 ][ 0 ] );
				dl[4] = *( ( u_long128 * ) &d_LightingInfo.DirLightColors[ 1 ][ 0 ] );
				dl[5] = *( ( u_long128 * ) &d_LightingInfo.DirLightColors[ 2 ][ 0 ] );
				MasterList::CurrentDMAPointer() = (u_char*) &dl[6];
			}
			
			if( passSendFlags & VU_SEND_DATA_ALIGHT)
				MasterList::Append16( *( ( u_long128 * ) &d_LightingInfo.DirLightColors[ 3 ][ 0 ] ) ); // actually the Ambient Light
			
			MasterList::Append4xF( 1.0f,		// Vertex Add Multiplier
										  0.0f,		// Vertex Mul Multiplier
										  1.0f,		// Vertex Alpha Multiplier
										  0.0f );	// Constant Alpha
		}		
	// ------------ add Env Map block ------------------------------------------
		if( passSendFlags & VU_SEND_BLOCK_ENV_MAP)
		{
			PS2Vector4				eyeLocal;

			MasterList::Append4x4( myVUMemBase,
				                   myVUMemBase + myVUMemOffset,
										 0,
										 vuAddrExtraPasses[pass] );

			MasterList::AppendMtx( rndInfo.mEnvData.mFinalTransform );

			sceVu0ApplyMatrix( eyeLocal, *(PS2Matrix4x4 *) & rndInfo.mWorldToModel, *(PS2Vector4 *) &rndInfo.mEnvData.mEyePosition );

			MasterList::Append16( *(u_long128 *) &eyeLocal );
				
			// send EV Texture
			unsigned int 	bindEVMap = rndInfo.mEnvData.mEVMapTextureBind & 0x7FFFFFFF;
			
			u_long128 * pEVMapPacket = (u_long128 *)(*p_TextureDataCallback)( bindEVMap, true, 0);

			MasterList::Append( GIFInline::createGIFTAG( NUM_REGS_IN_BASE_GSPACKET, 1, 0xE, false) );
			MasterList::CopyToMasterList(pEVMapPacket, NUM_REGS_IN_BASE_GSPACKET);
		}
	
	// ------------ add Sphere Map block ------------------------------------------
		if( passSendFlags & VU_SEND_BLOCK_SPHERE_MAP)
		{
			PS2Matrix4x4		EVToTexture;

			PS2Matrix4x4		EVTemp, EVTrans, EVFinal;
			
			MasterList::Append4x4( myVUMemBase,
				                   myVUMemBase + myVUMemOffset,
										 0,
										 vuAddrExtraPasses[pass] );

			// send Matrix
			const PS2Matrix4x4 *		pEVMapMtx;
					
			switch( rndInfo.drawFlags & LOCAL_MATRIX_MASK)
			{
			case LOCAL_TO_WORLD:
				sceVu0MulMatrix( EVTemp, * (PS2Matrix4x4 *) &rndInfo.mWorldToCamera, *(PS2Matrix4x4 *) &Mtx1 );		
				pEVMapMtx = & EVTemp;
				break;
			case LOCAL_TO_CAMERA:
				pEVMapMtx = & Mtx1;
				break;
			default:
				// unknown input matrix type specified.
				RENDER_ASSERT(FALSE);
				break;
			}

			sceVu0UnitMatrix( EVToTexture );
			EVToTexture[0][0] = EVToTexture[1][1] = .5f;
			EVToTexture[3][0] = EVToTexture[3][1] = .5f;

			PS2Renderer::Normalize( EVTrans, *pEVMapMtx );
		
			sceVu0MulMatrix( EVFinal, EVToTexture, EVTrans );

			MasterList::AppendMtx( EVFinal);
				
			MasterList::Append4x4( 0, 0, 0, 0 );

			// send EV Texture
			unsigned int 	bindEVMap = rndInfo.mEnvData.mEVMapTextureBind & 0x7FFFFFFF;
			
			u_long128 * pEVMapPacket = (u_long128 *)(*p_TextureDataCallback)( bindEVMap, true, 0);

			MasterList::Append( GIFInline::createGIFTAG( NUM_REGS_IN_BASE_GSPACKET, 1, 0xE, false) );
			MasterList::CopyToMasterList(pEVMapPacket, NUM_REGS_IN_BASE_GSPACKET);
		}
	}
	
// ------------ add GIFTag + PRIMReg -----------------------------------------
	unsigned int lclPRIMReg = Model.d_PRIMReg | (lclDrawFlags & GS_PRIM_MASK);
		
	if( lclDrawFlags & MICROCODE_CLIP)
		lclPRIMReg = Model.d_PRIMReg | (lclDrawFlags & GS_PRIM_MASK) | PRIM_TRIANGLE_FAN;
	else
	{
		if( lclDrawFlags & WIREFRAME_MODE)
			lclPRIMReg = Model.d_PRIMReg | (lclDrawFlags & GS_PRIM_MASK) | PRIM_LINE_STRIP;
		else
			lclPRIMReg = Model.d_PRIMReg | (lclDrawFlags & GS_PRIM_MASK) | PRIM_TRIANGLE_STRIP;
	}

	if( Setup.infoFlagsAll & VU_USES_REG_VERTEX )
	{
		lclPRIMReg &= ~(GS_GOURAUD | GS_TEXTURED);

		d_GIFTag.setNREGS( 1 );
		d_GIFTag.setReg( 0, GIFTag::REG_XYZ2 );
	}
	else
	{
		d_GIFTag.setNREGS( 3 );
		d_GIFTag.setReg( 0, GIFTag::REG_ST );
		d_GIFTag.setReg( 1, GIFTag::REG_RGBAQ );
		d_GIFTag.setReg( 2, pBasePass->infoFlags & VU_USES_XYZF2 ? GIFTag::REG_XYZF2 : GIFTag::REG_XYZ2);
	}

	GIFTag * p_GIFTag = &d_GIFTag;
	
	p_GIFTag->setPRIM( lclPRIMReg | pBasePass->extraPRIMBits);

	if( ((unsigned int) p_GIFTag) % 16)
	{
		u_int * data = (u_int *) p_GIFTag;
		MasterList::Append4x4( data[0], data[1], data[2], data[3] );
	}
	else
	{
		// this works instead of the memcpy if the alignment is right... 
		// we dont however get an exception if its not!
		MasterList::Append16( * ((u_long128 *) p_GIFTag) );
	}

// ------------ Add the MSCAL and MSCNT --------------------------------------
	MasterList::Append4x4( createMSCAL( (vuAddrBasePass << 3) ),
								  createMSCNT( ),
								  createNOP( ),
								  createNOP( ) );


// ------------ Fixup / Add the Dynamic Texture Table
	if( Model.d_Flags & OBJ_FLAG_DYNAMIC_TEXTURE)
		addToMasterList_DTTable( Model.getDTTableData( ), Model.getDMAPtr( ) );

// ------------ Done! Mark that we arent flushed -----------------------------
	if( bAppendFlushA )
	{
		MasterList::Append( createFLUSHA( ) );
		MasterList::SetFlushed( true );
	}
	else
		MasterList::SetFlushed( false );		
}
		
/* old school	
void VUDataClass::basicPrepDMA(
	DDMAObject &Model, const PS2Matrix4x4 & Mtx1, const PS2Matrix4x4 & Mtx2, const VURenderInfo & rndInfo,
	VUCodeSet* Std, VUCodeSet* All, VUCodeSet* VTP, VUCodeSet* ShM, VUCodeSet* PLight,
	bool earlyAlphaMult)
{
unsigned int			vuAddr, vuCallAddr, vuGCSetupAddr, vuGCClipAddr;
unsigned int 			vuAddrEvMap;

u_int						lclPRIMReg;
u_int						lclDrawFlags;

u_int 					qwc, unpack;

bool 						baseOffsetChanged;
bool						foggedpass = false;

VUCodeSet *				pCodeSet;

	vuGCSetupAddr = 0;
	vuGCClipAddr = 0;

	RENDER_ASSERT( p_CodeManager);

	prepLightingInfo(rndInfo);

	lclDrawFlags = rndInfo.drawFlags;

	// --- need to pass down the scale that was applied to the Model
	if( d_LightingInfo.numPointLights )
	{
		d_LightingInfo.PointLightColors[ 2 ][ 0 ] = sqrtf( (Mtx1[0][0] * Mtx1[0][0])+(Mtx1[0][1] * Mtx1[0][1])+(Mtx1[0][2] * Mtx1[0][2]));
		d_LightingInfo.PointLightColors[ 2 ][ 1 ] = sqrtf( (Mtx1[1][0] * Mtx1[1][0])+(Mtx1[1][1] * Mtx1[1][1])+(Mtx1[1][2] * Mtx1[1][2]));
		d_LightingInfo.PointLightColors[ 2 ][ 2 ] = sqrtf( (Mtx1[2][0] * Mtx1[2][0])+(Mtx1[2][1] * Mtx1[2][1])+(Mtx1[2][2] * Mtx1[2][2]));
	}
	else
	{
		lclDrawFlags &= ~MICROCODE_DYNAMIC_LIGHTING;
	}

#ifdef OBJ_DEBUG	
//	if( !(rndInfo.drawFlags & MICROCODE_SHADOWED))
//		return;

//	if( !rndInfo.drawFlags & MICROCODE_CLIP )
//		return;

//	if( !(rndInfo.drawFlags & MICROCODE_FOG) )
//		return;
#endif
	

	// microcodes required for modes...:
	if(!PLight)
		lclDrawFlags &= ~MICROCODE_DYNAMIC_LIGHTING;
	if(!VTP)
		lclDrawFlags &= ~MICROCODE_VERTEX_TEXTURE_PROJECTION;
	if(!ShM)
		if(lclDrawFlags & MICROCODE_SHADOWED)
			return;
	ASSERT(All);
	ASSERT(Std);
	
	lclPRIMReg = Model.d_PRIMReg | (lclDrawFlags & GS_PRIM_MASK);
		
	if( lclDrawFlags & MICROCODE_CLIP)
		lclPRIMReg = Model.d_PRIMReg | (lclDrawFlags & GS_PRIM_MASK) | PRIM_TRIANGLE_FAN;
	else
	{
		if( lclDrawFlags & WIREFRAME_MODE)
			lclPRIMReg = Model.d_PRIMReg | (lclDrawFlags & GS_PRIM_MASK) | PRIM_LINE_STRIP;
		else
			lclPRIMReg = Model.d_PRIMReg | (lclDrawFlags & GS_PRIM_MASK) | PRIM_TRIANGLE_STRIP;
	}
		
	// start inserting the codes for uploading the microcode
	// insert codes to download ucode if nessisary...
	d_GIFTag.setReg( 1, GIFTag::REG_RGBAQ);
	
	if( lclDrawFlags & MICROCODE_SHADOWED)
	{
		lclDrawFlags &= (u_int)(~MICROCODE_FOG);
		lclPRIMReg |= 1L << 5; // TJC - this doesnt happen in color set data?
//		lclPRIMReg &= ~ (1L << 4); // turn off texturing
		
		pCodeSet = ShM;
		d_GIFTag.setReg( 2, GIFTag::REG_XYZF2);
	}
	else if (lclDrawFlags & MICROCODE_VERTEX_TEXTURE_PROJECTION)
	{
		lclDrawFlags &= (u_int)(~MICROCODE_FOG);
		lclPRIMReg |= 1L << 5;
		
		pCodeSet = VTP;
		d_GIFTag.setReg( 2, GIFTag::REG_XYZF2);
	}
	else if (lclDrawFlags & MICROCODE_DYNAMIC_LIGHTING)
	{
		pCodeSet = PLight;
//		pCodeSet = All;
		d_GIFTag.setReg( 2, GIFTag::REG_XYZF2);
		foggedpass = true;
	}
	else if (lclDrawFlags & MICROCODE_EVMAP)
	{
		pCodeSet = All;
		d_GIFTag.setReg( 2, GIFTag::REG_XYZF2);
		foggedpass = true;
	}
	else if( lclDrawFlags & MICROCODE_FOG || rndInfo.alphaMultiplier != 1.0f)
	{
		pCodeSet = All;
		d_GIFTag.setReg( 2, GIFTag::REG_XYZF2);
		foggedpass = true;
	}
	else
	{
		pCodeSet = Std;
		d_GIFTag.setReg( 2, GIFTag::REG_XYZ2);
	}
		
		
	d_GIFTag.setPRIM( lclPRIMReg );
	
	bool bSingleCode = (lclDrawFlags & MICROCODE_EVMAP) ? false : true;
		

	if( lclDrawFlags & MICROCODE_CLIP)
	{
// TJC - which is it?  is it fatal or not?
//		RENDER_ASSERT( pCodeSet->Clip);
		if( ! pCodeSet->Clip)
			return;
		
		p_CodeManager->loadManagedCode( pCodeSet->Clip->code( ), vuAddr, true, false );
		vuCallAddr = vuAddr + pCodeSet->Clip->offset( );
		
		if( pCodeSet->ClipCore)
		{
			p_CodeManager->loadManagedCode( * pCodeSet->ClipCore, vuAddr, true, bSingleCode );
			
			if( pCodeSet->ClipCoreSetup)
				vuGCSetupAddr = vuAddr + pCodeSet->ClipCoreSetup->offset( );
				
			if( pCodeSet->ClipCoreRun)
				vuGCClipAddr = vuAddr + pCodeSet->ClipCoreRun->offset( );
		}
	}
	else if( lclDrawFlags & MICROCODE_CULL)
	{
//		RENDER_ASSERT( pCodeSet->Cull);
		if( ! pCodeSet->Cull)
			return;
		
		p_CodeManager->loadManagedCode( pCodeSet->Cull->code( ), vuAddr, true, bSingleCode );
		vuCallAddr = vuAddr + pCodeSet->Cull->offset( );
	}
	else
	{
		p_CodeManager->loadManagedCode( pCodeSet->Xfrm->code( ), vuAddr, true, bSingleCode );
		vuCallAddr = vuAddr + pCodeSet->Xfrm->offset( );
	}

	if( lclDrawFlags & MICROCODE_EVMAP)
	{
		p_CodeManager->loadManagedCode( Static_EVMap, vuAddrEvMap, true, true);
		vuAddrEvMap += Static_EVMap_Entry.offset( );
	}
	
#ifdef OBJ_DEBUG
	{
		static unsigned int f = MasterList::GetFrameNumber();
		if(f == MasterList::GetFrameNumber()) return;
		// uncomment this to run this microcode only once per frame
		//f = MasterList::GetFrameNumber();
	}

	objCount++;
	
	if( (objNoDrawID != 0)  &&  (objCount == objNoDrawID) )
		return;
		
	if( (objDrawID != 0)  &&  (objCount != objDrawID) )
		return;
		
	if( (objNoDrawAddr != 0)  &&  (objNoDrawAddr == (unsigned int) &Model))
		return;

	if( (objDrawAddr != 0)  &&  (objDrawAddr != (unsigned int) &Model))
		return;
		
#endif
	unsigned int ModelVUMemBase = (Model.d_Flags >> 24) & 0xff;
	unsigned int myVUMemBase, myVUMemOffset;
	
	RENDER_ASSERT( ModelVUMemBase >= d_VUMemBase);
	if( rndInfo.drawFlags & MICROCODE_EVMAP)
	{
//		RENDER_ASSERT( ModelVUMemBase >= 45);

		myVUMemBase = ModelVUMemBase;	
		myVUMemOffset = (1024 - myVUMemBase) / 2;
	} else {
		myVUMemBase = d_VUMemBase;
		myVUMemOffset = d_VUMemOffset;
	}
	
	baseOffsetChanged = !MasterList::IsBaseOffsetSame( myVUMemBase, myVUMemOffset );
	
	// now, start dma'ing the first matrix into spr...
	// reserve space for extra codes
	if(baseOffsetChanged)
	{
		qwc = 2;
	}
	else
	{
		qwc = 1;
	}

	unpack = 9;
	
	// --- add lights into the packet
	if (lclDrawFlags & MICROCODE_DYNAMIC_LIGHTING )
		unpack += 5;

	if( foggedpass )
		unpack += 4;
	else if(lclDrawFlags & (MICROCODE_SHADOWED | MICROCODE_VERTEX_TEXTURE_PROJECTION))
		unpack += 6;

	if( lclDrawFlags & (MICROCODE_CLIP | MICROCODE_CULL))
		unpack += 2;

	if( lclDrawFlags & MICROCODE_EVMAP )
		unpack += 5 + 1 + ((rndInfo.EVMapTextureBind & 0x80000000) ? 6 : 5); 
		
	qwc += unpack;
	
	// build dma tag
	if( Model.d_Flags & OBJ_FLAG_DYNAMIC_TEXTURE)
		MasterList::AppendDMATAG( qwc, TAGID_CNT, 0);
	else
		MasterList::AppendDMATAG( qwc, TAGID_CALL, Model.getDMAPtr( ));

	MasterList::Append(createNOP( ));

	// build base/offset/flush stuff..
	if(baseOffsetChanged)
	{
		if(!MasterList::GetFlushed( ))
		{
			MasterList::Append(createFLUSH( ));
			MasterList::SetFlushed(true);
		}
		else
		{
			MasterList::Append(createNOP());
		}
		MasterList::Append(createBASE(myVUMemBase));
		MasterList::Append(createOFFSET(myVUMemOffset));
		MasterList::Append(createNOP());

		MasterList::SetBase( myVUMemBase );
		MasterList::SetOffset( myVUMemOffset );
	}

	// stick unpack on...
	MasterList::Append(createUNPACK( 0, V4_32, unpack, 1 ));

	// append matrices
	MasterList::AppendMtx(Mtx2);
	MasterList::AppendMtx(Mtx1);

	if( foggedpass )
	{
		const 			PS2Matrix4x4 & texMtx = rndInfo.mTexture;
		
		float 			fogScale, fogConst;
		
		if( rndInfo.fogFar == 0.0f || !(lclDrawFlags & MICROCODE_FOG))
		{
			fogScale = 0.0f;
			fogConst = 255.0f;
		}
		else
		{
			// TJC  - fix floating point rounding error.  As the
			// GS takes fog values as 8.4, there are 4 bits of error
			// that the equation can absorb, so we generate fog values
			// from 0.125 - 254.875  to avoid overflowing/underflowing
			// the fog value.

			fogScale = 254.875f / (rndInfo.fogFar - rndInfo.fogNear);
			fogConst = rndInfo.fogFar * fogScale + 0.125f;
			
			//printf("%x %x ", *(int*)&rndInfo.fogNear, *(int*)&rndInfo.fogFar);
			//printf("%x %f\n", *(int*)&fogConst, fogConst);
			//(*(int*)&fogConst)+=1;
			//printf("%x %f\n", *(int*)&fogConst, fogConst);
		}

		// FogNear, FogFar, FogScale, FogConst
		MasterList::AppendF( rndInfo.fogNear );
		MasterList::AppendF( rndInfo.fogFar );
		MasterList::AppendF( fogScale );
		MasterList::AppendF( fogConst );
		
		// TexMtx 1
		MasterList::AppendF( texMtx[0][0] );
		MasterList::AppendF( texMtx[0][1] );
		MasterList::AppendF( 0.0f );
		if(earlyAlphaMult)
			MasterList::AppendF( rndInfo.alphaMultiplier ); // TJC - alpha multiplier moved?
		else
			MasterList::AppendF( 0.0f ); // TJC - alpha multiplier moved?

		// TexMtx 2
		MasterList::AppendF( texMtx[1][0] );
		MasterList::AppendF( texMtx[1][1] );
		MasterList::AppendF( 0.0f );
		MasterList::AppendF( 0.0f );
		
		// TexMtx 3
		MasterList::AppendF( texMtx[2][0] );
		MasterList::AppendF( texMtx[2][1] );
		MasterList::AppendF( texMtx[2][2] );
		if(earlyAlphaMult)
			MasterList::AppendF( 0.0f ); // TJC - alpha multiplier moved?
		else
			MasterList::AppendF( rndInfo.alphaMultiplier ); // TJC - alpha multiplier moved?
	}
	else if( lclDrawFlags & MICROCODE_SHADOWED)
	{
//		PS2Matrix4x4	ClipTexMtx;
		sceVu0FMATRIX  *pTexMtx = (PS2Matrix4x4 *) &rndInfo.mTexture;
		
		float 			fogScale, fogConst;
		
		if( rndInfo.fogFar == 0.0f)
		{
			fogScale = 0.0f;
			fogConst = 255.0f;
		}
		else
		{
			// TJC - fix floating point rounding error.  As the
			// GS takes fog values as 8.4, there are 4 bits of error
			// that the equation can absorb, so we generate fog values
			// from 0.125 - 254.875  to avoid overflowing/underflowing
			// the fog value.

			fogScale = 254.875f / (rndInfo.fogFar - rndInfo.fogNear);
			fogConst = rndInfo.fogFar * fogScale + 0.125f;
			
			//printf("%x %x ", *(int*)&rndInfo.fogNear, *(int*)&rndInfo.fogFar);
			//printf("%x %f\n", *(int*)&fogConst, fogConst);
			//(*(int*)&fogConst)+=1;
			//printf("%x %f\n", *(int*)&fogConst, fogConst);
		}

		// FogNear, FogFar, FogScale, FogConst
		MasterList::AppendF( rndInfo.fogNear );
		MasterList::AppendF( rndInfo.fogFar );
		MasterList::AppendF( fogScale );
		MasterList::AppendF( fogConst );
		
		// TexMtx 1, 2, 3, & 4
		MasterList::AppendMtx( *pTexMtx);
		
		// Shadow Color
		MasterList::Append4( rndInfo.ShadowR);	
		MasterList::Append4( rndInfo.ShadowG);
		MasterList::Append4( rndInfo.ShadowB);
		MasterList::AppendF( rndInfo.alphaMultiplier );
	}
	else if( lclDrawFlags & MICROCODE_VERTEX_TEXTURE_PROJECTION)
	{
		PS2Matrix4x4	ClipTexMtx;
		sceVu0FMATRIX  *pTexMtx = (PS2Matrix4x4 *) &rndInfo.mTexture;

		float 			fogScale, fogConst;
		
		if(lclDrawFlags & MICROCODE_CLIP)
		{ // we need texMtx = Clip->Texture
			PS2Matrix4x4	LocalClipMtx, ClipLocalMtx;
			
			sceVu0MulMatrix( LocalClipMtx, *(PS2Matrix4x4 *) &Mtx2, *(PS2Matrix4x4 *) &Mtx1); 	// Local->Clip
			PS2Renderer::Invert( ClipLocalMtx, LocalClipMtx); 												// Clip->Local
			sceVu0MulMatrix( ClipTexMtx, *(PS2Matrix4x4 *) &rndInfo.mTexture, ClipLocalMtx); 	// Clip->Texture
			
			pTexMtx = &ClipTexMtx;
		}
		
		if( rndInfo.fogFar == 0.0f)
		{
			fogScale = 0.0f;
			fogConst = 255.0f;
		}
		else
		{
			// TJC - fix floating point rounding error.  As the
			// GS takes fog values as 8.4, there are 4 bits of error
			// that the equation can absorb, so we generate fog values
			// from 0.125 - 254.875  to avoid overflowing/underflowing
			// the fog value.

			fogScale = 254.875f / (rndInfo.fogFar - rndInfo.fogNear);
			fogConst = rndInfo.fogFar * fogScale + 0.125f;
			
			//printf("%x %x ", *(int*)&rndInfo.fogNear, *(int*)&rndInfo.fogFar);
			//printf("%x %f\n", *(int*)&fogConst, fogConst);
			//(*(int*)&fogConst)+=1;
			//printf("%x %f\n", *(int*)&fogConst, fogConst);
		}

		// FogNear, FogFar, FogScale, FogConst
		MasterList::AppendF( rndInfo.fogNear );
		MasterList::AppendF( rndInfo.fogFar );
		MasterList::AppendF( fogScale );
		MasterList::AppendF( fogConst );
		
		// TexMtx 1, 2, 3, & 4
		MasterList::AppendMtx(*pTexMtx);

		// Vertex Color
		MasterList::Append4( rndInfo.ShadowR);	
		MasterList::Append4( rndInfo.ShadowG);
		MasterList::Append4( rndInfo.ShadowB);		
		MasterList::AppendF( rndInfo.alphaMultiplier );
	}
		
	
	const PS2Matrix4x4 * viewMtx;
	if( (lclDrawFlags & MICROCODE_CLIP)  ||  (lclDrawFlags & MICROCODE_CULL) )
	{
		if(lclDrawFlags & MICROCODE_CLIP)
			viewMtx = &rndInfo.mClipToScreen;
		else
			viewMtx = &rndInfo.mCullToScreen;

		MasterList::AppendF((*viewMtx)[3][0] );
		MasterList::AppendF((*viewMtx)[3][1] );
		MasterList::Append4(vuGCSetupAddr );
		MasterList::Append4(vuGCClipAddr );
		
		MasterList::AppendF((*viewMtx)[0][0] );
		MasterList::AppendF((*viewMtx)[1][1] );
		MasterList::AppendF((*viewMtx)[2][2] );
		MasterList::AppendF((*viewMtx)[3][2] );
		
	}
	
	if( lclDrawFlags & MICROCODE_EVMAP)
	{
		PS2Matrix4x4		EVTemp, EVFinal;
		
		MasterList::Append4( myVUMemBase );
		MasterList::Append4( myVUMemBase + myVUMemOffset );
		MasterList::Append4( 0 );
		MasterList::Append4( vuAddrEvMap );

		// send Matrix
		const PS2Matrix4x4 *		pEVMapMtx;
				
		switch( rndInfo.drawFlags & LOCAL_MATRIX_MASK)
		{
		case LOCAL_TO_WORLD:
			pEVMapMtx = & rndInfo.mWorldToView;
			break;
		case LOCAL_TO_CAMERA:
			pEVMapMtx = & rndInfo.mCameraToView;
			break;
		default:
			// unknown input matrix type specified.
			RENDER_ASSERT(FALSE);
			break;
		}

		sceVu0MulMatrix( EVTemp, *(PS2Matrix4x4 *) pEVMapMtx, *(PS2Matrix4x4 *) &Mtx1);		
		PS2Renderer::Normalize( EVFinal, EVTemp);
		EVFinal[0][2] = EVFinal[1][2] = EVFinal[2][2] = 0.0f; EVFinal[3][2] = 1.0f;
		
		MasterList::AppendMtx( EVFinal);
			
		// send EV Texture
		
		bool 				includesAlpha = rndInfo.EVMapTextureBind & 0x80000000;
		unsigned int 	bindEVMap = rndInfo.EVMapTextureBind & 0x7FFFFFFF;
		unsigned int	regsEVMap = includesAlpha ? 6 : 5;
		
		u_long128 * pEVMapPacket = (u_long128 *)(*p_TextureDataCallback)( bindEVMap, includesAlpha, 0);
		
		for( unsigned int i = 0; i < regsEVMap + 1; i++)
			MasterList::Append16( pEVMapPacket[i] );
	}
	
	if( ((unsigned int) &d_GIFTag) % 16)
	{
		u_int* data = (u_int*)&d_GIFTag;
		MasterList::Append4(data[0]);
		MasterList::Append4(data[1]);
		MasterList::Append4(data[2]);
		MasterList::Append4(data[3]);
	}
	else
	{
		// this works instead of the memcpy if the alignment is right... 
		// we dont however get an exception if its not!
		MasterList::Append16( * ((u_long128 *) &d_GIFTag));
	}

	// --- add the lights
	if (lclDrawFlags & MICROCODE_DYNAMIC_LIGHTING )
	{
		MasterList::Append16(*( ( u_long128 * )&d_LightingInfo.PointLightMtx[ 0 ][ 0 ] ) );
		MasterList::Append16(*( ( u_long128 * )&d_LightingInfo.PointLightMtx[ 1 ][ 0 ] ) );
		MasterList::Append16(*( ( u_long128 * )&d_LightingInfo.PointLightColors[ 0 ][ 0 ] ) );
		MasterList::Append16(*( ( u_long128 * )&d_LightingInfo.PointLightColors[ 1 ][ 0 ] ) );
		MasterList::Append16(*( ( u_long128 * )&d_LightingInfo.PointLightColors[ 2 ][ 0 ] ) );
	}

	// add last qw...
	MasterList::Append(createMSCAL( (vuCallAddr << 3) ));
	MasterList::Append(createMSCNT( ));
	MasterList::Append(createNOP( ));
	MasterList::Append(createNOP( ));

	// TJC - masterlist will check this for us (supposedly)
	// make sure we didnt go off the end...
	//ASSERT(qwc+1 <= PACKET_SIZE);
	//ASSERT((int)spr <= (int)(&u128Packet[PACKET_SIZE]));

	// now move it to the display list...
	//MasterList::CopyToMasterList( u128Packet, qwc + 1);

	if( Model.d_Flags & OBJ_FLAG_DYNAMIC_TEXTURE)
		addToMasterList_DTTable( Model.getDTTableData( ), Model.getDMAPtr( ) );

	// mark that we arent flushed...
	MasterList::SetFlushed( false);
		
}
*/
