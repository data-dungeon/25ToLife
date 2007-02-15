////////////////////////////////////////////////////////////////////////////
//
// TupperwareStringManager
//
// This file contains the code compressing the strings down to a unique string table.  
// This class is used for saving the Tupperware data into a binary file.
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#include "TupperwarePch.h"
#include "stringmanager.h"

////////////////////////////////////////////////////////////////////////////

TupperwareStringManager::TupperwareStringManager(void)
{
	m_stringKeeper.SetHashTableSize(1024);
	TupString emptyString;
	m_stringKeeper.Add(emptyString);
}

TupperwareStringManager::~TupperwareStringManager()
{
}

int TupperwareStringManager::AddString(const char *pString)
{
	if (pString)
	{
		TupString newString(pString);
		return m_stringKeeper.Add(newString);
	}
	// if string is NULL use index 0
	else
	{
		return 0;
	}
}

const char *TupperwareStringManager::GetString(int index) const
{
	return m_stringKeeper[index];
}

// allocate a buffer for the string and fill with the strings from the keeper
void TupperwareStringManager::BuildBuffer(char **pBuffer,int &bufferSize)
{
	int keeperSize = m_stringKeeper.GetSize();
	int keeperIndex;
	bufferSize = 0;
	for (keeperIndex=0;keeperIndex<keeperSize;keeperIndex++)
	{
		bufferSize += m_stringKeeper[keeperIndex].GetLength()+1;
	}
	if (bufferSize)
	{
		*pBuffer = new char[bufferSize];
		char *pCurrent = *pBuffer;
		for (keeperIndex=0;keeperIndex<keeperSize;keeperIndex++)
		{
			int length = m_stringKeeper[keeperIndex].GetLength()+1;
			memcpy(pCurrent,(const char *)m_stringKeeper[keeperIndex],length);
			pCurrent += length;
		}
	}
	else
	{
		*pBuffer = NULL;
	}
}

void TupperwareStringManager::BuildFromBuffer(const char *pBuffer,int bufferSize)
{
	m_stringKeeper.RemoveAll();

	int bufferIndex = 0;
	while (bufferIndex<bufferSize)
	{
		TupString newString(&pBuffer[bufferIndex]);
		m_stringKeeper.Add(newString);
		bufferIndex+=newString.GetLength()+1;
	}
}

////////////////////////////////////////////////////////////////////////////
