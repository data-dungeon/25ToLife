#pragma once

/*!
The CSnipeDocTemplate class overrides the virtual OpenDocumentFile function
from CMultiDocTemplate.  It takes the steps necessary to open a new CGLSnipeView
and CBaseDoc for the object since we don't have the traditional MFC CChildFrame
and such.  It then calls the newly created document's OnOpenDocument() function.
*/
class CGLControl;
class CBaseDoc;

class CSnipeDocTemplate : public CMultiDocTemplate
{
public:
   CRuntimeClass *m_pFrameClass; // class for creating new frame controls
   CRuntimeClass *m_pViewClass; // class for creating new view controls

            CSnipeDocTemplate( UINT nIDResource, CRuntimeClass *pDocClass, CRuntimeClass *pFrameClass, CRuntimeClass *pViewClass );
   virtual ~CSnipeDocTemplate();

// Overridables
   virt_base virtual CGLControl *CreateControl( CBaseDoc *pDoc );

// CMultiDocTemplate overrides
   virtual CDocument* OpenDocumentFile( LPCTSTR lpszPathName, BOOL bMakeVisible=TRUE );
};


