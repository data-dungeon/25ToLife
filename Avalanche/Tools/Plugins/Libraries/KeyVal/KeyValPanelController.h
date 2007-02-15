//-----------------------------------------------------------------------------
//	File:		KeyValPanelController.h
//	Created:	April 29, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The KeyValPanelController class controlls all functionality
//			regarding the Key/Value panel dealing with objects, faces,
//			textures, etc.
//
//	History:	4/29/2002: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------


#ifndef __KEYVALPANELCONTROLLER__
#define __KEYVALPANELCONTROLLER__

using namespace AttrControls;


//-----------------------------------------------------------------------------
// Includes:



//-----------------------------------------------------------------------------
// Helper Structures and Classes:

#pragma pack(push, Start_KeyValIndices, 1)
struct KeyValIndices
{
	short m_nKeyIndex;
	short m_nValIndex;
};
#pragma pack(pop, Start_KeyValIndices)

//-----------------------------------------------------------------------------
// ValInfo Class Declaration:

class ValInfo
{
public:
	// Constructor:
	ValInfo() : 
		m_bChecked(false),
		m_bVisible(false),
		m_activeCount(0),
		m_defaultCount(0),
		m_unusedCount(0),
		m_hTreeItem(NULL)
	{
	}

	// Equals operator
	bool operator==(const ValInfo& srcValInfo) const
	{
		// Compare only the m_strVal (for use with the keeper)
		return m_strVal == srcValInfo.m_strVal;
	}

	// Public data members
	TupString m_strVal;
	TupString m_strNewVal;
	bool m_bVisible;
	bool m_bChecked;
	int m_activeCount;
	int m_defaultCount;
	int m_unusedCount;
	HTREEITEM m_hTreeItem;
};

inline unsigned int GetHashValue(const ValInfo& entry)
{
	int size = entry.m_strVal.GetLength();

	if (size)
	{
		return HashValue::GetHashValue(entry.m_strVal, size);
	}
	else
	{
		return -1;
	}
}


//-----------------------------------------------------------------------------
// KeyInfo Class Declaration:

class KeyInfo
{
public:
	// Constructor:
	KeyInfo() :
		m_bChecked(false),
		m_bExpanded(false),
		m_bVisible(false),
		m_activeCount(0),
		m_defaultCount(0),
		m_unusedCount(0),
		m_hTreeItem(NULL)
	{
	}

	// Equals operator
	bool operator==(const KeyInfo& srcKeyInfo) const
	{
		// Compare only the m_strKeyname (for use with the keeper)
		return m_strKeyname == srcKeyInfo.m_strKeyname;
	}

	// Public data members
	TupString m_strKeyname;
	TupString m_strNewKeyname;
	bool m_bVisible;
	bool m_bChecked;
	bool m_bExpanded;
	int m_activeCount;
	int m_defaultCount;
	int m_unusedCount;
	HTREEITEM m_hTreeItem;
	Keeper<ValInfo> m_valKeeper;
};

inline unsigned int GetHashValue(const KeyInfo& entry)
{
	int size = entry.m_strKeyname.GetLength();

	if (size)
	{
		return HashValue::GetHashValue(entry.m_strKeyname, size);
	}
	else
	{
		return -1;
	}
}

//-----------------------------------------------------------------------------
// KeyValueFind Class Declaration:

class KeyValueFind
{
public:
	// Constructor:
	KeyValueFind() :
		m_bUseKey(true),
		m_bUseValue(true),
		m_bMatchCase(true),
		m_bMatchWholeWord(true),
		m_bSearchDefaults(false),
		m_bSearchUnused(false)
	{
	}

	// Public data members
	TupString m_keyName;
	TupString m_valueName;
	bool m_bUseKey;
	bool m_bUseValue;
	bool m_bMatchCase;
	bool m_bMatchWholeWord;
	bool m_bSearchDefaults;
	bool m_bSearchUnused;
};


//-----------------------------------------------------------------------------
// Pre-declarations, etc.:

class KeyValCommunicatorBase;
class KeyValFindDialog;


//-----------------------------------------------------------------------------
// KeyValPanelController Class Declaration:

class KeyValPanelController
{
public:
	// Class enums:
	enum Mode
	{
		NONE,
		SELECT,
		APPLY,
		PURGE,
		REMAP,
	};
	enum MeansToEditLabel
	{
		EL_NONE,
		EL_KEYCHECK,
		EL_VALCHECK,
		EL_KEYLABEL,
		EL_VALLABEL,
	};

	// Constructors and Destructors:
	KeyValPanelController(KeyValCommunicatorBase* pKeyValCommBase);
	virtual ~KeyValPanelController();

	// Accessor and Mutators:
	void SetMode(Mode newMode, bool bRespectLastState = true);
	Mode GetMode();

	// Public methods:
	void SetHWnd(HWND hWnd);
	void OnInitDialog(void);
	BOOL OnNotify(LPARAM lParam);
	void OnSize(HWND hWnd,int cx,int cy);
	BOOL OnSetCursor(UINT nHitTest,UINT message);
		
	void InitKeyValPanel(ICustomRollupWindow* pCustomRollupWindow, HINSTANCE hInstance, LPARAM lParam);
	void IdCancelMessage();
	void SetApplyBttnStates(bool bApplySelEnabled, bool bApplySceneEnabled);
	void HandleCheckBoxClick(HTREEITEM hItemClicked);
	void HandleItemExpanded(LPARAM lParam);
	void HandleBeginLabelEdit(LPNMTVDISPINFO pTVDispInfo);
	void HandleEndLabelEdit(LPNMTVDISPINFO pTVDispInfo);
	void HandleFindButton(void);
	void ShowFindDialog(const char *pKeyName, const char *pValueName);
	void HandleShowActiveButton(void);
	void HandleShowDefaultButton(void);
	void HandleShowUnusedButton(void);
	void HandleFilterButton(void);
	void HandleApplyButtons(bool bApplyToAll = false);
	int HandleFind(const KeyValueFind &keyValueFind, bool bCurrentSelection, bool bAppendSelection);
	void UpdateTreeViewAndKeeper();
	void UncheckAllKeysAndVals();
	char* GetRegKeyName();

	static void SetRenameStartCallback(void (CALLBACK *renameStartCallback)(void));
	static void SetRenameEndCallback(void (CALLBACK *renameEndCallback)(void));
	

private:
	// private methods:
	void HandleKeyCheckSelect(short nKeyIndex, UINT nCheckState);
	void HandleKeyCheckPurge(short nKeyIndex, UINT nCheckState);
	void HandleKeyCheckRemap(short nKeyIndex, UINT nCheckState);
	void HandleValCheckSelect(short nKeyIndex, short nValIndex, UINT nCheckState);
	void HandleValCheckPurge(short nKeyIndex, short nValIndex, UINT nCheckState);
	void HandleValCheckRemap(short nKeyIndex, short nValIndex, UINT nCheckState);
	void HandleValCheckApply(short nKeyIndex, short nValIndex, UINT nCheckState);
	void HandleBeginLabelEditKey(LPNMTVDISPINFO pTVDispInfo);
	void HandleBeginLabelEditVal(LPNMTVDISPINFO pTVDispInfo);
	void HandleEndLabelEditKeyCheck(LPNMTVDISPINFO pTVDispInfo);
	void HandleEndLabelEditValCheck(LPNMTVDISPINFO pTVDispInfo);
	void MarkVisibleEntries();
	void MergeDataIntoMainKeeper();
	void UpdateKeeperBasedOnMode();
	void BuildTreeView();
	void AdjustTreeViewScroll();
	void UpdateTreeViewStyles();
	void ReplaceWithNewNamesInTV();

	// private data members:
	KeyValCommunicatorBase* m_pKeyValCommunicatorBase;
	Mode m_mode;
	HWND m_hwndPanel;
	Keeper<KeyInfo> m_keyKeeper;
	Keeper<KeyInfo> m_previousKeyKeeper;
	int m_previousVisibleKeyIndex;
	int m_previousVisibleValueIndex;

	bool m_bShowActive;
	bool m_bShowDefault;
	bool m_bShowUnused;

	Keeper<KeyInfo> m_filterKeyKeeper;

	MeansToEditLabel m_meansToEditLabel;
	HWND m_hwndTreeCtrl;

	HWND m_hWnd;
	HWND m_hSelectButton;
	HWND m_hApplyButton;
	HWND m_hPurgeButton;
	HWND m_hRemapButton;
	HWND m_hClearButton;
	HWND m_hShowActiveButton;
	HWND m_hShowDefaultButton;
	HWND m_hShowUnusedButton;
	HWND m_hFilterButton;
	HWND m_hApplySel;
	HWND m_hApplyScene;
	
	HWND m_hToolTip;
	
	HICON m_hIconSelect;
	HICON m_hIconApply;
	HICON m_hIconPurge;
	HICON m_hIconRemap;
	HICON m_hIconClear;
	HICON m_hIconShowActive;
	HICON m_hIconShowDefault;
	HICON m_hIconShowUnused;
	HICON m_hIconFilter;

	KeyValFindDialog *m_pFindDialog;
	
	HINSTANCE m_hInstance;

	static void (CALLBACK *m_pRenameStartCallback)(void);
	static void (CALLBACK *m_pRenameEndCallback)(void);
	

private:
};


extern KeyValPanelController* g_pTheKeyValPanelController;

//-----------------------------------------------------------------------------


#endif
