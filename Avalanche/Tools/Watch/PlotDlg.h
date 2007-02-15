//{{AFX_INCLUDES()
#include "strip.h"
//}}AFX_INCLUDES
#if !defined(AFX_PLOTDLG_H__AC4ED3A0_B32E_45B3_AB88_A37343A271D7__INCLUDED_)
#define AFX_PLOTDLG_H__AC4ED3A0_B32E_45B3_AB88_A37343A271D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PlotDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPlotDlg dialog

#include "ModelessDlg.h"
#include "WatchInfo.h"

class CPlotDlg : public CModelessDlg
{
// Construction
public:
	CPlotDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPlotDlg)
	enum { IDD = IDD_PLOT_DIALOG };
	CStrip	m_strip;
	//}}AFX_DATA

	void AddXY(VAR_ADD_XY& vaxy);
	void Clear() {m_strip.ClearAll();}
	void Extents(VAR_EXTENTS& vext)
	{
		m_strip.SetTrackDisplayMax(vext.ymax);
		m_strip.SetTrackDisplayMin(vext.ymin);
		m_strip.SetXDisplayMin(vext.xmin);
		m_strip.SetXDisplayMax(vext.xmax);
	}


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlotDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPlotDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnStripchartProperties();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLOTDLG_H__AC4ED3A0_B32E_45B3_AB88_A37343A271D7__INCLUDED_)
