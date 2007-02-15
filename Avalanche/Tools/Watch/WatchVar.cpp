//===================================================================
//  File:  WatchVar.cpp
//  Description:
//     Implementation of a Variable class that handles all variable
//     specific stuff in the watch program.
//===================================================================

#include "stdafx.h"
#include "watch.h"
#include "WatchVar.h"
#include "NumberRoot.h"

//===================================================================
//   WatchVar
//===================================================================
WatchVar* WatchVar::GetInstance(VAR_INFO& vi)
{
	switch (vi.type)
	{
		case VARTYPE_INT:
			return (new WatchVarInt(vi));
		case VARTYPE_REAL:
			return new WatchVarReal(vi);
		case VARTYPE_LOG:
			return new WatchVarLog(vi);
		case VARTYPE_STRING:
			return new WatchVarString(vi);
	}
	return 0;
}


WatchVar::WatchVar(VAR_INFO& vi)
{
	varinfo = vi;
	range = vi.max - vi.min;
}

//===================================================================
//   WatchVarReal
//===================================================================
WatchVarReal::WatchVarReal(VAR_INFO& vi)
	: WatchVar(vi)
{
//	ASSERT(vi.value.rval <= vi.max && vi.value.rval >= vi.min);
}

char* WatchVarReal::String()
{
	ASSERT(varinfo.type == VARTYPE_REAL);
	sprintf(stringVal, "%f", varinfo.value.rval);
	return stringVal;
}

void WatchVarReal::String(const char *str)
{
	ASSERT(varinfo.type == VARTYPE_REAL);
	varinfo.value.rval = (float)atof(str);
}

void WatchVarReal::Update(float p)
{
	varinfo.value.rval = varinfo.min + range*p;
}

//===================================================================
//   WatchVarInt
//===================================================================
WatchVarInt::WatchVarInt(VAR_INFO& vi)
	: WatchVar(vi)
{
//	ASSERT(vi.value.ival <= vi.max && vi.value.ival >= vi.min);
}

char* WatchVarInt::String()
{
	ASSERT(varinfo.type == VARTYPE_INT);
	sprintf(stringVal, "%d", varinfo.value.ival);
	return stringVal;
}

void WatchVarInt::String(const char *str)
{
	ASSERT(varinfo.type == VARTYPE_INT);
	varinfo.value.ival = atoi(str);
}

void WatchVarInt::Update(float p)
{
	varinfo.value.ival = int(varinfo.min + range*p);
}

//===================================================================
//   WatchVarLog
//===================================================================

#define LOG_RANGE  100   // Must be an even number.

WatchVarLog::WatchVarLog(VAR_INFO& vi)
	: WatchVar(vi)
{
	realMode = true;

	if ((vi.min <= (real)0) || (vi.max <= vi.min))
		return;

	bool imag;
	if (!CNumberRoot::Evaluate(vi.max / vi.min, LOG_RANGE, base, imag))
		 return;
	minExp = -(LOG_RANGE >> 1);
	varinfo.min = Power(base, minExp);
	float prod = vi.min * vi.max;
	float newBase, newMin;
	if (prod >= 1.0f)
	{
		while (true)
		{
			if (!CNumberRoot::Evaluate(prod, 2 * (minExp + 1) + LOG_RANGE,
					newBase, imag))
				return;
			newMin = Power(newBase, minExp + 1);
			if (newMin > vi.min)
				break;
			base = newBase;
			varinfo.min = newMin;
			minExp++;
		}
	}
	else
	{
		while (true)
		{
			if (!CNumberRoot::Evaluate(prod, 2 * (minExp - 1) + LOG_RANGE,
					newBase, imag))
				return;
			newMin = Power(newBase, minExp - 1);
			if (newMin > vi.min)
				break;
			base = newBase;
			varinfo.min = newMin;
			minExp--;
		}
	}

	varinfo.max = Power(base, minExp + LOG_RANGE);
	realMode = false;
}

char* WatchVarLog::String()
{
	ASSERT(varinfo.type == VARTYPE_LOG);
	sprintf(stringVal, "%f", varinfo.value.rval);
	return stringVal;
}

void WatchVarLog::String(const char *str)
{
	ASSERT(varinfo.type == VARTYPE_LOG);
	varinfo.value.rval = (float)atof(str);
}

float WatchVarLog::Percent()
{
	if (realMode)
		return((varinfo.value.rval - varinfo.min) / range);

	if (varinfo.value.rval <= varinfo.min)
		return(0.0f);

	if (varinfo.value.rval >= varinfo.max)
		return(1.0f);

	int expo = 0;
	float val = 1.0f;
	if (varinfo.value.rval >= 1.0f)
	{
		while (varinfo.value.rval > val)
		{
			val *= base;
			expo++;
		}
	}
	else
	{
		float target = 1.0f / varinfo.value.rval;
		while (target > val)
		{
			val *= base;
			expo--;
		}
	}

	return((float)(expo - minExp) / (float)LOG_RANGE);
}

void WatchVarLog::Update(float p)
{
	if (realMode)
		varinfo.value.rval = varinfo.min + range*p;
	else
	{
		int expo = (int)(p * (float)LOG_RANGE + 0.5f);
		if (expo < 0)
			expo = 0;
		if (expo > LOG_RANGE)
			expo = LOG_RANGE;
		varinfo.value.rval = Power(base, expo + minExp);
	}
}

float WatchVarLog::Power(float i_base, int i_expo)
{
	float val = 1.0f;
	if (i_expo < 0)
	{
		for (int i = 0; i > i_expo; i--)
			val *= i_base;
		val = 1.0f / val;
	}
	else if (i_expo > 0)
	{
		for (int i = 0; i < i_expo; i++)
			val *= i_base;
	}
	return(val);
}

//===================================================================
//   WatchVarInt
//===================================================================
WatchVarString::WatchVarString(VAR_INFO& vi)
	: WatchVar(vi)
{
//	ASSERT(vi.value.ival <= vi.max && vi.value.ival >= vi.min);
}

char* WatchVarString::String()
{
	ASSERT(varinfo.type == VARTYPE_STRING);
	return varinfo.value.strval;
}

void WatchVarString::String(const char *str)
{
	ASSERT(varinfo.type == VARTYPE_STRING);
	strncpy(varinfo.value.strval, str, sizeof(varinfo.value.strval) - 1);
	varinfo.value.strval[sizeof(varinfo.value.strval) - 1] = '\0';
}

float WatchVarString::Percent()
{
	return 0.0f;
}

void WatchVarString::Update(float p)
{
}

void WatchVarString::Update(const char *str)
{
	strncpy(varinfo.value.strval, str, sizeof(varinfo.value.strval) - 1);
	varinfo.value.strval[sizeof(varinfo.value.strval) - 1] = '\0';
}
