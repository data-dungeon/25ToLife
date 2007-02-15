#pragma once

#ifndef IMAGETITANEXPORT
#define IMAGETITANEXPORT
#endif

#include "SDK/BufferType.h"
class ImageFormatEntry;
class BitmapBuffer;

//enum { 
class IMAGETITANEXPORT BitmapObject
{
public:
   enum BufferType{ 
      IMAGE_BUFFER    = 0,
      //DIFFUSE_BUFFER  = 1,
      //SPECULAR_BUFFER = 2,
      //AMBIANCE_BUFFER = 3,
      //DEPTH_BUFFER    = 4,
      //NORMAL_BUFFER   = 5,
      NUM_BUFFERS
   };

private:
   BitmapBuffer *m_bufferarray[NUM_BUFFERS];
   int m_nWidth, m_nHeight;
   float m_fAspect;
   String m_strFileName;

   void SetBuffer(BufferType nBufferType, BitmapBuffer *pBuffer);
   BitmapBuffer *CreateBuffer(BufferDataType nDataType, BufferChannels nBufferChannels);

   void LoadImage(ImageFormatEntry *pEntry, const String &strFileName);
   ImageFormatEntry *FindEntry(const String &strFileName);
   ImageFormatEntry *FindEntry(const String &strPluginClassName, const String &FormatClassName);

public:

//Constructors
   BitmapObject() {
      memset(&m_bufferarray, NULL, sizeof(void *) * NUM_BUFFERS);
      m_nWidth = m_nHeight = 0;
      m_fAspect = 1.0f;
   }
   ~BitmapObject() {
      FreeAllBuffers();
   }

//Operations
   void Init(int nWidth, int nHeight, float fAspect = 1.0f);

   int GetWidth() const { return m_nWidth; }
   int GetHeight() const { return m_nHeight; }
   float GetAspect() const { return m_fAspect; }

   const String &GetFileName() const { return m_strFileName; }

   void FreeAllBuffers();
   BitmapBuffer *GetBuffer(UINT nBufferIndex) { return m_bufferarray[nBufferIndex]; }

   BOOL QueryLoadImage();
   void LoadImage(const String &strFileName);
   void LoadImage(const String &strPluginClassName, const String &strFormatClassName, const String &strFileName);
   
   //Buffer Creations
   BitmapBuffer *CreateImageBuffer(BufferDataType nDataType, BufferChannels nBufferChannels);

   //Statics
   static UINT GetFormatCount();
};