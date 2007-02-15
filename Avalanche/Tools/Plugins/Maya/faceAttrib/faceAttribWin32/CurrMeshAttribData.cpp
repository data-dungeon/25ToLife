//-----------------------------------------------------------------------------
//	File:		CurrMeshAttribData.cpp
//	Created:	Nov. 29, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The CurrMeshAttribData class handles storing, getting, setting, etc. of
//			the current attribute data for the selected entity(s).
//
//	History:	11/29/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Includes:

#include "StdAfx.h"
#include "CurrMeshAttribData.h"

using namespace AttrControls;

//-----------------------------------------------------------------------------
// Class Definition:


//-----------------------------------------------------------------------------
// CurrMeshAttribData::CurrMeshAttribData
// This method is the default constructor for this class.
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
CurrMeshAttribData::CurrMeshAttribData()
{
}


//-----------------------------------------------------------------------------
// CurrMeshAttribData::~CurrMeshAttribData
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
CurrMeshAttribData::~CurrMeshAttribData()
{
}


#ifdef _DEBUG
//-----------------------------------------------------------------------------
// CurrMeshAttribData::PrintDebug
// This method will print out the contents of this object to the output window
// in dev studio.
//
// Input: None.
//
// Output: None. 
//-----------------------------------------------------------------------------
void CurrMeshAttribData::PrintDebug(void) const
{
	ATDebugPrint("CURRENT MESH ATTRIBUTE DATA:\n");
	int nVectSize = m_vectAttribSets.GetSize();
	int nIndex;
	for (nIndex = 0; nIndex < nVectSize; nIndex++)
	{
		ATDebugPrint("Set %d\n",nIndex);
		const CurrAttribData &currentAttrib = m_vectAttribSets[nIndex];
		currentAttrib.PrintDebug();
	}
	int numSelectedFaces = m_selectedFaces.GetSize();
	for (nIndex=0;nIndex<numSelectedFaces;nIndex++)
	{
		ATDebugPrint("Selected Face %d using set %d\n",m_selectedFaces[nIndex],m_faceAttribIndices[m_selectedFaces[nIndex]]);
	}
	int numFaces = m_faceAttribIndices.GetSize();
	for (nIndex = 0; nIndex < numFaces; nIndex++)
	{
		ATDebugPrint("Face %d using set %d\n",nIndex,m_faceAttribIndices[nIndex]);
	}
}
#endif




//-----------------------------------------------------------------------------
// CurrMeshAttribData::CurrObjDataToStr
// This method will form a string based on all of the keyname-value sets in
// the data vector and pass back the string.
//
// Input: TupString &strToFill
//
// Output: None
//
// strToFill will be formatted as follows:
// [key=value\r
// key=value\r
// ]\r
// [key=value\r
// key=value\r
// ]\r
//-----------------------------------------------------------------------------
TupString CurrMeshAttribData::CurrObjDataToStr(void) const
{
	TupString strToFill = "";
	
	// Cycle through the data vector, forming the string as we go:
	int nNumDataSets = m_vectAttribSets.GetSize();
	for (int nIndex = 0; nIndex < nNumDataSets; nIndex++)
	{
		strToFill+="[";
		strToFill+=m_vectAttribSets[nIndex].CurrObjDataToStr();
		strToFill+="]\r";
	}
	return strToFill;
}

// attribute sets
int CurrMeshAttribData::GetNumAttribSets(void) const
{
	return m_vectAttribSets.GetSize();
}

const CurrAttribData &CurrMeshAttribData::GetAttribSet(int index) const
{
	return m_vectAttribSets[index];
}

// faces
int CurrMeshAttribData::GetNumFaces(void) const
{
	return m_faceAttribIndices.GetSize();
}

int CurrMeshAttribData::GetFaceAttributeSet(int index) const
{
	return m_faceAttribIndices[index];
}

// selected faces
int CurrMeshAttribData::GetNumSelectedFaces(void) const
{
	return m_selectedFaces.GetSize();
}

int CurrMeshAttribData::GetSelectedFace(int selectedFaceIndex) const
{
	return m_selectedFaces[selectedFaceIndex];
}

bool CurrMeshAttribData::FaceChanged(int faceIndex) const
{
	return m_changedFaces[faceIndex];
}

bool CurrMeshAttribData::SelectedFacesUsingSet(int index) const
{
	return (m_selectedFacesUsingSets[index]>0);
}


// Mutators:
void CurrMeshAttribData::StrToCurrAttribData(const TupString &strData)
{
	m_vectAttribSets.RemoveAll();
	m_selectedFacesUsingSets.RemoveAll();
	m_facesUsingSets.RemoveAll();
	
	// Parse through the attributes:
	int pos = 0;
	while (pos<strData.GetLength())
	{
		int startIndex = strData.Find('[',pos);
		if (startIndex != -1)
		{
			int endIndex = strData.Find(']',startIndex);
			if (endIndex != -1)
			{
				CurrAttribData tempEntry;
				m_vectAttribSets.Add(tempEntry);
				int noFacesYet = 0;
				m_selectedFacesUsingSets.Add(noFacesYet);
				m_facesUsingSets.Add(noFacesYet);
				// fill in added entry if something to add in
				if (startIndex+1<endIndex)
				{
					CurrAttribData &newEntry = m_vectAttribSets[m_vectAttribSets.GetSize()-1];
					newEntry.StrToCurrAttribData(strData.Mid(startIndex+1,endIndex-(startIndex+1)));
				}
				pos=endIndex+1;
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}
	if (m_vectAttribSets.GetSize()==0)
	{
		CurrAttribData tempEntry;
		m_vectAttribSets.Add(tempEntry);
		int noFacesYet = 0;
		m_selectedFacesUsingSets.Add(noFacesYet);
		m_facesUsingSets.Add(noFacesYet);
	}
}

void CurrMeshAttribData::SetNumFaces(int numFaces)
{
	m_faceAttribIndices.SetSize(numFaces);
	m_changedFaces.SetSize(numFaces);
	ResetChangedFaces();
}

void CurrMeshAttribData::SetFaceAttribSet(int faceIndex,int setIndex)
{
	m_faceAttribIndices[faceIndex] = setIndex;
	m_facesUsingSets[setIndex]++;
}

void CurrMeshAttribData::AddSelectedFace(int faceIndex)
{
	m_selectedFaces.Add(faceIndex);
	int setIndex = m_faceAttribIndices[faceIndex];
	m_selectedFacesUsingSets[setIndex]++;
	m_selectedSets.Add(setIndex);
}

void CurrMeshAttribData::ResetChangedFaces(void)
{
	int numFaces = m_changedFaces.GetSize();
	for (int index=0;index<numFaces;index++)
	{
		m_changedFaces[index]=false;
	}
}

void CurrMeshAttribData::SetChangedFace(int faceIndex,bool bValue)
{
	m_changedFaces[faceIndex] = bValue;
}


//-----------------------------------------------------------------------------
// CurrMeshAttribData::RemoveKeyname
// This method will remove all instances of the key specified.
//
// Input:
//  const char *pKey -- The keyname to remove.
//
// Output: returns true if keys were removed
//-----------------------------------------------------------------------------
bool CurrMeshAttribData::RemoveKeyname(const char *pKey)
{
	bool bChanged = false;
	IsolateSelectedSets();
	int nNumSets = m_vectAttribSets.GetSize();
	for (int nIndex = 0; nIndex <nNumSets; nIndex++)
	{
		if (SelectedFacesUsingSet(nIndex))
		{
			if (m_vectAttribSets[nIndex].RemoveKeyname(pKey))
			{
				bChanged = true;
			}
		}
	}
	if (bChanged)
	{
		CompactSets();
	}
	return bChanged;
}

//-----------------------------------------------------------------------------
// CurrMeshAttribData::RemoveKeyname
// This method will remove the keys that match the key and value
//
// Input:
//  const char *strKeyname -- The keyname to remove.
//  const char *strValname -- The value that needs to match
//
// Output: returns true if key was removed
//-----------------------------------------------------------------------------
bool CurrMeshAttribData::RemoveKeyname(const char *strKeyname,const char *strValname)
{
	bool bChanged = false;
	IsolateSelectedSets();
	int nNumSets = m_vectAttribSets.GetSize();
	for (int nIndex = 0; nIndex <nNumSets; nIndex++)
	{
		if (SelectedFacesUsingSet(nIndex))
		{
			int keyIndex = m_vectAttribSets[nIndex].KeynameExistsAt(strKeyname);
			if (keyIndex!=-1)
			{
				if (!strcmp(m_vectAttribSets[nIndex].GetValueAsString(keyIndex),strValname))
				{
					m_vectAttribSets[nIndex].RemoveKeyname(strKeyname);
					bChanged = true;
				}
			}
		}
	}
	if (bChanged)
	{
		CompactSets();
	}
	return bChanged;
}

//-----------------------------------------------------------------------------
// CurrMeshAttribData::RenameKey
// This method will remove the keys that match the key and value
//
// Input:
//  const char *strOldKeyname -- The keyname to rename
//  const char *strNewKeyname -- The name of the new key
//
// Output: returns true if key was renamed
//-----------------------------------------------------------------------------
bool CurrMeshAttribData::RenameKey(const char *strOldKeyname,const char *strNewKeyname)
{
	bool bChanged = false;
	IsolateSelectedSets();
	int nNumSets = m_vectAttribSets.GetSize();
	for (int nIndex = 0; nIndex <nNumSets; nIndex++)
	{
		if (SelectedFacesUsingSet(nIndex))
		{
			int keyIndex = m_vectAttribSets[nIndex].KeynameExistsAt(strOldKeyname);
			if (keyIndex!=-1)
			{
				TupString valString = m_vectAttribSets[nIndex].GetValueAsString(keyIndex);
				m_vectAttribSets[nIndex].RemoveKeyname(strOldKeyname); // get rid of old one
				m_vectAttribSets[nIndex].AddKeyValue(strNewKeyname,valString); // add in new one
				bChanged = true;
			}
		}
	}
	if (bChanged)
	{
		CompactSets();
	}
	return bChanged;
}

//-----------------------------------------------------------------------------
// CurrMeshAttribData::RenameVal
// This method will remove the keys that match the key and value
//
// Input:
//  const char *strKeyname -- The key to look for
//  const char *strOldValname -- The value to rename
//  const char *strNewValname -- The name of the new value
//
// Output: returns true if value was renamed
//-----------------------------------------------------------------------------
bool CurrMeshAttribData::RenameVal(const char *strKeyname,const char *strOldValname,const char *strNewValname)
{
	bool bChanged = false;
	IsolateSelectedSets();
	int nNumSets = m_vectAttribSets.GetSize();
	for (int nIndex = 0; nIndex <nNumSets; nIndex++)
	{
		if (SelectedFacesUsingSet(nIndex))
		{
			int keyIndex = m_vectAttribSets[nIndex].KeynameExistsAt(strKeyname);
			if (keyIndex!=-1)
			{
				TupString valString = m_vectAttribSets[nIndex].GetValueAsString(keyIndex);
				if (!strcmp(valString,strOldValname)) // if the val matches then rename
				{
					m_vectAttribSets[nIndex].AddKeyValue(strKeyname,strNewValname); // add in new one
					bChanged = true;
				}
			}
		}
	}
	if (bChanged)
	{
		CompactSets();
	}
	return bChanged;
}


//-----------------------------------------------------------------------------
// CurrMeshAttribData::RemoveAllKeynames
// This method will empty out the vector containing all of the current attrib
// data.
//
// Input: None.
//
// Output: returns true if keys were removed
//-----------------------------------------------------------------------------
bool CurrMeshAttribData::RemoveAllKeynames()
{
	bool bChanged = false;
	IsolateSelectedSets();
	int nNumSets = m_vectAttribSets.GetSize();
	for (int nIndex = 0; nIndex <nNumSets; nIndex++)
	{
		if (SelectedFacesUsingSet(nIndex))
		{
			if (m_vectAttribSets[nIndex].RemoveAllKeynames())
			{
				bChanged = true;
			}
		}
	}
	if (bChanged)
	{
		CompactSets();
	}
	return bChanged;
}

//-----------------------------------------------------------------------------
// CurrMeshAttribData::SetStringAsString
// This method will add an entry to the current attrib data vector.  It will
// use the string passed in.
//
// Input:
//	KeyValuePair &keyValuePair -- The key and value to add
//
// Output: returns true if key was added
//-----------------------------------------------------------------------------
bool CurrMeshAttribData::AddKeyValue(const KeyValuePair &keyValuePair)
{
	return ApplyKeyValue(keyValuePair);
}

//-----------------------------------------------------------------------------
// CurrMeshAttribData::AddKeyValue
// This method will add an entry to the current attrib data vector.  It will
// use the string passed in.
//
// Input:
//	const char *pKey -- The keyname to use for this attribute.
//  const char *pValue -- The value to use for this attribute.
//
// Output: returns true if key was added
//-----------------------------------------------------------------------------
bool CurrMeshAttribData::AddKeyValue(const char *pKey, const char *pValue)
{
	KeyValuePair newEntry(pKey,pValue);
	return ApplyKeyValue(newEntry);
}


//-----------------------------------------------------------------------------
// CurrMeshAttribData::AddKeyValue
// This method will add an entry to the current attrib data vector.  It will
// use the boolean passed in.
//
// Input:
//  const char *pKey -- The keyname to use for this attribute.
//  bool bValue -- The boolean value to use for this attribute (converted to a
//				   string.
//
// Output: returns true if key was added
//-----------------------------------------------------------------------------
bool CurrMeshAttribData::AddKeyValue(const char *pKey, bool bValue)
{
	KeyValuePair newEntry(pKey);
	if (bValue == true)
		newEntry.m_strValue = "1";
	else
		newEntry.m_strValue = "0";
	return ApplyKeyValue(newEntry);
}

static char pChValue[50];

//-----------------------------------------------------------------------------
// CurrMeshAttribData::AddKeyValue
// This method will add an entry to the current attrib data vector.  It will
// use the integer passed in.
//
// Input:
//  const char *pKey -- The keyname to use for this attribute.
//  int nValue -- The int value to use for this attribute (converted to a
//				   string.
//
// Output: returns true if key was added
//-----------------------------------------------------------------------------
bool CurrMeshAttribData::AddKeyValue(const char *pKey, int nValue)
{
	KeyValuePair newEntry(pKey);
	newEntry.m_strValue = itoa(nValue, pChValue, 10);
	return ApplyKeyValue(newEntry);
}


//-----------------------------------------------------------------------------
// CurrMeshAttribData::AddKeyValue
// This method will add an entry to the current attrib data vector.  It will
// use the float passed in.
//
// Input:
//  const char *pKey -- The keyname to use for this attribute.
//  float fValue -- The float value to use for this attribute (converted to a
//					string.
//
// Output: returns true if key was added
//-----------------------------------------------------------------------------
bool CurrMeshAttribData::AddKeyValue(const char *pKey, float fValue)
{
	KeyValuePair newEntry(pKey);
	newEntry.m_strValue = gcvt(fValue, 10, pChValue);
	return ApplyKeyValue(newEntry);
}


//-----------------------------------------------------------------------------
// CurrMeshAttribData::AddKeyValue
// This method will add an entry to the current attrib data vector.  It will
// use the color passed in.
//
// Input:
//  const char *pKey -- The keyname to use for this attribute.
//  const AttribColor &colorValue -- The color value to use for this attribute
//							  (converted to a string).
//
// Output: returns true if key was added
//-----------------------------------------------------------------------------
bool CurrMeshAttribData::AddKeyValue(const char *pKey,const AttribColor &colorValue)
{
	KeyValuePair newEntry(pKey);

	// Set the value:
	TupString strColor = "";
	strColor+=itoa(colorValue.m_nRed, pChValue, 10);
	strColor+=",";
	strColor+=itoa(colorValue.m_nGreen, pChValue, 10);
	strColor+=",";
	strColor+=itoa(colorValue.m_nBlue, pChValue, 10);
	strColor+=",";
	strColor+=colorValue.m_strName;
	newEntry.m_strValue = strColor;

	return ApplyKeyValue(newEntry);
}

//-----------------------------------------------------------------------------
// CurrMeshAttribData::ApplyKeyValue
// This method will apply a key value pair to the selected faces
// use the string passed in.
//
// Input:
//	const KeyValuePair &keyValuePair -- The key and value to set
//
// Output: returns true if keyvalue was set or added.
//
//  will update the database with new sets as well as update the m_faceChangedArray to 
// indicate which faces need to be updated
//-----------------------------------------------------------------------------
bool CurrMeshAttribData::ApplyKeyValue(const KeyValuePair &keyValuePair)
{
	bool bChanged = false;
	Keeper<int> newSelectedSets; // build this as we go and replace m_selectedSets after we are done
	int numSets = m_vectAttribSets.GetSize();
	int numSelectedFaces = m_selectedFaces.GetSize();
	int numSelectedSets = m_selectedSets.GetSize();
	int selectedSetIndex;
	// loop through the sets that are used from the selected faces
	for (selectedSetIndex=0;selectedSetIndex<numSelectedSets;selectedSetIndex++)
	{
		int setIndex = m_selectedSets[selectedSetIndex];
		CurrAttribData &currentSet = m_vectAttribSets[setIndex];
		// if the first set or our set is going to change
		if (setIndex==0 || currentSet.Find(keyValuePair)==-1)
		{
			bChanged = true;

			int facesUsingSet = m_facesUsingSets[setIndex];
			int selectedFacesUsingSet = m_selectedFacesUsingSets[setIndex];
			// make a copy
			CurrAttribData newSet(currentSet);
			newSet.AddKeyValue(keyValuePair);
			// keep track of the first unused set
			int unusedSetIndex = -1;
			int setSearchIndex;
			bool bFoundSet = false;
			int foundSetIndex;

			// search through the sets and see if any one of them will work so we don't need to make a new one
			for (setSearchIndex=1;setSearchIndex<numSets;setSearchIndex++)
			{
				// if we found one we need to point to this one now
				if (newSet.Compare(m_vectAttribSets[setSearchIndex]))
				{
					bFoundSet = true;
					foundSetIndex = setSearchIndex;
					break;
				}
				if (unusedSetIndex == -1 && m_facesUsingSets[setSearchIndex]==0)
				{
					unusedSetIndex = setSearchIndex;
				}
			}
			// if we found another set that is what we need
			int setToUse;
			if (bFoundSet)
			{
				setToUse = foundSetIndex;
				// if we were the only one to use the previous set and not the default
				// we are going to clear the previous set
				if (setIndex!=0 && selectedFacesUsingSet==facesUsingSet)
				{
					currentSet.RemoveAllKeynames();
				}
			}
			// if we didn't find a match and we own the slot (only one using it)
			// update the set and go on to the next one
			else if (setIndex!=0 && selectedFacesUsingSet==facesUsingSet)
			{
				currentSet.AddKeyValue(keyValuePair);
				newSelectedSets.Add(setIndex); // for our new keeper
				continue;
			}
			// if we didn't find a set but we found an empty one to use
			else if (unusedSetIndex!=-1)
			{
				// update the unused set with our new one
				m_vectAttribSets[unusedSetIndex] = newSet;
				setToUse = unusedSetIndex;
			}
			// if we didn't find one to use then we need to add a new one
			else
			{
				m_vectAttribSets.Add(newSet);
				int numFacesUsingSet = 0;
				m_selectedFacesUsingSets.Add(numFacesUsingSet);
				m_facesUsingSets.Add(numFacesUsingSet);
				setToUse = m_vectAttribSets.GetSize()-1;
			}
			// remap all selected faces that are using the set to point to the other set
			for (int selectedFaceIndex=0;selectedFaceIndex<numSelectedFaces;selectedFaceIndex++)
			{
				int faceIndex = m_selectedFaces[selectedFaceIndex];
				if (m_faceAttribIndices[faceIndex]==setIndex)
				{
					m_faceAttribIndices[faceIndex]=setToUse;
					m_changedFaces[faceIndex]=true;
				}
			}
			// add in the using set counts
			m_selectedFacesUsingSets[setToUse]+=selectedFacesUsingSet;
			m_facesUsingSets[setToUse]+=selectedFacesUsingSet;
			// no selected faces are using this set now
			m_facesUsingSets[setIndex]-=selectedFacesUsingSet;
			m_selectedFacesUsingSets[setIndex]=0;
			newSelectedSets.Add(setToUse); // for our new keeper
		}
		else
		{
			newSelectedSets.Add(setIndex); // for our new keeper
		}
	}
	if (bChanged)
	{
		m_selectedSets = newSelectedSets; // update to the new keeper
	}
	return bChanged;
}

//-----------------------------------------------------------------------------
// CurrMeshAttribData::GetAllUniqueValues
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
void CurrMeshAttribData::GetAllUniqueValues(const TupString &strKey, const TupString &strDefaultValue, UniqueValues &uniqueValues, Keeper<TupString> *pValidValues) const
{
	// loop through all the sets that represent the selected objects
	int numSelectedSets = m_selectedSets.GetSize();
	for (int selectedSetIndex=0;selectedSetIndex<numSelectedSets;selectedSetIndex++)
	{
		bool bValidValue = false;

		int setIndex = m_selectedSets[selectedSetIndex];
		int keyIndex = m_vectAttribSets[setIndex].KeynameExistsAt(strKey);
		int numFacesUsingSet = m_selectedFacesUsingSets[setIndex];
		uniqueValues.m_totalCount += numFacesUsingSet;
		if (keyIndex >= 0)
		{
			TupString strValue(m_vectAttribSets[setIndex].GetValueAsString(keyIndex));
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
						uniqueValues.m_uniqueValues[nLoop].m_count+=numFacesUsingSet; // keep track of how many use this value
						bFoundValue = true;
						break;
					}
				}
				// add if not found
				if (!bFoundValue)
				{
					UniqueValue newUniqueValue;
					newUniqueValue.m_strValue = strValue;
					newUniqueValue.m_count = numFacesUsingSet;
					uniqueValues.m_uniqueValues.Add(newUniqueValue);
				}
				uniqueValues.m_totalHaveValue+=numFacesUsingSet;
				bValidValue = true;
			}
		}
		if (!bValidValue)
		{
			// See if the default is already added.  If not add it.
			bool bFoundValue = false;
			int nNumValues = uniqueValues.m_uniqueValues.GetSize();
			for (int nLoop = 0; nLoop < nNumValues; nLoop++)
			{
				if (uniqueValues.m_uniqueValues[nLoop].m_strValue == strDefaultValue)
				{
					uniqueValues.m_uniqueValues[nLoop].m_count+=numFacesUsingSet; // keep track of how many use this value
					bFoundValue = true;
					break;
				}
			}
			// add if not found
			if (!bFoundValue)
			{
				UniqueValue newUniqueValue;
				newUniqueValue.m_strValue = strDefaultValue;
				newUniqueValue.m_count = numFacesUsingSet;
				uniqueValues.m_uniqueValues.Add(newUniqueValue);
			}
		}
	}
}

// this method will make sure that all selected sets are not having any non selected faces within them
void CurrMeshAttribData::IsolateSelectedSets(void)
{
	Keeper<int> newSelectedSets; // build this as we go and replace m_selectedSets after we are done
	int numSets = m_vectAttribSets.GetSize();
	int numSelectedFaces = m_selectedFaces.GetSize();
	int numSelectedSets = m_selectedSets.GetSize();
	int selectedSetIndex;
	// loop through the sets that are used from the selected faces
	for (selectedSetIndex=0;selectedSetIndex<numSelectedSets;selectedSetIndex++)
	{
		int setIndex = m_selectedSets[selectedSetIndex];
		int facesUsingSet = m_facesUsingSets[setIndex];
		int selectedFacesUsingSet = m_selectedFacesUsingSets[setIndex];
		CurrAttribData &currentSet = m_vectAttribSets[setIndex];
		// if the first set or our set is going to change
		if (setIndex==0 || facesUsingSet!=selectedFacesUsingSet)
		{
			// make a copy
			CurrAttribData newSet(currentSet);
			// keep track of the first unused set
			int unusedSetIndex = -1;
			int setSearchIndex;
			bool bFoundSet = false;
			int foundSetIndex;
			
			// search through the sets and see if any one of them will work so we don't need to make a new one
			for (setSearchIndex=1;setSearchIndex<numSets;setSearchIndex++)
			{
				// if we found one that matches and is used by only selected faces
				if ((m_facesUsingSets[setSearchIndex]==m_selectedFacesUsingSets[setSearchIndex]) &&
					newSet.Compare(m_vectAttribSets[setSearchIndex]))
				{
					bFoundSet = true;
					foundSetIndex = setSearchIndex;
					break;
				}
				if (unusedSetIndex == -1 && m_facesUsingSets[setSearchIndex]==0)
				{
					unusedSetIndex = setSearchIndex;
				}
			}
			// if we found another set that is what we need
			int setToUse;
			if (bFoundSet)
			{
				setToUse = foundSetIndex;
			}
			// if we didn't find a set but we found an empty one to use
			else if (unusedSetIndex!=-1)
			{
				// update the unused set with our new one
				m_vectAttribSets[unusedSetIndex] = newSet;
				setToUse = unusedSetIndex;
			}
			// if we didn't find one to use then we need to add a new one
			else
			{
				m_vectAttribSets.Add(newSet);
				int numFacesUsingSet = 0;
				m_selectedFacesUsingSets.Add(numFacesUsingSet);
				m_facesUsingSets.Add(numFacesUsingSet);
				setToUse = m_vectAttribSets.GetSize()-1;
			}
			// remap all selected faces that are using the set to point to the other set
			for (int selectedFaceIndex=0;selectedFaceIndex<numSelectedFaces;selectedFaceIndex++)
			{
				int faceIndex = m_selectedFaces[selectedFaceIndex];
				if (m_faceAttribIndices[faceIndex]==setIndex)
				{
					m_faceAttribIndices[faceIndex]=setToUse;
					m_changedFaces[faceIndex]=true;
				}
			}
			// add in the using set counts
			m_selectedFacesUsingSets[setToUse]+=selectedFacesUsingSet;
			m_facesUsingSets[setToUse]+=selectedFacesUsingSet;
			// no selected faces are using this set now
			m_facesUsingSets[setIndex]-=selectedFacesUsingSet;
			m_selectedFacesUsingSets[setIndex]=0;
			newSelectedSets.Add(setToUse); // for our new keeper
		}
		else
		{
			newSelectedSets.Add(setIndex); // for our new keeper
		}
	}
	m_selectedSets = newSelectedSets; // update to the new keeper
}

// this method will attempt to reduce the number of sets at much as possible
void CurrMeshAttribData::CompactSets(void)
{
	int numFaces = m_faceAttribIndices.GetSize();
	int numSets = m_vectAttribSets.GetSize();

	TupArray<int> setRemapArray; // this holds the index of the new set (size is the number of old sets)
	// build a new set of data to be replaced at the end
	TupArray<CurrAttribData> newVectAttribSets;

	// add for first entry (for 0)
	newVectAttribSets.Add(m_vectAttribSets[0]);
	setRemapArray.SetSize(numSets);
	setRemapArray[0] = 0;

	// loop through the sets and move over needed ones
	for (int mySetIndex=1;mySetIndex<numSets;mySetIndex++)
	{
		CurrAttribData &mySet = m_vectAttribSets[mySetIndex];
		bool bFoundSet = false;
		int foundSetIndex;
		// make sure the 
		if (m_facesUsingSets[mySetIndex])
		{
			int numNewSets = newVectAttribSets.GetSize();
			// loop through to find a match
			for (int setSearchIndex=0;setSearchIndex<numNewSets;setSearchIndex++)
			{
				// if found one that is not being used 
				if (mySet.Compare(newVectAttribSets[setSearchIndex]))
				{
					bFoundSet = true;
					foundSetIndex = setSearchIndex;
					break;
				}
			}
			// if we found another set that is what we need
			if (bFoundSet)
			{
				setRemapArray[mySetIndex] = foundSetIndex;
			}
			// if we have a new set to add
			else
			{
				setRemapArray[mySetIndex] = newVectAttribSets.GetSize(); // index of new entry
				newVectAttribSets.Add(mySet); // add this one in
			}
		}
		else
		{
			setRemapArray[mySetIndex] = 0; // point to first (no one is using me)
		}
	}
	int numNewSets = newVectAttribSets.GetSize();
	TupArray<int>newSelectedFacesUsingSets;
	TupArray<int>newFacesUsingSets;
	newSelectedFacesUsingSets.SetSize(numNewSets);
	newFacesUsingSets.SetSize(numNewSets);
	int setIndex;
	// set face counts to 0
	for (setIndex=0;setIndex<numNewSets;setIndex++)
	{
		newSelectedFacesUsingSets[setIndex] = 0;
		newFacesUsingSets[setIndex] = 0;
	}

	// remap all the faces to the new sets
	for (int faceIndex=0;faceIndex<numFaces;faceIndex++)
	{
		setIndex = m_faceAttribIndices[faceIndex];
		int newSetIndex = setRemapArray[setIndex];
		if (newSetIndex != setIndex)
		{
			m_faceAttribIndices[faceIndex]=newSetIndex;
			m_changedFaces[faceIndex]=true;
		}
	}

	// update face counts for the new sets
	for (setIndex=0;setIndex<numSets;setIndex++)
	{
		int newSetIndex = setRemapArray[setIndex];
		newSelectedFacesUsingSets[newSetIndex] += m_selectedFacesUsingSets[setIndex];
		newFacesUsingSets[newSetIndex] += m_facesUsingSets[setIndex];
	}

	// set up the newSelectedSets
	Keeper<int> newSelectedSets;
	for (setIndex=0;setIndex<numNewSets;setIndex++)
	{
		if (newSelectedFacesUsingSets[setIndex])
		{
			newSelectedSets.Add(setIndex);
		}
	}

	// update the object data
	m_vectAttribSets = newVectAttribSets;
	m_selectedFacesUsingSets = newSelectedFacesUsingSets;
	m_facesUsingSets = newFacesUsingSets;
	m_selectedSets = newSelectedSets;
}
