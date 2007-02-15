//
#include "stdafx.h"
#include "Snipe.h"
#include "SnipeDocTemplate.h"
#include "BaseDoc.h"
#include "GLBaseFrame.h"
#include "GLBaseView.h"

// CSnipeDocTemplate

CSnipeDocTemplate::CSnipeDocTemplate( UINT nIDResource, CRuntimeClass *pDocClass, CRuntimeClass *pFrameClass, CRuntimeClass *pViewClass )
 : CMultiDocTemplate( nIDResource, pDocClass, NULL, NULL )
{
   ASSERT(pFrameClass == NULL ||
      pFrameClass->IsDerivedFrom(RUNTIME_CLASS(CGLBaseFrame)));
	ASSERT(pViewClass == NULL ||
		pViewClass->IsDerivedFrom(RUNTIME_CLASS(CGLBaseView)));

   m_pFrameClass = pFrameClass;
   m_pViewClass = pViewClass;
}

CSnipeDocTemplate::~CSnipeDocTemplate()
{
}

// CSnipeDocTemplate member functions

//! Create a root control wired to the specified document
CGLControl *CSnipeDocTemplate::CreateControl( CBaseDoc *pDoc )
{
	if (pDoc != NULL)
		ASSERT_VALID(pDoc);

	ASSERT(m_nIDResource != 0); // must have a resource ID to load from

   if (m_pFrameClass == NULL)
   {
		TRACE(traceAppMsg, 0, "Error: No frame class.  You must override CSnipeDocTemplate::CreateControl.\n");
		ASSERT(FALSE);
		return NULL;
	}

   CGLBaseFrame *pBaseFrame = (CGLBaseFrame *)m_pFrameClass->CreateObject();
   if (pBaseFrame == NULL)
   {
      TRACE(traceAppMsg, 0, "Warning: Dynamic create of frame %hs failed.\n", m_pFrameClass->m_lpszClassName);
      return NULL;
   }
   ASSERT_KINDOF(CGLBaseFrame, pBaseFrame);

   pBaseFrame->m_pViewClass = m_pViewClass;

	// it worked !
	return pBaseFrame;
}

CDocument* CSnipeDocTemplate::OpenDocumentFile( LPCTSTR lpszPathName, BOOL bMakeVisible/*=TRUE*/ )
{
	CDocument* pDocument = CreateNewDocument();
	if (pDocument == NULL)
	{
		TRACE(traceAppMsg, 0, "CDocTemplate::CreateNewDocument returned NULL.\n");
		AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
		return NULL;
	}
	ASSERT_VALID(pDocument);

	BOOL bAutoDelete = pDocument->m_bAutoDelete;
	pDocument->m_bAutoDelete = FALSE;   // don't destroy if something goes wrong
/*
   CFrameWnd* pFrame = CreateNewFrame(pDocument, NULL);
	pDocument->m_bAutoDelete = bAutoDelete;
	if (pFrame == NULL)
	{
		AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
		delete pDocument;       // explicit delete on error
		return NULL;
	}
	ASSERT_VALID(pFrame);
*/
	if (lpszPathName == NULL)
	{
		// create a new document - with default document name
		SetDefaultTitle(pDocument);

		// avoid creating temporary compound file when starting up invisible
		if (!bMakeVisible)
			pDocument->m_bEmbedded = TRUE;

		if (!pDocument->OnNewDocument())
		{
			// user has be alerted to what failed in OnNewDocument
			TRACE(traceAppMsg, 0, "CDocument::OnNewDocument returned FALSE.\n");
//			pFrame->DestroyWindow();
			return NULL;
		}

		// it worked, now bump untitled count
		m_nUntitledCount++;
	}
	else
	{
		// open an existing document
		CWaitCursor wait;
		if (!pDocument->OnOpenDocument(lpszPathName))
		{
			// user has be alerted to what failed in OnOpenDocument
			TRACE(traceAppMsg, 0, "CDocument::OnOpenDocument returned FALSE.\n");
//			pFrame->DestroyWindow();
			return NULL;
		}
		pDocument->SetPathName(lpszPathName);
	}

//	InitialUpdateFrame(pFrame, pDocument, bMakeVisible);
	return pDocument;
}
