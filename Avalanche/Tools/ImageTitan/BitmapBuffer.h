#pragma once

#ifndef IMAGETITANEXPORT
#define IMAGETITANEXPORT
#endif

#include "SDK/BufferType.h"
#include "BitmapChannel.h"
#include "BitmapObject.h"

class IMAGETITANEXPORT BitmapBuffer
{
protected:
   BitmapObject *m_pBitmapObject;
   BufferDataType m_nBufferDataType;
   BufferChannels m_nBufferChannels;
   int m_nWidthBytes;

public:
// Constructors
   BitmapBuffer(BitmapObject *pBitmapObject, BufferChannels nBufferChannels, BufferDataType nBufferDataType ) :
    m_pBitmapObject(pBitmapObject),
    m_nBufferChannels(nBufferChannels),
    m_nBufferDataType(nBufferDataType)
   {
      m_nWidthBytes = 0;
   }
   virtual ~BitmapBuffer() {}

// Operations
   BufferDataType GetBufferDataType() { return m_nBufferDataType; }
   BufferChannels GetBufferChannels() { return m_nBufferChannels; }

   virtual void Allocate() = 0;
   virtual void SetRow(int nRow, const void *pRow) = 0;
   virtual void *GetRow(int nRow) = 0;
   virtual void *GetData() = 0;
   virtual int GetNumChannels() = 0;
};

template<class TYPE, class RTYPE>
class IMAGETITANEXPORT TypedBitmapBuffer : public BitmapBuffer
{
private:
   RTYPE *m_pData;
   BitmapChannel<TYPE> m_channelarray[sizeof(RTYPE)/sizeof(TYPE)];

public:
// Constructors
   TypedBitmapBuffer(BitmapObject *pObject, BufferChannels nBufferChannels, BufferDataType nBufferDataType) :
    BitmapBuffer(pObject, nBufferChannels, nBufferDataType),
    m_pData(NULL)
   { 
      InitChannelArray();
   }

   ~TypedBitmapBuffer() { delete [] m_pData; }

// Operations
   void InitChannelArray() {
      int nNumChannels = GetNumChannels();
      for (int i=0; i < nNumChannels; i++)
         m_channelarray[i].Init((TYPE **)&m_pData, i, nNumChannels, nNumChannels*m_pBitmapObject->GetWidth());
   }

// BitmapBuffer overrides
   int GetNumChannels() { return sizeof(RTYPE)/sizeof(TYPE); };

   virtual void Allocate() 
   { 
      int nWidth = m_pBitmapObject->GetWidth();
      int nHeight = m_pBitmapObject->GetHeight();

      m_nWidthBytes = nWidth * sizeof(RTYPE);

      if (m_nWidthBytes % 4)
         m_nWidthBytes += 4 - m_nWidthBytes % 4;

      try {
         ASSERT(m_nWidthBytes && nHeight);
         m_pData = (RTYPE *) SNEW TYPE[m_nWidthBytes * nHeight]; 
      }
      catch(...) {
         String msg("BitmapBuffer::Allocate failed: width = %1, heigth = %2");
         msg.Replace("%1", (String)nWidth);
         msg.Replace("%2", (String)nHeight);
         THROW_FATAL_ERROR(msg);
      }
   }

   virtual void SetRow(int nRow, const void *pSrcRow)
   {
      try {
         ASSERT(m_pData);
         ASSERT(nRow < m_pBitmapObject->GetHeight());

         RTYPE *pDstRow = (RTYPE *)(((TYPE *)m_pData) + (nRow * m_nWidthBytes)); 
         memcpy((void *)pDstRow, pSrcRow, m_pBitmapObject->GetWidth()*sizeof(RTYPE)); 
      }
      catch(...) {
         THROW_FATAL_ERROR("BitmapBuffer::SetRow failed");
      }
   }

   virtual void *GetRow(int nRow)
   {
      try {
         ASSERT(m_pData);
         ASSERT(nRow < m_pBitmapObject->GetHeight());

         RTYPE *pDstRow = (RTYPE *)(((TYPE *)m_pData) + (nRow * m_nWidthBytes)); 
         return pDstRow;
      }
      catch(...) {
         THROW_FATAL_ERROR("BitmapBuffer::GetRow failed");
      }
   }

   virtual void *GetData() { return m_pData; }
};
