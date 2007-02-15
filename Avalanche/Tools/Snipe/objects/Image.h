#pragma once
#include "BitmapObject.h"
#include "HierObject.H"
#include "../Properties/Property.h"

#define GLTEXTURETYPE GL_TEXTURE_2D
//#define GLTEXTURETYPE GL_TEXTURE_RECTANGLE_ARB

class Image : public HierObject
{
public:
   static FileInfoProperty::Info m_fileinfoinfo;

   FileInfoProperty m_fileinfo;

private:
   BitmapObject m_bitmapobject;
   GLuint m_glTexture[BitmapObject::NUM_BUFFERS];

   BOOL CreateGLTexture( UINT nBufferIndex );

public:
   DECLARE_SNIPEOBJECT_CREATE(Image, HierObject)
   DECLARE_COMMANDOBJECT()

            Image();
   virtual ~Image();

// Operations
   GLuint GetGLTexture( UINT nBufferIndex );

   int GetWidth() { return m_bitmapobject.GetWidth(); }
   int GetHeight() { return m_bitmapobject.GetHeight(); }

   BOOL LoadImage( const String &strFileName );
   BOOL QueryLoadImage();

   const String &GetFilename() { return m_bitmapobject.GetFileName(); }

   void Draw( CGLControl *pControl, const Vector &vPos, const Vector2 &v2Size, float fOpacity=1.0f );

// HierObject overrides
   virtual CSnipeDocTemplate *GetDocTemplate();

// InstanceObject overrides
   virtual String GetFileName();
   virtual void Draw( CGLBaseView *pView );
   virtual void DrawTreeItem( CGLControl *pItem, float x=0.0f );
   virtual int GetItemHeight();

// SnipeObject overrides
   virtual UINT GetDefaultMenuItemID();
   virtual FileInfoProperty *GetFileInfo() { return &m_fileinfo; }
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
   virtual catch_msg void OnEndObjectLoad();
};
