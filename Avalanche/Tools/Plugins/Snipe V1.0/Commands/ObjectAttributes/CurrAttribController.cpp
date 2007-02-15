//-----------------------------------------------------------------------------
//	File:		CurrAttribController.cpp
//	Created:	Feb. 6, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The CurrAttribController class handles multiple CurrAttribData
//			objects, along with the storing, getting, setting, etc. of these
//			objects.
//
//	History:	2/06/2002: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Includes:

#include "StdAfx.h"
#include "CurrAttribController.h"
#include "CurrAttribData.h"

//-----------------------------------------------------------------------------
// Defines, etc.:

#define ARBITRARY_LARGE_NUM	5000

//-----------------------------------------------------------------------------
// Class Definition:


//-----------------------------------------------------------------------------
// CurrAttribController::CurrAttribController
// This method is the default constructor for this class.
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
CurrAttribController::CurrAttribController()
{
}


//-----------------------------------------------------------------------------
// CurrAttribController::~CurrAttribController
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
CurrAttribController::~CurrAttribController()
{
	RemoveAll();
}

//-----------------------------------------------------------------------------
// CurrAttribController::GetSize
// This method will return the number of CurrAttribData objects in the vector.
//
// Input: None.
//
// Output: int representing the number of objects.
//-----------------------------------------------------------------------------
int CurrAttribController::GetSize(void) const
{
	return m_vectAttribDataPtrs.GetSize();
}


//-----------------------------------------------------------------------------
// CurrAttribController::GetIndex
// This method will return index in the keeper for the given mObject
//
// Input: HSnipeObject *pObject
//
// Output: int representing the index into the Snipe objects keeper (-1) if not found
//-----------------------------------------------------------------------------
int CurrAttribController::GetIndex(HSnipeObject *pObject) const
{
	return m_snipeObjects.GetIndex(pObject);
}

//-----------------------------------------------------------------------------
// CurrAttribController::GetSnipeObject
// This method will return the MObject in the keeper for a given index
//
// Input: int index
//
// Output: MObject from the given index
//-----------------------------------------------------------------------------
HSnipeObject *CurrAttribController::GetSnipeObject(int index) const
{
	assert(index>=0 && index<m_snipeObjects.GetSize());
	return m_snipeObjects[index];
}

//-----------------------------------------------------------------------------
// CurrAttribController::AddMayaObject
// This method will add the mObject to the keeper
//
// Input: MObject
//
// Output: int representing the index into the maya objects keeper
//-----------------------------------------------------------------------------
int CurrAttribController::AddSnipeObject(HSnipeObject *pObject)
{
	return m_snipeObjects.Add(pObject);
}

//-----------------------------------------------------------------------------
// CurrAttribController::RemoveAll
// This method will clear the maya objects keeper
//
// Input: none
//
// Output: none
//-----------------------------------------------------------------------------
void CurrAttribController::RemoveAll(void)
{
	m_snipeObjects.RemoveAll();
	// delete attrib data from array
	int nVectSize = m_vectAttribDataPtrs.GetSize();
	for (int nIndex = 0; nIndex < nVectSize; nIndex++)
	{
		if (m_vectAttribDataPtrs[nIndex] != NULL)
		{
			delete m_vectAttribDataPtrs[nIndex];
			m_vectAttribDataPtrs[nIndex] = NULL;
		}
	}
	m_vectAttribDataPtrs.RemoveAll();
}

//-----------------------------------------------------------------------------
// CurrAttribController::GetCurrAttribData
// This method will return the CurrAttribData pointer specified by the index
// passed in.
//
// Input:
//  int nIndex -- The index of the pointer to get.
//
// Output: CurrAttribData* to get.  NOTE: If the index passed in is out of the
//		   vector's bounds, then NULL is passed back.
//-----------------------------------------------------------------------------
CurrAttribData* CurrAttribController::GetCurrAttribData(int nIndex)
{
	int nVectSize = m_vectAttribDataPtrs.GetSize();
	if (nIndex >= 0 && nIndex < nVectSize)
		return m_vectAttribDataPtrs[nIndex];
	else
		return NULL;
}

//-----------------------------------------------------------------------------
// CurrAttribController::GetCurrAttribData
// This method will return the CurrAttribData pointer specified by the index
// passed in.
//
// Input:
//  int nIndex -- The index of the pointer to get.
//
// Output: CurrAttribData* to get.  NOTE: If the index passed in is out of the
//		   vector's bounds, then NULL is passed back.
//-----------------------------------------------------------------------------
const CurrAttribData* CurrAttribController::GetCurrAttribData(int nIndex) const
{
	int nVectSize = m_vectAttribDataPtrs.GetSize();
	if (nIndex >= 0 && nIndex < nVectSize)
		return m_vectAttribDataPtrs[nIndex];
	else
		return NULL;
}

//-----------------------------------------------------------------------------
// CurrAttribController::GetAllUniqueValues
// This method will look through all the CurrAttribData objects for the key
// specified, it will then return all the unique values for the key.
//
// Input:
//  TupString strKey -- The key to get the values for.
//  TupString strDefaultValue -- the default value for the key
//  UniqueValues uniqueValues -- the class to update with the keys as well as stats for the selection
//
// Output: none
//
//-----------------------------------------------------------------------------
void CurrAttribController::GetAllUniqueValues(const TupString &strKey, const TupString &strDefaultValue, UniqueValues &uniqueValues, Keeper<TupString> *pValidValues) const
{
	// First, start with no values
	uniqueValues.m_uniqueValues.RemoveAll();

	// we have not added the default value to the array
	bool bDefaultAdded = false;

	// Cycle through all of the CurrAttribData objects:
	int nNumObjects = m_vectAttribDataPtrs.GetSize();
	for (int nIndex = 0; nIndex < nNumObjects; nIndex++)
	{
		bool bValidValue = false;

		int nKeyAt = (m_vectAttribDataPtrs[nIndex])->KeynameExistsAt(strKey);
		// if this object has a value to go with the key
		if (nKeyAt >= 0)
		{
			TupString strValue = (m_vectAttribDataPtrs[nIndex])->GetValueAsString(nKeyAt);
			// make sure this value is ok to have (based on the valid values keeper)
			if (!pValidValues || pValidValues->GetIndex(strValue)!=-1)
			{
				// See if this value is already added.  If not add it.
				bool bFoundValue = false;
				int nNumValues = uniqueValues.m_uniqueValues.GetSize();
				for (int nLoop = 0; nLoop < nNumValues; nLoop++)
				{
					if (uniqueValues.m_uniqueValues[nLoop].m_strValue == strValue)
					{
						uniqueValues.m_uniqueValues[nLoop].m_count++; // keep track of how many use this value
						bFoundValue = true;
						break;
					}
				}
				// add if not found
				if (!bFoundValue)
				{
					UniqueValue newUniqueValue;
					newUniqueValue.m_strValue = strValue;
					newUniqueValue.m_count = 1;
					uniqueValues.m_uniqueValues.Add(newUniqueValue);
					// if this value is the default then we have added it
					if (strValue==strDefaultValue)
						bDefaultAdded = true;
				}
				uniqueValues.m_totalHaveValue++;
				bValidValue = true;
			}
		}
		// if the selected object did not have a value we can use
		// add the default value
		if (!bValidValue)
		{
			// See if the default is already added.  If not add it.
			bool bFoundValue = false;
			int nNumValues = uniqueValues.m_uniqueValues.GetSize();
			for (int nLoop = 0; nLoop < nNumValues; nLoop++)
			{
				if (uniqueValues.m_uniqueValues[nLoop].m_strValue == strDefaultValue)
				{
					uniqueValues.m_uniqueValues[nLoop].m_count++; // keep track of how many use this value
					bFoundValue = true;
					break;
				}
			}
			// add if not found
			if (!bFoundValue)
			{
				UniqueValue newUniqueValue;
				newUniqueValue.m_strValue = strDefaultValue;
				newUniqueValue.m_count = 1;
				uniqueValues.m_uniqueValues.Add(newUniqueValue);
				bDefaultAdded = true;
			}
		}
	}
	uniqueValues.m_totalCount = nNumObjects;
	// if the selection did not use the default then add it
	if (!bDefaultAdded)
	{
		UniqueValue newUniqueValue;
		newUniqueValue.m_strValue = strDefaultValue;
		newUniqueValue.m_count = 0; // no one is using it
		uniqueValues.m_uniqueValues.Add(newUniqueValue);
	}

	// sort the values
	TupSort<UniqueValue>::SortAscending(uniqueValues.m_uniqueValues.GetData(),uniqueValues.m_uniqueValues.GetSize());

	// now find the index to the default (should always be there)
	uniqueValues.m_defaultIndex = -1;
	// find the index to a unique value (only if all use it)
	uniqueValues.m_uniqueIndex = -1;
	int nNumValues = uniqueValues.m_uniqueValues.GetSize();
	for (int nLoop = 0; nLoop < nNumValues; nLoop++)
	{
		if (uniqueValues.m_defaultIndex==-1 && uniqueValues.m_uniqueValues[nLoop].m_strValue == strDefaultValue)
		{
			uniqueValues.m_defaultIndex = nLoop;
		}
		if (uniqueValues.m_uniqueIndex==-1 && uniqueValues.m_uniqueValues[nLoop].m_count == nNumObjects)
		{
			uniqueValues.m_uniqueIndex = nLoop;
		}
	}
}


//-----------------------------------------------------------------------------
// CurrAttribController::FindCheckStates
// This method will return the checked states for the node names passed in.
// The currently selected nodes will be inspected to see if they contain the
// keyname passed in.  If so, the vals can be inspected and compared to the
// incoming node names to find the final checked states.
//
// Input:
//  TupString strKeyname -- The keyname to look for on the selected nodes.
//  Keeper<TupString>& vectEntityNames -- The names to look for in the
//												 vals of the selected nodes.
//  int nDefaultState -- The default checked state.
//  TupArray<int>& vectCheckedStates -- Upon output, this vector will hold
//										   the checked state for each node
//										   name.  The size of this vector will
//										   be the same as the incoming node
//										   name vector.
//
// Output: True if successful, false otherwise.
//-----------------------------------------------------------------------------
bool CurrAttribController::FindCheckStates(const TupString &strKeyname,const Keeper<TupString>& vectEntityNames,
										   int nDefaultState, TupArray<int>& vectCheckedStates) const
{
	bool bResult = true;

	// First set up two count vectors:
	TupArray<short> vectCheckedCounts;
	TupArray<short> vectUncheckedCounts;
	int nNumNodeNames = vectEntityNames.GetSize();
	for (int nVectIndex = 0; nVectIndex < nNumNodeNames; nVectIndex++)
	{
		vectCheckedCounts.Add(0);
		vectUncheckedCounts.Add(0);
	}

	// Now cycle through all the selected nodes, looking for the keyname in
	// question and if their vals match any of the incoming node names:
	int nNumSelNodes = GetSize();
	for (int nSelNodeIndex = 0; nSelNodeIndex < nNumSelNodes; nSelNodeIndex++)
	{
		const CurrAttribData* pCurrAttribData = GetCurrAttribData(nSelNodeIndex);
		int nKeyIndex = pCurrAttribData->KeynameExistsAt(strKeyname);
		if (nKeyIndex > -1)
		{
			// Keyname exists for this node, so get the value
			TupString strVal = pCurrAttribData->GetValueAsString(nKeyIndex);
			int semiColonIndex = strVal.Find(';');
			if (semiColonIndex == -1)
				semiColonIndex = ARBITRARY_LARGE_NUM;

			// Now cycle through the incoming vector of entity names and see
			// which ones are in this node's value:
			for (int nNodeNameIndex = 0; nNodeNameIndex < nNumNodeNames; nNodeNameIndex++)
			{
				int nNameAt = strVal.Find(vectEntityNames[nNodeNameIndex]);
				while (nNameAt!=-1)
				{
					// Node name is in the value, but make sure it's not just a substring:
					bool bNameNotSub = false;
					// Make sure next char is a comma:
					int commaIndex = strVal.Find(',', nNameAt);
					int nEntityLength = vectEntityNames[nNodeNameIndex].GetLength();
					if (commaIndex == (nNameAt + nEntityLength))
					{
						if (nNameAt != 0)
						{
							// Make sure prev char is either a comma or semicolon:
							if ((strVal[nNameAt - 1] == ',') || (strVal[nNameAt - 1] == ';'))
							{
								bNameNotSub = true;
							}
						}
						else
						{
							bNameNotSub = true;
						}
					}

					if (bNameNotSub == true)
					{
						// Node name is not a substring, so see if it should be checked
						// or unchecked:
						if (nNameAt < semiColonIndex)
							vectCheckedCounts[nNodeNameIndex] = vectCheckedCounts[nNodeNameIndex] + 1;
						else
							vectUncheckedCounts[nNodeNameIndex] = vectUncheckedCounts[nNodeNameIndex] + 1;
						break;
					}

					nNameAt = strVal.Find(vectEntityNames[nNodeNameIndex], nNameAt + 1);
				}
			}
		}
	}

	// Now, based on the two count vectors, set the checked states:
	vectCheckedStates.RemoveAll();
	for (int nLoop = 0; nLoop < nNumNodeNames; nLoop++)
	{
		if (nDefaultState == BST_UNCHECKED)
		{
			if (vectCheckedCounts[nLoop] == nNumSelNodes)
				vectCheckedStates.Add(BST_CHECKED);
			else if (vectCheckedCounts[nLoop] == 0)
				vectCheckedStates.Add(BST_UNCHECKED);
			else
				vectCheckedStates.Add(BST_INDETERMINATE);
		}
		else
		{
			if (vectUncheckedCounts[nLoop] == nNumSelNodes)
				vectCheckedStates.Add(BST_UNCHECKED);
			else if (vectUncheckedCounts[nLoop] == 0)
				vectCheckedStates.Add(BST_CHECKED);
			else
				vectCheckedStates.Add(BST_INDETERMINATE);
		}
	}

	return bResult;
}


//-----------------------------------------------------------------------------
// CurrAttribController::FindEntityNames
// This method will return the names of all the nodes in the scene that match
// the incoming criteria.
//
// Input:
//  TupArray<KeyValPair> vectCriteria -- The vector of criteria to match
//											against the nodes.
//  Keeper<TupString>& vectEntityNames -- Upon output, this vector will
//												 contain the node names that
//												 match the criteria.
//
// Output: True if successful, false otherwise.
//-----------------------------------------------------------------------------
bool CurrAttribController::FindEntityNames(const TupArray<KeyValPair> &vectCriteria, Keeper<TupString>& vectEntityNames) const
{
	bool bResult = true;

	// First clear out the name vector:
	vectEntityNames.RemoveAll();

	// Get a temporary snapshot of the current attrib data for the scene:
	CurrAttribController tempAttribController;
	bool bNodesInScene = tempAttribController.LoadCurrAttribDataScene();
	if (bNodesInScene == true)
	{
		// Inspect each node and see if it matches the criteria:
		int nNumNodes = tempAttribController.GetSize();
		for (int nIndex = 0; nIndex < nNumNodes; nIndex++)
		{
			CurrAttribData* pCurrAttribData = tempAttribController.GetCurrAttribData(nIndex);
			HSnipeObject *pObject = tempAttribController.GetSnipeObject(nIndex);
			bool bAddNode = true;
			// Cycle through each criteria:
			int nNumCriteria = vectCriteria.GetSize();
			for (int nCritIndex = 0; nCritIndex < nNumCriteria; nCritIndex++)
			{
				int nKeynameIndex = pCurrAttribData->KeynameExistsAt(vectCriteria[nCritIndex].m_strKeyname);
				if (nKeynameIndex > -1)
				{
					TupString strVal = pCurrAttribData->GetValueAsString(nKeynameIndex);
					if (strVal != vectCriteria[nCritIndex].m_strVal)
					{
						// Node doesn't match criteria, so don't add it:
						bAddNode = false;
						break;
					}
				}
				else
				{
					// Node doesn't match criteria, so don't add it:
					bAddNode = false;
					break;
				}
			}

			if (bAddNode == true)
			{
				// Get the name of the node:
            HHierObject *pHeirObject = (HHierObject *)pObject;
				TupString strNodeName = pHeirObject->GetName().Get();
				vectEntityNames.Add(strNodeName);
			}
		}
	}
	else
	{
		vectEntityNames.RemoveAll();
		bResult = false;
	}

	return bResult;
}


//-----------------------------------------------------------------------------
// CurrAttribController::LoadCurrAttribDataScene
// This method will take the incoming currAttribController and create any
// necessary CurrAttribData objects for all the objects in the scene and load
// the data into them.  This method makes sure that the whole node tree is
// walked through.
//
// Input: None
//
// Output: True if there are nodes in the scene, false otherwise.
//-----------------------------------------------------------------------------
bool CurrAttribController::LoadCurrAttribDataScene()
{
	bool bNodesInScene = false;
	RemoveAll();
   HMainInterface *pMI = GetHMainInterface();
   HHierObject *pScene = pMI->GetCurrentCommandObject();
   int numChildren = pScene->GetNumChildren();
   for (int childIndex = 0; childIndex < numChildren; childIndex++)
   {
      HSnipeObject *pChildObject = pScene->GetChildAt(childIndex);
      if (ValidNode(pChildObject))
      {
         bNodesInScene = true;
         LoadCurrAttribDataNode(pChildObject);
      }
   }
	return bNodesInScene;
}

// CurrAttribController::LoadCurrAttribDataNode
// This method will take the incoming node and load its attrib data into the
// current attrib data controller.  It will then cycle through each of its
// children recursively calling this method to load in their data.
//
// Input:
//  MDagPath mDagPath -- The node that should have its attrib data loaded.
//
// Output: None.
//------------------------------------------------------------------------
void CurrAttribController::LoadCurrAttribDataNode(HSnipeObject *pObject)
{
	// if not already done
	if (m_snipeObjects.GetIndex(pObject)==-1)
	{
		// get index within keeper
		int objectIndex = m_snipeObjects.Add(pObject);
		
		CurrAttribData* pNewAttribData = new CurrAttribData();

		TupString strFullData;
		GetNote(pObject,strFullData);
	
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
		AddCurrAttribData(pNewAttribData);
		
		// Now cycle through all the children and have them load their data:
		int numChildren = pObject->GetNumChildren();
		// all my childern...
		for (int childIndex = 0; childIndex < numChildren; childIndex++)
		{
			HSnipeObject *pChildObject = pObject->GetChildAt(childIndex);
			if (ValidNode(pChildObject))
			{
				LoadCurrAttribDataNode(pChildObject);
			}
		}
	}
}

//-----------------------------------------------------------------------------
// CurrAttribController::StripAllButRawData
// This method will take the incoming string and strip out everything but the
// actual raw attribute data substring.
//
// Input:
//  TupString& strToFix -- String to strip.
//
// Output: True if both of the begin and end block indicators were found, false
//		   otherwise.
//-----------------------------------------------------------------------------
bool CurrAttribController::StripAllButRawData(TupString &strToFix)
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

//-----------------------------------------------------------------------------
// CurrAttribController::AddCurrAttribData
// This method will add a CurrAttribData pointer to the vector.
//
// Input:
//  CurrAttribData* pAttribData -- The pointer to add.
//
// Output: int index of entry within vector
//-----------------------------------------------------------------------------
int CurrAttribController::AddCurrAttribData(CurrAttribData* pAttribData)
{
	int index = m_vectAttribDataPtrs.GetSize();
	m_vectAttribDataPtrs.Add(pAttribData);
	return index;
}

//-----------------------------------------------------------------------------
// CurrAttribController::CurrObjDataToStr
// This method will look at the CurrAttribData object specified by the index
// passed in and will then form a string based on all of the keyname-value sets
// in that data's vector and pass back the string.
//
// Input:
//	int nIndex -- Index of the object to get the data string for.
//
// Output: TupString representing all the keyname-value sets in the vector.
//		   The string passed back will be in the format:
//				keyname=value\n
//				keyname=value\n
//				keyname=value\n
//		   NOTE: If the index passed in is out of the vector's bounds, then
//				 the string passed back will be empty.
//-----------------------------------------------------------------------------
TupString CurrAttribController::CurrObjDataToStr(int nIndex)
{
	TupString strEmpty = "";

	int nVectSize = m_vectAttribDataPtrs.GetSize();
	if (nIndex >= 0 && nIndex < nVectSize)
		return (m_vectAttribDataPtrs[nIndex])->CurrObjDataToStr();
	else
		return strEmpty;
}


//-----------------------------------------------------------------------------
// CurrAttribController::FormCheckListVal
// This method tell each of the CurrAttribDatas to form their checklist values
// based on the incoming strings and flags.
//
// Input:
//  const TupString strKeyname -- The keyname to use.
//  const TupString strValue -- The "sub" value to form with.
//  bool bChecked true for checked false for unchecked
//
// Output: None.
//-----------------------------------------------------------------------------
void CurrAttribController::FormCheckListVal(const TupString &strKeyname, const TupString &strValue, bool bChecked)
{
	int nNumPtrs = m_vectAttribDataPtrs.GetSize();
	for (int nIndex = 0; nIndex < nNumPtrs; nIndex++)
	{
		(m_vectAttribDataPtrs[nIndex])->FormCheckListVal(strKeyname, strValue, bChecked);
	}
}


#ifdef _DEBUG
//-----------------------------------------------------------------------------
// CurrAttribController::PrintDebug
// This method will print out the contents of this object to the output window
// in dev studio.
//
// Input: None.
//
// Output: None. 
//-----------------------------------------------------------------------------
void CurrAttribController::PrintDebug(void) const
{
	int nVectSize = m_vectAttribDataPtrs.GetSize();
	for (int nIndex = 0; nIndex < nVectSize; nIndex++)
	{
		(m_vectAttribDataPtrs[nIndex])->PrintDebug();
	}
}
#endif


static const char *invalidNodeNames[] = 
{
	"groundPlane_transform",
		"persp",
		"top",
		"front",
		"side",
		"References",
		NULL
};

bool CurrAttribController::ValidNodeName(const char *pName)
{
	int index=0;
	while (invalidNodeNames[index])
	{
		if (!strcmp(pName,invalidNodeNames[index]))
		{
			return false;
		}
		index++;
	}
	return true;
}

bool CurrAttribController::ValidNode(HSnipeObject *pObject)
{
	// todo return false if invalid node
	return true;
}

void CurrAttribController::CreateOrReplaceNote(HSnipeObject *pObject,const TupString &noteString)
{
	HHierObject *pHeirObject = (HHierObject *)pObject;
	if (pHeirObject)
	{
		HStringProperty *pStringProperty = pHeirObject->GetNotes();
		if (pStringProperty)
		{
			String newString(noteString);
			pStringProperty->StoreValue(newString,FALSE);
		}
	}
}

void CurrAttribController::GetNote(HSnipeObject *pObject,TupString &stringToFill)
{
	stringToFill.Empty();
	HHierObject *pHeirObject = (HHierObject *)pObject;
	if (pHeirObject)
	{
		HStringProperty *pStringProperty = pHeirObject->GetNotes();
		if (pStringProperty)
		{
			stringToFill = pStringProperty->GetValue().Get();
		}
	}
}
