#include "pch.h"
#include "callback.h"

void MemberCallBack( void *pObject, MEMBERCALLBACK Function, PARAM Param )
{
	UnknownClass *pThis = (UnknownClass *) pObject;

	if( pThis && Function )
		(*pThis.*Function)( Param );

	return;
}

void MemberCallBackVoid( void *pObject, MEMBERCALLBACKVOID Function )
{
	UnknownClass *pThis = (UnknownClass *) pObject;

	if( pThis && Function )
		(*pThis.*Function)();

	return;
}

void StaticCallBack( STATICCALLBACK Function, PARAM Param )
{
	if( Function )
		(*Function)(Param);

	return;
}

void StaticCallBackVoid( STATICCALLBACKVOID Function )
{
	if( Function )
		(*Function)();

	return;
}