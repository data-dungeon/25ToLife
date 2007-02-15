// TupViewerDoc.cpp : implementation of the CTupViewerDoc class
//

#include "stdafx.h"
#include "TupViewer.h"

#include "TupViewerDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTupViewerDoc

IMPLEMENT_DYNCREATE(CTupViewerDoc, CDocument)

BEGIN_MESSAGE_MAP(CTupViewerDoc, CDocument)
END_MESSAGE_MAP()


// CTupViewerDoc construction/destruction

CTupViewerDoc::CTupViewerDoc() : m_pAgg(NULL)
{
	// TODO: add one-time construction code here

}

CTupViewerDoc::~CTupViewerDoc()
{
	delete m_pAgg;
}

BOOL CTupViewerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CTupViewerDoc serialization

void CTupViewerDoc::Serialize(CArchive& ar)
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


// CTupViewerDoc diagnostics

#ifdef _DEBUG
void CTupViewerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTupViewerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CTupViewerDoc commands

BOOL CTupViewerDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	if (m_pAgg)
	{
		int result = TupperwareLoadSaveManager::Write(TupperwareLoadSaveManager::ASCII,lpszPathName,m_pAgg);
		return result;
	}
	return FALSE;
}

BOOL CTupViewerDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	SetTitle(lpszPathName);

	TupperwareAggregate *pAgg;
	int result = TupperwareLoadSaveManager::Read(lpszPathName,&pAgg);
	if (result)
	{
		m_pAgg = pAgg;
		//		TriggerTotalRefresh();
		return TRUE;
	}
	else
	{
		delete pAgg;
		return FALSE;
	}
}

void CTupViewerDoc::TriggerTotalRefresh(void)
{
	theApp.TriggerTotalRefresh();
}

TupperwareAggregate *CTupViewerDoc::GetTupData(void)
{
	return m_pAgg;
}
