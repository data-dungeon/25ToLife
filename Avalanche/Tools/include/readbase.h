#ifndef __READBASE_H
#define __READBASE_H

#include <istream.h>
#include <t_types.h>
#include <t_struct.h>

#include <str_wrap.h>

enum
{
	RERR_NONE = 0,
	RERR_STREAM,
	RERR_UNSUPPORTED,
	RERR_ALLOC,
	RERR_BADIMAGE,
	RERR_IMAGEMEM,
	RERR_NOTIMPLEMENTED
} ;

// Caps
#define RB_CAP_HASALPHA		0x00000001
#define RB_CAP_PACKED		0x00000002

// GetRGB flags
#define RB_GET_RGB			0x00000000
#define RB_GET_RGBA			0x00000001
#define RB_GET_PACKED555	0x00000002
#define RB_GET_PACKED565	0x00000003
#define RB_GET_RGB_M			0x00000004
#define RB_GET_PACKED_M		0x00000005
#define RB_GET_INTERNAL		0x00000007

#define RB_GET_MASK			0x00000007


class ReadBase : public StreamWrapper
{
	void				Init( void);

protected:
	bool					bPreped;
	bool					bRead;

	t_error 				Error;
	t_lflagbits			Caps;

	unsigned short		usWidth;
	unsigned short		usHeight;

	virtual void 	StreamChanged( void) { Init( ); }

public:
	ReadBase( );
	ReadBase( const char *pcName);
	ReadBase( istream &str);
	virtual ~ReadBase( ) { }

	static unsigned short	BitsToBytes( unsigned short);
	virtual bool 				Prep( void) = 0;
	virtual bool 				Read( void) = 0;

	virtual ts_RBImage *		GetImage( t_lflagbits GetFlags) = 0;

	unsigned short				Width( void) { return (bPreped == true) ? usWidth : 0; }
	unsigned short				Height( void) { return (bPreped == true) ? usHeight : 0; }
	virtual t_lflagbits		GetCaps( void) { return Caps; }

} ;

extern "C" void SwapRB_RGB( ts_bRGB *pIn, unsigned long ulPixels);
extern "C" void SwapRB_RGBA( ts_bRGBA *pIn, unsigned long ulPixels);
extern "C" void SwapRB_Packed555( unsigned short *pIn, unsigned long ulPixels);
extern "C" void SwapRB_Packed565( unsigned short *pIn, unsigned long ulPixels);

extern "C" void ConversionMatrix( void *pIn, void *pOut, unsigned short usInBits, unsigned short usOutBits, unsigned long ulPixels, bool b555AlphaFlag);

extern "C" void RGBtoRGBA( ts_bRGB *pIn, ts_bRGBA *pOut, unsigned long ulBytes);
extern "C" void RGBtoPacked555( ts_bRGB *pIn, unsigned short *pOut, unsigned long ulBytes);
extern "C" void RGBtoPacked565( ts_bRGB *pIn, unsigned short *pOut, unsigned long ulBytes);

extern "C" void RGBAtoRGB( ts_bRGBA *pIn, ts_bRGB *pOut, unsigned long ulBytes);
extern "C" void RGBAtoPacked555( ts_bRGBA *pIn, unsigned short *pOut, unsigned long ulBytes);
extern "C" void RGBAtoPacked565( ts_bRGBA *pIn, unsigned short *pOut, unsigned long ulBytes);

extern "C" void Packed555toRGB( unsigned short *pIn, ts_bRGB *pOut, unsigned long ulBytes);
extern "C" void Packed555toRGBA( unsigned short *pIn, ts_bRGBA *pOut, unsigned long ulBytes, bool bHasAlpha);
extern "C" void Packed555toPacked565( unsigned short *pIn, unsigned short *pOut, unsigned long ulBytes);

extern "C" void Packed565toRGB( unsigned short *pIn, ts_bRGB *pOut, unsigned long ulBytes);
extern "C" void Packed565toRGBA( unsigned short *pIn, ts_bRGBA *pOut, unsigned long ulBytes);
extern "C" void Packed565toPacked555( unsigned short *pIn, unsigned short *pOut, unsigned long ulBytes);


#endif //__READREAD_H
