/* --------------------------------------------------
 * File    : ARGPARSE.H
 * Created : Mon Aug 31 14:56:42 1998
 * Descript: 
 * --------------------------------------------------*/
#ifndef __ARGPARSE_H
#define __ARGPARSE_H

/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Defines
 * --------------------------------------------------*/
enum
{
	ARG_NONE = 0,
	ARG_DOUBLE,
	ARG_FLOAT,
	ARG_LONG,
	ARG_ULONG,
	ARG_INT,
	ARG_UINT,
	ARG_SHORT,
	ARG_USHORT,
	ARG_STRINGGET,
	ARG_STRINGCPY,
	ARG_CHAR,
	ARG_UCHAR,
	ARG_BOOLT,
	ARG_BOOLF
} ;

typedef void (*t_ArgFill)( unsigned char *pStream, unsigned short usType, void *pFill);

typedef struct ArgParseList
{
	char *			pArg;
	t_ArgFill		ArgFunc;
	void				*pData;
	unsigned short	usFlags;
} ts_ArgParseList;


/* --------------------------------------------------
 * Exported Data
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/

int ParseArgs( int iArgC, char **ppcArgList, ts_ArgParseList *pArgs, char ***pppcRetList);
void ParseFile( char *pFile, ts_ArgParseList *pArgs);

#endif __ARGPARSE_H