#pragma once
#include "ContainerObject.h"

class Image;

class ImageContainer : public ContainerObject
{
public:
   DECLARE_SNIPEOBJECT_CREATE(ImageContainer, ContainerObject)

            ImageContainer();
   virtual ~ImageContainer();

   UINT GetNumImages() const { return GetNumChildren(); }
   Image *GetImageAt( UINT nIndex ) { return (Image*)GetChildAt(nIndex); }

// SnipeObject overrides
   virtual BOOL IsDeletable() { return FALSE; }
   virtual BOOL Load( BaseStream &bs );
};