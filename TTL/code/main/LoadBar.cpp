//============================================================================
//=
//= LoadBar.h - handle loading bar updates
//=
//============================================================================

#include "TTLPCH.h"
#include "LoadBar.h"

#if defined( PS2 )
#include "Display/PS2/Renderer/Common/VIFInline.h"
#include "Display/PS2/Renderer/Common/VIFCreator.h"
#include "Display/PS2/Renderer/Code/MasterList.h"
#endif

//============================================================================

LoadBar g_LoadBar;

//============================================================================

void LoadBar::Start()
{
	d_x = 0;
	d_y = 0;
	d_color = DrawUtility::BLUE;

	DrawColoredBlock( d_x, d_y, d_color );
//	NewLine();
}

//============================================================================

void LoadBar::Update()
{
	DrawColoredBlock( d_x, d_y, d_color );
//	NewLine();
}

//============================================================================

void LoadBar::NewLine()
{
	d_x = 0;
	d_y++;
}

//============================================================================

void LoadBar::DrawColoredBlock( int x, int y, DrawUtility::COLOR color )
{
	// constrain
	if ( y < 1  ) y = 1;
	if ( y > 25 ) y = 25;
	if ( x < 0  ) x = 0;
	if ( x > 15 ) x = 15;

#if defined( PS2 )

	// color
	#define MAKE_RGB(r,g,b) ( ((r)>>3) | (((g)>>2)<<5) | (((b)>>3)<<11) )
	u16 pixelColor = 0;
	switch ( color )
	{
		case DrawUtility::RED:			pixelColor = MAKE_RGB( 255, 0, 0 );			break;
		case DrawUtility::GREEN:		pixelColor = MAKE_RGB( 0, 255, 0 );			break;
		case DrawUtility::BLUE:			pixelColor = MAKE_RGB( 0, 0, 255 );			break;
		case DrawUtility::CYAN:			pixelColor = MAKE_RGB( 0, 255, 255 );		break;
		case DrawUtility::MAGENTA:		pixelColor = MAKE_RGB( 255, 0, 255 );		break;
		case DrawUtility::YELLOW:		pixelColor = MAKE_RGB( 255, 255, 0 );		break;
		default:								pixelColor = MAKE_RGB( 255, 255, 255 );	break;
	}

	// create an image
	#define COLORED_BLOCK_WIDTH	8
	#define COLORED_BLOCK_HEIGHT	8
	u16 image[ COLORED_BLOCK_WIDTH * COLORED_BLOCK_HEIGHT ];
	for ( int i = 0 ; i < COLORED_BLOCK_WIDTH * COLORED_BLOCK_HEIGHT ; i++ )
	{
		if ((i <= COLORED_BLOCK_WIDTH) ||											// top row
			(i >= COLORED_BLOCK_WIDTH * (COLORED_BLOCK_HEIGHT - 1)) ||		// bottom row
			((i % COLORED_BLOCK_WIDTH) == 0) ||										// left edge
			((i % COLORED_BLOCK_WIDTH) == COLORED_BLOCK_WIDTH - 1))			// right edge
			image[ i ] = 0;				// black border
		else
			image[ i ] = pixelColor;	// interior
	}

	FlushCache(WRITEBACK_DCACHE);

	// this calculation allows for 16 dots wide by 26 dots high. each row of 4 dots is in the same 0..256 range,
	// and then each column of 4 dots is in the same 0..16 range. the dot order within a column is 0, 2, 1, 3
	int block = 0;
	if ( DisplayState.u32DisplayWidth == 512 )
	{
		block = ( ( y & ~3 ) >> 2 ) * 256;
		block += ( x * 16 );
		switch ( y & 3 )
		{
			case 0:		block += 0;	 break;
			case 1:		block += 8;  break;
			case 2:		block += 4;  break;
			case 3:		block += 12; break;
		}
	}
	else
	{
		// cheezy.  just skip by pages, 8192/page, 256/block... so 8192/256 is a block address to the next page
		block = ( 8192 / 256 ) * ( ( y - 1 ) * 16 + x + 10 ) + 16;
	}

	// draw to both frame buffers-- one is frozen on screen, but which? this is hardwired to the frame buffer
	// configuration for the juju title screen
	DMAImageToVRAM( DisplayState.u32FrameBufferAddr[0] + block, COLORED_BLOCK_WIDTH, COLORED_BLOCK_HEIGHT, image, SCE_GS_PSMCT16S );
	DMAImageToVRAM( DisplayState.u32FrameBufferAddr[1] + block, COLORED_BLOCK_WIDTH, COLORED_BLOCK_HEIGHT, image, SCE_GS_PSMCT16S );

#endif //PS2

	// update position
	d_x = x + 1;
	d_y = y;
	if ( d_x == 16 )
	{
		NewLine();
	}
	d_color = color;
}

//============================================================================

#ifdef PS2
void LoadBar::DMAImageToVRAM(
	u32 u32VRAMAddr,		// vram addr in blocks
	u32 u32Width,			// width in pixels
	u32 u32Height,			// height in pixels
	void *pImage,			// image buffer in system ram
	u32 u32Format)			// pixel format
{
//yuch-- there are known texture sizes that we cannot handle right now. this has to do with the vram layout on the ps2.
// for example, tall skinny 8-bit textures are a problem. to be more precise, textures that are less than one vram page
// wide but more than one vram page tall are a problem. most of the time, these are handled by ImportExport because it
// flips these textures along the diagonal so they are wider than high, and reverses the uv coordinates of the geometry
// that uses them. however, the ImportExport check stops at height or width = 128, so it doesn't handle things like
// 64 wide by 256 high-- the main problem is that these textures create lots and lots of vram holes. with some effort it
// would be possible to handle these textures, but is it a good idea to let so many holes be created? anyway, let's look
// for problems in case they sneak through-- they won't display correctly

// TJC - ImportExport is now capable of handling these holes, and it always flips textures to be wider than they
// are tall because of the speed implications of crossing too many page boundries.
#ifdef _DEBUG
	u32 u32PageWidth;
	u32 u32PageHeight;
	switch(u32Format)
	{
	case SCE_GS_PSMT4:
		u32PageWidth = 128;
		u32PageHeight = 128;
		break;
	case SCE_GS_PSMT8:
		u32PageWidth = 128;
		u32PageHeight = 64;
		break;
	case SCE_GS_PSMCT16:
	case SCE_GS_PSMCT16S:
		u32PageWidth = 64;
		u32PageHeight = 64;
		break;
	case SCE_GS_PSMCT32:
	case SCE_GS_PSMCT24:
		u32PageWidth = 64;
		u32PageHeight = 32;
		break;
	default:
		ASSERT(FALSE);
		break;
	}
//	if ((u32Width < u32PageWidth) && (u32Height > u32PageHeight))
//		ASSERT(FALSE);
#endif //_DEBUG

	/* set up gs registers for transfer */

	GSState__Set(SCE_GS_TEXFLUSH, 0);	// write any value to flush texture page (since we're sending new data to vram)
	GSState__Set(
		SCE_GS_BITBLTBUF,
		SCE_GS_SET_BITBLTBUF(
			0,								// sbp-- source buffer base pointer, vram word addr / 64 (vram block addr)
			0,								// sbw-- source buffer width, pixels / 64
			0,								// spsm-- source pixel format
			u32VRAMAddr,				// dbp-- dest buffer base pointer, vram word addr / 64 (vram block addr)
//yuch-- it's possible that this should be (u32Width + 63) / 64-- i.e. if it's 0 make it 1
			u32Width / 64,				// dbw-- dest buffer width, pixels / 64
			u32Format)					// dpsm-- dest pixel format
			);
	GSState__Set(
		SCE_GS_TRXPOS,
		SCE_GS_SET_TRXPOS(
			0,	// ssax-- source upper left xpos
			0,	// ssay-- source upper left ypos
			0,	// dsax-- dest upper left xpos
			0,	// dsay-- dest upper left ypos
			0)	// dir-- pixel transmission order (active in local -> local transmission only)
			);
	GSState__Set(
		SCE_GS_TRXREG,
		SCE_GS_SET_TRXREG(
			u32Width,	// rrw-- width of transmission area in pixels
			u32Height)	// rrh-- height of transmission area in pixels
			);
	GSState__Set(SCE_GS_TRXDIR, SCE_GS_SET_TRXDIR(0));	// 0 = host -> local, 1 = local -> host, 2 = local -> local
	GSState__Send();

	/* calculate how many quad words to send */

	u32 nQWC = u32Width * u32Height;
	switch(u32Format)
	{
	case SCE_GS_PSMCT32:
	case SCE_GS_PSMCT24:
		nQWC = (nQWC * 4) / BYTES_PER_QUAD_WORD;	// 4 bytes per pixel
		break;
	case SCE_GS_PSMCT16:
	case SCE_GS_PSMCT16S:
		nQWC = (nQWC * 2) / BYTES_PER_QUAD_WORD;	// 2 bytes per pixel
		break;
	case SCE_GS_PSMT8:
		nQWC = (nQWC * 1) / BYTES_PER_QUAD_WORD;	// 1 bytes per pixel
		break;
	case SCE_GS_PSMT4:
		nQWC = (nQWC / 2) / BYTES_PER_QUAD_WORD;	// 2 pixels per byte
		break;
	default:
		ASSERT(0);
		break;
	}
	if (nQWC == 0)
		nQWC = 1;	// must be at least one quad word for dma (very small textures may be less than 16 bytes)

	// maximum IMAGE NLOOP is:
	// ie.. if this fires, your image is TOO BIG muchacho.
	if(nQWC >= 32768)
	{
		// image is way too big...
		// this technically could be done in chunks to get it up there...
		// print warning...
		#ifndef CONSUMER_BUILD
		dbgPrint("*** Image is too large to go up to texture memory in a single transfer.. skipping...\n");
		#endif

		// skip it...
		return;
	}

	/* now fill in texture dma packet-- goes on path2, through vif1 */

//	CHECK_LAST_FROM_SPR_DMA(u8TexturePath2Packet);

	if(!MasterList::CurrentlyActive())
	{
		MasterList::Flush();
		DMA_Wait(D1_CHCR);
	}

	ASSERT(IS_DMA_ALIGNED(pImage));
	u_char* addr = (u_char*) u8TexturePath2Packet + BYTES_PER_QUAD_WORD;	// to 2nd quad word in packet
	MasterList::Append8(
		addr, 
		SCE_GIF_SET_TAG(nQWC, 1, 0, 0, SCE_GIF_IMAGE, 0));

	addr += 8;	// to 3rd quad word-- APPEND8 skips first 8 bytes

	if(MasterList::CurrentlyActive())
	{
		MasterList::Append(
			addr, 
			MasterList::createDMATAG(nQWC, TAGID_REF, pImage));
		addr += 4;	// skip VIF_NOP already there
		MasterList::Append(addr, VIFInline::createDIRECT(nQWC));
		COPY_TO_MASTER_LIST(u8TexturePath2Packet, TEXTURE_PATH2_PACKET_QWC);
	}
	else
	{
		MasterList::Append(
			addr, 
			MasterList::createDMATAG(nQWC, TAGID_REFE, pImage));
		addr += 4;	// skip VIF_NOP already there
		MasterList::Append(addr, VIFInline::createDIRECT(nQWC));

		DPUT_D1_TADR((u_int) (0x80000000 | (u_int) u8TexturePath2Packet));	// 0x80000000 means buffer is in scratchpad
		DPUT_D1_CHCR(D_CHCR_DIR_M | D_CHCR_STR_M | D_CHCR_SCHAIN_MODE_M | D_CHCR_TTE_M);
		DMA_Wait(D1_CHCR);	// this may take a while...i get a lock up if i don't wait for it, something must be writing over the dma source address register without doing a DMA_Wait first
	}
}
#endif //PS2

//============================================================================
