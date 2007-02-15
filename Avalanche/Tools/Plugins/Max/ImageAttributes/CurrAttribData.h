//-----------------------------------------------------------------------------
//	File:		CurrAttribData.h
//	Created:	Nov. 29, 2001
//				Copyright (c) 2001, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The CurrAttribData class handles storing, getting, setting, etc. of
//			the current attribute data for the selected entity(s).
//
//	History:	11/29/2001: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------


#ifndef __CURRATTRIBDATA__
#define __CURRATTRIBDATA__



//-----------------------------------------------------------------------------
// Includes:

#ifndef __CONFIGBASE__
#include "../UIDisplay/ConfigBase.h"
#endif


//-----------------------------------------------------------------------------
// Helper Structures:

struct AttribKeynameValue
{
	std::string		m_strKeyname;
	std::string		m_strValue;
};



//-----------------------------------------------------------------------------
// Pre-declarations, etc.:



//-----------------------------------------------------------------------------
// Class Declaration:

class CurrAttribData
{
public:
	CurrAttribData();
	virtual ~CurrAttribData();

	// Accessors:
	int KeynameExistsAt(std::string strKeyname);
	std::string GetKeyAsString(int nIndex);
	std::string GetValueAsString(int nIndex);
	bool GetValueAsBool(int nIndex);
	int GetValueAsInt(int nIndex);
	float GetValueAsFloat(int nIndex);
	AttribColor GetValueAsColor(int nIndex);
	std::string CurrObjDataToStr();
	const char *GetEntityString();
	int GetNumAttribs();

	// Mutators:
	void RemoveKeyname(int nIndex);
	void RemoveKeyname(std::string strKeyname);
	void RemoveAllKeynames();
	void StrToCurrAttribData(std::string strData);
	void SetStringAsString(std::string strKeyname, std::string strValue);
	void SetBoolAsString(std::string strKeyname, bool bValue);
	void SetIntAsString(std::string strKeyname, int nValue);
	void SetFloatAsString(std::string strKeyname, float fValue);
	void SetColorAsString(std::string strKeyname, AttribColor colorValue);
	void SetEntityString(const char *pNewEntity);

	// Misc:
#ifdef _DEBUG
	void PrintDebug();
#endif

protected:
	// Protected data members:
	std::vector<AttribKeynameValue> m_vectAttribData;	// Holds the actual data.
	std::string m_pEntityString; // Points to whatever component or object that the attrib
								 // data pertains to.

private:
};

//-----------------------------------------------------------------------------


#endif
