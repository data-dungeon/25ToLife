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

#include "pfdModPCH.h"
#include "CurrAttribController.h"
#include "CurrMeshAttribData.h"



//-----------------------------------------------------------------------------
// Defines, etc.:


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
int CurrAttribController::GetSize()
{
	return m_vectAttribDataPtrs.GetSize();
}


//-----------------------------------------------------------------------------
// CurrAttribController::GetIndex
// This method will return index in the keeper for the given mObject
//
// Input: MObject mObject
//
// Output: int representing the index into the maya objects keeper (-1) if not found
//-----------------------------------------------------------------------------
int CurrAttribController::GetIndex(CModData *pModData)
{
	return m_modDataPtrs.GetIndex(pModData);
}

//-----------------------------------------------------------------------------
// CurrAttribController::GetMayaObject
// This method will return the MObject in the keeper for a given index
//
// Input: int index
//
// Output: MObject from the given index
//-----------------------------------------------------------------------------
CModData *CurrAttribController::GetModData(int index)
{
	assert(index>=0 && index<m_modDataPtrs.GetSize());
	return m_modDataPtrs[index];
}

//-----------------------------------------------------------------------------
// CurrAttribController::AddModData
// This method will add the CModData to the keeper
//
// Input: MObject
//
// Output: int representing the index into the maya objects keeper
//-----------------------------------------------------------------------------
int CurrAttribController::AddModData(CModData *pModData)
{
	return m_modDataPtrs.Add(pModData);
}

//-----------------------------------------------------------------------------
// CurrAttribController::RemoveAll
// This method will clear the CModData keeper
//
// Input: none
//
// Output: none
//-----------------------------------------------------------------------------
void CurrAttribController::RemoveAll()
{
	m_modDataPtrs.RemoveAll();
	// delete attrib data from array
	int nVectSize = m_vectAttribDataPtrs.GetSize();
	for (int nIndex = 0; nIndex < nVectSize; nIndex++)
	{
		if (m_vectAttribDataPtrs[nIndex])
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
CurrMeshAttribData* CurrAttribController::GetCurrMeshAttribData(int nIndex)
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
	uniqueValues.m_totalCount = 0;
	uniqueValues.m_totalHaveValue = 0;

	// Cycle through all of the CurrMeshAttribData objects:
	int nNumObjects = m_vectAttribDataPtrs.GetSize();
	for (int nIndex = 0; nIndex < nNumObjects; nIndex++)
	{
		(m_vectAttribDataPtrs[nIndex])->GetAllUniqueValues(strKey,strDefaultValue,uniqueValues,pValidValues);
	}

	// check to see if the default is in the list
	bool bDefaultFound = false;
	int nNumValues;
	nNumValues = uniqueValues.m_uniqueValues.GetSize();
	for (int nLoop = 0; nLoop < nNumValues; nLoop++)
	{
		if (uniqueValues.m_uniqueValues[nLoop].m_strValue == strDefaultValue)
		{
			bDefaultFound = true;
			break;
		}
	}

	// if the selection did not use the default then add it
	if (!bDefaultFound)
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
	nNumValues = uniqueValues.m_uniqueValues.GetSize();
	for (int nLoop = 0; nLoop < nNumValues; nLoop++)
	{
		if (uniqueValues.m_defaultIndex==-1 && uniqueValues.m_uniqueValues[nLoop].m_strValue == strDefaultValue)
		{
			uniqueValues.m_defaultIndex = nLoop;
		}
		if (uniqueValues.m_uniqueIndex==-1 && uniqueValues.m_uniqueValues[nLoop].m_count == uniqueValues.m_totalCount)
		{
			uniqueValues.m_uniqueIndex = nLoop;
		}
	}
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
int CurrAttribController::AddCurrMeshAttribData(CurrMeshAttribData* pAttribData)
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
void CurrAttribController::RemoveAllKeynames(void)
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
void CurrAttribController::PrintDebug()
{
	int nVectSize = m_vectAttribDataPtrs.GetSize();
	for (int nIndex = 0; nIndex < nVectSize; nIndex++)
	{
		(m_vectAttribDataPtrs[nIndex])->PrintDebug();
	}
}
#endif

