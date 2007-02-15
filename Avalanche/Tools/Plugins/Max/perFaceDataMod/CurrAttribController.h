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

class CModData;

//-----------------------------------------------------------------------------
// Class Declaration:

class CurrAttribController : public CurrAttribBase
{
public:
	CurrAttribController();
	~CurrAttribController();

	// Accessors:
	CurrMeshAttribData* GetCurrMeshAttribData(int nIndex);
	void GetAllUniqueValues(const TupString &strKey, const TupString &strDefaultValue,UniqueValues &uniqueValues,Keeper<TupString> *pValidValues) const;
	int GetSize(void);
	int GetIndex(CModData *pModData);
	CModData *GetModData(int nIndex); // return the maya object from the keeper

	// Mutators:
	int AddCurrMeshAttribData(CurrMeshAttribData* pAttribData);
	int AddModData(CModData *pModData);
	void RemoveAll(void);
	void RemoveKeyInstances(const char *pKey);
	void RemoveAllKeynames();
	void AddKeyValue(const char *pKey,const char *pValue);

#ifdef _DEBUG
	void PrintDebug();
#endif

protected:
	// Protected data members:
	// Holds pointers to the CurrMeshAttribData objects:
	TupArray<CurrMeshAttribData*> m_vectAttribDataPtrs;
	Keeper<CModData *> m_modDataPtrs;	

private:
};

//-----------------------------------------------------------------------------


#endif
