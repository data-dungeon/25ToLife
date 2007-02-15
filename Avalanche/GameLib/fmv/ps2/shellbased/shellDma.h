//-----------------------------------------------------------------------------
//                           PS2 Shell System Core
//
// shellDma header file
//
// $Workfile: shellDma.h $
// $Author: Tyler Colbert $
// $Date: 8/15/03 11:17a $
// $Revision: 1 $
//
// Copyright (c) 2002-3 Sony Computer Entertainment Europe.
// All Rights Reserved.
//
//-----------------------------------------------------------------------------

#ifndef _SHELL_DMA_H
#define _SHELL_DMA_H

//-----------------------------------------------------------------------------
// T Y P E D E F S
//-----------------------------------------------------------------------------

typedef enum
{
	SHELLDMA_CHANNEL_VIF0	=0,
	SHELLDMA_CHANNEL_VIF1	=1,
	SHELLDMA_CHANNEL_GIF		=2,
	SHELLDMA_CHANNEL_FROMIPU	=3,
	SHELLDMA_CHANNEL_TOIPU	=4,
	SHELLDMA_CHANNEL_FROMSPR	=8,
	SHELLDMA_CHANNEL_TOSPR	=9
}	EShellDmaChannel;

#define SHELLDMA_TAG_ID_REFE (0x00)
#define SHELLDMA_TAG_ID_CNT (0x10)
#define SHELLDMA_TAG_ID_NEXT (0x20)
#define SHELLDMA_TAG_ID_REF (0x30)
#define SHELLDMA_TAG_ID_REFS (0x40)
#define SHELLDMA_TAG_ID_CALL (0x50)
#define SHELLDMA_TAG_ID_RET (0x60)
#define SHELLDMA_TAG_ID_END (0x70)

#define SHELLDMA_TAG_ID_IRQ (0x80)

#define SHELLDMA_TAG_PCE_OFF (0x0)
#define SHELLDMA_TAG_PCE_LOW (0x8)
#define SHELLDMA_TAG_PCE_HIGH (0xc)

//--------------
// ShellDmaTag
//--------------
// Structure used to describe a DMA tag.
//
// Description:
//   This structure represents a DMATag in two different ways,
//   using a union of both a field-by-field structure, and two unsigned long
//   (64-bit) integer values. This structure should be aligned to a quadword
//   boundary.
//
// Also:
//   N/A


typedef union _ShellDmaTag {
	struct {
		unsigned short		qwc;	// Quadword count for transfer.
		unsigned char		mark;	// Mark field. This field is not actually
									// used for anything by the hardware, so
									// is available for user data.
		unsigned char		id;		// ID field - describes the type of DMATag.
		union _ShellDmaTag	*next;	// next - points to data, or more DMATags.
		unsigned int		p[2];	// Padding - can be used to store VIF codes
									// or other data if TTE is enabled.
	} fields;						// this contains the individual fields of
									// the DMAtag as seperate values.
	struct {
		unsigned long 		ul[2];	// array of unsigned long values
	} ulong;						// this allows the DMAtag to be accessed as
									// unsigned long data.
} ShellDmaTag __attribute__ ((aligned(16)));	


//-----------------------------------------------------------------------------
// P R O T O T Y P E S
//-----------------------------------------------------------------------------

void shellDmaInit(void);
EShellResult shellDmaSync(EShellDmaChannel channel, int timeout);
void shellDmaSetChannelOptions(EShellDmaChannel channel,int direction,int tte,
							int tie,int interleave,u_int skip_qwc,u_int trans_qwc);
void shellDmaStart(EShellDmaChannel channel, void *memory, u_int qwc);
void shellDmaStartChain(EShellDmaChannel channel, ShellDmaTag *memory);
void shellDmaStartSpad(EShellDmaChannel channel, void *memory, void *spad,
						u_int qwc);
void shellDmaStartChainSpad(EShellDmaChannel channel, void *memory, void *spad);



void shellDmaAppendVif1List(ShellDmaTag *first_tag, ShellDmaTag *last_tag);
void shellDmaPrependVif1List(ShellDmaTag *first_tag, ShellDmaTag *last_tag);
void shellDmaKickVif1List(void);

#endif



