//-----------------------------------------------------------------------------
//	File:		KeyValCommunicator.h
//	Created:	May 1, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The KeyValCommunicator class derives from KeyValCommunicatorBase
//			class in the KeyVal shared library.  This class will handle all
//			communication between the plugin and the shared library.  The types
//			of communication necessary would be things such as redrawing the
//			dynamic UI, purging keys, remaping keys or values, etc.
//
//	History:	5/01/2002: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------


#ifndef __KEYVALCOMMUNICATOR__
#define __KEYVALCOMMUNICATOR__



//-----------------------------------------------------------------------------
// Includes:


//-----------------------------------------------------------------------------
// Pre-declarations, etc.:

class MotionAttrController;
class CurrAttribController;
class CurrAttribData;


//-----------------------------------------------------------------------------
// Class Declaration:

class KeyValCommunicator : public KeyValCommunicatorBase
{
public:
	// Constructors and destructors:
	KeyValCommunicator(MotionAttrController* pMotionAttrController, char* pRegKeyName);
	~KeyValCommunicator();
	
	// Misc public methods:
	void LoadKeyValPanelData(Keeper<KeyInfo>& keyKeeper);
	void AddKeyInfoData(CurrAttribController *pCurrAttribController,CurrAttribData* pCurrAttribData,Keeper<KeyInfo>& keyKeeper);
	int HandleApplySelect(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll = false);
	int HandleApplyApply(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll = false);
	int HandleApplyPurge(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll = false);
	int HandleApplyRemap(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll = false);
	int HandleFind(const KeyValueFind &keyValueFind, bool bCurrentSelection, bool bAppendSelection);
	bool IsCheckListKey(const TupString &keyString);
	void UpdateStatsText(const TupString &statsLine);
	
protected:
	// Protected methods:
	
	// Protected data members:
	MotionAttrController* m_pMotionAttrController;
	
private:
};

//-----------------------------------------------------------------------------


#endif
