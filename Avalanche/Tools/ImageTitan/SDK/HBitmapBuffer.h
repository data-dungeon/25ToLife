#pragma once

#ifndef IMAGETITANEXPORT
#define IMAGETITANEXPORT
#endif

#include "BufferType.h"

class IMAGETITANEXPORT HBitmapBuffer
{
public:
   BufferDataType GetBufferDataType();
   void SetRow(int nRow, const void *pRow);
   void *GetRow(int nRow);
};