// StripDlg.cpp : implementation file
//

#include "stdafx.h"
#include "watch.h"
#include "StripDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define NUM_LINE_COLORS  11
static int lineColor[NUM_LINE_COLORS][3] =
{
	0xff, 0x00, 0x00,    // Red
	0x00, 0xff, 0x00,    // Green
	0x00, 0x00, 0xff,    // Blue
	0xe7, 0xe7, 0x00,    // Yellow
	0xff, 0x00, 0xff,    // Magenta
	0x7f, 0x00, 0x00,    // Dark Red
	0x00, 0x7f, 0x00,    // Dark Green
	0x00, 0x00, 0x7f,    // Dark Blue
	0x7f, 0x7f, 0x00,    // Olive Green
	0x7f, 0x00, 0x7f,    // Purple
	0x00, 0x7f, 0x7f     // Teal
};

/////////////////////////////////////////////////////////////////////////////
// CStripDlg dialog


CStripDlg::CStripDlg(CWnd* pParent /*=NULL*/)
	: CModelessDlg(pParent, CStripDlg::IDD)
{
	//{{AFX_DATA_INIT(CStripDlg)
	//}}AFX_DATA_INIT
	Create(CStripDlg::IDD, pParent);
	m_imageList.Create(IDB_LEGEND, 16, 1, RGB(0xff, 0xff, 0xff));
	m_varList.SetImageList(&m_imageList, LVSIL_SMALL);
//	ShowWindow(SW_SHOW);
}


void CStripDlg::DoDataExchange(CDataExchange* pDX)
{
	CModelessDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStripDlg)
	DDX_Control(pDX, IDC_LIST, m_varList);
	DDX_Control(pDX, IDC_STRIPCTRL, m_strip);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStripDlg, CModelessDlg)
	//{{AFX_MSG_MAP(CStripDlg)
	ON_WM_TIMER()
	ON_BN_CLICKED(ID_STRIPCHART_PROPERTIES, OnStripchartProperties)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStripDlg message handlers

void CStripDlg::OnTimer(UINT nIDEvent) 
{
//  	static double x = 0;
//  	x += 0.1;
//  	double y = 8 * sin(x) + 3 * cos(x*0.5);
//  	m_strip.AddY((short)0, y);
	
	CModelessDlg::OnTimer(nIDEvent);
}

void CStripDlg::OnStripchartProperties() 
{
	m_strip.ShowPropertyPage();
}

BOOL CStripDlg::OnInitDialog() 
{
	CModelessDlg::OnInitDialog();
	
	m_strip.SetHandles(3);
	m_strip.SetXSpan(10);
	m_strip.SetMaxBufferSize(600); // 20 Seconds at 30 FPS
	m_strip.SetTrackMax(200);
	m_strip.SetTrackMin(-10);
	m_strip.SetTrackDisplayMax(10);
	m_strip.SetTrackDisplayMin(-10);

	m_strip.SetVariables(20);
	for (int i = 0; i < 20; i++)
	{
		m_strip.SetVariableID(i);
		m_strip.SetVariableDeltaX(.034);
		m_strip.SetVariableColor(RGB(lineColor[i % NUM_LINE_COLORS][0], lineColor[i % NUM_LINE_COLORS][1], lineColor[i % NUM_LINE_COLORS][2]));
	}
	
	return TRUE;
}

//----------------------------------------------------------------------
// CStripDlg::AddVarWatch
//    Add a variable that can be viewed in the strip chart control.
//----------------------------------------------------------------------
void CStripDlg::AddVarWatch(VAR_ADD_WATCH& vaw)
{
	m_varList.InsertItem(vaw.index, vaw.name, vaw.index % NUM_LINE_COLORS);
}

//----------------------------------------------------------------------
// CStripDlg::UpdateVarWatch
//    This is called from the run loop of simloop and contains updated
//    values for all the variables that we want to see in the strip
//    chart.
//----------------------------------------------------------------------
void CStripDlg::UpdateVarWatch(VAR_UPDATE_WATCH& vuw)
{
	if (0 != vuw.x)
	{
		for (short i = 0; i < vuw.count; i++)
			m_strip.AddXY(i, vuw.x, vuw.vals[i]);
	}
	else
	{
		for (short i = 0; i < vuw.count; i++)
			m_strip.AddY(i, vuw.vals[i]);
	}
}

