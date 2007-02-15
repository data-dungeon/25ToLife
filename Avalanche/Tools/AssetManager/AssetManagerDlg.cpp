//-----------------------------------------------------------------------------
//	File:		AssetManagerDlg.cpp
//	Created:	Sep 13, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The CAssetManagerDlg class defines behavior for the main
//			application dialog.
//
//	History:	9/13/2002: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Includes:

#include "stdafx.h"
#include "AssetManager.h"
#include "AssetManagerDlg.h"
#include "Registry.h"
#include "AssetManagerTreeCtrl.h"
#include "AMSyncTreeFactory.h"
#include "AMSyncTreeNodeFolder.h"
#include "AMSyncTreeNodeFile.h"
#include "AMLoadDBFeedback.h"
#include "LoadDBDialog.h"
#include "AMSyncFeedback.h"
#include "SyncDialog.h"
#include "SyncConfirmDialog.h"
#include "OptionsDlg.h"
#include "AssociatedIcon.h"
#include "RegistryKeys.h"
#include "FolderSets.h"
#include "OpenDatabaseDialog.h"
#include "UpdateDuplicatesDialog.h"
#include "BuildImageList.h"
#include "PPTooltip.h"
//-----------------------------------------------------------------------------
// Defines, etc.:

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Image indexes:
#define IMG_NORM_IDX_FOLDER					0
#define IMG_NORM_IDX_FOLDER_FROZEN_PATH	1
#define IMG_NORM_IDX_FILE_FROZEN_PATH		2
#define IMG_NORM_IDX_BLANK						3

#define IMG_IND_IDX_COPY_LEFT		0
#define IMG_IND_IDX_COPY_RIGHT	5
#define IMG_IND_IDX_DEL_LEFT		10
#define IMG_IND_IDX_DEL_RIGHT		15
#define IMG_IND_IDX_DEL_BOTH		20
#define IMG_IND_IDX_EXCLUDED		25
#define IMG_IND_IDX_FROZEN			30
#define IMG_IND_IDX_EXCLUDED_LONE 35
#define IMG_IND_IDX_CHILD_ACTION	36

#define IMG_IND_IDX_LEFT_ONLY		0
#define IMG_IND_IDX_RIGHT_ONLY	1
#define IMG_IND_IDX_LEFT_NEWER	2
#define IMG_IND_IDX_RIGHT_NEWER	3
#define IMG_IND_IDX_EQUAL			4


// Misc:
#define LEFT_TREE_DELETE_TEXT_BK_COLOR RGB(255,250,233)
#define LEFT_TREE_COPY_SOURCE_TEXT_BK_COLOR RGB(238,255,237)
#define LEFT_TREE_COPY_DEST_TEXT_BK_COLOR RGB(237,237,255)
#define RIGHT_TREE_DELETE_TEXT_BK_COLOR RGB(255,236,233)
#define RIGHT_TREE_COPY_SOURCE_TEXT_BK_COLOR RGB(237,237,255)
#define RIGHT_TREE_COPY_DEST_TEXT_BK_COLOR RGB(238,255,237)
#define FROZEN_TEXT_COLOR RGB(64,200,200)


//-----------------------------------------------------------------------------
// CAboutDlg class declaration and definition:

class CAboutDlg : public CDialog
{
public:
	// Constructors and destructors:
	CAboutDlg();

	// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CString	m_versionString;
	CString	m_dateString;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation

protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


//-----------------------------------------------------------------------------
// CAboutDlg::CAboutDlg
// This is the default constructor for this class.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	char tempText[80];
	sprintf(tempText,"Version %s",g_pAssetManagerVersion);
	m_versionString = tempText;
	
	sprintf(tempText,"Build %s %s",g_pAssetManagerDate,g_pAssetManagerTime);
	m_dateString = tempText;
	
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}


//-----------------------------------------------------------------------------
// CAboutDlg::DoDataExchange
// This method can be called by the framework to exchange and validate data.
// Never call this function directly.
//
// Input:
//  CDataExchange* pDX -- A pointer to a CDataExchange object.
//
// Output: None.
//-----------------------------------------------------------------------------
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_STATIC_VERSION, m_versionString);
	DDX_Text(pDX, IDC_STATIC_DATE, m_dateString);
	//}}AFX_DATA_MAP
}


//-----------------------------------------------------------------------------
// CAboutDlg message map:

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



//-----------------------------------------------------------------------------
// CAssetManagerDlg class definition:


//-----------------------------------------------------------------------------
// CAssetManagerDlg::CAssetManagerDlg
// This method is the default constructor for this class.
//
// Input:
//	CWnd* pParent -- CWnd pointer to this dialog's parent.
//
// Output: None.
//-----------------------------------------------------------------------------
CAssetManagerDlg::CAssetManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAssetManagerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAssetManagerDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_bClosing = false;
	m_bRefreshDatabases = false;

	m_mode = PRECOMPARE;
	m_bContextFromLeft = true;

	// Create the factory for creating the databases:
	m_pDBFactory = new SyncDBFactory;

	// Create a sync tree factory and a sync tree manager to tie the databases together:
	m_pAMSyncTreeFactory = new AMSyncTreeFactory;
	m_pSyncTreeManager = new SyncTreeManager(*m_pAMSyncTreeFactory);
	m_pSyncTreeManager->SetAutoExclude(&m_autoExclude);
	m_pUpdateDuplicatesManager = NULL;

	// Set the databases to null here:
	m_pLeftDataBase = NULL;			
	m_pRightDataBase = NULL;			

	// no load db dialog
	m_pLoadDBDialog = NULL;
	m_pFeedbackThread = NULL;
	// no sync dialog
	m_pSyncDialog = NULL;
	m_syncEnable.m_bLRCopyEnable = false;
	m_syncEnable.m_bRLCopyEnable = false;
	m_syncEnable.m_bLDeleteEnable = false;
	m_syncEnable.m_bRDeleteEnable = false;

	m_pToolTip = NULL;
	m_pDataTip = NULL;

	m_pFolderSets = NULL;
	m_pLogFile = NULL;

	m_bAutoPressCopyToRight = false;
	m_bAutoPressCopyToLeft = false;
	m_bAutoPressDeleteRight = false;
	m_bAutoPressDeleteLeft = false;
	m_bAutoPressSync = false;
	m_overrideSyncStyle = -1;
}


//-----------------------------------------------------------------------------
// CAssetManagerDlg::~CAssetManagerDlg
// This method is the destructor for this dialog class.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
CAssetManagerDlg::~CAssetManagerDlg()
{
	delete m_pToolTip;
	delete m_pDataTip;

	// Delete any member pointers:
	if (m_pTreeCtrlLeft != NULL)
	{
		delete m_pTreeCtrlLeft;
		m_pTreeCtrlLeft = NULL;
	}
	if (m_pTreeCtrlRight != NULL)
	{
		delete m_pTreeCtrlRight;
		m_pTreeCtrlRight = NULL;
	}
	if (m_pTreeCtrlIndicators != NULL)
	{
		delete m_pTreeCtrlIndicators;
		m_pTreeCtrlIndicators = NULL;
	}
	if (m_pAssociatedIconManager != NULL)
	{
		delete m_pAssociatedIconManager;
		m_pAssociatedIconManager = NULL;
	}
	if (m_pDBFactory != NULL)
	{
		delete m_pDBFactory;
		m_pDBFactory = NULL;
	}
	if (m_pAMSyncTreeFactory != NULL)
	{
		delete m_pAMSyncTreeFactory;
		m_pAMSyncTreeFactory = NULL;
	}
	if (m_pSyncTreeManager != NULL)
	{
		delete m_pSyncTreeManager;
		m_pSyncTreeManager = NULL;
	}
	if (m_pLeftDataBase != NULL)
	{
		delete m_pLeftDataBase;
		m_pLeftDataBase = NULL;
	}
	if (m_pRightDataBase != NULL)
	{
		delete m_pRightDataBase;
		m_pRightDataBase = NULL;
	}
	if (m_pFolderSets != NULL)
	{
		delete m_pFolderSets;
		m_pFolderSets = NULL;
	}
	if (m_pLogFile != NULL)
	{
		delete m_pLogFile;
		m_pLogFile = NULL;
	}
}


//-----------------------------------------------------------------------------
// CAssetManagerDlg::DoDataExchange
// This method can be called by the framework to exchange and validate data.
// Never call this function directly.
//
// Input:
//  CDataExchange* pDX -- A pointer to a CDataExchange object.
//
// Output: None.
//-----------------------------------------------------------------------------
void CAssetManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAssetManagerDlg)
	DDX_Control(pDX, IDC_FRAME1, m_syncBitmapFrame);
	DDX_Control(pDX, IDC_SYNC_STYLE_TITLE, m_syncStyleTitle);
	DDX_Control(pDX, IDC_SYNC_STYLE, m_syncStyleCombo);
	DDX_Control(pDX, IDC_BUTTON_FOLDER_SETS_BROWSE, m_bttnFolderSetsBrowse);
	DDX_Control(pDX, IDC_BUTTON_FOLDER_SETS_SORT, m_bttnFolderSetsSort);
	DDX_Control(pDX, IDC_FOLDER_SETS_TITLE, m_folderSetsTitle);
	DDX_Control(pDX, IDC_FOLDER_SETS, m_folderSetsCombo);
	DDX_Control(pDX, IDC_FOLDER_PATH_RIGHT, m_folderPathRight);
	DDX_Control(pDX, IDC_FOLDER_PATH_LEFT, m_folderPathLeft);
	DDX_Control(pDX, IDC_LOCK_FROZEN_BITMAP, m_lockFrozenBitmap);
	DDX_Control(pDX, IDC_LOCK_FROZEN, m_lockFrozenCheckbox);
	DDX_Control(pDX, IDC_LOCK_EXCLUDED_BITMAP, m_lockExcludedBitmap);
	DDX_Control(pDX, IDC_LOCK_EXCLUDED, m_lockExcludedCheckbox);
	DDX_Control(pDX, IDC_BTTN_VIEW_FROZEN, m_bttnViewFrozen);
	DDX_Control(pDX, IDC_RL_COPY_BUTTON, m_rlCopyButton);
	DDX_Control(pDX, IDC_L_DELETE, m_lDeleteButton);
	DDX_Control(pDX, IDC_R_DELETE, m_rDeleteButton);
	DDX_Control(pDX, IDC_LR_COPY_BUTTON, m_lrCopyButton);
	DDX_Control(pDX, IDC_SYNCBUTTON, m_syncButton);
	DDX_Control(pDX, IDC_BTTN_REFRESH, m_bttnRefresh);
	DDX_Control(pDX, IDC_BTTN_VIEW_ALL, m_bttnViewAll);
	DDX_Control(pDX, IDC_BTTN_VIEW_CPY_LEFT, m_bttnViewCpyLeft);
	DDX_Control(pDX, IDC_BTTN_VIEW_CPY_RIGHT, m_bttnViewCpyRight);
	DDX_Control(pDX, IDC_BTTN_VIEW_DLT_BOTH, m_bttnViewDltBoth);
	DDX_Control(pDX, IDC_BTTN_VIEW_DLT_LEFT, m_bttnViewDltLeft);
	DDX_Control(pDX, IDC_BTTN_VIEW_DLT_RIGHT, m_bttnViewDltRight);
	DDX_Control(pDX, IDC_BTTN_VIEW_EQUAL, m_bttnViewEqual);
	DDX_Control(pDX, IDC_BTTN_VIEW_NEWER_LEFT, m_bttnViewNewerLeft);
	DDX_Control(pDX, IDC_BTTN_VIEW_NEWER_RIGHT, m_bttnViewNewerRight);
	DDX_Control(pDX, IDC_BTTN_VIEW_ORPHAN_LEFT, m_bttnViewOrphanLeft);
	DDX_Control(pDX, IDC_BTTN_VIEW_ORPHAN_RIGHT, m_bttnViewOrphanRight);
	DDX_Control(pDX, IDC_BTTN_VIEW_EXCLUDED, m_bttnViewExcluded);
	DDX_Control(pDX, IDC_RIGHT_STATS_TEXT4, m_rightStatsText4);
	DDX_Control(pDX, IDC_RIGHT_STATS_TEXT3, m_rightStatsText3);
	DDX_Control(pDX, IDC_RIGHT_STATS_TEXT2, m_rightStatsText2);
	DDX_Control(pDX, IDC_RIGHT_STATS_TEXT1, m_rightStatsText1);
	DDX_Control(pDX, IDC_LEFT_STATS_TEXT4, m_leftStatsText4);
	DDX_Control(pDX, IDC_LEFT_STATS_TEXT3, m_leftStatsText3);
	DDX_Control(pDX, IDC_LEFT_STATS_TEXT2, m_leftStatsText2);
	DDX_Control(pDX, IDC_LEFT_STATS_TEXT1, m_leftStatsText1);
	//}}AFX_DATA_MAP
}


//-----------------------------------------------------------------------------
// CAssetManagerDlg message map:

BEGIN_MESSAGE_MAP(CAssetManagerDlg, CDialog)
	//{{AFX_MSG_MAP(CAssetManagerDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_COMMAND(ID_DEFAULT_OPTIONS, OnDefaultOptions)
	ON_COMMAND(ID_FOLDER_SET_OPTIONS, OnFolderSetOptions)
	ON_COMMAND(ID_COPY_TO_LEFT, OnContextCopyToLeft)
	ON_COMMAND(ID_COPY_TO_RIGHT, OnContextCopyToRight)
	ON_COMMAND(ID_DELETE_FROM_LEFT, OnContextDeleteFromLeft)
	ON_COMMAND(ID_DELETE_FROM_RIGHT, OnContextDeleteFromRight)
	ON_COMMAND(ID_DELETE_FROM_BOTH, OnContextDeleteFromBoth)
	ON_COMMAND(ID_EXCLUDE, OnContextExclude)
	ON_COMMAND(ID_SET_DEFAULT, OnContextSetDefault)
	ON_COMMAND(ID_SELECT_ALL, OnContextSelectAll)
	ON_COMMAND(ID_SELECT_NONE, OnContextSelectNone)
	ON_COMMAND(ID_INVERT_SELECTION, OnContextInvertSelection)
	ON_COMMAND(ID_FREEZE, OnContextFreeze)
	ON_COMMAND(ID_UNFREEZE, OnContextUnFreeze)
	ON_COMMAND(ID_UNFREEZE_ONLY, OnContextUnFreezeOnly)
	ON_COMMAND(ID_EXPAND_ALL, OnContextExpandAll)
	ON_COMMAND(ID_COLLAPSE_ALL, OnContextCollapseAll)
	ON_COMMAND(ID_ADD_PREFIX, OnContextAddPrefix)
	ON_COMMAND(ID_REMOVE_PREFIX, OnContextRemovePrefix)
	ON_COMMAND(ID_RENAME, OnContextRename)
	ON_COMMAND(ID_UPDATE_DUPLICATES, OnContextUpdateDuplicates)
	ON_COMMAND(ID_EXPLORE, OnContextExplore)
	ON_BN_CLICKED(IDC_SYNCBUTTON, OnSyncbutton)
	ON_BN_CLICKED(IDC_L_DELETE, OnLDelete)
	ON_BN_CLICKED(IDC_LR_COPY_BUTTON, OnLrCopyButton)
	ON_BN_CLICKED(IDC_R_DELETE, OnRDelete)
	ON_BN_CLICKED(IDC_RL_COPY_BUTTON, OnRlCopyButton)
	ON_BN_CLICKED(IDC_BTTN_VIEW_CPY_LEFT, OnBttnViewCpyLeft)
	ON_BN_CLICKED(IDC_BUTTON_FOLDER_SETS_SORT, OnFolderSetsSort)
	ON_WM_GETMINMAXINFO()
	ON_BN_CLICKED(IDC_BTTN_REFRESH, OnBttnRefresh)
	ON_BN_CLICKED(IDC_BTTN_VIEW_ALL, OnBttnViewAll)
	ON_BN_CLICKED(IDC_BTTN_VIEW_CPY_RIGHT, OnBttnViewCpyRight)
	ON_BN_CLICKED(IDC_BTTN_VIEW_DLT_BOTH, OnBttnViewDltBoth)
	ON_BN_CLICKED(IDC_BTTN_VIEW_DLT_LEFT, OnBttnViewDltLeft)
	ON_BN_CLICKED(IDC_BTTN_VIEW_DLT_RIGHT, OnBttnViewDltRight)
	ON_BN_CLICKED(IDC_BTTN_VIEW_EQUAL, OnBttnViewEqual)
	ON_BN_CLICKED(IDC_BTTN_VIEW_NEWER_LEFT, OnBttnViewNewerLeft)
	ON_BN_CLICKED(IDC_BTTN_VIEW_NEWER_RIGHT, OnBttnViewNewerRight)
	ON_BN_CLICKED(IDC_BTTN_VIEW_ORPHAN_LEFT, OnBttnViewOrphanLeft)
	ON_BN_CLICKED(IDC_BTTN_VIEW_ORPHAN_RIGHT, OnBttnViewOrphanRight)
	ON_BN_CLICKED(IDC_BTTN_VIEW_EXCLUDED, OnBttnViewExcluded)
	ON_BN_CLICKED(IDC_BTTN_VIEW_FROZEN, OnBttnViewFrozen)
	ON_BN_CLICKED(IDC_LOCK_EXCLUDED, OnLockExcluded)
	ON_BN_CLICKED(IDC_LOCK_FROZEN, OnLockFrozen)
	ON_BN_CLICKED(IDC_BUTTON_FOLDER_SETS_BROWSE, OnFolderSetsBrowse)
	ON_CBN_SELCHANGE(IDC_FOLDER_SETS, OnSelchangeFolderSets)
	ON_COMMAND(ID_ABOUT, OnAbout)
	ON_COMMAND(ID_AUTO_FREEZE_ADD, OnContextAutoFreezeAdd)
	ON_COMMAND(ID_AUTO_FREEZE_REMOVE, OnContextAutoFreezeRemove)
	ON_COMMAND(ID_AUTO_FREEZE_REMOVE_INCLUDE_CHILDREN, OnContextAutoFreezeRemoveIncludeChildren)
	ON_UPDATE_COMMAND_UI(ID_SET_DEFAULT, OnUpdateSetDefault)
	ON_UPDATE_COMMAND_UI(ID_EXCLUDE, OnUpdateExclude)
	ON_UPDATE_COMMAND_UI(ID_COPY_TO_LEFT, OnUpdateCopyToLeft)
	ON_UPDATE_COMMAND_UI(ID_COPY_TO_RIGHT, OnUpdateCopyToRight)
	ON_UPDATE_COMMAND_UI(ID_FREEZE, OnUpdateFreeze)
	ON_UPDATE_COMMAND_UI(ID_UNFREEZE, OnUpdateUnfreeze)
	ON_UPDATE_COMMAND_UI(ID_UNFREEZE_ONLY, OnUpdateUnfreezeOnly)
	ON_UPDATE_COMMAND_UI(ID_AUTO_FREEZE_ADD, OnUpdateAutoFreezeAdd)
	ON_UPDATE_COMMAND_UI(ID_DELETE_FROM_LEFT, OnUpdateDeleteFromLeft)
	ON_UPDATE_COMMAND_UI(ID_DELETE_FROM_RIGHT, OnUpdateDeleteFromRight)
	ON_UPDATE_COMMAND_UI(ID_DELETE_FROM_BOTH, OnUpdateDeleteFromBoth)
	ON_UPDATE_COMMAND_UI(ID_SELECT_ALL, OnUpdateSelectAll)
	ON_UPDATE_COMMAND_UI(ID_SELECT_NONE, OnUpdateSelectNone)
	ON_UPDATE_COMMAND_UI(ID_INVERT_SELECTION, OnUpdateInvertSelection)
	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAll)
	ON_UPDATE_COMMAND_UI(ID_COLLAPSE_ALL, OnUpdateCollapseAll)
	ON_UPDATE_COMMAND_UI(ID_ADD_PREFIX, OnUpdateAddPrefix)
	ON_UPDATE_COMMAND_UI(ID_REMOVE_PREFIX, OnUpdateRemovePrefix)
	ON_UPDATE_COMMAND_UI(ID_RENAME, OnUpdateRename)
	ON_UPDATE_COMMAND_UI(ID_UPDATE_DUPLICATES, OnUpdateUpdateDuplicates)
	ON_UPDATE_COMMAND_UI(ID_EXPLORE, OnUpdateExplore)
	ON_CBN_SELCHANGE(IDC_SYNC_STYLE, OnSelchangeSyncStyle)
	ON_WM_VSCROLL()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_FOLDER_SETS, OnFolderSetsBrowse)
	ON_COMMAND(ID_HELP_CONTENTS, OnHelpContents)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
// CAssetManagerDlg::SetMode
// This mutator will set the current mode for the application.
//
// Input:
//  Mode newMode -- The new mode to set.
//
// Output: None.
//-----------------------------------------------------------------------------
void CAssetManagerDlg::SetMode(Mode newMode)
{
	m_mode = newMode;
}


//-----------------------------------------------------------------------------
// CAssetManagerDlg::GetMode
// This accessor will return the current mode for the application.
//
// Input: None.
//
// Output: The current mode. 
//-----------------------------------------------------------------------------
CAssetManagerDlg::Mode CAssetManagerDlg::GetMode()
{
	return m_mode;
}


//-----------------------------------------------------------------------------
// CAssetManagerDlg::OnInitDialog
// This handler is called in response to the WM_INITDIALOG message.  This method
// will perform any special processing when the dialog is initialized.
//
// Input: None.
//
// Output: TRUE.
//-----------------------------------------------------------------------------
BOOL CAssetManagerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	Registry registry(ASSET_MANAGER_KEY);
	DWORD regInt;

	// Set the icon for this dialog.  The framework does this automatically
	// when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Set up the buttons dealing with syncing:
	m_hSync = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SYNC), IMAGE_BITMAP,0,0,LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
	m_hSyncDisabled = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SYNC_DISABLED), IMAGE_BITMAP,0,0,LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
	m_hCopyToLeft = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_COPYTOLEFT), IMAGE_BITMAP,0,0,LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
	m_hCopyToLeftDisabled = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_COPYTOLEFT_DISABLED), IMAGE_BITMAP,0,0,LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
	m_hCopyToRight = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_COPYTORIGHT), IMAGE_BITMAP,0,0,LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
	m_hCopyToRightDisabled = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_COPYTORIGHT_DISABLED), IMAGE_BITMAP,0,0,LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
	m_hDeleteLeft = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_DELETELEFT), IMAGE_BITMAP,0,0,LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
	m_hDeleteLeftDisabled = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_DELETELEFT_DISABLED), IMAGE_BITMAP,0,0,LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
	m_hDeleteRight = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_DELETERIGHT), IMAGE_BITMAP,0,0,LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
	m_hDeleteRightDisabled = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_DELETERIGHT_DISABLED), IMAGE_BITMAP,0,0,LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
	m_syncButton.SetBitmap(m_hSyncDisabled);
	m_lrCopyButton.SetBitmap(m_hCopyToRightDisabled);
	m_lDeleteButton.SetBitmap(m_hDeleteLeftDisabled);
	m_rlCopyButton.SetBitmap(m_hCopyToLeftDisabled);
	m_rDeleteButton.SetBitmap(m_hDeleteRightDisabled);
	m_lrCopyButton.SetState((int)m_syncEnable.m_bLRCopyEnable);
	m_rlCopyButton.SetState((int)m_syncEnable.m_bRLCopyEnable);
	m_lDeleteButton.SetState((int)m_syncEnable.m_bLDeleteEnable);
	m_rDeleteButton.SetState((int)m_syncEnable.m_bRDeleteEnable);

	// Set up the view filter buttons:
	m_hBmapRefresh = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BTTN_REFRESH), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
	m_hBmapViewAll = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BTTN_VIEW_ALL), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
	m_hBmapViewCpyLeft = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BTTN_VIEW_CPY_LEFT), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
	m_hBmapViewCpyRight = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BTTN_VIEW_CPY_RIGHT), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
	m_hBmapViewDltBoth = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BTTN_VIEW_DLT_BOTH), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
	m_hBmapViewDltLeft = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BTTN_VIEW_DLT_LEFT), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
	m_hBmapViewDltRight = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BTTN_VIEW_DLT_RIGHT), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
	m_hBmapViewEqual = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BTTN_VIEW_EQUAL), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
	m_hBmapViewNewerLeft = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BTTN_VIEW_NEWER_LEFT), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
	m_hBmapViewNewerRight = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BTTN_VIEW_NEWER_RIGHT), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
	m_hBmapViewOrphanLeft = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BTTN_VIEW_ORPHAN_LEFT), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
	m_hBmapViewOrphanRight = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BTTN_VIEW_ORPHAN_RIGHT), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
	m_hBmapViewExcluded = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BTTN_VIEW_EXCLUDED), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
	m_hBmapViewExcludedDisabled = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BTTN_VIEW_EXCLUDED_DISABLED), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
	m_hBmapViewFrozen = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BTTN_VIEW_FROZEN), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
	m_hBmapViewFrozenDisabled = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BTTN_VIEW_FROZEN_DISABLED), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
	m_hBmapLock= (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_LOCK), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);
	m_hIconBrowse = (HICON)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_BROWSE), IMAGE_ICON, 0, 0, 0);
	m_hIconSort = (HICON)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_SORT), IMAGE_ICON, 0, 0, 0);
	
	m_bttnRefresh.SetBitmap(m_hBmapRefresh);
	m_bttnViewAll.SetBitmap(m_hBmapViewAll);
	m_bttnViewCpyLeft.SetBitmap(m_hBmapViewCpyLeft);
	m_bttnViewCpyRight.SetBitmap(m_hBmapViewCpyRight);
	m_bttnViewDltBoth.SetBitmap(m_hBmapViewDltBoth);
	m_bttnViewDltLeft.SetBitmap(m_hBmapViewDltLeft);
	m_bttnViewDltRight.SetBitmap(m_hBmapViewDltRight);
	m_bttnViewEqual.SetBitmap(m_hBmapViewEqual);
	m_bttnViewNewerLeft.SetBitmap(m_hBmapViewNewerLeft);
	m_bttnViewNewerRight.SetBitmap(m_hBmapViewNewerRight);
	m_bttnViewOrphanLeft.SetBitmap(m_hBmapViewOrphanLeft);
	m_bttnViewOrphanRight.SetBitmap(m_hBmapViewOrphanRight);
	m_bttnViewExcluded.SetBitmap(m_hBmapViewExcluded);
	m_bttnViewFrozen.SetBitmap(m_hBmapViewFrozen);
	m_lockFrozenBitmap.SetBitmap(m_hBmapLock);
	m_lockExcludedBitmap.SetBitmap(m_hBmapLock);
	m_bttnFolderSetsBrowse.SetIcon(m_hIconBrowse);
	m_bttnFolderSetsSort.SetIcon(m_hIconSort);

	// turn on all buttons
	m_viewFlags = SyncTreeNodeFlags::VIEW_FLAGS_ALL|
				  SyncTreeNodeFlags::VIEW_FLAGS_STATUS_ACTION|
				  SyncTreeNodeFlags::VIEW_FLAGS_ACTION_FROZEN|
				  SyncTreeNodeFlags::VIEW_FLAGS_ACTION_EXCLUDE;

	if (registry.Get(EXCLUDE_LOCK_FLAG_FIELD,&regInt) && (regInt==1))
	{
		m_viewFlags |= SyncTreeNodeFlags::VIEW_FLAGS_EXCLUDE_LOCKED;
		if (registry.Get(VIEW_EXCLUDE_FLAG_FIELD,&regInt) && (regInt==0))
		{
			m_viewFlags &= ~SyncTreeNodeFlags::VIEW_FLAGS_ACTION_EXCLUDE; // clear if set to 0
		}
	}
	if (registry.Get(FROZEN_LOCK_FLAG_FIELD,&regInt) && (regInt==1))
	{
		m_viewFlags |= SyncTreeNodeFlags::VIEW_FLAGS_FROZEN_LOCKED;
		if (registry.Get(VIEW_FROZEN_FLAG_FIELD,&regInt) && (regInt==0))
		{
			m_viewFlags &= ~SyncTreeNodeFlags::VIEW_FLAGS_ACTION_FROZEN; // clear if set to 0
		}
	}

	if (registry.Get(SORT_FOLDER_SETS_FIELD,&regInt))
	{
		m_bSortFolderSets = (regInt == 1);
	}
	else
	{
		m_bSortFolderSets = true;
	}
	m_bttnFolderSetsSort.SetState((int)m_bSortFolderSets);


	m_folderPathLeft.SetWindowText("");
	m_folderPathRight.SetWindowText("");
	
	UpdateViewButtons();

	// Set up the status bar:
	CRect rectWindowDlg;
	GetWindowRect(&rectWindowDlg);
	rectWindowDlg.top = rectWindowDlg.bottom - 25;

	m_dialogRect.SetRectEmpty();
	if (registry.Get(DIALOG_RECT_TOP_FIELD,&regInt))
		m_dialogRect.top = (LONG)regInt;
	if (registry.Get(DIALOG_RECT_BOTTOM_FIELD,&regInt))
		m_dialogRect.bottom = (LONG)regInt;
	if (registry.Get(DIALOG_RECT_LEFT_FIELD,&regInt))
		m_dialogRect.left = (LONG)regInt;
	if (registry.Get(DIALOG_RECT_RIGHT_FIELD,&regInt))
		m_dialogRect.right = (LONG)regInt;

	char excludeBuffer[512];
	TupString autoExcludeString;

	if (registry.Get(AUTO_EXCLUDE_FILE_EXTENSIONS_FIELD,&excludeBuffer,512))
	{
		m_defaultAutoFreezeFileExtensions = excludeBuffer;
	}
	else
	{
		m_defaultAutoFreezeFileExtensions = ".tmp;.bak;.pix;.ava;.attr;.iff;.log;.mel";
	}
	if (registry.Get(AUTO_EXCLUDE_FILE_PREFIXES_FIELD,&excludeBuffer,512))
	{
		m_defaultAutoFreezeFilePrefixes = excludeBuffer;
	}
	else
	{
		m_defaultAutoFreezeFilePrefixes = "_;buttmap;devicemap";
	}
	if (registry.Get(AUTO_EXCLUDE_FOLDER_PREFIXES_FIELD,&excludeBuffer,512))
	{
		m_defaultAutoFreezeFolderPrefixes = excludeBuffer;
	}
	else
	{
		m_defaultAutoFreezeFolderPrefixes = "_;.mayaSwatches;cache;Latest";
	}

	if (registry.Get(UPDATE_DUPLICATES_INCLUDE_FROZEN_DUPLICATES,&regInt))
	{
		m_updateDuplicatesIncludeFrozenDuplicates = regInt;
	}
	else
	{
		m_updateDuplicatesIncludeFrozenDuplicates = TRUE;
	}

	if (registry.Get(UPDATE_DUPLICATES_INCLUDE_PARENT_NAMES,&regInt))
	{
		m_updateDuplicatesIncludeParentNames = regInt;
	}
	else
	{
		m_updateDuplicatesIncludeParentNames = TRUE;
	}

	if (m_dialogRect.IsRectEmpty())
	{
		GetWindowRect(&m_dialogRect);
	}
	CRect offsetRect(0,0,0,GetSystemMetrics(SM_CYCAPTION) - m_dialogRect.Height());
	MakeRectOnScreen(m_dialogRect,offsetRect);
	MoveWindow(&m_dialogRect);

	// Set up the tree controls:
	SetUpTreeControls();

	SetupToolTips();

	// Now update the window so it's displayed with child windows on top of it:
	ShowWindow(TRUE);
	UpdateWindow();

	m_pFolderSets = NULL;


	char nameBuffer[512];
	DWORD maxSize = 512;
	GetUserName(nameBuffer,&maxSize);
	m_userName = nameBuffer;
	GetComputerName(nameBuffer,&maxSize);
	m_computerName = nameBuffer;
	m_pLogFile = new LogFile("AssetManager.log",m_userName,m_computerName);
	
	// Set up the folder sets
	if (SetUpFolderSets())
	{
		// Load the databases:
		LoadDatabases();
		if (m_bAutoPressCopyToRight) OnLrCopyButton();
		if (m_bAutoPressCopyToLeft) OnRlCopyButton();
		if (m_bAutoPressDeleteRight) OnRDelete();
		if (m_bAutoPressDeleteLeft) OnLDelete();
		if (m_bAutoPressSync) OnSyncbutton();
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAssetManagerDlg::MakeRectOnScreen(CRect &windowRect,const CRect &offsetRect)
{
	CRect testRect(windowRect.left+offsetRect.left,windowRect.top+offsetRect.top,windowRect.right+offsetRect.right,windowRect.bottom+offsetRect.bottom);

	// test four points and if any are onscreen then we are ok
	HMONITOR hMonitorTL = MonitorFromPoint(CPoint(testRect.left,testRect.top),MONITOR_DEFAULTTONULL);
	HMONITOR hMonitorTR = MonitorFromPoint(CPoint(testRect.right,testRect.top),MONITOR_DEFAULTTONULL);
	HMONITOR hMonitorBL = MonitorFromPoint(CPoint(testRect.left,testRect.bottom),MONITOR_DEFAULTTONULL);
	HMONITOR hMonitorBR = MonitorFromPoint(CPoint(testRect.right,testRect.bottom),MONITOR_DEFAULTTONULL);

	// if all points are offscreen
	if (!hMonitorTL && !hMonitorTR && !hMonitorBL && !hMonitorBR)
	{
		HMONITOR hNearestMonitor = MonitorFromRect(testRect,MONITOR_DEFAULTTONEAREST);
		MONITORINFO monitorInfo;
		monitorInfo.cbSize = sizeof(monitorInfo);
		GetMonitorInfo(hNearestMonitor, &monitorInfo);

		CRect desktopRect(monitorInfo.rcWork);
		CPoint newOffset(0,0);

		// set the offset rect to make sure the window is visible on the screen
		if (windowRect.right>desktopRect.right)
		{
			newOffset.x = desktopRect.right-windowRect.right;
		}
		if (windowRect.bottom>desktopRect.bottom)
		{
			newOffset.y = desktopRect.bottom-windowRect.bottom;
		}
		if (windowRect.left<desktopRect.left)
		{
			newOffset.x = desktopRect.left-windowRect.left;
		}
		if (windowRect.top<desktopRect.top)
		{
			newOffset.y = desktopRect.top-windowRect.top;
		}
		// adjust window rectangle to be onscreen
		windowRect.OffsetRect(newOffset);
	}
}

BOOL CAssetManagerDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (NULL != m_pToolTip)
		m_pToolTip->RelayEvent(pMsg);

	if (NULL != m_pDataTip)
		m_pDataTip->RelayEvent(pMsg);

// copied from CDialog::PreTranslateMessage source - we don't want the escape to be handled the default way

	// for modeless processing (or modal)
	ASSERT(m_hWnd != NULL);
	
	// allow tooltip messages to be filtered
	if (CWnd::PreTranslateMessage(pMsg))
		return TRUE;
	
	// don't translate dialog messages when in Shift+F1 help mode
	CFrameWnd* pFrameWnd = GetTopLevelFrame();
	if (pFrameWnd != NULL && pFrameWnd->m_bHelpMode)
		return FALSE;
	
#if 0
	// fix around for VK_ESCAPE in a multiline Edit that is on a Dialog
	// that doesn't have a cancel or the cancel is disabled.
	if (pMsg->message == WM_KEYDOWN &&
		(pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_CANCEL) &&
		(::GetWindowLong(pMsg->hwnd, GWL_STYLE) & ES_MULTILINE) &&
		_AfxCompareClassName(pMsg->hwnd, _T("Edit")))
	{
		HWND hItem = ::GetDlgItem(m_hWnd, IDCANCEL);
		if (hItem == NULL || ::IsWindowEnabled(hItem))
		{
			SendMessage(WM_COMMAND, IDCANCEL, 0);
			return TRUE;
		}
	}
#endif
	// filter both messages to dialog and from children
	return PreTranslateInput(pMsg);
}

class FolderSetSortData
{
public:
	TupString m_name;
	int m_index;
	CTime m_scanTime;

	FolderSetSortData(void) : m_index(0)
	{
	}

	bool FolderSetSortData::operator==(const FolderSetSortData& other) const
	{
		return (m_scanTime==other.m_scanTime &&
			strcmp(m_name,other.m_name)==0);
	}

	bool FolderSetSortData::operator<=(const FolderSetSortData& other) const
	{
		if (m_scanTime>other.m_scanTime)
			return true;
		else if (m_scanTime<other.m_scanTime)
			return false;
		// size ==
		// return true if path is less than or equal other
		return (strcmp(m_name,other.m_name)<1);
	}

	bool FolderSetSortData::operator>=(const FolderSetSortData& other) const
	{
		if (m_scanTime<other.m_scanTime)
			return true;
		else if (m_scanTime>other.m_scanTime)
			return false;
		// size ==
		// return true if path is greater than or equal other
		return (strcmp(m_name,other.m_name)>-1);
	}

	bool FolderSetSortData::operator<(const FolderSetSortData& other) const
	{
		if (m_scanTime>other.m_scanTime)
			return true;
		else if (m_scanTime<other.m_scanTime)
			return false;
		// size ==
		// return true if path is less than other
		return (strcmp(m_name,other.m_name)<0);
	}

	bool FolderSetSortData::operator>(const FolderSetSortData& other) const
	{
		if (m_scanTime<other.m_scanTime)
			return true;
		else if (m_scanTime>other.m_scanTime)
			return false;
		// size ==
		// return true if path is greater than other
		return (strcmp(m_name,other.m_name)>0);
	}
};


void CAssetManagerDlg::RebuildFolderSetsCombo()
{
	int activeSet = m_pFolderSets->GetActiveSet();
	int numSets = m_pFolderSets->GetNumFolderSets();

	m_folderSetsCombo.ResetContent();

	// set up the folder set combo box
	TupArray<FolderSetSortData> sortedSets;
	for (int setIndex=0;setIndex<numSets;setIndex++)
	{
		if (m_pFolderSets->IsEnabled(setIndex))
		{
			FolderSetSortData newEntry;
			newEntry.m_name = m_pFolderSets->GetName(setIndex);
			newEntry.m_index = setIndex;
			// time is set if not sorting alphabetically otherwize use the same value
			newEntry.m_scanTime = m_bSortFolderSets ? 0 : m_pFolderSets->GetLastScanTime(setIndex);
			sortedSets.Add(newEntry);
		}
	}
	int numEnabledSets = sortedSets.GetSize();
	if (numEnabledSets)
	{
		TupSort<FolderSetSortData>::SortAscending(sortedSets.GetData(),sortedSets.GetSize());

		for (int sortIndex = 0; sortIndex < numEnabledSets; sortIndex++)
		{
			const FolderSetSortData &entry = sortedSets[sortIndex];
			int comboIndex = m_folderSetsCombo.AddString(entry.m_name);
			if (entry.m_index == activeSet)
				m_folderSetsCombo.SetCurSel(comboIndex);
			m_folderSetsCombo.SetItemData(comboIndex,entry.m_index);
		}
	}
}



//-----------------------------------------------------------------------------
// CAssetManagerDlg::SetUpFolderSets
// This method is called to set up and load the left and right combo boxes.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
bool CAssetManagerDlg::SetUpFolderSets()
{
	m_pFolderSets = new FolderSets;
	m_pFolderSets->LoadFromRegistry(m_folderSetToLoad);

	int activeSet = m_pFolderSets->GetActiveSet();
	int numSets = m_pFolderSets->GetNumFolderSets();

	bool bFolderSetOk = true;

	// if we have a good set
	if (activeSet!=-1)
	{
		const char *pLeftFolderPath = m_pFolderSets->GetLeftFolderPath(activeSet);
		if (!strlen(pLeftFolderPath) || !::PathFileExists((LPCTSTR) pLeftFolderPath))
			bFolderSetOk = false;
		const char *pRightFolderPath = m_pFolderSets->GetRightFolderPath(activeSet);
		if (!strlen(pRightFolderPath) || !::PathFileExists((LPCTSTR) pRightFolderPath))
			bFolderSetOk = false;
	}
	else
	{
		bFolderSetOk = false;
	}

	// put up dialog to deal with the sets
	if (!bFolderSetOk)
	{
		theApp.m_bAccelEnable = false;
		OpenDatabaseDialog openDatabaseDialog(this);
		openDatabaseDialog.SetFolderSets(*m_pFolderSets);
		theApp.m_bAccelEnable = false;
		int result = openDatabaseDialog.DoModal();
		theApp.m_bAccelEnable = true;
		if (result==FALSE)
		{
			EndDialog(FALSE);
			return false;
		}
		(*m_pFolderSets) = openDatabaseDialog.GetFolderSets();

		theApp.m_bAccelEnable = true;
	}
	// if we got here then we are ok
	activeSet = m_pFolderSets->GetActiveSet();
	numSets = m_pFolderSets->GetNumFolderSets();

	RebuildFolderSetsCombo();

	m_folderPathLeft.SetWindowText(m_pFolderSets->GetLeftFolderPath(activeSet));
	m_folderPathRight.SetWindowText(m_pFolderSets->GetRightFolderPath(activeSet));

	int syncStyle = m_pFolderSets->GetSyncStyle(activeSet);
	if (m_overrideSyncStyle!=-1)
	{
		syncStyle = m_overrideSyncStyle;
		m_pFolderSets->SetSyncStyle(activeSet,syncStyle);
		m_overrideSyncStyle = -1;
	}
	m_syncBitmapFrame.SetSyncStyle(syncStyle);
	m_syncStyleCombo.SetCurSel(syncStyle);

	return true;
}

void CAssetManagerDlg::OnFolderSetsBrowse() 
{
	OpenDatabaseDialog openDatabaseDialog(this);
	openDatabaseDialog.SetFolderSets(*m_pFolderSets);
	theApp.m_bAccelEnable = false;
	int result = openDatabaseDialog.DoModal();
	theApp.m_bAccelEnable = true;
	if (result==TRUE)
	{
		SaveClientData();

		(*m_pFolderSets) = openDatabaseDialog.GetFolderSets();

		// if we got here then we are ok
		int activeSet = m_pFolderSets->GetActiveSet();
		int numSets = m_pFolderSets->GetNumFolderSets();
		m_folderSetsCombo.ResetContent();
		
		// set up the folder set combo box
		for (int setIndex=0;setIndex<numSets;setIndex++)
		{
			if (m_pFolderSets->IsEnabled(setIndex))
			{
				int comboIndex = m_folderSetsCombo.AddString(m_pFolderSets->GetName(setIndex));
				if (setIndex == activeSet)
					m_folderSetsCombo.SetCurSel(comboIndex);
				m_folderSetsCombo.SetItemData(comboIndex,setIndex);
			}
		}
		
		m_folderPathLeft.SetWindowText(m_pFolderSets->GetLeftFolderPath(activeSet));
		m_folderPathRight.SetWindowText(m_pFolderSets->GetRightFolderPath(activeSet));

		int syncStyle = m_pFolderSets->GetSyncStyle(activeSet);
		m_syncBitmapFrame.SetSyncStyle(syncStyle);
		m_syncStyleCombo.SetCurSel(syncStyle);

		m_pTreeCtrlLeft->DeleteAllItems();
		m_pTreeCtrlRight->DeleteAllItems();
		m_pTreeCtrlIndicators->DeleteAllItems();
		m_pAssociatedIconManager->ClearAll();

		m_pSyncTreeManager->ClearDatabases();
		delete m_pRightDataBase;
		m_pRightDataBase = NULL;
		delete m_pLeftDataBase;
		m_pLeftDataBase = NULL;
		m_syncEnable.m_bLRCopyEnable = false;
		m_syncEnable.m_bRLCopyEnable = false;
		m_syncEnable.m_bLDeleteEnable = false;
		m_syncEnable.m_bRDeleteEnable = false;
		LoadDatabases();
	}
	
}

void CAssetManagerDlg::OnSelchangeFolderSets() 
{
	int comboSelIndex = m_folderSetsCombo.GetCurSel();
	int activeSet = m_folderSetsCombo.GetItemData(comboSelIndex);	

	bool bLeftFolderSetOk = true;
	bool bRightFolderSetOk = true;
	const char *pLeftFolderPath = m_pFolderSets->GetLeftFolderPath(activeSet);
	if (!strlen(pLeftFolderPath) || !::PathFileExists((LPCTSTR) pLeftFolderPath))
		bLeftFolderSetOk = false;
	const char *pRightFolderPath = m_pFolderSets->GetRightFolderPath(activeSet);
	if (!strlen(pRightFolderPath) || !::PathFileExists((LPCTSTR) pRightFolderPath))
		bRightFolderSetOk = false;
	if (!bLeftFolderSetOk || !bRightFolderSetOk)
	{
		CString strErrorString;
		if (!bLeftFolderSetOk && !bRightFolderSetOk)
			strErrorString.Format(IDS_FOLDER_DOESNT_EXIST_BOTH,(LPCTSTR) pLeftFolderPath,(LPCTSTR) pRightFolderPath);
		else if (!bLeftFolderSetOk)
			strErrorString.Format(IDS_FOLDER_DOESNT_EXIST_LEFT,(LPCTSTR) pLeftFolderPath);
		else
			strErrorString.Format(IDS_FOLDER_DOESNT_EXIST_RIGHT,(LPCTSTR) pRightFolderPath);

		::AfxMessageBox((LPCTSTR) strErrorString, MB_OK | MB_ICONEXCLAMATION);
		activeSet = m_pFolderSets->GetActiveSet();
		int numSets = m_folderSetsCombo.GetCount();
		
		// set up the folder set combo box
		for (int setIndex=0;setIndex<numSets;setIndex++)
		{
			if (activeSet == (int)m_folderSetsCombo.GetItemData(setIndex))
			{
				m_folderSetsCombo.SetCurSel(setIndex);
				break;
			}
		}
		return;
	}

	SaveClientData();

	// if we got here then we are ok
	m_pFolderSets->SetActiveSet(activeSet);

	int syncStyle = m_pFolderSets->GetSyncStyle(activeSet);
	m_syncBitmapFrame.SetSyncStyle(syncStyle);
	m_syncStyleCombo.SetCurSel(syncStyle);
	
	m_folderPathLeft.SetWindowText(m_pFolderSets->GetLeftFolderPath(activeSet));
	m_folderPathRight.SetWindowText(m_pFolderSets->GetRightFolderPath(activeSet));
	
	m_pSyncTreeManager->ClearDatabases();
	delete m_pRightDataBase;
	m_pRightDataBase = NULL;
	delete m_pLeftDataBase;
	m_pLeftDataBase = NULL;

	m_pTreeCtrlLeft->DeleteAllItems();
	m_pTreeCtrlRight->DeleteAllItems();
	m_pTreeCtrlIndicators->DeleteAllItems();
	m_pAssociatedIconManager->ClearAll();

	m_syncEnable.m_bLRCopyEnable = false;
	m_syncEnable.m_bRLCopyEnable = false;
	m_syncEnable.m_bLDeleteEnable = false;
	m_syncEnable.m_bRDeleteEnable = false;
	LoadDatabases();
}


//-----------------------------------------------------------------------------
// CAssetManagerDlg::SetUpTreeControls
// This method is called to set up the tree controls.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void CAssetManagerDlg::SetUpTreeControls()
{
	// Instantiate the tree control objects:
	m_pTreeCtrlLeft = new CAssetManagerTreeCtrl(this);
	m_pTreeCtrlRight = new CAssetManagerTreeCtrl(this);
	m_pTreeCtrlIndicators = new CAssetManagerTreeCtrl(this);
	m_pAssociatedIconManager = new AssociatedIconManager;

	m_pTreeCtrlLeft->SetOtherFileTree(m_pTreeCtrlRight);
	m_pTreeCtrlLeft->SetIndicatorTree(m_pTreeCtrlIndicators);
	m_pTreeCtrlRight->SetOtherFileTree(m_pTreeCtrlLeft);
	m_pTreeCtrlRight->SetIndicatorTree(m_pTreeCtrlIndicators);

	// Subclass the tree controls:
	m_pTreeCtrlLeft->SubclassTreeCtrlId(IDC_TREE_LEFT, this);
	m_pTreeCtrlRight->SubclassTreeCtrlId(IDC_TREE_RIGHT, this);
	m_pTreeCtrlIndicators->SubclassTreeCtrlId(IDC_TREE_SYNC_INDICATORS, this);

	// Set the background color:
	m_pTreeCtrlLeft->SetDeleteTextBkColor(LEFT_TREE_DELETE_TEXT_BK_COLOR);
	m_pTreeCtrlLeft->SetCopySourceTextBkColor(LEFT_TREE_COPY_SOURCE_TEXT_BK_COLOR);
	m_pTreeCtrlLeft->SetCopyDestTextBkColor(LEFT_TREE_COPY_DEST_TEXT_BK_COLOR);
	m_pTreeCtrlRight->SetDeleteTextBkColor(RIGHT_TREE_DELETE_TEXT_BK_COLOR);
	m_pTreeCtrlRight->SetCopySourceTextBkColor(RIGHT_TREE_COPY_SOURCE_TEXT_BK_COLOR);
	m_pTreeCtrlRight->SetCopyDestTextBkColor(RIGHT_TREE_COPY_DEST_TEXT_BK_COLOR);

	m_pTreeCtrlLeft->SetFrozenTextColor(FROZEN_TEXT_COLOR);
	m_pTreeCtrlRight->SetFrozenTextColor(FROZEN_TEXT_COLOR);

	m_pTreeCtrlLeft->SetAssociatedIconManager(m_pAssociatedIconManager);
	m_pTreeCtrlRight->SetAssociatedIconManager(m_pAssociatedIconManager);

	// Set the tree and list styles:
	m_pTreeCtrlLeft->SetTreeCtrlStyles(TVS_DISABLEDRAGDROP | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_HASLINES,
									   TVXS_COLUMNHEADER | TVXS_FLYBYTOOLTIPS | TVXS_MULTISEL,
									   FALSE);
	m_pTreeCtrlLeft->SetListCtrlStyleEx(LVXS_HILIGHTSUBITEMS | LVXS_LINESBETWEENCOLUMNS |
										LVXS_LINESBETWEENITEMS, FALSE);
	m_pTreeCtrlLeft->SetListCtrlStyle(LVS_EDITLABELS|LVS_REPORT|LVS_NOLABELWRAP|LVS_SHAREIMAGELISTS|LVS_NOCOLUMNHEADER|LVS_NOSORTHEADER, FALSE);
	m_pTreeCtrlLeft->SetRole(CAssetManagerTreeCtrl::LEFT_TREE);

	m_pTreeCtrlRight->SetTreeCtrlStyles(TVS_DISABLEDRAGDROP | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_HASLINES,
										TVXS_COLUMNHEADER | TVXS_FLYBYTOOLTIPS | TVXS_MULTISEL,
										FALSE);
	m_pTreeCtrlRight->SetListCtrlStyleEx(LVXS_HILIGHTSUBITEMS | LVXS_LINESBETWEENCOLUMNS |
										 LVXS_LINESBETWEENITEMS, FALSE);
	m_pTreeCtrlRight->SetListCtrlStyle(LVS_EDITLABELS|LVS_REPORT|LVS_NOLABELWRAP|LVS_SHAREIMAGELISTS|LVS_NOCOLUMNHEADER|LVS_NOSORTHEADER, FALSE);
	m_pTreeCtrlRight->SetRole(CAssetManagerTreeCtrl::RIGHT_TREE);

	m_pTreeCtrlIndicators->SetTreeCtrlStyles(TVS_DISABLEDRAGDROP | TVS_SHOWSELALWAYS,
											 TVXS_COLUMNHEADER | TVXS_FLYBYTOOLTIPS | TVXS_MULTISEL,
											 FALSE);
	m_pTreeCtrlIndicators->SetListCtrlStyleEx(LVXS_LINESBETWEENCOLUMNS | LVXS_LINESBETWEENITEMS, FALSE);
	DWORD style = m_pTreeCtrlIndicators->GetListCtrlStyle();
	m_pTreeCtrlIndicators->SetListCtrlStyle(LVS_REPORT|LVS_NOLABELWRAP|LVS_SHAREIMAGELISTS|LVS_NOCOLUMNHEADER|LVS_NOSORTHEADER|LVS_SHOWSELALWAYS|LVS_NOSCROLL, FALSE);
	m_pTreeCtrlIndicators->SetRole(CAssetManagerTreeCtrl::INDICATOR_TREE);
	m_pTreeCtrlIndicators->SetIndent(48);

	// Set the image lists for the left and right trees:
	HBITMAP hbmTreeControl = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_TREENORMAL), IMAGE_BITMAP,
												  0, 0, LR_CREATEDIBSECTION | LR_LOADTRANSPARENT);
	CBitmap bitmapTreeCtrls;
	bitmapTreeCtrls.Attach(hbmTreeControl);
	m_ilTreeCtrls.Create(16, 20, ILC_COLOR8, 4, 4);
	m_ilTreeCtrls.Add(&bitmapTreeCtrls, (CBitmap*) NULL);
	m_pTreeCtrlLeft->SetImageList(&m_ilTreeCtrls, TVSIL_NORMAL);
	m_pTreeCtrlRight->SetImageList(&m_ilTreeCtrls, TVSIL_NORMAL);

	// Set the image list for the indicator tree:
	BuildImageList::Construct(m_ilTreeIndicator,50,20,ILC_COLOR32,GetSysColor(COLOR_WINDOW),IDR_TREE_INDICATOR);
	m_pTreeCtrlIndicators->SetImageList(&m_ilTreeIndicator, TVSIL_NORMAL);
}

// If you add a minimize button to your dialog, you will need the code below
// to draw the icon.  For MFC applications using the document/view model,
// this is automatically done for you by the framework.

//-----------------------------------------------------------------------------
// CAssetManagerDlg::OnPaint
// This handler is called by the framework whenever a portion of this dialog
// needs to be repainted.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void CAssetManagerDlg::OnPaint() 
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
		CPaintDC dc(this); // device context for painting
		CRect paintRect(dc.m_ps.rcPaint);
		CRect myRect,testRect;

		if (m_bRefreshDatabases)
		{
			m_bRefreshDatabases = false;
			Refresh();
		}
		CDialog::OnPaint();
	}
}




//-----------------------------------------------------------------------------
// CAssetManagerDlg::OnQueryDragIcon
// This handler is called by the system to obtain the cursor to display while
// the user drags the minimized window.
//
// Input:None.
//
// Output: HCURSOR handle to the cursor.
//-----------------------------------------------------------------------------
HCURSOR CAssetManagerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


//-----------------------------------------------------------------------------
// CAssetManagerDlg::OnClose
// This method is called by the framework as a signal that the dialog is about
// to terminate. 
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void CAssetManagerDlg::OnClose()
{
	m_bClosing = true;

	CDialog::OnClose();

	SaveClientData();
	m_pFolderSets->SaveToRegistry();
	// Save out the position of the app:
	Registry registry(ASSET_MANAGER_KEY);
	GetWindowRect(&m_dialogRect);
	registry.Set(DIALOG_RECT_TOP_FIELD,(DWORD)m_dialogRect.top);
	registry.Set(DIALOG_RECT_BOTTOM_FIELD,(DWORD)m_dialogRect.bottom);
	registry.Set(DIALOG_RECT_LEFT_FIELD,(DWORD)m_dialogRect.left);
	registry.Set(DIALOG_RECT_RIGHT_FIELD,(DWORD)m_dialogRect.right);

	registry.Set(AUTO_EXCLUDE_FILE_EXTENSIONS_FIELD,m_defaultAutoFreezeFileExtensions);
	registry.Set(AUTO_EXCLUDE_FILE_PREFIXES_FIELD,m_defaultAutoFreezeFilePrefixes);
	registry.Set(AUTO_EXCLUDE_FOLDER_PREFIXES_FIELD,m_defaultAutoFreezeFolderPrefixes);

	registry.Set(UPDATE_DUPLICATES_INCLUDE_FROZEN_DUPLICATES,m_updateDuplicatesIncludeFrozenDuplicates);
	registry.Set(UPDATE_DUPLICATES_INCLUDE_PARENT_NAMES,m_updateDuplicatesIncludeParentNames);

	registry.Set(VIEW_EXCLUDE_FLAG_FIELD,(m_viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_ACTION_EXCLUDE) ? DWORD(1) : DWORD(0));
	registry.Set(VIEW_FROZEN_FLAG_FIELD,(m_viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_ACTION_FROZEN) ? DWORD(1) : DWORD(0));
	registry.Set(EXCLUDE_LOCK_FLAG_FIELD,(m_viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_EXCLUDE_LOCKED) ? DWORD(1) : DWORD(0));
	registry.Set(FROZEN_LOCK_FLAG_FIELD,(m_viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_FROZEN_LOCKED) ? DWORD(1) : DWORD(0));

	registry.Set(SORT_FOLDER_SETS_FIELD,(DWORD)m_bSortFolderSets);
}


//-----------------------------------------------------------------------------
// CAssetManagerDlg::OnOK
// This message handler will be called by the framework when the user hits the
// enter key while the main app dialog is up.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void CAssetManagerDlg::OnOK()
{
	// We just want to absorb this message, thus keeping the app from quitting
	// when the user hits the enter key:
	SetFocus();
}

//-----------------------------------------------------------------------------
// CAssetManagerDlg::OnCancel
// This message handler will be called by the framework when the user hits the
// esc key while the main app dialog is up.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void CAssetManagerDlg::OnCancel()
{
	if (m_bClosing)
	{
		CDialog::OnCancel();
	}
}

void CAssetManagerDlg::OnGetMinMaxInfo(MINMAXINFO *lpMMI)
{
	lpMMI->ptMinTrackSize.x = 620;
	lpMMI->ptMinTrackSize.y = 419;
}

//-----------------------------------------------------------------------------
// CAssetManagerDlg::OnSize
// This handler is called by the framework after the dialog’s size has changed:
//
// Input:
//  UINT nType -- Specifies the type of resizing requested.
//  int cx -- Specifies the new width of the client area.
//  int cy -- Specifies the new height of the client area.
//
// Output: None.
//-----------------------------------------------------------------------------
void CAssetManagerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	CRect rect;
	GetClientRect(&rect);
	int centerX = rect.Width()>>1; // horizontal center line
	const int topButtonsRow0Y = 7;
	const int topButtonsRow1Y = 36;

	const int foldersSetsY = 72;
	const int foldersPathY = 115;
	const int folderPathWidth = (rect.Width()-48)/2;
	const int folderPathLeftGap = 6;
	const int folderPathRightGap = 10;
	const int folderSetsTitleOffset = 124;
	const int folderSetsComboOffset = 70;
	const int folderSetsBrowseOffset = 64;
	
	const int treeFoldersY = foldersPathY+18;
	const int treeIndicatorWidth = 53;
	const int treeBottomY = 49;

	const int syncButtonX = 22;
	const int copyDeleteButtonLeftX = 54;
	const int copyDeleteButtonRightX = 26;
	const int syncButtonY = 43;
	const int copyButtonY = 44;
	const int deleteButtonY = 22;

	const float statsTextRatio = 0.5f;
	const int statsTextLeftLeftX = 8;
	const int statsTextLeftRightX = 62;
	const int statsTextRightLeftX = 53;
	const int statsTextRightRightX = 14;
	const int statsTextY1 = 41;
	const int statsTextY2 = 20;

	int tBO = 0; // if the top icons are needing two rows this will be changed


	// left folder name
	if (::IsWindow(m_folderPathLeft.m_hWnd))
	{
		CRect myRect;
		m_folderPathLeft.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		m_folderPathLeft.MoveWindow(folderPathLeftGap, tBO + foldersPathY,folderPathWidth, myRect.Height());
	}
	// right folder name
	if (::IsWindow(m_folderPathRight.m_hWnd))
	{
		CRect myRect;
		m_folderPathRight.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		m_folderPathRight.MoveWindow(rect.Width()-(folderPathWidth+folderPathRightGap), tBO + foldersPathY,folderPathWidth, myRect.Height());
	}

	if (::IsWindow(m_syncBitmapFrame.m_hWnd))
	{
		CRect myRect;
		m_syncBitmapFrame.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		m_syncBitmapFrame.MoveWindow(myRect.left, myRect.top,rect.right-(myRect.left+4), myRect.Height());
	}
	
	// tree controls
	CWnd *pCWnd = GetDlgItem(IDC_TREE_LEFT);
	if (pCWnd && ::IsWindow(pCWnd->m_hWnd))
	{
		CRect myRect;
		pCWnd->GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		pCWnd->MoveWindow(0, tBO + treeFoldersY,centerX-33, (rect.bottom-treeBottomY)-(tBO + treeFoldersY));
	}

	pCWnd = GetDlgItem(IDC_TREE_RIGHT);
	if (pCWnd && ::IsWindow(pCWnd->m_hWnd))
	{
		CRect myRect;
		pCWnd->GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		pCWnd->MoveWindow(centerX+26, tBO + treeFoldersY,centerX-29, (rect.bottom-treeBottomY)-(tBO + treeFoldersY));
	}

	pCWnd = GetDlgItem(IDC_TREE_SYNC_INDICATORS);
	if (pCWnd && ::IsWindow(pCWnd->m_hWnd))
	{
		CRect myRect;
		pCWnd->GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		pCWnd->MoveWindow(centerX-30, tBO + treeFoldersY,treeIndicatorWidth, (rect.bottom-treeBottomY)-(tBO + treeFoldersY));
	}

	// sync / copy / delete buttons
	if (::IsWindow(m_syncButton.m_hWnd))
	{
		CRect myRect;
		m_syncButton.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		m_syncButton.MoveWindow(centerX-syncButtonX, rect.bottom-syncButtonY,myRect.Width(), myRect.Height());
		m_syncButton.Invalidate();
	}

	if (::IsWindow(m_lrCopyButton.m_hWnd))
	{
		CRect myRect;
		m_lrCopyButton.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		m_lrCopyButton.MoveWindow(centerX-copyDeleteButtonLeftX, rect.bottom-copyButtonY,myRect.Width(), myRect.Height());
		m_lrCopyButton.Invalidate();
	}

	if (::IsWindow(m_rlCopyButton.m_hWnd))
	{
		CRect myRect;
		m_rlCopyButton.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		m_rlCopyButton.MoveWindow(centerX+copyDeleteButtonRightX, rect.bottom-copyButtonY,myRect.Width(), myRect.Height());
		m_rlCopyButton.Invalidate();
	}

	if (::IsWindow(m_lDeleteButton.m_hWnd))
	{
		CRect myRect;
		m_lDeleteButton.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		m_lDeleteButton.MoveWindow(centerX-copyDeleteButtonLeftX, rect.bottom-deleteButtonY,myRect.Width(), myRect.Height());
		m_lDeleteButton.Invalidate();
	}

	if (::IsWindow(m_rDeleteButton.m_hWnd))
	{
		CRect myRect;
		m_rDeleteButton.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		m_rDeleteButton.MoveWindow(centerX+copyDeleteButtonRightX, rect.bottom-deleteButtonY,myRect.Width(), myRect.Height());
		m_rDeleteButton.Invalidate();
	}

	// left/right stats text
	if (::IsWindow(m_leftStatsText1.m_hWnd))
	{
		CRect myRect;
		m_leftStatsText1.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		m_leftStatsText1.MoveWindow(statsTextLeftLeftX, rect.bottom-statsTextY1,(int)((centerX-(statsTextLeftLeftX+statsTextLeftRightX))*statsTextRatio), myRect.Height());
		m_leftStatsText1.Invalidate();
	}

	if (::IsWindow(m_leftStatsText2.m_hWnd))
	{
		CRect myRect;
		m_leftStatsText2.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		m_leftStatsText2.MoveWindow(statsTextLeftLeftX, rect.bottom-statsTextY2,(int)((centerX-(statsTextLeftLeftX+statsTextLeftRightX))*statsTextRatio), myRect.Height());
		m_leftStatsText2.Invalidate();
	}

	if (::IsWindow(m_leftStatsText3.m_hWnd))
	{
		CRect myRect;
		m_leftStatsText3.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		int width = (int)((centerX-(statsTextLeftLeftX+statsTextLeftRightX))*(1.0f-statsTextRatio));
		m_leftStatsText3.MoveWindow(centerX-(statsTextLeftRightX+width), rect.bottom-statsTextY1,width, myRect.Height());
		m_leftStatsText3.Invalidate();
	}

	if (::IsWindow(m_leftStatsText4.m_hWnd))
	{
		CRect myRect;
		m_leftStatsText4.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		int width = (int)((centerX-(statsTextLeftLeftX+statsTextLeftRightX))*(1.0f-statsTextRatio));
		m_leftStatsText4.MoveWindow(centerX-(statsTextLeftRightX+width), rect.bottom-statsTextY2,width, myRect.Height());
		m_leftStatsText4.Invalidate();
	}

	if (::IsWindow(m_rightStatsText1.m_hWnd))
	{
		CRect myRect;
		m_rightStatsText1.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		int width = (int)((centerX-(statsTextLeftLeftX+statsTextLeftRightX))*statsTextRatio);
		m_rightStatsText1.MoveWindow(rect.right-(statsTextRightRightX+width), rect.bottom-statsTextY1,width, myRect.Height());
		m_rightStatsText1.Invalidate();
	}

	if (::IsWindow(m_rightStatsText2.m_hWnd))
	{
		CRect myRect;
		m_rightStatsText2.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		int width = (int)((centerX-(statsTextLeftLeftX+statsTextLeftRightX))*statsTextRatio);
		m_rightStatsText2.MoveWindow(rect.right-(statsTextRightRightX+width), rect.bottom-statsTextY2,width, myRect.Height());
		m_rightStatsText2.Invalidate();
	}

	if (::IsWindow(m_rightStatsText3.m_hWnd))
	{
		CRect myRect;
		m_rightStatsText3.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		int width = (int)((centerX-(statsTextLeftLeftX+statsTextLeftRightX))*(1.0f-statsTextRatio));
		m_rightStatsText3.MoveWindow(centerX+statsTextRightLeftX, rect.bottom-statsTextY1,width, myRect.Height());
		m_rightStatsText3.Invalidate();
	}
	if (::IsWindow(m_rightStatsText4.m_hWnd))
	{
		CRect myRect;
		m_rightStatsText4.GetWindowRect(&myRect);
		ScreenToClient(&myRect);
		int width = (int)((centerX-(statsTextLeftLeftX+statsTextLeftRightX))*(1.0f-statsTextRatio));
		m_rightStatsText4.MoveWindow(centerX+statsTextRightLeftX, rect.bottom-statsTextY2,width, myRect.Height());
		m_rightStatsText4.Invalidate();
	}

//	Invalidate(FALSE);
}

//-----------------------------------------------------------------------------
// CAssetManagerDlg::OnNotify
// This handler will be called by the framework to inform the dialog of a
// control that an event has occurred in or that the control requires some
// kind of information.
//
// Input:
//  WPARAM wParam -- Identifies the control that sends the message if the
//					 message is from a control. Otherwise, wParam is 0.
//  LPARAM lParam -- Structure that contains the notification code and
//					 additional information.
//  LRESULT* pResult -- Pointer to an LRESULT variable in which to store the
//						result code if the message is handled.
//
// Output: TRUE. 
//-----------------------------------------------------------------------------
BOOL CAssetManagerDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// See what notification message is being sent:
	LPNMHDR  pNMHDR = (LPNMHDR)lParam;
	if (UDM_TOOLTIP_DISPLAY == pNMHDR->code)
	{
		// Set up some strings, etc.
		NM_PPTOOLTIP_DISPLAY *pNotify = (NM_PPTOOLTIP_DISPLAY*) pNMHDR;
		CString strTipText = "";

		// Get the control and use the text from it:
		UINT nIDControl = ::GetDlgCtrlID((HWND)pNotify->ti->hWnd);
		if (nIDControl != 0) // Will be zero on a separator
		{
			if (nIDControl==IDC_TREE_LEFT || nIDControl==IDC_TREE_RIGHT || nIDControl==IDC_TREE_SYNC_INDICATORS)
			{
				strTipText = GetMouseOverTipText();
			}
			else
			{
				// Get our status and tip text:
				TCHAR szFullText[256];
				AfxLoadString(nIDControl, szFullText);
				strTipText = szFullText;
			}
		}
		pNotify->ti->sTooltip = strTipText;

		// Set the result:
		*pResult = 0;
	}
	else if (TVN_BEGINLABELEDIT == pNMHDR->code)
	{
		theApp.m_bAccelEnable = false;
		*pResult = FALSE;
	}
	else if (TVN_ENDLABELEDIT == pNMHDR->code)
	{
		theApp.m_bAccelEnable = true;

		TV_DISPINFO *pDispInfo = (TV_DISPINFO*) pNMHDR;

		if (pDispInfo->item.pszText)
		{
			if (pDispInfo->hdr.idFrom == IDC_TREE_LEFT)
			{
				HTREEITEM hRenameItem = pDispInfo->item.hItem;
				if (hRenameItem)
				{
					SEC_DWORD sDWItemData = m_pTreeCtrlLeft->GetItemData(hRenameItem);
					AMSyncTreeNodeFolder* pSyncNodeFolder = dynamic_cast<AMSyncTreeNodeFolder*>((TreeNodeBase*) sDWItemData);
					if (pSyncNodeFolder)
					{
						// did the name change?
						if (stricmp(pSyncNodeFolder->GetFileName(),pDispInfo->item.pszText)!=0)
						{
							TreeNodeFolder *pFolderNode = pSyncNodeFolder->GetLeftFolderNode();
							if (pFolderNode)
							{
								int result = m_pLeftDataBase->RenameFolder(pFolderNode,pDispInfo->item.pszText);
								if (result!=0)
								{
									CString strErrorString;
									strErrorString.Format(IDS_FOLDER_RENAME_ERROR,pFolderNode->GetFileName());
									::AfxMessageBox((LPCTSTR) strErrorString, MB_OK | MB_ICONEXCLAMATION);
									*pResult = FALSE;
								}
								else
								{
									*pResult = TRUE;
									m_bRefreshDatabases = true;
									Invalidate(FALSE);
								}
							}
						}
					}
					AMSyncTreeNodeFile* pSyncNodeFile = dynamic_cast<AMSyncTreeNodeFile*>((TreeNodeBase*) sDWItemData);
					if (pSyncNodeFile)
					{
						// did the name change?
						if (stricmp(pSyncNodeFile->GetFileName(),pDispInfo->item.pszText)!=0)
						{
							TreeNodeFile *pFileNode = pSyncNodeFile->GetLeftFileNode();
							if (pFileNode)
							{
								int result = m_pLeftDataBase->RenameFile(pFileNode,pDispInfo->item.pszText);
								if (result!=0)
								{
									CString strErrorString;
									strErrorString.Format(IDS_FILE_RENAME_ERROR,pFileNode->GetFileName());
									::AfxMessageBox((LPCTSTR) strErrorString, MB_OK | MB_ICONEXCLAMATION);
									*pResult = FALSE;
								}
								else
								{
									*pResult = TRUE;
									m_bRefreshDatabases = true;
									Invalidate(FALSE);
								}
							}
						}
					}
				}
			}
			else if (pDispInfo->hdr.idFrom == IDC_TREE_RIGHT)
			{
				HTREEITEM hRenameItem = pDispInfo->item.hItem;
				if (hRenameItem)
				{
					SEC_DWORD sDWItemData = m_pTreeCtrlRight->GetItemData(hRenameItem);
					AMSyncTreeNodeFolder* pSyncNodeFolder = dynamic_cast<AMSyncTreeNodeFolder*>((TreeNodeBase*) sDWItemData);
					if (pSyncNodeFolder)
					{
						// did the name change?
						if (stricmp(pSyncNodeFolder->GetFileName(),pDispInfo->item.pszText)!=0)
						{
							TreeNodeFolder *pFolderNode = pSyncNodeFolder->GetRightFolderNode();
							if (pFolderNode)
							{
								int result = m_pRightDataBase->RenameFolder(pFolderNode,pDispInfo->item.pszText);
								if (result!=0)
								{
									CString strErrorString;
									strErrorString.Format(IDS_FOLDER_RENAME_ERROR,pFolderNode->GetFileName());
									::AfxMessageBox((LPCTSTR) strErrorString, MB_OK | MB_ICONEXCLAMATION);
									*pResult = FALSE;
								}
								else
								{
									*pResult = TRUE;
									m_bRefreshDatabases = true;
									Invalidate(FALSE);
								}
							}
						}
					}
					AMSyncTreeNodeFile* pSyncNodeFile = dynamic_cast<AMSyncTreeNodeFile*>((TreeNodeBase*) sDWItemData);
					if (pSyncNodeFile)
					{
						// did the name change?
						if (stricmp(pSyncNodeFile->GetFileName(),pDispInfo->item.pszText)!=0)
						{
							TreeNodeFile *pFileNode = pSyncNodeFile->GetRightFileNode();
							if (pFileNode)
							{
								int result = m_pRightDataBase->RenameFile(pFileNode,pDispInfo->item.pszText);
								if (result!=0)
								{
									CString strErrorString;
									strErrorString.Format(IDS_FILE_RENAME_ERROR,pFileNode->GetFileName());
									::AfxMessageBox((LPCTSTR) strErrorString, MB_OK | MB_ICONEXCLAMATION);
									*pResult = FALSE;
								}
								else
								{
									*pResult = TRUE;
									m_bRefreshDatabases = true;
									Invalidate(FALSE);
								}
							}
						}
					}
				}
			}
		}
	}
	return TRUE;
}

//-----------------------------------------------------------------------------
// CAssetManagerDlg::OnDefaultOptions
// This handler will be called when the user clicks on the Options menu item.
// It will bring up the Options dialog.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void CAssetManagerDlg::OnDefaultOptions()
{
	COptionSheet defaultOptions(_T("Global Options"), this);

	CDefaultAutoFreezePage autoFreezePage;
	CDefaultUpdateDuplicatesPage updateDuplicatesPage;

	autoFreezePage.m_fileExtensions = m_defaultAutoFreezeFileExtensions;
	autoFreezePage.m_filePrefixes = m_defaultAutoFreezeFilePrefixes;
	autoFreezePage.m_folderPrefixes = m_defaultAutoFreezeFolderPrefixes;

	updateDuplicatesPage.m_includeFrozenDuplicates = m_updateDuplicatesIncludeFrozenDuplicates;
	updateDuplicatesPage.m_includeParentDirectory = m_updateDuplicatesIncludeParentNames;

	defaultOptions.AddPage(&autoFreezePage);
	defaultOptions.AddPage(&updateDuplicatesPage);

	theApp.m_bAccelEnable = false;

	bool bDoRefresh = false;

	int activeSet = m_pFolderSets->GetActiveSet();
	bool bUseDefaultAutoExclude = m_pFolderSets->IsUsingDefaultAutoFreezeExtensions(activeSet);

	if (defaultOptions.DoModal() == IDOK)
	{
		// AutoFreeze
		if (strcmp(autoFreezePage.m_fileExtensions,m_defaultAutoFreezeFileExtensions)!=0)
		{
			m_defaultAutoFreezeFileExtensions = autoFreezePage.m_fileExtensions;
			if (bUseDefaultAutoExclude)
				bDoRefresh = true;
		}
		if (strcmp(autoFreezePage.m_filePrefixes,m_defaultAutoFreezeFilePrefixes)!=0)
		{
			m_defaultAutoFreezeFilePrefixes = autoFreezePage.m_filePrefixes;
			if (bUseDefaultAutoExclude)
				bDoRefresh = true;
		}
		if (strcmp(autoFreezePage.m_folderPrefixes,m_defaultAutoFreezeFolderPrefixes)!=0)
		{
			m_defaultAutoFreezeFolderPrefixes = autoFreezePage.m_folderPrefixes;
			if (bUseDefaultAutoExclude)
				bDoRefresh = true;
		}
		// Update Duplicates
		m_updateDuplicatesIncludeFrozenDuplicates = updateDuplicatesPage.m_includeFrozenDuplicates;
		m_updateDuplicatesIncludeParentNames = updateDuplicatesPage.m_includeParentDirectory;
	}
	theApp.m_bAccelEnable = true;
	if (bDoRefresh)
		Refresh();
}

void CAssetManagerDlg::OnFolderSetOptions()
{
	COptionSheet folderSetOptions(_T("Current Folder Set Options"), this);
	
	CFolderSetAutoFreezePage autoFreezePage;
	CAutoFreezeListPage autoFreezeListPage;
	CScanSettingsPage scanSettingsPage;
	CSyncSettingsPage syncSettingsPage;
	
	int activeSet = m_pFolderSets->GetActiveSet();
	bool bPreviousUseDefaultAutoExclude = m_pFolderSets->IsUsingDefaultAutoFreezeExtensions(activeSet);
	
	TupString previousFileExtensionsString = m_pFolderSets->GetAutoFreezeFileExtensions(activeSet);
	TupString previousFilePrefixesString = m_pFolderSets->GetAutoFreezeFilePrefixes(activeSet);
	TupString previousFolderPrefixesString = m_pFolderSets->GetAutoFreezeFolderPrefixes(activeSet);

	autoFreezePage.m_fileExtensions = previousFileExtensionsString;
	autoFreezePage.m_filePrefixes = previousFilePrefixesString;
	autoFreezePage.m_folderPrefixes = previousFolderPrefixesString;
	autoFreezePage.m_bUseDefaultAutoExclude = bPreviousUseDefaultAutoExclude ? TRUE : FALSE;

	scanSettingsPage.m_bDoCRCCheck = m_pFolderSets->GetDoCRCCheck(activeSet) ? TRUE : FALSE;
	scanSettingsPage.m_threadPriority = m_pFolderSets->GetScanThreadPriority(activeSet);

	syncSettingsPage.m_bUseSyncDates = m_pFolderSets->GetUseSyncDates(activeSet) ? TRUE : FALSE;
	syncSettingsPage.m_bVerifyCopiedFiles = m_pFolderSets->GetVerifyCopiedFiles(activeSet) ? TRUE : FALSE;
	syncSettingsPage.m_bCloseWhenCompleted = m_pFolderSets->GetCloseWhenCompleted(activeSet) ? TRUE : FALSE;
	syncSettingsPage.m_bExitWhenClosed = m_pFolderSets->GetExitWhenClosed(activeSet) ? TRUE : FALSE;
	syncSettingsPage.m_threadPriority = m_pFolderSets->GetSyncThreadPriority(activeSet);

	int numAutoFrozenPaths = m_autoExclude.GetNumPaths();
	m_pSyncTreeManager->GetUsedAutoFreezePaths(autoFreezeListPage.m_entryUsed);
	assert(autoFreezeListPage.m_entryUsed.GetSize()==numAutoFrozenPaths);
	for (int pathIndex=0;pathIndex<numAutoFrozenPaths;pathIndex++)
	{
		TupString path;
		m_autoExclude.GetPath(path,pathIndex);
		autoFreezeListPage.m_entries.Add(path);
	}
	
	folderSetOptions.AddPage(&autoFreezePage);
	folderSetOptions.AddPage(&autoFreezeListPage);
	folderSetOptions.AddPage(&scanSettingsPage);
	folderSetOptions.AddPage(&syncSettingsPage);
	folderSetOptions.SetActivePage(1);
	
	theApp.m_bAccelEnable = false;
	
	bool bDoRefresh = false;

	if (folderSetOptions.DoModal() == IDOK)
	{
		bool bUseDefaultAutoExclude = autoFreezePage.m_bUseDefaultAutoExclude ? true : false;
		if (bUseDefaultAutoExclude != bPreviousUseDefaultAutoExclude)
		{
			m_pFolderSets->SetUsingDefaultAutoFreezeExtensions(activeSet,bUseDefaultAutoExclude);
			bDoRefresh = true;
		}

		// AutoFreeze
		if (strcmp(autoFreezePage.m_fileExtensions,previousFileExtensionsString)!=0)
		{
			m_pFolderSets->SetAutoFreezeFileExtensions(activeSet,autoFreezePage.m_fileExtensions);
			if (!bUseDefaultAutoExclude)
				bDoRefresh = true;
		}
		if (strcmp(autoFreezePage.m_filePrefixes,previousFilePrefixesString)!=0)
		{
			m_pFolderSets->SetAutoFreezeFilePrefixes(activeSet,autoFreezePage.m_filePrefixes);
			if (!bUseDefaultAutoExclude)
				bDoRefresh = true;
		}
		if (strcmp(autoFreezePage.m_folderPrefixes,previousFolderPrefixesString)!=0)
		{
			m_pFolderSets->SetAutoFreezeFolderPrefixes(activeSet,autoFreezePage.m_folderPrefixes);
			if (!bUseDefaultAutoExclude)
				bDoRefresh = true;
		}
		int numPaths = autoFreezeListPage.m_entries.GetSize();
		if (autoFreezeListPage.m_changed || numPaths!=m_autoExclude.GetNumPaths())
		{
			m_autoExclude.RemovePaths();
			for (int pathIndex=0;pathIndex<numPaths;pathIndex++)
			{
				m_autoExclude.AddPath(autoFreezeListPage.m_entries[pathIndex]);
			}
			bDoRefresh = true;
		}

		bool bDoCRCCheck = scanSettingsPage.m_bDoCRCCheck ? true : false;
		if (m_pFolderSets->GetDoCRCCheck(activeSet) != bDoCRCCheck)
		{
			m_pFolderSets->SetDoCRCCheck(activeSet,bDoCRCCheck);
			bDoRefresh = true;
		}

		int scanThreadPriority = scanSettingsPage.m_threadPriority;
		if (m_pFolderSets->GetScanThreadPriority(activeSet) != scanThreadPriority)
		{
			m_pFolderSets->SetScanThreadPriority(activeSet,scanThreadPriority);
		}

		bool bUseSyncDates = syncSettingsPage.m_bUseSyncDates ? true : false;
		if (m_pFolderSets->GetUseSyncDates(activeSet) != bUseSyncDates)
		{
			m_pFolderSets->SetUseSyncDates(activeSet,bUseSyncDates);
			bDoRefresh = true;
		}

		bool bVerifyCopiedFiles = syncSettingsPage.m_bVerifyCopiedFiles ? true : false;
		if (m_pFolderSets->GetVerifyCopiedFiles(activeSet) != bVerifyCopiedFiles)
		{
			m_pFolderSets->SetVerifyCopiedFiles(activeSet,bVerifyCopiedFiles);
		}

		bool bCloseWhenCompleted = syncSettingsPage.m_bCloseWhenCompleted ? true : false;
		if (m_pFolderSets->GetCloseWhenCompleted(activeSet) != bCloseWhenCompleted)
		{
			m_pFolderSets->SetCloseWhenCompleted(activeSet,bCloseWhenCompleted);
		}

		bool bExitWhenClosed = syncSettingsPage.m_bExitWhenClosed ? true : false;
		if (m_pFolderSets->GetExitWhenClosed(activeSet) != bExitWhenClosed)
		{
			m_pFolderSets->SetExitWhenClosed(activeSet,bExitWhenClosed);
		}

		int syncThreadPriority = syncSettingsPage.m_threadPriority;
		if (m_pFolderSets->GetSyncThreadPriority(activeSet) != syncThreadPriority)
		{
			m_pFolderSets->SetSyncThreadPriority(activeSet,syncThreadPriority);
		}

	}
	theApp.m_bAccelEnable = true;
	if (bDoRefresh)
		Refresh();
}

//-----------------------------------------------------------------------------
// CAssetManagerDlg::OnCopyToLeft
// This message handler is called when the user selects "Copy To Left" from
// the context menu for the indicator tree.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void CAssetManagerDlg::OnContextCopyToLeft()
{
	ChangeAction(SyncTreeManager::ACTION_COPY_RIGHT_TO_LEFT);
}


//-----------------------------------------------------------------------------
// CAssetManagerDlg::OnCopyToRight
// This message handler is called when the user selects "Copy To Right" from
// the context menu for the indicator tree.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void CAssetManagerDlg::OnContextCopyToRight()
{
	ChangeAction(SyncTreeManager::ACTION_COPY_LEFT_TO_RIGHT);
}


//-----------------------------------------------------------------------------
// CAssetManagerDlg::OnDeleteFromLeft
// This message handler is called when the user selects "Delete From Left" from
// the context menu for the indicator tree.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void CAssetManagerDlg::OnContextDeleteFromLeft()
{
	ChangeAction(SyncTreeManager::ACTION_DELETE_LEFT);
}


//-----------------------------------------------------------------------------
// CAssetManagerDlg::OnDeleteFromRight
// This message handler is called when the user selects "Delete From Right" from
// the context menu for the indicator tree.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void CAssetManagerDlg::OnContextDeleteFromRight()
{
	ChangeAction(SyncTreeManager::ACTION_DELETE_RIGHT);
}


//-----------------------------------------------------------------------------
// CAssetManagerDlg::OnDeleteFromBoth
// This message handler is called when the user selects "Delete From Both" from
// the context menu for the indicator tree.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void CAssetManagerDlg::OnContextDeleteFromBoth()
{
	ChangeAction(SyncTreeManager::ACTION_DELETE_BOTH);
}


//-----------------------------------------------------------------------------
// CAssetManagerDlg::OnContextDoNothing
// This message handler is called when the user selects "Do Nothing" from
// the context menu for the indicator tree.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void CAssetManagerDlg::OnContextExclude()
{
	ChangeAction(SyncTreeManager::ACTION_EXCLUDE);
}

void CAssetManagerDlg::OnContextSetDefault()
{
	ChangeAction(SyncTreeManager::ACTION_DEFAULT_ACTION);
}

void CAssetManagerDlg::OnContextSelectAll()
{
	// Check to see which tree items we need to change:
	CAssetManagerTreeCtrl* pTreeControl;
	if (m_bContextFromLeft == true)
		pTreeControl = m_pTreeCtrlLeft;
	else
		pTreeControl = m_pTreeCtrlRight;
	pTreeControl->SelectAll();
}

void CAssetManagerDlg::OnContextSelectNone()
{
	// Check to see which tree items we need to change:
	CAssetManagerTreeCtrl* pTreeControl;
	if (m_bContextFromLeft == true)
		pTreeControl = m_pTreeCtrlLeft;
	else
		pTreeControl = m_pTreeCtrlRight;
	pTreeControl->DeselectAll();
}

void CAssetManagerDlg::OnContextInvertSelection()
{
	// Check to see which tree items we need to change:
	CAssetManagerTreeCtrl* pTreeControl;
	if (m_bContextFromLeft == true)
		pTreeControl = m_pTreeCtrlLeft;
	else
		pTreeControl = m_pTreeCtrlRight;
	pTreeControl->InvertSelection();
}

void CAssetManagerDlg::OnContextFreeze()
{
	ChangeAction(SyncTreeManager::ACTION_FREEZE);
}

void CAssetManagerDlg::OnContextUnFreeze()
{
	ChangeAction(SyncTreeManager::ACTION_UNFREEZE);
}

void CAssetManagerDlg::OnContextUnFreezeOnly()
{
	// Check to see which tree items we need to change:
	CAssetManagerTreeCtrl* pTreeControl;
	if (m_bContextFromLeft == true)
		pTreeControl = m_pTreeCtrlLeft;
	else
		pTreeControl = m_pTreeCtrlRight;
	pTreeControl->InvertSelection();
	ChangeAction(SyncTreeManager::ACTION_FREEZE,false);
	pTreeControl->InvertSelection();
	ChangeAction(SyncTreeManager::ACTION_UNFREEZE,false);
	ChangeAction(SyncTreeManager::ACTION_DEFAULT_ACTION); // update tree here
}

void CAssetManagerDlg::OnContextExpandAll()
{
	CAssetManagerTreeCtrl* pTreeControl;
	if (m_bContextFromLeft == true)
		pTreeControl = m_pTreeCtrlLeft;
	else
		pTreeControl = m_pTreeCtrlRight;

	m_pTreeCtrlLeft->SetNodifyExpanded(false);
	m_pTreeCtrlRight->SetNodifyExpanded(false);
	m_pTreeCtrlIndicators->SetNodifyExpanded(false);

	HTREEITEM hCurrItem = pTreeControl->GetFirstSelectedItem();
	while (hCurrItem != NULL)
	{
		SEC_DWORD sDWItemData = pTreeControl->GetItemData(hCurrItem);
		AMSyncTreeNodeFolder* pSyncNodeFolder = dynamic_cast<AMSyncTreeNodeFolder*>((TreeNodeBase*) sDWItemData);
		if (pSyncNodeFolder)
		{
			m_pTreeCtrlLeft->ExpandCompletely(pSyncNodeFolder->GetHTreeItemLeft(),FALSE);
			m_pTreeCtrlRight->ExpandCompletely(pSyncNodeFolder->GetHTreeItemRight(),FALSE);
			m_pTreeCtrlIndicators->ExpandCompletely(pSyncNodeFolder->GetHTreeItemIndicator(),FALSE);
		}
		hCurrItem = pTreeControl->GetNextSelectedItem(hCurrItem);
	}
	m_pTreeCtrlLeft->ReMeasureAllItems();
	m_pTreeCtrlRight->ReMeasureAllItems();
	m_pTreeCtrlIndicators->ReMeasureAllItems();

	int topIndex = pTreeControl->GetTopIndex();
	m_pTreeCtrlLeft->SetTopIndex(topIndex);
	m_pTreeCtrlRight->SetTopIndex(topIndex);
	m_pTreeCtrlIndicators->SetTopIndex(topIndex);

	m_pTreeCtrlLeft->SetNodifyExpanded(true);
	m_pTreeCtrlRight->SetNodifyExpanded(true);
	m_pTreeCtrlIndicators->SetNodifyExpanded(true);

	m_pTreeCtrlLeft->Invalidate();
	m_pTreeCtrlRight->Invalidate();
	m_pTreeCtrlIndicators->Invalidate();
}

void CAssetManagerDlg::OnContextCollapseAll()
{
	CAssetManagerTreeCtrl* pTreeControl;
	if (m_bContextFromLeft == true)
		pTreeControl = m_pTreeCtrlLeft;
	else
		pTreeControl = m_pTreeCtrlRight;

	m_pTreeCtrlLeft->SetNodifyExpanded(false);
	m_pTreeCtrlRight->SetNodifyExpanded(false);
	m_pTreeCtrlIndicators->SetNodifyExpanded(false);

	HTREEITEM hCurrItem = pTreeControl->GetFirstSelectedItem();
	while (hCurrItem != NULL)
	{
		SEC_DWORD sDWItemData = pTreeControl->GetItemData(hCurrItem);
		AMSyncTreeNodeFolder* pSyncNodeFolder = dynamic_cast<AMSyncTreeNodeFolder*>((TreeNodeBase*) sDWItemData);
		if (pSyncNodeFolder)
		{
			m_pTreeCtrlLeft->CollapseCompletely(pSyncNodeFolder->GetHTreeItemLeft(),FALSE);
			m_pTreeCtrlRight->CollapseCompletely(pSyncNodeFolder->GetHTreeItemRight(),FALSE);
			m_pTreeCtrlIndicators->CollapseCompletely(pSyncNodeFolder->GetHTreeItemIndicator(),FALSE);
		}
		hCurrItem = pTreeControl->GetNextSelectedItem(hCurrItem);
	}
	m_pTreeCtrlLeft->ReMeasureAllItems();
	m_pTreeCtrlRight->ReMeasureAllItems();
	m_pTreeCtrlIndicators->ReMeasureAllItems();

	int topIndex = pTreeControl->GetTopIndex();
	m_pTreeCtrlLeft->SetTopIndex(topIndex);
	m_pTreeCtrlRight->SetTopIndex(topIndex);
	m_pTreeCtrlIndicators->SetTopIndex(topIndex);

	m_pTreeCtrlLeft->SetNodifyExpanded(true);
	m_pTreeCtrlRight->SetNodifyExpanded(true);
	m_pTreeCtrlIndicators->SetNodifyExpanded(true);

	m_pTreeCtrlLeft->Invalidate();
	m_pTreeCtrlRight->Invalidate();
	m_pTreeCtrlIndicators->Invalidate();
}

void CAssetManagerDlg::OnContextAddPrefix()
{
	CAssetManagerTreeCtrl* pTreeControl;

	if (m_bContextFromLeft == true)
	{
		pTreeControl = m_pTreeCtrlLeft;
	}
	else
	{
		pTreeControl = m_pTreeCtrlRight;
	}
	
	bool bNeedToRefresh = false;
	HTREEITEM hCurrItem = pTreeControl->GetFirstSelectedItem();
	while (hCurrItem != NULL)
	{
		SEC_DWORD sDWItemData = pTreeControl->GetItemData(hCurrItem);
		AMSyncTreeNodeFolder* pSyncNodeFolder = dynamic_cast<AMSyncTreeNodeFolder*>((TreeNodeBase*) sDWItemData);
		if (pSyncNodeFolder)
		{
			TupString folderName = pSyncNodeFolder->GetFileName();
			if (m_autoExclude.AddFolderPrefix(folderName))
			{
				if (m_bContextFromLeft)
				{
					TreeNodeFolder *pFolderNode = pSyncNodeFolder->GetLeftFolderNode();
					int result = m_pLeftDataBase->RenameFolder(pFolderNode,folderName);
					if (result!=0)
					{
						
						CString strErrorString;
						strErrorString.Format(IDS_FOLDER_ADD_PREFIX_ERROR,pFolderNode->GetFileName());
						::AfxMessageBox((LPCTSTR) strErrorString, MB_OK | MB_ICONEXCLAMATION);
					}
					else
					{
						bNeedToRefresh = true;
					}
				}
				else
				{
					TreeNodeFolder *pFolderNode = pSyncNodeFolder->GetRightFolderNode();
					int result = m_pRightDataBase->RenameFolder(pFolderNode,folderName);
					if (result!=0)
					{
						
						CString strErrorString;
						strErrorString.Format(IDS_FOLDER_ADD_PREFIX_ERROR,pFolderNode->GetFileName());
						::AfxMessageBox((LPCTSTR) strErrorString, MB_OK | MB_ICONEXCLAMATION);
					}
					else
					{
						bNeedToRefresh = true;
					}
				}
			}
		}
		AMSyncTreeNodeFile* pSyncNodeFile = dynamic_cast<AMSyncTreeNodeFile*>((TreeNodeBase*) sDWItemData);
		if (pSyncNodeFile)
		{
			TupString fileName = pSyncNodeFile->GetFileName();
			if (m_autoExclude.AddFilePrefix(fileName))
			{
				if (m_bContextFromLeft)
				{
					TreeNodeFile *pFileNode = pSyncNodeFile->GetLeftFileNode();
					int result = m_pLeftDataBase->RenameFile(pFileNode,fileName);
					if (result!=0)
					{
						
						CString strErrorString;
						strErrorString.Format(IDS_FILE_ADD_PREFIX_ERROR,pFileNode->GetFileName());
						::AfxMessageBox((LPCTSTR) strErrorString, MB_OK | MB_ICONEXCLAMATION);
					}
					else
					{
						bNeedToRefresh = true;
					}
				}
				else
				{
					TreeNodeFile *pFileNode = pSyncNodeFile->GetRightFileNode();
					int result = m_pRightDataBase->RenameFile(pFileNode,fileName);
					if (result!=0)
					{
						
						CString strErrorString;
						strErrorString.Format(IDS_FILE_ADD_PREFIX_ERROR,pFileNode->GetFileName());
						::AfxMessageBox((LPCTSTR) strErrorString, MB_OK | MB_ICONEXCLAMATION);
					}
					else
					{
						bNeedToRefresh = true;
					}
				}
			}
		}
		hCurrItem = pTreeControl->GetNextSelectedItem(hCurrItem);
	}
	if (bNeedToRefresh)
	{
		m_bRefreshDatabases = true;
		Invalidate(FALSE);
	}
}

void CAssetManagerDlg::OnContextRemovePrefix()
{
	CAssetManagerTreeCtrl* pTreeControl;

	if (m_bContextFromLeft == true)
	{
		pTreeControl = m_pTreeCtrlLeft;
	}
	else
	{
		pTreeControl = m_pTreeCtrlRight;
	}
	
	bool bNeedToRefresh = false;
	HTREEITEM hCurrItem = pTreeControl->GetFirstSelectedItem();
	while (hCurrItem != NULL)
	{
		SEC_DWORD sDWItemData = pTreeControl->GetItemData(hCurrItem);
		AMSyncTreeNodeFolder* pSyncNodeFolder = dynamic_cast<AMSyncTreeNodeFolder*>((TreeNodeBase*) sDWItemData);
		if (pSyncNodeFolder)
		{
			TupString folderName = pSyncNodeFolder->GetFileName();
			if (m_autoExclude.RemoveFolderPrefix(folderName))
			{
				if (m_bContextFromLeft)
				{
					TreeNodeFolder *pFolderNode = pSyncNodeFolder->GetLeftFolderNode();
					int result = m_pLeftDataBase->RenameFolder(pFolderNode,folderName);
					if (result!=0)
					{
						
						CString strErrorString;
						strErrorString.Format(IDS_FOLDER_REMOVE_PREFIX_ERROR,pFolderNode->GetFileName());
						::AfxMessageBox((LPCTSTR) strErrorString, MB_OK | MB_ICONEXCLAMATION);
					}
					else
					{
						bNeedToRefresh = true;
					}
				}
				else
				{
					TreeNodeFolder *pFolderNode = pSyncNodeFolder->GetRightFolderNode();
					int result = m_pRightDataBase->RenameFolder(pFolderNode,folderName);
					if (result!=0)
					{
						
						CString strErrorString;
						strErrorString.Format(IDS_FOLDER_REMOVE_PREFIX_ERROR,pFolderNode->GetFileName());
						::AfxMessageBox((LPCTSTR) strErrorString, MB_OK | MB_ICONEXCLAMATION);
					}
					else
					{
						bNeedToRefresh = true;
					}
				}
			}
		}
		AMSyncTreeNodeFile* pSyncNodeFile = dynamic_cast<AMSyncTreeNodeFile*>((TreeNodeBase*) sDWItemData);
		if (pSyncNodeFile)
		{
			TupString fileName = pSyncNodeFile->GetFileName();
			if (m_autoExclude.RemoveFilePrefix(fileName))
			{
				if (m_bContextFromLeft)
				{
					TreeNodeFile *pFileNode = pSyncNodeFile->GetLeftFileNode();
					int result = m_pLeftDataBase->RenameFile(pFileNode,fileName);
					if (result!=0)
					{
						
						CString strErrorString;
						strErrorString.Format(IDS_FILE_REMOVE_PREFIX_ERROR,pFileNode->GetFileName());
						::AfxMessageBox((LPCTSTR) strErrorString, MB_OK | MB_ICONEXCLAMATION);
					}
					else
					{
						bNeedToRefresh = true;
					}
				}
				else
				{
					TreeNodeFile *pFileNode = pSyncNodeFile->GetRightFileNode();
					int result = m_pRightDataBase->RenameFile(pFileNode,fileName);
					if (result!=0)
					{
						
						CString strErrorString;
						strErrorString.Format(IDS_FILE_REMOVE_PREFIX_ERROR,pFileNode->GetFileName());
						::AfxMessageBox((LPCTSTR) strErrorString, MB_OK | MB_ICONEXCLAMATION);
					}
					else
					{
						bNeedToRefresh = true;
					}
				}
			}
		}
		hCurrItem = pTreeControl->GetNextSelectedItem(hCurrItem);
	}
	if (bNeedToRefresh)
	{
		m_bRefreshDatabases = true;
		Invalidate(FALSE);
	}
}

void CAssetManagerDlg::OnContextRename()
{
	CAssetManagerTreeCtrl* pTreeControl;
	
	if (m_bContextFromLeft == true)
	{
		pTreeControl = m_pTreeCtrlLeft;
	}
	else
	{
		pTreeControl = m_pTreeCtrlRight;
	}
	
	HTREEITEM hCurrItem = pTreeControl->GetFirstSelectedItem();
	if (hCurrItem)
	{
		SEC_DWORD sDWItemData = pTreeControl->GetItemData(hCurrItem);
		CEdit *pEdit = pTreeControl->EditLabel(hCurrItem);
	}
}

int static GetWindowsThreadPriority(int priority)
{
	int windowsPriority;
	switch (priority)
	{
		case FolderSet::PRIORITY_REALTIME:
			windowsPriority = THREAD_PRIORITY_TIME_CRITICAL;
			break;
		case FolderSet::PRIORITY_HIGH:
			windowsPriority = THREAD_PRIORITY_HIGHEST;
			break;
		case FolderSet::PRIORITY_ABOVE_NORMAL:
			windowsPriority = THREAD_PRIORITY_ABOVE_NORMAL;
			break;
		case FolderSet::PRIORITY_NORMAL:
			windowsPriority = THREAD_PRIORITY_NORMAL;
			break;
		case FolderSet::PRIORITY_BELOW_NORMAL:
			windowsPriority = THREAD_PRIORITY_BELOW_NORMAL;
			break;
		case FolderSet::PRIORITY_LOW:
			windowsPriority = THREAD_PRIORITY_LOWEST;
			break;
		default:
			windowsPriority = THREAD_PRIORITY_NORMAL;
	}
	return windowsPriority;
}


void CAssetManagerDlg::OnContextUpdateDuplicates()
{
	CAssetManagerTreeCtrl* pTreeControl;
	SyncDBBase *pDatabase;
	
	if (m_bContextFromLeft == true)
	{
		pTreeControl = m_pTreeCtrlLeft;
		pDatabase = m_pLeftDataBase;
	}
	else
	{
		pTreeControl = m_pTreeCtrlRight;
		pDatabase = m_pRightDataBase;
	}

	ASSERT(!m_pUpdateDuplicatesManager);
	int flags = 0;
	if (m_updateDuplicatesIncludeParentNames)
		flags |= UpdateDuplicatesManager::INCLUDE_PARENT_IN_NAME;
	if (m_updateDuplicatesIncludeFrozenDuplicates)
		flags |= UpdateDuplicatesManager::INCLUDE_FROZEN_DUPLICATES;
	if (!m_bContextFromLeft)
		flags |= UpdateDuplicatesManager::RIGHT_SYNC_NODES;

	m_pUpdateDuplicatesManager = new UpdateDuplicatesManager(flags);

	// add all the selected files
	HTREEITEM hCurrItem;
	hCurrItem = pTreeControl->GetFirstSelectedItem();
	while (hCurrItem != NULL)
	{
		SEC_DWORD sDWItemData = pTreeControl->GetItemData(hCurrItem);
		SyncTreeNodeFile* pSyncNodeFile = dynamic_cast<SyncTreeNodeFile*>((TreeNodeBase*) sDWItemData);
		if (pSyncNodeFile)
		{
			m_pUpdateDuplicatesManager->AddSelectedFile(pSyncNodeFile);
		}
		hCurrItem = pTreeControl->GetNextSelectedItem(hCurrItem);
	}
	// add all the selected folders
	hCurrItem = pTreeControl->GetFirstSelectedItem();
	while (hCurrItem != NULL)
	{
		SEC_DWORD sDWItemData = pTreeControl->GetItemData(hCurrItem);
		SyncTreeNodeFolder* pSyncNodeFolder = dynamic_cast<SyncTreeNodeFolder*>((TreeNodeBase*) sDWItemData);
		if (pSyncNodeFolder)
		{
			m_pUpdateDuplicatesManager->AddSelectedFolder(pSyncNodeFolder);
		}
		hCurrItem = pTreeControl->GetNextSelectedItem(hCurrItem);
	}
	m_pUpdateDuplicatesManager->AddDuplicateFiles(m_pSyncTreeManager->GetSyncTree());
	m_pUpdateDuplicatesManager->InitAfterAdd();

	UpdateDuplicatesDialog updateDuplicatesDialog(this);
	updateDuplicatesDialog.SetUpdateDuplicatesManager(m_pUpdateDuplicatesManager);

	// save old accelerator stuff
	CWnd *pOldAccelTarget = theApp.m_pAccelTarget;
	CAssetManagerApp::AccelType oldAccelType = theApp.m_accelType;
	// set accelerator to go to the dialog
	theApp.m_pAccelTarget = &updateDuplicatesDialog;
	theApp.m_accelType = CAssetManagerApp::ACCEL_UPDATE_DUPLICATES;

	int result = updateDuplicatesDialog.DoModal();

	// restore the accelerator stuff
	theApp.m_pAccelTarget = pOldAccelTarget;
	theApp.m_accelType = oldAccelType;

	if (result==TRUE)
	{
		ASSERT(!m_pSyncDialog); // this should not be set...
		// create dialog for showing feedback
		m_pSyncDialog = new SyncDialog; 
		m_pSyncDialog->SetDialogTitle("Updating Duplicates...");
		m_pSyncDialog->m_bUpdateDuplicates = true;

		int activeSet = m_pFolderSets->GetActiveSet();
		int threadPriority = m_pFolderSets->GetSyncThreadPriority(activeSet);
		// the current thread will be wainting in the domodal loop while waiting for the
		// thread that will actually be performing the syncing
		m_pFeedbackThread = AfxBeginThread(UpdateDuplicatesProc,this,GetWindowsThreadPriority(threadPriority)); // start the other thread
		m_pSyncDialog->SetThreadRunning(true);
		m_pSyncDialog->SetThread(m_pFeedbackThread); // pass the thread on to the dialog so it can wait
		m_pSyncDialog->DoModal(); // wait here until the thread is done
		delete m_pSyncDialog; // we are finished with the dialog
		m_pSyncDialog = NULL;
		Refresh();
	}

	delete m_pUpdateDuplicatesManager;
	m_pUpdateDuplicatesManager = NULL;
}

void CAssetManagerDlg::OnContextExplore()
{
	CAssetManagerTreeCtrl* pTreeControl;

	if (m_bContextFromLeft == true)
	{
		pTreeControl = m_pTreeCtrlLeft;
	}
	else
	{
		pTreeControl = m_pTreeCtrlRight;
	}

	bool bFoundPathFile = false;
	bool bFoundPathFolder = false;
	TupString filePath;

	HTREEITEM hCurrItem = pTreeControl->GetFirstSelectedItem();

	SEC_DWORD sDWItemData = pTreeControl->GetItemData(hCurrItem);
	SyncTreeNodeFile* pSyncNodeFile = dynamic_cast<SyncTreeNodeFile*>((TreeNodeBase*) sDWItemData);
	if (pSyncNodeFile)
	{
		if (m_bContextFromLeft == true)
		{
			pSyncNodeFile->GetLeftFileNode()->GetPath(filePath);
		}
		else
		{
			pSyncNodeFile->GetRightFileNode()->GetPath(filePath);
		}
		bFoundPathFile = true;
	}

	SyncTreeNodeFolder* pSyncNodeFolder = dynamic_cast<SyncTreeNodeFolder*>((TreeNodeBase*) sDWItemData);
	if (pSyncNodeFolder)
	{
		if (m_bContextFromLeft == true)
		{
			pSyncNodeFolder->GetLeftFolderNode()->GetPath(filePath);
		}
		else
		{
			pSyncNodeFolder->GetRightFolderNode()->GetPath(filePath);
		}
		bFoundPathFolder = true;
	}
	ASSERT(bFoundPathFile || bFoundPathFolder);
	TupString commandLine;
	

	if (bFoundPathFolder)
	{
		commandLine.Format("/e,""%s""",(const char *)filePath);
	}
	else
	{
		commandLine.Format("/e,/select,""%s""",(const char *)filePath);
	}

	__int64 retValue = (__int64) ShellExecute(NULL, // hwnd
															NULL, // operation
															"explorer", // filepath
															commandLine, // parameters
															NULL, // start directory
															SW_SHOWNORMAL); // show command
	if (retValue<32)
	{
		CString strErrorString;
		strErrorString.Format("Error running Explore %s",(const char *)commandLine);
		::AfxMessageBox((LPCTSTR) strErrorString, MB_OK | MB_ICONEXCLAMATION);
	}
}

void CAssetManagerDlg::OnContextAutoFreezeAdd() 
{
	ChangeAction(SyncTreeManager::ACTION_AUTO_FREEZE_ADD,false);
	ChangeAction(SyncTreeManager::ACTION_DEFAULT_ACTION); // update tree here
}

void CAssetManagerDlg::OnContextAutoFreezeRemove() 
{
	ChangeAction(SyncTreeManager::ACTION_UNFREEZE,false);
	ChangeAction(SyncTreeManager::ACTION_AUTO_FREEZE_REMOVE,false);
	ChangeAction(SyncTreeManager::ACTION_DEFAULT_ACTION); // update tree here
}

void CAssetManagerDlg::OnContextAutoFreezeRemoveIncludeChildren() 
{
	ChangeAction(SyncTreeManager::ACTION_UNFREEZE,false);
	ChangeAction(SyncTreeManager::ACTION_AUTO_FREEZE_REMOVE_INCLUDE_CHILDREN,false);
	ChangeAction(SyncTreeManager::ACTION_DEFAULT_ACTION); // update tree here
}

bool CAssetManagerDlg::CheckAction(int checkFlags)
{
	if (GetMode() != POSTCOMPARE)
		return false;

	CWnd *pFocusWnd = GetFocus();
	if (pFocusWnd && ::IsWindow(pFocusWnd->m_hWnd))
	{
		if (pFocusWnd == m_pTreeCtrlLeft)
		{
			m_bContextFromLeft=true;
		}
		else if (pFocusWnd == m_pTreeCtrlRight)
		{
			m_bContextFromLeft=false;
		}
		else
		{
			return false; // if a tree does not have the focus
		}
		unsigned long validActionFlags = GetValidActions();
		return (validActionFlags & checkFlags)!=0;
	}
	else
	{
		return false;
	}
}

void CAssetManagerDlg::OnUpdateSetDefault(CCmdUI* pCmdUI) 
{
	if (CheckAction(SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_SET_DEFAULT))
		pCmdUI -> SetCheck (TRUE);
	else
		pCmdUI -> SetCheck (FALSE);
}

void CAssetManagerDlg::OnUpdateExclude(CCmdUI* pCmdUI) 
{
	if (CheckAction(SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXCLUDE))
		pCmdUI -> SetCheck (TRUE);
	else
		pCmdUI -> SetCheck (FALSE);
}

void CAssetManagerDlg::OnUpdateCopyToLeft(CCmdUI* pCmdUI) 
{
	if (CheckAction(SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_RIGHT_TO_LEFT))
		pCmdUI -> SetCheck (TRUE);
	else
		pCmdUI -> SetCheck (FALSE);
}

void CAssetManagerDlg::OnUpdateCopyToRight(CCmdUI* pCmdUI) 
{
	if (CheckAction(SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_LEFT_TO_RIGHT))
		pCmdUI -> SetCheck (TRUE);
	else
		pCmdUI -> SetCheck (FALSE);
}

void CAssetManagerDlg::OnUpdateFreeze(CCmdUI* pCmdUI) 
{
	if (CheckAction(SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_FREEZE))
		pCmdUI -> SetCheck (TRUE);
	else
		pCmdUI -> SetCheck (FALSE);
}

void CAssetManagerDlg::OnUpdateUnfreeze(CCmdUI* pCmdUI) 
{
	if (CheckAction(SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_UNFREEZE))
		pCmdUI -> SetCheck (TRUE);
	else
		pCmdUI -> SetCheck (FALSE);
}

void CAssetManagerDlg::OnUpdateUnfreezeOnly(CCmdUI* pCmdUI) 
{
	if (CheckAction(SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_UNFREEZE_ONLY))
		pCmdUI -> SetCheck (TRUE);
	else
		pCmdUI -> SetCheck (FALSE);
}

void CAssetManagerDlg::OnUpdateAutoFreezeAdd(CCmdUI* pCmdUI) 
{
	if (CheckAction(SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_ADD_AUTO_FREEZE))
		pCmdUI -> SetCheck (TRUE);
	else
		pCmdUI -> SetCheck (FALSE);
}

void CAssetManagerDlg::OnUpdateDeleteFromLeft(CCmdUI* pCmdUI) 
{
	if (CheckAction(SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_LEFT))
		pCmdUI -> SetCheck (TRUE);
	else
		pCmdUI -> SetCheck (FALSE);
}

void CAssetManagerDlg::OnUpdateDeleteFromRight(CCmdUI* pCmdUI) 
{
	if (CheckAction(SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_RIGHT))
		pCmdUI -> SetCheck (TRUE);
	else
		pCmdUI -> SetCheck (FALSE);
}

void CAssetManagerDlg::OnUpdateDeleteFromBoth(CCmdUI* pCmdUI) 
{
	if (CheckAction(SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_BOTH))
		pCmdUI -> SetCheck (TRUE);
	else
		pCmdUI -> SetCheck (FALSE);
}

void CAssetManagerDlg::OnUpdateSelectAll(CCmdUI* pCmdUI) 
{
	if (CheckAction(SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_SELECT_ALL))
		pCmdUI -> SetCheck (TRUE);
	else
		pCmdUI -> SetCheck (FALSE);
}

void CAssetManagerDlg::OnUpdateSelectNone(CCmdUI* pCmdUI) 
{
	if (CheckAction(SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_SELECT_NONE))
		pCmdUI -> SetCheck (TRUE);
	else
		pCmdUI -> SetCheck (FALSE);
}

void CAssetManagerDlg::OnUpdateInvertSelection(CCmdUI* pCmdUI) 
{
	if (CheckAction(SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_INVERT_SELECTION))
		pCmdUI -> SetCheck (TRUE);
	else
		pCmdUI -> SetCheck (FALSE);
}

void CAssetManagerDlg::OnUpdateExpandAll(CCmdUI* pCmdUI) 
{
	if (CheckAction(SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXPAND_ALL))
		pCmdUI -> SetCheck (TRUE);
	else
		pCmdUI -> SetCheck (FALSE);
}

void CAssetManagerDlg::OnUpdateCollapseAll(CCmdUI* pCmdUI) 
{
	if (CheckAction(SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COLLAPSE_ALL))
		pCmdUI -> SetCheck (TRUE);
	else
		pCmdUI -> SetCheck (FALSE);
}

void CAssetManagerDlg::OnUpdateAddPrefix(CCmdUI* pCmdUI) 
{
	if (CheckAction(SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_ADD_PREFIX))
		pCmdUI -> SetCheck (TRUE);
	else
		pCmdUI -> SetCheck (FALSE);
}

void CAssetManagerDlg::OnUpdateRemovePrefix(CCmdUI* pCmdUI) 
{
	if (CheckAction(SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_REMOVE_PREFIX))
		pCmdUI -> SetCheck (TRUE);
	else
		pCmdUI -> SetCheck (FALSE);
}

void CAssetManagerDlg::OnUpdateRename(CCmdUI* pCmdUI) 
{
	if (CheckAction(SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_RENAME))
		pCmdUI -> SetCheck (TRUE);
	else
		pCmdUI -> SetCheck (FALSE);
}

void CAssetManagerDlg::OnUpdateUpdateDuplicates(CCmdUI* pCmdUI) 
{
	if (CheckAction(SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_UPDATE_DUPLICATES))
		pCmdUI -> SetCheck (TRUE);
	else
		pCmdUI -> SetCheck (FALSE);
}

void CAssetManagerDlg::OnUpdateExplore(CCmdUI* pCmdUI) 
{
	if (CheckAction(SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXPLORE))
		pCmdUI -> SetCheck (TRUE);
	else
		pCmdUI -> SetCheck (FALSE);
}


//-----------------------------------------------------------------------------
// CAssetManagerDlg::RemoveStrFromCombo
// This method can be called to remove any items in the incoming combobox that
// match the incoming string.  It will also remove any blank entries.
//
// Input:
//  CComboBox& comboBox -- The combobox to remove items from.
//  CString strToRemove -- The string to look for.
//
// Output: None.
//-----------------------------------------------------------------------------
void CAssetManagerDlg::RemoveStrFromCombo(CComboBox& comboBox, CString strToRemove)
{
	CString strCurrItem;
	int nNumItems = comboBox.GetCount();
	for (int nIndex = nNumItems - 1; nIndex >= 0; nIndex--)
	{
		comboBox.GetLBText(nIndex, strCurrItem);
		if ((strCurrItem.CompareNoCase(strToRemove) == 0) || (strCurrItem == ""))
			comboBox.DeleteString(nIndex);
	}
}


//-----------------------------------------------------------------------------
// CAssetManagerDlg::ChangeAction
// This method is called to switch the action in the sync tree to the one
// passed in.  It will also change the images in the indicator tree accordingly.
//
// Input:
//  unsigned long ulNewAction -- The new action to switch to.
//
// Output: None.
//-----------------------------------------------------------------------------
void CAssetManagerDlg::ChangeAction(SyncTreeManager::Actions newAction,bool bUpdateTreeItems)
{
	// Only do this if there are two trees:
	if (GetMode() == POSTCOMPARE)
	{
		// Check to see which tree items we need to change:
		CAssetManagerTreeCtrl* pTreeControl;
		if (m_bContextFromLeft == true)
			pTreeControl = m_pTreeCtrlLeft;
		else
			pTreeControl = m_pTreeCtrlRight;

		// Now change the items:
		HTREEITEM hCurrItem = pTreeControl->GetFirstSelectedItem();
		while (hCurrItem != NULL)
		{
			SEC_DWORD sDWItemData = pTreeControl->GetItemData(hCurrItem);
			AMSyncTreeNodeFolder* pSyncNodeFolder = dynamic_cast<AMSyncTreeNodeFolder*>((TreeNodeBase*) sDWItemData);
			if (pSyncNodeFolder != NULL)
			{
				// Change the action flags:
				m_pSyncTreeManager->SetAction(pSyncNodeFolder, newAction, false, true);
			}
			else
			{
				AMSyncTreeNodeFile* pSyncNodeFile = dynamic_cast<AMSyncTreeNodeFile*>((TreeNodeBase*) sDWItemData);
				if (pSyncNodeFile != NULL)
				{
					// Change the action flags:
					m_pSyncTreeManager->SetAction(pSyncNodeFile, newAction, false, true);
				}
				else
				{
					// Should be either a file or folder!
					ASSERT(false);
				}				
			}
			hCurrItem = pTreeControl->GetNextSelectedItem(hCurrItem);
		}
		if (bUpdateTreeItems)
			UpdateTreeItems();
	}
}


//-----------------------------------------------------------------------------
// CAssetManagerDlg::LoadTreeControl
// This method can be called to load the left, right, and indicator trees based
// on the TreeNodeBase pointer coming in.
//
// Input:
//  CAssetManagerTreeCtrl* pTreeCtrlLeft -- The left tree control.
//  CAssetManagerTreeCtrl* pTreeCtrlRight -- The right tree control.
//  CAssetManagerTreeCtrl* pTreeCtrlIndicators -- The middle indicator
//												  control.
//  TreeNodeBase* pTreeNodeBase -- The sync tree to get data from.
//
// Output: None.
//-----------------------------------------------------------------------------
void CAssetManagerDlg::LoadTreeControl(CAssetManagerTreeCtrl* pTreeCtrlLeft, CAssetManagerTreeCtrl* pTreeCtrlRight,
									   CAssetManagerTreeCtrl* pTreeCtrlIndicators, TreeNodeBase* pTreeNodeBase)
{
	// Make sure the base is a sync folder:
	AMSyncTreeNodeFolder* pAMSyncNodeFolder = dynamic_cast<AMSyncTreeNodeFolder*>(pTreeNodeBase);
	if (pAMSyncNodeFolder != NULL)
	{
		// Remove everything from the tree controls:
		SaveFolderExpandState();
		SaveTreeSelectedState();

		int topIndex = 0;
		CWnd *pFocusWnd = GetFocus();
		if (pFocusWnd && ::IsWindow(pFocusWnd->m_hWnd))
		{
			if (pFocusWnd == m_pTreeCtrlLeft)
			{
				topIndex = m_pTreeCtrlLeft->GetTopIndex();
			}
			else if (pFocusWnd == m_pTreeCtrlRight)
			{
				topIndex = m_pTreeCtrlRight->GetTopIndex();
			}
		}

		pTreeCtrlLeft->SetAllowPaint(false);
		pTreeCtrlRight->SetAllowPaint(false);
		pTreeCtrlIndicators->SetAllowPaint(false);

		pTreeCtrlLeft->DeleteAllItems();
		pTreeCtrlRight->DeleteAllItems();
		pTreeCtrlIndicators->DeleteAllItems();
		m_pAssociatedIconManager->ClearAll();

		m_pTreeCtrlLeft->SetNodifyExpanded(false);
		m_pTreeCtrlRight->SetNodifyExpanded(false);
		m_pTreeCtrlIndicators->SetNodifyExpanded(false);

		// We have a sync folder:
		TreeIterator first(pAMSyncNodeFolder);
		TreeIterator last(pAMSyncNodeFolder);
		last.End();

		bool bFoundSelected = false;

		// Iterate through all the children of this folder:
		for ( ; first < last; first.Increment())
		{
			LoadTreeItem(pTreeCtrlLeft, pTreeCtrlRight, pTreeCtrlIndicators, (*first),
						 TVI_ROOT, TVI_ROOT, TVI_ROOT,bFoundSelected);
		}
		pTreeCtrlLeft->SetTopIndex(topIndex);
		pTreeCtrlRight->SetTopIndex(topIndex);
		pTreeCtrlIndicators->SetTopIndex(topIndex);
		pTreeCtrlLeft->RecalcScrollBars();
		pTreeCtrlRight->RecalcScrollBars();
		pTreeCtrlIndicators->RecalcScrollBars();

		m_pTreeCtrlLeft->SetNodifyExpanded(true);
		m_pTreeCtrlRight->SetNodifyExpanded(true);
		m_pTreeCtrlIndicators->SetNodifyExpanded(true);

		pTreeCtrlLeft->SetAllowPaint(true);
		pTreeCtrlRight->SetAllowPaint(true);
		pTreeCtrlIndicators->SetAllowPaint(true);
	}
	else
	{
		pTreeCtrlLeft->DeleteAllItems();
		pTreeCtrlRight->DeleteAllItems();
		pTreeCtrlIndicators->DeleteAllItems();
	}
}


//-----------------------------------------------------------------------------
// CAssetManagerDlg::LoadTreeItem
// This method can be called to load the left, right, and indicator trees items
// based on the position in the TreeNodeBase pointer coming in.
//
// Input:
//  CAssetManagerTreeCtrl* pTreeCtrlLeft -- The left tree control.
//  CAssetManagerTreeCtrl* pTreeCtrlRight -- The right tree control.
//  CAssetManagerTreeCtrl* pTreeCtrlIndicators -- The middle indicator
//												  control.
//  TreeNodeBase* pTreeNodeBase -- The sync tree to get data from.
//  HTREEITEM hItemLeftParent -- The HTREEITEM parent of the new item created
//								 in the left control.
//  HTREEITEM hItemRightParent -- The HTREEITEM parent of the new item created
//								  in the right control.
//  HTREEITEM hItemIndicatorParent -- The HTREEITEM parent of the new item
//									  created in the indicator control.
//
// Output: None.
//-----------------------------------------------------------------------------
void CAssetManagerDlg::LoadTreeItem(CAssetManagerTreeCtrl* pTreeCtrlLeft, CAssetManagerTreeCtrl* pTreeCtrlRight,
									CAssetManagerTreeCtrl* pTreeCtrlIndicators, TreeNodeBase* pTreeNodeBase,
									HTREEITEM hItemLeftParent, HTREEITEM hItemRightParent, HTREEITEM hItemIndicatorParent,
									bool &bFoundSelected)
{
	// See if the base is a sync folder:
	AMSyncTreeNodeFolder* pAMSyncNodeFolder = dynamic_cast<AMSyncTreeNodeFolder*>(pTreeNodeBase);
	if (pAMSyncNodeFolder != NULL)
	{
		// We have a sync folder.
		// Figure out the image indexes and text to use for the left and right trees :
		CString strLeftText;
		CString strRightText;
		int nLeftImage=0;
		int nRightImage=0;
		bool bChildAction = false;
		bool bFrozen = false;
		bool bExcluded = false;
		bool bLeftCopySource = false;
		bool bLeftCopyDest = false;
		bool bLeftDelete = false;
		bool bRightCopySource = false;
		bool bRightCopyDest = false;
		bool bRightDelete = false;
		bool bExpanded = false;
		bool bSelected = false;
		bool bFocus = false;
		bool bTreeHasFocus = false;
		bool bLeftTree;
		bool bVisible = false;
		bool bFrozenPath = false;

		CWnd *pFocusWnd = GetFocus();
		if (pFocusWnd && ::IsWindow(pFocusWnd->m_hWnd))
		{
			if (pFocusWnd == m_pTreeCtrlLeft)
			{
				bTreeHasFocus=true;
				bLeftTree = true;
			}
			else if (pFocusWnd == m_pTreeCtrlRight)
			{
				bTreeHasFocus=true;
				bLeftTree = false;
			}
		}

		if (pAMSyncNodeFolder->GetFlags() & SyncTreeNodeFlags::FLAGS_FROZEN)
			bFrozen = true;

		if (pAMSyncNodeFolder->GetFlags() & SyncTreeNodeFlags::FLAGS_CHILD_HAS_ACTION)
			bChildAction = true;

		if (pAMSyncNodeFolder->GetFlags() & SyncTreeNodeFlags::FLAGS_EXPANDED)
			bExpanded = true;

		if (pAMSyncNodeFolder->GetFlags() & SyncTreeNodeFlags::FLAGS_SELECTED)
			bSelected = true;

		if (pAMSyncNodeFolder->GetFlags() & SyncTreeNodeFlags::FLAGS_FOCUS)
			bFocus = true;

		if (pAMSyncNodeFolder->GetFlags() & SyncTreeNodeFlags::FLAGS_VISIBLE)
			bVisible = true;

		if (pAMSyncNodeFolder->GetFlags() & SyncTreeNodeFlags::FLAGS_PATH_AUTO_FROZEN_PATH)
			bFrozenPath = true;


		int indicatorStatusIndex =0;

		switch (pAMSyncNodeFolder->GetFlags() & SyncTreeNodeFlags::FLAGS_STATUS_MASK)
		{
			case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY:
			{
				strLeftText += pAMSyncNodeFolder->GetFileName();
				strRightText += " ";
				nLeftImage = bFrozenPath ? IMG_NORM_IDX_FOLDER_FROZEN_PATH : IMG_NORM_IDX_FOLDER;
				nRightImage = IMG_NORM_IDX_BLANK;
				indicatorStatusIndex = IMG_IND_IDX_LEFT_ONLY;
				break;
			}
			case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY:
			{
				strLeftText += " ";
				strRightText += pAMSyncNodeFolder->GetFileName();
				nLeftImage = IMG_NORM_IDX_BLANK;
				nRightImage = bFrozenPath ? IMG_NORM_IDX_FOLDER_FROZEN_PATH : IMG_NORM_IDX_FOLDER;
				indicatorStatusIndex = IMG_IND_IDX_RIGHT_ONLY;
				break;
			}
			case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL:
			{
				strLeftText += pAMSyncNodeFolder->GetFileName();
				strRightText += pAMSyncNodeFolder->GetFileName();
				nLeftImage = nRightImage = bFrozenPath ? IMG_NORM_IDX_FOLDER_FROZEN_PATH : IMG_NORM_IDX_FOLDER;
				indicatorStatusIndex = IMG_IND_IDX_EQUAL;
				break;
			}
		}

		// Figure out the image indexes to use for the indicator tree:
		int nIndImage;
		if (bFrozen)
		{
			nIndImage = IMG_IND_IDX_FROZEN + indicatorStatusIndex;
		}
		else
		{
			switch (pAMSyncNodeFolder->GetFlags() & SyncTreeNodeFlags::FLAGS_ACTION_MASK)
			{
				case SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED:
				{
					if (bChildAction)
					{
						nIndImage = IMG_IND_IDX_CHILD_ACTION;
					}
					else
					{
						nIndImage = IMG_IND_IDX_EXCLUDED + indicatorStatusIndex;
						bExcluded = true;
					}
					break;
				}
				case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_LEFT:
				{
					bLeftDelete = true;
					nIndImage = IMG_IND_IDX_DEL_LEFT + indicatorStatusIndex;
					break;
				}
				case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_RIGHT:
				{
					bRightDelete = true;
					nIndImage = IMG_IND_IDX_DEL_RIGHT + indicatorStatusIndex;
					break;
				}
				case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_BOTH:
				{
					bLeftDelete = true;
					bRightDelete = true;
					nIndImage = IMG_IND_IDX_DEL_BOTH + indicatorStatusIndex;
					break;
				}
				case SyncTreeNodeFlags::FLAGS_ACTION_COPY_LEFT_TO_RIGHT:
				{
					bLeftCopySource = true;
					bRightCopyDest = true;
					nIndImage = IMG_IND_IDX_COPY_RIGHT + indicatorStatusIndex;
					break;
				}
				case SyncTreeNodeFlags::FLAGS_ACTION_COPY_RIGHT_TO_LEFT:
				{
					bLeftCopyDest = true;
					bRightCopySource = true;
					nIndImage = IMG_IND_IDX_COPY_LEFT + indicatorStatusIndex;
					break;
				}
			}
		}

		int stateFlags = bExpanded ? TVIS_EXPANDED : 0;
		int stateMask = TVIS_EXPANDED;

		// Create the items in the three trees:
		HTREEITEM hItemNewLeftPar = pTreeCtrlLeft->InsertItem(TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_HANDLE | TVIF_PARAM | TVIF_HANDLE | TVIF_STATE,
			strLeftText, nLeftImage, nLeftImage, stateFlags, stateMask,
															  (LPARAM) pAMSyncNodeFolder,
															  hItemLeftParent, TVI_LAST);
		{ 
			TV_ITEM item;
			TV_ITEM_EX itemEx;

			item.mask = TVIF_HANDLE;
			item.hItem = hItemNewLeftPar;

			itemEx.maskEx = TVIF_EX_STATEEX;
			itemEx.stateEx = 0;
			if (bFrozen)
				itemEx.stateEx |= TVIS_EX_FROZEN;
			if (bExcluded)
				itemEx.stateEx |= TVIS_EX_EXCLUDED;
			if (bLeftDelete)
				itemEx.stateEx |= TVIS_EX_TO_DELETE;
			if (bLeftCopySource)
				itemEx.stateEx |= TVIS_EX_COPY_SOURCE;
			if (bLeftCopyDest)
				itemEx.stateEx |= TVIS_EX_COPY_DEST;

			itemEx.stateExMask = TVIS_EX_ASSOCIATED_ICON|
								 TVIS_EX_FROZEN|
								 TVIS_EX_EXCLUDED|
								 TVIS_EX_TO_DELETE|
								 TVIS_EX_COPY_SOURCE|
								 TVIS_EX_COPY_DEST;

			pTreeCtrlLeft->SetItemEx( &item, &itemEx );
			pTreeCtrlLeft->HideItem(hItemNewLeftPar,bVisible ? FALSE : TRUE);
		}

		if (bTreeHasFocus && bLeftTree && bSelected)
		{
			pTreeCtrlLeft->SelectItemNew(hItemNewLeftPar,!bFoundSelected,bFocus);
			bFoundSelected = true; // keep track of when we need to delect all others (only first time)
		}

		stateFlags = bExpanded ? TVIS_EXPANDED : 0;
		stateMask = TVIS_EXPANDED;

		HTREEITEM hItemNewRightPar = pTreeCtrlRight->InsertItem(TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_HANDLE | TVIF_PARAM | TVIF_HANDLE | TVIF_STATE,
																strRightText, nRightImage, nRightImage, stateFlags, stateMask,
																(LPARAM) pAMSyncNodeFolder,
																hItemRightParent, TVI_LAST);
		{ 
			TV_ITEM item;
			TV_ITEM_EX itemEx;

			item.mask = TVIF_HANDLE;
			item.hItem = hItemNewRightPar;

			itemEx.maskEx = TVIF_EX_STATEEX;
			itemEx.stateEx = 0;
			if (bFrozen)
				itemEx.stateEx |= TVIS_EX_FROZEN;
			if (bExcluded)
				itemEx.stateEx |= TVIS_EX_EXCLUDED;
			if (bRightDelete)
				itemEx.stateEx |= TVIS_EX_TO_DELETE;
			if (bRightCopySource)
				itemEx.stateEx |= TVIS_EX_COPY_SOURCE;
			if (bRightCopyDest)
				itemEx.stateEx |= TVIS_EX_COPY_DEST;

			itemEx.stateExMask = TVIS_EX_FROZEN|
								 TVIS_EX_EXCLUDED|
								 TVIS_EX_TO_DELETE|
								 TVIS_EX_COPY_SOURCE|
								 TVIS_EX_COPY_DEST;

			pTreeCtrlRight->SetItemEx( &item, &itemEx );
			pTreeCtrlRight->HideItem(hItemNewRightPar,bVisible ? FALSE : TRUE);
		}

		if (bTreeHasFocus && !bLeftTree && bSelected)
		{
			pTreeCtrlRight->SelectItemNew(hItemNewRightPar,!bFoundSelected,bFocus);
			bFoundSelected = true; // keep track of when we need to delect all others (only first time)
		}

		HTREEITEM hItemNewIndicatorPar = pTreeCtrlIndicators->InsertItem(TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_HANDLE | TVIF_PARAM | TVIF_HANDLE | TVIF_STATE,
																		 "", nIndImage, nIndImage, stateFlags, stateMask,
																		 (LPARAM) pAMSyncNodeFolder,
																		 hItemIndicatorParent, TVI_LAST);
		{ 
			pTreeCtrlIndicators->HideItem(hItemNewIndicatorPar,bVisible ? FALSE : TRUE);
		}

		pAMSyncNodeFolder->SetHTreeItemLeft(hItemNewLeftPar);
		pAMSyncNodeFolder->SetHTreeItemRight(hItemNewRightPar);
		pAMSyncNodeFolder->SetHTreeItemIndicator(hItemNewIndicatorPar);

		// Iterate through the children of this folder:
		TreeIterator first(pAMSyncNodeFolder);
		TreeIterator last(pAMSyncNodeFolder);
		last.End();

		// Iterate through all the children of this folder:
		for ( ; first < last; first.Increment())
		{
			LoadTreeItem(pTreeCtrlLeft, pTreeCtrlRight, pTreeCtrlIndicators, (*first),
						 hItemNewLeftPar, hItemNewRightPar, hItemNewIndicatorPar,bFoundSelected);
		}
		return;
	}

	// See if the base is a sync file:
	AMSyncTreeNodeFile* pAMSyncNodeFile = dynamic_cast<AMSyncTreeNodeFile*>(pTreeNodeBase);
	if (pAMSyncNodeFile != NULL)
	{
		// We have a sync file.
		// Figure out the image indexes and text to use for the left and right trees :
		CString strLeftText;
		CString strRightText;
		int nLeftImage=0;
		int nRightImage=0;
		bool bFrozen = false;
		bool bExcluded = false;
		bool bLeftCopySource = false;
		bool bLeftCopyDest = false;
		bool bLeftDelete = false;
		bool bRightCopySource = false;
		bool bRightCopyDest = false;
		bool bRightDelete = false;
		bool bSelected = false;
		bool bFocus = false;
		bool bTreeHasFocus = false;
		bool bVisible = false;
		bool bLeftTree;
		bool bLeftAssociatedIcon = false;
		bool bRightAssociatedIcon = false;
		bool bFrozenPath = false;
		
		CWnd *pFocusWnd = GetFocus();
		if (pFocusWnd && ::IsWindow(pFocusWnd->m_hWnd))
		{
			if (pFocusWnd == m_pTreeCtrlLeft)
			{
				bTreeHasFocus=true;
				bLeftTree = true;
			}
			else if (pFocusWnd == m_pTreeCtrlRight)
			{
				bTreeHasFocus=true;
				bLeftTree = false;
			}
		}

		if (pAMSyncNodeFile->GetFlags() & SyncTreeNodeFlags::FLAGS_FROZEN)
			bFrozen = true;

		if (pAMSyncNodeFile->GetFlags() & SyncTreeNodeFlags::FLAGS_SELECTED)
			bSelected = true;

		if (pAMSyncNodeFile->GetFlags() & SyncTreeNodeFlags::FLAGS_FOCUS)
			bFocus = true;

		if (pAMSyncNodeFile->GetFlags() & SyncTreeNodeFlags::FLAGS_VISIBLE)
			bVisible = true;

		if (pAMSyncNodeFile->GetFlags() & SyncTreeNodeFlags::FLAGS_PATH_AUTO_FROZEN_PATH)
			bFrozenPath = true;

		char extString[_MAX_EXT];
		_splitpath(pAMSyncNodeFile->GetFileName(),NULL,NULL,NULL,extString);

		int associatedIconIndex = m_pAssociatedIconManager->GetIconIndex(extString);

		int indicatorStatusIndex = 0;

		switch (pAMSyncNodeFile->GetFlags() & SyncTreeNodeFlags::FLAGS_STATUS_MASK)
		{
			case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY:
			{
				strLeftText += pAMSyncNodeFile->GetFileName();
				strRightText += " ";
				if (bFrozenPath)
				{
					nLeftImage = IMG_NORM_IDX_FILE_FROZEN_PATH;
				}
				else
				{
					nLeftImage = associatedIconIndex;
					bLeftAssociatedIcon = true;
				}
				nRightImage = IMG_NORM_IDX_BLANK;
				indicatorStatusIndex = IMG_IND_IDX_LEFT_ONLY;
				break;
			}
			case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY:
			{
				strLeftText += " ";
				strRightText += pAMSyncNodeFile->GetFileName();
				if (bFrozenPath)
				{
					nRightImage = IMG_NORM_IDX_FILE_FROZEN_PATH;
				}
				else
				{
					nRightImage = associatedIconIndex;
					bRightAssociatedIcon = true;
				}
				nLeftImage = IMG_NORM_IDX_BLANK;
				indicatorStatusIndex = IMG_IND_IDX_RIGHT_ONLY;
				break;
			}
			case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL:
			{
				strLeftText += pAMSyncNodeFile->GetFileName();
				strRightText += pAMSyncNodeFile->GetFileName();
				if (bFrozenPath)
				{
					nLeftImage = IMG_NORM_IDX_FILE_FROZEN_PATH;
					nRightImage = IMG_NORM_IDX_FILE_FROZEN_PATH;
				}
				else
				{
					nLeftImage = associatedIconIndex;
					nRightImage = associatedIconIndex;
					bLeftAssociatedIcon = true;
					bRightAssociatedIcon = true;
				}
				indicatorStatusIndex = IMG_IND_IDX_EQUAL;
				break;
			}
			case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_NEWER:
			{
				strLeftText += pAMSyncNodeFile->GetFileName();
				strRightText += pAMSyncNodeFile->GetFileName();
				if (bFrozenPath)
				{
					nLeftImage = IMG_NORM_IDX_FILE_FROZEN_PATH;
					nRightImage = IMG_NORM_IDX_FILE_FROZEN_PATH;
				}
				else
				{
					nLeftImage = associatedIconIndex;
					nRightImage = associatedIconIndex;
					bLeftAssociatedIcon = true;
					bRightAssociatedIcon = true;
				}
				indicatorStatusIndex = IMG_IND_IDX_LEFT_NEWER;
				break;
			}
			case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_NEWER:
			{
				strLeftText += pAMSyncNodeFile->GetFileName();
				strRightText += pAMSyncNodeFile->GetFileName();
				if (bFrozenPath)
				{
					nLeftImage = IMG_NORM_IDX_FILE_FROZEN_PATH;
					nRightImage = IMG_NORM_IDX_FILE_FROZEN_PATH;
				}
				else
				{
					nLeftImage = associatedIconIndex;
					nRightImage = associatedIconIndex;
					bLeftAssociatedIcon = true;
					bRightAssociatedIcon = true;
				}
				indicatorStatusIndex = IMG_IND_IDX_RIGHT_NEWER;
				break;
			}
		}

		// Figure out the image indexes to use for the indicator tree:
		int nIndImage;
		if (bFrozen)
		{
			nIndImage = IMG_IND_IDX_FROZEN + indicatorStatusIndex;
		}
		else
		{
			switch (pAMSyncNodeFile->GetFlags() & SyncTreeNodeFlags::FLAGS_ACTION_MASK)
			{
				case SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED:
				{
					nIndImage = IMG_IND_IDX_EXCLUDED + indicatorStatusIndex;
					bExcluded = true;
					break;
				}
				case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_LEFT:
				{
					bLeftDelete = true;
					nIndImage = IMG_IND_IDX_DEL_LEFT + indicatorStatusIndex;
					break;
				}
				case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_RIGHT:
				{
					bRightDelete = true;
					nIndImage = IMG_IND_IDX_DEL_RIGHT + indicatorStatusIndex;
					break;
				}
				case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_BOTH:
				{
					bLeftDelete = true;
					bRightDelete = true;
					nIndImage = IMG_IND_IDX_DEL_BOTH + indicatorStatusIndex;
					break;
				}
				case SyncTreeNodeFlags::FLAGS_ACTION_COPY_LEFT_TO_RIGHT:
				{
					bLeftCopySource = true;
					bRightCopyDest = true;
					nIndImage = IMG_IND_IDX_COPY_RIGHT + indicatorStatusIndex;
					break;
				}
				case SyncTreeNodeFlags::FLAGS_ACTION_COPY_RIGHT_TO_LEFT:
				{
					bLeftCopyDest = true;
					bRightCopySource = true;
					nIndImage = IMG_IND_IDX_COPY_LEFT + indicatorStatusIndex;
					break;
				}
			}
		}

		// Create the items in the three trees:
		HTREEITEM hItemNewLeftPar = pTreeCtrlLeft->InsertItem(TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_HANDLE | TVIF_PARAM | TVIF_HANDLE | TVIF_STATE,
															  strLeftText, nLeftImage, nLeftImage, 0, 0,
															  (LPARAM) pAMSyncNodeFile,
															  hItemLeftParent, TVI_LAST);

		{ 
			TV_ITEM item;
			TV_ITEM_EX itemEx;

			item.mask = TVIF_HANDLE;
			item.hItem = hItemNewLeftPar;

			itemEx.maskEx = TVIF_EX_STATEEX;
			itemEx.stateEx = 0;
			if (bFrozen)
				itemEx.stateEx |= TVIS_EX_FROZEN;
			if (bExcluded)
				itemEx.stateEx |= TVIS_EX_EXCLUDED;
			if (bLeftDelete)
				itemEx.stateEx |= TVIS_EX_TO_DELETE;
			if (bLeftCopySource)
				itemEx.stateEx |= TVIS_EX_COPY_SOURCE;
			if (bLeftCopyDest)
				itemEx.stateEx |= TVIS_EX_COPY_DEST;
			if (bLeftAssociatedIcon)
				itemEx.stateEx |= TVIS_EX_ASSOCIATED_ICON;
			
			itemEx.stateExMask = TVIS_EX_ASSOCIATED_ICON|
								 TVIS_EX_FROZEN|
								 TVIS_EX_EXCLUDED|
								 TVIS_EX_TO_DELETE|
								 TVIS_EX_COPY_SOURCE|
								 TVIS_EX_COPY_DEST;

			pTreeCtrlLeft->SetItemEx( &item, &itemEx );
			pTreeCtrlLeft->HideItem(hItemNewLeftPar,bVisible ? FALSE : TRUE);
		}

		if (bTreeHasFocus && bLeftTree && bSelected)
		{
			pTreeCtrlLeft->SelectItemNew(hItemNewLeftPar,!bFoundSelected,bFocus);
			bFoundSelected = true; // keep track of when we need to delect all others (only first time)
		}


		HTREEITEM hItemNewRightPar = pTreeCtrlRight->InsertItem(TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_HANDLE | TVIF_PARAM | TVIF_HANDLE | TVIF_STATE,
																strRightText, nRightImage, nRightImage, 0, 0,
																(LPARAM) pAMSyncNodeFile,
																hItemRightParent, TVI_LAST);

		{ 
			TV_ITEM item;
			TV_ITEM_EX itemEx;

			item.mask = TVIF_HANDLE;
			item.hItem = hItemNewRightPar;

			itemEx.maskEx = TVIF_EX_STATEEX;
			itemEx.stateEx = 0;
			if (bFrozen)
				itemEx.stateEx |= TVIS_EX_FROZEN;
			if (bExcluded)
				itemEx.stateEx |= TVIS_EX_EXCLUDED;
			if (bRightDelete)
				itemEx.stateEx |= TVIS_EX_TO_DELETE;
			if (bRightCopySource)
				itemEx.stateEx |= TVIS_EX_COPY_SOURCE;
			if (bRightCopyDest)
				itemEx.stateEx |= TVIS_EX_COPY_DEST;
			if (bRightAssociatedIcon)
				itemEx.stateEx |= TVIS_EX_ASSOCIATED_ICON;
			
			itemEx.stateExMask = TVIS_EX_ASSOCIATED_ICON|
								 TVIS_EX_FROZEN|
								 TVIS_EX_EXCLUDED|
								 TVIS_EX_TO_DELETE|
								 TVIS_EX_COPY_SOURCE|
								 TVIS_EX_COPY_DEST;

			pTreeCtrlRight->SetItemEx( &item, &itemEx );
			pTreeCtrlRight->HideItem(hItemNewRightPar,bVisible ? FALSE : TRUE);
		}

		if (bTreeHasFocus && !bLeftTree && bSelected)
		{
			pTreeCtrlRight->SelectItemNew(hItemNewRightPar,!bFoundSelected,bFocus);
			bFoundSelected = true; // keep track of when we need to delect all others (only first time)
		}


		HTREEITEM hItemNewIndicatorPar = pTreeCtrlIndicators->InsertItem(TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_HANDLE | TVIF_PARAM | TVIF_HANDLE,
																		 "", nIndImage, nIndImage, 0, 0,
																		 (LPARAM) pAMSyncNodeFile,
																		 hItemIndicatorParent, TVI_LAST);
		{ 
			pTreeCtrlIndicators->HideItem(hItemNewIndicatorPar,bVisible ? FALSE : TRUE);
		}

		pAMSyncNodeFile->SetHTreeItemLeft(hItemNewLeftPar);
		pAMSyncNodeFile->SetHTreeItemRight(hItemNewRightPar);
		pAMSyncNodeFile->SetHTreeItemIndicator(hItemNewIndicatorPar);

		return;
	}
	// Somebody got screwed up!!!!
	ASSERT(false);
}

void CAssetManagerDlg::SetTreeItem(TreeNodeBase* pTreeNodeBase,bool bDoChildren)
{
	TV_ITEM item;
	TV_ITEM_EX itemEx;

	// See if the base is a sync folder:
	AMSyncTreeNodeFolder* pAMSyncNodeFolder = dynamic_cast<AMSyncTreeNodeFolder*>(pTreeNodeBase);
	if (pAMSyncNodeFolder != NULL)
	{
		// We have a sync folder.
		int nLeftImage;
		int nRightImage;
		bool bChildAction = false;
		bool bFrozen = false;
		bool bExcluded = false;
		bool bLeftCopySource = false;
		bool bLeftCopyDest = false;
		bool bLeftDelete = false;
		bool bRightCopySource = false;
		bool bRightCopyDest = false;
		bool bRightDelete = false;
		bool bExpanded = false;
		bool bVisible = false;
		bool bFrozenPath = false;

		if (pAMSyncNodeFolder->GetFlags() & SyncTreeNodeFlags::FLAGS_FROZEN)
			bFrozen = true;

		if (pAMSyncNodeFolder->GetFlags() & SyncTreeNodeFlags::FLAGS_CHILD_HAS_ACTION)
			bChildAction = true;

		if (pAMSyncNodeFolder->GetFlags() & SyncTreeNodeFlags::FLAGS_VISIBLE)
			bVisible = true;

		if (pAMSyncNodeFolder->GetFlags() & SyncTreeNodeFlags::FLAGS_PATH_AUTO_FROZEN_PATH)
			bFrozenPath = true;

		int indicatorStatusIndex;
		
		switch (pAMSyncNodeFolder->GetFlags() & SyncTreeNodeFlags::FLAGS_STATUS_MASK)
		{
			case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY:
				{
					nLeftImage = bFrozenPath ? IMG_NORM_IDX_FOLDER_FROZEN_PATH : IMG_NORM_IDX_FOLDER;
					nRightImage = IMG_NORM_IDX_BLANK;
					indicatorStatusIndex = IMG_IND_IDX_LEFT_ONLY;
					break;
				}
			case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY:
				{
					nLeftImage = IMG_NORM_IDX_BLANK;
					nRightImage = bFrozenPath ? IMG_NORM_IDX_FOLDER_FROZEN_PATH : IMG_NORM_IDX_FOLDER;
					indicatorStatusIndex = IMG_IND_IDX_RIGHT_ONLY;
					break;
				}
			case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL:
				{
					nLeftImage = nRightImage = bFrozenPath ? IMG_NORM_IDX_FOLDER_FROZEN_PATH : IMG_NORM_IDX_FOLDER;
					indicatorStatusIndex = IMG_IND_IDX_EQUAL;
					break;
				}
		}


		// Figure out the image indexes to use for the indicator tree:
		int nIndImage;
		if (bFrozen)
		{
			nIndImage = IMG_IND_IDX_FROZEN + indicatorStatusIndex;
		}
		else
		{
			switch (pAMSyncNodeFolder->GetFlags() & SyncTreeNodeFlags::FLAGS_ACTION_MASK)
			{
				case SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED:
				{
					if (bChildAction)
					{
						nIndImage = IMG_IND_IDX_CHILD_ACTION;
					}
					else
					{
						nIndImage = IMG_IND_IDX_EXCLUDED + indicatorStatusIndex;
						bExcluded = true;
					}
					break;
				}
				case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_LEFT:
				{
					bLeftDelete = true;
					nIndImage = IMG_IND_IDX_DEL_LEFT + indicatorStatusIndex;
					break;
				}
				case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_RIGHT:
				{
					bRightDelete = true;
					nIndImage = IMG_IND_IDX_DEL_RIGHT + indicatorStatusIndex;
					break;
				}
				case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_BOTH:
				{
					bLeftDelete = true;
					bRightDelete = true;
					nIndImage = IMG_IND_IDX_DEL_BOTH + indicatorStatusIndex;
					break;
				}
				case SyncTreeNodeFlags::FLAGS_ACTION_COPY_LEFT_TO_RIGHT:
				{
					bLeftCopySource = true;
					bRightCopyDest = true;
					nIndImage = IMG_IND_IDX_COPY_RIGHT + indicatorStatusIndex;
					break;
				}
				case SyncTreeNodeFlags::FLAGS_ACTION_COPY_RIGHT_TO_LEFT:
				{
					bLeftCopyDest = true;
					bRightCopySource = true;
					nIndImage = IMG_IND_IDX_COPY_LEFT + indicatorStatusIndex;
					break;
				}
			}
		}

		// left state

		item.mask = TVIF_HANDLE;
		item.hItem = pAMSyncNodeFolder->GetHTreeItemLeft();

		itemEx.maskEx = TVIF_EX_STATEEX;
		itemEx.stateEx = 0;
		if (bFrozen)
			itemEx.stateEx |= TVIS_EX_FROZEN;
		if (bExcluded)
			itemEx.stateEx |= TVIS_EX_EXCLUDED;
		if (bLeftDelete)
			itemEx.stateEx |= TVIS_EX_TO_DELETE;
		if (bLeftCopySource)
			itemEx.stateEx |= TVIS_EX_COPY_SOURCE;
		if (bLeftCopyDest)
			itemEx.stateEx |= TVIS_EX_COPY_DEST;

		itemEx.stateExMask = TVIS_EX_FROZEN|
							 TVIS_EX_EXCLUDED|
							 TVIS_EX_TO_DELETE|
							 TVIS_EX_COPY_SOURCE|
							 TVIS_EX_COPY_DEST;

		m_pTreeCtrlLeft->SetItemEx( &item, &itemEx );
		m_pTreeCtrlLeft->HideItem(pAMSyncNodeFolder->GetHTreeItemLeft(),bVisible ? FALSE : TRUE);
		item.mask = TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		item.hItem = pAMSyncNodeFolder->GetHTreeItemLeft();
		item.iImage = item.iSelectedImage = nLeftImage;
		m_pTreeCtrlRight->SetItem( &item );

		// right state
		item.mask = TVIF_HANDLE;
		item.hItem = pAMSyncNodeFolder->GetHTreeItemRight();

		itemEx.maskEx = TVIF_EX_STATEEX;
		itemEx.stateEx = 0;
		if (bFrozen)
			itemEx.stateEx |= TVIS_EX_FROZEN;
		if (bExcluded)
			itemEx.stateEx |= TVIS_EX_EXCLUDED;
		if (bRightDelete)
			itemEx.stateEx |= TVIS_EX_TO_DELETE;
		if (bRightCopySource)
			itemEx.stateEx |= TVIS_EX_COPY_SOURCE;
		if (bRightCopyDest)
			itemEx.stateEx |= TVIS_EX_COPY_DEST;

		itemEx.stateExMask = TVIS_EX_FROZEN|
							 TVIS_EX_EXCLUDED|
							 TVIS_EX_TO_DELETE|
							 TVIS_EX_COPY_SOURCE|
							 TVIS_EX_COPY_DEST;

		m_pTreeCtrlRight->SetItemEx( &item, &itemEx );
		m_pTreeCtrlRight->HideItem(pAMSyncNodeFolder->GetHTreeItemRight(),bVisible ? FALSE : TRUE);
		item.mask = TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		item.hItem = pAMSyncNodeFolder->GetHTreeItemRight();
		item.iImage = item.iSelectedImage = nRightImage;
		m_pTreeCtrlRight->SetItem( &item );


		// indicator state
		m_pTreeCtrlIndicators->HideItem(pAMSyncNodeFolder->GetHTreeItemIndicator(),bVisible ? FALSE : TRUE);
		item.mask = TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		item.hItem = pAMSyncNodeFolder->GetHTreeItemIndicator();
		item.iImage = item.iSelectedImage = nIndImage;
		m_pTreeCtrlIndicators->SetItem( &item );

		if (bDoChildren)
		{
			// Iterate through the children of this folder:
			TreeIterator first(pAMSyncNodeFolder);
			TreeIterator last(pAMSyncNodeFolder);
			last.End();

			// Iterate through all the children of this folder:
			for ( ; first < last; first.Increment())
			{
				SetTreeItem(*first,true);
			}
		}
		return;
	}

	// See if the base is a sync file:
	AMSyncTreeNodeFile* pAMSyncNodeFile = dynamic_cast<AMSyncTreeNodeFile*>(pTreeNodeBase);
	if (pAMSyncNodeFile != NULL)
	{
		// We have a sync file.
		int nLeftImage;
		int nRightImage;
		bool bFrozen = false;
		bool bExcluded = false;
		bool bLeftCopySource = false;
		bool bLeftCopyDest = false;
		bool bLeftDelete = false;
		bool bRightCopySource = false;
		bool bRightCopyDest = false;
		bool bRightDelete = false;
		bool bExpanded = false;
		bool bVisible = false;
		bool bLeftAssociatedIcon = false;
		bool bRightAssociatedIcon = false;
		bool bFrozenPath = false;

		if (pAMSyncNodeFile->GetFlags() & SyncTreeNodeFlags::FLAGS_FROZEN)
			bFrozen = true;

		if (pAMSyncNodeFile->GetFlags() & SyncTreeNodeFlags::FLAGS_VISIBLE)
			bVisible = true;

		if (pAMSyncNodeFile->GetFlags() & SyncTreeNodeFlags::FLAGS_PATH_AUTO_FROZEN_PATH)
			bFrozenPath = true;

		char extString[_MAX_EXT];
		_splitpath(pAMSyncNodeFile->GetFileName(),NULL,NULL,NULL,extString);
		
		int associatedIconIndex = m_pAssociatedIconManager->GetIconIndex(extString);

		int indicatorStatusIndex;
		
		switch (pAMSyncNodeFile->GetFlags() & SyncTreeNodeFlags::FLAGS_STATUS_MASK)
		{
			case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY:
				{
					if (bFrozenPath)
					{
						nLeftImage = IMG_NORM_IDX_FILE_FROZEN_PATH;
					}
					else
					{
						nLeftImage = associatedIconIndex;
						bLeftAssociatedIcon = true;
					}
					nRightImage = IMG_NORM_IDX_BLANK;
					indicatorStatusIndex = IMG_IND_IDX_LEFT_ONLY;
					break;
				}
			case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY:
				{
					if (bFrozenPath)
					{
						nRightImage = IMG_NORM_IDX_FILE_FROZEN_PATH;
					}
					else
					{
						nRightImage = associatedIconIndex;
						bRightAssociatedIcon = true;
					}
					nLeftImage = IMG_NORM_IDX_BLANK;
					indicatorStatusIndex = IMG_IND_IDX_RIGHT_ONLY;
					break;
				}
			case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL:
				{
					if (bFrozenPath)
					{
						nLeftImage = IMG_NORM_IDX_FILE_FROZEN_PATH;
						nRightImage = IMG_NORM_IDX_FILE_FROZEN_PATH;
					}
					else
					{
						nLeftImage = associatedIconIndex;
						nRightImage = associatedIconIndex;
						bLeftAssociatedIcon = true;
						bRightAssociatedIcon = true;
					}
					indicatorStatusIndex = IMG_IND_IDX_EQUAL;
					break;
				}
			case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_NEWER:
				{
					if (bFrozenPath)
					{
						nLeftImage = IMG_NORM_IDX_FILE_FROZEN_PATH;
						nRightImage = IMG_NORM_IDX_FILE_FROZEN_PATH;
					}
					else
					{
						nLeftImage = associatedIconIndex;
						nRightImage = associatedIconIndex;
						bLeftAssociatedIcon = true;
						bRightAssociatedIcon = true;
					}
					indicatorStatusIndex = IMG_IND_IDX_LEFT_NEWER;
					break;
				}
			case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_NEWER:
				{
					if (bFrozenPath)
					{
						nLeftImage = IMG_NORM_IDX_FILE_FROZEN_PATH;
						nRightImage = IMG_NORM_IDX_FILE_FROZEN_PATH;
					}
					else
					{
						nLeftImage = associatedIconIndex;
						nRightImage = associatedIconIndex;
						bLeftAssociatedIcon = true;
						bRightAssociatedIcon = true;
					}
					indicatorStatusIndex = IMG_IND_IDX_RIGHT_NEWER;
					break;
				}
		}


		// Figure out the image indexes to use for the indicator tree:
		int nIndImage;
		if (bFrozen)
		{
			nIndImage = IMG_IND_IDX_FROZEN + indicatorStatusIndex;
		}
		else
		{
			switch (pAMSyncNodeFile->GetFlags() & SyncTreeNodeFlags::FLAGS_ACTION_MASK)
			{
				case SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED:
				{
					nIndImage = IMG_IND_IDX_EXCLUDED + indicatorStatusIndex;
					bExcluded = true;
					break;
				}
				case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_LEFT:
				{
					bLeftDelete = true;
					nIndImage = IMG_IND_IDX_DEL_LEFT + indicatorStatusIndex;
					break;
				}
				case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_RIGHT:
				{
					bRightDelete = true;
					nIndImage = IMG_IND_IDX_DEL_RIGHT + indicatorStatusIndex;
					break;
				}
				case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_BOTH:
				{
					bLeftDelete = true;
					bRightDelete = true;
					nIndImage = IMG_IND_IDX_DEL_BOTH + indicatorStatusIndex;
					break;
				}
				case SyncTreeNodeFlags::FLAGS_ACTION_COPY_LEFT_TO_RIGHT:
				{
					bLeftCopySource = true;
					bRightCopyDest = true;
					nIndImage = IMG_IND_IDX_COPY_RIGHT + indicatorStatusIndex;
					break;
				}
				case SyncTreeNodeFlags::FLAGS_ACTION_COPY_RIGHT_TO_LEFT:
				{
					bLeftCopyDest = true;
					bRightCopySource = true;
					nIndImage = IMG_IND_IDX_COPY_LEFT + indicatorStatusIndex;
					break;
				}
			}
		}

		// left state
		item.mask = TVIF_HANDLE;
		item.hItem = pAMSyncNodeFile->GetHTreeItemLeft();

		itemEx.maskEx = TVIF_EX_STATEEX;
		itemEx.stateEx = 0;
		if (bFrozen)
			itemEx.stateEx |= TVIS_EX_FROZEN;
		if (bExcluded)
			itemEx.stateEx |= TVIS_EX_EXCLUDED;
		if (bLeftDelete)
			itemEx.stateEx |= TVIS_EX_TO_DELETE;
		if (bLeftCopySource)
			itemEx.stateEx |= TVIS_EX_COPY_SOURCE;
		if (bLeftCopyDest)
			itemEx.stateEx |= TVIS_EX_COPY_DEST;
		if (bLeftAssociatedIcon)
			itemEx.stateEx |= TVIS_EX_ASSOCIATED_ICON;

		itemEx.stateExMask = TVIS_EX_ASSOCIATED_ICON|
								 TVIS_EX_FROZEN|
								 TVIS_EX_EXCLUDED|
								 TVIS_EX_TO_DELETE|
								 TVIS_EX_COPY_SOURCE|
								 TVIS_EX_COPY_DEST;

		m_pTreeCtrlLeft->SetItemEx( &item, &itemEx );
		m_pTreeCtrlLeft->HideItem(pAMSyncNodeFile->GetHTreeItemLeft(),bVisible ? FALSE : TRUE);
		item.mask = TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		item.hItem = pAMSyncNodeFile->GetHTreeItemLeft();
		item.iImage = item.iSelectedImage = nLeftImage;
		m_pTreeCtrlLeft->SetItem( &item );

		// right state
		item.mask = TVIF_HANDLE;
		item.hItem = pAMSyncNodeFile->GetHTreeItemRight();

		itemEx.maskEx = TVIF_EX_STATEEX;
		itemEx.stateEx = 0;
		if (bFrozen)
			itemEx.stateEx |= TVIS_EX_FROZEN;
		if (bExcluded)
			itemEx.stateEx |= TVIS_EX_EXCLUDED;
		if (bRightDelete)
			itemEx.stateEx |= TVIS_EX_TO_DELETE;
		if (bRightCopySource)
			itemEx.stateEx |= TVIS_EX_COPY_SOURCE;
		if (bRightCopyDest)
			itemEx.stateEx |= TVIS_EX_COPY_DEST;
		if (bRightAssociatedIcon)
			itemEx.stateEx |= TVIS_EX_ASSOCIATED_ICON;
		
		itemEx.stateExMask = TVIS_EX_ASSOCIATED_ICON|
								 TVIS_EX_FROZEN|
								 TVIS_EX_EXCLUDED|
								 TVIS_EX_TO_DELETE|
								 TVIS_EX_COPY_SOURCE|
								 TVIS_EX_COPY_DEST;

		m_pTreeCtrlRight->SetItemEx( &item, &itemEx );
		m_pTreeCtrlRight->HideItem(pAMSyncNodeFile->GetHTreeItemRight(),bVisible ? FALSE : TRUE);
		item.mask = TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		item.hItem = pAMSyncNodeFile->GetHTreeItemRight();
		item.iImage = item.iSelectedImage = nRightImage;
		m_pTreeCtrlRight->SetItem( &item );

		// indicator state
		m_pTreeCtrlIndicators->HideItem(pAMSyncNodeFile->GetHTreeItemIndicator(),bVisible ? FALSE : TRUE);
		item.mask = TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		item.hItem = pAMSyncNodeFile->GetHTreeItemIndicator();
		item.iImage = item.iSelectedImage = nIndImage;
		m_pTreeCtrlIndicators->SetItem( &item );

		return;
	}
	// Somebody got screwed up!!!!
	ASSERT(false);
}



//-----------------------------------------------------------------------------
// CAssetManagerDlg::ExpandAllTreeItems
// This method can be called to expand all the tree items in a tree control.
//
// Input:
//  CAssetManagerTreeCtrl* pTreeToExpand -- The tree to expand.
//
// Output: None.
//-----------------------------------------------------------------------------
void CAssetManagerDlg::ExpandAllTreeItems(CAssetManagerTreeCtrl* pTreeToExpand)
{
	ASSERT(m_pSyncTreeManager != NULL);
	AMSyncTreeNodeFolder* pAMSyncTreeRoot = dynamic_cast<AMSyncTreeNodeFolder*>(m_pSyncTreeManager->GetSyncTree());
	if (pAMSyncTreeRoot != NULL)
	{
		if (pTreeToExpand == m_pTreeCtrlLeft)
		{
			TreeIterator first(pAMSyncTreeRoot);
			TreeIterator last(pAMSyncTreeRoot);
			last.End();

			// Iterate through all the children of this folder, expanding them all:
			for ( ; first < last; first.Increment())
			{
				HTREEITEM hItemLeft;
				AMSyncTreeNodeFolder* pChildFolder = dynamic_cast<AMSyncTreeNodeFolder*>(*first);
				if (pChildFolder != NULL)
				{
					hItemLeft = pChildFolder->GetHTreeItemLeft();
				}
				else
				{
					AMSyncTreeNodeFile* pChildFile = dynamic_cast<AMSyncTreeNodeFile*>(*first);
					if (pChildFile != NULL)
					{
						hItemLeft = pChildFile->GetHTreeItemLeft();
					}
					else
					{
						// Shouldn't reach this case:
						ASSERT(false);
					}
				}

				m_pTreeCtrlLeft->ExpandCompletely(hItemLeft, FALSE);
			}
		}
		else if (pTreeToExpand == m_pTreeCtrlRight)
		{
			TreeIterator first(pAMSyncTreeRoot);
			TreeIterator last(pAMSyncTreeRoot);
			last.End();

			// Iterate through all the children of this folder, expanding them all:
			for ( ; first < last; first.Increment())
			{
				HTREEITEM hItemRight;
				AMSyncTreeNodeFolder* pChildFolder = dynamic_cast<AMSyncTreeNodeFolder*>(*first);
				if (pChildFolder != NULL)
				{
					hItemRight = pChildFolder->GetHTreeItemRight();
				}
				else
				{
					AMSyncTreeNodeFile* pChildFile = dynamic_cast<AMSyncTreeNodeFile*>(*first);
					if (pChildFile != NULL)
					{
						hItemRight = pChildFile->GetHTreeItemRight();
					}
					else
					{
						// Shouldn't reach this case:
						ASSERT(false);
					}
				}

				m_pTreeCtrlRight->ExpandCompletely(hItemRight, FALSE);
			}
		}
	}
}


//-----------------------------------------------------------------------------
// CAssetManagerDlg::ExpandItem
// This method will be called whenever the user expands or collapses the left
// or right tree control.  It will tell the other trees to expand or collapse
// accordingly.
//
// Input:
//  HTREEITEM hItem -- The item that is being expanded or collapsed.
//  SEC_DWORD sDWItemData -- In our case, this will hold a pointer to the 
//							 sync tree folder object.  This will be used to get
//							 the HTREEITEMS for the corresponding items in the
//							 the other trees.
//  UINT nCode -- Species whether to expand or collapse.
//
// Output: None.
//-----------------------------------------------------------------------------
void CAssetManagerDlg::ExpandItem(HTREEITEM hItem, SEC_DWORD sDWItemData, UINT nCode)
{
	if (GetMode() == POSTCOMPARE)
	{
		AMSyncTreeNodeFolder* pSyncNodeFolder = (AMSyncTreeNodeFolder*) sDWItemData;
		if (pSyncNodeFolder != NULL)
		{
			HTREEITEM hItemLeft = pSyncNodeFolder->GetHTreeItemLeft();
			HTREEITEM hItemRight = pSyncNodeFolder->GetHTreeItemRight();
			HTREEITEM hItemIndicator = pSyncNodeFolder->GetHTreeItemIndicator();

			if (hItemLeft == hItem)
			{
				// The left tree has been expanded or collapsed.
				// Expand the right tree:
				if (hItemRight != NULL)
				{
					m_pTreeCtrlRight->SetTellParentOfExpand(false);
					m_pTreeCtrlRight->Expand(hItemRight, nCode);
				}
				// Expand the indicator tree:
				if (hItemIndicator != NULL)
				{
					m_pTreeCtrlIndicators->SetTellParentOfExpand(false);
					m_pTreeCtrlIndicators->Expand(hItemIndicator, nCode);
				}
			}
			else if (hItemRight == hItem)
			{
				// The right tree has been expanded or collapsed:
				// Expand the left tree:
				if (hItemLeft != NULL)
				{
					m_pTreeCtrlLeft->SetTellParentOfExpand(false);
					m_pTreeCtrlLeft->Expand(hItemLeft, nCode);
				}
				// Expand the indicator tree:
				if (hItemIndicator != NULL)
				{
					m_pTreeCtrlIndicators->SetTellParentOfExpand(false);
					m_pTreeCtrlIndicators->Expand(hItemIndicator, nCode);
				}
			}
		}
	}
}


//-----------------------------------------------------------------------------
// CAssetManagerDlg::ShowContextMenu
// This method can be called to show the context menu for the left or right
// tree control.
//
// Input:
//	CPoint point -- Position of the cursor, in screen coordinates, at the time
//					of the mouse click.
//	int nRole -- Indicates whether this was called from the left tree or the
//				 right tree.
//
// Output: None.
//-----------------------------------------------------------------------------
void CAssetManagerDlg::ShowContextMenu(CPoint point, int nRole)
{
	// Set the flag:
	if (nRole == CAssetManagerTreeCtrl::LEFT_TREE)
		m_bContextFromLeft = true;
	else if (nRole == CAssetManagerTreeCtrl::RIGHT_TREE)
		m_bContextFromLeft = false;
	else
		ASSERT(false);	// Has to be either from the left or right!


	// Popup the context menu:
	if (GetMode() == POSTCOMPARE)
	{
		unsigned long validActionFlags = GetValidActions();
		UINT menuFlags;

		CMenu menu;
		menu.CreatePopupMenu();

		CMenu autoFreezeMenu;
		autoFreezeMenu.CreatePopupMenu();
		menuFlags = (validActionFlags & SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_ADD_AUTO_FREEZE) ? MF_STRING|MF_ENABLED : MF_STRING|MF_GRAYED;
		autoFreezeMenu.AppendMenu(menuFlags, ID_AUTO_FREEZE_ADD, "Add to List");
		menuFlags = (validActionFlags & SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_REMOVE_AUTO_FREEZE) ? MF_STRING|MF_ENABLED : MF_STRING|MF_GRAYED;
		autoFreezeMenu.AppendMenu(menuFlags, ID_AUTO_FREEZE_REMOVE, "Remove from List");
		menuFlags = (validActionFlags & SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_REMOVE_AUTO_FREEZE_INCLUDE_CHILDREN) ? MF_STRING|MF_ENABLED : MF_STRING|MF_GRAYED;
		autoFreezeMenu.AppendMenu(menuFlags, ID_AUTO_FREEZE_REMOVE_INCLUDE_CHILDREN, "Remove from List (Including Children)");

		menuFlags = (validActionFlags & SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_SET_DEFAULT) ? MF_STRING|MF_ENABLED : MF_STRING|MF_GRAYED;
		menu.AppendMenu(menuFlags, ID_SET_DEFAULT, "Set Default\tCtrl+Q");
		menuFlags = (validActionFlags & SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXCLUDE) ? MF_STRING|MF_ENABLED : MF_STRING|MF_GRAYED;
		menu.AppendMenu(menuFlags, ID_EXCLUDE, "Exclude\tCtrl+W");
		menuFlags = (validActionFlags & SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_RIGHT_TO_LEFT) ? MF_STRING|MF_ENABLED : MF_STRING|MF_GRAYED;
		menu.AppendMenu(menuFlags, ID_COPY_TO_LEFT, "Copy to Left\tCtrl+E");
		menuFlags = (validActionFlags & SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_LEFT_TO_RIGHT) ? MF_STRING|MF_ENABLED : MF_STRING|MF_GRAYED;
		menu.AppendMenu(menuFlags, ID_COPY_TO_RIGHT, "Copy to Right\tCtrl+R");
		menu.AppendMenu(MF_SEPARATOR);
		menuFlags = (validActionFlags & SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_FREEZE) ? MF_STRING|MF_ENABLED : MF_STRING|MF_GRAYED;
		menu.AppendMenu(menuFlags, ID_FREEZE, "Freeze\tCtrl+F");
		menu.AppendMenu(MF_STRING|MF_ENABLED|MF_POPUP,(unsigned int) autoFreezeMenu.GetSafeHmenu(), "Auto Freeze Paths");
		menuFlags = (validActionFlags & SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_UNFREEZE) ? MF_STRING|MF_ENABLED : MF_STRING|MF_GRAYED;
		menu.AppendMenu(menuFlags, ID_UNFREEZE, "UnFreeze\tShift+Ctrl+F");
		menuFlags = (validActionFlags & SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_UNFREEZE_ONLY) ? MF_STRING|MF_ENABLED : MF_STRING|MF_GRAYED;
		menu.AppendMenu(menuFlags, ID_UNFREEZE_ONLY, "Freeze all Except\tShift+Ctrl+G");
		menu.AppendMenu(MF_SEPARATOR);
		menuFlags = (validActionFlags & SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_LEFT) ? MF_STRING|MF_ENABLED : MF_STRING|MF_GRAYED;
		menu.AppendMenu(menuFlags, ID_DELETE_FROM_LEFT, "Delete Left\tShift+Ctrl+E");
		menuFlags = (validActionFlags & SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_RIGHT) ? MF_STRING|MF_ENABLED : MF_STRING|MF_GRAYED;
		menu.AppendMenu(menuFlags, ID_DELETE_FROM_RIGHT, "Delete Right\tShift+Ctrl+R");
		menuFlags = (validActionFlags & SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_BOTH) ? MF_STRING|MF_ENABLED : MF_STRING|MF_GRAYED;
		menu.AppendMenu(menuFlags, ID_DELETE_FROM_BOTH, "Delete Both\tShift+Ctrl+B");
		menu.AppendMenu(MF_SEPARATOR);
		menuFlags = (validActionFlags & SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_SELECT_ALL) ? MF_STRING|MF_ENABLED : MF_STRING|MF_GRAYED;
		menu.AppendMenu(menuFlags, ID_SELECT_ALL, "Select All\tCtrl+A");
		menuFlags = (validActionFlags & SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_SELECT_NONE) ? MF_STRING|MF_ENABLED : MF_STRING|MF_GRAYED;
		menu.AppendMenu(menuFlags, ID_SELECT_NONE, "Select None\tCtrl+N");
		menuFlags = (validActionFlags & SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_INVERT_SELECTION) ? MF_STRING|MF_ENABLED : MF_STRING|MF_GRAYED;
		menu.AppendMenu(menuFlags, ID_INVERT_SELECTION, "Invert Selection\tCtrl+I");
		menu.AppendMenu(MF_SEPARATOR);
		menuFlags = (validActionFlags & SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXPLORE) ? MF_STRING|MF_ENABLED : MF_STRING|MF_GRAYED;
		menu.AppendMenu(menuFlags, ID_EXPLORE, "Explore\tShift+Ctrl+X");
		menu.AppendMenu(MF_SEPARATOR);
		menuFlags = (validActionFlags & SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXPAND_ALL) ? MF_STRING|MF_ENABLED : MF_STRING|MF_GRAYED;
		menu.AppendMenu(menuFlags, ID_EXPAND_ALL, "Expand All\tCtrl+X");
		menuFlags = (validActionFlags & SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COLLAPSE_ALL) ? MF_STRING|MF_ENABLED : MF_STRING|MF_GRAYED;
		menu.AppendMenu(menuFlags, ID_COLLAPSE_ALL, "Collapse All\tCtrl+C");
		menu.AppendMenu(MF_SEPARATOR);
		menuFlags = (validActionFlags & SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_RENAME) ? MF_STRING|MF_ENABLED : MF_STRING|MF_GRAYED;
		menu.AppendMenu(menuFlags, ID_RENAME, "Rename");
		menuFlags = (validActionFlags & SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_ADD_PREFIX) ? MF_STRING|MF_ENABLED : MF_STRING|MF_GRAYED;
		menu.AppendMenu(menuFlags, ID_ADD_PREFIX, "Add Prefix\tCtrl+P");
		menuFlags = (validActionFlags & SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_REMOVE_PREFIX) ? MF_STRING|MF_ENABLED : MF_STRING|MF_GRAYED;
		menu.AppendMenu(menuFlags, ID_REMOVE_PREFIX, "Remove Prefix\tShift+Ctrl+P");
		menu.AppendMenu(MF_SEPARATOR);
		menuFlags = (validActionFlags & SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_UPDATE_DUPLICATES) ? MF_STRING|MF_ENABLED : MF_STRING|MF_GRAYED;
		menu.AppendMenu(menuFlags, ID_UPDATE_DUPLICATES, "Update Duplicates\tCtrl+D");
		menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
									 point.x, point.y, this);
	}
}

//-----------------------------------------------------------------------------
// CAssetManagerDlg::LoadDatabases
// This method can be called to load the left and right databases as well as set up the tree
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void CAssetManagerDlg::LoadDatabases(void)
{
	ASSERT(!m_pLoadDBDialog); // this should not be set...
	// create dialog for showing feedback
	m_pLoadDBDialog = new LoadDBDialog;

	int activeSet = m_pFolderSets->GetActiveSet();
	int threadPriority = m_pFolderSets->GetScanThreadPriority(activeSet);
	m_pFolderSets->SetLastScanTime(activeSet,CTime::GetCurrentTime());

	// the current thread will be wainting in the domodal loop while waiting for the
	// thread that will actually be performing the scanning
	m_pFeedbackThread = AfxBeginThread(LoadDatabaseProc,this,GetWindowsThreadPriority(threadPriority));	// start the other thread
	m_pLoadDBDialog->SetThreadRunning(true);
	m_pLoadDBDialog->SetThread(m_pFeedbackThread); // pass the thread on to the dialog so it can wait
	theApp.m_bAccelEnable = false;
	m_pLoadDBDialog->DoModal();	 // wait here until the thread is done
	theApp.m_bAccelEnable = true;
	delete m_pLoadDBDialog;	// we are finished with the dialog
	m_pLoadDBDialog = NULL;
}

// this is a static function that is called when the new thread is started
// the pParam is the address of the asset manager dialog
UINT CAssetManagerDlg::LoadDatabaseProc(LPVOID pParam)
{
	// we will now call a member function of the dialog to do the stuff
	// any data we need to pass to the thread needs to be member variables within the dialog
	CAssetManagerDlg *pThis = (CAssetManagerDlg *) pParam;
	return pThis->LoadDatabaseProcessThread();
}

// this is the member function that is called by the static function
// is has access to all the member variables of the dialog
UINT CAssetManagerDlg::LoadDatabaseProcessThread(void)
{
	// wait until init dialog is called in other thread
	while (!m_pLoadDBDialog->GetInitDialogCalled())
		Sleep(1);

	// the feedback object is used to pass information to the sync dialog
	// the other thread is sitting in the domodal loop ready to handle any ui messages for the load db dialog
	AMLoadDBFeedback feedback;
	feedback.SetDialog(m_pLoadDBDialog); // connect the feedback object to the feedback dialog

	int activeSet = m_pFolderSets->GetActiveSet();
	
	if (!m_pLeftDataBase && !feedback.GetAbortFlag())
	{
		const char *pLeftFolderPath = m_pFolderSets->GetLeftFolderPath(activeSet);
		if (strlen(pLeftFolderPath))
		{
			feedback.SetStatus(AMLoadDBFeedback::STATUS_LOADING_LEFT_DB);
			m_pLeftDataBase = m_pDBFactory->CreateDB(pLeftFolderPath,feedback,*m_pLogFile);
			if (!m_pLeftDataBase->GetTreeRoot())
			{
				delete m_pLeftDataBase;
				m_pLeftDataBase = NULL;

				if (!feedback.GetAbortFlag())
				{
					CString strErrorString;
					strErrorString.Format("Error scanning left database %s",pLeftFolderPath);
					::AfxMessageBox((LPCTSTR) strErrorString, MB_OK | MB_ICONEXCLAMATION);
				}
			}
		}
	}
	if (!m_pRightDataBase && !feedback.GetAbortFlag())
	{
		const char *pRightFolderPath = m_pFolderSets->GetRightFolderPath(activeSet);
		if (strlen(pRightFolderPath))
		{
			feedback.SetStatus(AMLoadDBFeedback::STATUS_LOADING_RIGHT_DB);
			m_pRightDataBase = m_pDBFactory->CreateDB(pRightFolderPath,feedback,*m_pLogFile);
			if (!m_pRightDataBase->GetTreeRoot())
			{
				delete m_pRightDataBase;
				m_pRightDataBase = NULL;

				if (!feedback.GetAbortFlag())
				{
					CString strErrorString;
					strErrorString.Format("Error scanning right database %s",pRightFolderPath);
					::AfxMessageBox((LPCTSTR) strErrorString, MB_OK | MB_ICONEXCLAMATION);
				}
			}
		}
	}
	bool bUseDefaultAutoExclude = m_pFolderSets->IsUsingDefaultAutoFreezeExtensions(activeSet);
	if (bUseDefaultAutoExclude)
	{
		m_autoExclude.SetFileExtensions(m_defaultAutoFreezeFileExtensions);
		m_autoExclude.SetFilePrefixes(m_defaultAutoFreezeFilePrefixes);
		m_autoExclude.SetFolderPrefixes(m_defaultAutoFreezeFolderPrefixes);
	}
	else
	{
		m_autoExclude.SetFileExtensions(m_pFolderSets->GetAutoFreezeFileExtensions(activeSet));
		m_autoExclude.SetFilePrefixes(m_pFolderSets->GetAutoFreezeFilePrefixes(activeSet));
		m_autoExclude.SetFolderPrefixes(m_pFolderSets->GetAutoFreezeFolderPrefixes(activeSet));
	}

	int syncStyle = m_pFolderSets->GetSyncStyle(activeSet);
	m_pSyncTreeManager->SetSyncStyle((SyncTreeManager::SyncStyles) syncStyle);

	bool bDoCRCCheck = m_pFolderSets->GetDoCRCCheck(activeSet);
	m_pSyncTreeManager->SetCheckingCRC(bDoCRCCheck);

	bool bUseSyncDates = m_pFolderSets->GetUseSyncDates(activeSet);
	m_pSyncTreeManager->SetUsingSyncDates(bUseSyncDates);

	bool bVerifyFileCopies = m_pFolderSets->GetVerifyCopiedFiles(activeSet);
	m_pSyncTreeManager->SetVerifyCopiedFiles(bVerifyFileCopies);

	if (bDoCRCCheck)
		feedback.SetStatus(AMLoadDBFeedback::STATUS_BUILDING_SYNC_TREE_WITH_CRC);
	else
		feedback.SetStatus(AMLoadDBFeedback::STATUS_BUILDING_SYNC_TREE);

	// get the filename for the client database
	TupString clientDataFilePath = GetClientDataFilePath(activeSet);

	ClientDataManager *pClientDataManager = NULL;
	// if it is a valid path
	if (clientDataFilePath.GetLength())
	{
		pClientDataManager = new ClientDataManager;

		DWORD waitResult = WaitForSingleObject(theApp.m_hSyncClientDatabasesMutex,INFINITE);

		if (waitResult == WAIT_OBJECT_0)
		{
			pClientDataManager->LoadData(clientDataFilePath);
			ReleaseMutex(theApp.m_hSyncClientDatabasesMutex);
		}

		// pull over the autofrozen path list regardless of the folder paths
		int numPaths = pClientDataManager->GetNumAutoFrozenPaths();
		m_autoExclude.RemovePaths();
		for (int pathIndex=0;pathIndex<numPaths;pathIndex++)
		{
			TupString path;
			pClientDataManager->GetAutoFrozenPath(path,pathIndex);
			m_autoExclude.AddPath(path);
		}

		// if this database is not pointing to the same folders then we kill it
		if (stricmp(pClientDataManager->GetLeftFolderPath(),m_pFolderSets->GetLeftFolderPath(activeSet))!=0 ||
			stricmp(pClientDataManager->GetRightFolderPath(),m_pFolderSets->GetRightFolderPath(activeSet))!=0)
		{
			delete pClientDataManager;
			pClientDataManager = NULL;
		}
	}
	
	m_pSyncTreeManager->SetDatabases(m_pLeftDataBase,m_pRightDataBase,pClientDataManager,feedback);
	// done with this
	if (pClientDataManager)
	{
		delete pClientDataManager;
		pClientDataManager = NULL;
	}
	
	// Set the app mode:
	if (m_pLeftDataBase != NULL && m_pRightDataBase != NULL)
		SetMode(POSTCOMPARE);
	else
		SetMode(PRECOMPARE);		

	UpdateSyncStats();

	feedback.SetStatus(AMLoadDBFeedback::STATUS_BUILDING_TREE_CONTROLS);

	LoadDBFeedback::Stats emptyStats;
	emptyStats.m_fileName.Empty();
	emptyStats.m_totalFilesScanned = 0;
	feedback.Update(emptyStats);

	// Get the sync tree (the SyncTreeManager owns it):
	AMSyncTreeNodeFolder* pAMSyncTreeRoot = dynamic_cast<AMSyncTreeNodeFolder*>(m_pSyncTreeManager->GetSyncTree());
	LoadTreeControl(m_pTreeCtrlLeft, m_pTreeCtrlRight, m_pTreeCtrlIndicators, pAMSyncTreeRoot);

	InvalidateRect( 0, FALSE );

	m_pLoadDBDialog->SetThreadRunning(false);
	m_pLoadDBDialog->PostMessage(WM_KICKIDLE);
	return 0;
}

const char *CAssetManagerDlg::GetClientDataFilePath(int setIndex)
{
	static TupString pathString = "";

	TCHAR path[MAX_PATH];
	HRESULT hResult = SHGetFolderPath(NULL,                //hWnd	
									   CSIDL_APPDATA | CSIDL_FLAG_CREATE,       //csidl
									   NULL,                //hToken
									   SHGFP_TYPE_CURRENT,  //dwFlags
									   path);             //pszPath
	if (SUCCEEDED(hResult))
	{
		PathAppend(path, FOLDER_SETS_APP_DATA_FOLDER);
	}
	else
	{
		strcpy(path,"c:\\"); // just use c:
		PathAppend(path, FOLDER_SETS_APP_DATA_FOLDER);
	}

	PathAppend(path, TEXT("FolderSet"));
	char numString[10];
	itoa(setIndex,numString,10);
	pathString = path;
	pathString += numString;
	pathString += ".tup";

	CreateDirs(pathString,TRUE);

	return pathString;
}

bool CAssetManagerDlg::DirectoryExists(const char *pPath)
{
	DWORD fileAttributes = ::GetFileAttributes(pPath);
	return (fileAttributes != INVALID_FILE_ATTRIBUTES);
}

//
//  CreateDirs()
//  DESC: Creates each directory in a given path. 
//  ARGS: sPath - The fully qualified path whose directories we will create.
//           As each portion of the the path is parsed out, if it doesn't
//           exists, it will be created by CreateDirectory().  
//        bIsFilePath - If TRUE, CreateDirs() assumes the last token in the
//           path is a filename, and skips it.
bool CAssetManagerDlg::CreateDirs (const char *pPath, bool bIsFilePath /*= false*/)
{
    char outpath[MAX_PATH] = "";      // we build this during the parsing of the source path
	char inpath[MAX_PATH];           // create local copy of source path (we destructively parse it)  
	char *p = strcpy (inpath, pPath); 
    char *slash = NULL;
	
    // store the drive portion of path and skip past it
    if (!((slash = strchr (p, '\\')) || (slash = strchr (p, '/'))))
        return false;
    *slash = 0;           // terminate p at first slash so we can...
    strcpy (outpath, p);  // copy drive letter and : to outpath
    p = slash + 1;        // advance p to next token in path
	
    ASSERT(outpath[1] == ':'); // so far, so good
	
    // parse out slash delimited tokens, 
    // creating each part of the directory path as we go
    do 
	{
        if ((slash = strchr (p, '\\')) || (slash = strchr (p, '/')))     
            *slash = 0;                         // terminate so we can call strcat() with p
		else if (bIsFilePath)                   // skip last token if the path is to a file
            return true;
        strcat (outpath, "\\");                 // add token to the output path path...
		strcat (outpath, p);                    // ...
        if (!DirectoryExists (outpath) &&       // if path doesn't already exists
            !CreateDirectory (outpath, NULL))   // try to create it
            return false;                       // something went wrong
        p = slash + 1;                          // advance p to next token in path
    }
	while (slash);
	
    return true;
}


void CAssetManagerDlg::SaveClientData(void)
{
	// make sure we do this without interruption
	DWORD waitResult = WaitForSingleObject(theApp.m_hSyncClientDatabasesMutex,INFINITE);
	if (waitResult != WAIT_OBJECT_0)
	{
		::AfxMessageBox((LPCTSTR)"Error saving client data", MB_OK | MB_ICONEXCLAMATION);
		return;
	}
	int activeSet = m_pFolderSets->GetActiveSet();
	if (activeSet!=-1 && m_pSyncTreeManager)
	{
		// get the filename for the client database
		TupString clientDataFilePath = GetClientDataFilePath(activeSet);
		
		// if it is a valid path
		if (clientDataFilePath.GetLength())
		{
			ClientDataManager pClientDataManager;
			pClientDataManager.BuildTree(m_pSyncTreeManager->GetSyncTree());
			pClientDataManager.SetLeftFolderPath(m_pFolderSets->GetLeftFolderPath(activeSet));
			pClientDataManager.SetRightFolderPath(m_pFolderSets->GetRightFolderPath(activeSet));
			int numAutoFrozenPaths = m_autoExclude.GetNumPaths();
			for (int pathIndex=0;pathIndex<numAutoFrozenPaths;pathIndex++)
			{
				TupString path;
				m_autoExclude.GetPath(path,pathIndex);
				pClientDataManager.AddAutoFrozenPath(path);
			}
			pClientDataManager.SaveData(clientDataFilePath);
		}
	}
	ReleaseMutex(theApp.m_hSyncClientDatabasesMutex);
}


//-----------------------------------------------------------------------------
// CAssetManagerDlg::SyncDatabases
// This method can be called sync the databases
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void CAssetManagerDlg::SyncDatabases(void)
{
	ASSERT(!m_pSyncDialog); // this should not be set...
	// make sure both databases are present
	if (!m_pLeftDataBase || !m_pRightDataBase)
		return;
	// make sure the the sync tree is present
	if (!m_pSyncTreeManager->GetSyncTree())
		return;

	// create dialog for showing feedback
	m_pSyncDialog = new SyncDialog; 

	int activeSet = m_pFolderSets->GetActiveSet();
	int threadPriority = m_pFolderSets->GetSyncThreadPriority(activeSet);

	m_pSyncTreeManager->SetVerifyCopiedFiles(m_pFolderSets->GetVerifyCopiedFiles(activeSet));
	m_pSyncDialog->m_bCloseCheck = m_pFolderSets->GetCloseWhenCompleted(activeSet) ? TRUE : FALSE;
	m_pSyncDialog->m_bExitCheck = m_pFolderSets->GetExitWhenClosed(activeSet) ? TRUE : FALSE;

	// the current thread will be wainting in the domodal loop while waiting for the
	// thread that will actually be performing the syncing
	m_pFeedbackThread = AfxBeginThread(SyncDatabasesProc,this,GetWindowsThreadPriority(threadPriority)); // start the other thread
	m_pSyncDialog->SetThreadRunning(true);
	m_pSyncDialog->SetThread(m_pFeedbackThread); // pass the thread on to the dialog so it can wait
	theApp.m_bAccelEnable = false;
	m_pSyncDialog->DoModal(); // wait here until the thread is done
	m_pFolderSets->SetCloseWhenCompleted(activeSet,m_pSyncDialog->m_bCloseCheck ? true : false);
	m_pFolderSets->SetExitWhenClosed(activeSet,m_pSyncDialog->m_bExitCheck ? true : false);
	theApp.m_bAccelEnable = true;
	delete m_pSyncDialog; // we are finished with the dialog
	m_pSyncDialog = NULL;
	if (m_pFolderSets->GetExitWhenClosed(activeSet))
	{
		OnClose();
		EndDialog(IDOK);
		return;
	}
// try it without the refresh step
//	Refresh();
}

// this is a static function that is called when the new thread is started
// the pParam is the address of the asset manager dialog
UINT CAssetManagerDlg::SyncDatabasesProc(LPVOID pParam)
{
	// we will now call a member function of the dialog to do the stuff
	// any data we need to pass to the thread needs to be member variables within the dialog
	CAssetManagerDlg *pThis = (CAssetManagerDlg *) pParam;
	return pThis->SyncDatabasesProcessThread();
}

// this is the member function that is called by the static function
// is has access to all the member variables of the dialog
UINT CAssetManagerDlg::SyncDatabasesProcessThread(void)
{
	// wait until init dialog is called in other thread
	while (!m_pSyncDialog->GetInitDialogCalled())
		Sleep(1);

	// the feedback object is used to pass information to the sync dialog
	// the other thread is sitting in the domodal loop ready to handle any ui messages for the sync dialog
	AMSyncFeedback feedback;
	feedback.SetDialog(m_pSyncDialog); // connect the feedback object to the feedback dialog

	// to the sync - pass the feedback object so the dialog can be updated
	m_pSyncTreeManager->Synchronize(feedback,m_syncEnable,m_viewFlags);
	if (feedback.GetAbortFlag())
	{
		feedback.AddToLog(LogFlags::FONT_COLOR_DK_RED,"Canceled...");
	}
	m_syncEnable.m_bLRCopyEnable = false;
	m_syncEnable.m_bRLCopyEnable = false;
	m_syncEnable.m_bLDeleteEnable = false;
	m_syncEnable.m_bRDeleteEnable = false;
	
	UpdateSyncStats();

	// Get the sync tree (the SyncTreeManager owns it):
	AMSyncTreeNodeFolder* pAMSyncTreeRoot = dynamic_cast<AMSyncTreeNodeFolder*>(m_pSyncTreeManager->GetSyncTree());
	ASSERT(pAMSyncTreeRoot != NULL);
	// rebuild the tree controls with new info
	LoadTreeControl(m_pTreeCtrlLeft, m_pTreeCtrlRight, m_pTreeCtrlIndicators, pAMSyncTreeRoot);

	InvalidateRect( 0, FALSE );
	m_pSyncDialog->SetThreadRunning(false);
	m_pSyncDialog->PostMessage(WM_KICKIDLE);
	return 0;
}

void CAssetManagerDlg::UpdateSyncStats(void)
{
	// gets information from the tree as well as sets the visible flags
	m_pSyncTreeManager->GetTreeStats(m_syncStats,m_viewFlags);

	if ((m_syncStats.m_filesFoldersCopiedLeftRight && m_syncEnable.m_bLRCopyEnable) ||
		(m_syncStats.m_filesFoldersCopiedRightLeft && m_syncEnable.m_bRLCopyEnable) ||
		(m_syncStats.m_filesFoldersDeletedLeft && m_syncEnable.m_bLDeleteEnable) ||
		(m_syncStats.m_filesFoldersDeletedRight && m_syncEnable.m_bRDeleteEnable))
	{
		m_syncButton.SetBitmap(m_hSync);
	}
	else
	{
		m_syncButton.SetBitmap(m_hSyncDisabled);
	}

	m_rlCopyButton.SetState((int)m_syncEnable.m_bRLCopyEnable);
	m_rDeleteButton.SetState((int)m_syncEnable.m_bRDeleteEnable);
	m_lrCopyButton.SetState((int)m_syncEnable.m_bLRCopyEnable);
	m_lDeleteButton.SetState((int)m_syncEnable.m_bLDeleteEnable);

	if (m_pLeftDataBase)
	{
		// change the text to indicate a remote database
		if (m_pLeftDataBase->IsRemoteDB())
		{
			TupString string;
			string.Format("%s (R)",m_pFolderSets->GetLeftFolderPath(m_pFolderSets->GetActiveSet()));
			m_folderPathLeft.SetWindowText(string);
		}

		CString string,string2;
		string.Format("Files: %d  Folders: %d",m_syncStats.m_filesLeft,m_syncStats.m_foldersLeft);
		m_leftStatsText1.SetWindowText(string);
		string2.Format("%I64d",m_syncStats.m_totalBytesLeft);

		// insert the commas
		int end = string2.GetLength()-3;
		while(end>0)
		{
			string2.Insert(end,",");
			end-=3;
		}

		string.Format("Total Size: %s Bytes",string2);
		m_leftStatsText2.SetWindowText(string);

		if (m_pRightDataBase)
		{
			// change the text to indicate a remote database
			if (m_pRightDataBase->IsRemoteDB())
			{
				TupString string;
				string.Format("%s (R)",m_pFolderSets->GetRightFolderPath(m_pFolderSets->GetActiveSet()));
				m_folderPathRight.SetWindowText(string);
			}

			if (m_syncStats.m_filesFoldersCopiedLeftRight)
				m_lrCopyButton.SetBitmap(m_hCopyToRight);
			else
				m_lrCopyButton.SetBitmap(m_hCopyToRightDisabled);
			if (m_syncStats.m_filesFoldersDeletedLeft)
				m_lDeleteButton.SetBitmap(m_hDeleteLeft);
			else
				m_lDeleteButton.SetBitmap(m_hDeleteLeftDisabled);

			string2.Format("%I64d",m_syncStats.m_totalBytesCopiedLeftRight);

			// insert the commas
			end = string2.GetLength()-3;
			while(end>0)
			{
				string2.Insert(end,",");
				end-=3;
			}

			string.Format("Files/Folders Copied: %d (%s Bytes)",m_syncStats.m_filesFoldersCopiedLeftRight,string2);
			m_leftStatsText3.SetWindowText(string);

			string.Format("Files/Folders Deleted: %d",m_syncStats.m_filesFoldersDeletedLeft);
			m_leftStatsText4.SetWindowText(string);

			if (m_syncEnable.m_bLRCopyEnable)
			{
				m_leftStatsText3.EnableWindow(TRUE);
			}
			else
			{
				m_leftStatsText3.EnableWindow(FALSE);
			}
			if (m_syncEnable.m_bLDeleteEnable)
			{
				m_leftStatsText4.EnableWindow(TRUE);
			}
			else
			{
				m_leftStatsText4.EnableWindow(FALSE);
			}
		}
		else
		{
			m_leftStatsText3.SetWindowText("");
			m_leftStatsText4.SetWindowText("");
			m_lrCopyButton.SetBitmap(m_hCopyToRightDisabled);
			m_lDeleteButton.SetBitmap(m_hDeleteLeftDisabled);
		}
	}
	else
	{
		m_leftStatsText1.SetWindowText("");
		m_leftStatsText2.SetWindowText("");
		m_leftStatsText3.SetWindowText("");
		m_leftStatsText4.SetWindowText("");
		m_lrCopyButton.SetBitmap(m_hCopyToRightDisabled);
		m_lDeleteButton.SetBitmap(m_hDeleteLeftDisabled);
	}

	if (m_pRightDataBase)
	{
		CString string,string2;
		string.Format("Files: %d  Folders: %d",m_syncStats.m_filesRight,m_syncStats.m_foldersRight);
		m_rightStatsText1.SetWindowText(string);
		string2.Format("%I64d",m_syncStats.m_totalBytesRight);

		// insert the commas
		int end = string2.GetLength()-3;
		while(end>0)
		{
			string2.Insert(end,",");
			end-=3;
		}

		string.Format("Total Size: %s Bytes",string2);
		m_rightStatsText2.SetWindowText(string);

		if (m_pLeftDataBase)
		{
			if (m_syncStats.m_filesFoldersCopiedRightLeft)
				m_rlCopyButton.SetBitmap(m_hCopyToLeft);
			else
				m_rlCopyButton.SetBitmap(m_hCopyToLeftDisabled);
			if (m_syncStats.m_filesFoldersDeletedRight)
				m_rDeleteButton.SetBitmap(m_hDeleteRight);
			else
				m_rDeleteButton.SetBitmap(m_hDeleteRightDisabled);

			string2.Format("%d",m_syncStats.m_totalBytesCopiedRightLeft);

			// insert the commas
			end = string2.GetLength()-3;
			while(end>0)
			{
				string2.Insert(end,",");
				end-=3;
			}

			string.Format("Files/Folders Copied: %d (%s Bytes)",m_syncStats.m_filesFoldersCopiedRightLeft,string2);
			m_rightStatsText3.SetWindowText(string);

			string.Format("Files/Folders Deleted: %d",m_syncStats.m_filesFoldersDeletedRight);
			m_rightStatsText4.SetWindowText(string);

			if (m_syncEnable.m_bRLCopyEnable)
			{
				m_rightStatsText3.EnableWindow(TRUE);
			}
			else
			{
				m_rightStatsText3.EnableWindow(FALSE);
			}
			if (m_syncEnable.m_bRDeleteEnable)
			{
				m_rightStatsText4.EnableWindow(TRUE);
			}
			else
			{
				m_rightStatsText4.EnableWindow(FALSE);
			}

		}
		else
		{
			m_rightStatsText3.SetWindowText("");
			m_rightStatsText4.SetWindowText("");
			m_rlCopyButton.SetBitmap(m_hCopyToLeftDisabled);
			m_rDeleteButton.SetBitmap(m_hDeleteRightDisabled);
		}
	}
	else
	{
		m_rightStatsText1.SetWindowText("");
		m_rightStatsText2.SetWindowText("");
		m_rightStatsText3.SetWindowText("");
		m_rightStatsText4.SetWindowText("");
		m_rlCopyButton.SetBitmap(m_hCopyToLeftDisabled);
		m_rDeleteButton.SetBitmap(m_hDeleteRightDisabled);
	}
}

void CAssetManagerDlg::OnFolderSetsSort()
{
	m_bSortFolderSets = !m_bSortFolderSets;
	RebuildFolderSetsCombo();
	m_bttnFolderSetsSort.SetState((BOOL)m_bSortFolderSets);
}

void CAssetManagerDlg::OnSyncbutton() 
{
	if ((m_syncStats.m_filesFoldersCopiedLeftRight && m_syncEnable.m_bLRCopyEnable) ||
		(m_syncStats.m_filesFoldersCopiedRightLeft && m_syncEnable.m_bRLCopyEnable) ||
		(m_syncStats.m_filesFoldersDeletedLeft && m_syncEnable.m_bLDeleteEnable) ||
		(m_syncStats.m_filesFoldersDeletedRight && m_syncEnable.m_bRDeleteEnable))
	{
		CSyncConfirmDialog confirmDialog;
		CString *stringPtrArray[4];
		stringPtrArray[0] = &confirmDialog.m_confirmText1;
		stringPtrArray[1] = &confirmDialog.m_confirmText2;
		stringPtrArray[2] = &confirmDialog.m_confirmText3;
		stringPtrArray[3] = &confirmDialog.m_confirmText4;
		int currentIndex = 0;
		if ((m_syncStats.m_filesFoldersCopiedLeftRight && m_syncEnable.m_bLRCopyEnable))
		{
			CString string;
			string.Format("%d",m_syncStats.m_totalBytesCopiedLeftRight);

			// insert the commas
			int end = string.GetLength()-3;
			while(end>0)
			{
				string.Insert(end,",");
				end-=3;
			}

			stringPtrArray[currentIndex]->Format("%d Files/Folders being Copied L->R (%s Bytes)",
				m_syncStats.m_filesFoldersCopiedLeftRight,
				string);
			currentIndex++;
		}
		if ((m_syncStats.m_filesFoldersCopiedRightLeft && m_syncEnable.m_bRLCopyEnable))
		{
			CString string;
			string.Format("%d",m_syncStats.m_totalBytesCopiedRightLeft);

			// insert the commas
			int end = string.GetLength()-3;
			while(end>0)
			{
				string.Insert(end,",");
				end-=3;
			}

			stringPtrArray[currentIndex]->Format("%d Files/Folders being Copied R->L (%s Bytes)",
				m_syncStats.m_filesFoldersCopiedRightLeft,
				string);
			currentIndex++;
		}
		if ((m_syncStats.m_filesFoldersDeletedLeft && m_syncEnable.m_bLDeleteEnable))
		{
			stringPtrArray[currentIndex]->Format("%d Files/Folders deleted from Left",
				m_syncStats.m_filesFoldersDeletedLeft);
			currentIndex++;
		}
		if ((m_syncStats.m_filesFoldersDeletedRight && m_syncEnable.m_bRDeleteEnable))
		{
			stringPtrArray[currentIndex]->Format("%d Files/Folders deleted from Right",
				m_syncStats.m_filesFoldersDeletedRight);
			currentIndex++;
		}
		for (int index=currentIndex;index<4;index++)
		{
			(*stringPtrArray[index]) = "";
		}
		theApp.m_bAccelEnable = false;
		int result = confirmDialog.DoModal();
		theApp.m_bAccelEnable = true;
		if (result==IDOK)
		{
			SyncDatabases();	
		}
	}
}

void CAssetManagerDlg::OnLDelete() 
{
	if (m_syncStats.m_filesFoldersDeletedLeft)
	{
		// toggle it
		m_syncEnable.m_bLDeleteEnable = !m_syncEnable.m_bLDeleteEnable;	
		UpdateSyncStats();
	}
}

void CAssetManagerDlg::OnLrCopyButton() 
{
	if (m_syncStats.m_filesFoldersCopiedLeftRight)
	{
		// toggle it
		m_syncEnable.m_bLRCopyEnable = !m_syncEnable.m_bLRCopyEnable;	
		UpdateSyncStats();
	}
}

void CAssetManagerDlg::OnRDelete() 
{
	if (m_syncStats.m_filesFoldersDeletedRight)
	{
		// toggle it
		m_syncEnable.m_bRDeleteEnable = !m_syncEnable.m_bRDeleteEnable;	
		UpdateSyncStats();
	}
}

void CAssetManagerDlg::OnRlCopyButton() 
{
	if (m_syncStats.m_filesFoldersCopiedRightLeft)
	{
		// toggle it
		m_syncEnable.m_bRLCopyEnable = !m_syncEnable.m_bRLCopyEnable;	
		UpdateSyncStats();
	}
}

void CAssetManagerDlg::OnBttnRefresh() 
{
	Refresh();
}

void CAssetManagerDlg::Refresh(void)
{
	SaveClientData();
	m_pSyncTreeManager->ClearDatabases();
	delete m_pRightDataBase;
	m_pRightDataBase = NULL;
	delete m_pLeftDataBase;
	m_pLeftDataBase = NULL;

	m_pTreeCtrlLeft->DeleteAllItems();
	m_pTreeCtrlRight->DeleteAllItems();
	m_pTreeCtrlIndicators->DeleteAllItems();
	m_pAssociatedIconManager->ClearAll();

	m_syncEnable.m_bLRCopyEnable = false;
	m_syncEnable.m_bRLCopyEnable = false;
	m_syncEnable.m_bLDeleteEnable = false;
	m_syncEnable.m_bRDeleteEnable = false;
	LoadDatabases();
}


void CAssetManagerDlg::OnBttnViewAll() 
{
	if ((m_viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_ALL)==0)
	{
		m_viewFlags |= SyncTreeNodeFlags::VIEW_FLAGS_ALL|
					   SyncTreeNodeFlags::VIEW_FLAGS_STATUS_ACTION;
		if ((m_viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_EXCLUDE_LOCKED)==0)
		{
			m_viewFlags |= SyncTreeNodeFlags::VIEW_FLAGS_ACTION_EXCLUDE;
		}
		if ((m_viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_FROZEN_LOCKED)==0)
		{
			m_viewFlags |= SyncTreeNodeFlags::VIEW_FLAGS_ACTION_FROZEN;
		}
	}
	UpdateViewButtons();
	UpdateTreeItems();
}

void CAssetManagerDlg::SetViewFlags(unsigned long flags) 
{
	if (m_viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_ALL)
	{
		m_viewFlags &= ~(SyncTreeNodeFlags::VIEW_FLAGS_ALL|SyncTreeNodeFlags::VIEW_FLAGS_STATUS_ACTION);
		if ((m_viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_EXCLUDE_LOCKED)==0)
		{
			m_viewFlags &= ~SyncTreeNodeFlags::VIEW_FLAGS_ACTION_EXCLUDE;
		}
		if ((m_viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_FROZEN_LOCKED)==0)
		{
			m_viewFlags &= ~SyncTreeNodeFlags::VIEW_FLAGS_ACTION_FROZEN;
		}
		m_viewFlags |= flags;
	}
	else
	{
		m_viewFlags ^= flags;
	}
	UpdateViewButtons();
	UpdateTreeItems();
}
	
void CAssetManagerDlg::OnBttnViewCpyLeft() 
{
	SetViewFlags(SyncTreeNodeFlags::VIEW_FLAGS_ACTION_COPY_RIGHT_TO_LEFT);
}

void CAssetManagerDlg::OnBttnViewCpyRight() 
{
	SetViewFlags(SyncTreeNodeFlags::VIEW_FLAGS_ACTION_COPY_LEFT_TO_RIGHT);
}

void CAssetManagerDlg::OnBttnViewDltBoth() 
{
	SetViewFlags(SyncTreeNodeFlags::VIEW_FLAGS_ACTION_DELETE_BOTH);
}

void CAssetManagerDlg::OnBttnViewDltLeft() 
{
	SetViewFlags(SyncTreeNodeFlags::VIEW_FLAGS_ACTION_DELETE_LEFT);
}

void CAssetManagerDlg::OnBttnViewDltRight() 
{
	SetViewFlags(SyncTreeNodeFlags::VIEW_FLAGS_ACTION_DELETE_RIGHT);
}

void CAssetManagerDlg::OnBttnViewEqual() 
{
	SetViewFlags(SyncTreeNodeFlags::VIEW_FLAGS_STATUS_LEFT_RIGHT_EQUAL);
}

void CAssetManagerDlg::OnBttnViewNewerLeft() 
{
	SetViewFlags(SyncTreeNodeFlags::VIEW_FLAGS_STATUS_LEFT_NEWER);
}

void CAssetManagerDlg::OnBttnViewNewerRight() 
{
	SetViewFlags(SyncTreeNodeFlags::VIEW_FLAGS_STATUS_RIGHT_NEWER);
}

void CAssetManagerDlg::OnBttnViewOrphanLeft() 
{
	SetViewFlags(SyncTreeNodeFlags::VIEW_FLAGS_STATUS_LEFT_ONLY);
}

void CAssetManagerDlg::OnBttnViewOrphanRight() 
{
	SetViewFlags(SyncTreeNodeFlags::VIEW_FLAGS_STATUS_RIGHT_ONLY);
}

void CAssetManagerDlg::OnBttnViewExcluded() 
{
	if ((m_viewFlags&SyncTreeNodeFlags::VIEW_FLAGS_EXCLUDE_LOCKED)==0)
	{
		SetViewFlags(SyncTreeNodeFlags::VIEW_FLAGS_ACTION_EXCLUDE);
	}
	else
	{
		m_viewFlags ^= SyncTreeNodeFlags::VIEW_FLAGS_ACTION_EXCLUDE;
		UpdateViewButtons();
		UpdateTreeItems();
	}
}

void CAssetManagerDlg::OnBttnViewFrozen() 
{
	if ((m_viewFlags&SyncTreeNodeFlags::VIEW_FLAGS_FROZEN_LOCKED)==0)
	{
		SetViewFlags(SyncTreeNodeFlags::VIEW_FLAGS_ACTION_FROZEN);
	}
	else
	{
		m_viewFlags ^= SyncTreeNodeFlags::VIEW_FLAGS_ACTION_FROZEN;
		UpdateViewButtons();
		UpdateTreeItems();
	}
}

void CAssetManagerDlg::OnLockExcluded() 
{
	m_viewFlags ^= SyncTreeNodeFlags::VIEW_FLAGS_EXCLUDE_LOCKED;
	// if not locked and view all button down then turn on view excluded
	if ((m_viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_ALL) &&
		(m_viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_EXCLUDE_LOCKED)==0)
	{
		m_viewFlags |= SyncTreeNodeFlags::VIEW_FLAGS_ACTION_EXCLUDE;
	}
	UpdateViewButtons();
	UpdateTreeItems();
}

void CAssetManagerDlg::OnLockFrozen() 
{
	m_viewFlags ^= SyncTreeNodeFlags::VIEW_FLAGS_FROZEN_LOCKED;
	// if not locked and view all button down then turn on view frozen
	if ((m_viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_ALL) &&
		(m_viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_FROZEN_LOCKED)==0)
	{
		m_viewFlags |= SyncTreeNodeFlags::VIEW_FLAGS_ACTION_FROZEN;
	}
	UpdateViewButtons();
	UpdateTreeItems();
}


void CAssetManagerDlg::UpdateViewButtons(void)
{
	m_bttnViewAll.SetState((m_viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_ALL) ? TRUE : FALSE);
	m_bttnViewCpyLeft.SetState((m_viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_ACTION_COPY_RIGHT_TO_LEFT) ? TRUE : FALSE);
	m_bttnViewCpyRight.SetState((m_viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_ACTION_COPY_LEFT_TO_RIGHT) ? TRUE : FALSE);
	m_bttnViewDltBoth.SetState((m_viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_ACTION_DELETE_BOTH) ? TRUE : FALSE);
	m_bttnViewDltLeft.SetState((m_viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_ACTION_DELETE_LEFT) ? TRUE : FALSE);
	m_bttnViewDltRight.SetState((m_viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_ACTION_DELETE_RIGHT) ? TRUE : FALSE);
	m_bttnViewEqual.SetState((m_viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_STATUS_LEFT_RIGHT_EQUAL) ? TRUE : FALSE);
	m_bttnViewNewerLeft.SetState((m_viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_STATUS_LEFT_NEWER) ? TRUE : FALSE);
	m_bttnViewNewerRight.SetState((m_viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_STATUS_RIGHT_NEWER) ? TRUE : FALSE);
	m_bttnViewOrphanLeft.SetState((m_viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_STATUS_LEFT_ONLY) ? TRUE : FALSE);
	m_bttnViewOrphanRight.SetState((m_viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_STATUS_RIGHT_ONLY) ? TRUE : FALSE);

	m_lockExcludedCheckbox.SetCheck((m_viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_EXCLUDE_LOCKED) ? TRUE : FALSE);
	m_lockFrozenCheckbox.SetCheck((m_viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_FROZEN_LOCKED) ? TRUE : FALSE);
	
	m_bttnViewExcluded.SetState((m_viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_ACTION_EXCLUDE) ? TRUE : FALSE);
//	m_bttnViewExcluded.SetBitmap((m_viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_EXCLUDE_LOCKED) ? m_hBmapViewExcludedDisabled : m_hBmapViewExcluded);
	
	m_bttnViewFrozen.SetState((m_viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_ACTION_FROZEN) ? TRUE : FALSE);
//	m_bttnViewFrozen.SetBitmap((m_viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_FROZEN_LOCKED) ? m_hBmapViewFrozenDisabled : m_hBmapViewFrozen);
}

// used when the view flags have changed or an action change has occurred
void CAssetManagerDlg::UpdateTreeItems(void)
{
	UpdateSyncStats();

	m_pTreeCtrlLeft->SetAllowPaint(false);
	m_pTreeCtrlRight->SetAllowPaint(false);
	m_pTreeCtrlIndicators->SetAllowPaint(false);

	SetCursor( LoadCursor( NULL, IDC_WAIT ) );

//	m_pTreeCtrlLeft->DeselectAll();
//	m_pTreeCtrlRight->DeselectAll();
//	m_pTreeCtrlIndicators->DeselectAll();

	AMSyncTreeNodeFolder* pAMSyncTreeRoot = dynamic_cast<AMSyncTreeNodeFolder*>(m_pSyncTreeManager->GetSyncTree());
	ASSERT(pAMSyncTreeRoot != NULL);

	TreeIterator first(pAMSyncTreeRoot);
	TreeIterator last(pAMSyncTreeRoot);
	last.End();

	// Iterate through all the children of this folder:
	for ( ; first < last; first.Increment())
	{
		SetTreeItem(*first,true);
	}

	m_pTreeCtrlLeft->ReMeasureAllItems();
	m_pTreeCtrlRight->ReMeasureAllItems();
	m_pTreeCtrlIndicators->ReMeasureAllItems();

	m_pTreeCtrlLeft->SetAllowPaint(true);
	m_pTreeCtrlRight->SetAllowPaint(true);
	m_pTreeCtrlIndicators->SetAllowPaint(true);


	m_pTreeCtrlLeft->Invalidate();
	m_pTreeCtrlRight->Invalidate();
	m_pTreeCtrlIndicators->Invalidate();

	SetCursor( LoadCursor( NULL, IDC_ARROW ) );

}

void CAssetManagerDlg::SaveFolderExpandState(void)
{
	AMSyncTreeNodeFolder* pAMSyncTreeRoot = dynamic_cast<AMSyncTreeNodeFolder*>(m_pSyncTreeManager->GetSyncTree());
	ASSERT(pAMSyncTreeRoot != NULL);

	TreeIterator first(pAMSyncTreeRoot);
	TreeIterator last(pAMSyncTreeRoot);
	last.End();
	// step through child containers
	for (;first<last;first.Increment())
	{
		SaveFolderExpandState(*first);
	}
}

void CAssetManagerDlg::SaveFolderExpandState(TreeNodeBase *pNode)
{
	// check and see if this node is a sync folder
	AMSyncTreeNodeFolder *pAMSyncNodeFolder = dynamic_cast<AMSyncTreeNodeFolder *>(pNode);
	// if this is not null then it is a sync folder
	if (pAMSyncNodeFolder)
	{
		int flags = pAMSyncNodeFolder->GetFlags();

		HTREEITEM leftItem = pAMSyncNodeFolder->GetHTreeItemLeft();
		if (leftItem) 
		{
			flags &= ~SyncTreeNodeFlags::FLAGS_EXPANDED; // not expanded as a default
			if (m_pTreeCtrlLeft->IsExpanded(leftItem))
				flags |= SyncTreeNodeFlags::FLAGS_EXPANDED;
			pAMSyncNodeFolder->SetFlags(flags);
		}

		TreeIterator first(pAMSyncNodeFolder);
		TreeIterator last(pAMSyncNodeFolder);
		last.End();
		// step through child containers
		for (;first<last;first.Increment())
		{
			SaveFolderExpandState(*first);
		}
	}
}

void CAssetManagerDlg::ClearHtreeItems(TreeNodeBase *pNode,bool doChildren)
{
	// check and see if this node is a sync folder
	AMSyncTreeNodeFolder *pAMSyncNodeFolder = dynamic_cast<AMSyncTreeNodeFolder *>(pNode);
	// if this is not null then it is a sync folder
	if (pAMSyncNodeFolder)
	{
		pAMSyncNodeFolder->SetHTreeItemLeft(NULL);
		pAMSyncNodeFolder->SetHTreeItemRight(NULL);
		pAMSyncNodeFolder->SetHTreeItemIndicator(NULL);

		if (doChildren)
		{
			TreeIterator first(pAMSyncNodeFolder);
			TreeIterator last(pAMSyncNodeFolder);
			last.End();
			// step through child containers
			for (;first<last;first.Increment())
			{
				ClearHtreeItems(*first,true);
			}
		}
	}

	AMSyncTreeNodeFile *pAMSyncNodeFile = dynamic_cast<AMSyncTreeNodeFile *>(pNode);
	// if this is not null then it is a sync folder
	if (pAMSyncNodeFile)
	{
		pAMSyncNodeFile->SetHTreeItemLeft(NULL);
		pAMSyncNodeFile->SetHTreeItemRight(NULL);
		pAMSyncNodeFile->SetHTreeItemIndicator(NULL);
	}
}

void CAssetManagerDlg::SaveTreeSelectedState(void)
{
	AMSyncTreeNodeFolder* pAMSyncTreeRoot = dynamic_cast<AMSyncTreeNodeFolder*>(m_pSyncTreeManager->GetSyncTree());
	ASSERT(pAMSyncTreeRoot != NULL);

	bool bLeftTree=true;
	bool bTreeHasFocus=false;
	CWnd *pFocusWnd = GetFocus();
	if (pFocusWnd && ::IsWindow(pFocusWnd->m_hWnd))
	{
		if (pFocusWnd == m_pTreeCtrlLeft)
		{
			bTreeHasFocus=true;
			bLeftTree = true;
		}
		else if (pFocusWnd == m_pTreeCtrlRight)
		{
			bTreeHasFocus=true;
			bLeftTree = false;
		}
	}

	TreeIterator first(pAMSyncTreeRoot);
	TreeIterator last(pAMSyncTreeRoot);
	last.End();
	// step through child containers
	for (;first<last;first.Increment())
	{
		SaveTreeSelectedState(*first,bTreeHasFocus,bLeftTree);
	}
}

void CAssetManagerDlg::SaveTreeSelectedState(TreeNodeBase *pNode,bool bGetFromTree,bool bLeftTree)
{
	// check and see if this node is a sync folder
	AMSyncTreeNodeFolder *pAMSyncNodeFolder = dynamic_cast<AMSyncTreeNodeFolder *>(pNode);
	// if this is not null then it is a sync folder
	if (pAMSyncNodeFolder)
	{
		int flags = pAMSyncNodeFolder->GetFlags();
		flags &= ~SyncTreeNodeFlags::FLAGS_SELECTED; // not selected

		if (bGetFromTree)
		{
			if (bLeftTree)
			{
				HTREEITEM leftItem = pAMSyncNodeFolder->GetHTreeItemLeft();
				if (leftItem) 
				{
					if (m_pTreeCtrlLeft->IsSelected(leftItem))
						flags |= SyncTreeNodeFlags::FLAGS_SELECTED;
				}
			}
			else
			{
				HTREEITEM rightItem = pAMSyncNodeFolder->GetHTreeItemRight();
				if (rightItem) 
				{
					if (m_pTreeCtrlRight->IsSelected(rightItem))
						flags |= SyncTreeNodeFlags::FLAGS_SELECTED;
				}
			}
		}
		pAMSyncNodeFolder->SetFlags(flags);

		TreeIterator first(pAMSyncNodeFolder);
		TreeIterator last(pAMSyncNodeFolder);
		last.End();
		// step through child containers
		for (;first<last;first.Increment())
		{
			SaveTreeSelectedState(*first,bGetFromTree,bLeftTree);
		}
	}

	AMSyncTreeNodeFile *pAMSyncNodeFile = dynamic_cast<AMSyncTreeNodeFile *>(pNode);
	// if this is not null then it is a sync folder
	if (pAMSyncNodeFile)
	{
		int flags = pAMSyncNodeFile->GetFlags();
		flags &= ~(SyncTreeNodeFlags::FLAGS_SELECTED|SyncTreeNodeFlags::FLAGS_FOCUS); // not selected or focus

		if (bGetFromTree)
		{
			if (bLeftTree)
			{
				HTREEITEM leftItem = pAMSyncNodeFile->GetHTreeItemLeft();
				if (leftItem) 
				{
					if (m_pTreeCtrlLeft->IsFocus(leftItem))
						flags |= SyncTreeNodeFlags::FLAGS_FOCUS;

					if (m_pTreeCtrlLeft->IsSelected(leftItem))
						flags |= SyncTreeNodeFlags::FLAGS_SELECTED;
				}
			}
			else
			{
				HTREEITEM rightItem = pAMSyncNodeFile->GetHTreeItemRight();
				if (rightItem) 
				{
					if (m_pTreeCtrlRight->IsFocus(rightItem))
						flags |= SyncTreeNodeFlags::FLAGS_FOCUS;

					if (m_pTreeCtrlRight->IsSelected(rightItem))
						flags |= SyncTreeNodeFlags::FLAGS_SELECTED;
				}
			}
		}
		pAMSyncNodeFile->SetFlags(flags);
	}
}

unsigned long CAssetManagerDlg::GetValidActions(void)
{
	unsigned long validActionFlags = 0;

	// Only do this if there are two trees:
	if (GetMode() == POSTCOMPARE)
	{
		// Check to see which tree items we need to change:
		CAssetManagerTreeCtrl* pTreeControl;
		if (m_bContextFromLeft == true)
			pTreeControl = m_pTreeCtrlLeft;
		else
			pTreeControl = m_pTreeCtrlRight;

		HTREEITEM hCurrItem = pTreeControl->GetFirstSelectedItem();
		bool bFoundCtrlNode = false;
		while (hCurrItem != NULL)
		{
			SEC_DWORD sDWItemData = pTreeControl->GetItemData(hCurrItem);
			AMSyncTreeNodeFolder* pSyncNodeFolder = dynamic_cast<AMSyncTreeNodeFolder*>((TreeNodeBase*) sDWItemData);
			if (pSyncNodeFolder != NULL)
			{
				TreeNodeFolder *pCtrlFolder = m_bContextFromLeft ? pSyncNodeFolder->GetLeftFolderNode() : pSyncNodeFolder->GetRightFolderNode();
				if (pCtrlFolder)
					bFoundCtrlNode = true;

				validActionFlags |= m_pSyncTreeManager->GetValidActionFlags(pSyncNodeFolder,m_bContextFromLeft, true);
			}
			else
			{
				AMSyncTreeNodeFile* pSyncNodeFile = dynamic_cast<AMSyncTreeNodeFile*>((TreeNodeBase*) sDWItemData);
				if (pSyncNodeFile != NULL)
				{
					TreeNodeFile *pCtrlFile = m_bContextFromLeft ? pSyncNodeFile->GetLeftFileNode() : pSyncNodeFile->GetRightFileNode();
					if (pCtrlFile)
						bFoundCtrlNode = true;

					validActionFlags |= m_pSyncTreeManager->GetValidActionFlags(pSyncNodeFile,m_bContextFromLeft, true);
				}
				else
				{
					// Should be either a file or folder!
					ASSERT(false);
				}				
			}
			hCurrItem = pTreeControl->GetNextSelectedItem(hCurrItem);
		}
		int numSelected = pTreeControl->GetSelectedCount();
		int numItems = pTreeControl->GetItemCount();
		if (numItems)
		{
			// if none are selected
			if (numSelected==0)
			{
				validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_SELECT_ALL |
											SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_INVERT_SELECTION);
			}
			// if not all are selected
			else if (numSelected<numItems)
			{
				validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_SELECT_NONE |
											SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_SELECT_ALL |
											SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_INVERT_SELECTION |
											SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_UPDATE_DUPLICATES |
											SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_UNFREEZE_ONLY);

				if (bFoundCtrlNode && numSelected==1)
				{
					validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_RENAME |
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXPLORE);
				}
			}
			// if all are selected
			else
			{
				validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_SELECT_NONE |
											SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_UPDATE_DUPLICATES |
											SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_INVERT_SELECTION);
			}
		}

	}
	return validActionFlags;
}

void CAssetManagerDlg::SetupToolTips(void)
{
	//Set up the tooltip
	m_pToolTip = new CPPToolTip;
	int toolTipBehavior = PPTOOLTIP_MULTIPLE_SHOW | PPTOOLTIP_TRACKING_MOUSE | PPTOOLTIP_CLOSE_LEAVEWND | PPTOOLTIP_NOCLOSE_OVER;
	int toolTipStyle = PPTOOLTIP_SHADOW;
	m_pToolTip->SetBehaviour(toolTipBehavior);
	m_pToolTip->SetStyles(toolTipStyle);

	m_pToolTip->Create(this);

	m_pToolTip->SetDelayTime(TTDT_AUTOPOP,0);
	m_pToolTip->SetDelayTime(TTDT_INITIAL,500);
	m_pToolTip->SetDelayTime(TTDT_RESHOW,100);

	m_pToolTip->AddTool(&m_syncButton,IDC_SYNCBUTTON);
	m_pToolTip->AddTool(&m_bttnViewFrozen,IDC_BTTN_VIEW_FROZEN);
	m_pToolTip->AddTool(&m_rlCopyButton,IDC_RL_COPY_BUTTON);
	m_pToolTip->AddTool(&m_lDeleteButton,IDC_L_DELETE);
	m_pToolTip->AddTool(&m_rDeleteButton,IDC_R_DELETE);
	m_pToolTip->AddTool(&m_lrCopyButton,IDC_LR_COPY_BUTTON);
	m_pToolTip->AddTool(&m_bttnFolderSetsBrowse,IDC_BUTTON_FOLDER_SETS_BROWSE);
	m_pToolTip->AddTool(&m_bttnFolderSetsSort,IDC_BUTTON_FOLDER_SETS_SORT);
	m_pToolTip->AddTool(&m_folderSetsCombo,IDC_FOLDER_SETS);
	m_pToolTip->AddTool(&m_bttnRefresh,IDC_BTTN_REFRESH);
	m_pToolTip->AddTool(&m_bttnViewAll,IDC_BTTN_VIEW_ALL);
	m_pToolTip->AddTool(&m_bttnViewCpyLeft,IDC_BTTN_VIEW_CPY_LEFT);
	m_pToolTip->AddTool(&m_bttnViewCpyRight,IDC_BTTN_VIEW_CPY_RIGHT);
	m_pToolTip->AddTool(&m_bttnViewDltBoth,IDC_BTTN_VIEW_DLT_BOTH);
	m_pToolTip->AddTool(&m_bttnViewDltLeft,IDC_BTTN_VIEW_DLT_LEFT);
	m_pToolTip->AddTool(&m_bttnViewDltRight,IDC_BTTN_VIEW_DLT_RIGHT);
	m_pToolTip->AddTool(&m_bttnViewEqual,IDC_BTTN_VIEW_EQUAL);
	m_pToolTip->AddTool(&m_bttnViewNewerLeft,IDC_BTTN_VIEW_NEWER_LEFT);
	m_pToolTip->AddTool(&m_bttnViewNewerRight,IDC_BTTN_VIEW_NEWER_RIGHT);
	m_pToolTip->AddTool(&m_bttnViewOrphanLeft,IDC_BTTN_VIEW_ORPHAN_LEFT);
	m_pToolTip->AddTool(&m_bttnViewOrphanRight,IDC_BTTN_VIEW_ORPHAN_RIGHT);
	m_pToolTip->AddTool(&m_bttnViewExcluded,IDC_BTTN_VIEW_EXCLUDED);
	m_pToolTip->AddTool(&m_lockExcludedCheckbox,IDC_LOCK_EXCLUDED);
	m_pToolTip->AddTool(&m_lockFrozenCheckbox,IDC_LOCK_FROZEN);
	m_pToolTip->AddTool(&m_syncStyleCombo,IDC_SYNC_STYLE);
	m_pToolTip->Activate(TRUE);

	m_pDataTip = new CPPToolTip;
	int dataTipBehavior = PPTOOLTIP_MULTIPLE_SHOW | PPTOOLTIP_TRACKING_MOUSE | PPTOOLTIP_CLOSE_LEAVEWND;
	int dataTipStyle = PPTOOLTIP_SHADOW;
	m_pDataTip->SetBehaviour(dataTipBehavior);
	m_pDataTip->SetStyles(dataTipStyle);

	m_pDataTip->Create(this);

	m_pDataTip->SetDelayTime(TTDT_AUTOPOP,0);
	m_pDataTip->SetDelayTime(TTDT_INITIAL,200);
	m_pDataTip->SetDelayTime(TTDT_RESHOW,200);

	m_pDataTip->AddTool(m_pTreeCtrlLeft,IDC_LOCK_FROZEN);
	m_pDataTip->AddTool(m_pTreeCtrlRight,IDC_LOCK_FROZEN);
	m_pDataTip->AddTool(m_pTreeCtrlIndicators,IDC_LOCK_FROZEN);
	m_pDataTip->SetNotify(TRUE);
	m_pDataTip->SetNotify(m_hWnd);
	m_pDataTip->Activate(TRUE);
}

void CAssetManagerDlg::TreeMouseMove(UINT nFlags, CPoint point) 
{
	HTREEITEM hTreeItem;
	CPoint screenPoint(point);
	ClientToScreen(&screenPoint);

	MouseOverData mouseOverData;
	CPoint testPoint;
	testPoint = screenPoint;
	m_pTreeCtrlLeft->ScreenToClient(&testPoint);
	hTreeItem = m_pTreeCtrlLeft->HitTest(testPoint);
	if (hTreeItem)
	{
		CWnd *pFocusWnd = GetFocus();
		if (pFocusWnd!=m_pTreeCtrlLeft && pFocusWnd!=m_pTreeCtrlRight)
		{
			m_pTreeCtrlLeft->SetFocus();
		}

		SEC_DWORD sDWItemData = m_pTreeCtrlLeft->GetItemData(hTreeItem);
		AMSyncTreeNodeFolder* pSyncNodeFolder = dynamic_cast<AMSyncTreeNodeFolder*>((TreeNodeBase*) sDWItemData);
		if (pSyncNodeFolder)
		{
			mouseOverData.m_type = MouseOverData::OVER_FOLDER;
			mouseOverData.m_control = MouseOverData::LEFT_TREE;
			mouseOverData.m_pSyncNodeFolder = pSyncNodeFolder;
			mouseOverData.m_pSyncNodeFile = NULL;
		}
		AMSyncTreeNodeFile* pSyncNodeFile = dynamic_cast<AMSyncTreeNodeFile*>((TreeNodeBase*) sDWItemData);
		if (pSyncNodeFile)
		{
			mouseOverData.m_type = MouseOverData::OVER_FILE;
			mouseOverData.m_control = MouseOverData::LEFT_TREE;
			mouseOverData.m_pSyncNodeFile = pSyncNodeFile;
			mouseOverData.m_pSyncNodeFolder = NULL;
		}
	}
	if (mouseOverData.m_type==MouseOverData::OVER_NONE)
	{
		testPoint = screenPoint;
		m_pTreeCtrlRight->ScreenToClient(&testPoint);
		hTreeItem = m_pTreeCtrlRight->HitTest(testPoint);
		if (hTreeItem)
		{
			CWnd *pFocusWnd = GetFocus();
			if (pFocusWnd!=m_pTreeCtrlLeft && pFocusWnd!=m_pTreeCtrlRight)
			{
				m_pTreeCtrlRight->SetFocus();
			}

			SEC_DWORD sDWItemData = m_pTreeCtrlRight->GetItemData(hTreeItem);
			AMSyncTreeNodeFolder* pSyncNodeFolder = dynamic_cast<AMSyncTreeNodeFolder*>((TreeNodeBase*) sDWItemData);
			if (pSyncNodeFolder)
			{
				mouseOverData.m_type = MouseOverData::OVER_FOLDER;
				mouseOverData.m_control = MouseOverData::RIGHT_TREE;
				mouseOverData.m_pSyncNodeFolder = pSyncNodeFolder;
				mouseOverData.m_pSyncNodeFile = NULL;
			}
			AMSyncTreeNodeFile* pSyncNodeFile = dynamic_cast<AMSyncTreeNodeFile*>((TreeNodeBase*) sDWItemData);
			if (pSyncNodeFile)
			{
				mouseOverData.m_type = MouseOverData::OVER_FILE;
				mouseOverData.m_control = MouseOverData::RIGHT_TREE;
				mouseOverData.m_pSyncNodeFile = pSyncNodeFile;
				mouseOverData.m_pSyncNodeFolder = NULL;
			}
		}
	}
	if (mouseOverData.m_type==MouseOverData::OVER_NONE)
	{
		testPoint = screenPoint;
		m_pTreeCtrlIndicators->ScreenToClient(&testPoint);
		hTreeItem = m_pTreeCtrlIndicators->HitTest(testPoint);
		if (hTreeItem)
		{
			CWnd *pFocusWnd = GetFocus();
			if (pFocusWnd!=m_pTreeCtrlLeft && pFocusWnd!=m_pTreeCtrlRight && pFocusWnd!=m_pTreeCtrlIndicators)
			{
				m_pTreeCtrlIndicators->SetFocus();
			}

			SEC_DWORD sDWItemData = m_pTreeCtrlIndicators->GetItemData(hTreeItem);
			AMSyncTreeNodeFolder* pSyncNodeFolder = dynamic_cast<AMSyncTreeNodeFolder*>((TreeNodeBase*) sDWItemData);
			if (pSyncNodeFolder)
			{
				mouseOverData.m_type = MouseOverData::OVER_FOLDER;
				mouseOverData.m_control = MouseOverData::INDICATOR_TREE;
				mouseOverData.m_pSyncNodeFolder = pSyncNodeFolder;
				mouseOverData.m_pSyncNodeFile = NULL;
			}
			AMSyncTreeNodeFile* pSyncNodeFile = dynamic_cast<AMSyncTreeNodeFile*>((TreeNodeBase*) sDWItemData);
			if (pSyncNodeFile)
			{
				mouseOverData.m_type = MouseOverData::OVER_FILE;
				mouseOverData.m_control = MouseOverData::INDICATOR_TREE;
				mouseOverData.m_pSyncNodeFile = pSyncNodeFile;
				mouseOverData.m_pSyncNodeFolder = NULL;
			}
		}
	}
	if (mouseOverData.m_type==MouseOverData::OVER_NONE)
	{
		if (m_mouseOverData.m_type!=MouseOverData::OVER_NONE)
		{
			m_pDataTip->Activate(FALSE);
		}
	}
	else
	{
		if (m_mouseOverData != mouseOverData)
		{
			m_mouseOverData = mouseOverData;
			m_pDataTip->Activate(FALSE);
			m_pDataTip->Activate(TRUE);
		}
	}
}

void CAssetManagerDlg::SetFolderSetToLoad(const char *pFolderSetName)
{
	if (pFolderSetName)
	{
		m_folderSetToLoad = pFolderSetName;
	}
}

void CAssetManagerDlg::SetSyncSyleToUse(int syncStyle)
{
	m_overrideSyncStyle = syncStyle;
}

void CAssetManagerDlg::SetAutoPressButtons(bool bCopyToRight,
														 bool bCopyToLeft,
														 bool bDeleteRight,
														 bool bDeleteLeft,
														 bool bSync)
{
	m_bAutoPressCopyToRight = bCopyToRight;
	m_bAutoPressCopyToLeft = bCopyToLeft;
	m_bAutoPressDeleteRight = bDeleteRight;
	m_bAutoPressDeleteLeft = bDeleteLeft;
	m_bAutoPressSync = bSync;
}

CString CAssetManagerDlg::GetIndicatorFileText(int flags)
{
	CString result = "";

	if (flags & SyncTreeNodeFlags::FLAGS_FROZEN)
	{
		result = "Frozen\n";
	}
	else
	{
		switch (flags & SyncTreeNodeFlags::FLAGS_ACTION_MASK)
		{
			case SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED:
				result = "Excluded\n";
				break;
			case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_LEFT:
				result = "Delete from the Left\n";
				break;
			case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_RIGHT:
				result = "Delete from the Right\n";
				break;
			case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_BOTH:
				result = "Delete from the Left and Right\n";
				break;
			case SyncTreeNodeFlags::FLAGS_ACTION_COPY_LEFT_TO_RIGHT:
				result = "Copy from Left to Right\n";
				break;
			case SyncTreeNodeFlags::FLAGS_ACTION_COPY_RIGHT_TO_LEFT:
				result = "Copy from Right to Left\n";
				break;
		}
	}
	switch (flags & SyncTreeNodeFlags::FLAGS_STATUS_MASK)
	{
		case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY:
			result += "Orphan Left";
			break;
		case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY:
			result += "Orphan Right";
			break;
		case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL:
			result += "Left and Right Equal";
			break;
		case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_NEWER:
			result += "Left Newer";
			break;
		case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_NEWER:
			result += "Right Newer";
			break;
	}
	return result;
}

CString CAssetManagerDlg::GetIndicatorFolderText(SyncTreeNodeFolder *pFolderNode)
{
	CString result;
	SyncTreeManager::TreeStats treeStats;
	memset(&treeStats,0,sizeof(SyncTreeManager::TreeStats));
	m_pSyncTreeManager->GetTreeStats(pFolderNode,treeStats);
	bool bAddNewline = false;
	CString sizeString;
	if (treeStats.m_filesFoldersCopiedLeftRight)
	{
		sizeString.Format("%I64d",treeStats.m_totalBytesCopiedLeftRight);
		// insert the commas
		int end = sizeString.GetLength()-3;
		while(end>0)
		{
			sizeString.Insert(end,",");
			end-=3;
		}
		CString tempString;
		tempString.Format("%d Files/Folders Copy Left to Right %s Bytes",treeStats.m_filesFoldersCopiedLeftRight,(const char *)sizeString);
		result += tempString;
		bAddNewline = true;
	}
	if (treeStats.m_filesFoldersCopiedRightLeft)
	{
		sizeString.Format("%I64d",treeStats.m_totalBytesCopiedRightLeft);
		// insert the commas
		int end = sizeString.GetLength()-3;
		while(end>0)
		{
			sizeString.Insert(end,",");
			end-=3;
		}
		if (bAddNewline)
			result += "\n";
		CString tempString;
		tempString.Format("%d Files/Folders Copy Right to Left",treeStats.m_filesFoldersCopiedRightLeft,(const char *)sizeString);
		result += tempString;
		bAddNewline = true;
	}
	if (treeStats.m_filesFoldersDeletedLeft)
	{
		if (bAddNewline)
			result += "\n";
		CString tempString;
		tempString.Format("%d Files/Folders Deleted Left",treeStats.m_filesFoldersDeletedLeft);
		result += tempString;
		bAddNewline = true;
	}
	if (treeStats.m_filesFoldersDeletedRight)
	{
		if (bAddNewline)
			result += "\n";
		CString tempString;
		tempString.Format("%d Files/Folders Deleted Right",treeStats.m_filesFoldersDeletedRight);
		result += tempString;
		bAddNewline = true;
	}
	return result;
}

CString CAssetManagerDlg::GetFormattedFileText(TreeNodeFile *pFileNode)
{
	CString result = "";
	CTime modifiedTime(pFileNode->GetModifiedTime());
	CString timeString = modifiedTime.Format("%A, %B %d, %Y %I:%M %p");
	TupString pathString;
	pFileNode->GetPath(pathString);
	CString sizeString;
	sizeString.Format("%d",pFileNode->GetFileLength());
	// insert the commas
	int end = sizeString.GetLength()-3;
	while(end>0)
	{
		sizeString.Insert(end,",");
		end-=3;
	}
	result.Format(IDS_TIP_FILE,(const char *)pathString,(const char *)timeString,(const char *)sizeString);
	return result;
}

CString CAssetManagerDlg::GetFormattedFileTextSync(TreeNodeFile *pFileNode,FILETIME syncDate)
{
	CString result = "";
	CTime modifiedTime(pFileNode->GetModifiedTime());
	CString modifiedTimeString = modifiedTime.Format("%A, %B %d, %Y %I:%M %p");
	CTime syncTime(syncDate);
	CString syncTimeString = syncTime.Format("%A, %B %d, %Y %I:%M %p");

	TupString pathString;
	pFileNode->GetPath(pathString);
	CString sizeString;
	sizeString.Format("%d",pFileNode->GetFileLength());
	// insert the commas
	int end = sizeString.GetLength()-3;
	while(end>0)
	{
		sizeString.Insert(end,",");
		end-=3;
	}
	result.Format(IDS_TIP_FILE_SYNC,(const char *)pathString,(const char *)modifiedTimeString,(const char *)syncTimeString,(const char *)sizeString);
	return result;
}

CString CAssetManagerDlg::GetFormattedFolderText(TreeNodeFolder *pFolderNode)
{
	CString result = "";
	TupString pathString;
	pFolderNode->GetPath(pathString);
	CString sizeString;
	sizeString.Format("%I64d",pFolderNode->GetTotalSize());
	// insert the commas
	int end = sizeString.GetLength()-3;
	while(end>0)
	{
		sizeString.Insert(end,",");
		end-=3;
	}
	result.Format(IDS_TIP_FOLDER,(const char *)pathString,(const char *)sizeString);
	return result;
}

CString CAssetManagerDlg::GetMouseOverTipText(void)
{
	CString result = "";
	switch (m_mouseOverData.m_type)
	{
		case MouseOverData::OVER_FILE:
			{
				TreeNodeFile *pFileNode;
				switch (m_mouseOverData.m_control)
				{
				case MouseOverData::LEFT_TREE:
					{
						pFileNode = m_mouseOverData.m_pSyncNodeFile->GetLeftFileNode();
						if (pFileNode)
						{
							int activeSet = m_pFolderSets->GetActiveSet();
							if (m_pFolderSets->GetUseSyncDates(activeSet) && (m_mouseOverData.m_pSyncNodeFile->GetFlags() & SyncTreeNodeFlags::FLAGS_HAS_SYNC_DATE))
							{
								result = GetFormattedFileTextSync(pFileNode,m_mouseOverData.m_pSyncNodeFile->GetLeftSyncDate());
							}
							else
							{
								result = GetFormattedFileText(pFileNode);
							}
						}
					}
					break;
				case MouseOverData::RIGHT_TREE:
					{
						pFileNode = m_mouseOverData.m_pSyncNodeFile->GetRightFileNode();
						if (pFileNode)
						{
							int activeSet = m_pFolderSets->GetActiveSet();
							if (m_pFolderSets->GetUseSyncDates(activeSet) && (m_mouseOverData.m_pSyncNodeFile->GetFlags() & SyncTreeNodeFlags::FLAGS_HAS_SYNC_DATE))
							{
								result = GetFormattedFileTextSync(pFileNode,m_mouseOverData.m_pSyncNodeFile->GetRightSyncDate());
							}
							else
							{
								result = GetFormattedFileText(pFileNode);
							}
						}
					}
					break;
				case MouseOverData::INDICATOR_TREE:
					{
						int flags = m_mouseOverData.m_pSyncNodeFile->GetFlags();
						result = GetIndicatorFileText(flags);
					}
					break;
				}
			}
			break;
		case MouseOverData::OVER_FOLDER:
			{
				TreeNodeFolder *pFolderNode;
				switch (m_mouseOverData.m_control)
				{
				case MouseOverData::LEFT_TREE:
					{
						pFolderNode = m_mouseOverData.m_pSyncNodeFolder->GetLeftFolderNode();
						if (pFolderNode)
						{
							result = GetFormattedFolderText(pFolderNode);
						}
					}
					break;
				case MouseOverData::RIGHT_TREE:
					{
						pFolderNode = m_mouseOverData.m_pSyncNodeFolder->GetRightFolderNode();
						if (pFolderNode)
						{
							result = GetFormattedFolderText(pFolderNode);
						}
					}
					break;
				case MouseOverData::INDICATOR_TREE:
					{
						result = GetIndicatorFolderText(m_mouseOverData.m_pSyncNodeFolder);
					}
					break;
				}
				
			}
			break;
	}
	return result;
}


// this is a static function that is called when the new thread is started
// the pParam is the address of the asset manager dialog
UINT CAssetManagerDlg::UpdateDuplicatesProc(LPVOID pParam)
{
	// we will now call a member function of the dialog to do the stuff
	// any data we need to pass to the thread needs to be member variables within the dialog
	CAssetManagerDlg *pThis = (CAssetManagerDlg *) pParam;
	return pThis->UpdateDuplicatesProcessThread();
}

// this is the member function that is called by the static function
// is has access to all the member variables of the dialog
UINT CAssetManagerDlg::UpdateDuplicatesProcessThread(void)
{
	// wait until init dialog is called in other thread
	while (!m_pSyncDialog->GetInitDialogCalled())
		Sleep(1);
	
	// the feedback object is used to pass information to the sync dialog
	// the other thread is sitting in the domodal loop ready to handle any ui messages for the sync dialog
	AMSyncFeedback feedback;
	feedback.SetDialog(m_pSyncDialog); // connect the feedback object to the feedback dialog
	
	SyncDBBase *pDatabase;
	if (m_bContextFromLeft == true)
	{
		pDatabase = m_pLeftDataBase;
	}
	else
	{
		pDatabase = m_pRightDataBase;
	}
	m_pUpdateDuplicatesManager->UpdateDuplicates(pDatabase,feedback);
	if (feedback.GetAbortFlag())
	{
		feedback.AddToLog(LogFlags::FONT_COLOR_DK_RED,"Canceled...");
	}
	m_pSyncDialog->SetThreadRunning(false);
	m_pSyncDialog->PostMessage(WM_KICKIDLE);
	return 0;
}

void CAssetManagerDlg::OnAbout() 
{
	CAboutDlg dlgAbout;
	theApp.m_bAccelEnable = false;
	dlgAbout.DoModal();
	theApp.m_bAccelEnable = true;
}

void CAssetManagerDlg::OnSelchangeSyncStyle() 
{
	if (m_pFolderSets)
	{
		int activeSet = m_pFolderSets->GetActiveSet();

		int newSyncStyle = m_syncStyleCombo.GetCurSel();
		int oldSyncStyle = m_pFolderSets->GetSyncStyle(activeSet);

		if (newSyncStyle!=oldSyncStyle)
		{
			m_pFolderSets->SetSyncStyle(activeSet,newSyncStyle);
			m_syncBitmapFrame.SetSyncStyle(newSyncStyle);
			Refresh();
		}
	}
}

void CAssetManagerDlg::OnHelpContents() 
{
	char helpPathBuffer[_MAX_PATH];

	char fdrive[_MAX_DRIVE];
	char fdir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char fext[_MAX_EXT];

#ifdef _DEBUG
	// grab from the release directory
	char *pEnvName = getenv("ASSET_MANAGER");
	if (!pEnvName)
		return;
	strcpy(helpPathBuffer,pEnvName);
	strcat(helpPathBuffer,"\\");
#else
	// grab with the executable (release)
	::GetModuleFileName(NULL,helpPathBuffer,_MAX_PATH);
#endif
	_splitpath(helpPathBuffer,fdrive,fdir,fname,fext);
	strcat(fdir,"Docs\\html\\");
	_makepath(helpPathBuffer,fdrive,fdir,"AssetManagerHelp","htm");
	
	ShellExecute(NULL, "open",helpPathBuffer, NULL, NULL, SW_SHOWNORMAL);
}
