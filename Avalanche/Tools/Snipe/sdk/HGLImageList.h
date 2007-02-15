#pragma once

#ifndef SNIPEEXPORT
#define SNIPEEXPORT
#endif

class SNIPEEXPORT HGLImageList
{
public:
   static HGLImageList *New( UINT nBitmapID, int cx=0, HINSTANCE hInstance=NULL);
   void operator delete (void *pVoid);

   BOOL Draw(  float x=0.0f, float y=0.0f, UINT nImage=0, float fAlpha=1.0f );

   BOOL IsEmpty() const;
   UINT GetImageCount() const;
   UINT GetWidthEach() const;
   UINT GetWidth() const;
   UINT GetHeight() const;
};