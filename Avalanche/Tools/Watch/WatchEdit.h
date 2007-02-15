#if !defined(AFX_WATCHEDIT_H__68D95943_6FCF_4BFA_AFA8_33D755301B45__INCLUDED_)
#define AFX_WATCHEDIT_H__68D95943_6FCF_4BFA_AFA8_33D755301B45__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WatchEdit.h : header file
//

class CWatchSlider;

/////////////////////////////////////////////////////////////////////////////
// CWatchEdit window

class CWatchEdit : public CEdit
{
// Construction
public:
	CWatchEdit();
	virtual ~CWatchEdit();
	void Slider(CWatchSlider* s) {fSlider = s;}
	void IsTextOnly(bool isTextOnly) { textOnly = isTextOnly; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWatchEdit)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CWatchEdit)
	afx_msg void OnChange();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
	CWatchSlider*	fSlider;
	bool				textOnly;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WATCHEDIT_H__68D95943_6FCF_4BFA_AFA8_33D755301B45__INCLUDED_)
