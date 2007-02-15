// TupPropertyView

#include "stdafx.h"
#include "TupViewer.h"
#include "TupPropertyView.h"
#include "PropertyManager.h"
#include "PropertyTables.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// TupPropertyView dialog

TupPropertyView::TupPropertyView(CWnd* pParent /*=NULL*/)
: CExtResizableDialog(TupPropertyView::IDD, pParent)
{
	//{{AFX_DATA_INIT(TupPropertyView)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_uApplyFontID = 0;
	m_pPropertyManager = new PropertyManager(m_otTree);
	m_pPropertyManager->RegisterProperty("PropertyTableAggregate",PropertyTableAggregate);
	m_pPropertyManager->RegisterProperty("PropertyTableScalarInt",PropertyTableScalarInt);
	m_pPropertyManager->RegisterProperty("PropertyTableScalarFloat",PropertyTableScalarFloat);
	m_pPropertyManager->RegisterProperty("PropertyTableScalarString",PropertyTableScalarString);
	m_pPropertyManager->RegisterProperty("PropertyTableListInt",PropertyTableListInt);
	m_pPropertyManager->RegisterProperty("PropertyTableListFloat",PropertyTableListFloat);
	m_pPropertyManager->RegisterProperty("PropertyTableListString",PropertyTableListString);
	m_pPropertyManager->RegisterProperty("PropertyTableNode",PropertyTableNode);

}

TupPropertyView::~TupPropertyView()
{
	delete m_pPropertyManager;
}

void TupPropertyView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TupPropertyView)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(TupPropertyView, CDialog)
	//{{AFX_MSG_MAP(TupPropertyView)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_MESSAGE(OT_FS_NOTIFY_APPLY, WM_ApplyButton)
	ON_NOTIFY(OT_NOTIFY_ITEMCHANGED, IDC_PROPERTYVIEW_TREE, OnOptionItemChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TupPropertyView message handlers

BOOL TupPropertyView::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Declare variables
	COptionTreeItem *otiRoot = NULL;
	COptionTreeItem *otiItem = NULL;
	COptionTreeItemStatic *otiStatic = NULL;
	COptionTreeItemEdit *otiEdit = NULL;
	COptionTreeItemComboBox *otiCombo = NULL;
	COptionTreeItemCheckBox *otiCheck = NULL;
	COptionTreeItemRadio *otiRadio = NULL;
	COptionTreeItemSpinner *otiSpinner = NULL;
	COptionTreeItemColor *otiColor = NULL;
	COptionTreeItemDate *otiDate = NULL;
	COptionTreeItemImage *otiImage = NULL;
	COptionTreeItemFont *otiFont = NULL;
	COptionTreeItemFile *otiFile = NULL;
	COptionTreeItemIPAddress *otiIPAddress = NULL;
	COptionTreeItemHyperLink *otiHyperlink = NULL;
	COptionTreeItemList *otiList = NULL;
	CRect rcClient;
	DWORD dwStyle, dwOptions;
	LOGFONT lfFont, lfDefaultFont;


	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
//	SetIcon(m_hIcon, TRUE);			// Set big icon
//	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Get log fonts
	GetFont()->GetLogFont(&lfFont);
	GetFont()->GetLogFont(&lfDefaultFont);
	strcpy(lfDefaultFont.lfFaceName, _T("Arial"));

	// Get the clients rectangle
	GetClientRect(rcClient);

	// Setup the window style
	dwStyle = WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	// Setup the tree options 
	// OT_OPTIONS_SHOWINFOWINDOW
	dwOptions = OT_OPTIONS_SHADEEXPANDCOLUMN | OT_OPTIONS_SHADEROOTITEMS | OT_OPTIONS_SHOWINFOWINDOW;

	// Create tree options
	if (m_otTree.Create(dwStyle, rcClient, this, dwOptions, IDC_PROPERTYVIEW_TREE) == FALSE)
	{
		TRACE0("Failed to create options control.\r\n");
		return FALSE;
	}

	// Want to be notified
	m_otTree.SetNotify(TRUE, this);

	return TRUE;  // return TRUE  unless you set the focus to a control
}


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void TupPropertyView::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR TupPropertyView::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BOOL TupPropertyView::PreTranslateMessage(MSG* pMsg) 
{
	// Don't exit when press Enter of Esc
//	if(pMsg->message == WM_KEYDOWN)
//	{
//		if(pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
//		{
//			return TRUE;
//		}
//	}
//
	return CWnd::PreTranslateMessage(pMsg);
}

void TupPropertyView::OnSize(UINT nType, int cx, int cy) 
{
	// Resize the control
	if ((cx > 0) && (cy > 0))
	{
		if (::IsWindow(m_otTree.GetSafeHwnd()))
		{
			m_otTree.MoveWindow(0, 0, cx, cy);
		}
	}

	CDialog::OnSize(nType, cx, cy);	
}

LRESULT TupPropertyView::WM_ApplyButton(WPARAM wParam, LPARAM lParam)
{
	// Declare variables
	UINT uID;

	// Get incoming ID
	uID = (UINT) wParam;

	// See what ID it is
	if (uID == m_uApplyFontID)
	{
		OnApplyFont(lParam);
	}

	return 0;
}

void TupPropertyView::OnOptionItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	// Declare variables
	LPNMOPTIONTREE pNMOptionTree = (LPNMOPTIONTREE)pNotifyStruct;

	// Validate
	if (pNMOptionTree == NULL)
	{
		return; //1;
	}
	m_pPropertyManager->UpdateFromOptions(pNMOptionTree->otiItem);
//	int fileBufferIndex = theApp.m_pSelectedAtom->m_bufferIndex;
//	TRACE(Selected (*theApp.m_pTupBuffers)[fileBufferIndex].m_filename

	return; // 0;
}

void TupPropertyView::OnApplyFont(LPARAM lParam)
{
	// Note depending on if you are usng OT_FS_USECHARFORMAT or not
	// for this item a LOGFONT or CHARFORMAT font is sent.

	// Declare variables
	PLOGFONTA lf;

	// Get font
	lf = (PLOGFONTA) lParam;

	MessageBox("Include apply code to be able to accept font chages live.", "COptionTreeDemo", MB_OK);
}

void TupPropertyView::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{		
	// Set minimum and maximum dimensions
	lpMMI->ptMinTrackSize.x = 425;
	lpMMI->ptMinTrackSize.y = 450;

	CDialog::OnGetMinMaxInfo(lpMMI);
}

void TupPropertyView::Refresh(void)
{
	BuildProperties(theApp.m_pSelectedAtom);
	if (theApp.m_pSelectedAtom)
	{
		TRACE(_T("Starting Line #%d\n"),theApp.m_pSelectedAtom->GetStartingLineNum());
		TRACE(_T("Ending Line #%d\n"),theApp.m_pSelectedAtom->GetEndingLineNum());
		TRACE(_T("Starting Char Pos #%d\n"),theApp.m_pSelectedAtom->GetStartingCharPos());
		TRACE(_T("Ending Line #%d\n"),theApp.m_pSelectedAtom->GetEndingCharPos());
	}
	m_otTree.Invalidate();
}

void TupPropertyView::BuildProperties(TupperwareAtom *pAtom)
{
	if (!pAtom)
		return;

	switch(pAtom->GetAtomType())
	{
		case TupperwareAtom::ATOM_AGGREGATE:
			{
				if (!stricmp(pAtom->GetKey(),"Node"))
				{
					m_pPropertyManager->BuildProperties(pAtom,"PropertyTableNode");
				}
				else
				{
					m_pPropertyManager->BuildProperties(pAtom,"PropertyTableAggregate");
				}
			}
			break;
		case TupperwareAtom::ATOM_SCALAR:
			{
				TupperwareScalar *pScalar = (TupperwareScalar *)pAtom;
				switch (pScalar->GetDataType())
				{
					case TupperwareScalar::DATA_INT:
						{
							m_pPropertyManager->BuildProperties(pAtom,"PropertyTableScalarInt");
						}
						break;
					case TupperwareScalar::DATA_FLOAT:
						{
							m_pPropertyManager->BuildProperties(pAtom,"PropertyTableScalarFloat");
						}
						break;
					case TupperwareScalar::DATA_STRING:
						{
							m_pPropertyManager->BuildProperties(pAtom,"PropertyTableScalarString");
						}
						break;
					case TupperwareScalar::DATA_BINARY:
						{
							//m_pPropertyManager->BuildProperties(pAtom,"PropertyTableScalarInt");
						}
						break;
				}
			}
			break;
		case TupperwareAtom::ATOM_LIST:
			{
				TupperwareList *pList = (TupperwareList *)pAtom;
				switch (pList->GetDataType())
				{
					case TupperwareList::DATA_INT:
						{
							m_pPropertyManager->BuildProperties(pAtom,"PropertyTableListInt");
						}
						break;
					case TupperwareList::DATA_FLOAT:
						{
							m_pPropertyManager->BuildProperties(pAtom,"PropertyTableListFloat");
						}
						break;
					case TupperwareList::DATA_STRING:
						{
							m_pPropertyManager->BuildProperties(pAtom,"PropertyTableListString");
						}
						break;
				}
			}
			break;
	}
}

