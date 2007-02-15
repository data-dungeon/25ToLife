#pragma once

#ifndef IMAGETITANEXPORT
#define IMAGETITANEXPORT
#endif

template<class TYPE>
class IMAGETITANEXPORT BitmapChannel
{
private:
   TYPE **m_pData; // Pointer to the data in the BitmapBuffer;

   int m_nStartOffset;
   int m_nXStride;
   int m_nYStride;

public:
   void Init(TYPE **pData, int nStartOffset, int nXStride, int nYStride)
   {
      m_pData = pData;
      m_nStartOffset = nStartOffset;
      m_nXStride = nXStride;
      m_nYStride = nYStride;
   }
};