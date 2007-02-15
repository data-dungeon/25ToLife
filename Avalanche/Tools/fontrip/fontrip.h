#ifndef __FONTRIP_H__
#define __FONTRIP_H__

#include <libwin32.h>
#include <toolsmith.h>

struct FontMetrics
{
	struct {
		short x1;				// texture coordinate left
		short y1;				// texture coordinate top
		short x2;				// texture coordinate right
		short y2;				// texture coordinate bottom
		int left;				// space to the left of the character
		int cy;					// character offset (16.16 fixed point format) 
		int width;				// character width
		int ch;					// character height (16.16 fixed point format)
		int right;				// space to the right of the character
	} glyph[256];

	int height;
	int ascent;
	int descent;
	int leadin;
	int leadout;
};

class ColorBox : public CWindow
{
	public:
		COLORREF m_bkColor;

	protected:
		virtual void WhenPaint( HDC hdcPaint, PAINTSTRUCT *pstruct );
		virtual void WhenEraseBkgnd( HDC hdcErase ) {};
};

class PictureBox : public CWindow
{
	public:
		HBITMAP m_hbmBitmap;

		PictureBox();
		virtual ~PictureBox();
		virtual void Release();

	protected:
		virtual void WhenDestroy();
		virtual void WhenLButtonUp( DWORD dwFlags, int x, int y );
		virtual void WhenPaint( HDC hdcPaint, PAINTSTRUCT *pstruct );
};

class PreviewWindow : public CWindow
{
	public:
		HBITMAP m_hbmPreview;
		FontMetrics *m_pMetrics;
		BOOL m_bAlpha;

	protected:
		virtual BOOL WhenCreate( LPCREATESTRUCT lpCS );
		virtual void WhenDestroy( void );
		virtual BOOL WhenCommand( HWND hWindow, int nCommand, int nNotifyCode );
		virtual void WhenPaint( HDC hdcPaint, PAINTSTRUCT *pstruct );
		virtual void WhenEraseBkgnd( HDC hdcErase );
};

class FontWindow : public CDialog
{
	public:
		static int CALLBACK FontWindow::EnumFontsCallback( LPLOGFONT pLogFont, LPNEWTEXTMETRIC *pTextMetric, DWORD dwType, FontWindow *pFontWindow );

	public:
		FontWindow( void );

	public:
		virtual void EnumFonts( void );
		virtual BOOL SelectFont( LPCTSTR lpszFontName, int nBold, BOOL bItalic, int nSize, int nCharSet );
		virtual void UpdateFont( void );
		virtual void ProcessCharacterMap( LPCTSTR lpszFileName = NULL );

	protected:
		virtual void WhenOptimize( void );
		virtual BOOL WhenCommand( HWND hWindow, int nCommand, int nNotifyCode );
		virtual void WhenClose();
		virtual void WhenDestroy();
		virtual BOOL WhenInitDialog( void );
		virtual void WhenHScroll( int nCode, int nPos, HWND hWindow );

	private:
		PreviewWindow m_wndPreview;
		PictureBox m_wndComposite;
		ColorBox m_wndRedWindow;
		ColorBox m_wndGreenWindow;
		ColorBox m_wndBlueWindow;
		ColorBox m_wndColorWindow;
		HBITMAP m_hbmOutput;
		HFONT m_hFont;
		TCHAR m_szFontName[64];
		int m_nFontSize;
		int m_nCharSet;
		int m_nBold;
		BOOL m_bItalic;
		COLORREF m_crFont;
		FontMetrics m_Metrics;
};

#endif // __FONTRIP_H__
