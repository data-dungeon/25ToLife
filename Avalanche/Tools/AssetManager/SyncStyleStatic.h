#ifndef __SYNCSTYLESTATIC__
#define __SYNCSTYLESTATIC__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SyncStyleStatic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SyncStyleStatic window

class SyncStyleStatic : public CStatic
{
// Construction
public:
	SyncStyleStatic();

// Attributes
public:
	int m_syncStyle;
// Operations
public:
	void SetSyncStyle(int syncStyle);
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SyncStyleStatic)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~SyncStyleStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(SyncStyleStatic)
	afx_msg void OnPaint();
	//}}AFX_MSG

	HBITMAP CreateAlphaMixedBitmap(HDC hDC,int resourceID,COLORREF bkColor,BITMAPINFO *pBmi);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif 
