#ifndef __XPR_BUNDLER_H
#define __XPR_BUNDLER_H

#include <assert.h>
#include <windows.h>

// Bundler output file versions
enum
{
    VERSION_XPR0 = 0, // Version 0 = Default version
    VERSION_XPR1 = 1, // Version 1 = Outputs resource offsets table
};

#define XPR0_MAGIC_VALUE	0x30525058 // 'XPR0'
#define XPR1_MAGIC_VALUE	0x31525058 // 'XPR1'


// Default extensions for output files
#define XPREXT					".xpr"
#define HDREXT					".h"
#define ERREXT					".err"

#define MAX_RESOURCES		4096


// Resource struct for tracking
struct RESOURCE_STRUCT
{
	char					strIdentifier[128];
	char					strName[128];

	DWORD					dwOffset;
};




class XPRBundler
{
public:
    // Filenames
	char					m_strPath[MAX_PATH];

	char					m_strXPR[MAX_PATH];
	char *				m_extXPR;
	char					m_strHDR[MAX_PATH];
	char *				m_extHDR;
	char					m_strERR[MAX_PATH];

	// File handles
	HANDLE				m_hfXPR;
	HANDLE				m_hfHDR;
	HANDLE				m_hfERR;

    // Prefix for header file constants
	char					m_strPrefix[MAX_PATH];

	// Bundler output file version
	DWORD					m_dwOutputFileVersion;

	DWORD					m_cbHeader;
	DWORD					m_cbData;

	// Data buffer management
	BYTE *				m_pbHeaderBuff;
	DWORD					m_cbHeaderAlloc;
	BYTE *				m_pbDataBuff;
	DWORD					m_cbDataAlloc;

	// Values for the resource offsets table
	DWORD					m_dwOffsetsTableSize;
	DWORD					m_dwOffsetsTablePadBytes;
	DWORD					m_dwBaseResourceOffset;

	// Resource tracking
	RESOURCE_STRUCT * m_pResources;
	DWORD					m_dwNumResources;

	XPRBundler( );
	~XPRBundler( );

	void					setXPRExt( const char * pXPRExt );
	void					setHDRExt( const char * pHDRExt );

	HRESULT				startBundle( const char * strFile );
	HRESULT				finalizeBundle( void );

	HRESULT				padToAlignment( DWORD dwAlign );

	HRESULT				writeHeader( void * pbBuff, DWORD cb );
	HRESULT				writeData( void * pbBuff, DWORD cb );

	void					addResourceItem( const char * strIdentifier, const char * strName, DWORD dwHdrSize, DWORD dwDataSize );

	// Routine to call w/ error messages - prints to stdout and file
	// Prepends "Bundler : error : " so that Visual Studio will recognize the
	// error message. Only use for fatal errors.
	void					errorMsg( const char * strFmt, ... );
	// Ditto for warnings - prepends "Bundler : warning :". Only use for
	// genuine warnings.
	void					warningMsg( const char * strFmt, ... );
	// No prefix. Just print the specified text.
	void					standardMsg( const char * strFmt, ... );


private:
	HRESULT				openFiles( );            // Open output files
	HRESULT				closeFiles( );           // Close all files
	HRESULT				flushDataFile( );        // Flush from buffer to output file
	HRESULT				writeHeaderFile( );      // Create .h header file
	HRESULT				setBaseFilename( const char * strFile );
    // Support for function for ErrorMsg, WarningMsg and Msg
	void					internalPrintMessage( const char* prefix, const char* strFmt, va_list args);
};

#endif // __XPR_BUNDLER_H
