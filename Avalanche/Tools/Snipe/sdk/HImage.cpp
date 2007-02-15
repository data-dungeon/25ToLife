#include "stdafx.h"
#include "HImage.h"
#include "Image.h"

int HImage::GetWidth()
{
   return ((Image *)this)->GetWidth();
}

int HImage::GetHeight()
{
   return ((Image *)this)->GetHeight();
}

String HImage::GetFileName()
{
   return ((Image *)this)->GetFileName();
}