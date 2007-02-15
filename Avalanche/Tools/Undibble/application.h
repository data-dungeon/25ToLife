#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <afxwin.h>

typedef struct
{
	char marker[8];
	DWORD count;
}
UNDIBBLEFILEHEADER;

typedef struct
{
	DWORD width;
	DWORD height;
	DWORD format;
	DWORD stride;
	DWORD bpp;
	DWORD imagesize;
	DWORD palettesize;
}
UNDIBBLEIMAGEHEADER;

class UndibbleApp : public CWinApp
{
	public:
		UndibbleApp( void );
		virtual ~UndibbleApp();

	public:
		RECT m_StartupRect;

		virtual CFont * DefaultGuiFont( BOOL Bold = FALSE );

	protected:
		virtual BOOL InitInstance( void );
		virtual void InitRegistry( void );
		virtual void InitImageList( void );
		virtual void InitDocTemplates( void );
		virtual void InitGuiFonts( void );
		
		//////////////////////////////////////////////////////////////////////////
		
		virtual int ExitInstance( void );
		virtual void SaveSettings( void );

		//////////////////////////////////////////////////////////////////////////
		
		virtual BOOL ProcessCommandLine( void );

		//////////////////////////////////////////////////////////////////////////

	protected:
		afx_msg void OnFileNew( void );
		afx_msg void OnFileOpen( void );
		afx_msg void OnFileUndibble( void );
		
	protected:
		CFont m_DefaultGuiFont;
		CFont m_DefaultBoldFont;

	protected:
		DECLARE_MESSAGE_MAP();
};

extern UndibbleApp App;

#endif // _APPLICATION_H_