/////////////////////////////////////////////////////////////////////////////
// List of ejecting brass types
/////////////////////////////////////////////////////////////////////////////
#ifndef EBRASSLIB_H
#define EBRASSLIB_H

#include "Components/weapons/volib.h"
#include "weapons/ebrass.h"
#include "EngineHelper/Singleton.h"

// This class needs to be subclassed in order to create a EjectingBrass
class EjectingBrassCreator : public VarObjCreator<EjectingBrass>
{
public:
	// Hello
	EjectingBrassCreator() :
		VarObjCreator<EjectingBrass>("ejectingBrass") {}

	// You must provide this for the type
	EjectingBrass *CreateInstance(const VarObjProperties<EjectingBrass> &properties)
		{ return new EjectingBrass((const EjectingBrassProperties &)properties); }

protected:
	// Subclass this to provide new
	EjectingBrassProperties *NewProperties(int index)
		{ return new EjectingBrassProperties(*this, index); }
};

// The manager of acive EjectingBrasss
class EjectingBrassLibrary : public VarObjLibrary<EjectingBrass>
{
public:
	// Initialize all the known EjectingBrasss
	EjectingBrassLibrary() {}
	~EjectingBrassLibrary() {}

protected:
	// What is the path
	const char *Path()
		{ return "weapons\\"; }

	// What is the context name
	const char *Context()
		{ return "brass"; }
};

// EjectingBrass singleton
DefineSingleton(EjectingBrassLibrary)
#define g_ejectingBrassLib GetSingleton(EjectingBrassLibrary)

#endif
