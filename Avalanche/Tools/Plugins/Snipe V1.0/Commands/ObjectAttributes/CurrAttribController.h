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
	int GetIndex(HSnipeObject *pObject) const;
	HSnipeObject *GetSnipeObject(int nIndex) const; // return the maya object from the keeper

	// Mutators:
	int AddCurrAttribData(CurrAttribData* pAttribData);
	int AddSnipeObject(HSnipeObject *pObject);
	void RemoveAll(void);

	// Misc:
	TupString CurrObjDataToStr(int nIndex);
	bool FindEntityNames(const TupArray<KeyValPair> &vectCriteria, Keeper<TupString> &vectEntityNames) const;
	bool FindCheckStates(const TupString &strKeyname,const Keeper<TupString> &vectEntityNames,
						 int nDefaultState, TupArray<int>& vectCheckedStates) const;
	void FormCheckListVal(const TupString &strKeyname, const TupString &strValue, bool bChecked);

	void LoadCurrAttribDataNode(HSnipeObject *pObject);

#ifdef _DEBUG
	void PrintDebug(void) const;
#endif

	static bool ValidNodeName(const char *pName);
	static bool ValidNode(HSnipeObject *pObject);

	static void CreateOrReplaceNote(HSnipeObject *pObject,const TupString &noteString);
	static void GetNote(HSnipeObject *pObject,TupString &stringToFill);
	
protected:
	// Protected data members:
	// Holds pointers to the CurrAttribData objects:
	TupArray<CurrAttribData*> m_vectAttribDataPtrs;
	Keeper<HSnipeObject *> m_snipeObjects;	
	
	// Protected methods:
	bool LoadCurrAttribDataScene(void);
	bool StripAllButRawData(TupString& strToFix);

private:
};

//-----------------------------------------------------------------------------


#endif
