// TupViewerTreeCtrl.h : header file
//
#ifndef TUP_VIEW_TREE_CTRL
#define TUP_VIEW_TREE_CTRL

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProfStudioTreeCtrl.h : header file
//

#define __IMG_CW_ROOT 0
#define __IMG_CW_MACROS_AND_CONSTANTS 1
#define __IMG_CW_CLASS 2
#define __IMG_CW_MAPS 3
#define __IMG_CW_ENUM 4
#define __IMG_CW_FUNC 5
#define __IMG_CW_FUNC_P 6
#define __IMG_CW_VAR 7
#define __IMG_CW_VAR_P 8
#define __IMG_FILE_FOLDER_CLOSE 9
#define __IMG_RES_ROOT 10
#define __IMG_RES_DIALOG 11
#define __IMG_RES_ICON 12
#define __IMG_RES_STRING_TABLE 13
#define __IMG_RES_VER_INFO 14
#define __IMG_SE_ROOT 15
#define __IMG_FILE_FOLDER_OPEN 16
#define __IMG_SE_FILE_CPP 17
#define __IMG_SE_FILE_HDR 18

#define __IMG_BOOK_CLOSE_0 19
#define __IMG_BOOK_OPEN_0 20
#define __IMG_BOOK_ARTICLE_0 21
#define __IMG_BOOK_OPEN_1 22
#define __IMG_BOOK_CLOSE_1 23
#define __IMG_BOOK_ARTICLE_1 24

#define __IMG_SRVEXP_DATA_CONNECTIONS 25
#define __IMG_SRVEXP_SERVERS_ROOT 26
#define __IMG_SRVEXP_SERVER_ITEM 27
#define __IMG_SRVEXP_CRYSTAL_SERVICES 28
#define __IMG_SRVEXP_CRYSTAL_ENTERPRISE 29
#define __IMG_SRVEXP_CRYSTAL_FILES 30
#define __IMG_SRVEXP_EVENT_LOGS 31
#define __IMG_SRVEXP_MESSAGE_QUEUES 32
#define __IMG_SRVEXP_PERFORMANCE_COUNTERS 33
#define __IMG_SRVEXP_SERVICES 34
#define __IMG_SRVEXP_SQL_SERVERS 35

#define __IMG_RED_CROSS 36

#define __IMG_EMPTY 37
#define __IMG_TREE_BOX_PLUS 38
#define __IMG_TREE_BOX_MINUS 39

#define __IMG_TOOLBOX_POINTER 40
#define __IMG_TOOLBOX_CIPBOARD_MARKUP_FRAGMENT 41
#define __IMG_TOOLBOX_BUTTON 42
#define __IMG_TOOLBOX_EDIT 43
#define __IMG_TOOLBOX_CHECK 44
#define __IMG_TOOLBOX_COMBO 45
#define __IMG_TOOLBOX_LISTBOX 46
#define __IMG_TOOLBOX_RADIO 47
#define __IMG_TOOLBOX_STATIC 48
#define __IMG_TOOLBOX_PICTURE 49
#define __IMG_TOOLBOX_VSB 50
#define __IMG_TOOLBOX_SLIDER 51
#define __IMG_TOOLBOX_HSB 52
#define __IMG_TOOLBOX_SPIN 53
#define __IMG_TOOLBOX_PROGRESS 54
#define __IMG_TOOLBOX_HOTKEY 55
#define __IMG_TOOLBOX_TREE 56
#define __IMG_TOOLBOX_TAB 57
#define __IMG_TOOLBOX_ANIM 58
#define __IMG_TOOLBOX_RICHEDIT20 59
#define __IMG_TOOLBOX_DTPICKER 60
#define __IMG_TOOLBOX_MONTHCAL 61
#define __IMG_TOOLBOX_IPADDR 62
#define __IMG_TOOLBOX_EXCOMBO 63
#define __IMG_TOOLBOX_CUSTOMCTRL 64
#define __IMG_TOOLBOX_GROUP 65
#define __IMG_TOOLBOX_LISTCTRL 66

#define __IMG_TASKLIST_EXCLAMATION_POINT 67
#define __IMG_TASKLIST_CHECKMARK_1 68
#define __IMG_TASKLIST_CHECKMARK_0 69
#define __IMG_TASKLIST_MARK 70

#define __IMG_FSR_GREEN_BLOCK 71
#define __IMG_FSR_EXCLAMATION_POINT 72

#define __IMG_ME_ROOT 73
#define __IMG_ME_FOLDER 74
#define __IMG_ME_MODULE 75
#define __IMG_ME_FUNCTION 76

#define __IMG_FAVORITES_FOLDER_CLOSE 77
#define __IMG_FAVORITES_FOLDER_OPEN 78
#define __IMG_FAVORITES_PAGE 79

#define __IMG_DEBUG_CURR_LINE 80
#define __IMG_DEBUG_BRAKABLE 81
#define __IMG_DEBUG_CURR_BRAKABLE 82
#define __IMG_DEBUG_MODULE 83

/////////////////////////////////////////////////////////////////////////////
// TupViewerTreeCtrl window

class TupViewerTreeCtrl : public CTreeCtrl
{
	CImageList m_ImageList;

	void _InsertMacroExplorerModule(
		HTREEITEM htiFolder,
		LPCTSTR sModuleName,
		INT nFuncCount = 5
		);
	// Construction
public:
	TupViewerTreeCtrl();

	// Attributes
public:

	// Operations
public:
	HTREEITEM InsertItem(
		LPCTSTR lpszItem,
		int nImage,
		int nExpandedImage,
		HTREEITEM hParent = TVI_ROOT,
		HTREEITEM hInsertAfter = TVI_LAST
		);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TupViewerTreeCtrl)
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~TupViewerTreeCtrl();

	void Refresh(void);
	void UpdateTreeItem(HTREEITEM hItem);

protected:
	void AddTupData(TupperwareAtom *pAtom,HTREEITEM hParent);

	// Generated message map functions
	//{{AFX_MSG(TupViewerTreeCtrl)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROFSTUDIOTREECTRL_H__8804C1F7_CD90_45A4_87F4_82DBB63EA81D__INCLUDED_)
