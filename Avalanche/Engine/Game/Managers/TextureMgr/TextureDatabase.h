#ifndef __TEXTURE_DATABASE_H__
#define __TEXTURE_DATABASE_H__

#if defined (_XBOX)
#include "platform/Xbox/xbresource.h"
#endif

struct ts_RecordList;

class TextureDatabase : public ReferenceCountable<u16>
{
protected:
	// name of .dbl file that contains texture chunk, for debugging
	char mName[TEXTURE_SET_NAME_LENGTH];

	u32 mFlags;
	u32 mPieceCount;
	ts_PieceRecord* mPieces;
	DynamicArray<Texture*> mTextures;

//	#if defined (_XBOX)
//	CXBPackedResource       m_xprResources;
//	#endif

public:
	/// @{
	/// to exist on a list
	TextureDatabase* next;
	TextureDatabase* prev;
	/// @}

	enum {
		PERMENANT = 0x00000001,
	};

	void SetFlags(u32 flags);
	void ClearFlags(u32 flags);

	/// make an empty database
	TextureDatabase(const char* name);

	/// 
	~TextureDatabase();

	/// dbl support
	void ImportDBL(const char* Filename,  void* Dx, ts_TextureChunkHeader *header, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize);

	/// get # of textures in database
	u16 TextureCount(void) const { return mTextures.Count(); }

	/// get at a texture by index... should this be an operator[]?
	Texture* GetTexture(u16 index) NAMED_SECTION("text.rend1");

	/// append a new texture into the database...
	u16 AppendTexture(Texture* newTexture);

	/// find the index for the given texture
	u16 FindTexture(Texture* oldTexture);

	/// empty the database out (deletes the textures)
	void RemoveAllTextures(void);

	/// remove a texture from the database. (does not delete the texture)
	void RemoveTexture(Texture* oldTexture);

	/// @{
	/// I hate this..
	void StitchAnimatedSetIntoPlace(void *pChunkData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize);
	void RemoveAnimatedSet(int Handle);
	/// @}

	/// I hate this too...
	/// @{
	u32 PieceCount(void) const { return mPieceCount; }
	ts_PieceRecord* GetPiece(u16 index) { return &mPieces[index]; }
	/// @}

	/// I hate this too.. its here to support icky stuff also..
	/// @{
	ts_RecordList* pTextureContextList;
	/// @}

	/// make sure that no images in this database are locked
	void VerifyThatNoImagesAreLocked(void);

	/// get allocated size
	int AllocatedSize();
};

#endif // __TEXTURE_DATABASE_H__
