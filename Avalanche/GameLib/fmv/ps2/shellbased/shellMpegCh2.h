//-----------------------------------------------------------------------------
//                   shellMpeg - IPU/MPEG streaming module
//
// DMA tag setup for Channel 2 (GIF)
//
// $Workfile: shellMpegCh2.h $
// $Author: Tyler Colbert $
// $Date: 8/15/03 11:17a $
// $Revision: 1 $
//
// Copyright (c) 2002-3 Sony Computer Entertainment Europe.
// All Rights Reserved.
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// M A C R O S
//-----------------------------------------------------------------------------

// Macro for calculating DMA chain size for a row-ordered image upload
// (four quadwords header, six quads per macroblock, one quadword end tag,
// rounded up to a 64 byte boundary).

#define PATH3_TAG_SIZE_ROWS(mbw, mbh) \
	(((((5 + (6 * (mbw) * (mbh))) * 16) + 63) / 64) * 64)

// Macro for calculating DMA chain size for a column-ordered image upload
// (four quadwords header, six quads per column, one quadword end tag, rounded
// up to a 64 byte boundary).

#define PATH3_TAG_SIZE_COLUMNS(mbw) \
	(((((5 + (6 * (mbw) * (1))) * 16) + 63) / 64) * 64)


//-----------------------------------------------------------------------------
// P R O T O T Y P E S
//-----------------------------------------------------------------------------

void setLoadImageDest( ShellDmaTag *t, u_int dbp, u_int dbw, int dpsm );
void setLoadImageTagsRows( ShellDmaTag *t, void *image, u_int mbw, u_int mbh,
						   int opm );
void setLoadImageTagsColumns( ShellDmaTag *t, void *image,
							  u_int mbw, u_int mbh, int opm );
