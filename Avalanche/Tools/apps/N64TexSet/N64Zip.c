//=============================================================================
// CompFile - File Compressor.
//
//        Reads a file and compresses it using Huffman
//			 compression.
//
//			This code is based on "CompTPG".
//
//	Copyright 1997 Avalanche Software, LC.
//=============================================================================

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <ctype.h>
#include <string.h>
#include <conio.h>
#include <limits.h>

#define FLUSH_DICTIONARY 256
#define END_OF_PAGE 257

#define STARTING_CODE_SIZE 9
#define MAX_CODE_SIZE 12
#define MAX_DICTIONARY_ENTRIES (1 << MAX_CODE_SIZE)
#define DICTIONARY_HASH_ENTRIES 0x10000

typedef struct Dictionary_tag
{
	int nSize;
	unsigned char *pEntry;
} DictionaryDef;

typedef struct DictionaryHash_tag
{
	struct DictionaryHash_tag *pNext;
	int nDictionaryIndex;
} DictionaryHashDef;

DictionaryDef Dictionary[MAX_DICTIONARY_ENTRIES];

DictionaryHashDef *apHashTable[DICTIONARY_HASH_ENTRIES];

void AddBits (unsigned short data,int nBits,unsigned long **lCurrLong,int *nNextBit,int *nLongs);
void AddEntry (unsigned char *pCurrChar,
					int nCurrEntryLength,
					int *nNextCode,
					int *nCodeSize,
					unsigned long **pCompressedFile,
					int *nNextBit,
					int *nLongs);
void AddHashEntry (int nNextCode,unsigned char cCurrChar,int nCurrEntryLength);
int DictionaryEncode (unsigned char *pCurrBuffer,int nBufferLength,unsigned long *pCompressedFile);
int FindEntryCode (unsigned char *pCurrChar,int nCurrEntryLength,int nEntries);
int FindHashIndex (unsigned char cCurrChar,int nCurrEntryLength);
void InitDictionary (void);
void PrintString (unsigned char *pStr,int nLength);
unsigned long SwapLong (unsigned long lIn);

int DictionaryEncode (unsigned char *pCurrBuffer,int nBufferLength,unsigned long *pCompressedFile)
{

	int nNextBit,
		 nLongs = 0,
		 nCurrEntryLength = 1,
		 nPrevEntryLength,
		 nCurrCode,
		 nPrevCode,
		 nNextCode = END_OF_PAGE + 1,
		 nCodeSize = STARTING_CODE_SIZE,
		 nBytesOutput = 0;

	unsigned char *pCurrChar;
	
	nNextBit = 31;

	*pCompressedFile = 0;

	InitDictionary ();

	pCurrChar = pCurrBuffer;

	AddBits (FLUSH_DICTIONARY,nCodeSize,&pCompressedFile,&nNextBit,&nLongs);
	nPrevCode = FindEntryCode (pCurrChar,nCurrEntryLength,nNextCode);
	while ((pCurrChar + nCurrEntryLength) <= (pCurrBuffer + nBufferLength))
	{
		nCurrCode = FindEntryCode (pCurrChar,nCurrEntryLength,nNextCode);
		if (nCurrCode != -1)
		{
			nPrevCode = nCurrCode;
			nPrevEntryLength = nCurrEntryLength;
			nCurrEntryLength++;
		}
		else
		{
			AddBits ((unsigned short)nPrevCode,nCodeSize,&pCompressedFile,&nNextBit,&nLongs);
			nBytesOutput += Dictionary[nPrevCode].nSize;
			AddEntry (pCurrChar,nCurrEntryLength,&nNextCode,&nCodeSize,&pCompressedFile,&nNextBit,&nLongs);
			pCurrChar += nPrevEntryLength;
			nCurrEntryLength = 1;
		}
	}
	AddBits ((unsigned short)nPrevCode,nCodeSize,&pCompressedFile,&nNextBit,&nLongs);
	nBytesOutput += Dictionary[nPrevCode].nSize;
	AddBits (END_OF_PAGE,nCodeSize,&pCompressedFile,&nNextBit,&nLongs);

	if (nNextBit != 31)
	{
		nLongs++;
	}

	return nLongs;
}

void InitDictionary (void)
{

	int nIndex,
		 nNextCode = 0,
		 nCodeSize = STARTING_CODE_SIZE;

	unsigned char *pInitialEntries;

	for (nIndex = 0; nIndex < DICTIONARY_HASH_ENTRIES; nIndex++)
	{
		apHashTable[nIndex] = NULL;
	}

	pInitialEntries = (unsigned char *)malloc (256);
	for (nIndex = 0; nIndex < 256; nIndex++)
	{
		pInitialEntries[nIndex] = (unsigned char )nIndex;
		AddEntry (pInitialEntries + nIndex,1,&nNextCode,&nCodeSize,NULL,NULL,NULL);
	}
}

void AddEntry (unsigned char *pCurrChar,
					int nCurrEntryLength,
					int *nNextCode,
					int *nCodeSize,
					unsigned long **pCompressedFile,
					int *nNextBit,
					int *nLongs)
{
	if (*nNextCode == (1 << MAX_CODE_SIZE))
	{
		AddBits (FLUSH_DICTIONARY,*nCodeSize,pCompressedFile,nNextBit,nLongs);
		*nCodeSize = STARTING_CODE_SIZE;
		*nNextCode = END_OF_PAGE + 1;
		InitDictionary ();
		return;
	}
	Dictionary[*nNextCode].pEntry = pCurrChar;
	Dictionary[*nNextCode].nSize = nCurrEntryLength;
	AddHashEntry (*nNextCode,*pCurrChar,nCurrEntryLength);
	(*nNextCode)++;
	if (*nNextCode == (1 << *nCodeSize))
	{
		if (*nCodeSize < MAX_CODE_SIZE)
		{
			(*nCodeSize)++;
		}
	}
}

void AddHashEntry (int nNextCode,unsigned char cCurrChar,int nCurrEntryLength)
{

	DictionaryHashDef *pNewHashEntry;

	int nHashIndex;

	pNewHashEntry = malloc (sizeof(DictionaryHashDef));
	nHashIndex = FindHashIndex (cCurrChar,nCurrEntryLength);
	pNewHashEntry->nDictionaryIndex = nNextCode;
	pNewHashEntry->pNext = apHashTable[nHashIndex];
	apHashTable[nHashIndex] = pNewHashEntry;
}

int FindHashIndex (unsigned char cCurrChar,int nCurrEntryLength)
{

	int nHashIndex = 0,
		 nIndex;

	for (nIndex = 0; nIndex < sizeof(int); nIndex++)
	{
		nHashIndex += nCurrEntryLength & 0xFF;
		nCurrEntryLength >>= 8;
	}
	nHashIndex <<= 8;
	nHashIndex &= 0xFF00;
	nHashIndex |= cCurrChar;
	return nHashIndex;
}

int FindEntryCode (unsigned char *pCurrChar,int nCurrEntryLength,int nEntries)
{
	
	int nHashIndex,
		 nIndex;

	DictionaryHashDef *pHashEntry;

	nHashIndex = FindHashIndex (*pCurrChar,nCurrEntryLength);
	pHashEntry = apHashTable[nHashIndex];
	while (pHashEntry != NULL)
	{
		nIndex = pHashEntry->nDictionaryIndex;
		if (nCurrEntryLength == Dictionary[nIndex].nSize)
		{
			if (!memcmp (pCurrChar,Dictionary[nIndex].pEntry,nCurrEntryLength))
			{
				return nIndex;
			}
		}
		pHashEntry = pHashEntry->pNext;
	}

	return -1;
}

void AddBits (unsigned short data,int nBits,unsigned long **lCurrLong,int *nNextBit,int *nLongs)
{

	long iData = data;

	iData <<= 32 - nBits;
	while (nBits)
	{
		if (*nNextBit == -1)
		{
			(*nLongs)++;
			(*lCurrLong)++;
			**lCurrLong = 0;
			*nNextBit = 31;
		}
		if (iData < 0)
		{
			**lCurrLong |= 1 << *nNextBit;
		}
		(*nNextBit)--;
		nBits--;
		iData <<= 1;
	}
}
