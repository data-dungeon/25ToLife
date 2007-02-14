/////////////////////////////////////////////////////////////////////////////
// List of muzzle flash types
/////////////////////////////////////////////////////////////////////////////
#ifndef MUZZLELIB_H
#define MUZZLELIB_H

#include "Components/weapons/volib.h"
#include "weapons/muzzle.h"
#include "EngineHelper/Singleton.h"

// This class needs to be subclassed in order to create a MuzzleFlash
class MuzzleFlashCreator : public VarObjCreator<MuzzleFlash>
{
public:
	// Hello
	MuzzleFlashCreator() :
		VarObjCreator<MuzzleFlash>("muzzleFlash") {}

	// You must provide this for the type
	MuzzleFlash *CreateInstance(const VarObjProperties<MuzzleFlash> &properties)	
		{ return new MuzzleFlash((const MuzzleFlashProperties &)properties); }

protected:
	// Subclass this to provide new
	MuzzleFlashProperties *NewProperties(int index)
		{ return new MuzzleFlashProperties(*this, index); }
};

// The manager of acive MuzzleFlashs
class MuzzleFlashLibrary : public VarObjLibrary<MuzzleFlash>
{
public:
	// Initialize all the known MuzzleFlashs
	MuzzleFlashLibrary() {}
	~MuzzleFlashLibrary() {}

protected:
	// What is the path
	const char *Path()
		{ return "weapons\\"; }

	// What is the context name
	const char *Context()
		{ return "muzzle"; }
};

// MuzzleFlash singleton
DefineSingleton(MuzzleFlashLibrary)
#define g_muzzleFlashLib GetSingleton(MuzzleFlashLibrary)

#endif
