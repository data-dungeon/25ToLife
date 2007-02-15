//***************************************************************************
// Simple command line parser
//***************************************************************************
#include <stdio.h>
#include <toolhelper\cmdline.h>

//***************************************************************************
//***************************************************************************
CommandLine::CommandLine(void)
{
}

//***************************************************************************
//***************************************************************************
CommandLine::~CommandLine(void)
{
	unsigned int count = param.Count();
	for (unsigned int i = 0; i < count; i++)
		delete param[i];
}

//***************************************************************************
// Parse a string of data into command line arguments
// Returns the number of arguments found
// allowedSwitches gives a list of allowed switches (single letters)
// If allowedSwitched is NULL,
//***************************************************************************
int CommandLine::Parse(
char *commandLine,
char *allowedSwitches,
ErrorFunction errorFunction)
{
	// Duh
	if (commandLine == NULL)
		return(0);

	// Parse it
	ParseString(commandLine, allowedSwitches, errorFunction);

	// How many parameters do we have?
	return(param.Count());
}

//***************************************************************************
// Return a string corresponding to a given parameter index
//***************************************************************************
CommandLineParameter *CommandLine::GetParam(
unsigned int paramIndex)
{
	if (paramIndex >= (unsigned int)param.Count())
		return(NULL);
	else
		return(param[paramIndex]);
}

//***************************************************************************
// Return a string corresponding to a given switch
//***************************************************************************
char *CommandLine::GetParam(
char switchChar)
{
	unsigned int count = param.Count();
	for (unsigned int i = 0; i < count; i++)
		if (param[i]->Switch() == switchChar)
			return(param[i]->Value().Data());

	return(NULL);
}

//***************************************************************************
// Return a string corresponding to a given switch
//***************************************************************************
DynString *CommandLine::GetParamDynString(
char switchChar)
{
	unsigned int count = param.Count();
	for (unsigned int i = 0; i < count; i++)
		if (param[i]->Switch() == switchChar)
			return(&param[i]->Value());

	return(NULL);
}

//***************************************************************************
// Parse a string
//***************************************************************************
bool CommandLine::ParseString(
char *commandLine,
char *allowedSwitches,
ErrorFunction errorFunction)
{
	// pick stuff off
	while (*commandLine)
	{
		// Parse one off
		CommandLineParameter *parameter = ParseParameter(commandLine);

		// We are done
		if (!parameter)
			return(true);

		// Response file
		if (parameter->Switch() == '@')
		{
			// Process the response file
			ParseResponseFile(parameter->Value(), allowedSwitches, errorFunction);
			delete parameter;
		}
		else
		{
			// Is this a valid switch?
			if (!ValidSwitch(parameter->Switch(), allowedSwitches))
			{
				// Report it?
				bool keepParsing;
				if (!errorFunction)
					keepParsing = false;
				else
					keepParsing = errorFunction(parameter);

				delete parameter;
				parameter = NULL;
				if (!keepParsing)
					return(false);
			}

			// Add it
			if (parameter)
				param.Add(parameter);
		}
	}

	return(true);
}

//***************************************************************************
// Load in a response file
//***************************************************************************
bool CommandLine::ParseResponseFile(
char *fileName,
char *allowedSwitches,
ErrorFunction errorFunction)
{
	// Load in the file
	FILE *fh = fopen(fileName, "rb");
	if (!fh)
		return(false);

	// how big are we
	fseek(fh, 0, SEEK_END);
	int size = ftell(fh);
	fseek(fh, 0, SEEK_SET);
	if (size <= 0)
	{
		fclose(fh);
		return(false);
	}

	// Allocate space
	char *data = new char [size + 1];
	if (!data)
	{
		fclose(fh);
		return(false);
	}

	// Read it in
	if (fread(data, size, 1, fh) != 1)
	{
		fclose(fh);
		delete [] data;
		return(false);
	}
	data[size] = '\0';
	fclose(fh);

	// translate into a command line type thing
	for (int i = 0; i < size; i++)
		if (data[i] == '\n' || data[i] == '\r' || data[i] == 26)
			data[i] = ' ';

	// Process
	bool ret = ParseString(data, allowedSwitches, errorFunction);
	delete [] data;
	return(ret);
}

//***************************************************************************
// Pick off a parameter (returns NULL on error)
//***************************************************************************
CommandLineParameter *CommandLine::ParseParameter(
char *&commandLine)
{
	typedef enum
	{
		START,
		SWITCH,
		VALUESTART,
		VALUE,
		DONE
	} PARSESTATE;

	// Make sure we arent' parsing crap
	while (*commandLine == ' ')
		++commandLine;
	if (*commandLine == '\0')
		return(NULL);

	// Create a parameter
	CommandLineParameter *param = new CommandLineParameter;
	if (!param)
		return(NULL);
	DynString &value = param->Value();

	// Pick of the switch and the value
	bool inQuotes = false;
	PARSESTATE state = START;
	while (*commandLine && state != DONE)
	{
		// Toggle our quoted state
		if (*commandLine == '\"')
		{
			inQuotes = !inQuotes;
			commandLine++;
			continue;
		}

		// Parse based on our state
		switch (state)
		{
			case START:
				// Is this a switch coming up?
				if (*commandLine == '-' || *commandLine == '/')
					state = SWITCH;
				else if (*commandLine == '@')
				{
					state = VALUESTART;
					param->SetSwitch(*commandLine);
				}
				else if (*commandLine != ' ')	// Or is there no switch?
				{
					param->SetSwitch(0);
					state = VALUE;
					--commandLine;
				}
				break;

			case SWITCH:
				// We are expecting a switch
				if (*commandLine != ' ')
				{
					param->SetSwitch(*commandLine);
					state = VALUESTART;
				}
				break;

			case VALUESTART:
				if (*commandLine != ' ')
				{
					state = VALUE;
					--commandLine;
				}
				break;

			case VALUE:
				// We should be in here at the first character of the value (leading spaces ignored)

				// Are we done?
				if ((*commandLine == '-' || *commandLine == '/') && !inQuotes)
				{
					// There was no data for this switch
					--commandLine;
					state = DONE;
				}
				else if ((*commandLine == ' ' || *commandLine == ';' || *commandLine == '\n') && !inQuotes)
					state = DONE;
				else
					value.CatChar(*commandLine);
				break;
		}

		// advance the string
		commandLine++;
	}

	return(param);
}

//***************************************************************************
// Is this a valid switch?
//***************************************************************************
bool CommandLine::ValidSwitch(
char checkSwitch,
char *allowedSwitches)
{
	// All switches allowed
	if (allowedSwitches == NULL || checkSwitch == '\0')
		return(true);

	// Special case
	if (*allowedSwitches == '\0' && checkSwitch != '\0')
		return(false);

	// Scan the string
	unsigned int len = (int)strlen(allowedSwitches);
	for (unsigned int i = 0; i < len; i++)
		if (allowedSwitches[i] == checkSwitch)
			return(true);

	return(false);
}

//***************************************************************************
// Set the comnand line parameter up
//***************************************************************************
void CommandLineParameter::Set(
char switchChar,
char *value)
{
	this->switchChar = switchChar;
	this->value.Cpy(value);
}

