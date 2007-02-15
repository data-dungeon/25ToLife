//
#include "stdafx.h"
#include "HBitmapBuffer.h"
#include "../BitmapBuffer.h"

BufferDataType HBitmapBuffer::GetBufferDataType()
{
   return ((BitmapBuffer *)this)->GetBufferDataType();
}

void HBitmapBuffer::SetRow(int nRow, const void *pRow)
{
   ((BitmapBuffer *)this)->SetRow(nRow, pRow);
}

void *HBitmapBuffer::GetRow(int nRow)
{
   return ((BitmapBuffer *)this)->GetRow(nRow);
}