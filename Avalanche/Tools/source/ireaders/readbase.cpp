
#include <assert.h>
#include <string.h>
#include <fstream.h>
#include <readbase.h>


ReadBase::ReadBase( const char *pcFName) : StreamWrapper( pcFName)
{

	Init( );
}

ReadBase::ReadBase( istream &str) : StreamWrapper( str)
{

	Init( );
}

ReadBase::ReadBase( ) : StreamWrapper( )
{
	Init( );
}

void ReadBase::Init( void)
{
	bPreped = false;
	bRead = false;

	Error = RERR_NONE;

	usWidth = 0;
	usHeight = 0;

	Caps = 0;
}

unsigned short ReadBase::Width( void)
{
	assert( bPreped == true);

	return usWidth;
}

unsigned short ReadBase::Height( void)
{
	assert( bPreped == true);

	return usHeight;
}

unsigned short ReadBase::BitsToBytes( unsigned short usBits)
{

	return (usBits + 7) / 8;
}

void SwapRB_RGB( ts_bRGB *pIn, unsigned long ulPixels)
{
t_Byte				Temp;


	while( ulPixels--)
	{
		Temp = pIn->B;
		pIn->B = pIn->R;
		pIn->R = Temp;

		pIn++;
	}

}

void SwapRB_RGBA( ts_bRGBA *pIn, unsigned long ulPixels)
{
t_Byte				Temp;

	while( ulPixels--)
	{
		Temp = pIn->B;
		pIn->B = pIn->R;
		pIn->R = Temp;

		pIn++;
	}

}

void SwapRB_Packed555( unsigned short *pIn, unsigned long ulPixels)
{
	while( ulPixels--)
	{
	*pIn = ((*pIn & 0x001f) << 10) | (*pIn & 0x83e0) | ((*pIn & 0x7c00) >> 10);
	pIn++;
	}
}

void SwapRB_Packed565( unsigned short *pIn, unsigned long ulPixels)
{
	while( ulPixels--)
	{
	*pIn = ((*pIn & 0x001f) << 11) | (*pIn & 0x07e0) | ((*pIn & 0xf800) >> 11);
	pIn++;
	}
}

void ConversionMatrix( void *pIn, void *pOut, unsigned short usInBits, unsigned short usOutBits, unsigned long ulPixels, bool b555AlphaFlag)
{

	switch( usInBits)
	{
	case 15:
		switch( usOutBits)
		{
		case 15:
			memcpy( pOut, pIn, ulPixels * ReadBase::BitsToBytes( 15));
			break;
		case 16:
			Packed555toPacked565( (unsigned short *) pIn, (unsigned short *) pOut, ulPixels);
			break;
		case 24:
			Packed555toRGB( (unsigned short *) pIn, (ts_bRGB *) pOut, ulPixels);
			break;
		case 32:
			Packed555toRGBA( (unsigned short *) pIn, (ts_bRGBA *) pOut, ulPixels, b555AlphaFlag);
			break;
		}
		break;
	case 16:
		switch( usOutBits)
		{
		case 15:
			Packed565toPacked555( (unsigned short *) pIn, (unsigned short *) pOut, ulPixels);
			break;
		case 16:
			memcpy( pOut, pIn, ulPixels * ReadBase::BitsToBytes( 16));
			break;
		case 24:
			Packed565toRGB( (unsigned short *) pIn, (ts_bRGB *) pOut, ulPixels);
			break;
		case 32:
			Packed565toRGBA( (unsigned short *) pIn, (ts_bRGBA *) pOut, ulPixels);
			break;
		}
		break;
	case 24:
		switch( usOutBits)
		{
		case 15:
			RGBtoPacked555( (ts_bRGB *) pIn, (unsigned short *) pOut, ulPixels);
			break;
		case 16:
			RGBtoPacked565( (ts_bRGB *) pIn, (unsigned short *) pOut, ulPixels);
			break;
		case 24:
			memcpy( pOut, pIn, ulPixels * ReadBase::BitsToBytes( 24));
			break;
		case 32:
			RGBtoRGBA( (ts_bRGB *) pIn, (ts_bRGBA *) pOut, ulPixels);
			break;
		}
		break;
	case 32:
		switch( usOutBits)
		{
		case 15:
			RGBAtoPacked555( (ts_bRGBA *) pIn, (unsigned short *) pOut, ulPixels);
			break;
		case 16:
			RGBAtoPacked565( (ts_bRGBA *) pIn, (unsigned short *) pOut, ulPixels);
			break;
		case 24:
			RGBAtoRGB( (ts_bRGBA *) pIn, (ts_bRGB *) pOut, ulPixels);
			break;
		case 32:
			memcpy( pOut, pIn, ulPixels * ReadBase::BitsToBytes( 32));
			break;
		}
		break;
	}
}

void RGBtoRGBA( ts_bRGB *pIn, ts_bRGBA *pOut, unsigned long ulPixels)
{
	while( ulPixels--)
	{
		*((ts_bRGB *) pOut) = *pIn++;
		pOut->A = 0;
		pOut++;
	}
}

void RGBAtoRGB( ts_bRGBA *pIn, ts_bRGB *pOut, unsigned long ulPixels)
{
	while( ulPixels--)
	{
		*pOut++ = *((ts_bRGB *) pIn);
		pIn++;
	}
}

void Packed555toRGB( unsigned short *pIn, ts_bRGB *pOut, unsigned long ulPixels)
{
unsigned short		usPackedRGB;

	while( ulPixels--)
	{
		usPackedRGB = *pIn++;

		pOut->R = ((usPackedRGB >> 10) & 0x1f) << 3;
		pOut->G = ((usPackedRGB >>  5) & 0x1f) << 3;
		pOut->B = ((usPackedRGB      ) & 0x1f) << 3;

		pOut++;
	}

}

void Packed555toRGBA( unsigned short *pIn, ts_bRGBA *pOut, unsigned long ulPixels, bool bHasAlpha)
{
unsigned short		usPackedRGB;

	if( bHasAlpha == false)
	{
		while( ulPixels--)
		{
			usPackedRGB = *pIn++;

			pOut->R = ((usPackedRGB >> 10) & 0x1f) << 3;
			pOut->G = ((usPackedRGB >>  5) & 0x1f) << 3;
			pOut->B = ((usPackedRGB      ) & 0x1f) << 3;
			pOut->A = 255;
			pOut++;
		}
	}
	else
	{
		while( ulPixels--)
		{
			usPackedRGB = *pIn++;

			pOut->R = ((usPackedRGB >> 10) & 0x1f) << 3;
			pOut->G = ((usPackedRGB >>  5) & 0x1f) << 3;
			pOut->B = ((usPackedRGB      ) & 0x1f) << 3;

			pOut->A = (usPackedRGB & 0x8000) >> 12;
			pOut++;
		}
	}

}

void Packed555toPacked565( unsigned short *pIn, unsigned short *pOut, unsigned long ulPixels)
{

	while( ulPixels--)
	{
		*pOut = ((*pIn & 0x7c00) << 1) | ((*pIn & 0x03e0) << 1) | (*pIn & 0x001f);
		pOut++;
		pIn++;
	}

}

void Packed565toRGB( unsigned short *pIn, ts_bRGB *pOut, unsigned long ulPixels)
{
unsigned short		usPackedRGB;

	while( ulPixels--)
	{
		usPackedRGB = *pIn++;

		pOut->R = ((usPackedRGB >> 11) & 0x1f) << 3;
		pOut->G = ((usPackedRGB >>  5) & 0x3f) << 2;
		pOut->B = ((usPackedRGB      ) & 0x1f) << 3;

		pOut++;
	}
}


void Packed565toRGBA( unsigned short *pIn, ts_bRGBA *pOut, unsigned long ulPixels)
{
unsigned short		usPackedRGB;

	while( ulPixels--)
	{
		usPackedRGB = *pIn++;

		pOut->R = ((usPackedRGB >> 11) & 0x1f) << 3;
		pOut->G = ((usPackedRGB >>  5) & 0x3f) << 2;
		pOut->B = ((usPackedRGB      ) & 0x1f) << 3;
		pOut->A = 255;
		pOut++;
	}

}

void Packed565toPacked555( unsigned short *pIn, unsigned short *pOut, unsigned long ulPixels)
{

	while( ulPixels--)
	{
		*pOut = ((*pIn >> 1) & 0x7c00) | (( *pIn >> 1) & 0x03e0) | (*pIn & 0x001f);
		pOut++;
		pIn++;
	}

}

void RGBtoPacked555( ts_bRGB *pIn, unsigned short *pOut, unsigned long ulPixels)
{
unsigned short			usPackedRGB;

	while( ulPixels--)
	{
		usPackedRGB  = ((pIn->R >> 3) & 0x1f) << 10;
		usPackedRGB |= ((pIn->G >> 3) & 0x1f) <<  5;
		usPackedRGB |= ((pIn->B >> 3) & 0x1f);

		*pOut++ = usPackedRGB;
		pIn++;
	}

}

void RGBAtoPacked555( ts_bRGBA *pIn, unsigned short *pOut, unsigned long ulPixels)
{
unsigned short			usPackedRGB;

	while( ulPixels--)
	{
		usPackedRGB  = ((pIn->R >> 3) & 0x1f) << 10;
		usPackedRGB |= ((pIn->G >> 3) & 0x1f) <<  5;
		usPackedRGB |= ((pIn->B >> 3) & 0x1f);

		*pOut++ = usPackedRGB;
		pIn++;
	}

}

void RGBtoPacked565( ts_bRGB *pIn, unsigned short *pOut, unsigned long ulPixels)
{
unsigned short			usPackedRGB;

	while( ulPixels--)
	{
		usPackedRGB  = ((pIn->R >> 3) & 0x1f) << 11;
		usPackedRGB |= ((pIn->G >> 2) & 0x3f) <<  5;
		usPackedRGB |= ((pIn->B >> 3) & 0x1f);

		*pOut++ = usPackedRGB;
		pIn++;
	}

}

void RGBAtoPacked565( ts_bRGBA *pIn, unsigned short *pOut, unsigned long ulPixels)
{
unsigned short			usPackedRGB;

	while( ulPixels--)
	{
		usPackedRGB  = ((pIn->R >> 3) & 0x1f) << 11;
		usPackedRGB |= ((pIn->G >> 2) & 0x3f) <<  5;
		usPackedRGB |= ((pIn->B >> 3) & 0x1f);

		*pOut++ = usPackedRGB;
		pIn++;
	}

}

