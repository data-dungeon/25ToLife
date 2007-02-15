//------------------------------------------------------------------------
//	File:		FaceAttrController.cpp
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
//------------------------------------------------------------------------



//------------------------------------------------------------------------
// Includes:
#include "pfdModPCH.h"
#include "FaceAttrController.h"
#include "CurrMeshAttribData.h"
#include "CurrAttribController.h"
#include "KeyValCommunicator.h"
#include "ConfigFilesManager.h"
#include "FaceAttributes.h"
#include "modData.h"
#include "perFaceDataMod.h"

//------------------------------------------------------------------------
// Defines, etc.:

//------------------------------------------------------------------------
// Class Definition:

//------------------------------------------------------------------------
// FaceAttrController::FaceAttrController
// This method is the default constructor for this class.
//
// Input: None
//
// Output: None
//------------------------------------------------------------------------
FaceAttrController::FaceAttrController()
{
	m_pConfigRoot = NULL;
	m_pCurrAttribController = NULL;
	m_pUIController = NULL;
	m_pExclusiveController = NULL;
	m_pKeyValComm = NULL;
	m_pCopiedKeyValPairs = NULL;
}


//------------------------------------------------------------------------
// FaceAttrController::~FaceAttrController
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//------------------------------------------------------------------------
FaceAttrController::~FaceAttrController()
{
	if (m_pConfigRoot != NULL)
	{
		delete m_pConfigRoot;
		m_pConfigRoot = NULL;
	}
	if (m_pCurrAttribController != NULL)
	{
		delete m_pCurrAttribController;
		m_pCurrAttribController = NULL;
	}
	if (m_pUIController != NULL)
	{
		m_pUIController->RemoveToolTips(m_hParentDlg,m_hInstance,m_hToolTip);
		delete m_pUIController;
		m_pUIController = NULL;
	}
	if (m_pExclusiveController != NULL)
	{
		delete m_pExclusiveController;
		m_pExclusiveController = NULL;
	}
	if (m_pKeyValComm != NULL)
	{
		delete m_pKeyValComm;
		m_pKeyValComm = NULL;
	}
	if (m_pCopiedKeyValPairs != NULL)
	{
		delete m_pCopiedKeyValPairs;
		m_pCopiedKeyValPairs = NULL;
	}
}


//------------------------------------------------------------------------
// FaceAttrController::GetCurrAttribController
// This method is an accessor for the CurrAttribController data member.
//
// Input: None.
//
// Output: Pointer to the CurrAttribController.
//------------------------------------------------------------------------
CurrAttribController* FaceAttrController::GetCurrAttribController()
{
	return m_pCurrAttribController;
}

//------------------------------------------------------------------------
// ObjAttrController::GetUIController
// This method is an accessor for the UIController data member.
//
// Input: None.
//
// Output: Pointer to the UIController.
//------------------------------------------------------------------------
UIController* FaceAttrController::GetUIController()
{
	return m_pUIController;
}

// This method will read in the object attribute configuration file and load
// the data into the appropriate classes.
//
// Input:
//	Interface* pInterface -- The interface pointer to get the plugcfg folder
//							 from.
//
// Output: True if successful, false otherwise. 
//
bool FaceAttrController::LoadConfigFile(ConfigFilesManager &configFilesManager)
{
	TupperwareAggregate* pTupAggConfigData;
	configFilesManager.GetConfigData(&pTupAggConfigData);

	if (m_pConfigRoot != NULL)
	{
		delete m_pConfigRoot;
		m_pConfigRoot = NULL;
	}

	// Now create the ConfigRoot and tell it to build the config tree:
	m_pConfigRoot = new ConfigRoot();
	if (!m_pConfigRoot->BuildConfigTree(pTupAggConfigData))
		return false;

	// Free up the tupperware aggregate:
	delete pTupAggConfigData;

	// Set up the exclusive controls:
	// Create the Exclusive Controller and initialize it:
	if (m_pExclusiveController != NULL)
	{
		delete m_pExclusiveController;
		m_pExclusiveController = NULL;
	}
	m_pExclusiveController = new ExclusiveController();
	// build a string for the registry key (use the name of the current config file)
	TupString nonExclusiveKeysKey(NONEXCLUSIVE_KEYS_KEY);
	nonExclusiveKeysKey += "\\";
	nonExclusiveKeysKey += configFilesManager.GetName(configFilesManager.GetCurrentConfigIndex());
	m_pExclusiveController->Init(nonExclusiveKeysKey);
	// Tell the ConfigRoot to mark the Exclusive Controller appropriately.
	m_pConfigRoot->BuildExclusiveController(m_pExclusiveController);
	// Tell the Exclusive Controller to remove itself of unwanted keys:
	m_pExclusiveController->TossUnwantedKeys();

	return true;
}


//------------------------------------------------------------------------
// FaceAttrController::LoadCurrAttribData
// This method will look at the incoming Interface pointer and get the
// selection set from it.  If there is anything selected, it will then create
// the m_pCurrAttribController (if needed), and create any necessary
// CurrAttribData objects for it and load the data into them.  If there are any
// CurrAttribData objects already corresponding to items in the selection set,
// they are not touched.
//
// Input:
// Output: True if there was nodes in the selection set, false otherwise.
//------------------------------------------------------------------------
bool FaceAttrController::LoadCurrAttribData()
{

	TupArray<CModData *> contextData;
	if (theFaceAttributes.m_pCurrentModifier)
	{
		theFaceAttributes.m_pCurrentModifier->GetContextData(contextData);
	}
	int numSelectionContexts = contextData.GetSize();

	// Get rid of any current attribute data that may exist:
	if (m_pCurrAttribController != NULL)
	{
		m_pCurrAttribController->RemoveAll();
	}
	else
	{
		m_pCurrAttribController = new CurrAttribController();
	}

	int contextIndex;
	for (contextIndex=0;contextIndex<numSelectionContexts;contextIndex++)
	{
		CModData *pModData = contextData[contextIndex];

		if (!pModData)
			continue;

		TupArray<CurrAttribData> &keyValSets = pModData->GetKeyValSets();
		TupArray<int> &faceKeyValIndices = pModData->GetFaceKeyValIndices();
		const TupArray<int> &selectedFaces = pModData->GetSelectedFaces();

		Mesh *pMesh = pModData->GetMesh();
		if (!pMesh)
			continue;

		int numFaces = pMesh->getNumFaces();
		if (!numFaces)
			continue;

		// set up arrays if not done yet
		if (faceKeyValIndices.GetSize() != numFaces)
		{
			int oldSize = faceKeyValIndices.GetSize();
			faceKeyValIndices.SetSize(numFaces);
			for (int faceIndex=oldSize;faceIndex<numFaces;faceIndex++)
			{
				faceKeyValIndices[faceIndex] = 0;
			}
		}
		if (keyValSets.GetSize()==0)
		{
			keyValSets.SetSize(1);
		}

		int numSelectedFaces = selectedFaces.GetSize();
		if (numSelectedFaces==0)
			continue;

		int objectIndex = m_pCurrAttribController->GetIndex(pModData);
		// if this one is not in the controller already
		if (objectIndex==-1)
		{
			CurrMeshAttribData* pNewAttribData = new CurrMeshAttribData();
		
			// Set the object pointer:
			objectIndex = m_pCurrAttribController->AddModData(pModData);
			pNewAttribData->AddCurrAttribDataSets(keyValSets);
			int numSets = pNewAttribData->GetNumAttribSets();
			pNewAttribData->SetNumFaces(numFaces);

			// set up an array of ints for the faces
			TupArray<int> faceSets;
			faceSets.SetSize(numFaces);
			int faceIndex;
			for (faceIndex = 0; faceIndex<numFaces; faceIndex ++ ) 
			{ 
				faceSets[faceIndex] = 0; // start them all at 0
			}

			// set up the face attribute indexes
			for (faceIndex = 0; faceIndex<numFaces; faceIndex ++ ) 
			{
				int faceSet = faceKeyValIndices[faceIndex];
				if (faceSet<0 || faceSet>=numSets)
				{
					faceSet = 0;
					pNewAttribData->SetChangedFace(faceIndex,true); // mark any that are out of bounds to be set later
				}
				pNewAttribData->SetFaceAttribSet(faceIndex,faceSet);
			}
			// add the selected faces
			for (faceIndex=0;faceIndex<numSelectedFaces	;faceIndex++)
			{
				pNewAttribData->AddSelectedFace(selectedFaces[faceIndex]);
			}
		
			// Add the CurrAttribData object to the m_pCurrAttribController:
			m_pCurrAttribController->AddCurrMeshAttribData(pNewAttribData);
		}
		else
		{
			// if another selection from a previous mObject
			CurrMeshAttribData* pAttribData = m_pCurrAttribController->GetCurrMeshAttribData(objectIndex);
			for (int faceIndex=0;faceIndex<numSelectedFaces;faceIndex++)
			{
				pAttribData->AddSelectedFace(selectedFaces[faceIndex]);
			}
		}
	}
	return (m_pCurrAttribController->GetSize()!=0);
}

//------------------------------------------------------------------------
// FaceAttrController::SetUpUIDisplay
// This method will create the set up and create the UIDisplay structures and
// the actual UI in the UIDisplay structures prior to the dialog being shown.
//
// Input:
//  HWND hParentDlg -- The HWND for the parent dialog.
//	 HINSTANCE hInstance -- The handle to the application instance.
//	 HWND hToolTip -- The HWND for the tooltip for the parent dialog
//  HFONT hUIFont -- Some labels and controls need this font.
//  int nStartx -- The x coord that labels and controls should start at.
//  int nStarty -- The y coord that the first label/control should start at.
//  int nLabelWidth -- The largest allowable width for labels.
//  int nLabelHeight -- The height used when creating labels.
//  int nSpaceBetweenCtrls -- The amount of spacing between controls.
//	WNDPROC newColorSwatchProc -- The new color swatch proc.
//
// Output: True if successful, false otherwise.
//------------------------------------------------------------------------
bool FaceAttrController::SetUpUIDisplay(HWND hParentDlg, HINSTANCE hInstance,  HWND hToolTip,
													int nStartx, int nStarty,
													int nLabelWidth, int nLabelHeight,
													int nSpaceBetweenCtrls, WNDPROC newColorSwatchProc)
{
	bool bResult = true;
	m_hParentDlg = hParentDlg;
	m_hInstance = hInstance;
	m_hToolTip = hToolTip;

	if (m_pConfigRoot != NULL)
	{
		if (m_pUIController != NULL)
		{
			m_pUIController->RemoveToolTips(hParentDlg,hInstance,hToolTip);
			delete m_pUIController;
			m_pUIController = NULL;
		}
		m_pUIController = new UIController();
		bResult = m_pConfigRoot->BuildUIDisplay(m_pUIController, m_pCurrAttribController, m_pExclusiveController);
		if (bResult != false)
		{
			bResult = m_pUIController->SetNewSwatchProc(newColorSwatchProc);
			if (bResult != false)
			{
				bResult = m_pUIController->BuildActualUI(hParentDlg, hInstance, hToolTip,
					nStartx, nStarty,
					nLabelWidth, nLabelHeight,
					nSpaceBetweenCtrls);
			}
		}
	}
	else
	{
		// The config data needs to exist first.
		bResult = false;
	}

	return bResult;
}


//------------------------------------------------------------------------
// FaceAttrController::DestroyCurrAttribData
// This method will destroy any current attribute data.
//
// Input: None.
//
// Output: None.
//------------------------------------------------------------------------
void FaceAttrController::DestroyCurrAttribData()
{
	if (m_pCurrAttribController != NULL)
	{
		m_pCurrAttribController->RemoveAll();
		delete m_pCurrAttribController;
		m_pCurrAttribController = NULL;
	}
}


//------------------------------------------------------------------------
// FaceAttrController::DestroyUIDisplay
// This method will destroy any UIDisplay.
//
// Input: None.
//
// Output: None.
//------------------------------------------------------------------------
void FaceAttrController::DestroyUIDisplay()
{
	if (m_pUIController != NULL)
	{
		m_pUIController->RemoveToolTips(m_hParentDlg,m_hInstance,m_hToolTip);
		delete m_pUIController;
		m_pUIController = NULL;
	}
}

//------------------------------------------------------------------------
// FaceAttrController::SaveNonexclusiveKeys
// This method will tell the exclusive controller to save its nonexclusive keys
// to the registry.
//
// Input: None.
//
// Output: None.
//------------------------------------------------------------------------
void FaceAttrController::SaveNonexclusiveKeys()
{
	if (m_pExclusiveController != NULL)
		m_pExclusiveController->SaveKeyValsInReg();
}


//------------------------------------------------------------------------
// FaceAttrController::GetTotalY
// This method will ask the m_pUIController for its total Y spacing that its
// controls take up.
//
// Input: None.
//
// Output: int representing the total amount of y spacing for the controls
//		   in the m_pUIController.  If the controller is NULL, then -1 is
//		   returned.
//------------------------------------------------------------------------
int FaceAttrController::GetTotalY()
{
	if (m_pUIController != NULL)
		return m_pUIController->GetTotalY();
	else
		return -1;
}

//------------------------------------------------------------------------
// FaceAttrController::OkToCopy
// This method will indicate if we are able to copy some keyvalue pairs from a selected face
//
// Input:
//	none
//
// Output: True if we are able to copy to the copy buffer
//------------------------------------------------------------------------
bool FaceAttrController::OkToCopy(void) const
{
	// only able to copy into copy buffer if only one face selected
	if (m_pCurrAttribController != NULL)
	{
		int nNumMeshes = m_pCurrAttribController->GetSize();
		if (nNumMeshes==1)
		{
			CurrMeshAttribData *pCurrMeshAttribData = m_pCurrAttribController->GetCurrMeshAttribData(0);
			if (pCurrMeshAttribData->GetNumSelectedFaces() == 1)
				return true;
		}
	}
	return false;
}

//------------------------------------------------------------------------
// FaceAttrController::OkToPaste
// This method will indicate if we are able to paste some keyvalue pairs to some selected faces
//
// Input:
//	none
//
// Output: True if we are able to paste from the copy buffer
//------------------------------------------------------------------------
bool FaceAttrController::OkToPaste(void) const
{
	// only able to copy into copy buffer if only one face selected
	if (m_pCurrAttribController != NULL)
	{
		int nNumMeshes = m_pCurrAttribController->GetSize();
		if (nNumMeshes && m_pCopiedKeyValPairs)
		{
			return true;
		}
	}
	return false;
}

//------------------------------------------------------------------------
// FaceAttrController::CopySelection
// This method will copy the keyvalue pairs from the selected face into the copy buffer
//
// Input:
//	none
//
// Output: True if we are able to paste from the copy buffer
//------------------------------------------------------------------------
bool FaceAttrController::CopySelection(void)
{
	bool bResult = false;
	if (m_pCopiedKeyValPairs)
	{
		delete m_pCopiedKeyValPairs;
	}
	m_pCopiedKeyValPairs = new TupArray<KeyValuePair>;
	
	if (m_pCurrAttribController != NULL)
	{
		// Check to see if any objects are selected:
		int nNumMeshes = m_pCurrAttribController->GetSize();
		if (nNumMeshes==1)
		{
			CurrMeshAttribData *pCurrMeshAttribData = m_pCurrAttribController->GetCurrMeshAttribData(0);
			int numSelectedFaces = pCurrMeshAttribData->GetNumSelectedFaces();
			if (numSelectedFaces)
			{
				int selectedFaceIndex = pCurrMeshAttribData->GetSelectedFace(0);
				int attributeSet = pCurrMeshAttribData->GetFaceAttributeSet(selectedFaceIndex);
				const CurrAttribData &currAttribData = pCurrMeshAttribData->GetAttribSet(attributeSet);
				(*m_pCopiedKeyValPairs)=currAttribData.GetKeyValuePairs();
				bResult = true;
			}
		}
	}
	return bResult;
}


//------------------------------------------------------------------------
// FaceAttrController::PasteAttribsToSelSet
// This method will work with the m_pCurrAttribController to copy the attributes
// from the node passed in to the nodes in the current selection set (which
// should be currently stored in the m_pCurrAttribController).
//
// Input:
//	bool bReplace -- True if the destination nodes should have all of their
//					 attributes thrown away and replaced with the source node's
//					 attributes.  Default value is false.
//
// Output: False if there were no attributes to copy, the
//		   m_pCurrAttribController is currently NULL (which means there are no
//		   objects in the selection), or there are no objects selected.  True
//		   if the copying was successful.
//		   Actually, if the buttons are greyed out whenever there is nothing
//		   in the selection set, then the only time this method will return
//		   false is if there are no attribs to copy.
//------------------------------------------------------------------------
bool FaceAttrController::PasteAttribsToSelSet(bool bReplace)
{
	bool bResult = false;
	
	if (m_pCurrAttribController != NULL)
	{
		// Check to see if any objects are selected:
		int nNumMeshes = m_pCurrAttribController->GetSize();
		if (nNumMeshes && m_pCopiedKeyValPairs)
		{
			// Check to see if there are any attribs to copy:
			int nNumAttribs = m_pCopiedKeyValPairs->GetSize();
			// set on other selected objects
			for (int nMeshIndex = 0; nMeshIndex < nNumMeshes; nMeshIndex++)
			{
				CurrMeshAttribData *pCurrMeshAttribData = m_pCurrAttribController->GetCurrMeshAttribData(nMeshIndex);
				// See if the destination node's attributes should be tossed:
				if (bReplace == true)
				{
					pCurrMeshAttribData->RemoveAllKeynames();
				}
				for (int attribIndex=0;attribIndex<nNumAttribs;attribIndex++)
				{
					pCurrMeshAttribData->AddKeyValue((*m_pCopiedKeyValPairs)[attribIndex]);
				}
				SetModData(nMeshIndex);
				bResult = true;
			}
		}
	}
	return bResult;
}


//------------------------------------------------------------------------
// FaceAttrController::HandleWinMessage
// This method will take the incoming message and pass it onto the
// m_pUIController to see if any of its controls want to handle the message.
// The return value from m_pUIController is then passed back as the return
// value for this method.
//
// Input:
//  HWND hParent -- The parent HWND of the control the message is regarding.
//  UINT message -- Specifies the message.
//  WPARAM wParam -- Specifies additional message information. The contents of
//					 this parameter depend on the value of message. 
//  LPARAM lParam -- Specifies additional message information. The contents of
//					 this parameter depend on the value of message.
//
// Output: True if the message was handled, false otherwise.
//------------------------------------------------------------------------
bool FaceAttrController::HandleWinMessage(HWND hParent, UINT message, WPARAM wParam, LPARAM lParam)
{
	bool bResult = false;

	if (m_pUIController != NULL)
	{
		// Set up some parameters that can be used as return values to check:
		TupString strKeyname;
		TupString strValue;
		TupArray<TupString> vectKeysToPurge;
		int nFlags = 0;

		// Pass the message on to the UIController:
		bResult = m_pUIController->HandleWinMessage(hParent, message, wParam, lParam, strKeyname, strValue, vectKeysToPurge, nFlags);
		if (bResult == true)
		{
			if (nFlags & UIDisplayBase::WIN_MESSAGE_FLAG_EXCLUSIVE)
			{
				// Alter the current object data:
				if (m_pCurrAttribController != NULL)
				{
					bool bUpdateData = false;
					// set key value pair
					if (nFlags & UIDisplayBase::WIN_MESSAGE_FLAG_SET_KEY_VALUE)
					{
						m_pCurrAttribController->AddKeyValue(strKeyname, strValue);
						bUpdateData = true;
					}

					// check list
					if (nFlags & UIDisplayBase::WIN_MESSAGE_FLAG_FORM_CHECK_LIST)
					{
						bool bChecked = (nFlags & UIDisplayBase::WIN_MESSAGE_FLAG_CHECK_LIST_CHECKED) ? true : false;
//						m_pCurrAttribController->FormCheckListVal(strKeyname, strValue, bChecked);
						bUpdateData = true;
					}

					if (nFlags & UIDisplayBase::WIN_MESSAGE_FLAG_PURGE_KEYS)
					{
						// Now purge any of the specified keys:
						int nNumPurgeKeys = vectKeysToPurge.GetSize();
						for (int nLoop = 0; nLoop < nNumPurgeKeys; nLoop++)
						{
							m_pCurrAttribController->RemoveKeyInstances(vectKeysToPurge[nLoop]);
						}
						bUpdateData = true;
					}

					if (bUpdateData)
					{
						// Set the data on the object:
						int nNumObjects = m_pCurrAttribController->GetSize();
						for (int nIndex = 0; nIndex < nNumObjects; nIndex++)
						{
							CModData *pModData = m_pCurrAttribController->GetModData(nIndex);
							CurrMeshAttribData *pMeshAttribData = m_pCurrAttribController->GetCurrMeshAttribData(nIndex);
							int numSets = pMeshAttribData->GetNumAttribSets();
							int numFaces = pMeshAttribData->GetNumFaces();

							TupArray<CurrAttribData> &modKeyValSets = pModData->GetKeyValSets();
							modKeyValSets.RemoveAll();
							TupArray<int> &modFaceKeyValIndices = pModData->GetFaceKeyValIndices();
							modFaceKeyValIndices.RemoveAll();

							modKeyValSets.SetSize(numSets);
							modFaceKeyValIndices.SetSize(numFaces);

							for (int setIndex=0;setIndex<numSets;setIndex++)
							{
								modKeyValSets[setIndex] = pMeshAttribData->GetAttribSet(setIndex);
							}

							CurrMeshAttribData *pCurrMeshAttribData = m_pCurrAttribController->GetCurrMeshAttribData(nIndex);
							for (int faceIndex = 0; faceIndex<numFaces; faceIndex++) 
							{ 
								modFaceKeyValIndices[faceIndex] = pCurrMeshAttribData->GetFaceAttributeSet(faceIndex);
							}
						}
					}
				}
				else
				{
					// I don't believe that this should ever be reached!
					;
				}
			}
			else
			{
				// Set the data in the Exclusive Controller instead:
				if (nFlags & UIDisplayBase::WIN_MESSAGE_FLAG_SET_KEY_VALUE)
				{
					m_pExclusiveController->SetKeyVal(strKeyname, strValue);
				}
			}
		}
	}
	else
	{
		bResult = false;
	}

	return bResult;
}

//------------------------------------------------------------------------
// FaceAttrController::InitKeyValPanel
// This method will tell the KeyValCommunicator to set up the KeyVal panel.
//
// Input:
//	IRollupWindow* pRollupWindow -- The rollup window that the panel will be
//									appended to.
//  HINSTANCE hInstance -- The INSTANCE to use for the keyval panel creation.
//  LPARAM lParam -- The LPARAM to use for the keyval panel creation.
//	int nCategory -- The category to use for the keyval panel order.
//
// Output: None.
//------------------------------------------------------------------------
void FaceAttrController::InitKeyValPanel(ICustomRollupWindow* pCustomRollupWindow, HINSTANCE hInstance, LPARAM lParam)
{
	if (m_pKeyValComm == NULL)
	{
		m_pKeyValComm = new KeyValCommunicator(this, OBJECT_ATTRIB_KEY);
	}
	m_pKeyValComm->InitKeyValPanel(pCustomRollupWindow, hInstance, lParam);
}


//------------------------------------------------------------------------
// FaceAttrController::IdCancelMessage
// This method will tell the KeyValCommunicator that the IDCANCEL message has
// been received.
//
// Input: None.
//
// Output: None.
//------------------------------------------------------------------------
void FaceAttrController::IdCancelMessage()
{
	if (m_pKeyValComm != NULL)
		m_pKeyValComm->IdCancelMessage();
}


//------------------------------------------------------------------------
// FaceAttrController::SetApplyBttnStates
// This method will tell the KeyValCommunicator to set the apply button states
// for the key/val panel.
//
// Input:
//  bool bApplySelEnabled -- True if the Apply To Sel button is to be enabled.
//  bool bApplySceneEnabled -- True if the Apply To Scene button is to be
//							   enabled.
//
// Output: None.
//------------------------------------------------------------------------
void FaceAttrController::SetApplyBttnStates(bool bApplySelEnabled, bool bApplySceneEnabled)
{
	if (m_pKeyValComm != NULL)
		m_pKeyValComm->SetApplyBttnStates(bApplySelEnabled, bApplySceneEnabled);
}


//------------------------------------------------------------------------
// FaceAttrController::LoadKeyValPanelTree
// This method will tell the KeyValCommunicator to load the key/vals in the
// key/val panel.
//
// Input: None.
//
// Output: None.
//------------------------------------------------------------------------
void FaceAttrController::LoadKeyValPanelTree()
{
	if (m_pKeyValComm != NULL)
		m_pKeyValComm->LoadKeyValPanelTree();
}


//------------------------------------------------------------------------
// FaceAttrController::HandleApplySelect
// This method will be called to select nodes based on the keys and vals
// checked in the incoming keykeeper.  The boolean passed in specifies whether
// to select nodes from the selection set, or the whole scene.
//
// Input:
//  Keeper<KeyInfo>& keyKeeper -- The keeper structure that contains the keys
//								  and vals that are checked.
//	bool bApplyToAll -- Pass in false to select nodes from the selection set
//						only.  Pass in true to select nodes from the whole
//						scene.
//
// Output: None.
//------------------------------------------------------------------------
void FaceAttrController::HandleApplySelect(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll)
{
	// If the selection is to be applied to the whole scene, then load the
	// current attrib data for the whole scene:
	if (bApplyToAll == true)
	{
		LoadCurrAttribDataScene();
	}

	// Tell the FaceAttrController to get the current attribute data:
	if (m_pCurrAttribController != NULL)
	{
		// Make a copy of the keyKeeper with all the unchecked items removed:
		Keeper<KeyInfo> checkedKeyKeeper;
		int nNumKeys = keyKeeper.GetSize();
		for (int nKeyIndex = 0; nKeyIndex < nNumKeys; nKeyIndex++)
		{
			if (keyKeeper[nKeyIndex].m_bChecked == true)
			{
				KeyInfo newKeyInfo;
				newKeyInfo.m_bChecked = true;
				newKeyInfo.m_strKeyname = keyKeeper[nKeyIndex].m_strKeyname;
				int nNumVals = keyKeeper[nKeyIndex].m_valKeeper.GetSize();
				for (int nValIndex = 0; nValIndex < nNumVals; nValIndex++)
				{
					if (keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_bChecked == true)
					{
						ValInfo newValInfo;
						newValInfo.m_bChecked = true;
						newValInfo.m_strVal = keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_strVal;
						newKeyInfo.m_valKeeper.Add(newValInfo);
					}
				}
				checkedKeyKeeper.Add(newKeyInfo);
			}
		}		

		// Now check each node in the current attrib data and see if it matches
		// the items checked in the keeper.  If so, add it to our new selection
		// set:
		int nNumMeshes = m_pCurrAttribController->GetSize();
		for (int nMeshIndex = 0; nMeshIndex < nNumMeshes; nMeshIndex++)
		{
			CModData *pModData = m_pCurrAttribController->GetModData(nMeshIndex);

			if (!pModData)
				continue;

			CurrMeshAttribData* pCurrMeshAttribData = m_pCurrAttribController->GetCurrMeshAttribData(nMeshIndex);
			int numSets = pCurrMeshAttribData->GetNumAttribSets();
			TupArray<bool> setMatchArray;
			setMatchArray.SetSize(numSets);
			for (int nSetIndex = 0;nSetIndex<numSets;nSetIndex++)
			{
				if (pCurrMeshAttribData->SelectedFacesUsingSet(nSetIndex))
				{
					const CurrAttribData &currAttribData = pCurrMeshAttribData->GetAttribSet(nSetIndex);
					// set if this set matches the checked items
					setMatchArray[nSetIndex] = ObjectMatchesCheckedItems(currAttribData,checkedKeyKeeper);
				}
				else
				{
					setMatchArray[nSetIndex] = false;
				}
			}

			const TupArray<int> &faceKeyValIndices = pModData->GetFaceKeyValIndices();

			int numFaces = faceKeyValIndices.GetSize();

			TupArray<int> newSelectedFaces;

			for (int faceIndex=0;faceIndex<numFaces;faceIndex++)
			{
				int setIndex = faceKeyValIndices[faceIndex];
				if (setIndex>=0 && setIndex<numSets && setMatchArray[setIndex])
				{
					newSelectedFaces.Add(faceIndex);
				}
			}
			pModData->SetFaceSelection(newSelectedFaces);
		}
	}
	if (theFaceAttributes.m_pCurrentModifier)
	{
		theFaceAttributes.m_pCurrentModifier->LocalDataChanged();
	}
	GetCOREInterface()->ForceCompleteRedraw();
}


//------------------------------------------------------------------------
// FaceAttrController::ObjectMatchesCheckedItems
// This method will check the object represented by the incoming CurrAttribData
// pointer to see if the keys and values it has matches the ones checked in the
// incoming keykeeper.
//
// Input:
//  CurrAttribData* pCurrAttribData -- The current attrib data for the
//									   object to check.
//  Keeper<KeyInfo>& checkedKeyKeeper -- The keykeeper to match against.
//
// Output: True if the object matches, false otherwise.
//------------------------------------------------------------------------
bool FaceAttrController::ObjectMatchesCheckedItems(const CurrAttribData &currAttribData, Keeper<KeyInfo>& checkedKeyKeeper)
{
	bool bObjMatches = true;
	// Check that this object has each key specified in the keykeeper:
	int nNumKeys = checkedKeyKeeper.GetSize();
	for (int nKeyIndex = 0; nKeyIndex < nNumKeys; nKeyIndex++)
	{
		int nKeynameAt = currAttribData.KeynameExistsAt(checkedKeyKeeper[nKeyIndex].m_strKeyname);
		if (nKeynameAt < 0)
		{
			bObjMatches = false;
			break;
		}
		else
		{
			// Check to see if this object has a value equal to one of the vals
			// specified for this key in the keykeeper:
			bool bValMatches = false;
			int nNumVals = checkedKeyKeeper[nKeyIndex].m_valKeeper.GetSize();
			for (int nValIndex = 0; nValIndex < nNumVals; nValIndex++)
			{
				if (currAttribData.GetValueAsString(nKeynameAt) == checkedKeyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_strVal)
				{
					bValMatches = true;
					break;
				}
			}
			if ((nValIndex > 0) && (bValMatches == false))
			{
				bObjMatches = false;
				break;
			}
		}
	}
	return bObjMatches;
}

//------------------------------------------------------------------------
// FaceAttrController::HandleApplyApply
// This method will be called to apply specific vals for the keys selected in
// the incoming keeper, to nodes in either the scene or selection set,
// depending on the boolean passed in.
//
// Input:
//  Keeper<KeyInfo>& keyKeeper -- The keeper structure that contains the keys
//								  and vals that are checked.
//	bool bApplyToAll -- Pass in false to apply to nodes from the selection set
//						only.  Pass in true to apply to nodes from the whole
//						scene.
//
// Output: None.
//------------------------------------------------------------------------
void FaceAttrController::HandleApplyApply(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll)
{
	// If the Apply is to be applied to the whole scene, then load the
	// current attrib data for the whole scene:
	if (bApplyToAll == true)
	{
		LoadCurrAttribDataScene();
	}

	// Get the current attrib data:
	if (m_pCurrAttribController != NULL)
	{
		// Make a copy of the keyKeeper with all the unchecked vals removed:
		Keeper<KeyInfo> checkedKeyKeeper;
		int nNumKeys = keyKeeper.GetSize();
		for (int nKeyIndex = 0; nKeyIndex < nNumKeys; nKeyIndex++)
		{
			KeyInfo newKeyInfo;
			bool bValChecked = false;
			int nNumVals = keyKeeper[nKeyIndex].m_valKeeper.GetSize();
			for (int nValIndex = 0; nValIndex < nNumVals; nValIndex++)
			{
				if (keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_bChecked == true)
				{
					ValInfo newValInfo;
					newValInfo.m_bChecked = true;
					newValInfo.m_strVal = keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_strVal;
					newKeyInfo.m_valKeeper.Add(newValInfo);
					bValChecked = true;
					break;
				}
			}
			if (bValChecked == true)
			{
				newKeyInfo.m_bChecked = true;
				newKeyInfo.m_strKeyname = keyKeeper[nKeyIndex].m_strKeyname;
				checkedKeyKeeper.Add(newKeyInfo);
			}
		}		

		// Now walk through each node specified in the curr attrib data and
		// adjust any values necessary:
		int nNumCurrAttribDatas = m_pCurrAttribController->GetSize();
		for (int nCADataIndex = 0; nCADataIndex < nNumCurrAttribDatas; nCADataIndex++)
		{
			CurrMeshAttribData* pCurrMeshAttribData = m_pCurrAttribController->GetCurrMeshAttribData(nCADataIndex);
			int nNumKeeperKeys = checkedKeyKeeper.GetSize();
			for (int nKeeperKeyIndex = 0; nKeeperKeyIndex < nNumKeeperKeys; nKeeperKeyIndex++)
			{
				pCurrMeshAttribData->AddKeyValue(checkedKeyKeeper[nKeeperKeyIndex].m_strKeyname,
												   checkedKeyKeeper[nKeeperKeyIndex].m_valKeeper[0].m_strVal);
			}
			SetModData(nCADataIndex);
		}
		// Now reset the selection set so that data gets loaded correctly and
		// the dialogs get updated correctly:
		if (theFaceAttributes.m_pCurrentModifier)
		{
			theFaceAttributes.m_pCurrentModifier->LocalDataChanged();
		}
	}
}


//------------------------------------------------------------------------
// FaceAttrController::HandleApplyPurge
// This method will be called to purge keys and vals (specified in the incoming
// keykeeper) from nodes in either the selection set, or from the whole scene.
//
// Input:
//  Keeper<KeyInfo>& keyKeeper -- The keeper structure that contains the keys
//								  and vals that are marked for purging.
//	bool bApplyToAll -- Pass in false to apply to nodes from the selection set
//						only.  Pass in true to apply to nodes from the whole
//						scene.
//
// Output: None.
//------------------------------------------------------------------------
void FaceAttrController::HandleApplyPurge(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll)
{
	// If the purging is to be applied to the whole scene, then load the
	// current attrib data for the whole scene:
	if (bApplyToAll == true)
	{
		LoadCurrAttribDataScene();
	}

	// Get the current attrib data:
	if (m_pCurrAttribController != NULL)
	{
		// Make a copy of the keyKeeper with all the unchecked vals removed:
		Keeper<KeyInfo> checkedKeyKeeper;
		int nNumKeys = keyKeeper.GetSize();
		for (int nOrigKeyIndex = 0; nOrigKeyIndex < nNumKeys; nOrigKeyIndex++)
		{
			KeyInfo newKeyInfo;
			newKeyInfo.m_bChecked = keyKeeper[nOrigKeyIndex].m_bChecked;
			bool bValChecked = false;
			int nNumVals = keyKeeper[nOrigKeyIndex].m_valKeeper.GetSize();
			for (int nOrigValIndex = 0; nOrigValIndex < nNumVals; nOrigValIndex++)
			{
				if (keyKeeper[nOrigKeyIndex].m_valKeeper[nOrigValIndex].m_bChecked == true)
				{
					ValInfo newValInfo;
					newValInfo.m_bChecked = true;
					newValInfo.m_strVal = keyKeeper[nOrigKeyIndex].m_valKeeper[nOrigValIndex].m_strVal;
					newKeyInfo.m_valKeeper.Add(newValInfo);
					bValChecked = true;
				}
			}

			// Only add the key (with its checked vals), if there were any vals
			// checked for this key:
			if (bValChecked == true)
			{
				newKeyInfo.m_strKeyname = keyKeeper[nOrigKeyIndex].m_strKeyname;
				checkedKeyKeeper.Add(newKeyInfo);
			}
		}

		// Now walk through each node specified in the curr attrib data and
		// purge any keys and/or vals as necessary:
		int nNumCurrAttribDatas = m_pCurrAttribController->GetSize();
		for (int nCADataIndex = 0; nCADataIndex < nNumCurrAttribDatas; nCADataIndex++)
		{
			CurrMeshAttribData* pCurrMeshAttribData = m_pCurrAttribController->GetCurrMeshAttribData(nCADataIndex);
			int nNumKeeperKeys = checkedKeyKeeper.GetSize();
			for (int nKeeperKeyIndex = 0; nKeeperKeyIndex < nNumKeeperKeys; nKeeperKeyIndex++)
			{
				// Check to see if this node has this key:
				if (checkedKeyKeeper[nKeeperKeyIndex].m_bChecked == true)
				{
					// Purge the key from this node:
					pCurrMeshAttribData->RemoveKeyname(checkedKeyKeeper[nKeeperKeyIndex].m_strKeyname);
				}
				else
				{
					// Only purge if this node's val is equal to one of the vals
					// specified to be purged:
					int nNumKeeperVals = checkedKeyKeeper[nKeeperKeyIndex].m_valKeeper.GetSize();
					for (int nKeeperValIndex = 0; nKeeperValIndex < nNumKeeperVals; nKeeperValIndex++)
					{
						// Purge the key and value from the mesh
						pCurrMeshAttribData->RemoveKeyname(checkedKeyKeeper[nKeeperKeyIndex].m_strKeyname,
														   checkedKeyKeeper[nKeeperKeyIndex].m_valKeeper[nKeeperValIndex].m_strVal);
					}
				}
			}
			SetModData(nCADataIndex);
		}
		// Now reset the selection set so that data gets loaded correctly and
		// the dialogs get updated correctly:
		if (theFaceAttributes.m_pCurrentModifier)
		{
			theFaceAttributes.m_pCurrentModifier->LocalDataChanged();
		}
	}
}

//------------------------------------------------------------------------
// FaceAttrController::HandleApplyRemap
// This method will be called to remap keys and vals (specified in the incoming
// keykeeper) within nodes in either the selection set, or from the whole scene.
//
// Input:
//  Keeper<KeyInfo>& keyKeeper -- The keeper structure that contains the keys
//								  and vals that are marked for remapping.
//	bool bApplyToAll -- Pass in false to apply to nodes from the selection set
//						only.  Pass in true to apply to nodes from the whole
//						scene.
//
// Input:
//  Keeper<KeyInfo>& keyKeeper -- 
//  bool bApplyToAll -- 
//
// Output: None.
//------------------------------------------------------------------------
void FaceAttrController::HandleApplyRemap(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll)
{
	// If the remapping is to be applied to the whole scene, then load the
	// current attrib data for the whole scene:
	if (bApplyToAll == true)
	{
		LoadCurrAttribDataScene();
	}

	// Get the current attrib data:
	if (m_pCurrAttribController != NULL)
	{
		// Make a copy of the keyKeeper with all the unchecked vals and keys removed:
		Keeper<KeyInfo> checkedKeyKeeper;
		int nNumKeys = keyKeeper.GetSize();
		for (int nOrigKeyIndex = 0; nOrigKeyIndex < nNumKeys; nOrigKeyIndex++)
		{
			KeyInfo newKeyInfo;
			newKeyInfo.m_bChecked = keyKeeper[nOrigKeyIndex].m_bChecked;

			// Check to see if this key is checked:
			if (newKeyInfo.m_bChecked == true)
			{
				newKeyInfo.m_strKeyname = keyKeeper[nOrigKeyIndex].m_strKeyname;
				newKeyInfo.m_strNewKeyname = keyKeeper[nOrigKeyIndex].m_strNewKeyname;

				// Add all the vals for this key, regardless if they have been checked or not:
				int nNumVals = keyKeeper[nOrigKeyIndex].m_valKeeper.GetSize();
				for (int nOrigValIndex = 0; nOrigValIndex < nNumVals; nOrigValIndex++)
				{
					ValInfo newValInfo;
					newValInfo.m_bChecked = keyKeeper[nOrigKeyIndex].m_valKeeper[nOrigValIndex].m_bChecked;
					newValInfo.m_strVal = keyKeeper[nOrigKeyIndex].m_valKeeper[nOrigValIndex].m_strVal;
					newValInfo.m_strNewVal = keyKeeper[nOrigKeyIndex].m_valKeeper[nOrigValIndex].m_strNewVal;
					newKeyInfo.m_valKeeper.Add(newValInfo);
				}

				checkedKeyKeeper.Add(newKeyInfo);
			}
			else
			{
				bool bValChecked = false;
				int nNumVals = keyKeeper[nOrigKeyIndex].m_valKeeper.GetSize();
				for (int nOrigValIndex = 0; nOrigValIndex < nNumVals; nOrigValIndex++)
				{
					// Only add the checked vals:
					if (keyKeeper[nOrigKeyIndex].m_valKeeper[nOrigValIndex].m_bChecked == true)
					{
						ValInfo newValInfo;
						newValInfo.m_bChecked = true;
						newValInfo.m_strVal = keyKeeper[nOrigKeyIndex].m_valKeeper[nOrigValIndex].m_strVal;
						newValInfo.m_strNewVal = keyKeeper[nOrigKeyIndex].m_valKeeper[nOrigValIndex].m_strNewVal;
						newKeyInfo.m_valKeeper.Add(newValInfo);
						bValChecked = true;
					}
				}

				if (bValChecked == true)
				{
					newKeyInfo.m_strKeyname = keyKeeper[nOrigKeyIndex].m_strKeyname;
					newKeyInfo.m_strNewKeyname = keyKeeper[nOrigKeyIndex].m_strNewKeyname;
					checkedKeyKeeper.Add(newKeyInfo);
				}
			}
		}

		// Now walk through each node specified in the curr attrib data and
		// remap any keys and/or vals as necessary:
		int nNumCurrAttribDatas = m_pCurrAttribController->GetSize();
		for (int nCADataIndex = 0; nCADataIndex < nNumCurrAttribDatas; nCADataIndex++)
		{
			CurrMeshAttribData* pCurrMeshAttribData = m_pCurrAttribController->GetCurrMeshAttribData(nCADataIndex);
			int nNumKeeperKeys = checkedKeyKeeper.GetSize();
			for (int nKeeperKeyIndex = 0; nKeeperKeyIndex < nNumKeeperKeys; nKeeperKeyIndex++)
			{
				TupString strOrigKey = checkedKeyKeeper[nKeeperKeyIndex].m_strKeyname;
				TupString strOrigVal;
				TupString strNewVal;

				// Remap the values
				int nNumKeeperVals = checkedKeyKeeper[nKeeperKeyIndex].m_valKeeper.GetSize();
				for (int nKeeperValIndex = 0; nKeeperValIndex < nNumKeeperVals; nKeeperValIndex++)
				{
					if ((checkedKeyKeeper[nKeeperKeyIndex].m_valKeeper[nKeeperValIndex].m_bChecked == true))
					{
						strOrigVal = checkedKeyKeeper[nKeeperKeyIndex].m_valKeeper[nKeeperValIndex].m_strVal;
						strNewVal = checkedKeyKeeper[nKeeperKeyIndex].m_valKeeper[nKeeperValIndex].m_strNewVal;
						pCurrMeshAttribData->RenameVal(strOrigKey,strOrigVal,strNewVal);
					}
				}

				// Remap the key
				if (checkedKeyKeeper[nKeeperKeyIndex].m_bChecked == true)
				{
					TupString strNewKey = checkedKeyKeeper[nKeeperKeyIndex].m_strNewKeyname;
					pCurrMeshAttribData->RenameKey(strOrigKey,strNewKey);
				}
			}
			SetModData(nCADataIndex);
		}
		// Now reset the selection set so that data gets loaded correctly and
		// the dialogs get updated correctly:
		if (theFaceAttributes.m_pCurrentModifier)
		{
			theFaceAttributes.m_pCurrentModifier->LocalDataChanged();
		}
	}
}

void FaceAttrController::SetModData(int nIndex)
{
	CModData *pModData = m_pCurrAttribController->GetModData(nIndex);
	CurrMeshAttribData *pMeshAttribData = m_pCurrAttribController->GetCurrMeshAttribData(nIndex);
	int numSets = pMeshAttribData->GetNumAttribSets();
	int numFaces = pMeshAttribData->GetNumFaces();

	TupArray<CurrAttribData> &modKeyValSets = pModData->GetKeyValSets();
	modKeyValSets.RemoveAll();
	TupArray<int> &modFaceKeyValIndices = pModData->GetFaceKeyValIndices();
	modFaceKeyValIndices.RemoveAll();

	modKeyValSets.SetSize(numSets);
	modFaceKeyValIndices.SetSize(numFaces);

	for (int setIndex=0;setIndex<numSets;setIndex++)
	{
		modKeyValSets[setIndex] = pMeshAttribData->GetAttribSet(setIndex);
	}

	for (int faceIndex = 0; faceIndex<numFaces; faceIndex++) 
	{ 
		modFaceKeyValIndices[faceIndex] = pMeshAttribData->GetFaceAttributeSet(faceIndex);
	}
}

//------------------------------------------------------------------------
// FaceAttrController::LoadCurrAttribData
// This method will look at the incoming Interface pointer and get the
// selection set from it.  If there is anything selected, it will then create
// the m_pCurrAttribController (if needed), and create any necessary
// CurrAttribData objects for it and load the data into them.  If there are any
// CurrAttribData objects already corresponding to items in the selection set,
// they are not touched.
//
// Input:
// Output: True if there was nodes in the selection set, false otherwise.
//------------------------------------------------------------------------
bool FaceAttrController::LoadCurrAttribDataScene()
{
	TupArray<CModData *> contextData;
	if (theFaceAttributes.m_pCurrentModifier)
	{
		theFaceAttributes.m_pCurrentModifier->GetContextData(contextData);
	}
	int numContexts = contextData.GetSize();

	// Get rid of any current attribute data that may exist:
	if (m_pCurrAttribController != NULL)
	{
		m_pCurrAttribController->RemoveAll();
	}
	else
	{
		m_pCurrAttribController = new CurrAttribController();
	}

	int contextIndex;
	for (contextIndex=0;contextIndex<numContexts;contextIndex++)
	{
		CModData *pModData = contextData[contextIndex];

		if (!pModData)
			continue;

		const TupArray<CurrAttribData> &keyValSets = pModData->GetKeyValSets();
		const TupArray<int> &faceKeyValIndices = pModData->GetFaceKeyValIndices();

		int numFaces = faceKeyValIndices.GetSize();
		if (!numFaces)
			continue;

		int objectIndex = m_pCurrAttribController->GetIndex(pModData);
		// if this one is not in the controller already
		if (objectIndex==-1)
		{
			CurrMeshAttribData* pNewAttribData = new CurrMeshAttribData();

			// Set the object pointer:
			objectIndex = m_pCurrAttribController->AddModData(pModData);
			pNewAttribData->AddCurrAttribDataSets(keyValSets);
			int numSets = pNewAttribData->GetNumAttribSets();
			pNewAttribData->SetNumFaces(numFaces);

			// set up an array of ints for the faces
			TupArray<int> faceSets;
			faceSets.SetSize(numFaces);
			int faceIndex;
			for (faceIndex = 0; faceIndex<numFaces; faceIndex ++ ) 
			{ 
				faceSets[faceIndex] = 0; // start them all at 0
			}

			// set up the face attribute indexes
			for (faceIndex = 0; faceIndex<numFaces; faceIndex ++ ) 
			{
				int faceSet = faceKeyValIndices[faceIndex];
				if (faceSet<0 || faceSet>=numSets)
				{
					faceSet = 0;
					pNewAttribData->SetChangedFace(faceIndex,true); // mark any that are out of bounds to be set later
				}
				pNewAttribData->SetFaceAttribSet(faceIndex,faceSet);
				pNewAttribData->AddSelectedFace(faceIndex);
			}
			// Add the CurrAttribData object to the m_pCurrAttribController:
			m_pCurrAttribController->AddCurrMeshAttribData(pNewAttribData);
		}
	}
	return (m_pCurrAttribController->GetSize()!=0);
}
