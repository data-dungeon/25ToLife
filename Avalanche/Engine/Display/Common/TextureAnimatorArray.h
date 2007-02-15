
#ifndef __TEXTURE_ANIMATOR_ARRAY_H__
#define __TEXTURE_ANIMATOR_ARRAY_H__

#include "container/dynlist.h"
#include "Display/Common/TextureAnimator.h"


class TextureAnimatorArray : public DynList<TextureAnimator>
{
	TextureArray* pTextureArray;

public:
	TextureAnimatorArray(TextureArray* array);
	void AdjustTextures(void) NAMED_SECTION("text.rend1");
	void Update(s16 s16Milliseconds) NAMED_SECTION("text.rend1");

	TextureAnimator* GetOrCreateByTextureIndex(u32 index);
	TextureAnimator* GetOrCreateByTexture(Texture* targetTexture);

	TextureAnimatorArray* Clone(void);
};


#endif //__TEXTURE_ANIMATOR_ARRAY_H__
