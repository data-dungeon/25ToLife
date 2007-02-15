#ifndef __READ_TGA_H
#define __READ_TGA_H

#include <readbase.h>

#define TGA_IMG_NONE				0
#define TGA_IMG_UN_CLR			1
#define TGA_IMG_UN_TRU			2
#define TGA_IMG_UN_BLW			3
#define TGA_IMG_RL_CLR			9
#define TGA_IMG_RL_TRU			10
#define TGA_IMG_RL_BLW			11

enum
{
	TGA_ORIENTATION_BL = 0,
	TGA_ORIENTATION_BR,
	TGA_ORIENTATION_TL,
	TGA_ORIENTATION_TR
} ;

enum
{
	RERR_TGA_NOIMAGE = 0x00010000,
} ;

#pragma pack( 1)
typedef struct _CMAPSpec
{
	unsigned short		usFirstEntryIdx;
	unsigned short		usCMAPLength;
	unsigned char		ucCMAPEntrySize;
} CMAPSpec;

typedef struct _ImageSpec
{
	unsigned short		usOriginX;
	unsigned short		usOriginY;
	unsigned short		usWidth;
	unsigned short		usHeight;
	unsigned char		ucPixelDepth;
	unsigned char		ucDescriptor;
} ImageSpec;

typedef struct _TGAFooter
{
	unsigned long		ExtensionOffset;
	unsigned long		DeveloperDirOffset;
	char					cSignature[18];
} TGAFooter;


typedef struct _TGAHeader
{
	unsigned char		ucIDLength;
	unsigned char		ucColorMapType;
	unsigned char		ucImageType;
	CMAPSpec				CMAPInfo;
	ImageSpec			ImageInfo;
} TGAHeader;


typedef struct _TGAExtensions
{
	unsigned short		usSize;
	char					cAuthorName[41];
	char					cAuthorComment[324];
	short					Date[3];
	short					Time[3];
	char					cJobName[41];
	short					JobTime[3];
	char					cSoftwareID[41];
	char					cSoftwareVer[3];
	long					lKeyColor;
	short					sAspectRatio[2];
	short					sGammaValue[2];
	unsigned long		ulColorCorrectOffset;
	unsigned long		ulPostageStampOffset;
	unsigned long		ulScanLineOffset;
	char					cAttributeType;
} TGAExtension;




#pragma pack( )

class ReadTGA : public ReadBase
{
	bool		      bNewTGA;
	bool				bCompressed;

	char				ImageID[255];

	unsigned short	usPixelBytes;
	unsigned short	usRowBytes;

	unsigned long	ulImageSize;

	void				*pCMAP;
	void				*pImage;

	TGAFooter		Footer;
	TGAHeader		Header;

	void				TGAInit( void);
	void				FreeMyMem( void);

	bool				GetFooter( void);
	bool				GetHeader( void);
	void				CheckFooter( void);

	bool				AllocImage( void);

	bool				ReadRGB( void);
	void				FillLine( unsigned char *pLine);

	unsigned char *GetScanStart( unsigned short usRow);
	void				DecompBuffer( unsigned char *pBuffer, unsigned short usBytes);

	void				ConvertColor( void *pImage, unsigned short usOutDepth);
	void 				StreamChanged( void);

public:
	ReadTGA( );
	ReadTGA( const char *pcName);
	ReadTGA( istream &str);
	~ReadTGA( );

	ts_RBImage *	GetImage( t_lflagbits GetFlags);
	bool 				Prep( void);
	bool 				Read( void);
} ;

#endif //__READ_TGA_H
