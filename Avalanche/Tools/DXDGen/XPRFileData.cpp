#include "stdafx.h"
#include "XPRFileTypes.h"
#include "XPRFileData.h"

XPRFileData::XPRFileData() : 
	m_pHeaderBuffer(NULL),
	m_pVideoBuffer(NULL),
	m_headerBufferSize(0),
	m_videoBufferSize(0),
	m_bFileRead(false)

{
	memset(&m_fileHeader,0,sizeof(m_fileHeader));
}

XPRFileData::~XPRFileData()
{
	FreeBuffers();
}

void XPRFileData::FreeBuffers(void)
{
	if (m_pHeaderBuffer)
	{
		delete m_pHeaderBuffer;
		m_pHeaderBuffer = NULL;
		m_headerBufferSize = 0;
	}

	if (m_pVideoBuffer)
	{
		delete m_pVideoBuffer;
		m_pVideoBuffer = NULL;
		m_videoBufferSize = 0;
	}
}

void XPRFileData::ReadFile(const char *pFilename)
{
	FreeBuffers();

	FILE * pFile = fopen( pFilename, "rb" );

	if( !pFile )
	{
		throw(Exception(ERROR_FATAL,"XPRFileData::ReadFile - Error opening file\n"));
	}

	if( fread( & m_fileHeader, sizeof( XPRFileHeader ), 1, pFile ) != 1 )
	{
		throw(Exception(ERROR_FATAL,"XPRFileData::ReadFile - Error reading file header\n"));
	}

	m_headerBufferSize = m_fileHeader.dwHeaderSize-sizeof( XPRFileHeader );
	m_videoBufferSize = m_fileHeader.dwTotalSize-m_fileHeader.dwHeaderSize;

	if( m_headerBufferSize )
	{
		m_pHeaderBuffer = new u8[m_headerBufferSize];

		if( m_pHeaderBuffer == NULL )
		{
			throw(Exception(ERROR_FATAL,"XPRFileData::ReadFile - Error allocating header memory buffer"));
		}

		if( fread( m_pHeaderBuffer, m_headerBufferSize, 1, pFile ) != 1 )
		{
			throw(Exception(ERROR_FATAL,"XPRFileData::ReadFile - Error reading header memory data"));
		}

	}

	if( m_videoBufferSize )
	{
		m_pVideoBuffer = new u8[m_videoBufferSize];

		if( m_pVideoBuffer == 0 )
		{
			throw(Exception(ERROR_FATAL,"XPRFileData::ReadFile - Error allocating video memory buffer"));
		}

		if( fread( m_pVideoBuffer, m_videoBufferSize, 1, pFile ) != 1 )
		{
			throw(Exception(ERROR_FATAL,"XPRFileData::ReadFile - Error reading video memory data"));
		}
	}

	fclose( pFile );

	m_bFileRead = true;
}


