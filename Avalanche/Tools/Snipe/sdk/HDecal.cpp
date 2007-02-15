#include "stdafx.h"
#include "HDecal.h"
#include "Decal.h"

#if 0
HDecalMap *HDecal::AddMap(const char *name)
{
   return (HDecalMap*)((Decal*)this)->AddMap(name);
}

HDecalMap *HDecal::AddMap(HImage *pImage)
{
   return (HDecalMap*)((Decal*)this)->AddMap((Image*)pImage);
}
#endif

HDecalStamp *HDecal::AddStamp(const char *name)
{
   return (HDecalStamp*)((Decal*)this)->AddStamp(name);
}