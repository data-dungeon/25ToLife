//

#include "stdafx.h"
#include "HGLImageList.h"
#include "GUI/GLImageList.h"

HGLImageList *HGLImageList::New( UINT nBitmapID, int cx/*=0*/, HINSTANCE hInstance/*=NULL*/)
{
   GLImageList *pImageList;
   {
      AFX_MANAGE_STATE(AfxGetAppModuleState());
      pImageList = SNEW GLImageList;
   }
   pImageList->Create( nBitmapID, cx, hInstance );

   return (HGLImageList *)pImageList;
}

void HGLImageList::operator delete (void *pVoid)
{
   delete (GLImageList *)pVoid;
}

BOOL HGLImageList::Draw(float x/*=0.0f*/, float y/*=0.0f*/, UINT nImage/*=0*/, float fAlpha/**1.0f*/ )
{
   return ((GLImageList *)this)->Draw(x, y, nImage, fAlpha);
}

BOOL HGLImageList::IsEmpty() const
{
   return ((GLImageList *)this)->IsEmpty();
}

UINT HGLImageList::GetImageCount() const
{
   return ((GLImageList *)this)->GetImageCount();
}

UINT HGLImageList::GetWidthEach() const
{
   return ((GLImageList *)this)->GetWidthEach();
}

UINT HGLImageList::GetWidth() const
{
   return ((GLImageList *)this)->GetWidth();
}

UINT HGLImageList::GetHeight() const
{
   return ((GLImageList *)this)->GetHeight();
}
