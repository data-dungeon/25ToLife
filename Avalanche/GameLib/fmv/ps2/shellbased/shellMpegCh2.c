//-----------------------------------------------------------------------------
//                   shellMpeg - IPU/MPEG streaming module
//
// DMA tag setup for Channel 2 (GIF)
//
// $Workfile: shellMpegCh2.c $
// $Author: Tyler Colbert $
// $Date: 8/15/03 11:17a $
// $Revision: 1 $
//
// Copyright (c) 2002-3 Sony Computer Entertainment Europe.
// All Rights Reserved.
//
//-----------------------------------------------------------------------------


// Path 3 tags for decoded video transfer:
//
// The tags created here for uploading a decoded image are intended to be
// 'call'ed from a channel 2 DMA chain.
//
// Videos of type SHELLMPEG_VIDEO_IPU_STANDARD are transferred one macroblock
// at a time, in row order. All other video types can have their data sent in
// units of one-macroblock-wide columns.
//
//  ------ (Header) -----------------------------------------------
//  t[0]   DMA cnt
//  t[1]     GIF tag A&D
//  t[2]       BITBLTBUF
//  t[3]       TRXREG
//  ------ (Transfer repeat unit) ----------------------------------
//  t[4]   DMA cnt
//  t[5]     GIF tag A&D
//  t[6]       TRXPOS
//  t[7]       TRXDIF
//  t[8]     GIF tag image, loop = 1 macroblock or 1 column
//  t[9]   DMA ref, qwc = 1 macroblock or 1 column
//  ------ (above repeated for each macroblock, or for each strip of image) 
//  ------ (End) --------------------------------------------------
//  ....   DMA ret


//-----------------------------------------------------------------------------
// I N C L U D E S
//-----------------------------------------------------------------------------

#include <eekernel.h>
#include <eestruct.h>
#include <libgraph.h>
#include "shellCore.h"
#include "shellMpegCh2.h"


//-----------------------------------------------------------------------------
// G L O B A L   F U N C T I O N S
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// setLoadImageDest()
// Description: Alters an existing texture upload chain to specify dbw and dbp
// Parameters: t   - First tag of a DMA chain created by setLoadImageTagsRows()
//                   or setLoadImageTagsColumns()
//             dbp - dbp parameter for BITBLTBUF
//             dbw - dbw parameter for BITBLTBUF
// Returns: N/A
// Notes:
//-----------------------------------------------------------------------------
void setLoadImageDest( ShellDmaTag *t, u_int dbp, u_int dbw, int dpsm )
{
	t = (ShellDmaTag *) (((int) t) | 0x20000000);

	t[2].ulong.ul[0] = SCE_GS_SET_BITBLTBUF(0, 0, 0, dbp, dbw, dpsm);
	t[2].ulong.ul[1] = SCE_GS_BITBLTBUF;
}

//-----------------------------------------------------------------------------
// setLoadImageTagsRows()
// Description: Creates a DMA chain for uploading a decoded image to VRAM
//              with macroblocks ordered in rows
// Parameters: t     - An allocated block of memory for holding tags
//             image - Bitmap to upload
//             mbw   - Width of image, in macroblock (16-pixel) units
//             mbh   - Height of image, in macroblock (16-pixel) units
//             opm   - Pixel format: SCE_GS_PSMCT32 or SCE_GS_PSMCT16
// Returns: N/A
// Notes:
//   The amount of memory needed for t is can be calculated using the
//   PATH3_TAG_SIZE_ROWS macro in shellMpegCh2.h.
//
//   The parameters of the upload buffer (BITBLTBUF) are not specified in the
//   created chain and must be set after calling using setLoadImageDest().
//-----------------------------------------------------------------------------
void setLoadImageTagsRows( ShellDmaTag *t, void *image, u_int mbw, u_int mbh,
						   int opm )
{
	int mx, my;
	int qwc = (opm == SCE_GS_PSMCT32) ? (16*16*4)/16 : (16*16*2)/16;

	InvalidDCache(t, ((char *) t) + PATH3_TAG_SIZE_ROWS(mbw, mbh) - 1);

	t = (ShellDmaTag *) (((int) t) | 0x20000000);

	t->fields.qwc = 3;
	t++->fields.id = SHELLDMA_TAG_ID_CNT;

	t->ulong.ul[0] = SCE_GIF_SET_TAG(2, 0, 0, 0, SCE_GIF_PACKED, 1);
	t++->ulong.ul[1] = SCE_GIF_PACKED_AD;
	
	// Unless tags will be used in a ShellTexture texture, the following
	// A&D will be replaced by the appropriate BITBLTBUF for specifying the
	// upload target
	
	t->ulong.ul[0] = 0;
	t++->ulong.ul[1] = SCE_GS_NOP;
	
	t->ulong.ul[0] = SCE_GS_SET_TRXREG(16, 16);
	t++->ulong.ul[1] = SCE_GS_TRXREG;

	for (my = 0; my < mbh; my++)
	{
		for (mx = 0; mx < mbw; mx++)
		{
			int eop = (mx == (mbw)-1 && my == (mbh)-1) ? 1:0;
		
			t->fields.qwc = 4;
			t++->fields.id = SHELLDMA_TAG_ID_CNT;

			t->ulong.ul[0] = SCE_GIF_SET_TAG(2, 0, 0, 0, SCE_GIF_PACKED, 1);
			t++->ulong.ul[1] = SCE_GIF_PACKED_AD;
		
			t->ulong.ul[0] = SCE_GS_SET_TRXPOS(0, 0, mx*16, my*16, 0);
			t++->ulong.ul[1] = SCE_GS_TRXPOS;

			t->ulong.ul[0] = SCE_GS_SET_TRXDIR(0);
			t++->ulong.ul[1] = SCE_GS_TRXDIR;

			t->ulong.ul[0] = SCE_GIF_SET_TAG(qwc, eop, 0, 0, SCE_GIF_IMAGE, 0);
			t++->ulong.ul[1] = 0;
			
			t->fields.qwc = (u_short) qwc;
			t->fields.next = image;
			t++->fields.id = SHELLDMA_TAG_ID_REF;
			
			(u_long128 *) image += qwc;
		}
	}
	
	t->fields.qwc = 0;
	t++->fields.id = SHELLDMA_TAG_ID_RET;
}

//-----------------------------------------------------------------------------
// setLoadImageTagsColumns()
// Description: Creates a DMA chain for uploading a decoded image to VRAM
//              with macroblocks ordered in rows
// Parameters: t     - An allocated block of memory for holding tags
//             image - Bitmap to upload
//             mbw   - Width of image, in macroblock (16-pixel) units
//             mbh   - Height of image, in macroblock (16-pixel) units
//             opm   - Pixel format: SCE_GS_PSMCT32 or SCE_GS_PSMCT16
// Returns: N/A
// Notes:
//   The amount of memory needed for t is can be calculated using the
//   PATH3_TAG_SIZE_COLUMNS macro in shellMpegCh2.h.
//
//   The parameters of the upload buffer (BITBLTBUF) are not specified in the
//   created chain and must be set after calling using setLoadImageDest().
//-----------------------------------------------------------------------------
void setLoadImageTagsColumns( ShellDmaTag *t, void *image,
							  u_int mbw, u_int mbh, int opm )
{
	int mx;
	int qwc = (int) mbh *
				((opm == SCE_GS_PSMCT32) ? (16*16*4)/16 : (16*16*2)/16);

	InvalidDCache(t, ((char *) t) + PATH3_TAG_SIZE_COLUMNS(mbw) - 1);
	t = (ShellDmaTag *) (((int) t) | 0x20000000);

	t->fields.qwc = 3;
	t++->fields.id = SHELLDMA_TAG_ID_CNT;

	t->ulong.ul[0] = SCE_GIF_SET_TAG(2, 0, 0, 0, SCE_GIF_PACKED, 1);
	t++->ulong.ul[1] = SCE_GIF_PACKED_AD;

	// Unless tags will be used in a ShellTexture texture, the following
	// A&D will be replaced by the appropriate BITBLTBUF for specifying the
	// upload target
	
	t->ulong.ul[0] = 0;
	t++->ulong.ul[1] = SCE_GS_NOP;
	
	t->ulong.ul[0] = SCE_GS_SET_TRXREG(16, mbh * 16);
	t++->ulong.ul[1] = SCE_GS_TRXREG;

	for (mx = 0; mx < mbw; mx++)
	{
		int eop = (mx == (mbw)-1) ? 1:0;

		t->fields.qwc = 4;
		t++->fields.id = SHELLDMA_TAG_ID_CNT;
		
		t->ulong.ul[0] = SCE_GIF_SET_TAG(2, 0, 0, 0, SCE_GIF_PACKED, 1);
		t++->ulong.ul[1] = SCE_GIF_PACKED_AD;
		
		t->ulong.ul[0] = SCE_GS_SET_TRXPOS(0, 0, mx*16, 0, 0);
		t++->ulong.ul[1] = SCE_GS_TRXPOS;
		
		t->ulong.ul[0] = SCE_GS_SET_TRXDIR(0);
		t++->ulong.ul[1] = SCE_GS_TRXDIR;
		
		t->ulong.ul[0] = SCE_GIF_SET_TAG(qwc, eop, 0, 0, SCE_GIF_IMAGE, 0);
		t++->ulong.ul[1] = 0;
		
		t->fields.qwc = (u_short) qwc;
		t->fields.next = image;
		t++->fields.id = SHELLDMA_TAG_ID_REF;

		(u_long128 *) image += qwc;
	}
	
	t->fields.qwc = 0;
	t++->fields.id = SHELLDMA_TAG_ID_RET;
}
