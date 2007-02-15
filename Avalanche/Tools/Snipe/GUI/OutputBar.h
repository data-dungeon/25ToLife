#pragma once

class COutputWnd : public CRichEditCtrl
{
public:
   DECLARE_DYNCREATE(COutputWnd);
	
   COutputWnd();
   virtual ~COutputWnd();

   // Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COutputWnd)
	//}}AFX_VIRTUAL

   BOOL Create(CWnd *pParentWnd);
   void AppendLine(const char *line);
   void ClearText();

	// Generated message map functions
protected:
	//{{AFX_MSG(COutputWnd)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
