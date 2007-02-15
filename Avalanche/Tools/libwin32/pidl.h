//=============================================================================//
//  Copyright ©2003 Avalanche Software.  All Rights Reserved.                  //
//=============================================================================//
//      File: pidl.h                                                         //
//    Author: Daniel Silver                                                    //
//   Created: 10/15/2003                                                       //
//=============================================================================//

#ifndef _PIDL_H_
#define _PIDL_H_

#include "pch.h"
#include "pidl.h"

LPITEMIDLIST GetNextPidl( LPCITEMIDLIST pidl );
int SizeofPidl( LPCITEMIDLIST pidl );
LPITEMIDLIST AllocPidl( int Size );
LPITEMIDLIST DeletePidl( LPITEMIDLIST pidl );
LPITEMIDLIST DuplicatePidl( LPITEMIDLIST pidl );
LPITEMIDLIST ConcatPidls( LPCITEMIDLIST a, LPCITEMIDLIST b );
LPITEMIDLIST FileNameToPidl( LPCTSTR pFileName );

#define PIDL_NOMATCH			(0)
#define PIDL_PARTIALMATCH	(1)
#define PIDL_EXACTMATCH		(2)

int ComparePidls( LPCITEMIDLIST a, LPCITEMIDLIST b );


#endif // _PIDLE_H_