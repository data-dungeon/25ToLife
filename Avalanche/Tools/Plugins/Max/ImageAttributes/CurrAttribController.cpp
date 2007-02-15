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

#include "ImageAttributesPCH.h"
#include "CurrAttribController.h"
#include "CurrAttribData.h"



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
	this->RemoveAllPtrs();
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
	return m_vectAttribDataPtrs.size();
}


//-----------------------------------------------------------------------------
// CurrAttribController::GetObjectPtr
// This method will return the CurrAttribData pointer specified by the index
// passed in.
//
// Input:
//  int nIndex -- The index of the pointer to get.
//
// Output: CurrAttribData* to get.  NOTE: If the index passed in is out of the
//		   vector's bounds, then NULL is passed back.
//-----------------------------------------------------------------------------
CurrAttribData* CurrAttribController::GetObjectPtr(int nIndex)
{
	int nVectSize = m_vectAttribDataPtrs.size();
	if (nIndex >= 0 && nIndex < nVectSize)
		return m_vectAttribDataPtrs.at(nIndex);
	else
		return NULL;
}


//-----------------------------------------------------------------------------
// CurrAttribController::GetEntityPtr
// This method will return the entity pointer of the CurrAttribData object
// specified by the index passed in.
//
// Input:
//  int nIndex -- The index of the object to get the entity pointer of.
//
// Output: const char *representing the name of the entity.  NOTE: If the index passed in
//		   is out of the vector's bounds, then NULL is passed back.
//-----------------------------------------------------------------------------
const char *CurrAttribController::GetEntityString(int nIndex)
{
	int nVectSize = m_vectAttribDataPtrs.size();
	if (nIndex >= 0 && nIndex < nVectSize)
		return m_vectAttribDataPtrs.at(nIndex)->GetEntityString();
	else
		return NULL;	
}


//-----------------------------------------------------------------------------
// CurrAttribController::EntityExistsAt
// This method will see if there is a CurrAttribData in the m_vectAttribDataPtrs
// that has the same entity pointer as the node passed in.  If it does, the
// index in the vector is passed back.
//
// Input:
//  const char *pName -- The name to look for
//
// Output: index of the found entity, otherwise -1 if it is not found.
//-----------------------------------------------------------------------------
int CurrAttribController::EntityExistsAt(const char *pName)
{
	int nIndex = -1;

	int nVectSize = m_vectAttribDataPtrs.size();
	for (int nLoop = 0; nLoop < nVectSize; nLoop++)
	{
		if (!stricmp(m_vectAttribDataPtrs.at(nLoop)->GetEntityString(),pName))
		{
			nIndex = nLoop;
			break;
		}
	}

	return nIndex;
}


//-----------------------------------------------------------------------------
// CurrAttribController::GetAllUniqueValues
// This method will look through all the CurrAttribData objects for the key
// specified, it will then return all the unique values for the key.
//
// Input:
//  std::string strKey -- The key to get the values for.
//  std::vector<ValueString>& vectValues -- The vector to store the value
//											strings in.
//
// Output: True if any of the CurrAttribData objects do not have the key
//		   specified, false if they all have the key.
//-----------------------------------------------------------------------------
bool CurrAttribController::GetAllUniqueValues(std::string strKey, std::vector<ValueString>& vectValues)
{
	bool bObjWithNoKey = false;

	// First, empty out the incoming vector:
	vectValues.clear();

	// Cycle through all of the CurrAttribData objects:
	int nNumObjects = m_vectAttribDataPtrs.size();
	for (int nIndex = 0; nIndex < nNumObjects; nIndex++)
	{
		int nKeyAt = m_vectAttribDataPtrs.at(nIndex)->KeynameExistsAt(strKey);
		if (nKeyAt >= 0)
		{
			std::string strValue = m_vectAttribDataPtrs.at(nIndex)->GetValueAsString(nKeyAt);
			// See if this value is already in the vector, if not add it:
			bool bAddValue = true;
			int nNumValues = vectValues.size();
			for (int nLoop = 0; nLoop < nNumValues; nLoop++)
			{
				if (vectValues.at(nLoop).m_strValue == strValue)
				{
					bAddValue = false;
					break;
				}
			}
			if (bAddValue == true)
			{
				ValueString newValString;
				newValString.m_strValue = strValue;
				vectValues.push_back(newValString);
			}
		}
		else
		{
			bObjWithNoKey = true;
		}
	}

	return bObjWithNoKey;
}


//-----------------------------------------------------------------------------
// CurrAttribController::AddObjectPtr
// This method will add a CurrAttribData pointer to the vector.
//
// Input:
//  CurrAttribData* pAttribData -- The pointer to add.
//
// Output: None.
//-----------------------------------------------------------------------------
void CurrAttribController::AddObjectPtr(CurrAttribData* pAttribData)
{
	m_vectAttribDataPtrs.push_back(pAttribData);
}


//-----------------------------------------------------------------------------
// CurrAttribController::RemoveAllPtrs
// This method will delete all of the objects that the pointers point to and
// then clear out the pointer vector.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void CurrAttribController::RemoveAllPtrs()
{
	int nVectSize = m_vectAttribDataPtrs.size();
	for (int nIndex = 0; nIndex < nVectSize; nIndex++)
	{
		if (m_vectAttribDataPtrs.at(nIndex) != NULL)
		{
			delete m_vectAttribDataPtrs.at(nIndex);
			m_vectAttribDataPtrs.at(nIndex) = NULL;
		}
	}
	m_vectAttribDataPtrs.clear();
}


//-----------------------------------------------------------------------------
// CurrAttribController::RemoveKeyInstances
// This method will go through all of the CurrAttribData objects and remove
// any instances of the key specified.
//
// Input:
//  std::string strKey -- The key to remove
//
// Output: None.
//-----------------------------------------------------------------------------
void CurrAttribController::RemoveKeyInstances(std::string strKey)
{
	int nNumPtrs = m_vectAttribDataPtrs.size();
	for (int nIndex = 0; nIndex < nNumPtrs; nIndex++)
	{
		m_vectAttribDataPtrs.at(nIndex)->RemoveKeyname(strKey);
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
	int nNumPtrs = m_vectAttribDataPtrs.size();
	for (int nIndex = 0; nIndex < nNumPtrs; nIndex++)
	{
		m_vectAttribDataPtrs.at(nIndex)->RemoveAllKeynames();
	}
}


//-----------------------------------------------------------------------------
// CurrAttribController::SetStringAsString
// This method will set the keyname and value on all the CurrAttribData objects
// in the vector.
//
// Input:
//  std::string strKeyname -- The keyname to set.
//  std::string strValue -- The value to set.
//
// Output: None.
//-----------------------------------------------------------------------------
void CurrAttribController::SetStringAsString(std::string strKeyname, std::string strValue)
{
	int nNumPtrs = m_vectAttribDataPtrs.size();
	for (int nIndex = 0; nIndex < nNumPtrs; nIndex++)
	{
		m_vectAttribDataPtrs.at(nIndex)->SetStringAsString(strKeyname, strValue);
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
// Output: std::string representing all the keyname-value sets in the vector.
//		   The string passed back will be in the format:
//				keyname=value\n
//				keyname=value\n
//				keyname=value\n
//		   NOTE: If the index passed in is out of the vector's bounds, then
//				 the string passed back will be empty.
//-----------------------------------------------------------------------------
std::string CurrAttribController::CurrObjDataToStr(int nIndex)
{
	std::string strEmpty = "";

	int nVectSize = m_vectAttribDataPtrs.size();
	if (nIndex >= 0 && nIndex < nVectSize)
		return m_vectAttribDataPtrs.at(nIndex)->CurrObjDataToStr();
	else
		return strEmpty;
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
	int nVectSize = m_vectAttribDataPtrs.size();
	for (int nIndex = 0; nIndex < nVectSize; nIndex++)
	{
		m_vectAttribDataPtrs.at(nIndex)->PrintDebug();
	}
}
#endif
