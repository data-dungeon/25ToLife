#pragma once
#include "HContainerObject.h"

class HImage;

class SNIPEEXPORT HImageContainer : public HContainerObject
{
public:
   UINT GetNumImages();
   HImage *GetImageAt(UINT nIndex);
};