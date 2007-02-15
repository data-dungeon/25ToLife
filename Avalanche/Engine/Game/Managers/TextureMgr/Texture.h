

#ifndef __TEXTURE_H__
#define __TEXTURE_H__


/// @{
/// length of name field in structures, for debugging

#define TEXTURE_SET_NAME_LENGTH			32
#define TEXTURE_NAME_LENGTH				32
/// @}


#if defined (_XBOX)
#include "platform/Xbox/xbresource.h"
#endif

class Texture : public ReferenceCountable<u32>
{
protected:
	/// our owner
	TextureDatabase* mOwner;

	/// flags
	u32 mFlags;

	/// Image this texture currently uses.
	DynamicArray<Image*> mImages;
	
	/// Current image number.  Having this in the texture is lousy, but
	/// compatible with current code.
	int mImageNumber;

	/// name of this texture for debugging
	char mName[TEXTURE_NAME_LENGTH];

	/// the texture that is currently selected for rendering
	static Texture* sCurrentlySelected;

	/// the user data
	void* mUserData;

#ifdef _DEBUG
public:
	static void Testing(bool testing) { sIsTesting = testing; }
	static bool IsTesting(void) { return sIsTesting; }
protected:
	static bool sIsTesting;
#endif

public:

	enum {
		ANIMATED 		= 0x00000001,
		REFLECTS 		= 0x00000002,
//		VOLATILE 		= 0x00000004,
		PERMENANT		= 0x00000008,
		WRITE_ONLY		= 0x00000010,
		READ_ONLY		= 0x00000020,
		ENV_MAP			= 0x00000040,
		REF_MAP			= 0x00000080,

		MAP_SPACE_MSK	= 0x00000700,

		MAP_SPHERICAL	= 0x00000800,

		MAP_COORD0_MSK = 0x0000F000,
		MAP_COORD1_MSK = 0x000F0000,

		// internal flags
		SELECTED			= 0x40000000,
		RENDER_LOCKED	= 0x80000000,
	};

	/// construct an empty texture
	Texture(TextureDatabase* owner, const char* name);

	/// destroy it
	~Texture();

	/// Change the flags for this texture bind
	void SetFlags(u32 flags);

	/// Get the flags for this texture bind.
	u32 GetFlags(void) NAMED_SECTION("text.rend1");

	/// setup to draw with this texture.
	void Lock(void) NAMED_SECTION("text.rend1");

	/// cleanup from rendering with this texture.
	void Unlock(void) NAMED_SECTION("text.rend1");

	/// Setup to render to this texture.  It's set must be Lock()'ed already.
	void LockForRender(void);

	/// Cleanup from rendering to this texture.  It's set must still be Lock()'ed.
	void UnlockForRender(void);

	/// Select this texture for the next drawing operations.  Only one
	/// can be selected at a time, all subsequent selects are counted
	/// to match with unselects, but otherwise have no effect.  It is
	/// not ok to select the same image more than once however.
	void Select(void);
	/// unmark this texture for drawing operations.
	void Unselect(void);
	/// get the currently selected texture or NULL
	static Texture* GetSelectedTexture(void) { return sCurrentlySelected; }

	char*	GetTextureName() {	return mName; }

	/// Get the image associated with this texture.
	Image* GetImage(void) { return mImages[mImageNumber]; }

	void SetImage (Image* IP) { mImages[mImageNumber] = IP; }

	/// purge all iamges...
	void RemoveAllImages(void);
	
	/// remove a single image
	void RemoveImage(u32 index);

	/// add a new image into the array
	u32 AddImage(bool FromXpr, u16 flags, u16 format, u16 width, u16 height, void* data, u16 palFmt = 0, u16 palWidth = 0, u16 palHeight = 0, void* palette = NULL);

	
	#ifdef MASSIVE_AD
	u32 AddPlatformImage(bool FromXpr, u16 flags, u16 format, u16 width, u16 height, void* data, u16 palFmt = 0, u16 palWidth = 0, u16 palHeight = 0, void* palette = NULL);
	#endif


	void AppendImageMipLevel(u32 imageidx, u16 format, u16 width, u16 height, void* data, u16 palFmt = 0, u16 palWidth = 0, u16 palHeight = 0, void* palette = NULL);

	/// initialize with new image
	u32 AddImageFromDBL(ts_TextureChunkRecord* record);
	u16 DblFlagsToFormat(u32 flags);

	#if defined (_XBOX)
	void checkEnvMapping( const char * pTextureName );
	u32 AddImageFromXPR(ts_TextureChunkRecord* record, CXBPackedResource *rsc, int i);
	#endif

	// these are currently used by the particle manager to track guesses into the particle list
	// for where to create particles.
	void* GetUserData(void) { return mUserData; }
	void SetUserData(void* data) { mUserData = data; }

	/// ick, for legacy code
	void SetCurrentFrame(u16 frame);

	/// how many images
	u16 GetImageCount(void);

	// copy this texture into the destination.  Must be selected
	void BitBlt(int x, int y, int w, int h);

	// copy this texture's G into the destination A.  Must be selected
	void BltGtoA(int x, int y, int w, int h);

	/// set to some percentage of the way through the animation.
	/// the whole counts are if the first and last image get a range of
	/// percentages or only 0 & 100...
	void SetCurrentFrameByPercentage(float percent, bool firstWholeCount = true, bool lastWholeCount = true);

#ifdef _DEBUG
	void VerifyThatNoImagesAreLocked(void);
#endif

	/// return allocated size
	int  AllocatedSize();
};

#endif // __TEXTURE_H__
