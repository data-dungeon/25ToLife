#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include "ReadImage.h"

/* --------------------------------------------------
 * Local Defines
 * --------------------------------------------------*/

#define KEY_TYPE_SKIP 0
#define KEY_TYPE_DUMP 1
#define KEY_TYPE_RUN 2
#define KEY_TYPE_END 3

#define LARGE_PAGE_SIZE 0x10000

#include <pshpack1.h>
struct TGAHeader 
{
   BYTE  IDFieldLength;
   BYTE  ColorMapType;
   BYTE  ImageType;
   short FirstColorMapEntry;
   short ColorMapLength;
   BYTE  ColorMapEntrySize;
   short XOrigin;
   short YOrigin;
   short ImageWidth;
   short ImageHeight;
   BYTE  BitsPerPixel;
   BYTE  ImageDescriptorBits;
};

typedef struct AnimHeader_tag
{
	char	sFileID[4];
	short nMaxLP,
			nLargePages;
	long	nRecords;
	short nMaxRecordsPerLP;
	unsigned short cbLPTableOffset;
	char	sAnimID[4];
	short nFrameWidth,
			nFrameHeight;
	char	variant,
			version,
			bHasLastDelta,
			bLastDeltaValid,
			pixelType,
			compressionType,
			bOtherRecordsPerFrame,
			bBitmapRecordPerFrame,
			recordTypes[32];
	long	nFrames;
	short	animSpeed;
	char	filler[58],
			unknown[128];
} AnimHeaderDef;

typedef struct AnimPaletteEntry_tag
{
	char rgbBlue,
		  rgbGreen,
		  rgbRed,
		  rgbReserved;
} AnimPaletteEntryDef;

typedef struct AnimPalette_tag
{
	AnimPaletteEntryDef aAnimPaletteEntry[256];
} AnimPaletteDef;

typedef struct AnimLPFTableEntry_tag
{
	short baseRecord,
			nRecords;
	unsigned short cbDataSize;
} AnimLPFTableEntryDef;

typedef struct AnimLPFTable_tag
{
	AnimLPFTableEntryDef aAnimLPFTableEntry[256];
} AnimLPFTableDef;

#include <poppack.h>

/* --------------------------------------------------
 * Local Prototypes
 * --------------------------------------------------*/
BOOL ReadBMP (HANDLE,BITMAPINFO **,char **);
BOOL ReadTGA (HANDLE,BITMAPINFO **,char **);
BOOL ReadPCX (HANDLE,BITMAPINFO **,char **);
BOOL AnimCreateBitMapHeader (BITMAPINFO **,AnimHeaderDef *,AnimPaletteDef *);
BOOL ReadAnimData (HANDLE,short *,BITMAPINFO **,char *(**)[]);
BOOL TGAConvertUncompressed (BITMAPINFO *,char **,char *,struct TGAHeader *);
BOOL TGAConvertCompressed (BITMAPINFO *,char **,char *,struct TGAHeader *);
BOOL TGACreateBitMapHeader (BITMAPINFO **,struct TGAHeader *);
void TGAGetColorMap (BITMAPINFO *,char *,struct TGAHeader *);

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/
BOOL ReadImage (HANDLE hFile,BITMAPINFO **bmi,char **bmBits)
{
	switch (DetermineImageType (hFile))
	{
		case TYPE_BMP:
			return ReadBMP (hFile,bmi,bmBits);

		case TYPE_TGA:
			return ReadTGA (hFile,bmi,bmBits);

		case TYPE_PCX:
			return ReadPCX (hFile,bmi,bmBits);

		case TYPE_ANM:
		case TYPE_UNKNOWN:
		default:
			*bmi = NULL;
			*bmBits = NULL;
			return FALSE;
	}
}

int DetermineImageType (HANDLE hFile)
{
	
	char header[35],
		  footer[35];

	DWORD nCountRead;

	BOOL bResult;

	if (SetFilePointer (hFile,0,NULL,FILE_BEGIN) == -1)
	{
		return TYPE_UNKNOWN;
	}

	bResult = ReadFile (hFile,header,35,&nCountRead,NULL);
	if (!bResult)
	{
		return TYPE_UNKNOWN;
	}

	if (SetFilePointer (hFile,-26,NULL,FILE_END) == -1)
	{
		return TYPE_UNKNOWN;
	}

	bResult = ReadFile (hFile,footer,26,&nCountRead,NULL);
	if (!bResult)
	{
		return TYPE_UNKNOWN;
	}

	if (SetFilePointer (hFile,0,NULL,FILE_BEGIN) == -1)
	{
		return TYPE_UNKNOWN;
	}

	if (header[0] == 'B' && header[1] == 'M')
	{
		return TYPE_BMP;
	}

	if (header[0] == 0x0A)
	{
		return TYPE_PCX;
	}

	if (!strncmp (header,"LPF ",strlen ("LPF ")) && !strncmp (header + 16,"ANIM",strlen ("ANIM")))
	{
		return TYPE_ANM;
	}

	if (!strncmp (footer + 8,"TRUEVISION-TARGA",strlen ("TRUEVISION-TARGA")))
	{
		return TYPE_TGA;
	}

	return TYPE_TGA;
}

BOOL ReadAnim (HANDLE hFile,short *nFrames,BITMAPINFO **bmi,char *(**bmBits)[])
{
	switch (DetermineImageType (hFile))
	{
		case TYPE_ANM:
			return ReadAnimData (hFile,nFrames,bmi,bmBits);

		case TYPE_BMP:
		case TYPE_TGA:
		case TYPE_PCX:
		case TYPE_UNKNOWN:
		default:
			*bmi = NULL;
			*bmBits = NULL;
			return FALSE;
	}
}

/* --------------------------------------------------
 * Local Functions
 * --------------------------------------------------*/
BOOL ReadAnimData (HANDLE hFile,short *nFrames,BITMAPINFO **bmi,char *(**bmBits)[])
{
   
	AnimHeaderDef *AnimHeader;

	AnimPaletteDef *AnimPalette;

	AnimLPFTableDef *AnimLPFTable;

	AnimLPFTableEntryDef *AnimLPFTableEntry;

	char *buffer,
		  *recordData,
		  *rowData,
		   byteKey;

	unsigned short *acbRecordSize;

	short key;

	DWORD	bytes_read = 0,
			file_size = 0;

	BOOL bResult,
		  bNewFrame;

	int i,
		 j,
		 x,
		 lp,
		 keyType,
		 frame,
		 cbFrameSize,
		 moveSize,
		 rowsLeft,
		 record,
		 recordSize;

	*bmi = NULL;
	*bmBits = NULL;
	*nFrames = 0;
	file_size = GetFileSize (hFile,NULL);
	buffer = (char *)malloc (file_size);
	if (buffer == NULL)
	{
		return FALSE;
	}

	bResult = ReadFile (hFile,
							  buffer,
							  file_size,
							  &bytes_read,
							  NULL);

	AnimHeader = (AnimHeaderDef *)buffer;

	AnimPalette = (AnimPaletteDef *)(buffer + sizeof(AnimHeaderDef));

	AnimLPFTable = (AnimLPFTableDef *)(buffer + sizeof(AnimHeaderDef) + sizeof(AnimPaletteDef));

	bResult = AnimCreateBitMapHeader (bmi,AnimHeader,AnimPalette);
	if (!bResult)
	{
		free (buffer);
		return FALSE;
	}

	*nFrames = (short)AnimHeader->nFrames;
	if (AnimHeader->bHasLastDelta && AnimHeader->bLastDeltaValid)
	{
		(*nFrames)--;
	}

	*bmBits = (char *(*)[])malloc (*nFrames * sizeof(char *));
	if (*bmBits == NULL)
	{
		*nFrames = 0;
		free (*bmi);
		*bmi = NULL;
		free (buffer);
		return FALSE;
	}
	
	cbFrameSize = (*bmi)->bmiHeader.biWidth * (*bmi)->bmiHeader.biHeight;

	for (i = 0; i < *nFrames; i++)
	{
		(**bmBits)[i] = (char *)malloc (cbFrameSize);
		if ((**bmBits)[i] == NULL)
		{
			for (j = 0; j < i; j++)
			{
				free ((**bmBits)[j]);
			}
			*nFrames = 0;
			free (*bmi);
			*bmi = NULL;
			free (buffer);
			return FALSE;
		}
	}

	AnimLPFTableEntry = (AnimLPFTableEntryDef *)(buffer + sizeof(AnimHeaderDef) 
																		 + sizeof(AnimPaletteDef) 
																		 + sizeof(AnimLPFTableDef));

	frame = 0;

	rowData = (**bmBits)[frame] + (((*bmi)->bmiHeader.biHeight - 1) * (*bmi)->bmiHeader.biWidth);
	x = 0;
	rowsLeft = (*bmi)->bmiHeader.biHeight;
	
	for (lp = 0; lp < AnimHeader->nLargePages; lp++)
	{
		for (i = 0; i < AnimHeader->nLargePages; i++)
		{
			if (AnimLPFTable->aAnimLPFTableEntry[i].baseRecord == frame)
			{
				AnimLPFTableEntry = (AnimLPFTableEntryDef *)(((char *)(AnimLPFTable + 1)) + (i * LARGE_PAGE_SIZE));
				break;
			}
		}
		acbRecordSize = (unsigned short *)(((char *)AnimLPFTableEntry) + sizeof(AnimLPFTableEntryDef));
		recordData = (char *)(acbRecordSize + (AnimLPFTableEntry->nRecords + 1));
		for (record = 0;record < AnimLPFTableEntry->nRecords; record++)
		{
			bNewFrame = FALSE;
			if (acbRecordSize[record + 1] != 0)
			{
				recordSize = acbRecordSize[record + 1];
				if (*recordData != 'B')
				{
					return FALSE;
				}
				recordSize--;
				recordData++;

				if (recordData[0])
				{
					return FALSE;
				}
				else
				{
					recordSize--;
					recordData++;
				}

				if (recordData[0] == 0)
				{
					recordData += 2;
					recordSize -= 2;
					while (recordSize)
					{
					 	moveSize = min(recordSize,(*bmi)->bmiHeader.biWidth - x);
						memcpy (rowData + x,recordData,moveSize);
						recordSize -= moveSize;
						recordData += moveSize;
						x += moveSize;
						if (x >= (*bmi)->bmiHeader.biWidth)
						{
							rowData -= (*bmi)->bmiHeader.biWidth;
							x = 0;
							rowsLeft--;
						}
					}
					if (rowsLeft <= 0)
					{
						bNewFrame = TRUE;
					}
				}
				else
				{
					recordData += 2;
					recordSize -= 2;
					while (recordSize)
					{
						byteKey = recordData[0];
						recordData++;
						recordSize--;
						if (byteKey > 0)
						{
							key = byteKey;
							keyType = KEY_TYPE_DUMP;
						}
						else
						if (byteKey == 0)
						{
							key = (unsigned char)recordData[0];
							keyType = KEY_TYPE_RUN;
							recordData++;
							recordSize--;
						}
						else
						{
							byteKey &= 0x7F;
							if (byteKey != 0)
							{
								key = byteKey;
								keyType = KEY_TYPE_SKIP;
							}
							else
							{
								key = *((short *)recordData);
								recordData += 2;
								recordSize -= 2;
								if (key > 0)
								{
									keyType = KEY_TYPE_SKIP;
								}
								else
								if (key == 0)
								{
									keyType = KEY_TYPE_END;
								}
								else
								{
									key &= 0x7FFF;
									if (key & 0x4000)
									{
										key &= 0x3FFF;
										keyType = KEY_TYPE_RUN;
									}
									else
									{
										keyType = KEY_TYPE_DUMP;
									}
								}
							}
						}
						switch (keyType)
						{
							case KEY_TYPE_SKIP:
								while (key)
								{
					 				moveSize = min(key,(*bmi)->bmiHeader.biWidth - x);
									key -= moveSize;
									x += moveSize;
									if (x >= (*bmi)->bmiHeader.biWidth)
									{
										rowData -= (*bmi)->bmiHeader.biWidth;
										x = 0;
										rowsLeft--;
									}
								}
								break;
							case KEY_TYPE_DUMP:
								while (key)
								{
					 				moveSize = min(key,(*bmi)->bmiHeader.biWidth - x);
									memcpy (rowData + x,recordData,moveSize);
									key -= moveSize;
									recordData += moveSize;
									recordSize -= moveSize;
									x += moveSize;
									if (x >= (*bmi)->bmiHeader.biWidth)
									{
										rowData -= (*bmi)->bmiHeader.biWidth;
										x = 0;
										rowsLeft--;
									}
								}
								break;
							case KEY_TYPE_RUN:
								while (key)
								{
					 				moveSize = min(key,(*bmi)->bmiHeader.biWidth - x);
									memset (rowData + x,recordData[0],moveSize);
									key -= moveSize;
									x += moveSize;
									if (x >= (*bmi)->bmiHeader.biWidth)
									{
										rowData -= (*bmi)->bmiHeader.biWidth;
										x = 0;
										rowsLeft--;
									}
								}
								recordData++;
								recordSize--;
								break;
							case KEY_TYPE_END:
								bNewFrame = TRUE;
								break;
						}
					}
				}
			}
			else
			{
				bNewFrame = TRUE;
			}

			if (bNewFrame)
			{
				frame++;
				if (frame >= *nFrames)
				{
					free (buffer);
					return TRUE;
				}
				memcpy ((**bmBits)[frame],(**bmBits)[frame - 1],cbFrameSize);
				bNewFrame = FALSE;
				rowData = (**bmBits)[frame] + (((*bmi)->bmiHeader.biHeight - 1) * (*bmi)->bmiHeader.biWidth);
				x = 0;
				rowsLeft = (*bmi)->bmiHeader.biHeight;
			}
		}
	}
	free (buffer);
	return TRUE;
}

BOOL AnimCreateBitMapHeader (BITMAPINFO **bmi,AnimHeaderDef *AnimHeader,AnimPaletteDef *AnimPalette)
{  

	int i;

	*bmi = (BITMAPINFO *)malloc(sizeof(BITMAPINFOHEADER) + (256 * sizeof(RGBQUAD)));
	if (*bmi == NULL)
	{
		return FALSE;
	}

	memset (*bmi,0,sizeof(BITMAPINFOHEADER));

	(*bmi)->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	(*bmi)->bmiHeader.biWidth = AnimHeader->nFrameWidth;
	(*bmi)->bmiHeader.biHeight = AnimHeader->nFrameHeight;
	(*bmi)->bmiHeader.biPlanes = 1;
	(*bmi)->bmiHeader.biBitCount = 8;
	(*bmi)->bmiHeader.biClrUsed = 256;
	(*bmi)->bmiHeader.biClrImportant = 256;

	for (i = 0; i < 256; i++)
	{
		(*bmi)->bmiColors[i].rgbRed = AnimPalette->aAnimPaletteEntry[i].rgbRed;
		(*bmi)->bmiColors[i].rgbGreen = AnimPalette->aAnimPaletteEntry[i].rgbGreen;
		(*bmi)->bmiColors[i].rgbBlue = AnimPalette->aAnimPaletteEntry[i].rgbBlue;
		(*bmi)->bmiColors[i].rgbReserved = 0;
	}

	return TRUE;
}

BOOL ReadTGA (HANDLE hFile,BITMAPINFO **bmi,char **bmBits)
{  
   
	struct TGAHeader *tga_header;

	char	*buffer;

	DWORD	bytes_read = 0,
			file_size = 0;

	BOOL result;

	*bmi = NULL;
	*bmBits = NULL;
	file_size = GetFileSize (hFile,NULL);
	buffer = (char *)malloc (file_size);
	if (buffer == NULL)
	{
		return FALSE;
	}

	result = ReadFile (hFile,
							 buffer,
							 file_size,
							 &bytes_read,
							 NULL);

	tga_header = (struct TGAHeader *)buffer;

	result = TGACreateBitMapHeader (bmi,tga_header);
	if (!result)
	{
		free (buffer);
		return FALSE;
	}

  switch (tga_header->ImageType)
  {  
		case 0:
			//No Image Present;
			break;
  
		case 1:			//Color-Mapped, uncompressed
			TGAGetColorMap (*bmi,buffer,tga_header);
		case 2:			//True Color uncompressed
			result = TGAConvertUncompressed (*bmi,bmBits,buffer,tga_header);
			if (!result)
			{
			  free (buffer);
			  free (*bmi);
			  *bmi = NULL;
			  return FALSE;
			}
			break;

		case 9:			//Color-Mapped, RLE Compressed
			TGAGetColorMap (*bmi,buffer,tga_header);
		case 10:			// True Color, RLE Compressed
			result = TGAConvertCompressed (*bmi,bmBits,buffer,tga_header);
			if (!result)
			{
			  free (buffer);
			  free (*bmi);
			  *bmi = NULL;
			  return FALSE;
			}
			break;

		default:
			break;
	}

	free (buffer);
	
	return TRUE;
}
       
//Fills in the BITMAPINFOHEADER  
BOOL TGACreateBitMapHeader(BITMAPINFO **bmi, struct TGAHeader *tga_header)
{  

	long colorMapSize;

	if (tga_header->ColorMapType)
	{
		colorMapSize = (tga_header->FirstColorMapEntry + tga_header->ColorMapLength) * sizeof(RGBQUAD);
	}
	else
	{
		colorMapSize = 0;
	}
	
	*bmi = (BITMAPINFO *)malloc(sizeof(BITMAPINFOHEADER) + colorMapSize);
	if (*bmi == NULL)
	{
		return FALSE;
	}

	memset (*bmi, '\0', sizeof(BITMAPINFOHEADER));

	(*bmi)->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	(*bmi)->bmiHeader.biWidth = tga_header->ImageWidth;
	(*bmi)->bmiHeader.biHeight = tga_header->ImageHeight;
	(*bmi)->bmiHeader.biPlanes = 1;
	(*bmi)->bmiHeader.biBitCount = tga_header->BitsPerPixel;
	if (tga_header->ColorMapType)
	{
		(*bmi)->bmiHeader.biClrUsed		= tga_header->ColorMapLength;
		(*bmi)->bmiHeader.biClrImportant = tga_header->ColorMapLength;
	}

	return TRUE;
}

BOOL TGAConvertUncompressed (BITMAPINFO *bmi,char **bmBits, char *buffer, struct TGAHeader *tga_header)
{
	
	int i = 0,
		 image_size,
		 rowSize,
		 bytesPerPixel,
		 pad;

	BYTE image_descriptor = 0;
	
	char *buffer_ptr,
		  *bmi_ptr;

	image_descriptor = tga_header->ImageDescriptorBits & 0x30;
	
	bytesPerPixel = (tga_header->BitsPerPixel + 7) / 8;

	rowSize = bytesPerPixel * tga_header->ImageWidth;
	
	pad = 4 - (rowSize % 4);
	if (pad == 4)
	{
		pad = 0;
	}
	
	image_size = (rowSize + pad) * tga_header->ImageHeight;

	*bmBits = (char *) malloc(image_size);
	if (*bmBits == NULL)
	{
		return FALSE;
	}

	buffer_ptr = buffer + sizeof(struct TGAHeader) + tga_header->IDFieldLength;
	
	if (tga_header->ColorMapType)
	{
		buffer_ptr += tga_header->ColorMapLength * ((tga_header->ColorMapEntrySize + 7) / 8);
	}

	switch (image_descriptor)
	{
		case 0x00:
			bmi_ptr = *bmBits;
			for (i = 0; i < tga_header->ImageHeight; i++)
			{
				memcpy (bmi_ptr,buffer_ptr,rowSize);
				memset (bmi_ptr + rowSize,0,pad);
				bmi_ptr += (rowSize + pad);
				buffer_ptr += rowSize;
			}
			break;

		case 0x10:
		  break;

		case 0x20:
			bmi_ptr = *bmBits + ((tga_header->ImageHeight - 1) * (rowSize + pad));
			for (i = 0; i < tga_header->ImageHeight; i++)
			{
				memcpy (bmi_ptr,buffer_ptr,rowSize);
				memset (bmi_ptr + rowSize,0,pad);
				bmi_ptr -= (rowSize + pad);
				buffer_ptr += rowSize;
			}
		  break;

		case 0x30:
		  break;

		default:
		  break;
	}	  
	return TRUE;
}
				  
BOOL TGAConvertCompressed (BITMAPINFO *bmi,char **bmBits, char *buffer, struct TGAHeader *tga_header)
{
	
	int i = 0,
		 image_size,
		 rowSize,
		 bytesPerPixel,
		 rowsLeft,
		 bytesOut,
		 srcCount,
		 pad;

	BYTE image_descriptor = 0;
	
	char *buffer_ptr,
		  *bmi_ptr;

	image_descriptor = tga_header->ImageDescriptorBits & 0x30;
	
	bytesPerPixel = (tga_header->BitsPerPixel + 7) / 8;

	rowSize = bytesPerPixel * tga_header->ImageWidth;
	
	pad = 4 - (rowSize % 4);
	if (pad == 4)
	{
		pad = 0;
	}
	
	image_size = (rowSize + pad) * tga_header->ImageHeight;

	*bmBits = (char *) malloc(image_size);
	if (*bmBits == NULL)
	{
		return FALSE;
	}

	buffer_ptr = buffer + sizeof(struct TGAHeader) + tga_header->IDFieldLength;
	
	if (tga_header->ColorMapType)
	{
		buffer_ptr += tga_header->ColorMapLength * ((tga_header->ColorMapEntrySize + 7) / 8);
	}

	rowsLeft = tga_header->ImageHeight;

	bytesOut = 0;

	switch (image_descriptor)
	{
		case 0x00:
			bmi_ptr = *bmBits;
			do
			{
				srcCount = (*buffer_ptr & 0x7F) + 1;
				if (*buffer_ptr & 0x80)
				{
					buffer_ptr++;
					for (i = 0; i < srcCount; i++)
					{
						memcpy (bmi_ptr,buffer_ptr,bytesPerPixel);
						bmi_ptr += bytesPerPixel;
					}
					buffer_ptr += bytesPerPixel;
				}
				else
				{
					buffer_ptr++;
					memcpy (bmi_ptr,buffer_ptr,bytesPerPixel * srcCount);
					bmi_ptr += bytesPerPixel * srcCount;
					buffer_ptr += bytesPerPixel * srcCount;
				}
				bytesOut += srcCount * bytesPerPixel;
				if (bytesOut >= rowSize)
				{
					if (pad)
					{
						memset (bmi_ptr,0,pad);
						bmi_ptr += pad;
					}
					rowsLeft--;
					bytesOut = 0;
				}
			}
			while (rowsLeft);
			break;

		case 0x10:
		  break;

		case 0x20:
			bmi_ptr = *bmBits + ((tga_header->ImageHeight - 1) * (rowSize + pad));
			do
			{
				srcCount = (*buffer_ptr & 0x7F) + 1;
				if (*buffer_ptr & 0x80)
				{
					*buffer_ptr++;
					for (i = 0; i < srcCount; i++)
					{
						memcpy (bmi_ptr,buffer_ptr,bytesPerPixel);
						bmi_ptr += bytesPerPixel;
					}
					buffer_ptr += bytesPerPixel;
				}
				else
				{
					*buffer_ptr++;
					memcpy (bmi_ptr,buffer_ptr,bytesPerPixel * srcCount);
					bmi_ptr += bytesPerPixel * srcCount;
					buffer_ptr += bytesPerPixel * srcCount;
				}
				bytesOut += srcCount * bytesPerPixel;
				if (bytesOut >= rowSize)
				{
					if (pad)
					{
						memset (bmi_ptr,0,pad);
						bmi_ptr += pad;
					}
					bmi_ptr -= 2 * (rowSize + pad);
					rowsLeft--;
					bytesOut = 0;
				}
			}
			while (rowsLeft);
		  break;

		case 0x30:
		  break;

		default:
		  break;
	}	  
	return TRUE;
}
				  
void TGAGetColorMap (BITMAPINFO *bmi, char *buffer, struct TGAHeader *tga_header)
{

	int i;

	char *map_ptr;

	short *mapShort;

	map_ptr = buffer + sizeof(struct TGAHeader) + tga_header->IDFieldLength;

	switch (tga_header->ColorMapEntrySize)
	{
		case 15:
		case 16:
			for (i = tga_header->FirstColorMapEntry; i < (tga_header->FirstColorMapEntry + tga_header->ColorMapLength); i++)
			{
				mapShort = (short *)map_ptr;
				map_ptr += 2;
				bmi->bmiColors[i].rgbBlue = (*mapShort & 0x1F) << 3;
				bmi->bmiColors[i].rgbGreen = ((*mapShort >> 5) & 0x1F) << 3;
				bmi->bmiColors[i].rgbRed = ((*mapShort >> 10) & 0x1F) << 3;
				bmi->bmiColors[i].rgbReserved = 0;
			}
			break;
		
		case 24:
			for (i = tga_header->FirstColorMapEntry; i < (tga_header->FirstColorMapEntry + tga_header->ColorMapLength); i++)
			{
				bmi->bmiColors[i].rgbBlue = map_ptr[0];
				bmi->bmiColors[i].rgbGreen = map_ptr[1];
				bmi->bmiColors[i].rgbRed = map_ptr[2];
				bmi->bmiColors[i].rgbReserved = 0;
				map_ptr += 3;
			}
			break;

		case 32:
			for (i = tga_header->FirstColorMapEntry; i < (tga_header->FirstColorMapEntry + tga_header->ColorMapLength); i++)
			{
				bmi->bmiColors[i].rgbBlue = map_ptr[0];
				bmi->bmiColors[i].rgbGreen = map_ptr[1];
				bmi->bmiColors[i].rgbRed = map_ptr[2];
				bmi->bmiColors[i].rgbReserved = 0;
				map_ptr += 4;
			}
			break;
		default:
			break;
	}
}

BOOL ReadBMP (HANDLE hFile,BITMAPINFO **bmi,char **bmBits)
{

	BITMAPFILEHEADER bmh;

	BOOL bResult;

	DWORD nCountRead,
			nbmiSize,
			nbmBitsSize;

	*bmi = NULL;
	*bmBits = NULL;

	bResult = ReadFile (hFile,&bmh,sizeof(BITMAPFILEHEADER),&nCountRead,NULL);
	if ((!bResult) || (nCountRead != sizeof(BITMAPFILEHEADER)))
	{
		return FALSE;
	}

	nbmiSize = bmh.bfOffBits - sizeof(BITMAPFILEHEADER);
	*bmi = (BITMAPINFO *)malloc (nbmiSize);
	if (*bmi == NULL)
	{
		return FALSE;
	}

	bResult = ReadFile (hFile,*bmi,nbmiSize,&nCountRead,NULL);
	if ((!bResult) || (nCountRead != nbmiSize))
	{
		free (*bmi);
		*bmi = NULL;
		return FALSE;
	}

	nbmBitsSize = GetFileSize (hFile,NULL) - SetFilePointer (hFile,0,NULL,FILE_CURRENT);
	*bmBits = (char *)malloc (nbmBitsSize);
	if (*bmBits == NULL)
	{
		free (*bmi);
		*bmi = NULL;
		return FALSE;
	}

	bResult = ReadFile (hFile,*bmBits,nbmBitsSize,&nCountRead,NULL);
	if ((!bResult) || (nCountRead != nbmBitsSize))
	{
		free (*bmi);
		free (*bmBits);
		*bmi = NULL;
		*bmBits = NULL;
		return FALSE;
	}

	return TRUE;
}

BOOL ReadPCX (HANDLE hFile,BITMAPINFO **bmi,char **bmBits)
{

	int i,
		 j,
		 k,
		 iPixel,
		 iSrc,
		 moveCount,
		 iSrcRowSize,
		 rowsLeft;

	BOOL bResult;

	DWORD nFileSize,
			nBitsSize,
			nBMISize,
			nRowSize,
			nBMPRowSize,
			nCountRead,
			nBitsPerPixel;

	PCXHeaderDef pcxh;

	BYTE *bmCompressedBits;

	char *rowPtr,
		  *srcPtr,
		  *srcRow,
		   srcByte;

	PCXPaletteDef *pcxPalette;

	*bmi = NULL;
	*bmBits = NULL;

	bResult = ReadFile (hFile,&pcxh,sizeof(PCXHeaderDef),&nCountRead,NULL);
	if ((!bResult) || (nCountRead != sizeof(PCXHeaderDef)))
	{
		return FALSE;
	}

	nFileSize = GetFileSize (hFile,NULL);

	if (nFileSize == 0xFFFFFFFF)
	{
		return FALSE;
	}

	nBitsPerPixel = pcxh.pcxBitsPerPixel * pcxh.pcxPlanes;

	nBitsSize = nFileSize - sizeof(PCXHeaderDef);
	if (pcxh.pcxBitsPerPixel == 8 && pcxh.pcxPlanes == 1)
	{
		nBitsSize -= 256 * sizeof(PCXPaletteDef);
	}

	bmCompressedBits = (BYTE *)malloc (nBitsSize);
	if (bmCompressedBits == NULL)
	{
		return FALSE;
	}

	bResult = ReadFile (hFile,bmCompressedBits,nBitsSize,&nCountRead,NULL);
	if ((!bResult) || (nCountRead != nBitsSize))
	{
		free (bmCompressedBits);
		return FALSE;
	}

	if (pcxh.pcxBitsPerPixel == 8 && pcxh.pcxPlanes == 1)
	{
		pcxPalette = (PCXPaletteDef *)malloc (256 * sizeof(PCXPaletteDef));
		if (pcxPalette == NULL)
		{
			free (bmCompressedBits);
			return FALSE;
		}

		bResult = ReadFile (hFile,pcxPalette,256 * sizeof(PCXPaletteDef),&nCountRead,NULL);
		if ((!bResult) || (nCountRead != 256 * sizeof(PCXPaletteDef)))
		{
			free (pcxPalette);
			free (bmCompressedBits);
			return FALSE;
		}
	}

	nBMISize = sizeof (BITMAPINFO);

	if (nBitsPerPixel <= 8)
	{
		nBMISize += ((1 << nBitsPerPixel) - 1) * sizeof(RGBQUAD);
	}

	*bmi = (BITMAPINFO *)malloc (nBMISize);
	if (*bmi == NULL)
	{
		if (pcxh.pcxBitsPerPixel == 8 && pcxh.pcxPlanes == 1)
		{
			free (pcxPalette);
		}
		free (bmCompressedBits);
		return FALSE;
	}

	if (nBitsPerPixel == 8)
	{
		for (i = 0; i < 256; i++)
		{
			(*bmi)->bmiColors[i].rgbRed = pcxPalette[i].pcxRed;
			(*bmi)->bmiColors[i].rgbGreen = pcxPalette[i].pcxGreen;
			(*bmi)->bmiColors[i].rgbBlue = pcxPalette[i].pcxBlue;
			(*bmi)->bmiColors[i].rgbReserved = 0;
		}

		free (pcxPalette);
	}
	else
	if (nBitsPerPixel < 8)
	{
		for (i = 0; i < (1 << nBitsPerPixel); i++)
		{
			(*bmi)->bmiColors[i].rgbRed = pcxh.pcxHeaderPalette[i].pcxRed;
			(*bmi)->bmiColors[i].rgbGreen = pcxh.pcxHeaderPalette[i].pcxGreen;
			(*bmi)->bmiColors[i].rgbBlue = pcxh.pcxHeaderPalette[i].pcxBlue;
			(*bmi)->bmiColors[i].rgbReserved = 0;
		}
	}

	(*bmi)->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	(*bmi)->bmiHeader.biWidth = pcxh.pcxXMax - pcxh.pcxXMin + 1;
	(*bmi)->bmiHeader.biHeight = pcxh.pcxYMax - pcxh.pcxYMin + 1;
	(*bmi)->bmiHeader.biPlanes = 1;
	(*bmi)->bmiHeader.biBitCount = (short)nBitsPerPixel;
	(*bmi)->bmiHeader.biCompression = BI_RGB;
	(*bmi)->bmiHeader.biSizeImage = 0;
	(*bmi)->bmiHeader.biXPelsPerMeter = 0;
	(*bmi)->bmiHeader.biYPelsPerMeter = 0;
	(*bmi)->bmiHeader.biClrUsed = 1 << nBitsPerPixel;
	(*bmi)->bmiHeader.biClrImportant = 1 << nBitsPerPixel;

	switch ((*bmi)->bmiHeader.biBitCount)
	{
		case 1:
			nRowSize = ((*bmi)->bmiHeader.biWidth + 7) / 8;
			break;

		case 4:
			nRowSize = ((*bmi)->bmiHeader.biWidth + 1) / 2;
			break;

		case 8:
			nRowSize = (*bmi)->bmiHeader.biWidth;
			break;

		case 24:
			nRowSize = (*bmi)->bmiHeader.biWidth * 3;
			break;
	}

	nBMPRowSize = (nRowSize + 3) & ~3;

	*bmBits = (char *)malloc (nBMPRowSize * (*bmi)->bmiHeader.biHeight);
	if (*bmBits == NULL)
	{
		free (*bmi);
		free (bmCompressedBits);
		*bmi = NULL;
		return FALSE;
	}

	memset (*bmBits,'\0',nBMPRowSize * (*bmi)->bmiHeader.biHeight);

	rowPtr = *bmBits + (nBMPRowSize * ((*bmi)->bmiHeader.biHeight - 1));

	rowsLeft = (*bmi)->bmiHeader.biHeight;

	iSrcRowSize = pcxh.pcxBytesPerLine * pcxh.pcxPlanes;
	srcRow = (char *)malloc (iSrcRowSize);
	if (srcRow == NULL)
	{
		free (*bmBits);
		free (*bmi);
		free (bmCompressedBits);
		*bmi = NULL;
		*bmBits = NULL;
		return FALSE;
	}

	srcPtr = bmCompressedBits;
	
	do
	{
		iSrc = 0;
		do
		{
			if ((*srcPtr & 0xC0) == 0xC0)
			{
				moveCount = *srcPtr & 0x3F;
				srcPtr++;
			}
			else
			{
				moveCount = 1;
			}
			memset (srcRow + iSrc,*srcPtr,moveCount);
			srcPtr++;
			iSrc += moveCount;
		}
		while (iSrc < iSrcRowSize);

		if (pcxh.pcxPlanes == 1)
		{
			memcpy (rowPtr,srcRow,nRowSize);
		}
		else
		{
			if (pcxh.pcxBitsPerPixel < 8)
			{
				for (i = 0; i < pcxh.pcxPlanes; i++)
				{
					for (j = 0,iPixel = 0; (j < pcxh.pcxBytesPerLine) && (iPixel < (*bmi)->bmiHeader.biWidth); j++)
					{
						for (k = (8 / pcxh.pcxBitsPerPixel) - 1; (k >= 0) && (iPixel < (*bmi)->bmiHeader.biWidth); k--)
						{
					 		srcByte = srcRow[(i * pcxh.pcxBytesPerLine) + j];
							srcByte >>= k * pcxh.pcxBitsPerPixel;
							srcByte &= ((1 << pcxh.pcxBitsPerPixel) - 1);
							srcByte <<= i * pcxh.pcxBitsPerPixel;
							srcByte <<= nBitsPerPixel * (1 - (iPixel & 0x01));
							rowPtr[iPixel / (8 / nBitsPerPixel)] |= srcByte;
							iPixel++;
						}
					}
				}
			}
			else
			{
				for (i = 0; i < (*bmi)->bmiHeader.biWidth; i++)
				{
					for (j = 0; j < pcxh.pcxPlanes; j++)
					{
						rowPtr[(i * pcxh.pcxPlanes) + j] = srcRow[(((pcxh.pcxPlanes - 1) - j) * pcxh.pcxBytesPerLine) + i];
					}
				}
			}
		}
		rowPtr -= nBMPRowSize;
		rowsLeft--;
	}
	while (rowsLeft);

	free (srcRow);
	free (bmCompressedBits);

	return TRUE;
}

