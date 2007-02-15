/*
**
**  File:   PackBuffer.h
**  Date:   April 21, 2005
**  By:     Bryant Collard
**  Desc:   A couple of templated functions that pack and unpack objects to and
**          from a buffer. Basically a memcpy for little things.
**
**  Copyright (c) 2005, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/EngineHelper/PackBuffer.h $
**  $Revision: #2 $
**  $DateTime: 2005/04/21 14:24:56 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef PACK_BUFFER_H
#define PACK_BUFFER_H

#include <string.h>

namespace PackBuffer
{
	// General pack function.
	template<class Type>
	inline void* Pack(void* i_entry, Type i_value)
	{
		char* dst = (char*)i_entry;
		char* src = (char*)&i_value;
		for (int i = 0; i < sizeof(Type); i++)
			*(dst++) = *(src++);
		return((void*)dst);
	}

	// General unpack function.
	template<class Type>
	inline void* Unpack(void* i_entry, Type &o_value)
	{
		char* dst = (char*)&o_value;
		char* src = (char*)i_entry;
		for (int i = 0; i < sizeof(Type); i++)
			*(dst++) = *(src++);
		return((void*)src);
	}

	// Specialized string pack function.
	inline void* Pack(void* i_entry, const char* i_value)
	{
		char* dst = (char*)i_entry;
		while(*i_value != '\0')
			*(dst++) = *(i_value++);
		*(dst++) = '\0';
		return((void*)dst);
	}

	// Specialized string pack function.
	inline void* Pack(void* i_entry, char* i_value)
	{
		char* dst = (char*)i_entry;
		while(*i_value != '\0')
			*(dst++) = *(i_value++);
		*(dst++) = '\0';
		return((void*)dst);
	}

	// Specialized unpack a string to a character pointer function. The pointer
	// points to the string in the buffer.
	inline void* Unpack(void* i_entry, const char* &o_value)
	{
		o_value = (char*)i_entry;
		return((void*)(o_value + strlen(o_value) + 1));
	}

	// Specialized unpack a string to a character pointer function. The pointer
	// points to the string in the buffer.
	inline void* Unpack(void* i_entry, char* &o_value)
	{
		o_value = (char*)i_entry;
		return((void*)(o_value + strlen(o_value) + 1));
	}

	// Skip function.
	inline void* Skip(void* i_entry, int i_size)
	{
		return((void*)((char*)i_entry + i_size));
	}

	// String skip function.
	inline void* SkipString(void* i_entry)
	{
		char* src = (char*)i_entry;
		return((void*)(src + strlen(src) + 1));
	}
};

#endif // PACK_BUFFER_H
