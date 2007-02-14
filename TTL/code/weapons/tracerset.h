/////////////////////////////////////////////////////////////////////////////
// set of tracers
/////////////////////////////////////////////////////////////////////////////
#ifndef TRACERSET_H
#define TRACERSET_H

#include "Components/weapons/volib.h"
#include "Components/weapons/weapdep.h"
#include "Effects/tracer.h"
#include "EngineHelper/Singleton.h"

// Forwards
class TracerSet;
class TracerSetCreator;

// The properties
class TracerSetProperties : public VarObjProperties<TracerSet>
{
public:
	TracerSetProperties(TracerSetCreator &creator, int index);
	~TracerSetProperties();

	// For parsing the data
	void RegisterContext(const char *context);

	// the per-loop stuff
	void InitializeEvery();
	void TerminateEvery();

	// Tracer info
	Vector4			m_tracerHeadColor, m_tracerTailColor;
	float				m_tracerHeadWidth, m_tracerTailWidth;
	float				m_tracerMinStartD, m_tracerMaxStartD;
	float				m_tracerMinLength, m_tracerMaxLength;
	int				m_tracerPercentage;
	int				m_tracerInstances;
	float				m_tracerTextureWrap;
	float				m_tracerWiggle;
	bool				m_tracerDistort;
	float				m_tracerTTL;

	Tracer			*m_tracer;
};

// The mighty class
class TracerSet
{
public:
	TracerSet(const TracerSetProperties &properties);
	~TracerSet() {}

	// Create a tracer!
	bool CreateChord(const Vector3 &origin, const Vector3 &intersection);
	bool CreateRay(const Vector3 &origin, const Vector3 &dir);

protected:
	// Alias our special properties
	const TracerSetProperties &m_properties;
};

// The object factory
class TracerSetCreator : public VarObjCreator<TracerSet>
{
public:
	// Hello
	TracerSetCreator() :
		VarObjCreator<TracerSet>("tracer") {}

	// You must provide this for the type
	TracerSet *CreateInstance(const VarObjProperties<TracerSet> &properties)
		{ return new TracerSet((const TracerSetProperties &)properties); }

protected:
	// Subclass this to provide new
	TracerSetProperties *NewProperties(int index)
		{ return new TracerSetProperties(*this, index); }
};

// The manager of acive TracerSets
class TracerSetLibrary : public VarObjLibrary<TracerSet>
{
public:
	// Initialize all the known TracerSets
	TracerSetLibrary() {}
	~TracerSetLibrary() {}
	
	// the per-loop stuff
	void InitializeEvery();
	void TerminateEvery();

protected:
	// What is the path
	const char *Path()
		{ return "weapons\\"; }

	// What is the context name
	const char *Context()
		{ return "tracer"; }
};

class TracerSetDependency : public WeaponDependency
{
public:
	TracerSetDependency() {}

	void InitializeOnce();
	void TerminateOnce();

	void InitializeEvery();
	void TerminateEvery();

	WeaponDependency::Phase WantPhase()
		{ return WeaponDependency::PREINIT; }
};


// TracerSet singleton
DefineSingleton(TracerSetLibrary)
#define g_tracerSetLibrary GetSingleton(TracerSetLibrary)


#endif
