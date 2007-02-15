//-----------------------------------------------------------------------------
//	File:		FaceAttrController.h
//	Created:	Nov. 14, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The FaceAttrController class acts as the commander so-to-speak for
//			the Object Attribute plugin.  This class, plus its data member
//			classes, handles most of the functionality except for direct user
//			interface and a few other assundry things.
//
//	History:	11/14/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------


#ifndef __FACEATTRCONTROLLER__
#define __FACEATTRCONTROLLER__



//-----------------------------------------------------------------------------
// Includes:



//-----------------------------------------------------------------------------
// Pre-declarations, etc.:

class ConfigRoot;
class CurrAttribController;
class CurrAttribData;
class UIController;
class ExclusiveController;
class KeyValCommunicator;
class ApplyColorsCommunicator;
class Interface;
class KeyInfo;
struct KeyValuePair;
class ConfigFilesManager;

//-----------------------------------------------------------------------------
// Class Declaration:
class FaceAttrController
{
public:
	// Constructors and destructors:
	FaceAttrController();
	~FaceAttrController();

	// Accessors:
	CurrAttribController* GetCurrAttribController();
	UIController* GetUIController();

	// Public methods:
	bool LoadCurrAttribData();
	bool LoadConfigFile(ConfigFilesManager &configFilesManager);
	bool SetUpUIDisplay(HWND hParentDlg, HINSTANCE hInstance, HWND hToolTip,
						int nStartx, int nStarty,
						int nLabelWidth, int nLabelHeight,
						int nSpaceBetweenCtrls, WNDPROC newColorSwatchProc);
	bool HandleWinMessage(HWND hParent, UINT message, WPARAM wParam, LPARAM lParam);
	void DestroyCurrAttribData();
	void DestroyUIDisplay();
	int GetTotalY();
	void SaveNonexclusiveKeys();

	void InitKeyValPanel(ICustomRollupWindow* pCustomRollupWindow, HINSTANCE hInstance, LPARAM lParam);
	void IdCancelMessage();
	void SetApplyBttnStates(bool bApplySelEnabled, bool bApplySceneEnabled);
	void LoadKeyValPanelTree();
	void HandleApplySelect(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll = false);
	void HandleApplyApply(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll = false);
	void HandleApplyPurge(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll = false);
	void HandleApplyRemap(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll = false);

	void InitApplyColorsPanel(ICustomRollupWindow* pCustomRollupWindow, HINSTANCE hInstance, LPARAM lParam);

	bool OkToCopy(void) const;
	bool OkToPaste(void) const;
	bool PasteAttribsToSelSet(bool bReplace = false);
	bool CopySelection(void);
	
protected:
	// Protected methods:
	static bool StripAllButRawData(TupString &strToFix);
	static void AddRawDataToString(TupString &strToAddTo,const TupString &strToInsert);

	void BuildObjIndices(TupString strKey, TupString strVal,
		TupArray<short>& vectObjIndices);
	
	bool LoadCurrAttribDataScene();
	void LoadCurrAttribDataNode(MDagPath mDagPath);
	void LoadCurrAttribDataNode();

	bool ObjectMatchesCheckedItems(const CurrAttribData &currAttribData, Keeper<KeyInfo>& checkedKeyKeeper);

	void SetMayaData(int nIndex);
		
	// Protected data members:
	ConfigRoot* m_pConfigRoot;
	CurrAttribController* m_pCurrAttribController;
	UIController* m_pUIController;
	ExclusiveController* m_pExclusiveController;
	KeyValCommunicator* m_pKeyValComm;
	ApplyColorsCommunicator* m_pApplyColorsComm;
	TupArray<KeyValuePair> *m_pCopiedKeyValPairs;
	
	HWND m_hParentDlg;
	HINSTANCE m_hInstance;
	HWND m_hToolTip;

private:
};
//-----------------------------------------------------------------------------


#endif
