#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <afxwin.h>
#include "direct3d.h"
#include "optionsdlg.h"

//=============================================================================
// SevenupCommandLine
//
//		Processes the command line for this application.
//=============================================================================

class SevenupCommandLine : public CCommandLineInfo
{
	public:
		SevenupCommandLine( void );
		virtual void ParseParam( const char* pParam, BOOL Flag, BOOL Last );

	BOOL m_DoExport;
	CString m_strExportName;
};

//=============================================================================
// SevenupApp
//
//		Application core.
//=============================================================================


class SevenupApp : public CWinApp
{
	public:
		SevenupApp( void );
		virtual ~SevenupApp();

	public:
		RECT m_StartupRect;
		CImageList m_ImageList;
		tstring m_ExportUtility;
		tstring m_ThumbnailPath;
		tstring m_CurrentDirectory;
		tstring m_StartupDirectory;

		virtual void SetThumbnailDirectory( TCHAR *pPathName );
		virtual void SetCurrentDirectory( TCHAR *pPathName );
		virtual TCHAR* GetCurrentDirectory( void );
		virtual TCHAR* GetStartupDirectory( void );
		virtual CFont* DefaultGuiFont( BOOL Bold = FALSE );
		virtual t_Handle DefaultTextureHandle( void );
		virtual Font* GetGameFont( void );
		virtual Font* GetSystemFont( void );
		virtual void OnOptionsOK( void );

		BOOL AskDocOpen( CString& FileName );
		BOOL AskDocSave( CString& FileName );

	protected:
		virtual BOOL InitInstance( void );
		virtual void InitStartupDirectory( void );
		virtual void InitRegistry( void );
		virtual void InitImageList( void );
		virtual void InitDocTemplates( void );
		virtual void InitGuiFonts( void );
		virtual BOOL InitMainFrame( void );
		virtual BOOL InitDirect3D( void );
		virtual void InitGameEngine( void );
		virtual void InitGameFonts( void );
		virtual BOOL InitProject( void );
		virtual void InitCurrentDirectory( void );
		
		//////////////////////////////////////////////////////////////////////////
		
		virtual int ExitInstance( void );
		virtual void SaveSettings( void );
		virtual void ShutdownGameEngine( void );
		virtual void ShutdownDirect3D( void );
		virtual void ShutdownProject( void );

		//////////////////////////////////////////////////////////////////////////
		
		virtual void ParseCommandLine( SevenupCommandLine &cli );
		virtual BOOL ProcessCommandLine( SevenupCommandLine &cli );

		//////////////////////////////////////////////////////////////////////////

	protected:
		afx_msg void OnFileNew( void );
		afx_msg void OnFileOpen( void );
		afx_msg void OnViewOptions( void );

	protected:
		virtual BOOL OnIdle( LONG Count );
		
	protected:
		CFont m_DefaultGuiFont;
		CFont m_DefaultBoldFont;
		Font* m_pSystemFont;
		t_Handle m_DefaultTextureHandle;

		OptionsDialog m_OptionsDialog;

	protected:
		DECLARE_MESSAGE_MAP();
};

extern SevenupApp App;

#endif // _APPLICATION_H_