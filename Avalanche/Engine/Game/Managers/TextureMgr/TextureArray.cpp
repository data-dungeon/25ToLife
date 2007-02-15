
/*************************************/
/*                                   */
/*   TextureArray.cpp                  */
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



/// Create a texture set.  
///
///
TextureArray::TextureArray( int size /* = 8 */ ) : mTextureHolders( size, 8 )
{
	mCacheID = -1;
	mFlags = 0;
}

/// Duplicate the texture set into a new one.
///
///
TextureArray* TextureArray::Clone()
{
	TextureArray* newset = new TextureArray( TextureCount() );
	
	int count = TextureCount();
	for(int i = 0; i < count; i++)
	{
		newset->AddTexture(GetTexture(i));
	}
	newset->SetCacheID( mCacheID );
	newset->mFlags = mFlags & TSET_CLONABLE_FLAGS;
	return newset;
}


/// Get a texture from the set.
///
/// @param index index of the texture within the set
///
/// @return The requested texture or null
///
Texture* TextureArray::GetTexture(u16 index)
{
	ASSERT(index < TextureCount());
	return mTextureHolders[index].mTexture;
}

/// Lock this set
///
///
void TextureArray::Lock(void)
{
	int count = TextureCount();

	// for now we do the naive approch of locking all child textures...
	for(int i = 0; i < count; i++)
	{
		GetTexture(i)->Lock();
	}
}

/// Unlock this set
///
///
void TextureArray::Unlock(void)
{
	int count = TextureCount();

	// for now we do the naive approch of unlocking all child textures...
	for(int i = 0; i < count; i++)
	{
		GetTexture(i)->Unlock();
	}
}

/// Get the number of textures in the set
///
///
/// @return number of textures in the set
///
u16 TextureArray::TextureCount(void) const
{
	return mTextureHolders.Count();
}

/// find the texture in the holders list.
///
/// @param searchTexture texture to search fo
///
/// @return index or 0xFFFFFFFF if not found
///
u32 TextureArray::FindHolder(Texture* searchTexture)
{
	u32 count = TextureCount();
	
	// see if its already in the holders list...
	for(u32 i = 0; i < count; i++)
	{
		if(mTextureHolders[i].mTexture == searchTexture)
		{
			return i;
		}
	}
	return 0xFFFFFFFF;
}


/// Add a Texture to the set
///
/// @param texture the texture to add
///
void TextureArray::AddTexture(Texture* texture)
{
	int count = TextureCount();

	u32 idx = FindHolder(texture);
	
	if(idx != 0xFFFFFFFF)
	{
		mTextureHolders[idx].mAddCount++;
		return;
	}

	if(texture->GetFlags() & Texture::ANIMATED)
		mFlags |= TSET_FLAG_CONTAINS_ANIM;

	// make sure we havent run out of indices
	ASSERT(count < ~(0xFFFFFFFF << (sizeof(u16)*8)));

	// ok, then we need to add it.
	TextureHolder tempHolder;
	tempHolder.mTexture = texture;
	tempHolder.mAddCount = 1;
	mTextureHolders.Add(tempHolder);
}

/// Decrement reference count for texture in this set and remove from
/// set if its 0.
///
/// @param texture texture to remove
///
void TextureArray::RemoveTexture(Texture* texture)
{
	u32 idx = FindHolder(texture);
	ASSERTS(idx != 0xFFFFFFFF, "Texture being removed is not in set.");
	if(idx == 0xFFFFFFFF) return;

	mTextureHolders[idx].mAddCount--;

	if(mTextureHolders[idx].mAddCount == 0)
	{
		// last remove really does it..
		mTextureHolders.Delete(idx);
	}

	// see if we still contain an animated texture! Icky, but
	// better than some alternatives we've used in the past.
	if(texture->GetFlags() & Texture::ANIMATED)
	{
		mFlags &= ~TSET_FLAG_CONTAINS_ANIM;
		int count = TextureCount();
		for(int i = 0; i < count; i++)
		{
			Texture* tex = GetTexture(i);
			if(tex->GetFlags() & Texture::ANIMATED)
			{
				mFlags |= TSET_FLAG_CONTAINS_ANIM;
				return;
			}
		}
	}
}

/// scans an entire texture database and adds all the textures in it
/// to the current texture set.
///
/// @param textureDatabase the database to scan
///
void TextureArray::AddEntireDatabase(TextureDatabase* textureDatabase)
{
	for(int i = 0, count = textureDatabase->TextureCount(); i < count; i++)
	{
		AddTexture(textureDatabase->GetTexture(i));
	}
}

/// scan the entire database and call RemoveTexture on every texture
/// in the database.
///
/// @param textureDatabase database to scan
///
void TextureArray::RemoveEntireDatabase(TextureDatabase* textureDatabase)
{
	for(int i = 0, count = textureDatabase->TextureCount(); i < count; i++)
	{
		RemoveTexture(textureDatabase->GetTexture(i));
	}
}

/// removes all textures from the set.  Note that this will probably
/// get add/remove calls out of balance, so you probably shouldnt use
/// it.  Its here mostly for legacy reasons.
///
///
void TextureArray::Clear(void)
{
	mTextureHolders.Reset();
	mFlags &= ~TSET_FLAG_CONTAINS_ANIM;
}

/// Add the white texture to this array
///
///
void TextureArray::AddWhiteTexture(void)
{
	AddTexture(TextureMgr::GetWhiteTexture());
}

/// Remove the white texture from this array
///
///
void TextureArray::RemoveWhiteTexture(void)
{
	RemoveTexture(TextureMgr::GetWhiteTexture());
}

// Return if the array contains an animated texture.
bool TextureArray::ContainsAnimatedTexture(void)
{
	return (mFlags & TSET_FLAG_CONTAINS_ANIM) == TSET_FLAG_CONTAINS_ANIM;
}



