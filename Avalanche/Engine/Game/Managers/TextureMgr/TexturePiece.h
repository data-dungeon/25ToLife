

#ifndef __TEXTURE_PIECE_H__
#define __TEXTURE_PIECE_H__

class TexturePiece : public ReferenceCountable<u16>
{
protected:
	/// our owner
	Texture* mOwner;

public:
	s16	mOffX;			// in pixels
	s16	mOffY;			// in pixels
	s16	mWidth;  		// in pixels
	s16	mHeight; 		// in pixels
	s16	mHotSpotX;
	s16	mHotSpotY;
	u16	mPieces;	 	// for textures that have been cut up into smaller textures
};

#endif // __TEXTURE_PIECE_H__
