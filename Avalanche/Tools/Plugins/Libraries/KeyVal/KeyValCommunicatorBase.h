//-----------------------------------------------------------------------------
//	File:		KeyValCommunicatorBase.h
//	Created:	May 1, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The KeyValCommunicatorBase class acts as a base class for classes
//			that reside in seperate plugins that provide communication between
//			the plugin and the shared library.  The types of communication
//			necessary would be things such as redrawing the dynamic UI, purging
//			keys, remaping keys or values, etc.
//
//	History:	5/01/2002: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------


#ifndef __KEYVALCOMMUNICATORBASE__
#define __KEYVALCOMMUNICATORBASE__



//-----------------------------------------------------------------------------
// Includes:



//-----------------------------------------------------------------------------
// Pre-declarations, etc.:

class KeyValPanelController;
class KeyInfo;
class KeyValueFind;

using namespace AttrControls;

//-----------------------------------------------------------------------------
// Class Declaration:

class KeyValCommunicatorBase
{
public:
	// Constructors and destructors:
	KeyValCommunicatorBase();
	virtual ~KeyValCommunicatorBase();

	// Accessors & Mutators:
	char* GetRegKeyName();

	// Misc public methods:
	void InitKeyValPanel(ICustomRollupWindow* pCustomRollupWindow, HINSTANCE hInstance, LPARAM lParam);
	void IdCancelMessage();
	void SetApplyBttnStates(bool bApplySelEnabled, bool bApplySceneEnabled);
	void LoadKeyValPanelTree();
	void ShowFindDialog(const char *pKeyName, const char *pValueName);
	void HandleFindButton(void);

	// Necessary methods to implement in derived classes:
	virtual void LoadKeyValPanelData(Keeper<KeyInfo>& keyKeeper) = 0;
	virtual int HandleApplySelect(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll = false) = 0;
	virtual int HandleApplyApply(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll = false) = 0;
	virtual int HandleApplyPurge(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll = false) = 0;
	virtual int HandleApplyRemap(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll = false) = 0;
	virtual int HandleFind(const KeyValueFind &keyValueFind, bool bCurrentSelection, bool bAppendSelection) = 0;
	virtual bool IsCheckListKey(const TupString &keyString) = 0;
	virtual void UpdateStatsText(const TupString &statsLine) = 0;

protected:
	// Protected methods:

	// Protected data members:
	KeyValPanelController* m_pKeyValPanelController;
	char* m_pRegKeyName;

private:
};

//-----------------------------------------------------------------------------


#endif
