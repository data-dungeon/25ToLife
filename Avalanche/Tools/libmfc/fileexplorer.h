#ifndef _FILEEXPLORER_H_
#define _FILEEXPLORER_H_

#include <afxwin.h>
#include <afxcview.h>

#define FXN_DIRECTORYCHANGING		(WM_USER+100)
#define FXN_DIRECTORYCHANGED		(WM_USER+101)

#define IDC_FILEEXPLORER (0x101)

class DirectoryFile : public CTreeNode
{
	public:
		DirectoryFile( void );
		virtual ~DirectoryFile();

	public:
		virtual void SetDisplayName( TCHAR *pDisplayName );
		virtual void SetFilePath( TCHAR *pFilePath );
		virtual void DeleteChildren( void );

	public:
		tstring m_DisplayName;
		tstring m_FilePath;
};

class FileExplorer : public CTreeView
{
	DECLARE_DYNCREATE( FileExplorer );

	public:
		FileExplorer( void );
		virtual ~FileExplorer();

	public:
		BOOL Initialize( void );

		//////////////////////////////////////////////////////////////////////////

		BOOL ScanForFolders( void );
		BOOL ScanForFolders( LPSHELLFOLDER lpsf, LPCITEMIDLIST pidl, HTREEITEM hRoot );

		//////////////////////////////////////////////////////////////////////////

		void SetFilePattern( TCHAR *pFilePattern );
		BOOL GetCurrentDirectoryListing( void );
		BOOL GetDirectoryListing( DirectoryFile *pFileList, LPCITEMIDLIST pidl );
		void SetCompletionCallBack( STATICCALLBACK CallBackFunction, PARAM CallBackParam );

		//////////////////////////////////////////////////////////////////////////

		int GetShellDirectoryImage( LPITEMIDLIST pidl, UINT Flags );
		int GetShellDesktopImage( UINT Flags );
		BOOL SetCurrentDirectory( TCHAR *pPathName );
		BOOL SetCurrentDirectory( LPITEMIDLIST pidl );
		BOOL GetCurrentDirectory( TCHAR *pPathName, int MaxPath );
		DirectoryFile * GetDirectoryFile( int Index );

		//////////////////////////////////////////////////////////////////////////

		void ExpandItem( HTREEITEM hItem );

		//////////////////////////////////////////////////////////////////////////

		void PostMessageToAllViews( UINT message, WPARAM wParam, LPARAM lParam );

	public:
		virtual DirectoryFile * NewDirectoryFile( void );
		virtual BOOL ProcessFile( DirectoryFile *pFile );
		virtual void FinishDirectoryListing( DirectoryFile *pFileList );

	public:
		virtual BOOL OnCmdMsg( UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo );
		virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );

	protected:
		afx_msg int OnCreate( LPCREATESTRUCT lpcs );
		afx_msg void OnDestroy();
		afx_msg BOOL OnItemExpanding( UINT Id, NMHDR *pNotifyStruct, LRESULT *pResult );
		afx_msg BOOL OnSelectionChanged( UINT Id, NMHDR *pNotifyStruct, LRESULT *pResult );

	private:
		static DWORD WINAPI ThreadEntryPoint( LPVOID pParam );
		DWORD LocalThreadFunction( LPVOID pParam );

	public:
		TCHAR m_szCurrentPath[_MAX_PATH];
		HTREEITEM m_hDesktopItem;
		CDocument *m_pDocumentFriend;
		DirectoryFile m_FileTree;
		STATICCALLBACK m_CallBackFunction;
		PARAM m_CallBackParam;
		tstring m_FilePattern;

	private:
		HANDLE m_hDirScanThread;
		DWORD m_dwDirScanThreadId;
		LPSHELLFOLDER m_lpsf;
		LPMALLOC m_lpsm;
		BOOL m_Frozen;

	protected:
		DECLARE_MESSAGE_MAP();
};

#endif // _FILEEXPLORER_H_