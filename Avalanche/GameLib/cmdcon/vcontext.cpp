/***************************************************************************/
// Variable context for the command console
/***************************************************************************/
#include "cmdcon/CmdConPCH.h"

#include "enginehelper/hash.h"
#include "Game/Database/DBFile.h"

/***************************************************************************/
// Set the context name
/***************************************************************************/
VariableContext::VariableContext(
const char *contextName,
bool setPermanent,
int startVars) :
v(startVars, startVars / 2)
{
	// Setup the name
	ASSERT(contextName);
	ASSERT(strlen(contextName) < sizeof(context));
	strncpy(context, contextName, sizeof(context) - 1);
	context[sizeof(context) - 1] = '\0';
	strlwr(context);

	permanent = setPermanent;

	script[0] = '\0';

	for (int h = 0; h < VCON_HASH_SIZE; h++)
		hashTable[h] = -1;
}

/***************************************************************************/
/***************************************************************************/
VariableContext::~VariableContext(void)
{
	int count = v.Count();
	for (int c = count - 1; c >= 0; c--)
		delete v[c].var;
}

/***************************************************************************/
// Bind a script file to a context
/***************************************************************************/
bool VariableContext::BindScript(
const char *pathFile)
{
	if (pathFile == NULL || *pathFile == '\0')
		script[0] = '\0';
	else
	{
		ASSERT(strlen(pathFile) < sizeof(script) - 1);
		strcpy(script, pathFile);
	}

	return(true);
}

/***************************************************************************/
// Add a variable to the context (context is in charge now)
/***************************************************************************/
bool VariableContext::Add(
ConsoleVariable *variable)
{
	// Does it already exist?
	if (FindProperCase(variable->Name()))
		return(false);

	// Get the index
	uint hashIndex = (Hash(variable->Name()) & VCON_HASH_MASK);

	// Create our new element
	HashElement e;
	e.var = variable;

	// Add the element
	int index = v.Count();
	bool ret = v.Add(e, index);
	ASSERT(ret);
	if (!ret)
		return false;

	// Stick it in the list
	v[index].next = hashTable[hashIndex];
	hashTable[hashIndex] = index;

	return true;
}

/***************************************************************************/
// Return the varible interface if we have the variable
/***************************************************************************/
ConsoleVariable *VariableContext::Find(
const char *variableName)
{
	char properCase[CONVAR_NAME_SIZE];
	ASSERT(strlen(variableName) < sizeof(properCase) - 1);
	strncpy(properCase, variableName, sizeof(properCase) - 1);
	properCase[sizeof(properCase) - 1] = '\0';
	strlwr(properCase);

	return FindProperCase(properCase);
}

/***************************************************************************/
// Enumeration (set to 0 and go until NULL returned)
/***************************************************************************/
ConsoleVariable *VariableContext::operator()(
int &i)
{
	if (i >= v.Count())
		return(NULL);

	return(v[i++].var);
}

/***************************************************************************/
// Return the varible interface if we have the variable
/***************************************************************************/
ConsoleVariable *VariableContext::FindProperCase(
const char *properCase)
{
	// Where are we going to look
	uint hashIndex = (Hash(properCase) & VCON_HASH_MASK);

	int searchIndex = hashTable[hashIndex];
	while (searchIndex >= 0)
	{
		if (strcmp(v[searchIndex].var->Name(), properCase) == 0)
			return v[searchIndex].var;
		searchIndex = v[searchIndex].next;
	}

	// Not here
	return NULL;
}

