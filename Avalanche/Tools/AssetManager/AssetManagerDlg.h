//-----------------------------------------------------------------------------
//	File:		AssetManagerDlg.h
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

#ifndef __ASSETMANAGERDLG__
#define __ASSETMANAGERDLG__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __PUSH_BUTTON__
#include "PushButton.h"
#endif

#ifndef __SYNCSTYLESTATIC__
#include "SyncStyleStatic.h"
#endif

//-----------------------------------------------------------------------------
// Callbacks:



//-----------------------------------------------------------------------------
// Pre-declarations:

class CAssetManagerTreeCtrl;
class AMSyncTreeFactory;
class LoadDBDialog;
class SyncDialog;
class AssociatedIconManager;
class FolderSets;
class AMSyncTreeNodeFolder;
class AMSyncTreeNodeFile;
class CPPToolTip;
//-----------------------------------------------------------------------------
// CAssetManagerDlg class declaration:

class CAssetManagerDlg : public CDialog
{
public:
	// Class enums:
	enum Mode
	{
		PRECOMPARE,
		POSTCOMPARE,
	};

	class MouseOverData
	{
	public:
		enum Type
		{
			OVER_NONE,
			OVER_FOLDER,
			OVER_FILE,
		};

		enum Control
		{
			LEFT_TREE,
			RIGHT_TREE,
			INDICATOR_TREE,
		};

		Type m_type;
		Control m_control;
		AMSyncTreeNodeFolder *m_pSyncNodeFolder;
		AMSyncTreeNodeFile *m_pSyncNodeFile;

		MouseOverData(void) : m_type(OVER_NONE) {}
		bool operator==(const MouseOverData &s) 
		{
			return (m_type == s.m_type && m_control == s.m_control && m_pSyncNodeFolder==s.m_pSyncNodeFolder && m_pSyncNodeFile==s.m_pSyncNodeFile);
		}
		bool operator!=(const MouseOverData &s) 
		{
			return (m_type != s.m_type || m_control != s.m_control || m_pSyncNodeFolder!=s.m_pSyncNodeFolder || m_pSyncNodeFile!=s.m_pSyncNodeFile);
		}
	};

	// Constructors and destructors:
	CAssetManagerDlg(CWnd* pParent = NULL);
	~CAssetManagerDlg();

	// Accessors & Mutators:
	void SetTempRootDir(CString strNewRoot);
	CString GetTempRootDir();
	void SetMode(Mode newMode);
	Mode GetMode();

	// Public methods:
	void ExpandItem(HTREEITEM hItem, SEC_DWORD sDWItemData, UINT nCode);
	void ShowContextMenu(CPoint point, int nRole);
	static int CALLBACK FolderPickCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM dwData);
	void TreeMouseMove(UINT nFlags, CPoint point);

	void SetFolderSetToLoad(const char *pFolderSetName);
	void SetSyncSyleToUse(int syncStyle);
	void SetAutoPressButtons(bool bCopyToRight,
									 bool bCopyToLeft,
									 bool bDeleteRight,
									 bool bDeleteLeft,
									 bool bSync);

	// Dialog Data
	//{{AFX_DATA(CAssetManagerDlg)
	enum { IDD = IDD_ASSETMANAGER_DIALOG };
	SyncStyleStatic	m_syncBitmapFrame;
	CStatic	m_syncStyleTitle;
	CComboBox	m_syncStyleCombo;
	CButton	m_bttnFolderSetsBrowse;
	CPushButton m_bttnFolderSetsSort;
	CStatic	m_folderSetsTitle;
	CComboBox	m_folderSetsCombo;
	CStatic	m_folderPathRight;
	CStatic	m_folderPathLeft;
	CStatic	m_lockFrozenBitmap;
	CButton	m_lockFrozenCheckbox;
	CStatic	m_lockExcludedBitmap;
	CButton	m_lockExcludedCheckbox;
	CPushButton	m_bttnViewFrozen;
	CPushButton	m_rlCopyButton;
	CPushButton	m_lDeleteButton;
	CPushButton	m_rDeleteButton;
	CPushButton	m_lrCopyButton;
	CButton m_syncButton;
	CPushButton	m_bttnRefresh;
	CPushButton	m_bttnViewAll;
	CPushButton	m_bttnViewCpyLeft;
	CPushButton	m_bttnViewCpyRight;
	CPushButton	m_bttnViewDltBoth;
	CPushButton	m_bttnViewDltLeft;
	CPushButton	m_bttnViewDltRight;
	CPushButton	m_bttnViewEqual;
	CPushButton	m_bttnViewNewerLeft;
	CPushButton	m_bttnViewNewerRight;
	CPushButton	m_bttnViewOrphanLeft;
	CPushButton	m_bttnViewOrphanRight;
	CPushButton	m_bttnViewExcluded;
	CStatic	m_rightStatsText4;
	CStatic	m_rightStatsText3;
	CStatic	m_rightStatsText2;
	CStatic	m_rightStatsText1;
	CStatic	m_leftStatsText4;
	CStatic	m_leftStatsText3;
	CStatic	m_leftStatsText2;
	CStatic	m_leftStatsText1;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAssetManagerDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

protected:
	// Generated message map functions
	//{{AFX_MSG(CAssetManagerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	afx_msg void OnOK();
	afx_msg void OnCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDefaultOptions();
	afx_msg void OnFolderSetOptions();
	afx_msg void OnContextCopyToLeft();
	afx_msg void OnContextCopyToRight();
	afx_msg void OnContextDeleteFromLeft();
	afx_msg void OnContextDeleteFromRight();
	afx_msg void OnContextDeleteFromBoth();
	afx_msg void OnContextExclude();
	afx_msg void OnContextSetDefault();
	afx_msg void OnContextSelectAll();
	afx_msg void OnContextSelectNone();
	afx_msg void OnContextInvertSelection();
	afx_msg void OnContextFreeze();
	afx_msg void OnContextUnFreeze();
	afx_msg void OnContextUnFreezeOnly();
	afx_msg void OnContextExpandAll();
	afx_msg void OnContextCollapseAll();
	afx_msg void OnContextAddPrefix();
	afx_msg void OnContextRemovePrefix();
	afx_msg void OnContextRename();
	afx_msg void OnContextUpdateDuplicates();
	afx_msg void OnContextExplore();
	afx_msg void OnSyncbutton();
	afx_msg void OnLDelete();
	afx_msg void OnLrCopyButton();
	afx_msg void OnRDelete();
	afx_msg void OnRlCopyButton();
	afx_msg void OnBttnViewCpyLeft();
	afx_msg void OnFolderSetsSort();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO *lpMMI);
	afx_msg void OnBttnRefresh();
	afx_msg void OnBttnViewAll();
	afx_msg void OnBttnViewCpyRight();
	afx_msg void OnBttnViewDltBoth();
	afx_msg void OnBttnViewDltLeft();
	afx_msg void OnBttnViewDltRight();
	afx_msg void OnBttnViewEqual();
	afx_msg void OnBttnViewNewerLeft();
	afx_msg void OnBttnViewNewerRight();
	afx_msg void OnBttnViewOrphanLeft();
	afx_msg void OnBttnViewOrphanRight();
	afx_msg void OnBttnViewExcluded();
	afx_msg void OnBttnViewFrozen();
	afx_msg void OnLockExcluded();
	afx_msg void OnLockFrozen();
	afx_msg void OnFolderSetsBrowse();
	afx_msg void OnSelchangeFolderSets();
	afx_msg void OnAbout();
	afx_msg void OnContextAutoFreezeAdd();
	afx_msg void OnContextAutoFreezeRemove();
	afx_msg void OnContextAutoFreezeRemoveIncludeChildren();
	afx_msg void OnUpdateSetDefault(CCmdUI* pCmdUI);
	afx_msg void OnUpdateExclude(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCopyToLeft(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCopyToRight(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFreeze(CCmdUI* pCmdUI);
	afx_msg void OnUpdateUnfreeze(CCmdUI* pCmdUI);
	afx_msg void OnUpdateUnfreezeOnly(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAutoFreezeAdd(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDeleteFromLeft(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDeleteFromRight(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDeleteFromBoth(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSelectNone(CCmdUI* pCmdUI);
	afx_msg void OnUpdateInvertSelection(CCmdUI* pCmdUI);
	afx_msg void OnUpdateExpandAll(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCollapseAll(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAddPrefix(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRemovePrefix(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRename(CCmdUI* pCmdUI);
	afx_msg void OnUpdateUpdateDuplicates(CCmdUI* pCmdUI);
	afx_msg void OnUpdateExplore(CCmdUI* pCmdUI);
	afx_msg void OnSelchangeSyncStyle();
	afx_msg void OnHelpContents();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Protected methods:
	void MakeRectOnScreen(CRect &windowRect,const CRect &offsetRect);
	void SetViewFlags(unsigned long flags); 
	CString ShowFolderPickDlg(UINT nCaptionStringID, CString strRootDir);
	void RebuildFolderSetsCombo();
	bool SetUpFolderSets();
	void SetUpTreeControls();
	void LoadTreeControl(CAssetManagerTreeCtrl* pTreeCtrlLeft, CAssetManagerTreeCtrl* pTreeCtrlRight,
								CAssetManagerTreeCtrl* pTreeCtrlIndicators, TreeNodeBase* pTreeNodeBase);
	void LoadTreeItem(CAssetManagerTreeCtrl* pTreeCtrlLeft, CAssetManagerTreeCtrl* pTreeCtrlRight,
							  CAssetManagerTreeCtrl* pTreeCtrlIndicators, TreeNodeBase* pTreeNodeBase,
							  HTREEITEM hItemLeftParent, HTREEITEM hItemRightParent, HTREEITEM hItemIndicatorParent,
							  bool &bFoundSelected);
	void SetTreeItem(TreeNodeBase* pTreeNodeBase,bool bDoChildren);
	void ExpandAllTreeItems(CAssetManagerTreeCtrl* pTreeToExpand);
	void ChangeAction(SyncTreeManager::Actions newAction,bool bUpdateTreeItems=true);
	void RemoveStrFromCombo(CComboBox& comboBox, CString strToRemove);
	void LoadDatabases(void);
	static UINT LoadDatabaseProc(LPVOID pParam);
	UINT LoadDatabaseProcessThread(void);
	const char *GetClientDataFilePath(int setIndex);
	void CAssetManagerDlg::SaveClientData(void);
	void SyncDatabases(void);
	static UINT SyncDatabasesProc(LPVOID pParam);
	UINT SyncDatabasesProcessThread(void);
	void UpdateSyncStats(void);
	void UpdateViewButtons(void);
	void UpdateTreeItems(void);
	void SaveFolderExpandState(void);
	void SaveFolderExpandState(TreeNodeBase *pNode);
	void ClearHtreeItems(TreeNodeBase *pNode,bool doChildren);
	void SaveTreeSelectedState(void);
	void SaveTreeSelectedState(TreeNodeBase *pNode,bool bGetFromTree,bool bLeftTree);
	unsigned long GetValidActions(void);
	void SetupToolTips(void);
	bool DirectoryExists(const char *pPath);
	bool CreateDirs (const char *pPath, bool bIsFilePath = false);
	CString GetIndicatorFileText(int flags);
	CString GetIndicatorFolderText(SyncTreeNodeFolder *pFolderNode);
	CString GetFormattedFileText(TreeNodeFile *pFileNode);
	CString GetFormattedFileTextSync(TreeNodeFile *pFileNode,FILETIME syncDate);
	CString GetFormattedFolderText(TreeNodeFolder *pFolderNode);
	CString GetMouseOverTipText(void);

	static UINT UpdateDuplicatesProc(LPVOID pParam);
	UINT UpdateDuplicatesProcessThread(void);

	bool CheckAction(int checkFlags);

	void Refresh(void);

	void DrawSyncStyleBitmapWindow(CPaintDC &dc,CRect myRect);

	// Protected data members:
	CAssetManagerTreeCtrl* m_pTreeCtrlLeft;				// Left tree control
	CAssetManagerTreeCtrl* m_pTreeCtrlRight;				// Right tree control
	CAssetManagerTreeCtrl* m_pTreeCtrlIndicators;		// Indicator tree control
	AssociatedIconManager* m_pAssociatedIconManager;	// Icon manager for tree controls
	UpdateDuplicatesManager * m_pUpdateDuplicatesManager; // manager for updating duplicate files
	CImageList m_ilTreeCtrls;									// Image list for the left and right tree controls
	CImageList m_ilTreeIndicator;								// Image list for the indicator tree control
	HICON m_hIcon;													// App icon
	CString m_strTempRootDir;									// Temporary root directory to use for the combo boxes
	Mode m_mode;													// Current mode for the application
	unsigned long m_viewFlags;									// flags for viewing parts of the tree
	int m_focusIndex;												// which item should have the focus
	bool m_bRefreshDatabases;
	bool m_bSortFolderSets;										// of the folder set combo box is sorted alphabetically

	// tool tips
	CPPToolTip *m_pToolTip;										// control for tooltips
	CPPToolTip *m_pDataTip;										// control for tooltips
	MouseOverData m_mouseOverData;							// data holding data tooltip info

	bool m_bContextFromLeft;									// Used for the context menu message
	SyncDBFactory* m_pDBFactory;								// The database factory
	AMSyncTreeFactory* m_pAMSyncTreeFactory;				// The sync tree factory
	SyncTreeManager* m_pSyncTreeManager;					// The sync tree manager
	SyncDBBase* m_pLeftDataBase;								// The database for the left tree
	SyncDBBase* m_pRightDataBase;								// The database for the right tree
	bool m_bClosing;												// If we are in the middle of closing the app
																		
	LoadDBDialog* m_pLoadDBDialog;							// the load database dialog
	CWinThread *m_pFeedbackThread;							// the thread we are waiting for on loading or synching
	SyncDialog* m_pSyncDialog;									// the sync database dialog
	SyncTreeManager::TreeStats m_syncStats;				// information concerning sync
	SyncTreeManager::SyncEnable m_syncEnable;				// what is enabled with the sync
	// bitmap handles for buttons
	HBITMAP m_hSync,m_hSyncDisabled;
	HBITMAP m_hCopyToLeft,m_hCopyToLeftDisabled;
	HBITMAP m_hCopyToRight,m_hCopyToRightDisabled;
	HBITMAP m_hDeleteLeft,m_hDeleteLeftDisabled;
	HBITMAP m_hDeleteRight,m_hDeleteRightDisabled;
	HBITMAP m_hBmapRefresh;
	HBITMAP m_hBmapViewAll;
	HBITMAP m_hBmapViewCpyLeft;
	HBITMAP m_hBmapViewCpyRight;
	HBITMAP m_hBmapViewDltBoth;
	HBITMAP m_hBmapViewDltLeft;
	HBITMAP m_hBmapViewDltRight;
	HBITMAP m_hBmapViewEqual;
	HBITMAP m_hBmapViewNewerLeft;
	HBITMAP m_hBmapViewNewerRight;
	HBITMAP m_hBmapViewOrphanLeft;
	HBITMAP m_hBmapViewOrphanRight;
	HBITMAP m_hBmapViewExcluded,m_hBmapViewExcludedDisabled;
	HBITMAP m_hBmapViewFrozen,m_hBmapViewFrozenDisabled;
	HBITMAP m_hBmapLock;
	HICON m_hIconBrowse;
	HICON m_hIconSort;

	CRect m_dialogRect;											// to keep track of dialog size and position

	AutoExclude m_autoExclude;

	BOOL m_updateDuplicatesIncludeFrozenDuplicates;
	BOOL m_updateDuplicatesIncludeParentNames;

	TupString m_folderSetToLoad;
	int m_overrideSyncStyle;
	bool m_bAutoPressCopyToRight;
	bool m_bAutoPressCopyToLeft;
	bool m_bAutoPressDeleteRight;
	bool m_bAutoPressDeleteLeft;
	bool m_bAutoPressSync;

	FolderSets *m_pFolderSets;

	TupString m_defaultAutoFreezeFileExtensions;
	TupString m_defaultAutoFreezeFilePrefixes;
	TupString m_defaultAutoFreezeFolderPrefixes;

	TupString m_userName;
	TupString m_computerName;

	LogFile *m_pLogFile;
};

//-----------------------------------------------------------------------------


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
