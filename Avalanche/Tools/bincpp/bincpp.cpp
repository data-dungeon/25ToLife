///////////////////////////////////////////////////////////////////////////////
// Convert a bin file into a .h / .cpp file pair
///////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

// Read in a file
void *OpenReadClose(const char *inputFile, int &size);

// Force a file extention (pass in '.'-whatever for the ext)
void StripExt(char *pathFile);
void ForceExtension(char *pathFile, const char *newExt);
void JustFileName(const char *pathFile, char *file);

// Exit with an error
void ErrorExit(char *format, ...);

// The default comment line
static char *commentLine = "/////////////////////////////////////////////////////////////////////////////\n";

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
int main(
int argc,
char *argv[])
{
	// Version
	printf("bincpp v1.0 - Convert binary file into .h/.cpp pair\n");

	// Duh?
	if (argc != 2 && argc != 3)
		ErrorExit("Usage: bincpp <input file> (<varname override>)\n");

	// Build the header file name
	char inputFile[256];
	strcpy(inputFile, argv[1]);

	// Read in the file
	int size = 0;
	unsigned char *data = (unsigned char *)OpenReadClose(inputFile, size);

	// Read in the data
	if (!data)
		ErrorExit("Error: Cannot open binary file \"%s\" for input.", inputFile);

	// duh?
	if (size == 0)
		ErrorExit("Error: Cannot operate on 0 size file \"%s\".", inputFile);

	// Write the output file
	char outputFile[256];
	strcpy(outputFile, inputFile);
	ForceExtension(outputFile, ".cpp");

	// Open it
	FILE *output = fopen(outputFile, "wt");
	if (!output)
	{
		delete [] data;
		ErrorExit("Error: Cannot create output file \"%s\".", outputFile);
	}

	// Printout the header
	fprintf(output, commentLine);
	fprintf(output, "// Automatically generated output for \"%s\".\n", inputFile);
	fprintf(output, commentLine);
	fprintf(output, "\n");

	// Now do the data
	char varName[256];
	if (argc != 3)
		JustFileName(inputFile, varName);
	else
		strcpy(varName, argv[2]);
	fprintf(output, "unsigned char %s[%d] CODERESIDENT_DATA_ALIGN = \n{\n\t", varName, size);

	for (int i = 0; i < size; i++)
	{
		if (data[i] < 16)
			fprintf(output, "0x0%x", (unsigned int)data[i]);
		else
			fprintf(output, "0x%x", (unsigned int)data[i]);
		if (i != size - 1)
			fprintf(output, ", ");
		if (((i + 1) % 16) == 0 && i != size - 1)
			fprintf(output, "\n\t");
	}

	// Complete it!
	fprintf(output, "\n};\n\n");
	fclose(output);

	// Clean up the data, we are done
	delete [] data;

	// tell them we are happy
	printf("Wrote \"%s\" successfully.\n", outputFile);

	// Now do the H file
	ForceExtension(outputFile, ".h");
	output = fopen(outputFile, "wt");
	if (!output)
		ErrorExit("Error: Cannot create output file \"%s\".", outputFile);

	// Our name
	char defineName[256];
	JustFileName(outputFile, defineName);
	strupr(defineName);

	// print out the header
	fprintf(output, commentLine);
	fprintf(output, "// Automatically generated output for \"%s\".\n", inputFile);
	fprintf(output, commentLine);
	fprintf(output, "#ifndef __%s_H__\n#define __%s_H__\n\n", defineName, defineName);

	fprintf(output, "extern char %s[%d];\n\n", varName, size);

	fprintf(output, "#endif\n");
	fclose(output);

	// tell them we are happy
	printf("Wrote \"%s\" successfully.\n", outputFile);

	return(0);
}

///////////////////////////////////////////////////////////////////////////////
// Read in a file
///////////////////////////////////////////////////////////////////////////////
void *OpenReadClose(
const char *inputFile, 
int &size)
{
	FILE *in = fopen(inputFile, "rb");
	if (!in)
		return(NULL);

	// how big is the file
	fseek(in, 0, SEEK_END);
	size = ftell(in);
	fseek(in, 0, SEEK_SET);

	// Allocate an array
	void *data = new unsigned char[size];
	if (!data)
	{
		fclose(in);
		return(NULL);
	}

	// Read it in
	if (fread(data, size, 1, in) != 1)
	{
		delete [] data;
		fclose(in);
		return(NULL);
	}

	fclose(in);
	return(data);
}

///////////////////////////////////////////////////////////////////////////////
// Strip off the extension on a file
///////////////////////////////////////////////////////////////////////////////
void StripExt(
char *pathFile)
{
	int len = strlen(pathFile);
	while (--len > 0 && pathFile[len] != '\\')
	{
		if (pathFile[len] == '.')
		{
			pathFile[len] = '\0';
			break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// Force a file extention (pass in '.'-whatever for the ext)
///////////////////////////////////////////////////////////////////////////////
void ForceExtension(
char *pathFile,
const char *newExt)
{
	StripExt(pathFile);
	if (newExt && *newExt)
		strcat(pathFile, newExt);
}

///////////////////////////////////////////////////////////////////////////////
// Pick off just the filename
///////////////////////////////////////////////////////////////////////////////
void JustFileName(
const char *pathFile,
char *file)
{
	// Save the length
	int len = strlen(pathFile) - 1;

	// Find the beginning of the name, or a slash
	while (len > 0 && pathFile[len] != '\\')
		--len;
	if (pathFile[len] == '\\')
		++len;

	// Now copy to the dot or the end
	int fileLen = 0;
	while (pathFile[len] != '\0' && pathFile[len] != '.')
		file[fileLen++] = pathFile[len++];
	file[fileLen] = '\0';
}

/////////////////////////////////////////////////////////////////////////////
// Exit with an error
/////////////////////////////////////////////////////////////////////////////
void ErrorExit(
char *format,
...)
{
	char temp[1024];

	// Pick apart the error
	va_list args;
	va_start(args, format);

	// this could overflow
	vsprintf(temp, format, args);

	va_end(args);

	// Show the error
	printf("%s\n", temp);

	// We are out of here
	exit(1);
}


