
#ifndef __TEXTURE_SET_H__
#define __TEXTURE_SET_H__

#define TSET_FLAG_LOCKED			0x0001
#define TSET_FLAG_CONTAINS_ANIM	0x0002

#define TSET_CLONABLE_FLAGS (TSET_FLAG_CONTAINS_ANIM)

class Image;
class Texture;
class TextureLOD;
class TexturePiece;
class TextureArray;
class TextureDatabase;

class TextureArray : public ReferenceCountable<u16>
{
protected:
	/// flags
	u16 mFlags;

	/// cache id
	int mCacheID;

	struct TextureHolder
	{
		Texture* mTexture;
		u32 mAddCount;
	};
	
	DynamicArray<TextureHolder> mTextureHolders;
	u32 FindHolder(Texture* searchTexture);

public:
	TextureArray( int size = 8 );
	TextureArray* Clone(void);

	Texture* GetTexture(u16 index) NAMED_SECTION("text.rend1");
	Texture* operator[](u16 index) { return GetTexture(index); }

	void Clear(void);
	void AddTexture(Texture* texture);
	void RemoveTexture(Texture* texture);
	void AddEntireDatabase(TextureDatabase* textureDatabase);
	void RemoveEntireDatabase(TextureDatabase* textureDatabase);

	void AddWhiteTexture(void);
	void RemoveWhiteTexture(void);

	void Lock(void) NAMED_SECTION("text.rend1");
	void Unlock(void) NAMED_SECTION("text.rend1");

	void SetCacheID( const int id ) { mCacheID = id; }
	int  CacheID() const            { return mCacheID; }

	u16 TextureCount(void) const NAMED_SECTION("text.rend1");

	bool ContainsAnimatedTexture(void);
};

#endif // __TEXTURE_SET_H__
