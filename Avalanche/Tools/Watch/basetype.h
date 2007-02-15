#ifndef BASETYPE_H
#define BASETYPE_H

// Integrate the av math types
//#include <p_BaseType.h>
//#include <m_struct.h>
typedef float real;

#ifdef WIN32
#ifndef CDECL
#define CDECL __cdecl
#endif
#else
#define CDECL
#endif

#endif
