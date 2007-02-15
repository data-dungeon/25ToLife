#ifndef __TEXWRITE_H
#define __TEXWRITE_H

#include <t_types.h>

#undef CLASS_EXPORT

#if defined DLL_EXPORT
#define CLASS_EXPORT	_declspec(dllexport)
#elif defined DLL_IMPORT
#define CLASS_EXPORT __declspec(dllimport)
#else
#define CLASS_EXPORT
#endif

#define TEX_FMT_RGB			0x0000
#define TEX_FMT_RGBA			0x0001

class CLASS_EXPORT TexWrite
{
public:
	virtual bool AddTexture( t_lflagbits Flags, unsigned short usWidth, unsigned short usHeight, void *pData) = 0;
	virtual bool Write( ) = 0;

};
#undef CLASS_EXPORT

#endif
