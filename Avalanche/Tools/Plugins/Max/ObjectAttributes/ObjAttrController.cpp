//-----------------------------------------------------------------------------
//	File:		ObjAttrController.cpp
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



//-----------------------------------------------------------------------------
// Includes:

#include "ObjectAttributesPCH.h"

#include "ObjAttrController.h"
#include "CurrAttribData.h"
#include "CurrAttribController.h"
#include "KeyValCommunicator.h"
#include "ConfigFilesManager.h"

//-----------------------------------------------------------------------------
// Defines, etc.:

// Class Definition:


//-----------------------------------------------------------------------------
// ObjAttrController::ObjAttrController
// This method is the default constructor for this class.
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
ObjAttrController::ObjAttrController()
{
	m_pConfigRoot = NULL;
	m_pCurrAttribController = NULL;
	m_pUIController = NULL;
	m_pExclusiveController = NULL;
	m_pKeyValComm = NULL;
	m_pCopiedKeyValPairs = NULL;
}


//-----------------------------------------------------------------------------
// ObjAttrController::~ObjAttrController
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
ObjAttrController::~ObjAttrController()
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


//-----------------------------------------------------------------------------
// ObjAttrController::GetCurrAttribController
// This method is an accessor for the CurrAttribController data member.
//
// Input: None.
//
// Output: Pointer to the CurrAttribController.
//-----------------------------------------------------------------------------
CurrAttribController* ObjAttrController::GetCurrAttribController()
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
UIController* ObjAttrController::GetUIController()
{
	return m_pUIController;
}


//------------------------------------------------------------------------
// This method will read in the object attribute configuration file and load
// the data into the appropriate classes.
//
// Input:
//	Interface* pInterface -- The interface pointer to get the plugcfg folder
//							 from.
//
// Output: True if successful, false otherwise. 
//
bool ObjAttrController::LoadConfigFile(ConfigFilesManager &configFilesManager)
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


//-----------------------------------------------------------------------------
// ObjAttrController::LoadCurrAttribData
// This method will look at the incoming Interface pointer and get the
// selection set from it.  If there is anything selected, it will then create
// the m_pCurrAttribController (if needed), and create any necessary
// CurrAttribData objects for it and load the data into them.  If there are any
// CurrAttribData objects already corresponding to items in the selection set,
// they are not touched.
//
// Input:
//  Interface* pInterface -- The selection set is retrieved from this Interface
//							 pointer.  Any current attribute data is then
//							 retrieved from the selection set.
//
// Output: True if there was nodes in the selection set, false otherwise.
//-----------------------------------------------------------------------------
bool ObjAttrController::LoadCurrAttribData()
{
	Interface* pInterface = GetCOREInterface();

   // Get rid of any current attribute data that may exist:
   if (m_pCurrAttribController != NULL)
   {
		m_pCurrAttribController->RemoveAll();
   }
   else
   {
     m_pCurrAttribController = new CurrAttribController();
   }

	// Make sure there is something selected:
	int nNumSelNodes = pInterface->GetSelNodeCount();
	for (int nIndex = 0; nIndex < nNumSelNodes; nIndex++)
	{
		INode* pCurrNode = pInterface->GetSelNode(nIndex);

		int objectIndex = m_pCurrAttribController->GetIndex(pCurrNode);
		// if this one is not in the controller already
		if (objectIndex==-1)
		{
			CurrAttribData* pNewAttribData = new CurrAttribData();
			
			// Set the object pointer:
			objectIndex = m_pCurrAttribController->AddMaxNode(pCurrNode);
			
			// Set any data:
			TSTR buf;
			pCurrNode->GetUserPropBuffer(buf);
			char *pChObjectBuf = buf.data();
			TupString strFullData = pChObjectBuf;
			// Get to just the raw data substring:
			if (StripAllButRawData(strFullData) == true)
			{
				// Now pass on the string to load it into the current attrib data:
				pNewAttribData->StrToCurrAttribData(strFullData);
			}
			else
			{
				// No block indicators found, so don't set any data for this object:
				;
			}

			// Add the CurrAttribData object to the m_pCurrAttribController:
			m_pCurrAttribController->AddCurrAttribData(pNewAttribData);
		}
	}
	return (m_pCurrAttribController->GetSize()!=0);
}


//-----------------------------------------------------------------------------
// ObjAttrController::LoadCurrAttribDataScene
// This method will create a new m_pCurrAttribController and create any
// necessary CurrAttribData objects for all the objects in the scene and load
// the data into them.  This method makes sure that the whole node tree is
// walked through.
//
// Input: None.
//
// Output: True if there are nodes in the scene, false otherwise.
//-----------------------------------------------------------------------------
bool ObjAttrController::LoadCurrAttribDataScene()
{
	bool bNodesInScene = false;

	// Get the root node if one exists:
	Interface* pInterface = GetCOREInterface();
	INode *pRootNode = pInterface->GetRootNode();
	if (pRootNode != NULL)
	{
		bNodesInScene = true;

		// At least one node exists, so reset the m_pCurrAttribController to
		// prepare it for new attrib data:
		if (m_pCurrAttribController != NULL)
			m_pCurrAttribController->RemoveAll();
		else
			m_pCurrAttribController = new CurrAttribController();

		// Now load the data in for the root node (which in turn loads the data
		// in recursively for any of its children):
		m_pCurrAttribController->LoadCurrAttribDataNode(pRootNode);
	}

	return bNodesInScene;
}



//-----------------------------------------------------------------------------
// ObjAttrController::SetUpUIDisplay
// This method will create the set up and create the UIDisplay structures and
// the actual UI in the UIDisplay structures prior to the dialog being shown.
//
// Input:
//  HWND hParentDlg -- The HWND for the parent dialog.
//	HINSTANCE hInstance -- The handle to the application instance.
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
bool ObjAttrController::SetUpUIDisplay(HWND hParentDlg, HINSTANCE hInstance,  HWND hToolTip,
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
// ObjAttrController::DestroyCurrAttribData
// This method will destroy any current attribute data.
//
// Input: None.
//
// Output: None.
//------------------------------------------------------------------------
void ObjAttrController::DestroyCurrAttribData()
{
	if (m_pCurrAttribController != NULL)
	{
		m_pCurrAttribController->RemoveAll();
		delete m_pCurrAttribController;
		m_pCurrAttribController = NULL;
	}
}


//------------------------------------------------------------------------
// ObjAttrController::DestroyUIDisplay
// This method will destroy any UIDisplay.
//
// Input: None.
//
// Output: None.
//------------------------------------------------------------------------
void ObjAttrController::DestroyUIDisplay()
{
	if (m_pUIController != NULL)
	{
		m_pUIController->RemoveToolTips(m_hParentDlg,m_hInstance,m_hToolTip);
		delete m_pUIController;
		m_pUIController = NULL;
	}
}

//------------------------------------------------------------------------
// ObjAttrController::SaveNonexclusiveKeys
// This method will tell the exclusive controller to save its nonexclusive keys
// to the registry.
//
// Input: None.
//
// Output: None.
//------------------------------------------------------------------------
void ObjAttrController::SaveNonexclusiveKeys()
{
	if (m_pExclusiveController != NULL)
		m_pExclusiveController->SaveKeyValsInReg();
}


//------------------------------------------------------------------------
// ObjAttrController::GetTotalY
// This method will ask the m_pUIController for its total Y spacing that its
// controls take up.
//
// Input: None.
//
// Output: int representing the total amount of y spacing for the controls
//		   in the m_pUIController.  If the controller is NULL, then -1 is
//		   returned.
//------------------------------------------------------------------------
int ObjAttrController::GetTotalY()
{
	if (m_pUIController != NULL)
		return m_pUIController->GetTotalY();
	else
		return -1;
}

//------------------------------------------------------------------------
// ObjAttrController::OkToCopy
// This method will indicate if we are able to copy some keyvalue pairs from a selected face
//
// Input:
//	none
//
// Output: True if we are able to copy to the copy buffer
//------------------------------------------------------------------------
bool ObjAttrController::OkToCopy(void) const
{
	// only able to copy into copy buffer if only one face selected
	if (m_pCurrAttribController != NULL)
	{
		int nNumMeshes = m_pCurrAttribController->GetSize();
		if (nNumMeshes==1)
		{
			return true;
		}
	}
	return false;
}

//------------------------------------------------------------------------
// ObjAttrController::OkToPaste
// This method will indicate if we are able to paste some keyvalue pairs to some selected faces
//
// Input:
//	none
//
// Output: True if we are able to paste from the copy buffer
//------------------------------------------------------------------------
bool ObjAttrController::OkToPaste(void) const
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
// ObjAttrController::CopySelection
// This method will copy the keyvalue pairs from the selected face into the copy buffer
//
// Input:
//	none
//
// Output: True if we are able to paste from the copy buffer
//------------------------------------------------------------------------
bool ObjAttrController::CopySelection(void)
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
			CurrAttribData *pCurrAttribData = m_pCurrAttribController->GetCurrAttribData(0);
			(*m_pCopiedKeyValPairs)=pCurrAttribData->GetKeyValuePairs();
				bResult = true;
		}
	}
	return bResult;
}

//------------------------------------------------------------------------
// ObjAttrController::PasteAttribsToSelSet
// This method will work with the m_pCurrAttribController to copy the attributes
// from the node passed in to the nodes in the current selection set (which
// should be currently stored in the m_pCurrAttribController).
//
// Input:
//  INode* pNode -- The node to get the attributes from.
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
bool ObjAttrController::PasteAttribsToSelSet(bool bReplace)
{
	bool bResult = false;

	if (m_pCurrAttribController != NULL)
	{
		// Check to see if any objects are selected:
		int nNumObjects = m_pCurrAttribController->GetSize();
		if (nNumObjects && m_pCopiedKeyValPairs)
		{
			// set on other selected objects
			for (int nIndex = 0; nIndex < nNumObjects; nIndex++)
			{
				CurrAttribData *pDestAttribData = m_pCurrAttribController->GetCurrAttribData(nIndex);
				// See if the destination node's attributes should be tossed:
				if (bReplace == true)
				{
					pDestAttribData->RemoveAllKeynames();
				}
				// Cycle through all the attribs, copying them (or changing
				// them if the key already exists) on all the dest nodes:
				int nNumAttribs = m_pCopiedKeyValPairs->GetSize();
				for (int nAttribIndex = 0; nAttribIndex < nNumAttribs; nAttribIndex++)
				{
					pDestAttribData->AddKeyValue((*m_pCopiedKeyValPairs)[nAttribIndex]);
				}
            INode* pCurrNode = m_pCurrAttribController->GetMaxNode(nIndex);
            TSTR buf;
            pCurrNode->GetUserPropBuffer(buf);
            char* pChObjectBuf = buf.data();
            TupString strFullData = pChObjectBuf;
            TupString strAttribData = m_pCurrAttribController->CurrObjDataToStr(nIndex);
            AddRawDataToString(strFullData, strAttribData);
            pCurrNode->SetUserPropBuffer((const char *)strFullData);
				bResult = true;
			}
		}
		else
		{
			// No objects in the selection set:
			bResult = false;
		}
	}
	else
	{
		// m_pCurrAttribController is NULL:
		bResult = false;
	}
	return bResult;
}



//------------------------------------------------------------------------
// ObjAttrController::HandleWinMessage
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
bool ObjAttrController::HandleWinMessage(HWND hParent, UINT message, WPARAM wParam, LPARAM lParam)
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
						m_pCurrAttribController->FormCheckListVal(strKeyname, strValue, bChecked);
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
							INode* pCurrNode = m_pCurrAttribController->GetMaxNode(nIndex);
							TSTR buf;
							pCurrNode->GetUserPropBuffer(buf);
							char* pChObjectBuf = buf.data();
							TupString strFullData = pChObjectBuf;
							TupString strAttribData = m_pCurrAttribController->CurrObjDataToStr(nIndex);
							AddRawDataToString(strFullData, strAttribData);
							pCurrNode->SetUserPropBuffer((const char *)strFullData);
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


//-----------------------------------------------------------------------------
// ObjAttrController::RemoveCharsFromStr
// This method will look at the string passed in and remove all instances of 
// the specified character.
//
// Input:
//  TupString strToFix -- The string to remove characters from.
//  char chToRemove -- The character to remove.
//
// Output: True if characters were removed, false otherwise.
//-----------------------------------------------------------------------------
bool ObjAttrController::RemoveCharsFromStr(TupString& strToFix, char chToRemove)
{
	return (strToFix.Remove(chToRemove)>0);
}


//-----------------------------------------------------------------------------
// ObjAttrController::AddCarrRtrnsToStr
// This method will add a carriage return before each linefeed in the
// incoming string.
//
// Input:
//  TupString& strToFix -- The string to add carriage returns to.
//
// Output: True if any carriage returns were added.  False otherwise.
//-----------------------------------------------------------------------------
bool ObjAttrController::AddCarrRtrnsToStr(TupString& strToFix)
{
	return (strToFix.Replace("\n","\r\n")>0);
}

//------------------------------------------------------------------------
// ObjAttrController::StripAllButRawData
// This method will take the incoming string and strip out everything but the
// actual raw attribute data substring.
//
// Input:
//  TupString& strToFix -- String to strip.
//
// Output: True if both of the begin and end block indicators were found, false
//		   otherwise.
//------------------------------------------------------------------------
bool ObjAttrController::StripAllButRawData(TupString& strToFix)
{
	RemoveCharsFromStr(strToFix,'\r');
	// Find and remove the begin block indicator:
	int beginIndex = strToFix.Find(BEGIN_BLOCK);
	if (beginIndex == -1)
	{
		return false;
	}
	int newLineIndex = strToFix.Find('\n',beginIndex);
	if (newLineIndex == -1)
	{
		return false;
	}
	
	// Find and remove the end block indicator:
	int endIndex = strToFix.Find(END_BLOCK,newLineIndex);
	if (endIndex == -1)
	{
		return false;
	}
	TupString tempString(strToFix.Mid(newLineIndex+1,endIndex-(newLineIndex+1)));
	strToFix = tempString;
	return true;
}
	

//------------------------------------------------------------------------
// ObjAttrController::AddRawDataToString
// This method will take the strToInsert and add it into the appropriate place
// in the strToAddTo.  It does this by trying to find the begin/end block
// indicators and replacing any substring that may be between the indicators.
// If no indicators are found, new ones are created at the end of the string,
// with the strToInsert added in between them.
//
// Input:
//  TupString& strToAddTo -- String to insert into.
//  TupString strToInsert -- Sting to add.
//
// Output: None.
//------------------------------------------------------------------------
void ObjAttrController::AddRawDataToString(TupString& strToAddTo,const TupString &strToInsert)
{
	// First get rid of any carriage returns:
	RemoveCharsFromStr(strToAddTo, '\r');
	
	int beginIndex = strToAddTo.Find(BEGIN_BLOCK);
	int endIndex = strToAddTo.Find(END_BLOCK);
	if (beginIndex == -1 && endIndex == -1)
	{
		// Add a newline to the end of the string if needed:
		int nStrSize = strToAddTo.GetLength();
		int newLineIndex = strToAddTo.ReverseFind('\n');
		if (newLineIndex != (nStrSize - 1))
			strToAddTo+="\n";
		
		// Add both of the indicators to the end of the string, with the data between:
		strToAddTo+=BEGIN_BLOCK;
		strToAddTo+="\n";
		strToAddTo+=strToInsert;
		strToAddTo+="\n";
		strToAddTo+=END_BLOCK;
		strToAddTo+="\n";
	}
	else if (beginIndex != -1 && endIndex != -1)
	{
		// Replace any data between the block indicators with the new data:
		int newLineIndex = strToAddTo.Find('\n', beginIndex);
		strToAddTo.Delete(newLineIndex+1, endIndex - (newLineIndex + 1));
		strToAddTo.Insert(newLineIndex+1, strToInsert);
	}
	AddCarrRtrnsToStr(strToAddTo);
}
	

//------------------------------------------------------------------------
// ObjAttrController::InitKeyValPanel
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
void ObjAttrController::InitKeyValPanel(ICustomRollupWindow* pCustomRollupWindow, HINSTANCE hInstance, LPARAM lParam)
{
	if (m_pKeyValComm == NULL)
	{
		m_pKeyValComm = new KeyValCommunicator(this, OBJECT_ATTRIB_KEY);
	}
	m_pKeyValComm->InitKeyValPanel(pCustomRollupWindow, hInstance, lParam);
}

//------------------------------------------------------------------------
// ObjAttrController::IdCancelMessage
// This method will tell the KeyValCommunicator that the IDCANCEL message has
// been received.
//
// Input: None.
//
// Output: None.
//------------------------------------------------------------------------
void ObjAttrController::IdCancelMessage()
{
	if (m_pKeyValComm != NULL)
		m_pKeyValComm->IdCancelMessage();
}


//------------------------------------------------------------------------
// ObjAttrController::SetApplyBttnStates
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
void ObjAttrController::SetApplyBttnStates(bool bApplySelEnabled, bool bApplySceneEnabled)
{
	if (m_pKeyValComm != NULL)
		m_pKeyValComm->SetApplyBttnStates(bApplySelEnabled, bApplySceneEnabled);
}


//------------------------------------------------------------------------
// ObjAttrController::LoadKeyValPanelTree
// This method will tell the KeyValCommunicator to load the key/vals in the
// key/val panel.
//
// Input: None.
//
// Output: None.
//------------------------------------------------------------------------
void ObjAttrController::LoadKeyValPanelTree()
{
	if (m_pKeyValComm != NULL)
		m_pKeyValComm->LoadKeyValPanelTree();
}


//------------------------------------------------------------------------
// ObjAttrController::HandleApplySelect
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
void ObjAttrController::HandleApplySelect(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll)
{
	// If the selection is to be applied to the whole scene, then load the
	// current attrib data for the whole scene:
	if (bApplyToAll == true)
	{
		LoadCurrAttribDataScene();
	}

	// Tell the ObjAttrController to get the current attribute data:
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
		INodeTab nodesToSel;
		int nNumObjects = m_pCurrAttribController->GetSize();
		for (int nObjIndex = 0; nObjIndex < nNumObjects; nObjIndex++)
		{
			CurrAttribData* pCurrAttribData = m_pCurrAttribController->GetCurrAttribData(nObjIndex);
			if (ObjectMatchesCheckedItems(pCurrAttribData, checkedKeyKeeper) == true)
			{
				INode* pNodeToAdd = m_pCurrAttribController->GetMaxNode(nObjIndex);
				nodesToSel.Append(1, &pNodeToAdd);
			}
		}

		// Now change the selection set:
		Interface* ip = GetCOREInterface();
		ip->ClearNodeSelection();
		ip->SelectNodeTab(nodesToSel, TRUE);

#if 0
		// The following code has been compiled out because it only performs an
		// or on the keys specified in the keyval panel.  We wanted it to 
		// perform as an and, which the above code does.  If it's ever deemed
		// necessary to perform an or as well as an and, then the following
		// code can be used.
		// Set up our vector of object indices:
		TupArray<short> vectObjIndices;
		int nNumObjects = m_pCurrAttribController->GetSize();
		for (int nObjIndex = 0; nObjIndex < nNumObjects; nObjIndex++)
		{
			vectObjIndices.Add(0);
		}

		// Cycle through all the keys looking for ones that are checked:
		int nNumKeys = keyKeeper.GetSize();
		for (int nKeyIndex = 0; nKeyIndex < nNumKeys; nKeyIndex++)
		{
			if (keyKeeper[nKeyIndex].m_bChecked == true)
			{
				// Cycle through all the vals for this key to see which
				// are checked:
				bool bValChecked = false;
				int nNumVals = keyKeeper[nKeyIndex].m_valKeeper.GetSize();
				for (int nValIndex = 0; nValIndex < nNumVals; nValIndex++)
				{
					if (keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_bChecked == true)
					{
						// Select objects based on the key/val combo:
						BuildObjIndices(keyKeeper[nKeyIndex].m_strKeyname,
											  keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_strVal,
											  vectObjIndices);
						bValChecked = true;
					}
				}

				if (bValChecked == false)
				{
					// Select objects based on the key only:
					TupString nullVal = "";
					BuildObjIndices(keyKeeper[nKeyIndex].m_strKeyname,
										  nullVal, vectObjIndices);
				}
			}
		}

		// Create the selection set based on the vector of object indices:
		bool bObjsToSelect = false;
		INodeTab nodesToSel;
		int nNumObjs = vectObjIndices.size();
		for (int nLoop = 0; nLoop < nNumObjs; nLoop++)
		{
			if (vectObjIndices.at(nLoop) == 1)
			{
				INode* pNodeToAdd = (INode*) (m_pCurrAttribController->GetObjectPtr(nLoop)->GetEntityPtr());
				nodesToSel.Append(1, &pNodeToAdd);
				bObjsToSelect = true;
			}
		}
		if (bObjsToSelect == true)
		{
			Interface* ip = GetCOREInterface();
			ip->ClearNodeSelection();
			ip->SelectNodeTab(nodesToSel, TRUE);
		}
#endif
	}
}


//------------------------------------------------------------------------
// ObjAttrController::ObjectMatchesCheckedItems
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
bool ObjAttrController::ObjectMatchesCheckedItems(CurrAttribData* pCurrAttribData, Keeper<KeyInfo>& checkedKeyKeeper)
{
	bool bObjMatches = true;

	// Check that this object has each key specified in the keykeeper:
	int nNumKeys = checkedKeyKeeper.GetSize();
	for (int nKeyIndex = 0; nKeyIndex < nNumKeys; nKeyIndex++)
	{
		int nKeynameAt = pCurrAttribData->KeynameExistsAt(checkedKeyKeeper[nKeyIndex].m_strKeyname);
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
				if (pCurrAttribData->GetValueAsString(nKeynameAt) == checkedKeyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_strVal)
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
// ObjAttrController::BuildObjIndices
// This method will look at the key and val strings coming in and see what
// objects in the current attribute controller matches them.  Any that match
// are marked with a 1 at the correct index in the vector of indices.  If the
// val string coming in is null, then only the key is used to find matching
// objects.
//
// Input:
//  TupString strKey -- The key to match against.
//  TupString strVal -- The val to match against.
//  TupArray<short>& vectObjIndices -- The vector of object indices.
//	CurrAttribController* m_pCurrAttribController -- The current attribute data
//												   to look in.
//
// Output: None.
//------------------------------------------------------------------------
void ObjAttrController::BuildObjIndices(TupString strKey, TupString strVal,
										TupArray<short>& vectObjIndices)
{
	// Cycle through the object indices and only look at those not yet marked:
	int nNumObjects = vectObjIndices.GetSize();
	for (int nObjIndex = 0; nObjIndex < nNumObjects; nObjIndex++)
	{
		if (vectObjIndices[nObjIndex] == 0)
		{
			// Check to see if this object matches:
			CurrAttribData* pCurrAttribData = m_pCurrAttribController->GetCurrAttribData(nObjIndex);
			int nKeyIndex = pCurrAttribData->KeynameExistsAt(strKey);
			if (nKeyIndex > -1)
			{
				// Key matches, see if the val should be matched:
				if (strVal != "")
				{
					if (pCurrAttribData->GetValueAsString(nKeyIndex) == strVal)
					{
						// Object matches based on key and val:
						vectObjIndices[nObjIndex] = 1;
					}
				}
				else
				{
					// Object matches based on key only:
					vectObjIndices[nObjIndex] = 1;
				}
			}
		}
	}
}


//------------------------------------------------------------------------
// ObjAttrController::HandleApplyApply
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
void ObjAttrController::HandleApplyApply(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll)
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
			CurrAttribData* pCurrAttribData = m_pCurrAttribController->GetCurrAttribData(nCADataIndex);
			int nNumKeeperKeys = checkedKeyKeeper.GetSize();
			for (int nKeeperKeyIndex = 0; nKeeperKeyIndex < nNumKeeperKeys; nKeeperKeyIndex++)
			{
				pCurrAttribData->AddKeyValue(checkedKeyKeeper[nKeeperKeyIndex].m_strKeyname,
												   checkedKeyKeeper[nKeeperKeyIndex].m_valKeeper[0].m_strVal);
			}
			
			// Now set the data out on the actual node:
			INode* pCurrNode = m_pCurrAttribController->GetMaxNode(nCADataIndex);
			TSTR buf;
			pCurrNode->GetUserPropBuffer(buf);
			char* pChObjectBuf = buf.data();
			TupString strFullData = pChObjectBuf;
			TupString strAttribData = pCurrAttribData->CurrObjDataToStr();
			AddRawDataToString(strFullData, strAttribData);
			pCurrNode->SetUserPropBuffer((const char *)strFullData);
		}

		// Now reset the selection set so that data gets loaded correctly and
		// the dialogs get updated correctly:
		Interface* pInterface = GetCOREInterface();
		int nNumSelNodes = pInterface->GetSelNodeCount();
		if (nNumSelNodes > 0)
		{
			// Cycle through each selected node and add it to our new set of
			// selected nodes:
			INodeTab nodesToSel;
			for (int nNodeIndex = 0; nNodeIndex < nNumSelNodes; nNodeIndex++)
			{
				INode* pCurrNode = pInterface->GetSelNode(nNodeIndex);
				nodesToSel.Append(1, &pCurrNode);
			}
			pInterface->ClearNodeSelection();
			pInterface->SelectNodeTab(nodesToSel, TRUE);
		}
	}
}


//------------------------------------------------------------------------
// ObjAttrController::HandleApplyPurge
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
void ObjAttrController::HandleApplyPurge(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll)
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
			CurrAttribData* pCurrAttribData = m_pCurrAttribController->GetCurrAttribData(nCADataIndex);
			int nNumKeeperKeys = checkedKeyKeeper.GetSize();
			for (int nKeeperKeyIndex = 0; nKeeperKeyIndex < nNumKeeperKeys; nKeeperKeyIndex++)
			{
				// Check to see if this node has this key:
				int nNodeKeyIndex = pCurrAttribData->KeynameExistsAt(checkedKeyKeeper[nKeeperKeyIndex].m_strKeyname);
				if (nNodeKeyIndex > -1)
				{
					if (checkedKeyKeeper[nKeeperKeyIndex].m_bChecked == true)
					{
						// Purge the key from this node:
						pCurrAttribData->RemoveKeyname(checkedKeyKeeper[nKeeperKeyIndex].m_strKeyname);
					}
					else
					{
						// Only purge if this node's val is equal to one of the vals
						// specified to be purged:
						int nNumKeeperVals = checkedKeyKeeper[nKeeperKeyIndex].m_valKeeper.GetSize();
						for (int nKeeperValIndex = 0; nKeeperValIndex < nNumKeeperVals; nKeeperValIndex++)
						{
							if (pCurrAttribData->GetValueAsString(nNodeKeyIndex) == 
								checkedKeyKeeper[nKeeperKeyIndex].m_valKeeper[nKeeperValIndex].m_strVal)
							{
								// Purge the key from this node since it matches the val:
								pCurrAttribData->RemoveKeyname(checkedKeyKeeper[nKeeperKeyIndex].m_strKeyname);
								break;
							}
						}
					}
				}
			}

			// Now set the data out on the actual node:
			INode* pCurrNode = (INode*) m_pCurrAttribController->GetMaxNode(nCADataIndex);
			TSTR buf;
			pCurrNode->GetUserPropBuffer(buf);
			char* pChObjectBuf = buf.data();
			TupString strFullData = pChObjectBuf;
			TupString strAttribData = pCurrAttribData->CurrObjDataToStr();
			AddRawDataToString(strFullData, strAttribData);
			pCurrNode->SetUserPropBuffer((const char *)strFullData);
		}

		// Now reset the selection set so that data gets loaded correctly and
		// the dialogs get updated correctly:
		Interface* pInterface = GetCOREInterface();
		int nNumSelNodes = pInterface->GetSelNodeCount();
		if (nNumSelNodes > 0)
		{
			// Cycle through each selected node and add it to our new set of
			// selected nodes:
			INodeTab nodesToSel;
			for (int nNodeIndex = 0; nNodeIndex < nNumSelNodes; nNodeIndex++)
			{
				INode* pCurrNode = pInterface->GetSelNode(nNodeIndex);
				nodesToSel.Append(1, &pCurrNode);
			}
			pInterface->ClearNodeSelection();
			pInterface->SelectNodeTab(nodesToSel, TRUE);
		}
	}
}

//------------------------------------------------------------------------
// ObjAttrController::HandleApplyRemap
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
void ObjAttrController::HandleApplyRemap(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll)
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
			CurrAttribData* pCurrAttribData = m_pCurrAttribController->GetCurrAttribData(nCADataIndex);
			int nNumKeeperKeys = checkedKeyKeeper.GetSize();
			for (int nKeeperKeyIndex = 0; nKeeperKeyIndex < nNumKeeperKeys; nKeeperKeyIndex++)
			{
				// Check to see if this node has this key:
				int nNodeKeyIndex = pCurrAttribData->KeynameExistsAt(checkedKeyKeeper[nKeeperKeyIndex].m_strKeyname);
				if (nNodeKeyIndex > -1)
				{
					bool bRemapKey = false;
					bool bRemapVal = false;
					TupString strOrigKey = checkedKeyKeeper[nKeeperKeyIndex].m_strKeyname;
					TupString strNewKey = "";
					TupString strOrigVal = "";
					TupString strNewVal = "";

					// First check to see if the val for this key should be remapped:
					int nNumKeeperVals = checkedKeyKeeper[nKeeperKeyIndex].m_valKeeper.GetSize();
					for (int nKeeperValIndex = 0; nKeeperValIndex < nNumKeeperVals; nKeeperValIndex++)
					{
						strOrigVal = pCurrAttribData->GetValueAsString(nNodeKeyIndex);
						if ((checkedKeyKeeper[nKeeperKeyIndex].m_valKeeper[nKeeperValIndex].m_bChecked == true) &&
							(strOrigVal == checkedKeyKeeper[nKeeperKeyIndex].m_valKeeper[nKeeperValIndex].m_strVal))
						{
							strNewVal = checkedKeyKeeper[nKeeperKeyIndex].m_valKeeper[nKeeperValIndex].m_strNewVal;
							bRemapVal = true;
							// At this point, there will only be one val that will match.
							// Since we found it, we can break:
							break;
						}
					}

					// Now check to see if this key should be remapped:
					if (checkedKeyKeeper[nKeeperKeyIndex].m_bChecked == true)
					{
						strNewKey = checkedKeyKeeper[nKeeperKeyIndex].m_strNewKeyname;
						bRemapKey = true;
					}

					// Now do the remap, if necessary:
					if (bRemapKey == true)
					{
						if (bRemapVal == true)
						{
							// Remap the key and remap the val:
							pCurrAttribData->RemoveKeyname(strOrigKey);
							pCurrAttribData->AddKeyValue(strNewKey, strNewVal);
						}
						else
						{
							// Remap the key, don't remap the val:
							pCurrAttribData->RemoveKeyname(strOrigKey);
							pCurrAttribData->AddKeyValue(strNewKey, strOrigVal);
						}
					}
					else
					{
						if (bRemapVal == true)
						{
							// Don't remap the key, remap the val:
							pCurrAttribData->AddKeyValue(strOrigKey, strNewVal);
						}
						else
						{
							// Don't remap the key, don't remap the val:
							;
						}
					}
				}
			}

			// Now set the data out on the actual node:
			INode* pCurrNode = m_pCurrAttribController->GetMaxNode(nCADataIndex);
			TSTR buf;
			pCurrNode->GetUserPropBuffer(buf);
			char* pChObjectBuf = buf.data();
			TupString strFullData = pChObjectBuf;
			TupString strAttribData = pCurrAttribData->CurrObjDataToStr();
			AddRawDataToString(strFullData, strAttribData);
			pCurrNode->SetUserPropBuffer((const char *)strFullData);
		}

		// Now reset the selection set so that data gets loaded correctly and
		// the dialogs get updated correctly:
		Interface* pInterface = GetCOREInterface();
		int nNumSelNodes = pInterface->GetSelNodeCount();
		if (nNumSelNodes > 0)
		{
			// Cycle through each selected node and add it to our new set of
			// selected nodes:
			INodeTab nodesToSel;
			for (int nNodeIndex = 0; nNodeIndex < nNumSelNodes; nNodeIndex++)
			{
				INode* pCurrNode = pInterface->GetSelNode(nNodeIndex);
				nodesToSel.Append(1, &pCurrNode);
			}
			pInterface->ClearNodeSelection();
			pInterface->SelectNodeTab(nodesToSel, TRUE);
		}
	}
}
