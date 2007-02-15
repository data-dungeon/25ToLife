//-----------------------------------------------------------------------------
//                           PS2 Shell System Core
//
// shellDma - simple low level control of DMA hardware
//
// $Workfile: shellDma.c $
// $Author: Tyler Colbert $
// $Date: 8/15/03 11:17a $
// $Revision: 1 $
//
// Copyright (c) 2002-3 Sony Computer Entertainment Europe.
// All Rights Reserved.
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// I N C L U D E S
//-----------------------------------------------------------------------------

#include "shellCore.h"

#if defined(__LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif


//-----------------------------------------------------------------------------
// M A C R O S
//-----------------------------------------------------------------------------

#define CHCR(a) (*(volatile u_int *)((int)a + 0x00))
#define MADR(a) (*(volatile u_int *)((int)a + 0x10))
#define QWC(a)  (*(volatile u_int *)((int)a + 0x20))
#define TADR(a) (*(volatile u_int *)((int)a + 0x30))
#define SADR(a) (*(volatile u_int *)((int)a + 0x80))

#define CHCR_STR 0x100
#define CHCR_TIE  0x80
#define CHCR_TTE  0x40
#define CHCR_CHAIN 0x4
#define CHCR_INTERLEAVE 0x8

#define D_CTRL          ((volatile u_int *)(0x1000e000))
#define D_STAT          ((volatile u_int *)(0x1000e010))
#define D_PCR           ((volatile u_int *)(0x1000e020))
#define D_SQWC			((volatile u_int *)(0x1000e030))
#define D_ENABLEW       ((volatile u_int *)(0x1000f590))


//-----------------------------------------------------------------------------
// S T A T I C   V A R I A B L E S 
//-----------------------------------------------------------------------------

static volatile u_int *_baseAddresses[10]={
		(volatile u_int *) 0x10008000,			// VIF0
		(volatile u_int *) 0x10009000,			// VIF1
		(volatile u_int *) 0x1000a000,			// GIF
		(volatile u_int *) 0x1000b000,			// fromIPU
		(volatile u_int *) 0x1000b400,			// toIPU
		(volatile u_int *) 0x00000000,			// N/A
		(volatile u_int *) 0x00000000,			// N/A
		(volatile u_int *) 0x00000000,			// N/A
		(volatile u_int *) 0x1000d000,			// fromSPR
		(volatile u_int *) 0x1000d400};			// toSPR

static u_int _channelDefaults[10];

static ShellDmaTag *_currentFirst;
static ShellDmaTag *_currentLast;


//-----------------------------------------------------------------------------
// G L O B A L   F U N C T I O N S
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// shellDmaKickVif1List()
// Description:	Kick off the current VIF1 DMA list
// Parameters:	N/A
// Returns: N/A
// Notes:		This function is used when allowing the PS2 Shell System to
//				maintain the VIF drawlist (for example, when using the ShellDisp
//				functions to manage the draw buffers). This kicks off the
//				current draw list and resets the internal pointers to start a
//				new one.
//-----------------------------------------------------------------------------
void shellDmaKickVif1List(void)
{
	if(_currentFirst)
	{
		u_int def=_channelDefaults[1];
		_channelDefaults[1]|=CHCR_TTE | 1;
		_currentLast->fields.id=SHELLDMA_TAG_ID_END;
		shellDmaStartChain(SHELLDMA_CHANNEL_VIF1,_currentFirst);
		_currentFirst=0;
		_channelDefaults[1]=def;
	}
}

//-----------------------------------------------------------------------------
// shellDmaAppendVif1List()
// Description: This function adds a new DMA list segment to the end of the
//				current VIF1 list, to be used when drawing the next frame
// Parameters:	first_tag	- the first tag in the DMA list segment
//				last_tag	- the last tag in the DMA list segment, which may
//							be altered when joining lists together. Should be
//							initialised to an empty END tag by the caller
// Returns: N/A
// Notes:
//-----------------------------------------------------------------------------
void shellDmaAppendVif1List(ShellDmaTag *first_tag, ShellDmaTag *last_tag)
{
	if(!_currentFirst)
	{
		_currentFirst=first_tag;
		_currentLast=last_tag;
	} else {
		unsigned int ptr=(unsigned int) first_tag;
		ptr&=0xcfffffff;
		if(ptr & 0xc0000000) ptr+=0x40000000;
		_currentLast->fields.id=SHELLDMA_TAG_ID_NEXT;
		_currentLast->fields.next=(ShellDmaTag *)ptr;
		_currentLast=last_tag;
	}
}

//-----------------------------------------------------------------------------
// shellDmaAppendVif1List()
// Description: This function adds a new DMA list segment to the start of the
//				current VIF1 list, to be used when drawing the next frame
// Parameters:	first_tag	- the first tag in the DMA list segment
//				last_tag	- the last tag in the DMA list segment, which may
//							be altered when joining lists together. Should be
//							initialised to an empty END tag by the caller
// Returns: N/A
// Notes:
//-----------------------------------------------------------------------------
void shellDmaPrependVif1List(ShellDmaTag *first_tag, ShellDmaTag *last_tag)
{
	if(!_currentFirst)
	{
		_currentFirst=first_tag;
		_currentLast=last_tag;
	} else {
		unsigned int ptr=(unsigned int) _currentFirst;
		ptr&=0xcfffffff;
		if(ptr & 0xc0000000) ptr+=0x40000000;
		last_tag->fields.id=SHELLDMA_TAG_ID_NEXT;
		last_tag->fields.next=(ShellDmaTag *)ptr;
		_currentFirst=first_tag;
	}
}

//-----------------------------------------------------------------------------
// shellDmaInit()
// Description: This function initialises the DMA module, including basic
//				initialisation of the DMA controller itself. Must be called
//				before using the other functions of the DMA module.
// Parameters:	N/A
// Returns: N/A
// Notes:
//-----------------------------------------------------------------------------
void shellDmaInit(void)
{
	int i;
	
	*D_CTRL=1;		// Enable DMA, no cycle steal, no FIFO, no stall control
	*D_STAT=0xe31f | (*D_STAT & 0x631f0000);	// Reset all condition codes
	*D_PCR=0;					// No priority control or status reporting
	*D_ENABLEW=0;				// DMA transfer enable - no hold state.
	
	_currentFirst=0;
	_currentLast=0;

	for(i=0;i<10;i++)
	{
		_channelDefaults[i]=0x1;
	}
}

//-----------------------------------------------------------------------------
// shellDmaSetChannelOptions()
// Description: Each channel has a selection of options available. This
//				function is responsible for setting these, which will be
//				remembered for subsequent DMA operations.
// Parameters:	channel - the channel ID to set options for
//				direction - direction of transfer for this channel
//				tte - transfer tag enable flag
//				tie - transfer interrupt enable flag
//				interleave - interleaved transfer enable flag
//				skip_qwc - number of quadwords to skip when interleaving
//				trans_qwc - number of quadwords to transfer when interleaving
// Returns: N/A
// Notes:
//-----------------------------------------------------------------------------
void shellDmaSetChannelOptions(EShellDmaChannel channel,int direction,int tte,
								int tie,int interleave,u_int skip_qwc,
								u_int trans_qwc)
{
	ASSERTS(channel>=0 && channel <=9 && _baseAddresses[channel] , "DMA Channel out of range");
	ASSERTS(!(interleave && channel<8) ,"interleave set on non SPAD channel");

	*D_SQWC=skip_qwc | (trans_qwc<<16);

	_channelDefaults[channel]=(unsigned int) ((direction?1:0) | (tte?CHCR_TTE:0) |
				(tie?CHCR_TIE:0) | (interleave?CHCR_INTERLEAVE:0));
}

//-----------------------------------------------------------------------------
// shellDmaStart()
// Description: Start a normal or interleaved DMA transfer on a specified
//				channel, according to previously definied options.
// Parameters:	channel - the channel ID used for the operation
//				memory - a pointer to the memory to transfer from or to
//					(deals internally with cache flags and spad addresses)
//				qwc - how many quad words to transfer
// Returns: N/A
// Notes:
//-----------------------------------------------------------------------------
void shellDmaStart(EShellDmaChannel channel, void *memory, u_int qwc)
{
	volatile int *cbase=(int *)_baseAddresses[channel];
	u_int ptr=(u_int)memory;

	ASSERTS(channel>=0 && channel <=9 && _baseAddresses[channel] , "DMA Channel out of range");

#ifdef DEBUG
	if(CHCR(cbase) & CHCR_STR)
	{
		PRINT("Warning in shellDmaSendNormal\nDMA already running when activating channel %d\nAttemping sync..\n",channel);
		shellDmaSync(channel,0);
	}
#endif

	ptr&=0xcfffffff;
	if(ptr & 0xc0000000) ptr+=0x40000000;		// Have we a spad address?
		
	MADR(cbase)=ptr;
	QWC(cbase)=qwc;
	*D_STAT=(unsigned int) (1<<channel);
	CHCR(cbase)=CHCR_STR | _channelDefaults[channel];
	asm("sync.l");
}

//-----------------------------------------------------------------------------
// shellDmaStartChain()
// Description: Start a chain mode DMA transfer on a specified channel.
// Parameters:	channel - the channel ID used for the operation
//				memory - memory address to transfer from or too
//					(deals internally with cache flags and spad addresses)
// Returns: N/A
// Notes:
//-----------------------------------------------------------------------------
void shellDmaStartChain(EShellDmaChannel channel, ShellDmaTag *memory)
{
	volatile unsigned int *cbase=_baseAddresses[channel];
	u_int ptr=(u_int)memory;

	ASSERTS(channel>=0 && channel <=9 && _baseAddresses[channel] , "DMA Channel out of range");

#ifdef DEBUG
	if(CHCR(cbase) & CHCR_STR)
	{
		PRINT("Warning in shellDmaSendChain\nDMA already running when activating channel %d\nAttemping sync...",channel);
		if(!shellDmaSync(channel,10000000))
		{
			#ifdef DEBUG
			PRINT("...shellDmaStartChain sync timed out");
			PRINT("VIF1_STAT = %08x",*VIF1_STAT);
			PRINT("VIF1_CODE = %08x",*VIF1_CODE);
			#endif;
		}
	}
#endif

	ptr&=0xcfffffff;
	if(ptr & 0xc0000000) ptr+=0x40000000;		// Have we a spad address?

	TADR(cbase)=ptr;
	QWC(cbase)=0;
	*D_STAT=(unsigned int) (1<<channel);
	CHCR(cbase)=CHCR_STR | CHCR_CHAIN | (_channelDefaults[channel]&~0xc);
	asm("sync.l");
}

//-----------------------------------------------------------------------------
// shellDmaStartSpad()
// Description: Start a transfer relating to one of the Scratchpad channels,
//				either in normal or interleaved mode depending on the channel
//				options.
// Parameters:	channel - channel ID for the transfer (should be a scratchpad
//						channel)
//				memory - memory address for the transfer (should be in main
//						memory)
//				spad - scratchpad address for the transfer
//				qwc - number of quadwords to transfer
// Returns: N/A
// Notes:
//-----------------------------------------------------------------------------
void shellDmaStartSpad(EShellDmaChannel channel, void *memory,
						void *spad, u_int qwc)
{
	volatile unsigned int *cbase=_baseAddresses[channel];
	ASSERTS(channel>7 , "SPAD DMA on non SPAD channel");
	ASSERTS(((int)memory & 0xc0000000) == 0 , "SPAD DMA cannot have SPAD memory address as well");
	
	SADR(cbase)=(u_int)spad;
	shellDmaStart(channel,memory,qwc);
}

//-----------------------------------------------------------------------------
// shellDmaStartChainSpad()
// Description: Start a chain mode DMA transfer on one of the scratchpad DMA
//				channels.
// Parameters:	channel - channel ID for the transfer (should be one of the
//						scratchpad channels.
//				memory - pointer to the memory for the transfer
//				spad - pointer to the scrachpad area for the transfer
// Returns: N/A
// Notes:
//-----------------------------------------------------------------------------
void shellDmaStartChainSpad(EShellDmaChannel channel, void *memory, void *spad)
{
	volatile unsigned int *cbase=_baseAddresses[channel];
	ASSERTS(channel>7 , "SPAD DMA on non SPAD channel");
	ASSERTS(((int)memory & 0xc0000000) == 0 , "SPAD DMA cannot have SPAD memory address as well");
	
	SADR(cbase)=(u_int)spad;
	shellDmaStartChain(channel, (ShellDmaTag*) memory);
}

//-----------------------------------------------------------------------------
// shellDmaSync()
// Description: Wait for the end of a specified DMA transfer.
// Parameters:	channel - ID of channel to wait on
//				timeout - timeout value in units of roughly 4 CPU cycles. 0 for
//						maximum wait period (around a minute).
// Returns: returns 1 if the DMA synced ok, 0 if a timeout occured.
// Notes: Should move this to .h (for inlining)
//-----------------------------------------------------------------------------
EShellResult shellDmaSync(EShellDmaChannel channel, int timeout)
{
	volatile unsigned int *cbase=_baseAddresses[channel];

	ASSERTS(channel>=0 && channel <=9 && _baseAddresses[channel] , "DMA Channel out of range");

	*D_PCR=(unsigned int) (1<<channel);
	
	asm("
	.set noreorder
			sync.l
			sync.p
			addiu %1,%1,-1
		bigloop:
			lw	$8,0(%2)
			andi $8,$8,0x100
			beq $8,$0,dmaquit
		dmaloop:
			andi $8,%1,0x7f
			addiu %1,%1,-1
			beq	$8,$0,bigloop
			nop
			beq %1,$0,dmaquit
			nop
			bc0f dmaloop
			nop
		dmaquit:
			add %0,%1,$0
	.set reorder
	":"=r"(timeout):"r"(timeout),"r"(cbase):"$8");		// don't like this, but CodeWarrior is odd
	
	if(timeout==0) return SHELL_FAIL;

	while(CHCR(cbase) & CHCR_STR){}	
	return SHELL_SUCCESS;
}

#if defined(__LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif
