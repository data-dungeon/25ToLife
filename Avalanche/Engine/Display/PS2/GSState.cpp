/*************************************/
/*                                   */
/*   GSState.cpp                     */
/*   new engine  04/6/01             */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   gs chip state management        */
/*                                   */
/*************************************/

/* the gs graphics chip on the ps2 has a set of registers that control rendering. this file provides an easy way to
set them. the routine GSState__Set will queue up a register address and the associated value. GSState__Send will dma the
values to the gs via path2 (through vif1) (for synchronization reasons). if you are building up a master display list to 
dma in one big chunk, SendGSState will copy the dma packet into the master display list instead of sending it
immediately */

/******************* includes ****************************/

#include "Display/DisplayPCH.h"





/******************* defines *****************************/

/******************* macros ******************************/

/******************* structures **************************/

/******************* externs *****************************/

/******************* global variables ********************/

ts_GSState GSState = {0};

/******************* local variables *********************/

static u32 u32GSStatePacketToggle;
u32 u32GSStateCount = 0;
ts_ADPair *pADPairs = NULL;
static u32 *pu32GSStatePacket[2] = {u32GSState1Packet, u32GSState2Packet};
static int g_MainThreadId = -1;

/******************* local prototypes ********************/
void GSState__Debug_SetRegs( ts_ADPair *pPairs, unsigned int nPairs);

/******************* functions ***************************/

/* initialize gs state tracking */

void GSState__Init(void)
{
	u_char* addr;

	g_MainThreadId = GetThreadId();

	/* build scratchpad dma packets that handle sending gs state commands to gs through vif1. we double */
	/* buffer these so that we can be filling one while the other is potentially waiting to be sent */

	addr = (u_char*) u32GSState1Packet;
//yuch-- TAGID_END for stand-alone, TAGID_CNT for copyable-to-master-list version
	MasterList::Append(addr, MasterList::createDMATAG(0 /*count*/, TAGID_END));	// count will be 1 for giftag plus gs register count
//yuch-- may want flushe here...
	MasterList::Append(addr, createNOP());
	MasterList::Append(addr, createDIRECT(0));	// count will be 1 for giftag plus gs register count

	/* now a giftag to set the desired number of registers on the gs */

	MasterList::Append(
		addr,
		createGIFTAG(
			0,								// nloop (filled in later)
			1,								// regcount
			0xeeeeeeeeeeeeeeee)		// registers-- A + D
		);

	/* and gs register data follows... */

	/* copy into 2nd packet */

	memcpy(u32GSState2Packet, u32GSState1Packet, 2 * BYTES_PER_QUAD_WORD);

	/* point to first packet */

	u32GSStatePacketToggle = 0;
	u32GSStateCount = 0;
	pADPairs = (ts_ADPair *) &pu32GSStatePacket[u32GSStatePacketToggle][8];	// addr/data pairs follow dma tag and gif tag

	/* initialize gs registers we care about. some special cases: ALPHA_1 start at 0, get set by */
	/* RenderState__Init. FRAME_1 and XYOFFSET get set by gfxSetFrameBuffer */

	/* Set TEST_1 so we meet TRC */
	GSState.TEST_1 = SCE_GS_SET_TEST_1(
		0, 	// ate, alpha test enable, 0 = off, 1 = on
		0x0, 	// atst, alpha test method, 111 = notequal
		0x80, // aref, alpha reference value for alpha test, 0x80 = opaque
		0x0, 	// afail, what to do when alpha test fails, 0 = KEEP (transparent wireframe) 10=ZB_ONLY (opaque wireframe)
		0, 	// date, destination alpha test, 0 = off, 1 = on
		0, 	// datm, destination alpha test mode, 1 bit
		1, 	// zte, depth test, 1 = on (TRC Requirement)
		0x1);	// ztst, depth test method, 0 = never, 1 = always, 2 = gequal, 3 = greater
	GSState__Set( SCE_GS_TEST_1, GSState.TEST_1);

	/* set up z-buffer (frame buffer gets set up at start of each render, no need to do it here) */

	GSState.ZBUF_1 = SCE_GS_SET_ZBUF_1(
		DisplayState.u32ZBufferAddr / 32,					// zbp, z buffer base pointer
		DisplayState.u32ZPixelFormat - SCE_GS_PSMZ32,	// psm, z buffer pixel format
		0);															// zmsk, z value drawing mask, 0 = zbuffer write on, 1 = off
	GSState__Set(SCE_GS_ZBUF_1, GSState.ZBUF_1);

	/* record initial states of ZTST and ZMSK bits for use by RenderState__Request_RS_ZTEST (see notes there) */

	ASSERT((GSState.TEST_1 & GS_TEST_ZTST_M) == (1L << GS_TEST_ZTST_O));
	GSState.u16LastZFUNC = RS_ZFUNC_ALWAYS;
	ASSERT((GSState.ZBUF_1 & GS_ZBUF_ZMSK_M) == 0);
	GSState.u16LastZWRITE = RS_TRUE;

	/* dither matrix for 16-bit frame buffer */

// TJC - always set these, they only have an effect when we are 16 bit, ans
// will soon be able to change between levels and such...
// 	if (DisplayState.u32FramePixelFormat == SCE_GS_PSMCT16S)
// 	{
		/* note that this dither matrix contradicts the gs documentation of the DIMX register, which */
		/* says that each value is a signed 3-bit value. so 7, for example, shouldn't be a valid value */
		/* or at least should be interpreted as -1). but jeff's dither matrix results in MUCH better */
		/* results than the sony matrix...looking at the SCE_GS_SET_DIMX macro, it doesn't do any sign */
		/* checking or masking or sign extension. later-- confirmed, the macro's broken */

		GSState.DIMX = SCE_GS_SET_DIMX(4, 2, 5, 3, 0, 6, 1, 7, 5, 3, 4, 2, 1, 7, 0, 6);	// from jeff g
//		GSState.DIMX = SCE_GS_SET_DIMX(-4, 2, -3, 3, 0, -2, 1, -1, -3, 3, -4, 2, 1, -1, 0, -2);	// from sony docs, looks like doodie
//		GSState.DIMX = SCE_GS_SET_DIMX(0, 8, 2, 10, 12, 4, 14, 6, 3, 11, 1, 9, 15, 7, 13, 5);	// The standard dither matrix from FvD- not useable here
		GSState.POSDIMX = SCE_GS_SET_DIMX(0, 2, 0, 2, 3, 1, 3, 1, 0, 2, 0, 2, 3, 1, 3, 1); // all positive form of dither matrix

		GSState__Set(SCE_GS_DIMX, GSState.DIMX);

		// if we're downsampling don't enable the dither.. the downsample will dither and
		// we don't want to double dither, it looks like crap.  if we're 32bit don't enable
		// cause we just don't need it.
		if ( Display__DownSampling() || DisplayState.u32FramePixelFormat[0] == SCE_GS_PSMCT32 )
			GSState.DTHE = 0;
		else
			GSState.DTHE = 1;

		GSState__Set(SCE_GS_DTHE, GSState.DTHE);
// 	}

	/* some other register initialization */

	GSState.PRMODECONT = 1;	// use PRIM register, not PRMODE
	GSState__Set(SCE_GS_PRMODECONT, GSState.PRMODECONT);
	GSState.COLCLAMP = 1;	// clamp colors 0-255 (for example, dither matrix can push a white pixel beyond 255. this clamps it back)
	GSState__Set(SCE_GS_COLCLAMP, GSState.COLCLAMP);
	GSState.TEXA = SCE_GS_SET_TEXA(
		0,				// ta0-- alpha value to use when a = 0 in rgba16 or in rgb24
		0,				// aem-- if 1, alpha always 0 if r = g = b = 0
		0x80);		// ta1-- alpha value to use when a = 1 in rgba16
	GSState__Set(SCE_GS_TEXA, GSState.TEXA);

	/* and send them all */

//	GSState__Send();
}

/* dma current buffer, switch to other one */

// TJC - it is faster to not do the DMA... little DMAs are expensive...
// this define is just for debugging...
#define DMA_STATE_DATA

void GSState__Send(void)
{
	u_char* addr;
	u_char* src;

	/* maybe nothing to do */

	if (u32GSStateCount == 0)
		return;

	/* fill in counts correctly in pre-build dma packet */
	addr = (u_char*) pu32GSStatePacket[u32GSStatePacketToggle];

	bool useMasterList = MasterList::CurrentlyActiveAtAll() && 
								( MasterList::CurrentlyActiveThreadId() == g_MainThreadId ||
								  MasterList::CurrentlyActive() );

	if(useMasterList)
	{
#ifndef DMA_STATE_DATA
		src = addr + 8 * sizeof(u32);
		addr = MasterList::CurrentDMAPointer();
#else
		src = addr;
#endif
		MasterList::Append(
			addr,
			MasterList::createDMATAG(1 + u32GSStateCount));
	}
	else
	{
		src = addr;
		MasterList::Append(addr, MasterList::createDMATAG(1 + u32GSStateCount, TAGID_END)); // count will be 1 for giftag plus gs register count
	}
	// flush to wait for completion of path1 GIF stuff, before we change settings on it...
	if(MasterList::GetFlushed())
	{
		MasterList::Append(addr, createNOP());
	}
	else
	{
		MasterList::SetFlushed(true);
		MasterList::Append(addr, createFLUSH());
	}
	MasterList::Append(addr, createDIRECT(1 + u32GSStateCount));	// count will be 1 for giftag plus gs register count

	/* now a giftag to set the desired number of registers on the gs */

	MasterList::Append(
		addr,
		createGIFTAG(
			u32GSStateCount,	// nloop
			1,						// nreg-- register count-- 0 means 16
			0xe)					// regs (e=A+D)
		);

	/* send it */

	/* if we are building a masterlist, append to it... */
	/* otherwise, just send the data */
	if(MasterList::CurrentlyActive())
	{
#ifndef DMA_STATE_DATA
		// TJC - yes, this is faster than a DMA... setting up the dma takes 4 bus accesses... this is
		// usually only one or two registers worth (and the reads go through the cache)...
		u_long128 *copysrc = (u_long128*)src;
		u_long128 *dst = (u_long128*)addr;
		for(int i = 0; i < u32GSStateCount; i++)
			*dst++ = *copysrc++;
		MasterList::CurrentDMAPointer() = (u8*)dst;
#else
		COPY_TO_MASTER_LIST(src, u32GSStateCount + 2);
		//DMA_Wait(D1_CHCR);
#endif
	}
	else
	{
		MasterList::Flush();
		DMA_Wait(D1_CHCR);
		DPUT_D1_QWC(0);
		DPUT_D1_TADR((u_int) (0x80000000 | (u_int) src));	// 0x80000000 means buffer is in scratchpad
		DPUT_D1_CHCR(D_CHCR_DIR_M | D_CHCR_STR_M | D_CHCR_SCHAIN_MODE_M | D_CHCR_TTE_M);
	}
	/* swap buffers */

	u32GSStatePacketToggle ^= 1;
	u32GSStateCount = 0;
	pADPairs = (ts_ADPair *) &pu32GSStatePacket[u32GSStatePacketToggle][8];	// addr/data pairs follow dma tag and gif tag
}

/* remove queued state commands */

void GSState__Clear(void)
{
	u32GSStateCount = 0;
}

#ifdef DEBUG
void GSState__Debug_SetRegs( ts_ADPair *pPairs, unsigned int nPairs)
{
	for( unsigned int i = 0; i < nPairs; i++)
	{
		unsigned int addr = (unsigned int) pPairs[i].u64Address;

		switch( addr)
		{
		case SCE_GS_ALPHA_1:
			GSState.ALPHA_1 = pPairs[i].u64Data;
			break;
		case SCE_GS_COLCLAMP:
			GSState.COLCLAMP = pPairs[i].u64Data;
			break;
		case SCE_GS_DIMX:
			GSState.DIMX = pPairs[i].u64Data;
			break;
		case SCE_GS_DTHE:
			GSState.DTHE = pPairs[i].u64Data;
			break;
		case SCE_GS_FRAME_1:
			GSState.FRAME_1 = pPairs[i].u64Data;
			break;
		case SCE_GS_PRMODECONT:
			GSState.PRMODECONT = pPairs[i].u64Data;
			break;
		case SCE_GS_SCISSOR_1:
			GSState.SCISSOR_1 = pPairs[i].u64Data;
			break;
		case SCE_GS_TEST_1:
			GSState.TEST_1 = pPairs[i].u64Data;
			break;
		case SCE_GS_TEXA:
			GSState.TEXA = pPairs[i].u64Data;
			break;
		case SCE_GS_XYOFFSET_1:
			GSState.XYOFFSET_1 = pPairs[i].u64Data;
			break;
		case SCE_GS_ZBUF_1:
			GSState.ZBUF_1 = pPairs[i].u64Data;
			break;
		default:
			break;
		}
	}

}

void GSState__Debug_SetTrackedRegs( void )
{
	GSState__Debug_SetRegs( pADPairs, u32GSStateCount);
}

void GSState__Debug_ValidateTrackedRegs( void)
{
#define dPrint64Diff(name,val1,val2) printf( "%s 0x%08X%08X -- 0x%08X%08X\n", (name), ((val1) >> 32), ((val1) & 0xffffffff), ((val2) >> 32), ((val2) & 0xffffffff))

	for( unsigned int i = 0; i < u32GSStateCount; i++)
	{
		unsigned int addr = (unsigned int) pADPairs[i].u64Address;

		switch( addr)
		{
		case SCE_GS_ALPHA_1:
			if( GSState.ALPHA_1 != pADPairs[i].u64Data)
				dPrint64Diff( "ALPHA_1", GSState.ALPHA_1, pADPairs[i].u64Data);
			break;
		case SCE_GS_COLCLAMP:
			if( GSState.COLCLAMP != pADPairs[i].u64Data)
				dPrint64Diff( "COLCLAMP", GSState.COLCLAMP, pADPairs[i].u64Data);
			break;
		case SCE_GS_DIMX:
			if( GSState.DIMX != pADPairs[i].u64Data)
				dPrint64Diff( "DIMX", GSState.DIMX, pADPairs[i].u64Data);
			break;
		case SCE_GS_DTHE:
			if( GSState.DTHE != pADPairs[i].u64Data)
				dPrint64Diff( "DTHE", GSState.DTHE, pADPairs[i].u64Data);
			break;
		case SCE_GS_FRAME_1:
			if( GSState.FRAME_1 != pADPairs[i].u64Data)
				dPrint64Diff( "FRAME_1", GSState.FRAME_1, pADPairs[i].u64Data);
			break;
		case SCE_GS_PRMODECONT:
			if( GSState.PRMODECONT != pADPairs[i].u64Data)
				dPrint64Diff( "PRMODECONT", GSState.PRMODECONT, pADPairs[i].u64Data);
			break;
		case SCE_GS_SCISSOR_1:
			if( GSState.SCISSOR_1 != pADPairs[i].u64Data)
				dPrint64Diff( "SCISSOR_1", GSState.SCISSOR_1, pADPairs[i].u64Data);
			break;
		case SCE_GS_TEST_1:
			if( GSState.TEST_1 != pADPairs[i].u64Data)
				dPrint64Diff( "TEST_1", GSState.TEST_1, pADPairs[i].u64Data);
			break;
		case SCE_GS_TEXA:
			if( GSState.TEXA != pADPairs[i].u64Data)
				dPrint64Diff( "TEXA", GSState.TEXA, pADPairs[i].u64Data);
			break;
		case SCE_GS_XYOFFSET_1:
			if( GSState.XYOFFSET_1 != pADPairs[i].u64Data)
				dPrint64Diff( "XYOFFSET_1", GSState.XYOFFSET_1, pADPairs[i].u64Data);
			break;
		case SCE_GS_ZBUF_1:
			if( GSState.ZBUF_1 != pADPairs[i].u64Data)
				dPrint64Diff( "ZBUF_1", GSState.ZBUF_1, pADPairs[i].u64Data);
			break;
		default:
			break;
		}
	}

}

#define dGetBits(reg, shift, mask) ((reg >> (shift)) & mask)

#define dBitOnOffText(bit) ( (bit) ? "ON" : "OFF")
#define dPrint64(val) printf( "Value: 0x%08X%08X\n", (val >> 32), (val & 0xffffffff))

void GSState__DebugSpew_Test( u64 Test_x)
{
unsigned long		temp;

char *				sATSTStrings[] = 
{	
	"NEVER   - All Pixels Fail",
	"ALWAYS  - All Pixels Pass",
	"LESS    - Pixels with alpha < AREF Pass",
	"LEQUAL  - Pixels with alpha <= AREF Pass",
	"EQUAL   - Pixels with alpha == AREF Pass",
	"GEQUAL  - Pixels with alpha >= AREF Pass",
	"GREATER - Pixels with alpha > AREF Pass",
	"NEQUAL  - Pixels with alpha != AREF Pass"
} ;

char *				sAFAILStrings[] = 
{
	"KEEP    - Neither Frame or Z Buffer Updated",
	"FB_ONLY - Only Frame Buffer is Updated",
	"ZB_ONLY - Only Z Buffer is Updated",
	"RGB_ONLY- Only Frame Buffer RGB is Updated"
} ;

char *				sZTSTStrings[] = 
{	
	"NEVER   - All Pixels Fail",
	"ALWAYS  - All Pixels Pass",
	"GEQUAL  - Pixels with Z >= Z Buffer Z Val Pass",
	"GREATER - Pixels with Z >  Z Buffer Z Val Pass",
} ;

	dPrint64( Test_x);

	temp = dGetBits( Test_x, 0, 0x01);
	printf( "ATE   [ %01x]: Alpha Test %s\n", temp, dBitOnOffText( temp) );

	temp = dGetBits( Test_x, 1, 0x07);
//	printf( "Alpha Test Method\n");
	printf( "ATST  [ %01x]: %s\n", temp, sATSTStrings[temp] );

	temp = dGetBits( Test_x, 4, 0xff);
//	printf( "Alpha Compare Value\n");
	printf( "AREF  [%02x]: Alpha Compare 0x%02x\n", temp, temp);

	temp = dGetBits( Test_x, 12, 0x03);
//	printf( "Processing for Failed Alpha Test\n");
	printf( "AFAIL [ %01x]: %s\n", temp, sAFAILStrings[temp] );

	temp = dGetBits( Test_x, 14, 0x01);
//	printf( "Destination Alpha Test\n");
	printf( "DATE  [ %01x]: Destination Alpha Test %s\n", temp, dBitOnOffText( temp) );

	temp = dGetBits( Test_x, 15, 0x01);
//	printf( "Destination Alpha Test Mode\n");
	printf( "DATM  [ %01x]: Pixels with Destination Alpha == %d Pass\n", temp, temp);

	temp = dGetBits( Test_x, 16, 0x01);
//	printf( "Depth Test\n");
	printf( "ZTE   [ %01x]: Depth Test %s\n", temp, dBitOnOffText( temp) );

	temp = dGetBits( Test_x, 17, 0x03);
//	printf( "Depth Test Method\n");
	printf( "ZTST  [ %01x]: %s\n", temp, sZTSTStrings[temp]);

}

void GSState__DebugSpew_Frame( u64 Frame_x)
{
unsigned int		temp;

	dPrint64( Frame_x);

	temp = dGetBits( Frame_x, 0, 0x1ff);
	printf( "FBP  [%04x]: Frame Buffer Ptr 0x%04x Word Addr: 0x%08x\n", temp, temp, temp * 2048);

	temp = dGetBits( Frame_x, 16, 0x03f);
	printf( "FBW  [%04x]: Width %d Blocks, %d Pixels\n", temp, temp, temp * 64);

	temp = dGetBits( Frame_x, 24, 0x3f);
	printf( "PSM  [%04x]: ", temp);
	switch( temp)
	{
	case 0x00:
		printf( "PSMCT32\n");
		break;
	case 0x01:
		printf( "PSMCT24\n");
		break;
	case 0x02:
		printf( "PSMCT16\n");
		break;
	case 0x0a:
		printf( "PSMCT16S\n");
		break;
	case 0x30:
		printf( "PSMZ32\n");
		break;
	case 0x31:
		printf( "PSMZ24\n");
		break;
	case 0x32:
		printf( "PSMZ16\n");
		break;
	case 0x3a:
		printf( "PSMZ16S\n");
		break;
	default:
		printf( "reserved\n");
		break;
	}
}


void GSState__DebugSpew_ZBuf( u64 ZBuf_x)
{
unsigned int		temp;

	dPrint64( ZBuf_x);

	temp = dGetBits( ZBuf_x, 0, 0x1ff);
	printf( "ZBP  [%04x]: Z Buffer Ptr %04x Word Addr: %08x\n", temp, temp, temp * 2048);

	temp = dGetBits( ZBuf_x, 24, 0xf);
	printf( "PSM  [%04x]: ", temp);
	switch( temp)
	{
	case 0x00:
		printf( "PSMZ32\n");
		break;
	case 0x01:
		printf( "PSMZ24\n");
		break;
	case 0x02:
		printf( "PSMZ16\n");
		break;
	case 0x0a:
		printf( "PSMZ16S\n");
		break;
	default:
		printf( "reserved\n");
		break;
	}

	temp = dGetBits( ZBuf_x, 32, 1);
	printf( "ZMSK [   %01x]: %s\n", temp, temp ? "Z Buffer is NOT Updated" : "Z Buffer is Updated");

}

void GSState__DebugSpew_Alpha( u64 Alpha_x)
{
unsigned int		temp;

char *				sCsStrings[] = 
{
	"Cs  - Source RGB",
	"Cd  - Destination (FB) RGB",
	"0   -",
	"reserved"
} ;

char *				sAStrings[] = 
{
	"As  - Source Alpha",
	"Ad  - Destination Alpha",
	"FIX - FIX Alpha",
	"reserved"
} ;

	dPrint64( Alpha_x);

	printf( "Alpha Blend = (((A - B) * C) >> 7) + D\n");
	temp = dGetBits( Alpha_x, 0, 0x03);
	printf( "A    [%02x]: %s\n", temp, sCsStrings[temp]);

	temp = dGetBits( Alpha_x, 2, 0x03);
	printf( "B    [%02x]: %s\n", temp, sCsStrings[temp]);

	temp = dGetBits( Alpha_x, 4, 0x03);
	printf( "C    [%02x]: %s\n", temp, sAStrings[temp]);

	temp = dGetBits( Alpha_x, 6, 0x03);
	printf( "D    [%02x]: %s\n", temp, sCsStrings[temp]);

	temp = dGetBits( Alpha_x, 32, 0xff);
	printf( "FIX  [%02x]: FIX Alpha = 0x%02x\n", temp, temp);

}

void GSState__DebugSpew_TexA( u64 TexA)
{
unsigned int		temp;

	dPrint64( TexA);

	temp = dGetBits( TexA, 0, 0xff);
	printf( "TA0   [%02x]: As = %02x when (RGBA16 && A = 0) or (RGBA24)\n", temp, temp);

	temp = dGetBits( TexA, 15, 0x01);
	printf( "AEM   [ %01x]: %s when R=G=B=0\n", temp, temp ? "Transparent" : "Normal"); 

	temp = dGetBits( TexA, 32, 0xff);
	printf( "TA1   [%02x]: As = 0x%02x when (RGBA16 && A = 1)\n", temp, temp);
}

#define dFixed4(val)	(((float) (val) / 16) )

void GSState__DebugSpew_XYOffset( u64 XYOffset_x)
{
unsigned int		temp;

	dPrint64( XYOffset_x);

	temp = dGetBits( XYOffset_x, 0, 0xffff);
	printf( "OFX   [%04x]: Offset X = %9.4f\n", temp, dFixed4( temp));

	temp = dGetBits( XYOffset_x, 32, 0xffff);
	printf( "OFY   [%04x]: Offset Y = %9.4f\n", temp, dFixed4( temp));

}

void GSState__DebugSpew_Scissor( u64 Scissor_x)
{
unsigned int		temp;

	dPrint64( Scissor_x);

	temp = dGetBits( Scissor_x, 0, 0x7ff);
	printf( "SCAX0 [%03x]: Upper Left X %d\n", temp, temp);

	temp = dGetBits( Scissor_x, 32, 0x7ff);
	printf( "SCAY0 [%03x]: Upper Left Y %d\n", temp, temp);

	temp = dGetBits( Scissor_x, 16, 0x7ff);
	printf( "SCAX1 [%03x]: Lower Left X %d\n", temp, temp);

	temp = dGetBits( Scissor_x, 48, 0x7ff);
	printf( "SCAY1 [%03x]: Lower Left Y %d\n", temp, temp);

}

void GSState__DebugSpew_PRModeCont( u64 PRModeCont)
{
	printf( "AC    [%01x]: %s\n", PRModeCont & 0x01, 
		(PRModeCont & 0x01) ? "Enabled (PRIM is Used)" : "Disabled (PRMODE is Used)");
}

void GSState__DebugSpew( void )
{

	printf( "TEST_1\n");
	GSState__DebugSpew_Test( GSState.TEST_1);

	printf( "FRAME_1\n");
	GSState__DebugSpew_Frame( GSState.FRAME_1);

	printf( "ZBUF_1\n");
	GSState__DebugSpew_ZBuf( GSState.ZBUF_1);

	printf( "ALPHA_1\n");
	GSState__DebugSpew_Alpha( GSState.ALPHA_1);

	printf( "TEXA\n");
	GSState__DebugSpew_TexA( GSState.TEXA);

	printf( "XYOFFSET_1\n");
	GSState__DebugSpew_XYOffset( GSState.XYOFFSET_1);

	printf( "SCISSOR_1\n");
	GSState__DebugSpew_Scissor( GSState.SCISSOR_1);

	printf( "PRMODECONT\n");
	GSState__DebugSpew_PRModeCont( GSState.PRMODECONT);

}
#endif // DEBUG

