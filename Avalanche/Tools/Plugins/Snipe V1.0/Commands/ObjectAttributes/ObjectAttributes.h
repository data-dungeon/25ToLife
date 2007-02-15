#ifndef __OBJECTATTRIBUTES_H__
#define __OBJECTATTRIBUTES_H__

//------------------------------------------------------------------------
// Defines, etc.

// Control position defines:
#define START_X			10
#define START_Y			10
#define LABEL_WD		110
#define LABEL_HT		15
#define SPACE_BET_CTRLS	8

// Min/max window size defines:
#define MIN_DLG_WIDTH				165
#define MIN_DLG_HEIGHT				190

// Window spacing/position defines:
#define ROLLUP_LEFT	2
#define ROLLUP_RIGHT	2
#define ROLLUP_TOP	31
#define ROLLUP_BOTTOM	20
#define STATS_LEFT 2
#define STATS_RIGHT 2
#define STATS_TOP 17
#define STATS_BOTTOM 4

//------------------------------------------------------------------------
// forward declaration of the object attribute controller class
class ObjAttrController;
namespace AttrControls
{
	class ICustomRollupWindow;
};
using namespace AttrControls;

extern TCHAR *GetString(int id);

class ConfigFilesManager;

#define WM_SELCHANGE (WM_USER + 200)

//------------------------------------------------------------------------
class ObjectAttributes
{
public:
	// public member variables
	bool	bDialogUp;
	bool	bRefreshWindowEnable;
	HWND	hDlg;

	HWND m_hOneToManyButton;
	HWND m_hReplaceButton;
	
	HWND m_hToolTip;
	HWND m_hAttributesToolTip;

	HWND m_hStatusStaticText;
	
	HICON m_hIconOneToMany;
	HICON m_hIconOneToManyDisabled;
	HICON m_hIconReplace;
	HICON m_hIconReplaceDisabled;

	HWND m_hCopyButton;
	HICON m_hIconCopy;
	HICON m_hIconCopyDisabled;

	HWND m_hBrowseButton;
	HICON m_hIconBrowse;

	HWND m_hPurgeUnusedButton;
	HICON m_hIconPurgeUnused;

	HWND m_hFindButton;
	HICON m_hIconFind;

	HWND m_hConfigList;

	int m_oldScrollPos;

//	MCallbackId m_updateActiveListCallbackID;
//	MCallbackId m_beforeOpenCallbackID;

	// public member functions
	ObjectAttributes	();
	~ObjectAttributes	();

	bool ExecuteCommand(const CommandArgs &commandArgs, TupArray<TupString>& resultStrings, UndoRedoState &undoRedoState);
	void RefreshWindow();
	void RefreshAttributes();
	void ShowAttributeDialog();
	void InitDialog			();
	void EndDialog			();	
	bool AdjustPanelHeights	(bool bUseDefaultSize = false);

	bool SetUpUIDisplay(HWND hParentDlg, HINSTANCE hInstance,
						int nStartx, int nStarty, int nLabelWidth, int nLabelHeight,
						int nSpaceBetweenCtrls, WNDPROC newColorSwatchProc,
						bool bAdjustPanelHeights = true);
	bool DrawUIDisplayLines(HDC hdc);
	bool HandleWinMessage(HWND hParent, UINT message, WPARAM wParam, LPARAM lParam);
	bool LoadCurrAttribData();
	void DestroyCurrAttribData();
	void DestroyUIDisplay();
	void SaveNonexclusiveKeys();

	bool OkToCopy(void) const;
	bool OkToPaste(void) const;
	bool PasteAttribsToSelSet(bool bReplace = false);
	bool CopySelection(void);
	void UpdateCopyPasteButtons(void);
	void PurgeUnusedKeys(void);
	void HandleFindButton(void);
	
	void InitKeyValPanel(ICustomRollupWindow* pCustomRollupWindow, HINSTANCE hInstance, LPARAM lParam);
	void IdCancelMessage();
	void SetApplyBttnStates(bool bApplySelEnabled, bool bApplySceneEnabled);
	void LoadKeyValPanelTree();

	ConfigFilesManager &GetConfigFilesManager(void);

	void ChangeConfigFile(int selectionIndex);
	void RefreshConfig(void);

	int NumSelectedNodes(void) const;

	void NotifySelSetChanged(void);
	
protected:
	// protected member variables
	ObjAttrController *m_pController;

	ConfigFilesManager *m_pConfigFilesManager;

	//protected member functions
};

extern ObjectAttributes theObjectAttributes;

#endif	// __OBJECTATTRIBUTES_H__
