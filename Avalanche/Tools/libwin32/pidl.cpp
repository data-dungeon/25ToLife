//=============================================================================//
//  Copyright ©2003 Avalanche Software.  All Rights Reserved.                  //
//=============================================================================//
//      File: pidl.cpp                                                         //
//    Author: Daniel Silver                                                    //
//   Created: 10/15/2003                                                       //
//=============================================================================//

#include "pch.h"
#include "pidl.h"

#include <atlconv.h>

LPITEMIDLIST GetNextPidl( LPCITEMIDLIST pidl )
{ 
	LPITEMIDLIST nextPidl = 0;

	if( pidl )
	{
		// Get the size of the specified item identifier. 
		int cb = pidl->mkid.cb; 

		if( cb > 0 )
		{
			// Add cb to pidl (casting to increment by bytes). 
			nextPidl = (LPITEMIDLIST) (((LPBYTE) pidl) + cb);

			if( nextPidl->mkid.cb == 0 )
				nextPidl = 0;
		}
	}

	return nextPidl; 
}

int SizeofPidl( LPCITEMIDLIST pidl )
{
	int nSize = 0;

	if( pidl )
	{
		nSize += sizeof(pidl->mkid.cb);

		while( pidl )
		{
			nSize += pidl->mkid.cb;
			pidl = GetNextPidl( pidl );
		}
	}

	return nSize;
}

LPITEMIDLIST AllocPidl( int Size )
{
	LPITEMIDLIST newPidl = 0;
	LPMALLOC lpm;
	HRESULT hresult;

	hresult = SHGetMalloc( &lpm );

	if( !FAILED(hresult) )
		newPidl = (LPITEMIDLIST) lpm->Alloc( Size );

	return newPidl;
}

LPITEMIDLIST DeletePidl( LPITEMIDLIST pidl )
{
	LPMALLOC lpm;
	HRESULT hresult;

	hresult = SHGetMalloc( &lpm );

	if( FAILED(hresult) )
		return pidl;

	lpm->Free( pidl );

	return 0;
}

LPITEMIDLIST DuplicatePidl( LPCITEMIDLIST pidl )
{
	int Size = SizeofPidl( pidl );
	LPITEMIDLIST newPidl = AllocPidl( Size );

	if( newPidl )
		CopyMemory( newPidl, pidl, Size );

	return newPidl;
}

LPITEMIDLIST ConcatPidls( LPCITEMIDLIST a, LPCITEMIDLIST b )
{
	LPITEMIDLIST newPidl = 0;
	int size1 = 0;
	int size2 = 0;
	int size3 = 0;

	if( a && !b )
		return ConcatPidls( b, a );

	if( a )
		size1 = SizeofPidl( a ) - sizeof(a->mkid.cb);

	if( b )
		size2 = SizeofPidl( b );

	size3 = size1 + size2;

	newPidl = AllocPidl( size3 );

	if( newPidl )
	{
		if( a )
			memcpy( newPidl, a, size1 );

		if( b )
			memcpy( (BYTE *) newPidl + size1, b, size2 );
	}

	return newPidl;
}

LPITEMIDLIST FileNameToPidl( LPCTSTR pFileName )
{
	IShellFolder *pShellFolder;
	SHGetDesktopFolder( &pShellFolder );

	USES_CONVERSION;
	LPOLESTR lpsz = T2OLE(pFileName);

	LPITEMIDLIST Pidl;
	pShellFolder->ParseDisplayName( 0, 0, lpsz, 0, &Pidl, 0 );
		
	return Pidl;
}

int ComparePidls( LPCITEMIDLIST a, LPCITEMIDLIST b )
{
	if( a == b )
		return PIDL_EXACTMATCH;

	if( a == 0 || b == 0 )
		return PIDL_NOMATCH;

	if( a->mkid.cb != b->mkid.cb )
		return PIDL_NOMATCH;

	int cb = a->mkid.cb-sizeof(a->mkid.cb);
	BYTE x, y;

	for( int i = 0; i < cb; i++ )
	{
		x = a->mkid.abID[i];
		y = b->mkid.abID[i];

		if( x != y )
			return PIDL_NOMATCH;
	}

	// at this point we have an exact match, but there may be more pidls to compare

	a = GetNextPidl( a );
	b = GetNextPidl( b );

	// if there are no more pidls, then we have an exact match
	if( a == 0 && b == 0 )
		return PIDL_EXACTMATCH;

	// if one of the pidls is terminated, but the other hasn't, we have a partial match
	if( a != 0 && b == 0 )
		return PIDL_PARTIALMATCH;

	// if one of the pidls is terminated, but the other hasn't, we have a partial match
	if( a == 0 && b != 0 )
		return PIDL_PARTIALMATCH;
	
	// compare the next two pidls and return the result.  it could be no match, a partial match or an exact match
	return ComparePidls( a, b );
}