/* --------------------------------------------------
 * File    : ANM_PROC.C
 * Created : Fri Apr  9 16:43:35 1999
 * Descript:
 * --------------------------------------------------*/
/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <windows.h>
#include <conio.h>
#include <direct.h>

#include "anm_gram.h"
#include "animrip.h"

/* --------------------------------------------------
 * Local Defines
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Local Prototypes
 * --------------------------------------------------*/
extern int yyparse(void);
extern int zz_reset(void);
void *zz_get_current_buffer(void);
void zz_switch_to_buffer(void *);
void zz_delete_buffer(void *);
void zzrestart(FILE *);
ts_Hdr *FindDataByLineNumber( int lineNumber );
ts_Symbol *FindVariableNameByIndex( int index );
ts_Symbol *FindFunctionNameByIndex( int index );
ts_Symbol *FindStringByIndex( int index );
ts_Symbol *FindSymbolByAddress( int address );
ts_Symbol *FindSymbolByLine( int nLine );
int FindStringIndex( char *strValue );

static ts_Symbol *FindSymbolByName( char* symbolName );
static int CreateVariableIndex( char *varName, int tType );
static int FindVariableIndex( char* varName );
static int CreateFunctionIndex( char *varName );
static int FindFunctionIndex( char* varName );
static int CreateStringIndex( char *strValue );
static char *ExpandString( char *dst, char *src );
static void doUnaryReduction( void );
static void doBinaryReduction( void );
static void unPushLast( void );
static void removeLastData( void );
extern Hash__CalcChecksum( const void* buf, int length, unsigned int initVal );

extern FILE *zzin;

/* --------------------------------------------------
 * Local Data
 * --------------------------------------------------*/
BOOL    bVerbose,
		    bBigEndian,
				bPauseWhenDone,
				bPCOutput,
        bCPP,
        bDebug,
        bListing;

int     nGlobalOffset = 0;


ts_Hdr  DataList,
        *pDataList;

ts_BackPatch  PatchList,
              *pPatchList;

ts_Symbol SymbolList,
          *pSymbolList;

ts_Symbol FunctionList,
          *pFunctionList;

ts_Symbol VarList,
          *pVarList;

ts_Symbol StringList,
          *pStringList;

FILE  *fInfile,
      *fOutfile;

unsigned long *pOutputArray;

char *pOpStackSize = NULL;
char *pCallStackSize = NULL;

char **debugOffsetTable = NULL;

char *debugText = NULL;

extern int iLineCount;
extern char szFileName[];
extern int unprocessedFileLine;
extern char unprocessedFileName[];

#define CHECK_ERROR( a ) if ( a != TRUE ) ExitWithError()
#define ADD_DEBUG if ( bDebug ) { \
    nReturnVal = AddInt( "DEBUG", LIST_INTEGER, iLineCount - 1 ); \
    CHECK_ERROR( nReturnVal ); \
    nGlobalOffset += INT_LEN; }

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/

/* --------------------------------------------------
 * ProcessCppLine
 * --------------------------------------------------*/
int ProcessCppLine( int line, char* file )
{
	unprocessedFileLine = line - 1;		// -1 because the yacc will increment at the CR

	// make the filename, put the cwd on if there is no path in the file
	unprocessedFileName[ 0 ] = 0;
	if ( strchr( file, '/' ) == NULL && strchr( file, '\\' ) == NULL )
	{
		_getcwd( unprocessedFileName, 256 );
		strcat( unprocessedFileName, "\\" );
	}
	strcat( unprocessedFileName, file );

	return TRUE;
}

/* --------------------------------------------------
 * ProcessPushF
 * --------------------------------------------------*/
int ProcessPushF( float fVal )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to PUSH float\n");
	}

  nReturnVal = AddInt( "PUSHF", LIST_OPCODE, CMD_PUSHF );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nReturnVal = AddFloat( "PUSHF", fVal );
  CHECK_ERROR( nReturnVal );

  nGlobalOffset += OPCODE_LEN + FLOAT_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessPushI
 * --------------------------------------------------*/
int ProcessPushI( int iVal )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to PUSH int\n");
	}

  nReturnVal = AddInt( "PUSHI", LIST_OPCODE, CMD_PUSHI );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nReturnVal = AddInt( "PUSHI", LIST_INTEGER, iVal );
  CHECK_ERROR( nReturnVal );

  nGlobalOffset += OPCODE_LEN + INT_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessPushA
 * --------------------------------------------------*/
int ProcessPushA( char* pcString )
{
  int nReturnVal;
  char *temp = pcString;

  if (bVerbose == TRUE)
	{
    printf("Got to PUSH address\n");
	}

  if ( *pcString == '%' )
  { /* check for a reference value */
    temp++;
  }

  nReturnVal = AddInt( "PUSHA", LIST_OPCODE, CMD_PUSHI );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nReturnVal = AddAddress( "PUSHA", temp );
  CHECK_ERROR( nReturnVal );

  nGlobalOffset += OPCODE_LEN + ADDRESS_LEN;
	free(pcString);

  return TRUE;
}

/* --------------------------------------------------
 * ProcessPushS
 * --------------------------------------------------*/
int ProcessPushS( char* pcString )
{
  int index;
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to PUSH string\n");
	}

  /* Lookup pcString index */
  index = CreateStringIndex( pcString );

  /* build pushs <index> instruction */
  nReturnVal = AddInt( "PUSHS", LIST_OPCODE, CMD_PUSHS );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nReturnVal = AddInt( "PUSHS", LIST_INTEGER, index );
  CHECK_ERROR( nReturnVal );

  nGlobalOffset += OPCODE_LEN + STRING_LEN;

  free( pcString );

  return TRUE;
}

/* --------------------------------------------------
 * ProcessPop
 * --------------------------------------------------*/
int ProcessPop( void )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to POP\n");
	}

  nReturnVal = AddInt( "POP", LIST_OPCODE, CMD_POP );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessSwap
 * --------------------------------------------------*/
int ProcessSwap( void )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to SWAP\n");
	}

  nReturnVal = AddInt( "SWAP", LIST_OPCODE, CMD_SWAP );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessDup
 * --------------------------------------------------*/
int ProcessDup( void )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to DUP\n");
	}

  nReturnVal = AddInt( "DUP", LIST_OPCODE, CMD_DUP );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessLoad
 * --------------------------------------------------*/
int ProcessLoad( char* varName )
{
  int index;
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to LOAD TEXT_\n");
	}

  /* Lookup varName index */
  index = FindVariableIndex( varName );
  if ( index < 0 )
  {
		fprintf( stderr, "%s(%d) : Error on LOAD: Undeclared identifier: %s\n", unprocessedFileName, unprocessedFileLine, varName );
		ExitWithError( );
  }

  /* build load <index> instruction */
  nReturnVal = AddInt( "LOAD", LIST_OPCODE, CMD_LOAD );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nReturnVal = AddInt( "LOAD", LIST_INTEGER, index );
  CHECK_ERROR( nReturnVal );

  nGlobalOffset += OPCODE_LEN + INT_LEN;

//  free( varName );

  return TRUE;
}

/* --------------------------------------------------
 * ProcessStore
 * --------------------------------------------------*/
int ProcessStore( char* varName )
{
  int index;
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to STORE TEXT_\n");
	}

  /* Lookup varName index */
  index = FindVariableIndex( varName );
  if ( index < 0 )
  {
		fprintf( stderr, "%s(%d) : Error on STORE: Undeclared identifier: %s\n", unprocessedFileName, unprocessedFileLine, varName );
		ExitWithError( );
  }
  
  /* build load <index> instruction */
  nReturnVal = AddInt( "STORE", LIST_OPCODE, CMD_STORE );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nReturnVal = AddInt( "STORE", LIST_INTEGER, index );
  CHECK_ERROR( nReturnVal );

  nGlobalOffset += OPCODE_LEN + INT_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessStore
 * --------------------------------------------------*/
int ProcessStoreVar( char* dstName, char* srcName )
{
  int nReturnVal;

  if ( bVerbose == TRUE )
  {
    printf(" Got to STORE dst src\n");
  }

  nReturnVal = ProcessLoad( srcName );
  CHECK_ERROR( nReturnVal );

  nReturnVal = ProcessStore( dstName );
  CHECK_ERROR( nReturnVal );

  return TRUE;

}

/* --------------------------------------------------
 * ProcessStore
 * --------------------------------------------------*/
int ProcessStoreF( char* dstName, float fVal )
{
  int nReturnVal;

  if ( bVerbose == TRUE )
  {
    printf(" Got to STORE dst <floatLiteral>\n");
  }

  nReturnVal = ProcessPushF( fVal );
  CHECK_ERROR( nReturnVal );

  nReturnVal = ProcessStore( dstName );
  CHECK_ERROR( nReturnVal );

  return TRUE;
}

/* --------------------------------------------------
 * ProcessStore
 * --------------------------------------------------*/
int ProcessStoreI( char* dstName, int iVal )
{
  int nReturnVal;

  if ( bVerbose == TRUE )
  {
    printf(" Got to STORE dst <intLiteral>\n");
  }

  nReturnVal = ProcessPushI( iVal );
  CHECK_ERROR( nReturnVal );

  nReturnVal = ProcessStore( dstName );
  CHECK_ERROR( nReturnVal );

  return TRUE;
}

/* --------------------------------------------------
 * ProcessAdd
 * --------------------------------------------------*/
int ProcessAdd( void )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to ADD\n");
	}

  nReturnVal = AddInt( "ADD", LIST_OPCODE, CMD_ADD );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  doBinaryReduction( );

  nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessSub
 * --------------------------------------------------*/
int ProcessSub( void )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to SUB\n");
	}

  nReturnVal = AddInt( "SUB", LIST_OPCODE, CMD_SUB );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  doBinaryReduction( );

  nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessMul
 * --------------------------------------------------*/
int ProcessMul( void )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to MUL\n");
	}

  nReturnVal = AddInt( "MUL", LIST_OPCODE, CMD_MUL );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  doBinaryReduction( );

  nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessDiv
 * --------------------------------------------------*/
int ProcessDiv( void )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to DIV\n");
	}

  nReturnVal = AddInt( "DIV", LIST_OPCODE, CMD_DIV );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  doBinaryReduction( );

  nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessMod
 * --------------------------------------------------*/
int ProcessMod( void )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to MOD\n");
	}

  nReturnVal = AddInt( "MOD", LIST_OPCODE, CMD_MOD );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  doBinaryReduction( );

  nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessBitAnd
 * --------------------------------------------------*/
int ProcessBitAnd( void )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to BIT_AND\n");
	}

  nReturnVal = AddInt( "BIT_AND", LIST_OPCODE, CMD_BIT_AND );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  doBinaryReduction( );

  nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessBitOr
 * --------------------------------------------------*/
int ProcessBitOr( void )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to BIT_OR\n");
	}

  nReturnVal = AddInt( "BIT_OR", LIST_OPCODE, CMD_BIT_OR );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  doBinaryReduction( );

  nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessBitXor
 * --------------------------------------------------*/
int ProcessBitXor( void )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to BIT_XOR\n");
	}

  nReturnVal = AddInt( "BIT_XOR", LIST_OPCODE, CMD_BIT_XOR );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  doBinaryReduction( );

  nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessBitNot
 * --------------------------------------------------*/
int ProcessBitNot( void )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to BIT_NOT\n");
	}

  nReturnVal = AddInt( "BIT_NOT", LIST_OPCODE, CMD_BIT_NOT );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  doUnaryReduction( );

  nGlobalOffset += OPCODE_LEN;

  return TRUE;
}
/* --------------------------------------------------
 * ProcessAnd
 * --------------------------------------------------*/
int ProcessAnd( void )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to AND\n");
	}

  nReturnVal = AddInt( "AND", LIST_OPCODE, CMD_AND );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  doBinaryReduction( );

  nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessOr
 * --------------------------------------------------*/
int ProcessOr( void )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to BIT_OR\n");
	}

  nReturnVal = AddInt( "BIT_OR", LIST_OPCODE, CMD_OR );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  doBinaryReduction( );

  nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessNot
 * --------------------------------------------------*/
int ProcessNot( void )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to BIT_NOT\n");
	}

  nReturnVal = AddInt( "BIT_NOT", LIST_OPCODE, CMD_NOT );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  doUnaryReduction( );

  nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessShiftLeft
 * --------------------------------------------------*/
int ProcessShiftLeft( void )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to SHL\n");
	}

  nReturnVal = AddInt( "SHL", LIST_OPCODE, CMD_SHL );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  doBinaryReduction( );

  nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessShiftRight
 * --------------------------------------------------*/
int ProcessShiftRight( void )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to SHR\n");
	}

  nReturnVal = AddInt( "SHR", LIST_OPCODE, CMD_SHR );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  doBinaryReduction( );

  nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessShiftRigthUnsigned
 * --------------------------------------------------*/
int ProcessShiftRightUnsigned( void )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to SHRZ\n");
	}

  nReturnVal = AddInt( "SHRZ", LIST_OPCODE, CMD_SHRZ );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  doBinaryReduction( );

  nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessInt
 * --------------------------------------------------*/
int ProcessInt( void )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to INT\n");
	}

  nReturnVal = AddInt( "INT", LIST_OPCODE, CMD_INT );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  doUnaryReduction( );

  nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessFloat
 * --------------------------------------------------*/
int ProcessFloat( void )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to FLOAT\n");
	}

  nReturnVal = AddInt( "FLOAT", LIST_OPCODE, CMD_FLOAT );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  doUnaryReduction( );

  nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessCMP
 * --------------------------------------------------*/
int ProcessCMP( void )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to CMP\n");
	}

  nReturnVal = AddInt( "CMP", LIST_OPCODE, CMD_CMP );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessJNET
 * --------------------------------------------------*/
int ProcessJNET( char* jumpAddr )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to JNE TEXT_\n");
	}

	nReturnVal = ProcessPushA( jumpAddr );
  CHECK_ERROR( nReturnVal );

  nReturnVal = AddInt( "JNE TEXT_", LIST_OPCODE, CMD_JNE );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessJNE
 * --------------------------------------------------*/
int ProcessJNE( void )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to JNE\n");
	}

  nReturnVal = AddInt( "JNE", LIST_OPCODE, CMD_JNE );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessJET
 * --------------------------------------------------*/
int ProcessJET( char* jumpAddr )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to JE TEXT_\n");
	}

	nReturnVal = ProcessPushA( jumpAddr );
  CHECK_ERROR( nReturnVal );

  nReturnVal = AddInt( "JE TEXT_", LIST_OPCODE, CMD_JE );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessJE
 * --------------------------------------------------*/
int ProcessJE( void )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to JE\n");
	}

  nReturnVal = AddInt( "JE", LIST_OPCODE, CMD_JE );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessJLT
 * --------------------------------------------------*/
int ProcessJLT( char* jumpAddr )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to JL TEXT_\n");
	}

	nReturnVal = ProcessPushA( jumpAddr );
  CHECK_ERROR( nReturnVal );

  nReturnVal = AddInt( "JL TEXT_", LIST_OPCODE, CMD_JL );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessJL
 * --------------------------------------------------*/
int ProcessJL( void )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to JL\n");
	}

  nReturnVal = AddInt( "JL", LIST_OPCODE, CMD_JL );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessJGT
 * --------------------------------------------------*/
int ProcessJGT( char* jumpAddr )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to JG TEXT_\n");
	}

	nReturnVal = ProcessPushA( jumpAddr );
  CHECK_ERROR( nReturnVal );

  nReturnVal = AddInt( "JG TEXT_", LIST_OPCODE, CMD_JG );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessJG
 * --------------------------------------------------*/
int ProcessJG( void )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to JG\n");
	}

  nReturnVal = AddInt( "JG", LIST_OPCODE, CMD_JG );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessJLET
 * --------------------------------------------------*/
int ProcessJLET( char* jumpAddr )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to JLE TEXT_\n");
	}

	nReturnVal = ProcessPushA( jumpAddr );
  CHECK_ERROR( nReturnVal );

  nReturnVal = AddInt( "JLE TEXT_", LIST_OPCODE, CMD_JLE );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessJLE
 * --------------------------------------------------*/
int ProcessJLE( void )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to JLE\n");
	}

  nReturnVal = AddInt( "JLE", LIST_OPCODE, CMD_JLE );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessJGET
 * --------------------------------------------------*/
int ProcessJGET( char* jumpAddr )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to JGE TEXT_\n");
	}

	nReturnVal = ProcessPushA( jumpAddr );
  CHECK_ERROR( nReturnVal );

  nReturnVal = AddInt( "JGE TEXT_", LIST_OPCODE, CMD_JGE );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessJGE
 * --------------------------------------------------*/
int ProcessJGE( void )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to JGE\n");
	}

  nReturnVal = AddInt( "JGE", LIST_OPCODE, CMD_JGE );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessCallC
 * --------------------------------------------------*/
int ProcessCallC( char* funcName )
{
  int index;
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to CALL TEXT_\n");
	}

  /* attempt to lookup the function */
  index = FindFunctionIndex( funcName );

  if ( index < 0 )
  { /* didn't find the function - add a new one and use it */
    nReturnVal = AddFunction( "CALL", funcName, Hash__CalcChecksum( funcName, strlen(funcName), 0 ) );
    CHECK_ERROR( nReturnVal );

    index = FindFunctionIndex( funcName );
    if ( index < 0 )
    {
      ExitWithError();
    }
  }

  nReturnVal = AddInt( "CALLC", LIST_OPCODE, CMD_CALLC );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nReturnVal = AddInt( "CALLC", LIST_INTEGER, index );
  CHECK_ERROR( nReturnVal );

  nGlobalOffset += OPCODE_LEN + INT_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessCallT
 * --------------------------------------------------*/
int ProcessCallT( char* pcString )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to CALL TEXT_\n");
	}

	nReturnVal = ProcessPushA( pcString );
  CHECK_ERROR( nReturnVal );

  nReturnVal = AddInt( "CALL TEXT_", LIST_OPCODE, CMD_CALL );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessCall
 * --------------------------------------------------*/
int ProcessCall( void )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to CALL\n");
	}

  nReturnVal = AddInt( "CALL", LIST_OPCODE, CMD_CALL );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessReturn
 * --------------------------------------------------*/
int ProcessReturn( void )
{
	int nReturnVal;
  
  nReturnVal = AddInt( "RETURN", LIST_OPCODE, CMD_RETURN );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessBNET
 * --------------------------------------------------*/
int ProcessBNET( char* jumpAddr )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to BNE TEXT_\n");
	}

	nReturnVal = ProcessPushA( jumpAddr );
  CHECK_ERROR( nReturnVal );

  nReturnVal = AddInt( "BNE TEXT_", LIST_OPCODE, CMD_BNE );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessBNE
 * --------------------------------------------------*/
int ProcessBNE( void )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to BNE\n");
	}

  nReturnVal = AddInt( "BNE", LIST_OPCODE, CMD_BNE );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessBET
 * --------------------------------------------------*/
int ProcessBET( char* jumpAddr )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to BE TEXT_\n");
	}

	nReturnVal = ProcessPushA( jumpAddr );
  CHECK_ERROR( nReturnVal );

  nReturnVal = AddInt( "BE TEXT_", LIST_OPCODE, CMD_BE );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessBE
 * --------------------------------------------------*/
int ProcessBE( void )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to BE\n");
	}

  nReturnVal = AddInt( "BE", LIST_OPCODE, CMD_BE );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessBLT
 * --------------------------------------------------*/
int ProcessBLT( char* jumpAddr )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to BL TEXT_\n");
	}

	nReturnVal = ProcessPushA( jumpAddr );
  CHECK_ERROR( nReturnVal );

  nReturnVal = AddInt( "BL TEXT_", LIST_OPCODE, CMD_BL );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessBL
 * --------------------------------------------------*/
int ProcessBL( void )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to BL\n");
	}

  nReturnVal = AddInt( "BL", LIST_OPCODE, CMD_BL );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessBGT
 * --------------------------------------------------*/
int ProcessBGT( char* jumpAddr )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to BG TEXT_\n");
	}

	nReturnVal = ProcessPushA( jumpAddr );
  CHECK_ERROR( nReturnVal );

  nReturnVal = AddInt( "BG TEXT_", LIST_OPCODE, CMD_BG );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessBG
 * --------------------------------------------------*/
int ProcessBG( void )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to BG\n");
	}

  nReturnVal = AddInt( "BG", LIST_OPCODE, CMD_BG );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessBLET
 * --------------------------------------------------*/
int ProcessBLET( char* jumpAddr )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to BLE TEXT_\n");
	}

	nReturnVal = ProcessPushA( jumpAddr );
  CHECK_ERROR( nReturnVal );

  nReturnVal = AddInt( "BLE TEXT_", LIST_OPCODE, CMD_BLE );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessBLE
 * --------------------------------------------------*/
int ProcessBLE( void )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to BLE\n");
	}

  nReturnVal = AddInt( "BLE", LIST_OPCODE, CMD_BLE );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessBGET
 * --------------------------------------------------*/
int ProcessBGET( char* jumpAddr )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to BGE TEXT_\n");
	}

	nReturnVal = ProcessPushA( jumpAddr );
  CHECK_ERROR( nReturnVal );

  nReturnVal = AddInt( "BGE TEXT_", LIST_OPCODE, CMD_BGE );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessBGE
 * --------------------------------------------------*/
int ProcessBGE( void )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to BGE\n");
	}

  nReturnVal = AddInt( "BGE", LIST_OPCODE, CMD_BGE );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessPushReg
 * --------------------------------------------------*/
int ProcessPushReg( void )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to PUSHREG\n");
	}

  nReturnVal = AddInt( "PUSHREG", LIST_OPCODE, CMD_PUSHREG );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessPopReg
 * --------------------------------------------------*/
int ProcessPopReg( void )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to POPREG\n");
	}

  nReturnVal = AddInt( "POPREG", LIST_OPCODE, CMD_POPREG );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessAIScript-- we have an opcode that means "this is an ai script" that doesn't add any data to the compiled script
 * --------------------------------------------------*/
int ProcessAIScript( void )
{
	nExportType = EXPORT_AI_SCRIPT;
	return TRUE;
}

/* --------------------------------------------------
 * ProcessIntDecl
 * --------------------------------------------------*/
int ProcessIntDecl( char *varName, int published )
{
  int index = CreateVariableIndex( varName, INT_TYPE );
  ts_Symbol *var = NULL;

  if ( index < 0 )
  {
    ExitWithError();
  }

  var = FindVariableNameByIndex( index );
  var->bInitialized = 0;
  var->iInitialValue = 0;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessIntDeclInitial
 * --------------------------------------------------*/
int ProcessIntDeclInitial( char *varName, int initialValue, int published )
{
  int index = CreateVariableIndex( varName, INT_TYPE );
  ts_Symbol *var = NULL;

  if ( index < 0 )
  {
    ExitWithError();
  }

  var = FindVariableNameByIndex( index );
  var->bInitialized = 1;
  var->iInitialValue = initialValue;

  // remove the data that was pushed by processing any expressions
  // and/or simple literal expressions
  unPushLast( );

  return TRUE;
}

/* --------------------------------------------------
 * ProcessFloatDecl
 * --------------------------------------------------*/
int ProcessFloatDecl( char *varName, int published )
{
  int index = CreateVariableIndex( varName, FLOAT_TYPE );
  ts_Symbol *var = NULL;

  if ( index < 0 )
  {
    ExitWithError();
  }

  var = FindVariableNameByIndex( index );
  var->bInitialized = 0;
  var->iInitialValue = -1;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessFloatDeclInitial
 * --------------------------------------------------*/
int ProcessFloatDeclInitial( char *varName, float initialValue, int published )
{
  int index = CreateVariableIndex( varName, FLOAT_TYPE );
  ts_Symbol *var = NULL;

  if ( index < 0 )
  {
    ExitWithError();
  }

  var = FindVariableNameByIndex( index );
  var->bInitialized = 1;
  var->iInitialValue = *( unsigned int * )&initialValue;

  // remove the data that was pushed by processing any expressions
  // and/or simple literal expressions
  unPushLast( );

  return TRUE;
}

/* --------------------------------------------------
 * ProcessStringDecl
 * --------------------------------------------------*/
int ProcessStringDecl( char *varName, int published )
{
  int index = CreateVariableIndex( varName, STRING_TYPE );
  ts_Symbol *var = NULL;

  if ( index < 0 )
  {
    ExitWithError();
  }

  var = FindVariableNameByIndex( index );
  var->bInitialized = 0;
  var->iInitialValue = -1;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessStringDeclInitial
 * --------------------------------------------------*/
int ProcessStringDeclInitial( char *varName, char *initialValue, int published )
{
  int index = CreateVariableIndex( varName, STRING_TYPE );
  ts_Symbol *var = NULL;

  if ( index < 0 )
  {
    ExitWithError();
  }

  var = FindVariableNameByIndex( index );
  var->bInitialized = 1;
  var->iInitialValue = CreateStringIndex( initialValue );

  return TRUE;
}

/* --------------------------------------------------
 * ProcessAnimTable
 * --------------------------------------------------*/
int ProcessAnimTable( char *pcString )
{
	if (bVerbose == TRUE)
	{
		printf("Got to ANIM_TABLE\n");
	}

	free(pcString);

	return TRUE;
}

/* --------------------------------------------------
 * ProcessDW
 * --------------------------------------------------*/
int ProcessDW( char *pcString )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to DW\n");
	}

	nReturnVal = AddDW( "DW", pcString );
  CHECK_ERROR( nReturnVal );

  nGlobalOffset += DATAWORD_LEN;
	free(pcString);

	return TRUE;
}

/* --------------------------------------------------
 * ProcessAnim
 * --------------------------------------------------*/
int ProcessAnim( char *pcString )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to ANIM\n");
	}

	nReturnVal = AddSymbol( "ANIM", pcString, (unsigned long)nGlobalOffset );
  CHECK_ERROR( nReturnVal );

  free(pcString);

	return TRUE;
}

/* --------------------------------------------------
 * ProcessStartMotion
 * --------------------------------------------------*/
int ProcessStartMotion( int nMotionNumber, int nStartFrame, int nFlags )
{
	int	nReturnVal;

	if ( bVerbose == TRUE )
	{
		printf( "Got to START_MOTION\n" );
	}

	nReturnVal = AddInt( "START_MOTION", LIST_OPCODE, CMD_START_MOTION );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessKillMotion
 * --------------------------------------------------*/
int ProcessKillMotion( void )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to KILL_MOTION\n");
	}

	nReturnVal = AddInt( "KILL_MOTION", LIST_OPCODE, CMD_KILL_MOTION );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessKeyFrame
 * --------------------------------------------------*/
int ProcessKeyFrame( int iFrame )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to KEY_FRAME\n");
	}

	nReturnVal = AddInt( "KEY_FRAME", LIST_OPCODE, CMD_KEY_FRAME );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessInterpFrame
 * --------------------------------------------------*/
int ProcessInterpFrame( int nMotionNumber, int nStartFrame, int nFlags, int nFrames )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to INTERP_FRAME\n");
	}

	nReturnVal = AddInt( "INTERP_FRAME", LIST_OPCODE, CMD_INTERP_FRAME);
	CHECK_ERROR( nReturnVal );

	ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessDelay
 * --------------------------------------------------*/
int ProcessDelay( int nDelay )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to DELAY\n");
	}

  nReturnVal = AddInt( "DELAY", LIST_OPCODE, CMD_DELAY);
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessEndAnim
 * --------------------------------------------------*/
int ProcessEndAnim( void )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to END_ANIM\n");
	}

	nReturnVal = AddInt( "END_ANIM", LIST_OPCODE, CMD_END_ANIM);
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessForce
 * --------------------------------------------------*/
int ProcessForce( float fX, float fY, float fZ )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to FORCE\n");
	}

  nReturnVal = AddInt( "FORCE", LIST_OPCODE, CMD_FORCE );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessImpulse
 * --------------------------------------------------*/
int ProcessImpulse( float fX, float fY, float fZ )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to IMPULSE\n");
	}

	nReturnVal = AddInt( "IMPULSE", LIST_OPCODE, CMD_IMPULSE );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessImpulse_X
 * --------------------------------------------------*/
int ProcessImpulse_X( float fX )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to IMPULSE_X\n");
	}

	nReturnVal = AddInt( "IMPULSE_X", LIST_OPCODE, CMD_IMPULSE_X );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessImpulse_Y
 * --------------------------------------------------*/
int ProcessImpulse_Y( float fY )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to IMPULSE_Y\n");
	}

	nReturnVal = AddInt( "IMPULSE_Y", LIST_OPCODE, CMD_IMPULSE_Y );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessImpulse_Z
 * --------------------------------------------------*/
int ProcessImpulse_Z( float fZ )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to IMPULSE_Z\n");
	}

	nReturnVal = AddInt( "IMPULSE_Z", LIST_OPCODE, CMD_IMPULSE_Z );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessOffset
 * --------------------------------------------------*/
int ProcessOffset( float fX, float fY, float fZ )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to OFFSET\n");
	}

  nReturnVal = AddInt( "OFFSET", LIST_OPCODE, CMD_OFFSET );
  CHECK_ERROR( nReturnVal );

	nGlobalOffset += OPCODE_LEN;

  ADD_DEBUG;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessJump
 * --------------------------------------------------*/
int ProcessJump( void )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to JUMP\n");
	}

	nReturnVal = AddInt( "JUMP", LIST_OPCODE, CMD_JUMP );
  CHECK_ERROR( nReturnVal );

  nGlobalOffset += OPCODE_LEN;

  ADD_DEBUG;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessJumpT
 * --------------------------------------------------*/
int ProcessJumpT( char *pcString )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to JUMP TEXT_\n");
	}

	nReturnVal = ProcessPushA( pcString );
  CHECK_ERROR( nReturnVal );

	nReturnVal = AddInt( "JUMP TEXT_", LIST_OPCODE, CMD_JUMP );
  CHECK_ERROR( nReturnVal );

  nGlobalOffset += OPCODE_LEN;

  ADD_DEBUG;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessNewAnim
 * --------------------------------------------------*/
int ProcessNewAnim( int nAnim )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to NEW_ANIM\n");
	}

	nReturnVal = AddInt( "NEW_ANIM", LIST_OPCODE, CMD_NEW_ANIM );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessCode
 * --------------------------------------------------*/
int ProcessCode( int nFuncIndex )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to CODE\n");
	}

	nReturnVal = AddInt( "CODE", LIST_OPCODE, CMD_CODE);
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessCode1I
 * --------------------------------------------------*/
int ProcessCode1I( int nFuncIndex, int nVal )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to CODE1I\n");
	}

	nReturnVal = AddInt( "CODE1I", LIST_OPCODE, CMD_CODE_1I );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessCode1F
 * --------------------------------------------------*/
int ProcessCode1F( int nFuncIndex, float fX )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to CODE1F\n");
	}

	nReturnVal = AddInt( "CODE1F", LIST_OPCODE, CMD_CODE_1F );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessCode2F
 * --------------------------------------------------*/
int ProcessCode2F( int nFuncIndex, float fX, float fY )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to CODE2F\n");
	}

	nReturnVal = AddInt( "CODE2F", LIST_OPCODE, CMD_CODE_2F );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessCode3F
 * --------------------------------------------------*/
int ProcessCode3F( int nFuncIndex, float fX, float fY, float fZ )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to CODE3F\n");
	}

	nReturnVal = AddInt( "CODE3F", LIST_OPCODE, CMD_CODE_3F );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessIfSet
 * --------------------------------------------------*/
int ProcessIfSet( char *pcString )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to IF_SET\n");
	}

	nReturnVal = ProcessPushA( pcString );
  CHECK_ERROR( nReturnVal );

	nReturnVal = AddInt( "IF_SET", LIST_OPCODE, CMD_IF_SET );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessIfNotSet
 * --------------------------------------------------*/
int ProcessIfNotSet( char *pcString )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to IF_NOT_SET\n");
	}

	nReturnVal = ProcessPushA( pcString );
  CHECK_ERROR( nReturnVal );

	nReturnVal = AddInt( "IF_NOT_SET", LIST_OPCODE, CMD_IF_NOT_SET );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessIfEq
 * --------------------------------------------------*/
int ProcessIfEq( char *pcString )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to IF_EQ\n");
	}

	nReturnVal = ProcessPushA( pcString );
  CHECK_ERROR( nReturnVal );

	nReturnVal = AddInt( "IF_EQ", LIST_OPCODE, CMD_IF_EQ );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessIFLE
 * --------------------------------------------------*/
int ProcessIfLE( char *pcString )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to IF_LE\n");
	}

	nReturnVal = ProcessPushA( pcString );
  CHECK_ERROR( nReturnVal );

	nReturnVal = AddInt( "IF_LE", LIST_OPCODE, CMD_IF_LE );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessIFLT
 * --------------------------------------------------*/
int ProcessIfLT( char *pcString )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to IF_LT\n");
	}

	nReturnVal = ProcessPushA( pcString );
  CHECK_ERROR( nReturnVal );
  
	nReturnVal = AddInt( "IF_LT", LIST_OPCODE, CMD_IF_LT );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessIFNE
 * --------------------------------------------------*/
int ProcessIfNE( char *pcString )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to IF_NE\n");
	}

	nReturnVal = ProcessPushA( pcString );
  CHECK_ERROR( nReturnVal );

	nReturnVal = AddInt( "IF_NE", LIST_OPCODE, CMD_IF_NE );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessIFGE
 * --------------------------------------------------*/
int ProcessIfGE( char *pcString )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to IF_GE\n");
	}

	nReturnVal = ProcessPushA( pcString );
  CHECK_ERROR( nReturnVal );

	nReturnVal = AddInt( "IF_GE", LIST_OPCODE, CMD_IF_GE );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessIFGT
 * --------------------------------------------------*/
int ProcessIfGT( char *pcString )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to IF_GT\n");
	}

	nReturnVal = ProcessPushA( pcString );
  CHECK_ERROR( nReturnVal );

	nReturnVal = AddInt( "IF_GT", LIST_OPCODE, CMD_IF_GT );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessAction
 * --------------------------------------------------*/
int ProcessAction( int nAction )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to ACTION\n");
	}

  nReturnVal = AddInt( "ACTION", LIST_OPCODE, CMD_ACTION );
  CHECK_ERROR( nReturnVal );

	nGlobalOffset += OPCODE_LEN;

  ADD_DEBUG;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessWaitHitGround
 * --------------------------------------------------*/
int ProcessWaitHitGround( void )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to WAIT_HIT_GROUND\n");
	}

	nReturnVal = AddInt( "WAIT_HIT_GROUND", LIST_OPCODE, CMD_WAIT_HIT_GROUND );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessWaitTilHold
 * --------------------------------------------------*/
int ProcessWaitTilHold( void )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to WAIT_TIL_HOLD\n");
	}

	nReturnVal = AddInt( "WAIT_TIL_HOLD", LIST_OPCODE, CMD_WAIT_TIL_HOLD );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessWaitForPause
 * --------------------------------------------------*/
int ProcessWaitForPause( void )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to WAIT_FOR_PAUSE\n");
	}

	nReturnVal = AddInt( "WAIT_FOR_PAUSE", LIST_OPCODE, CMD_WAIT_FOR_PAUSE );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessWaitInterpDone
 * --------------------------------------------------*/
int ProcessWaitInterpDone( void )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to WAIT_INTERP_DONE\n");
	}

	nReturnVal = AddInt( "WAIT_INTERP_DONE", LIST_OPCODE, CMD_WAIT_INTERP_DONE );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessWaitForFrame
 * --------------------------------------------------*/
int ProcessWaitForFrame( int iFrame )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to WAIT_FOR_FRAME\n");
	}

	nReturnVal = AddInt( "WAIT_FOR_FRAME", LIST_OPCODE, CMD_WAIT_FOR_FRAME );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessWaitForEnd
 * --------------------------------------------------*/
int ProcessWaitForEnd( void )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to WAIT_FOR_END\n");
	}

	nReturnVal = AddInt( "WAIT_FOR_END", LIST_OPCODE, CMD_WAIT_FOR_END );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessPlayRandomSound
 * --------------------------------------------------*/
int ProcessPlayRandomSound( int nSoundTableIndex, int nChannel, int nVolume )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to PLAY_RANDOM_SOUND\n");
	}

	nReturnVal = AddInt( "PLAY_RANDOM_SOUND", LIST_OPCODE, CMD_PLAY_RANDOM_SOUND );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessPlaySound
 * --------------------------------------------------*/
int ProcessPlaySound( int nSoundTableIndex )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to PLAY_SOUND <index>\n");
	}

	nReturnVal = AddInt( "PLAY_SOUND", LIST_OPCODE, CMD_PLAY_SOUND );
	CHECK_ERROR( nReturnVal );

	ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}
/* --------------------------------------------------
 * ProcessPlaySoundStr
 * --------------------------------------------------*/
int ProcessPlaySoundStr( char *pcString )
{
	int nReturnVal = 0;

	if ( bVerbose == TRUE )
	{
		printf( "Got to PLAY_SOUND <string>\n" );
	}

	nReturnVal = AddInt( "PLAY_SOUND_STR", LIST_OPCODE, CMD_PLAY_SOUND_STR );
	CHECK_ERROR( nReturnVal );

	ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessPlaySoundVolume
 * --------------------------------------------------*/
int ProcessPlaySoundVolume( int nSoundTableIndex, int nVolume )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to PLAY_SOUND_VOLUME <index>\n");
	}

	nReturnVal = AddInt( "PLAY_SOUND_VOLUME", LIST_OPCODE, CMD_PLAY_SOUND_VOLUME );
	CHECK_ERROR( nReturnVal );

	ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessPlaySoundVolumeStr
 * --------------------------------------------------*/
int ProcessPlaySoundVolumeStr( char *pcString, int nVolume )
{
	int nReturnVal = 0;

	if ( bVerbose == TRUE )
	{
		printf( "Got to PLAY_SOUND_VOLUME_STR <string>\n" );
	}

	nReturnVal = AddInt( "PLAY_SOUND_VOLUME_STR", LIST_OPCODE, CMD_PLAY_SOUND_VOLUME_STR );
	CHECK_ERROR( nReturnVal );

	ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessCollisionDelay
 * --------------------------------------------------*/
int ProcessCollisionDelay( int nTicks )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to COLLISION_DELAY\n");
	}

	nReturnVal = AddInt( "COLLISION_DELAY", LIST_OPCODE, CMD_COLLISION_DELAY );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}


/* --------------------------------------------------
 * ProcessSetFlag
 * --------------------------------------------------*/
int ProcessSetFlag( int nFlags )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to SET_FLAG\n");
	}

	nReturnVal = AddInt( "SET_FLAG", LIST_OPCODE, CMD_SET_FLAG );
  CHECK_ERROR( nReturnVal );

	nGlobalOffset += OPCODE_LEN;

  ADD_DEBUG;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessClrFlag
 * --------------------------------------------------*/
int ProcessClrFlag( int nFlags )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to CLR_FLAG\n");
	}

	nReturnVal = AddInt( "CLR_FLAG", LIST_OPCODE, CMD_CLR_FLAG );
  CHECK_ERROR( nReturnVal );

	nGlobalOffset += OPCODE_LEN;

  ADD_DEBUG;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessCreateProc
 * --------------------------------------------------*/
int ProcessCreateProc( int nProcNumber, int nProcID )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to CREATE_PROC\n");
	}

	nReturnVal = AddInt( "CREATE_PROC", LIST_OPCODE, CMD_CREATE_PROC );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessKillProc
 * --------------------------------------------------*/
int ProcessKillProc( int nProcID )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to KILL_PROC\n");
	}

	nReturnVal = AddInt( "KILL_PROC", LIST_OPCODE, CMD_KILL_PROC );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessSound
 * --------------------------------------------------*/
int ProcessSound( int nSequence )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to SOUND\n");
	}

	nReturnVal = AddInt( "SOUND", LIST_OPCODE, CMD_SOUND );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessAttachObj
 * --------------------------------------------------*/
int ProcessAttachObj( void )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to ATTACH_OBJ\n");
	}

	nReturnVal = AddInt( "ATTACH_OBJ", LIST_OPCODE, CMD_ATTACH_OBJ );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessDetachObj
 * --------------------------------------------------*/
int ProcessDetachObj( void )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to DETACH_OBJ\n");
	}

	nReturnVal = AddInt( "DETACH_OBJ", LIST_OPCODE, CMD_DETACH_OBJ );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessSlavePos
 * --------------------------------------------------*/
int ProcessSlavePos( float fX, float fY, float fZ )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to SLAVE_POS\n");
	}

	nReturnVal = AddInt( "SLAVE_POS", LIST_OPCODE, CMD_SLAVE_POS );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessSlaveForce
 * --------------------------------------------------*/
int ProcessSlaveForce( float fX, float fY, float fZ )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to SLAVE_FORCE\n");
	}

	nReturnVal = AddInt( "SLAVE_FORCE", LIST_OPCODE, CMD_SLAVE_FORCE );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessSlaveImpulse
 * --------------------------------------------------*/
int ProcessSlaveImpulse( float fX, float fY, float fZ )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to SLAVE_IMPULSE\n");
	}

	nReturnVal = AddInt( "SLAVE_IMPULSE", LIST_OPCODE, CMD_SLAVE_IMPULSE );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessCheckDead
 * --------------------------------------------------*/
int ProcessCheckDead( void )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to CHECK_DEAD\n");
	}

	nReturnVal = AddInt( "CHECK_DEAD", LIST_OPCODE, CMD_CHECK_DEAD );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessCollision
 * --------------------------------------------------*/
int ProcessCollision( int nType )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to COLLISION\n");
	}

	nReturnVal = AddInt( "COLLISION", LIST_OPCODE, CMD_COLLISION );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessPauseMotion
 * --------------------------------------------------*/
int ProcessPauseMotion( void )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to PAUSE_MOTION\n");
	}

	nReturnVal = AddInt( "PAUSE_MOTION", LIST_OPCODE, CMD_PAUSE_MOTION );
	CHECK_ERROR( nReturnVal );

	ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessResumeMotion
 * --------------------------------------------------*/
int ProcessResumeMotion( void )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to RESUME_MOTION\n");
	}

	nReturnVal = AddInt( "RESUME_MOTION", LIST_OPCODE, CMD_RESUME_MOTION );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

int ProcessUnknown( char *pcCommand)
{
	fprintf( stderr, "%s(%d) : ERROR: Unknown command '%s'\n", unprocessedFileName, unprocessedFileLine, pcCommand );
	ExitWithError( );

	return FALSE;
}

/* --------------------------------------------------
 * ProcessRotate
 * --------------------------------------------------*/
int ProcessRotate( float fX, float fY, float fZ )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to ROTATE\n");
	}

	nReturnVal = AddInt( "ROTATE", LIST_OPCODE, CMD_ROTATE );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessLabel
 * --------------------------------------------------*/
int ProcessLabel( char *pcString )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to LABEL: %s\n", pcString);
	}

 	// Remove the colon
	pcString[strlen(pcString)-1] = 0;

	nReturnVal = AddSymbol( "LABEL", pcString, (unsigned long)nGlobalOffset);
  CHECK_ERROR( nReturnVal );

  free(pcString);

	return TRUE;
}


/* --------------------------------------------------
 * ProcessGetRandom
 * --------------------------------------------------*/
int ProcessGetRandom( int iMax )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to GET_RANDOM\n");
	}

	nReturnVal = AddInt( "GET_RANDOM", LIST_OPCODE, CMD_GET_RANDOM );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessGetFrame
 * --------------------------------------------------*/
int ProcessGetFrame( void )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to GET_FRAME\n");
	}

	nReturnVal = AddInt( "GET_FRAME", LIST_OPCODE, CMD_GET_FRAME );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessGetAction
 * --------------------------------------------------*/
int ProcessGetAction( void )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to GET_ACTION\n");
	}

	nReturnVal = AddInt( "GET_ACTION", LIST_OPCODE, CMD_GET_ACTION );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessTestValue
 * --------------------------------------------------*/
int ProcessTestValue( int iValue )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to TEST_VALUE\n");
	}

  nReturnVal = AddInt( "TEST_VALUE", LIST_OPCODE, CMD_TEST_VALUE );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessTestFlag
 * --------------------------------------------------*/
int ProcessTestFlag( int nFlag )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to TEST_FLAG\n");
	}

  nReturnVal = AddInt( "TEST_FLAG", LIST_OPCODE, CMD_TEST_FLAG );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessTestAction
 * --------------------------------------------------*/
int ProcessTestAction( int nAction )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to TEST_ACTION\n");
	}

  nReturnVal = AddInt( "TEST_ACTION", LIST_OPCODE, CMD_TEST_ACTION );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessTestEndMotion
 * --------------------------------------------------*/
int ProcessTestEndMotion( void )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to TEST_ENDMOTION\n");
	}

  nReturnVal = AddInt( "TEST_ENDMOTION", LIST_OPCODE, CMD_TEST_ENDMOTION );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessTestOnGround
 * --------------------------------------------------*/
int ProcessTestOnGround( void )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to TEST_ONGROUND\n");
	}

  nReturnVal = AddInt( "TEST_ONGROUND", LIST_OPCODE, CMD_TEST_ONGROUND );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessTestRandom
 * --------------------------------------------------*/
int ProcessTestRandom( float fThreshold )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to TEST_RANDOM\n");
	}

  nReturnVal = AddInt( "TEST_RANDOM", LIST_OPCODE, CMD_TEST_RANDOM );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessTestFrame
 * --------------------------------------------------*/
int ProcessTestFrame( int iFrame )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to TEST_FRAME\n");
	}

  nReturnVal = AddInt( "TEST_FRAME", LIST_OPCODE, CMD_TEST_FRAME );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessInterpDone
 * --------------------------------------------------*/
int ProcessTestInterpDone( void )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to TEST_INTERPDONE\n");
	}

  nReturnVal = AddInt( "TEST_INTERPDONE", LIST_OPCODE, CMD_TEST_INTERPDONE );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

int labelElements = 0;
char *labelList[4096];

void CreateEmptyLabelList( void )
{
	labelElements = 0;
}

void AppendLabelToLabelList( char *pcString, char *pcString2 )
{
	labelList[labelElements++] = _strdup(pcString2);
}


/* --------------------------------------------------
 * ProcessCase
 * --------------------------------------------------*/
int ProcessCase( char *pcString )
{
	int	nReturnVal;
	int i;

	if (bVerbose == TRUE)
	{
		printf("Got to CASE, with %d labels\n", labelElements);
	}

	nReturnVal = AddInt( "CASE", LIST_OPCODE, CMD_CASE );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nReturnVal = AddInt( "CASE", LIST_INTEGER, labelElements);
  CHECK_ERROR( nReturnVal );

	if ( labelElements <= 0 )
  {
		ExitWithError();
  }

	for( i = 0; i < labelElements; i++ )
	{
		nReturnVal = AddAddress( "CASE", labelList[ i ] );
    CHECK_ERROR( nReturnVal );
		free( labelList[ i ] );
	}

	nGlobalOffset += OPCODE_LEN + INT_LEN;
	nGlobalOffset += labelElements * ADDRESS_LEN;

// don't know what should be passed in here, but it's not a char *..., so don't free
//	free(pcString);

	return TRUE;
}

/* --------------------------------------------------
 * ProcessRESOP0A
 * --------------------------------------------------*/
int ProcessRESOP0A( void )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to RES_OP0A\n");
	}

	nReturnVal = AddInt( "RES_OP0A", LIST_OPCODE, CMD_RES_OP0A );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessRESOP0B
 * --------------------------------------------------*/
int ProcessRESOP0B( void )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to RES_OP0B\n");
	}

	nReturnVal = AddInt( "RES_OP0B", LIST_OPCODE, CMD_RES_OP0B );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessRESOP1F
 * --------------------------------------------------*/
int ProcessRESOP1F( float fParm )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to RES_OP1F\n");
	}

	nReturnVal = AddInt( "RES_OP1F", LIST_OPCODE, CMD_RES_OP1F );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessRESOP1I
 * --------------------------------------------------*/
int ProcessRESOP1I( int iValue )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to RES_OP1I\n");
	}

	nReturnVal = AddInt( "RES_OP1I", LIST_OPCODE, CMD_RES_OP1I );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessSleep
 * --------------------------------------------------*/
int ProcessSleep( float fVal )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to SLEEP\n");
	}

  nReturnVal = AddInt( "SLEEP", LIST_OPCODE, CMD_SLEEP );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessStartMotion2
 * --------------------------------------------------*/
int ProcessStartMotion2( int nMotionNumber, int nStartFrame, int nFlags, int nMotnInfIndex )
{
	int	nReturnVal;

	if ( bVerbose == TRUE )
	{
		printf( "Got to START_MOTION2\n" );
	}

	nReturnVal = AddInt( "START_MOTION2", LIST_OPCODE, CMD_START_MOTION2 );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessKillMotion2
 * --------------------------------------------------*/
int ProcessKillMotion2( int nMotnInfIndex )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to KILL_MOTION2\n");
	}

	nReturnVal = AddInt( "KILL_MOTION2", LIST_OPCODE, CMD_KILL_MOTION2 );
	CHECK_ERROR( nReturnVal );

	ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}


/* --------------------------------------------------
 * ProcessTestEndMotion2
 * --------------------------------------------------*/
int ProcessTestEndMotion2( int nMotnInfIndex )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to TEST_ENDMOTION2\n");
	}

  nReturnVal = AddInt( "TEST_ENDMOTION2", LIST_OPCODE, CMD_TEST_ENDMOTION2 );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessTestFrame2
 * --------------------------------------------------*/
int ProcessTestFrame2( int frame, int nMotnInfIndex )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to TEST_FRAME2\n");
	}

  nReturnVal = AddInt( "TEST_FRAME2", LIST_OPCODE, CMD_TEST_FRAME2 );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessWaitForEnd2
 * --------------------------------------------------*/
int ProcessWaitForEnd2( int nMotnInfIndex )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to WAIT_FOR_END2\n");
	}

	nReturnVal = AddInt( "WAIT_FOR_END2", LIST_OPCODE, CMD_WAIT_FOR_END2 );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessWaitForFrame2
 * --------------------------------------------------*/
int ProcessWaitForFrame2( int iFrame, int nMotnInfIndex )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to WAIT_FOR_FRAME2\n");
	}

	nReturnVal = AddInt( "WAIT_FOR_FRAME2", LIST_OPCODE, CMD_WAIT_FOR_FRAME2 );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessInterpFrame2
 * --------------------------------------------------*/
int ProcessInterpFrame2( int nMotionNumber, int nStartFrame, int nFlags, int nFrames, int nMotnInfIndex )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to INTERP_FRAME2\n");
	}

	nReturnVal = AddInt( "INTERP_FRAME2", LIST_OPCODE, CMD_INTERP_FRAME2 );
	CHECK_ERROR( nReturnVal );

	ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessWaitInterpDone2
 * --------------------------------------------------*/
int ProcessWaitInterpDone2( int nMotnInfIndex )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to WAIT_INTERP_DONE2\n");
	}

	nReturnVal = AddInt( "WAIT_INTERP_DONE2", LIST_OPCODE, CMD_WAIT_INTERP_DONE2 );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessPauseMotion2
 * --------------------------------------------------*/
int ProcessPauseMotion2( int nMotnInfIndex )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to PAUSE_MOTION2\n");
	}

	nReturnVal = AddInt( "PAUSE_MOTION2", LIST_OPCODE, CMD_PAUSE_MOTION2 );
	CHECK_ERROR( nReturnVal );

	ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessResumeMotion2
 * --------------------------------------------------*/
int ProcessResumeMotion2( int nMotnInfIndex )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to RESUME_MOTION2\n");
	}

	nReturnVal = AddInt( "RESUME_MOTION2", LIST_OPCODE, CMD_RESUME_MOTION2 );
	CHECK_ERROR( nReturnVal );

	ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessGetFrame2
 * --------------------------------------------------*/
int ProcessGetFrame2( int nMotnInfIndex )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to GET_FRAME2\n");
	}

	nReturnVal = AddInt( "GET_FRAME2", LIST_OPCODE, CMD_GET_FRAME2 );
	CHECK_ERROR( nReturnVal );

	ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessMin
 * --------------------------------------------------*/
int ProcessMin( void )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to MIN\n");
	}

	nReturnVal = AddInt( "MIN", LIST_OPCODE, CMD_MIN );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessMax
 * --------------------------------------------------*/
int ProcessMax( void )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to MAX\n");
	}

	nReturnVal = AddInt( "MAX", LIST_OPCODE, CMD_MAX );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessAbs
 * --------------------------------------------------*/
int ProcessAbs( void )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to ABS\n");
	}

	nReturnVal = AddInt( "ABS", LIST_OPCODE, CMD_ABS );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessDeref
 * --------------------------------------------------*/
int ProcessDeref( void )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to DEREF\n");
	}

	nReturnVal = AddInt( "DEREF", LIST_OPCODE, CMD_DEREF );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessDerefInt
 * --------------------------------------------------*/
int ProcessDerefInt( void )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to DEREF_INT\n");
	}

	nReturnVal = AddInt( "DEREF_INT", LIST_OPCODE, CMD_DEREF_INT );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessDerefFloat
 * --------------------------------------------------*/
int ProcessDerefFloat( void )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to DEREF_FLOAT\n");
	}

	nReturnVal = AddInt( "DEREF_FLOAT", LIST_OPCODE, CMD_DEREF_FLOAT );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessPushCall
 * --------------------------------------------------*/
int ProcessPushCall( char *pcString )
{
  int nReturnVal;
  char *temp = pcString;

  if (bVerbose == TRUE)
	{
    printf("Got to PUSH_CALL address\n");
	}

  if ( *pcString == '%' )
  { /* check for a reference value */
    temp++;
  }

  nReturnVal = AddInt( "PUSH_CALL", LIST_OPCODE, CMD_PUSH_CALL );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nReturnVal = AddAddress( "PUSH_CALL", temp );
  CHECK_ERROR( nReturnVal );

  nGlobalOffset += OPCODE_LEN + ADDRESS_LEN;
	free(pcString);

  return TRUE;
}

/* --------------------------------------------------
 * ProcessPopCall
 * --------------------------------------------------*/
int ProcessPopCall( void )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to POP_CALL\n");
	}

  nReturnVal = AddInt( "POP_CALL", LIST_OPCODE, CMD_POP_CALL );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessLoadCall
 * --------------------------------------------------*/
int ProcessLoadCall( char* varName )
{
  int index;
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to LOAD_CALL TEXT_\n");
	}

  /* Lookup varName index */
  index = FindVariableIndex( varName );
  if ( index < 0 )
  {
		fprintf( stderr, "%s(%d) : Error on LOAD_CALL: Undeclared identifier: %s\n", unprocessedFileName, unprocessedFileLine, varName );
		ExitWithError( );
  }

  /* build load <index> instruction */
  nReturnVal = AddInt( "LOAD_CALL", LIST_OPCODE, CMD_LOAD_CALL );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nReturnVal = AddInt( "LOAD_CALL", LIST_INTEGER, index );
  CHECK_ERROR( nReturnVal );

  nGlobalOffset += OPCODE_LEN + INT_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessStoreCall
 * --------------------------------------------------*/
int ProcessStoreCall( char* varName )
{
  int index;
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to STORE_CALL TEXT_\n");
	}

  /* Lookup varName index */
  index = FindVariableIndex( varName );
  if ( index < 0 )
  {
		fprintf( stderr, "%s(%d) : Error on STORE_CALL: Undeclared identifier: %s\n", unprocessedFileName, unprocessedFileLine, varName );
		ExitWithError( );
  }
  
  /* build load <index> instruction */
  nReturnVal = AddInt( "STORE_CALL", LIST_OPCODE, CMD_STORE_CALL );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nReturnVal = AddInt( "STORE_CALL", LIST_INTEGER, index );
  CHECK_ERROR( nReturnVal );

  nGlobalOffset += OPCODE_LEN + INT_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessSetAnimRate
 * --------------------------------------------------*/
int ProcessSetAnimRate( float fRate )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to SET_ANIM_RATE float\n");
	}

  nReturnVal = AddInt( "SET_ANIM_RATE", LIST_OPCODE, CMD_SET_ANIM_RATE );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessSetAnimRate2
 * --------------------------------------------------*/
int ProcessSetAnimRate2( float fRate )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to SET_ANIM_RATE2 float\n");
	}

  nReturnVal = AddInt( "SET_ANIM_RATE2", LIST_OPCODE, CMD_SET_ANIM_RATE2 );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessSetActionLo
 * --------------------------------------------------*/
int ProcessSetActionLo( int action )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to SET_ACTION_LO int\n");
	}

  nReturnVal = AddInt( "SET_ACTION_LO", LIST_OPCODE, CMD_SET_ACTION_LO );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessGetActionLo
 * --------------------------------------------------*/
int ProcessGetActionLo( void )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to GET_ACTION_LO int\n");
	}

  nReturnVal = AddInt( "GET_ACTION_LO", LIST_OPCODE, CMD_GET_ACTION_LO );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessSetActionHi
 * --------------------------------------------------*/
int ProcessSetActionHi( int action )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to SET_ACTION_HI int\n");
	}

  nReturnVal = AddInt( "SET_ACTION_HI", LIST_OPCODE, CMD_SET_ACTION_HI );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

/* --------------------------------------------------
 * ProcessGetActionHi
 * --------------------------------------------------*/
int ProcessGetActionHi( void )
{
  int nReturnVal;

  if (bVerbose == TRUE)
	{
    printf("Got to GET_ACTION_HI int\n");
	}

  nReturnVal = AddInt( "GET_ACTION_HI", LIST_OPCODE, CMD_GET_ACTION_HI );
  CHECK_ERROR( nReturnVal );

  ADD_DEBUG;

  nGlobalOffset += OPCODE_LEN;

  return TRUE;
}

















/* --------------------------------------------------
 * 	AddInt
 * --------------------------------------------------*/
int AddInt( char *pcRoutine, int nType, int nData )
{
	ts_DataInt 		*pCurOpcode;

	// Allocate the opcode link for the data list
	pCurOpcode = (ts_DataInt *) malloc( sizeof(ts_DataInt) );
	if (pCurOpcode == NULL)
	{
		printf( "Could not allocate memory in %s (for data list opcode).\n", pcRoutine);
		return FALSE;
	}
	pCurOpcode->hBlock.pNext = NULL;
  pCurOpcode->hBlock.pPrev = NULL;
	pCurOpcode->hBlock.nType = nType;
  pCurOpcode->hBlock.line = iLineCount;
	pCurOpcode->nData = nData;
	AddToDataList( (ts_Hdr *) pCurOpcode);

	return TRUE;
}

/* --------------------------------------------------
 * 	AddFloat
 * --------------------------------------------------*/
int AddFloat( char *pcRoutine, float fData )
{
	ts_DataFloat	*pCurFloat;

	// Allocate the float link for the data list
	pCurFloat = (ts_DataFloat *) malloc( sizeof(ts_DataFloat) );
	if (pCurFloat == NULL)
	{
		printf( "Could not allocate memory in %s (for data list float).\n", pcRoutine);
		return FALSE;
	}
	pCurFloat->hBlock.pNext = NULL;
  pCurFloat->hBlock.pPrev = NULL;
	pCurFloat->hBlock.nType = LIST_FLOAT;
  pCurFloat->hBlock.line = iLineCount;
	pCurFloat->fData = fData;
	AddToDataList( (ts_Hdr *) pCurFloat);

	return TRUE;
}

/* --------------------------------------------------
 * 	AddPatchAddress
 * --------------------------------------------------*/
int AddPatchAddress( char *pcRoutine, char *pcString, int nType )
{
	ts_DataAddress 	*pCurAddress;
	ts_BackPatch		*pCurPatch;

	// Allocate the address link for the data list
	pCurAddress = (ts_DataAddress *) malloc( sizeof(ts_DataAddress) );
	if (pCurAddress == NULL)
	{
		printf( "Could not allocate memory in %s (for data list address).\n",pcRoutine);
		return FALSE;
	}
	pCurAddress->hBlock.pNext = NULL;
  pCurAddress->hBlock.pPrev = NULL;
	pCurAddress->hBlock.nType = nType;
  pCurAddress->hBlock.line = iLineCount;
	pCurAddress->ulData = NEEDS_PATCH;
	AddToDataList( (ts_Hdr *) pCurAddress);

	// Allocate the new link for the patch list
	pCurPatch = (ts_BackPatch *) malloc( sizeof(ts_BackPatch) );
	if (pCurPatch == NULL)
	{
		printf( "Could not allocate memory in %s (for patch list).\n", pcRoutine);
		return FALSE;
	}
	pCurPatch->pNext = NULL;
	pCurPatch->pData = (ts_Hdr *) pCurAddress;
	strcpy( pCurPatch->caName, pcString );
	AddToPatchList( pCurPatch );

	return TRUE;
}

/* --------------------------------------------------
 * 	AddAddress
 * --------------------------------------------------*/
int AddAddress( char *pcRoutine, char *pcString )
{
  return( AddPatchAddress( pcRoutine, pcString, LIST_ADDRESS ) );
}

/* --------------------------------------------------
 * 	AddDW
 * --------------------------------------------------*/
int AddDW( char *pcRoutine, char *pcString )
{
  return( AddPatchAddress( pcRoutine, pcString, LIST_DATAWORD ) );
}

/* --------------------------------------------------
 * 	AddString
 * --------------------------------------------------*/
int AddString( char *pcRoutine, char *pcString )
{
	ts_Symbol		*pCurSymbol;

	// Allocate the new link for the symbol list
	pCurSymbol = (ts_Symbol *) malloc( sizeof(ts_Symbol) );
	if (pCurSymbol == NULL)
	{
		printf( "Could not allocate memory in %s (for string list).\n", pcRoutine);
		return FALSE;
	}
	pCurSymbol->pNext = NULL;
	pCurSymbol->ulValue = ( unsigned int )malloc( strlen( pcString ) + 1 );
  strcpy( ( char * )pCurSymbol->ulValue, pcString );
  pCurSymbol->line = iLineCount;
  memset( pCurSymbol->caName, 0, MAX_NAME_LEN );
  strcpy( pCurSymbol->caName, pcString );
	AddToStringList( pCurSymbol );

	return TRUE;
}

/* --------------------------------------------------
 * 	AddSymbol
 * --------------------------------------------------*/
int AddSymbol( char *pcRoutine, char *pcString, unsigned long ulValue )
{
	ts_Symbol		*pCurSymbol;

	// Allocate the new link for the symbol list
	pCurSymbol = (ts_Symbol *) malloc( sizeof(ts_Symbol) );
	if (pCurSymbol == NULL)
	{
		printf( "Could not allocate memory in %s (for symbol list).\n", pcRoutine);
		return FALSE;
	}
	pCurSymbol->pNext = NULL;
	pCurSymbol->ulValue = ulValue;
  pCurSymbol->line = iLineCount;
  memset( pCurSymbol->caName, 0, MAX_NAME_LEN );
	strcpy( pCurSymbol->caName, pcString);
	AddToSymbolList( pCurSymbol);

	return TRUE;
}

/* --------------------------------------------------
 * 	AddVar
 * --------------------------------------------------*/
int AddVar( char *pcRoutine, char *pcString, unsigned long ulValue )
{
  int         index;
	ts_Symbol		*pCurSymbol;

	// Allocate the new link for the symbol list
	pCurSymbol = (ts_Symbol *) malloc( sizeof(ts_Symbol) );
	if (pCurSymbol == NULL)
	{
		printf( "Could not allocate memory in %s (for symbol list).\n", pcRoutine);
		return FALSE;
	}
	pCurSymbol->pNext = NULL;
	pCurSymbol->ulValue = ulValue;
  pCurSymbol->line = iLineCount;
  memset( pCurSymbol->caName, 0, MAX_NAME_LEN );
	strcpy( pCurSymbol->caName, pcString);
	AddToVarList( pCurSymbol );

  /* performs hash value sanity check */
  index = FindVariableIndex( pcString );

  /* ned says: script variable names shouldn't be needed in the string table that gets output to the .sdb/.adb file, */
  /* since they are in the variable hash table. save a bit of space by removing them */
//  pCurSymbol->iNameIndex = CreateStringIndex( pcString );
  pCurSymbol->iNameIndex = CreateStringIndex( "" );	// all variable records will get the same index

	return TRUE;
}

/* --------------------------------------------------
 * 	AddFunction
 * --------------------------------------------------*/
int AddFunction( char *pcRoutine, char *pcString, unsigned long ulValue )
{
  int         index;
	ts_Symbol		*pCurSymbol;

	// Allocate the new link for the symbol list
	pCurSymbol = (ts_Symbol *) malloc( sizeof(ts_Symbol) );
	if (pCurSymbol == NULL)
	{
		printf( "Could not allocate memory in %s (for symbol list).\n", pcRoutine);
		return FALSE;
	}
	pCurSymbol->pNext = NULL;
	pCurSymbol->ulValue = ulValue;
  pCurSymbol->line = iLineCount;
  memset( pCurSymbol->caName, 0, MAX_NAME_LEN );
	strcpy( pCurSymbol->caName, pcString);
	AddToFunctionList( pCurSymbol);

  /* performs hash value sanity check */
  index = FindFunctionIndex( pcString );

  pCurSymbol->iNameIndex = CreateStringIndex( pcString );

	return TRUE;
}

/* --------------------------------------------------
 * 	AddToDataList
 * --------------------------------------------------*/
void AddToDataList( ts_Hdr *pCurData )
{
	pDataList->pNext = pCurData;
  pCurData->pPrev = pDataList;
	pDataList = pCurData;
}

/* --------------------------------------------------
 * 	AddToPatchList
 * --------------------------------------------------*/
void AddToPatchList( ts_BackPatch *pCurPatch )
{
	pPatchList->pNext = pCurPatch;
	pPatchList = pCurPatch;
}

/* --------------------------------------------------
 * 	AddToSymbolList
 * --------------------------------------------------*/
void AddToSymbolList( ts_Symbol *pCurSymbol )
{
	pSymbolList->pNext = pCurSymbol;
	pSymbolList = pCurSymbol;
}

/* --------------------------------------------------
 * 	AddToStringList
 * --------------------------------------------------*/
void AddToStringList( ts_Symbol *pCurSymbol )
{
	pStringList->pNext = pCurSymbol;
	pStringList = pCurSymbol;
}

/* --------------------------------------------------
 * 	AddToVarList
 * --------------------------------------------------*/
void AddToVarList( ts_Symbol *pCurSymbol )
{ /* make sure we add the new variable to the end of the list */
  pVarList = &VarList;
  if ( pVarList != NULL )
  {
    while( pVarList != NULL )
    {
      if ( pVarList->pNext == NULL )
      {
        pVarList->pNext = pCurSymbol;
        pVarList = pCurSymbol;
        return;
      }
      pVarList = pVarList->pNext;
    }
  }
}

/* --------------------------------------------------
 * 	AddToStringList
 * --------------------------------------------------*/
void AddToFunctionList( ts_Symbol *pCurSymbol )
{
	pFunctionList->pNext = pCurSymbol;
	pFunctionList = pCurSymbol;
}

/* --------------------------------------------------
 * 	ExitWithError
 * --------------------------------------------------*/
void ExitWithError( void )
{
	// Perform cleanup of all allocated memory and files
	FreeMem();
  if( fInfile )
    fclose(fInfile);
  if( fOutfile )
    fclose(fOutfile);

  if( bPauseWhenDone )
    getch();

  exit(1);
}

/* --------------------------------------------------
 * 	FreeMem
 * --------------------------------------------------*/
void FreeMem( void )
{
	ts_Hdr		*pTempData;
	ts_BackPatch	*pTempPatch;
	ts_Symbol	*pTempSymbol;

	pDataList = &DataList;
	pDataList = pDataList->pNext;		//Skip anchor
	if (pDataList != NULL)
	{
		while (pDataList->pNext != NULL)
		{
			pTempData = pDataList;
			pDataList = pDataList->pNext;
			free(pTempData);
		}
		free(pDataList);
	}

	pPatchList = &PatchList;
	pPatchList = pPatchList->pNext;		//Skip anchor
	if (pPatchList != NULL)
	{
		while (pPatchList->pNext != NULL)
		{
			pTempPatch = pPatchList;
			pPatchList = pPatchList->pNext;
			free(pTempPatch);
		}
		free(pPatchList);
	}

	pSymbolList = &SymbolList;
	pSymbolList = pSymbolList->pNext;	//Skip anchor
	if (pSymbolList != NULL)
	{
		while (pSymbolList->pNext != NULL)
		{
			pTempSymbol = pSymbolList;
			pSymbolList = pSymbolList->pNext;
			free(pTempSymbol);
		}
		free(pSymbolList);
	}

  pVarList = &VarList;
  pVarList = pVarList->pNext;
	if (pVarList != NULL)
	{
		while (pVarList->pNext != NULL)
		{
			pTempSymbol = pVarList;
			pVarList = pVarList->pNext;
			free(pTempSymbol);
		}
		free(pVarList);
	}

  pFunctionList = &FunctionList;
  pFunctionList = pFunctionList->pNext;
	if ( pFunctionList != NULL )
	{
		while ( pFunctionList->pNext != NULL )
		{
			pTempSymbol = pFunctionList;
			pFunctionList = pFunctionList->pNext;
			free( pTempSymbol );
		}
		free( pFunctionList );
	}

  pStringList = &StringList;
  pStringList = pStringList->pNext;
	if ( pStringList != NULL )
	{
		while ( pStringList->pNext != NULL )
		{
			pTempSymbol = pStringList;
			pStringList = pStringList->pNext;
      if ( pTempSymbol->ulValue != 0 )
      {
        free( ( char * )pTempSymbol->ulValue );
      }
			free( pTempSymbol );
		}
		free( pStringList );
	}

  if (pOutputArray != NULL)
	{
		free(pOutputArray);
	}

  if ( debugOffsetTable != NULL )
  {
    /* all these pointers point into debugText buffer */

    /* TODO: SCL - why is this crashing?? */
    //free( debugOffsetTable );

    debugOffsetTable = NULL;
  }

  if ( debugText != NULL )
  {
    /* TODO: SCL - whis is this crashing?? */
    //free( debugText );
    debugText = NULL;
  }

}

/* --------------------------------------------------
 * Local Functions
 * --------------------------------------------------*/
void removeLastData( void )
{
  ts_Hdr *pCurrList = &DataList;

  if ( DataList.pNext == NULL )
  {
    // no datanodes
    return;
  }

  // find the last node - DataList is NOT a circular list!!
  while ( pCurrList->pNext != NULL )
  {
    pCurrList = pCurrList->pNext;
  }

  if ( pCurrList->pPrev != NULL )
  {
    pCurrList->pPrev->pNext = NULL;
  }

  pDataList = pCurrList->pPrev;

  free( pCurrList );

}

void unPushLast( void )
{
  /* assumes that a PUSH operation lives at the end of the data list */
  removeLastData( );
  removeLastData( );

  // be sure to update our global offset
  nGlobalOffset -= ( OPCODE_LEN + INT_LEN );

  if ( bDebug )
  {
    removeLastData( );

    // be sure to update our global offset
    nGlobalOffset -= INT_LEN;
  }

  return;
}

ts_Hdr *FindDataByLineNumber( int lineNumber )
{
  ts_Hdr *curr = DataList.pNext;
  while( curr != NULL )
  {
    if ( curr->line == lineNumber )
    {
      return( curr );
    }
    curr = curr->pNext;
  }
  return( NULL );
}

ts_Symbol *FindVariableNameByIndex( int index )
{
  int i = 0;
  ts_Symbol *curr = VarList.pNext;

  while( curr != NULL )
  {
    if ( i++ == index )
    {
      return( curr );
    }
    curr = curr->pNext;
  }

  return( NULL );
}

ts_Symbol *FindFunctionNameByIndex( int index )
{
  int i = 0;
  ts_Symbol *curr = FunctionList.pNext;

  while( curr != NULL )
  {
    if ( i++ == index )
    {
      return( curr );
    }
    curr = curr->pNext;
  }

  return( NULL );
}

ts_Symbol *FindStringByIndex( int index )
{
  int i = 0;
  ts_Symbol *curr = StringList.pNext;

  if ( index < 0 )
  {
	  return( NULL );
  }

  while( curr != NULL )
  {
    if ( i++ == index )
    {
      return( curr );
    }
    curr = curr->pNext;
  }

  return( NULL );
}

ts_Symbol *FindSymbolByAddress( int address )
{
  ts_Symbol *curr = &SymbolList;

  while ( curr != NULL )
  {
    if ( curr->ulValue == ( unsigned int )address )
    {
      return( curr );
    }
    curr = curr->pNext;
  }
  return( NULL );
}

ts_Symbol *FindSymbolByLine( int nLine )
{
  ts_Symbol *curr = &SymbolList;

  while ( curr != NULL )
  {
    if ( curr->line == nLine )
    {
      return( curr );
    }
    curr = curr->pNext;
  }
  return( NULL );
}

ts_Symbol *FindSymbolByName( char* symbolName )
{
  int i = 0;
  ts_Symbol *prev = NULL, *found = NULL;

  pSymbolList = &SymbolList;
  prev = pSymbolList;
  pSymbolList = pSymbolList->pNext;

  if ( pSymbolList != NULL )
  {
    while( pSymbolList != NULL )
    {
      if ( strcmp( pSymbolList->caName, symbolName ) == 0 )
      {
        found = pSymbolList;
      }
      else
      {
        i++;
      }
      prev = pSymbolList;
      pSymbolList = pSymbolList->pNext;
    }
  }

  pSymbolList = prev;
  return( found );
}

int CreateStringIndex( char *strValue )
{ /* only create a new String index if one doesn't exist */
  int nReturnValue;
  int index = FindStringIndex( strValue );
  if ( index == -1 )
  {
    nReturnValue = AddString( "CreateStringIndex", strValue );
    CHECK_ERROR( nReturnValue );

    index = FindStringIndex( strValue );
    if ( index == -1 )
    {
      ExitWithError();
    }
  }

  return index;
}

int FindStringIndex( char* strValue )
{
  int i = -1;
  ts_Symbol *curr = StringList.pNext;

  while( curr != NULL )
  {
    i++;
    if ( strcmp( ( char * )curr->ulValue, strValue ) == 0 )
    {
      return( i );
    }
    curr = curr->pNext;
  }

  return( -1 );
}

int CreateFunctionIndex( char *funcName )
{ /* only create a new function index if one doesn't exist */
  int nReturnValue;
  int index = FindFunctionIndex( funcName );
  if ( index == -1 )
  {
    nReturnValue = AddFunction( "CreateFunctionIndex", funcName, Hash__CalcChecksum( funcName, strlen( funcName ), 0 ) );
    CHECK_ERROR( nReturnValue );

    index = FindFunctionIndex( funcName );
    if ( index == -1 )
    {
      ExitWithError();
    }
  }

  return index;
}

int FindFunctionIndex( char* funcName )
{
  unsigned int funcHash;
  int i = 0, found = -1;
  ts_Symbol *prev = NULL;

  pFunctionList = &FunctionList;
  prev = pFunctionList;
  pFunctionList = pFunctionList->pNext;

  funcHash = Hash__CalcChecksum( funcName, strlen(funcName), 0 );

  if ( pFunctionList != NULL )
  {
    while( pFunctionList != NULL )
    {
      /* sanity check on hash keys */
      if ( ( funcHash == pFunctionList->ulValue ) && ( strcmp( pFunctionList->caName, funcName ) != 0 ) )
      {
        printf( "ERROR: differing function names produced identical hash values - rename one of the functions:" );
        printf( "\t%s, %s\n", pVarList->caName, funcName );
        ExitWithError();
      }

      if ( strcmp( pFunctionList->caName, funcName ) == 0 )
      {
        found = i;
      }
      else
      {
        i++;
      }
      prev = pFunctionList;
      pFunctionList = pFunctionList->pNext;
    }
  }

  pFunctionList = prev;
  return( found );
}

int CreateVariableIndex( char *varName, int tType )
{ /* only create a new variable index if one doesn't exist */
  int nReturnValue;
  int index = FindVariableIndex( varName );
  ts_Symbol *var = NULL;

  if ( index == -1 )
  {
    nReturnValue = AddVar( "CreateVariableIndex", varName, Hash__CalcChecksum( varName, strlen( varName ), 0 ) );
    CHECK_ERROR( nReturnValue );

    index = FindVariableIndex( varName );
    if ( index == -1 )
    {
      ExitWithError();
    }

    var = FindVariableNameByIndex( index );
    var->uVarType = tType;
  }

  return index;
}

int FindVariableIndex( char* varName )
{
  unsigned int varHash;
  int i = 0, found = -1;
  ts_Symbol *prev = NULL;

  pVarList = &VarList;
  prev = pVarList;
  pVarList = pVarList->pNext;

  varHash = Hash__CalcChecksum( varName, strlen(varName), 0 );

  if ( pVarList != NULL )
  {
    while( pVarList != NULL )
    {
      /* sanity check on hash keys */
      if ( ( varHash == pVarList->ulValue ) && ( strcmp( pVarList->caName, varName ) != 0 ) )
      {
        printf( "ERROR: differing variable names produced identical hash values - rename one of the varialbes:" );
        printf( "\t%s, %s\n", pVarList->caName, varName );
        ExitWithError();
      }
      
      if ( strcmp( pVarList->caName, varName ) == 0 )
      {
        found = i;
      }
      else
      {
        i++;
      }
      prev = pVarList;
      pVarList = pVarList->pNext;
    }
  }

  pVarList = prev;
  return( found );
}

int HandleIntParam( char *pParam )
{
  int nReturnVal;
  int iVal;
  char *p;

  iVal = strtol( pParam, &p, 10 );
  if ( ( iVal == 0 ) && ( p == pParam ) )
  { /* not an int - a variable */
    nReturnVal = ProcessLoad( pParam );
    CHECK_ERROR( nReturnVal );

    nReturnVal = ProcessInt( );
    CHECK_ERROR( nReturnVal );
  }
  else
  {
    nReturnVal = ProcessPushI( iVal );
    CHECK_ERROR( nReturnVal );
    free( pParam );
  }

  return iVal;
}

float HandleFloatParam( char *pParam )
{
  int nReturnVal;
  float fVal;

  fVal = ( float )atof( pParam );
  if ( ( fVal == 0.0f ) && ( strcmp( pParam, "0.0" ) != 0 ) )
  { /* not a float - a variable */
    nReturnVal = ProcessLoad( pParam );
    CHECK_ERROR( nReturnVal );

    nReturnVal = ProcessFloat( );
    CHECK_ERROR( nReturnVal );

	/* SCL - shouldn't we free pParam here? */
	/* free( pParam ); */
  }
  else
  {
    nReturnVal = ProcessPushF( fVal );
    CHECK_ERROR( nReturnVal );
    free( pParam );
  }

  return fVal;
}

void HandleStringParam( char *pParam )
{
	int nReturnVal = 0;

	/* load the value of the variable onto the op stack */
	nReturnVal = ProcessLoad( pParam );
	CHECK_ERROR( nReturnVal );

	free( pParam );

	return;
}

int HandleParameter( char *pcString )
{
  int nReturnVal;
  int index;
  ts_Symbol *symbol;
  char *pEnd = NULL;
  float fVal;

  if ( pcString == NULL )
  {
    ExitWithError();
  }

  symbol = FindSymbolByName( pcString );
  if ( symbol == NULL )
  { /* not a symbol */
    index = strtol( pcString, &pEnd, 16 );
    if ( ( ( index == 0 ) && ( pcString == pEnd ) ) || ( _strnicmp( pcString, "0x", 2 ) != 0 ) )
    { /* not a hex */
      index = strtol( pcString, &pEnd, 10 );
      if ( ( ( index == 0 ) && ( pcString == pEnd ) ) || ( strchr( pcString, '.' ) != NULL ) )
      { /* not an int */
        fVal = ( float )atof( pcString );
        if ( ( fVal == 0.0f ) && ( strcmp( pcString, "0.0" ) != 0 ) )
        { /* not a float - a variable */
            nReturnVal = ProcessLoad( pcString );
            CHECK_ERROR( nReturnVal );
        }
        else
        { /* a float */
          nReturnVal = ProcessPushF( fVal );
          CHECK_ERROR( nReturnVal );
          free( pcString );
        }
      }
      else
      { /* an integer */
        nReturnVal = ProcessPushI( index );
        CHECK_ERROR( nReturnVal );
        free( pcString );
      }
    }
    else
    { /* a hex value */
      nReturnVal = ProcessPushI( index );
      CHECK_ERROR( nReturnVal );
      free( pcString );
    }
  }
  else
  { /* a symbol lookup */
    nReturnVal = ProcessPushA( pcString );
    CHECK_ERROR( nReturnVal );
  }

  return TRUE;
}


char *ExpandString( char *dst, char *src )
{
  char *retval = dst;
  char *curr;

  if ( src == NULL )
  {
    return NULL;
  }

  curr = src;
  while( *curr != '\0' )
  {
    switch( *curr )
    {
    case '\\':
      *dst++ = '\\';
      break;
    case '"':
      *dst++ = '\\';
      break;
    case '\n':
      *dst++ = '\\';
      *dst++ = 'n';
      *curr++;
      continue;
    case '\t':
      *dst++ = '\\';
      *dst++ = 't';
      *curr++;
      continue;
    case '\r':
      *dst++ = '\\';
      *dst++ = 'r';
      *curr++;
      continue;
    case '\b':
      *dst++ = '\\';
      *dst++ = 'b';
      *curr++;
      continue;
    case '\f':
      *dst++ = '\\';
      *dst++ = 'f';
      *curr++;
      continue;
    default:
      break;
    }

    *dst++ = *curr++;
  }

  return( retval );
}




void doUnaryReduction( void )
{
  /* starting with the last opcode (assuming it's a binary operator) */
  /* check the symbol table to see if there are any labels that are  */
  /* referencing any of the operations to be collapsed.              */

  /* this code assumes that pDataList points to the last operation.  */
  ts_Symbol *stops_optimization = NULL;
  ts_Hdr *temp = NULL;
  ts_DataInt *param_first = NULL;
  ts_DataInt *p1IVal = NULL;
  ts_DataFloat *p1FVal = NULL;
  unsigned int param_first_offset;
  ts_DataInt *op = ( ts_DataInt * )pDataList;
  int myOffset = nGlobalOffset;

  if ( bDebug )
  {
    op = ( ts_DataInt * )pDataList->pPrev;
    myOffset -= sizeof( unsigned int );
  }

  if ( op->hBlock.nType != LIST_OPCODE )
  {
    ExitWithError( );
  }

  param_first = ( ts_DataInt * )op->hBlock.pPrev->pPrev;
  param_first_offset = myOffset - ( 2 * sizeof( unsigned int ) );
  if ( bDebug )
  {
    param_first = ( ts_DataInt * )param_first->hBlock.pPrev;
    param_first_offset -= sizeof( unsigned int );
  }
  if ( ( param_first->nData != CMD_PUSHI ) && ( param_first->nData != CMD_PUSHF ) )
  {
    return;
  }

  /* look for symbols within the current expression */
  stops_optimization = FindSymbolByAddress( myOffset );
  if ( stops_optimization != NULL )
  {
    return;
  }

  /* A label at param_first_offset is still OK to optimize          */

  /* if there are no referencing symbols, check that the previous 2  */
  /* operations were a push of a constant float or int.  If so, go   */
  /* ahead and perform the operation and replace it with the result. */

  if ( param_first->nData == CMD_PUSHI )
  { /* easy case - no casting required */

    p1IVal = ( ts_DataInt * )param_first->hBlock.pNext;
    if ( bDebug )
    {
      p1IVal = ( ts_DataInt * )p1IVal->hBlock.pNext;
    }

    p1FVal = ( ts_DataFloat * )p1IVal;

    switch( op->nData )
    {
    case CMD_NOT:
      p1IVal->nData = !p1IVal->nData;
      break;
    case CMD_BIT_NOT:
      p1IVal->nData = ~p1IVal->nData;
      break;
    case CMD_INT:
      /* nothing to do here */
      break;
    case CMD_FLOAT:
      p1FVal->fData = ( float )p1IVal->nData;
      break;
    default:
      ExitWithError( );
      break;
    }

    p1IVal->hBlock.nType = LIST_INTEGER;
    p1IVal->hBlock.line = op->hBlock.line;
    if ( bDebug )
    { /* update the debug information with the opcode's line index */
      ( ( ts_DataInt * )p1IVal->hBlock.pPrev )->nData = ( ( ts_DataInt * )op->hBlock.pNext )->nData;
    }

    param_first->nData = CMD_PUSHI;

  }
  else
  { /* upcast ints to floats as necessary, the perform float operations */
    p1FVal = ( ts_DataFloat * )param_first->hBlock.pNext;
    if ( bDebug )
    {
      p1FVal = ( ts_DataFloat * )p1FVal->hBlock.pNext;
    }
    p1IVal = ( ts_DataInt * )p1FVal;

    if ( p1FVal->hBlock.nType == LIST_INTEGER )
    {
      p1FVal->fData = ( float )p1IVal->nData;
      p1FVal->hBlock.nType = LIST_FLOAT;
    }

    switch( op->nData )
    {
    case CMD_INT:
      p1FVal->fData = ( float )p1IVal->nData;
      break;
    case CMD_FLOAT:
      /* nothing to do here */
      break;
    default:
      ExitWithError( );
      break;
    }

    p1FVal->hBlock.nType = LIST_FLOAT;
    p1FVal->hBlock.line = op->hBlock.line;

    param_first->nData = CMD_PUSHF;

    if ( bDebug )
    { /* update the debug information for the new opcode to the operator's debug info */
      ( ( ts_DataInt * )p1IVal->hBlock.pPrev )->nData = ( ( ts_DataInt * )op->hBlock.pNext )->nData;
    }

  }

  param_first->hBlock.nType = LIST_OPCODE;
  param_first->hBlock.line = op->hBlock.line;

  /* remove the extra opcodes */
  temp = ( ts_Hdr * )p1IVal->hBlock.pNext;
  while ( temp != NULL )
  {
    pDataList = temp->pNext;
    free( temp );
    temp = pDataList;
  }
  pDataList = ( ts_Hdr * )p1IVal;
  p1IVal->hBlock.pNext = NULL;

  /* update nGlobalOffset to reflect the removed opcodes */
  myOffset = param_first_offset + ( sizeof( unsigned int ) );
  if ( bDebug )
  {
    myOffset += INT_LEN;
  }

  nGlobalOffset = myOffset;
}

void doBinaryReduction( void )
{
  /* starting with the last opcode (assuming it's a binary operator) */
  /* check the symbol table to see if there are any labels that are  */
  /* referencing any of the operations to be collapsed.              */

  /* this code assumes that pDataList points to the last operation.  */
  ts_Symbol *stops_optimization = NULL;
  ts_Hdr *temp = NULL;
  ts_DataInt *param_first = NULL, *param_second = NULL;
  ts_DataInt *p1IVal = NULL, *p2IVal = NULL;
  ts_DataFloat *p1FVal = NULL, *p2FVal = NULL;
  unsigned int param_first_offset = nGlobalOffset, param_second_offset = nGlobalOffset;
  ts_DataInt *op = ( ts_DataInt * )pDataList;
  int myOffset = nGlobalOffset;

  if ( bDebug )
  {
    op = ( ts_DataInt * )pDataList->pPrev;
    myOffset -= sizeof( unsigned int );
  }

  if ( op->hBlock.nType != LIST_OPCODE )
  {
    ExitWithError( );
  }

  param_first = ( ts_DataInt * )op->hBlock.pPrev->pPrev;
  param_first_offset = myOffset - ( 2 * sizeof( unsigned int ) );
  if ( bDebug )
  {
    param_first = ( ts_DataInt * )param_first->hBlock.pPrev;
    param_first_offset -= sizeof( unsigned int );
  }
  if ( ( param_first->nData != CMD_PUSHI ) && ( param_first->nData != CMD_PUSHF ) )
  {
    return;
  }

  param_second = ( ts_DataInt * )param_first->hBlock.pPrev->pPrev;
  param_second_offset = param_first_offset - ( 2 * sizeof( unsigned int ) );
  if ( bDebug )
  {
    param_second = ( ts_DataInt * )param_second->hBlock.pPrev;
    param_second_offset -= ( sizeof( unsigned int ) );
  }
  if ( ( param_second->nData != CMD_PUSHI ) && ( param_second->nData != CMD_PUSHF ) )
  {
    return;
  }

  /* look for symbols within the current expression */
  stops_optimization = FindSymbolByAddress( myOffset );
  if ( stops_optimization != NULL )
  {
    return;
  }
  stops_optimization = FindSymbolByAddress( param_first_offset );
  if ( stops_optimization != NULL )
  {
    return;
  }
  /* A label at param_second_offset is still OK to optimize          */

  /* if there are no referencing symbols, check that the previous 2  */
  /* operations were a push of a constant float or int.  If so, go   */
  /* ahead and perform the operation and replace it with the result. */

  if ( ( param_first->nData == CMD_PUSHI ) && ( param_second->nData == CMD_PUSHI ) )
  { /* easy case - no casting required */

    p1IVal = ( ts_DataInt * )param_first->hBlock.pNext;
    p2IVal = ( ts_DataInt * )param_second->hBlock.pNext;
    if ( bDebug )
    {
      p1IVal = ( ts_DataInt * )p1IVal->hBlock.pNext;
      p2IVal = ( ts_DataInt * )p2IVal->hBlock.pNext;
    }

    switch( op->nData )
    {
    case CMD_ADD:
      p2IVal->nData = p2IVal->nData + p1IVal->nData;
      break;
    case CMD_SUB:
      p2IVal->nData = p2IVal->nData - p1IVal->nData;
      break;
    case CMD_MUL:
      p2IVal->nData = p2IVal->nData * p1IVal->nData;
      break;
    case CMD_DIV:
      p2IVal->nData = p2IVal->nData / p1IVal->nData;
      break;
    case CMD_MOD:
      p2IVal->nData = p2IVal->nData % p1IVal->nData;
      break;
    case CMD_BIT_AND:
      p2IVal->nData = p2IVal->nData & p1IVal->nData;
      break;
    case CMD_BIT_OR:
      p2IVal->nData = p2IVal->nData | p1IVal->nData;
      break;
    case CMD_BIT_XOR:
      p2IVal->nData = p2IVal->nData ^ p1IVal->nData;
      break;
    case CMD_AND:
      p2IVal->nData = p2IVal->nData && p1IVal->nData;
      break;
    case CMD_OR:
      p2IVal->nData = p2IVal->nData || p1IVal->nData;
      break;
    case CMD_SHL:
      p2IVal->nData = p2IVal->nData << p1IVal->nData;
      break;
    case CMD_SHR:
      p2IVal->nData = p2IVal->nData >> p1IVal->nData;
      break;
    case CMD_SHRZ:
      p2IVal->nData = ( unsigned int )p2IVal->nData >> p1IVal->nData;
      break;
    default:
      ExitWithError( );
      break;
    }

    p2IVal->hBlock.nType = LIST_INTEGER;
    p2IVal->hBlock.line = op->hBlock.line;
    if ( bDebug )
    { /* update the debug information with the opcode's line index */
      ( ( ts_DataInt * )p2IVal->hBlock.pPrev )->nData = ( ( ts_DataInt * )op->hBlock.pNext )->nData;
    }

    param_second->nData = CMD_PUSHI;

  }
  else
  { /* upcast ints to floats as necessary, the perform float operations */
    p1FVal = ( ts_DataFloat * )param_first->hBlock.pNext;
    if ( bDebug )
    {
      p1FVal = ( ts_DataFloat * )p1FVal->hBlock.pNext;
    }
    p1IVal = ( ts_DataInt * )p1FVal;

    p2FVal = ( ts_DataFloat * )param_second->hBlock.pNext;
    if ( bDebug )
    {
      p2FVal = ( ts_DataFloat * )p2FVal->hBlock.pNext;
    }
    p2IVal = ( ts_DataInt * )p2FVal;

    if ( p1FVal->hBlock.nType == LIST_INTEGER )
    {
      p1FVal->fData = ( float )p1IVal->nData;
      p1FVal->hBlock.nType = LIST_FLOAT;
    }
    if ( p2FVal->hBlock.nType == LIST_INTEGER )
    {
      p2FVal->fData = ( float )p2IVal->nData;
      p2FVal->hBlock.nType = LIST_FLOAT;
    }

    switch( op->nData )
    {
    case CMD_ADD:
      p2FVal->fData = p2FVal->fData + p1FVal->fData;
      break;
    case CMD_SUB:
      p2FVal->fData = p2FVal->fData - p1FVal->fData;
      break;
    case CMD_MUL:
      p2FVal->fData = p2FVal->fData * p1FVal->fData;
      break;
    case CMD_DIV:
      p2FVal->fData = p2FVal->fData / p1FVal->fData;
      break;
    case CMD_AND:
      p2IVal->nData = p2FVal->fData && p1FVal->fData;
      break;
    case CMD_OR:
      p2IVal->nData = p2FVal->fData || p1FVal->fData;
      break;
    default:
      ExitWithError( );
      break;
    }

    if ( ( op->nData != CMD_AND ) && ( op->nData != CMD_OR ) )
    {
      p2FVal->hBlock.nType = LIST_FLOAT;
      param_second->nData = CMD_PUSHF;
    }
    else
    {
      p2IVal->hBlock.nType = LIST_INTEGER;
      param_second->nData = CMD_PUSHI;
    }
    p2FVal->hBlock.line = op->hBlock.line;

    if ( bDebug )
    { /* update the debug information for the new opcode to the operator's debug info */
      ( ( ts_DataInt * )p2IVal->hBlock.pPrev )->nData = ( ( ts_DataInt * )op->hBlock.pNext )->nData;
    }

  }

  param_second->hBlock.nType = LIST_OPCODE;
  param_second->hBlock.line = op->hBlock.line;

  /* remove the extra opcodes */
  temp = ( ts_Hdr * )p2IVal->hBlock.pNext;
  while ( temp != NULL )
  {
    pDataList = temp->pNext;
    free( temp );
    temp = pDataList;
  }
  pDataList = ( ts_Hdr * )p2IVal;
  p2IVal->hBlock.pNext = NULL;

  /* update nGlobalOffset to reflect the removed opcodes */
  myOffset = param_second_offset + ( sizeof( unsigned int ) );
  if ( bDebug )
  {
    myOffset += INT_LEN;
  }

  nGlobalOffset = myOffset;
}



//============================================================================
