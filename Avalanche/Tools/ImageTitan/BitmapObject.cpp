//
#include "stdafx.h"
#include "ImageTitan.h"
#include "BitmapObject.h"
#include "BitmapBuffer.h"

extern CImageTitanApp g_theApp;

UINT BitmapObject::GetFormatCount()
{
   return (UINT)g_theApp.m_registration.GetCount();
}

void BitmapObject::Init(int nWidth, int nHeight, float fAspect)
{
   FreeAllBuffers();

   m_nWidth  = nWidth;
   m_nHeight = nHeight;
   m_fAspect = fAspect;
}

void BitmapObject::FreeAllBuffers()
{
   for (int i=0; i<NUM_BUFFERS; i++) {
      if (m_bufferarray[i])
         delete m_bufferarray[i];
      m_bufferarray[i] = NULL;
   }
}

void BitmapObject::SetBuffer(BufferType nBufferType, BitmapBuffer *pBuffer)
{
   ASSERT(nBufferType < NUM_BUFFERS);
   ASSERT(pBuffer);

   if (m_bufferarray[nBufferType])
      delete m_bufferarray[nBufferType];

   m_bufferarray[nBufferType] = pBuffer;
}

// this is lame, there has got to be a better soluton.
BitmapBuffer *BitmapObject::CreateBuffer(BufferDataType nBufferDataType, BufferChannels nBufferChannels)
{
   BitmapBuffer *pBuffer = NULL;
   
   switch (nBufferChannels) {
      case BC_BGR:
         switch (nBufferDataType) {
            case BDT_BYTE:
               pBuffer = SNEW TypedBitmapBuffer<BYTE, RGBByte>(this, nBufferChannels, nBufferDataType);
               break;
            //case BDT_HALF:
            //   pBuffer = SNEW TypedBitmapBuffer<half, RGBHalf>(this, nBufferChannels, nBufferDataType);
            //   break;
            case BDT_FLOAT:
               pBuffer = SNEW TypedBitmapBuffer<float, RGBFloat>(this, nBufferChannels, nBufferDataType);
               break;
         }
         break;
      case BC_BGRA:
         switch (nBufferDataType) {
            case BDT_BYTE:
               pBuffer = SNEW TypedBitmapBuffer<BYTE, RGBAByte>(this, nBufferChannels, nBufferDataType);
               break;
            //case BDT_HALF:
            //   pBuffer = SNEW TypedBitmapBuffer<half, RGBAHalf>(this, nBufferChannels, nBufferDataType);
            //   break;
            case BDT_FLOAT:
               pBuffer = SNEW TypedBitmapBuffer<float, RGBAFloat>(this, nBufferChannels, nBufferDataType);
               break;
         }
         break;
      case BC_LUMINANCE:
         switch (nBufferDataType) {
            case BDT_BYTE:
               pBuffer = SNEW TypedBitmapBuffer<BYTE, BYTE>(this, nBufferChannels, nBufferDataType);
               break;
            //case BDT_HALF:
            //   pBuffer = SNEW TypedBitmapBuffer<half, Half>(this, nBufferChannels, nBufferDataType);
            //   break;
            case BDT_INT:
               pBuffer = SNEW TypedBitmapBuffer<int, int>(this, nBufferChannels, nBufferDataType);
               break;
            case BDT_FLOAT:
               pBuffer = SNEW TypedBitmapBuffer<float, float>(this, nBufferChannels, nBufferDataType);
               break;
         }
         break;
      case BC_LUMINANCE_ALPHA:
         switch (nBufferDataType) {
            case BDT_BYTE:
               pBuffer = SNEW TypedBitmapBuffer<BYTE, LAByte>(this, nBufferChannels, nBufferDataType);
               break;
            //case BDT_HALF:
            //   pBuffer = SNEW TypedBitmapBuffer<half, LAHalf>(this, nBufferChannels, nBufferDataType);
            //   break;
            case BDT_FLOAT:
               pBuffer = SNEW TypedBitmapBuffer<float, LAFloat>(this, nBufferChannels, nBufferDataType);
               break;
         }
         break;
   }

   return pBuffer;
}

BitmapBuffer *BitmapObject::CreateImageBuffer(BufferDataType nDataType, BufferChannels nBufferChannels) 
{
   ASSERT(nDataType == BDT_BYTE || nDataType == BDT_HALF || nDataType == BDT_FLOAT);

   BitmapBuffer *pBuffer = CreateBuffer(nDataType, nBufferChannels);
   ASSERT(pBuffer);
 
   pBuffer->Allocate();
   SetBuffer(IMAGE_BUFFER, pBuffer);

   return pBuffer;
}
