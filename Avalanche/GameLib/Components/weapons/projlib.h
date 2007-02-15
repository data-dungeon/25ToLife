/////////////////////////////////////////////////////////////////////////////
// Manage a list of projectile types with a fast way of instantiating them.
// Please note, at this point in the data desingn a projectile is NOT AN
// ACTOR, rather a description of functionality that may eventually spawn
// actors.
/////////////////////////////////////////////////////////////////////////////
#ifndef PROJLIB_H
#define PROJLIB_H

#include "Components/weapons/volib.h"
#include "Components/weapons/projectile.h"

// This class needs to be subclassed in order to create a projectile
class ProjectileCreator : public VarObjCreator<Projectile>
{
public:
	// Hello
	ProjectileCreator(const char *type) :
		VarObjCreator<Projectile>(type) {}

	// You must provide this for the type
	virtual Projectile *CreateInstance(const VarObjProperties<Projectile> &properties) = 0;

protected:
	// Subclass this to provide new
	virtual ProjectileProperties *NewProperties(int index) = 0;
};

// The manager of acive projectiles
class ProjectileLibrary : public VarObjLibrary<Projectile>
{
public:
	// Initialize all the known projectiles
	ProjectileLibrary() {}
	~ProjectileLibrary() {}

protected:
	// What is the path
	const char *Path()
		{ return "weapons\\"; }

	// What is the context name
	const char *Context()
		{ return "proj"; }
};

// projectile singleton
extern ProjectileLibrary *g_projectileLibPtr;
inline ProjectileLibrary &SafePtr(ProjectileLibrary *ptr)
{
	ASSERT(ptr);
	return *ptr;
}
#define g_projectileLib SafePtr(g_projectileLibPtr)

#endif
