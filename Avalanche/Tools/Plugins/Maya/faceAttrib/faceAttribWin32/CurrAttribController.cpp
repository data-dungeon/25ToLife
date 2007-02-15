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
#include "CurrMeshAttribData.h"


struct MObjectTemp
{
	MPtrBase *		ptr;
	MFn::Type		tp;
};


#ifndef TEMPLATE_SPECIALIZATION
unsigned int GetHashValue(const MObject& entry)
#else
template<> unsigned int GetHashValue<MObject> (const MObject& entry)
#endif
{
	return (unsigned int)((const MObjectTemp *)&entry)->ptr;
}

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
int CurrAttribController::GetSize(void) const
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
int CurrAttribController::GetIndex(MObject mObject) const
{
	return m_mayaObjects.GetIndex(mObject);
}

//-----------------------------------------------------------------------------
// CurrAttribController::GetMayaObject
// This method will return the MObject in the keeper for a given index
//
// Input: int index
//
// Output: MObject from the given index
//-----------------------------------------------------------------------------
MObject CurrAttribController::GetMayaObject(int index) const
{
	assert(index>=0 && index<m_mayaObjects.GetSize());
	return m_mayaObjects[index];
}

//-----------------------------------------------------------------------------
// CurrAttribController::AddMayaObject
// This method will add the mObject to the keeper
//
// Input: MObject
//
// Output: int representing the index into the maya objects keeper
//-----------------------------------------------------------------------------
int CurrAttribController::AddMayaObject(MObject mObject)
{
	return m_mayaObjects.Add(mObject);
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
	m_mayaObjects.RemoveAll();
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
CurrMeshAttribData* CurrAttribController::GetCurrMeshAttribData(int nIndex)
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
CurrMeshAttribData* CurrAttribController::GetCurrMeshAttribData(int nIndex) const
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
// That is, if the test key and value are not currently there.
//
// Input:
//  const char *pKeyTest -- test if key is on the object
//  const char *pValueTest -- test if value is on the object
//  const char *pKey -- remove this key if key and value are not there
//
// Output: returns true if a key was removed
//-----------------------------------------------------------------------------
bool CurrAttribController::RemoveKeyInstances(const char *pKeyTest, const char *pValueTest, const char *pKey)
{
	bool bResult = false;
	int nNumPtrs = m_vectAttribDataPtrs.GetSize();
	for (int nIndex = 0; nIndex < nNumPtrs; nIndex++)
	{
		KeyValuePair keyValuePair(pKeyTest,pValueTest);
		CurrMeshAttribData *pCurrMeshAttribData = m_vectAttribDataPtrs[nIndex];
		if (pCurrMeshAttribData->RemoveKeyname(pKey))
		{
			bResult = true;
		}
	}
	return bResult;
}

//-----------------------------------------------------------------------------
// CurrAttribController::RemoveKeyInstances
// This method will go through all of the CurrAttribData objects and remove
// any instances of the key specified.
//
// Input:
//  TupString strKey -- The key to remove
//
// Output: returns true if a key was removed
//-----------------------------------------------------------------------------
bool CurrAttribController::RemoveKeyInstances(const char *pKey)
{
	bool bResult = false;
	int nNumPtrs = m_vectAttribDataPtrs.GetSize();
	for (int nIndex = 0; nIndex < nNumPtrs; nIndex++)
	{
		if ((m_vectAttribDataPtrs[nIndex])->RemoveKeyname(pKey))
		{
			bResult = true;
		}
	}
	return bResult;
}

//-----------------------------------------------------------------------------
// CurrAttribController::RemoveAllKeynames
// This method will go through all the CurrAttribData objects and remove
// all the key/values from them.
//
// Input: None.
//
// Output: returns true if any keys were  removed
//-----------------------------------------------------------------------------
bool CurrAttribController::RemoveAllKeynames()
{
	bool bResult = false;
	int nNumPtrs = m_vectAttribDataPtrs.GetSize();
	for (int nIndex = 0; nIndex < nNumPtrs; nIndex++)
	{
		if ((m_vectAttribDataPtrs[nIndex])->RemoveAllKeynames())
		{
			bResult = true;
		}
	}
	return bResult;
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
// Output: returns true if any keys were  added
//-----------------------------------------------------------------------------
bool CurrAttribController::AddKeyValue(const char *pKey, const char *pValue)
{
	bool bResult = false;
	int nNumPtrs = m_vectAttribDataPtrs.GetSize();
	for (int nIndex = 0; nIndex < nNumPtrs; nIndex++)
	{
		if ((m_vectAttribDataPtrs[nIndex])->AddKeyValue(pKey, pValue))
		{
			bResult = true;
		}
	}
	return bResult;
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

bool CurrAttribController::ValidNode(MDagPath &mDagPath)
{
	MFn::Type type = mDagPath.apiType();
	return (type == MFn::kMesh);
}

void CurrAttribController::CreateOrReplaceNote(MObject object,const TupString &noteString)
{
	MDGModifier dgModifier;
	MStatus status;
	MString command;
	MFnDependencyNode fnNode(object);
	MFnDagNode fnDagNode(object);
	MDagPath mDagPath;
	fnDagNode.getPath(mDagPath);
	TupString pathString = mDagPath.fullPathName().asChar();
	
	MString note(noteString);
	
	const char *debug = note.asChar();
	
	MPlug plug = fnNode.findPlug("notes",&status);
	if (!status)
	{
		command = MString("addAttr -dt \"string\" -ln \"notes\" -sn \"nts\" ") + mDagPath.fullPathName();
		MGlobal::executeCommand(command);
		
		plug = fnNode.findPlug("notes",&status);
		if (!status)
			return;
	}
	
	status = plug.setValue(note);
	if (!status)
		return;
	
	dgModifier.doIt();
	
	MGlobal::executeCommand("autoUpdateAttrEd");
	dgModifier.doIt();
	
}

void CurrAttribController::GetNote(MObject mObject,TupString &stringToFill)
{
	MFnDagNode dagNode(mObject);

	// Set any data:
	MStatus status;
	MPlug notesPlug = dagNode.findPlug("notes",&status);
	if (status)
	{
		MString tempString;
		notesPlug.getValue(tempString);	
		stringToFill = tempString.asChar();
	}
}

