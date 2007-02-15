// ViewBar.cpp: implementation of the CViewBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ViewBar.h"

IMPLEMENT_DYNCREATE(CViewBar, TViewBarBase)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CViewBar::CViewBar()
{
}


CViewBar::~CViewBar()
{
}


BEGIN_MESSAGE_MAP(CViewBar, TViewBarBase)
	//{{AFX_MSG_MAP(CRegBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewBar message handlers

/* ---------------------------------------------------------------
		Overridden to set the view class before calling the
	base create function. Note the other version of the create
	function is "obsolete" so I don't support it.
		For an SDI application the main frame window is created
	when the document template is created and a valid
	CreateContext is passed through (hurray!). Meaning there is
	no problem creating the ViewBar in the frame window create.
	However, for an MDI application the main frame window is
	constructed outside the document creation, so the
	CreateContext isn't present. In this case you can either
	create and setup a CreateContext object with the desired
	characteristics (doc template, frame window, etc.) and pass
	it, or let the CViewBar::Create() create a CreateContext
	with only the runtime class of the view, and the main frame
	window set.
--------------------------------------------------------------- */
BOOL CViewBar::Create(
	CWnd* pParentWnd,
	CRuntimeClass *pViewClass,
	CCreateContext *pContext,
	LPCTSTR lpszWindowName,
	DWORD dwStyle,
	UINT nID)
{
	// zero context
	ZeroMemory(&m_Context, sizeof(m_Context));

	// create frame
	CRuntimeClass *pRuntimeClass = RUNTIME_CLASS(CFrameWnd);
	CObject* pObject = pRuntimeClass->CreateObject();
	ASSERT( pObject->IsKindOf( RUNTIME_CLASS( CFrameWnd ) ) );
	m_pFrameWnd = (CFrameWnd *)pObject;

	ASSERT(pViewClass != NULL);
	ASSERT(pViewClass->IsDerivedFrom( RUNTIME_CLASS(CWnd)) );

	if (pContext)
		memcpy(&m_Context, pContext, sizeof(m_Context));
	else
	{
		CFrameWnd *fw = (CFrameWnd *)AfxGetMainWnd();
		if (fw)
		{
			m_Context.m_pCurrentDoc = fw->GetActiveDocument();
			m_Context.m_pCurrentFrame = fw;
		}
	}
	m_Context.m_pNewViewClass = pViewClass;

	// create document
	CWinApp *pApp = AfxGetApp();
	POSITION docpos = pApp->GetFirstDocTemplatePosition();
	CDocTemplate *pTemplate = pApp->GetNextDocTemplate(docpos);
	m_Context.m_pCurrentDoc = pTemplate->CreateNewDocument();

	return TViewBarBase::Create(
		lpszWindowName,
		pParentWnd,
		nID,
		dwStyle);
}


/* ---------------------------------------------------------------
		Create the frame window associated with the view bar.
--------------------------------------------------------------- */
int CViewBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (TViewBarBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_pFrameWnd->Create(NULL, NULL,
		WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE,
		CRect(0,0,0,0), this, NULL, 0,
		&m_Context))
		return -1;
	return 0;
}


/* ---------------------------------------------------------------
		Must remember to move the frame window as well...
--------------------------------------------------------------- */
void CViewBar::OnSize(UINT nType, int cx, int cy) 
{
	CRect rc;

	TViewBarBase::OnSize(nType, cx, cy);
	GetClientRect(rc);
	m_pFrameWnd->MoveWindow(rc);
}

CView *CViewBar::GetView()//AKJ
{
	if (!m_Context.m_pCurrentDoc)
		return NULL;
	POSITION pos =  m_Context.m_pCurrentDoc->GetFirstViewPosition();
	return (CView*)m_Context.m_pCurrentDoc->GetNextView(pos);
}