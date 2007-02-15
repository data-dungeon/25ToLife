#ifndef __FVF_VERTEX_H
#define __FVF_VERTEX_H

#include "XPRBundler.h"

class FVFVertex
{
	unsigned int	d_numBytes;

	char *			p_FVFData;

public:

	FVFVertex( unsigned int numBytes, void * pFVFData = NULL)
	{
		assert( numBytes );

		p_FVFData = new char[numBytes];

		d_numBytes = numBytes;

		if( pFVFData)
			memcpy( p_FVFData, pFVFData, d_numBytes );
		else
			memset( p_FVFData, 0, d_numBytes );
	}

	FVFVertex( const FVFVertex & Other)
	{
		assert( Other.d_numBytes);

		d_numBytes = Other.d_numBytes;

		p_FVFData = new char[d_numBytes];

		assert( p_FVFData);

		memcpy( p_FVFData, Other.p_FVFData, d_numBytes);
	}

	FVFVertex & 		operator= ( const FVFVertex & Other )
	{
		assert( Other.d_numBytes);

		if( p_FVFData)
			delete [] p_FVFData;

		d_numBytes = Other.d_numBytes;

		p_FVFData = new char[d_numBytes];

		assert( p_FVFData);

		memcpy( p_FVFData, Other.p_FVFData, d_numBytes);

		return * this;
	}

	bool					operator< ( const FVFVertex & Other ) const
	{
		assert( Other.d_numBytes == d_numBytes );

		return memcmp( p_FVFData, Other.p_FVFData, d_numBytes) < 0 ? true : false;
	}

	~FVFVertex( )
	{
		if( p_FVFData)
			delete [] p_FVFData;
	}

	unsigned int		bytes( void ) const	{ return d_numBytes; }

	operator				void *	( void ) const				{ return p_FVFData; }

	bool					operator== ( const FVFVertex & Other) const
	{
		assert( d_numBytes == Other.d_numBytes);

		return memcmp( p_FVFData, Other.p_FVFData, d_numBytes) == 0 ? true : false;
	}

} ;

class FileIOHelper
{
	FILE *						p_File;

public:

	FileIOHelper(
		FILE *				_File
	) 
	{
		assert( _File);
		p_File = _File;
	}

	void 						operator() ( const FVFVertex & vtx)
	{
		fwrite( vtx, vtx.bytes( ), 1, p_File );
	}

} ;

class XPRIOHelper
{
	XPRBundler & Bundler;

public:

	XPRIOHelper(
		XPRBundler & _Bundler
	) :
		Bundler( _Bundler )
	{
	}

	void 						operator() ( const FVFVertex & vtx)
	{
		Bundler.writeData( vtx, vtx.bytes( ) );
	}

} ;

#endif