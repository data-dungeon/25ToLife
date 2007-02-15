#ifndef __TEXTDB_H
#define __TEXTDB_H

#include <texwrite.h>
#include <dbwriter.h>


#undef CLASS_EXPORT

#if defined DLL_EXPORT
#define CLASS_EXPORT	_declspec(dllexport)
#elif defined DLL_IMPORT
#define CLASS_EXPORT __declspec(dllimport)
#else
#define CLASS_EXPORT
#endif

#define TEX_DB_TYPE			0x010

#define TEX_DB_VERSION		0x0100

#define TEX_HDR_PACK			0x0001


typedef struct IntText
{
	struct IntText		*pNext;
	void					*pData;
	unsigned short		usWidth;
	unsigned short		usHeight;
	unsigned char		ucFlags;
} ts_IntText;



class CLASS_EXPORT TextureDB : public TexWrite
{

	DBWriter				*pMyDB;
	mpostream			*pStream;

	ts_IntText			*pTextList;
	unsigned long		ulTextCount;

	t_lflagbits			DBFlags;

	void ReleaseList( void);
	void WriteHdr( void);
	void WriteTexture( ts_IntText *pText);
	void WriteTextHdrPack( ts_IntText *pText);
	void WriteTextHdrNorm( ts_IntText *pText);
	void WriteTextData( ts_IntText *pText);

public:
	TextureDB( DBWriter *pNewDB);
	~TextureDB( );
	void SetDBFormat( t_lflagbits);

	bool AddTexture( t_lflagbits Flags, unsigned short usWidth, unsigned short usHeight, void *pData);

	bool Write( void) { return Write( NULL); }
	bool Write( char *pName = NULL);

} ;


#endif
