//-----------------------------------------------------------------------------
//
// $Workfile: shellTexture.h $
// $Author: Tyler Colbert $
// $Date: 8/15/03 11:17a $
// $Revision: 1 $
//
// Copyright (c) 2002 Sony Computer Entertainment Europe.
// All Rights Reserved.
//
//-----------------------------------------------------------------------------

#ifndef SHELLTEXTURE_H
#define SHELLTEXTURE_H

#include "shellCore.h"

typedef struct _ShellTextureFlags
{
	unsigned char TCC:1;
	unsigned char TFX:2;
	unsigned char pad1:5;
	
} ShellTextureFlags;


typedef struct _ShellTextureInfo
{
	ShellDmaTag* pImage;
	ShellDmaTag* pClut;		
	
	u_short	mmOffset[6];	
	u_short mmTBP[6];
	u_char 	mmTBW[6];	
	
	int		size;			//total upload size, ie image + clut etc...
	int		imageQW;
	u_char 	clutSize;		//clut upload size, in blocks
	u_char 	clutQW;
	
	u_short uWidth;
	u_short uHeight;
	u_char 	uMode;	
	u_char 	imageMode;
	u_char 	imageW;
	u_char 	imageH;
	u_char 	tbw;
	u_char 	clutMode;
	u_char 	clutWidth;
	u_char 	clutHeight;
	u_char 	mipmaps;
	ShellTextureFlags flag;
	
	void*	next;

} ShellTextureInfo;

typedef enum  
{
	SHELLTEXTURE_MODE_USERTAGS = 100,			// upload via user dma tags in pImage

} EShellTextureMode;			// some extra pixel storage modes (in addition to usual SCE_GS_PSM... )


typedef struct _ShellTextureSelect
{
	u_long		miptbp1;				
	u_long		miptbp2;				
	u_long		tex1;
	u_long		tex0;				

} ShellTextureSelect;


typedef struct _ShellTextureSetUp
{
	u_char* pImage;			
	u_char* pClut;
	u_short width;
	u_short height;
	u_char 	imageMode;
	u_char 	swizMode;
	u_short swizWidth;
	u_short swizHeight;
	u_char 	clutMode;
	u_char 	mipmaps;

} ShellTextureSetUp;


#if defined(__LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {  
#endif

//initialise module
void shellTextureInit(int numOfTextures);

//stop interrupts until the next kick
void shellTexturePause();

//load tim2 file into ee mem or set ShellTextureInfo using pre-loaded data
ShellTextureInfo* shellTextureLoad(char* file);
ShellTextureInfo* shellTextureMemLoad(u_char* file);
ShellTextureInfo* shellTextureMemLoadRaw(ShellTextureSetUp* pSet);

//set up some buffers starting at startAddr in vram, taking up the size of freeSpace (specified in pages), 
//so the free area in vram freeSpace= sizeofbuffer*numOfBuffers, will become completely managed by sceeTexture
void shellTextureConfigure(int startAddr, int freeSpace, int numOfBuffers);

//Upload fns: transfer texture from ee->gs local mem
//builds a gif packet internally to be dma'd by shellTextureTransferBegin()

//upload managed texture
//returns flag for syncing & fills in tex register settings to pTexSelect ready to be added to dma list
int shellTextureUpload(ShellTextureInfo* pTex, ShellTextureSelect* pTexSelect);
//upload texture into vram addr
//fills in tex register settings to pTexSelect ready to be added to dma list
void shellTextureUploadToAddr(ShellTextureInfo* pTex, int vramAddr, ShellTextureSelect* pTexSelect, int syncRequired);

//start texture transfer on gif
void shellTextureKick();

//texture group construction in code
void shellTextureGroupBegin(ShellTextureInfo* pFirstTex, int numOfTextures);		//alloc everything b4 adding texture info's
void shellTextureGroupAdd(ShellTextureInfo* pFirstTex, ShellTextureInfo* pTex);
void shellTextureGroupEnd(ShellTextureInfo* pFirstTex);					//generate dma list, do any arrangement etc here


#if defined(__LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif


#endif //SHELLTEXTURE_H
