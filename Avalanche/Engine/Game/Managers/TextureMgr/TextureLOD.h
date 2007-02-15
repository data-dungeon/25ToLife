
#ifndef __TEXTURE_LOD_H__
#define __TEXTURE_LOD_H__

class TextureLOD : public ReferenceCountable<u16>
{
protected:

	/// our owner
	Texture* mOwner;

	/// Image to use.
	Image* mImage;

	/// flags
	u16 mFlags;

	/// valid depth ranges - minimum plane where this lod should 
	/// be usable.
	float mNearDepth;

	/// valid depth ranges - maximum plane where this lod should
	/// be usable.
	float mFarDepth;

public:

};

#endif // __TEXTURE_LOD_H__
