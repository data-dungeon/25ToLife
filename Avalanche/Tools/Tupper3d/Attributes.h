////////////////////////////////////////////////////////////////////////////
//
// Attributes
//
// Class deal the attributes of nodes and faces
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: Attributes.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 3/18/02    Time: 4:26p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added Attributes class
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_ATTRIBUTES_H
#define TUPPER3D_ATTRIBUTES_H

class TupAttributes
{
public:
	struct KeyValuePair
	{
		std::string	m_strKey;
		std::string	m_strValue;
	};

	struct AttribColor
	{
		int m_nRed;
		int m_nGreen;
		int m_nBlue;
	};

private:
	// Private Default constructor so no one can use it
	TupAttributes(void);

	std::vector<KeyValuePair> m_keyValuePairArray;	// Holds the actual data.

public:
	TupAttributes(const char *pBuffer,const char *pBlockStart=NULL,const char *pBlockEnd=NULL);

	int KeyExistsAt(const char *pKey) const;
	const char *GetKeyAsString(int nIndex) const;
	const char *GetValueAsString(int nIndex) const;
	bool GetValueAsBool(int nIndex) const;
	int GetValueAsInt(int nIndex) const;
	float GetValueAsFloat(int nIndex) const;
	AttribColor GetValueAsColor(int nIndex) const;
	int GetNumAttribs() const;
private:
	bool StripAllButRawData(std::string& strToFix,const char *pBlockStart,const char *pBlockEnd);
	bool RemoveCharsFromStr(std::string& strToFix, char chToRemove);

};


#endif
