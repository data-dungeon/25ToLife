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
// ned says: I am piggybacking the texture transform stuff
// into this class. so a TextureAnimator record can be for
// texture animation, texture transform, or both. there is
// a new routine, TextureMgr__NewTextureAnimator() that
// sets up a TextureAnimator intended only for texture
// transform use. the reason that i am not creating a
// separate class for texture transform is that it would
// require every instance to have a new pointer, when very
// few instances use either texture animation or transform

/******************* includes ****************************/

#include "Display/DisplayPCH.h"

/******************* local variables *********************/

/* static class variables */

bool TextureAnimator::bActiveDefault = true;

/////////////////////////////////////////////////
//
// public interface for this class
//
/////////////////////////////////////////////////

// construction/destruction
TextureAnimator::TextureAnimator(Texture* pTexture)
	: mTexture(pTexture), u16CurrentFrame(0), s16TimerInit(ANIMATION_SPEED_STOPPED), s16Timer(ANIMATION_SPEED_STOPPED)
{
	ASSERT_PTR(mTexture);

	/* clear texture transform */

	fU = 0.0f;
	fV = 0.0f;
	fUScale = fVScale = 1.0f;
	fRotationRadians = 0.0f;
	fUVel = 0.0f;
	fVVel = 0.0f;
	fRotationVelRadians = 0.0f;

	/* flags initial state */

	u16Flags = 0;
	if (bActiveDefault)
		u16Flags |= TEXTURE_ANIMATOR_ACTIVE;

	SetCurrentFrame(0);
}

TextureAnimator::~TextureAnimator()
{
	// cleanup platform specific variables

}

TextureAnimator* TextureAnimator::Clone(void)
{
	TextureAnimator* newAnimator = new TextureAnimator(mTexture);

	newAnimator->s16Timer = s16Timer;
	newAnimator->s16TimerInit = s16TimerInit;
	newAnimator->u16CurrentFrame = u16CurrentFrame;
	newAnimator->u16Flags = u16Flags;
	newAnimator->fU = fU;
	newAnimator->fV = fV;
	newAnimator->fUScale = fUScale;
	newAnimator->fVScale = fVScale;
	newAnimator->fRotationRadians = fRotationRadians;
	newAnimator->fUVel = fUVel;
	newAnimator->fVVel = fVVel;
	newAnimator->fRotationVelRadians = fRotationVelRadians;

	return newAnimator;
}

// update called exactly once per frame.
void TextureAnimator::Update(s16 s16Milliseconds)
{
	/* bail if not active */

	if (IsActive() == false)
		return;

	u16 currentFrame = GetCurrentFrame();

	ASSERT(s16TimerInit != ANIMATION_SPEED_UNKNOWN);	// if this hits, CInstance::MakeInstance created this but nothing filled in desired animation rate
	if (s16TimerInit != ANIMATION_SPEED_STOPPED)
	{
		// adjust timer
		s16Timer -= s16Milliseconds;

		// find correct frame
		while(s16Timer <= 0)
		{
			s16Timer += s16TimerInit;
			currentFrame++;
		}
	}

	// change to that frame...
	SetCurrentFrame(currentFrame);

	/* update texture transform stuff too */

	if (HasTextureTransform())
	{
		float fSeconds = (float) s16Milliseconds / 1000.0f;
		fU += fUVel * fSeconds;
		if (fU > 1.0f)
			fU -= 1.0f;
		else if (fU < 0.0f)
			fU += 1.0f;
		fV += fVVel * fSeconds;
		if (fV > 1.0f)
			fV -= 1.0f;
		else if (fV < 0.0f)
			fV += 1.0f;
		fRotationRadians = Math::NormalAngle(fRotationRadians + (fRotationVelRadians * fSeconds));
	}

	AdjustTexture();
}

void TextureAnimator::SetCurrentFrame(u16 u16NewFrame)
{
	// deal with wrapping of index
	{
		while(u16NewFrame >= GetTextureCount())
		{
			// why is u16TextureCount declared as a u32?
			u16NewFrame -= (u16)GetTextureCount();
		}
	}

	// adjust index
	u16CurrentFrame = u16NewFrame;
}

/* set the TEXTURE_ANIMATOR_HAS_TEXTURE_TRANSFORM flag correctly */

void TextureAnimator::CheckTextureTransformEnable()
{
	if ((fU != 0.0f) ||
		 (fV != 0.0f) ||
		 (fUScale != 0.0f) ||
		 (fVScale != 0.0f) ||
		 (fRotationRadians != 0.0f) ||
		 (fUVel != 0.0f) ||
		 (fVVel != 0.0f) ||
		 (fRotationVelRadians != 0.0f))
	{
		u16Flags |= TEXTURE_ANIMATOR_HAS_TEXTURE_TRANSFORM;
	}
	else
		u16Flags &= ~TEXTURE_ANIMATOR_HAS_TEXTURE_TRANSFORM;
}

/////////////////////////////////////////////////
//
// internal interface for this class
//
/////////////////////////////////////////////////



void TextureAnimator::AdjustTexture(void)
{
	mTexture->SetCurrentFrame(u16CurrentFrame);
}


/* calculate and return the texture transform matrix. this is not needed every frame for every animator, so only
calculate when requested */

Graphics4x4 *TextureAnimator::GetTextureTransform()
{
	/* build texture transform matrix */

	static Graphics4x4 mTextureTransform;	// pointer to this gets returned

	mTextureTransform.SetScale(fUScale, fVScale, 1.0f);
	if (fRotationRadians != 0.0f)
		mTextureTransform.CatZRotation(fRotationRadians);	// sets upper 2 x 2 sub-matrix

	/* add translation. ps2 only looks at upper 3 x 3 sub-matrix */

#if defined( PS2 ) || defined( DIRECTX )
	Vector3 vRow2(fU, fV, 1.0f);
	mTextureTransform.SetRow2(vRow2);
#else //PS2
	mTextureTransform.PokeXTranslation(fU);
	mTextureTransform.PokeYTranslation(fV);
#endif //PS2

	/* done */

	return(&mTextureTransform);
}

