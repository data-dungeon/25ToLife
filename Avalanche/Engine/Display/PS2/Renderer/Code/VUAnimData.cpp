#include "VUAnimData.h"

#include "VUCodeBlock.h"

#include "VUManagedCode.h"
#include "VUManagedFunction.h"
#include "VUCodeSetupHelper.h"
#include "MasterCodeFactory.h"
#include "VUCode.h"
#include "MasterList.h"
#include "PS2Renderer.h"
#include "DMATag.h"
#include "VIFCreator.h"
#include "VUMicroTables.h"
#include "DDMAObjectClassIDs.h"

#include "GIFInline.h"
using namespace GIFInline;

#include <eestruct.h>

#include "VIFInline.h"

#define TEST_POINT_LIGHTS
#define NUM_REGS_IN_BASE_GSPACKET 6

using namespace			VIFInline;
using namespace			VUMicro;

VUManagedCode 				&AnimBasic = 	MasterCodeFactory::createManagedCode( &dbAnimBasic_CodeBegin, &dbAnimBasic_CodeEnd);
VUManagedCode 				&AnimLit = 		MasterCodeFactory::createManagedCode( &dbAnimLit_CodeBegin, &dbAnimLit_CodeEnd);

#ifdef TEST_POINT_LIGHTS
VUManagedCode				&AnimPLit =		MasterCodeFactory::createManagedCode( &dbAnimPLit_CodeBegin, &dbAnimPLit_CodeEnd );
VUManagedCode				&AnimLFCPLit = MasterCodeFactory::createManagedCode( &dbAnimLFCPLit_CodeBegin, &dbAnimLFCPLit_CodeEnd );
VUManagedCode				&AnimLFCPLitMP = 	MasterCodeFactory::createManagedCode( &dbAnimLFCPLit_MP_CodeBegin, &dbAnimLFCPLit_MP_CodeEnd );
#endif

VUManagedCode 				&AnimLC = 		MasterCodeFactory::createManagedCode( &dbAnimLC_CodeBegin, &dbAnimLC_CodeEnd);
VUManagedCode 				&AnimLF = 		MasterCodeFactory::createManagedCode( &dbAnimLF_CodeBegin, &dbAnimLF_CodeEnd);
VUManagedCode 				&AnimLFC = 		MasterCodeFactory::createManagedCode( &dbAnimLFC_CodeBegin, &dbAnimLFC_CodeEnd);

VUManagedCode				&AnimShadower = MasterCodeFactory::createManagedCode( &dbAnimShadower_CodeBegin, &dbAnimShadower_CodeEnd);

VUManagedCode				&AnimLMP = 		MasterCodeFactory::createManagedCode( &dbAnimLit_MP_CodeBegin, &dbAnimLit_MP_CodeEnd);
VUManagedCode				&AnimPLMP = 	MasterCodeFactory::createManagedCode( &dbAnimPLit_MP_CodeBegin, &dbAnimPLit_MP_CodeEnd);

VUManagedCode				&AnimLCMP = 	MasterCodeFactory::createManagedCode( &dbAnimLC_MP_CodeBegin, &dbAnimLC_MP_CodeEnd);
VUManagedCode				&AnimLFMP = 	MasterCodeFactory::createManagedCode( &dbAnimLF_MP_CodeBegin, &dbAnimLF_MP_CodeEnd);
VUManagedCode				&AnimLFCMP = 	MasterCodeFactory::createManagedCode( &dbAnimLFC_MP_CodeBegin, &dbAnimLFC_MP_CodeEnd);

VUManagedCode				&AnimEvMap = 	MasterCodeFactory::createManagedCode( &dbAnimEV_CodeBegin, &dbAnimEV_CodeEnd);

VUManagedFunction			&LitFunc = 		MasterCodeFactory::createManagedFunction( &dbAnimLit_Start);

#ifdef TEST_POINT_LIGHTS
VUManagedFunction			&PLitFunc = 	MasterCodeFactory::createManagedFunction( &dbAnimPLit_Start);
VUManagedFunction			&PLitMPFunc =	MasterCodeFactory::createManagedFunction( &dbAnimPLit_MP_Start);
VUManagedFunction			&PLitFCFunc = 	MasterCodeFactory::createManagedFunction( &dbAnimLFCPLit_Start);
VUManagedFunction			&PLitFCMPFunc = 	MasterCodeFactory::createManagedFunction( &dbAnimLFCPLit_MP_Start);

#endif

VUManagedFunction			&LFFunc = 		MasterCodeFactory::createManagedFunction( &dbAnimLF_Start);
VUManagedFunction			&LCFunc = 		MasterCodeFactory::createManagedFunction( &dbAnimLC_Start);
VUManagedFunction			&LFCFunc = 		MasterCodeFactory::createManagedFunction( &dbAnimLFC_Start);

VUManagedFunction			&ShadowerFunc = MasterCodeFactory::createManagedFunction( &dbAnimShadower_Start);

VUManagedFunction			&LitMP = 		MasterCodeFactory::createManagedFunction( &dbAnimLit_MP_Start);
VUManagedFunction			&LCMP = 			MasterCodeFactory::createManagedFunction( &dbAnimLC_MP_Start);
VUManagedFunction			&LFMP = 			MasterCodeFactory::createManagedFunction( &dbAnimLF_MP_Start);
VUManagedFunction			&LFCMP = 		MasterCodeFactory::createManagedFunction( &dbAnimLFC_MP_Start);

VUManagedFunction			&EvMap = 		MasterCodeFactory::createManagedFunction( &dbAnimEV_Start);

#define PACKET_SIZE	32
static SCRATCHPAD(u_long128, u8Packet[PACKET_SIZE]);

#define ROUND_UP_TO(x, val) ((((u_int)x) % (val)) ? (((u_int)x) + ((val)-(((u_int)x)) % (val))) : (x))


//////////////////////////////////////////////////////////////////////////////
// ick... the normal sce libraries dont declare const'ness correctly...
// so here are some replacement versions with the constness set correctly
//////////////////////////////////////////////////////////////////////////////
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
static inline void my_sceVu0CopyVector(sceVu0FVECTOR v0, const sceVu0FVECTOR v1)
{
	asm __volatile__("
		lq    $6,0x0(%1)
		sq    $6,0x0(%0)
		": : "r" (v0) , "r" (v1):"$6");
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

static inline void my_sceVu0NormalLightVector(PS2Matrix4x4 m, const PS2Vector4 l0, u_int row )
{
	PS2Vector4  t;
	my_sceVu0ScaleVector(t, l0, -1); my_sceVu0Normalize(m[row], t);
}

static inline void my_sceVu0LightColorMatrix(PS2Matrix4x4 m, const PS2Vector4 c0, const PS2Vector4 c1, const PS2Vector4 c2, const PS2Vector4 a)
{
	my_sceVu0CopyVector(m[0], c0);
	my_sceVu0CopyVector(m[1], c1);
	my_sceVu0CopyVector(m[2], c2);
	my_sceVu0CopyVector(m[3],  a);
}


//////////////////////////////////////////////////////////////////////////////
// now on to the show...
//////////////////////////////////////////////////////////////////////////////

VUAnimData::VUAnimData() :
	VUDataClass( DATA_ANIMATED ),
	d_GIFTag( 0, 1, 0, 0, 0, 3, 0x00000512),
	d_ShadowerTag( 0, 1, 0, 0, 0, 1, 0x00000005),
	bPersistantInited( false),
	pPersistantBasic( 0),
	pPersistantLit( 0)
{
	
}

#define VALIDATE_TABLES

void VUAnimData::init(VUCodeSetupHelper& helper)
{
#ifdef VALIDATE_TABLES
	validateTables( );
#endif

	helper.addGroupedCode( AnimBasic, 0xae, true);
	helper.addGroupedCode( AnimLit, 0xae, true);		
#ifdef TEST_POINT_LIGHTS
	helper.addGroupedCode( AnimPLit, 0xae, true);		
#endif
	helper.addGroupedCode( AnimLC, 0xae, true);		
	helper.addGroupedCode( AnimLF, 0xae, true);		
	helper.addGroupedCode( AnimLFC, 0xae, true);
	helper.addGroupedCode( AnimLFCPLit, 0xae, true);
	helper.addGroupedCode( AnimLFCPLitMP, 0xae, true);
	helper.addGroupedCode( AnimShadower, 0xae, true);		
	helper.addGroupedCode( AnimLMP, 0xae, true);
	helper.addGroupedCode( AnimPLMP, 0xae, true);
	helper.addGroupedCode( AnimLCMP, 0xae, true);
	helper.addGroupedCode( AnimLFMP, 0xae, true);
	helper.addGroupedCode( AnimLFCMP, 0xae, true);
	
	helper.addCode( AnimEvMap );

	if( pPersistantLit == 0)
		pPersistantLit = new unsigned int[vuAnimTableLitFogCullMPEntries];
	if( pPersistantBasic == 0)
		pPersistantBasic = new unsigned int[vuAnimTableBasicEntries];

	// force the tables to get refilled (incase for some reason they were filled previously)
	bPersistantInited = false;
}

void VUAnimData::prepDMA( DDMAObject &Model, const PS2Matrix4x4& Mtx1, const PS2Matrix4x4& Mtx2, const VURenderInfo& rndInfo)
{
u_char *					spr = (u_char *) &u8Packet[0];
u_int						vuAddr;
u_int						vuGCSetupAddr = 0;
u_int						vuGCClipAddr = 0;
u_int						lclPRIMReg;
u_int						lclDrawFlags;
unsigned int			myVUMemBase, myVUMemOffset;
unsigned int			dmaQWC, hdrUnpack;
GIFTag *					pGIFTag;

//	if( rndInfo.drawFlags & MICROCODE_SHADOWER)
//		return;
		
	// we don't shadow yet
	if( rndInfo.drawFlags & MICROCODE_SHADOWED)
		return;

	// we don't clip right now
	RENDER_ASSERT( !(rndInfo.drawFlags & MICROCODE_CLIP) );

	// copy alpha multiplier into the ambient alpha value...
//	PS2Vector4 ambient;
//	my_sceVu0CopyVector(ambient, rndInfo.mLights.mTotalAmbientLight);
//	ambient[3] = rndInfo.alphaMultiplier * 128.0f;
	prepLightingInfo(rndInfo);

	if( rndInfo.drawFlags & MICROCODE_SHADOWER ) d_LightingInfo.numPointLights = 0;
	
	my_sceVu0TransposeMatrix( d_LightingInfo.DirLightMtx, d_LightingInfo.DirLightMtx );
	my_sceVu0TransposeMatrix( d_LightingInfo.PointLightMtx, d_LightingInfo.PointLightMtx );
//	my_sceVu0LightColorMatrix( DirLightColors, 
//		rndInfo.mLights.mLight[0].mDiffuseColor, 
//		rndInfo.mLights.mLight[1].mDiffuseColor, 
//		rndInfo.mLights.mLight[2].mDiffuseColor, 
//		ambient);
		
	// get a non-const version of drawFlags
	lclDrawFlags = rndInfo.drawFlags;

	// 32 is pre-defined matrix cache size
	unsigned int ModelVUMemBase = (Model.d_Flags >> 24) & 0xff;
	
	RENDER_ASSERT( ModelVUMemBase >= d_VUMemBase);
	if( rndInfo.drawFlags & MICROCODE_ENV_MAP)
		RENDER_ASSERT( ModelVUMemBase >= 45);

	myVUMemBase = ModelVUMemBase + (32 * 4);	
	myVUMemOffset = (1024 - myVUMemBase) / 2;
	
	if( lclDrawFlags & MICROCODE_SHADOWER)
	{
		// turn of Clipping, Culling, Fogging
		lclDrawFlags &= (u_int) ~(MICROCODE_CULL | MICROCODE_CLIP | MICROCODE_FOG);
		
		p_CodeManager->loadManagedCode( AnimShadower, vuAddr, true);
		vuAddr += ShadowerFunc.offset( );
	}
	else if( lclDrawFlags & MICROCODE_POINT_LIGHTS)
	{
		if( lclDrawFlags & (MICROCODE_CULL | MICROCODE_FOG) )
		{
			if( lclDrawFlags & MICROCODE_ENV_MAP)
			{
				p_CodeManager->loadManagedCode( AnimLFCPLitMP, vuAddr, true);
				vuAddr += PLitFCMPFunc.offset( );
			}
			else
			{
				p_CodeManager->loadManagedCode( AnimLFCPLit, vuAddr, true);
				vuAddr += PLitFCFunc.offset( );
			}
			lclDrawFlags |= (MICROCODE_CULL | MICROCODE_FOG);
		} 
		else
		{
			if( lclDrawFlags & MICROCODE_ENV_MAP)
			{
				p_CodeManager->loadManagedCode( AnimPLMP, vuAddr, true);
				vuAddr += PLitMPFunc.offset( );
			}
			else
			{
				p_CodeManager->loadManagedCode( AnimPLit, vuAddr, true);
				vuAddr += PLitFunc.offset( );
			}
		}
	}
	else
	{
		if( lclDrawFlags & MICROCODE_CULL)
		{
			if( lclDrawFlags & MICROCODE_FOG)
			{
				if( lclDrawFlags & MICROCODE_ENV_MAP )
				{
					p_CodeManager->loadManagedCode( AnimLFCMP, vuAddr, true);
					vuAddr += LFCMP.offset( );
				} else {
					p_CodeManager->loadManagedCode( AnimLFC, vuAddr, true);
					vuAddr += LFCFunc.offset( );
				}
			}
			else
			{
				if( lclDrawFlags & MICROCODE_ENV_MAP )
				{
					p_CodeManager->loadManagedCode( AnimLCMP, vuAddr, true);
					vuAddr += LCMP.offset( );
				} else {
					p_CodeManager->loadManagedCode( AnimLC, vuAddr, true);
					vuAddr += LCFunc.offset( );
				}
			}
		}
		else
		{
			if( lclDrawFlags & MICROCODE_FOG)
			{
				if( lclDrawFlags & MICROCODE_ENV_MAP )
				{
					p_CodeManager->loadManagedCode( AnimLFMP, vuAddr, true);
					vuAddr += LFMP.offset( );
				} else {
					p_CodeManager->loadManagedCode( AnimLF, vuAddr, true);
					vuAddr += LFFunc.offset( );
				}
			}
			else
			{
				if( lclDrawFlags & MICROCODE_ENV_MAP )
				{
					p_CodeManager->loadManagedCode( AnimLMP, vuAddr, true);
					vuAddr += LitMP.offset( );
				} else {
					p_CodeManager->loadManagedCode( AnimLit, vuAddr, true);
					vuAddr += LitFunc.offset( );
				}
			}
		}
	}

#ifndef SHIP_IT
	{
	//	static u_int f = MasterList::GetFrameBeingGenerated();
	//	if(f == MasterList::GetFrameBeingGenerated()) return;
		// uncomment this to run this microcode only once per frame
		//f = MasterList::GetFrameNumber();
	}
#endif

	// end of Code load

	if( lclDrawFlags & MICROCODE_CLIP)
		lclPRIMReg = Model.d_PRIMReg | (lclDrawFlags & GS_PRIM_MASK) | PRIM_TRIANGLE_FAN;
	else
		lclPRIMReg = Model.d_PRIMReg | (lclDrawFlags & GS_PRIM_MASK) | PRIM_TRIANGLE_STRIP;

	if( lclDrawFlags & MICROCODE_SHADOWER)
	{
		pGIFTag = &d_ShadowerTag;
	}
	else if( lclDrawFlags & MICROCODE_FOG)
	{
		lclPRIMReg |= (1 << 5);
		d_GIFTag.setReg( 2, GIFTag::REG_XYZF2);
		pGIFTag = &d_GIFTag;
	}
	else
	{
		d_GIFTag.setReg( 2, GIFTag::REG_XYZ2);
		pGIFTag = &d_GIFTag;
	}

	pGIFTag->setPRIM( lclPRIMReg );

	dmaQWC = 0; hdrUnpack = 9; // Mtx1, Mtx2 and GIFTag are mandatory

	bool baseOffsetChanged = !MasterList::IsBaseOffsetSame( myVUMemBase, myVUMemOffset );

	if( baseOffsetChanged)
		dmaQWC++;
				
	if( !(lclDrawFlags & MICROCODE_SHADOWER)  &&  (Model.d_Flags & AnimDDMAObject::FLAG_LIGHT) )
	{
		AnimDDMAObject * pAnimModel = static_cast<AnimDDMAObject *> (&Model);

		if( pAnimModel->b_MatrixBufferLightsSet == false)
		{
			transformMatrixBufferLights( (int) pAnimModel->d_MatrixCount, pAnimModel->getMatrixBuffer( ), d_LightingInfo.DirLightMtx, d_LightingInfo.PointLightMtx );
			pAnimModel->b_MatrixBufferLightsSet = true;
		}
		if( lclDrawFlags & MICROCODE_POINT_LIGHTS )
			hdrUnpack += 7;
		else
			hdrUnpack += 4;
	}
	
	if( lclDrawFlags & MICROCODE_FOG )
		hdrUnpack++;
	
	if( lclDrawFlags & MICROCODE_CULL )
		hdrUnpack += 2;
	
	if( lclDrawFlags & MICROCODE_ENV_MAP )
		hdrUnpack += 5 + 1 + NUM_REGS_IN_BASE_GSPACKET; 
		
	if( hdrUnpack)
		dmaQWC += hdrUnpack + 1;

	// build dma tag
	if( Model.d_Flags & OBJ_FLAG_DYNAMIC_TEXTURE)
		MasterList::Append(spr, MasterList::createDMATAG( dmaQWC, TAGID_CNT, 0));
	else
		MasterList::Append(spr, MasterList::createDMATAG( dmaQWC, TAGID_CALL, Model.getDMAPtr()));
	APPEND4(spr, createFLUSH( ));

	MasterList::SetFlushed( true);

	// build base/offset/flush stuff..
	if(baseOffsetChanged)
	{
		APPEND4(spr, createNOP( ) );
		
		APPEND4(spr, createBASE( myVUMemBase) );
		
		APPEND4(spr, createOFFSET( myVUMemOffset) );
		APPEND4(spr, createNOP());

		MasterList::SetBase( myVUMemBase );
		MasterList::SetOffset( myVUMemOffset );
	}

	if( hdrUnpack )
	{
		APPEND4(spr, createUNPACK( 0, V4_32, hdrUnpack, 1 ));
	}
	
	// addition of header data to VU1 memory starts here	
	APPEND_MTX( spr, Mtx2);
	APPEND_MTX( spr, Mtx1);

	if( !(lclDrawFlags & MICROCODE_SHADOWER)  &&  (Model.d_Flags & AnimDDMAObject::FLAG_LIGHT) )
	{
		memcpy( spr, d_LightingInfo.DirLightColors, 64); spr += 64;

		if( lclDrawFlags & MICROCODE_POINT_LIGHTS )
		{
			memcpy( spr, d_LightingInfo.PointLightColors, 48); spr += 48;
		}
	}
	
	if( lclDrawFlags & MICROCODE_FOG)
	{
		float	fogScale, fogConst;
		
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
		}

		APPEND4f(spr, rndInfo.fogNear ); // fogNear
		APPEND4f(spr, rndInfo.fogFar ); 	// forFar
		APPEND4f(spr, fogScale ); 			// fogScale
		APPEND4f(spr, fogConst );			// fogConst
	}
	
	if( (lclDrawFlags & MICROCODE_CLIP)  ||  (lclDrawFlags & MICROCODE_CULL) )
	{
		const PS2Matrix4x4 & viewMtx = rndInfo.mCullToScreen;

		APPEND4f(spr, viewMtx[3][0] );
		APPEND4f(spr, viewMtx[3][1] );
		APPEND4(spr, vuGCSetupAddr );
		APPEND4(spr, vuGCClipAddr );
		
		APPEND4f(spr, viewMtx[0][0] );
		APPEND4f(spr, viewMtx[1][1] );
		APPEND4f(spr, viewMtx[2][2] );
		APPEND4f(spr, viewMtx[3][2] );
	}
	
	if( lclDrawFlags & MICROCODE_ENV_MAP )
	{
		unsigned int 		vuAddrEvMap;
		PS2Matrix4x4		EVFinal;
		
		p_CodeManager->loadManagedCode( AnimEvMap, vuAddrEvMap, true);
		vuAddrEvMap += EvMap.offset( );
		
		APPEND4( spr, myVUMemBase );
		APPEND4( spr, myVUMemBase + myVUMemOffset );
		APPEND4( spr, 0 );
		APPEND4( spr, vuAddrEvMap );

		// send Matrix
		const PS2Matrix4x4 *pEVMapMtx = ((lclDrawFlags & MICROCODE_CULL) ? &rndInfo.mViewToCull : &rndInfo.mViewToScreen);

		PS2Renderer::Invert( EVFinal, *pEVMapMtx);
		
		APPEND_MTX( spr, EVFinal);
			
		unsigned int 	bindEVMap = rndInfo.mEnvData.mEVMapTextureBind & 0x7FFFFFFF;
			
		u_long128 * pEVMapPacket = (u_long128 *)(*p_TextureDataCallback)( bindEVMap, true, 0);

		APPEND16( spr, GIFInline::createGIFTAG( NUM_REGS_IN_BASE_GSPACKET, 1, 0xE, false) );

		for( unsigned int i = 0; i < NUM_REGS_IN_BASE_GSPACKET; i++)
			APPEND16( spr, pEVMapPacket[i] );

	}
			
	
	memcpy( spr, pGIFTag, sizeof( GIFTag) ); spr += 16; 	
	
	// end of header unpack
	
	if( hdrUnpack )
	{
		APPEND4(spr, createMSCAL( (vuAddr << 3) ) );
		APPEND4(spr, createNOP( ));
		APPEND4(spr, createNOP( ));
		APPEND4(spr, createNOP( ));
	}
	else
	{
		APPEND4(spr, createNOP( ));
	}




	// now move it to the display list...
	MasterList::CopyToMasterList( u8Packet, dmaQWC + 1);

	if( Model.d_Flags & OBJ_FLAG_DYNAMIC_TEXTURE)
		addToMasterList_DTTable(Model.getDTTableData( ), Model.getDMAPtr( ) );

	// mark that we arent flushed...
	MasterList::SetFlushed( false);
	
}

void VUAnimData::getRequirementsFlags( DDMAObject &Model, VURenderInfo &rndInfo)
{
//	if( !(rndInfo.drawFlags & MICROCODE_SHADOWER) )
//	{
//		unsigned int ModelVUMemBase = (Model.d_Flags >> 24) & 0xff;
//		if( ModelVUMemBase == 48 )
//		{
//			rndInfo.drawFlags &= (u_int) ~MICROCODE_CLIP;
//			rndInfo.drawFlags &= (u_int) ~MICROCODE_CULL;
//			rndInfo.drawFlags &= (u_int) ~MICROCODE_FOG;
//			rndInfo.drawFlags |= MICROCODE_ENV_MAP;
//			rndInfo.drawFlags |= MICROCODE_CULL;
//			rndInfo.drawFlags |= MICROCODE_FOG;
//		}
//	}

	if( (rndInfo.drawFlags & MICROCODE_POINT_LIGHTS)  &&  (rndInfo.drawFlags & (MICROCODE_CULL | MICROCODE_FOG) ) )
		rndInfo.drawFlags |= MICROCODE_CULL | MICROCODE_FOG;
			
	// we dont clip right now...
	if(rndInfo.drawFlags & MICROCODE_CLIP)
	{
		rndInfo.drawFlags &= (u_int) ~MICROCODE_CLIP;
		rndInfo.drawFlags |= MICROCODE_CULL;
	}

	if(rndInfo.drawFlags & MICROCODE_CLIP)
	{
		rndInfo.requirementsFlags = 
			REQ_M1_SPACE_DONT_CARE |
			REQ_M1M2_SPACE_CLIP;
		rndInfo.validMatricies = MATRIX_CLIP_TO_SCREEN;
	}
	else if(rndInfo.drawFlags & MICROCODE_CULL)
	{
		rndInfo.requirementsFlags = 
			REQ_M1_SPACE_DONT_CARE |
			REQ_M1M2_SPACE_CULL;
		rndInfo.validMatricies = MATRIX_CULL_TO_SCREEN;
	}
	else
	{
		rndInfo.requirementsFlags = 
			REQ_M1_SPACE_DONT_CARE |
			REQ_M1M2_SPACE_SCREEN;
	}

	rndInfo.requirementsFlags |= 
		REQ_LIGHT_DIRECTIONAL_0 |
		REQ_LIGHT_DIRECTIONAL_1 |
		REQ_LIGHT_DIRECTIONAL_2 |
		REQ_LIGHT_POINT_0			|
		REQ_LIGHT_POINT_1;

	rndInfo.lightSpaceRequested = LIGHT_SPACE_MODEL;
}

void VUAnimData::transformMatrixBufferLights( int mtxCount, void *pMatrixBuffer, sceVu0FMATRIX LightMtx, sceVu0FMATRIX PointLightMtx )
{
#if 1

	asm(".set noreorder");
	asm("lqc2		vf01,	0x00(a3)");
	asm("lqc2		vf02,	0x10(a3)");
	asm("lqc2		vf03,	0x20(a3)");
	asm("lqc2		vf04,	0x30(a3)");

	asm("lqc2		vf05,	0x00(t0)");
	asm("lqc2		vf06,	0x10(t0)");
	asm("lqc2		vf07,	0x20(t0)");
	asm("lqc2		vf08,	0x30(t0)");

asm("LOOP:");

	asm("lqc2		vf09,	0x00(a2)");
	asm("VMULAx   acc,	vf01,	vf09");
	asm("VMADDAy  acc,	vf02,	vf09");
	asm("VMADDAz  acc,	vf03,	vf09");
	asm("VMADDw   vf10,	vf04,	vf09");
	asm("sqc2		vf10,	0x40(a2)");
	asm("VMULAx   acc,	vf05,	vf09");
	asm("VMADDAy  acc,	vf06,	vf09");
	asm("VMADDAz  acc,	vf07,	vf09");
	asm("VMADDw   vf11,	vf08,	vf09");

	asm("lqc2		vf09,	0x10(a2)");
	asm("VMULAx   acc,	vf01,	vf09");
	asm("VMADDAy  acc,	vf02,	vf09");
	asm("VMADDAz  acc,	vf03,	vf09");
	asm("VMADDw   vf10,	vf04,	vf09");
	asm("sqc2		vf10,	0x50(a2)");
	asm("VMULAx   acc,	vf05,	vf09");
	asm("VMADDAy  acc,	vf06,	vf09");
	asm("VMADDAz  acc,	vf07,	vf09");
	asm("VMADDw   vf12,	vf08,	vf09");

	asm("lqc2		vf09,	0x20(a2)");
	asm("VMULAx   acc,	vf01,	vf09");
	asm("VMADDAy  acc,	vf02,	vf09");
	asm("VMADDAz  acc,	vf03,	vf09");
	asm("VMADDw   vf10,	vf04,	vf09");
	asm("sqc2		vf10,	0x60(a2)");
	asm("VMULAx   acc,	vf05,	vf09");
	asm("VMADDAy  acc,	vf06,	vf09");
	asm("VMADDAz  acc,	vf07,	vf09");
	asm("VMADDw   vf13,	vf08,	vf09");

	asm("lqc2		vf09,	0x30(a2)");
	asm("VMULAx   acc,	vf05,	vf09");
	asm("VMADDAy  acc,	vf06,	vf09");
	asm("VMADDAz  acc,	vf07,	vf09");
	asm("VMADDw   vf14,	vf08,	vf09");

	asm("qmfc2		t0,	vf11");
	asm("qmfc2		t1,	vf12");
	asm("qmfc2		t2,	vf13");
	asm("qmfc2		t3,	vf14");

	asm("pextlw	t4,	t1,	t0");
	asm("pextlw	t6,	t3,	t2");
	asm("pcpyld	t0,	t6,	t4");
	asm("pcpyud	t1,	t4,	t6");

	asm("sq			t0,	0x70(a2)");
	asm("sq			t1,	0x80(a2)");

	asm("addiu	a1,	a1,	-1");
	asm("bne		a1,	zero,	LOOP");
	asm("addiu	a2,	a2,	0x90");
	asm(".set reorder");

#else  // 1|0

typedef struct
{
	sceVu0FMATRIX	BoneMatrix;
	sceVu0FVECTOR	LightData[3];
#ifdef TEST_POINT_LIGHTS
	sceVu0FVECTOR	pointLight[2];
#endif
} LitBoneData;

LitBoneData *		pLitBoneData;

	pLitBoneData = (LitBoneData *) pMatrixBuffer;

	for( unsigned int i = 0; i < (u_int)mtxCount; i++)
	{
		sceVu0FMATRIX		LightTemp;

#if 0
		static PS2Matrix4x4 Mtx1;
		sceVu0FMATRIX ident;
		sceVu0UnitMatrix( ident );
		memcpy( &pLitBoneData->BoneMatrix, &ident, 64 );
		memcpy( &Mtx1, &pLitBoneData->BoneMatrix, sizeof( PS2Matrix4x4 ) );
		float sx  = sqrtf( (Mtx1[0][0] * Mtx1[0][0])+(Mtx1[0][1] * Mtx1[0][1])+(Mtx1[0][2] * Mtx1[0][2]));
		float sy  = sqrtf( (Mtx1[1][0] * Mtx1[1][0])+(Mtx1[1][1] * Mtx1[1][1])+(Mtx1[1][2] * Mtx1[1][2]));
		float sz  = sqrtf( (Mtx1[2][0] * Mtx1[2][0])+(Mtx1[2][1] * Mtx1[2][1])+(Mtx1[2][2] * Mtx1[2][2]));

		if( sx < 0.995f || sx > 1.01f || sy < 0.995f || sy > 1.01f || sz < 0.995f || sz > 1.01f )
		{
			printf(" sx=%f, sy=%f, sz=%f...\n", sx, sy, sz );
			ASSERT(0);
		}
#endif
		// move directional lights into bone space
		sceVu0MulMatrix( LightTemp, LightMtx, pLitBoneData->BoneMatrix);

		// move point lights into bone space
		sceVu0FMATRIX		PointLightTemp;
		sceVu0FMATRIX		inv;

//		my_sceVu0TransposeMatrix( inv, inv );
//		PS2Renderer::Invert( inv, pLitBoneData->BoneMatrix );
		sceVu0MulMatrix( PointLightTemp, PointLightMtx, pLitBoneData->BoneMatrix );
//		sceVu0MulMatrix( PointLightTemp, PointLightMtx, inv );
		my_sceVu0TransposeMatrix( PointLightTemp, PointLightTemp );

		memcpy( pLitBoneData->pointLight, PointLightTemp, 2 * sizeof( sceVu0FVECTOR ) );

		// copy light vectors
		memcpy( pLitBoneData->LightData, LightTemp, 3 * sizeof( sceVu0FVECTOR) );



		pLitBoneData++;
	}
#endif // 1|0
}



#include "VUAnimCodeCheck.h"

void VUAnimData::validateTables( void )
{

#ifdef TEST_ANIM_SIZE
	CodeCheckTransTable AnimTable[] = 
		{
			{	 6, "LoadNextInst" },
			{	 0, "Finish" },
			{	 1, "LoadStreamToCache" },
			{	 2, "LoadCacheToMtx1" },
			{	 3, "LoadCacheToMtx2" },
			{	 4, "LoadStreamToMtx1" },
			{	 5, "LoadStreamToMtx2" },
			{	 8, "Transform1_Start" },
			{	15, "Transform2_Start" },
			{	 9, "Transform1_ContinueNW" },
			{	10, "Transform1_ContinueNW_ADC" },
			{	16, "Transform2_ContinueNW" },
			{	17, "Transform2_ContinueNW_ADC" },
			{	 7, "ProcessInst" },
			{	11, "Transform1_Continue" },
			{	12, "Transform1_Continue_ADC" },
			{	18, "Transform2_Continue" },
			{	19, "Transform2_Continue_ADC" },
			{	13, "Transform1_ContinueNR" },
			{	14, "Transform1_ContinueNR_ADC" },
			{	20, "Transform2_ContinueNR" },
			{	21, "Transform2_ContinueNR_ADC" },
			{	22, "Transform_Finish" },
			{	23 "Transform_Finish_ADC" },
		} ;

	VUCodeCheck	ALit( AnimLit, VUMicro::vuAnimTableLit, VUMicro::vuAnimTableLitEntries );
	VUCodeCheck	APLit( AnimPLit, VUMicro::vuAnimTablePLit, VUMicro::vuAnimTablePLitEntries );
	VUCodeCheck	ALC( AnimLC, VUMicro::vuAnimTableLitCull, VUMicro::vuAnimTableLitCullEntries );
	VUCodeCheck	ALF( AnimLF, VUMicro::vuAnimTableLitFog, VUMicro::vuAnimTableLitFogEntries );
	VUCodeCheck	ALFC( AnimLFC, VUMicro::vuAnimTableLitFogCull, VUMicro::vuAnimTableLitFogCullEntries );
	VUCodeCheck ALFCPLit( AnimLFCPLit, VUMicro::vuAnimTableLitFogCullPLit, VUMicro::vuAnimTableLitFogCullPLitEntries );

	VUCodeCheck ALitMP( AnimLMP, VUMicro::vuAnimTableLitMP, VUMicro::vuAnimTableLitMPEntries );
	VUCodeCheck APLitMP( AnimPLMP, VUMicro::vuAnimTablePLitMP, VUMicro::vuAnimTablePLitMPEntries );
	VUCodeCheck ALCMP( AnimLCMP, VUMicro::vuAnimTableLitCullMP, VUMicro::vuAnimTableLitCullMPEntries );
	VUCodeCheck ALFMP( AnimLFMP, VUMicro::vuAnimTableLitFogMP, VUMicro::vuAnimTableLitFogMPEntries );
	VUCodeCheck ALFCMP( AnimLFCMP, VUMicro::vuAnimTableLitFogCullMP, VUMicro::vuAnimTableLitFogCullMPEntries );
	VUCodeCheck ALFCPLitMP( AnimLFCPLitMP, VUMicro::vuAnimTableLitFogCullPLitMP, VUMicro::vuAnimTableLitFogCullPLitMPEntries );

	VUCodeCheck AEVM( AnimEvMap, VUMicro::vuAnimTableEVMap, VUMicro::vuAnimTableEVMapEntries );
	VUCodeCheck AShwr( AnimShadower, VUMicro::vuAnimTableShadower, VUMicro::vuAnimTableShadowerEntries );

	printf( "Lit\n");
	ALFCMP.validateAgainst( ALit, AnimTable, sizeof( AnimTable) / sizeof( CodeCheckTransTable));

	printf( "PLit\n");
	ALFCMP.validateAgainst( APLit, AnimTable, sizeof( AnimTable) / sizeof( CodeCheckTransTable));

	printf( "LitCull\n");
	ALFCMP.validateAgainst( ALC, AnimTable, sizeof( AnimTable) / sizeof( CodeCheckTransTable));

	printf( "LitFogCullPLit\n");
	ALFCMP.validateAgainst( ALFCPLit, AnimTable, sizeof( AnimTable) / sizeof( CodeCheckTransTable));
	
	printf( "LitFogCullPLitMP\n");
	ALFCMP.validateAgainst( ALFCPLitMP, AnimTable, sizeof( AnimTable) / sizeof( CodeCheckTransTable));

	printf( "LitFog\n");
	ALFCMP.validateAgainst( ALF, AnimTable, sizeof( AnimTable) / sizeof( CodeCheckTransTable));
	
	printf( "LitFogCull\n");
	ALFCMP.validateAgainst( ALFC, AnimTable, sizeof( AnimTable) / sizeof( CodeCheckTransTable));
	
	printf( "Shadower\n");
	ALFCMP.validateAgainst( AShwr, AnimTable, sizeof( AnimTable) / sizeof( CodeCheckTransTable));
	
	printf( "LitMP\n");
	ALFCMP.validateAgainst( ALitMP, AnimTable, sizeof( AnimTable) / sizeof( CodeCheckTransTable));
	
	printf( "PLitMP\n");
	ALFCMP.validateAgainst( APLitMP, AnimTable, sizeof( AnimTable) / sizeof( CodeCheckTransTable));
	
	printf( "Lit Cull MP\n");
	ALFCMP.validateAgainst( ALCMP, AnimTable, sizeof( AnimTable) / sizeof( CodeCheckTransTable));

	printf( "Lit Fog MP\n");
	ALFCMP.validateAgainst( ALFMP, AnimTable, sizeof( AnimTable) / sizeof( CodeCheckTransTable));

	printf( "Lit Fog Cull MP\n");
	ALFCMP.validateAgainst( ALFCMP, AnimTable, sizeof( AnimTable) / sizeof( CodeCheckTransTable));

	printf( "EVMap\n");	
	ALFCMP.validateAgainst( AEVM, AnimTable, sizeof( AnimTable) / sizeof( CodeCheckTransTable));
#endif
}
