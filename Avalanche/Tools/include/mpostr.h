#ifndef __MPOSTR_H
#define __MPOSTR_H

/* -----------------7/14/98 4:01PM-------------------
 * mpostream ( MultiPlatform Output STREAM).
 * --------------------------------------------------*/

/* -----------------7/14/98 4:01PM-------------------
 * Includes
 * --------------------------------------------------*/
#include <fstream>
#include <ostream>
#include <t_types.h>

#undef CLASS_EXPORT

#if defined DLL_EXPORT
#define CLASS_EXPORT	_declspec(dllexport)
#elif defined DLL_IMPORT
#define CLASS_EXPORT __declspec(dllimport)
#else
#define CLASS_EXPORT
#endif

/* -----------------7/14/98 4:01PM-------------------
 * Exported Defines
 * --------------------------------------------------*/
#define MPO_WR_ALIGNBLOCKS	0x000000001


/* -----------------7/14/98 4:01PM-------------------
 * Class Definitions
 * --------------------------------------------------*/
class CLASS_EXPORT mpostream
{
protected:
	std::ostream		*pMyStream;
	unsigned short		usAlignment;
	unsigned long		ulAlignOffset;
	t_lflagbits			WriteFlags;

	void					setpos( void);
	void					ByteSwap( short *);
	void					ByteSwap( unsigned short *);
	void					ByteSwap( int *);
	void					ByteSwap( unsigned int *);
	void					ByteSwap( long *);
	void					ByteSwap( unsigned long *);

	void					WordSwap( int *);

	

public:
	mpostream( std::ostream &str);
	void 					SetWriteFlag( t_lflagbits Flag);
	void 					ClearWriteFlag( t_lflagbits Flag);
	bool					IsFlagSet(t_lflagbits Flag);
	void 					Align( void);
	void 					CheckAlignment( unsigned short usSize);
	void 					SetAlignment( unsigned short usAlign) { usAlignment = usAlign; }
	void 					ResetAlignOffset( void) { ulAlignOffset = 0; }
	virtual void 		WriteBYTE( void *pb);
	virtual void 		WriteWORD( void *pw);
	virtual void 		WriteDWORD( void *pd);

	virtual void 		OutBYTE( void *pb) = 0;
	virtual void 		OutWORD( void *pw) = 0;
	virtual void 		OutDWORD( void *pd) = 0;
	virtual void 		Write( unsigned char *pData, int iCount);
	virtual void 		Write( unsigned short *pData, int iCount);
	virtual void 		Write( unsigned int *pData, int iCount);
	virtual void 		Write( float *pData, int iCount);
	virtual void 		Write( unsigned long *pData, int iCount);

	virtual mpostream & operator << ( char c);
	virtual mpostream & operator << ( unsigned char uc);
	virtual mpostream & operator << ( short s);
	virtual mpostream & operator << ( unsigned short us);
	virtual mpostream & operator << ( long l);
	virtual mpostream & operator << ( unsigned long ul);
	virtual mpostream & operator << ( int i);
	virtual mpostream & operator << ( unsigned int i);
	virtual mpostream & operator << ( float f);
	std::streampos			pos( void) { return pMyStream->tellp( ); }
	std::streampos			tellp( void) { return pMyStream->tellp( ); }
	void					seekp( std::streampos pos);
	void 					seekp( std::streamoff off, std::ios::seekdir dir);

	std::ostream &			stream( void) { return *pMyStream; }
} ;

class CLASS_EXPORT BigEndian : public mpostream
{

public:
	BigEndian( std::ostream &str) : mpostream( str) { }
	void OutBYTE( void *pData);
	void OutWORD( void *pData);
	void OutDWORD( void *pData);

} ;

class CLASS_EXPORT LittleEndian : public mpostream
{

public:
	LittleEndian( std::ostream &str) : mpostream( str) { }
	void OutBYTE( void *pData);
	void OutWORD( void *pData);
	void OutDWORD( void *pData);

} ;

#endif
