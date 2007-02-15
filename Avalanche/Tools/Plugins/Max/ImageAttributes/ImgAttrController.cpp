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



//-----------------------------------------------------------------------------
// Includes:

#include "ImageAttributesPCH.h"

#ifndef __IMGATTRCONTROLLER__
#include "ImgAttrController.h"
#endif
#ifndef __CURRATTRIBDATA__
#include "CurrAttribData.h"
#endif
#ifndef __CURRATTRIBCONTROLLER__
#include "CurrAttribController.h"
#endif
#ifndef __CONFIGROOT__
#include "../UIDisplay/ConfigRoot.h"
#endif
#ifndef __UICONTROLLER__
#include "../UIDisplay/UIController.h"
#endif
#ifndef __EXCLUSIVECONTROLLER__
#include "../UIDisplay/ExclusiveController.h"
#endif
#ifndef __KEYVALCOMMUNICATOR__
#include "KeyValCommunicator.h"
#endif
#ifndef __KEYVALPANELCONTROLLER__
#include "../KeyVal/KeyValPanelController.h"
#endif

#include "ImageList.h"


//-----------------------------------------------------------------------------
// Defines, etc.:

#define BEGIN_IMAGE_BLOCK		"BEGIN IMAGE "
#define END_IMAGE_BLOCK			"END IMAGE "
#define BEGIN_MATERIAL_BLOCK	"BEGIN MATERIAL "
#define END_MATERIAL_BLOCK		"END MATERIAL "
#define IMGCFG_FILENAME			_T("ImgAttr.cfg")	// Configuration filename
#define MTLCFG_FILENAME			_T("MtlAttr.cfg")	// Configuration filename
#define HIDDEN_NODE_NAME		"AvalancheImageAttributesNode"

#define NONEXCLUSIVE_KEYS_KEY (TEXT("Software\\Avalanche Software\\ImageAttributes\\NonExclusiveKeys"))

#define IMAGE_ATTRIB_KEY (TEXT("Software\\Avalanche Software\\ImageAttributes"))


//-----------------------------------------------------------------------------
// Class Definition:


//-----------------------------------------------------------------------------
// ImgAttrController::ImgAttrController
// This method is the default constructor for this class.
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
ImgAttrController::ImgAttrController(ImageAttributes* pOwner)
{
	m_pOwner = pOwner;
	m_pConfigRoot = NULL;
	m_pCurrAttribController = NULL;
	m_pUIController = NULL;
	m_pExclusiveController = NULL;
	m_pKeyValComm = NULL;
	m_pImageAttribNode = NULL;
	m_pImageList = NULL;
	m_showMaterials = false;
}


//-----------------------------------------------------------------------------
// ImgAttrController::~ImgAttrController
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
ImgAttrController::~ImgAttrController()
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

  if (m_pImageList != NULL)
  {
    delete m_pImageList;
    m_pImageList = NULL;
  }
}


//-----------------------------------------------------------------------------
// ImgAttrController::GetCurrAttribController
// This method is an accessor for the CurrAttribController data member.
//
// Input: None.
//
// Output: Pointer to the CurrAttribController.
//-----------------------------------------------------------------------------
CurrAttribController* ImgAttrController::GetCurrAttribController()
{
	return m_pCurrAttribController;
}


//-----------------------------------------------------------------------------
// ImgAttrController::LoadConfigFile
// This method will read in the iamge attribute configuration file and load
// the data into the appropriate classes.
//
// Input:
//	Interface* pInterface -- The interface pointer to get the plugcfg folder
//							 from.
//
// Output: True if successful, false otherwise. 
//-----------------------------------------------------------------------------
bool ImgAttrController::LoadConfigFile(Interface* pInterface)
{
	bool bSuccess = true;

	// Create the path to the default config file:
	TSTR tstrFilename;
	tstrFilename += pInterface->GetDir(APP_PLUGCFG_DIR);
	tstrFilename += "\\";
	if (m_showMaterials)
		tstrFilename += MTLCFG_FILENAME;
	else
		tstrFilename += IMGCFG_FILENAME;

	// Now read in the config file and create the tupperware tree:
	TupperwareAggregate* pTupAggConfigData;
	TupperwareLoadSaveManager::Read(tstrFilename, &pTupAggConfigData);

	// Now create the ConfigRoot and tell it to build the config tree:
	delete m_pConfigRoot;
	m_pConfigRoot = new ConfigRoot();
	bSuccess = m_pConfigRoot->BuildConfigTree(pTupAggConfigData);

	// Free up the tupperware aggregate:
	delete pTupAggConfigData;
	pTupAggConfigData = NULL;

	// Set up the exclusive controls:
	// Create the Exclusive Controller and initialize it:
	delete m_pExclusiveController;
	m_pExclusiveController = new ExclusiveController();
	m_pExclusiveController->Init(NONEXCLUSIVE_KEYS_KEY);
	// Tell the ConfigRoot to mark the Exclusive Controller appropriately.
	m_pConfigRoot->BuildExclusiveController(m_pExclusiveController);
	// Tell the Exclusive Controller to remove itself of unwanted keys:
	m_pExclusiveController->TossUnwantedKeys();

	return bSuccess;
}


//-----------------------------------------------------------------------------
// ImgAttrController::LoadCurrAttribData
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
bool ImgAttrController::LoadCurrAttribData(Interface* pInterface, bool showMaterials)
{
	if (m_showMaterials != showMaterials)
	{
		m_showMaterials = showMaterials;
		LoadConfigFile(pInterface);
	}

	CreateImageAttribNode(pInterface);

	int numImages = m_pImageList->SelCount();

	// Make sure there is something selected:
	if (numImages)
	{
		// Get rid of any current attribute data that may exist:
		if (m_pCurrAttribController != NULL)
		{
			m_pCurrAttribController->RemoveAllPtrs();
		}
		else
		{
			m_pCurrAttribController = new CurrAttribController();
		}
		
		// Cycle through each image, create a CurrAttribData for it
		// and load any data that it may have:
		for (int nIndex = 0; nIndex < numImages; ++nIndex)
		{
			const char *pName = m_pImageList->GetSelected(nIndex);
			CurrAttribData* pNewAttribData = new CurrAttribData();

			// Set the object string:
			pNewAttribData->SetEntityString(pName);
			
			std::string imageAttributes;
			// Set any data:
			if (GetImageAttribString(pName, imageAttributes))
			{
				// Now pass on the string to load it into the current attrib data:
				pNewAttribData->StrToCurrAttribData(imageAttributes);
			}
			// Add the CurrAttribData object to the m_pCurrAttribController:
			m_pCurrAttribController->AddObjectPtr(pNewAttribData);
		}
	}

	return numImages > 0;
}


//-----------------------------------------------------------------------------
// ImgAttrController::LoadCurrAttribDataScene
// This method will create a new m_pCurrAttribController and create any
// necessary CurrAttribData objects for all the objects in the scene and load
// the data into them.  This method makes sure that the whole node tree is
// walked through.
//
// Input: None.
//
// Output: True if there are nodes in the scene, false otherwise.
//-----------------------------------------------------------------------------
bool ImgAttrController::LoadCurrAttribDataScene()
{
	int numImages = m_pImageList->Count();

	// Make sure there is something selected:
	if (numImages)
	{
		// Get rid of any current attribute data that may exist:
		if (m_pCurrAttribController != NULL)
		{
			m_pCurrAttribController->RemoveAllPtrs();
		}
		else
		{
			m_pCurrAttribController = new CurrAttribController();
		}
		
		// Cycle through each image, create a CurrAttribData for it
		// and load any data that it may have:
		for (int nIndex = 0; nIndex < numImages; ++nIndex)
		{
			const char *pName = m_pImageList->GetImageAt(nIndex);
			CurrAttribData* pNewAttribData = new CurrAttribData();

			// Set the object string:
			pNewAttribData->SetEntityString(pName);
			
			std::string imageAttributes;
			// Set any data:
			if (GetImageAttribString(pName, imageAttributes))
			{
				// Now pass on the string to load it into the current attrib data:
				pNewAttribData->StrToCurrAttribData(imageAttributes);
			}
			// Add the CurrAttribData object to the m_pCurrAttribController:
			m_pCurrAttribController->AddObjectPtr(pNewAttribData);
		}

		return true;
	}

	return false;
}


//-----------------------------------------------------------------------------
// ImgAttrController::SetUpUIDisplay
// This method will create the set up and create the UIDisplay structures and
// the actual UI in the UIDisplay structures prior to the dialog being shown.
//
// Input:
//  HWND hParentDlg -- The HWND for the parent dialog.
//	HINSTANCE hInstance -- The handle to the application instance.
//  HFONT hUIFont -- Some labels and controls need this font.
//  int nStartx -- The x coord that labels and controls should start at.
//  int nStarty -- The y coord that the first label/control should start at.
//  int nLabelWidth -- The largest allowable width for labels.
//  int nLabelHeight -- The height used when creating labels.
//  int nSpaceBetweenCtrls -- The amount of spacing between controls.
//	WNDPROC newColorSwatchProc -- The new color swatch proc.
//
// Output: True if successful, false otherwise.
//-----------------------------------------------------------------------------
bool ImgAttrController::SetUpUIDisplay(HWND hParentDlg, HINSTANCE hInstance, HFONT hUIFont,
									   int nStartx, int nStarty,
									   int nLabelWidth, int nLabelHeight,
									   int nSpaceBetweenCtrls, WNDPROC newColorSwatchProc)
{
	bool bResult = true;

	if (m_pConfigRoot != NULL)
	{
    if (m_pImageList)
		if (m_pUIController != NULL)
		{
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
				bResult = m_pUIController->BuildActualUI(hParentDlg, hInstance, hUIFont,
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


//-----------------------------------------------------------------------------
// ImgAttrController::DestroyCurrAttribData
// This method will destroy any current attribute data.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void ImgAttrController::DestroyCurrAttribData()
{
	if (m_pCurrAttribController != NULL)
	{
		m_pCurrAttribController->RemoveAllPtrs();
		delete m_pCurrAttribController;
		m_pCurrAttribController = NULL;
	}
}


//-----------------------------------------------------------------------------
// ImgAttrController::DestroyUIDisplay
// This method will destroy any UIDisplay.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void ImgAttrController::DestroyUIDisplay()
{
	if (m_pUIController != NULL)
	{
		delete m_pUIController;
		m_pUIController = NULL;
	}
}


//-----------------------------------------------------------------------------
// ImgAttrController::SaveNonexclusiveKeys
// This method will tell the exclusive controller to save its nonexclusive keys
// to the registry.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void ImgAttrController::SaveNonexclusiveKeys()
{
	if (m_pExclusiveController != NULL)
		m_pExclusiveController->SaveKeyValsInReg(NONEXCLUSIVE_KEYS_KEY);
}


//-----------------------------------------------------------------------------
// ImgAttrController::GetTotalY
// This method will ask the m_pUIController for its total Y spacing that its
// controls take up.
//
// Input: None.
//
// Output: int representing the total amount of y spacing for the controls
//		   in the m_pUIController.  If the controller is NULL, then -1 is
//		   returned.
//-----------------------------------------------------------------------------
int ImgAttrController::GetTotalY()
{
	if (m_pUIController != NULL)
		return m_pUIController->GetTotalY();
	else
		return -1;
}


//-----------------------------------------------------------------------------
// ImgAttrController::CopyAttribsToSelSet
// This method will work with the m_pCurrAttribController to copy the attributes
// from the node passed in to the nodes in the current selection set (which
// should be currently stored in the m_pCurrAttribController).
//
// Input:
//  INode* pName -- The image name to get the attributes from.
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
//-----------------------------------------------------------------------------
bool ImgAttrController::CopyAttribsToSelSet(bool bReplace)
{
	bool bResult = true;

	if (m_pCurrAttribController != NULL)
	{
		// Check to see if any objects are selected:
		int nNumObjects = m_pCurrAttribController->GetSize();
		if (nNumObjects > 0)
		{
			// Get the attribute data for the incoming node:
			CurrAttribData* pNewAttribData = new CurrAttribData();

			const char* pName = m_pImageList->GetSelected(0);

			// Set the object pointer:
			pNewAttribData->SetEntityString(pName);

			// Set any data:
			TSTR buf;
			m_pImageAttribNode->GetUserPropBuffer(buf);
			char* pChObjectBuf = buf.data();
			std::string strFullData = pChObjectBuf;

			// Get to just the raw data substring:
			if (StripAllButRawData(pName, strFullData) == true)
			{
				// Now pass on the string to load it into the current attrib data:
				pNewAttribData->StrToCurrAttribData(strFullData);
			}
			else
			{
				// No block indicators found, so don't set any data for this object:
				;
			}

			// Check to see if there are any attribs to copy:
			int nNumAttribs = pNewAttribData->GetNumAttribs();
			if (nNumAttribs > 0)
			{
				// See if the destination node's attributes should be tossed:
				if (bReplace == true)
				{
					m_pCurrAttribController->RemoveAllKeynames();
				}

				// Cycle through all the attribs, copying them (or changing
				// them if the key already exists) on all the dest nodes:
				for (int nAttribIndex = 0; nAttribIndex < nNumAttribs; nAttribIndex++)
				{
					std::string strKeyname = pNewAttribData->GetKeyAsString(nAttribIndex);
					std::string strValue = pNewAttribData->GetValueAsString(nAttribIndex);
					m_pCurrAttribController->RemoveKeyInstances(strKeyname);
					m_pCurrAttribController->SetStringAsString(strKeyname, strValue);
				}

				// Set the data on the nodes:
				for (int nIndex = 0; nIndex < nNumObjects; nIndex++)
				{
					const char* pCurrName = m_pCurrAttribController->GetEntityString(nIndex);
					TSTR buf;
					m_pImageAttribNode->GetUserPropBuffer(buf);
					char* pChObjectBuf = buf.data();
					std::string strFullData = pChObjectBuf;
					std::string strAttribData = m_pCurrAttribController->CurrObjDataToStr(nIndex);
					this->AddRawDataToString(pCurrName, strFullData, strAttribData);
					m_pImageAttribNode->SetUserPropBuffer(strFullData.data());
				}
			}
			else
			{
				// No attribs to copy:
				bResult = false;
			}

			delete pNewAttribData;
			pNewAttribData = NULL;
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


//-----------------------------------------------------------------------------
// ImgAttrController::HandleWinMessage
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
//-----------------------------------------------------------------------------
bool ImgAttrController::HandleWinMessage(HWND hParent, UINT message, WPARAM wParam, LPARAM lParam)
{
	bool bResult = false;

	if (m_pUIController != NULL)
	{
		// Set up some parameters that can be used as return values to check:
		bool bExclusive = true;
		std::string strKeyname;
		std::string strValue;
		std::vector<std::string> vectKeysToPurge;
		int nFlags = 0;

		// Pass the message on to the UIController:
		bResult = m_pUIController->HandleWinMessage(hParent, message, wParam, lParam, strKeyname, strValue, bExclusive, vectKeysToPurge, nFlags);
		if (bResult == true)
		{
			if (bExclusive == true)
			{
				// Alter the current object data:
				if (m_pCurrAttribController != NULL)
				{
					m_pCurrAttribController->RemoveKeyInstances(strKeyname);
					m_pCurrAttribController->SetStringAsString(strKeyname, strValue);

					// Set the data on the object:
					int nNumObjects = m_pCurrAttribController->GetSize();
					for (int nIndex = 0; nIndex < nNumObjects; nIndex++)
					{
						const char *pName = m_pCurrAttribController->GetEntityString(nIndex);
						TSTR buf;
						m_pImageAttribNode->GetUserPropBuffer(buf);
						char* pChObjectBuf = buf.data();
						std::string strFullData = pChObjectBuf;
						std::string strAttribData = m_pCurrAttribController->CurrObjDataToStr(nIndex);
						AddRawDataToString(pName, strFullData, strAttribData);
						m_pImageAttribNode->SetUserPropBuffer(strFullData.data());
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
				m_pExclusiveController->SetKeyVal(strKeyname, strValue);
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
// ImgAttrController::RemoveCharsFromStr
// This method will look at the string passed in and remove all instances of 
// the specified character.
//
// Input:
//  std::string strToFix -- The string to remove characters from.
//  char chToRemove -- The character to remove.
//
// Output: True if characters were removed, false otherwise.
//-----------------------------------------------------------------------------
bool ImgAttrController::RemoveCharsFromStr(std::string &strToFix, char chToRemove)
{
	bool bCharsRemoved = false;

	std::string::size_type charIndex = strToFix.find(chToRemove);
	while (charIndex != std::string::npos)
	{
		strToFix.erase(charIndex, 1);
		charIndex = strToFix.find(chToRemove);
		bCharsRemoved = true;
	}

	return bCharsRemoved;
}


//-----------------------------------------------------------------------------
// ImgAttrController::AddCarrRtrnsToStr
// This method will add a carriage return before each linefeed in the
// incoming string.
//
// Input:
//  std::string& strToFix -- The string to add carriage returns to.
//
// Output: True if any carriage returns were added.  False otherwise.
//-----------------------------------------------------------------------------
bool ImgAttrController::AddCarrRtrnsToStr(std::string &strToFix)
{
	bool bCharsAdded = false;

	std::string::size_type newLineIndex = strToFix.find('\n');
	while (newLineIndex != std::string::npos)
	{
		strToFix.replace(newLineIndex, 1, "\r\n");
		newLineIndex = strToFix.find('\n', newLineIndex + 2);
		bCharsAdded = true;
	}

	return bCharsAdded;
}


//-----------------------------------------------------------------------------
// ImgAttrController::StripAllButRawData
// This method will take the incoming string and strip out everything but the
// actual raw attribute data substring.
//
// Input:
//  std::string& strToFix -- String to strip.
//
// Output: True if both of the begin and end block indicators were found, false
//		   otherwise.
//-----------------------------------------------------------------------------
bool ImgAttrController::StripAllButRawData(const char *pName,std::string &strToFix)
{
	// Remove carriage returns from the string:
	this->RemoveCharsFromStr(strToFix, '\r');

	// Find and remove the begin block indicator:
	std::string beginString;
	if (m_showMaterials)
		beginString = BEGIN_MATERIAL_BLOCK;
	else
		beginString = BEGIN_IMAGE_BLOCK;

	beginString += pName;
	std::string::size_type beginIndex = strToFix.find(beginString);
	if (beginIndex == std::string::npos)
	{
		// Error - There is no begin block indicator, so just bail:
		return false;
	}
	strToFix.erase(0, beginIndex);
	std::string::size_type newLineIndex = strToFix.find('\n');
	strToFix.erase(0, newLineIndex + 1);

	// Find and remove the end block indicator:
	std::string endString;
	if (m_showMaterials)
		endString = END_MATERIAL_BLOCK;
	else
		endString = END_IMAGE_BLOCK;

	endString += pName;
	std::string::size_type endIndex = strToFix.find(endString);
	if (endIndex == std::string::npos)
	{
		// Error - There is no end block indicator, so just bail:
		return false;
	}
	strToFix.erase(endIndex);

	return true;
}


//-----------------------------------------------------------------------------
// ImgAttrController::AddRawDataToString
// This method will take the strToInsert and add it into the appropriate place
// in the strToAddTo.  It does this by trying to find the begin/end block
// indicators and replacing any substring that may be between the indicators.
// If no indicators are found, new ones are created at the end of the string,
// with the strToInsert added in between them.
//
// Input:
//  std::string& strToAddTo -- String to insert into.
//  std::string strToInsert -- Sting to add.
//
// Output: None.
//-----------------------------------------------------------------------------
void ImgAttrController::AddRawDataToString(const char *pName, std::string& strToAddTo,const std::string&strToInsert)
{
	// First get rid of any carriage returns:
	this->RemoveCharsFromStr(strToAddTo, '\r');

	std::string beginString;
	if (m_showMaterials)
		beginString = BEGIN_MATERIAL_BLOCK;
	else
		beginString = BEGIN_IMAGE_BLOCK;

	beginString += pName;
	std::string::size_type beginIndex = strToAddTo.find(beginString);

	std::string endString;
	if (m_showMaterials)
		endString = END_MATERIAL_BLOCK;
	else
		endString = END_IMAGE_BLOCK;

	endString += pName;
	std::string::size_type endIndex = strToAddTo.find(endString);

	if (beginIndex == std::string::npos && endIndex == std::string::npos)
	{
		// Add a newline to the end of the string if needed:
		std::string::size_type nStrSize = strToAddTo.size();
		std::string::size_type newLineIndex = strToAddTo.rfind('\n');
		if (newLineIndex != (nStrSize - 1))
			strToAddTo.append("\n");

		// Add both of the indicators to the end of the string, with the data between:
		strToAddTo.append(beginString);
		strToAddTo.append("\n");
		strToAddTo.append(strToInsert);
		strToAddTo.append("\n");
		strToAddTo.append(endString);
		strToAddTo.append("\n");
	}
	else if (beginIndex != std::string::npos && endIndex != std::string::npos)
	{
		// Replace any data between the block indicators with the new data:
		std::string::size_type newLineIndex = strToAddTo.find('\n', beginIndex);
		strToAddTo.replace(newLineIndex + 1, endIndex - (newLineIndex + 1), strToInsert);

	}

	// Finally, add the carriage returns back in:
	this->AddCarrRtrnsToStr(strToAddTo);

}


//-----------------------------------------------------------------------------
// ImgAttrController::InitKeyValPanel
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
//-----------------------------------------------------------------------------
void ImgAttrController::InitKeyValPanel(IRollupWindow* pRollupWindow, HINSTANCE hInstance, LPARAM lParam, int nCategory)
{
	if (m_pKeyValComm == NULL)
	{
		m_pKeyValComm = new KeyValCommunicator(this,IMAGE_ATTRIB_KEY);
	}
	m_pKeyValComm->InitKeyValPanel(pRollupWindow, hInstance, lParam, nCategory);
}


//-----------------------------------------------------------------------------
// ImgAttrController::IdCancelMessage
// This method will tell the KeyValCommunicator that the IDCANCEL message has
// been received.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void ImgAttrController::IdCancelMessage()
{
	if (m_pKeyValComm != NULL)
		m_pKeyValComm->IdCancelMessage();
}


//-----------------------------------------------------------------------------
// ImgAttrController::SetApplyBttnStates
// This method will tell the KeyValCommunicator to set the apply button states
// for the key/val panel.
//
// Input:
//  bool bApplySelEnabled -- True if the Apply To Sel button is to be enabled.
//  bool bApplySceneEnabled -- True if the Apply To Scene button is to be
//							   enabled.
//
// Output: None.
//-----------------------------------------------------------------------------
void ImgAttrController::SetApplyBttnStates(bool bApplySelEnabled, bool bApplySceneEnabled)
{
	if (m_pKeyValComm != NULL)
		m_pKeyValComm->SetApplyBttnStates(bApplySelEnabled, bApplySceneEnabled);
}


//-----------------------------------------------------------------------------
// ImgAttrController::LoadKeyValPanelTree
// This method will tell the KeyValCommunicator to load the key/vals in the
// key/val panel.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void ImgAttrController::LoadKeyValPanelTree()
{
	if (m_pKeyValComm != NULL)
		m_pKeyValComm->LoadKeyValPanelTree();
}


/****************************************************************************
 ImgAttrController::CreateImageAttribNode()

 Creates the hidden node used to store image attributes.
****************************************************************************/
bool ImgAttrController::CreateImageAttribNode(Interface *ip)
{
  m_pImageAttribNode = ip->GetINodeByName(HIDDEN_NODE_NAME);
  
  if (!m_pImageAttribNode)
  {
    Object *object = (Object *)ip->CreateInstance(HELPER_CLASS_ID, Class_ID(DUMMY_CLASS_ID,0));
    if(!object)
      return false;
    m_pImageAttribNode = ip->CreateObjectNode(object);
    m_pImageAttribNode->SetName(HIDDEN_NODE_NAME);
  }

  return true;
}


/****************************************************************************
 ImgAttrController::GetImageAttribString()

 Extracts the image attribute string from the hidden node's property buffer.
****************************************************************************/
bool ImgAttrController::GetImageAttribString(const char *pName,std::string &attribString)
{
  if (NULL == m_pImageAttribNode)
  {
		return false;
  }

  TSTR buf;
  m_pImageAttribNode->GetUserPropBuffer(buf);
  char* pChObjectBuf = buf.data();
  attribString = pChObjectBuf;

  // Get to just the raw data substring:
  if (this->StripAllButRawData(pName, attribString))
  {
  }
  else
  {
    // No block indicators found, so no data existed
    return false;
  }

  return true;
}


/****************************************************************************
 ImgAttrController::InitImageList()

 Creates the image list if necessary and initializes it.
****************************************************************************/
void ImgAttrController::InitImageList(HWND hParent)
{
	if (m_pImageList == NULL)
	{
		m_pImageList = new ImageList();
	}
	m_pImageList->InitImageList(hParent);
}


/****************************************************************************
 ImgAttrController::GetImageList()

 Builds the image/material list based on the current selection or world.
****************************************************************************/
bool ImgAttrController::GetImageList(Interface* pInterface, bool viewAll, bool showMaterials)
{
  return m_pImageList->GetImageList(pInterface, viewAll, showMaterials);
}


/****************************************************************************
 ImgAttrController::DisplayImageList()

 Updates the image list box.
****************************************************************************/
void ImgAttrController::DisplayImageList()
{
  if (m_pImageList)
    m_pImageList->Display();
}


//-----------------------------------------------------------------------------
// ImgAttrController::HandleApplySelect
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
//-----------------------------------------------------------------------------
void ImgAttrController::HandleApplySelect(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll)
{
	// If the selection is to be applied to the whole scene, then load the
	// current attrib data for the whole scene:
	if (bApplyToAll == true)
	{
		this->LoadCurrAttribDataScene();
	}

	// Tell the ImgAttrController to get the current attribute data:
	CurrAttribController* pCurrAttribController = this->GetCurrAttribController();
	if (pCurrAttribController != NULL)
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
		IntTab imagesToSel;
		int nNumObjects = pCurrAttribController->GetSize();
		for (int nObjIndex = 0; nObjIndex < nNumObjects; nObjIndex++)
		{
			CurrAttribData* pCurrAttribData = pCurrAttribController->GetObjectPtr(nObjIndex);
			if (this->ImageMatchesCheckedItems(pCurrAttribData, checkedKeyKeeper) == true)
			{
				int imageIndex = m_pImageList->GetImageIndex(pCurrAttribData->GetEntityString());
				imagesToSel.Append(1, &imageIndex);
			}
		}

		// Now change the selection set:
		m_pImageList->SelectImages(imagesToSel);
		m_pOwner->UpdateAttribDisplay();

#if 0
		// The following code has been compiled out because it only performs an
		// or on the keys specified in the keyval panel.  We wanted it to 
		// perform as an and, which the above code does.  If it's ever deemed
		// necessary to perform an or as well as an and, then the following
		// code can be used.
		// Set up our vector of object indices:
		std::vector<short> vectObjIndices;
		int nNumObjects = pCurrAttribController->GetSize();
		for (int nObjIndex = 0; nObjIndex < nNumObjects; nObjIndex++)
		{
			vectObjIndices.push_back(0);
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
						this->BuildObjIndices(keyKeeper[nKeyIndex].m_strKeyname,
											  keyKeeper[nKeyIndex].m_valKeeper[nValIndex].m_strVal,
											  vectObjIndices, pCurrAttribController);
						bValChecked = true;
					}
				}

				if (bValChecked == false)
				{
					// Select objects based on the key only:
					std::string nullVal = "";
					this->BuildObjIndices(keyKeeper[nKeyIndex].m_strKeyname,
										  nullVal, vectObjIndices, pCurrAttribController);
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
				INode* pNodeToAdd = (INode*) (pCurrAttribController->GetObjectPtr(nLoop)->GetEntityPtr());
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


//-----------------------------------------------------------------------------
// ImgAttrController::ObjectMatchesCheckedItems
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
//-----------------------------------------------------------------------------
bool ImgAttrController::ImageMatchesCheckedItems(CurrAttribData* pCurrAttribData, Keeper<KeyInfo>& checkedKeyKeeper)
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


//-----------------------------------------------------------------------------
// ImgAttrController::BuildObjIndices
// This method will look at the key and val strings coming in and see what
// objects in the current attribute controller matches them.  Any that match
// are marked with a 1 at the correct index in the vector of indices.  If the
// val string coming in is null, then only the key is used to find matching
// objects.
//
// Input:
//  std::string strKey -- The key to match against.
//  std::string strVal -- The val to match against.
//  std::vector<short>& vectObjIndices -- The vector of object indices.
//	CurrAttribController* pCurrAttribController -- The current attribute data
//												   to look in.
//
// Output: None.
//-----------------------------------------------------------------------------
void ImgAttrController::BuildImgIndices(std::string strKey, std::string strVal, std::vector<short>& vectObjIndices, CurrAttribController* pCurrAttribController)
{
/*
	// Cycle through the object indices and only look at those not yet marked:
	int nNumObjects = vectObjIndices.size();
	for (int nObjIndex = 0; nObjIndex < nNumObjects; nObjIndex++)
	{
		if (vectObjIndices.at(nObjIndex) == 0)
		{
			// Check to see if this object matches:
			CurrAttribData* pCurrAttribData = pCurrAttribController->GetObjectPtr(nObjIndex);
			int nKeyIndex = pCurrAttribData->KeynameExistsAt(strKey);
			if (nKeyIndex > -1)
			{
				// Key matches, see if the val should be matched:
				if (strVal != "")
				{
					if (pCurrAttribData->GetValueAsString(nKeyIndex) == strVal)
					{
						// Object matches based on key and val:
						vectObjIndices.at(nObjIndex) = 1;
					}
				}
				else
				{
					// Object matches based on key only:
					vectObjIndices.at(nObjIndex) = 1;
				}
			}
		}
	}
*/
}


//-----------------------------------------------------------------------------
// ImgAttrController::HandleApplyApply
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
//-----------------------------------------------------------------------------
void ImgAttrController::HandleApplyApply(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll)
{
	// If the Apply is to be applied to the whole scene, then load the
	// current attrib data for the whole scene:
	if (bApplyToAll == true)
	{
		this->LoadCurrAttribDataScene();
	}

	// Get the current attrib data:
	CurrAttribController* pCurrAttribController = this->GetCurrAttribController();
	if (pCurrAttribController != NULL)
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
		int nNumCurrAttribDatas = pCurrAttribController->GetSize();
		for (int nCADataIndex = 0; nCADataIndex < nNumCurrAttribDatas; nCADataIndex++)
		{
			CurrAttribData* pCurrAttribData = pCurrAttribController->GetObjectPtr(nCADataIndex);
			int nNumKeeperKeys = checkedKeyKeeper.GetSize();
			for (int nKeeperKeyIndex = 0; nKeeperKeyIndex < nNumKeeperKeys; nKeeperKeyIndex++)
			{
				pCurrAttribData->RemoveKeyname(checkedKeyKeeper[nKeeperKeyIndex].m_strKeyname);
				pCurrAttribData->SetStringAsString(checkedKeyKeeper[nKeeperKeyIndex].m_strKeyname,
												   checkedKeyKeeper[nKeeperKeyIndex].m_valKeeper[0].m_strVal);
			}
			
			// Now set the data out on the actual node:
			const char *pName = m_pCurrAttribController->GetEntityString(nCADataIndex);
			TSTR buf;
			m_pImageAttribNode->GetUserPropBuffer(buf);
			char* pChObjectBuf = buf.data();
			std::string strFullData = pChObjectBuf;
			std::string strAttribData = m_pCurrAttribController->CurrObjDataToStr(nCADataIndex);
			AddRawDataToString(pName, strFullData, strAttribData);
			m_pImageAttribNode->SetUserPropBuffer(strFullData.data());
		}

		// Now reset the selection set so that data gets loaded correctly and
		// the dialogs get updated correctly:
		m_pOwner->UpdateAttribDisplay();
	}
}


//-----------------------------------------------------------------------------
// ImgAttrController::HandleApplyPurge
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
//-----------------------------------------------------------------------------
void ImgAttrController::HandleApplyPurge(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll)
{
	// If the purging is to be applied to the whole scene, then load the
	// current attrib data for the whole scene:
	if (bApplyToAll == true)
	{
		this->LoadCurrAttribDataScene();
	}

	// Get the current attrib data:
	CurrAttribController* pCurrAttribController = this->GetCurrAttribController();
	if (pCurrAttribController != NULL)
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
		int nNumCurrAttribDatas = pCurrAttribController->GetSize();
		for (int nCADataIndex = 0; nCADataIndex < nNumCurrAttribDatas; nCADataIndex++)
		{
			CurrAttribData* pCurrAttribData = pCurrAttribController->GetObjectPtr(nCADataIndex);
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
			const char *pName = m_pCurrAttribController->GetEntityString(nCADataIndex);
			TSTR buf;
			m_pImageAttribNode->GetUserPropBuffer(buf);
			char* pChObjectBuf = buf.data();
			std::string strFullData = pChObjectBuf;
			std::string strAttribData = m_pCurrAttribController->CurrObjDataToStr(nCADataIndex);
			AddRawDataToString(pName, strFullData, strAttribData);
			m_pImageAttribNode->SetUserPropBuffer(strFullData.data());
		}

		// Now reset the selection set so that data gets loaded correctly and
		// the dialogs get updated correctly:
		m_pOwner->UpdateAttribDisplay();
	}
}


//-----------------------------------------------------------------------------
// ImgAttrController::HandleApplyRemap
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
//-----------------------------------------------------------------------------
void ImgAttrController::HandleApplyRemap(Keeper<KeyInfo>& keyKeeper, bool bApplyToAll)
{
	// If the remapping is to be applied to the whole scene, then load the
	// current attrib data for the whole scene:
	if (bApplyToAll == true)
	{
		this->LoadCurrAttribDataScene();
	}

	// Get the current attrib data:
	CurrAttribController* pCurrAttribController = this->GetCurrAttribController();
	if (pCurrAttribController != NULL)
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
		int nNumCurrAttribDatas = pCurrAttribController->GetSize();
		for (int nCADataIndex = 0; nCADataIndex < nNumCurrAttribDatas; nCADataIndex++)
		{
			CurrAttribData* pCurrAttribData = pCurrAttribController->GetObjectPtr(nCADataIndex);
			int nNumKeeperKeys = checkedKeyKeeper.GetSize();
			for (int nKeeperKeyIndex = 0; nKeeperKeyIndex < nNumKeeperKeys; nKeeperKeyIndex++)
			{
				// Check to see if this node has this key:
				int nNodeKeyIndex = pCurrAttribData->KeynameExistsAt(checkedKeyKeeper[nKeeperKeyIndex].m_strKeyname);
				if (nNodeKeyIndex > -1)
				{
					bool bRemapKey = false;
					bool bRemapVal = false;
					std::string strOrigKey = checkedKeyKeeper[nKeeperKeyIndex].m_strKeyname;
					std::string strNewKey = "";
					std::string strOrigVal = "";
					std::string strNewVal = "";

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
							pCurrAttribData->SetStringAsString(strNewKey, strNewVal);
						}
						else
						{
							// Remap the key, don't remap the val:
							pCurrAttribData->RemoveKeyname(strOrigKey);
							pCurrAttribData->SetStringAsString(strNewKey, strOrigVal);
						}
					}
					else
					{
						if (bRemapVal == true)
						{
							// Don't remap the key, remap the val:
							pCurrAttribData->RemoveKeyname(strOrigKey);
							pCurrAttribData->SetStringAsString(strOrigKey, strNewVal);
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
			const char *pName = m_pCurrAttribController->GetEntityString(nCADataIndex);
			TSTR buf;
			m_pImageAttribNode->GetUserPropBuffer(buf);
			char* pChObjectBuf = buf.data();
			std::string strFullData = pChObjectBuf;
			std::string strAttribData = m_pCurrAttribController->CurrObjDataToStr(nCADataIndex);
			AddRawDataToString(pName, strFullData, strAttribData);
			m_pImageAttribNode->SetUserPropBuffer(strFullData.data());
		}

		// Now reset the selection set so that data gets loaded correctly and
		// the dialogs get updated correctly:
		m_pOwner->UpdateAttribDisplay();
	}
}