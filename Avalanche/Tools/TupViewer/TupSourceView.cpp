// TupSourceView.cpp : implementation of the TupSourceView class
//

#include "stdafx.h"
#include "TupViewer.h"
#include "TupSourceView.h"

#include "MainFrm.h"

#if !defined(__EXT_MEMORY_DC_H)
#include "ExtMemoryDC.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TupSourceView

TupSourceView::TupSourceView()
{
}

TupSourceView::~TupSourceView()
{
}

BEGIN_MESSAGE_MAP(TupSourceView, CEdit)
	//{{AFX_MSG_MAP(TupSourceView)
	ON_WM_CREATE()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_DEL, OnEditDel)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DEL, OnUpdateEditDel)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TupSourceView message handlers

BOOL TupSourceView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CEdit::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle &=
		~(WS_EX_CLIENTEDGE|WS_EX_STATICEDGE);
	cs.style &=
		~(WS_BORDER|WS_THICKFRAME|WS_DLGFRAME);
	cs.style |=
		WS_HSCROLL|WS_VSCROLL
		|ES_MULTILINE|ES_LEFT
		|ES_AUTOHSCROLL|ES_AUTOVSCROLL
		|ES_WANTRETURN;

	return TRUE;
}

int TupSourceView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if( CEdit::OnCreate(lpCreateStruct) == -1 )
		return -1;

	SetFont(
		CFont::FromHandle(
		(HFONT)::GetStockObject(DEFAULT_GUI_FONT)
		)
		);

	return 0;
}


void TupSourceView::OnEditCopy() 
{
	CEdit::Copy();
}
void TupSourceView::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	int nStartChar, nEndChar;
	CEdit::GetSel( nStartChar, nEndChar );
	pCmdUI->Enable( nStartChar != nEndChar );
}

void TupSourceView::OnEditCut() 
{
	CEdit::Cut();
}
void TupSourceView::OnUpdateEditCut(CCmdUI* pCmdUI) 
{
	OnUpdateEditCopy(pCmdUI);
}

void TupSourceView::OnEditPaste() 
{
	CEdit::Paste();
}
void TupSourceView::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(
		::IsClipboardFormatAvailable(CF_TEXT)
		);
}

void TupSourceView::OnEditUndo() 
{
	CEdit::Undo();
}
void TupSourceView::OnUpdateEditUndo(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( CEdit::CanUndo() );
}

void TupSourceView::OnEditDel() 
{
	CEdit::ReplaceSel( _T(""), TRUE );
}
void TupSourceView::OnUpdateEditDel(CCmdUI* pCmdUI) 
{
	OnUpdateEditCopy(pCmdUI);
}

void TupSourceView::OnEditSelectAll() 
{
	CEdit::SetSel( 0, -1 );
}
