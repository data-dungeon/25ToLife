
/*************************************/
/*                                   */
/*   TextureSet.cpp                  */
/*   Tyler Colbert                   */
/*   texture management              */
/*                                   */
/*************************************/

/* this module handles texture management for the engine. Texture files are sets of images, including size and depth info.
the texture set loading routine reads in the data and may upload it to video ram and get rid of the original data, or may
retain the original data for further texture management purposes. For example, the simplest OpenGL texture manager would
bind all the textures in the set and just keep a list of bind values for later reference.

*/

/******************* includes ****************************/

#include "Game/GamePCH.h"
#include "Display/TextureUtil.h"

#ifdef _XBOX
int g_xboxmotherfuckingpalettehint = 0;
#endif

#ifdef _DEBUG
#include "Layers/ttyColors.h"
#endif

#include "PlatformImage.h"

/// from ether
#ifdef MASSIVE_AD
Image::Image(u16 imageFlags, u16 imageFormat, u16 width, u16 height, void* data, u16 palFormat, u16 palWidth, u16 palHeight, void* palette, bool FromXPR, bool isMassive)
	: mLockCount(0),
	  mFlags(imageFlags)
#else
Image::Image(u16 imageFlags, u16 imageFormat, u16 width, u16 height, void* data, u16 palFormat, u16 palWidth, u16 palHeight, void* palette, bool FromXPR)
	: mLockCount(0),
	  mFlags(imageFlags)
#endif
{
	mFormat = imageFormat;
	mWidth = width;
	mHeight = height;

	u16 type = imageFormat & TYPE_MASK;

	if(type == TYPE_CI || type == TYPE_CI_IA)
	{
		mFlags |= HAS_PALETTE;

		if(palFormat == 0)
			palFormat = SIZE_32_BIT|TYPE_RGBA;

		if(palWidth == 0)
			palWidth = 16;

		if(palHeight == 0)
		{
			u16 depth = imageFormat & SIZE_MASK;
			if(depth == SIZE_8_BIT)
				palHeight = 16;
			else if(depth == SIZE_4_BIT)
				palHeight = 1;
			else
				ASSERT(false);
		}
	}
	mPaletteFormat = palFormat;
	mPaletteWidth = palWidth;
	mPaletteHeight = palHeight;
#ifdef MASSIVE_AD
	mPlatformImage = new PlatformImage(this, data, palette, FromXPR, isMassive);
#else
#ifdef _XBOX
	g_xboxmotherfuckingpalettehint = palWidth * palHeight;
#endif
	mPlatformImage = new PlatformImage(this, data, palette, FromXPR);
#endif
#if 0  //#ifdef _DEBUG
	if(!Texture::IsTesting())
		dbgPrint("%08X %c%c%c%c %2.2x (%3.3dx%3.3d)  %2.2x (%3.3dx%3.3d)\n",
					mPlatformImage,
					(mFlags & HAS_PALETTE) ? 'P' : '-',
					(mFlags & ALLOCATED) ? 'A' : '-',
					(mFlags & PERMANENT) ? 'P' : '-',
					(mFlags & UVREVERSED) ? 'R' : '-',
					mFormat,
					mWidth,
					mHeight,
					palFormat,
					palWidth,
					palHeight);
#endif
}

// create a new image and copy from one to the other
// pass in a pointer to an Image
bool Image::Clone ( Image** IP)
{
	void*	NewPalBuf = NULL;
	void* NewTexBuf = NULL;
	void* IB = GetImageBuffer();

	// get the palette and make a copy of it
	int	palwidth  = GetPaletteWidth();
	int	palheight = GetPaletteHeight();
	void*	PalBuf    = GetPaletteBuffer();
	int	PalSize   = (palwidth * palheight) * ( TextureUtil::GetBPP(this) / 8);
	
	int	TexSize   = GetSize();

	if ( (GetFormat() & TYPE_MASK) == TYPE_CI)
	{
		if (PalBuf)
		{
			NewPalBuf = new char[PalSize];

			// copy over the palette
			memcpy(NewPalBuf, PalBuf, PalSize);
		}
	}

	// create a new image
	(*IP) = new Image(0, GetFormat(), GetWidth(), GetHeight(),
								NULL, GetPaletteFormat(), 
								GetPaletteWidth(), GetPaletteHeight(), NewPalBuf, false);

	void*	test = (*IP)->GetImageBuffer();
	memcpy (test, IB, TexSize);
	(*IP)->ReleaseImageBuffer(true);

	ReleaseImageBuffer();
	ReleasePaletteBuffer();

//	(*IP)->SetFormat(GetFormat());
	if (!(*IP))
		return false;

   return	true;
}

void Image::SetMipmapDistance(float distance)
{
	mPlatformImage->SetMipmapDistance(distance);
}

void Image::AppendMipLevel(u16 format, u16 width, u16 height, void* data, u16 palFmt, u16 palWidth, u16 palHeight, void* palette)
{
	mPlatformImage->AppendMipLevel(format, width, height, data, palFmt, palWidth, palHeight, palette);
}

void Image::BitBlt(int x, int y, int w, int h)
{
	mPlatformImage->BitBlt(x,y,w,h);
}

void Image::BltGtoA(int x, int y, int w, int h)
{
	mPlatformImage->BltGtoA(x,y,w,h);
}


u8 Image::GetNumMipLevels(void)
{
	return mPlatformImage->GetNumMipLevels();
}

Image::~Image(void)
{
	delete mPlatformImage;
}

void Image::LockForRender(void)
{
	ASSERT(TextureMgr::GetRenderDestination() == NULL);
	TextureMgr::SetRenderDestination(this);
	mPlatformImage->LockForRender();
}

void Image::UnlockForRender(void)
{
	ASSERT(TextureMgr::GetRenderDestination() == this);
	TextureMgr::SetRenderDestination(NULL);
	mPlatformImage->UnlockForRender();
}

void Image::Lock(void)
{
	mLockCount++;
	if(mLockCount == 1)
	{
		mPlatformImage->Lock();
	}
	else
	{
		// we need to touch this texture in the LRU list.
#ifdef TRACK_TEXTURE_MEMORY
		TextureMgr::TouchUploadedImage(mPlatformImage);
#endif
	}
}

void Image::Unlock(void)
{
	if ( mLockCount > 0 )
	{
		mLockCount--;
		if(mLockCount == 0)
		{
			mPlatformImage->Unlock();
		}
	}
	else
	{
		ASSERTS( false, "unlocking an unlocked image" );
	}
}

bool Image::IsLocked(void)
{
	ASSERT(mLockCount == 0 || mPlatformImage->IsLocked() == true);
	ASSERT(mLockCount > 0 || mPlatformImage->IsLocked() == false);
	return mLockCount > 0;
}

/// prep this image for drawing operations
void Image::Select(void)
{
	mPlatformImage->Select();
}

/// unmark this image for drawing operations.
void Image::Unselect(void)
{
	mPlatformImage->Unselect();
}

u32 Image::GetSize(void)
{
	return mPlatformImage->GetSize();
}

/// call platform specific function to get a pointer to the image buffer
void* Image::GetImageBuffer(void)
{
	return mPlatformImage->GetImageBuffer();
}

#if defined (_XBOX)
/// call platform specific function to get a pointer to the image bufferin
int Image::GetImageFormat(void)
{
	return mPlatformImage->GetImageFormat();
}
#endif

/// call platform specific function to release the image buffer
void Image::ReleaseImageBuffer(bool modified)
{
	mPlatformImage->ReleaseImageBuffer(modified);
}
	
/// call platform specific function to get a pointer to the image buffer
void* Image::GetPaletteBuffer(void)
{
	return mPlatformImage->GetPaletteBuffer();
}

/// call platform specific function to release the image buffer
void Image::ReleasePaletteBuffer(bool modified)
{
	mPlatformImage->ReleasePaletteBuffer(modified);
}

void Image::Release()
{
	mPlatformImage->Release();
}

#ifdef _DEBUG
void Image::Test(void)
{
	TestDynamicPersistance();
	TestLockUnlockPairing();
}

void Image::TestDynamicPersistance(void)
{
	dbgPrint("Testing dynamic image persistance...");
	Image* img = new Image(0,
								  Image::SIZE_32_BIT|Image::TYPE_RGBA,
								  16,16,
								  NULL, false);
#ifdef DIRECTX
	img->mPlatformImage->EndMIPLevels( true );
#endif

	u32 tempbuff[16*16];
	memset(tempbuff,0xAE, sizeof(tempbuff));
	void* t = img->GetImageBuffer();
	ASSERT_PTR(t);
	memcpy(t, tempbuff, sizeof(tempbuff));
	img->ReleaseImageBuffer(true);
	t = img->GetImageBuffer();
	bool success = memcmp(t, tempbuff, sizeof(tempbuff)) == 0;
	img->ReleaseImageBuffer(true);
	delete img;
	
	if(success)
	{
		dbgPrint(TTY_GREEN "Success\n" TTY_RESET);
	}
	else
	{
		dbgPrint(TTY_RED "Failure\n" TTY_RESET);
	}
}

void Image::TestLockUnlockPairing(void)
{
	dbgPrint("Testing image lock/unlock pairing...");
	u32 tempbuff[16*16];
	memset(tempbuff,0xAE, sizeof(tempbuff));
	Image* img = new Image(0,
								  Image::SIZE_32_BIT|Image::TYPE_RGBA,
								  16,16,
								  &tempbuff[0], false);

	bool success = img->InternalTestLockUnlockPairing();

	if(success)
		dbgPrint(TTY_GREEN "Success\n" TTY_RESET);
	else
		dbgPrint(TTY_RED "Failure\n" TTY_RESET);

	delete img;
}

bool Image::InternalTestLockUnlockPairing(void)
{
	if(mLockCount != 0 || (GetPlatformImage()->IsLocked() == true))
		return false;
	
	Lock();
	if(mLockCount != 1 || (GetPlatformImage()->IsLocked() == false))
		return false;

	Lock();
	if(mLockCount != 2 || (GetPlatformImage()->IsLocked() == false))
		return false;

	Lock();
	if(mLockCount != 3 || (GetPlatformImage()->IsLocked() == false))
		return false;

	Lock();
	if(mLockCount != 4 || (GetPlatformImage()->IsLocked() == false))
		return false;

	Unlock();
	if(mLockCount != 3 || (GetPlatformImage()->IsLocked() == false))
		return false;

	Unlock();
	if(mLockCount != 2 || (GetPlatformImage()->IsLocked() == false))
		return false;

	Lock();
	if(mLockCount != 3 || (GetPlatformImage()->IsLocked() == false))
		return false;

	Unlock();
	if(mLockCount != 2 || (GetPlatformImage()->IsLocked() == false))
		return false;

	Unlock();
	if(mLockCount != 1 || (GetPlatformImage()->IsLocked() == false))
		return false;

	Unlock();
	if(mLockCount != 0 || (GetPlatformImage()->IsLocked() == true))
		return false;

	return true;
}
#endif

int Image::AllocatedSize()
{
	int imageSize = mWidth * mHeight;
	switch (mFlags & SIZE_MASK)
	{
		case SIZE_4_BIT:	imageSize /= 2;	break;
		case SIZE_8_BIT:	break;
		case SIZE_16_BIT:	imageSize *= 2;	break;
		case SIZE_24_BIT:	imageSize *= 3;	break;
		case SIZE_32_BIT: imageSize *= 4;	break;
		default:
			ASSERT( false );
			break;
	}

	int size = 0;
	size += sizeof( *this );
	size += imageSize;

	return size;
}