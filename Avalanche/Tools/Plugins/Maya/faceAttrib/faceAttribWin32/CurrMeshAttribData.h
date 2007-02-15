//-----------------------------------------------------------------------------
//	File:		CurrMeshAttribData.h
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


#ifndef __CURRMESHATTRIBDATA__
#define __CURRMESHATTRIBDATA__

#ifndef __CURRATTRIBDATA__
#include "CurrAttribData.h"
#endif


//-----------------------------------------------------------------------------
// Includes:


//-----------------------------------------------------------------------------
// Helper Structures:

struct AttribKeynameValue
{
	TupString		m_strKeyname;
	TupString		m_strValue;
};



//-----------------------------------------------------------------------------
// Pre-declarations, etc.:



//-----------------------------------------------------------------------------
// Class Declaration:

class CurrMeshAttribData
{
public:
	CurrMeshAttribData();
	virtual ~CurrMeshAttribData();
	
	// Accessors:
	TupString CurrObjDataToStr(void) const;

	// attribute sets
	int GetNumAttribSets(void) const;
	const CurrAttribData &GetAttribSet(int index) const;
	// faces
	int GetNumFaces(void) const;
	int GetFaceAttributeSet(int index) const;
	// selected faces
	int GetNumSelectedFaces(void) const;
	int GetSelectedFace(int selectedFaceIndex) const;
	void GetAllUniqueValues(const TupString &strKey, const TupString &strDefaultValue,UniqueValues &uniqueValues,Keeper<TupString> *pValidValues) const;
	bool FaceChanged(int faceIndex) const;
	bool SelectedFacesUsingSet(int index) const;
		
	// Mutators:
	void StrToCurrAttribData(const TupString &strData);
	void SetNumFaces(int numFaces);
	void SetFaceAttribSet(int faceIndex,int attribIndex);
	void AddSelectedFace(int faceIndex);
	void ResetChangedFaces(void);
	void SetChangedFace(int faceIndex,bool bValue);
	
	bool RemoveKeyname(const char *strKeyname);
	bool RemoveKeyname(const char *strKeyname,const char *strValname);
	bool RenameKey(const char *strOldKeyname,const char *strNewKeyname);
	bool RenameVal(const char *strKeyname,const char *strOldValname,const char *strNewValname);
	bool RemoveAllKeynames();
	bool AddKeyValue(const KeyValuePair &keyValuePair);
	bool AddKeyValue(const char *strKeyname,const char *strValue);
	bool AddKeyValue(const char *strKeyname, bool bValue);
	bool AddKeyValue(const char *strKeyname, int nValue);
	bool AddKeyValue(const char *strKeyname, float fValue);
	bool AddKeyValue(const char *strKeyname,const AttribColor &colorValue);
	
#ifdef _DEBUG
	void PrintDebug() const;
#endif
	
protected:
	// Protected methods
	bool ApplyKeyValue(const KeyValuePair &keyValuePair);
	void IsolateSelectedSets(void);
	void CompactSets(void);
	
	
	// Protected data members:
	TupArray<CurrAttribData> m_vectAttribSets;	// Holds the key value pairs for each set
	TupArray<int> m_selectedFacesUsingSets; // holds the total number of selected faces using each set
	TupArray<int> m_facesUsingSets; // holds the total number of faces using each set
	TupArray<int> m_faceAttribIndices; // size is the number of faces within the mesh - index is within the m_vectAttribData
	Keeper<int> m_selectedFaces; // keeper indicating the selected faces within the mesh
	Keeper<int> m_selectedSets; // keeper indicating which sets are being used by the selected faces
	TupArray<bool> m_changedFaces; // a bool per face indicating which ones need to have their indexes updated
};

//-----------------------------------------------------------------------------


#endif
