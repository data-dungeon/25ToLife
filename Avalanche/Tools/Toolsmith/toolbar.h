#ifndef __TOOLBAR_H__
#define __TOOLBAR_H__

#include "window.h"

typedef struct
{
	WORD Version;
	WORD Width;
	WORD Height;
	WORD ItemCount;
	WORD Items[1];
}
ToolBarResource;

class CToolBar : public CWindow
{
	public:
		CToolBar( void );
		virtual ~CToolBar();
		virtual BOOL CreateToolBar( HWND hParent, UINT Resource, UINT ToolBarID );
		virtual BOOL WhenCommand( HWND hWindow, int nCommand, int nNotifyCode );
};

BOOL ToolBarLoadResource( HWND hToolBar, LPCTSTR lpResouceName );
void ToolBarSetButtonSize( HWND hToolBar, int Width, int Height );
void ToolBarSetBitmapSize( HWND hToolBar, int Width, int Height );
int ToolBarAddBitmapResource( HWND hToolBar, int ItemCount, LPCTSTR lpResourceName );
int ToolBarAddBitmapHandle( HWND hToolBar, int ItemCount, HANDLE hBitmap );
int ToolBarAddSeparator( HWND hToolBar );
int ToolBarAddButton( HWND hToolBar, int Style, int Bitmap, int Command, int String );

#endif // __TOOLBAR_H__