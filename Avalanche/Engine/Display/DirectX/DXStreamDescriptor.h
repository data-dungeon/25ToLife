#ifndef __DX_STREAM_DESCRIPTOR_H
#define __DX_STREAM_DESCRIPTOR_H

#include "StreamFlags.h"

typedef struct _DXStreamDescriptor
{
	unsigned int				m_StreamFlags;
	unsigned int				m_StreamFVF;
	unsigned int				m_StreamComponents;
	void *						p_Descriptor;
} DXStreamDescriptor;

#endif
