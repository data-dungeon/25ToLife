#ifndef __CALLBACK_H__
#define __CALLBACK_H__

//==================================================================================================
// error C2440: 'initializing' : cannot convert from '' to 'void (__thiscall UnknownClass::*)(void)'
// None of the functions with this name in scope match the target type
//
// If you see this message, that means you are trying to implement a non-static member function
// callback on a class that has multiple inheritance.  Visual C++ does not allow it.  Sorry.
//==================================================================================================

typedef void * PARAM;

class UnknownClass{};

typedef void (*STATICCALLBACK)( PARAM Param );
typedef void (*STATICCALLBACKVOID)( void );
typedef void (UnknownClass::*MEMBERCALLBACK)( PARAM Param );
typedef void (UnknownClass::*MEMBERCALLBACKVOID)( void );

void MemberCallBack( void *pObject, MEMBERCALLBACK Function, PARAM Param );
void MemberCallBackVoid( void *pObject, MEMBERCALLBACKVOID Function );
void StaticCallBack( STATICCALLBACK Function, PARAM Param );
void StaticCallBackVoid( STATICCALLBACKVOID Function );

#endif // __CALLBACK_H__