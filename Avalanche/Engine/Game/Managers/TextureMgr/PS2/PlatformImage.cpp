
/*************************************/
/*                                   */
/*   PlatformImage.cpp               */
/*   Tyler Colbert                   */
/*   PS2 specific code for an image  */
/*                                   */
/*************************************/


/******************* includes ****************************/

#include "Display/DisplayPCH.h"

#include "Display/PS2/GSMemManager.h"
#include "Display/PS2/Renderer/Code/PS2Renderer.h"
#include "Game/Managers/TextureMgr/PlatformImage.h"
#include "Display/PS2/GSTricks.h"

#ifdef _DEBUG
#include "Layers/ttyColors.h"
#endif

#define FIXED_POINT_8_4(fValue) (((s16) (fValue * 16.0f)) & 0xfff)	// 8.4 signed fixed point

// we generate into a static datablock because we know that our
// caller always copies, never references.  This way we dont need
// to regenerate everything every time.
static u128 dataBlock[6];
static u64* clamp_1;
static u64* tex0_1;
static u64* tex1_1;
static u64* miptbp1_1;
static u64* miptbp2_1;
static u64* alpha_1;

static u8 gMinFilter[] =
	{
		0, // TS_FILTER_POINTSAMPLE
		5, // TS_FILTER_BILINEAR
		2, // TS_FILTER_NEARSET_MIPMAP_NEAREST
		3, // TS_FILTER_NEAREST_MIPMAP_LINEAR
		4, // TS_FILTER_LINEAR_MIPMAP_NEAREST
		5, // TS_FILTER_LINEAR_MIPMAP_LINEAR
	};

static u64 gBlendToAlpha[] =
	{
		// definition                     // Renderstate define        Note  Equation
		SCE_GS_SET_ALPHA_1(0,1,0,1,0x80), // RS_BLENDFUNC_UNDEFINED,    1    src * srcA + dst * (1-srcA)
		SCE_GS_SET_ALPHA_1(0,1,0,1,0x80), // RS_BLENDFUNC_DEFAULT,           src * srcA + dst * (1-srcA)
		SCE_GS_SET_ALPHA_1(1,2,0,2,0x80), // RS_BLENDFUNC_DIFFUSE,      2    src * dst
		SCE_GS_SET_ALPHA_1(0,1,0,1,0x80), // RS_BLENDFUNC_SATURATE,     2    src * 1 + dst * src
		SCE_GS_SET_ALPHA_1(0,2,2,1,0x80), // RS_BLENDFUNC_ADD,               dst + src
		SCE_GS_SET_ALPHA_1(0,1,0,1,0x80), // RS_BLENDFUNC_DOUBLE,       2    2 * src * dst
		SCE_GS_SET_ALPHA_1(0,2,2,2,0x80), // RS_BLENDFUNC_NONE,              src
		SCE_GS_SET_ALPHA_1(0,1,0,1,0x00), // RS_BLENDFUNC_FIXEDALPHA,   3    src * fixA + dst * (1-fixA)
		SCE_GS_SET_ALPHA_1(0,2,0,1,0x80), // RS_BLENDFUNC_ADDITIVE,          src * srcA + dst
		SCE_GS_SET_ALPHA_1(1,0,2,2,0x80), // RS_BLENDFUNC_SUBTRACTIVE,       dst - src
		SCE_GS_SET_ALPHA_1(0,1,2,2,0x80), // RS_BLENDFUNC_NEGATIVE,     4    src - dst

		// Notes
		// 1 - Same as DEFAULT.  Exists to support ImportExport
		// 2 - Unsupported on PS2.  Requires src * dst RGB blend which takes 3 passes on ps2.
		// 3 - Requires setting of FIXA value before it will work correctly
		// 4 - Requires the use of a white texture to get destination inversion.
	};

#define DEFAULT_TS_METHOD TS_METHOD_MODULATE

static u8 gMethodToFunction[] =
	{
		// def  // PS2 name          Notes  TextureState define      Equation
		1,   // TCC=RGBA,MODULATE   1    TS_METHOD_PASSTHRU,      C=Cf A=Af
		1,   //                     2    TS_METHOD_DECAL,         C=CtAt+Cf(1-At) A=Af
		3,   // TCC=RGBA,DECAL           TS_METHOD_REPLACE,       C=Ct A=At
		1,   // TCC=RGBA,MODULATE        TS_METHOD_MODULATE,      C=CtCf A=AtAf
		1,   //                     3    TS_METHOD_INTERPOLATE,   C=CtAc+Cf(1-Ac)
		1,   //                     3    TS_METHOD_CONSTANTALPHA, C=CtCf A=AcAt
		1,   //                     4    TS_METHOD_DOTPRODUCT,    C=Xr*Zr+Xg*Zg+Xb*Zb X=normal Z=lightdir

		// Notes
		//   1   Requires white texture
		//   2   Unsupported on PS2.  Uses texture alpha in texture blend.
		//   3   Unsupported on PS2.  Uses constant alpha in texture blend.
		//   4   Unsupported on PS2.  Uses lots of things the PS2 doesnt have.
	};

int PlatformImage::sRegionStart[NUM_GS_REGIONS];
int PlatformImage::sRegionEnd[NUM_GS_REGIONS];
int PlatformImage::sCurrentRegion;
int PlatformImage::sCurrentTexMemPtr;
int PlatformImage::sRegionSize;
DoublyLinkedList<PlatformImage> PlatformImage::sImagesInRegion[PlatformImage::NUM_GS_REGIONS];
DynamicArray<PlatformImage::QueuedTransfer> PlatformImage::sQueuedTransfers( 16, 0 );
ts_DisplayState PlatformImage::sDisplayStateCache;


/// @{
/// Statistics
static u32 contextChangeCount;
u32 PlatformImage::GetContextChangeCount(void)
{
	return contextChangeCount;
}
void PlatformImage::ClearContextChangeCount(void)
{
	contextChangeCount = 0;
}
/// @}
	
void PlatformImage::Init(void)
{
	CalculateRegions();

	PS2Renderer::SetTextureDataCallback(PlatformImage::RendererDataCallback);

	u8* data = (u8*)&dataBlock[0];
	::clamp_1 = (u64*)data;
	MasterList::Append8(data, 0);
	MasterList::Append8(data, SCE_GS_CLAMP_1);
	::tex0_1 = (u64*)data;
	MasterList::Append8(data, 0);
	MasterList::Append8(data, SCE_GS_TEX0_1);
	::tex1_1 = (u64*)data;
	MasterList::Append8(data, 0);
	MasterList::Append8(data, SCE_GS_TEX1_1);
	::miptbp1_1 = (u64*)data;
	MasterList::Append8(data, 0);
	MasterList::Append8(data, SCE_GS_MIPTBP1_1);
	::miptbp2_1 = (u64*)data;
	MasterList::Append8(data, 0);
	MasterList::Append8(data, SCE_GS_MIPTBP2_1);
	::alpha_1 = (u64*)data;
	MasterList::Append8(data, 0);
	MasterList::Append8(data, SCE_GS_ALPHA_1);
}

void PlatformImage::Cleanup(void)
{
}

void PlatformImage::PausingTextureManager(void)
{
	if ( MasterList::CurrentlyActive() )
	{
		MasterList::Append(MasterList::createDMATAG(0 /*count*/));
		MasterList::Append(createNOP());
		MasterList::Append(createFLUSHA());
	}
	else
	{
		MasterList::Flush();
	}
}

void PlatformImage::ResumingTextureManager(void)
{
	FlushTextureMemory();

	// just in case things changed on us... :)
	CalculateRegions();
}

void PlatformImage::FlushTextureMemory(void)
{
	for(int i = 0; i < NUM_GS_REGIONS; i++)
		FlushRegion(i);
}

void PlatformImage::CalculateRegions(void)
{
	// setup for texture memory management.
	sCurrentRegion = 0;
	sRegionStart[0] = GSMemManager::GetHighestBlockUsed() + 1;
	sRegionSize = ((GS_MEM_TOP - sRegionStart[0]) / NUM_GS_REGIONS) - 1;

	for(int i = 0; i < NUM_GS_REGIONS-1; i++)
	{
		sRegionEnd[i] = sRegionStart[i] + sRegionSize;
		sRegionStart[i+1] = sRegionEnd[i] + 1;
	}
	sRegionEnd[NUM_GS_REGIONS-1] = GS_MEM_TOP;
	ASSERT(
		sRegionEnd[NUM_GS_REGIONS-1] - sRegionStart[NUM_GS_REGIONS-1] >=
		sRegionSize);

	sCurrentTexMemPtr = sRegionStart[sCurrentRegion];
}

u64 PlatformImage::GetTex0_1(u8 tfx)
{
	return SCE_GS_SET_TEX0_1(
				 mTextureAddr[0],	// tbp
				 mTexBufWidth[0],	// tbw
				 mGSFormat,		// psm
				 mWidthPow2,		// tw
				 mHeightPow2,	// th
				 0,					// tcc (set by tfx param)
				 0,					// tfx (set by tfx param)
				 mPaletteAddr,	// cbp,
				 mPaletteGSFormat,	// cpsm,
				 0,					// csm,
				 0,					// csa,
				 1) |				// cld) |
			 (((u64)tfx) << 34);     // ((TFX<<1)|TCC)
}

void* PlatformImage::RendererDataCallback(u_int bind, bool includeAlpha1, PS2MaterialInfo *pMatInfo)
{
	u8 wrapu;
	u8 wrapv;
	u8 filter;
	u8 blend;
	u8 method;

	Texture* pTexture = Texture::GetSelectedTexture();

	if(pTexture)
	{
		// if a texture is selected, it should override all other
		// renderer textures.  This is to support things like the shadow
		// renderer.
		return &::dataBlock[0];
	}

	if(bind == 0x7FFFFFFF)
	{
		pTexture = TextureMgr::GetWhiteTexture();
		wrapu = TS_WRAP_CLAMP;
		wrapv = TS_WRAP_CLAMP;
		filter = TS_FILTER_POINTSAMPLE;
		blend = RS_BLENDFUNC_DEFAULT;
		method = TS_METHOD_MODULATE;
	}
	else
	{
		ts_TextureLayer* layer = (ts_TextureLayer*)bind;
		ASSERT_PTR(layer);

		TextureDatabase* pTextureDatabase = layer->textureDatabase;
		ASSERT_PTR(pTextureDatabase);

		pTexture = pTextureDatabase->GetTexture(layer->s16TextureIndex);
		ASSERT_PTR(pTexture);

		wrapu = layer->u8WrapU;
		wrapv = layer->u8WrapV;
		filter = layer->u8Filter;
		blend = layer->u8Blend;
		method = layer->u8Method;
	}

	Image* img = pTexture->GetImage();
	ASSERT_PTR(img);

	PlatformImage* pImage = img->GetPlatformImage();
	ASSERT_PTR(pImage);

	// are we locked... we better be.  Alternately, we dont need to be
	// locked if something else is selected, because we wont get used
	// (shadow rendering)
	if(!Texture::GetSelectedTexture())
		ASSERT(pImage->mFlags & TEXTURE_LOCK_OUTSTANDING);

	// Handle locked modes... (Is this really necessary?
	// cant the person just alter the layer?)
	if(RenderState__Locked(RS_BLENDFUNC))
		blend = RenderState__Get(RS_BLENDFUNC);
	if(TextureState__Locked(0, TS_METHOD))
		method = TextureState__Get(0, TS_METHOD);
	if(TextureState__Locked(0, TS_WRAPU))
		wrapu = TextureState__Get(0, TS_WRAPU);
	if(TextureState__Locked(0, TS_WRAPV))
		wrapv = TextureState__Get(0, TS_WRAPV);
	if(TextureState__Locked(0, TS_FILTER))
		filter = TextureState__Get(0, TS_FILTER);

	pImage->FillStaticRegisters(filter, blend, method, wrapu, wrapv);

	return &::dataBlock[0];
}

void PlatformImage::FillStaticRegistersUsingTextureState(void)
{
	u8 blend = RenderState__Get(RS_BLENDFUNC);
	u8 method = TextureState__Get(0, TS_METHOD);
	u8 wrapu = TextureState__Get(0, TS_WRAPU);
	u8 wrapv = TextureState__Get(0, TS_WRAPV);
	u8 filter = TextureState__Get(0, TS_FILTER);

	FillStaticRegisters(filter, blend, method, wrapu, wrapv);
}

void PlatformImage::FillStaticRegisters(u8 filter, u8 blend, u8 method, u8 wrapu, u8 wrapv)
{
	contextChangeCount++;

	// convert the non-trivial ones
	u8 magfilt = (filter == TS_FILTER_POINTSAMPLE) ? 0 : 1;
	u8 uclamp = (wrapu == TS_WRAP_CLAMP) ? 1 : 0;
	u8 vclamp = (wrapv == TS_WRAP_CLAMP) ? 1 : 0;

	// generate register settings into the static buffer...
	*::tex0_1 = GetTex0_1(gMethodToFunction[method]);
	*::tex1_1 = SCE_GS_SET_TEX1_1(0, mMipLevels-1, magfilt, gMinFilter[filter], 0, mL, mK);
	*::miptbp1_1 = SCE_GS_SET_MIPTBP1_1(mTextureAddr[1], mTexBufWidth[1], mTextureAddr[2], mTexBufWidth[2], mTextureAddr[3], mTexBufWidth[3]);
	*::miptbp2_1 = SCE_GS_SET_MIPTBP2_1(mTextureAddr[4], mTexBufWidth[4], mTextureAddr[5], mTexBufWidth[5], mTextureAddr[6], mTexBufWidth[6]);
	*::clamp_1 = SCE_GS_SET_CLAMP_1(uclamp, vclamp, 0, 0, 0, 0);
	*::alpha_1 = gBlendToAlpha[blend];
}

u16 PlatformImage::AllocGSMem(u16 numBlocks)
{
	// we need to force page alignment for render to frame buffer textures.
	// as a cheap hack to determine this, test to see if there is a palette,
	// and if there isn't one then force the alignment.
	if ( mCPUSidePaletteData == NULL )
	{
		sCurrentTexMemPtr = ( sCurrentTexMemPtr + 31 ) & ~31;
	}

	ASSERTS(numBlocks <= sRegionSize, "Trying to alloc texmem for a texture that is WAY too big..");

	u16 texAddr = sCurrentTexMemPtr;
	sCurrentTexMemPtr += numBlocks;

	if(sCurrentTexMemPtr > sRegionEnd[sCurrentRegion])
	{
		ChangeRegions();

		// same hack as above
		if ( mCPUSidePaletteData == NULL )
		{
			sCurrentTexMemPtr = ( sCurrentTexMemPtr + 31 ) & ~31;
		}

		texAddr = sCurrentTexMemPtr;
		sCurrentTexMemPtr += numBlocks;
	}

	return texAddr;
}

void PlatformImage::AddUploadedImage(void)
{
	ASSERT(next == NULL && prev == NULL);
	sImagesInRegion[sCurrentRegion].AddHead(this);
}

void PlatformImage::RemoveUploadedImage(void)
{
	sImagesInRegion[sCurrentRegion].Unlink(this);
}

void PlatformImage::FlushRegion(int region)
{
	PlatformImage* curr = sImagesInRegion[region].Head();
	while(curr)
	{
		PlatformImage* next = curr->next;
		curr->Release();
		curr = next;
	}
}

void PlatformImage::ChangeRegions(void)
{
	// add flush
//	GSState__Send(SCE_GS_FLUSHA, 0);

	// find new region
	int nextRegion = sCurrentRegion+1;
	if(nextRegion >= NUM_GS_REGIONS)
		nextRegion = 0;

	// empty out all textures in unused region
	FlushRegion(nextRegion);

	// swap regions
	sCurrentRegion = nextRegion;
	sCurrentTexMemPtr = sRegionStart[sCurrentRegion];
}

u16 PlatformImage::BlockSize(u16 format, u16 width, u16 height)
{
	// borrow GSMemManager code so that we dont have to duplicate it.
	GSMemManager::allocationInfo tempInfo;
	GSMemManager::GetAllocationInfo(width, height, (width + 63)/64, format, false, tempInfo);

	// some multiple of pages
	if(tempInfo.pagesWide > 1 || tempInfo.pagesTall > 1)
	{
		return tempInfo.pagesWide * tempInfo.pagesTall * GS_BLOCKS_PER_PAGE;
	}

	// smaller than a page
	int blocks = GS_BLOCKS_PER_PAGE;
	while(!(tempInfo.blockBitPattern & 0x80000000))
	{
		tempInfo.blockBitPattern <<= 1;
		blocks--;
	}
	return blocks;
}


/// Make this image available for rendering
void PlatformImage::Lock(void)
{
	ASSERT(!(mFlags & TEXTURE_LOCK_OUTSTANDING));

	if(mOwner->GetFlags() & Image::BACK_BUFFER)
	{
		// at this point in time the front buffer and back buffer are inverted.
		int backbuffer = DisplayState.u32FrontBufferIndex;
		if ( Display__DownSampling() )
			backbuffer = 1;
		mTextureAddr[0] = DisplayState.u32FrameBufferAddr[ backbuffer ];
		mFlags |= TEXTURE_LOCK_OUTSTANDING;
		return;
	}
	else if(mOwner->GetFlags() & Image::FRONT_BUFFER)
	{
		int frontbuffer = DisplayState.u32FrontBufferIndex ^ 1;
		if ( Display__DownSampling() )
			frontbuffer = 0;
		// at this point in time the front buffer and back buffer are inverted.
		mTextureAddr[0] = DisplayState.u32FrameBufferAddr[ frontbuffer ];
		mFlags |= TEXTURE_LOCK_OUTSTANDING;
		return;
	}
	else if(mOwner->GetFlags() & Image::Z_BUFFER)
	{
		mTextureAddr[0] = DisplayState.u32ZBufferAddr;
		mFlags |= TEXTURE_LOCK_OUTSTANDING;
		return;
	}
	else if(mOwner->GetFlags() & Image::SHADOW)
	{
		// holy cheap, batman. redirect to the fixed space allocated for shadows
		// sucks because it only allows for one redirected shadow.
		mTextureAddr[0] = DisplayState.u32ShadowAddr;
		mFlags |= TEXTURE_LOCK_OUTSTANDING;
		return;
	}

	// is it already uploaded?
	if(mRegion == sCurrentRegion)
	{
		mFlags |= TEXTURE_LOCK_OUTSTANDING;
		return;
	}

	if(mRegion != 0xFF)
	{
		// this is the legal case of locking an image in one region, then
		// unlocking it, only to lock it again in the next region.  The result
		// should be that the texture is moved from on region to the next
		// TODO: move textures between regions without re-uploading them
		// for now we just invalidate the other occurance and reupload.
		Release();
	}

	mFlags |= TEXTURE_LOCK_OUTSTANDING;

	// allocate for maps
	u16 texaddr = AllocGSMem(mContiguousBlocks);
	for(int i = 0; i < mMipLevels; i++)
	{
		mTextureAddr[i] = texaddr;
		texaddr += mMipBlocks[i];
		ASSERT(mTextureAddr[i] >= DisplayState.u32TextureOrigBaseAddr && mTextureAddr[i] < GS_MEM_TOP);
		ASSERT(texaddr <= GS_MEM_TOP);
	}

	// allocate for palette
	if(mCPUSidePaletteData)
	{
		mPaletteAddr = texaddr;
	}

	// TJC - sanity
	ASSERT(
		((mOwner->GetFormat() & Image::TYPE_MASK) == Image::TYPE_RGBA) ||
		((mOwner->GetFormat() & Image::TYPE_MASK) == Image::TYPE_RGB) ||
		(mPaletteAddr >= DisplayState.u32TextureOrigBaseAddr && mPaletteAddr < GS_MEM_TOP));

	//  	dbgPrint(TTY_CYAN "%08x " TTY_RED " tex: %x pal: %x\n" TTY_RESET, this, mTextureAddr, mPaletteAddr);

	mRegion = sCurrentRegion;
	// printf("locking %x (%x)\n",this, mRegion);

	// upload the image...
	UploadImage();

	// add us to the uploaded list.
	AddUploadedImage();
}

void PlatformImage::DMAToVRAM(void* pImage, u16 width, u16 height, u16 format, u16 bpp, u16 texAddr, u32 texBufWidth)
{
	if ( ! TextureMgr::AllowTextureTransfers() )
		return;

	/* set up gs registers for transfer */
	MasterList::SetFlushed(false);

	GSState__Set(
		SCE_GS_BITBLTBUF,
		SCE_GS_SET_BITBLTBUF(
			0,								// sbp-- source buffer base pointer, vram word addr / 64 (vram block addr)
			0,								// sbw-- source buffer width, pixels / 64
			0,								// spsm-- source pixel format
			texAddr,				// dbp-- dest buffer base pointer, vram word addr / 64 (vram block addr)
			//yuch-- it's possible that this should be (u32Width + 63) / 64-- i.e. if it's 0 make it 1
			texBufWidth,			  	// dbw-- dest buffer width, pixels / 64
			format)					// dpsm-- dest pixel format
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
			width,	// rrw-- width of transmission area in pixels
			height)	// rrh-- height of transmission area in pixels
	);
	GSState__Set(SCE_GS_TRXDIR, SCE_GS_SET_TRXDIR(0));	// 0 = host -> local, 1 = local -> host, 2 = local -> local
	GSState__Send();

	/* calculate how many quad words to send */

	u32 nQWC = (width * height * bpp/8) / BYTES_PER_QUAD_WORD;
	if (nQWC == 0)
		nQWC = 1;	// must be at least one quad word for dma (very small textures may be less than 16 bytes)

	SceneMgr::GetStats().IncTextureLoadBytes( nQWC * 16 );

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
		MasterList::Append(addr, createDIRECT(nQWC));
		COPY_TO_MASTER_LIST(u8TexturePath2Packet, TEXTURE_PATH2_PACKET_QWC);
	}
	else
	{
		MasterList::Append(
			addr,
			MasterList::createDMATAG(nQWC, TAGID_REFE, pImage));
		addr += 4;	// skip VIF_NOP already there
		MasterList::Append(addr, createDIRECT(nQWC));

		DPUT_D1_TADR((u_int) (0x80000000 | (u_int) u8TexturePath2Packet));	// 0x80000000 means buffer is in scratchpad
		FlushCache(WRITEBACK_DCACHE);
		DPUT_D1_CHCR(D_CHCR_DIR_M | D_CHCR_STR_M | D_CHCR_SCHAIN_MODE_M | D_CHCR_TTE_M);
		DMA_Wait(D1_CHCR);	// this may take a while...i get a lock up if i don't wait for it, something must be writing over the dma source address register without doing a DMA_Wait first
	}

	GSState__Set(SCE_GS_TEXFLUSH, 0);
	GSState__Send();
}

void PlatformImage::GenerateMipLevel(u8 mipLevel)
{
	ASSERT(mipLevel > 0);

	u8 parentMip = mipLevel-1;
	u16 srcWidth = mOwner->GetWidth() >> parentMip;
	u16 srcHeight = mOwner->GetHeight() >> parentMip;
	u16 srcAddr = mTextureAddr[parentMip];

	GSTricks::Prep();

	// turn on bilerp
	GSState__Set(SCE_GS_TEX1_1, SCE_GS_SET_TEX1_1( 0, 0, 1, 1, 0, 0, 0 ));

	u16 destWidth = srcWidth >> 1;
	u16 destHeight = srcHeight >> 1;
	u16 destAddr = mTextureAddr[mipLevel];

	// downsample
	GSTricks::CopyBuffer(srcAddr,  srcWidth,  srcHeight,  mGSFormat,
								destAddr, destWidth, destHeight, mGSFormat,
								false, 1<<3, 1<<3);

	GSTricks::Cleanup();
}

void PlatformImage::UploadImage(void)
{
	u16 w, h;

	SceneMgr::GetStats().IncTextureLoadCount( 1 );

	if(mCPUSidePaletteData)
	{
		w = mOwner->GetPaletteWidth();
		h = mOwner->GetPaletteHeight();
		DMAToVRAM(mCPUSidePaletteData, w, h, mPaletteGSFormat, LookupBPP(mPaletteGSFormat), mPaletteAddr, (w+63)/64);
	}

	w = mOwner->GetWidth();
	h = mOwner->GetHeight();

	for(int level = 0; level < mMipLevels; level++)
	{
		if(mCPUSideImageData[level])
			DMAToVRAM(mCPUSideImageData[level], w, h, mGSFormat, GetBPP(), mTextureAddr[level], mTexBufWidth[level]);
		else if(mOwner->GetFlags() & Image::AUTO_MIPMAP)
			GenerateMipLevel(level);
		w >>= 1;
		h >>= 1;
	}
	GSState__Set(SCE_GS_TEXFLUSH, 0);
}

void PlatformImage::UploadPalette(void)
{
	u16 w, h;

	SceneMgr::GetStats().IncTextureLoadCount( 1 );

	if(mCPUSidePaletteData)
	{
		w = mOwner->GetPaletteWidth();
		h = mOwner->GetPaletteHeight();
		DMAToVRAM(mCPUSidePaletteData, w, h, mPaletteGSFormat, LookupBPP(mPaletteGSFormat), mPaletteAddr , (w+63)/64);
	}

	GSState__Set(SCE_GS_TEXFLUSH, 0);
}

void PlatformImage::AllocCPUImageBuffer(void)
{
	MEM_SET_ALLOC_NAME("PlatformImage");
	mCPUSideImageData[0] = memAlloc(mSize);
	mFlags |= TEXTURE_OWNS_CPU_BUFFER;
}

void PlatformImage::AllocCPUPaletteBuffer(void)
{
	MEM_SET_ALLOC_NAME("PlatformPalette");
	mCPUSidePaletteData = memAlloc(mSize);
	mFlags |= PALETTE_OWNS_CPU_BUFFER;
}

void PlatformImage::DownloadImage(void)
{
	ASSERT(mMipLevels == 1);
	ASSERT(mFlags & TEXTURE_LOCK_OUTSTANDING);
	ASSERT(mTextureAddr[0] != 0xFFFF);
	ASSERT(mRegion != 0xFF);
	// only non-paletted images for now...
	ASSERT(mGSFormat != SCE_GS_PSMT4 && mGSFormat != SCE_GS_PSMT8);

	ASSERT((mSize % 16) == 0);

	// in case masterlist was used to render into the texture, but its
	// no longer active, we need to flush here..
	MasterList::Flush();

	sceGsStoreImage simage;
	u32 chunkLength, chunkHeight;
	s32 height = mOwner->GetHeight();
	u16 TMemAddress = mTextureAddr[0];
	char *image;
	s32 maxAtOnce = 32768 / (mOwner->GetWidth() * (GetBPP() / 8) / 16);

	if(mCPUSideImageData[0] == NULL)
	{
		AllocCPUImageBuffer();
	}
	image = (char*)mCPUSideImageData[0];

	GSState__Send();

	while (height > 0)
	{
		/*prepare to read this chunk*/
		chunkHeight = Math::Min(maxAtOnce, height);
		sceGsSetDefStoreImage(&simage, TMemAddress, mTexBufWidth[0], mGSFormat, 0, 0, mOwner->GetWidth(), chunkHeight);
		FlushCache(WRITEBACK_DCACHE);

		/*read teh chunk*/
		sceGsExecStoreImage(&simage, (u_long128 *)image);
		sceGsSyncPath(0, 0);

		/* prepare for the next chunk */
		chunkLength = chunkHeight * mOwner->GetWidth() * GetBPP() / 8;
		TMemAddress += chunkLength / 256;//inc VRAM ptr, VRAM addresses are in units of 256 bytes
		image += chunkLength;				//inc SysRAM ptr
		height -= chunkHeight;				//dec amount left to do
	}
}

#define MAXWAIT	0x1000000

int isceGsExecStoreImage(sceGsStoreImage *sp, u_long128 *dstaddr)
{
    static	u_int init_mp3[4] __attribute__((aligned(16))) = {
		0x06000000,
		0x00000000,
		0x00000000,
		0x00000000,
    };

    u_int vcnt = 0;
    int w, h, i;
    int dmasizeq = 0, allsizeq, rsizeq = 0;
    int remq = 0, remb = 0, ah = 0, sizeb;
    u_char tmpbuf[16];
    u_long oldIMR;

    w = sp->trxreg.RRW;
    h = sp->trxreg.RRH;

    switch(sp->bitbltbuf.SPSM){
    case SCE_GS_PSMCT32:
    case SCE_GS_PSMZ32:
		sizeb = (((int)w*(int)h)<<2);
		remb = sizeb&0xf;
		remq = (sizeb>>4)&0x7;
		dmasizeq = (sizeb>>4)&0xfffffff8;
		if(remb == 0){
			rsizeq = 0;
			ah = h;
		}else{
			ah = (h+3)&0xfffffffc;
			allsizeq = ((int)w*ah)>>2;
			rsizeq = allsizeq - dmasizeq - remq - 1;
		}
		break;
    case SCE_GS_PSMCT24:
    case SCE_GS_PSMZ24:
		sizeb = ((int)w*(int)h*3);
		remb = sizeb &0xf;
		remq = (sizeb>>4)&0x7;
		dmasizeq = (sizeb>>4)&0xfffffff8;
		if(remb == 0){
			rsizeq = 0;
			ah = h;
		}else{
			ah = (h + 15) & 0xfffffff0;
			allsizeq = ((int)w*ah*3)>>4;
			rsizeq = allsizeq - dmasizeq - remq - 1;
		}
		break;
    case SCE_GS_PSMCT16:
    case SCE_GS_PSMCT16S:
    case SCE_GS_PSMZ16:
    case SCE_GS_PSMZ16S:
		sizeb = (((int)w*(int)h)<<1);
		remb = sizeb & 0xf;
		remq = (sizeb >> 4) & 0x7;
		dmasizeq = (sizeb>>4)& 0xfffffff8;
		if(remb == 0){
			rsizeq = 0;
			ah = h;
		} else {
			ah = (h + 7) & 0xfffffff8;
			allsizeq = ((int)w*ah)>>3;
			rsizeq = allsizeq - dmasizeq - remq - 1;
		}
		break;
    case SCE_GS_PSMT8:
    case SCE_GS_PSMT8H:
		sizeb = ((int)w*(int)h);
		remb = sizeb & 0xf;
		remq = (sizeb >> 4) & 0x7;
		dmasizeq = (sizeb >> 4) & 0xfffffff8;
		if(remb == 0){
			rsizeq = 0;
			ah = h;
		}else{
			ah = (h+7) & 0xfffffff8;
			allsizeq = ((int)w*ah)>>4;
			rsizeq = allsizeq - dmasizeq - remq - 1;
		}
		break;
    case SCE_GS_PSMT4:
    case SCE_GS_PSMT4HL:
    case SCE_GS_PSMT4HH:
		sizeb = (((int)w*(int)h)>>1);
		remb = sizeb & 0xf;
		remq = (sizeb >> 4) & 0x7;
		dmasizeq = (sizeb >> 4) & 0xfffffff8;
		if(remb == 0){
			rsizeq = 0;
			ah = h;
		}else{
			ah = (h + 7) & 0xfffffff8;
			allsizeq = ((int)w*ah)>>5;
			rsizeq = allsizeq - dmasizeq - remq - 1;
		}
		break;
    }

    if(remb != 0){
		*(u_long *)(((u_int)(&(sp->trxreg))) | 0x20000000) =
			(u_long)(sp->trxreg.RRW) | ((u_long)ah << 32);
    }

    while(DGET_D1_CHCR() & 0x0100){
		if((vcnt++) > MAXWAIT){
			scePrintf("sceGsExecStoreImage: DMA Ch.1 does not terminate\r\n");
			return -1;
		}
    }

    oldIMR = isceGsPutIMR(isceGsGetIMR() | 0x0200);
    DPUT_GS_CSR(0x02);
	 u32 oldStat = *D_STAT;
	 *D_STAT = oldStat & 0x03FF0000;
	
    DPUT_D1_QWC(7);
    if(((u_int)sp & 0x70000000) == 0x70000000)
		DPUT_D1_MADR((((u_int)sp & 0x0fffffff) | 0x80000000));
    else
		DPUT_D1_MADR(((u_int)sp & 0x0fffffff));
    DPUT_D1_CHCR((1 | (1<<8)));

    while(DGET_D1_CHCR() & 0x0100){
		if((vcnt++) > MAXWAIT){
			scePrintf("sceGsExecStoreImage: DMA Ch.1 does not terminate\r\n");
			return -1;
		}
    }
	
    while((DGET_GS_CSR() & 0x02) == 0){
		if((vcnt++) > MAXWAIT){
			scePrintf("sceGsExecStoreImage: GS does not terminate\r\n");
			DPUT_VIF1_FIFO(*(u_long128 *)init_mp3);
			return -1;
		}
    }

    *VIF1_STAT = 0x00800000;
    DPUT_GS_BUSDIR((u_long)0x00000001);

    if(dmasizeq){
		DPUT_D1_QWC(dmasizeq);

		if(((u_int)dstaddr & 0x70000000) == 0x70000000)
			DPUT_D1_MADR((((u_int)dstaddr & 0x0fffffff) | 0x80000000));
		else
			DPUT_D1_MADR(((u_int)dstaddr & 0x0fffffff));

		DPUT_D1_CHCR(1<<8);

		while(DGET_D1_CHCR() & 0x0100){
			if((vcnt++) > MAXWAIT){
				scePrintf("sceGsExecStoreImage: DMA Ch.1 (GS->MEM) does not terminate\r\n");
				DPUT_GS_CSR((u_long)0x00000100);
				DPUT_GS_BUSDIR((u_long)0);
				DPUT_GIF_CTRL(0x00000001);
				DPUT_VIF1_FBRST(1);
				return -1;
			}
		}
    }

    for(i = 0; i < remq; i++){
		while((*VIF1_STAT & 0x1f000000) == 0){
			if((vcnt++) > MAXWAIT){
				scePrintf("sceGsExecStoreImage: Enough data does not reach VIF1\n");
				DPUT_GS_CSR((u_long)0x00000100);
				DPUT_GS_BUSDIR((u_long)0);
				DPUT_GIF_CTRL(0x00000001);
				DPUT_VIF1_FBRST(1);
				return -1;
			}
		}
		
		((u_long128 *)dstaddr)[dmasizeq+i] = DGET_VIF1_FIFO();
    }

    if(remb){
		while((*VIF1_STAT & 0x1f000000) == 0){
			if((vcnt++) > MAXWAIT){
				scePrintf("sceGsExecStoreImage: Enough data does not reach VIF1\n");
				DPUT_GS_CSR((u_long)0x00000100);
				DPUT_GS_BUSDIR((u_long)0);
				DPUT_GIF_CTRL(0x00000001);
				DPUT_VIF1_FBRST(1);
				return -1;
			}
		}
		*(u_long128 *)&tmpbuf = DGET_VIF1_FIFO();
		for(i = 0; i < remb; i++){
			((u_char *)((u_long128 *)&dstaddr[dmasizeq+remq]))[i] = 
				tmpbuf[i];
		}
		for(i = 0; i < rsizeq; i++){
			while((*VIF1_STAT & 0x1f000000) == 0){
				if((vcnt++) > MAXWAIT){
					scePrintf("sceGsExecStoreImage: Enough data does not reach VIF1\n");
					DPUT_GS_CSR((u_long)0x00000100);
					DPUT_GS_BUSDIR((u_long)0);
					DPUT_GIF_CTRL(0x00000001);
					DPUT_VIF1_FBRST(1);
					return -1;
				}
			}
			*(u_long128 *)&tmpbuf = DGET_VIF1_FIFO();
		}
    }

    *VIF1_STAT = 0;
    DPUT_GS_BUSDIR((u_long)0);

	 *D_STAT = oldStat & 0x03FF0000;

	 isceGsPutIMR(oldIMR);
    DPUT_GS_CSR(0x02);

    DPUT_VIF1_FIFO(*(u_long128 *)init_mp3);

    return 0;
}


void PlatformImage::QueueDownloadImage()
{
	ASSERT(mFlags & TEXTURE_LOCK_OUTSTANDING);

	QueuedTransfer entry;
	entry.image = this;
	entry.mTextureAddr = mTextureAddr[ 0 ];
	sQueuedTransfers.Add( entry );

	GSState__Set(SCE_GS_LABEL, SCE_GS_SET_LABEL((u32)this, 0xFFFFFFFF));
	GSState__Send();
	MasterList::AppendDMATAG(0, TAGID_CNT, NULL, true);
	MasterList::Append(createFLUSHA());
	MasterList::Append(createNOP());
	MasterList::SetDMAInterruptCallback( PlatformImage::QueuedDownloadImageCallback );

	mFlags &= ~TEXTURE_CPU_QUEUE_DMA;

	if(mCPUSideImageData[0] == NULL)
	{
		AllocCPUImageBuffer();
	}
}

void PlatformImage::DownloadImageInterrupt( u16 textureAddr )
{
	ASSERT(mMipLevels == 1);
	ASSERT(textureAddr != 0xFFFF);
//	ASSERT(mRegion != 0xFF);
	// only non-paletted images for now...
	ASSERT(mGSFormat != SCE_GS_PSMT4 && mGSFormat != SCE_GS_PSMT8);

	ASSERT((mSize % 16) == 0);

	sceGsStoreImage simage;
	u32 chunkLength, chunkHeight;
	s32 height = mOwner->GetHeight();
	u16 TMemAddress = textureAddr;
	char *image;
	s32 maxAtOnce = 32768 / (mOwner->GetWidth() * (GetBPP() / 8) / 16);

	ASSERT_PTR(mCPUSideImageData[0]);
	image = (char*)mCPUSideImageData[0];

	while (height > 0)
	{
		/*prepare to read this chunk*/
		chunkHeight = Math::Min(maxAtOnce, height);
		sceGsSetDefStoreImage(&simage, TMemAddress, mTexBufWidth[0], mGSFormat, 0, 0, mOwner->GetWidth(), chunkHeight);
		iFlushCache(WRITEBACK_DCACHE);

		/*read teh chunk*/
		isceGsExecStoreImage(&simage, (u_long128 *)image);
//		sceGsSyncPath(0, 0);
		DMA_Wait(D1_CHCR);

		/* prepare for the next chunk */
		chunkLength = chunkHeight * mOwner->GetWidth() * GetBPP() / 8;
		TMemAddress += chunkLength / 256;//inc VRAM ptr, VRAM addresses are in units of 256 bytes
		image += chunkLength;				//inc SysRAM ptr
		height -= chunkHeight;				//dec amount left to do
	}
}

void PlatformImage::QueuedDownloadImageCallback()
{
	// save dma regs
	u32 madr = *D1_MADR;
	u32 tadr = *D1_TADR;
	u32 asr0 = *D1_ASR0;
	u32 asr1 = *D1_ASR1;
	u32 qwc = *D1_QWC;
	u32 chcr = *D1_CHCR;

	// wait for GS & VIF1 to stop...
	while((*GIF_STAT & GIF_STAT_APATH_M) ||
			(*VIF1_STAT & VIF1_STAT_VPS_M) ||
			((*GS_SIGLBLID>>32) == 0));
	

	u32 address = *GS_SIGLBLID >> 32;
	PlatformImage* image = (PlatformImage*) address;

	// this this entry in the queued list
	for ( int i = 0 ; i < sQueuedTransfers.Count() ; i++ )
	{
		if ( sQueuedTransfers[ i ].image == image )
		{
			image->DownloadImageInterrupt( sQueuedTransfers[ i ].mTextureAddr );
			sQueuedTransfers.Delete( i );
			break;
		}
	}

	if ( sQueuedTransfers.Count() == 0 )
		MasterList::SetDMAInterruptCallback( NULL );

	// restore dma regs
	*D1_MADR = madr;
	*D1_TADR = tadr;
	*D1_ASR0 = asr0;
	*D1_ASR1 = asr1;
	*D1_QWC = qwc;
	*D1_CHCR = chcr | D_CHCR_STR_M;
}

void PlatformImage::DownloadPalette(void)
{
	ASSERT(mFlags & PALETTE_LOCK_OUTSTANDING);
	ASSERT(mTextureAddr[0] != 0xFFFF);
	ASSERT(mRegion != 0xFF);
	// only non-paletted images for now...
//	ASSERT(mGSFormat != SCE_GS_PSMT4 && mGSFormat != SCE_GS_PSMT8);

	ASSERT((mSize % 16) == 0);

	sceGsStoreImage simage;
	u32 chunkLength, chunkHeight;
	s32 height = mOwner->GetPaletteHeight();
	u16 TMemAddress = mPaletteAddr;
	char *palette;
	s32 maxAtOnce = 32768 / (mOwner->GetPaletteWidth() * mOwner->GetPaletteHeight());

	palette = (char*)mCPUSidePaletteData;

	GSState__Send();

	while (height > 0)
	{
		/*prepare to read this chunk*/
		chunkHeight = Math::Min(maxAtOnce, height);
		sceGsSetDefStoreImage(&simage, TMemAddress, mTexBufWidth[0], mGSFormat, 0, 0, mOwner->GetPaletteWidth(), chunkHeight);
		FlushCache(WRITEBACK_DCACHE);

		/*read teh chunk*/
		sceGsExecStoreImage(&simage, (u_long128 *)palette);
		sceGsSyncPath(0, 0);

		/* prepare for the next chunk */
		chunkLength = chunkHeight * mOwner->GetPaletteWidth() * GetBPP() / 8;
		TMemAddress += chunkLength / 256;//inc VRAM ptr, VRAM addresses are in units of 256 bytes
		palette += chunkLength;				//inc SysRAM ptr
		height -= chunkHeight;				//dec amount left to do
	}
}

/// we are done rendering with this image (but that doesnt mean we
/// wont do it again soon... )
void PlatformImage::Unlock(void)
{
	ASSERT(mFlags & TEXTURE_LOCK_OUTSTANDING);

	if(!(mOwner->GetFlags() & Image::LOCAL))
	{
		if((!mCPUSideImageData[0]) || (mFlags & TEXTURE_CPU_SIDE_INVALID))
		{
			if (mFlags & TEXTURE_CPU_QUEUE_DMA)
			{
				QueueDownloadImage();
			}
			else
			{
				DownloadImage();
			}
			mFlags &= ~TEXTURE_CPU_SIDE_INVALID;
		}
	}

	mFlags &= ~TEXTURE_LOCK_OUTSTANDING;
}

void PlatformImage::BitBlt(int x, int y, int w, int h)
{
	ASSERT(mFlags & TEXTURE_LOCK_OUTSTANDING);
	ASSERT(mMipLevels == 1);
	GSTricks::Prep();

	GSTricks::Color(0x80, 0x80, 0x80, 0x80);
	/*
	GSTricks::CopyBuffer(
		mTextureAddr[0],
		mOwner->GetWidth(), 
		mOwner->GetHeight(), 
		mGSFormat, 
		// at this point in time the front buffer and back buffer are inverted.
		DisplayState.u32FrameBufferAddr[ DisplayState.u32FrontBufferIndex ],
		DisplayState.u32FrameBufferWidth,
		DisplayState.u32FrameBufferHeight,
		DisplayState.u32FramePixelFormat[ DisplayState.u32FrontBufferIndex ]);
		*/
	Select();
	GSState__Set(SCE_GS_TEXFLUSH, 0);
	GSState__Set(SCE_GS_FBA_1,  1);
	GSState__Set(SCE_GS_PRIM, createPRIM(
		SCE_GS_PRIM_SPRITE, 
		false,	// iip
		true,		// tme
		false,	// fge
		false,	// abe (NO ALPHA BLEND RIGHT NOW!!! Due to fullscreendistort)
		false,	// aa1
		true,		// fst
		false,	// ctxt
		false));// fix
		
	GSTricks::LotsOfStrips(
			1, 
			0, 0, mOwner->GetWidth(), mOwner->GetHeight(), 0, 
			x, y, w, h, 0,
			/*offsetU*/0, /*offsetV*/0);

	// wipe alpha channel 
	GSTricks::Mask(0x00FFFFFF); // only write to alpha
	GSTricks::SolidPoly(mTextureAddr[0], mOwner->GetWidth(), mOwner->GetHeight(), mGSFormat, false);

	GSState__Set(SCE_GS_TEXFLUSH, 0);
	Unselect();
	GSTricks::Cleanup();
}

void PlatformImage::BltGtoA(int x, int y, int w, int h)
{
	// these are hardcoded limitations of the code below.  
	// it is probably possible to relax them a bit with
	// some rework and an example to work with.
	ASSERT(w == mOwner->GetWidth());
	ASSERT(h == mOwner->GetHeight());
	ASSERT(x == 0);
	ASSERT(y == 0);
	ASSERT(w == DisplayState.u32FrameBufferWidth);
	ASSERT(h == DisplayState.u32FrameBufferHeight);

	// check that we are doing a valid operation
	ASSERT(mFlags & TEXTURE_LOCK_OUTSTANDING);
	ASSERT(mMipLevels == 1);

	GSTricks::Prep();

	GSTricks::Color(0x80, 0x80, 0x80, 0x80);

	GSTricks::RGToBA(mTextureAddr[0], mOwner->GetWidth(), mOwner->GetHeight(), mGSFormat,
							DisplayState.u32ZBufferAddr,
							SCE_GS_PSMCT32);

	GSTricks::Mask(0x00FFFFFF); // only write to alpha

	GSTricks::CopyBuffer(DisplayState.u32ZBufferAddr,
								mOwner->GetWidth(), mOwner->GetHeight(), SCE_GS_PSMCT32,
								DisplayState.u32FrameBufferAddr[0],
								w, h, DisplayState.u32FramePixelFormat[0]);

	GSTricks::Cleanup();
}

void PlatformImage::ZapAlpha(int x, int y, int w, int h)
{
	ASSERT(x == 0);
	ASSERT(y == 0);

	GSTricks::Prep();

	GSTricks::Color(0x80, 0x80, 0x80, 0x80);
	GSTricks::Mask(0x00FFFFFF); // only write to alpha

	GSTricks::SolidPoly(DisplayState.u32FrameBufferAddr[0], w, h, DisplayState.u32FramePixelFormat[0]);

	GSTricks::Cleanup();
}

void PlatformImage::LockForRender(void)
{
	ASSERT(mMipLevels == 1);
	u16 width = mOwner->GetWidth();
	u16 height = mOwner->GetHeight();

	/* set up texture as frame buffer. to be a frame buffer, texture must be page aligned */
	ASSERT(mFlags & TEXTURE_LOCK_OUTSTANDING);
	ASSERT(((mTextureAddr[0] * (VRAM_BYTES_PER_BLOCK / VRAM_BYTES_PER_WORD)) % 2048) == 0);	// texture must be on 2048-word boundary to be rendered into

	GSState__Set(SCE_GS_LABEL, SCE_GS_SET_LABEL((u32)0x00000000, 0xFFFFFFFF));
	GSState__Set(SCE_GS_FINISH, 0);

	sDisplayStateCache = DisplayState;
	DisplayState.u32FrameBufferAddr[0] = 
		DisplayState.u32FrameBufferAddr[1] = mTextureAddr[0];
	DisplayState.u32FramePixelFormat[0] = 
		DisplayState.u32FramePixelFormat[1] = mGSFormat;
	DisplayState.nScreenWidth = width;
	DisplayState.nScreenHeight = height;
	DisplayState.u32DisplayWidth = DisplayState.u32FrameBufferWidth = width;
	DisplayState.u32DisplayHeight = DisplayState.u32FrameBufferHeight = height;
	Display__SetFrameBuffer();
	MatrixStack__FullScreenViewport();

	/* we may not want the resulting texture to have an alpha in it. what was happening was that in a 16-bit texture, */
	/* rendering particles into the texture set the 1-bit alpha to 0 for those pixels, and then rendering the rendered */
	/* texture into the final screen showed up with holes in the texture. so change the frame buffer alpha register */

	u16 format = mOwner->GetFormat();

	u16 size = format & Image::SIZE_MASK;
	u16 type = format & Image::TYPE_MASK;

	ASSERT(size == Image::SIZE_32_BIT || size == Image::SIZE_16_BIT || size == Image::SIZE_24_BIT);

	if (type == Image::TYPE_RGB)
		GSState__Set(SCE_GS_FBA_1, SCE_GS_SET_FBA_1(1));
	else if (type == Image::TYPE_RGBA)
		GSState__Set(SCE_GS_FBA_1, SCE_GS_SET_FBA_1(0));
	else
		ASSERT(false);

	if(size == Image::SIZE_16_BIT && !(mOwner->GetFlags() & Image::SHADOW))
		GSState__Set(SCE_GS_DTHE, 1);
	else
		GSState__Set(SCE_GS_DTHE, 0);

	GSState__Set(SCE_GS_SCANMSK, 0);

	GSState__Set(SCE_GS_SCISSOR_1, SCE_GS_SET_SCISSOR(0,width-1, 0, height-1));

	GSState__Send();
	//ned: i added these while i was investigating why rendering into a texture wasn't working. it doesn't seem to be necessary,
	//but note that gfxStartRender calls these with the screen dimensions
	//	PS2Renderer::SetViewToClip(u32Width, u32Height);
	//	PS2Renderer::SetViewToCull(u32Width, u32Height);
}

void PlatformImage::UnlockForRender(void)
{
	GSState__Set(SCE_GS_TEXFLUSH, 0);

	DisplayState = sDisplayStateCache;

	mFlags |= TEXTURE_CPU_SIDE_INVALID;
	if(MasterList::CurrentlyActive())
		mFlags |= TEXTURE_CPU_QUEUE_DMA;			// cause dma to happen after render completes

	Display__SetFrameBuffer();
	MatrixStack__FullScreenViewport();
	GSState__Send();
}

/// platform specific function to get a pointer to the palette buffer
void* PlatformImage::GetPaletteBuffer(void)
{
	ASSERT(!(mFlags & PALETTE_BUFFER_FETCHED));

	// if we have no cpu side alloc, and this is not a temp image, we
	// need to fetch it back from texmem... ick.. this is slow. we need
	// to be careful about using this.
	if((mCPUSidePaletteData == NULL) || (mFlags & PALETTE_CPU_SIDE_INVALID))
	{
		if(mFlags & PALETTE_LOCK_OUTSTANDING)
		{
			// these images should be marked LOCAL!
			ASSERT(mOwner->GetFlags() & Image::LOCAL);

			DownloadPalette();
		}
		else if(mCPUSidePaletteData == NULL)
			AllocCPUPaletteBuffer();
	}

	mFlags |= PALETTE_BUFFER_FETCHED;
	return mCPUSidePaletteData;
}

/// platform specific function to release the palette buffer
void PlatformImage::ReleasePaletteBuffer(bool modified)
{
	ASSERT(mFlags & PALETTE_BUFFER_FETCHED);

	if (mOwner->GetFlags() & Image::LOCAL)
	{
		// modified an image that didnt have a CPU side representation
		// before you requested it.  This should upload the image...
		if(modified)
		{
			// this wont work because FreeMainRam frees the CPU buffer
			// before we ever get around to running the DMA chain created
			// to upload the image..
			ASSERTS(false, "We need to write more code");
			UploadImage();
		}

		ASSERTS(false, "This should only free the palette");
		FreeMainRam();
	}

	mFlags &= ~PALETTE_BUFFER_FETCHED;
}

/// platform specific function to get a pointer to the image buffer
void* PlatformImage::GetImageBuffer(void)
{
	ASSERT(!(mFlags & TEXTURE_BUFFER_FETCHED));

	// if we have no cpu side alloc, and this is not a temp image, we
	// need to fetch it back from texmem... ick.. this is slow. we need
	// to be careful about using this.
	if((mCPUSideImageData[0] == NULL) || (mFlags & TEXTURE_CPU_SIDE_INVALID))
	{
		if(mFlags & TEXTURE_LOCK_OUTSTANDING)
		{
			// these images should be marked LOCAL!
			ASSERT(mOwner->GetFlags() & Image::LOCAL);

			DownloadImage();
		}
		else if(mCPUSideImageData[0] == NULL)
			AllocCPUImageBuffer();
	}

	mFlags |= TEXTURE_BUFFER_FETCHED;
	return mCPUSideImageData[0];
}

/// platform specific function to release the image buffer
void PlatformImage::ReleaseImageBuffer(bool modified)
{
	ASSERT(mFlags & TEXTURE_BUFFER_FETCHED);

	if (mOwner->GetFlags() & Image::LOCAL)
	{
		// modified an image that didnt have a CPU side representation
		// before you requested it.  This should upload the image...
		if(modified)
		{
			// this wont work because FreeMainRam frees the CPU buffer
			// before we ever get around to running the DMA chain created
			// to upload the image..
			ASSERTS(false, "We need to write more code");
			UploadImage();
		}

		ASSERTS(false, "This should not free the palette");
		FreeMainRam();
	}

	mFlags &= ~TEXTURE_BUFFER_FETCHED;
}

/// create the platform specific info given the inputs to the contructor
PlatformImage::PlatformImage(Image* owner, void* data, void* palette, bool FromXPR)
		: mOwner(owner),
		next(NULL),
		prev(NULL),
		mK(FIXED_POINT_8_4(-2.0f)),
		mL(0),
		mRegion(0xFF)
{
	u16 width = mOwner->GetWidth();
	u16 height = mOwner->GetHeight();

	mFlags = 0;

	// grab some info
	mGSFormat = GetPS2Format(mOwner->GetFormat());
	if(mOwner->GetFlags() & Image::HAS_PALETTE)
	{
		ASSERT((mOwner->GetFormat() & Image::TYPE_MASK) == Image::TYPE_CI);
		ASSERT(palette != NULL);
		mPaletteGSFormat = GetPS2Format(mOwner->GetPaletteFormat());
	}
	else
	{
		ASSERT(palette == NULL);
	}

	mBPP = LookupBPP(mGSFormat);
	mSize = width * height * GetBPP() / 8;

	// calculate some stuff..
	u32 texwidth = width - 1;
	u32 texheight = height - 1;
	mWidthPow2 = 0;
	mHeightPow2 = 0;
	while (texwidth)
	{
		texwidth >>= 1;
		mWidthPow2++;
	}
	while (texheight)
	{
		texheight >>= 1;
		mHeightPow2++;
	}

	mMaxMipLevels = Math::Max(
							 1, Math::Min(
								 MAX_MIP_LEVELS,
								 Math::Min(mWidthPow2-2, mHeightPow2-2)));

	texwidth = width;
	texheight = height;
	for(int level = 0; level < mMaxMipLevels; level++)
	{
		mTexBufWidth[level] = Math::Max(1u, (texwidth + 63) / 64);
		texwidth >>= 1;
		texheight >>= 1;
		mTextureAddr[level] = 0xFFFF;
		mCPUSideImageData[level] = NULL;
	}
	mMipBlocks[0] = BlockSize(mGSFormat, width, height);
	mContiguousBlocks = mMipBlocks[0];
	mPaletteAddr = 0xFFFF;
	mCPUSideImageData[0] = data;
	mCPUSidePaletteData = palette;
	int save = mContiguousBlocks;
	if(mCPUSidePaletteData)
	{
		mContiguousBlocks += BlockSize(mPaletteGSFormat,
												 mOwner->GetPaletteWidth(),
												 mOwner->GetPaletteWidth());
	}
	mMipLevels = 1;
}

PlatformImage::~PlatformImage()
{
	// printf("deleting %x  (%x)\n", this, mRegion);
	if(mRegion != 0xFF)
	{
		Release();
	}
	FreeMainRam();
}

void PlatformImage::SetMipmapDistance(float fDistance)
{
	// TJC - This code taken from the old TextureBind.cpp

	/* set the l an k values. the l value is a shift in the automatic LOD calculation. typically 0, the manuals say */
	/* you might use a non-zero value for oblique-to-the-screen polys-- what a nightmare! the k value is sort of a */
	/* "distance" in log2 space of the w value when a pixel is rendered. this is apparently not affected by the */
	/* depth of the z-buffer-- must be w in camera space. more negative means start the mipmap switch farther away */

#define BASE_TABLE_ENTRIES	11	// if you change this for more accuracy, have to rebuild the table
#define BASE_TABLE_KINC		(1.0f / (float) (BASE_TABLE_ENTRIES - 1))
	static float BaseValueTable[BASE_TABLE_ENTRIES][2] = {

				// first value is k, second value is distance in meters that it corresponds to

				{	1.0f,	2.75f},
				{	1.1f,	2.947f},
				{	1.2f,	3.159f},
				{	1.3f,	3.386f},
				{	1.4f,	3.629f},
				{	1.5f,	3.889f},
				{	1.6f,	4.168f},
				{	1.7f,	4.467f},
				{	1.8f,	4.788f},
				{	1.9f,	5.132f},
				{	2.0f,	5.5f}};

	/* ok, we have the equation dist = 2.75 * (2 ^ (k - 1)) (this was arrived at empirically, by looking at how far */
	/* the camera was from the origin when the mipmap switch occurred). we have dist, we want to solve for k. BaseValueTable */
	/* holds pre-calculated solutions for k in 0.1 increments. since this is a log2 table, if you add 1.0 to k, */
	/* the distance value doubles. so for k = 2.0, we have distance = 2.75 * 2.0. our technique is to figure out */
	/* which integer values of k that the passed distance falls, and then to calculate the values with a 0.1 increment, */
	/* then to interpolate between values. this should be plenty fast (this only gets called a few times during */
	/* texture initialization) and plenty accurate (as the distance increases, the accuracy decreases because the */
	/* delta between subsequent k values increases) */

	float fK;	// the result
	if (fDistance < BaseValueTable[0][1])
		fK = BaseValueTable[0][0];	// minimum value
	else
	{
		/* find integer k-value range */

		float fLowK = BaseValueTable[0][0];											// lowest k entry in BaseValueTable
		float fHighDistance = BaseValueTable[BASE_TABLE_ENTRIES - 1][1];	// highest distance entry in BaseValueTable
		float fMultiplier = 1.0f;														// multiplier doubles for each addition of 1.0 to k range
		while (fDistance > fHighDistance)
		{
			fLowK += 1.0f;
			fHighDistance *= 2.0f;
			fMultiplier *= 2.0f;
		}

		/* find pair of entries that distance lies between */

		float fLowDistance = fHighDistance;
		for (int i = 1; i < BASE_TABLE_ENTRIES; i++)
		{
			fHighDistance = BaseValueTable[i][1] * fMultiplier;
			if (fDistance <= fHighDistance)
				break;
			fLowK += BASE_TABLE_KINC;
			fLowDistance = fHighDistance;
		}
		ASSERT((fDistance >= fLowDistance) && (fDistance <= fHighDistance));

		/* interpolate */

		fK = fLowK + (BASE_TABLE_KINC * ((fDistance - fLowDistance) / (fHighDistance - fLowDistance)));
	}

	/* now put result into the register */

	mK = FIXED_POINT_8_4(-fK);	// k value (signed 8.4, -128.0 to 127.0)
	mL = 0;
}

void PlatformImage::AppendMipLevel(u16 format, u16 width, u16 height, void* data, u16 palFmt, u16 palWidth, u16 palHeight, void* palette)
{
	int level = GetNumMipLevels();
	ASSERT(level > 0);
	ASSERT(level < mMaxMipLevels);

	ASSERT(format == mOwner->GetFormat());
	ASSERT((mOwner->GetWidth() >> level) == width);
	ASSERT((mOwner->GetHeight() >> level) == height);
	ASSERT(palFmt == mOwner->GetPaletteFormat());
	ASSERT(palWidth == mOwner->GetPaletteWidth());
	ASSERT(palHeight == mOwner->GetPaletteHeight());
	ASSERT(palette == mCPUSidePaletteData);

	mCPUSideImageData[level] = data;

//	/// CRAPPY HACK, color the mip map levels -- Assumes 32 bit palette and 8-bit/pixel.
//	switch ( level )
//	{
//		case 1:			((u32*)palette)[ level ] = 0x7fff007f;	break;
//		case 2:			((u32*)palette)[ level ] = 0x7f00ff7f;	break;
//	}
//	for ( int xxx = 0 ; xxx < width * height ; xxx++ )
//		((char*)data)[ xxx ] = level;
//	/// CRAPPY HACK

	mMipBlocks[level] = BlockSize(mGSFormat, width, height);
	mContiguousBlocks += mMipBlocks[level];

	mMipLevels++;
}

u16 PlatformImage::LookupBPP(u16 gsFormat)
{
	switch(gsFormat)
	{
	case SCE_GS_PSMCT32:
		return 32;
	case SCE_GS_PSMCT24:
		return 24;
	case SCE_GS_PSMCT16:
	case SCE_GS_PSMCT16S:
		return 16;
	case SCE_GS_PSMT8:
		return 8;
	case SCE_GS_PSMT4:
		return 4;
	default:
		ASSERTS(false, "Unknown format encountered");
	}
	return 0;
}

u16 PlatformImage::GetPS2Format(u32 flags)
{
	switch(flags & Image::TYPE_MASK)
	{
	case Image::TYPE_RGB:
		switch(flags & Image::SIZE_MASK)
		{
		case Image::SIZE_16_BIT:
			return(SCE_GS_PSMCT16);
		case Image::SIZE_24_BIT:
			return(SCE_GS_PSMCT24);
		default:
			ASSERT(0);	// only supporting 16-bit rgb on ps2, no 24-bit
			break;
		}
		break;
	case Image::TYPE_RGBA:
		switch(flags & Image::SIZE_MASK)
		{
		case Image::SIZE_16_BIT:
			return(SCE_GS_PSMCT16);
		case Image::SIZE_32_BIT:
			return(SCE_GS_PSMCT32);
		default:
			ASSERT(0);
			break;
		}
		break;
	case Image::TYPE_CI:
		switch(flags & Image::SIZE_MASK)
		{
		case Image::SIZE_4_BIT:
			return(SCE_GS_PSMT4);
		case Image::SIZE_8_BIT:
			return(SCE_GS_PSMT8);
		default:
			ASSERT(0);
			break;
		}
		break;
	case Image::TYPE_IA:
		ASSERT(0);
		break;
	case Image::TYPE_LUMINANCE:
		ASSERT(0);
		break;
	default:
		ASSERT(0);
		break;
	}
	return 0;
}

/// release..  This is a PS2 specific function to actually release
/// the texture memory allocated by this image.
void PlatformImage::Release()
{
	// printf("releasing %x (%x)\n", this, mRegion);
	// this is how we know we ran out of texture memory!
	// too many textures are locked at one time.

//	if ( mFlags & TEXTURE_LOCK_OUTSTANDING )
//	{
//		int totalBlocksAlloced = 0;
//		dbgPrint("freeing from region %d\n", mRegion);
//		dbgPrint("Region  Blocks  Texture\n");
//		for ( int region = 0 ; region < NUM_GS_REGIONS; region++)
//		{
//			PlatformImage* curr = sImagesInRegion[region].Head();
//			while(curr)
//			{
//				dbgPrint("  %d      %d\n", region, curr->mContiguousBlocks);
//				totalBlocksAlloced += curr->mContiguousBlocks;
//				curr = curr->next;
//			}
//		}
//		dbgPrint("Total blocks: %d\n", totalBlocksAlloced);
//	}

	ASSERTS(!(mFlags & TEXTURE_LOCK_OUTSTANDING), "Out of texture memory");
	ASSERTS(mRegion != 0xFF, "Image doesnt belong to a texmem region, but were freeing it.");

	if(mRegion != 0xFF)
	{
		sImagesInRegion[mRegion].Unlink(this);
	}
	mRegion = 0xFF;
}

void PlatformImage::GetRegisterValues(u64 *pTex0_1, u64 *pTex1_1, u64 *pClamp_1, u64 *pMiptbp1_1, u64 *pMiptbp2_1)
{
	FillStaticRegistersUsingTextureState();

	if(pTex0_1)
	{
		// are we locked... we better be
		ASSERT(mFlags & TEXTURE_LOCK_OUTSTANDING);
		*pTex0_1 = *::tex0_1;
	}
	if(pTex1_1)
		*pTex1_1 = *::tex1_1;
	if(pClamp_1)
		*pClamp_1 = *::clamp_1;
	if(pMiptbp1_1)
		*pMiptbp1_1 = *::miptbp1_1;
	if(pMiptbp2_1)
		*pMiptbp2_1 = *::miptbp2_1;
}

/// release the memory used in main memory.  If this image is not
/// in texture memory, its data is lost to the ether.
void PlatformImage::FreeMainRam(void)
{
	ASSERTS(!(mFlags & TEXTURE_BUFFER_FETCHED), "Free while GetImageBuffer outstanding");
	ASSERTS(!(mFlags & PALETTE_BUFFER_FETCHED), "Free while GetPaletteBuffer outstanding");

	if(mFlags & TEXTURE_OWNS_CPU_BUFFER)
	{
		if(mCPUSideImageData[0])
			memFree(mCPUSideImageData[0]);
	}

	if (mFlags & PALETTE_OWNS_CPU_BUFFER)
	{
		if(mCPUSidePaletteData)
			memFree(mCPUSidePaletteData);
	}

	for(int level = 0; level < mMipLevels; level++)
	{
		mCPUSideImageData[level] = NULL;
	}
	mCPUSidePaletteData = NULL;
}

/// prep this image for drawing operations
/// this is rarely called on the ps2
void PlatformImage::Select(void)
{
	// are we locked... we better be
	ASSERT(mFlags & TEXTURE_LOCK_OUTSTANDING);

	FillStaticRegistersUsingTextureState();

	GSState__Set(SCE_GS_TEX0_1, *::tex0_1);
	GSState__Set(SCE_GS_TEX1_1, *::tex1_1);
	GSState__Set(SCE_GS_CLAMP_1, *::clamp_1);
	GSState__Set(SCE_GS_ALPHA_1, *::alpha_1);
	if (*::tex1_1 & GS_TEX1_MXL_M)	// if mipmapping is enabled
	{
		GSState__Set(SCE_GS_MIPTBP1_1, *::miptbp1_1);
		if (*::tex1_1 & GS_TEX1_MXL_M >> (3L << GS_TEX1_MXL_O))	// if levels 3-6 are needed
			GSState__Set(SCE_GS_MIPTBP2_1, *::miptbp2_1);
	}
}

/// unmark this image for drawing operations.
/// this is rarely called on the ps2.
void PlatformImage::Unselect(void)
{
	// are we locked... we better be
	ASSERT(mFlags & TEXTURE_LOCK_OUTSTANDING);

	// nothing to do here on ps2
}

#ifdef _DEBUG
void PlatformImage::Test(void)
{
	int gsmem = GSMemManager::GetHighestBlockUsed();

	TestThatTestImagesVary();
	TestUpDown();
	TestConstantAddress();
	TestLockForRender();
	TestSelect();

	// TODO: test lru algorithm...

	if(gsmem != GSMemManager::GetHighestBlockUsed())
		dbgPrint(TTY_RED "GSMemManager Leaked during testing\n" TTY_RESET);
}

void PlatformImage::DisplayResults(bool success)
{
	if(success)
	{
		dbgPrint(TTY_GREEN "Success\n" TTY_RESET);
	}
	else
	{
		dbgPrint(TTY_RED "Failure\n" TTY_RESET);
	}
}

bool PlatformImage::CheckTestImage(void)
{
	void* cpuimg = GetImageBuffer();
	bool results = memcmp(testImgData, cpuimg, mSize) == 0;
	ReleaseImageBuffer();
	return results;
}

bool PlatformImage::IsWhite(void)
{
	u8* buff1 = (u8*)GetImageBuffer();

	bool isWhite = true;
	for(u32 c = 0; c < mSize; c++)
	{
		if(buff1[c] != 0x80)
		{
			isWhite = false;
			break;
		}
	}

	ReleaseImageBuffer();
	return isWhite;
}

bool PlatformImage::CompareTo(PlatformImage* rhs)
{
	if(mSize != rhs->mSize) return false;
	void* buff1 = GetImageBuffer();
	void* buff2 = rhs->GetImageBuffer();
	bool same = memcmp(buff1, buff2, mSize) == 0;
	ReleaseImageBuffer();
	rhs->ReleaseImageBuffer();
	return same;
}

void PlatformImage::DestroyTestImage(PlatformImage* dumpme)
{
	Image* img = dumpme->mOwner;
	memFree(dumpme->testImgData);
	delete img;
}

PlatformImage* PlatformImage::CreateTestImage(u16 width, u16 height, u16 format)
{
	// it needs image data
	u32 size = width*height*LookupBPP(GetPS2Format(format))/8;
	void*test = memAlloc(size);
	static int value = 1;
	ASSERT((size & 1) == 0);
	memset(test, value++, size/2);
	memset((char*)test + size/2, value++, size/2);
	if(value > 250) value = 1;

	// now create it
	Image* i = new Image(0, format, width, height, test, false);
	PlatformImage* pi = i->GetPlatformImage();
	pi->testImgData = test;
	return pi;
}

void PlatformImage::TestConstantAddress(void)
{
	dbgPrint("Testing PlatformImage Constant VRam Address...");

	PlatformImage* test1 = CreateTestImage(16,16,Image::TYPE_RGBA|Image::SIZE_32_BIT);

	test1->Lock();
	u16 base = test1->mTextureAddr[0];
	test1->Unlock();
	test1->Lock();
	bool success = base == test1->mTextureAddr[0];
	test1->Unlock();
	DestroyTestImage(test1);

	DisplayResults(success);
}

void PlatformImage::TestThatTestImagesVary(void)
{
	dbgPrint("Testing PlatformImage test images should vary...");
	PlatformImage* test1 = CreateTestImage(16,16,Image::TYPE_RGBA|Image::SIZE_32_BIT);
	PlatformImage* test2 = CreateTestImage(16,16,Image::TYPE_RGBA|Image::SIZE_32_BIT);
	bool success = !test1->CompareTo(test2);
	DestroyTestImage(test1);
	DestroyTestImage(test2);
	DisplayResults(success);
}

void PlatformImage::TestLockForRender(void)
{
	dbgPrint("Testing PlatformImage LockForRender...");

	int width = 64;
	int height = 64;
	PlatformImage* test1 = CreateTestImage(width,height,Image::TYPE_RGBA|Image::SIZE_32_BIT);

	test1->Lock();
	test1->LockForRender();

	// disable z-buffer writes, turn off z-test
	u64 ZBUF_1 = GSState.ZBUF_1 | (1L << GS_ZBUF_ZMSK_O);	// disable z-buffer writes
	u64 TEST_1 = SCE_GS_SET_TEST(0,0,0,0,0,0,1,1);
	GSState__Set(SCE_GS_ZBUF_1, ZBUF_1);
	GSState__Set(SCE_GS_TEST_1, TEST_1);

	// draw a rectangle in correct color and at proper z-depth

	u32 u32X = 2048 - (width / 2);
	u32 u32Y = 2048 - (height / 2);

	// a sprite is top left and bottom right corner, and flat shaded,
	// so we only need the color once.
	GSState__Set(SCE_GS_PRMODECONT, 1);
	GSState__Set(SCE_GS_PRIM,SCE_GS_SET_PRIM(SCE_GS_PRIM_SPRITE,0,0,0,0,0,0,0,0));
	GSState__Set(SCE_GS_XYZ2,SCE_GS_SET_XYZ2((u32X << 4),(u32Y << 4),0));
	GSState__Set(SCE_GS_RGBAQ,0x80808080);
	GSState__Set(SCE_GS_XYZ2,SCE_GS_SET_XYZ2(((u32X + width) << 4),((u32Y + height) << 4),0));

	// restore original states
	GSState__Set(SCE_GS_ZBUF_1, GSState.ZBUF_1);
	GSState__Set(SCE_GS_TEST_1, GSState.TEST_1);

	test1->UnlockForRender();
	test1->Unlock();

	DisplayResults(test1->IsWhite());

	DestroyTestImage(test1);
}

void PlatformImage::TestSelect(void)
{
	dbgPrint("Testing PlatformImage Select...");

	int width = 64;
	int height = 64;
	PlatformImage* test1 = CreateTestImage(width,height,Image::TYPE_RGBA|Image::SIZE_32_BIT);
	PlatformImage* test2 = CreateTestImage(width,height,Image::TYPE_RGBA|Image::SIZE_32_BIT);

	test1->Lock();
	test2->Lock();

	test1->LockForRender();
	test2->Select();

	// disable z-buffer writes, turn off z-test
	u64 ZBUF_1 = GSState.ZBUF_1 | (1L << GS_ZBUF_ZMSK_O);	// disable z-buffer writes
	u64 TEST_1 = SCE_GS_SET_TEST(0,0,0,0,0,0,1,1);
	GSState__Set(SCE_GS_ZBUF_1, ZBUF_1);
	GSState__Set(SCE_GS_TEST_1, TEST_1);

	// draw a rectangle in correct color and at proper z-depth

	u32 u32X = 2048 - (width / 2);
	u32 u32Y = 2048 - (height / 2);

	// override tex1_1 to turn off bilerp
	GSState__Set(SCE_GS_TEX1_1,0);

	// a sprite is top left and bottom right corner, and uvs to go with
	// each.  one color, we're flat shaded.
	GSState__Set(SCE_GS_PRMODECONT, 1);
	GSState__Set(SCE_GS_PRIM,SCE_GS_SET_PRIM(SCE_GS_PRIM_SPRITE,0,1,0,0,0,1,0,0));
	GSState__Set(SCE_GS_RGBAQ,0x80808080);
	GSState__Set(SCE_GS_UV, SCE_GS_SET_UV(0, 0));
	GSState__Set(SCE_GS_XYZ2,SCE_GS_SET_XYZ2((u32X << 4),(u32Y << 4),0));
	GSState__Set(SCE_GS_UV, SCE_GS_SET_UV(width << 4, height << 4));
	GSState__Set(SCE_GS_XYZ2,SCE_GS_SET_XYZ2(((u32X + width) << 4),((u32Y + height) << 4),0));

	// restore original states
	GSState__Set(SCE_GS_ZBUF_1, GSState.ZBUF_1);
	GSState__Set(SCE_GS_TEST_1, GSState.TEST_1);

	test2->Unselect();
	test1->UnlockForRender();

	test2->Unlock();
	test1->Unlock();

	DisplayResults(test1->CompareTo(test2));

	DestroyTestImage(test1);
	DestroyTestImage(test2);
}

void PlatformImage::TestUpDown(void)
{
	static const s32 sizes[] = { 16, 32, 64, 128, 256, -1 };
	static const s32 formats[] = { Image::TYPE_RGBA|Image::SIZE_32_BIT, Image::TYPE_RGBA|Image::SIZE_16_BIT, -1 };
	u32 failures = 0;

	dbgPrint("Testing PlatformImage Upload/Download...");

	const s32* fmt = &formats[0];
	while(*fmt != -1)
	{
		const s32* w = &sizes[0];
		while(*w != -1)
		{
			const s32* h = &sizes[0];
			while(*h != -1)
			{

				PlatformImage* test = CreateTestImage(*w,*h,*fmt);

				test->Lock();

				test->FreeMainRam();

				test->Unlock();

				// test data...
				if(!test->CheckTestImage())
				{
					// 					dbgPrint(TTY_RED "\n\t(%dx%dx%d) Failed\n" TTY_RESET, *w,*h, LookupBPP(*fmt));
					failures++;
				}

				DestroyTestImage(test);

				h++;
			}

			w++;
		}

		fmt++;
	}
	DisplayResults(failures == 0);
}
#endif
