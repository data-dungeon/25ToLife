//-----------------------------------------------------------------------------
//	File:		ExclusiveController.cpp
//	Created:	Apr. 11, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The ExclusiveController class handles all the key/value pairs,
//			data, and corresponding methods regarding the EXCLUSIVE settings
//			in the config tree.
//
//	History:	4/11/2002: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Includes:

#include "UIDisplayPCH.h"
#include "ExclusiveController.h"
#include "Registry.h"

//-----------------------------------------------------------------------------
// Defines, enums, etc.:



//-----------------------------------------------------------------------------
// Class Definition:


//-----------------------------------------------------------------------------
// ExclusiveController::ExclusiveController
// This method is the default constructor for this class.
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
ExclusiveController::ExclusiveController()
{
}


//-----------------------------------------------------------------------------
// ExclusiveController::~ExclusiveController
// This method is the destructor for this class
//
// Input: None
//
// Output: None
//-----------------------------------------------------------------------------
ExclusiveController::~ExclusiveController()
{
}


//-----------------------------------------------------------------------------
// ExclusiveController::Init
// This method can be called to initialize this object.  It will open the
// registry key specified and load in any exclusive keys contained there.  It
// will also set up the boolean vector that specifies which keys to keep.
//
// Input:
//  char* pChKeyName -- The key name to use in the registry.
//  HKEY hRootKey -- The root registry key to use.
//
// Output: None.
//-----------------------------------------------------------------------------
void ExclusiveController::Init(const char* pChKeyName)
{
	if (m_keyName.GetLength())
	{
		SaveKeyValsInReg();
	}
	m_keyName = pChKeyName;

	// Open up the registry key:
	Registry myRegistry(pChKeyName);

	// Clear out the boolean vector specifying which keys to keep:
	m_vectKeyValsToKeep.RemoveAll();

	// Cycle through all the registry values/valuedatas and load them into the
	// key/val vector:
	char chRegValName[MAX_PATH];
	DWORD dwRegValLength = MAX_PATH;
	char chRegValData[MAX_PATH];
	DWORD dwRegDataLength = MAX_PATH;
	DWORD dwRetCode = ERROR_SUCCESS;
	for (DWORD dwIndex = 0; dwRetCode != ERROR_NO_MORE_ITEMS; dwIndex++)
	{
		dwRetCode = myRegistry.EnumValue(dwIndex, chRegValName, &dwRegValLength,
										 (LPBYTE) chRegValData, &dwRegDataLength);
		if (dwRetCode == ERROR_SUCCESS)
		{
			// Add the key/val to the vector:
			KeyVals newKeyVal;
			newKeyVal.m_strKeyname = chRegValName;
			newKeyVal.m_strValue = chRegValData;
			m_vectKeyVals.Add(newKeyVal);

			// Set up its corresponding entry in the bool vector:
			m_vectKeyValsToKeep.Add(false);
		}

		// Set the buffer lengths for the next call:
		dwRegValLength = MAX_PATH;
		dwRegDataLength = MAX_PATH;
	}
}


//-----------------------------------------------------------------------------
// ExclusiveController::SaveKeyValsInReg
// This method will go through all the nonexclusive keys and save them out to
// the registry location specified.  Any nonexclusive keys already contained
// in the registry location are removed.
//
// Input:
//  char* pChKeyName -- The key name to use in the registry.
//  HKEY hRootKey -- The root registry key to use.
//
// Output: None.
//-----------------------------------------------------------------------------
void ExclusiveController::SaveKeyValsInReg(void)
{
	if (!m_keyName.GetLength())
		return;

	// Open up the registry key:
	Registry myRegistry(m_keyName);

	// Cycle through all the registry values and delete them from the key:
	DWORD dwNumValues = myRegistry.GetNumValues();
	char chRegValName[MAX_PATH];
	DWORD dwRegValLength = MAX_PATH;
	char chRegValData[MAX_PATH];
	DWORD dwRegDataLength = MAX_PATH;
	DWORD dwRetCode = ERROR_SUCCESS;
	for (DWORD dwIndex = dwNumValues - 1; dwRetCode != ERROR_NO_MORE_ITEMS; dwIndex--)
	{
		dwRetCode = myRegistry.EnumValue(dwIndex, chRegValName, &dwRegValLength,
										 (LPBYTE) chRegValData, &dwRegDataLength);
		if (dwRetCode == ERROR_SUCCESS)
		{
			// Remove the value from the registry:
			myRegistry.Delete(chRegValName);
		}

		// Set the buffer lengths for the next call:
		dwRegValLength = MAX_PATH;
		dwRegDataLength = MAX_PATH;
	}

	// Now add all the nonexclusive keys to the registry:
	int nNumKeyVals = m_vectKeyVals.GetSize();
	for (int nIndex = 0; nIndex < nNumKeyVals; nIndex++)
	{
		myRegistry.Set(m_vectKeyVals[nIndex].m_strKeyname,
					   m_vectKeyVals[nIndex].m_strValue);
	}
}


//-----------------------------------------------------------------------------
// ExclusiveController::SetKeeperFlag
// This method will look at the keyname passed in, and if it is in the key/val
// vector, then it will set the corresponding index in the m_vectKeyValsToKeep
// to true (meaning that this key needs to be kept during a removal of keys).
//
// Input:
//  TupString strKeyname -- The key to check for.
//
// Output: None.
//-----------------------------------------------------------------------------
void ExclusiveController::SetKeeperFlag(TupString strKeyname)
{
	bool bFound = false;

	int nNumKeyVals = m_vectKeyVals.GetSize();
	for (int nIndex = 0; nIndex < nNumKeyVals; nIndex++)
	{
		if (m_vectKeyVals[nIndex].m_strKeyname == strKeyname)
		{
			bFound = true;
			break;
		}
	}

	if (bFound == true)
	{
		m_vectKeyValsToKeep[nIndex] = true;
	}
}


//-----------------------------------------------------------------------------
// ExclusiveController::TossUnwantedKeys
// This method will go through the m_vectKeyVals and remove any keys that are
// "marked" as unwanted by using the m_vectKeyValsToKeep vector.
//
// Input:
//  void -- None.
//
// Output: None.
//-----------------------------------------------------------------------------
void ExclusiveController::TossUnwantedKeys(void)
{
	int nNumKeys = m_vectKeyVals.GetSize();
	for (int nIndex = nNumKeys - 1; nIndex >= 0; nIndex--)
	{
		if (m_vectKeyValsToKeep[nIndex] == false)
		{
			// Get rid of the m_vectKeyVals entry:
			m_vectKeyVals.RemoveAt(nIndex);
			// Get rid of the m_vectKeyValsToKeep to keep both vectors in sync:
			m_vectKeyValsToKeep.RemoveAt(nIndex);
		}
	}
}


//-----------------------------------------------------------------------------
// ExclusiveController::SetKeyVal
// This method will take the incoming key and value and set them in the
// vector of nonexclusive key/vals.  If the key is already in the vector, then
// it is removed and the new value added.
//
// Input:
//  TupString strKeyname -- The key to set.
//  TupString strValue -- The value to set.
//
// Output: None.
//-----------------------------------------------------------------------------
void ExclusiveController::SetKeyVal(const TupString &strKeyname,const TupString &strValue)
{
	// Remove any keys of the same name that already exist:
	int nNumKeyVals = m_vectKeyVals.GetSize();
	for (int nIndex = nNumKeyVals - 1; nIndex >= 0; nIndex--)
	{
		if (m_vectKeyVals[nIndex].m_strKeyname == strKeyname)
		{
			// Remove the key/val
			m_vectKeyVals.RemoveAt(nIndex);
			// Remove the boolean:
			m_vectKeyValsToKeep.RemoveAt(nIndex);
		}
	}

	// Now add the key and its value:
	KeyVals newKeyVal;
	newKeyVal.m_strKeyname = strKeyname;
	newKeyVal.m_strValue = strValue;
	m_vectKeyVals.Add(newKeyVal);
	// Also add an entry into the boolean of keys to keep:
	m_vectKeyValsToKeep.Add(true);
}


//-----------------------------------------------------------------------------
// ExclusiveController::GetKeyVal
// This method will get the value for the specified key.
//
// Input:
//  TupString strKeyname -- The key to get the value for.
//	TupString& strValue -- Upon return, this will hold the value for the key.
//							 If the key does not exist, this value is not
//							 changed.
//
// Output: True if the key existed, false otherwise.
//-----------------------------------------------------------------------------
bool ExclusiveController::GetKeyVal(const TupString &strKeyname, TupString& strValue)
{
	bool bKeyExists = false;

	int nNumKeyVals = m_vectKeyVals.GetSize();
	for (int nIndex = 0; nIndex < nNumKeyVals; nIndex++)
	{
		if (m_vectKeyVals[nIndex].m_strKeyname == strKeyname)
		{
			strValue = m_vectKeyVals[nIndex].m_strValue;
			bKeyExists = true;
			break;
		}
	}

	return bKeyExists;
}
