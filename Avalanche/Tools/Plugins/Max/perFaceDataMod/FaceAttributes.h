#ifndef __FACEATTRIBUTES_H__
#define __FACEATTRIBUTES_H__

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
#define INSIDE_FRAME_SPACE_ROLLUP	2
#define SPACE_ABOVE_ROLLUP			31

//------------------------------------------------------------------------
// forward declaration of the object attribute controller class
class FaceAttrController;
namespace AttrControls
{
	class ICustomRollupWindow;
};
using namespace AttrControls;

extern TCHAR *GetString(int id);

class ConfigFilesManager;

class CPerFaceDataMod;

#define WM_SELCHANGE (WM_USER + 200)

//------------------------------------------------------------------------
class FaceAttributes
{
public:
	// public member variables
	bool	bDialogUp;
	HWND	hDlg;

	CPerFaceDataMod *m_pCurrentModifier;

	HWND m_hOneToManyButton;
	HWND m_hReplaceButton;
	HWND m_hCopyButton;

	HWND m_hToolTip;
	HWND m_hAttributesToolTip;

	HICON m_hIconOneToMany;
	HICON m_hIconOneToManyDisabled;
	HICON m_hIconReplace;
	HICON m_hIconReplaceDisabled;
	HICON m_hIconCopy;
	HICON m_hIconCopyDisabled;

	HWND m_hBrowseButton;
	HICON m_hIconBrowse;

	HWND m_hConfigList;

	// public member functions
	FaceAttributes();
	~FaceAttributes();

	void SetupAttributes();
	void CloseAttributes();
	void RefreshAttributes();

	void InitDialog();
	void EndDialog();	
	bool AdjustPanelHeights(bool bUseDefaultSize = false);

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

	void InitKeyValPanel(ICustomRollupWindow* pCustomRollupWindow, HINSTANCE hInstance, LPARAM lParam);
	void IdCancelMessage();
	void SetApplyBttnStates(bool bApplySelEnabled, bool bApplySceneEnabled);
	void LoadKeyValPanelTree();

	ConfigFilesManager &GetConfigFilesManager(void);

	void ChangeConfigFile(int selectionIndex);
	void RefreshConfig(void);

protected:
	// protected member variables
	FaceAttrController *m_pController;

	ConfigFilesManager *m_pConfigFilesManager;

	//protected member functions
};

extern FaceAttributes theFaceAttributes;

#endif	// __FACEATTRIBUTES_H__