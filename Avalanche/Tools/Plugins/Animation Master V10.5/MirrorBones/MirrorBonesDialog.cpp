// MirrorBonesDialog.cpp : implementation file
//

#include "stdafx.h"
//#include "Wizard.h"
#include "MirrorBonesDialog.h"
#include "F_Protos.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MirrorBonesDialog dialog

MirrorBonesDialog::MirrorBonesDialog(CWnd* pParent /*=NULL*/)
	: CDialog(MirrorBonesDialog::IDD, pParent)
//MirrorBonesDialog::MirrorBonesDialog()
//: CDialog(MirrorBonesDialog::IDD, CWnd::FromHandle(GetMainApplicationWnd()))
{
	//{{AFX_DATA_INIT(MirrorBonesDialog)
		// NOTE: the ClassWizard will add member initialization here
	m_adjustright = GetRegistryValue("MirrorBones HXT", "AdjustRight", TRUE);
	m_assignvertices = GetRegistryValue("MirrorBones HXT", "AssignVertices", FALSE);
	m_createbones = GetRegistryValue("MirrorBones HXT", "CreateBones", FALSE);
	m_generatelogfile = GetRegistryValue("MirrorBones HXT", "GenerateLogFile", FALSE);
	m_tolerance = GetRegistryValue("MirrorBones HXT", "PositionTolerance", 0.01f);
	//}}AFX_DATA_INIT
}


void MirrorBonesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_LEFT, (int &)m_adjustright);

	DDX_Check(pDX, IDC_ASSIGNVERTICES,m_assignvertices);
	DDX_Check(pDX, IDC_CREATEBONES,m_createbones);
	DDX_Check(pDX, IDC_GENERATELOGFILE, m_generatelogfile);

	DDX_Text(pDX, IDC_TOLERANCE, m_tolerance);
	DDV_MinMaxFloat(pDX, m_tolerance, 0.001f, 10.0f);

	//{{AFX_DATA_MAP(MirrorBonesDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(MirrorBonesDialog, CDialog)
	//{{AFX_MSG_MAP(MirrorBonesDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MirrorBonesDialog message handlers


BOOL MirrorBonesDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void MirrorBonesDialog::OnOK() 
{
	CDialog::OnOK();

	SetRegistryValue("MirrorBones HXT", "AdjustRight", m_adjustright); 
  	SetRegistryValue("MirrorBones HXT", "PositionTolerance", m_tolerance); 
	SetRegistryValue("MirrorBones HXT", "AssignVertices", m_assignvertices); 
	SetRegistryValue("MirrorBones HXT", "CreateBones", m_createbones); 
	SetRegistryValue("MirrorBones HXT", "GenerateLogFile", m_generatelogfile); 
}