// AMViewerSplitter.h : header file
//
#define __AMVIEWERSPLITTER_H

/////////////////////////////////////////////////////////////////////////////
// CAMViewerSplitter window

class CAMViewerSplitter : public CSplitterWnd
{
// Construction
public:
	CAMViewerSplitter();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAMViewerSplitter)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAMViewerSplitter();

	// Generated message map functions
protected:
	//{{AFX_MSG(CAMViewerSplitter)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
