#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <windows.h>

void Debugf( char *fmt, ... );

#ifdef _DEBUG
#define DEBUGF Debugf
#else
#define DEBUGF
#endif // _DEBUG

#endif // _DEBUG_H_