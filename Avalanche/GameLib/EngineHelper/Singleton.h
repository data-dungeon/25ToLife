/*
**
**  File:   Singleton.h
**  Date:   October 8, 2003
**  By:     Bryant Collard
**  Desc:   A template and some defines to aid in the creation of singletons.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/EngineHelper/Singleton.h $
**  $Revision: #7 $
**  $DateTime: 2005/02/04 19:17:45 $
**    $Author: Rob $
**
*/

//
// If, for example, you want to make a singleton of class CFoo, do the
// following:
//
// At the top of the appropriate .cpp file (e.g. foo.cpp) add:
//
//    DeclareSingleton(CFoo);
//
// If you want to dynamically create the singleton and the constructor takes
// no arguments, then during the owner's initialization:
//
//    NewSingleton(CFoo);
//
// otherwise (where inst is the instance to be the singleton):
//
//    SetSingleton(CFoo, inst);
//
// If the singleton was dynamically created, then during the owner's shutdown:
//
//    DeleteSingleton(CFoo);
//
// otherwise:
//
//    ClearSingleton(CFoo);
//
// You can access the singleton by:
//
//    GetSingleton(CFoo).SomeFunction(...);
//
// To make life easier, you probably want to add something like the following
// to the appropriate header (e.g. foo.h):
//
//    #define g_foo GetSingleton(CFoo)
//
// Then you can access the singleton by:
//
//    g_foo.SomeFunction(...);
//

#pragma once

#ifndef SINGLETON_H
#define SINGLETON_H

#include "Layers/Assert/Assert.h"

// Use a templated class for a cleaner, more robust solution.
#if 1

template<class T> class CSingleton
{
  public:
	static void Initialize(void)
	{
		ASSERT(s_instance == NULL);
		s_instance = new T;
	}

	static void Initialize(T* i_instance)
	{
		ASSERT(s_instance == NULL);
		s_instance = i_instance;
	}

	static void Terminate(void)
	{
		delete s_instance;
		s_instance = NULL;
	}

	static void Clear(void)
	{
		s_instance = NULL;
	}

	static T &Get(void)
	{
		ASSERT(s_instance);
		return(*s_instance);
	}

	static bool IsSet(void)
	{
		return(s_instance != NULL);
	}

	static bool IsEqual(T* i_instance)
	{
		return(s_instance == i_instance);
	}

  private:
	static T* s_instance;
};

#define DefineSingleton(T)
#define DeclareSingleton(T) T* CSingleton<T>::s_instance = NULL
#define NewSingleton(T) CSingleton<T>::Initialize()
#define SetSingleton(T, i) CSingleton<T>::Initialize(i)
#define DeleteSingleton(T) CSingleton<T>::Terminate()
#define ClearSingleton(T) CSingleton<T>::Clear()
#define GetSingleton(T) CSingleton<T>::Get()
#define ValidSingleton(T) CSingleton<T>::IsSet()
#define IsSingleton(T, i) CSingleton<T>::IsEqual(i)

// Use a macro class so singletons can be used in DLLs.
#else

#define DefineSingleton(T)                                                     \
	namespace N_##T##_Singleton                                                 \
	{                                                                           \
		extern T* g_instance;                                                    \
                                                                               \
		inline void Initialize(T* i_instance)                                    \
		{                                                                        \
			ASSERT(g_instance == NULL);                                           \
			g_instance = i_instance;                                              \
		}                                                                        \
                                                                               \
		inline void Terminate(void)                                              \
		{                                                                        \
			delete g_instance;                                                    \
			g_instance = NULL;                                                    \
		}                                                                        \
                                                                               \
		inline void Clear(void)                                                  \
		{                                                                        \
			g_instance = NULL;                                                    \
		}                                                                        \
                                                                               \
		inline T &Get(void)                                                      \
		{                                                                        \
			ASSERT(g_instance);                                                   \
			return(*g_instance);                                                  \
		}                                                                        \
                                                                               \
		inline bool IsSet(void)                                                  \
		{                                                                        \
			return(g_instance != NULL);                                           \
		}                                                                        \
                                                                               \
		inline bool IsEqual(T* i_instance)                                       \
		{                                                                        \
			return(g_instance == i_instance);                                     \
		}                                                                        \
	}

#define DeclareSingleton(T) T* N_##T##_Singleton::g_instance = NULL
#define NewSingleton(T)                                                        \
	do                                                                          \
	{                                                                           \
		ASSERT(!N_##T##_Singleton::IsSet());                                     \
		N_##T##_Singleton::Initialize(new T);                                    \
	} while(0)
#define SetSingleton(T, i) N_##T##_Singleton::Initialize(i)
#define DeleteSingleton(T) N_##T##_Singleton::Terminate()
#define ClearSingleton(T) N_##T##_Singleton::Clear()
#define GetSingleton(T) N_##T##_Singleton::Get()
#define ValidSingleton(T) N_##T##_Singleton::IsSet()
#define IsSingleton(T, i) N_##T##_Singleton::IsEqual(i)

#endif

#endif // SINGLETON_H
