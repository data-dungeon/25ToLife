//===================================================================
//  File:  WatchVar.h
//  Description:
//     Declaration of a Variable class
//===================================================================

#ifndef __VAR_H__
#define __VAR_H__

#include "WatchInfo.h"

class WatchVar
{
public:
	WatchVar(VAR_INFO&);
	~WatchVar() {}

	static WatchVar* GetInstance(VAR_INFO&);

	void* GetPtr() {return varinfo.address;}
	int Event() {return varinfo.event;}
	void* EventDest() {return varinfo.eventDest;}
	char* Name() {return varinfo.name;}
	VAR_VAL GetVal() {return varinfo.value;}
	void SetVal(VAR_VAL val) {varinfo.value = val;}
	int Type() {return varinfo.type;}

	// These must be overridden;
	virtual char* String() = 0;
	virtual void  String(const char*) = 0;
	virtual float Percent() = 0;
	virtual void Update(float) = 0;
	virtual void Update(const char *) {}

protected:
	VAR_INFO varinfo;
	float range;

	char stringVal[128];
};

class WatchVarReal : public WatchVar
{
public:
	WatchVarReal(VAR_INFO&);

	char* String();
	void  String(const char*);
	float Percent() {return (varinfo.value.rval - varinfo.min) / range;}
	void Update(float);
};

class WatchVarInt : public WatchVar
{
public:
	WatchVarInt(VAR_INFO&);

	char* String();
	void  String(const char*);
	float Percent() {return ((float)varinfo.value.ival - varinfo.min) / range;}
	void Update(float);
};

class WatchVarLog : public WatchVar
{
public:
	WatchVarLog(VAR_INFO&);

	char* String();
	void  String(const char*);
	float Percent();
	void Update(float);

private:
	float Power(float, int);
	bool realMode;
	float base;
	int minExp;
};

class WatchVarString : public WatchVar
{
public:
	WatchVarString(VAR_INFO&);

	char* String();
	void  String(const char*);
	float Percent();
	void Update(float);
	void Update(const char *);
};


#endif // __VAR_H__
