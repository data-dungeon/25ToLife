//
// class TextureAnimator
//
// Original author: Tyler Colbert
// Copyright 2001, Avalanche Software, All Rights Reserved
//

//
// This is a little class for managing the state of a texture
// that is being animated.  It tracks when it needs to be
// updated, handles determining which subtexture is current,
// and (for the PS2) manages the display list used to upload
// the texture, and handles making it current.
//

#ifndef __TEXTURE_ANIMATOR_H__
#define __TEXTURE_ANIMATOR_H__

/******************* includes ****************************/

/* system includes */

/* engine includes */

#include "platform/BaseType.h"
#include "Game/Managers/TextureMgr/TextureMgr.h"
#include "Math/Matrix.h"

/******************* forward declarations ****************/


/******************* defines *****************************/

#define ANIMATION_SPEED_STOPPED	-1
#define ANIMATION_SPEED_UNKNOWN	0x7fff	// make it positive so we don't end up in an infinite loop in TextureAnimator::Update

#define TEXTURE_ANIMATOR_HAS_TEXTURE_TRANSFORM	0x0001
#define TEXTURE_ANIMATOR_ACTIVE						0x0002

/******************* compile control *********************/

/******************* macros ******************************/

class TextureAnimator
{
	/////////////////////////////////////////////////
	//
	// public interface for this class
	//
	/////////////////////////////////////////////////
public:
	// construction/destruction
	// you should have the texture manager create these for you
	// by calling TextureMgr__NewTextureAnimator() so that it
	// can call update for you and allow LockTextureSet()
	// to work for the ps2...
	TextureAnimator(Texture* pTexture);
	~TextureAnimator(void);

	TextureAnimator* Clone(void);

	// update called exactly once per frame.
	void Update(s16 s16Milliseconds);

	// get the texture set this animator is working in

	// getting/setting the current subtexture...
	Texture* GetTexture() 											{ return mTexture; }
	u32 GetTextureCount(void)										{ return GetTexture()->GetImageCount(); }
	void SetCurrentFrame(u16 u16NewFrame);
	u16 GetCurrentFrame(void)										{ return u16CurrentFrame; }

	// adjusting playback rate...
	// fps is set to as close as the TextureAnimator supports (theres a little inaccuracy)
	// gets reflect the TextureAnimator's inaccuracy.
	void SetFPS(float fps)											{
		if(Math::Zero(fps))
			s16TimerInit = (s16) ANIMATION_SPEED_STOPPED;
		else
			s16TimerInit = (s16)(1000.0f / fps);
	}
	float GetFPS(void)												{ return 1000.0f / s16TimerInit; }
	void SetTimer(s16 s16Milliseconds)							{ s16Timer = s16Milliseconds; }

	// active state
	bool IsActive()												{ return((u16Flags & TEXTURE_ANIMATOR_ACTIVE) != 0); }
	void SetActive(bool bActive)								{ if (bActive) u16Flags |= TEXTURE_ANIMATOR_ACTIVE; else u16Flags &= ~TEXTURE_ANIMATOR_ACTIVE; }
	static void SetActiveDefault(bool bActive)			{ bActiveDefault = bActive; }


	// activate changes
	void AdjustTexture(void);

	/* access to texture transform info */

	bool HasTextureTransform()									{ return((u16Flags & TEXTURE_ANIMATOR_HAS_TEXTURE_TRANSFORM) != 0); }
	void CheckTextureTransformEnable();
	void SetTextureTransformU(float fValue)				{ fU = fValue; CheckTextureTransformEnable(); }
	void SetTextureTransformV(float fValue)				{ fV = fValue; CheckTextureTransformEnable(); }
	void SetTextureTransform(float fValueU, float fValueV){ fU = fValueU; fV = fValueV; CheckTextureTransformEnable(); }
	void SetTextureTransformUScale(float fValue)			{ fUScale = fValue; CheckTextureTransformEnable(); }
	void SetTextureTransformVScale(float fValue)			{ fVScale = fValue; CheckTextureTransformEnable(); }
	void SetTextureTransformScale(float uScale, float vScale){ fUScale = uScale; fVScale = vScale; CheckTextureTransformEnable(); }
	void SetTextureTransformRotation(float fValue)		{ fRotationRadians = fValue; CheckTextureTransformEnable(); }
	void SetTextureTransformUVel(float fVel)				{ fUVel = fVel; CheckTextureTransformEnable(); }
	void SetTextureTransformVVel(float fVel)				{ fVVel = fVel; CheckTextureTransformEnable(); }
	void SetTextureTransformRotationVel(float fVel)		{ fRotationVelRadians = fVel; CheckTextureTransformEnable(); }
	Graphics4x4 *GetTextureTransform();

	/////////////////////////////////////////////////
	//
	// platform specific public interface
	//
	/////////////////////////////////////////////////
public:

	/////////////////////////////////////////////////
	//
	// internal interface for this class
	//
	/////////////////////////////////////////////////
protected:
	/////////////////////////////////////////////////
	//
	// data used to manage the texture
	//
	/////////////////////////////////////////////////
protected:
	// time and frame
	s16		s16Timer;					// timer to next advance, in milliseconds
	s16		s16TimerInit;				// refresh value of timer, in milliseconds (calculated from FPS)
	u16		u16CurrentFrame;			// current image being shown
	u16		u16Flags;

	// which texture to animate-- point to animated texture chunk
	Texture* mTexture;

	/* this data structure also handles texture motion via the texture matrix. this is not texture animation, since only */
	/* one texture is used (though i suppose both texture animation and the texture matrix could be combined). the uv */
	/* coordinates are passed through the texture matrix to yield new uv coords-- this can be used to have a texture */
	/* "slide" through geometry */

//	bool bHasTextureTransform;			// true if texture matrix is active (non-identity)
	float fU;								// current u-translation, 0.0 to 1.0
	float fV;								// current v-translation, 0.0 to 1.0
	float fUScale, fVScale;				// Scaling!
	float fRotationRadians;				// current rotation, in radians
	float fUVel;							// how fast to scroll in "u" direction, 1.0 means scroll through entire texture in a second
	float fVVel;							// how fast to scroll in "v" direction
	float fRotationVelRadians;			// how fast to rotate, in radians/second

	/* default state for new texture animators */

	static bool bActiveDefault;

	/////////////////////////////////////////////////
	//
	// platform specific internal data
	//
	/////////////////////////////////////////////////
protected:


};


#endif //__TEXTURE_ANIMATOR_H__
