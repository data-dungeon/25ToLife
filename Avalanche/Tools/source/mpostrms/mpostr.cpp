/* -----------------7/14/98 3:33PM-------------------
 * Includes
 * --------------------------------------------------*/
#include <mpostr.h>
#include <iosfwd>

/* -----------------7/14/98 3:33PM-------------------
 * Local Defines
 * --------------------------------------------------*/

/* -----------------7/14/98 3:33PM-------------------
 * Local Prototypes
 * --------------------------------------------------*/
void ByteSwap( short *pVar);
void ByteSwap( unsigned short *pVar);
void ByteSwap( int *pVar);
void ByteSwap( unsigned int *pVar);
void ByteSwap( long *pVar);
void ByteSwap( unsigned long *pVar);

/* -----------------7/14/98 12:50PM------------------
 * MPOStream Class Code
 * --------------------------------------------------*/
mpostream::mpostream( std::ostream &str)
{
	pMyStream = &str;

	usAlignment = 1;

	WriteFlags = 0;

	setpos( );

}

void mpostream::setpos( void)
{
long					lPos;

	lPos = pMyStream->tellp( );
	ulAlignOffset = ( lPos < 0) ? 0 : (unsigned long) lPos;

}

void mpostream::SetWriteFlag( t_lflagbits Flag)
{

	FLAG_SET( WriteFlags, Flag);
}

void mpostream::ClearWriteFlag( t_lflagbits Flag)
{
	FLAG_UNSET( WriteFlags, Flag);
}

bool mpostream::IsFlagSet(t_lflagbits Flag)
{
	return((WriteFlags & Flag) != 0);
}

void mpostream::Align( void)
{
unsigned short		usMove;

	if( usAlignment > 1)
	{
		usMove = ((unsigned short) ulAlignOffset % usAlignment);

		if( usMove)
		{
			usMove = usAlignment - usMove;
			ulAlignOffset += usMove;

			while( usMove--)
				*pMyStream << (char) 0;
		}
	}
}

void mpostream::CheckAlignment( unsigned short usSize)
{
	if( (ulAlignOffset + usSize) % usAlignment)
		Align( );
}

void mpostream::seekp( std::streampos pos)
{
	pMyStream->seekp( pos);
	setpos( );
}

void mpostream::seekp( std::streamoff off, std::ios::seekdir dir)
{
	pMyStream->seekp( off, dir);

	setpos( );
}

/* -----------------7/14/98 3:54PM-------------------
 * Overloaded Operator <<
 * --------------------------------------------------*/
mpostream & mpostream::operator << (char c)
{
	WriteBYTE( &c);

	return *this;
}

mpostream & mpostream::operator << (unsigned char uc)
{
	WriteBYTE( &uc);

	return *this;
}

mpostream & mpostream::operator << (short s)
{
	WriteWORD( &s);

	return *this;
}

mpostream & mpostream::operator << (unsigned short us)
{
	WriteWORD( &us);

	return *this;
}

mpostream & mpostream::operator << (long l)
{
	WriteDWORD( &l);

	return *this;
}

mpostream & mpostream::operator << (unsigned long ul)
{
	WriteDWORD( &ul);

	return *this;
}

mpostream & mpostream::operator << ( int i)
{
	WriteDWORD( &i);

	return *this;
}

mpostream & mpostream::operator << ( unsigned int ui)
{
	WriteDWORD( &ui);

	return *this;
}

mpostream & mpostream::operator << ( float f)
{
	WriteDWORD( &f);

	return *this;
}

void mpostream::WriteBYTE( void *pb)
{
	OutBYTE( pb);
}

void mpostream::WriteWORD( void *pw)
{

	CheckAlignment( 2);
	OutWORD( pw);
}

void mpostream::WriteDWORD( void *pd)
{
	CheckAlignment( 4);
	OutDWORD( pd);
}

/* -----------------7/14/98 3:54PM-------------------
 * Block Write Routines
 * --------------------------------------------------*/
void mpostream::Write( unsigned char *pData, int iCount)
{
	int					i;
	
	if( WriteFlags & MPO_WR_ALIGNBLOCKS)
		Align( );

	for( i = 0; i < iCount; i++)
		OutBYTE( pData + i);

}

void mpostream::Write( unsigned short *pData, int iCount)
{
	int					i;

	if( WriteFlags & MPO_WR_ALIGNBLOCKS)
		Align( );

	for( i = 0; i < iCount; i++)
		OutWORD( pData + i);
}

void mpostream::Write( unsigned int *pData, int iCount)
{
	int					i;

	if( WriteFlags & MPO_WR_ALIGNBLOCKS)
		Align( );

	for( i = 0; i < iCount; i++)
		OutDWORD( pData + i);

}

void mpostream::Write( float *pData, int iCount)
{
	int					i;

	if( WriteFlags & MPO_WR_ALIGNBLOCKS)
		Align( );

	for( i = 0; i < iCount; i++)
		OutDWORD( pData + i);

}

void mpostream::Write( unsigned long *pData, int iCount)
{
	int					i;

	if( WriteFlags & MPO_WR_ALIGNBLOCKS)
		Align( );

	for( i = 0; i < iCount; i++)
		WriteDWORD( pData + i);
}

/* -----------------7/14/98 12:49PM------------------
 * Big Endian Class Code
 * --------------------------------------------------*/
void BigEndian::OutBYTE( void *pData)
{
	pMyStream->write( (const char *) pData, 1);
	ulAlignOffset++;
}

void BigEndian::OutWORD( void *pData)
{
	ByteSwap( (unsigned short *) pData);
	pMyStream->write( (const char *) pData, 2);
	ulAlignOffset += 2;
}

void BigEndian::OutDWORD( void *pData)
{
	ByteSwap( (unsigned long *) pData);
	pMyStream->write( (const char *) pData, 4);
	ulAlignOffset += 4;
}

/* -----------------7/14/98 12:51PM------------------
 * Little Endian Class Code
 * --------------------------------------------------*/
void LittleEndian::OutBYTE( void *pData)
{
	pMyStream->write( (const char *) pData, 1);

	ulAlignOffset++;
}

void LittleEndian::OutWORD( void *pData)
{
	pMyStream->write( (const char *) pData, 2);

	ulAlignOffset += 2;
}

void LittleEndian::OutDWORD( void *pData)
{
	pMyStream->write( (const char *) pData, 4);

	ulAlignOffset += 4;
}


/* -----------------7/14/98 3:30PM-------------------
 * Support
 * --------------------------------------------------*/
/* -----------------7/14/98 3:30PM-------------------
 * 16 bit
 * --------------------------------------------------*/

void mpostream::ByteSwap( short *pVar)
{
unsigned char		temp;
unsigned char		*puc;

	puc = (unsigned char *) pVar;
	temp = puc[0];
	puc[0] = puc[1];
	puc[1] = temp;
}

void mpostream::ByteSwap( unsigned short *pVar)
{
unsigned char		temp;
unsigned char		*puc;

	puc = (unsigned char *) pVar;
	temp = puc[0];
	puc[0] = puc[1];
	puc[1] = temp;
}

/* -----------------5/11/98 3:15PM-------------------
 * 32 bit
 * --------------------------------------------------*/
void mpostream::WordSwap( int *pVar)
{
unsigned short		temp;
unsigned short		*pus;

	pus = (unsigned short *) pVar;
	temp = pus[0];
	pus[0] = pus[1];
	pus[1] = temp;
}

void mpostream::ByteSwap( unsigned int *pVar)
{

	ByteSwap( (unsigned short *) pVar);
	ByteSwap( ((unsigned short *) pVar) + 1);

	WordSwap( (int *) pVar);
}

void mpostream::ByteSwap( int *pVar)
{

	ByteSwap( (short *) pVar);
	ByteSwap( ((short *) pVar) + 1);

	WordSwap( pVar);
}

void mpostream::ByteSwap( unsigned long *pVar)
{

	ByteSwap( (unsigned short *) pVar);
	ByteSwap( ((unsigned short *) pVar) + 1);

	WordSwap( (int *) pVar);
}

void mpostream::ByteSwap( long *pVar)
{

	ByteSwap( (short *) pVar);
	ByteSwap( ((short *) pVar) + 1);

	WordSwap( (int *) pVar);
}
