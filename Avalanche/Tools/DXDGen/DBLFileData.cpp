#include "stdafx.h"
#include "DBLFileData.h"

#pragma once

DBLFileData::DBLFileData() : 
m_pDataBuffer(NULL),
m_dataBufferSize(0),
m_padSize(0)
{
	memset(&m_fileHeader,0,sizeof(m_fileHeader));
}

DBLFileData::~DBLFileData()
{
	FreeBuffers();
}

void DBLFileData::FreeBuffers(void)
{
	if (m_pDataBuffer)
	{
		delete m_pDataBuffer;
		m_pDataBuffer = NULL;
	}
}

void DBLFileData::ReadFile(const char *pFilename)
{
	FreeBuffers();

	HANDLE hFile = ::CreateFile( pFilename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0 );

	if( hFile == INVALID_HANDLE_VALUE )
	{
		throw(Exception(ERROR_FATAL,"DBLFileData::ReadFile - Error opening file\n"));
	}

	m_dataBufferSize = ::GetFileSize( hFile, 0 );

	if( m_dataBufferSize == 0 )
	{
		throw(Exception(ERROR_FATAL,"DBLFileData::ReadFile - File has no size"));
	}

	m_pDataBuffer = new u8[m_dataBufferSize];
	if(m_pDataBuffer == NULL)
	{
		throw(Exception(ERROR_FATAL,"DBLFileData::ReadFile - Error allocating data memory buffer"));
	}

	DWORD numBytesRead;
	BOOL bResult = ::ReadFile( hFile, m_pDataBuffer, m_dataBufferSize, &numBytesRead, NULL );
	if (!bResult || numBytesRead!=m_dataBufferSize)
	{
		throw(Exception(ERROR_FATAL,"DBLFileData::ReadFile - Error reading file into buffer"));
	}

	memcpy( &m_fileHeader, m_pDataBuffer, sizeof(m_fileHeader) );

	// the dbl file may have a skip value
	m_padSize = atoi( m_fileHeader.cSkipCount); 

	// copy the header again to account for the pad
	if (m_padSize != 0)
	{
		memcpy( &m_fileHeader, (void *)( (u32)m_pDataBuffer + m_padSize), sizeof(m_fileHeader) );
	}

	if( !::CloseHandle( hFile ) )
	{
		throw(Exception(ERROR_FATAL,"DBLFileData::ReadFile - Error closing the file"));
	}
}


