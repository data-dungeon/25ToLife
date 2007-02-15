/ TupViewerView.cpp : implementation of the CTupViewerView class
//

#include "stdafx.h"
#include "TupViewer.h"

#include "TupViewerDoc.h"
#include "TupViewerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTupViewerView

IMPLEMENT_DYNCREATE(CTupViewerView, CView)

BEGIN_MESSAGE_MAP(CTupViewerView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CTupViewerView construction/destruction

CTupViewerView::CTupViewerView()
{
	// TODO: add construction code here

}

CTupViewerView::~CTupViewerView()
{
}

BOOL CTupViewerView::PreCreateWindow(CREATESTRUCT& cs)
{
	BOOL bPreCreated = TRUE;
	bPreCreated = CView::PreCreateWindow(cs);
	if( !bPreCreated )
		return FALSE;

	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style |= WS_CLIPSIBLINGS;


	return bPreCreated;
}

// CTupViewerView drawing

void CTupViewerView::OnDraw(CDC* /*pDC*/)
{
	CTupViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}


// CTupViewerView printing

BOOL CTupViewerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CTupViewerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CTupViewerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CTupViewerView diagnostics

#ifdef _DEBUG
void CTupViewerView::AssertValid() const
{
	CView::AssertValid();
}

void CTupViewerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTupViewerDoc* CTupViewerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTupViewerDoc)));
	return (CTupViewerDoc*)m_pDocument;
}
#endif //_DEBUG


// CTupViewerView message handlers
