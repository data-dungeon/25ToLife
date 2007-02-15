//-----------------------------------------------------------------------------
//	File:		CurrAttribController.h
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


#ifndef __CURRATTRIBCONTROLLER__
#define __CURRATTRIBCONTROLLER__



//-----------------------------------------------------------------------------
// Includes:


//-----------------------------------------------------------------------------
// Helper Structures:



//-----------------------------------------------------------------------------
// Pre-declarations, etc.:

class CurrMeshAttribData;



//-----------------------------------------------------------------------------
// Class Declaration:

class CurrAttribController : public CurrAttribBase
{
public:
	CurrAttribController();
	~CurrAttribController();

	// Accessors:
	CurrMeshAttribData* GetCurrMeshAttribData(int nIndex);
	CurrMeshAttribData* GetCurrMeshAttribData(int nIndex) const;
	void GetAllUniqueValues(const TupString &strKey, const TupString &strDefaultValue,UniqueValues &uniqueValues,Keeper<TupString> *pValidValues) const;
	int GetSize(void) const;
	int GetIndex(MObject mObject) const;
	MObject GetMayaObject(int nIndex) const; // return the maya object from the keeper

	// Mutators:
	int AddCurrMeshAttribData(CurrMeshAttribData* pAttribData);
	int AddMayaObject(MObject mObject);
	void RemoveAll(void);
	bool RemoveKeyInstances(const char *pKeyTest, const char *pValueTest, const char *pKey);
	bool RemoveKeyInstances(const char *pKey);
	bool RemoveAllKeynames(void);
	bool AddKeyValue(const char *pKey, const char *pValue);

	// Misc:
	TupString CurrObjDataToStr(int nIndex);
	void LoadCurrAttribDataNode(MDagPath mDagPath);

#ifdef _DEBUG
	void PrintDebug(void) const;
#endif

	static bool ValidNodeName(const char *pName);
	static bool ValidNode(MDagPath &mDagPath);

	static void CreateOrReplaceNote(MObject object,const TupString &noteString);
	static void GetNote(MObject object,TupString &stringToFill);
	
protected:
	// Protected data members:
	// Holds pointers to the CurrMeshAttribData objects:
	TupArray<CurrMeshAttribData*> m_vectAttribDataPtrs;
	Keeper<MObject> m_mayaObjects;	

private:
};

//-----------------------------------------------------------------------------


#endif
