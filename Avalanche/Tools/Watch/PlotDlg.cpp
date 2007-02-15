// PlotDlg.cpp : implementation file
//

#include "stdafx.h"
#include "watch.h"
#include "PlotDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlotDlg dialog


CPlotDlg::CPlotDlg(CWnd* pParent /*=NULL*/)
	: CModelessDlg(pParent, CPlotDlg::IDD)
{
	//{{AFX_DATA_INIT(CPlotDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	Create(CPlotDlg::IDD, pParent);
}


void CPlotDlg::DoDataExchange(CDataExchange* pDX)
{
	CModelessDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPlotDlg)
	DDX_Control(pDX, IDC_STRIPCTRL, m_strip);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPlotDlg, CModelessDlg)
	//{{AFX_MSG_MAP(CPlotDlg)
	ON_BN_CLICKED(ID_STRIPCHART_PROPERTIES, OnStripchartProperties)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlotDlg message handlers

BOOL CPlotDlg::OnInitDialog() 
{
	CModelessDlg::OnInitDialog();
	
	m_strip.SetHandles(3);
	m_strip.SetXSpan(10);
	m_strip.SetMaxBufferSize(600); // 20 Seconds at 30 FPS
	m_strip.SetTrackMax(50);
	m_strip.SetTrackMin(-50);
	m_strip.SetTrackDisplayMax(10);
	m_strip.SetTrackDisplayMin(-10);
	m_strip.SetVariables(20);
	return TRUE;
}

void CPlotDlg::OnStripchartProperties() 
{
	m_strip.ShowPropertyPage();
}

void CPlotDlg::AddXY(VAR_ADD_XY& vaxy)
{
	for (int i = 0; i < vaxy.count; i++)
	{
		m_strip.AddXY(vaxy.variable, vaxy.vals[i].x, vaxy.vals[i].y);
	}
}
