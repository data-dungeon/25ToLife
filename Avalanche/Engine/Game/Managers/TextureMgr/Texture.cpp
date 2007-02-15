
/*************************************/
/*                                   */
/*   Texture.cpp                     */
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

#if defined (DIRECTX) && !defined (_XBOX)
#include "Game/Managers/TextureMgr/DirectX/PlatformImage.h"
#endif

#if defined (DIRECTX) && defined (_XBOX)
#include "Game/Managers/TextureMgr/XBox/PlatformImage.h"
#endif

#ifdef _DEBUG
bool Texture::sIsTesting = false;
#endif
Texture* Texture::sCurrentlySelected = NULL;


/// construct an empty texture
Texture::Texture(TextureDatabase* owner, const char* name)
	: mOwner(owner), 
	  mImages(1,1), 
	  mFlags(0), 
	  mImageNumber(0),
	  mUserData(NULL)
{
	strncpy(mName, name, TEXTURE_NAME_LENGTH);
	mName[TEXTURE_NAME_LENGTH-1] = '\0';
}

/// convert dbl flags into image format
u16 Texture::DblFlagsToFormat(u32 flags)
{
	u16 format = 0;
	switch((u16)flags & TEX_FLAG_SIZE_MASK)
	{
	case TEX_FLAG_SIZE_4_BIT:  format |= Image::SIZE_4_BIT;	break;
	case TEX_FLAG_SIZE_8_BIT:  format |= Image::SIZE_8_BIT;	break;
	case TEX_FLAG_SIZE_16_BIT: format |= Image::SIZE_16_BIT;	break;
	case TEX_FLAG_SIZE_24_BIT: format |= Image::SIZE_24_BIT;	break;
	case TEX_FLAG_SIZE_32_BIT: format |= Image::SIZE_32_BIT;	break;
	case TEX_FLAG_SIZE_S3TC:   format |= Image::SIZE_S3TC;	break;
	default: ASSERT(false);
	}
	switch((u16)flags & TEX_FLAG_FMT_MASK)
	{
	case TEX_FLAG_FMT_RGB:  		format |= Image::TYPE_RGB;			break;
	case TEX_FLAG_FMT_CI:  			format |= Image::TYPE_CI;			break;
	case TEX_FLAG_FMT_IA: 			format |= Image::TYPE_IA;			break;
	case TEX_FLAG_FMT_RGBA: 		format |= Image::TYPE_RGBA;		break;
	case TEX_FLAG_FMT_LUMINANCE: 	format |= Image::TYPE_LUMINANCE;	break;
	case TEX_FLAG_FMT_CI_IA:   	format |= Image::TYPE_CI_IA;		break;
	default: ASSERT(false);
	}
	return format;
}

/// construct a texture from a dbl
u32 Texture::AddImageFromDBL(ts_TextureChunkRecord* record)
{
#ifdef PS2
	char			reflectName[] = "reflect.tga";
	char			envName[] = "env????.tga";
	char			refName[] = "ref?.tga";

	unsigned int testlen = strlen( reflectName );
	char * ptestptr = record->acTextureName + strlen(record->acTextureName) - testlen;

	if (strncmp( ptestptr, reflectName, testlen ) == 0 )
	{
		mFlags |= ENV_MAP;
//		mFlags |= MAP_SPHERICAL;
		mFlags |= ENVMAP_SPACE_VIEW << 8;

		mFlags |= 0 << 16;
		mFlags |= 2 << 12;
	}

	testlen = strlen( envName );
	ptestptr = record->acTextureName + strlen(record->acTextureName) - strlen( envName );
	if( !(mFlags & ENV_MAP)  &&  (strncmp( ptestptr, envName, 3 ) == 0) )
	{
		mFlags |= ENV_MAP;

		if( (ptestptr[3] == 's')  ||  (ptestptr[3] == 'S') )
			mFlags |= MAP_SPHERICAL;

		switch( ptestptr[4] )
		{
			default:
			case 'c':
			case 'C':
				mFlags |= 0 << 8;
				break;
			case 'v':
			case 'V':
				mFlags |= 1 << 8;
				break;
			case 'w':
			case 'W':
				mFlags |= 2 << 8;
				break;
		}

		switch( ptestptr[5] )
		{
		default:
		case 'x':
		case 'X':
			mFlags |= 0 << 16;
			break;
		case 'y':
		case 'Y':
			mFlags |= 1 << 16;
			break;
		case 'z':
		case 'Z':
			mFlags |= 2 << 16;
			break;
		}

		switch( ptestptr[6] )
		{
		default:
		case 'x':
		case 'X':
			mFlags |= 0 << 12;
			break;
		case 'y':
		case 'Y':
			mFlags |= 1 << 12;
			break;
		case 'z':
		case 'Z':
			mFlags |= 2 << 12;
			break;
		}
	}

	testlen = strlen( refName );
	ptestptr = record->acTextureName + strlen(record->acTextureName) - strlen( refName );
	if( !(mFlags & ENV_MAP)  &&  (strncmp( ptestptr, refName, 3 ) == 0) )
	{
		mFlags |= ENV_MAP | REF_MAP;

		if( (ptestptr[3] == 's')  ||  (ptestptr[3] == 'S') )
			mFlags |= MAP_SPHERICAL;
	}

#endif

	if (record->u32Flags & TEX_FLAG_REFLECTS)
		mFlags |= REFLECTS;
	if (record->u32Flags & TEX_FLAG_ANIMATED_TEXTURE)
		mFlags |= ANIMATED;

	u16 flags = 0;

	if (record->u32Flags & TEX_FLAG_UVREVERSED)
		flags |= Image::UVREVERSED;

	void* data    = (record->u32TextureOffset) ? (void*)((u32)record + record->u32TextureOffset) : NULL;
	void* palette = (record->u32PaletteOffset) ? (void*)((u32)record + record->u32PaletteOffset) : NULL;
	if ( data == NULL && palette == NULL )
	{
		// animated textures just have a stub for data in the main texture set,
		// the real data is stitched in later.
		ASSERT( mFlags & ANIMATED );
		return 0;
	}
	u32 idx = AddImage(false, flags, DblFlagsToFormat(record->u32Flags), record->s16TextureWidth, record->s16TextureHeight, data, 
							 Image::SIZE_32_BIT|Image::TYPE_RGBA, record->s16PaletteWidth, record->s16PaletteHeight, palette);

	//	internalizeImage( m_Owner->GetFormat( ), _Data, _Palette, pNewImage, pNewPalette );

	mImages[idx]->SetMipmapDistance((float)record->u16MipmapDistance);

	record = record->pNext;
	while(record)
	{
		data    = (record->u32TextureOffset) ? (void*)((u32)record + record->u32TextureOffset) : NULL;
		palette = (record->u32PaletteOffset) ? (void*)((u32)record + record->u32PaletteOffset) : NULL;
		AppendImageMipLevel(idx, DblFlagsToFormat(record->u32Flags), record->s16TextureWidth, record->s16TextureHeight, data,
								  Image::SIZE_32_BIT|Image::TYPE_RGBA, record->s16PaletteWidth, record->s16PaletteHeight, palette);
		record = record->pNext;
	}

#if defined (DIRECTX)
	PlatformImage * pPlatformImage = mImages[idx]->GetPlatformImage( );
	ASSERT( pPlatformImage );

	// PAB not best way to do this, but for now, you need to tell DX that you want dynamic before DBL is loaded
	// then reset it again after you are done
	pPlatformImage->EndMIPLevels( mFlags & ANIMATED );
#endif

	return idx;
}

#if defined (_XBOX)
void Texture::checkEnvMapping( const char * pTextureName )
{																														
	char			reflectName[] = "reflect.tga";
	char			envName[] = "env????.tga";
	char			refName[] = "ref?.tga";

	unsigned int testlen = strlen( reflectName );
	char * ptestptr = (char *) pTextureName + strlen( pTextureName ) - testlen;

	if (strncmp( ptestptr, reflectName, testlen ) == 0 )
	{
		mFlags |= ENV_MAP;
		return;
	}

	testlen = strlen( envName );
	ptestptr = (char *) pTextureName + strlen( pTextureName ) - strlen( envName );
	if( !(mFlags & ENV_MAP)  &&  (strncmp( ptestptr, envName, 3 ) == 0) )
	{
		mFlags |= ENV_MAP;
		return;
	}

	testlen = strlen( refName );
	ptestptr = (char *) pTextureName + strlen( pTextureName ) - strlen( refName );
	if( !(mFlags & ENV_MAP)  &&  (strncmp( ptestptr, refName, 3 ) == 0) )
	{
		mFlags |= ENV_MAP;
		return;
	}
}

/// construct a texture from an XBOX XPR file
u32 Texture::AddImageFromXPR(ts_TextureChunkRecord* record, CXBPackedResource *rsc, int index)
{
	checkEnvMapping( record->acTextureName );

	if (record->u32Flags & TEX_FLAG_REFLECTS)
		mFlags |= REFLECTS;
	if (record->u32Flags & TEX_FLAG_ANIMATED_TEXTURE)
		mFlags |= ANIMATED;

	u16 flags = 0;

	if (record->u32Flags & TEX_FLAG_UVREVERSED)
		flags |= Image::UVREVERSED;

	// on XBOX - the pointer is defined as 20UL * index...
	LPDIRECT3DTEXTURE8 data = rsc->GetTexture( 20UL * index );

	void* palette = NULL;
	if(record->u32PaletteOffset)
		palette = (void*)((u32)record + record->u32PaletteOffset);

	u32 idx = AddImage(true, flags, DblFlagsToFormat(record->u32Flags), record->s16TextureWidth, record->s16TextureHeight, data, 
							 Image::SIZE_32_BIT|Image::TYPE_RGBA, record->s16PaletteWidth, record->s16PaletteHeight, palette);

	mImages[idx]->SetMipmapDistance((float)record->u16MipmapDistance);

	// mips are handled inherently by XPR now....
	return idx;
}
#endif

/// destroy it
Texture::~Texture()
{
	// Hello?
	ASSERT(sCurrentlySelected != this);
	if (mFlags & SELECTED)
		Unselect();

	// remove from database...
	if(mOwner)
		mOwner->RemoveTexture(this);
	
	// remove its images
	RemoveAllImages();
}

/// purge all iamges...
void Texture::RemoveAllImages(void)
{
	for(int i = 0; i < mImages.Count(); i++)
	{
		delete mImages[i];
	}
	mImages.Clear();
}

/// remove a single image
void Texture::RemoveImage(u32 index)
{
	delete mImages[index];
	mImages.Delete(index);
}

/// add a new mip level to this image in the array (appends level)
void Texture::AppendImageMipLevel(u32 imageidx, u16 format, u16 width, u16 height, void* data, u16 palFmt /*= 0*/, u16 palWidth /*= 0*/, u16 palHeight /*= 0*/, void* palette /*= NULL*/)
{
	mImages[imageidx]->AppendMipLevel(format, width, height, data, palFmt, palWidth, palHeight, palette);
}


/// add a new image into the array
u32 Texture::AddImage(bool FromXPR, u16 flags, u16 format, u16 width, u16 height, void* data, u16 palFmt, u16 palWidth, u16 palHeight, void* palette /* = NULL*/)
{
#if 0 //#ifdef _DEBUG
	if(!IsTesting())
		dbgPrint("%*.*s %d : ",
					TEXTURE_NAME_LENGTH,
					TEXTURE_NAME_LENGTH,
					mName,
					mImages.Count());
#endif

	mImages.Add(new Image(flags, format, width, height, data, 
								 palFmt, palWidth, palHeight, palette, FromXPR));

	u32 index = mImages.Count()-1;

	// adjust gamma...
//	TextureUtil::GammaAdjustImage( *mImages.Get(index), 0.2f);


// 	if(strstr(mName, "1meter"))
// 	{
// 		int w,h;
// 		w = width >> 1;
// 		h = height >> 1;
// 		for(int i = 0; i < 3; i++)
// 		{
// 			char *buffer = new char[w*h];
// 			int color = i;
// 			memset(buffer,color,w*h);
// 			mImages[index]->AppendMipLevel(format, w, h, buffer, palFmt, palWidth, palHeight, palette);
// 			w >>= 1;
// 			h >>= 1;
// 		}
// 	}

	return index;
}


#ifdef MASSIVE_AD
/// add a new image into the array
u32 Texture::AddPlatformImage(bool FromXPR, u16 flags, u16 format, u16 width, u16 height, void* data, u16 palFmt, u16 palWidth, u16 palHeight, void* palette /* = NULL*/)
{

	mImages.Add(new Image(flags, format, width, height, data, 
								 palFmt, palWidth, palHeight, palette, FromXPR, true)); //a bit of a hack to ensure we can get 24-bit textures into the 32-bit texture space of the game

	u32 index = mImages.Count()-1;

	PlatformImage * pPlatformImage = mImages[index]->GetPlatformImage( );
	ASSERT( pPlatformImage );

	// PAB not best way to do this, but for now, you need to tell DX that you want dynamic before DBL is loaded
	// then reset it again after you are done
	pPlatformImage->EndMIPLevels( mFlags & ANIMATED );

	return index;
}
#endif

void Texture::Lock(void)
{
	// for now just fall to the Image to do the work.
	GetImage()->Lock();
}


void Texture::Unlock(void)
{
	// for now just fall to the Image to do the work.
	GetImage()->Unlock();
}

void Texture::LockForRender(void)
{
	GetImage()->LockForRender();
}

void Texture::UnlockForRender(void)
{
	GetImage()->UnlockForRender();
}

void Texture::Select(void)
{
	ASSERT(!(mFlags & SELECTED));
	if(sCurrentlySelected == NULL)
	{
		sCurrentlySelected = this;
		GetImage()->Select();
	}
	mFlags |= SELECTED;
}

void Texture::Unselect(void)
{
	ASSERT(mFlags & SELECTED);
	if(sCurrentlySelected == this)
	{
		sCurrentlySelected = NULL;
		GetImage()->Unselect();
	}
	mFlags &= ~SELECTED;
}

/// Change the flags for this texture bind
void Texture::SetFlags(u32 flags)
{
	mFlags |= flags;
}

u32 Texture::GetFlags(void)
{
	return mFlags;
}

void Texture::SetCurrentFrame(u16 frame)
{
	ASSERT(frame < GetImageCount());
//	ASSERT(frame >= 0);

	if (mImageNumber != frame)
	{
		ASSERT(mImages[mImageNumber]->IsLocked() == false);
		ASSERT(mImages[frame]->IsLocked() == false);
		mImageNumber = frame;
	}
}

u16 Texture::GetImageCount(void)
{
	return mImages.Count();
}

void Texture::SetCurrentFrameByPercentage(float percent, bool firstWholeCount, bool lastWholeCount)
{
	if(Math::Zero(percent))
	{
		SetCurrentFrame(0);
		return;
	}

	int count = GetImageCount()-1;

	if(Math::Equal(percent, 1.0f))
	{
		SetCurrentFrame(count-1);
	}


	if(!firstWholeCount)
		count--;
	if(!lastWholeCount)
		count--;

	int img = (int)(percent * count);

	// TJC - safety net
	if(img == count)
		img = count-1;

	if(!firstWholeCount)
		img++;

	ASSERT(lastWholeCount || (img != (GetImageCount()-1)));

	SetCurrentFrame(img);
}


#ifdef _DEBUG
void Texture::VerifyThatNoImagesAreLocked(void)
{	
#if 0 // This is fundamentally broken
	int count = GetImageCount();
	for(int i = 0; i < count; i++)
	{
		ASSERT(mImages[i]->IsLocked() == false);
	}
#endif
}
#endif

int Texture::AllocatedSize()
{
	int size = 0;
	size += sizeof(*this);
	for ( int i = 0 ; i < GetImageCount() ; i++ )
	{
		size += sizeof(Image*);
		size += mImages[i]->AllocatedSize();
	}
	return size;
}

void Texture::BitBlt(int x, int y, int w, int h)
{
	GetImage()->BitBlt(x,y,w,h);
}

void Texture::BltGtoA(int x, int y, int w, int h)
{
	GetImage()->BltGtoA(x,y,w,h);
}

