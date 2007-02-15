////////////////////////////////////////////////////////////////////////////
//
// TupperwareStringManager
//
// This file contains the code compressing the strings down to a unique string table.  
// This class is used for saving the Tupperware data into a binary file.
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERWARESTRINGMANAGER_H
#define TUPPERWARESTRINGMANAGER_H

#ifndef TUPPERWARE_ARRAY_H
#include "tuparray.h"
#endif

#ifndef TUPPERWARE_STRING_H
#include "tupstring.h"
#endif

#ifndef TUPPERWARE_KEEPER_H
#include "keeper.h"
#endif

////////////////////////////////////////////////////////////////////////////

class TupperwareStringManager
{
	Keeper<TupString> m_stringKeeper;
public:
	TupperwareStringManager(void);
	~TupperwareStringManager();

	int AddString(const char *string);
	const char *GetString(int index) const;
	void BuildBuffer(char **pBuffer,int &bufferSize);
	void BuildFromBuffer(const char *pBuffer,int bufferSize);
};

////////////////////////////////////////////////////////////////////////////

#endif
