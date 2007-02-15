//-----------------------------------------------------------------------------
//	File:		ImgAttrController.h
//	Created:	June 14, 2002
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Dave Astle, adapted from code by Brad Worthen
//
//	Desc:	The ImgAttrController class acts as the commander so-to-speak for
//			the Image Attribute plugin.  This class, plus its data member
//			classes, handles most of the functionality except for direct user
//			interface and a few other assundry things.
//-----------------------------------------------------------------------------


#ifndef __IMGATTRCONTROLLER__
#define __IMGATTRCONTROLLER__



//-----------------------------------------------------------------------------
// Includes:
#include "stdmat.h"
#include "imtl.h"
#include "ImageAttributes.h"


//-----------------------------------------------------------------------------
// Pre-declarations, etc.:

class ConfigRoot;
class CurrAttribController;
class UIController;
class ExclusiveController;
class KeyValCommunicator;
class Interface;
class ImageList;
class CurrAttribData;
class KeyInfo;
class ImageAttributes;

//-----------------------------------------------------------------------------
// Class Declaration:
class ImgAttrController
{
public:
	// Constructors and destructors:
	ImgAttrController(ImageAttributes* pOwner);
	~ImgAttrController();

	// Accessors:
	CurrAttribController* GetCurrAttribController();

	// Public methods:
	bool LoadConfigFile(Interface* pInterface);
	bool LoadCurrAttribData(Interface* pInterface, bool showMaterials);
	bool SetUpUIDisplay(HWND hParentDlg, HINSTANCE hInstance, HFONT hUIFont,
						int nStartx, int nStarty,
						int nLabelWidth, int nLabelHeight,
						int nSpaceBetweenCtrls, WNDPROC newColorSwatchProc);
	bool HandleWinMessage(HWND hParent, UINT message, WPARAM wParam, LPARAM lParam);
	void DestroyCurrAttribData();
	void DestroyUIDisplay();
	bool CopyAttribsToSelSet(bool bReplace = false);
	int GetTotalY();
	void SaveNonexclusiveKeys();
	void InitKeyValPanel(IRollupWindow* pRollupWindow, HINSTANCE hInstance, LPARAM lParam, int nCategory);
	void IdCancelMessage();
	void SetApplyBttnStates(bool bApplySelEnabled, bool bApplySceneEnabled);
	void LoadKeyValPanelTree();
	void HandleApplySelect(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll = false);
	void HandleApplyApply(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll = false);
	void HandleApplyPurge(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll = false);
	void HandleApplyRemap(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll = false);

	bool GetImageList(Interface* ip, bool viewAll, bool showMaterials);
	bool GetImageAttribString(const char *pName, std::string &attribString);
	void InitImageList(HWND hParent);
	void DisplayImageList();

protected:
	// Protected methods:
	bool CreateImageAttribNode(Interface* ip);
	bool RemoveCharsFromStr(std::string & strToFix, char chToRemove);
	bool AddCarrRtrnsToStr(std::string & strToFix);
	bool StripAllButRawData(const char *pName, std::string &strToFix);
	void AddRawDataToString(const char *pName, std::string &strToAddTo, const std::string &strToInsert);

	void BuildImgIndices(std::string strKey, std::string strVal,
								std::vector<short>& vectObjIndices, CurrAttribController* pCurrAttribController);
	bool LoadCurrAttribDataScene();
	bool ImageMatchesCheckedItems(CurrAttribData* pCurrAttribData, Keeper<KeyInfo>& checkedKeyKeeper);
	
	// Protected data members:
	ConfigRoot* m_pConfigRoot;
	CurrAttribController* m_pCurrAttribController;
	UIController* m_pUIController;
	ExclusiveController* m_pExclusiveController;
	KeyValCommunicator* m_pKeyValComm;
	ImageList* m_pImageList;
	INode* m_pImageAttribNode;
	ImageAttributes* m_pOwner;
	bool m_showMaterials;
  
private:
};
//-----------------------------------------------------------------------------


#endif
