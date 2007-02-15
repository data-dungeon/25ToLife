/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

// This dumps out all we know
//#define DUMPCSV

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
template <class Type>
inline VarObjProperties<Type>::VarObjProperties(
VarObjCreator<Type> &creator,
int index) :
m_creator(creator),
m_index(index)
{
	m_name[0] = '\0';
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
template <class Type>
inline VarObjProperties<Type>::~VarObjProperties()
{
	// We are going away
}

/////////////////////////////////////////////////////////////////////////////
// For parsing the data
/////////////////////////////////////////////////////////////////////////////
template <class Type>
inline void VarObjProperties<Type>::RegisterContext(
const char *context)
{
	g_console.CreateVar(context, "name", m_name, sizeof(m_name) - 1);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
template <class Type>
inline VarObjCreator<Type>::VarObjCreator(
const char *type)
{
	// Save off the type
	ASSERT(strlen(type) < sizeof(m_class));
	strcpy(m_class, type);

	// Insert this badboy in the list
	m_next = VarObjLibrary<Type>::AddCreator(this);
}

/////////////////////////////////////////////////////////////////////////////
// Clean up if we have to, but we should have already.
/////////////////////////////////////////////////////////////////////////////
template <class Type>
inline VarObjCreator<Type>::~VarObjCreator()
{
}

/////////////////////////////////////////////////////////////////////////////
// Create a properties object
/////////////////////////////////////////////////////////////////////////////
template <class Type>
inline VarObjProperties<Type> *VarObjCreator<Type>::CreateProperties(
int index)
{
	// You gotta write this part
	return NewProperties(index);
}

/////////////////////////////////////////////////////////////////////////////
// Nothing to see here
/////////////////////////////////////////////////////////////////////////////
template <class Type>
inline VarObjLibrary<Type>::VarObjLibrary() :
m_list(8, 8)
{
	// Nothing to see here, move along
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
template <class Type>
inline VarObjLibrary<Type>::~VarObjLibrary()
{
	TerminateOnce();
}

/////////////////////////////////////////////////////////////////////////////
// Initialize/terminate the library
/////////////////////////////////////////////////////////////////////////////
template <class Type>
inline void VarObjLibrary<Type>::InitializeOnce()
{
	// Do the creators
	VarObjCreator<Type> *creator = m_creatorList;
	while (creator)
	{
		creator->InitializeOnce();
		creator = creator->Next();
	}

	// Go through the types
	int onType = 0;

	// Gotta collect 'em all
	for (;;)
	{
		// Get the var file
		char pathFile[48];
		sprintf(pathFile, "var\\%s%s%d", Path(), Context(), onType);
		CommandConsole::ScriptHandle script = g_console.LoadScript(pathFile);
		if (!script)
			break;

		// Make this specific context name
		char uniqueContext[64];
		sprintf(uniqueContext, "%s%d", Context(), onType);

		// What type is it?
		char type[64];
		if (!GetClass(script, uniqueContext, type, sizeof(type)))
		{
			TRACE("VOLib:: Type not specified in script");
			ASSERT(false);
			continue;
		}

		// Make sure we have a handler for this type
		VarObjCreator<Type>	*creator = FindCreator(type);
		if (!creator)
		{
			TRACE("VOLib:: No creator installed for this type");
			ASSERT(false);
			continue;
		}

		// Where will they be in the list
		int index = m_list.Count();

		// Create a new properties object
		VarObjProperties<Type> *properties = creator->CreateProperties(index);
		ASSERT(properties);
		if (!properties)
			continue;

		// Put it in the list
		m_list.Add(properties);

		// Save this
		VERBOSITY oldVerbosity = g_console.Verbosity();
		g_console.SetVerbosity(VERBOSITY_ERROR);

		// Create the context
		properties->RegisterContext(uniqueContext);
		g_console.BindScript(uniqueContext, pathFile);

		// Execute the script
		g_console.ExecuteScript(script, uniqueContext);

		// We are done
		g_console.UnloadScript(script);

#ifdef DUMPCSV
		g_console.SaveContextCSV(uniqueContext);
#endif

		// Get rid of the context
		g_console.DeleteContext(uniqueContext);
		g_console.SetVerbosity(oldVerbosity);

		// Tell this properties we loaded
		properties->PostLoadSetup();

		// Next!
		++onType;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Terminate
/////////////////////////////////////////////////////////////////////////////
template <class Type>
inline void VarObjLibrary<Type>::TerminateOnce()
{
	// Destroy all the objects we loaded
	while (m_list.Count() > 0)
		m_list.Delete(m_list.Count() - 1);

	// Do the creators
	VarObjCreator<Type> *creator = m_creatorList;
	while (creator)
	{
		creator->TerminateOnce();
		creator = creator->Next();
	}
}

/////////////////////////////////////////////////////////////////////////////
// Initialize all the potentially dynamic data data
/////////////////////////////////////////////////////////////////////////////
template <class Type>
inline void VarObjLibrary<Type>::InitializeEvery()
{
	// Do the properties
	int count = m_list.Count();
	for (int i = 0; i < count; i++)
		m_list[i]->InitializeEvery();

	// Do the creators
	VarObjCreator<Type> *creator = m_creatorList;
	while (creator)
	{
		creator->InitializeEvery();
		creator = creator->Next();
	}
}

/////////////////////////////////////////////////////////////////////////////
// Terminate
/////////////////////////////////////////////////////////////////////////////
template <class Type>
inline void VarObjLibrary<Type>::TerminateEvery()
{
	int count = m_list.Count();
	for (int i = 0; i < count; i++)
		m_list[i]->TerminateEvery();

	// Do the creators
	VarObjCreator<Type> *creator = m_creatorList;
	while (creator)
	{
		creator->TerminateEvery();
		creator = creator->Next();
	}
}

/////////////////////////////////////////////////////////////////////////////
// Create an object ot type
/////////////////////////////////////////////////////////////////////////////
template <class Type>
inline Type *VarObjLibrary<Type>::CreateInstance(
const char *name)
{
	VarObjProperties<Type> *properties = GetProperties(name);
	if (!properties)
		return NULL;
	else
		return properties->CreateInstance();
}

/////////////////////////////////////////////////////////////////////////////
// Get a type based on an index, NULL means the index is out of range
/////////////////////////////////////////////////////////////////////////////
template <class Type>
inline VarObjProperties<Type> *VarObjLibrary<Type>::GetProperties(
int index)
{
	if (index < 0 || index >= m_list.Count())
		return NULL;
	else
		return m_list[index];
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
template <class Type>
inline VarObjProperties<Type> *VarObjLibrary<Type>::GetProperties(
const char *name)
{
	// Perform this check as everyone else will do it otherwise
	if (!name || !name[0])
		return NULL;

	// scan the list for the right creator
	int count = m_list.Count();
	for (int i = 0; i < count; i++)
	{
		// Is this the right one?
		if (strcmpi(m_list[i]->GetName(), name) == 0)
			return m_list[i];
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// Add a creator to the list of known creators.
/////////////////////////////////////////////////////////////////////////////
template <class Type>
inline VarObjCreator<Type> *VarObjLibrary<Type>::AddCreator(
VarObjCreator<Type> *creator)
{
	// Save the old head
	VarObjCreator<Type> *oldHead = m_creatorList;

	// Put them on the list
	m_creatorList = creator;

	// Return the old one so they can patch the list
	return oldHead;
}

/////////////////////////////////////////////////////////////////////////////
// Find the creator in question
/////////////////////////////////////////////////////////////////////////////
template <class Type>
VarObjCreator<Type> *VarObjLibrary<Type>::FindCreator(
const char *type)
{
	VarObjCreator<Type> *creator = m_creatorList;
	while (creator)
	{
		// Is this our creator?
		if (strcmpi(type, creator->GetClass()) == 0)
			return creator;

		// next?
		creator = creator->Next();
	}

	// We could not find a creator for this type
	// This is insanely bad
	ASSERT(false);
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// Get the type from a script
/////////////////////////////////////////////////////////////////////////////
template <class Type>
inline bool VarObjLibrary<Type>::GetClass(
CommandConsole::ScriptHandle script,
const char *context,
char *type,
int size)
{
	// We just care about the type
#if 1
	g_console.FindValueQuick(script, "class", type, size - 1);
#else
	type[0] = '\0';
	g_console.CreateVar(context, "class", type, size - 1);
	VERBOSITY oldVerbosity = g_console.Verbosity();
	g_console.SetVerbosity(VERBOSITY_ERROR);
	g_console.ExecuteScript(script, context);
	g_console.DeleteContext(context);
	g_console.SetVerbosity(oldVerbosity);
#endif

	// Did we get a type
	if (!type[0])
	{
		TRACE("VOLib:: No class specified.");
		ASSERT(false);
		return false;
	}

	return true;
}

