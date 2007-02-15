#ifndef _RENDERASSERT_H
#define _RENDERASSERT_H

#ifdef _DEBUG
	extern int renderassert( const char* expr, const char* file, int line );
	#define RENDER_ASSERT( expr ) ( (void) ( (expr) || renderassert( #expr, __FILE__, __LINE__ ) ) )
#else
	#define RENDER_ASSERT( expr ) ( (void) 0 )
#endif

#endif //_RENDERASSERT_H
