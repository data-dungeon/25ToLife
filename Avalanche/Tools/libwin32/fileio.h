//=============================================================================//
//  Copyright ©2001 Avalanche Software.  All Rights Reserved.                  //
//=============================================================================//
//      File: fileio.h                                                         //
//    Author: Daniel Silver                                                    //
//   Created: 03/20/2001                                                       //
//=============================================================================//

#ifndef __FILEIO_H__
#define __FILEIO_H__

#include <windows.h>

DWORD GetFilePointer( HANDLE hFile );
HANDLE FileOpen( const TCHAR *pFileName );
HANDLE FileCreate( const TCHAR *pFileName, DWORD OpenMode = CREATE_ALWAYS );
HANDLE FileClose( HANDLE hFile );
BOOL FileRead( HANDLE hFile, VOID *pBuffer, DWORD dwBuffer );
BOOL FileRead( HANDLE hFile, VOID *pBuffer, DWORD dwBuffer, DWORD FilePos );
BOOL FileWrite( HANDLE hFile, VOID *pBuffer, DWORD dwBuffer );
BOOL FileWrite( HANDLE hFile, VOID *pBuffer, DWORD dwBuffer, DWORD FilePos );
BOOL FileWriteStr( HANDLE hFile, const char *pString );
BOOL FileCreateDirectory( LPCTSTR pPath, BOOL RemoveFileName );

LPCTSTR AskFileOpen( HWND hParentWnd, LPTSTR pCaption, LPTSTR pFileFilters );
LPCTSTR AskFileSave( HWND hParentWnd, LPTSTR pCaption, LPTSTR pFileFilters );
LPCTSTR AskDirectoryOpen( LPCTSTR pCurrentDirectory, HWND hParentWnd );

#endif // __FILEIO_H__