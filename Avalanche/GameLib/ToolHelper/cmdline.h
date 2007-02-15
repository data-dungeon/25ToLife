//***************************************************************************
// Simple command line parser
//***************************************************************************
#ifndef CMDLINE_H
#define CMDLINE_H

#include "ToolHelper/dynstr.h"
#include "container/Darray.h"

class CommandLineParameter;

class CommandLine
{
public:
	CommandLine(void);
	~CommandLine(void);

	// Parse a string of data into command line arguments
	// Returns the number of arguments found
	// allowedSwitches gives a list of allowed switches (single letters)
	// If allowedSwitched is NULL, all swtiches are valid
	// IF errorFunction is non-null, bad switches will be reported, if true is returned
	// parsing continues, false halts parsing.
	typedef bool (*ErrorFunction)(CommandLineParameter *badParameter);
	int Parse(char *commandLine, char *allowedSwitches = NULL, ErrorFunction errorFunction = NULL);
	int Parse(int argc, char *argv[], char *allowedSwitches = NULL, ErrorFunction errorFunction = NULL);

	// How many paramters are there
	unsigned int Parameters(void)
		{ return(param.Count()); }

	// Return a string corresponding to a given parameter index
	CommandLineParameter *GetParam(unsigned int paramIndex);

	// Return a string corresponding to a given switch
	char *GetParam(char switchChar);

	// Get the param as a dynstring
	DynString *GetParamDynString(char switchChar);

private:
	// Parse a string
	bool ParseString(char *commandLine, char *allowedSwitches, ErrorFunction errorFunction);

	// Pick off a parameter (returns NULL on error)
	CommandLineParameter *ParseParameter(char *&commandLine);

	// Load in a response file
	bool ParseResponseFile(char *fileName, char *allowedSwitches, ErrorFunction errorFunction);

	// Is this a valid switch?
	bool ValidSwitch(char checkSwitch, char *allowedSwitches);

	// The list of parameters
	DynamicArray<CommandLineParameter *>	param;
};

// Simple container for a command line parameter
class CommandLineParameter
{
public:
	CommandLineParameter(void) {}
	~CommandLineParameter(void) {}

	// Set the comnand line parameter up
	void Set(char switchChar, char *value);

	// Get the switch character
	char Switch(void)
		{ return(switchChar); }

	// Set the switch character
	void SetSwitch(char setMe)
		{ switchChar = setMe; }

	// Get the parameter value
	DynString &Value(void)
		{ return(value); }

private:
	char			switchChar;
	DynString	value;
};

#endif
