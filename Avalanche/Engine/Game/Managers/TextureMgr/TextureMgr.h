
#ifndef __TEXTURE_MGR_H__
#define __TEXTURE_MGR_H__

#include "platform/CompilerControl.h"
#include "container/Darray.h"
#include "EngineHelper/drawutil.h"

class Image;
class Texture;
class TextureLOD;
class TexturePiece;
class TextureArray;
class TextureDatabase;
struct ts_DBChunkHdr;		// in Game/Database/DBFile.h
struct ts_RecordList;		// in Game/Database/DBFile.h
struct ts_TextureLayer;

#ifdef PS2
//#define TRACK_TEXTURE_MEMORY
#endif

template <class T>
class ReferenceCountable
{
protected:
	T mRefCount;
public:
	ReferenceCountable()
	{
		mRefCount = 0;
	}
	void AddReference()
	{
		ASSERT(mRefCount != ~(0xFFFFFFFF << (sizeof(T)*8)));
		mRefCount++;
	}
	void RemoveReference()
	{
		mRefCount--;
		if(mRefCount == 0){
			delete this;
		}
	}
};

template <class T>
class AutoPtr
{
protected:
	T* t;

public:
	AutoPtr(const AutoPtr<T>& it)	{ *this = it; }
	AutoPtr(T* it = NULL)
	{
		t = it;
		if(t) t->AddReference();
	}
	~AutoPtr()
	{
		if(t) t->RemoveReference();
	}
	AutoPtr<T>& operator=(T* it)
	{
		if(t) t->RemoveReference();
		t = it;
		if(t) t->AddReference();
		return *this;
	}
	operator T* () { return t; }
	T* operator->(){ return t; }
	T& operator*(){ return *t; }
	T& operator[](int index){ ASSERT(index == 0); return *t; }
	bool IsValid(void) { return t != NULL; }
};

#include "TextureDbl.h"
#include "Image.h"
#include "Texture.h"
#include "TextureLOD.h"
#include "TexturePiece.h"
#include "TextureArray.h"
#include "TextureDatabase.h"
#include "TextureCache.h"

namespace TextureMgr
{
	enum {
		ADVANCE_ENABLED = 0x00000001,
	};

	void Init(void);
	void Shutdown(void);

	void Pause(void);
	void Resume(void);
	bool IsPaused(void);

	void SetFlag(u32 flag);
	void ClearFlag(u32 flag);
	bool IsFlagSet(u32 flag);

	u16 GetTextureDatabaseCount(void);
	TextureDatabase* AddTextureDatabase(const char* Filename,  void* Dx, void *pChunkData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize);
	void RemoveTextureDatabase(TextureDatabase*  tdb);
	TextureDatabase* GetTextureDatabase(int index);
	TextureCache& Cache() NAMED_SECTION("text.rend1");

	void DisableTextureTransfers();
	void EnableTextureTransfers();
	bool AllowTextureTransfers() NAMED_SECTION("text.rend1");

	u16 GetTextureDatabaseIndex(TextureDatabase* tdb);

	/// TODO: refactor this function out of existance.
	/// this should not be handled this way, its sloppy.
	TextureDatabase* GetLastLoadedDatabase(void);

//	/// TODO: refactor this function out of existance.
//	/// This should definitely not be handled this way.  This function
//	/// gets a texture array from the last loaded som textureset chunk.
//	TextureArray* GetRecentTextureArray(u16 arrayIndex);

	// TJC - TODO Deal with replacing

	/// Called at the start of every frame regardless of pause state, etc.
	void NewFrame(void);

	/// Called at the start of every frame when we are not paused.
	/// Manages texture animation and things like that.
	void Advance(float deltaTime);

	/// create a temporary texture&bind for various special effects.  this is
	/// what used to be a RealTime Texture Set in the old texture manager.
	Texture* CreateSpecialTexture(u16 textureFlags, u16 imageFlags, u16 imageFormat, u16 width, u16 height, const char* name);

	/// @{
	/// special bind handling
	Texture* GetWhiteTexture(void);
	Texture* GetFrontBuffer(void);
	Texture* GetBackBuffer(void);
	Texture* GetZBuffer(void);
	Texture* GetDynamicBuffer(void);

	enum CopyMode
	{
		COPY_NORMAL = 0,
		COPY_GREYSCALE = 1,
		COPY_NEGATIVE = 2
	};

	Texture* CopyBackBuffer(float scale = 1.0f, CopyMode mode = COPY_NORMAL, bool canUseZBufferSpace = false);
	Texture* CopyFrontBuffer(float scale = 1.0f, CopyMode mode = COPY_NORMAL, bool canUseZBufferSpace = false);

	void GetBackBufferTextureScaling(float &uScale, float &vScale);
	void GetFrontBufferTextureScaling(float &uScale, float &vScale);

	void RequireDynamicBuffer(u32 bytesize);
	/// @}

	/// @{
	/// To avoid doing shaders and having to rewrite EVERYTHING, I have
	/// just implimented texture contexts pretty much vertbatim.  This
	/// should really be changed to a shader system. My quick bit of
	/// research says that this is a BIG can of worms.
	bool LoadTextureContexts(void *pChunkData, ts_DBChunkHdr *pChunkHdr, void **ppRetData, u32 *pu32DataSize);
	void SetLastTextureContextList(ts_RecordList* psTextureContextList);
	ts_TextureLayer *GetTextureContextLayer(int nContext, int nLayer);

	void SetWorldTextureInfo( void);
	/// @}

	/// @{
	/// these dont belong as part of the texture manager... I havent
	/// impliemtned them yet because they are not critical, and dont
	/// belong here.
	inline void DrawColoredBlock(int nX, int nY, DrawUtility::COLOR Color) {}
	inline void DrawNextColoredBlock(DrawUtility::COLOR Color = DrawUtility::NODRAW) {}
	inline void DrawNextColoredBlockNewColor(DrawUtility::COLOR Color) { DrawNextColoredBlock(Color); }
	inline void ResetColoredBlock() {}
	inline void ColoredBlockNewline() {}
	inline void ColoredBlockColor(DrawUtility::COLOR Color) {}
	/// @}

	/// @{
	/// these help to protect us from trying to render to two images at
	/// once.
	Image* GetRenderDestination(void);
	void SetRenderDestination(Image* img);
	/// @}

#ifdef TRACK_TEXTURE_MEMORY
	void AddUploadedImage(PlatformImage* image);
	void RemoveUploadedImage(PlatformImage* image);
	void Release(PlatformImage* image);
	bool ReleaseLRUImage(void);
	void TouchUploadedImage(PlatformImage* image);
	void VerifyEmptyTextureMemory(void);
	void EmptyTextureMemory(void);

	// This is to notify the texture manager that something (fmvs) just
	// trashed texture memory.
	void FlushTextureMemory(void);
#endif

}

#endif // __TEXTURE_MGR_H__
