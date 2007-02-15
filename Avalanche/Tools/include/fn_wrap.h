/* --------------------------------------------------
 * File    : FN_WRAP.H
 * Created : Mon Sep 21 11:37:24 1998
 * Descript: 
 * --------------------------------------------------*/
#ifndef __FN_WRAP_H
#define __FN_WRAP_H

/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/
#include <malloc.h>
#include <t_types.h>

/* --------------------------------------------------
 * Defines
 * --------------------------------------------------*/
enum
{
	FNW_NONE = 0,
	FNW_PATH,
	FNW_NAME,
	FNW_EXT
} ;

/* --------------------------------------------------
 * Exported Data
 * --------------------------------------------------*/
// Flag bits
#define FNW_HAS_PATH			0x00000001
#define FNW_HAS_NAME			0x00000002
#define FNW_HAS_EXT			0x00000004

// Flag bits
#define FNW_GET_PATH			0x00000001
#define FNW_GET_NAME			0x00000002
#define FNW_GET_EXT			0x00000004
#define FNW_GET_ALL			0x00000007

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/

class FNWrapper
{
	t_lflagbits		Flags;
	
	int				iAllocBytes;
	
	char				*pNamePtr;
	char				*pExtPtr;

	char				*pFileEnd;
	char				*pBufferEnd;

	char				*pFName;
	
	void				SetFromString( char *pcStr);
	void				FreeBuffer( void);
	void				NewBuffer( char *pNewBuffer);

	void				Setup( void);
	void				SetEnds( void);
	void				CopyToBuffer( char *pNewBuffer, char *pNewStr, int iReplace);

public:
	FNWrapper( );
	FNWrapper( const char *pcName);
	~FNWrapper( ) { if( pFName) free( pFName); }

	void				Break( char **ppPath, char **ppName, char **ppExt, t_lflagbits GetFlags);
	void				Debug( void);
	void				Release( void);
	char				*Ext( void) { return pExtPtr; }
	char				*FileName( void) { return pNamePtr; }
	void				ChangeExt( const char *pcExt);
	void				ChangeDrv( char cDrive);
	void				ChangePath( const char *pcPath);
	void				ChangeName( const char *pcName); 

	int				ExtLen( void);
	int				NameLen( void);
	int				PathLen( void);

	void operator			= ( char *pStr)			{ FreeBuffer( ); SetFromString( pStr); }
	void operator			= ( const char *pStr)	{ FreeBuffer( ); SetFromString( (char *) pStr); }
	void operator			= ( FNWrapper &FN)		{ FreeBuffer( ); SetFromString( (char *) FN); }

	operator const char*	( ) { return (char *) pFName; }
	operator char *		( ) { return pFName; }
} ;


void ConvSlash( char *pcName);
void AddSlash( char *pcName);

#endif __FN_WRAP_H