// WatchDoc.cpp : implementation of the CWatchDoc class
//

#include "stdafx.h"
#include "Watch.h"

#include "WatchDoc.h"
#include "WatchComm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWatchDoc

IMPLEMENT_DYNCREATE(CWatchDoc, CDocument)

BEGIN_MESSAGE_MAP(CWatchDoc, CDocument)
	//{{AFX_MSG_MAP(CWatchDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWatchDoc construction/destruction

CWatchDoc::CWatchDoc()
{
	WatchComm::Instance()->InitHosts("127.0.0.1", remoteMachine);
}

CWatchDoc::~CWatchDoc()
{
}

BOOL CWatchDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CWatchDoc serialization

void CWatchDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CWatchDoc diagnostics

#ifdef _DEBUG
void CWatchDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CWatchDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWatchDoc commands
