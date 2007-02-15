// The definitions in this file provide portability support to GameCube apps
// compiled with the CodeWarrior compilers for "standard" type functions
// not supplied by the CW compiler suite
#ifndef __GCN_CW_PORT__
#define __GCN_CW_PORT__

/*******************************************************************************
	INCLUDE
*******************************************************************************/
#include "Layers/Assert/Assert.h"
#include <stdlib.h>

#ifdef __cplusplus
extern	"C"	{
#endif	/* __cplusplus */

/*******************************************************************************
	DEFINE
*******************************************************************************/
#define stricmp		_stricmp
#define strcmpi		_stricmp
#define strnicmp		_strnicmp
#define strupr			_strupr
#define strlwr			_strlwr

/*******************************************************************************
	TYPEDEF
*******************************************************************************/

/*******************************************************************************
	EXTERN
*******************************************************************************/

/*******************************************************************************
	FUNCTION
*******************************************************************************/
int				_stricmp( const char *string1, const char *string2 );
int				_strnicmp( const char *string1, const char *string2, size_t count );
char *			_strupr( char *string);
char *			_strlwr( char *string);
char *			_strset( char *string, int c);

template <class T> inline T & ByteSwap( T &val)
{

	T			in,
				out;
	
	unsigned char	*pIn,
						*pOut;

	in = val;
	
	pIn = (unsigned char *)&in;
	pOut = (unsigned char *)&out;

	switch( sizeof(T))
	{
		case 2:
			pOut[0] = pIn[1];
			pOut[1] = pIn[0];
			break;

		case 4:
			pOut[0] = pIn[3];
			pOut[1] = pIn[2];
			pOut[2] = pIn[1];
			pOut[3] = pIn[0];
		break;

		default:
			ASSERT( 0);
	}

	val = out;

	return( val);
}

template <class T> inline T * ByteSwap( T * &ptr)
{

	T			*in,
				*out;
	
	unsigned char	*pIn,
						*pOut;

	in = ptr;
	
	pIn = (unsigned char *)&in;
	pOut = (unsigned char *)&out;

	pOut[0] = pIn[3];
	pOut[1] = pIn[2];
	pOut[2] = pIn[1];
	pOut[3] = pIn[0];

	ptr = out;

	return( out);
}

#ifdef __cplusplus
}
#endif	/* __cplusplus */

#endif	/* __GCN_CW_PORT__ */
