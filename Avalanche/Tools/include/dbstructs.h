#ifndef __DBSTRUCTS_H
#define __DBSTRUCTS_H

#include "t_types.h"

#define ID_BYTES		2

typedef struct DBFileHdr
{
	char					cID[ID_BYTES];
	short					sVersion;
   t_lflagbits       FileFlags;
	unsigned long		ulChunks;
	unsigned long		ulMaxChunkSize;
} ts_DBFileHdr;

typedef struct DBChunkHdr
{
	unsigned short		usType;
	t_sflagbits       ChunkFlags;
	unsigned long		ulChunkSize;
} ts_DBChunkHdr;

#endif
