
#ifndef __TEXTURE_DBL_H__
#define __TEXTURE_DBL_H__

/******************* defines *****************************/

/* flag bits for the u32Flags field in ts_TextureChunkRecord */

#define TEX_FLAG_SIZE_MASK					0x00000007
#define TEX_FLAG_SIZE_4_BIT				0x00000000
#define TEX_FLAG_SIZE_8_BIT				0x00000001
#define TEX_FLAG_SIZE_16_BIT				0x00000002
#define TEX_FLAG_SIZE_24_BIT				0x00000003
#define TEX_FLAG_SIZE_32_BIT				0x00000004
#define TEX_FLAG_SIZE_S3TC					0x00000005

#define TEX_FLAG_FMT_SHIFT					3												// leave room for size bits
#define TEX_FLAG_FMT_MASK					(0x00000007 << TEX_FLAG_FMT_SHIFT)
#define TEX_FLAG_FMT_RGB					(0x00000000 << TEX_FLAG_FMT_SHIFT)
#define TEX_FLAG_FMT_CI						(0x00000001 << TEX_FLAG_FMT_SHIFT)	// color-index-- paletted
#define TEX_FLAG_FMT_IA						(0x00000002 << TEX_FLAG_FMT_SHIFT)
#define TEX_FLAG_FMT_RGBA					(0x00000003 << TEX_FLAG_FMT_SHIFT)
#define TEX_FLAG_FMT_LUMINANCE			(0x00000004 << TEX_FLAG_FMT_SHIFT)	// for light maps
#define TEX_FLAG_FMT_CI_IA					(0x00000005 << TEX_FLAG_FMT_SHIFT)	// color-index -- intensity/alpha palette

/* additional flag bits for the u32Flags field in ts_TextureChunkRecord */

#define TEX_FLAG_ANIMATED_TEXTURE		0x00000040
#define TEX_FLAG_TEXTURE_MALLOCED		0x00000080	// for ps2-to-pc conversion. new buffer was malloc'd to hold converted texture, should be free'd
#define TEX_FLAG_TEXTURE_CONVERTED		0x00000100	// for ps2-to-pc conversion. texture was converted in place, no need to re-do
#define TEX_FLAG_DUMMY_TEXTURE			0x00000200	// for ps2-to-pc conversion. texture is the NULL placeholder for an animated texture set
#define TEX_FLAG_SUBSET0_PALETTE			0x00000400	// for ps2 only, this textures palette belongs in subset 0, not in the subset the texture is in.
#define TEX_FLAG_REFLECTS					0x00000800	// texture is a reflective texture, used for mirrors
#define TEX_FLAG_APERTURE					0x00001000	// texture is an aperture texture, used for views into other parts of the world
#define TEX_FLAG_DIFFUSE					0x00002000	// texture is diffuse-- it gets multiplied against frame buffer when rendered
#define TEX_FLAG_UVREVERSED				0x00004000	// texture has been uv-reversed for more efficient ps2 processing
#define TEX_FLAG_SEPARATE_ALPHA_PLANE	0x00008000	// for GC only,	texture has separate color and alpha planes
																	//						if CI, texture has an additional IA palette
/* for ps2, each block in vram is 64 4-byte words, or 256 bytes. refer
 * to gs manual 8.1 for more info */
// THESE ARE DEPRICATED!!!  They should be moved into the PS2 specific
// code block, and never be seen from again.
#define BLOCKS_PER_VRAM_PAGE				32
#define PSMCT16_VRAM_PAGE_HEIGHT			64	// in pixels
#define VRAM_BYTES_PER_WORD				4
#define BUFFER_ALIGN_BYTES					(2048 * VRAM_BYTES_PER_WORD)	// frame and z-buffers must be on 2048 vram-word boundary
#define VRAM_BYTES_PER_BLOCK				256									// texture addresses are in vram blocks
#define VRAM_TOTAL_BYTES					(1024*1024*4) // total size in bytes of vram
#define VRAM_NUMBER_OF_BLOCKS				(VRAM_TOTAL_BYTES / VRAM_BYTES_PER_BLOCK) // total blocks in vram
#define VRAM_BYTES_PER_PAGE				(1024*8)

#define TEXTURE_SET_NAME_LENGTH			32
#define TEXTURE_NAME_LENGTH				32

																	//						if RGBA, texture is S3TC compressed color, and 8-bit IA


struct ts_TextureChunkRecord
{
/***** if this structure changes, update the correct version of the overloaded function DBLUtil__WriteRecord in MultiGen Plug-ins\Export\DBLWrite.cpp ****/	

	u32	u32Flags;										// format and pixel depth, and more

	/* linked list of ts_TextureChunkRecords to support mipmaps. the mipmap sub-level texture chunk records are not in */
	/* the "real" texture record array-- they follow it, and get chained back to their "parent" image record. in this */
	/* way, they don't go through the TextureBind__ProcessTexture routine, so they don't get a bind assigned-- they */
	/* processed as a sub-image of their parent */

	union
	{
		u32								u32NextOffset;	// location in chunk (offset from start of chunk)
		ts_TextureChunkRecord	*pNext;			// texture in memory
	};
	u16	u16MipmapDistance;							// desired distance for starting mipmap switch, in meters. not all platforms support this

	/* image */

	union
	{
		u32	u32TextureOffset;							// location in chunk (offset from start of chunk)
		void	*pTexture;									// texture in memory
	};
	s16	s16TextureWidth;								// in pixels
	s16	s16TextureHeight;								// in pixels
	s16	s16TextureBlock;								// ps2 only-- target vram block, delta from beginning vram block of texture set

	/* sub-set number - currently ps2 only */
	u32	u32SubSetNumber;

	/* palette-- currently ps2 only */

	union
	{
		u32		u32PaletteOffset;						// location in chunk (offset from start of chunk)
		ts_bRGBA	*pPalette;								// palette in memory
	};
	s16	s16PaletteWidth;								// ps2 only-- palette gets treated like a small texture
	s16	s16PaletteHeight;								// ps2 only-- palette gets treated like a small texture
	s16	s16PaletteBlock;								// ps2 only-- target vram block, delta from beginning vram block of texture set

	/* for debugging */
	s16	s16ImageBlocks;	// number of blocks used by image (including possible wasted blocks) for out-of-vram check
	char	acTextureName[TEXTURE_NAME_LENGTH];

/***** if this structure changes, update the correct version of the overloaded function DBLUtil__WriteRecord in MultiGen Plug-ins\Export\DBLWrite.cpp ****/	
};

struct ts_AnimatedTextureChunkHeader
{
/***** if this structure changes, update the correct version of the overloaded function DBLUtil__WriteRecord in MultiGen Plug-ins\Export\DBLWrite.cpp ****/	

	// TJC - a bunch of these values are not used anymore.
	// specifically, I think that s16Timer and s16TimerInit
	// are both unused.  Instead, a new
	// in-game only class TextureAnimator handles managing 
	// the actively animating textures. The unsed values 
	// should be removed.

	ts_AnimatedTextureChunkHeader	*pNext;					// linked list pointer-- does not have to be first
	s16							s16Timer;								// timer to next advance, in milliseconds
	s16							s16TimerInit;							// refresh value of timer, in milliseconds (calculated from FPS)
	u32							u16TextureIndex;						// current image being shown
	u32							u16TextureCount;						// number of images in animation
	union
	{
		u32						u32TextureSlot;						// index into parent texture set's pu16BindList array
		u16						u16Bind;									// bind that this animated texture set uses, resolved at chunk load time
	};
	u16							u16Format;								// platform-specific format for this texture
	u16							u16Type;									// opengl requires 2 values to specify format

	/*** this must be last ***/

	ts_TextureChunkRecord	TextureChunkRecord[1];				// one record per texture in texture set being loaded

/***** if this structure changes, update the correct version of the overloaded function DBLUtil__WriteRecord in MultiGen Plug-ins\Export\DBLWrite.cpp ****/	
};

struct ts_TextureChunkHeader
{
/***** if this structure changes, update the correct version of the overloaded function DBLUtil__WriteRecord in MultiGen Plug-ins\Export\DBLWrite.cpp ****/	

	u32							u32TextureCount;
	u32							u32Flags;
	u32							u32VRAMInfoTableOffset;				// ps2 only, size of each texture sub set in VRAM blocks (offset to it within chunk)
	u32							u32NumTexSubSets;						// TJC - ps2 only, number of sub-sets in this texture including #0
	u32							u32PieceCount;							// for merged texture sets, 0 otherwise
	u32							u32PieceOffset;						// offset to start of texture pieces (from start of chunk)
	char			 				Name[TEXTURE_SET_NAME_LENGTH];	// name of .dbl file that contains texture chunk, for debugging

	/*** this must be last ***/

	ts_TextureChunkRecord	TextureChunkRecord[1];				// one record per texture in texture set being loaded

/***** if this structure changes, update the correct version of the overloaded function DBLUtil__WriteRecord in MultiGen Plug-ins\Export\DBLWrite.cpp ****/	
};

/* Terminology: a "piece" is a subset of a texture, it will have an index into a texture list,
	and an xy offset into that texture. */

struct ts_PieceRecord
{
	s16	s16OffX;			// in pixels
	s16	s16OffY;			// in pixels
	s16	s16Width;  		// in pixels
	s16	s16Height; 		// in pixels
	u16	u16TexIndex; 	// for texture header lookup
	s16	s16HotSpotX;
	s16	s16HotSpotY;
	u16	u16Pieces;	 	// for textures that have been cut up into smaller textures
};

#endif // __TEXTURE_DBL_H__


