
#ifndef __PLATFORM_IMAGE_H__
#define __PLATFORM_IMAGE_H__

#include "Display/PS2/GSMemManager.h"
#include "Display/PS2/Renderer/Code/PS2MaterialInfo.h"
#include "container/Darray.h"
class Image;

#define MAX_MIP_LEVELS 7

class PlatformImage
{
protected:
	Image* mOwner;

	void* mCPUSideImageData[MAX_MIP_LEVELS];
	void* mCPUSidePaletteData;

	u16 mTextureAddr[MAX_MIP_LEVELS]; // set to -1 if not in texmem...
	u16 mPaletteAddr; // set to -1 if not in texmem...

	u8 mWidthPow2;
	u8 mHeightPow2;
	u16 mTexBufWidth[MAX_MIP_LEVELS];

	u16 mGSFormat;
	u16 mPaletteGSFormat;

	u32 mSize;

	u16 mFlags;

	u8 mMipLevels; // TJC -  number of mip levels we should have (including base)
	u8 mMaxMipLevels;

	s16 mK;
	u8 mL;

	// this is used to cache DisplayState when rendering 
	// to a texture so that we can restore it later.
	// only 1 texture should be loaded for render at a time
	// but right now there is no assert for it.
	static ts_DisplayState sDisplayStateCache;

	//
	enum {
//		TEXTURE_TEMPORARY_IMAGE		= 0x0001,
//		PALETTE_TEMPORARY_IMAGE		= 0x0002,
		TEXTURE_CPU_QUEUE_DMA      = 0x0004,
	
		PALETTE_LOCK_OUTSTANDING	= 0x0100,
		PALETTE_BUFFER_FETCHED		= 0x0200,
		PALETTE_CPU_SIDE_INVALID	= 0x0400,
		PALETTE_OWNS_CPU_BUFFER		= 0x0800,

		TEXTURE_LOCK_OUTSTANDING	= 0x1000,
		TEXTURE_BUFFER_FETCHED		= 0x2000,
		TEXTURE_CPU_SIDE_INVALID	= 0x4000,
		TEXTURE_OWNS_CPU_BUFFER		= 0x8000,
	};

	/// do a dma to vram
	static void DMAToVRAM(void* pImage, u16 width, u16 height, u16 format, u16 bpp, u16 texAddr, u32 texBufWidth) NAMED_SECTION("text.rend1");

	/// generate the mip levels for this map
	void GenerateMipLevel(u8 level);

	/// actually upload the image to vram
	void UploadImage(void) NAMED_SECTION("text.rend1");

	/// fetch the image from vram
	void DownloadImage(void);

	/// handle queued downloads from vram to cpu ram
	void QueueDownloadImage();
	static void QueuedDownloadImageCallback();
	void DownloadImageInterrupt( u16 textureAddr );

	void AllocCPUImageBuffer(void);

	// Palette funcs
	void UploadPalette(void);
	void DownloadPalette(void);
	void AllocCPUPaletteBuffer(void);

	u64 GetTex0_1(u8 tfx = 1) NAMED_SECTION("text.rend1");

	void FillStaticRegistersUsingTextureState(void);
	void FillStaticRegisters(u8 filter, u8 blend, u8 method, u8 wrapu, u8 wrapv) NAMED_SECTION("text.rend1");

	u16 GetBPP() { return mBPP; }

	static u16 GetPS2Format(u32 flags);
	static u16 LookupBPP(u16 gsFormat);

	u16 AllocGSMem(u16 numBlocks) NAMED_SECTION("text.rend1");

	// texture memory pool management
	// some constants
	enum {
		NUM_GS_REGIONS = 2,
		GS_BLOCKS_PER_PAGE = 32,
		GS_MEM_TOP = 0x4000,
	};
	u8 mRegion;
	u16 mContiguousBlocks;
	u16 mBPP;
	u16 mMipBlocks[MAX_MIP_LEVELS];
	void AddUploadedImage(void) NAMED_SECTION("text.rend1");
	void RemoveUploadedImage(void);
	void ChangeRegions(void);
	static int sRegionStart[NUM_GS_REGIONS];
	static int sRegionEnd[NUM_GS_REGIONS];
	static int sCurrentRegion;
	static int sCurrentTexMemPtr;
	static int sRegionSize;
	static DoublyLinkedList<PlatformImage> sImagesInRegion[NUM_GS_REGIONS];
	static void CalculateRegions(void);
	static void FlushRegion(int region);
	static u16 BlockSize(u16 format, u16 width, u16 height);

#ifdef _DEBUG
public:
	static void Test(void);
	static void DisplayResults(bool success);
protected:
	void* testImgData;
	static PlatformImage* CreateTestImage(u16 width, u16 height, u16 fmt);
	static void DestroyTestImage(PlatformImage* dumpme);
	bool CompareTo(PlatformImage* rhs);
	bool IsWhite(void);
	bool CheckTestImage(void);

	static void TestThatTestImagesVary(void);
	static void TestSelect(void);
	static void TestLockForRender(void);
	static void TestUpDown(void);
	static void TestConstantAddress(void);
#endif

public:
	// TJC - texture manager keeps us in a list when we are in vram
	PlatformImage* next;
	PlatformImage* prev;

	static void Init(void);
	static void Cleanup(void);
	static u_int RendererBindCallback(u_int textureID, u_int textureLayer);
	static void* RendererDataCallback(u_int bind, bool includeAlpha1, PS2MaterialInfo *pMatInfo) NAMED_SECTION("text.rend1");

	static void PausingTextureManager(void);
	static void ResumingTextureManager(void);

	/// render this image into the xywh rectangle.
	/// this image is already selected.  Might be 
	/// writing to frame buffer or to image lockedforrender.
	void BitBlt(int x, int y, int w, int h);

	/// render this images G channel into A channel of
	/// the xywh rectangle.
	/// this image is already selected. Might be
	/// writing to the frame buffer or to image lockedforrender.
	void BltGtoA(int x, int y, int w, int h);
	static void ZapAlpha(int x, int y, int w, int h);

	static void FlushTextureMemory(void);

	int  VirtualWidth()  { return 1 << mWidthPow2;  }
	int  VirtualHeight() { return 1 << mHeightPow2; }

	struct QueuedTransfer
	{
		PlatformImage*	image;
		u16				mTextureAddr; // set to -1 if not in texmem...
	};
	static DynamicArray<QueuedTransfer>	sQueuedTransfers;

	/// create the platform specific info given the inputs to the contructor
	PlatformImage(Image* owner, void* data,
					  void* palette = NULL, bool FromXPR = false);
	~PlatformImage();

	/// record when the user wants the first mipmap change to occur.
	void SetMipmapDistance(float fDistance);

	/// append a new mip level to this image
	void AppendMipLevel(u16 format, u16 width, u16 height, void* data, u16 palFmt, u16 palWidth, u16 palHeight, void* palette);
	u8 GetNumMipLevels(void) { return mMipLevels; }

	bool IsLocked(void) const { return (mFlags & TEXTURE_LOCK_OUTSTANDING) != 0; }

	// TJC - profiling
	static u32 GetContextChangeCount(void);
	static void ClearContextChangeCount(void);

	/// platform specific lock routine.
	void Lock(void) NAMED_SECTION("text.rend1");

	/// platform specific unlock routine.
	void Unlock(void) NAMED_SECTION("text.rend1");

	void LockForRender(void);
	void UnlockForRender(void);

	/// prep this image for drawing operations
	void Select(void);
	/// unmark this image for drawing operations.
	void Unselect(void);

	/// release..  This is a PS2 specific function to actually release
	/// the texture memory allocated by this image.
	void Release(void);

	/// release the memory used in main memory.  If this image is not
	/// in texture memory, its data is lost to the ether.
	void FreeMainRam(void);

	/// platform specific function to get a pointer to the image buffer
	void* GetImageBuffer(void);

	/// platform specific function to release the image buffer
	void ReleaseImageBuffer(bool modified = false);

	/// platform specific function to get a pointer to the palette buffer
	void* GetPaletteBuffer(void);

	/// platform specific function to release the palette buffer
	void ReleasePaletteBuffer(bool modified = false);

	u32 GetSize(void) { return mSize; }

	/// PS2 specific function for retrieving register values for this
	/// image.  Image must be locked.  Once image is unlocked, register
	/// values returned become invalid.
	void GetRegisterValues(u64 *pTex0_1 = NULL, u64 *pTex1_1 = NULL, u64 *pClamp_1 = NULL, u64 *pMiptbp1_1 = NULL, u64 *pMiptbp2_1 = NULL);

};

#endif // __PLATFORM_IMAGE_H__
