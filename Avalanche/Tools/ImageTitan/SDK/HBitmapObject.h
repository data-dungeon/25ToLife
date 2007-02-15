//
#pragma once

#ifndef IMAGETITANEXPORT
#define IMAGETITANEXPORT
#endif

#include "BufferType.h"

class HBitmapBuffer;
class IMAGETITANEXPORT HBitmapObject
{
public:
   void Init(int nWidth, int nHeight, float fAspect = 1.0f);

// Buffers
   HBitmapBuffer *CreateImageBuffer( BufferDataType nBufferDataType, BufferChannels nBufferChannels );
};