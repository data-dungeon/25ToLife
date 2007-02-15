#include "Display/DisplayPCH.h"


#include "TextureAnimatorArray.h"

TextureAnimatorArray::TextureAnimatorArray(TextureArray* array)
	: pTextureArray(array)
{
}

void TextureAnimatorArray::AdjustTextures(void)
{
	u32 count = Count();
	for(u32 i = 0; i < count; i++)
	{
		Get(i)->AdjustTexture();
	}
}

void TextureAnimatorArray::Update(s16 s16Milliseconds)
{
	u32 count = Count();
	for(u32 i = 0; i < count; i++)
	{
		Get(i)->Update(s16Milliseconds);
	}
}

TextureAnimator* TextureAnimatorArray::GetOrCreateByTextureIndex(u32 index)
{
	return GetOrCreateByTexture((*pTextureArray)[(u16)index]);
}

TextureAnimator* TextureAnimatorArray::GetOrCreateByTexture(Texture* targetTexture)
{
	u32 count = Count();
	for(u32 i = 0; i < count; i++)
	{
		TextureAnimator* oldAnimator = Get(i);
		if(oldAnimator->GetTexture() == targetTexture)
			return oldAnimator;
	}

	TextureAnimator* newAnimator = new TextureAnimator(targetTexture);
	Add(newAnimator);
	return newAnimator;
}

TextureAnimatorArray* TextureAnimatorArray::Clone(void)
{
	TextureAnimatorArray* newArray = new TextureAnimatorArray(pTextureArray);
	u32 count = Count();
	for(u32 i = 0; i < count; i++)
	{
		TextureAnimator* oldAnimator = Get(i);
		newArray->Add(oldAnimator->Clone());
	}
	return newArray;
}

