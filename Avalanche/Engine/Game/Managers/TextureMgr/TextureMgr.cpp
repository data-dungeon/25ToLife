/*************************************/
/*                                   */
/*   TextureMgr.cpp                  */
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

#include "Game/Managers/TextureMgr/PlatformImage.h"

namespace TextureMgr
{
#ifdef _DEBUG
	void Test(void);
#endif
	void VerifyThatNoImagesAreLocked(void);

	static DoublyLinkedList<TextureDatabase> sTextureDatabases;
	static TextureDatabase sSpecialDatabase("SpecialTextures");
	static u32 sFlags;
	static Image* mRenderDestination;
	static TextureCache cache;

	TextureCache& Cache() { return cache; }

	static bool sIsPaused;

	/// @{
	/// Track requested back buffer copies.

	enum { MAX_BACK_BUFFER_COPIES_PER_FRAME = 1 };
	struct BackBufferCopyRequest {
		float scale;
		bool negate;
		bool canUseZBufferSpace;
	};
	static BackBufferCopyRequest sCopyRequests[MAX_BACK_BUFFER_COPIES_PER_FRAME];
	static u32 sCurrentCopyRequestIndex;

	/// @}

	/// @{
	/// this exists to support icky code that should probably be
	/// rewritten.

	static ts_RecordList* pLastTextureContextList;
	static void _FixTextureContextFirstTime(ts_TextureContext *pTextureContext);
	static void _FixTextureContextNew(ts_TextureContext *pTextureContext);
	static void _FixTextureContext(ts_TextureContext *pTextureContext, bool bFirstTime);

	ts_TextureChunkHeader* pDBLTextureChunk = NULL;

	static TextureDatabase* pWorldTextureSet;
	static ts_RecordList* pWorldTextureContextList;

	/// @}

	/// @{
	/// this exists to debug the timing of texture transfers
	static bool sDisableTextureTransfers = false;
	void DisableTextureTransfers() { sDisableTextureTransfers = true; }
	void EnableTextureTransfers()  { sDisableTextureTransfers = false; }
	bool AllowTextureTransfers()   { return !sDisableTextureTransfers; }
	/// @}

	/// @{
	/// Special textures
	static Texture* pWhiteTexture;
	static Texture* pBackBufferTexture;
	static Texture* pFrontBufferTexture;
	static Texture* CreateColoredTexture(ts_bRGBA* color, const char* name);
	static void CreateWhiteTexture(void);
	static void CreateBackBufferTexture(void);
	static void CreateFrontBufferTexture(void);
	/// @}

	/// Initialize the texture manager
	///
	///
	void Init(void)
	{
		PlatformImage::Init();

#if defined(WANTTEXTURETEST) && !defined(_XBOX)
		// This performs some unit testing.  Afterwards, it should leave
		// the texture manger cleanedup.
		Test();
#endif

		ASSERT(sTextureDatabases.Items() == 0);
		sTextureDatabases.AddTail(&sSpecialDatabase);

		CreateWhiteTexture();
		CreateBackBufferTexture();
		CreateFrontBufferTexture();

		sCurrentCopyRequestIndex = 0;

		ASSERT(pDBLTextureChunk == NULL);

		sFlags = ADVANCE_ENABLED;
		sIsPaused = false;
	}


	/// Shutdown the texture manager
	///
	///
	void Shutdown(void)
	{
		cache.Reset();

		VerifyThatNoImagesAreLocked();

#ifdef TRACK_TEXTURE_MEMORY
		EmptyTextureMemory();
#endif

		pDBLTextureChunk = NULL;

		ASSERT_PTR(pWhiteTexture);
		delete pWhiteTexture;

		ASSERT_PTR(pBackBufferTexture);
		delete pBackBufferTexture;

		ASSERT_PTR(pFrontBufferTexture);
		delete pFrontBufferTexture;

		sTextureDatabases.Unlink(&sSpecialDatabase);

		ASSERT(sTextureDatabases.Items() == 0);

		PlatformImage::Cleanup();
	}

	/// See if we are paused
	//

	bool IsPaused(void)
	{
		return sIsPaused;
	}

	/// Pause the texture manager.  This causes the texture manager to
	/// release any resources it can, in effect causing a temporary
	/// shutdown.
	///
	///
	void Pause(void)
	{
		ASSERT(!sIsPaused);
		cache.Unlock();

		VerifyThatNoImagesAreLocked();

#ifdef TRACK_TEXTURE_MEMORY
		EmptyTextureMemory();
#endif
		PlatformImage::PausingTextureManager();

		pDBLTextureChunk = NULL;
		sIsPaused = true;
	}

	/// Resume the texture menager.  Continue operation after a pause
	/// command.
	///
	///
	void Resume(void)
	{
		ASSERT(sIsPaused);
		ASSERT(pDBLTextureChunk == NULL);

#ifdef TRACK_TEXTURE_MEMORY
		VerifyEmptyTextureMemory();
#endif
		PlatformImage::ResumingTextureManager();
		sIsPaused = false;
	}

	/// Request the number of texture databases currently registered
	/// with the texture manager.
	///
	///
	/// @return The number of texture databases currently registered
	///
	u16 GetTextureDatabaseCount(void)
	{
		return sTextureDatabases.Items();
	}


	/// Register a texture database with the texture manager.  In this
	/// case, the texture database registered comes from a dbl file.
	///
	/// @param pChunkData Pointer into DBL
	/// @param pChunkHdr Header for dbl chunk
	/// @param ppRetData
	/// @param pu32DataSize size of dbl data
	/// @param animated is this an animated texture chunk
	///
	/// @return A pointer to the new texture database.
	///
	TextureDatabase* AddTextureDatabase(const char *Filename,  void* Dx, void *pChunkData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize)
	{
		ts_TextureChunkHeader* header = (ts_TextureChunkHeader*)pChunkData;
		TextureDatabase* db = new TextureDatabase(header->Name);
		db->ImportDBL(Filename, Dx, header, pChunkHdr, ppRetData, pu32DataSize);

		sTextureDatabases.AddTail(db);
		pChunkHdr->u16Flags &= ~DBL_FLUSHABLE;

		/* return address and size of active texture record */
		*ppRetData = db;
		*pu32DataSize = sizeof(TextureDatabase);

		// This is messy, but we have no better solution right now.
		// This needs to be refactored out.
		pDBLTextureChunk = (ts_TextureChunkHeader*)pChunkData;

		return db;
	}



//	// TODO: refactor this function out of existance.
//	// This should definitely not be handled this way.  This function
//	// gets a texture array from the last loaded som textureset chunk.
//	TextureArray* GetRecentTextureArray(u16 arrayIndex)
//	{
//		// no texture set loaded recently.
//		ASSERT_PTR(pDBLTextureChunk);
//
//		TextureDatabase* tdb = GetLastLoadedDatabase();
//		// sanity
//		ASSERT_PTR(tdb);
//
//		// hunt the texture set for the correct arary of elements...
//		#if !defined (_XBOX)
//		u32 count = pDBLTextureChunk->u32TextureCount;
//		#else
//		u32 count = pDBLCopiedTextureChunk->u32TextureCount;
//		#endif
//
//		// a new aray
//		TextureArray* array = new TextureArray( count + 1 );
//
//		u16 i = 0;
//		#if !defined (_XBOX)
//		ts_TextureChunkRecord* record = &pDBLTextureChunk->TextureChunkRecord[0];
//		#else
//		ts_TextureChunkRecord* record = &pDBLCopiedTextureChunk->TextureChunkRecord[0];
//		#endif
//		while(i < count)
//		{
//			// add all textures in set 0 or set arrayIndex.
//			if(record->u32SubSetNumber == 0 || record->u32SubSetNumber == arrayIndex)
//			{
//				// HACK!!!! HACK!!!! HACK!!!!  This assumes that
//				// tdb->GetTexture(i) is the same texture as
//				// pDBLTextureSet->TextureChunkRecord[i]
//				array->AddTexture(tdb->GetTexture(i));
//			}
//
//			// next
//			i++;
//			record++;
//		}
//		return array;
//	}

	/// remove a database from the texture manager... duh...
	///
	/// @param tdb database to remove
	///
	void RemoveTextureDatabase(TextureDatabase*  tdb)
	{
		ASSERT_PTR(tdb);

		tdb->VerifyThatNoImagesAreLocked();

		sTextureDatabases.Unlink(tdb);

		delete tdb;
	}

	/// Mark a particular texture database as always being required.  This
	/// does not gaurantee that all its textures will always be loaded.
	///
	/// @param tdb Texture database to mark as permanent
	/// @param perm true to mark permanent, false to unmark
	///
	void MakeTextureDatabasePermenant(TextureDatabase* tdb, bool perm = true)
	{
		// set a flag...
		tdb->SetFlags(TextureDatabase::PERMENANT);

		// Not sure what else to do here?  Guess we need to do something...
		LOCKUP();
	}

	/// retrieve a database from the manager by index.  This is not a very
	/// good thing to use, but getting things up and working with classes
	/// instead of the monolithic TextureMgr__* stuff has required it.
	///
	/// @param index index of database to retrieve
	///
	/// @return the database
	///
	TextureDatabase* GetTextureDatabase(int index)
	{
		TextureDatabase* tdb = sTextureDatabases.Head();
		while(tdb && index--)
		{
			tdb = tdb->next;
		}
		return tdb;
	}

	u16 GetTextureDatabaseIndex(TextureDatabase* tdb)
	{
		u16 index = 0;
		TextureDatabase* currDB = sTextureDatabases.Head();
		while(currDB && currDB !=  tdb)
		{
			currDB = currDB->next;
			index++;
		}
		if(!currDB)
			return 0xFFFF;

		return index;
	}

	/// Start a new frame.  Called at the start of every frame regardless
	/// of pause state, etc.
	///
	///
	void NewFrame(void)
	{
		VerifyThatNoImagesAreLocked();
		sCurrentCopyRequestIndex = 0;
	}

	/// Animate things for a new frame.  Called at the start of every
	/// frame when we are not paused.  Manages texture animation and
	/// things like that.
	///
	/// @param deltaTime time that passed since last call in seconds.
	///
	void Advance(float deltaTime)
	{
		VerifyThatNoImagesAreLocked();

		if(!IsFlagSet(ADVANCE_ENABLED))
			return;
	}

	/// set a bunhc of manager flags
	///
	/// @param flag flags to set
	///
	void SetFlag(u32 flag)
	{
		sFlags |= flag;
	}
	/// clear some manager flags
	///
	/// @param flag flag to clear
	///
	void ClearFlag(u32 flag)
	{
		sFlags &= ~flag;
	}

	/// is any one of the passed flags set?
	///
	/// @param flag flags to test
	///
	/// @return true or false
	///
	bool IsFlagSet(u32 flag)
	{
		return (sFlags & flag) != 0;
	}

	/// Create a new texture that lives until all its references are
	/// dead.  Useful for dynamically creating textures.  This is what
	/// used to be a RealTime Texture Set in the old texture manager.
	///
	/// @param TextureFlags flags for the new texture
	/// @param format format of the new texture
	/// @param width width of the new texture
	/// @param height height of the new texture
	///
	/// @return a pointer to the new texture
	///
	Texture* CreateSpecialTexture(u16 textureFlags, u16 imageFlags, u16 imageFormat, u16 width, u16 height, const char* name)
	{
		Texture* tex = new Texture(&sSpecialDatabase, name);
		tex->SetFlags(textureFlags);
		tex->AddImage(false, imageFlags, imageFormat, width, height, NULL);
		sSpecialDatabase.AppendTexture(tex);
		return tex;
	}

	/// creates a special texture which is 16x16 and consists entirely
	/// of one color.
	///
	/// @param color color of the texture
	///
	/// @return the texture
	///
	Texture* CreateColoredTexture(ts_bRGBA* color, const char* name)
	{
		Texture* tex = CreateSpecialTexture(
			Texture::READ_ONLY,
			0,
			Image::SIZE_32_BIT | Image::TYPE_RGBA,
			8, 8, name);
		ASSERT_PTR(tex);

		Image* img = tex->GetImage();
		ts_bRGBA* buff = (ts_bRGBA*)img->GetImageBuffer();

		// set color
		for(int x = 0; x < 8; x++)
			for(int y = 0; y < 8; y++)
				buff[x+y*8] = *color;

		img->ReleaseImageBuffer();

		return tex;
	}

	/// creates the white texture
	///
	///
	void CreateWhiteTexture(void)
	{
		ts_bRGBA white;
		white.R = 0xFF;
		white.G = 0xFF;
		white.B = 0xFF;
		white.A = 0x80;
		pWhiteTexture = CreateColoredTexture(&white, "WhiteTexture");
		ASSERT_PTR(pWhiteTexture);
	}

	/// Create the back buffer texture
	///
	///
	void CreateBackBufferTexture(void)
	{
		ASSERT( pBackBufferTexture == NULL );

		#ifdef PS2
		u16 width  = Display__GetRenderTargetWidth();
		u16 height = Display__GetRenderTargetHeight();
		#else
		u16 width  = Display__GetScreenWidth();
		u16 height = Display__GetScreenHeight();
		#endif

		#ifdef PS2
			u32 bpp    = Display__GetDrawBufferBPP();
		#else
			u32 bpp	  = 32;	// HACK
		#endif

		u16 format = 0;
		if ( bpp == 16 )
			format = Image::SIZE_16_BIT | Image::TYPE_RGBA;
		else if ( bpp == 24 )
			format = Image::SIZE_24_BIT | Image::TYPE_RGB;
		else if ( bpp == 32 )
			format = Image::SIZE_32_BIT | Image::TYPE_RGBA;
		else
			ASSERTS( false, "unsupported bpp depth" );

		pBackBufferTexture = CreateSpecialTexture( 0, Image::LOCAL|Image::BACK_BUFFER, format, width, height, "backbuffer" );
	}

	/// Create the front buffer texture
	///
	///
	void CreateFrontBufferTexture(void)
	{
		ASSERT( pFrontBufferTexture == NULL );

		u16 width  = Display__GetScreenWidth();
		u16 height = Display__GetScreenHeight();

		#ifdef PS2
			u32 bpp    = Display__GetDisplayBufferBPP();
		#else
			u32 bpp	  = 32;	// HACK
		#endif

		u16 format = 0;
		if ( bpp == 16 )
			format = Image::SIZE_16_BIT | Image::TYPE_RGBA;
		else if ( bpp == 24 )
			format = Image::SIZE_24_BIT | Image::TYPE_RGB;
		else if ( bpp == 32 )
			format = Image::SIZE_32_BIT | Image::TYPE_RGBA;
		else
			ASSERTS( false, "unsupported bpp depth" );

		pFrontBufferTexture = CreateSpecialTexture( 0, Image::LOCAL|Image::FRONT_BUFFER, format, width, height, "frontbuffer" );
	}

	/// Get a pointer to the white texture
	///
	///
	/// @return pointer to the white texture
	///
	Texture* GetWhiteTexture(void)
	{
		return pWhiteTexture;
	}

	/// Get a pointer to the front buffer texture
	///
	///
	/// @return a pointer to the front buffer texture
	///
	Texture* GetFrontBuffer(void)
	{
		return pFrontBufferTexture;
	}

	/// Get a pointer to the back buffer texture
	///
	///
	/// @return a pointer to the back buffer texture
	///
	Texture* GetBackBuffer(void)
	{
		return pBackBufferTexture;
	}

	/// Get a pointer to the zbuffer texture
	///
	///
	/// @return a pointer to the z buffer texture
	///
	Texture* GetZBuffer(void)
	{
		// Textures need to be created to represent the various buffers
		// they need to be appropriately marked as readonly/writeonly and
		// stuff as well (although those flags should be written around
		// on all platforms for things like the full screen distortion)
		LOCKUP();
		return NULL;
	}

	/// Get a pointer to a dynamic buffer texture.  This dynamic buffer
	/// texture is for short lived dynamic textures.  This is probably
	/// obsoleted by the CreateSpecialTexture() call.
	///
	///
	/// @return a pointer to a dynamic buffer texture
	///
	Texture* GetDynamicBuffer(void)
	{
		// This call should be obsoleted by the CreateSpecialTexture() call...
		LOCKUP();
		return NULL;
	}

	/// Get a temporary copy of the back buffer.  On the PS2, this copy
	/// is not in main ram, just up in texture memory.  If you want it in
	/// main ram, you'll have to clear the VOLATILE flag from the texture
	/// before you unlock it.  To release the texture, call Unlock on it,
	/// then delete the Texture*.
	///
	/// @param scale How much do you want to scale the copy by
	/// @param mode what kind of copy do you want?  NORMAL?  NEGATIVE?  GREYSCALE?
	/// @param canUseZBufferSpace can the copy be placed into the zbuffer memory (ignored on some platforms)
	///
	/// @return a locked Texture* that represets the copy... 
	///
	Texture* CopyBackBuffer(float scale /*= 1.0f*/, CopyMode mode /*= COPY_NORMAL*/, bool canUseZBufferSpace /*= false*/)
	{
		Texture* back = GetBackBuffer();
		Image* backImage = back->GetImage();
		int origWidth = backImage->GetWidth();
		int origHeight = backImage->GetHeight();
		int width = (int)(origWidth * scale);
		int height = (int)(origHeight * scale);

#ifdef _XBOX
		u16 flags = Image::RENDER_TARGET;

		// seriously ghetto check for large copy buffers on HDTV
		if( width == 1280 )
		{
			width = 1280 / 2;
			height = 720 / 2;
		}
		else if( width == 1920 )
		{
			width == 1920 / 3;
			height = 1080 / 3;
		}

		ASSERT( width <= 640 );
#else
		u16 flags = Image::LOCAL;
#endif

		if(canUseZBufferSpace)
			flags |= Image::Z_BUFFER;
		Texture* copy = CreateSpecialTexture(0, flags, backImage->GetFormat(), width, height, "BackCopy");
		back->Lock();
//		back->Select(); // have to select before we lockforrender, because back changes on the PS2.. ick...
		copy->Lock();
		copy->LockForRender();
		back->BitBlt(0,0,width,height);
		copy->UnlockForRender();
//		back->Unselect();
		back->Unlock();
		return copy;
	}
	
	/// Get a temporary copy of the front buffer.  On the PS2, this copy
	/// is not in main ram, just up in texture memory.  If you want it in
	/// main ram, you'll have to clear the TEMORARY flag from the texture
	/// before you unlock it.
	///
	/// @param scale How much do you want to scale the copy by
	/// @param mode what kind of copy do you want?  NORMAL?  NEGATIVE?  GREYSCALE?
	/// @param canUseZBufferSpace can the copy be placed into the zbuffer memory (ignored on some platforms)
	///
	/// @return a locked Texture* that represets the copy... 
	///
	Texture* CopyFrontBuffer(float scale /*= 1.0f*/, CopyMode mode /*= COPY_NORMAL*/, bool canUseZBufferSpace /*= false*/)
	{
		LOCKUP();
		return NULL;
	}

	void GetBackBufferTextureScaling(float &uScale, float &vScale)
	{
#if defined(_XBOX) || defined(PS2)
		Texture *texture = GetBackBuffer();
		ASSERT(texture);
		Image *image = texture->GetImage();
		ASSERT(image);
		PlatformImage *platformImage = image->GetPlatformImage();
		ASSERT(platformImage);
#if defined(_XBOX)
		platformImage->GetUVScaling(uScale, vScale);
#elif defined(PS2)
		uScale = (float)image->GetWidth() / platformImage->VirtualWidth();
		vScale = (float)image->GetHeight() / platformImage->VirtualHeight();
#endif
#else
		uScale = vScale = 1.0f;
#endif
	}
	void GetFrontBufferTextureScaling(float &uScale, float &vScale)
	{
#if defined(_XBOX) || defined(PS2)
		Texture *texture = GetFrontBuffer();
		ASSERT(texture);
		Image *image = texture->GetImage();
		ASSERT(image);
		PlatformImage *platformImage = image->GetPlatformImage();
		ASSERT(platformImage);
#if defined(_XBOX)
		platformImage->GetUVScaling(uScale, vScale);
#elif defined(PS2)
		uScale = (float)image->GetWidth() / platformImage->VirtualWidth();
		vScale = (float)image->GetHeight() / platformImage->VirtualHeight();
#endif
#else
		uScale = vScale = 1.0f;
#endif
	}

	/// This function is probably going to be depricated due to
	/// CreateSpecialTexture().  But it tells the texture manager how much
	/// memory you'll need for your dynamic buffer
	///
	/// @param bytesize size of the requested buffer
	///
	void RequireDynamicBuffer(u32 bytesize)
	{
		// This call should be obsoleted by the CreateSpecialTexture() call...
		LOCKUP();
	}

	void VerifyThatNoImagesAreLocked(void)
	{
#ifdef _DEBUG
		TextureDatabase* tdb = sTextureDatabases.Head();
		while(tdb)
		{
			tdb->VerifyThatNoImagesAreLocked();
			tdb = tdb->next;
		}
#endif
	}


	/// Load a texture context block.  This really doesnt belong in the
	/// texture manager.
	/// To avoid doing shaders and having to rewrite EVERYTHING, I have
	/// just implimented texture contexts pretty much vertbatim.  This
	/// should really be changed to a shader system. My quick bit of
	/// research says that this is a BIG can of worms.
	///
	/// @param pChunkData pointer into dbl
	/// @param pChunkHdr header in dbl
	/// @param ppRetData
	/// @param pu32DataSize size in dbl
	///
	/// @return if it succeeded
	///
	bool LoadTextureContexts(void *pChunkData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize)
	{
		unsigned short usIndex;
		ts_RecordList *pTextureContextList;
		ts_TextureContext *pTextureContext;

		/* point to data */

		pTextureContextList = (ts_RecordList *) pChunkData;
		ResolveRecordList(pTextureContextList);

		/* fix up each texture context */

		for (usIndex = 0; usIndex < pTextureContextList->usRecordCount; usIndex++)
		{
			pTextureContext = (ts_TextureContext *) GetRecordAddress(pTextureContextList, usIndex);
			_FixTextureContext(pTextureContext, true);
		}

		/* record this as the last-loaded texture context chunk. this is for ps2 dma load fixup-- */
		/* we need access to texture contexts to set up gs texture registers */

		pLastTextureContextList = pTextureContextList;

		/* hack fix for ps2 som load/fixup problems. link this texture context back to its texture set. this */
		/* enables us to set TextureMgr.pLastTextureContextList to the correct value when fixing up a SOM. */
		/* the issue is: PS2 SOM's have unresolved texture references that are currently fixed up into texture */
		/* register values, but might also be fixed up into texture binds. however, if a SOM has bones (or */
		/* alternate vertex color sets), then the fix-up is delayed until an actor is instantiated. at this */
		/* point the last-loaded-texture-context is probably bogus, and the fixup will fail. since SOM's */
		/* contain a pointer to their texture set, i can point the texture set to the correct texture context, */
		/* and will be able to get to the data i need at fixup time */

		// double ick... I have to fetch the last loaded texture
		// database... ick ick...
		// it's possible to have a texture context chunk without a texture chunk, though rare...
		if(GetLastLoadedDatabase())
		{
			GetLastLoadedDatabase()->pTextureContextList = pTextureContextList;
		}

		/* this is permanent data */

		pChunkHdr->u16Flags &= ~(DBL_FLUSHABLE);
		*ppRetData = pChunkData;
		*pu32DataSize = pChunkHdr->u32Size;
		return( true);
	}

	/// This really doesnt belong in the
	/// texture manager.
	/// To avoid doing shaders and having to rewrite EVERYTHING, I have
	/// just implimented texture contexts pretty much vertbatim.  This
	/// should really be changed to a shader system. My quick bit of
	/// research says that this is a BIG can of worms.
	///
	/// @param psTextureContextList list of contexts to set as 'last'
	///
	void SetLastTextureContextList(ts_RecordList* psTextureContextList)
	{
		pLastTextureContextList = psTextureContextList;
	}

	/// This really doesnt belong in the
	/// texture manager.
	/// To avoid doing shaders and having to rewrite EVERYTHING, I have
	/// just implimented texture contexts pretty much vertbatim.  This
	/// should really be changed to a shader system. My quick bit of
	/// research says that this is a BIG can of worms.
	///
	/// @param nContext context index
	/// @param nLayer layer index
	///
	/// @return layer tag from given context
	///
	ts_TextureLayer *GetTextureContextLayer(int nContext, int nLayer)
	{
		//// bind to the given layer (ahh, the obvious and straightforward approach)
		//ts_TextureContext *pTextureContext;
		//pTextureContext = (ts_TextureContext *) GetRecordAddress(pLastTextureContextList, nContext);
		//return &pTextureContext->TextureLayer[nLayer];

		// instead of binding to the exact layer the filterchain exports, we bind to the
		// first exact match for the layer.  this allows us to do far less dynamic patching of the
		// layers in SharedBody.  the filter chain should do this, but I need immediate results.

		// get the layer they want
		ts_TextureContext* wantContext = (ts_TextureContext*) GetRecordAddress( pLastTextureContextList, nContext );
		ts_TextureLayer*   wantLayer   = &wantContext->TextureLayer[ nLayer ];

		// check to see if we've already remapped it (see below for creation of remap record)
		u16* redirect = (u16*) &wantLayer->TextureName[ 0 ];
		if ( redirect[0] == 0xff00 )
		{
			int contextIndex = redirect[1];
			int layerIndex   = redirect[2];
			//dbgPrint( "%d,%d --> using %d,%d\n", nContext, nLayer, contextIndex, layerIndex );
			ts_TextureContext* context = (ts_TextureContext*) GetRecordAddress( pLastTextureContextList, contextIndex );
			ts_TextureLayer* layer = &context->TextureLayer[ layerIndex ];
			return layer;
		}

		// find the first exact match and return it instead.
		for ( int contextIndex = 0 ; pLastTextureContextList->usRecordCount ; contextIndex++ )
		{
			ts_TextureContext* context = (ts_TextureContext*) GetRecordAddress( pLastTextureContextList, contextIndex );
			for ( int layerIndex = 0 ; layerIndex < context->TextureCount ; layerIndex++ )
			{
				ts_TextureLayer* layer = &context->TextureLayer[ layerIndex ];
				if ( layer->s16TextureIndex == wantLayer->s16TextureIndex &&
					  layer->textureDatabase == wantLayer->textureDatabase &&
					  layer->u8Blend         == wantLayer->u8Blend         &&
					  layer->u8Filter        == wantLayer->u8Filter        &&
					  layer->u8Flags         == wantLayer->u8Flags         &&
					  layer->u8Method        == wantLayer->u8Method        &&
					  layer->u8WrapU         == wantLayer->u8WrapU         &&
					  layer->u8WrapV         == wantLayer->u8WrapV )
				{
					// if we're different, store info about the remap in the old filename (yes, hacky),
					// so we can find it later, and also so that SharedBody won't try to map this
					// record in the future.
					if ( nContext != contextIndex || nLayer != layerIndex )
					{
						//dbgPrint( "rebinding '%s' from %d,%d --> %d,%d\n", layer->TextureName, nContext, nLayer, contextIndex, layerIndex );
						u16* redirect = (u16*) &wantLayer->TextureName[ 0 ];
						redirect[0] = 0xff00;
						redirect[1] = contextIndex;
						redirect[2] = layerIndex;
					}
					return layer;
				}
			}
		}
		ASSERT( false );
		return NULL;
	}

	Image* GetRenderDestination(void)
	{
		return mRenderDestination;
	}

	void SetRenderDestination(Image* img)
	{
		mRenderDestination = img;
	}


#ifdef TRACK_TEXTURE_MEMORY

	// TODO: make this a singly linked list instead.
	static DoublyLinkedList<PlatformImage> sImageInTexMem;

	/// Adds an image to the list of images currently in texture
	/// memory.  This is good for managing texture memory ourselves.
	///
	/// @param image image that was uploaded
	///
	void AddUploadedImage(PlatformImage* image)
	{
		ASSERT(image->next == NULL && image->prev == NULL);
		sImageInTexMem.InsertDescending(image);
	}

	/// remove an image from the set of images in texture memory.
	///
	/// @param image image to remove
	///
	void RemoveUploadedImage(PlatformImage* image)
	{
		sImageInTexMem.Unlink(image);
	}

	/// Free the next candidate image from texmem.
	///
	///
	/// @return true if it freed something, false otherwise
	///
	bool ReleaseLRUImage(void)
	{
		PlatformImage* releaseImage = sImageInTexMem.Tail();
		if(releaseImage == NULL)
			return false;
		if(releaseImage->ReleasePriority() == 0xFFFFFFFF)
			return false;
		releaseImage->Release();
		return true;
	}

	void TouchUploadedImage(PlatformImage* image)
	{
		RemoveUploadedImage(image);
		AddUploadedImage(image);
	}

	void VerifyEmptyTextureMemory(void)
	{
		ASSERT(sImageInTexMem.Items() == 0);
	}

	void EmptyTextureMemory(void)
	{
		int count = sImageInTexMem.Items();
		while(count--)
		{
			PlatformImage* head = sImageInTexMem.Head();
			head->Release();
		}
		ASSERT(sImageInTexMem.IsEmpty());
	}

	void FlushTextureMemory(void)
	{
		PlatformImage* releaseImage = sImageInTexMem.Head();
		while(releaseImage)
		{
			PlatformImage* next = releaseImage->next;
			releaseImage->Release();
			releaseImage = next;
		}
	}

#endif


#ifdef _DEBUG
	void Test(void)
	{
		Texture::Testing(true); // suppress some messages
		PlatformImage::Test();
		Image::Test();
		Texture::Testing(false);
	}
#endif


	TextureDatabase* GetLastLoadedDatabase(void)
	{
		return sTextureDatabases.Tail();
	}

/* set the info in the texture context to match the current rendering settings. for example, things change depending
	on whether lighting is on or off. this also allows us to update all the models when a setting changes. during development,
	it's nice to be able to turn lighting on and off */

	static void _FixTextureContext(ts_TextureContext *pTextureContext, bool bFirstTime)
	{
		/* handle first-time-only fix-up */
		/*** ned says: for now, texture contexts are only fixed up once, at load time, so bFirstTime is always ***/
		/*** true. it used to be that the fix-up depended on some rendering settings (e.g. lighting on or off) ***/
		/*** and would be re-done when lighting was toggled during runtime, but that dependency is now gone.   ***/
		/*** bringing it back is problematic, because the new form of data has the desired blend function for  ***/
		/*** each texture layer in the u8Blend field, but thos values may get changed based on multi-texture   ***/
		/*** considerations                                                                                    ***/

		ASSERT(bFirstTime);	// see comment above, talk to ned if this hits
		if (bFirstTime)
			_FixTextureContextFirstTime(pTextureContext);

//#define PRINT_TEXTURE_CONTEXT_FIXUP
#ifdef PRINT_TEXTURE_CONTEXT_FIXUP
		if (pTextureContext->TextureCount != 0)
			dbgPrint("Texture context fixup, %d layer(s):\n", pTextureContext->TextureCount);
#endif //PRINT_TEXTURE_CONTEXT_FIXUP

		/* if no texture layers, we're done */

		if (pTextureContext->TextureCount == 0)
			return;

		/* look for new-style data that has u8Blend already filled in */

		if (pTextureContext->TextureLayer->u8Blend != RS_BLENDFUNC_UNDEFINED)
		{
			_FixTextureContextNew(pTextureContext);
			return;
		}

		/* loop through texture layers */

		int nNextPass = TextureState__GetNumUnits();
		bool bFirstPass = true;
		bool bFirstTexture = true;
		ts_TextureLayer *pTextureLayer = pTextureContext->TextureLayer;
		for (int i = 0; i < (signed) pTextureContext->TextureCount; i++, pTextureLayer++)
		{
			/* any texture context that comes through here is ok for multi-texturing-- that's what this code is for */

			pTextureLayer->u8Flags |= TEXTURE_MULTITEXTURE_OK;

			/* rest of layer record */

			pTextureLayer->u8Blend = RS_BLENDFUNC_DEFAULT;
			if (pTextureLayer->u8Flags & TEXTURE_IS_ENVIRONMENT_MAP)
			{
				/************** environment maps *****************/

				if ((bFirstPass) || (!bFirstTexture))
				{
					/* env map is base layer, or is not first layer in multi-texture scenario. blend it into the incoming fragment */
#if defined(WIN32) || defined(GCN)
					pTextureLayer->u8Method = TS_METHOD_INTERPOLATE;
#else
					pTextureLayer->u8Method = TS_METHOD_MODULATE;	// on ps2, this works ok with white base color
#endif //WIN32
				}
				else
				{
					/* cause resulting alpha to be the constant alpha, so it blends that way into the framebuffer */

					pTextureLayer->u8Method = TS_METHOD_CONSTANTALPHA;	// becomes DECAL (same as REPLACE) on ps2

					/* on ps2, we can use a blend mode that uses a fixed alpha value instead of the source texel's alpha */
					/* this is good for blending a semi-transparent env map onto another texture, but it overwrites */
					/* the alpha calculated for fogging or cross-fading, so it may end up being an inappropriate technique. */
					/* but the alternative is ugly: to somehow tell the VU to push the current global alpha, multiply it */
					/* by some passed opacity value, and then restore it when the env map was finished. this would work, */
					/* but it means that we could never do semi-transparent env maps in a multi-texture context, since */
					/* each vertex only has one color that is passed to the gs, and the modified global alpha value would */
					/* be incorrect for the base texture layer. the even scarier fix to that is to go through an modify */
					/* all vertex colors before the second texture context xgkick. ugh. */

					/* if we knew that we were always in multi-pass instead possibly going to dual texture context, then */
					/* we could possibly do this by setting the correct alpha into the base color and re-sending the base */
					/* color. since the base color gets replicated to provide a color for each vertex, this would be */
					/* correctly multiplied by the fog/cross-fade alpha in the global ambient, and everything would be */
					/* happy. this is only valid for lighted models, though-- for vertex-lighted models you would have to */
					/* jam the alpha into all vertex colors */

					/* what we REALLY need on the ps2 is a way to multiply all vertex color alpha values by a constant alpha. */
					/* but even this is dead-on-arrival in a dual-texture-context scenario, since we would need a different */
					/* constant alpha per layer */

#ifdef PS2
					pTextureLayer->u8Blend = RS_BLENDFUNC_FIXEDALPHA;	// fixed alpha replaces texel alpha
//yuch-- use this to use alpha channel instead of overriding with opacity value
//pTextureLayer->u8Method = TS_METHOD_DECAL;
//pTextureLayer->u8Blend = RS_BLENDFUNC_DEFAULT;
#endif //PS2
				}
			}

			else if (pTextureLayer->u8Flags & TEXTURE_IS_DIFFUSE_MAP)
			{
				/************** diffuse maps *****************/

				if ((bFirstPass) && (bFirstTexture))
				{
					/* diffuse map over flat color patch-- modulate material color */

					ASSERT(pTextureLayer->u8Flags & TEXTURE_BASE_COLOR_SHOWS);	// a patch that has a diffuse map as the first texture should always have a base color-- if this hits, tell ned
					pTextureLayer->u8Method = TS_METHOD_MODULATE;		// multiply fragment by texel
				}
				else if (bFirstTexture)
				{
					/* first texture unit, so change blend-into-frame-buffer method */

					pTextureLayer->u8Method = TS_METHOD_REPLACE;		// use texel as is
					pTextureLayer->u8Blend = RS_BLENDFUNC_DIFFUSE;	// pixel = frame buffer * texel
				}
				else
				{
					/* not the first texture unit in a pass */

					pTextureLayer->u8Method = TS_METHOD_MODULATE;		// multiply fragment by texel
				}
			}
			else
			{
				/************** normal texture *****************/

				if ((bFirstPass) && (bFirstTexture))
				{
					/* patch's first texture */

					if ((pTextureLayer->u8Flags & TEXTURE_HAS_ALPHA) == 0)
					{
						/* first texture has no alpha, need to modulate it if lighting enabled */

						ASSERT((pTextureLayer->u8Flags & TEXTURE_BASE_COLOR_SHOWS) == 0);	// no-alpha textures shouldn't need a base color unless we start supporting the modulate slider in animation master
//yuch-- always modulate so that base color is effective. if you don't want base color to have an effect, use white.
						pTextureLayer->u8Method = TS_METHOD_MODULATE;	// multiply fragment (lighting/gouraud result) by texel
////yuch-- this check is not complete. it ignores the possibility of a solid-color light map (which essentially turns into
//// a base color). it also ignores the INSTANCE_NO_LIGHT flag, which this model's parent instance may have set, and it
//// also ignores the possibility that TextureMgr__GetVertexColors may return a NULL. should be ok for now
//					if ((RenderMgr__LightingOn()) || (RenderMgr__VertexColorsOn()))
//						pTextureLayer->u8Method = TS_METHOD_MODULATE;	// multiply fragment (lighting/gouraud result) by texel
//					else
//						pTextureLayer->u8Method = TS_METHOD_REPLACE;	// use texel as is
					}
					else
					{
						/* texture has alpha. it can decal over the constant color, or it can cookie-cut */
						/* (be transparent where the alpha is non-opaque) */

//nm092001-- at this point, we always want the base texture to be modulated by lighting info (either vertex colors
// or real-time lighting). this doesn't support true decalling-- where there's a base color that shows through
// based on the opacity of the texel. by using MODULATE, any base color will just multiply with the texel, without
// taking the texel alpha into account. to get true lighted decalling, i think we'd have to render the base color
// as a modulated first pass, then render the decal using MODULATE again so that the lighting affects it. this would
// also mean that we shouldn't bake the base color into the vertex colors...on the ps2, we'd have to insert the
// white texture as the first texture context layer-- but then we'd have to bake the base color into the vertex
// color so that it showed up as the correct color-- what a mess.
//					if (pTextureLayer->u8Flags & TEXTURE_BASE_COLOR_SHOWS)	// flag is clear if marked cookie-cut
//						pTextureLayer->u8Method = TS_METHOD_DECAL;		// constant color shows through alpha mask
//yuch-- always modulate so that base color is effective. if you don't want base color to have an effect, use white.
//					else
						pTextureLayer->u8Method = TS_METHOD_MODULATE;	// multiply fragment (lighting/gouraud result) by texel
////yuch-- this check is not complete. it ignores the possibility of a solid-color light map (which essentially turns into
//// a base color). it also ignores the INSTANCE_NO_LIGHT flag, which this model's parent instance may have set, and it
//// also ignores the possibility that TextureMgr__GetVertexColors may return a NULL. should be ok for now
//					else if ((RenderMgr__LightingOn()) || (RenderMgr__VertexColorsOn()))
//						pTextureLayer->u8Method = TS_METHOD_MODULATE;	// cookie cut, but with lighting
//					else
//						pTextureLayer->u8Method = TS_METHOD_REPLACE;	// cookie cut, transparent through alpha mask
					}
				}
				else if (bFirstTexture)
				{
					/* first texture in a multi-pass scenario, so just use GL_REPLACE. should always have alpha, */
					/* since it is being laid down over some existing texture */

//ned says: this used to be a valid assert, but now with crossfade\ing textures the alpha is in the vertex colors
//				ASSERT(pTextureLayer->u8Flags & TEXTURE_HAS_ALPHA);
//#ifndef SHIP_IT
//				if((pTextureLayer->u8Flags & TEXTURE_HAS_ALPHA) == 0)
//					dbgPrint("first of n layers in subsequent pass w no alpha %s\n", pTextureLayer->TextureName);
//#endif
					pTextureLayer->u8Method = TS_METHOD_REPLACE;		// use texel as is
				}
				else
				{
					/* second or greater texture in a multi-texture scenario. should always have alpha, since it */
					/* is being laid down over some existing texture */

//ned says: this used to be a valid assert, but now with crossfade\ing textures the alpha is in the vertex colors
//				ASSERT(pTextureLayer->u8Flags & TEXTURE_HAS_ALPHA);
//#ifndef SHIP_IT
//				if((pTextureLayer->u8Flags & TEXTURE_HAS_ALPHA) == 0)
//					dbgPrint("second of n layers w no alpha %s\n", pTextureLayer->TextureName);
//#endif
					pTextureLayer->u8Method = TS_METHOD_DECAL;			// decal over incoming pixel
				}
			}

#ifdef PRINT_TEXTURE_CONTEXT_FIXUP
			static char *WrapString[2] = {"Clamp ", "Repeat"};
			static char *FilterString[6] = {"point sample", "bilinear    ", "mipmap point", "mipmap pttri", "mipmap bilin", "mipmap triln"};
			static char *MethodString[6] = {"passthru  ", "decal     ", "replace   ", "modulate  ", "interpol. ", "constalpha"};
			dbgPrint("   Layer %d: wrap U: %s, wrap V %s, filter: %s, method: %s, bind %d, %s\n",
						i,
						WrapString[pTextureLayer->u8WrapU],
						WrapString[pTextureLayer->u8WrapV],
						FilterString[pTextureLayer->u8Filter],
						MethodString[pTextureLayer->u8Method],
						pTextureLayer->s16TextureIndex,
						pTextureLayer->TextureName);
#endif //PRINT_TEXTURE_CONTEXT_FIXUP

			/* update pass/texture flags */

			if (i + 1 == nNextPass)
			{
				bFirstPass = false;
				bFirstTexture = true;
				nNextPass++;	// for now, downshift to single-texture-per-pass after first pass
			}
			else
				bFirstTexture = false;
		}
	}

	/* fix up new-style texture context-- desired blend function for each layer is already in u8Blend field */

	static void _FixTextureContextNew(ts_TextureContext *pTextureContext)
	{
		/* set up for multi-texturing fix-up */

		int nCountThisPass = Math::Min(TextureState__GetNumUnits(), pTextureContext->TextureCount);
		int nIndexThisPass = 0;

		/* currently, multi-texturing is only used for first pass, subsequent passes (if any) are all a single */
		/* texture layer. sometimes, texture context settings force first pass to be a single texture layer too */

		if ((nCountThisPass > 1) && (pTextureContext->TextureCount > 1))
		{
			ts_TextureLayer *pTextureLayer = &pTextureContext->TextureLayer[1];
			switch(pTextureLayer->u8Blend)
			{
			case RS_BLENDFUNC_DIFFUSE:
				/* multi-texturing allowed for these requested blend modes */
				break;
			case RS_BLENDFUNC_DEFAULT:
				if ((pTextureLayer->u8Flags & (TEXTURE_IS_DIFFUSE_MAP | TEXTURE_IS_ENVIRONMENT_MAP)) == 0)
				{
					/* no multi-texturing allowed because there is no way to get the lighting info to apply correctly-- */
					/* if you use TS_METHOD_DECAL you get no lighting info, if you use TS_METHOD_MODULATE, the base */
					/* texture gets blended into the new texture layer regardless of alpha values */

					nCountThisPass = 1;
				}
				break;
			default:
				/* no multi-texturing allowed-- have to use multi-pass. many more combinations are possible, but */
				/* we have to supply extra TS_METHOD implementations (e.g. have an additive nethod) */

				nCountThisPass = 1;
				break;
			}
		}

		/* tell renderer whether multi-texturing is allowed for this texture context */

		if (nCountThisPass > 1)
			pTextureContext->TextureLayer->u8Flags |= TEXTURE_MULTITEXTURE_OK;

		/* loop through texture layers */

		ts_TextureLayer *pTextureLayer = pTextureContext->TextureLayer;
		for (int i = 0; i < (signed) pTextureContext->TextureCount; i++, pTextureLayer++)
		{
			/* very first texture? */

			if (i == 0)
			{
				switch(pTextureLayer->u8Flags & (TEXTURE_IS_DIFFUSE_MAP | TEXTURE_IS_ENVIRONMENT_MAP))
				{
				case 0:
					/* normal texture. always use modulate method, which multiplies the texel by the incoming fragment. */
					/* in this case (very first texture layer), incoming fragment is base color, either lighted by */
					/* realtime lighting or by vertex colors. using a method other than modulate loses lighting info */

					pTextureLayer->u8Method = TS_METHOD_MODULATE;		// multiply fragment (lighting/gouraud result) by texel
					break;
				case TEXTURE_IS_DIFFUSE_MAP:
					/* diffuse map as base layer, use modulate so that it gets lighting info */

					pTextureLayer->u8Method = TS_METHOD_MODULATE;		// multiply fragment (lighting/gouraud result) by texel
					break;
				case TEXTURE_IS_ENVIRONMENT_MAP:
					/* environmental map as base layer, incoming fragment gets lighting info, interpolate method */
					/* blends texel with incoming fragment according to constant alpha, but they are not multiplied */

#if defined(WIN32) || defined(GCN)
					pTextureLayer->u8Method = TS_METHOD_INTERPOLATE;	// blend with incoming fragment based on constant alpha
#elif defined(PS2)
					ASSERT(false);	// should always be a base texture for environment maps. at least the white texture
					pTextureLayer->u8Method = TS_METHOD_MODULATE;		// on ps2, this works ok with white base color
#endif //WIN32
					break;
				default:
					ASSERT(false);	// unknown case
					pTextureLayer->u8Method = TS_METHOD_MODULATE;
					break;
				}
			}
			else if (nIndexThisPass == 0)
			{
				/* first texture layer of a second or later rendering pass */

				switch(pTextureLayer->u8Flags & (TEXTURE_IS_DIFFUSE_MAP | TEXTURE_IS_ENVIRONMENT_MAP))
				{
				case 0:
					/* normal texture. probably a decal, or a crossfade texture. use the modulate method so that we */
					/* get lighting info. it is possible that this method should be replace, if the artist is putting */
					/* down an almost transparent layer of something, in which case it probably shouldn't get lighting */
					/* info. but that seems the rare case */

					pTextureLayer->u8Method = TS_METHOD_MODULATE;		// multiply fragment (lighting/gouraud result) by texel
					break;
				case TEXTURE_IS_DIFFUSE_MAP:
					/* diffuse map going to be added to the frame buffer (typically multiplied, but maybe added or */
					/* subtracted). use texels as is */

					pTextureLayer->u8Method = TS_METHOD_REPLACE;			// use texel as is
					break;
				case TEXTURE_IS_ENVIRONMENT_MAP:
					/* cause resulting alpha (output from texture unit) to be the constant alpha, so it blends that */
					/* way into the framebuffer */

					pTextureLayer->u8Method = TS_METHOD_CONSTANTALPHA;	// becomes DECAL (same as REPLACE) on ps2

					/* on ps2, we can use a blend mode that uses a fixed alpha value instead of the source texel's alpha */
					/* this is good for blending a semi-transparent env map onto another texture, but it overwrites */
					/* the alpha calculated for fogging or cross-fading, so it may end up being an inappropriate technique. */
					/* but the alternative is ugly: to somehow tell the VU to push the current global alpha, multiply it */
					/* by some passed opacity value, and then restore it when the env map was finished. this would work, */
					/* but it means that we could never do semi-transparent env maps in a multi-texture context, since */
					/* each vertex only has one color that is passed to the gs, and the modified global alpha value would */
					/* be incorrect for the base texture layer. the even scarier fix to that is to go through an modify */
					/* all vertex colors before the second texture context xgkick. ugh. */

					/* if we knew that we were always in multi-pass instead possibly going to dual texture context, then */
					/* we could possibly do this by setting the correct alpha into the base color and re-sending the base */
					/* color. since the base color gets replicated to provide a color for each vertex, this would be */
					/* correctly multiplied by the fog/cross-fade alpha in the global ambient, and everything would be */
					/* happy. this is only valid for lighted models, though-- for vertex-lighted models you would have to */
					/* jam the alpha into all vertex colors */

					/* what we REALLY need on the ps2 is a way to multiply all vertex color alpha values by a constant alpha. */
					/* but even this is dead-on-arrival in a dual-texture-context scenario, since we would need a different */
					/* constant alpha per layer */

#ifdef PS2
					pTextureLayer->u8Blend = RS_BLENDFUNC_FIXEDALPHA;	// fixed alpha replaces texel alpha
//yuch-- use this to use alpha channel instead of overriding with opacity value
//pTextureLayer->u8Method = TS_METHOD_DECAL;
//pTextureLayer->u8Blend = RS_BLENDFUNC_DEFAULT;
#endif //PS2
					break;
				default:
					ASSERT(false);	// unknown case
					pTextureLayer->u8Method = TS_METHOD_MODULATE;
					break;
				}
			}
			else
			{
				/* second or greater texture layer of a multi-texture pass. texel will be combined with */
				/* fragment coming out of previous texture unit */

				switch(pTextureLayer->u8Flags & (TEXTURE_IS_DIFFUSE_MAP | TEXTURE_IS_ENVIRONMENT_MAP))
				{
				case 0:
					/* normal texture. probably a decal, or a crossfade texture */

					ASSERT(false);	// this is disallowed in check at top of routine
					pTextureLayer->u8Method = TS_METHOD_MODULATE;
					break;
				case TEXTURE_IS_DIFFUSE_MAP:
					/* diffuse map, multiply into incoming fragment */

					pTextureLayer->u8Method = TS_METHOD_MODULATE;		// multiply fragment (lighting/gouraud result) by texel
					break;
				case TEXTURE_IS_ENVIRONMENT_MAP:
					/* environmental map as second or greater layer in multitexture. interpolate method blends */
					/* texel with incoming fragment according to constant alpha, but they are not multiplied */

					pTextureLayer->u8Method = TS_METHOD_INTERPOLATE;	// blend with incoming fragment based on constant alpha
					break;
				default:
					ASSERT(false);	// unknown case
					pTextureLayer->u8Method = TS_METHOD_MODULATE;
					break;
				}
			}

#ifdef PRINT_TEXTURE_CONTEXT_FIXUP
			static char *WrapString[2] = {"Clamp ", "Repeat"};
			static char *FilterString[6] = {"point sample", "bilinear    ", "mipmap point", "mipmap pttri", "mipmap bilin", "mipmap triln"};
			static char *MethodString[6] = {"passthru  ", "decal     ", "replace   ", "modulate  ", "interpol. ", "constalpha"};
			dbgPrint("   Layer %d: wrap U: %s, wrap V %s, filter: %s, method: %s, bind %d, %s\n",
						i,
						WrapString[pTextureLayer->u8WrapU],
						WrapString[pTextureLayer->u8WrapV],
						FilterString[pTextureLayer->u8Filter],
						MethodString[pTextureLayer->u8Method],
						pTextureLayer->s16TextureBind,
						pTextureLayer->TextureName);
#endif //PRINT_TEXTURE_CONTEXT_FIXUP

			/* update pass/texture flags */

			if (++nIndexThisPass == nCountThisPass)
			{
				nIndexThisPass = 0;
				nCountThisPass = 1;	// for now, downshift to single-texture-per-pass after first pass
			}
		}
	}

	// set the name of the layer to match the original texture it points to
	static void SetLayerName( ts_TextureLayer* layer, const char* name )
	{
		// find end of path
		const char* p;
		for ( p = name + strlen( name ) ; p > name ; p-- )
			if ( p[-1] == '/' || p[-1] == '\\' || p[-1] == ':' )
				break;

		// copy filename, skip extension
		int len = 0;
		while ( *p && *p != '.' && len < (int)sizeof( layer->TextureName ) - 1 )
			layer->TextureName[ len++ ] = tolower( *p++ );
		layer->TextureName[ len ] = 0;
	}

/* do first-time-only fix up on texture context, like bind resolution */

	static void _FixTextureContextFirstTime(ts_TextureContext *pTextureContext)
	{
		ts_TextureLayer *pTextureLayer = pTextureContext->TextureLayer;
		for (int i = 0; i < (signed) pTextureContext->TextureCount; i++, pTextureLayer++)
		{
			/* we are no longer handling light maps */

			ASSERT(pTextureLayer->s16TextureIndex != LIGHT_MAP_TEXTURE);

			/* resolve texture bind */

			if (pTextureLayer->u8Flags & TEXTURE_USE_WHITE_TEXTURE)
			{
// 				ASSERT(TextureMgr.u16WhiteTextureBind != NO_BIND);	// make sure white texture has been downloaded
				pTextureLayer->textureDatabase = NULL;
				SetLayerName( pTextureLayer, "white" );
			}
			else
			{
				ASSERT(pTextureLayer->s16TextureIndex != TEXTURE_NOT_IN_DBL);
				pTextureLayer->textureDatabase = GetTextureDatabase( GetTextureDatabaseCount() - 1 );
				SetLayerName( pTextureLayer, pTextureLayer->textureDatabase->GetTexture( pTextureLayer->s16TextureIndex )->GetTextureName() );
			}

// #ifdef PS2
// 			/* for ps2, this is the place to set other texture attributes like wrap/clamp since the */
// 			/* models are converted without going through the PatchSOM__Render logic. these attributes */
// 			/* are stored on a per-texture basis. since these are texture-by-texture, force value to be */
// 			/* accepted so that textures that have the same value still get set correctly */

// 			TextureState__Request( 0, TS_BIND, pTextureLayer->Texture );	// point to texture to work on
// 			TextureState__Force( 0, TS_WRAPU );
// 			TextureState__Request( 0, TS_WRAPU, pTextureLayer->u8WrapU );
// 			TextureState__Force( 0, TS_WRAPV );
// 			TextureState__Request( 0, TS_WRAPV, pTextureLayer->u8WrapV );
// 			TextureState__Force( 0, TS_FILTER );
// 			TextureState__Request( 0, TS_FILTER, pTextureLayer->u8Filter );
// #endif //PS2
		}

#ifdef PS2
		/* adjust base color values for ps2-- if patch is textured, rgb values need to be half-range. alpha is always half-range */

		if (pTextureContext->TextureCount != 0)
		{
			pTextureContext->BaseColor.R = PS2_HALF_COLOR(pTextureContext->BaseColor.R);
			pTextureContext->BaseColor.G = PS2_HALF_COLOR(pTextureContext->BaseColor.G);
			pTextureContext->BaseColor.B = PS2_HALF_COLOR(pTextureContext->BaseColor.B);
		}
		pTextureContext->BaseColor.A = PS2_HALF_COLOR(pTextureContext->BaseColor.A);
#endif
	}

	void SetWorldTextureInfo( void)
	{
		pWorldTextureSet = GetLastLoadedDatabase( );
		pWorldTextureContextList = pLastTextureContextList;
	}

}

