#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <read_tga.h>


#define TGA_SIGNATURE "TRUEVISION-XFILE"

ReadTGA::ReadTGA( const char *pcFName) : ReadBase( pcFName)
{
	TGAInit( );
}

ReadTGA::ReadTGA( istream &str) : ReadBase( str)
{
	TGAInit( );
}

ReadTGA::ReadTGA( ) : ReadBase( )
{
	TGAInit( );
}

ReadTGA::~ReadTGA( void)
{

	FreeMyMem( );

}

void ReadTGA::TGAInit( void)
{
	bPreped = false;
	bRead = false;

	pCMAP = NULL;
	pImage = NULL;
}

void ReadTGA::FreeMyMem( void)
{
	if( pCMAP)
		free( pCMAP);

	pCMAP = NULL;

	if( pImage)
		free( pImage);

	pImage = NULL;

}

void ReadTGA::StreamChanged( void )
{
	ReadBase::StreamChanged( );

	FreeMyMem( );

	TGAInit( );
}

bool ReadTGA::Prep( void)
{
unsigned long		ulCMAPSize;

	assert( pStream != NULL);

	if( bPreped == true)
		return true;

	if( Error != RERR_NONE)
		return false;

	if( !pStream->good( ))
	{
		Error = RERR_STREAM;
		return false;
	}

	if( GetFooter( ) == false)
		return false;

	CheckFooter( );

	if( GetHeader( ) == false)
		return false;


	if( Header.ucIDLength != 0)
		pStream->read( ImageID, Header.ucIDLength);

	if( Header.ucColorMapType != 0)
	{
		ulCMAPSize = Header.CMAPInfo.usCMAPLength * BitsToBytes( Header.CMAPInfo.ucCMAPEntrySize);

		pCMAP = malloc( ulCMAPSize);

		pStream->read( (char *) pCMAP, ulCMAPSize);

		switch( Header.CMAPInfo.ucCMAPEntrySize)
		{
		case 15:
		case 16:
			SwapRB_Packed555( (unsigned short *) pCMAP, Header.CMAPInfo.usCMAPLength);
			break;
		case 24:
			SwapRB_RGB( (ts_bRGB *) pCMAP, Header.CMAPInfo.usCMAPLength);
			break;
		case 32:
			SwapRB_RGBA( (ts_bRGBA *) pCMAP, Header.CMAPInfo.usCMAPLength);
			break;
		}
	}

	usPixelBytes = BitsToBytes( Header.ImageInfo.ucPixelDepth);

	usRowBytes = Header.ImageInfo.usWidth * usPixelBytes;

	usWidth = Header.ImageInfo.usWidth;
	usHeight = Header.ImageInfo.usHeight;

	if( Header.ImageInfo.ucPixelDepth == 32  ||
		 Header.ImageInfo.ucPixelDepth == 16 )
		Caps |= RB_CAP_HASALPHA;

	bPreped = true;

	if( !pStream->good( ))
		Error = RERR_STREAM;

	return (Error == RERR_NONE) ? true : false;
}

bool ReadTGA::Read( void)
{
bool					bReturnCode;

	assert( pStream != NULL);

	if( bPreped == false)
		if( Prep( ) == false)
			return false;

	if( Error != RERR_NONE)
		return false;

	if( !pStream->good( ))
		return false;

	switch( Header.ucImageType)
	{
	case 0:
		Error = RERR_TGA_NOIMAGE;
		bReturnCode = false;
		break;
	case 1:
	case 2:
		bCompressed = false;
		bReturnCode = ReadRGB( );
		break;
	case 9:
	case 10:
		bCompressed = true;
		bReturnCode = ReadRGB( );
		break;
	case 3:
	case 11:
		Error = RERR_NOTIMPLEMENTED;
		bReturnCode = false;
		break;

	default:
		Error = RERR_BADIMAGE;
		bReturnCode = false;
	}

	bRead = true;

	return bReturnCode;
}

ts_RBImage *ReadTGA::GetImage( t_lflagbits GetFlags)
{
bool					bRetCode;
bool					bPackedAlpha;

unsigned short		usImageBPP;
unsigned short		usImageBits;

ts_RBImage			*pNewImage;

	assert( bRead == true);

	pNewImage = (ts_RBImage *) malloc( sizeof( ts_RBImage));
	memset( pNewImage, 0, sizeof( ts_RBImage));

	pNewImage->usWidth = usWidth;
	pNewImage->usHeight = usHeight;

	bPackedAlpha = false;

	switch( GetFlags & RB_GET_MASK)
	{
	case RB_GET_RGB:
		usImageBits = 24;
		break;
	case RB_GET_RGBA:
		usImageBits = 32;
		break;
	case RB_GET_PACKED555:
		usImageBits = 15;
		break;
	case RB_GET_PACKED565:
		usImageBits = 16;
		break;
	case RB_GET_RGB_M:
		if( Caps & RB_CAP_HASALPHA)
		{
			usImageBits = 32;
			GetFlags = RB_GET_RGBA;
		}
		else
		{
			usImageBits = 24;
			GetFlags = RB_GET_RGB;
		}
		break;
	case RB_GET_PACKED_M:
		usImageBits = 15;
		bPackedAlpha = (Caps & RB_CAP_HASALPHA) ? true : false;
		GetFlags = RB_GET_PACKED555;
		break;
	}

	usImageBPP = BitsToBytes( usImageBits);

	pNewImage->pImage = malloc( usWidth * usHeight * usImageBPP);

	if( pNewImage->pImage == NULL)
		Error = RERR_ALLOC;

	pNewImage->IFlags |= GetFlags & 0x03;

	switch( Header.ucImageType)
	{
	case TGA_IMG_UN_TRU:
	case TGA_IMG_RL_TRU:
		if( Header.ImageInfo.ucPixelDepth == 16)
			ConversionMatrix( pImage, pNewImage->pImage, 15, usImageBits, usWidth * usHeight, bPackedAlpha);
		else
			ConversionMatrix( pImage, pNewImage->pImage, Header.ImageInfo.ucPixelDepth, usImageBits, usWidth * usHeight, bPackedAlpha);

		bRetCode = true;
		break;
	case TGA_IMG_UN_CLR:
	case TGA_IMG_RL_CLR:
		ConvertColor( pNewImage->pImage, usImageBits);
		bRetCode = true;
		break;
	default:
		Error = RERR_NOTIMPLEMENTED;
		bRetCode = false;
	}

	if( bRetCode == false)
	{
		if( pNewImage->pImage)
			free( pNewImage->pImage);

		free( pNewImage);

		return NULL;
	}

	return pNewImage;
}
void ReadTGA::CheckFooter( void)
{
	if( strncmp( Footer.cSignature, TGA_SIGNATURE, strlen( TGA_SIGNATURE)) == 0)
		bNewTGA = true;
	else
		bNewTGA = false;

}

bool ReadTGA::GetHeader( void)
{
	pStream->read( (unsigned char *) &Header, sizeof( TGAHeader));

	return ( pStream->good( )) ? true : false;
}

bool ReadTGA::GetFooter( void)
{
streampos			current;

	current = pStream->tellg( );

	pStream->seekg( -26, ios::end);

	pStream->read( (unsigned char *) &Footer, sizeof( TGAFooter));

	pStream->seekg( current);

	return( pStream->good( )) ? true : false;
}

bool ReadTGA::ReadRGB( void)
{
	unsigned char				*pBuffer;
	unsigned char				*pLine, *pLineTemp;

	unsigned short				usRow, usCol;
	short							sPixelDelta;

	assert( pImage == NULL);

	if( AllocImage( ) == false)
		return false;

	pLine = (unsigned char *) malloc( usRowBytes);

	sPixelDelta = ( (Header.ImageInfo.ucDescriptor >> 4) & 0x01) ? -1 : 1;
	sPixelDelta *= usPixelBytes;

	for( usRow = 0; usRow < usHeight; usRow++)
	{
		FillLine( pLine);

		if( !pStream->good( ))
			break;

		pBuffer = GetScanStart( usRow);
		pLineTemp = pLine;

		switch( usPixelBytes)
		{
		case 3:
			for( usCol = 0; usCol < usWidth; usCol++)
			{
				pBuffer[0] = pLineTemp[2];
				pBuffer[1] = pLineTemp[1];
				pBuffer[2] = pLineTemp[0];
				pBuffer += sPixelDelta;
				pLineTemp += usPixelBytes;
			}
			break;
		case 4:
			for( usCol = 0; usCol < usWidth; usCol++)
			{
				pBuffer[0] = pLineTemp[2];
				pBuffer[1] = pLineTemp[1];
				pBuffer[2] = pLineTemp[0];
				pBuffer[3] = pLineTemp[3];
				pBuffer += sPixelDelta;
				pLineTemp += usPixelBytes;
			}
			break;
		default:
			for( usCol = 0; usCol < usWidth; usCol++)
			{
				memcpy( pBuffer, pLineTemp, usPixelBytes);
				pBuffer += sPixelDelta;
				pLineTemp += usPixelBytes;
			}
			break;
		}

	}

	free( pLine);

	return (pStream->good( )) ? true : false;
}


unsigned char *ReadTGA::GetScanStart( unsigned short usRow)
{

	switch( Header.ImageInfo.ucDescriptor >> 4)
	{
	case TGA_ORIENTATION_BL:
		return ((unsigned char *) pImage) + ((usHeight - (usRow + 1)) * usRowBytes);
		break;
	case TGA_ORIENTATION_BR:
		return ((unsigned char *) pImage) + ((usHeight - usRow) * usRowBytes) - usPixelBytes;
		break;
	default:
	case TGA_ORIENTATION_TL:
		return ((unsigned char *) pImage) + (usRow * usRowBytes);
		break;
	case TGA_ORIENTATION_TR:
		return ((unsigned char *) pImage) + ((usRow + 1) * usRowBytes) - usPixelBytes;
		break;
	}

}

void ReadTGA::FillLine( unsigned char *pLine)
{
	if( bCompressed == false)
		pStream->read( pLine, usRowBytes);
	else
		DecompBuffer( pLine, usRowBytes);
}

void ReadTGA::DecompBuffer( unsigned char *pBuffer, unsigned short usDecompBytes)
{
unsigned char			ucRLCode;
unsigned char			ucRLCount;
unsigned char			ucPixelBuffer[4];

unsigned short			us;

unsigned long			usProcessedBytes;

	usProcessedBytes = 0;

	do
	{
		pStream->read( &ucRLCode, 1);
		ucRLCount = (ucRLCode & 0x7f) + 1;

		if( ucRLCode & 0x80)
		{
			pStream->read( ucPixelBuffer, usPixelBytes);

			for( us = 0; us < ucRLCount; us++)
			{
				memcpy( pBuffer, ucPixelBuffer, usPixelBytes);
				pBuffer += usPixelBytes;
			}
		}
		else
		{
			pStream->read( pBuffer, ucRLCount * usPixelBytes);
			pBuffer += ((unsigned long) ucRLCount) * usPixelBytes;
		}

		usProcessedBytes += ((unsigned long) ucRLCount) * usPixelBytes;

	} while( usProcessedBytes < usDecompBytes  &&  pStream->good( ));

}

bool ReadTGA::AllocImage( )
{

	ulImageSize = (unsigned long) Header.ImageInfo.usWidth *
					  (unsigned long) Header.ImageInfo.usHeight *
					  (unsigned long) BitsToBytes( Header.ImageInfo.ucPixelDepth);

	pImage = malloc( ulImageSize);

	if( pImage == NULL)
	{
		Error = RERR_IMAGEMEM;
		return false;
	}

	return true;
}


void ReadTGA::ConvertColor( void *pOutImage, unsigned short usOutBits)
{
int					i;

void					*pTempPal;

unsigned char		*pIn;

	pTempPal = malloc( Header.CMAPInfo.usCMAPLength * BitsToBytes( usOutBits));

	ConversionMatrix( pCMAP, pTempPal, Header.CMAPInfo.ucCMAPEntrySize, usOutBits, Header.CMAPInfo.usCMAPLength, false);

	pIn = (unsigned char *) pImage;

	switch( usOutBits)
	{
	case 15:
	case 16:
		{
		unsigned short *pPalPacked = (unsigned short *) pTempPal;
		unsigned short *pOutPacked = (unsigned short *) pOutImage;
		for( i = 0; i < usWidth * usHeight; i++)
			*pOutPacked++ = pPalPacked[*pIn++];
		}
		break;
	case 24:
		{
		ts_bRGB			*pPalRGB = (ts_bRGB *) pTempPal;
		ts_bRGB			*pOutRGB = (ts_bRGB *) pOutImage;
		for( i = 0; i < usWidth * usHeight; i++)
			*pOutRGB++ = pPalRGB[*pIn++];
		}
		break;
	case 32:
		{
		ts_bRGBA			*pPalRGBA = (ts_bRGBA *) pTempPal;
		ts_bRGBA			*pOutRGBA = (ts_bRGBA *) pOutImage;
		for( i = 0; i < usWidth * usHeight; i++)
			*pOutRGBA++ = pPalRGBA[*pIn++];
		}
		break;
	}

	free( pTempPal);

}


