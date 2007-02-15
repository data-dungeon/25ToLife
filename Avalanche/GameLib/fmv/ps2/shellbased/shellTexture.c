//-----------------------------------------------------------------------------
//
// $Workfile: shellTexture.c $
// $Author: Tyler Colbert $
// $Date: 8/15/03 11:17a $
// $Revision: 1 $
//
// Copyright (c) 2002 Sony Computer Entertainment Europe.
// All Rights Reserved.
//
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------
#include "shellCore.h"
#include "shellTexture.h"

#define MAX_NUM_MANAGED_BUFFERS (3)
#define INTERRUPT_PRINT PRINT

//storage for texture transfer gif packets
static u_long128* _texUploadPacket[2] __attribute__((aligned(64)));
static ShellDmaTag* _pDmaEndTag[2];
static ShellDmaTag* _pDmaLastInterrupt[2];
static int _pkt=0;
static u_int _maxGifDmaQW=0;

//for texture upload packet building 
static int _bufStart[MAX_NUM_MANAGED_BUFFERS];	//in blocks(address in vram/64)
static int _bufSizeM=0;
static int _bufFreeSpace;						//space left in managed buffers
static int _numBuffers=0;
static int _uploadBuffer=0;		//current buffer ID for uploading managed textures
static int _firstManagedUpload=1;
static int _lastTexManaged=0;	//flag true if the last tex uploaded was to a managed buffer
static int _imageAllocd=0;		//manage image & clut arrangement within a buffer
static int _clutAllocd=0;	
static int _reconfAdjust=0;

static int _irqNum;
static int _lastUsed[MAX_NUM_MANAGED_BUFFERS];
static int _vifIRQ;
static int _gifIRQ;
static int _gifStall;
static int _paused;

//record handler id's
static int _dmacHandlerId;
static int _vif1HandlerId;

//texture group data
typedef struct _ShellTextureGroup
{
	ShellDmaTag* pGifData;
	ShellTextureInfo* next;			
	int size;	//total upload size, in blocks
	short* pTBP;
	short* pCBP;
	u_char nTextures;

} ShellTextureGroup;

//Texture register settings state
typedef struct _ShellTextureState
{
	u_char		magFilter;				
	u_char		minFilter;				
	u_char		mmFilter;				
	u_char		mmLodMethod;
	u_char		mmL;				
	int			mmK;
	u_char 		mmAuto;
					
} ShellTextureState;

static ShellTextureState _ts;

//Gif packet building functions & interrupt handlers
static void _AddGifTransferInterrupt();
static ShellDmaTag* _AddGifTransfer(ShellDmaTag* pGifData, ShellTextureInfo* pTex, int vramAddr, int clutAddr, ShellTextureSelect* pTexSelect);
static ShellDmaTag* _AddUserTagsTransfer(ShellDmaTag* pGifData, ShellTextureInfo* pTex, int vramAddr, ShellTextureSelect* pTexSelect);
int dmacHandler(int channel, void * arg, void * addr);
int vif1Handler(int cause, void* arg, void* addr);


//-----------------------------------------------------------------------------
// shellTextureInit()
// Description: initialise module 
// Parameters: 	numOfTextures - the max number of images used per frame
// Returns:     void
// Notes:       
//      the mem allocated for the gif dma lists = 2*numOfTextures*maxSizeOfPacketsPerTexture
//		this does not take  mipmaps into account so vary the numOfTextures accordingly...
//-----------------------------------------------------------------------------
void shellTextureInit(int numOfTextures)
{
	char __version[]="PsIIShlTextr1230";

	asm volatile("nop" : : "r"(__version));

	// Add Interrupt handlers
	_paused=1;
	
	//init gif dma lists
	if(numOfTextures)
	{
		u_int listSize= (u_int)numOfTextures*17*16+16;		// max bytes per texture: 8qw image transfer + 8qw for clut + 1qw for interrupt tag
		ShellDmaTag dmaEnd;

		listSize=(listSize+63)&~63;
		dmaEnd.ulong.ul[0]=0;
		dmaEnd.ulong.ul[1]=0;
		dmaEnd.fields.id = SHELLDMA_TAG_ID_END;	

		//allocate double buffers on current heap
		_texUploadPacket[0] = (void *) ((int)shellMemoryAllocate(64, listSize) | 0x20000000);
		_texUploadPacket[1] = (void *) ((int)shellMemoryAllocate(64, listSize) | 0x20000000);
		
		FlushCache(0);	
		
		ASSERTS(_texUploadPacket[0]&&_texUploadPacket[1], "not enough space left in heap");
		PRINT("shellTextureInit allocated %d bytes", 2*listSize); 
		_maxGifDmaQW=listSize/16;
		
		//terminate them with end tags		
		_pDmaEndTag[0] = (ShellDmaTag*)&_texUploadPacket[0][0];
		_pDmaEndTag[1] = (ShellDmaTag*)&_texUploadPacket[1][0];		
		*(_pDmaEndTag[0])=dmaEnd;
		*(_pDmaEndTag[1])=dmaEnd;		
		_pDmaLastInterrupt[0]=NULL;
		_pDmaLastInterrupt[1]=NULL;
	}
	
	//set intermittent mode for gif transfers
	{
		u_int imtMode=GIF_MODE_IMT_M;
		DPUT_GIF_MODE(imtMode);
	}
	
	{
		int n=0;
		while(n<MAX_NUM_MANAGED_BUFFERS)
		{
			_lastUsed[n]=0;
			n++;
		}
	}
	
	//initial register settings
	_ts.magFilter=SCE_GS_LINEAR;				
	_ts.minFilter=SCE_GS_LINEAR;				
	_ts.mmFilter=SCE_GS_NEAREST_MIPMAP_LINEAR;				
	_ts.mmLodMethod=0;	//use formula
	_ts.mmL=0;				
	_ts.mmK=0;
	_ts.mmAuto=0;		//use miptbp1 & miptbp2
	
	{
		float fData=-2.0f;
		if(fData < 0)
			fData += 256.0f;
		_ts.mmK= (int)(16.0f * fData);
	}
}

//-----------------------------------------------------------------------------
// shellTexturePause()
// Description: pause module 
// Parameters: 	void
// Returns:     void
// Notes:       
//      
//-----------------------------------------------------------------------------
void shellTexturePause()
{	
	shellCoreRemoveDmacHandler( DMAC_GIF, _dmacHandlerId);
	shellCoreRemoveIntcHandler( INTC_VIF1, _vif1HandlerId);
	_paused=1;
}

//-----------------------------------------------------------------------------
// shellTextureConfigure()
// Description: set up buffers in VRAM to be completely managed by sceeTexture
// Parameters: 	startAddr - texture buffers start in VRAM (in blocks)
//         	    freeSpace - managed buffers take up this size (in blocks)
//         	    numOfBuffers - number of buffers to split managed area into
// Returns:     void
// Notes:   
//      uploads to this area in vram will be arranged by module via shellTextureUpload()
//      anywhere else can be uploaded to directly with shellTextureUploadToAddr()
//-----------------------------------------------------------------------------
void shellTextureConfigure(int startAddr, int freeSpace, int numOfBuffers)
{
	int n=0;
	ASSERTS(numOfBuffers<=MAX_NUM_MANAGED_BUFFERS, "too many buffers");

	_reconfAdjust=1;

	//finish any managed uploads before reconfig
	if(_lastTexManaged)	
	{
		_AddGifTransferInterrupt();
		_firstManagedUpload=1;	
		_lastTexManaged=0;
	}
	_uploadBuffer=0;
	
	//set up new buffers
	_numBuffers = numOfBuffers;
	_bufSizeM = freeSpace/numOfBuffers;
	WARNING((_bufSizeM%32)==0, "managed buffer size should be multiples of pages");
	while(n<numOfBuffers)
	{
		_bufStart[n]=startAddr+n*_bufSizeM;
		n++;
	}	
	_bufFreeSpace=_bufSizeM;
	_clutAllocd=0;	//reset clut allocation etc
	_imageAllocd=0;
}

//-----------------------------------------------------------------------------
// shellTextureUpload()
// Description: set up managed texture transfer from ee->gs local mem
// Parameters: 	pTex - texture to upload
//         	    pTexSelect - GS register settings to use texture
// Returns:     buffer sync flag - 1=need to synchronise drawing & texture transfer with irq on vif
// Notes:       
//      add to internal dma list to upload a managed texture
//      image goes into next available space, cluts are packed at the end of the buffer
//		When the texture is the first in a group all textures in the group are uploaded
//-----------------------------------------------------------------------------
int shellTextureUpload(ShellTextureInfo* pTex, ShellTextureSelect* pTexSelect)
{
	int vramAddr=0;
	int clutAddr=0;
	int id=0;
	ShellTextureGroup* pGrp=pTex->next;
	int size= (pGrp)? pGrp->size:pTex->size;
		
	ASSERTS(size<=_bufSizeM, 0);

	_lastTexManaged=1;
	if(_firstManagedUpload) 
	{
		id=1;
		_firstManagedUpload=0;
	}			
	if(size>_bufFreeSpace)	//need to swap buffers
	{
		_AddGifTransferInterrupt();												//mark the end of each buffer to vram
		id=1;
		_bufFreeSpace=_bufSizeM;									//reset the current upload area	
		_uploadBuffer = (_uploadBuffer==(_numBuffers-1))? 0 : _uploadBuffer+1;	//set to start filling next upload area
		_clutAllocd=0;												//reset clut & image allocation etc
		_imageAllocd=0;	
	}	
	vramAddr = _bufStart[_uploadBuffer] + _imageAllocd;
	_bufFreeSpace-=size;		
	_imageAllocd+=( (pGrp)? size:(pTex->size-pTex->clutSize) );
	
	if(pGrp) //add a group of textures
	{
		void* pNext=pTex;
		int i=0;
		while(pNext!=NULL)
		{
			if(pNext==pGrp)
			{
				pNext = pGrp->next;
			}
			else	//upload texture using arrangement defined in group
			{
				_pDmaEndTag[_pkt]=_AddGifTransfer(_pDmaEndTag[_pkt], (ShellTextureInfo*)pNext, pGrp->pTBP[i]+vramAddr, pGrp->pCBP[i]+vramAddr, &(pTexSelect[i]) );	
				pNext = ((ShellTextureInfo*)pNext)->next;
				i++;		
			}
		}
	}
	else	//add a normal texture
	{	
		if(pTex->pClut)	//get clut addr by working back from end of the buffer
		{
			clutAddr= (_bufStart[_uploadBuffer] + _bufSizeM) - _clutAllocd - (pTex->clutSize);
			_clutAllocd+=pTex->clutSize;
		}
		_pDmaEndTag[_pkt]=_AddGifTransfer(_pDmaEndTag[_pkt], pTex, vramAddr, clutAddr, pTexSelect);	
	}
	
	return id;
}


//-----------------------------------------------------------------------------
// shellTextureUploadToAddr()
// Description: set up un-managed texture transfer from ee->gs local mem
// Parameters: 	pTex - texture to upload
//              vramAddr - where to put it (in blocks)
//         	    pTexSelect - GS register settings to use texture
//         	    syncRequired - flag if this upload needs to be synchronised with drawing process
// Returns:     void
// Notes:       
//      the current managed buffer is uploaded regardless of how full it was
//      add to internal dma list to upload an un-managed texture
//      if synchronisation is required an interrupt is added to signal when this upload is complete
//      synchronisation is not required for static textures, ie ones that remain in VRAM throughout rendering
//		When the texture is the first in a group all textures in the group are uploaded
//-----------------------------------------------------------------------------
void shellTextureUploadToAddr(ShellTextureInfo* pTex, int vramAddr, ShellTextureSelect* pTexSelect, int syncRequired)
{
	ShellTextureGroup* pGrp=pTex->next;

	if(_lastTexManaged)	//finish adding to upload buffer
	{
		_AddGifTransferInterrupt();
		_bufFreeSpace=_bufSizeM;						//reset the current upload area	
		_uploadBuffer = (_uploadBuffer==(_numBuffers-1))? 0 : _uploadBuffer+1;
		_clutAllocd=0;	
		_imageAllocd=0;
		_firstManagedUpload=1;	
	}
	_lastTexManaged=0;

	//add the transfer
	if(pGrp)
	{
		void* pNext=pTex;
		int i=0;
		while(pNext!=NULL)
		{
			if(pNext==pGrp)
			{
				pNext = pGrp->next;
			}
			else	//upload texture using arrangement defined in group
			{
				_pDmaEndTag[_pkt]=_AddGifTransfer(_pDmaEndTag[_pkt], (ShellTextureInfo*)pNext, pGrp->pTBP[i]+vramAddr, pGrp->pCBP[i]+vramAddr, &(pTexSelect[i]) );	
				pNext = ((ShellTextureInfo*)pNext)->next;
				i++;		
			}
		}	
	}
	else
	{
		_pDmaEndTag[_pkt]=_AddGifTransfer(_pDmaEndTag[_pkt], pTex, vramAddr, vramAddr+(pTex->size-pTex->clutSize), pTexSelect);//put clut straight after image
	}
	
	if(syncRequired)
	{
		_AddGifTransferInterrupt();	
	}
}

//-----------------------------------------------------------------------------
// shellTextureKick()
// Description: setup & kick texture transfer dma on gif, path3 
// Parameters: 	void
// Returns:     ShellDmaTag* points to the start of the dma list
// Notes:       
//-----------------------------------------------------------------------------
void shellTextureKick()
{
	int n=0;
	ShellDmaTag* start=(ShellDmaTag*)(_texUploadPacket[_pkt]);
	ShellDmaTag dmaEnd;
	dmaEnd.ulong.ul[0]=0;
	dmaEnd.ulong.ul[1]=0;
	dmaEnd.fields.id = SHELLDMA_TAG_ID_END;

	ASSERTS(_maxGifDmaQW, "no textures to kick");

	if(_lastTexManaged)	//finish adding to upload buffer
	{
		_AddGifTransferInterrupt();												//mark the end of each buffer to vram
		_bufFreeSpace=_bufSizeM;									//reset the current upload area	
		_uploadBuffer=0;	//set to start on 0 incase config changes at start
		_clutAllocd=0;	//reset clut allocation etc
		_imageAllocd=0;
		_lastTexManaged=0;
	}
	
	//reset tex manager data for new frame / transfer
		
	_vifIRQ=0;
	_gifIRQ=0;
	_irqNum=0;	
	_reconfAdjust=0;
	
	_firstManagedUpload=1;	

	shellDmaSetChannelOptions(SHELLDMA_CHANNEL_GIF,1,0,1,0,0,0);	//tag interrupts enabled tie=1	
	while(n<MAX_NUM_MANAGED_BUFFERS)
	{
		_lastUsed[n]=0;
		n++;
	}
	
	//start building a new gif list
	_pkt ^= 0x01;
	_pDmaEndTag[_pkt] = (ShellDmaTag*)&_texUploadPacket[_pkt][0];
	*(_pDmaEndTag[_pkt])=dmaEnd;
	_pDmaLastInterrupt[_pkt]=NULL;

	if(_paused)
	{
		_paused=0;
		_dmacHandlerId=shellCoreAddDmacHandler(DMAC_GIF, dmacHandler, 0, 0);	
		_vif1HandlerId=shellCoreAddIntcHandler(INTC_VIF1, vif1Handler, 0, 0);	
    }
	
	shellDmaStartChain(SHELLDMA_CHANNEL_GIF, start);
}

//-----------------------------------------------------------------------------
// _AddGifTransferInterrupt()
// Description: adds interrupt to dma list to signal the end of a buffer upload 
// Parameters: 	void
// Returns:     void
// Notes:       
//      add dummy giftag with interrupt bit set, move endtag forward
//-----------------------------------------------------------------------------
static void _AddGifTransferInterrupt()
{
	ShellDmaTag dmaInt, dmaPrevInt;
	int wait_on;
	
	_irqNum++;
		
	if(_lastTexManaged)
	{
		wait_on=_lastUsed[_uploadBuffer];
		_lastUsed[_uploadBuffer]=_irqNum+1;
		
		if(_reconfAdjust)
		{
			wait_on=0;
			_reconfAdjust=0;
		}
	}
	else 
	{
		wait_on=0;			// GIF can just keep going, VIF will still wait ok.
	}

	dmaInt.ulong.ul[0]=0;
	dmaInt.ulong.ul[1]=0;
	dmaInt.fields.id = SHELLDMA_TAG_ID_CNT|SHELLDMA_TAG_ID_IRQ;	

	*(_pDmaEndTag[_pkt]+1)=*(_pDmaEndTag[_pkt]);
	*(_pDmaEndTag[_pkt])=dmaInt;
	
	if(_pDmaLastInterrupt[_pkt]!=NULL)	//this is not the first upload in the list -fill in data on previous upload tag
	{
		dmaPrevInt=*_pDmaLastInterrupt[_pkt];
		dmaPrevInt.fields.mark = (u_char)wait_on;
		*_pDmaLastInterrupt[_pkt]=dmaPrevInt;
	}
	_pDmaLastInterrupt[_pkt]=_pDmaEndTag[_pkt];
	_pDmaEndTag[_pkt]++;
}

#define GET_TBW(w) (w+63)>>6

//-----------------------------------------------------------------------------
// _AddGifTransfer()
// Description: adds texture transfer to dma list, records info for referring to texture 
// Parameters: 	pTex - texture
//         	    vramAddr - load image to this address in VRAM (in GS blocks)
//         	    clutAddr - load clut here
//         	    pTexSelect - GS register settings to use texture
// Returns:     void
// Notes:       
//-----------------------------------------------------------------------------
static ShellDmaTag* _AddGifTransfer(ShellDmaTag* pGifData, ShellTextureInfo* pTex, int vramAddr, int clutAddr, ShellTextureSelect* pTexSelect)
{
#define GIF_NLOOP_MAX 32767

	ShellDmaTag dmaCnt, dmaRef;
	ShellDmaTag* pPkt=pGifData;
	u_int tbw= (pTex->uMode==pTex->imageMode)? pTex->tbw:(u_int)GET_TBW(pTex->uWidth);
	int width = pTex->uWidth;
	int height = pTex->uHeight;
	int addr=vramAddr;
	int m;
	int qwcImage = pTex->imageQW;
	int qwc = pTex->imageQW;
	ShellDmaTag* data = pTex->pImage;
	ShellDmaTag* dataStart = pTex->pImage;
		
	ASSERTS(((u_long128*)_pDmaEndTag[_pkt])<(&_texUploadPacket[_pkt][_maxGifDmaQW]), "transfer buffer too small");
	
	if(	pTex->uMode==SHELLTEXTURE_MODE_USERTAGS )
	{
		pPkt = _AddUserTagsTransfer(pPkt, pTex, vramAddr, pTexSelect);
		return pPkt;
	}


	dmaCnt.ulong.ul[0]=0;
	dmaCnt.ulong.ul[1]=0;
	dmaRef.ulong.ul[0]=0;
	dmaRef.ulong.ul[1]=0;
	
	dmaCnt.fields.id = SHELLDMA_TAG_ID_CNT;				
	dmaCnt.fields.qwc = 6;			//send 4 qwords for gs reg settings
	dmaCnt.fields.next = 0;			//address of next tag

	dmaRef.fields.id = SHELLDMA_TAG_ID_REF;				
	
	for(m=0; m<pTex->mipmaps; m++)
	{		
		pPkt->ulong.ul[0] = (SHELLDMA_TAG_ID_CNT<<24) | 5;
		pPkt->ulong.ul[1] = 0;
		pPkt++;
		pPkt->ulong.ul[0] = SCE_GIF_SET_TAG( 4, 0, 0, 0, SCE_GIF_PACKED, 1 );
		pPkt->ulong.ul[1] = 0xe;
		pPkt++;
		pPkt->ulong.ul[0] = SCE_GS_SET_BITBLTBUF(0, 0, 0, addr, tbw, (u_int)(pTex->uMode) );
		pPkt->ulong.ul[1] = SCE_GS_BITBLTBUF;	
		pPkt++;
		pPkt->ulong.ul[0] = SCE_GS_SET_TRXPOS(0, 0, 0, 0, 0);
		pPkt->ulong.ul[1] = SCE_GS_TRXPOS;
		pPkt++;
		pPkt->ulong.ul[0] = SCE_GS_SET_TRXREG(width, height);
		pPkt->ulong.ul[1] = SCE_GS_TRXREG;
		pPkt++;
		pPkt->ulong.ul[0] = SCE_GS_SET_TRXDIR(0);
		pPkt->ulong.ul[1] = SCE_GS_TRXDIR;
		pPkt++;

		while(qwc>0)
		{	
			dmaRef.fields.qwc = (unsigned short)( (qwc>GIF_NLOOP_MAX)? GIF_NLOOP_MAX:qwc );		
			dmaRef.fields.next = data;			
			qwc-=GIF_NLOOP_MAX;
			data+=GIF_NLOOP_MAX;
			pPkt->ulong.ul[0] = (SHELLDMA_TAG_ID_CNT<<24) | 1;
			pPkt->ulong.ul[1] = 0;
			pPkt++;
			pPkt->ulong.ul[0] = (u_long)SCE_GIF_SET_TAG( dmaRef.fields.qwc, (qwc>0)?0:1, 0, 0, SCE_GIF_IMAGE, 0 );
			pPkt->ulong.ul[1] = 0;
			pPkt++;
			*(pPkt) = dmaRef;
			pPkt++;
		} 
		
		//next mipmap
		qwcImage>>=2;	
		dataStart+=pTex->mmOffset[m];	
		width>>=1;
		height>>=1;	
		addr=vramAddr+pTex->mmTBP[m];
		tbw=pTex->mmTBW[m];			
		//reset 'over NLOOP_MAX' counters
		qwc=qwcImage;	
		data=dataStart;		
	}
	
	if(pTex->mipmaps>1)
	{
		pTexSelect->miptbp1=SCE_GS_SET_MIPTBP1_1( vramAddr+pTex->mmTBP[0], pTex->mmTBW[0], vramAddr+pTex->mmTBP[1], pTex->mmTBW[1], vramAddr+pTex->mmTBP[2], pTex->mmTBW[2]);
		pTexSelect->miptbp2=SCE_GS_SET_MIPTBP2_1( vramAddr+pTex->mmTBP[3], pTex->mmTBW[3], vramAddr+pTex->mmTBP[4], pTex->mmTBW[4], vramAddr+pTex->mmTBP[5], pTex->mmTBW[5]);
		pTexSelect->tex1=SCE_GS_SET_TEX1(_ts.mmLodMethod,pTex->mipmaps-1,
								_ts.magFilter,_ts.mmFilter,_ts.mmAuto,_ts.mmL,_ts.mmK);
	}
	else
	{
		pTexSelect->tex1=SCE_GS_SET_TEX1(0,0,_ts.magFilter,_ts.minFilter,0,0,0);
	}
	
	if(pTex->pClut)
	{
		dmaCnt.fields.next = (pPkt)+7;			//address of next packet
		dmaRef.fields.qwc = (unsigned short)pTex->clutQW;		//size of vif data 
		dmaRef.fields.next = pTex->pClut;			//address of data
		
		*(pPkt) = dmaCnt;
		pPkt++;
		pPkt->ulong.ul[0] = SCE_GIF_SET_TAG( 4, 0, 0, 0, SCE_GIF_PACKED, 1 );
		pPkt->ulong.ul[1] = 0xe;
		pPkt++;
		pPkt->ulong.ul[0] = SCE_GS_SET_BITBLTBUF(0, 0, 0, clutAddr, 1, (u_int)(pTex->clutMode) );
		pPkt->ulong.ul[1] = SCE_GS_BITBLTBUF;	
		pPkt++;
		pPkt->ulong.ul[0] = SCE_GS_SET_TRXPOS(0, 0, 0, 0, 0);
		pPkt->ulong.ul[1] = SCE_GS_TRXPOS;
		pPkt++;
		pPkt->ulong.ul[0] = SCE_GS_SET_TRXREG(pTex->clutWidth, pTex->clutHeight);
		pPkt->ulong.ul[1] = SCE_GS_TRXREG;
		pPkt++;
		pPkt->ulong.ul[0] = SCE_GS_SET_TRXDIR(0);
		pPkt->ulong.ul[1] = SCE_GS_TRXDIR;
		pPkt++;
		pPkt->ulong.ul[0] = (u_long)SCE_GIF_SET_TAG( pTex->clutQW, 1, 0, 0, SCE_GIF_IMAGE, 0 );
		pPkt->ulong.ul[1] = 0;
		pPkt++;
		*(pPkt) = dmaRef;
		pPkt++;	
	}
	
	pPkt->ulong.ul[0] = (SHELLDMA_TAG_ID_END<<24) | 0;
	pPkt->ulong.ul[1] = 0;
	pTexSelect->tex0=SCE_GS_SET_TEX0(vramAddr, pTex->tbw, pTex->imageMode, pTex->imageW, pTex->imageH, pTex->flag.TCC, pTex->flag.TFX, clutAddr, pTex->clutMode, 0, 0, 1);

	return pPkt;
}

//-----------------------------------------------------------------------------
// _AddUserTagsTransfer()
// Description: adds texture transfer to dma list, records info for referring to texture 
// Parameters: 	pTex - texture
//         	    vramAddr - load image to this address in VRAM (in GS blocks)
//         	    pTexSelect - GS register settings to use texture
// Returns:     void
// Notes:       instead of building the usual transfer tags, this function inserts
//				a call to user defined tags, this allows different sorts of transfers
//				such as ipu images.
//-----------------------------------------------------------------------------
static ShellDmaTag* _AddUserTagsTransfer(ShellDmaTag* pGifData, ShellTextureInfo* pTex, int vramAddr, ShellTextureSelect* pTexSelect)
{
	ShellDmaTag* pPkt=pGifData;

	pPkt->ulong.ul[0] = (SHELLDMA_TAG_ID_CNT<<24) | 2;
	pPkt->ulong.ul[1] = 0;
	pPkt++;
	pPkt->ulong.ul[0] = SCE_GIF_SET_TAG( 1, 0, 0, 0, SCE_GIF_PACKED, 1 );
	pPkt->ulong.ul[1] = 0xe;
	pPkt++;
	pPkt->ulong.ul[0] = SCE_GS_SET_BITBLTBUF(0, 0, 0, vramAddr, pTex->tbw, pTex->imageMode );
	pPkt->ulong.ul[1] = SCE_GS_BITBLTBUF;	
	pPkt++;
	pPkt->ulong.ul[0] = 0;
	pPkt->ulong.ul[1] = 0;
	pPkt->fields.id = SHELLDMA_TAG_ID_CALL;
	pPkt->fields.next = pTex->pImage;
	pPkt++;

	pPkt->ulong.ul[0] = (SHELLDMA_TAG_ID_END<<24) | 0;
	pPkt->ulong.ul[1] = 0;

	pTexSelect->tex0=SCE_GS_SET_TEX0(vramAddr, pTex->tbw, pTex->imageMode, pTex->imageW, pTex->imageH, pTex->flag.TCC, pTex->flag.TFX, 0, 0, 0, 0, 1);
	pTexSelect->tex1=SCE_GS_SET_TEX1(0,0,_ts.magFilter,_ts.minFilter,0,0,0);

	return pPkt;
}
//-----------------------------------------------------------------------------
// dmacHandler()
// Description: interrupt handler for texture synchronisation 
// Parameters: 	cause - 
//         	    arg - 
//         	    addr - interrupt processing status
// Returns:     int
// Notes:       
//      checks GIF & VIF1 channels, mainly for interrupts in texture upload list
//      checks what sort of interrupt - is it an end of transfer / tag irq=1 interrupt...
//      manages which buffer needs to be loaded next 
//      allows upload to continue if its safe to do so 
//-----------------------------------------------------------------------------

int dmacHandler(int channel, void* arg, void* addr)
{
(void) arg;
(void) addr;

	if(channel==DMAC_GIF) // Check the channel
	{
		int n=(int) *D2_CHCR;

		DPUT_VIF1_FBRST(VIF1_FBRST_STC_M);		// Always restart VIF

		if(n & 0x80000000)				// Restart GIF if this is an interrupt
		{
			n>>=16;
			n&=0xff;
			
			_gifIRQ++;
			if(_vifIRQ>=n)
				*D2_CHCR|=0x100;
			else
				_gifStall=n;
		}
	}

	ExitHandler();	
	return 0;
}

//-----------------------------------------------------------------------------
// vif1Handler()
// Description: interrupt handler for texture synchronisation 
// Parameters: 	cause - 
//         	    arg - interrupt processing status
//         	    addr - 
// Returns:     int
// Notes:    
//      checks for VIF1 IRQ interrupts in geometry rendering
//      starts texture transfer to GIF 
//      allows drawing to continue if the correct buffer is ready  
//-----------------------------------------------------------------------------
int vif1Handler(int cause, void* arg, void* addr) 
{
(void) cause;
(void) arg;
(void) addr;

	_vifIRQ++;

	if(_gifStall && _vifIRQ>=_gifStall)
	{
		*D2_CHCR|=0x100;
		_gifStall=0;
	}
		
	if(_vifIRQ<=_gifIRQ)
		DPUT_VIF1_FBRST(VIF1_FBRST_STC_M);

    ExitHandler();  // Must call this just before returning from handler
    return 0;
}


//----------------------------------------------------------------------------
// code to load tim2 file in ee mem
//----------------------------------------------------------------------------

// for Loading Tim2 format ...
// constant definition for ClutType & ImageType in picture header
typedef enum  
{
	TIM2_NONE = 0,			// no CLUT (for ClutType)
	TIM2_RGB16,				// 16 bit color (for both of ClutType, ImageType)
	TIM2_RGB24,				// 24 bit color (for ImageType)
	TIM2_RGB32,				// 32 bit color (for ClutType, ImageType)
	TIM2_IDTEX4,			// 16 color texture (for ImageType)
	TIM2_IDTEX8				// 256 color texture (for ImageType)

} ETim2Attr; //TIM2_gattr_type;

typedef struct _Tm2PictureHeader		// TIM2 picture header
{
	unsigned int totalSize;				// total size of the picture data in bytes
	unsigned int clutSize;				// CLUT data size in bytes
	unsigned int imageSize;				// image data size in bytes
	unsigned short headerSize;				// amount of headers
	unsigned short clutColors;				// colors in CLUT
	unsigned char pictFormat;				// picture format
	unsigned char mipmapTextures;			// number of MIPMAP texture
	unsigned char clutType;				// CLUT type
	unsigned char imageType;				// image type
	unsigned short imageWidth;				// width of image (not in bits)
	unsigned short imageHeight;			// height of image (not in bits)

	unsigned long gsTex0;					// TEX0
	unsigned long gsTex1;					// TEX1
	unsigned int gsTexaFbaPabe;			// bitfield of TEXA, FBA and PABE
	unsigned int gsTexClut;				// TEXCLUT (lower 32 bits)

} Tm2PictureHeader;

typedef struct _Tm2FileHeader				// TIM2 file header
{
	unsigned char fileId[4];				// file ID ('T','I','M','2' or 'C','L','T','2')
	unsigned char formatVersion;			// version of file format
	unsigned char formatId;					// ID of format
	unsigned short pictures;				// number of picture data
	unsigned char pad[8];					// for alignment

} Tm2FileHeader;
	
typedef struct _Tm2MipmapHeader				// TIM2 MIPMAP header
{
	unsigned long gsMiptbp1;				// MIPTBP1 (actual 64 bit image)
	unsigned long gsMiptbp2;				// MIPTBP2 (actual 64 bit image)
	unsigned int mmImageSize[8];			// image size of N-th MIPMAP texture in bytes

} Tm2MipmapHeader;

typedef struct _Tm2ExHeader				// TIM2 extended header
{
	unsigned char exHeaderId[4];		// extended header ID ('e','X','t','\x00')
	unsigned int userSpaceSize;			// size of user space
	unsigned int userDataSize;			// size of user data
	unsigned int reserved;				// reserved

} Tm2ExHeader;

#define SWIZ_NULL (100)

typedef struct _SwizzleData
{
	unsigned char 	swizzleDataId[4];		// extended header ID ('s','w','i','z')
	unsigned short 	width;
	unsigned short	height;
	unsigned char	psm;
	unsigned char	idxMode;

} SwizzleData;

typedef struct _Tm2File
{
	Tm2FileHeader*  	pFileHeader;
	Tm2PictureHeader* 	pPictureHeader;
	Tm2MipmapHeader* 	pMipmapHeader;
	Tm2ExHeader*		pExtendedHeader;
	SwizzleData*		pSwizzleData;
	unsigned char* 		pictureData;
	unsigned char*	 	clutData;

} Tm2File;

static const u_char _pixelModes[]= //map from tim2 types to sce pixel modes
{
	SCE_GS_PSMCT16,
	SCE_GS_PSMCT24,
	SCE_GS_PSMCT32,
	SCE_GS_PSMT4,
	SCE_GS_PSMT8
};

static Tm2File _tim2File; 								//file to be loaded
static int _GetSizeQW(int width, int height, int psm);
static int _GetSizeBlock(int total, int tbw, int w, int h, int psm, int qwc);
static void _GetTim2Info( u_char* pFile, ShellTextureInfo* pTex, int* pWidth, int* pHeight);
static void _FinishInfo( ShellTextureInfo* pTex, int width, int height);
static u_char _FixTBW(int tbw, u_char mode);
static u_char _GetLog2(int nSrc);

//-----------------------------------------------------------------------------
// shellTextureLoad()
// Description: Load a tim2 file 
// Parameters: 	file - Tim2 filename to load
// Returns:     texture struct
// Notes:       
//-----------------------------------------------------------------------------
ShellTextureInfo* shellTextureLoad(char* file)
{
	u_char* pFile=NULL;	//file ptr
	unsigned int len=0;
	unsigned int fileBlocks=0;
	int width=0, height=0;
	ShellTextureInfo* pTex=shellMemoryAllocate(16, sizeof(ShellTextureInfo));	//make a textureInfo struct on the heap

	//allocate more memory from the current heap & load the whole file
	len=(unsigned int)shellFileGetSize(file);
	ASSERTS(len, "shellFileGetSize failed");
	pFile=shellMemoryAllocate(128, len);	//align it to 128 incase tm2 is 128byte aligned
	ASSERTS(pFile, "shellMemoryAllocate failed");
	fileBlocks=(len+2047)>>11;
	shellFileRead( file, 1, pFile, 0, (int)fileBlocks);
//	PRINT("\nopened %s\n\n", file);
	
	_GetTim2Info( pFile, pTex, &width, &height);
	_FinishInfo( pTex, width, height);
	
	return pTex;
}

//-----------------------------------------------------------------------------
// ShellTextureMemLoad()
// Description: Set ShellTextureInfo from a loaded tim2 file 
// Parameters:  file - Tim2 file location
// Returns:     texture struct
// Notes:       
//-----------------------------------------------------------------------------
ShellTextureInfo* shellTextureMemLoad(u_char* file)
{
	ShellTextureInfo* pTex=shellMemoryAllocate(16, sizeof(ShellTextureInfo));	//make a textureInfo struct on the heap
	int width=0, height=0;

	_GetTim2Info( file, pTex, &width, &height);
	_FinishInfo( pTex, width, height);

	return pTex;
}

//-----------------------------------------------------------------------------
// ShellTextureMemLoadRaw()
// Description: Set ShellTextureInfo from pre-loaded file/ raw data 
// Parameters:  pSet - contains all info needed to set up ShellTextureInfo
// Returns:     texture struct
// Notes:       
//-----------------------------------------------------------------------------
ShellTextureInfo* shellTextureMemLoadRaw(ShellTextureSetUp* pSet)
{
	ShellTextureInfo* pTex=shellMemoryAllocate(16, sizeof(ShellTextureInfo));	//make a textureInfo struct on the heap

	pTex->pClut=(ShellDmaTag*)(pSet->pClut);
	pTex->clutMode=pSet->clutMode;
	pTex->pImage=(ShellDmaTag*)(pSet->pImage);
	pTex->imageMode=pSet->imageMode;
	pTex->uMode=pSet->swizMode;	
	pTex->mipmaps=(pSet->mipmaps<2)? (u_char)1:(u_char)1;	//no mipmap support here yet
	
	if(pSet->imageMode==pSet->swizMode)
	{
		pTex->uWidth=pSet->width;
		pTex->uHeight=pSet->height;
	}
	else
	{
		pTex->uWidth=pSet->swizWidth;
		pTex->uHeight=pSet->swizHeight;
	}
	
	//CSM1 clut only
	if(	pTex->imageMode==SCE_GS_PSMT8 || pTex->imageMode==SCE_GS_PSMT8H )	
	{
		pTex->clutWidth = 16;
		pTex->clutHeight = 16; 
	}					
	else if( pTex->imageMode==SCE_GS_PSMT4 || pTex->imageMode==SCE_GS_PSMT4HL || pTex->imageMode==SCE_GS_PSMT4HH ) 
	{
		pTex->clutWidth = 8;
		pTex->clutHeight = 2; 
	}	
	else
	{
		pTex->pClut=NULL;
		pTex->clutSize=0;
	}
	
	_FinishInfo( pTex, pSet->width, pSet->height);
	
	return pTex;
}

//-----------------------------------------------------------------------------
// _FixTBW()
// Description: 
// Parameters:  
// Returns:    
// Notes: 
//-----------------------------------------------------------------------------
static u_char _FixTBW(int tbw, u_char mode)
{
	int tbwReturn=tbw;
	
	if(	mode==SCE_GS_PSMT8 || mode==SCE_GS_PSMT8H || 
		mode==SCE_GS_PSMT4 || mode==SCE_GS_PSMT4HH || mode==SCE_GS_PSMT4HL )
	{
		if(tbw<2) tbwReturn=2;
	}
	else if(tbw<1) tbwReturn=1;
	
	return (u_char)tbwReturn;
}

//-----------------------------------------------------------------------------
// _FinishInfo()
// Description: 
// Parameters:  
// Returns:    
// Notes: 
//-----------------------------------------------------------------------------
static void _FinishInfo( ShellTextureInfo* pTex, int width, int height)
{
	//set more image related info
	pTex->imageW = _GetLog2(width);
	pTex->imageH = _GetLog2(height);
	pTex->tbw=_FixTBW(( (1<<pTex->imageW) >>6), pTex->imageMode);
	pTex->flag.TCC=1;
	pTex->flag.TFX=0;
	pTex->flag.pad1=0;
	pTex->next=NULL;
	
	//calculate sizes in quadwords & vram blocks (256bytes)
	pTex->imageQW = _GetSizeQW(width, height, pTex->imageMode);
	pTex->size = _GetSizeBlock(1, pTex->tbw, pTex->imageW, pTex->imageH, pTex->imageMode, pTex->imageQW);
	
	if(pTex->mipmaps>1)
	{
		int m;
		u_short mmSize=(u_short)(pTex->size>>2);
		u_char tbw=pTex->tbw;
		
		for(m=0; m<pTex->mipmaps; m++)
		{
			tbw=_FixTBW((tbw>>1), pTex->imageMode);
			pTex->mmTBP[m] = (u_short)pTex->size;
			pTex->mmTBW[m] = tbw;
			pTex->size+=mmSize;
			mmSize>>=2;	
			if(mmSize<1) mmSize=1;
		}
	}
		
	if(pTex->pClut)//add clut size in vram blocks
	{
		pTex->clutQW = (u_char)_GetSizeQW(pTex->clutWidth, pTex->clutHeight, pTex->clutMode);
		pTex->clutSize = (u_char)(pTex->clutQW/16);
		if(pTex->clutSize<1) pTex->clutSize=1;
		pTex->size+=pTex->clutSize;
	}
}

//-----------------------------------------------------------------------------
// _GetTim2Info()
// Description: 
// Parameters:  
// Returns:    
// Notes: 
//-----------------------------------------------------------------------------
static void _GetTim2Info( u_char* pFile, ShellTextureInfo* pTex, int* pWidth, int* pHeight )
{
	int userSpaceSize=0; 
	
	//work out where the various tm2 headers are located
	{
	 	int mmHeaderSize=0;
		_tim2File.pFileHeader = (Tm2FileHeader*)pFile;	
		ASSERTS( (   _tim2File.pFileHeader->fileId[0]=='T' 
				 && _tim2File.pFileHeader->fileId[1]=='I'
				 && _tim2File.pFileHeader->fileId[2]=='M' 
				 && _tim2File.pFileHeader->fileId[3]=='2'), "not a TIM2 file\n" );

		ASSERTS( (_tim2File.pFileHeader->pictures==1),"not supporting multiple picture tim2 files, just reading the first\n"); 
		pFile+= (_tim2File.pFileHeader->formatId == 1)? 128 : 16; //128 alignment , otherwise its 16byte aligned
		_tim2File.pPictureHeader = (Tm2PictureHeader*)pFile;
		pFile+=48;
		if(_tim2File.pPictureHeader->mipmapTextures > 1)		//there are MipMap textures
		{
			mmHeaderSize = (_tim2File.pPictureHeader->mipmapTextures > 4)? 48 : 32;
			_tim2File.pMipmapHeader = (Tm2MipmapHeader*)pFile;
			pFile+=mmHeaderSize;
		}
		userSpaceSize = _tim2File.pPictureHeader->headerSize - (48+mmHeaderSize);	
		if(userSpaceSize)	
		{
//			PRINT("User space detected: %d bytes\n", userSpaceSize);
			_tim2File.pExtendedHeader = (Tm2ExHeader*)pFile;
			_tim2File.pSwizzleData = (SwizzleData*)(pFile+sizeof(Tm2ExHeader));
			pFile+=userSpaceSize;
		}
	}
		
	//fill in shellTextureInfo 
	{	
		u_char mipmaps = _tim2File.pPictureHeader->mipmapTextures;
		u_short width = _tim2File.pPictureHeader->imageWidth;
		u_short height = _tim2File.pPictureHeader->imageHeight;
		*pWidth = width;
		*pHeight = height;

		//get some image info
		pTex->pImage = (ShellDmaTag*)pFile;
		pTex->imageMode = _pixelModes[_tim2File.pPictureHeader->imageType - 1];
		pTex->mipmaps = mipmaps;
		if(mipmaps>1)
		{
			int i;
//			sceGsMiptbp1* pMipData1=(sceGsMiptbp1*)&_tim2File.pMipmapHeader->gsMiptbp1;
//			sceGsMiptbp2* pMipData2=(sceGsMiptbp2*)&_tim2File.pMipmapHeader->gsMiptbp2;
			for (i=0; i<mipmaps-1; i++) 
			{				
				pTex->mmOffset[i]=(u_short)(_tim2File.pMipmapHeader->mmImageSize[i]>>4);	//bytes->qwords	
			}	
		}
			
		//set image upload info
		pTex->uMode = pTex->imageMode;		
		if(userSpaceSize>sizeof(Tm2ExHeader))//check if theres valid swizzle data
		{
			if(    _tim2File.pSwizzleData->swizzleDataId[0]=='s'
				&& _tim2File.pSwizzleData->swizzleDataId[1]=='w'
				&& _tim2File.pSwizzleData->swizzleDataId[2]=='i'
				&& _tim2File.pSwizzleData->swizzleDataId[3]=='z')
			{
				if(_tim2File.pSwizzleData->idxMode)			//its 8H/4HH/4HL
				{
					pTex->imageMode=_tim2File.pSwizzleData->idxMode;
					pTex->uMode=_tim2File.pSwizzleData->idxMode;
				}				
				if(_tim2File.pSwizzleData->psm!=SWIZ_NULL)	//its been swizzled
				{
					pTex->uMode=_tim2File.pSwizzleData->psm;
					pTex->uWidth=_tim2File.pSwizzleData->width;
					pTex->uHeight=_tim2File.pSwizzleData->height;
				}
			}
		}
		if(pTex->imageMode==pTex->uMode)//its not a swizzled image
		{
			pTex->uWidth=width;
			pTex->uHeight=height;
		}		
		
		//set clut info
		if(_tim2File.pPictureHeader->clutType)	
		{
			pTex->pClut = (ShellDmaTag*)( pFile + (_tim2File.pPictureHeader->imageSize) );	
			pTex->clutMode = _pixelModes[((_tim2File.pPictureHeader->clutType) & 0x1F ) - 1];  
			{
				int csm= _tim2File.pPictureHeader->clutType >>7; //CSM1=0,CSM2=1
				if(csm==0) 	//CSM1 
				{
					if(_tim2File.pPictureHeader->clutColors==16) 
					{
						pTex->clutWidth = 8;
						pTex->clutHeight = 2; 
					}
					else 
					{
						pTex->clutWidth = 16;
						pTex->clutHeight = 16; 
					}
				}	
				else 		//CSM2 
				{
					pTex->clutWidth = (u_char)_tim2File.pPictureHeader->clutColors;
					pTex->clutHeight = 1;		
				}
			}			
		}
		else
		{
			pTex->pClut=NULL;
			pTex->clutSize=0;
		}	
	}
}

//-----------------------------------------------------------------------------
// _GetPower2()
// Description: return n where 2^n = src
// Parameters: nSrc - 64, 128, 256, 512, 1024 
// Returns:    n where 2^n = src
// Notes: 
//-----------------------------------------------------------------------------
static u_char _GetLog2(int nSrc)
{
	int n=0;
	int src=nSrc;
	
	while(src>>=1)
	{
		n++;
	}	
	WARNING(nSrc==1<<n,"width or height is not a power of 2, ie 64, 128, 256...");	
	if(nSrc>(1<<n)) n++;	//if src is not an exact power of 2 round up transfer dimensions (then tex coord range needs to be reduced)
	
	return (u_char)n;
}

//-----------------------------------------------------------------------------
// _GetSizeQW()
// Description: calculate the quadword count 
// Parameters: 	width - transfer width
//				height - transfer height
//				psm - gs pixel storage mode
// Returns:    size of transfer in quadwords
// Notes: 
//-----------------------------------------------------------------------------
static int _GetSizeQW(int width, int height, int psm)
{
	int size=0;
	
	switch(psm)
	{
	case SCE_GS_PSMCT32:
	case SCE_GS_PSMZ32:
		size = ((width*height)>>2);
		break;
	case SCE_GS_PSMCT24:
	case SCE_GS_PSMZ24:
		size = ((width*height*3)>>4);
		break;
	case SCE_GS_PSMCT16:
	case SCE_GS_PSMCT16S:
	case SCE_GS_PSMZ16:
	case SCE_GS_PSMZ16S:
		size = ((width*height)>>3);
		break;
	case SCE_GS_PSMT8:
	case SCE_GS_PSMT8H:
		size = ((width*height)>>4);
		break;
	case SCE_GS_PSMT4:
	case SCE_GS_PSMT4HL:
	case SCE_GS_PSMT4HH:
		size = ((width*height)>>5);
		break;
	}
		
	WARNING((size <= 32767), "texture is too big for single transfer");
	
	return size;
}

//-----------------------------------------------------------------------------
// _GetSizeBlock()
// Description: calculate the size in vram 
// Parameters: 	total - including unused areas, 
//						ie padding on clut based <page images & non power2 sizes
//				tbw - transfer tbw
//				h - transfer height (in power of 2 's)
//				psm - gs pixel storage mode
// Returns:    size of transfer in gs blocks (64bytes)
// Notes: 
//-----------------------------------------------------------------------------
static int _GetSizeBlock(int total, int tbw, int w, int h, int psm, int qwc)
{
	int size=0;
	
	if(total)	//count total vram area, include all padding
	{
		int width=tbw<<6;
		int height=1<<h;
		int padHeight=0;
		
		if(w>h)	
		{
			//flag when width>height
			//if height is less than a page, upload will go into non sequential blocks
			// so pad to 1 page height... 
			padHeight=1;
			WARNING(0,"w>h, if h<1 page, manager will pad to 1 page");
		}
		
		switch(psm)
		{
		case SCE_GS_PSMCT32:
		case SCE_GS_PSMZ32:
		case SCE_GS_PSMCT24:
		case SCE_GS_PSMZ24:
		case SCE_GS_PSMT8H:
		case SCE_GS_PSMT4HL:
		case SCE_GS_PSMT4HH:
			if(padHeight&&height<32) height=32;
			size = ((width*height)>>6);
			break;
		case SCE_GS_PSMCT16:
		case SCE_GS_PSMCT16S:
		case SCE_GS_PSMZ16:
		case SCE_GS_PSMZ16S:
			if(padHeight&&height<64) height=64;
			size = ((width*height)>>7);
			break;
		case SCE_GS_PSMT8:
			if(padHeight&&height<64) height=64;
			size = ((width*height)>>8);
			break;
		case SCE_GS_PSMT4:
			if(padHeight&&height<128) height=128;
			size = ((width*height)>>9);
			break;
		}	
	}
	else	//only count the 24->32bit padding
	{
		size=( (psm==SCE_GS_PSMCT24)? (qwc*4/3 )/16 : qwc/16 );	//24 bit image is padded to 32 in vram	
	}
	
	ASSERTS(size>0, "block size 0"); 
		
	return size;
}

//-----------------------------------------------------------------------------
// shellTextureGroupBegin()
// Description: Start defining a group of textures
// Parameters:  pFirstTex - the first texture in the group
//              numOfTextures - the maximum number of textures which will be in the group
// Returns:     void
// Notes:       
//-----------------------------------------------------------------------------
void shellTextureGroupBegin(ShellTextureInfo* pFirstTex, int numOfTextures)
{
	ShellTextureGroup* pGrp = shellMemoryAllocate(16, sizeof(ShellTextureGroup));
	pGrp->pGifData=NULL;
	pGrp->next=NULL;
	pGrp->size=numOfTextures;
	pGrp->pTBP = shellMemoryAllocate(16,numOfTextures*sizeof(short));
	pGrp->pCBP = shellMemoryAllocate(16,numOfTextures*sizeof(short));
	pGrp->nTextures=1;
	WARNING((pFirstTex->next==NULL), "overwriting previous group data");
	pFirstTex->next=pGrp;
}

//-----------------------------------------------------------------------------
// shellTextureGroupAdd()
// Description: continue defining a group of textures
// Parameters:  pFirstTex - the first texture in the group
//              pTex - a texture to be added to the group
// Returns:     void
// Notes:       
//-----------------------------------------------------------------------------
void shellTextureGroupAdd(ShellTextureInfo* pFirstTex, ShellTextureInfo* pTex)
{
	ShellTextureGroup* pGrp=pFirstTex->next;
	if(pGrp->next==NULL)	//2nd tex is linked with grp struct
	{
		pGrp->next=pTex;
	}
	else					//next tex is added onto the end of the group
	{
		ShellTextureInfo* pLastTex = pGrp->next;
		while(pLastTex->next!=NULL)
		{
			 pLastTex = pLastTex->next;
		}
		pLastTex->next=pTex;
	}
	
	WARNING((pTex->next==NULL), "overwriting previous group data");
	pTex->next=NULL;	//make sure group terminates
	pGrp->nTextures++;
}

//-----------------------------------------------------------------------------
// shellTextureGroupEnd()
// Description: finish defining a group of textures
// Parameters:  pFirstTex - the first texture in the group
// Returns:     void
// Notes:	do a simple arrangement in vram for now- 
//			all cluts are at the end, leave all padding unused...
//-----------------------------------------------------------------------------
void shellTextureGroupEnd(ShellTextureInfo* pFirstTex)
{
	ShellTextureInfo* pTex=pFirstTex;
	ShellTextureGroup* pGrp=pFirstTex->next;
	short tbpOffset=0;
	short cbpOffset=0;
	int i=0;
	
	ASSERTS((pGrp->size>=pGrp->nTextures),"too many textures in group"); 
		
	while(pTex!=NULL)
	{
		pGrp->pTBP[i]=tbpOffset;
		tbpOffset+=(short)(pTex->size-pTex->clutSize);	
		if(pTex->pClut)//calculate clut addr relative to end of entire upload
		{
			cbpOffset+=pTex->clutSize;
			pGrp->pCBP[i]=cbpOffset;
		}
		else
		{
			pGrp->pCBP[i]=0;
		}			
		pTex = (i)? pTex->next:pGrp->next;
		i++;
	}		
	pGrp->size=tbpOffset+cbpOffset;	//set total size of group in gs blocks
	
	for(i=0; i<pGrp->nTextures; i++)//make clut offset rel. start, zero offset means no clut
	{
		if(pGrp->pCBP[i]) 
			pGrp->pCBP[i]=(short)(pGrp->size-pGrp->pCBP[i]);
	}
}




