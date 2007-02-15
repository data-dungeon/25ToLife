#include "XPRBundler.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <d3dx8.h>
#include <xgraphics.h>

#define INITIAL_HEADER_ALLOC	1024
#define INITIAL_DATA_ALLOC		65536

//-----------------------------------------------------------------------------
// Name: CBundler()
// Desc: Sets initial values of member variables
//-----------------------------------------------------------------------------
XPRBundler::XPRBundler( )
{
	m_dwOutputFileVersion = VERSION_XPR0;
    
	m_strXPR[0]    = '\0';
	m_extXPR = strdup( XPREXT );

	m_strHDR[0]    = '\0';
	m_extHDR = strdup( HDREXT );

	m_strERR[0]    = '\0';
	m_strPrefix[0] = '\0';

	m_hfXPR    = INVALID_HANDLE_VALUE;
	m_hfHDR    = INVALID_HANDLE_VALUE;
	m_hfERR    = INVALID_HANDLE_VALUE;
	
	m_pbHeaderBuff = NULL;
	m_cbHeader     = 0;
	m_pbDataBuff   = NULL;
	m_cbData       = 0;
    
	m_dwOffsetsTableSize     = 0L;
	m_dwOffsetsTablePadBytes = 0L;
	m_dwBaseResourceOffset   = 0L;

	m_pResources     = NULL;
	m_dwNumResources = 0;
}

//-----------------------------------------------------------------------------
// Name: ~CBundler()
// Desc: Performs any needed cleanup before shutting down the object
//-----------------------------------------------------------------------------
XPRBundler::~XPRBundler( )
{
	closeFiles( );

	if( m_pbHeaderBuff )
		free( m_pbHeaderBuff );

	if( m_pbDataBuff )
		free( m_pbDataBuff );

	if( m_pResources )
		delete[] m_pResources;

	if( m_extXPR )
		free( m_extXPR );

	if( m_extHDR )
		free( m_extHDR );
}

//-----------------------------------------------------------------------------
// Name: setXPRExt( )
// Desc: Change default extension for XPR File
//-----------------------------------------------------------------------------
void XPRBundler::setXPRExt( const char * pXPRExt )
{
	assert( pXPRExt );
	assert( strlen( pXPRExt ) > 0 );

	if( m_extXPR )
		free( m_extXPR );

	m_extXPR = strdup( pXPRExt );
}

//-----------------------------------------------------------------------------
// Name: setHDRExt( )
// Desc: Change default extension for HDR File
//-----------------------------------------------------------------------------
void XPRBundler::setHDRExt( const char * pHDRExt )
{
	assert( pHDRExt );
	assert( strlen( pHDRExt ) > 0 );

	if( m_extHDR )
		free( m_extHDR );

	m_extHDR = strdup( pHDRExt );
}

//-----------------------------------------------------------------------------
// Name: InternalPrintMessage()
// Desc: Helper function for ErrorMsg, WarningMsg and Msg
//-----------------------------------------------------------------------------
void XPRBundler::internalPrintMessage( const char* strPrefix, const char* strFmt, va_list arglist )
{
	char strTemp[MAX_PATH];

	if( INVALID_HANDLE_VALUE == m_hfERR )
		m_hfERR = CreateFile( m_strERR, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );

	// Print to standard out.
	printf( "%s", strPrefix );
	vprintf( strFmt, arglist );

	if( INVALID_HANDLE_VALUE != m_hfERR)
	{
		// Have to use _vsnprintf to avoid potential buffer overruns.
		const int numElements = sizeof(strTemp) / sizeof(strTemp[0]);
		_vsnprintf( strTemp, numElements, strFmt, arglist );
		// _vsnprintf does not guarantee null-termination, so we have to explicitly force it.
		strTemp[numElements-1] = 0;
		DWORD cb;
		// Write the prefix.
		WriteFile( m_hfERR, strPrefix, strlen( strPrefix ), &cb, NULL );
		// Write the formatted data.
		WriteFile( m_hfERR, strTemp, strlen( strTemp ), &cb, NULL );
	}
}

//-----------------------------------------------------------------------------
// Name: ErrorMsg()
// Desc: Prints an error message to stdout, and writes to the error file
//-----------------------------------------------------------------------------
void XPRBundler::errorMsg( const char* strFmt, ... )
{
	va_list arglist;
	va_start( arglist, strFmt );

	// For Visual Studio to recognize the error message it must fit a certain
	// format. A tool name (or sourcefile(linenum)) then " : error :" or
	// ": warning :". See Kevin Perry for details on this format.
	internalPrintMessage( "XPRBundler : error : ", strFmt, arglist );

	va_end( arglist );
}

//-----------------------------------------------------------------------------
// Name: WarningMsg()
// Desc: Prints a warning message to stdout, and writes to the error file
//-----------------------------------------------------------------------------
void XPRBundler::warningMsg( const char* strFmt, ... )
{
	va_list arglist;
	va_start( arglist, strFmt );

	// For Visual Studio to recognize the error message it must fit a certain
	// format. A tool name (or sourcefile(linenum)) then " : error :" or
	// ": warning :". See Kevin Perry for details on this format.
	internalPrintMessage( "XPRBundler : warning : ", strFmt, arglist );

	va_end( arglist );
}

//-----------------------------------------------------------------------------
// Name: Msg()
// Desc: Prints a message to stdout, and writes to the error file
//-----------------------------------------------------------------------------
void XPRBundler::standardMsg( const char* strFmt, ... )
{
	va_list arglist;
	va_start( arglist, strFmt );

	internalPrintMessage( "", strFmt, arglist );

	va_end( arglist );
}


HRESULT XPRBundler::setBaseFilename( const char * strFile )
{
	if( strFile == 0 )
		return E_INVALIDARG;

	unsigned int lenFileName = strlen( strFile );

	if( lenFileName == 0 )
		return E_INVALIDARG;

	// Copy the base filename + appropriate extension to each string
	strcpy( m_strXPR, strFile );
	strcat( m_strXPR, m_extXPR );

	strcpy( m_strHDR, strFile );
	strcat( m_strHDR, m_extHDR );

	strcpy( m_strERR, strFile );
	strcat( m_strERR, ERREXT );

	strcpy( m_strPath, m_strXPR );

	if( strrchr( m_strPath, '\\' ) )
		strrchr( m_strPath, '\\' )[1] = 0;
	else
		m_strPath[0] = 0;

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Parses and validates the command line arguments
//-----------------------------------------------------------------------------
HRESULT XPRBundler::startBundle( const char * strFile )
{
	HRESULT result = setBaseFilename( strFile );

	if( result != S_OK )
		return result;

    // Allocate buffers for storing data
	if( m_pResources == 0 )
	{
		m_pResources = new RESOURCE_STRUCT[MAX_RESOURCES];
		if( NULL == m_pResources )
			return E_OUTOFMEMORY;
	}

	if( m_pbHeaderBuff == 0 )
	{
		m_pbHeaderBuff = (BYTE *) malloc( INITIAL_HEADER_ALLOC );

		if( NULL == m_pbHeaderBuff )
        return E_OUTOFMEMORY;
		
		m_cbHeaderAlloc = INITIAL_HEADER_ALLOC;
	}

	if( m_pbDataBuff == 0 )
	{
		m_pbDataBuff = (BYTE *) malloc( INITIAL_DATA_ALLOC );
		if( NULL == m_pbDataBuff )
        return E_OUTOFMEMORY;

		m_cbDataAlloc = INITIAL_DATA_ALLOC;
	}

	return S_OK;
}

void XPRBundler::addResourceItem( const char * strIdentifier, const char * strName, DWORD dwHdrSize, DWORD dwDataSize )
{
	assert( strlen( strIdentifier ) < 127 );
	assert( strlen( strName ) < 127 );
	assert( m_dwNumResources < MAX_RESOURCES );

	strcpy( m_pResources[m_dwNumResources].strIdentifier, strIdentifier );
	strcpy( m_pResources[m_dwNumResources].strName, strName );

    m_pResources[m_dwNumResources++].dwOffset = m_cbHeader;
    m_cbHeader += dwHdrSize;
}

//-----------------------------------------------------------------------------
// Name: OpenFiles()
// Desc: Opens file handles for all our output files
//-----------------------------------------------------------------------------
HRESULT XPRBundler::openFiles( )
{
	assert( strlen( m_strXPR ) > 0 );
	assert( strlen( m_strHDR ) > 0 );
	assert( strlen( m_strERR ) > 0 );

	m_hfXPR = CreateFile( m_strXPR, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );
	if( INVALID_HANDLE_VALUE == m_hfXPR )
	{
		errorMsg( "Couldn't open XPR file <%s>\n", m_strXPR );
		return E_FAIL;
	}

	m_hfHDR = CreateFile( m_strHDR, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );
	if( INVALID_HANDLE_VALUE == m_hfHDR )
	{
		errorMsg( "Couldn't open header file <%s>\n", m_strHDR );
		return E_FAIL;
	}

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: CloseFiles()
// Desc: Closes file handles for all our input/output files
//-----------------------------------------------------------------------------
HRESULT XPRBundler::closeFiles( )
{
	if( INVALID_HANDLE_VALUE != m_hfXPR )
		CloseHandle( m_hfXPR );
	if( INVALID_HANDLE_VALUE != m_hfHDR )
		CloseHandle( m_hfHDR );
	if( INVALID_HANDLE_VALUE != m_hfERR )
		CloseHandle( m_hfERR );

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: FlushDataFile()
// Desc: Writes out all the buffered data to the data file, including
//       appropriate padding
//-----------------------------------------------------------------------------
HRESULT XPRBundler::flushDataFile( )
{
	bool					bWriteOK;
	DWORD					cb;
	XPR_HEADER			xprh;
	BYTE *				pbTemp;
	DWORD					cbFill = 0;

	// Pad the data buffer out to be a multiple of the DVD sector size (2k).
	// This is needed in order to DMA the file off the DVD drive on the
	// Xbox.
	padToAlignment( 2048 );

	// Choose the magic value to identify the file. XPR0 is the default (version 0)
	// format, where XPR1 (version1) embeds an offsets table.
	switch( m_dwOutputFileVersion )
	{
		case VERSION_XPR0:
			xprh.dwMagic = XPR0_MAGIC_VALUE;
			break;
		case VERSION_XPR1:
			xprh.dwMagic = XPR1_MAGIC_VALUE;
			break;
	}

	// Compute size of resource table
	m_dwOffsetsTableSize	 = 0L;
	m_dwOffsetsTablePadBytes = 0L;
	m_dwBaseResourceOffset   = 0L;

	if( m_dwOutputFileVersion >= VERSION_XPR1 )
	{
		// Add offsets table
		m_dwOffsetsTableSize = 4 + 8*m_dwNumResources + 4;

		// Add string space
		for( DWORD i = 0; i < m_dwNumResources; i++ )
			m_dwOffsetsTableSize += strlen(m_pResources[i].strName)+1;

		// Pad to 16 bytes
		m_dwOffsetsTablePadBytes = (16-(m_dwOffsetsTableSize%16))%16;
		m_dwOffsetsTableSize += m_dwOffsetsTablePadBytes;

		// The base resource offset just skips past the size of the table
		m_dwBaseResourceOffset = m_dwOffsetsTableSize;
	}

	// Calculate header size
	xprh.dwHeaderSize = sizeof(XPR_HEADER) + m_dwOffsetsTableSize + m_cbHeader;

	// We may need to pad the file to a sector multiple for the 
	// start of the data block
	if( xprh.dwHeaderSize % 2048 )
	{
		cbFill = 2048 - ( xprh.dwHeaderSize % 2048 );
		xprh.dwHeaderSize += cbFill;
	}
	xprh.dwTotalSize = xprh.dwHeaderSize + m_cbData;

	// Write out header stuff
	if( !WriteFile( m_hfXPR, &xprh, sizeof(XPR_HEADER), &cb, NULL ) )
		return E_FAIL;

	// Write out the resource offsets table
	if( m_dwOutputFileVersion >= VERSION_XPR1 )
	{
		// Write out the number of resources
		DWORD dwNumResources = m_dwNumResources;
		if( !WriteFile( m_hfXPR, &m_dwNumResources, sizeof(DWORD), &cb, NULL ) )
			return E_FAIL;

		// Write out the offsets of each resource
		DWORD dwStringBase = m_dwNumResources*(sizeof(CHAR*)+sizeof(DWORD)) + sizeof(DWORD) + sizeof(CHAR*);

		for( DWORD i = 0; i < m_dwNumResources; i++ )
		{
			// Write offset to string
			if( !WriteFile( m_hfXPR, &dwStringBase, sizeof(CHAR*), &cb, NULL ) )
				return E_FAIL;
			dwStringBase += strlen(m_pResources[i].strName)+1;

			// Write offset
			DWORD dwOffset = m_pResources[i].dwOffset + m_dwBaseResourceOffset;
			if( !WriteFile( m_hfXPR, &dwOffset, sizeof(DWORD), &cb, NULL ) )
				return E_FAIL;
		}

		// Write a NULL terminator
		DWORD dwNULL = 0L;
		if( !WriteFile( m_hfXPR, &dwNULL, sizeof(DWORD), &cb, NULL ) )
			return E_FAIL;

		// Write the string table
		for( i = 0; i < m_dwNumResources; i++ )
		{
			if( !WriteFile( m_hfXPR, m_pResources[i].strName, strlen(m_pResources[i].strName)+1, &cb, NULL ) )
				return E_FAIL;
		}

		// Add padding
		if( m_dwOffsetsTablePadBytes )
		{
			CHAR pad[16] = {0};
			if( !WriteFile( m_hfXPR, pad, m_dwOffsetsTablePadBytes, &cb, NULL ) )
				return E_FAIL;
		}
	}

	// Write out the header
	if( !WriteFile( m_hfXPR, m_pbHeaderBuff, m_cbHeader, &cb, NULL ) )
		return E_FAIL;

	if( cbFill )
	{
		// Allocate a buffer for fill data
		pbTemp = (BYTE *)malloc( cbFill );
		memset( pbTemp, 0xDEAD, cbFill );

		// Write it out
		bWriteOK = WriteFile( m_hfXPR, pbTemp, cbFill, &cb, NULL ) != 0;
		free( pbTemp );
		if( !bWriteOK )
			return E_FAIL;
	}

	if( !WriteFile( m_hfXPR, m_pbDataBuff, m_cbData, &cb, NULL ) )
		return E_FAIL;

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: WriteHeaderFile()
// Desc: Writes out a .h file with useful defitions:
//       * NUM_RESOURCES
//       * XXX_OFFSET for each resource
//-----------------------------------------------------------------------------
HRESULT XPRBundler::writeHeaderFile( )
{
	DWORD					i;
	DWORD					cb;
	char					strBuff[256];

	// strip root filename out of m_strRDF;
	CHAR strPrefix[MAX_PATH];

	if( m_strPrefix[0] )
	{
		strcpy( strPrefix, m_strPrefix );
	}
	else
	{
		// Use the resource name as a prefix, after removing prepending path
		// and file extension.
		CHAR* pStr = strrchr( m_strXPR, '\\' );
		if( pStr )
			strcpy( strPrefix, pStr + 1 );
		else
			strcpy( strPrefix, m_strXPR );

		pStr = strchr( strPrefix, '.' );
		if( pStr )
			*pStr = '\0';
	}

	// Write out the number of resources
	sprintf( strBuff, "#define %s_NUM_RESOURCES %dUL\n\n", strPrefix, m_dwNumResources );
	if( !WriteFile( m_hfHDR, strBuff, strlen(strBuff), &cb, NULL ) )
		return E_FAIL;

	for( i = 0; i < m_dwNumResources; i++ )
	{
		sprintf( strBuff, "#define %s_%s_OFFSET %dUL\n", strPrefix, 
			  m_pResources[i].strIdentifier, m_pResources[i].dwOffset + m_dwBaseResourceOffset );

		if( !WriteFile( m_hfHDR, strBuff, strlen(strBuff), &cb, NULL ) )
			return E_FAIL;
	}

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: HandleEOFToken()
// Desc: Handles any necessary end-of-input processing, such as patching up
//	   header information
//-----------------------------------------------------------------------------
HRESULT XPRBundler::finalizeBundle( void )
{
	if( m_dwNumResources == 0 )
		return S_OK;

	if( FAILED( openFiles( ) ) )
		return E_FAIL;

	// write one last header entry to terminate the list
	DWORD endHeader = 0xffffffff;
	writeHeader( &endHeader, sizeof( endHeader ) );
	m_cbHeader += sizeof( endHeader );

	if( FAILED( flushDataFile( ) ) )
		return E_FAIL;

	if( FAILED( writeHeaderFile( ) ) )
		return E_FAIL;

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: WriteData()
// Desc: Copies data to the data buffer.  Grows our data buffer by multiples
//       of 2
//-----------------------------------------------------------------------------
HRESULT XPRBundler::writeData( void * pbBuff, DWORD cb )
{
	DWORD cbNew = m_cbDataAlloc;

	// Find the next power-of-2 size that is  big enough to hold the data
	while( m_cbData + cb > cbNew )
		cbNew <<= 1;
	
	// Realloc if necessary
	if( cbNew > m_cbDataAlloc )
	{
		BYTE * pbTemp = (BYTE *)realloc( m_pbDataBuff, cbNew );
		if( !pbTemp )
			return E_OUTOFMEMORY;
		m_pbDataBuff = pbTemp;
		m_cbDataAlloc = cbNew;
	}

	// Copy over the data
	memcpy( m_pbDataBuff + m_cbData, pbBuff, cb );
	m_cbData += cb;

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: WriteHeader()
// Desc: Copies resource header info to the header buffer.  Grows the header
//       buffer by multiples of 2.
//-----------------------------------------------------------------------------
HRESULT XPRBundler::writeHeader( void * pbBuff, DWORD cb )
{
	DWORD cbNew = m_cbHeaderAlloc;

	// Find the next power-of-2 size that is big enough to hold the data
	while( m_cbHeader + cb > cbNew )
		cbNew <<= 1;

	// Realloc if necessary
	if( cbNew > m_cbHeaderAlloc )
	{
		BYTE * pbTemp = (BYTE *)realloc( m_pbHeaderBuff, cbNew );
		if( !pbTemp )
			return E_OUTOFMEMORY;
        
		m_pbHeaderBuff = pbTemp;
        
		m_cbHeaderAlloc = cbNew;
	}

	// Copy over the data
	memcpy( m_pbHeaderBuff + m_cbHeader, pbBuff, cb );

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: PadToAlignment()
// Desc: Pads the data buffer to the specified alignment
//-----------------------------------------------------------------------------
HRESULT XPRBundler::padToAlignment( DWORD dwAlign )
{
	HRESULT hr = S_OK;
	BYTE*   pbTemp = NULL;
	DWORD   cbFill = 0;

	if( m_cbData % dwAlign )
		cbFill = dwAlign - ( m_cbData % dwAlign );

	if( cbFill )
	{
		pbTemp = (BYTE *)malloc( cbFill );
		memset( pbTemp, 0xDEAD, cbFill );

		hr = writeData( pbTemp, cbFill );
		
		free( pbTemp );
	}

	return hr;
}

