//=============================================================================//
//  Copyright ©2001 Avalanche Software.  All Rights Reserved.                  //
//=============================================================================//
//      File: fileio.cpp                                                       //
//    Author: Daniel Silver                                                    //
//   Created: 03/20/2001                                                       //
//=============================================================================//

#include "pch.h"
#include "fileio.h"
#include "pidl.h"

//=============================================================================
// GetFilePointer
//=============================================================================

DWORD GetFilePointer( HANDLE hFile )
{
	return SetFilePointer( hFile, 0, 0, FILE_CURRENT );
}

//=============================================================================
// FileOpen
//=============================================================================

HANDLE FileOpen( const TCHAR *pFileName )
{
	return CreateFile( pFileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0 );
}

//=============================================================================
// FileCreate
//=============================================================================

HANDLE FileCreate( const TCHAR *pFileName, DWORD OpenMode )
{
	return CreateFile( pFileName, GENERIC_WRITE, 0, 0, OpenMode, FILE_ATTRIBUTE_NORMAL, 0 );
}

//=============================================================================
// FileClose
//=============================================================================

HANDLE FileClose( HANDLE hFile )
{
	if( !CloseHandle( hFile ) )
		return hFile;

	return INVALID_HANDLE_VALUE;
}

//=============================================================================
// FileRead
//=============================================================================

BOOL FileRead( HANDLE hFile, VOID *pBuffer, DWORD dwSize )
{
	return ReadFile( hFile, pBuffer, dwSize, &dwSize, NULL );
}

//=============================================================================
// FileRead
//=============================================================================

BOOL FileRead( HANDLE hFile, VOID *pBuffer, DWORD dwSize, DWORD FilePos )
{
	DWORD OldFilePos = GetFilePointer( hFile );

	SetFilePointer( hFile, FilePos, 0, FILE_BEGIN );

	BOOL bResult = FileRead( hFile, pBuffer, dwSize );

	SetFilePointer( hFile, OldFilePos, 0, FILE_BEGIN );

	return bResult;
}

//=============================================================================
// FileWrite
//=============================================================================

BOOL FileWrite( HANDLE hFile, VOID *pBuffer, DWORD dwSize )
{
	return WriteFile( hFile, pBuffer, dwSize, &dwSize, NULL );
}

//=============================================================================
// FileWrite
//=============================================================================

BOOL FileWrite( HANDLE hFile, VOID *pBuffer, DWORD dwSize, DWORD FilePos )
{
	DWORD OldFilePos = GetFilePointer( hFile );

	SetFilePointer( hFile, FilePos, 0, FILE_BEGIN );

	BOOL bResult = FileWrite( hFile, pBuffer, dwSize );

	SetFilePointer( hFile, OldFilePos, 0, FILE_BEGIN );

	return bResult;
}

//=============================================================================
// FileWriteStr
//=============================================================================

BOOL FileWriteStr( HANDLE hFile, const char *pString )
{
	return FileWrite( hFile, (VOID *) pString, strlen(pString) );
}

//=============================================================================
// FileCreateDirectory
//=============================================================================

BOOL FileCreateDirectory( LPCTSTR pPath, BOOL RemoveFileName )
{
	if( !RemoveFileName )
		return CreateDirectory( pPath, 0 );

	int c, ch;
	int len = strlen(pPath);

	TCHAR *pActualPath = new TCHAR[len+1];

	if( !pActualPath )
		return FALSE;

	strcpy( pActualPath, pPath );

	for( c = len-1; c >= 0; c-- )
	{
		ch = pActualPath[c];

		if( ch == '\\' || ch == '/' || ch == ':' )
			break;

		pActualPath[c] = 0;
	}

	return FileCreateDirectory( pActualPath, FALSE );
}

//=============================================================================
// AskFileOpen
//=============================================================================

LPCTSTR AskFileOpen( HWND hParentWnd, LPTSTR pCaption, LPTSTR pFileFilters )
{
	static TCHAR szFileName[_MAX_PATH+1] = {'\0'};

	OPENFILENAME o;

	o.lStructSize				= sizeof(o);
	o.hwndOwner					= hParentWnd;
	o.hInstance					= 0;
	o.lpstrFilter				= pFileFilters;
	o.lpstrCustomFilter		= 0;
	o.nMaxCustFilter			= 0;
	o.nFilterIndex				= 1;
	o.lpstrFile					= szFileName;
	o.nMaxFile					= sizeof(szFileName)/sizeof(TCHAR);
	o.lpstrFileTitle			= 0;
	o.nMaxFileTitle			= 0;
	o.lpstrInitialDir			= 0;
	o.lpstrTitle				= pCaption;
	o.Flags						= OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST;
	o.nFileOffset				= 0;
	o.nFileExtension			= 0;
	o.lpstrDefExt				= 0;
	o.lCustData					= 0;
	o.lpfnHook					= 0;
	o.lpTemplateName			= 0;

	if( !GetOpenFileName( &o ) )
		return NULL;

	return szFileName;
}

//=============================================================================
// AskFileSave
//=============================================================================

LPCTSTR AskFileSave( HWND hParentWnd, LPTSTR pCaption, LPTSTR pFileFilters )
{
	static TCHAR szFileName[_MAX_PATH+1] = {'\0'};

	OPENFILENAME o;

	o.lStructSize				= sizeof(o);
	o.hwndOwner					= hParentWnd;
	o.hInstance					= 0;
	o.lpstrFilter				= pFileFilters;
	o.lpstrCustomFilter		= 0;
	o.nMaxCustFilter			= 0;
	o.nFilterIndex				= 0;
	o.lpstrFile					= szFileName;
	o.nMaxFile					= sizeof(szFileName)/sizeof(TCHAR);
	o.lpstrFileTitle			= 0;
	o.nMaxFileTitle			= 0;
	o.lpstrInitialDir			= 0;
	o.lpstrTitle				= pCaption;
	o.Flags						= OFN_PATHMUSTEXIST;
	o.nFileOffset				= 0;
	o.nFileExtension			= 0;
	o.lpstrDefExt				= 0;
	o.lCustData					= 0;
	o.lpfnHook					= 0;
	o.lpTemplateName			= 0;

	if( !GetSaveFileName( &o ) )
		return NULL;

	return szFileName;
}

//=============================================================================
// AskDirectoryOpen
//=============================================================================

LPCTSTR AskDirectoryOpen( LPCTSTR pCurrentDirectory, HWND hParentWnd )
{
	static TCHAR szDirectory[_MAX_PATH+1] = {'\0'};

	BROWSEINFO bi;

	bi.hwndOwner = hParentWnd;
	bi.pidlRoot = FileNameToPidl( pCurrentDirectory );
	bi.pszDisplayName = szDirectory;
	bi.lpszTitle = "Choose a Directory";
	bi.ulFlags = BIF_NEWDIALOGSTYLE|BIF_EDITBOX|BIF_SHAREABLE;
	bi.lpfn = 0;
	bi.lParam = 0;
	bi.iImage = 0;

	LPITEMIDLIST pidl = SHBrowseForFolder( &bi );

	if( bi.pidlRoot )
		DeletePidl( (LPITEMIDLIST) bi.pidlRoot );

	if( !pidl )
		strcpy( szDirectory, "" );
	else
	{
		SHGetPathFromIDList( pidl, szDirectory );

		DeletePidl( pidl );
	}

	return szDirectory;
}