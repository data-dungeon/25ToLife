#include "stdafx.h"
#include "HImageContainer.h"
#include "ImageContainer.h"

UINT HImageContainer::GetNumImages()
{
   return ((ImageContainer*)this)->GetNumImages();
}

HImage *HImageContainer::GetImageAt(UINT nIndex)
{
   return (HImage*)((ImageContainer*)this)->GetImageAt(nIndex);
}

