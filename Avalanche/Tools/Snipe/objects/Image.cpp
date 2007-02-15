//
#include "stdafx.h"
#include "Image.h"
#include "BitmapBuffer.h"
#include "../GUI/MainFrm.h"
#include "../GUI/Snipe.h"
#include "Interface/Implement.h"
#include "Interface/SelectionList.hpp"
#include "resource.h"

IMPLEMENT_SNIPEOBJECT_CREATE( Image )
IMPLEMENT_COMMANDOBJECT( Image )

FileInfoProperty::Info Image::m_fileinfoinfo( "File Information", "FileInfo", "Untitled.snipeimage", "Image File (*.snipeimage)|*.snipeimage||", "Image");

Image::Image()
{
   memset( m_glTexture, 0, sizeof(GLuint) * BitmapObject::NUM_BUFFERS );

   m_fileinfo.Init( &m_fileinfoinfo, this );
}

Image::~Image()
{
   if (m_glTexture)
   {
      for (int i=0; i<BitmapObject::NUM_BUFFERS; i++)
      {
         if (m_glTexture[i] && m_glTexture[i] != -1)
            glDeleteTextures( 1, &m_glTexture[i] );
      }
   }
}

void Image::PreDestruct()
{
   m_commandlist.Empty();
   HierObject::PreDestruct();
}

//String Image::GetTitle() const
//{
//   return m_bitmapobject.GetFileName().GetNameOnly();
//}

String Image::GetFileName()
{
   return m_bitmapobject.GetFileName();
}

CSnipeDocTemplate *Image::GetDocTemplate()
{
   return GetApp().GetImageDocTemplate();
}

BOOL Image::LoadImage( const String &strFileName )
{
   m_fileinfo.m_strFileName.StoreValue( m_bitmapobject.GetFileName() );

   return TRUE;
}

BOOL Image::QueryLoadImage()
{
   try
   {
      if (m_bitmapobject.QueryLoadImage())
         m_fileinfo.m_strFileName.StoreValue( m_bitmapobject.GetFileName() );
      return TRUE;
   }
   catch(const String &strError)
   {
      g_outputWindow << strError << "\n";
      return FALSE;
   }
}

BOOL Image::CreateGLTexture( UINT nBufferIndex )
{
   ASSERT(m_glTexture[nBufferIndex]==0);

   BitmapBuffer *pBuffer = m_bitmapobject.GetBuffer( nBufferIndex );
   if (!pBuffer)
   {
      m_glTexture[nBufferIndex] = -1;
      return false;
   }

   GLContextPusher pusher( &GetMainFrame()->m_glSharedRenderWnd );

   glEnable( GLTEXTURETYPE );
	glGenTextures( 1, &m_glTexture[nBufferIndex] );
   glBindTexture( GLTEXTURETYPE, m_glTexture[nBufferIndex] );
	glTexParameteri( GLTEXTURETYPE, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GLTEXTURETYPE, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

   //if (m_bTileU)
      glTexParameteri(GLTEXTURETYPE, GL_TEXTURE_WRAP_S, GL_REPEAT);
   //else
   //   glTexParameteri(GLTEXTURETYPE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER_SGIS);

   //if (m_bTileV)
      glTexParameteri(GLTEXTURETYPE, GL_TEXTURE_WRAP_T, GL_REPEAT);
   //else
   //   glTexParameteri(GLTEXTURETYPE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER_SGIS);

   void *pData = pBuffer->GetData();
   
   int nExponent = fast_round( log( (double)m_bitmapobject.GetWidth() ) / log(2.0) );
   int nWidth = (int)pow( 2.0, nExponent );
   nExponent = fast_round( log( (double)m_bitmapobject.GetHeight() )/log(2.0) );
   int nHeight = (int)pow( 2.0, nExponent );

   GLint nMaxSize = 256;
   glGetIntegerv( GL_MAX_TEXTURE_SIZE, &nMaxSize );
   if (nWidth > nMaxSize)
      nWidth = nMaxSize;
   if (nHeight > nMaxSize)
      nHeight = nMaxSize;

   if (nWidth!=m_bitmapobject.GetWidth() || nHeight!=m_bitmapobject.GetHeight())
   {
      void *pScaledData = SNEW GLubyte[nWidth * nHeight * pBuffer->GetNumChannels()];
      if (gluScaleImage( pBuffer->GetBufferChannels(), m_bitmapobject.GetWidth(), m_bitmapobject.GetHeight(),
       pBuffer->GetBufferDataType(), pBuffer->GetData(), nWidth, nHeight, pBuffer->GetBufferDataType(), pScaledData ))
      {
         delete [] pScaledData;
         return FALSE;
      }
      pData = pScaledData;
   }

   glTexImage2D( GLTEXTURETYPE, 0, pBuffer->GetNumChannels(), nWidth, nHeight,
      0, pBuffer->GetBufferChannels(), pBuffer->GetBufferDataType(), pData );

   if (pData != pBuffer->GetData())
      delete [] pData;

   glDisable( GLTEXTURETYPE );

   return TRUE;
}

GLuint Image::GetGLTexture(UINT nBufferIndex)
{
   if (m_glTexture[nBufferIndex]==0)
   {
      if (!CreateGLTexture(nBufferIndex))
         return -1;
   }
   return m_glTexture[nBufferIndex];
}

UINT Image::GetDefaultMenuItemID()
{
   return ID_OBJECT_EDIT;
}

BOOL Image::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
      case NOTIFYOBJ_VALUECHANGED:
      {
         int nStoreUndo = (int)wParam;
         Property *pProperty = (Property *)lParam;
         if (pProperty == &m_fileinfo.m_strFileName)
         {
            String strFileName(m_fileinfo.m_strFileName.GetValue());
            if (nStoreUndo != 2)
            {
               BOOL bChanged;
               ValidateFileName(strFileName, &bChanged);
               if (bChanged)
               {
                  m_fileinfo.m_strFileName.StoreValue(strFileName, GetTime(), nStoreUndo);
                  return TRUE;
               }

               try
               {
                  m_bitmapobject.LoadImage( strFileName );
                  //m_fileinfo.m_strFileName.StoreValue( m_bitmapobject.GetFileName() );
               }
               catch (const String &msg)
               {
                  g_outputWindow << "<E>" << msg << "\n";
                  return FALSE;
               }
            }
            SetName( strFileName.GetNameOnly() );

         }
         break;
      }
   }

   return HierObject::OnObjectNotify( msg, wParam, lParam );
}

void Image::OnEndObjectLoad()
{
   if (!m_bitmapobject.GetBuffer(BitmapObject::IMAGE_BUFFER))
      Destroy();
   else
      HierObject::OnEndObjectLoad();
}
