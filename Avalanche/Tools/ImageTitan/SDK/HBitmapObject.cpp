//

#include "stdafx.h"
#include "HBitmapObject.h"
#include "../BitmapObject.h"

void HBitmapObject::Init(int nWidth, int nHeight, float fAspect)
{
   ((BitmapObject *)this)->Init(nWidth, nHeight, fAspect);
}

HBitmapBuffer *HBitmapObject::CreateImageBuffer( BufferDataType nBufferDataType, BufferChannels nBufferChannels )
{
   return (HBitmapBuffer *)((BitmapObject *)this)->CreateImageBuffer( nBufferDataType, nBufferChannels );
}
