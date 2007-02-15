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

#ifndef __CURRATTRIBBASE__
#include "../UIDisplay/CurrAttribBase.h"
#endif



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
	CurrAttribData* GetObjectPtr(int nIndex);
	bool GetAllUniqueValues(std::string strKey, std::vector<ValueString>& vectValues);

	int GetSize();
	const char *GetEntityString(int nIndex);
	int EntityExistsAt(const char *pName);

	// Mutators:
	void AddObjectPtr(CurrAttribData* pAttribData);
	void RemoveAllPtrs();
	void RemoveKeyInstances(std::string strKey);
	void RemoveAllKeynames();
	void SetStringAsString(std::string strKeyname, std::string strValue);

	// Misc:
	std::string CurrObjDataToStr(int nIndex);

#ifdef _DEBUG
	void PrintDebug();
#endif

protected:
	// Protected data members:
	// Holds pointers to the CurrAttribData objects:
	std::vector<CurrAttribData*> m_vectAttribDataPtrs;

private:
};

//-----------------------------------------------------------------------------


#endif
