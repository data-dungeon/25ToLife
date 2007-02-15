#include "renderassert.h"

#include "VUStaticData3.h"

#include "VUManagedCode.h"
#include "VUManagedFunction.h"
#include "VUCodeSetupHelper.h"
#include "MasterCodeFactory.h"
#include "VUCode.h"
#include "MasterList.h"
#include "PS2Renderer.h"
#include "DDMAObject.h"

#include "VIFInline.h"
using namespace			VIFInline;

VUManagedCode				&StaticCompressed = MasterCodeFactory::createManagedCode( &dbStaticCompress_CodeBegin, &dbStaticCompress_CodeEnd);	
VUManagedFunction 		&StaticFunc = MasterCodeFactory::createManagedFunction( &dbStaticCompress_CodeBegin);

VUManagedCode				&StaticCompressedCull = MasterCodeFactory::createManagedCode( &dbStaticCompressCull_CodeBegin, &dbStaticCompressCull_CodeEnd);	
VUManagedFunction 		&StaticCullFunc = MasterCodeFactory::createManagedFunction( &dbStaticCompressCull_CodeBegin);

VUManagedCode				&StaticCompressedClip = MasterCodeFactory::createManagedCode( &dbStaticCompressClip_CodeBegin, &dbStaticCompressClip_CodeEnd);	
VUManagedFunction 		&StaticClipFunc = MasterCodeFactory::createManagedFunction( &dbStaticCompressClip_CodeBegin);

VUManagedCode				&GenericClip = MasterCodeFactory::createManagedCode( &GenericClip_CodeBegin, &GenericClip_CodeEnd);

VUManagedFunction			&gcSetup = MasterCodeFactory::createManagedFunction( &GC_ClipBufferSetup);
VUManagedFunction			&gcClip = MasterCodeFactory::createManagedFunction( &GC_ClipBuffer);

// SPR usage...
#define PACKET_SIZE	32
static SCRATCHPAD(u_long128, u8Packet[PACKET_SIZE]);

VUStaticData3::VUStaticData3( void ) :
	VUDataClass( 0x00),
	d_GIFTag( 0, 1, 0, 0, 0, 3, 0x00000512)
{

	d_VUMemOffset = (1024 - d_VUMemBase) / 2;
	
}

void VUStaticData3::init(VUCodeSetupHelper& helper)
{
	// add to the helper
	helper.addCode( StaticCompressed );
	helper.addCode( StaticCompressedCull );
	helper.addCode( StaticCompressedClip );
	helper.addCode( GenericClip );
}

void VUStaticData3::prepDMA( DDMAObject &Model, const PS2Matrix4x4& Mtx1, const PS2Matrix4x4& Mtx2, const VURenderInfo &rndInfo)
{
unsigned int 			vuAddr, vuCallAddr, vuGCSetupAddr, vuGCClipAddr;

u_char *					spr = (u_char *) &u8Packet[0];
u_char *					mtxptr;
u_int 					qwc, unpack;

bool 						baseOffsetChanged = !MasterList::IsBaseOffsetSame( d_VUMemBase, d_VUMemOffset );

	RENDER_ASSERT( p_CodeManager);

	// start inserting the codes for uploading the microcode
	// insert codes to download ucode if nessisary...
	if( rndInfo.drawFlags & MICROCODE_CLIP)
	{
		p_CodeManager->loadManagedCode( StaticClipFunc.code( ), vuAddr, true );
		vuCallAddr = vuAddr + StaticClipFunc.offset( );
		
		p_CodeManager->loadManagedCode( GenericClip, vuAddr, true );
		vuGCSetupAddr = vuAddr + gcSetup.offset( );
		vuGCClipAddr = vuAddr + gcClip.offset( );
	}
	else if( rndInfo.drawFlags & MICROCODE_CULL)
	{
		p_CodeManager->loadManagedCode( StaticCullFunc.code( ), vuAddr, true );
		vuCallAddr = vuAddr + StaticCullFunc.offset( );
		
		vuGCSetupAddr = 0;
		vuGCClipAddr = 0;
	}
	else
	{
		p_CodeManager->loadManagedCode( StaticFunc.code( ), vuAddr, true );
		vuCallAddr = vuAddr + StaticFunc.offset( );
	}

	if(MasterList::GetFrameNumber() == 0) return;

	// now, start dma'ing the first matrix into spr...
	// reserve space for extra codes
	if(baseOffsetChanged)
	{
		mtxptr = spr + 32;
		qwc = 11;
	}
	else
	{
		mtxptr = spr + 16;
		qwc = 10;
	}
	unpack = 9;
	

	if( (rndInfo.drawFlags & MICROCODE_CLIP)  ||  (rndInfo.drawFlags & MICROCODE_CULL) )
	{
		unpack += 2;
		qwc += 2;
	}
		
	// make sure last copy finished before now..
	CHECK_LAST_FROM_SPR_DMA(spr);

	// start dma 1 - copy matrix 2
	APPEND_DMA(mtxptr, Mtx2, sizeof(PS2Matrix4x4) / sizeof(u_long128));

	// build dma tag
	APPEND8(spr, MAKE_DMATAG(qwc, DTAG_PCE_NOTHING, TAGID_CALL, 0, Model.GetDMAData()));
	APPEND4(spr, createNOP( ));

	// build base/offset/flush stuff..
	if(baseOffsetChanged)
	{
		if(!MasterList::GetFlushed( ))
		{
			APPEND4(spr, createFLUSH( ));
			MasterList::SetFlushed(true);
		}
		else
		{
			APPEND4(spr, createNOP());
		}
		APPEND4(spr, createBASE(d_VUMemBase));
		APPEND4(spr, createOFFSET(d_VUMemOffset));
		APPEND4(spr, createNOP());

		MasterList::SetBase( d_VUMemBase );
		MasterList::SetOffset( d_VUMemOffset );
	}

	// stick unpack on...
	APPEND4(spr, createUNPACK( 0, V4_32, unpack, 1 ));
	RENDER_ASSERT(((u_int) spr % 16) == 0); // should be aligned now for matrix data

	// hopefully, dma 1 has had some time to get closer to completion...
	// start dma 2 - copy matrix 1
	APPEND_DMA(mtxptr, Mtx1, sizeof(PS2Matrix4x4) / sizeof(u_long128));

	// setup GIFTag
	if( rndInfo.drawFlags & MICROCODE_CLIP)
		Model.mPRIMReg.setPRIM( PRIM_TRIANGLE_FAN);
	else
		Model.mPRIMReg.setPRIM( PRIM_TRIANGLE_STRIP); // TJC - whats this here for?
		
	d_GIFTag.setPRIM( Model.mPRIMReg | (rndInfo.drawFlags & GS_PRIM_MASK) );

	if( (rndInfo.drawFlags & MICROCODE_CLIP)  ||  (rndInfo.drawFlags & MICROCODE_CULL) )
	{
		const PS2Matrix4x4 & viewMtx = PS2Renderer::GetClipToScreen( );

		APPEND4f(mtxptr, viewMtx[3][0] );
		APPEND4f(mtxptr, viewMtx[3][1] );
		APPEND4(mtxptr, vuGCSetupAddr );
		APPEND4(mtxptr, vuGCClipAddr );
		
		APPEND4f(mtxptr, viewMtx[0][0] );
		APPEND4f(mtxptr, viewMtx[1][1] );
		APPEND4f(mtxptr, viewMtx[2][2] );
		APPEND4f(mtxptr, viewMtx[3][2] );
	}
		
		
	memcpy( mtxptr, &d_GIFTag, sizeof( GIFTag) );
	//*(u_long128*)mtxptr = *(u_long128*)&d_GIFTag; // why doesnt this work instead of the memcpy?...
	//((GIFTag*)mtxptr)->setPRIM( Model.d_PRIMReg | (flags & GS_PRIM_MASK));
	mtxptr += 16; // move to next qw..

	// add last qw...
	APPEND4(mtxptr, createMSCAL( (vuCallAddr << 3) ));
	APPEND4(mtxptr, createMSCNT( ));
//	APPEND4(mtxptr, createNOP( ));
	APPEND4(mtxptr, createNOP( ));
	APPEND4(mtxptr, createNOP( ));

	// hopefully, dma 2 has had some time to get closer to completion...
	// we now need its data to be in the spr... so...
	DMA_Wait(D9_CHCR);

	// now move it to the display list...
	DMA_TO_MASTER_LIST( u8Packet, qwc + 1);

	// mark that we arent flushed...
	MasterList::SetFlushed( false);
}




