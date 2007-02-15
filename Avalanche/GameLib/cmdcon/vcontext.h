/***************************************************************************/
// Variable context for the command console
/***************************************************************************/
#ifndef VCONTEXT_H
#define VCONTEXT_H

#include "cmdcon/convar.h"
#include "container/darray.h"
#include "enginehelper/hash.h"

#define VCON_HASH_POWOF2	5
#define VCON_HASH_SIZE		(1 << VCON_HASH_POWOF2)
#define VCON_HASH_MASK		(VCON_HASH_SIZE - 1)

#define VCONTEXT_NAME_SIZE	20

// A set of command variables
class VariableContext
{
public:
	VariableContext(const char *contextName, bool setPermanent, int startVars = 32);
	~VariableContext(void);

	// Get the context name
	const char *Name(void)
		{ return(context); }

	// Bind a script file to a context
	bool BindScript(const char *pathFile);

	// What is our bound script?
	const char *Script(void)
		{ return(script[0] ? script : NULL); }

	// Are we permanent?
	bool Permanent(void)
		{ return(permanent); }

	// Control our permanence
	void SetPermanence(bool setPermanent)
		{ permanent = setPermanent; }

	// Add a variable to the context (context is in charge now)
	bool Add(ConsoleVariable *variable);

	// Return the varible interface if we have the variable
	ConsoleVariable *Find(const char *variableName);

	// Enumeration (set to 0 and go until NULL returned)
	ConsoleVariable *operator()(int &i);

private:
	// The name of our context
	char 					context[VCONTEXT_NAME_SIZE];

	// Are we permanent?
	bool					permanent;

	// Our script
	char					script[32];

	// Our list of known variables
	struct HashElement
	{
		ConsoleVariable	*var;
		int					next;
	};
	DynamicArray<HashElement>			v;

	// The hash
	int						hashTable[VCON_HASH_SIZE];
	uint						Hash(const char *s)
		{ return Hash::CalcChecksum(s, strlen(s)); }

	ConsoleVariable *FindProperCase(const char *properCase);
};

#endif //VCONTEXT_H
