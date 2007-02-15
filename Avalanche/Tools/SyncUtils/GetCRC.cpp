////////////////////////////////////////////////////////////////////////////
//
// GetCRC
//
// Class obtaining a CRC value from a file
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: GetCRC.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 3/26/03    Time: 4:23p
 * Updated in $/Avalanche/tools/SyncUtils
 * CRC checks
 */

#include "Stdafx.h"
#include "GetCRC.h"

//***********************************************
GetCRC::GetCRC() : m_pCRC32Table(NULL)
{
}

//***********************************************
GetCRC::~GetCRC()
{
	Free();
}

//***********************************************
void GetCRC::Init(void)
{
	// This is the official polynomial used by CRC32 in PKZip.
	// Often times the polynomial shown reversed as 0x04C11DB7.
	DWORD dwPolynomial = 0xEDB88320;
	int i, j;

	Free();
	m_pCRC32Table = new DWORD[256];

	DWORD dwCrc;
	for(i = 0; i < 256; i++)
	{
		dwCrc = i;
		for(j = 8; j > 0; j--)
		{
			if(dwCrc & 1)
				dwCrc = (dwCrc >> 1) ^ dwPolynomial;
			else
				dwCrc >>= 1;
		}
		m_pCRC32Table[i] = dwCrc;
	}
}

//***********************************************
void GetCRC::Free(void)
{
	delete m_pCRC32Table;
	m_pCRC32Table = NULL;
}


//***********************************************
bool GetCRC::GetCRCFromFile(const char *pFilename, DWORD &crc32) const
{
	HANDLE hFile = NULL;

	crc32 = 0xFFFFFFFF;

	try
	{
		// Is the table initialized?
		if(m_pCRC32Table == NULL)
			throw 0;

		// Open the file
		hFile = CreateFile(pFilename,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM | FILE_FLAG_SEQUENTIAL_SCAN,
			NULL);
		if(hFile == INVALID_HANDLE_VALUE)
		{
			return false;
		}
		else
		{
			BYTE buffer[4096];
			DWORD dwBytesRead;
			BOOL bSuccess = ReadFile(hFile, buffer, sizeof(buffer), &dwBytesRead, NULL);
			while(bSuccess && dwBytesRead)
			{
				// Register use:
				//		eax - CRC32 value
				//		ebx - a lot of things
				//		ecx - CRC32 value
				//		edx - address of end of buffer
				//		esi - address of start of buffer
				//		edi - CRC32 table
				__asm
				{
					// Save the esi and edi registers
					push esi
					push edi

					mov eax, crc32			// Load the pointer to crc32
					mov ecx, [eax]				// Dereference the pointer to load crc32

					mov ebx, this				// Load the CRC32 table
					mov edi, [ebx]GetCRC.m_pCRC32Table

					lea esi, buffer				// Load buffer
					mov ebx, dwBytesRead		// Load dwBytesRead
					lea edx, [esi + ebx]		// Calculate the end of the buffer

				crc32loop:
					xor eax, eax				// Clear the eax register
					mov bl, byte ptr [esi]		// Load the current source byte
					
					mov al, cl					// Copy crc value into eax
					inc esi						// Advance the source pointer

					xor al, bl					// Create the index into the CRC32 table
					shr ecx, 8

					mov ebx, [edi + eax * 4]	// Get the value out of the table
					xor ecx, ebx				// xor with the current byte

					cmp edx, esi				// Have we reached the end of the buffer?
					jne crc32loop

					// Restore the edi and esi registers
					pop edi
					pop esi

					mov eax, crc32			// Load the pointer to crc32
					mov [eax], ecx				// Write the result
				}
				bSuccess = ReadFile(hFile, buffer, sizeof(buffer), &dwBytesRead, NULL);
			}
		}
	}
	catch(...)
	{
		// An unknown exception happened, or the table isn't initialized
		return false;
	}

	if(hFile != NULL) CloseHandle(hFile);

	crc32 = ~crc32;

	return true;
}
