//-----------------------------------------------------------------------------
//	File:		ExclusiveController.h
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


#ifndef __EXCLUSIVECONTROLLER__
#define __EXCLUSIVECONTROLLER__



//-----------------------------------------------------------------------------
// Includes:



//-----------------------------------------------------------------------------
// Defines, globals, etc.:



//-----------------------------------------------------------------------------
// Helper Structures:

struct KeyVals
{
	TupString m_strKeyname;
	TupString m_strValue;
};



//-----------------------------------------------------------------------------
// Class Declaration:

class ExclusiveController
{
public:
	ExclusiveController();
	~ExclusiveController();

	void Init(const char* pChKeyName);
	void SaveKeyValsInReg(void);
	void SetKeeperFlag(TupString strKeyname);
	void TossUnwantedKeys(void);
	void SetKeyVal(const TupString &strKeyname,const TupString &strValue);
	bool GetKeyVal(const TupString &strKeyname, TupString &strValue);

protected:
	// Protected data members:
	TupArray<KeyVals> m_vectKeyVals;	// Holds the nonexclusive key/vals.
	TupArray<bool> m_vectKeyValsToKeep;	// Specifies which key/vals to keep.
	TupString m_keyName;

private:
};

//-----------------------------------------------------------------------------


#endif
