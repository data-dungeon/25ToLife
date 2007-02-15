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
	int GetIndex(INode *pNode) const;
	INode *GetMaxNode(int nIndex) const; // return the max node from the keeper
	
	// Mutators:
	int AddCurrAttribData(CurrAttribData* pAttribData);
	int AddMaxNode(INode *pNode);
	void RemoveAll(void);
	void RemoveKeyInstances(const char *pKey);
	void RemoveAllKeynames(void);
	void AddKeyValue(const char *pKey, const char *pValue);

	// Misc:
	TupString CurrObjDataToStr(int nIndex);
	bool FindEntityNames(const TupArray<KeyValPair> &vectCriteria, TupArray<TupString> &vectEntityNames) const;
	bool FindCheckStates(const TupString &strKeyname,const TupArray<TupString> &vectEntityNames,
		int nDefaultState, TupArray<int>& vectCheckedStates) const;
	void FormCheckListVal(const TupString &strKeyname, const TupString &strValue, bool bChecked);

	void LoadCurrAttribDataNode(INode *pNode);

#ifdef _DEBUG
	void PrintDebug(void) const;
#endif

protected:
	// Protected data members:
	// Holds pointers to the CurrAttribData objects:
	TupArray<CurrAttribData*> m_vectAttribDataPtrs;
	Keeper<INode *> m_maxNodes;	
	
	// Protected methods:
	bool LoadCurrAttribDataScene(void);
	bool StripAllButRawData(TupString& strToFix);
	bool RemoveCharsFromStr(TupString& strToFix, char chToRemove);
};

//-----------------------------------------------------------------------------


#endif
