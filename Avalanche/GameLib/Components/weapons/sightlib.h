/////////////////////////////////////////////////////////////////////////////
// Manage a list of aim sight types with a fast way of instantiating them.
/////////////////////////////////////////////////////////////////////////////
#ifndef SIGHTLIB_H
#define SIGHTLIB_H

#include "Components/weapons/volib.h"
#include "Components/weapons/aimsight.h"

// This class needs to be subclassed in order to create a AimSight
class AimSightCreator : public VarObjCreator<AimSight>
{
public:
	// Hello
	AimSightCreator(const char *type) :
		VarObjCreator<AimSight>(type) {}

	// You must provide this for the type
	virtual AimSight *CreateInstance(const VarObjProperties<AimSight> &properties) = 0;

protected:
	// Subclass this to provide new
	virtual AimSightProperties *NewProperties(int index)
		{ return new AimSightProperties(*this, index); }
};

// The manager of acive AimSights
class AimSightLibrary : public VarObjLibrary<AimSight>
{
public:
	// Initialize all the known AimSights
	AimSightLibrary() {}
	~AimSightLibrary() {}

protected:
	// What is the path
	const char *Path()
		{ return "weapons\\"; }

	// What is the context name
	const char *Context()
		{ return "sight"; }
};

// AimSight singleton
extern AimSightLibrary *g_aimSightLibPtr;
inline AimSightLibrary &SafePtr(AimSightLibrary *ptr)
{
	ASSERT(ptr);
	return *ptr;
}
#define g_aimSightLib SafePtr(g_aimSightLibPtr)

#endif
