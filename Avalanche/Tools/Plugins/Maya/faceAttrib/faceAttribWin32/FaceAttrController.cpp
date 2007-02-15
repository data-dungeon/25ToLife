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
#include "StdAfx.h"
#include "FaceAttrController.h"
#include "CurrMeshAttribData.h"
#include "CurrAttribController.h"
#include "KeyValCommunicator.h"
#include "ApplyColorsCommunicator.h"

#include "ConfigFilesManager.h"

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
	m_pApplyColorsComm = NULL;
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
	if (m_pApplyColorsComm != NULL)
	{
		delete m_pApplyColorsComm;
		m_pApplyColorsComm = NULL;
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

	// create the apply colors communicator if not already done
	if (m_pApplyColorsComm == NULL)
	{
		m_pApplyColorsComm = new ApplyColorsCommunicator(this);
	}
	TupString applyRegistryName = APPLY_COLORS_KEY;
	applyRegistryName += "\\";
	if (configFilesManager.GetCurrentConfigIndex()>=0)
	{
		applyRegistryName += configFilesManager.GetName(configFilesManager.GetCurrentConfigIndex());
	}
	else
	{
		applyRegistryName += "Dummy";
	}
	m_pApplyColorsComm->InitializeColorSets(applyRegistryName,pTupAggConfigData);

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
	MStatus status;
	MSelectionList slist;
	MGlobal::getActiveSelectionList( slist );
	int numSelectionNodes = slist.length();

	// Get rid of any current attribute data that may exist:
	if (m_pCurrAttribController != NULL)
	{
		m_pCurrAttribController->RemoveAll();
	}
	else
	{
		m_pCurrAttribController = new CurrAttribController();
	}

	// update dependency graph
	MGlobal::executeCommand(MString("dgdirty -a"));
	
	int nodeIndex;
	for (nodeIndex=0;nodeIndex<numSelectionNodes;nodeIndex++)
	{
		TupString pathString;
		MDagPath objectPath;
		MObject component;
		// get the selected node
		slist.getDagPath(nodeIndex,objectPath,component);
		pathString = objectPath.fullPathName().asChar();

		if (!CurrAttribController::ValidNode(objectPath))
			continue;

		MFnMesh fnMesh(objectPath, &status);
		if (status != MS::kSuccess)
			continue;
		
		int numPolys = fnMesh.numPolygons();
		if (numPolys<=0)
			continue;

		MFn::Type componentType = component.apiType();
		if (componentType!=MFn::kMeshPolygonComponent)
			continue;

		TupArray<int> faceIndices;
		
		MItMeshPolygon polyIter(objectPath,component);
		for (;!polyIter.isDone();polyIter.next())
		{
			int faceIndex = polyIter.index();
			faceIndices.Add(faceIndex);
		}
		int numSelectedFaces = faceIndices.GetSize();
		if (numSelectedFaces==0)
			continue;

		MObject mObject = objectPath.node();
		int objectIndex = m_pCurrAttribController->GetIndex(mObject);
		// if this one is not in the controller already
		if (objectIndex==-1)
		{
			CurrMeshAttribData* pNewAttribData = new CurrMeshAttribData();
		
			// Set the object pointer:
			objectIndex = m_pCurrAttribController->AddMayaObject(mObject);

			TupString strFullData;
			CurrAttribController::GetNote(mObject,strFullData);
			
			// Get to just the raw data substring:
			if (StripAllButRawData(strFullData) != true)
			{
				strFullData = ""; // clear if no header
			}
			// Now pass on the string to load it into the current attrib data:
			pNewAttribData->StrToCurrAttribData(strFullData);
			int numSets = pNewAttribData->GetNumAttribSets();
			pNewAttribData->SetNumFaces(numPolys);

			// set up an array of ints for the faces
			TupArray<int> polyFaceSets;
			polyFaceSets.SetSize(numPolys);
			int polyIndex;
			for (polyIndex = 0; polyIndex<numPolys; polyIndex ++ ) 
			{ 
				polyFaceSets[polyIndex] = 0; // start them all at 0
			}

			if (fnMesh.isBlindDataTypeUsed( BLIND_DATA_ID ))
			{
				if (fnMesh.hasBlindData(MFn::kMeshPolygonComponent,BLIND_DATA_ID))
				{
					MIntArray compIDArray;
					MIntArray compIntDataArray;
					status = fnMesh.getIntBlindData( MFn::kMeshPolygonComponent, BLIND_DATA_ID,BLIND_DATA_LONG_NAME,compIDArray,compIntDataArray); 
					if (status == MStatus::kSuccess)
					{
						int numCompAttribs = compIntDataArray.length();
						for (int compIndex=0;compIndex<numCompAttribs;compIndex++)
						{
							int compID = compIDArray[compIndex];
							int value = compIntDataArray[compIndex];
							if (compID>=0&&compID<numPolys)
							{
								polyFaceSets[compID] = value;
							}
						}
					}
				}
			}
			// set up the face attribute indexes
			for (polyIndex = 0; polyIndex<numPolys; polyIndex ++ ) 
			{
				int polySet = polyFaceSets[polyIndex];
				if (polySet<0 || polySet>=numSets)
				{
					polySet = 0;
					pNewAttribData->SetChangedFace(polyIndex,true); // mark any that are out of bounds to be set later
				}
				pNewAttribData->SetFaceAttribSet(polyIndex,polySet);
			}
			// add the selected faces
			for (polyIndex=0;polyIndex<numSelectedFaces	;polyIndex++)
			{
				pNewAttribData->AddSelectedFace(faceIndices[polyIndex]);
			}
		
			// Add the CurrAttribData object to the m_pCurrAttribController:
			m_pCurrAttribController->AddCurrMeshAttribData(pNewAttribData);
		}
		else
		{
			// if another selection from a previous mObject
			CurrMeshAttribData* pAttribData = m_pCurrAttribController->GetCurrMeshAttribData(objectIndex);
			for (int polyIndex=0;polyIndex<numSelectedFaces;polyIndex++)
			{
				pAttribData->AddSelectedFace(faceIndices[polyIndex]);
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
				SetMayaData(nMeshIndex);
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

	MStringArray longNames,shortNames,formatNames;
	longNames.append( BLIND_DATA_LONG_NAME ); 
	shortNames.append( BLIND_DATA_SHORT_NAME ); 
	formatNames.append( BLIND_DATA_FORMAT_NAME ); 

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

					if (nFlags & UIDisplayBase::WIN_MESSAGE_FLAG_PURGE_KEYS)
					{
						// Now purge any of the specified keys:
						int nNumPurgeKeys = vectKeysToPurge.GetSize();
						for (int nLoop = 0; nLoop < nNumPurgeKeys; nLoop++)
						{
							if (m_pCurrAttribController->RemoveKeyInstances(vectKeysToPurge[nLoop]))
							{
								bUpdateData = true;
							}
						}
					}

					// set key value pair
					if (nFlags & UIDisplayBase::WIN_MESSAGE_FLAG_SET_KEY_VALUE)
					{
						if (m_pCurrAttribController->AddKeyValue(strKeyname, strValue))
						{
							bUpdateData = true;
						}
					}

					// check list
					if (nFlags & UIDisplayBase::WIN_MESSAGE_FLAG_FORM_CHECK_LIST)
					{
						bool bChecked = (nFlags & UIDisplayBase::WIN_MESSAGE_FLAG_CHECK_LIST_CHECKED) ? true : false;
//						m_pCurrAttribController->FormCheckListVal(strKeyname, strValue, bChecked);
						bUpdateData = true;
					}

					if (bUpdateData)
					{
						// Set the data on the object:
						int nNumObjects = m_pCurrAttribController->GetSize();
						for (int nIndex = 0; nIndex < nNumObjects; nIndex++)
						{
							MObject mObject = m_pCurrAttribController->GetMayaObject(nIndex);
							TupString strFullData;
							CurrAttribController::GetNote(mObject,strFullData);
							TupString strAttribData = m_pCurrAttribController->CurrObjDataToStr(nIndex);
							AddRawDataToString(strFullData, strAttribData);
							CurrAttribController::CreateOrReplaceNote(mObject,strFullData);						

							// place the ints on the mesh faces
							MStatus status;
							MFnDagNode fnDagNode(mObject);
							MDagPath dagPath;
							status = fnDagNode.getPath( dagPath );
							MFnMesh fnMesh(dagPath);
							if (!fnMesh.isBlindDataTypeUsed(BLIND_DATA_ID))
							{
								status = fnMesh.createBlindDataType ( BLIND_DATA_ID,longNames,shortNames,formatNames) ;
							}
							CurrMeshAttribData *pCurrMeshAttribData = m_pCurrAttribController->GetCurrMeshAttribData(nIndex);
							int numFaces = pCurrMeshAttribData->GetNumFaces();
							MIntArray polyArray;
							MIntArray polyDataArray;
							int polyIndex;
							for (polyIndex = 0; polyIndex<numFaces; polyIndex++) 
							{ 
								if (pCurrMeshAttribData->FaceChanged(polyIndex))
								{
									polyArray.append( polyIndex ); 
									polyDataArray.append(pCurrMeshAttribData->GetFaceAttributeSet(polyIndex));
								}
							}
							if (polyArray.length()) // if any faces changed
							{
								status = fnMesh.setIntBlindData( polyArray, MFn::kMeshPolygonComponent,BLIND_DATA_ID,BLIND_DATA_LONG_NAME,polyDataArray); 
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

//-----------------------------------------------------------------------------
// FaceAttrController::StripAllButRawData
// This method will take the incoming string and strip out everything but the
// actual raw attribute data substring.
//
// Input:
//  TupString& strToFix -- String to strip.
//
// Output: True if both of the begin and end block indicators were found, false
//		   otherwise.
//-----------------------------------------------------------------------------
bool FaceAttrController::StripAllButRawData(TupString &strToFix)
{
	// handle different combinations of new lines gracefully
	strToFix.Replace("\r\n","\r");
	strToFix.Replace("\n\r","\r");
	strToFix.Replace("\n","\r");

	// Find and remove the begin block indicator:
	int beginIndex = strToFix.Find(BEGIN_BLOCK);
	if (beginIndex == -1)
	{
		return false;
	}
	int newLineIndex = strToFix.Find('\r',beginIndex);
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
// FaceAttrController::AddRawDataToString
// This method will take the strToInsert and add it into the appropriate place
// in the strToAddTo.  It does this by trying to find the begin/end block
// indicators and replacing any substring that may be between the indicators.
// If no indicators are found, new ones are created at the end of the string,
// with the strToInsert added in between them.
//
// Input:
//  TupString& strToAddTo -- String to insert into.
//  const TupString& strToInsert -- Sting to add.
//
// Output: None.
//------------------------------------------------------------------------
void FaceAttrController::AddRawDataToString(TupString& strToAddTo, const TupString& strToInsert)
{
	// handle different combinations of new lines gracefully
	strToAddTo.Replace("\r\n","\r");
	strToAddTo.Replace("\n\r","\r");
	strToAddTo.Replace("\n","\r");

	int beginIndex = strToAddTo.Find(BEGIN_BLOCK);
	int endIndex = strToAddTo.Find(END_BLOCK);
	if (beginIndex == -1 && endIndex == -1)
	{
		// Add a newline to the end of the string if needed:
		int nStrSize = strToAddTo.GetLength();
		int newLineIndex = strToAddTo.ReverseFind('\r');
		if (newLineIndex != (nStrSize - 1))
			strToAddTo+="\r";

		// Add both of the indicators to the end of the string, with the data between:
		strToAddTo+=BEGIN_BLOCK;
		strToAddTo+="\r";
		strToAddTo+=strToInsert;
		strToAddTo+="\r";
		strToAddTo+=END_BLOCK;
		strToAddTo+="\r";
	}
	else if (beginIndex != -1 && endIndex != -1)
	{
		// Replace any data between the block indicators with the new data:
		int newLineIndex = strToAddTo.Find('\r', beginIndex);
		strToAddTo.Delete(newLineIndex+1, endIndex - (newLineIndex + 1));
		strToAddTo.Insert(newLineIndex+1, strToInsert);
	}
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
// FaceAttrController::InitApplyColorsPanel
// This method will tell the ApplyColorsCommunicator to set up the ApplyColorsPanel
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
void FaceAttrController::InitApplyColorsPanel(ICustomRollupWindow* pCustomRollupWindow, HINSTANCE hInstance, LPARAM lParam)
{
	if (m_pApplyColorsComm == NULL)
	{
		m_pApplyColorsComm = new ApplyColorsCommunicator(this);
	}
	m_pApplyColorsComm->InitApplyColorsPanel(pCustomRollupWindow, hInstance, lParam);
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
	if (m_pApplyColorsComm != NULL)
		m_pApplyColorsComm->IdCancelMessage();
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
		MSelectionList selectionList;
		int nNumMeshes = m_pCurrAttribController->GetSize();
		for (int nMeshIndex = 0; nMeshIndex < nNumMeshes; nMeshIndex++)
		{
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
			MObject mObject = m_pCurrAttribController->GetMayaObject(nMeshIndex);
			MFnDagNode dagNode(mObject);
			MDagPathArray dagPathArray;
			dagNode.getAllPaths(dagPathArray);
			int numPaths = dagPathArray.length();
			for (int pathIndex=0;pathIndex<numPaths;pathIndex++)
			{
				MItMeshPolygon polyIter(dagPathArray[pathIndex]);
				for (;!polyIter.isDone();polyIter.next())
				{
					int faceIndex = polyIter.index(); // the index for this face
					int faceSet = pCurrMeshAttribData->GetFaceAttributeSet(faceIndex); // which set to use
					if (setMatchArray[faceSet]) // is this set one we need to select
					{
						MObject faceComponent = polyIter.polygon();
						selectionList.add(dagPathArray[pathIndex],faceComponent);
					}

				}
			}
		}
		// Now change the selection set:
		MGlobal::setActiveSelectionList(selectionList);
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
											  vectObjIndices, m_pCurrAttribController);
						bValChecked = true;
					}
				}

				if (bValChecked == false)
				{
					// Select objects based on the key only:
					TupString nullVal = "";
					BuildObjIndices(keyKeeper[nKeyIndex].m_strKeyname,
										  nullVal, vectObjIndices, m_pCurrAttribController);
				}
			}
		}

		// Create the selection set based on the vector of object indices:
		bool bObjsToSelect = false;
		MSelectionList selectionList;
		int nNumObjs = vectObjIndices.GetSize();
		for (int nLoop = 0; nLoop < nNumObjs; nLoop++)
		{
			if (vectObjIndices[nLoop] == 1)
			{
				MObject mObject = m_pCurrAttribController->GetMayaObject(nObjIndex);
				MFnDagNode dagNode(mObject);
				MDagPathArray dagPathArray;
				dagNode.getAllPaths(dagPathArray);
				int numPaths = dagPathArray.length();
				for (int pathIndex=0;pathIndex<numPaths;pathIndex++)
				{
					selectionList.add(dagPathArray[pathIndex]);
				}
				bObjsToSelect = true;
			}
		}
		if (bObjsToSelect == true)
		{
			MGlobal::setActiveSelectionList(selectionList);
		}
#endif
	}
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
// FaceAttrController::BuildObjIndices
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
void FaceAttrController::BuildObjIndices(TupString strKey, TupString strVal,
										TupArray<short>& vectObjIndices)
{
#if 0
	
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
#endif
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
			SetMayaData(nCADataIndex);
		}
		// Now reset the selection set so that data gets loaded correctly and
		// the dialogs get updated correctly:
		MSelectionList selectionList;
		MGlobal::getActiveSelectionList(selectionList);
		MGlobal::setActiveSelectionList(selectionList);
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
			SetMayaData(nCADataIndex);
		}
		// Now reset the selection set so that data gets loaded correctly and
		// the dialogs get updated correctly:
		MSelectionList selectionList;
		MGlobal::getActiveSelectionList(selectionList);
		MGlobal::setActiveSelectionList(selectionList);
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
			SetMayaData(nCADataIndex);
		}
		// Now reset the selection set so that data gets loaded correctly and
		// the dialogs get updated correctly:
		MSelectionList selectionList;
		MGlobal::getActiveSelectionList(selectionList);
		MGlobal::setActiveSelectionList(selectionList);
	}
}

void FaceAttrController::SetMayaData(int nIndex)
{
	MStringArray longNames,shortNames,formatNames;
	longNames.append( BLIND_DATA_LONG_NAME ); 
	shortNames.append( BLIND_DATA_SHORT_NAME ); 
	formatNames.append( BLIND_DATA_FORMAT_NAME ); 
	
	MObject mObject = m_pCurrAttribController->GetMayaObject(nIndex);
	TupString strFullData;
	CurrAttribController::GetNote(mObject,strFullData);
	TupString strAttribData = m_pCurrAttribController->CurrObjDataToStr(nIndex);
	AddRawDataToString(strFullData, strAttribData);
	CurrAttribController::CreateOrReplaceNote(mObject,strFullData);						
	
	// place the ints on the mesh faces
	MStatus status;
	MFnDagNode fnDagNode(mObject);
	MDagPath dagPath;
	status = fnDagNode.getPath( dagPath );
	MFnMesh fnMesh(dagPath);
	if (!fnMesh.isBlindDataTypeUsed(BLIND_DATA_ID))
	{
		status = fnMesh.createBlindDataType ( BLIND_DATA_ID,longNames,shortNames,formatNames) ;
	}
	CurrMeshAttribData *pCurrMeshAttribData = m_pCurrAttribController->GetCurrMeshAttribData(nIndex);
	int numFaces = pCurrMeshAttribData->GetNumFaces();
	MIntArray polyArray;
	MIntArray polyDataArray;
	int polyIndex;
	for (polyIndex = 0; polyIndex<numFaces; polyIndex++) 
	{ 
		if (pCurrMeshAttribData->FaceChanged(polyIndex))
		{
			polyArray.append( polyIndex ); 
			polyDataArray.append(pCurrMeshAttribData->GetFaceAttributeSet(polyIndex));
		}
	}
	if (polyArray.length()) // if any faces changed
	{
		status = fnMesh.setIntBlindData( polyArray, MFn::kMeshPolygonComponent,BLIND_DATA_ID,BLIND_DATA_LONG_NAME,polyDataArray); 
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
	MStatus status;
	MSelectionList slist;
	MGlobal::getHiliteList( slist );
	int numSelectionNodes = slist.length();

	// Get rid of any current attribute data that may exist:
	if (m_pCurrAttribController != NULL)
	{
		m_pCurrAttribController->RemoveAll();
	}
	else
	{
		m_pCurrAttribController = new CurrAttribController();
	}
	
	int nodeIndex;
	for (nodeIndex=0;nodeIndex<numSelectionNodes;nodeIndex++)
	{
		// the hilight list gives us a list of transforms that are selected
		// we need to get the child meshes that are used
		MDagPath transformPath;
		// get the selected node
		slist.getDagPath(nodeIndex,transformPath);

		if (transformPath.apiType()!=MFn::kTransform)
			continue;

		bool bFoundMesh = false;
		MDagPath objectPath;
		int numChildren = transformPath.childCount();
		for (int childIndex=0;childIndex<numChildren;childIndex++)
		{
			MObject childObject = transformPath.child(childIndex);
			if (childObject.apiType()==MFn::kMesh)
			{
				bFoundMesh = true;
				objectPath = transformPath;
				objectPath.push(childObject);
				break;
			}
		}
		if (!bFoundMesh)
			continue;

		MFnMesh fnMesh(objectPath, &status);
		if (status != MS::kSuccess)
			continue;
		
		int numPolys = fnMesh.numPolygons();
		if (numPolys<=0)
			continue;

		MObject mObject = objectPath.node();
		int objectIndex = m_pCurrAttribController->GetIndex(mObject);
		// if this one is not in the controller already
		if (objectIndex==-1)
		{
			CurrMeshAttribData* pNewAttribData = new CurrMeshAttribData();
		
			// Set the object pointer:
			objectIndex = m_pCurrAttribController->AddMayaObject(mObject);

			TupString strFullData;
			CurrAttribController::GetNote(mObject,strFullData);
			
			// Get to just the raw data substring:
			if (StripAllButRawData(strFullData) != true)
			{
				strFullData = ""; // clear if no header
			}
			// Now pass on the string to load it into the current attrib data:
			pNewAttribData->StrToCurrAttribData(strFullData);
			int numSets = pNewAttribData->GetNumAttribSets();
			pNewAttribData->SetNumFaces(numPolys);

			// set up an array of ints for the faces
			TupArray<int> polyFaceSets;
			polyFaceSets.SetSize(numPolys);
			int polyIndex;
			for (polyIndex = 0; polyIndex<numPolys; polyIndex ++ ) 
			{ 
				polyFaceSets[polyIndex] = 0; // start them all at 0
			}

			if (fnMesh.isBlindDataTypeUsed( BLIND_DATA_ID ) && 
				fnMesh.hasBlindData(MFn::kMeshPolygonComponent,BLIND_DATA_ID))
			{
				MIntArray compIDArray;
				MIntArray compIntDataArray;
				status = fnMesh.getIntBlindData( MFn::kMeshPolygonComponent, BLIND_DATA_ID,BLIND_DATA_LONG_NAME,compIDArray,compIntDataArray); 
				if (status == MStatus::kSuccess)
				{
					int numCompAttribs = compIntDataArray.length();
					for (int compIndex=0;compIndex<numCompAttribs;compIndex++)
					{
						int compID = compIDArray[compIndex];
						int value = compIntDataArray[compIndex];
						if (compID>=0&&compID<numPolys)
						{
							polyFaceSets[compID] = value;
						}
					}
				}
			}
			// set up the face attribute indexes
			for (polyIndex = 0; polyIndex<numPolys; polyIndex ++ ) 
			{
				int polySet = polyFaceSets[polyIndex];
				if (polySet<0 || polySet>=numSets)
				{
					polySet = 0;
					pNewAttribData->SetChangedFace(polyIndex,true); // mark any that are out of bounds to be set later
				}
				pNewAttribData->SetFaceAttribSet(polyIndex,polySet);
				pNewAttribData->AddSelectedFace(polyIndex);
			}
		
			// Add the CurrAttribData object to the m_pCurrAttribController:
			m_pCurrAttribController->AddCurrMeshAttribData(pNewAttribData);
		}
	}
	return (m_pCurrAttribController->GetSize()!=0);
}
