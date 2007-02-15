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

#include "ObjectAttributesPCH.h"
#include "CurrAttribController.h"
#include "CurrAttribData.h"



//-----------------------------------------------------------------------------
// Defines, etc.:

#define BEGIN_BLOCK	"BEGIN OBJECTATTRIBUTES"
#define END_BLOCK	"END OBJECTATTRIBUTES"

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
// Input: INode *pNode
//
// Output: int representing the index into the maya objects keeper (-1) if not found
//-----------------------------------------------------------------------------
int CurrAttribController::GetIndex(INode *pNode) const
{
	return m_maxNodes.GetIndex(pNode);
}

//-----------------------------------------------------------------------------
// CurrAttribController::GetMaxNode
// This method will return the MObject in the keeper for a given index
//
// Input: int index
//
// Output: INode * from the given index
//-----------------------------------------------------------------------------
INode *CurrAttribController::GetMaxNode(int index) const
{
	assert(index>=0 && index<m_maxNodes.GetSize());
	return m_maxNodes[index];
}

//-----------------------------------------------------------------------------
// CurrAttribController::AddMaxNode
// This method will add the mObject to the keeper
//
// Input: INode *pNode
//
// Output: int representing the index into the maya objects keeper
//-----------------------------------------------------------------------------
int CurrAttribController::AddMaxNode(INode *pNode)
{
	return m_maxNodes.Add(pNode);
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
	m_maxNodes.RemoveAll();
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
//  TupArray<ValueString>& vectValues -- The vector to store the value
//											strings in.
//
// Output: True if any of the CurrAttribData objects do not have the key
//		   specified, false if they all have the key.
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
//  TupArray<TupString>& vectEntityNames -- The names to look for in the
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
bool CurrAttribController::FindCheckStates(const TupString &strKeyname,const TupArray<TupString>& vectEntityNames,
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
//  TupArray<TupString>& vectEntityNames -- Upon output, this vector will
//												 contain the node names that
//												 match the criteria.
//
// Output: True if successful, false otherwise.
//-----------------------------------------------------------------------------
bool CurrAttribController::FindEntityNames(const TupArray<KeyValPair> &vectCriteria, TupArray<TupString>& vectEntityNames) const
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
			INode *pNode = tempAttribController.GetMaxNode(nIndex);
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
				vectEntityNames.Add(pNode->GetName());
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
// Input: none
//
// Output: True if there are nodes in the scene, false otherwise.
//-----------------------------------------------------------------------------
bool CurrAttribController::LoadCurrAttribDataScene(void)
{
	bool bNodesInScene = false;

	// Get the root node if one exists:
	Interface* pInterface = GetCOREInterface();
	INode *pRootNode = pInterface->GetRootNode();
	if (pRootNode != NULL)
	{
		bNodesInScene = true;

		// Now load the data in for the root node (which in turn loads the data
		// in recursively for any of its children):
		LoadCurrAttribDataNode(pRootNode);
	}
	return bNodesInScene;
}


//-----------------------------------------------------------------------------
// CurrAttribController::LoadCurrAttribDataNode
// This method will take the incoming node and load its attrib data into the
// incoming CurrAttribController.  It will then cycle through each of its
// children recursively calling this method to load in their data.
//
// Input:
//  INode* pNode -- The node that should have its attrib data loaded.
//
// Output: None.
//-----------------------------------------------------------------------------
void CurrAttribController::LoadCurrAttribDataNode(INode* pNode)
{
	if (pNode && (m_maxNodes.GetIndex(pNode)==-1))
	{
		int objectIndex = m_maxNodes.Add(pNode);
		
		CurrAttribData* pNewAttribData = new CurrAttribData();

		// Set any data:
		TSTR buf;
		pNode->GetUserPropBuffer(buf);
		TupString strFullData = buf.data();
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

		// Add the CurrAttribData object to the currAttribController:
		AddCurrAttribData(pNewAttribData);

		// Now cycle through all the children and have them load their data:
		int nNumChildren = pNode->NumberOfChildren();
		for (int nIndex = 0; nIndex < nNumChildren; nIndex++)
		{
			INode* pChildNode = pNode->GetChildNode(nIndex);
			if (pChildNode == NULL)
				continue;
			LoadCurrAttribDataNode(pChildNode);
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

//-----------------------------------------------------------------------------
// CurrAttribController::RemoveCharsFromStr
// This method will look at the string passed in and remove all instances of 
// the specified character.
//
// Input:
//  TupString strToFix -- The string to remove characters from.
//  char chToRemove -- The character to remove.
//
// Output: True if characters were removed, false otherwise.
//-----------------------------------------------------------------------------
bool CurrAttribController::RemoveCharsFromStr(TupString& strToFix, char chToRemove)
{
	return (strToFix.Remove(chToRemove)>0);
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
// CurrAttribController::RemoveKeyInstances
// This method will go through all of the CurrAttribData objects and remove
// any instances of the key specified.
//
// Input:
//  TupString strKey -- The key to remove
//
// Output: None.
//-----------------------------------------------------------------------------
void CurrAttribController::RemoveKeyInstances(const char *pKey)
{
	int nNumPtrs = m_vectAttribDataPtrs.GetSize();
	for (int nIndex = 0; nIndex < nNumPtrs; nIndex++)
	{
		(m_vectAttribDataPtrs[nIndex])->RemoveKeyname(pKey);
	}
}


//-----------------------------------------------------------------------------
// CurrAttribController::RemoveAllKeynames
// This method will go through all the CurrAttribData objects and remove
// all the key/values from them.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void CurrAttribController::RemoveAllKeynames()
{
	int nNumPtrs = m_vectAttribDataPtrs.GetSize();
	for (int nIndex = 0; nIndex < nNumPtrs; nIndex++)
	{
		(m_vectAttribDataPtrs[nIndex])->RemoveAllKeynames();
	}
}


//-----------------------------------------------------------------------------
// CurrAttribController::AddKeyValue
// This method will set the keyname and value on all the CurrAttribData objects
// in the vector.
//
// Input:
//  const char *pKey -- The keyname to set.
//  const char *pValue -- The value to set.
//
// Output: None.
//-----------------------------------------------------------------------------
void CurrAttribController::AddKeyValue(const char *pKey, const char *pValue)
{
	int nNumPtrs = m_vectAttribDataPtrs.GetSize();
	for (int nIndex = 0; nIndex < nNumPtrs; nIndex++)
	{
		(m_vectAttribDataPtrs[nIndex])->AddKeyValue(pKey, pValue);
	}
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
		m_vectAttribDataPtrs[nIndex]->PrintDebug();
	}
}
#endif
