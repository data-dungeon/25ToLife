#ifndef __Helper_H
#define __Helper_H

#ifdef unix
#include <alloca.h>     // for alloca()
#include <wchar.h>      // for wcslen
#else
#include <malloc.h>     // for alloca()
#endif
#include <xsi_value.h>
#include <xsi_status.h>
#include <xsi_decl.h>
#include <SIBCUtil.h>

inline void XSIA2WHelper( wchar_t* out_wcs, const char* in_sz, int in_cch = -1 )
{
	if ( out_wcs != NULL && 0 != in_cch )
	{
		out_wcs[0] = L'\0' ;

		if ( in_sz != NULL )
		{
			// Right now, we can't really test the return value because mbstowcs() is
			// mapped to strcpy() on the Irix o32 build. When this build is not done
			// anymore, the != 0 test should be updated.

			size_t l_iLen = 0;
			l_iLen = ::mbstowcs( out_wcs, in_sz, ( in_cch < 0 ) ? ::strlen(in_sz) + 1 : (size_t) in_cch ) ;
			assert( (int)l_iLen != -1 );

        		if ( in_cch > 0 ) { out_wcs[ in_cch - 1 ] = L'\0'; }
		}
	}
}

inline void XSIW2AHelper( char* out_sz, const wchar_t* in_wcs, int in_cch = -1 )
{
	if ( out_sz != NULL && 0 != in_cch )
	{
		out_sz[0] = '\0' ;

		if ( in_wcs != NULL )
		{
			// Right now, we can't really test the return value because wcstombs() is
			// mapped to strcpy() on the Irix o32 build. When this build is not done
			// anymore, the != 0 test should be updated.

			size_t l_iLen = 0;
			l_iLen = ::wcstombs( out_sz, in_wcs, ( in_cch < 0 ) ? ::wcslen(in_wcs) + 1 : (size_t) in_cch ) ;
			assert( (int)l_iLen != -1 );

        		if ( in_cch > 0 ) { out_sz[ in_cch - 1 ] = '\0'; }
		}
	}
}


#define XSIA2W(out_wcs, in_sz) \
	if (NULL == (const char *)(in_sz)) \
		*(out_wcs) = NULL; \
	else \
	{ \
		*(out_wcs) = (wchar_t*)alloca((strlen((in_sz)) + 1) * sizeof(wchar_t)); \
		XSIA2WHelper(*(out_wcs), (in_sz)); \
	}

#define XSIW2A(out_sz, in_wcs) \
	if (NULL == (const wchar_t *)(in_wcs)) \
		*(out_sz) = NULL; \
	else \
	{ \
		*(out_sz) = (char*)alloca(wcslen((in_wcs)) + 1); \
		XSIW2AHelper(*(out_sz), (in_wcs)); \
	}



#endif
