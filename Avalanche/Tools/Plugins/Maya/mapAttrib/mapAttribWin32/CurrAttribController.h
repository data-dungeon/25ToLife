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

class CurrAttribData;



//-----------------------------------------------------------------------------
// Class Declaration:

class CurrAttribController : public CurrAttribBase
{
public:
	CurrAttribController();
	~CurrAttribController();

	// Accessors:
	CurrAttribData* GetCurrAttribData(int nIndex);
	const CurrAttribData* GetCurrAttribData(int nIndex) const;
	void GetAllUniqueValues(const TupString &strKey, const TupString &strDefaultValue,UniqueValues &uniqueValues,Keeper<TupString> *pValidValues) const;
	int GetSize(void) const;
	int GetIndex(MObject mObject) const;
	MObject GetMayaObject(int nIndex) const; // return the maya object from the keeper

	// Mutators:
	int AddCurrAttribData(CurrAttribData* pAttribData);
	int AddMayaObject(MObject mObject);
	void RemoveAll(void);
	bool RemoveKeyInstances(const char *pKeyTest, const char *pValueTest, const char *pKey);
	bool RemoveKeyInstances(const char *pKey);
	bool RemoveAllKeynames(void);
	bool AddKeyValue(const char *pKey, const char *pValue);

	// Misc:
	TupString CurrObjDataToStr(int nIndex);
	bool FindEntityNames(const TupArray<KeyValPair> &vectCriteria, TupArray<TupString> &vectEntityNames) const;
	bool FindCheckStates(const TupString &strKeyname,const TupArray<TupString> &vectEntityNames,
						 int nDefaultState, TupArray<int>& vectCheckedStates) const;
	void FormCheckListVal(const TupString &strKeyname, const TupString &strValue, bool bChecked);

	void LoadCurrAttribDataNode(MObject mObject);

#ifdef _DEBUG
	void PrintDebug(void) const;
#endif

	static void CreateOrReplaceNote(MObject object,const TupString &noteString);
	static void GetNote(MObject object,TupString &stringToFill);

	static void GetFileTextures(MDagPath dagPath,MObject component,MObjectArray &fileTextures);

protected:
	// Protected data members:
	// Holds pointers to the CurrAttribData objects:
	TupArray<CurrAttribData*> m_vectAttribDataPtrs;
	Keeper<MObject> m_mayaObjects;	
	
	// Protected methods:
	bool LoadCurrAttribDataScene(void);
	bool StripAllButRawData(TupString& strToFix);

private:
};

//-----------------------------------------------------------------------------


#endif
