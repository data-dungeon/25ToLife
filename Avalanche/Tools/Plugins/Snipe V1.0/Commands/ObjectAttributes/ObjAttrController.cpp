//------------------------------------------------------------------------
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
//------------------------------------------------------------------------



//------------------------------------------------------------------------
// Includes:
#include "StdAfx.h"
#include "ObjAttrController.h"
#include "CurrAttribData.h"
#include "CurrAttribController.h"
#include "KeyValCommunicator.h"
#include "ConfigFilesManager.h"
#include "ObjectAttributes.h"

//------------------------------------------------------------------------
// Defines, etc.:

//------------------------------------------------------------------------
// Class Definition:

//------------------------------------------------------------------------
// ObjAttrController::ObjAttrController
// This method is the default constructor for this class.
//
// Input: None
//
// Output: None
//------------------------------------------------------------------------
ObjAttrController::ObjAttrController()
{
	m_pConfigRoot = NULL;
	m_pCurrAttribController = NULL;
	m_pUIController = NULL;
	m_pExclusiveController = NULL;
	m_pKeyValComm = NULL;
	m_pCopiedKeyValPairs = NULL;
	m_pCheckListBoxKeys = NULL;
}


//------------------------------------------------------------------------
// ObjAttrController::~ObjAttrController
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//------------------------------------------------------------------------
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
	if (m_pCheckListBoxKeys != NULL)
	{
		delete m_pCheckListBoxKeys;
		m_pCheckListBoxKeys = NULL;
	}
}


//------------------------------------------------------------------------
// ObjAttrController::GetConfigRoot
// This method is an accessor for the GetConfigRoot data member.
//
// Input: None.
//
// Output: Pointer to the m_pConfigRoot.
//------------------------------------------------------------------------
ConfigRoot* ObjAttrController::GetConfigRoot()
{
	return m_pConfigRoot;
}

//------------------------------------------------------------------------
// ObjAttrController::GetCurrAttribController
// This method is an accessor for the CurrAttribController data member.
//
// Input: None.
//
// Output: Pointer to the CurrAttribController.
//------------------------------------------------------------------------
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

	// set up list of check list keys
	if (m_pCheckListBoxKeys != NULL)
	{
		delete m_pCheckListBoxKeys;
		m_pCheckListBoxKeys = NULL;
	}
	m_pCheckListBoxKeys = new Keeper<TupString>();
	m_pConfigRoot->FindConfigKeys(*m_pCheckListBoxKeys,TupString("ConfigCheckList"));

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
// ObjAttrController::LoadCurrAttribData
// This method will look at the incoming Interface pointer and get the
// selection set from it.  If there is anything selected, it will then create
// the m_pCurrAttribController (if needed), and create any necessary
// CurrAttribData objects for it and load the data into them.  If there are any
// CurrAttribData objects already corresponding to items in the selection set,
// they are not touched.
//
// Input: none
//
// Output: True if there was nodes in the selection set, false otherwise.
//------------------------------------------------------------------------
bool ObjAttrController::LoadCurrAttribData()
{
	HMainInterface *pMI = GetHMainInterface();

	HSelectionList *pSelectionList = pMI->GetCurrentSelectionList();
	HSnipeObjectList *pObjectList = pSelectionList->CreateSnipeObjectList();
	bool result = LoadCurrAttribData(pObjectList);
	delete pObjectList;
	return result;
}

//------------------------------------------------------------------------
// ObjAttrController::LoadCurrAttribData
// This method will look at the incoming Interface pointer and get the
// selection set from it.  If there is anything selected, it will then create
// the m_pCurrAttribController (if needed), and create any necessary
// CurrAttribData objects for it and load the data into them.  If there are any
// CurrAttribData objects already corresponding to items in the selection set,
// they are not touched.
//
// Input: const MSelectionList &slist - list of objects to search through
//
// Output: True if there was nodes in the selection set, false otherwise.
//
bool ObjAttrController::LoadCurrAttribData(HSnipeObjectList *pSelectionList)
{

	// Get rid of any current attribute data that may exist:
	if (m_pCurrAttribController != NULL)
	{
		m_pCurrAttribController->RemoveAll();
	}
	else
	{
		m_pCurrAttribController = new CurrAttribController();
	}

	for (POSITION pos = pSelectionList->GetHeadPosition(); pos; )
	{
		HSnipeObject *pObject = pSelectionList->GetNext(pos);

		if (CurrAttribController::ValidNode(pObject))
		{
			int objectIndex = m_pCurrAttribController->GetIndex(pObject);
			// if this one is not in the controller already
			if (objectIndex==-1)
			{
				CurrAttribData* pNewAttribData = new CurrAttribData();
			
				// Set the object pointer:
				objectIndex = m_pCurrAttribController->AddSnipeObject(pObject);

				TupString strFullData;
				CurrAttribController::GetNote(pObject,strFullData);
				
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
	}
	return (m_pCurrAttribController->GetSize()!=0);
}


//------------------------------------------------------------------------
// ObjAttrController::LoadCurrAttribDataScene
// This method will create a new m_pCurrAttribController and create any
// necessary CurrAttribData objects for all the objects in the scene and load
// the data into them.  This method makes sure that the whole node tree is
// walked through.
//
// Input: None.
//
// Output: True if there are nodes in the scene, false otherwise.
//------------------------------------------------------------------------
bool ObjAttrController::LoadCurrAttribDataScene()
{
	bool bFoundNode = false;

   HMainInterface *pMI = GetHMainInterface();
   HHierObject *pScene = pMI->GetCurrentCommandObject();
   int numChildren = pScene->GetNumChildren();
   for (int childIndex = 0; childIndex < numChildren; childIndex++)
   {
      HSnipeObject *pChildObject = pScene->GetChildAt(childIndex);
      if (CurrAttribController::ValidNode(pChildObject))
      {
         if (!bFoundNode)
         {
            bFoundNode = true;
            // At least one node exists, so reset the m_pCurrAttribController to
            // prepare it for new attrib data:
            if (m_pCurrAttribController != NULL)
               m_pCurrAttribController->RemoveAll();
            else
               m_pCurrAttribController = new CurrAttribController();
         }
         // Now load the data in for the children of the root (which in turn loads the data
         // in recursively for any of its children):
         m_pCurrAttribController->LoadCurrAttribDataNode(pChildObject);
      }
   }
	return bFoundNode;
}

//------------------------------------------------------------------------
// ObjAttrController::SetUpUIDisplay
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
				::SendMessage(hParentDlg,WM_SETREDRAW,FALSE,0);
				bResult = m_pUIController->BuildActualUI(hParentDlg, hInstance, hToolTip,
														 nStartx, nStarty,
														 nLabelWidth, nLabelHeight,
														 nSpaceBetweenCtrls);
				::SendMessage(hParentDlg,WM_SETREDRAW,TRUE,0);
				::RedrawWindow(hParentDlg,NULL,NULL, RDW_UPDATENOW|RDW_ERASE|RDW_INVALIDATE|RDW_ALLCHILDREN);
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
			String command(kSetFlag);
			if (bReplace)
			{
				command += " ";
				command += kReplaceAllValuesFlag;
			}
			int nNumAttribs = m_pCopiedKeyValPairs->GetSize();
			for (int nAttribIndex = 0; nAttribIndex < nNumAttribs; nAttribIndex++)
			{
				// checklistbox key - multiple values instead of 1
				if (m_pCheckListBoxKeys->GetIndex(((*m_pCopiedKeyValPairs)[nAttribIndex]).m_strKey)!=-1)
				{
					Keeper<TupString> keyValues;
					GetCheckListBoxValues(((*m_pCopiedKeyValPairs)[nAttribIndex]).m_strValue,keyValues);
					for (int valueIndex = 0; valueIndex < keyValues.GetSize(); valueIndex++)
					{
						command += " ";
						command += kKeyValueFlag;
						command += " \"";
						command += (const char *)((*m_pCopiedKeyValPairs)[nAttribIndex]).m_strKey;
						command += "\" \"";
						command += (const char *)keyValues[valueIndex];
						command += "\"";
					}
				}
				else // normal - one value only
				{
					command += " ";
					command += kKeyValueFlag;
					command += " \"";
					command += (const char *)FixKeyString(((*m_pCopiedKeyValPairs)[nAttribIndex]).m_strKey);
					command += "\" \"";
					command += (const char *)FixValueString(((*m_pCopiedKeyValPairs)[nAttribIndex]).m_strValue);
					command += "\"";
				}
				bResult = true;
			}
			if (bResult)
			{
				StringList result;
				ExecuteCommand(command,result);
				int totalAffected = 0;
				if (result.GetSize() > 0)
				{
					totalAffected = result.GetHead().AsInt();
				}
				TupString stats;
				stats.Format("%d Changed",totalAffected);
				UpdateStatsText(stats);
				return true;
			}
		}
	}
	return false;
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
			// show the find dialog
			if (nFlags & UIDisplayBase::WIN_MESSAGE_FLAG_FIND_DIALOG)
			{
				m_pKeyValComm->ShowFindDialog(strKeyname,strValue);
				bResult = false;
			}
			else if (nFlags & UIDisplayBase::WIN_MESSAGE_FLAG_EXCLUSIVE)
			{
				// Alter the current object data:
				if (m_pCurrAttribController != NULL)
				{
					bool bUpdateData = false;

					String command(kSetFlag);

					if (nFlags & UIDisplayBase::WIN_MESSAGE_FLAG_PURGE_KEYS)
					{
						int nNumPurgeKeys = vectKeysToPurge.GetSize();
						for (int nLoop = 0; nLoop < nNumPurgeKeys; nLoop++)
						{
							command += " ";
							command += kPurgeKeyOnSetFlag;
							command += " \"";
							command += (const char *)vectKeysToPurge[nLoop];
							command += "\"";
							bUpdateData = true;
						}
					}

					// set key value pair
					if (nFlags & UIDisplayBase::WIN_MESSAGE_FLAG_SET_KEY_VALUE)
					{
						command += " ";
						command += kKeyValueFlag;
						command += " \"";
						command += (const char *)FixKeyString(strKeyname);
						command += "\" \"";
						command += (const char *)FixValueString(strValue);
						command += "\"";
						bUpdateData = true;
					}

					// check list
					if (nFlags & UIDisplayBase::WIN_MESSAGE_FLAG_FORM_CHECK_LIST)
					{
						command += " ";
						command += kKeyValueFlag;
						command += " \"";
						command += (const char *)FixKeyString(strKeyname);
						command += "\" \"";
						command += (const char *)FixValueString(strValue);
						command += "\"";
						bUpdateData = true;

						bool bChecked = (nFlags & UIDisplayBase::WIN_MESSAGE_FLAG_CHECK_LIST_CHECKED) ? true : false;
						if (!bChecked)
						{
							command += " ";
							command += kRemoveValueOnSetFlag;
						}
						bUpdateData = true;
					}

					if (bUpdateData)
					{
						StringList result;
						ExecuteCommand(command,result);
						int totalAffected = 0;
						if (result.GetSize() > 0)
						{
							totalAffected = result.GetHead().AsInt();
						}
						TupString stats;
						stats.Format("%d Changed",totalAffected);
						UpdateStatsText(stats);
						return true;
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
	// attempt to gracefully get to just carriage returns
	strToFix.Replace("\r\n","\n");
	strToFix.Replace("\n\r","\n");
	strToFix.Replace("\r","\n");

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
	// attempt to gracefully get to just carriage returns
	strToAddTo.Replace("\r\n","\n");
	strToAddTo.Replace("\n\r","\n");
	strToAddTo.Replace("\r","\n");
	
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
}

TupString ObjAttrController::FixKeyString(const char *pStringToFix)
{
	TupString value(pStringToFix);
//	value.Replace("\\","\\\\"); // make backslashes double backslashes
	return value;
}

TupString ObjAttrController::FixValueString(const char *pStringToFix)
{
	TupString value(pStringToFix);
//	value.Replace("\\","\\\\"); // make backslashes double backslashes
	return value;
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
// Output: int -- total number of selected nodes
//------------------------------------------------------------------------
int ObjAttrController::HandleApplySelect(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll)
{
	String command(kFindFlag);
	command += " ";
	command += kSelectFlag;
	command += " ";
	command += kMatchCaseFlag;
	command += " ";
	command += kMatchWholeWordFlag;
	command += " ";
	command += kSearchDefaultFlag;
	if (bApplyToAll)
	{
		command += " ";
		command += kApplySceneFlag;
	}
	int numKeys = keyKeeper.GetSize();
	for (int keyIndex = 0; keyIndex < numKeys; keyIndex++)
	{
		if (!keyKeeper[keyIndex].m_bVisible)
			continue;

		// Check to see if this key is checked:
		if (keyKeeper[keyIndex].m_bChecked == true)
		{
			bool bAddedValue = false;
			int numVals = keyKeeper[keyIndex].m_valKeeper.GetSize();
			for (int valueIndex = 0; valueIndex < numVals; valueIndex++)
			{
				if (!keyKeeper[keyIndex].m_valKeeper[valueIndex].m_bVisible)
					continue;

				if (keyKeeper[keyIndex].m_valKeeper[valueIndex].m_bChecked)
				{
					command += " ";
					command += kKeyValueFlag;
					command += " \"";
					command += (const char *)FixKeyString(keyKeeper[keyIndex].m_strKeyname);
					command += "\" \"";
					command += (const char *)FixValueString(keyKeeper[keyIndex].m_valKeeper[valueIndex].m_strVal);
					command += "\"";
					bAddedValue = true;
				}
			}
			if (!bAddedValue)
			{
				command += " ";
				command += kKeyFlag;
				command += " \"";
				command += (const char *)FixKeyString(keyKeeper[keyIndex].m_strKeyname);
				command += "\"";
			}
		}
	}
	StringList result;
	ExecuteCommand(command,result);
	if (result.GetSize() > 0)
	{
		return result.GetHead().AsInt();
	}
	return 0;
}

//-----------------------------------------------------------------------------
// ObjAttrController::HandleFind
// This method will be select nodes that match the parameters
//
// Input:
// const KeyValueFind & -- criteria to search for
// bool bCurrentSelection -- true = search through selection false = search entire scene
// bool bAppendSelection -- if true found items are appended to current selection
//
// Output: int -- total number found
//-----------------------------------------------------------------------------
int ObjAttrController::HandleFind(const KeyValueFind &keyValueFind, bool bCurrentSelection, bool bAppendSelection)
{
	String command(kFindFlag);
	command += " ";
	command += kSelectFlag;
	if (keyValueFind.m_bMatchCase)
	{
		command += " ";
		command += kMatchCaseFlag;
	}
	if (keyValueFind.m_bMatchWholeWord)
	{
		command += " ";
		command += kMatchWholeWordFlag;
	}
	if (keyValueFind.m_bSearchDefaults)
	{
		command += " ";
		command += kSearchDefaultFlag;
	}
	if (keyValueFind.m_bSearchUnused)
	{
		command += " ";
		command += kSearchUnusedFlag;
	}
	if (!bCurrentSelection)
	{
		command += " ";
		command += kApplySceneFlag;
	}
	if (bAppendSelection)
	{
		command += " ";
		command += kAppendSelectionFlag;
	}
	if (keyValueFind.m_bUseKey)
	{
		if (keyValueFind.m_bUseValue)
		{
			command += " ";
			command += kKeyValueFlag;
			command += " \"";
			command += (const char *)FixKeyString(keyValueFind.m_keyName);
			command += "\" \"";
			command += (const char *)FixValueString(keyValueFind.m_valueName);
			command += "\"";
		}
		else
		{
			command += " ";
			command += kKeyFlag;
			command += " \"";
			command += (const char *)FixKeyString(keyValueFind.m_keyName);
			command += "\"";
		}
	}
	else if (keyValueFind.m_bUseValue)
	{
		command += " ";
		command += kValueFlag;
		command += " \"";
		command += (const char *)FixValueString(keyValueFind.m_valueName);
		command += "\"";
	}

	StringList result;
	ExecuteCommand(command,result);
	if (result.GetSize() > 0)
	{
		return result.GetHead().AsInt();
	}
	return 0;
}

//-----------------------------------------------------------------------------
// ObjAttrController::HandleFind
// This method will be select nodes that match the parameters
//
// Input:
// const CommandArgs &commandArgs -- arguments
// TupArray<TupString>& resultStrings -- result of command
// UndoRedoState &undoRedoState -- data to set up for undo/redo
//
// Output: bool - result of command
//-----------------------------------------------------------------------------
bool ObjAttrController::HandleFindCommand(const CommandArgs &commandArgs, TupArray<TupString> &resultStrings, UndoRedoState &undoRedoState)
{
	HMainInterface *pMI = GetHMainInterface();

	int totalFound = 0;
	// If the selection is to be applied to the whole scene, then load the
	// current attrib data for the whole scene:
	Keeper<HSnipeObject *> previousSelectionObjects;
	if (commandArgs.m_bApplyScene)
	{
		// if we are appending then we need to remember the previous selection to add these regardless
		if (commandArgs.m_bAppendSelection)
		{
			int nNumObjects = m_pCurrAttribController->GetSize();
			for (int nObjIndex = 0; nObjIndex < nNumObjects; nObjIndex++)
			{
				previousSelectionObjects.Add(m_pCurrAttribController->GetSnipeObject(nObjIndex));
			}
		}
		LoadCurrAttribDataScene();
	}

	// Tell the ObjAttrController to get the current attribute data:
	if (m_pCurrAttribController != NULL)
	{
		if (commandArgs.m_bSelect)
		{
			HSnipeObjectList *pNewSelection = HSnipeObjectList::New();
			int nNumObjects = m_pCurrAttribController->GetSize();
			for (int nObjIndex = 0; nObjIndex < nNumObjects; nObjIndex++)
			{
				HSnipeObject *pObject = m_pCurrAttribController->GetSnipeObject(nObjIndex);
				CurrAttribData* pCurrAttribData = m_pCurrAttribController->GetCurrAttribData(nObjIndex);

				Keeper<KeyInfo> contextKeyInfo;
				m_pKeyValComm->AddKeyInfoData(m_pCurrAttribController,pCurrAttribData,contextKeyInfo);

				if (previousSelectionObjects.GetIndex(pObject)!=-1 || ObjectMatchesFindCriteria(contextKeyInfo,commandArgs.m_keyValueKeeper,commandArgs.m_valueKeeper,commandArgs.m_bMatchCase,commandArgs.m_bMatchWholeWord,commandArgs.m_bSearchDefaults,commandArgs.m_bSearchUnused))
				{
					pNewSelection->AddTail(pObject);
					totalFound++;
				}
			}
			if (totalFound)
			{
				undoRedoState.m_bSelectionChanged = true;
				undoRedoState.m_pNewFocusObject = pNewSelection->GetHead();
				undoRedoState.m_pNewSelectedObjects = pNewSelection;

				undoRedoState.m_pPreviousFocusObject = HSelectionList::GetFocusObject();
				undoRedoState.m_pPreviousNearestFocusObject = HSelectionList::GetNearestFocusObject();

				HSelectionList *pCurrentSelectionList = pMI->GetCurrentSelectionList();

				undoRedoState.m_pPreviousSelectedObjects = pCurrentSelectionList->CreateSnipeObjectList();
			}
			else
			{
				delete pNewSelection;
			}
			char pChValue[8];
			itoa(totalFound, pChValue, 10);
			resultStrings.Add(pChValue);
			return true;
		}
		else // don't select just put out a list of objects
		{
			int nNumObjects = m_pCurrAttribController->GetSize();
			for (int nObjIndex = 0; nObjIndex < nNumObjects; nObjIndex++)
			{
				HSnipeObject *pObject = m_pCurrAttribController->GetSnipeObject(nObjIndex);
				CurrAttribData* pCurrAttribData = m_pCurrAttribController->GetCurrAttribData(nObjIndex);

				Keeper<KeyInfo> contextKeyInfo;
				m_pKeyValComm->AddKeyInfoData(m_pCurrAttribController,pCurrAttribData,contextKeyInfo);

				if (ObjectMatchesFindCriteria(contextKeyInfo,commandArgs.m_keyValueKeeper,commandArgs.m_valueKeeper,commandArgs.m_bMatchCase,commandArgs.m_bMatchWholeWord,commandArgs.m_bSearchDefaults,commandArgs.m_bSearchUnused))
				{
					String description;
					pMI->ObjectToDescription(pObject,description,false);
					resultStrings.Add(description.Get());
				}
			}
			return true;
		}
	}

	// output 0 since no found nodes
	resultStrings.Add(TupString("0"));

	return true;
}

//------------------------------------------------------------------------
// ObjAttrController::ObjectMatchesFindCriteria
// This method will check the object represented by the incoming CurrAttribData
// pointer to see if the keys and values it has matches the ones checked in the
// incoming keykeeper.
//
// Input:
//				const Keeper<KeyInfo> &contextKeyValues -- key values for object (all including default and unused)
//				const Keeper<KeyInfo> &keyValueKeeper -- keys to search for
//				const Keeper<ValInfo> &valueKeeper -- values to search for
//				bool bMatchCase -- value must match case
//				bool bMatchWholeWord -- value must match whole word
//				bool bSearchDefaults -- search default keys
//				bool bSearchUnused -- search unused keys
//
// Output: True if the object matches, false otherwise.
//------------------------------------------------------------------------
bool ObjAttrController::ObjectMatchesFindCriteria(const Keeper<KeyInfo> &contextKeyValues, 
																  const Keeper<KeyInfo> &keyValueKeeper,
																  const Keeper<ValInfo> &valueKeeper,
																  bool bMatchCase,bool bMatchWholeWord,
																  bool bSearchDefaults, bool bSearchUnused)
{
	int numKeeperKeys = keyValueKeeper.GetSize();
	for (int keyKeeperIndex = 0; keyKeeperIndex < numKeeperKeys; keyKeeperIndex++)
	{
		const KeyInfo &keyInfo = keyValueKeeper[keyKeeperIndex];

		int contextKeyIndex = contextKeyValues.GetIndex(keyInfo);
		if (contextKeyIndex == -1)
			return false;

		const KeyInfo &contextKeyInfo = contextKeyValues[contextKeyIndex];
		// make sure we enable key to be searched
		if (contextKeyInfo.m_activeCount == 0 &&
			 (contextKeyInfo.m_defaultCount == 0 || !bSearchDefaults) &&
			 (contextKeyInfo.m_unusedCount == 0 || !bSearchUnused))
			 return false;
		
		int numKeeperValues = keyInfo.m_valKeeper.GetSize();
		if (numKeeperValues)
		{
			bool bFoundValue = false;
			for (int valueKeeperIndex = 0; valueKeeperIndex < numKeeperValues; valueKeeperIndex++)
			{
				if (ValueMatchesFindCriteria(contextKeyValues,contextKeyIndex,keyInfo.m_valKeeper[valueKeeperIndex].m_strVal,bMatchCase,bMatchWholeWord,bSearchDefaults,bSearchUnused))
				{
					bFoundValue = true;
					break;
				}
			}
			// if no value matched we are done
			if (!bFoundValue)
				return false;
		}
	}

	int numKeeperValues = valueKeeper.GetSize();
	for (int valueKeeperIndex = 0; valueKeeperIndex < numKeeperValues; valueKeeperIndex++)
	{
		const ValInfo &valInfo = valueKeeper[valueKeeperIndex];

		int numContextKeys = contextKeyValues.GetSize();
		bool bFoundValue = false;
		for (int contextKeyIndex = 0; contextKeyIndex < numContextKeys; contextKeyIndex++)
		{
			const KeyInfo &contextKeyInfo = contextKeyValues[contextKeyIndex];
			// make sure we enable key to be searched
			if (contextKeyInfo.m_activeCount == 0 &&
				(contextKeyInfo.m_defaultCount == 0 || !bSearchDefaults) &&
				(contextKeyInfo.m_unusedCount == 0 || !bSearchUnused))
				continue;

			// see if any key on this object matches this value
			if (ValueMatchesFindCriteria(contextKeyValues,contextKeyIndex,valInfo.m_strVal,bMatchCase,bMatchWholeWord,bSearchDefaults,bSearchUnused))
			{
				bFoundValue = true;
				break;
			}
		}
		if (!bFoundValue)
			return false;
	}
	// all passed
	return true;
}

//------------------------------------------------------------------------
// ObjAttrController::ValueMatchesFindCriteria
// This method will check the object represented by the incoming CurrAttribData
// pointer to see if the keys and values it has matches the ones checked in the
// incoming keykeeper.
//
// Input:
//				const Keeper<KeyInfo> &contextKeyValues -- key values for object (all including default and unused)
//				int nKeyIndex -- index into contextKeyValues to look
//				const TupString &matchValue -- value string to look for
//				bool bMatchCase -- value must match case
//				bool bMatchWholeWord -- value must match whole word
//				bool bSearchDefaults -- search default keys
//				bool bSearchUnused -- search unused keys
//
// Output: True if the object matches, false otherwise.
//------------------------------------------------------------------------
bool ObjAttrController::ValueMatchesFindCriteria(const Keeper<KeyInfo> &contextKeyValues, int nKeyIndex, const TupString &matchValue, bool bMatchCase, bool bMatchWholeWord, bool bSearchDefaults, bool bSearchUnused)
{
	const KeyInfo &sourceKeyInfo = contextKeyValues[nKeyIndex];
	const TupString &keyString = sourceKeyInfo.m_strKeyname;

	for (int valueIndex = 0; valueIndex < sourceKeyInfo.m_valKeeper.GetSize(); valueIndex++)
	{
		const ValInfo &sourceValInfo = sourceKeyInfo.m_valKeeper[valueIndex];
		if (sourceValInfo.m_activeCount == 0 &&
			(sourceValInfo.m_defaultCount == 0 || !bSearchDefaults) &&
			(sourceValInfo.m_unusedCount == 0 || !bSearchUnused))
			continue;

		if (MatchSplitString(matchValue,",",sourceValInfo.m_strVal,bMatchCase,bMatchWholeWord))
		{
			return true;
		}
	}
	return false;
}


//------------------------------------------------------------------------
// ObjAttrController::UpdateStatsText
// This method get the sub values from a check list box value
//
// Input:
//  const TupString &statsLine -- text to show
//
// Output: none
//------------------------------------------------------------------------
void ObjAttrController::UpdateStatsText(const TupString &statsLine)
{
	SetWindowText(theObjectAttributes.m_hStatusStaticText,statsLine);
}

void ObjAttrController::ExecuteCommand(const String &parameters,StringList &result)
{
	result.RemoveAll();
	HMainInterface *pMI = GetHMainInterface();
	if (pMI)
	{
		String strCommand("ObjectAttributes");
		pMI->DoCommand(strCommand,parameters,result);
	}
}


//------------------------------------------------------------------------
// ObjAttrController::IsCheckListKey
// This method get the sub values from a check list box value
//
// Input:
//  const TupString &keyString -- key to check
//
// Output: True if the string matches a key in the checklist box key list
//------------------------------------------------------------------------
bool ObjAttrController::IsCheckListKey(const TupString &keyString)
{
	return (m_pCheckListBoxKeys->GetIndex(keyString)!=-1);
}

//------------------------------------------------------------------------
// ObjAttrController::BuildCheckListBoxValue
// This method get the sub values from a check list box value
//
// Input:
//  const Keeper<TupString> &keyValues -- values to add
//
// Output: string representing values separated by commas
//------------------------------------------------------------------------
TupString ObjAttrController::BuildCheckListBoxValue(const Keeper<TupString> &keyValues)
{
	TupString newString;
	for (int index = 0; index < keyValues.GetSize(); index++)
	{
		newString += keyValues[index] + ",";
	}
	return newString;
}

//------------------------------------------------------------------------
// ObjAttrController::GetCheckListBoxValues
// This method get the sub values from a check list box value
//
// Input:
//  const char *stringToSplit -- String to split up
//  Keeper<TupString> &keyValues -- Values to add to 
//
// Output: none
//------------------------------------------------------------------------
void ObjAttrController::GetCheckListBoxValues(const char *pValueString,Keeper<TupString> &keyValues)
{
	TupString sourceString(pValueString);
	int separatorIndex = sourceString.Find(';');
	if (separatorIndex != -1)
		sourceString = sourceString.Left(separatorIndex);
   if (sourceString.GetLength())
   {
      SplitString(sourceString,",",keyValues);
   }
   // leave alone if string empty
}

//------------------------------------------------------------------------
// ObjAttrController::MatchSplitString
// This method attempt to split the string using a token and see if it matches matchstring
//
// Input:
//  const char *stringToSplit -- String to split up
//  const char *pTokenString -- Token to split up the string with
//  const char *pMatchString -- String to check for match with
//  bool bMatchCase -- true if matching case
//  bool bMatchWholeWord -- true if matching whole word
//
// Output: True if the string matches, false otherwise.
//------------------------------------------------------------------------
bool ObjAttrController::MatchSplitString(const char *pStringToSplit, const char *pTokenString, const char *pMatchString, bool bMatchCase, bool bMatchWholeWord)
{
	int matchStringLength = strlen(pMatchString);

	Keeper<TupString> findValues;
	SplitString(pStringToSplit,pTokenString,findValues);

	for (int findIndex = 0;findIndex < findValues.GetSize(); findIndex++)
	{
		TupString &findString = findValues[findIndex];
		// if matching against empty strings
		if (matchStringLength == 0 && findString.GetLength() == 0)
		{
			return true;
		}
		// make sure both string have length
		else if (matchStringLength > 0 && findString.GetLength() > 0)
		{
			if(bMatchWholeWord)
			{
				if (bMatchCase)
				{
					if (strcmp(pMatchString,findString)==0)
					{
						return true;
					}
				}
				else
				{
					if (stricmp(pMatchString,findString)==0)
					{
						return true;
					}
				}
			}
			else
			{
				if (bMatchCase)
				{
					if (strstr(pMatchString,findString)!=NULL)
					{
						return true;
					}
				}
				else
				{
					TupString lowerValueString(pMatchString);
					lowerValueString.MakeLower();

					TupString lowerValueName(findString);
					lowerValueName.MakeLower();

					if (lowerValueString.Find(lowerValueName)!=-1)
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}

//------------------------------------------------------------------------
// ObjAttrController::SplitString
// This method attempt to split the string using a token and see if it matches matchstring
//
// Input:
//  const char *pSourceString -- String to split up
//  const char *pTokenString -- Token to split up the string with
//  Keeper<TupString> &destArray -- Keeper to fill with substrings
//
// Output: none
//------------------------------------------------------------------------
void ObjAttrController::SplitString(const char *pSourceString,const char *pTokenString,Keeper<TupString> &destArray)
{
	int tokenStringLength = strlen(pTokenString);
	TupString sourceString(pSourceString);
	while (sourceString.GetLength())
	{
		int foundIndex = sourceString.Find(pTokenString);
		// if we found the token
		if (foundIndex!=-1)
		{
			TupString subString = sourceString.Left(foundIndex);
			destArray.Add(subString);
			sourceString.Delete(0,foundIndex+tokenStringLength); // get rid of text to the left of the token including the token
		}
		else
		{
			destArray.Add(sourceString); // add what is left since no tokens found
			sourceString.Empty(); // we are done
		}
	}
	if (destArray.GetSize()==0)
	{
		TupString emptyString;
		destArray.Add(emptyString);
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
	// Check that this object has each key specified in the keykeeper:
	int nNumKeys = checkedKeyKeeper.GetSize();
	int nMatchedKeys = 0; // keep track of how many keys matched
	for (int nKeyIndex = 0; nKeyIndex < nNumKeys; nKeyIndex++)
	{
		bool bKeyMatched = false;
		int nKeynameAt = pCurrAttribData->KeynameExistsAt(checkedKeyKeeper[nKeyIndex].m_strKeyname);
		if (nKeynameAt != -1)
		{
			// checklistbox key
			if (m_pCheckListBoxKeys->GetIndex(checkedKeyKeeper[nKeyIndex].m_strKeyname)!=-1)
			{
				// Check to see if this object has a value equal to one of the vals
				// specified for this key in the keykeeper:
				int nNumVals = checkedKeyKeeper[nKeyIndex].m_valKeeper.GetSize();
				int nMatchedValues = 0;
				for (int nValIndex = 0; nValIndex < nNumVals; nValIndex++)
				{
					Keeper<TupString> keyValues;
					GetCheckListBoxValues(pCurrAttribData->GetValueAsString(nKeynameAt),keyValues);
					for (int valueIndex = 0; valueIndex < keyValues.GetSize(); valueIndex++)
					{
						if (keyValues[valueIndex] == checkedKeyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_strVal)
							nMatchedValues++;
					}
				}
				// if a value was matched or no values to match (checked key only)
				if (nMatchedValues>0 || nNumVals == 0)
					nMatchedKeys++;
			}
			else
			{
				// Check to see if this object has a value equal to one of the vals
				// specified for this key in the keykeeper:
				int nNumVals = checkedKeyKeeper[nKeyIndex].m_valKeeper.GetSize();
				int nMatchedValues = 0;
				for (int nValIndex = 0; nValIndex < nNumVals; nValIndex++)
				{
					if (pCurrAttribData->GetValueAsString(nKeynameAt) == checkedKeyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_strVal)
						nMatchedValues++;
				}
				// if a value was matched or no values to match (checked key only)
				if (nMatchedValues>0 || nNumVals == 0)
					nMatchedKeys++;
			}
		}
	}
	// return true if all the keys matched
	return nNumKeys == nMatchedKeys;
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
// Output: int -- returns total number of affected nodes
//------------------------------------------------------------------------
int ObjAttrController::HandleApplyApply(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll)
{
	String command(kSetFlag);
	if (bApplyToAll)
	{
		command += " ";
		command += kApplySceneFlag;
	}
	int numKeys = keyKeeper.GetSize();
	for (int keyIndex = 0; keyIndex < numKeys; keyIndex++)
	{
		if (!keyKeeper[keyIndex].m_bVisible)
			continue;

		// Add all the vals for this key, regardless if they have been checked or not:
		int numVals = keyKeeper[keyIndex].m_valKeeper.GetSize();
		for (int valueIndex = 0; valueIndex < numVals; valueIndex++)
		{
			if (!keyKeeper[keyIndex].m_valKeeper[valueIndex].m_bVisible)
				continue;

			if (keyKeeper[keyIndex].m_valKeeper[valueIndex].m_bChecked)
			{
				command += " ";
				command += kKeyValueFlag;
				command += " \"";
				command += (const char *)FixKeyString(keyKeeper[keyIndex].m_strKeyname);
				command += "\" \"";
				command += (const char *)FixValueString(keyKeeper[keyIndex].m_valKeeper[valueIndex].m_strVal);
				command += "\"";
			}
		}
	}
	StringList result;
	ExecuteCommand(command,result);
	if (result.GetSize() > 0)
	{
		return result.GetHead().AsInt();
	}
	return 0;

}

//------------------------------------------------------------------------
// ObjAttrController::HandleQueryCommand
// This method do various query functions
//
// Input:
// const CommandArgs &commandArgs -- arguments
// TupArray<TupString>& resultStrings -- result of command
// UndoRedoState &undoRedoState -- data to set up for undo/redo
//
// Output: bool
//------------------------------------------------------------------------
bool ObjAttrController::HandleQueryCommand(const CommandArgs &commandArgs, TupArray<TupString> &resultStrings, UndoRedoState &undoRedoState)
{
	if (commandArgs.m_bApplyScene)
	{
		LoadCurrAttribDataScene();
	}
	else
	{
		if (commandArgs.m_pPickedObjects)
		{
			LoadCurrAttribData(commandArgs.m_pPickedObjects); // objects on command line
		}
		else
		{
			LoadCurrAttribData(); // selected objects
		}
	}

	char pChValue[10]; // for converting values to strings

	if (m_pCurrAttribController->GetSize()>0)
	{
		// clear any cached data in the tree
		m_pConfigRoot->ClearCachedData();

		CurrAttribData* pCurrAttribData = m_pCurrAttribController->GetCurrAttribData(0);

		KeyValueContext keyValueContext;
		KeyValue keyValue(pCurrAttribData->GetKeyValuePairs());

		// if only requesting certain keys
		if (commandArgs.m_keyValueKeeper.GetSize())
		{
			for (int keyIndex = 0; keyIndex < commandArgs.m_keyValueKeeper.GetSize(); keyIndex++)
			{
				keyValueContext.AddKey(commandArgs.m_keyValueKeeper[keyIndex].m_strKeyname);
			}
			m_pConfigRoot->GetKeyValueContexts(m_pCurrAttribController,keyValue,keyValueContext,false);
		}
		else
		{
			m_pConfigRoot->GetKeyValueContexts(m_pCurrAttribController,keyValue,keyValueContext,true);
		}

		int numKeys = keyValueContext.GetNumAttribs();
		int keyIndex;
		int numValidKeys = 0;
		for (keyIndex = 0; keyIndex < numKeys; keyIndex++)
		{
			if (keyValueContext.ValueValidAt(keyIndex))
				numValidKeys++;
		}

		itoa(numValidKeys, pChValue, 10);
		resultStrings.Add(TupString(pChValue)); // # of valid keys

		for (keyIndex = 0; keyIndex < numKeys; keyIndex++)
		{
			if (!keyValueContext.ValueValidAt(keyIndex))
				continue;
			TupString keyName = keyValueContext.GetKeyAsString(keyIndex);
			resultStrings.Add(TupString("key"));
			resultStrings.Add(TupString(keyName));
			if (commandArgs.m_bQueryValues)
			{
				resultStrings.Add(TupString("values"));
				// if a check list box (more than one value)
				if (m_pCheckListBoxKeys->GetIndex(keyName)!=-1)
				{
					Keeper<TupString> keyValues;
					GetCheckListBoxValues(keyValueContext.GetValueAsString(keyIndex),keyValues);
					itoa(keyValues.GetSize(), pChValue, 10);
					resultStrings.Add(TupString(pChValue)); // # of values
					for (int valueIndex = 0; valueIndex < keyValues.GetSize(); valueIndex++)
					{
						resultStrings.Add(TupString(keyValues[valueIndex])); // each value
					}
				}
				else // single value
				{
					resultStrings.Add(TupString("1"));
					resultStrings.Add(TupString(keyValueContext.GetValueAsString(keyIndex)));
				}
			}
			if (commandArgs.m_bQueryControlType)
			{
				resultStrings.Add(TupString("controlType"));

				const ConfigBase *pControl = keyValueContext.GetControlPtr(keyIndex);
				TupString controlTypeName("Unknown");
				if (pControl)
				{
					ConfigBase::ControlType controlType = pControl->GetControlType();
					switch (controlType)
					{
						case ConfigBase::ControlType::BASE:
							controlTypeName = "base";
							break;
						case ConfigBase::ControlType::ROOT:
							controlTypeName = "root";
							break;
						case ConfigBase::ControlType::SELECTION:
							controlTypeName = "selection";
							break;
						case ConfigBase::ControlType::CHECK_BOX:
							controlTypeName = "checkBox";
							break;
						case ConfigBase::ControlType::COMBO_BOX:
							controlTypeName = "comboBox";
							break;
						case ConfigBase::ControlType::RADIO_GROUP:
							controlTypeName = "radioGroup";
							break;
						case ConfigBase::ControlType::EDIT_BOX:
							controlTypeName = "editBox";
							break;
						case ConfigBase::ControlType::NUMERIC:
							controlTypeName = "numeric";
							break;
						case ConfigBase::ControlType::FREE_COLOR_BOX:
							controlTypeName = "freeColorBox";
							break;
						case ConfigBase::ControlType::SET_COLOR_BOX:
							controlTypeName = "setColorBox";
							break;
						case ConfigBase::ControlType::CHECK_LIST:
							controlTypeName = "checkList";
							break;
						case ConfigBase::ControlType::LABEL:
							controlTypeName = "label";
							break;
					}
					resultStrings.Add(controlTypeName);
				}
			}
			if (commandArgs.m_bQueryIsDefault)
			{
				resultStrings.Add(TupString("isDefault"));

				if (keyValueContext.IsDefaultValue(keyIndex))
					resultStrings.Add(TupString("1"));
				else
					resultStrings.Add(TupString("0"));
			}
			if (commandArgs.m_bQueryDefaultValue)
			{
				resultStrings.Add(TupString("defaultValue"));
				resultStrings.Add(TupString(keyValueContext.GetDefaultValueAsString(keyIndex)));
			}
			if (commandArgs.m_bQuerySelectionIndex)
			{
				resultStrings.Add(TupString("selectionIndex"));
				itoa(keyValueContext.GetSelectionIndex(keyIndex), pChValue, 10);
				resultStrings.Add(TupString(pChValue));
			}
			if (commandArgs.m_bQueryAvailableValues)
			{
				resultStrings.Add(TupString("availableValues"));
				KeySelection keySelections;
				m_pConfigRoot->GetKeySelections(m_pCurrAttribController,keyValue,keyValueContext.GetKeyAsString(keyIndex),keySelections);
				itoa(keySelections.GetNumSelections(), pChValue, 10);
				resultStrings.Add(TupString(pChValue));
				for (int selectionIndex = 0; selectionIndex < keySelections.GetNumSelections(); selectionIndex++)
				{
					resultStrings.Add(TupString(keySelections.GetSelection(selectionIndex)));
				}
			}
			if (commandArgs.m_bQueryControlUI)
			{
				resultStrings.Add(TupString("controlUI"));
				const ConfigBase *pControl = keyValueContext.GetControlPtr(keyIndex);
				TupString controlUI("0");
				if (pControl)
				{
					if (pControl->GetNonUI())
						controlUI = "0";
					else
						controlUI = "1";
				}
				resultStrings.Add(controlUI);
			}
		}
	}
	return true;
}

//------------------------------------------------------------------------
// ObjAttrController::HandleSetCommand
// This method will be called to apply specific vals for the keys selected in
// the incoming keeper, to nodes in either the scene or selection set,
// depending on the boolean passed in.
//
// Input:
// const CommandArgs &commandArgs -- arguments
// TupArray<TupString>& resultStrings -- result of command
// UndoRedoState &undoRedoState -- data to set up for undo/redo
//
// Output: None.
//------------------------------------------------------------------------
bool ObjAttrController::HandleSetCommand(const CommandArgs &commandArgs, TupArray<TupString> &resultStrings, UndoRedoState &undoRedoState)
{
	if (commandArgs.m_bApplyScene)
	{
		LoadCurrAttribDataScene();
	}
	else
	{
		if (commandArgs.m_pPickedObjects)
		{
			LoadCurrAttribData(commandArgs.m_pPickedObjects); // objects on command line
		}
		else
		{
			LoadCurrAttribData(); // selected objects
		}
	}

	int numAffectedNodes = 0;

	// Get the current attrib data:
	if (m_pCurrAttribController != NULL)
	{
		// Now walk through each node specified in the curr attrib data and
		// adjust any values necessary:
		int nNumCurrAttribDatas = m_pCurrAttribController->GetSize();
		for (int nCADataIndex = 0; nCADataIndex < nNumCurrAttribDatas; nCADataIndex++)
		{
			bool bChanged = false;
			CurrAttribData* pCurrAttribData = m_pCurrAttribController->GetCurrAttribData(nCADataIndex);
			if (commandArgs.m_bReplaceAllValues)
			{
				pCurrAttribData->RemoveAllKeynames();
			}
			int nNumKeeperKeys = commandArgs.m_keyValueKeeper.GetSize();
			for (int nKeeperKeyIndex = 0; nKeeperKeyIndex < nNumKeeperKeys; nKeeperKeyIndex++)
			{
				// if checklistbox
				if (m_pCheckListBoxKeys->GetIndex(commandArgs.m_keyValueKeeper[nKeeperKeyIndex].m_strKeyname)!=-1)
				{
					int nNodeKeyIndex = pCurrAttribData->KeynameExistsAt(commandArgs.m_keyValueKeeper[nKeeperKeyIndex].m_strKeyname);
					Keeper<TupString> keyValues;
					if (nNodeKeyIndex != -1)
					{
						GetCheckListBoxValues(pCurrAttribData->GetValueAsString(nNodeKeyIndex),keyValues);
					}
					// if any specified values need to be removed from list instead
					if (commandArgs.m_bRemoveValueOnSet)
					{
						Keeper<TupString> valuesToKeep;
						// keep only values that are not in the commandArgs.m_keyValueKeeper
						for (int keyValueIndex = 0; keyValueIndex < keyValues.GetSize(); keyValueIndex++)
						{
							bool bFoundValue = false;
							for (int checkedValueIndex = 0; checkedValueIndex < commandArgs.m_keyValueKeeper[nKeeperKeyIndex].m_valKeeper.GetSize(); checkedValueIndex++)
							{
								if (keyValues[keyValueIndex] == commandArgs.m_keyValueKeeper[nKeeperKeyIndex].m_valKeeper[checkedValueIndex].m_strVal)
								{
									bFoundValue = true;
									break;
								}
							}
							if (!bFoundValue)
							{
								valuesToKeep.Add(keyValues[keyValueIndex]);
							}
						}
						keyValues = valuesToKeep;
					}
					else // add values to keeper
					{
						for (int checkedValueIndex = 0; checkedValueIndex < commandArgs.m_keyValueKeeper[nKeeperKeyIndex].m_valKeeper.GetSize(); checkedValueIndex++)
						{
							keyValues.Add(commandArgs.m_keyValueKeeper[nKeeperKeyIndex].m_valKeeper[checkedValueIndex].m_strVal);
						}
					}
					TupString newValue = BuildCheckListBoxValue(keyValues);
					pCurrAttribData->AddKeyValue(commandArgs.m_keyValueKeeper[nKeeperKeyIndex].m_strKeyname, newValue);
					bChanged = true;
				}
				else
				{
					KeyValuePair keyValuePair(commandArgs.m_keyValueKeeper[nKeeperKeyIndex].m_strKeyname,commandArgs.m_keyValueKeeper[nKeeperKeyIndex].m_valKeeper[0].m_strVal);
					if (pCurrAttribData->Find(keyValuePair)==-1)
					{
						for (int keyIndex = 0; keyIndex < commandArgs.m_purgeKeysOnSet.GetSize(); keyIndex++)
						{
							if (pCurrAttribData->RemoveKeyname(commandArgs.m_purgeKeysOnSet[keyIndex]))
							{
								bChanged = true;
							}
						}
					}
					if (pCurrAttribData->AddKeyValue(commandArgs.m_keyValueKeeper[nKeeperKeyIndex].m_strKeyname,
						commandArgs.m_keyValueKeeper[nKeeperKeyIndex].m_valKeeper[0].m_strVal))
					{
						bChanged = true;
					}
				}
			}
			if (bChanged)
			{
				HSnipeObject *pObject = m_pCurrAttribController->GetSnipeObject(nCADataIndex);
				TupString strFullData;
				CurrAttribController::GetNote(pObject,strFullData);

				undoRedoState.m_updatedObjects.Add(pObject);
				undoRedoState.m_previousObjectAttributes.Add(strFullData);
				TupString strAttribData = m_pCurrAttribController->CurrObjDataToStr(nCADataIndex);
				AddRawDataToString(strFullData, strAttribData);
				undoRedoState.m_newObjectAttributes.Add(strFullData);
				numAffectedNodes++;
			}
		}
	}

	// output the number of affected nodes
	char pChValue[8];
	itoa(numAffectedNodes, pChValue, 10);
	resultStrings.Add(TupString(pChValue));

	return true;
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
// Output: int -- total number of purged nodes
//------------------------------------------------------------------------
int ObjAttrController::HandleApplyPurge(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll)
{
	String command(kPurgeFlag);
	if (bApplyToAll)
	{
		command += " ";
		command += kApplySceneFlag;
	}
   	
	int numKeys = keyKeeper.GetSize();
	for (int keyIndex = 0; keyIndex < numKeys; keyIndex++)
	{
		if (!keyKeeper[keyIndex].m_bVisible)
			continue;

		// Check to see if this key is checked:
		if (keyKeeper[keyIndex].m_bChecked == true)
		{
			command += " ";
			command += kKeyFlag;
			command += " \"";
			command += (const char *)keyKeeper[keyIndex].m_strKeyname;
			command += "\"";
		}
		else
		{
			// Add all the vals for this key, regardless if they have been checked or not:
			int numVals = keyKeeper[keyIndex].m_valKeeper.GetSize();
			for (int valueIndex = 0; valueIndex < numVals; valueIndex++)
			{
				if (!keyKeeper[keyIndex].m_valKeeper[valueIndex].m_bVisible)
					continue;

				if (keyKeeper[keyIndex].m_valKeeper[valueIndex].m_bChecked)
				{
					command += " ";
					command += kKeyValueFlag;
					command += " \"";
					command += (const char *)FixKeyString(keyKeeper[keyIndex].m_strKeyname);
					command += "\" \"";
					command += (const char *)FixValueString(keyKeeper[keyIndex].m_valKeeper[valueIndex].m_strVal);
					command += "\"";
				}
			}
		}
	}
	StringList result;
	ExecuteCommand(command,result);
	if (result.GetSize() > 0)
	{
		return result.GetHead().AsInt();
	}
	return 0;
}

//------------------------------------------------------------------------
// ObjAttrController::HandlePurgeCommand
// This method will be called to purge keys and vals 
// within nodes in either the selection set, or from the whole scene.
//
// Input:
// const CommandArgs &commandArgs -- arguments
// TupArray<TupString>& resultStrings -- result of command
// UndoRedoState &undoRedoState -- data to set up for undo/redo
//
// Output: None.
//------------------------------------------------------------------------
bool ObjAttrController::HandlePurgeCommand(const CommandArgs &commandArgs, TupArray<TupString> &resultStrings, UndoRedoState &undoRedoState)
{
	if (commandArgs.m_bApplyScene)
	{
		LoadCurrAttribDataScene();
	}
	else
	{
		if (commandArgs.m_pPickedObjects)
		{
			LoadCurrAttribData(commandArgs.m_pPickedObjects); // objects on command line
		}
		else
		{
			LoadCurrAttribData(); // selected objects
		}
	}

	int numAffectedNodes = 0;

	// Get the current attrib data:
	if (m_pCurrAttribController != NULL)
	{
		// Now walk through each node specified in the curr attrib data and
		// purge any keys and/or vals as necessary:
		int nNumCurrAttribDatas = m_pCurrAttribController->GetSize();
		for (int nCADataIndex = 0; nCADataIndex < nNumCurrAttribDatas; nCADataIndex++)
		{
			bool bChanged = false;
			CurrAttribData* pCurrAttribData = m_pCurrAttribController->GetCurrAttribData(nCADataIndex);
			int nNumKeeperKeys = commandArgs.m_keyValueKeeper.GetSize();
			for (int nKeeperKeyIndex = 0; nKeeperKeyIndex < nNumKeeperKeys; nKeeperKeyIndex++)
			{
				// Check to see if this node has this key:
				int nNodeKeyIndex = pCurrAttribData->KeynameExistsAt(commandArgs.m_keyValueKeeper[nKeeperKeyIndex].m_strKeyname);
				if (nNodeKeyIndex > -1)
				{
					if (commandArgs.m_keyValueKeeper[nKeeperKeyIndex].m_bChecked == true)
					{
						// Purge the key from this node:
						pCurrAttribData->RemoveKeyname(commandArgs.m_keyValueKeeper[nKeeperKeyIndex].m_strKeyname);
						bChanged = true;
					}
					else
					{
						// if checklistbox
						if (m_pCheckListBoxKeys->GetIndex(commandArgs.m_keyValueKeeper[nKeeperKeyIndex].m_strKeyname)!=-1)
						{
							int nNodeKeyIndex = pCurrAttribData->KeynameExistsAt(commandArgs.m_keyValueKeeper[nKeeperKeyIndex].m_strKeyname);
							Keeper<TupString> keyValues;
							if (nNodeKeyIndex != -1)
							{
								GetCheckListBoxValues(pCurrAttribData->GetValueAsString(nNodeKeyIndex),keyValues);
							}
							Keeper<TupString> purgeKeyValues;
							for (int checkedValueIndex = 0; checkedValueIndex < commandArgs.m_keyValueKeeper[nKeeperKeyIndex].m_valKeeper.GetSize(); checkedValueIndex++)
							{
								purgeKeyValues.Add(commandArgs.m_keyValueKeeper[nKeeperKeyIndex].m_valKeeper[checkedValueIndex].m_strVal);
							}
							Keeper<TupString> remainingValues;
							for (int keyValueIndex = 0; keyValueIndex < keyValues.GetSize(); keyValueIndex++)
							{
								// if not in purgeList
								if (purgeKeyValues.GetIndex(keyValues[keyValueIndex])==-1)
								{
									remainingValues.Add(keyValues[keyValueIndex]);
								}
							}
							if (keyValues.GetSize())
							{
								TupString newValue = BuildCheckListBoxValue(remainingValues);
								pCurrAttribData->AddKeyValue(commandArgs.m_keyValueKeeper[nKeeperKeyIndex].m_strKeyname, newValue);
								bChanged = true;
							}
							else // if none left then get rid of key
							{
								pCurrAttribData->RemoveKeyname(commandArgs.m_keyValueKeeper[nKeeperKeyIndex].m_strKeyname);
								bChanged = true;
							}
						}
						else
						{
							// Only purge if this node's val is equal to one of the vals
							// specified to be purged:
							int nNumKeeperVals = commandArgs.m_keyValueKeeper[nKeeperKeyIndex].m_valKeeper.GetSize();
							for (int nKeeperValIndex = 0; nKeeperValIndex < nNumKeeperVals; nKeeperValIndex++)
							{

								if (pCurrAttribData->GetValueAsString(nNodeKeyIndex) == 
									commandArgs.m_keyValueKeeper[nKeeperKeyIndex].m_valKeeper[nKeeperValIndex].m_strVal)
								{
									// Purge the key from this node since it matches the val:
									pCurrAttribData->RemoveKeyname(commandArgs.m_keyValueKeeper[nKeeperKeyIndex].m_strKeyname);
									bChanged = true;
									break;
								}
							}
						}
					}
				}
			}
			if (commandArgs.m_valueKeeper.GetSize())
			{
				int numKeys = pCurrAttribData->GetNumAttribs();
				for (int attribKeyIndex = 0; attribKeyIndex < numKeys; attribKeyIndex++)
				{
					TupString strKeyname = pCurrAttribData->GetKeyAsString(attribKeyIndex);
					// if checklistbox
					if (m_pCheckListBoxKeys->GetIndex(strKeyname)!=-1)
					{
						int nNodeKeyIndex = pCurrAttribData->KeynameExistsAt(commandArgs.m_keyValueKeeper[nKeeperKeyIndex].m_strKeyname);
						Keeper<TupString> keyValues;
						if (nNodeKeyIndex != -1)
						{
							GetCheckListBoxValues(pCurrAttribData->GetValueAsString(nNodeKeyIndex),keyValues);
						}
						Keeper<TupString> purgeKeyValues;
						for (int checkedValueIndex = 0; checkedValueIndex < commandArgs.m_valueKeeper.GetSize(); checkedValueIndex++)
						{
							purgeKeyValues.Add(commandArgs.m_valueKeeper[checkedValueIndex].m_strVal);
						}
						Keeper<TupString> remainingValues;
						for (int keyValueIndex = 0; keyValueIndex < keyValues.GetSize(); keyValueIndex++)
						{
							// if not in purgeList
							if (purgeKeyValues.GetIndex(keyValues[keyValueIndex])==-1)
							{
								remainingValues.Add(keyValues[keyValueIndex]);
							}
						}
						if (keyValues.GetSize())
						{
							TupString newValue = BuildCheckListBoxValue(remainingValues);
							pCurrAttribData->AddKeyValue(strKeyname, newValue);
							bChanged = true;
						}
						else // if none left then get rid of key
						{
							pCurrAttribData->RemoveKeyname(strKeyname);
							bChanged = true;
						}
					}
					else
					{
						// Only purge if this node's val is equal to one of the vals
						// specified to be purged:
						int nNumKeeperVals = commandArgs.m_valueKeeper.GetSize();
						for (int nKeeperValIndex = 0; nKeeperValIndex < nNumKeeperVals; nKeeperValIndex++)
						{
							if (pCurrAttribData->GetValueAsString(attribKeyIndex) == 
								commandArgs.m_valueKeeper[nKeeperValIndex].m_strVal)
							{
								// Purge the key from this node since it matches the val:
								pCurrAttribData->RemoveKeyname(strKeyname);
								bChanged = true;
								break;
							}
						}
					}
				}
			}
			if (bChanged)
			{
				HSnipeObject *pObject = m_pCurrAttribController->GetSnipeObject(nCADataIndex);
				TupString strFullData;
				CurrAttribController::GetNote(pObject,strFullData);

				undoRedoState.m_updatedObjects.Add(pObject);
				undoRedoState.m_previousObjectAttributes.Add(strFullData);
				TupString strAttribData = m_pCurrAttribController->CurrObjDataToStr(nCADataIndex);
				AddRawDataToString(strFullData, strAttribData);
				undoRedoState.m_newObjectAttributes.Add(strFullData);
				numAffectedNodes++;
			}
		}
	}

	// output the number of affected nodes
	char pChValue[8];
	itoa(numAffectedNodes, pChValue, 10);
	resultStrings.Add(TupString(pChValue));

	return true;

}

//------------------------------------------------------------------------
// ObjAttrController::HandleApplyRemap
// This method will be called to remap keys and vals (specified in the incoming
// keykeeper) from nodes in either the selection set, or from the whole scene.
//
// Input:
//  Keeper<KeyInfo>& keyKeeper -- The keeper structure that contains the keys
//								  and vals that are marked for purging.
//	bool bApplyToAll -- Pass in false to apply to nodes from the selection set
//						only.  Pass in true to apply to nodes from the whole
//						scene.
//
// Output: int -- total number of remapped nodes
//------------------------------------------------------------------------
int ObjAttrController::HandleApplyRemap(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll)
{
	String command(kRemapFlag);
	if (bApplyToAll)
	{
		command += " ";
		command += kApplySceneFlag;
	}
	int numKeys = keyKeeper.GetSize();
	for (int keyIndex = 0; keyIndex < numKeys; keyIndex++)
	{
		if (!keyKeeper[keyIndex].m_bVisible)
			continue;

		// Check to see if this key is checked:
		if (keyKeeper[keyIndex].m_bChecked == true)
		{
			command += " ";
			command += kRemapKeyFlag;
			command += " \"";
			command += (const char *)keyKeeper[keyIndex].m_strKeyname;
			command += "\" \"";
			command += (const char *)keyKeeper[keyIndex].m_strNewKeyname;
			command += "\"";
		}

		// Add all the vals for this key, regardless if they have been checked or not:
		int numVals = keyKeeper[keyIndex].m_valKeeper.GetSize();
		for (int valueIndex = 0; valueIndex < numVals; valueIndex++)
		{
			if (!keyKeeper[keyIndex].m_valKeeper[valueIndex].m_bVisible)
				continue;

			if (keyKeeper[keyIndex].m_valKeeper[valueIndex].m_bChecked)
			{
				command += " ";
				command += kRemapKeyValueFlag;
				command += " \"";
				command += (const char *)FixKeyString(keyKeeper[keyIndex].m_strKeyname);
				command += "\" \"";
				command += (const char *)FixValueString(keyKeeper[keyIndex].m_valKeeper[valueIndex].m_strVal);
				command += "\" \"";
				command += (const char *)FixValueString(keyKeeper[keyIndex].m_valKeeper[valueIndex].m_strNewVal);
				command += "\"";
			}
		}
	}
	StringList result;
	ExecuteCommand(command,result);
	if (result.GetSize() > 0)
	{
		return result.GetHead().AsInt();
	}
	return 0;
}

//------------------------------------------------------------------------
// ObjAttrController::HandleApplyRemap
// This method will be called to remap keys and vals (specified in the incoming
// keykeeper) within nodes in either the selection set, or from the whole scene.
//
// Input:
// const CommandArgs &commandArgs -- arguments
// TupArray<TupString>& resultStrings -- result of command
// UndoRedoState &undoRedoState -- data to set up for undo/redo
//
// Output: None.
//------------------------------------------------------------------------
bool ObjAttrController::HandleRemapCommand(const CommandArgs &commandArgs, TupArray<TupString>& resultStrings, UndoRedoState &undoRedoState)
{
	if (commandArgs.m_bApplyScene)
	{
		LoadCurrAttribDataScene();
	}
	else
	{
		if (commandArgs.m_pPickedObjects)
		{
			LoadCurrAttribData(commandArgs.m_pPickedObjects); // objects on command line
		}
		else
		{
			LoadCurrAttribData(); // selected objects
		}
	}

	int numAffectedNodes = 0;

	// Get the current attrib data:
	if (m_pCurrAttribController != NULL)
	{
		// Now walk through each node specified in the curr attrib data and
		// remap any keys and/or vals as necessary:
		int nNumCurrAttribDatas = m_pCurrAttribController->GetSize();
		for (int nCADataIndex = 0; nCADataIndex < nNumCurrAttribDatas; nCADataIndex++)
		{
			bool bChanged = false;
			CurrAttribData* pCurrAttribData = m_pCurrAttribController->GetCurrAttribData(nCADataIndex);

			Keeper<KeyInfo> contextKeyInfo;
			m_pKeyValComm->AddKeyInfoData(m_pCurrAttribController,pCurrAttribData,contextKeyInfo);

			int nNumKeeperKeys = commandArgs.m_keyValueKeeper.GetSize();
			for (int nKeeperKeyIndex = 0; nKeeperKeyIndex < nNumKeeperKeys; nKeeperKeyIndex++)
			{
				const KeyInfo &keyInfo = commandArgs.m_keyValueKeeper[nKeeperKeyIndex];

				// if this key exists in the context
				int contextKeyIndex = contextKeyInfo.GetIndex(keyInfo);
				if (contextKeyIndex == -1)
					continue;

				bool bRemapKey = false;
				bool bRemapVal = false;
				TupString strOrigKey = commandArgs.m_keyValueKeeper[nKeeperKeyIndex].m_strKeyname;
				TupString strNewKey = "";
				TupString strOrigVal = "";
				TupString strNewVal = "";

				// First check to see if the val for this key should be remapped:
				int nNumKeeperVals = commandArgs.m_keyValueKeeper[nKeeperKeyIndex].m_valKeeper.GetSize();

				// if a checklist key
				if (m_pCheckListBoxKeys->GetIndex(commandArgs.m_keyValueKeeper[nKeeperKeyIndex].m_strKeyname)!=-1)
				{
					int nNodeKeyIndex = pCurrAttribData->KeynameExistsAt(keyInfo.m_strKeyname);
					Keeper<TupString> keyValues;
					GetCheckListBoxValues(pCurrAttribData->GetValueAsString(nNodeKeyIndex),keyValues);
					Keeper<TupString> remappedValues;
					Keeper<TupString> newRemapValues;
					for (int nKeeperValIndex = 0; nKeeperValIndex < nNumKeeperVals; nKeeperValIndex++)
					{
						if ((commandArgs.m_keyValueKeeper[nKeeperKeyIndex].m_valKeeper[nKeeperValIndex].m_bChecked == true) &&
								(keyValues.GetIndex(commandArgs.m_keyValueKeeper[nKeeperKeyIndex].m_valKeeper[nKeeperValIndex].m_strVal)!=-1))
						{
							remappedValues.Add(commandArgs.m_keyValueKeeper[nKeeperKeyIndex].m_valKeeper[nKeeperValIndex].m_strVal);
							newRemapValues.Add(commandArgs.m_keyValueKeeper[nKeeperKeyIndex].m_valKeeper[nKeeperValIndex].m_strNewVal);
						}
					}
					for (int keyValueIndex = 0; keyValueIndex < keyValues.GetSize(); keyValueIndex++)
					{
						// not remapped values get added to newRemap table
						if (remappedValues.GetIndex(keyValues[keyValueIndex]) == -1)
						{
							newRemapValues.Add(keyValues[keyValueIndex]);
						}
					}
					strNewVal = BuildCheckListBoxValue(newRemapValues);
					bRemapVal = true;
				}
				else
				{
					int numValInfos = contextKeyInfo[contextKeyIndex].m_valKeeper.GetSize();
					for (int valInfoIndex = 0; valInfoIndex < numValInfos; valInfoIndex++)
					{
						strOrigVal = contextKeyInfo[contextKeyIndex].m_valKeeper[valInfoIndex].m_strVal;
						for (int nKeeperValIndex = 0; nKeeperValIndex < nNumKeeperVals; nKeeperValIndex++)
						{
							if ((commandArgs.m_keyValueKeeper[nKeeperKeyIndex].m_valKeeper[nKeeperValIndex].m_bChecked == true) &&
								(strOrigVal == commandArgs.m_keyValueKeeper[nKeeperKeyIndex].m_valKeeper[nKeeperValIndex].m_strVal))
							{
								strNewVal = commandArgs.m_keyValueKeeper[nKeeperKeyIndex].m_valKeeper[nKeeperValIndex].m_strNewVal;
								bRemapVal = true;
								// At this point, there will only be one val that will match.
								// Since we found it, we can break:
								break;
							}
						}
						// if we remapped a value then we can break
						if (bRemapVal)
							break;
					}
				}

				// Now check to see if this key should be remapped:
				if (commandArgs.m_keyValueKeeper[nKeeperKeyIndex].m_bChecked == true)
				{
					strNewKey = commandArgs.m_keyValueKeeper[nKeeperKeyIndex].m_strNewKeyname;
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
						bChanged = true;
					}
					else
					{
						// Remap the key, don't remap the val:
						pCurrAttribData->RemoveKeyname(strOrigKey);
						pCurrAttribData->AddKeyValue(strNewKey, strOrigVal);
						bChanged = true;
					}
				}
				else
				{
					if (bRemapVal == true)
					{
						// Don't remap the key, remap the val:
						pCurrAttribData->AddKeyValue(strOrigKey, strNewVal);
						bChanged = true;
					}
					else
					{
						// Don't remap the key, don't remap the val:
						;
					}
				}
			}
			if (bChanged)
			{
				HSnipeObject *pObject = m_pCurrAttribController->GetSnipeObject(nCADataIndex);
				TupString strFullData;
				CurrAttribController::GetNote(pObject,strFullData);

				undoRedoState.m_updatedObjects.Add(pObject);
				undoRedoState.m_previousObjectAttributes.Add(strFullData);
				TupString strAttribData = m_pCurrAttribController->CurrObjDataToStr(nCADataIndex);
				AddRawDataToString(strFullData, strAttribData);
				undoRedoState.m_newObjectAttributes.Add(strFullData);
				numAffectedNodes++;
			}
		}
	}

	// output the number of affected nodes
	char pChValue[8];
	itoa(numAffectedNodes, pChValue, 10);
	resultStrings.Add(TupString(pChValue));

	return true;
}

//------------------------------------------------------------------------
// ObjAttrController::HandleFindButton
// This method will be bring up the find dialog
//
// Input: None.
// Output: None.
//------------------------------------------------------------------------
void ObjAttrController::HandleFindButton(void)
{
	m_pKeyValComm->HandleFindButton();
}

//------------------------------------------------------------------------
// ObjAttrController::HandlePurgeUnused
// This method will be called to purge any keys and vals that are not used
//
// Input:
//	bool bApplyToAll -- Pass in false to apply to nodes from the selection set
//						only.  Pass in true to apply to nodes from the whole
//						scene.
// Output: int -- number of affected nodes
//------------------------------------------------------------------------
int ObjAttrController::HandlePurgeUnused(bool bApplyToAll)
{
	String command(kPurgeUnusedFlag);
	if (bApplyToAll)
	{
		command += " ";
		command += kApplySceneFlag;
	}
	StringList result;
	ExecuteCommand(command,result);
	if (result.GetSize() > 0)
	{
		return result.GetHead().AsInt();
	}
	return 0;
}

//------------------------------------------------------------------------
// ObjAttrController::HandlePurgeUnusedCommand
// This method will be called to remove any unused keys,value pairs
//
// Input:
// const CommandArgs &commandArgs -- arguments
// TupArray<TupString>& resultStrings -- result of command
// UndoRedoState &undoRedoState -- data to set up for undo/redo
//
// Output: bool - result of command
//------------------------------------------------------------------------
bool ObjAttrController::HandlePurgeUnusedCommand(const CommandArgs &commandArgs, TupArray<TupString>& resultStrings, UndoRedoState &undoRedoState)
{
	if (commandArgs.m_bApplyScene)
	{
		LoadCurrAttribDataScene();
	}
	else
	{
		if (commandArgs.m_pPickedObjects)
		{
			LoadCurrAttribData(commandArgs.m_pPickedObjects); // objects on command line
		}
		else
		{
			LoadCurrAttribData(); // selected objects
		}
	}

	int numAffectedNodes = 0;

	// Get the current attrib data:
	if (m_pCurrAttribController != NULL)
	{
		// clear any cached data in the tree
		m_pConfigRoot->ClearCachedData();
		// Now walk through each node specified in the curr attrib data and
		// purge any keys and/or vals as necessary:
		int nNumCurrAttribDatas = m_pCurrAttribController->GetSize();
		for (int nCADataIndex = 0; nCADataIndex < nNumCurrAttribDatas; nCADataIndex++)
		{
			CurrAttribData* pCurrAttribData = m_pCurrAttribController->GetCurrAttribData(nCADataIndex);

			KeyValueContext keyValueContext;
			KeyValue keyValue(pCurrAttribData->GetKeyValuePairs());
			m_pConfigRoot->GetKeyValueContexts(m_pCurrAttribController,keyValue,keyValueContext,true);

			int numKeys = keyValue.GetNumAttribs();
			bool bChanged = false;
			for (int keyIndex = 0; keyIndex < numKeys; keyIndex++)
			{
				const char *pKeyName = keyValue.GetKeyAsString(keyIndex);
				int contextKeyIndex = keyValueContext.KeyExistsAt(pKeyName);
				// if this key is in the context (it is used)
				if (contextKeyIndex != -1)
				{
					// if the default value (don't need it)
					if (keyValueContext.IsDefaultValue(contextKeyIndex))
					{
						pCurrAttribData->RemoveKeyname(pKeyName);
						bChanged = true;
					}
					// checklist
					// checks against validated names from context
					const ConfigBase *pConfigBase = keyValueContext.GetControlPtr(contextKeyIndex);
					if (pConfigBase)
					{
						if (pConfigBase->GetControlType() == ConfigBase::ControlType::CHECK_LIST)
						{
							ConfigCheckList *pConfigCheckList = (ConfigCheckList *)pConfigBase;
							Keeper<TupString> listValues;
							if (strcmp(keyValue.GetValueAsString(keyIndex),keyValueContext.GetValueAsString(contextKeyIndex))!=0)
							{
								pCurrAttribData->AddKeyValue(pKeyName,keyValueContext.GetValueAsString(contextKeyIndex));
								bChanged = true;
							}
						}
					}
				}
				else //(not used)
				{
					// out of here
					pCurrAttribData->RemoveKeyname(pKeyName);
					bChanged = true;
				}
			}
			if (bChanged)
			{
				HSnipeObject *pObject = m_pCurrAttribController->GetSnipeObject(nCADataIndex);
				TupString strFullData;
				CurrAttribController::GetNote(pObject,strFullData);

				undoRedoState.m_updatedObjects.Add(pObject);
				undoRedoState.m_previousObjectAttributes.Add(strFullData);
				TupString strAttribData = m_pCurrAttribController->CurrObjDataToStr(nCADataIndex);
				AddRawDataToString(strFullData, strAttribData);
				undoRedoState.m_newObjectAttributes.Add(strFullData);
				numAffectedNodes++;
			}
		}
	}

	// output the number of affected nodes
	char pChValue[8];
	itoa(numAffectedNodes, pChValue, 10);
	resultStrings.Add(TupString(pChValue));

	return true;

}

//------------------------------------------------------------------------
// ObjAttrController::HandleApplyDefaultValues
// This method will be called to apply any valid default values
//
// Input:
//	bool bApplyToAll -- Pass in false to apply to nodes from the selection set
//						only.  Pass in true to apply to nodes from the whole
//						scene.
// Output: None.
//------------------------------------------------------------------------
void ObjAttrController::HandleApplyDefaultValues(bool bApplyToAll)
{
	String command(kApplyDefaultsFlag);
	if (bApplyToAll)
	{
		command += " ";
		command += kApplySceneFlag;
	}
	StringList result;
	ExecuteCommand(command,result);
}

//------------------------------------------------------------------------
// ObjAttrController::HandleApplyDefaultValuesCommand
// This method will be called to apply any valid default values
//
// Input:
// const CommandArgs &commandArgs -- arguments
// TupArray<TupString>& resultStrings -- result of command
// UndoRedoState &undoRedoState -- data to set up for undo/redo
//
// Output: bool - result of command
//------------------------------------------------------------------------
bool ObjAttrController::HandleApplyDefaultValuesCommand(const CommandArgs &commandArgs, TupArray<TupString>& resultStrings, UndoRedoState &undoRedoState)
{
	if (commandArgs.m_bApplyScene)
	{
		LoadCurrAttribDataScene();
	}
	else
	{
		if (commandArgs.m_pPickedObjects)
		{
			LoadCurrAttribData(commandArgs.m_pPickedObjects); // objects on command line
		}
		else
		{
			LoadCurrAttribData(); // selected objects
		}
	}

	int numAffectedNodes = 0;

	// Get the current attrib data:
	if (m_pCurrAttribController != NULL)
	{
		// clear any cached data in the tree
		m_pConfigRoot->ClearCachedData();
		// Now walk through each node specified in the curr attrib data and
		// purge any keys and/or vals as necessary:
		int nNumCurrAttribDatas = m_pCurrAttribController->GetSize();
		for (int nCADataIndex = 0; nCADataIndex < nNumCurrAttribDatas; nCADataIndex++)
		{
			CurrAttribData* pCurrAttribData = m_pCurrAttribController->GetCurrAttribData(nCADataIndex);

			KeyValueContext keyValueContext;
			KeyValue keyValue(pCurrAttribData->GetKeyValuePairs());
			m_pConfigRoot->GetKeyValueContexts(m_pCurrAttribController,keyValue,keyValueContext,true);
			int numKeys = keyValueContext.GetNumAttribs();
			bool bChanged = false;
			for (int keyIndex = 0; keyIndex < numKeys; keyIndex++)
			{
				if (keyValueContext.IsDefaultValue(keyIndex))
				{
					if (pCurrAttribData->AddKeyValue(keyValueContext.GetKeyAsString(keyIndex),keyValueContext.GetValueAsString(keyIndex)))
					{
						bChanged = true;
					}
				}
			}

			if (bChanged)
			{
				HSnipeObject *pObject = m_pCurrAttribController->GetSnipeObject(nCADataIndex);
				TupString strFullData;
				CurrAttribController::GetNote(pObject,strFullData);

				undoRedoState.m_updatedObjects.Add(pObject);
				undoRedoState.m_previousObjectAttributes.Add(strFullData);
				TupString strAttribData = m_pCurrAttribController->CurrObjDataToStr(nCADataIndex);
				AddRawDataToString(strFullData, strAttribData);
				undoRedoState.m_newObjectAttributes.Add(strFullData);
				numAffectedNodes++;
			}
		}
	}

	// output the number of affected nodes
	char pChValue[8];
	itoa(numAffectedNodes, pChValue, 10);
	resultStrings.Add(TupString(pChValue));

	return true;
}



