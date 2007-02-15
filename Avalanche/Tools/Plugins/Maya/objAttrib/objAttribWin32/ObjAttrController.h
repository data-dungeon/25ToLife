//-----------------------------------------------------------------------------
//	File:		ObjAttrController.h
//	Created:	Nov. 14, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The ObjAttrController class acts as the commander so-to-speak for
//			the Object Attribute plugin.  This class, plus its data member
//			classes, handles most of the functionality except for direct user
//			interface and a few other assundry things.
//
//	History:	11/14/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------


#ifndef __OBJATTRCONTROLLER__
#define __OBJATTRCONTROLLER__



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
class Interface;
class KeyInfo;
class KeyValueFind;
struct KeyValuePair;
class ConfigFilesManager;

//-----------------------------------------------------------------------------
// Class Declaration:
class ObjAttrController
{
public:
	// Constructors and destructors:
	ObjAttrController();
	~ObjAttrController();

	// Accessors:
	ConfigRoot* GetConfigRoot();
	CurrAttribController* GetCurrAttribController();
	UIController* GetUIController();


	// Public methods:
	bool LoadConfigFile(ConfigFilesManager &configFilesManager);
	bool LoadCurrAttribData();
	bool LoadCurrAttribData(const MSelectionList &sList);
	bool SetUpUIDisplay(HWND hParentDlg, HINSTANCE hInstance, HWND hToolTip,
						int nStartx, int nStarty,
						int nLabelWidth, int nLabelHeight,
						int nSpaceBetweenCtrls, WNDPROC newColorSwatchProc);
	bool HandleWinMessage(HWND hParent, UINT message, WPARAM wParam, LPARAM lParam);
	void DestroyCurrAttribData();
	void DestroyUIDisplay();
	bool CopyAttribsToSelSet(bool bReplace = false);
	int GetTotalY();
	void SaveNonexclusiveKeys();

	void InitKeyValPanel(ICustomRollupWindow* pCustomRollupWindow, HINSTANCE hInstance, LPARAM lParam);
	void IdCancelMessage();
	void SetApplyBttnStates(bool bApplySelEnabled, bool bApplySceneEnabled);
	void LoadKeyValPanelTree();
	int HandleApplySelect(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll = false);
	int HandleApplyApply(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll = false);
	int HandleApplyPurge(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll = false);
	int HandleApplyRemap(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll = false);
	int HandleFind(const KeyValueFind &keyValueFind, bool bCurrentSelection, bool bAppendSelection);
	void HandleFindButton(void);
	int HandlePurgeUnused(bool bApplyToAll = false);
	void HandleApplyDefaultValues(bool bApplyToAll = false);

	void GetCommandKeyValues(const MArgDatabase &argDatabase,Keeper<KeyInfo> &keyValueKeeper,Keeper<ValInfo> &valueKeeper);

	MStatus HandleQueryCommand(const MArgDatabase &argDatabase, MStringArray& resultStrings, UndoRedoState &undoRedoState);
	MStatus HandleSetCommand(const MArgDatabase &argDatabase, MStringArray& resultStrings, UndoRedoState &undoRedoState);
	MStatus HandlePurgeCommand(const MArgDatabase &argDatabase, MStringArray& resultStrings, UndoRedoState &undoRedoState);
	MStatus HandleRemapCommand(const MArgDatabase &argDatabase, MStringArray& resultStrings, UndoRedoState &undoRedoState);
	MStatus HandleFindCommand(const MArgDatabase &argDatabase, MStringArray& resultStrings, UndoRedoState &undoRedoState);
	MStatus HandlePurgeUnusedCommand(const MArgDatabase &argDatabase, MStringArray& resultStrings, UndoRedoState &undoRedoState);
	MStatus HandleApplyDefaultValuesCommand(const MArgDatabase &argDatabase, MStringArray& resultStrings, UndoRedoState &undoRedoState);
   MStatus HandleRefreshDialogCommand(const MArgDatabase &argDatabase, MStringArray& resultStrings, UndoRedoState &undoRedoState);

	void UpdateStatsText(const TupString &statsLine);

	bool IsCheckListKey(const TupString &keyString);
	void GetCheckListBoxValues(const char *pValueString,Keeper<TupString> &keyValues);

	bool OkToCopy(void) const;
	bool OkToPaste(void) const;
	bool PasteAttribsToSelSet(bool bReplace = false);
	bool CopySelection(void);

protected:
	// Protected methods:
	static bool StripAllButRawData(TupString& strToFix);
	static void AddRawDataToString(TupString& strToAddTo,const TupString &strToInsert);
	static TupString FixKeyString(const char *pStringToFix);
	static TupString FixValueString(const char *pStringToFix);

	bool LoadCurrAttribDataScene();
	void LoadCurrAttribDataNode(MDagPath mDagPath);
	void LoadCurrAttribDataNode();

	bool ObjectMatchesFindCriteria(const Keeper<KeyInfo> &contextKeyValues, const Keeper<KeyInfo> &keyValueKeeper,	const Keeper<ValInfo> &valueKeeper,	bool bMatchCase,bool bMatchWholeWord,	bool bSearchDefaults, bool bSearchUnused);
	bool ValueMatchesFindCriteria(const Keeper<KeyInfo> &contextKeyValues, int nKeyIndex, const TupString &matchValue, bool bMatchCase, bool bMatchWholeWord, bool bSearchDefaults, bool bSearchUnused);
	bool ObjectMatchesCheckedItems(CurrAttribData* pCurrAttribData, Keeper<KeyInfo>& checkedKeyKeeper);
	bool MatchSplitString(const char *pStringToSplit, const char *pTokenString, const char *pMatchString, bool bMatchCase, bool bMatchWholeWord);
	void SplitString(const char *pSourceString,const char *pTokenString,Keeper<TupString> &destArray);
	TupString BuildCheckListBoxValue(const Keeper<TupString> &keyValues);


	// Protected data members:
	ConfigRoot* m_pConfigRoot;
	CurrAttribController* m_pCurrAttribController;
	UIController* m_pUIController;
	ExclusiveController* m_pExclusiveController;
	KeyValCommunicator* m_pKeyValComm;
	TupArray<KeyValuePair> *m_pCopiedKeyValPairs;
	Keeper<TupString> *m_pCheckListBoxKeys;

	HWND m_hParentDlg;
	HINSTANCE m_hInstance;
	HWND m_hToolTip;

private:
};
//-----------------------------------------------------------------------------


#endif
