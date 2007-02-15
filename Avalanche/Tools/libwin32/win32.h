#ifndef _WIN32_H_
#define _WIN32_H_

#include <windows.h>
#include <commctrl.h>

void Win32_TreeView_DeleteChildren( HWND hWnd, HTREEITEM hItem );
BOOL Win32_TreeView_ItemHasChildren( HWND hWnd, HTREEITEM hItem );
void Win32_ScrollBar_OnUpdate( UINT Code, int LineSize, SCROLLINFO &si );
RECT Win32_GetClientRect( HWND hWnd );
void Win32_CenterWindow( HWND hWnd, RECT &rc );
void Win32_RunProgram( LPCTSTR pApplication, LPCTSTR pCommandLine );

#endif // _WIN32_H_