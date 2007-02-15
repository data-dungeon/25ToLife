
/*************************************/
/*                                   */
/*   TextureDatabase.cpp             */
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

#if defined (_XBOX)
#include "platform/Xbox/xbresource.h"
#include "EngineHelper/pathfile.h"
#include "platform/Xbox/dxdmanager.h"
#endif

TextureDatabase::TextureDatabase(const char* name)
	: mFlags(0), 
	  mPieceCount(0), 
	  mPieces(NULL)
{
	strncpy(mName, name, TEXTURE_SET_NAME_LENGTH);
	mName[TEXTURE_SET_NAME_LENGTH-1] = '\0';
}

TextureDatabase::~TextureDatabase()
{
	// Technically speaking, this should not be allowed, however, Juju
	// does it.  ICK!
//  	ASSERTS((mFlags & PERMENANT) == 0, "Attempt to free permanent texture database");

	for ( int i = mTextures.Count() - 1 ; i >= 0 ; i-- )
	{
		delete mTextures[ i ];
		mTextures.Delete( i );
	}

	#if defined (_XBOX)
//	m_xprResources.Destroy();
	#endif
}

void TextureDatabase::ImportDBL(const char *Filename,  void* Dx, ts_TextureChunkHeader *header, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize)
{
	bool	DibbleHasXPR = true;

	ASSERT(mPieceCount == 0);
	ASSERT_PTR(header);

	mFlags = header->u32Flags;
	mPieceCount = header->u32PieceCount;
	mPieces = (ts_PieceRecord*)(((u32)header) + header->u32PieceOffset);

	ASSERT(header->u32TextureCount != 0);

	#if defined( GAMMA_CORRECTION_ON_LOAD )
	DynamicArray<void*> GammaCorrectedPalettes;
	#endif

	// XBOX ONLY
	// if we are using Xbox, then load in the .XPR texture resource file now
	#if defined (_XBOX)

#if 0
	char	NewPath[256];
	char	NewFname[256];
	strcpy (NewFname, Filename);
	int	len = strlen(NewFname);
	strcpy ((char*)&NewFname[len-3], "xpr");
//	sprintf (NewPath, "d:\\data_xbox\\%s", NewFname);

	sprintf (NewPath, "%s", NewFname);
	if (FAILED(m_xprResources.Create( NewPath, 1, NULL )))
	{
		ASSERTF(false, ("Failed to load resource file: %s", Filename));
		DibbleHasXPR = false;
	}
#endif
	CDXD_Data*	DxD;
	DxD = (CDXD_Data*) Dx;
	if (DxD == NULL)
		DibbleHasXPR = false;

	if (DxD != NULL)
	{
		if (!DxD->m_XPRresourceCreated)
			DibbleHasXPR = false;

		ASSERTS ( DxD->m_xprResources.m_numTextures == header->u32TextureCount, "XPR resource and DBL have mismatched texture count");
		if (DxD->m_xprResources.m_numTextures != header->u32TextureCount)
			return;
	}
	#endif

	// check that the number of textures in resource matches the DBL
	#if defined (_XBOX)
//	ASSERTS ( m_xprResources.m_numTextures == header->u32TextureCount, "XPR resource and DBL have mismatched texture count");
//	if (m_xprResources.m_numTextures != header->u32TextureCount)
//		return;
	#endif

	// create array of textures & another of texture sets...
	ts_TextureChunkRecord* chunkRecord = &header->TextureChunkRecord[0];
	for(u32 i = 0; i < header->u32TextureCount; i++, chunkRecord++)
	{
		// make pNext pointers valid for this texture...
		ts_TextureChunkRecord* subRecord = chunkRecord;
		while(subRecord != NULL)
		{
			if(subRecord->u32NextOffset)
				subRecord->pNext = (ts_TextureChunkRecord *) ((u32) header + subRecord->u32NextOffset);

			// make the offsets relative to the texturechunkrecord they
			// are pointed to by, rather than relative to the header.
			if(subRecord->u32TextureOffset != 0)
			{
				u32 texdata = ((u32)header + subRecord->u32TextureOffset);
				subRecord->u32TextureOffset = (u32)texdata - (u32)subRecord;
			}
			if(subRecord->u32PaletteOffset != 0)
			{
				u32 paldata = ((u32)header + subRecord->u32PaletteOffset);
				subRecord->u32PaletteOffset = (u32)paldata - (u32)subRecord;
			}

			subRecord = subRecord->pNext;
		}

		// XBOX - setup the texture using the name lookup..
		#if defined (_XBOX)
		Texture* tex = new Texture(this, chunkRecord->acTextureName);
		tex->AddImageFromXPR(chunkRecord, &DxD->m_xprResources, i);
		mTextures.Add(tex);
		#else
		Texture* tex = new Texture(this, chunkRecord->acTextureName);
		tex->AddImageFromDBL(chunkRecord);
		mTextures.Add(tex);
		#endif

		// set gamma mode
		#if defined( GAMMA_CORRECTION_ON_LOAD )
		if (TextureUtil::GetGammaOnLoadFlag() == true)
		{
			if ( ( tex->GetFlags() & Texture::ANIMATED ) == 0 )
			{
				Image* I = tex->GetImage();
				TextureUtil::SetBlendMode(TEXTUREUTIL_GAMMA);
				if ( ( I->GetFormat() & TYPE_MASK ) == TYPE_RGB || ( I->GetFormat() & TYPE_MASK ) == TYPE_RGBA )
				{
					TextureUtil::BlitColor( I, 0, 0, I->GetWidth(), I->GetHeight() );
				}
				else
				{
					void* Palette = I->GetPaletteBuffer();
					int p;
					for ( p = 0 ; p < GammaCorrectedPalettes.Count() ; p++ )
						if ( GammaCorrectedPalettes[ p ] == Palette )
							break;
					I->ReleasePaletteBuffer();

					if ( p >= GammaCorrectedPalettes.Count() )
					{
						TextureUtil::FastGammaAdjust(I);
						GammaCorrectedPalettes.Add( Palette );
					}
				}
			}
		}
		#endif
	}
}

u16 TextureDatabase::AppendTexture(Texture* newTexture)
{
	mTextures.Add(newTexture);
	return mTextures.Count()-1;
}

u16 TextureDatabase::FindTexture(Texture* oldTexture)
{
	int count = mTextures.Count();
	for(u16 i = 0; i < count; i++)
	{
		if(GetTexture(i) == oldTexture)
			return i;
	}
	return 0xFFFF;
}

void TextureDatabase::RemoveTexture(Texture* oldTexture)
{
	// seek and destroy...
	u16 texidx = FindTexture(oldTexture);
	ASSERT(texidx != 0xFFFF);
	if(texidx == 0xFFFF)
		return; 

	mTextures.Delete(texidx);
}

void TextureDatabase::RemoveAllTextures(void)
{
	while(mTextures.Count())
	{
		Texture* tex = GetTexture(0);
		RemoveTexture(tex);
		delete tex;
	}
}

void TextureDatabase::StitchAnimatedSetIntoPlace(void *pChunkData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize)
{
	ts_AnimatedTextureChunkHeader* header = (ts_AnimatedTextureChunkHeader*)pChunkData;
	ASSERT_PTR(header);

	Texture* animatedTexture = GetTexture((u16)header->u32TextureSlot);
	ASSERT_PTR(animatedTexture);

	animatedTexture->RemoveAllImages();

	if(header->u16TextureCount > 1)
		animatedTexture->SetFlags(Texture::ANIMATED);


	for(u16 i = 0; i < header->u16TextureCount; i++)
	{
		ts_TextureChunkRecord* chunkRecord = &header->TextureChunkRecord[i];

		// fixup relative to record instead of header.
		if(chunkRecord->u32TextureOffset != 0)
		{
			u32 texdata = (u32)pChunkData + chunkRecord->u32TextureOffset;
			chunkRecord->u32TextureOffset = texdata - (u32)chunkRecord;
		}
		if(chunkRecord->u32PaletteOffset != 0)
		{
			u32 paldata = (u32)pChunkData + chunkRecord->u32PaletteOffset;
			chunkRecord->u32PaletteOffset = paldata - (u32)chunkRecord;
		}

		u32 flags = 0;
		if (chunkRecord->u32Flags & TEX_FLAG_UVREVERSED)
			flags |= Image::UVREVERSED;

		int idx = animatedTexture->AddImageFromDBL(chunkRecord);
		ASSERT(idx == i);
	}
}

void TextureDatabase::RemoveAnimatedSet(int Handle)
{
	// Hmmm... what do I do here...?
	LOCKUP();
}


Texture* TextureDatabase::GetTexture(u16 index)
{
	ASSERT( index < TextureCount() );
	return mTextures[index];
}

void TextureDatabase::SetFlags(u32 flags)
{
	mFlags |= flags;
}

void TextureDatabase::ClearFlags(u32 flags)
{
	mFlags &= ~flags;
}

void TextureDatabase::VerifyThatNoImagesAreLocked(void)
{
#ifdef _DEBUG
	int count = TextureCount();
	for(int i = 0; i < count; i++)
	{
		GetTexture(i)->VerifyThatNoImagesAreLocked();
	}
#endif
}

int TextureDatabase::AllocatedSize()
{
	int size = 0;
	for ( int i = 0 ; i < TextureCount() ; i++ )
	{
		size += sizeof(Texture*);
		size += GetTexture( i )->AllocatedSize();
	}
	return size;
}
