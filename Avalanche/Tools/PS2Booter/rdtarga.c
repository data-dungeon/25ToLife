#include <sys/types.h>
#include <stdbool.h>
#include <stdio.h>

#include <eekernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include <libdev.h>
#include <eeregs.h>
#include <libgraph.h>
#include <libdma.h>
#include <libpkt.h>
#include <sifdev.h>
#include <sifrpc.h>
#include <libvu0.h>
#include <stdbool.h>



/* Macros to deal with unsigned chars as efficiently as compiler allows */

typedef struct {
	u_char R,G,B;
} COLOURMAP;

u_char *gFile;
int gColourType;	// 0 = 3 cols, 1 = greyscale
u_char *gWholeImage;
int gWidth, gHeight;
int gFilelen;
int gFileoffset;


typedef enum { GET_8BIT_GREY_ROW, GET_8BIT_ROW, GET_16BIT_ROW, GET_24BIT_ROW } PIXELMETHOD;
int	gGetPixelRowsMethod;

typedef enum {READ_RLE_PIXEL, READ_NON_RLE_PIXEL } READMETHOD;
int gReadPixelMethod;


u_char gTga_pixel[4];	// result of readpixel is delivered here
int gPixel_size;		/* Bytes per Targa pixel (1 to 4) */

bool gHasColourmap;
bool gBottomToTop;

COLOURMAP gColourmap[256];

  /* State info for reading RLE-coded pixels; both counts must be init to 0 */
int gBlock_count;		/* # of pixels remaining in RLE block */
int gDup_pixel_count;		/* # of times to duplicate previous pixel */

int gCurrentRow;




/* For expanding 5-bit pixel values to 8-bit with best rounding */

static const u_char c5to8bits[32] = {
    0,   8,  16,  25,  33,  41,  49,  58,
   66,  74,  82,  90,  99, 107, 115, 123,
  132, 140, 148, 156, 165, 173, 181, 189,
  197, 206, 214, 222, 230, 239, 247, 255
};




static int TGA_readByte (void)
/* Read next byte from Targa file */
{
	if(gFileoffset >= gFilelen) {
		printf("premature file end!\n");
		exit(1);
	}
  return gFile[gFileoffset++];
}


static void TGA_readColourmap (int cmaplen, int mapentrysize)
/* Read the colormap from a Targa file */
{
  int i;

  /* Presently only handles 24-bit BGR format */
  if (mapentrysize != 24) {
		printf("colourmap not in 24-bit BGR format!!!\n");
		exit(1);
	}

  for (i = 0; i < cmaplen; i++) {
    gColourmap[i].B =  TGA_readByte();
    gColourmap[i].G =  TGA_readByte();
    gColourmap[i].R =  TGA_readByte();
  }
}











/*
 * read_pixel methods: get a single pixel from Targa file into tga_pixel[]
 */

static void TGA_readNonRlePixel(void) {
		// Read one Targa pixel from the input file; no RLE expansion
	int i;

	for (i = 0; i < gPixel_size; i++) {
		gTga_pixel[i] = TGA_readByte();
	}
}


static void TGA_readRlePixel(void) {
		// Read one Targa pixel from the input file, expanding RLE data as needed 
	int i;

		// Duplicate previously read pixel?
	if(gDup_pixel_count > 0) {
		gDup_pixel_count--;
		return;
	}

		// Time to read RLE block header?
	if(--gBlock_count < 0) { /* decrement pixels remaining in block */
		i = TGA_readByte();
		if (i & 0x80) {		/* Start of duplicate-pixel block? */
			gDup_pixel_count = i & 0x7F; /* number of dups after this one */
			gBlock_count = 0;	/* then read new block header */
		} else {
			gBlock_count = i & 0x7F; /* number of pixels after this one */
		}
	}

		// Read next pixel 
	for(i = 0; i < gPixel_size; i++) {
		gTga_pixel[i] = TGA_readByte();
	}
}


static void TGA_readPixel(void) {
	switch(gReadPixelMethod) {
		case READ_RLE_PIXEL:
			TGA_readRlePixel();
			break;
		case READ_NON_RLE_PIXEL:
			TGA_readNonRlePixel();
			break;
		default:
			printf("error! gReadPixelMethod is not set !\n");
			exit(1);
	}
}





static int TGA_getPixelRow(void) {
	int col;
	u_int t;
	u_char *ptr;

//	printf("reading row %d\n", gCurrentRow);
	ptr = gWholeImage + gCurrentRow * 4 * gWidth;

	for(col = 0; col < gWidth; col++) {
		TGA_readPixel();
		switch(gGetPixelRowsMethod) {
			case GET_8BIT_GREY_ROW:
				ptr[0] = gTga_pixel[0];
				ptr[1] = gTga_pixel[0];
				ptr[2] = gTga_pixel[0];
				break;
			case GET_8BIT_ROW:
			    t = gTga_pixel[0];
			    ptr[0] = gColourmap[t].R;
			    ptr[1] = gColourmap[t].G;
			    ptr[2] = gColourmap[t].B;
				break;

			case GET_16BIT_ROW:
				t = gTga_pixel[0];
				t += gTga_pixel[1] << 8;
						// We expand 5 bit data to 8 bit sample width.
						// The format of the 16-bit (LSB first) input word is
						// xRRRRRGGGGGBBBBB
				ptr[2] = c5to8bits[t & 0x1F];
				t >>= 5;
				ptr[1] = c5to8bits[t & 0x1F];
				t >>= 5;
				ptr[0] = c5to8bits[t & 0x1F];
				break;

			case GET_24BIT_ROW:
				ptr[0] = gTga_pixel[2]; /* change BGR to RGB order */
				ptr[1] = gTga_pixel[1];
				ptr[2] = gTga_pixel[0];
				break;

			default:
				printf("error! gGetPixelRowsMethod is not set !\n");
				exit(1);
		}
		ptr[3] = 0x80;
		ptr += 4;
	}

	if(gBottomToTop) {
		gCurrentRow--;
		if(gCurrentRow < 0) {
			return 1;
		}
	} else {
		gCurrentRow++;
		if(gCurrentRow >= gHeight) {
			return 1;
		}
	}
	return 0;
}




/*
 * Read the file header; return image size and component count.
 */

static void start_input_tga(void) {
	u_char targaheader[18];
	int idlen, cmaptype, subtype, flags, interlace_type, components;
	u_int width, height, maplen;
	bool is_bottom_up;
	int i;

#define GET_2B(offset)	((unsigned int) targaheader[offset] + \
			 (((unsigned int) targaheader[offset+1]) << 8))

	for(i = 0; i < 18; i++) {
		targaheader[i] = TGA_readByte();
	}

		// Pretend "15-bit" pixels are 16-bit --- we ignore attribute bit anyway 
	if(targaheader[16] == 15) {
		targaheader[16] = 16;
	}

	idlen = targaheader[0];
	cmaptype = targaheader[1];
	subtype = targaheader[2];
	maplen = GET_2B(5);
	width = GET_2B(12);
	height = GET_2B(14);
	gPixel_size = targaheader[16] >> 3;
	flags = targaheader[17];	/* Image Descriptor byte */

	is_bottom_up = ((flags & 0x20) == 0);	/* bit 5 set => top-down */
	interlace_type = flags >> 6;	/* bits 6/7 are interlace code */

	if (cmaptype > 1 ||		/* cmaptype must be 0 or 1 */
			gPixel_size < 1 || gPixel_size > 4 ||
			(targaheader[16] & 7) != 0 || /* bits/pixel must be multiple of 8 */
			interlace_type != 0) {	/* currently don't allow interlaced image */
		printf("Cannot load this targa! It contains features I can't handle!\n");
		exit(1);
	}
	
	if (subtype > 8) {
		puts("Run length encoded");
			// It's an RLE-coded file
		gReadPixelMethod = READ_RLE_PIXEL;
		gDup_pixel_count = 0;
		subtype -= 8;
	} else {
		puts("NOT Run length encoded");
			// Non-RLE file
		gReadPixelMethod = READ_NON_RLE_PIXEL;
	}

		// Now should have subtype 1, 2, or 3
	components = 3;		// until proven different
	gColourType = 0;
	puts("assuming full colour");

	switch (subtype) {
		case 1:			/* Colormapped image */
			if (gPixel_size == 1 && cmaptype == 1) {
				puts("8 bit");
				gGetPixelRowsMethod = GET_8BIT_ROW;
			} else {
				printf("colormap problems 1\n");
				exit(1);
			}
			break;
		case 2:			/* RGB image */
			switch (gPixel_size) {
				case 2:
					gGetPixelRowsMethod = GET_16BIT_ROW;
					puts("16 bit");
					break;
				case 3:
					gGetPixelRowsMethod = GET_24BIT_ROW;
					puts("24 bit");
					break;
				case 4:
					gGetPixelRowsMethod = GET_24BIT_ROW;
					puts("32 bit");
					break;
				default:
					printf("Cannot load this targa! It contains features I can't handle!\n");
					exit(1);
					break;
			}
			break;

		case 3:			/* Grayscale image */
			components = 1;
			printf("Grayscale\n");
			gColourType = 1;
			if(gPixel_size == 1) {
				gGetPixelRowsMethod = GET_8BIT_GREY_ROW;
			} else {
				printf("Grayscale image problems. Sorry\n");
				exit(1);
			}
		    break;

		default:
			printf("Cannot load this targa! It contains features I can't handle!\n");
			exit(1);
  	}

	gWholeImage = memalign(16, width * height * 4);

	if(is_bottom_up) {
		puts("Bottom up");
		gBottomToTop = true;
		gCurrentRow = height -1;
	} else {
		gBottomToTop = false;
		gCurrentRow = 0;
	}

	while(idlen--) {		/* Throw away ID field */
		TGA_readByte();
	}

	if(maplen > 0) {
		if (maplen > 256 || GET_2B(3) != 0) {
			printf("Bad colourmap!\n");
			exit(1);
		}
		TGA_readColourmap((int) maplen, targaheader[7]);
		gHasColourmap = true;
	} else {
		if(cmaptype) {
			printf("Colormap problems!\n");
			exit(1);
		}
		gHasColourmap = false;
	}

	gWidth = width;
	gHeight = height;
}


u_char *loadTGAfile(char *filename, int *width, int *height) {
	int fd;

	fd = sceOpen(filename, SCE_RDONLY);
	if(fd < 0) {
		printf("error opening file %s: %d\n", filename, fd);
		exit(1);
	}
	gFilelen = sceLseek(fd, 0, SCE_SEEK_END);
	sceLseek(fd, 0, SCE_SEEK_SET);
	gFile = memalign(16, gFilelen+16);
	if(gFile == NULL) {
		printf("Could not allocate %x bytes of memory for TGA file\n", gFilelen);
		exit(1);
	}
	if(gFilelen != sceRead(fd, gFile, gFilelen)) {
		printf("Error reading file\n");
		exit(1);
	}
	sceClose(fd);
	gFileoffset = 0;

	start_input_tga();

	while(TGA_getPixelRow() == 0);
	free(gFile);

	*width = gWidth;
	*height = gHeight;
	return gWholeImage;
}
