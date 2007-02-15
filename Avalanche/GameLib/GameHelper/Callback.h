/*
**
**  File:   Callback.h
**  Date:   February 16, 2005
**  By:     Bryant Collard
**  Desc:   A utility allowing member functions to be used as callbacks.
**
**  Copyright (c) 2002, Avalanche Software Inc.
**  All Rights Reserved.
**
*/
 
#pragma once

#ifndef CALLBACK_H
#define CALLBACK_H

// Example:
#if 0
class CService
{
  public:
	CService() : d_callback(NULL) {}
	void SetCallback(CCallbackBase* i_callback) {d_callbackPtr = i_callback;}
	void Action(void);

  private:
	CCallbackBase* d_callbackPtr;
};

class CUser
{
  public:
	Setup(void);

  private:
	void Function(void) {}
	CCallback<CUser> d_callbackCntr;
	CService d_service;
};

void CUser::Setup(void)
{
	d_service.SetCallback(d_callbackCntr.Set(this, &CUser::Function));
}

void CService::Action(void)
{
	if (d_callbackPtr)
		d_callbackPtr->Call();  // CUser::Function() gets called.
}

#endif
 
///////////////////////////// 0 Argument Callbacks /////////////////////////////

class CCallbackBase
{
  public:
	virtual void Call(void) = 0;
};
 
template<class Type>
class CCallback : public CCallbackBase
{
  public:
	typedef void (Type::*Function)(void);
	CCallback()
		{Set((Type*)NULL, (Function)NULL);}
	CCallback(Type* i_object, Function i_function)
		{Set(i_object, i_function);}
	CCallbackBase* Set(Type* i_object, Function i_function)
		{d_object = i_object; d_function = i_function; return(this);}
	virtual void Call(void)
		{
			if ((d_object != NULL) && (d_function != (Function)NULL))
				(d_object->*d_function)();
		}

  private:
	Type* d_object;
	Function d_function;
};
 
///////////////////////////// 1 Argument Callbacks /////////////////////////////

template<class Arg1>
class CCallbackBase_1
{
  public:
	virtual void Call(Arg1 i_arg1) = 0;
};
 
template<class Type, class Arg1>
class CCallback_1 : public CCallbackBase_1<Arg1>
{
  public:
	typedef void (Type::*Function)(Arg1 i_arg1);
	CCallback_1()
		{Set((Type*)NULL, (Function)NULL);}
	CCallback_1(Type* i_object, Function i_function)
		{Set(i_object, i_function);}
	CCallbackBase_1<Arg1>* Set(Type* i_object, Function i_function)
		{d_object = i_object; d_function = i_function; return(this);}
	virtual void Call(Arg1 i_arg1)
		{
			if ((d_object != NULL) && (d_function != (Function)NULL))
				(d_object->*d_function)(i_arg1);
		}

  private:
	Type* d_object;
	Function d_function;
};
 
///////////////////////////// 2 Argument Callbacks /////////////////////////////

template<class Arg1, class Arg2>
class CCallbackBase_2
{
  public:
	virtual void Call(Arg1 i_arg1, Arg2 i_arg2) = 0;
};
 
template<class Type, class Arg1, class Arg2>
class CCallback_2 : public CCallbackBase_2<Arg1, Arg2>
{
  public:
	typedef void (Type::*Function)(Arg1 i_arg1, Arg2 i_arg2);
	CCallback_2()
		{Set((Type*)NULL, (Function)NULL);}
	CCallback_2(Type* i_object, Function i_function)
		{Set(i_object, i_function);}
	CCallbackBase_2<Arg1, Arg2>* Set(Type* i_object, Function i_function)
		{d_object = i_object; d_function = i_function; return(this);}
	virtual void Call(Arg1 i_arg1, Arg2 i_arg2)
		{
			if ((d_object != NULL) && (d_function != (Function)NULL))
				(d_object->*d_function)(i_arg1, i_arg2);
		}

  private:
	Type* d_object;
	Function d_function;
};
 
///////////////////////////// 3 Argument Callbacks /////////////////////////////

template<class Arg1, class Arg2, class Arg3>
class CCallbackBase_3
{
  public:
	virtual void Call(Arg1 i_arg1, Arg2 i_arg2, Arg3 i_arg3) = 0;
};
 
template<class Type, class Arg1, class Arg2, class Arg3>
class CCallback_3 : public CCallbackBase_3<Arg1, Arg2, Arg3>
{
  public:
	typedef void (Type::*Function)(Arg1 i_arg1, Arg2 i_arg2, Arg3 i_arg3);
	CCallback_3()
		{Set((Type*)NULL, (Function)NULL);}
	CCallback_3(Type* i_object, Function i_function)
		{Set(i_object, i_function);}
	CCallbackBase_3<Arg1, Arg2, Arg3>* Set(Type* i_object, Function i_function)
		{d_object = i_object; d_function = i_function; return(this);}
	virtual void Call(Arg1 i_arg1, Arg2 i_arg2, Arg3 i_arg3)
		{
			if ((d_object != NULL) && (d_function != (Function)NULL))
				(d_object->*d_function)(i_arg1, i_arg2, i_arg3);
		}

  private:
	Type* d_object;
	Function d_function;
};
 
///////////////////////////// 4 Argument Callbacks /////////////////////////////

template<class Arg1, class Arg2, class Arg3, class Arg4>
class CCallbackBase_4
{
  public:
	virtual void Call(Arg1 i_arg1, Arg2 i_arg2, Arg3 i_arg3) = 0;
};
 
template<class Type, class Arg1, class Arg2, class Arg3, class Arg4>
class CCallback_4 : public CCallbackBase_4<Arg1, Arg2, Arg3, Arg4>
{
  public:
	typedef void (Type::*Function)(Arg1 i_arg1, Arg2 i_arg2, Arg3 i_arg3, Arg4 i_arg4);
	CCallback_4()
		{Set((Type*)NULL, (Function)NULL);}
	CCallback_4(Type* i_object, Function i_function)
		{Set(i_object, i_function);}
	CCallbackBase_4<Arg1, Arg2, Arg3, Arg4>* Set(Type* i_object, Function i_function)
		{d_object = i_object; d_function = i_function; return(this);}
	virtual void Call(Arg1 i_arg1, Arg2 i_arg2, Arg3 i_arg3, Arg4 i_arg4)
		{
			if ((d_object != NULL) && (d_function != (Function)NULL))
				(d_object->*d_function)(i_arg1, i_arg2, i_arg3, i_arg4);
		}

  private:
	Type* d_object;
	Function d_function;
};
 
#endif // CALLBACK_H
