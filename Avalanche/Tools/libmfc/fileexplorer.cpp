#include "pch.h"
#include "fileexplorer.h"
#include "pidl.h"

//=============================================================================//
// DirectoryFile::DirectoryFile
//=============================================================================//

DirectoryFile::DirectoryFile( void )
{
}

//=============================================================================//
// DirectoryFile::~DirectoryFile
//=============================================================================//

DirectoryFile::~DirectoryFile()
{
	DeleteChildren();
}

//=============================================================================//
// DirectoryFile::SetDisplayName
//=============================================================================//

void DirectoryFile::SetDisplayName( TCHAR *pDisplayName )
{
	m_DisplayName.set( pDisplayName );
}

//=============================================================================//
// DirectoryFile::SetFilePath
//=============================================================================//

void DirectoryFile::SetFilePath( TCHAR *pFilePath )
{
	m_FilePath.set( pFilePath );
}

//=============================================================================//
// DirectoryFile::DeleteChildren
//=============================================================================//

void DirectoryFile::DeleteChildren( void )
{
	while( Child() )
		delete Child();

	return;
}

typedef struct
{
	LPSHELLFOLDER lpsf;
	LPITEMIDLIST lpi;
	LPITEMIDLIST lpifq;
}
FXTREEVIEWDATA;

IMPLEMENT_DYNCREATE( FileExplorer, CTreeView );

BEGIN_MESSAGE_MAP( FileExplorer, CTreeView )
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_NOTIFY_EX_RANGE( TVN_ITEMEXPANDING, 0, 0x7fffffff, OnItemExpanding )
	ON_NOTIFY_EX_RANGE( TVN_SELCHANGED, 0, 0x7fffffff, OnSelectionChanged )
END_MESSAGE_MAP()

FileExplorer::FileExplorer( void )
{
	m_hDesktopItem = 0;
	m_pDocumentFriend = 0;
	m_CallBackFunction = 0;
	m_CallBackParam = 0;
	m_hDirScanThread = 0;
	m_dwDirScanThreadId = 0;

	m_lpsf = 0;
	m_lpsm = 0;
	m_Frozen = FALSE;
}

FileExplorer::~FileExplorer()
{
	if( m_lpsf )
		m_lpsf->Release();

	if( m_lpsm )
		m_lpsm->Release();

	return;
}

BOOL FileExplorer::Initialize( void )
{
	HRESULT hresult;

	if( m_lpsf == 0 )
	{
		hresult = SHGetDesktopFolder( &m_lpsf );

		if( FAILED(hresult) )
			return FALSE;
	}

	if( m_lpsm == 0 )
	{
		hresult = SHGetMalloc( &m_lpsm );

		if( FAILED(hresult) )
			return FALSE;
	}

	return TRUE;
}

BOOL FileExplorer::ScanForFolders( void )
{
	if( !Initialize() )
		return FALSE;

	BOOL bResult = ScanForFolders( m_lpsf, 0, TVI_ROOT );

	if( bResult )
		GetTreeCtrl().Expand( m_hDesktopItem, TVE_EXPAND );

	return bResult;
}

BOOL FileExplorer::ScanForFolders( LPSHELLFOLDER lpsf, LPCITEMIDLIST pidl, HTREEITEM hRoot )
{
	LPENUMIDLIST lpe;
	LPITEMIDLIST lpi;
	HRESULT hresult;
	ULONG nCount;
	ULONG nAttributes;
	STRRET str;
	FXTREEVIEWDATA *pParam;
	CTreeCtrl &c = GetTreeCtrl();

	if( m_hDirScanThread )
		return FALSE;

	Win32_TreeView_DeleteChildren( c.GetSafeHwnd(), hRoot );

	if( !lpsf )
		return FALSE;

	hresult = lpsf->EnumObjects( GetSafeHwnd(), SHCONTF_FOLDERS, &lpe );

	if( FAILED(hresult) )
		return FALSE;

	hresult = lpe->Reset();
	hresult = lpe->Next( 1, &lpi, &nCount );

	while( hresult == S_OK )
	{
		nAttributes = SFGAO_HASSUBFOLDER|SFGAO_FOLDER;

		lpsf->GetAttributesOf( 1, (LPCITEMIDLIST *) &lpi, &nAttributes );

		if( nAttributes & SFGAO_FOLDER )
		{
			TCHAR szDisplayName[_MAX_PATH];

			hresult = lpsf->GetDisplayNameOf( lpi, SHGDN_NORMAL, &str );

			if( !FAILED(hresult) )
			{
				if( str.uType == STRRET_WSTR )
				{
					WideCharToMultiByte( CP_ACP, 0, str.pOleStr, -1, szDisplayName, _MAX_PATH, 0, 0 );
				}
				else if( str.uType == STRRET_OFFSET )
				{
					strcpy( szDisplayName, (const char *) lpi + str.uOffset );
				}
				else if( str.uType == STRRET_CSTR )
				{
					strcpy( szDisplayName, str.cStr );
				}

				if( hRoot == TVI_ROOT )
				{
					m_hDesktopItem = c.InsertItem( "Desktop", TVI_ROOT );

					if( m_hDesktopItem )
					{
						pParam = new FXTREEVIEWDATA;

						if( pParam )
						{
							pParam->lpsf = lpsf;
							pParam->lpi = 0;
							pParam->lpifq = 0;

							c.SetItemData( m_hDesktopItem, (DWORD_PTR) pParam );

							int Image = GetShellDesktopImage( SHGFI_SMALLICON );

							c.SetItemImage( m_hDesktopItem, Image, Image );
						}

						hRoot = m_hDesktopItem;
					}
				}

				HTREEITEM hItem = c.InsertItem( szDisplayName, hRoot );

				if( hItem )
				{
					if( nAttributes & SFGAO_HASSUBFOLDER )
						c.InsertItem( "", hItem );

					pParam = new FXTREEVIEWDATA;

					if( pParam )
					{
						pParam->lpsf = 0;
						pParam->lpi = lpi;
						pParam->lpifq = 0;

						hresult = lpsf->BindToObject( (LPCITEMIDLIST) lpi, 0, IID_IShellFolder, (VOID **) &pParam->lpsf );

						if( !FAILED(hresult) )
							pParam->lpifq = ConcatPidls( pidl, lpi );
					}

					LPITEMIDLIST pidl = pParam->lpifq;

					if( pidl == 0 )
						pidl = pParam->lpi;

					int Image1 = GetShellDirectoryImage( pidl, SHGFI_SMALLICON );
					int Image2 = GetShellDirectoryImage( pidl, SHGFI_SMALLICON|SHGFI_OPENICON );

					c.SetItemData( hItem, (DWORD_PTR) pParam );
					c.SetItemImage( hItem, Image1, Image2 );
				}
			}
		}

		hresult = lpe->Next( 1, &lpi, &nCount );
	}

	lpe->Release();

	return TRUE;
}

void FileExplorer::SetFilePattern( TCHAR *pFilePattern )
{
	m_FilePattern.set( pFilePattern );
}

BOOL FileExplorer::GetCurrentDirectoryListing( void )
{
	CTreeCtrl &c = GetTreeCtrl();

	FXTREEVIEWDATA *pParam = (FXTREEVIEWDATA *) c.GetItemData( c.GetSelectedItem() );

	if( !pParam )
		return FALSE;

	return GetDirectoryListing( &m_FileTree, pParam->lpifq );
}

BOOL FileExplorer::GetDirectoryListing( DirectoryFile *pFileList, LPCITEMIDLIST pidl )
{
	TCHAR szDisplayName[_MAX_PATH];
	TCHAR szPathName[_MAX_PATH];
	LPSHELLFOLDER lpsf = 0;
	LPITEMIDLIST lpi = 0;
	LPENUMIDLIST lpe = 0;
	LPITEMIDLIST newPidl;
	ULONG nCount = 0;
	HRESULT hresult;
	STRRET sr;
	DirectoryFile *pNewFile;

	if( !pFileList )
		return FALSE;

	hresult = m_lpsf->BindToObject( pidl, 0, IID_IShellFolder, (VOID **) &lpsf );

	if( FAILED(hresult) )
		return FALSE;

	hresult = lpsf->EnumObjects( GetSafeHwnd(), SHCONTF_NONFOLDERS, &lpe );

	if( FAILED(hresult) )
		return FALSE;

	PostMessageToAllViews( FXN_DIRECTORYCHANGING, 0, 0 );

	pFileList->DeleteChildren();

	hresult = lpe->Reset();
	hresult = lpe->Next( 1, &lpi, &nCount );

	MSG message = {0, WM_NULL};

	while( hresult == S_OK )
	{
		// check to see if we're shutting down, and if so, stop searching for more files...

		if( PeekMessage( &message, NULL, 0, 0, PM_NOREMOVE ) )
		{
			if( message.message == WM_QUIT )
				break;
		}

		newPidl = ConcatPidls( pidl, lpi );
		SHGetPathFromIDList( newPidl, szPathName );

		lpsf->GetDisplayNameOf( lpi, SHGDN_NORMAL, &sr );

		StrRetToBuf( &sr, pidl, szDisplayName, sizeof(szDisplayName)/sizeof(szDisplayName[0]) );

		m_lpsm->Free( newPidl );
		m_lpsm->Free( lpi );

		hresult = lpe->Next( 1, &lpi, &nCount );

		pNewFile = NewDirectoryFile();

		if( pNewFile )
		{
			pNewFile->SetDisplayName( szDisplayName );
			pNewFile->SetFilePath( szPathName );

			if( ProcessFile( pNewFile ) )
				m_FileTree.AddChild( pNewFile );
			else
			{
				delete pNewFile;
				pNewFile = 0;
			}
		}
	}

	lpe->Release();

	if( m_CallBackFunction )
		StaticCallBack( m_CallBackFunction, m_CallBackParam );

	FinishDirectoryListing( pFileList );

	PostMessageToAllViews( FXN_DIRECTORYCHANGED, 0, 0 );

	return TRUE;
}

void FileExplorer::SetCompletionCallBack( STATICCALLBACK CallBackFunction, PARAM CallBackParam )
{
	m_CallBackFunction = CallBackFunction;
	m_CallBackParam = CallBackParam;
}

int FileExplorer::GetShellDirectoryImage( LPITEMIDLIST pidl, UINT Flags )
{
	SHFILEINFO fileinfo;

	HIMAGELIST hImageList = (HIMAGELIST) SHGetFileInfo( (LPCSTR) pidl, 0, &fileinfo, sizeof(fileinfo), Flags|SHGFI_PIDL|SHGFI_SYSICONINDEX|SHGFI_DISPLAYNAME );

	if( !hImageList )
		return -1;

	return fileinfo.iIcon;
}

int FileExplorer::GetShellDesktopImage( UINT Flags )
{
	LPITEMIDLIST pidl;

	HRESULT hresult = SHGetSpecialFolderLocation( NULL, CSIDL_DESKTOP, &pidl );

	if( FAILED(hresult) )
		return -1;

	int Image = GetShellDirectoryImage( pidl, Flags );

	m_lpsm->Free( pidl );

	return Image;
}

BOOL FileExplorer::SetCurrentDirectory( TCHAR *pPathName )
{
	LPITEMIDLIST pidl = FileNameToPidl( pPathName );

	BOOL Result = SetCurrentDirectory( pidl );

	DeletePidl( pidl );

	return Result;
}

BOOL FileExplorer::SetCurrentDirectory( LPITEMIDLIST pidl )
{
	ScanForFolders();

	if( !pidl )
		return FALSE;

	CTreeCtrl &c = GetTreeCtrl();

	HTREEITEM hRoot = c.GetRootItem();

	if( !hRoot )
		return FALSE;

	HTREEITEM hItem = c.GetNextItem( hRoot, TVGN_CHILD );
	FXTREEVIEWDATA *p;
	int result;
	int NextItem;
	CString str;

	while( hItem )
	{
		NextItem = TVGN_NEXT;

		p = (FXTREEVIEWDATA*) c.GetItemData( hItem );

		str = c.GetItemText( hItem );

		if( p )
		{
			if( p->lpifq )
			{
				result = ComparePidls( p->lpifq, pidl );

				if( result == PIDL_EXACTMATCH )
				{
					ScanForFolders( p->lpsf, p->lpifq, hItem );
					ExpandItem( hItem );
					c.Select( hItem, TVGN_CARET );
					return TRUE;
				}
				else
				if( result == PIDL_PARTIALMATCH )
				{
					ScanForFolders( p->lpsf, p->lpifq, hItem );
					NextItem = TVGN_CHILD;
				}
			}
		}

		hItem = c.GetNextItem( hItem, NextItem );
	}

	return FALSE;
}

BOOL FileExplorer::GetCurrentDirectory( TCHAR *pPathName, int MaxPath )
{
	CTreeCtrl &c = GetTreeCtrl();

	FXTREEVIEWDATA *pParam = (FXTREEVIEWDATA *) c.GetItemData( c.GetSelectedItem() );

	if( pParam )
	{
		TCHAR szPathName[_MAX_PATH];
		BOOL bResult = SHGetPathFromIDList( pParam->lpifq, szPathName );

		strncpy( pPathName, szPathName, MaxPath );

		return TRUE;
	}

	pPathName[0] = 0;

	return FALSE;
}

DirectoryFile *FileExplorer::GetDirectoryFile( int Index )
{
	return (DirectoryFile *) m_FileTree.GetNthChild( Index );
}

void FileExplorer::ExpandItem( HTREEITEM hItem )
{
	CTreeCtrl &c = GetTreeCtrl();

	c.Expand( hItem, TVE_EXPAND );

	if( m_hDirScanThread )
		PostThreadMessage( m_dwDirScanThreadId, WM_QUIT, 0, 0 );
	else
	{
		m_hDirScanThread = CreateThread( 0, 0, ThreadEntryPoint, (LPVOID) this, 0, &m_dwDirScanThreadId );
	}

	return;
}

void FileExplorer::PostMessageToAllViews( UINT message, WPARAM wParam, LPARAM lParam )
{
	CDocument *pDocument = GetDocument();

	if( pDocument )
	{
		POSITION pos = pDocument->GetFirstViewPosition();

		while (pos != NULL)
		{
			CView* pView = pDocument->GetNextView(pos);
			pView->PostMessage( message, wParam, lParam );
		}
	}

	return;
}

DirectoryFile * FileExplorer::NewDirectoryFile( void )
{
	return new DirectoryFile;
}

BOOL FileExplorer::ProcessFile( DirectoryFile *pFile )
{
	char *name = pFile->m_DisplayName.get();
	int namelen = pFile->m_DisplayName.length();
	char *s = m_FilePattern.get();
	int len = m_FilePattern.length();
	int ch1, ch2;
	int a = 0;
	int b = 0;
	int wild;

	while( a < len )
	{
		ch1 = tolower(s[a]);
		ch2 = tolower(name[b]);

		if( ch1 == '*' )
		{
			wild = 1;
			a++;
		}

		if( ch1 == '?' )
			ch1 = ch2;

		if( ch1 != ch2 )
		{
			if( !wild )
				return FALSE;

			if( ch2 == 0 )
				return FALSE;
		}
		else if( ch1 != '*' )
		{
			wild = 0;
		}

		if( !wild)
			a++;

		b++;
	}

	return TRUE;
}

void FileExplorer::FinishDirectoryListing( DirectoryFile *pFileList )
{
}

DWORD WINAPI FileExplorer::ThreadEntryPoint( LPVOID pParam )
{
	FileExplorer *pSelf = (FileExplorer *) pParam;

	if( pSelf )
	{
		DWORD dwResult = pSelf->LocalThreadFunction( pParam );

		if( GetCurrentThreadId() == pSelf->m_dwDirScanThreadId )
		{
			pSelf->m_hDirScanThread = 0;
			pSelf->m_dwDirScanThreadId = 0;
		}
	}

	return 0;
}

DWORD FileExplorer::LocalThreadFunction( LPVOID pParam )
{
	MSG message = {0, WM_NULL};

	while( message.message != WM_QUIT )
	{
		if( PeekMessage( &message, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &message );
			DispatchMessage( &message );
		}
		else
		{
			GetCurrentDirectoryListing();
			PostQuitMessage(0);
		}
	}

	return (DWORD) message.wParam;
}

BOOL FileExplorer::OnCmdMsg( UINT Id, int Code, void *pExtra, AFX_CMDHANDLERINFO* pHandlerInfo )
{
	return CTreeView::OnCmdMsg( Id, Code, pExtra, pHandlerInfo );
}

LRESULT FileExplorer::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message ) 
	{
	case TVM_DELETEITEM:
		{
			CTreeCtrl &c = GetTreeCtrl();

			FXTREEVIEWDATA *pParam = (FXTREEVIEWDATA *) c.GetItemData( (HTREEITEM) lParam );

			if( pParam )
			{
				if( pParam->lpsf )
					pParam->lpsf->Release();

				if( pParam->lpi )
					m_lpsm->Free( pParam->lpi );

				if( pParam->lpifq )
					m_lpsm->Free( pParam->lpifq );

				delete pParam;
			}

			c.SetItemData( (HTREEITEM) lParam, 0 );
		}
		break;
	}

	return CTreeView::WindowProc(	message, wParam, lParam );
}

int FileExplorer::OnCreate( LPCREATESTRUCT lpcs )
{
	if( -1 == CTreeView::OnCreate( lpcs ) )
		return -1;

	SHFILEINFO shfi;
	CTreeCtrl &c = GetTreeCtrl();

	c.ModifyStyle( 0, TVS_HASBUTTONS );

	HIMAGELIST hImageList = (HIMAGELIST) SHGetFileInfo( (LPCTSTR) "C:\\",
		0, &shfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);

	c.SendMessage( TVM_SETIMAGELIST, TVSIL_NORMAL, (LPARAM) hImageList );

	return 0;
}

void FileExplorer::OnDestroy()
{
	CTreeCtrl &c = GetTreeCtrl();

	m_Frozen = TRUE;

	Win32_TreeView_DeleteChildren( c.m_hWnd, TVI_ROOT );

	m_Frozen = FALSE;

	if( m_hDirScanThread )
		TerminateThread( m_hDirScanThread, 0 );

	CTreeView::OnDestroy();
}

BOOL FileExplorer::OnItemExpanding( UINT Id, NMHDR *pNotifyStruct, LRESULT *pResult )
{
	CTreeCtrl &c = GetTreeCtrl();

	if( c.GetDlgCtrlID() != Id )
		return FALSE;

	if( pNotifyStruct )
	{
		LPNMTREEVIEW pTreeNotify = (LPNMTREEVIEW) pNotifyStruct;
		FXTREEVIEWDATA *pParam = (FXTREEVIEWDATA *) pTreeNotify->itemNew.lParam;

		if( pTreeNotify->action == TVE_EXPAND )
		{
			if( pParam )
				ScanForFolders( pParam->lpsf, pParam->lpifq, pTreeNotify->itemNew.hItem );
		}
		else
		if( pTreeNotify->action == TVE_COLLAPSE )
		{
			HTREEITEM hTreeItem = pTreeNotify->itemNew.hItem;

			Win32_TreeView_DeleteChildren( c.m_hWnd, hTreeItem );
			c.InsertItem( "", hTreeItem );
		}
	}

	return TRUE;
}

BOOL FileExplorer::OnSelectionChanged( UINT Id, NMHDR *pNotifyStruct, LRESULT *pResult )
{
	if( m_Frozen )
		return FALSE;

	CTreeCtrl &c = GetTreeCtrl();

	if( c.GetDlgCtrlID() != Id )
		return FALSE;

	ExpandItem( c.GetSelectedItem() );

	return TRUE;
}