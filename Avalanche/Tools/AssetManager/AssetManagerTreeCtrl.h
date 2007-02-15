//-----------------------------------------------------------------------------
//	File:		AssetManagerTreeCtrl.h
//	Created:	Oct 7, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The CAssetManagerTreeCtrl class derives from SECTreeCtrl and adds
//			some extra data and methods needed to operate in the Asset Manager.
//
//	History:	10/7/2002: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------

#ifndef __ASSETMANAGERTREECTRL__
#define __ASSETMANAGERTREECTRL__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



//-----------------------------------------------------------------------------
// Predeclarations:

class CAssetManagerDlg;
class UpdateDuplicatesDialog;
class AssociatedIconManager;

#define TVIS_EX_TO_DELETE			0x01000
#define TVIS_EX_COPY_SOURCE			0x02000
#define TVIS_EX_COPY_DEST			0x04000
#define TVIS_EX_EXCLUDED			0x08000
#define TVIS_EX_FROZEN				0x10000
#define TVIS_EX_ASSOCIATED_ICON		0x20000

#define LVIS_EX_TO_DELETE				0x010000
#define LVIS_EX_COPY_SOURCE				0x020000
#define LVIS_EX_COPY_DEST				0x040000
#define LVIS_EX_EXCLUDED				0x080000
#define LVIS_EX_FROZEN					0x100000
#define LVIS_EX_ASSOCIATED_ICON			0x200000

//-----------------------------------------------------------------------------
// CAssetManagerTreeCtrl class declaration:

class CAssetManagerTreeCtrl : public SECTreeCtrl
{
public:
	// Class enums:
	enum Role
	{
		LEFT_TREE,
		RIGHT_TREE,
		INDICATOR_TREE,
	};

	// Constructors and destructors:
	CAssetManagerTreeCtrl(CDialog *pParent = NULL);
	virtual ~CAssetManagerTreeCtrl();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAssetManagerTreeCtrl)
	//}}AFX_VIRTUAL

	// Accessors & mutators:
	void SetTellParentOfExpand(bool bTellParentOfExpand);
	void SetRole(Role newRole);
	void SetDeleteTextBkColor(COLORREF newColor);
	void SetCopySourceTextBkColor(COLORREF newColor);
	void SetCopyDestTextBkColor(COLORREF newColor);
	void SetFrozenTextColor(COLORREF newColor);
	void SetAssociatedIconManager(AssociatedIconManager *pAssociatedIconManager);
	void SetAllowPaint(bool bAllowPaint);

	// Public methods:
	BOOL SetItemEx(TV_ITEM* pTVI, TV_ITEM_EX* pTVIX);
	BOOL GetItemEx(TV_ITEM* pTVI, TV_ITEM_EX* pTVIX, BOOL bGetDispInfoEx = FALSE) const;
	BOOL GetItemEx(LV_ITEM* pLVI, LV_ITEM_EX* pLVIX, BOOL bGetDispInfoEx = FALSE) const;
	BOOL LoadPaintContext( LvPaintContext* pPC );
	BOOL SelectItemNew(HTREEITEM hti,bool bDeselectAll,bool bSetFocus);
	BOOL IsFocus(HTREEITEM hti);
	void SetTopIndex(int index);
	void SelectAll(void);
	void DeselectAll(void);
	void InvertSelection(void);
	void SetNodifyExpanded(bool bNodifyExpanded);
	void SetOtherFileTree(CAssetManagerTreeCtrl *pOtherFileTree);
	void SetIndicatorTree(CAssetManagerTreeCtrl *pIndicatorTree);
	void ScrollVert(int nScrollItems, BOOL bFromBottom = FALSE);
	BOOL ProcessSelectChange(int nHitIndex,UINT iHitFlags,UINT nFlags,SECListCtrl::SelectAction sa);
	void MeasureSubItem(LvPaintContext* pPC, UINT& cySubItem);
		
protected:
	// Generated message map functions
	//{{AFX_MSG(CAssetManagerTreeCtrl)
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG

	// Protected methods:
    void DrawSubItem(LvPaintContext* pPC);
	void DrawHierarchyLines(LvPaintContext* pPC);
	void NotifyItemExpanded(HTREEITEM hti, UINT nCode) const;

	// Protected data members:
	CAssetManagerTreeCtrl *m_pOtherFileTree;
	CAssetManagerTreeCtrl *m_pIndicatorTree;

	CDialog* m_pParent;
	bool m_bTellParentOfVScroll;
	mutable bool m_bTellParentOfExpand;
	bool m_bTellParentOfSel;
	Role m_role;
	COLORREF m_rgbDeleteTextBk;
	COLORREF m_rgbCopySourceTextBk;
	COLORREF m_rgbCopyDestTextBk;
	COLORREF m_rgbFrozenText;

	bool m_bNodifyExpanded;
	bool m_bAllowPaint;

	AssociatedIconManager *m_pAssociatedIconManager;

	DECLARE_MESSAGE_MAP()
};

//-----------------------------------------------------------------------------


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
