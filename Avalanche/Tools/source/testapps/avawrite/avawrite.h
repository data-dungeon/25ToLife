#ifndef __AVAWRITE_H
#define __AVAWRITE_H

#include <t_types.h>

#include <dbwriter.h>
#include <hashproc.h>

enum
{
	AVA_FMT_PC = 0,
	AVA_FMT_N64,
	AVA_FMT_PSX
} ;

#define AVA_WR_TEXTURES				0x01
#define AVA_WR_POLYS					0x02


#define AVA_TEX_PACKHDR				0x01


class AVAWrite
{
	HashProc			*pAVA;
	DBWriter			*pDB;

	bool				WriteTextures( t_lflagbits Flags);
public:
	AVAWrite( void);
	~AVAWrite( void);

	bool				AttachInput( const char *pcIn);
	bool				AttachOutput( const char *pcOut, unsigned char ucFmt);

	bool				IsOpen( void);
	bool				Write( t_lflagbits WriteFlags, t_lflagbits TexFlags);


} ;


#endif