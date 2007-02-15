/////////////////////////////////////////////////////////////////////////////
// Manage a list of var defined object types with a fast way of
// instantiating them.
/////////////////////////////////////////////////////////////////////////////
#ifndef VOLIB_H
#define VOLIB_H

#include "cmdcon/cmdcon.h"
#include "container/dllist.h"

// Forward declare the properties
template <class Type> class VarObjCreator;

/////////////////////////////////////////////////////////////////////////////
// The properties object defines the data that an object loads in from the
// .var file.
/////////////////////////////////////////////////////////////////////////////
template <class Type> class VarObjProperties
{
public:
	inline VarObjProperties(VarObjCreator<Type> &creator, int index);
	inline virtual ~VarObjProperties();

	// Who is our creator
	VarObjCreator<Type> &GetCreator() const
		{ return m_creator; }

	// We appear in a list, what is our index?
	int GetIndex() const
		{ return m_index; }

	// What is our type
	const char *GetClass() const
		{ return m_creator.GetClass(); }

	// What is our name?
	const char *GetName() const
		{ return m_name; }

	// For parsing the data
	inline virtual void RegisterContext(const char *context);

	// After the script is executed on the context
	virtual void PostLoadSetup() {}

	// Init and terminate
	virtual void InitializeEvery() {}
	virtual void TerminateEvery() {}

	// Create an instance using these properties
	virtual Type *CreateInstance()
		{ return m_creator.CreateInstance(*this); }

private:
	// Our creator
	VarObjCreator<Type>  &m_creator;

	// our index in the list
	int						m_index;

	// Our name
	char						m_name[64];
};

/////////////////////////////////////////////////////////////////////////////
// This class needs to be subclassed in order to create an object
// The creator is a static object that is defined per class that will
// be read & created from a .var object
/////////////////////////////////////////////////////////////////////////////
template <class Type> class VarObjCreator
{
public:
	// Hello
	inline VarObjCreator(const char *type);
	inline virtual ~VarObjCreator();

	// What is our type
	const char *GetClass() const
		{ return m_class; }

	// Create a properties object
	inline virtual VarObjProperties<Type> *CreateProperties(int index);

	// You must provide this for the type
	virtual Type *CreateInstance(const VarObjProperties<Type> &properties) = 0;

	// Initialize & terminate so you can tie in stuff
	// with this type of object.
	virtual void InitializeOnce() {}
	virtual void TerminateOnce() {}

	virtual void InitializeEvery() {}
	virtual void TerminateEvery() {}

	// Next in the list
	VarObjCreator<Type> *Next() const
		{ return m_next; }

protected:
	// Subclass this to provide new
	virtual VarObjProperties<Type> *NewProperties(int index) = 0;

private:
	// The type of this creator
	char 	 						m_class[64];

	// We appear in a singly-linked list
	VarObjCreator<Type> 		*m_next;
};

/////////////////////////////////////////////////////////////////////////////
// The library maintains a list of properties and their creators. This allows
// objects to be instantiated.
/////////////////////////////////////////////////////////////////////////////
template <class Type> class VarObjLibrary
{
public:
	// Initialize all the known objects
	inline VarObjLibrary();
	inline virtual ~VarObjLibrary();

	// Initialize/terminate the library
	inline virtual void InitializeOnce();
	inline virtual void TerminateOnce();

	// Initialize all the potentially dynamic data data
	inline virtual void InitializeEvery();
	inline virtual void TerminateEvery();

	// Create a object with the given name (call delete when done)
	inline Type *CreateInstance(const char *name);

	// How many types are there
	int Types()
		{ return m_list.Count(); }

	// Get a type based on an index, NULL means the index is out of range
	inline VarObjProperties<Type> *GetProperties(int index);

	// Get the type based on the name, may return null
	inline VarObjProperties<Type> *GetProperties(const char *name);

protected:
	// What is the path
	virtual const char *Path()
		{ return ""; }

	// What is the context name
	virtual const char *Context() = 0;

private:
	// Get the type from a script
	inline bool GetClass(CommandConsole::ScriptHandle script, const char *context, char *type, int size);

	// The array of creation classes
	static VarObjCreator<Type>	*m_creatorList;

	// Find the creator in question
	static inline VarObjCreator<Type> *FindCreator(const char *type);

	// The array of known objects and their creators
	typedef VarObjProperties<Type> Props;
	DynList<Props> m_list;

public:
	// Add a creator to the list (returns prior head)
	static inline VarObjCreator<Type> *AddCreator(VarObjCreator<Type> *creator);
};

// The implementation
#include "Components/weapons/volib.hpp"

#endif
