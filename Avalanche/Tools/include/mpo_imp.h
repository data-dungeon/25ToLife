#ifndef __MPO_IMP_H
#define __MPO_IMP_H

#include "mpostr.h"


#undef CLASS_EXPORT

#if defined DLL_EXPORT
#define CLASS_EXPORT	_declspec(dllexport)
#elif defined DLL_IMPORT
#define CLASS_EXPORT __declspec(dllimport)
#else
#define CLASS_EXPORT
#endif

class CLASS_EXPORT PCStream : public LittleEndian
{

public:
	PCStream( std::ostream &str) : LittleEndian( str) { SetAlignment( 4); }
} ;

class CLASS_EXPORT N64Stream : public BigEndian
{
public:
	N64Stream( std::ostream &str) : BigEndian( str) { SetAlignment( 4); SetWriteFlag( MPO_WR_ALIGNBLOCKS); }
} ;

class CLASS_EXPORT PSXStream : public LittleEndian
{
	int				iPrec;
	float				fPrec;

public:
	PSXStream( std::ostream &str) : LittleEndian( str) { SetAlignment( 4); SetWriteFlag( MPO_WR_ALIGNBLOCKS); SetFPrec( 12); }
	void SetFPrec( int iNPrec) { iPrec = iNPrec;	fPrec = (float) ( 1 << iPrec); }

	mpostream & operator << ( float);
} ;

#endif
