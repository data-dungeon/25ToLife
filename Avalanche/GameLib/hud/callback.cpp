/******************* includes ****************************/

#include "hud/HudPCH.h"
#include "hud/callback.h"

class DummyClass
{
	public:
		DummyClass();
		virtual void CallBackFunction( void );

	public:
		int m_Counter;
};

DummyClass::DummyClass()
{
	// If the following line of code generates an error, then the compiler version
	// you are using doesn't support C++ member callbacks (CodeWarrior?)

#if !defined(CW) && !defined(PS2)
	MemberCallBackVoid( this, (MEMBERCALLBACKVOID) &DummyClass::CallBackFunction );
#endif
}

void DummyClass::CallBackFunction()
{
}

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