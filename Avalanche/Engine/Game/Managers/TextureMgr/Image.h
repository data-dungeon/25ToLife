
#ifndef __IMAGE_H__
#define __IMAGE_H__

class PlatformImage;

#define TEXTURE_BIND_WHITE_TEXTURE	0x7FFF
#define TEXTURE_BIND_BACK_BUFFER		0x7FFE
#define TEXTURE_BIND_FRONT_BUFFER	0x7FFD
#define TEXTURE_BIND_Z_BUFFER			0x7FFC

class Image : public ReferenceCountable<u16>
{
protected:

	/// This contains some platform specific information.  For example,
	/// the current image, if in main memory should be contained here.
	/// The pointers in texture memory if its there, etc...
	PlatformImage* mPlatformImage;

	/// flags
	u16 mFlags;

	/// Format
	u16 mFormat;

	/// width of this image
	u16 mWidth;

	/// height of this image
	u16 mHeight;

	/// Palette format
	u16 mPaletteFormat;
	u16 mPaletteWidth;
	u16 mPaletteHeight;

	/// lock count.
	u8  mLockCount;

#ifdef _DEBUG
public:
	static void Test(void);
protected:
	static void TestDynamicPersistance(void);
	static void TestLockUnlockPairing(void);
	bool InternalTestLockUnlockPairing(void);
#endif

public:

	/// image flags
	enum {
		ALLOCATED			= 0x0001,
		HAS_PALETTE			= 0x0002,
		UVREVERSED			= 0x0004,
		PERMANENT			= 0x0008,

		AUTO_MIPMAP			= 0x0010,
		BACK_BUFFER			= 0x0020,
		FRONT_BUFFER		= 0x0040,
		SHADOW				= 0x0080,
		Z_BUFFER				= 0x0100,
		RENDER_TARGET		= 0x0200,
#ifdef DIRECTX_PC
		DEPTH_BUFFER		= 0x0400,
#endif

		// This means texture only exists in texture memory on
		// the PS2, or on a video card.  (It is LOCAL to the graphics
		// processor).  Used for front/back/z buffers, and for
		// textures created mid frame for use within that frame
		// It causes CPU side buffers to be created & released 
		// automatically when you call GetImageBuffer()
		LOCAL				= 0x8000,
	};
	

	/// Image formats (combine one value from each category)
	enum {
		SIZE_MASK		= 0x000F,
		SIZE_4_BIT		= 0x0000,
		SIZE_8_BIT		= 0x0001,
		SIZE_16_BIT		= 0x0002,
		SIZE_24_BIT		= 0x0003,
		SIZE_32_BIT		= 0x0004,
		SIZE_S3TC		= 0x0005,

		TYPE_MASK		= 0x00F0,
		TYPE_RGB			= 0x0000,
		TYPE_CI			= 0x0010,
		TYPE_IA			= 0x0020,
		TYPE_RGBA		= 0x0030,
		TYPE_LUMINANCE	= 0x0040,
		TYPE_CI_IA		= 0x0050,
	};

	/// from ether
#ifdef MASSIVE_AD
	Image(u16 imageFlags, 
			u16 imageFormat, u16 width, u16 height, void* data, 
			u16 palFormat = 0, u16 palWidth = 0, u16 palHeight = 0, void* palette = NULL, bool FromXPR = false, bool isMassive = false);
#else
	Image(u16 imageFlags, 
			u16 imageFormat, u16 width, u16 height, void* data, 
			u16 palFormat = 0, u16 palWidth = 0, u16 palHeight = 0, void* palette = NULL, bool FromXPR = false);
#endif
	~Image(void);
	
	u16 GetWidth(void)  { return mWidth; }
	u16 GetHeight(void) { return mHeight; }
	u32 GetSize(void);
	u16 GetFormat(void) { return mFormat; }
	u16 GetFlags(void)  { return mFlags; }
	u16 GetPaletteWidth(void)  { return mPaletteWidth; }
	u16 GetPaletteHeight(void) { return mPaletteHeight; }
	u16 GetPaletteFormat(void) { return mPaletteFormat; }
	u8  GetNumMipLevels(void);
#ifdef DIRECTX_PC
	int	  realImageHeight;
	int	  realImageWidth;
	int	  GetRealImageHeight() {return realImageHeight;}
	int   GetRealImageWidth()  {return realImageWidth;}
	float imageUScale;
	float imageVScale;
	float GetImageUScale() {return imageUScale;}
	float GetImageVScale() {return imageVScale;}
	void  SetImageUScale(float UScale) {imageUScale = UScale;}
	void  SetImageVScale(float VScale) {imageVScale = VScale;}
#endif

	void	SetFormat(u16 Format) { mFormat = Format; }
	void  SetPaletteWidth(int width) { mPaletteWidth = width; }
	void  SetPaletteHeight(int height) { mPaletteHeight = height; }
	void  SetPaletteFormat(int format) { mPaletteFormat = format; }
	void	SetFlags(u16 Flags) { mFlags = Flags; }
	bool	Clone (Image** IP);

	/// call platform specific function to get a pointer to the image buffer
	void* GetImageBuffer(void);

	#if defined (_XBOX)
		/// call platform specific function to get a pointer to the image buffer
	int GetImageFormat(void);
	#endif

	/// call platform specific function to release the image buffer
	void ReleaseImageBuffer(bool modified = false);

	void* GetPaletteBuffer(void);
	void ReleasePaletteBuffer(bool modified = false);

	void Release();

	/// make this image available for rendering.  Locks can be nested.
	void Lock(void) NAMED_SECTION("text.rend1");

	void UnlockForRender(void);

	void LockForRender(void);

	/// we are done rendering with this image (but that doesnt mean we
	/// wont do it again soon... )
	void Unlock(void) NAMED_SECTION("text.rend1");

	/// ar we locked
	bool IsLocked(void);

	/// prep this image for drawing operations
	void Select(void);
	/// unmark this image for drawing operations.
	void Unselect(void);

	/// copy this image into the destination
	/// image must be selected already.
	void BitBlt(int x, int y, int w, int h);

	/// copy this image G channel into the destination A channel
	/// image must be selected already.
	void BltGtoA(int x, int y, int w, int h);

	/// get the platform image... USE SPARINGLY!
	PlatformImage* GetPlatformImage(void) { return mPlatformImage; }

	/// record when the user wants the first mipmap change to occur.
	void SetMipmapDistance(float distance);

	/// Add a mip level to this image.
	void AppendMipLevel(u16 format, u16 width, u16 height, void* data, u16 palFmt, u16 palWidth, u16 palHeight, void* palette);

	/// Get RAM usage
	int AllocatedSize();
};

#endif // __IMAGE_H__
