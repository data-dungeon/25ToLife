/*
**
**  File:   ParseAP.cpp
**  Date:   June 7, 2005
**  By:     Bryant Collard
**  Desc:   Parses .AP files into tab delimited records for importation into
**          Excel.
**
**  Copyright (c) 2005, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

#include <stdio.h>
#include <string.h>

#define MAX_NUM_FIELDS 256
#define MAX_FIELD_LEN  128
#define MAX_LINE       (2 * MAX_FIELD_LEN + 1)

int main(int argc, char* argv[])
{
	// Make sure the correct number of arguments is passed.
	if (argc < 2)
	{
		printf("Missing target\n");
		printf("\tUsage: %s <.AP file name>\n", argv[0]);
		return(-1);
	}
	if (argc > 2)
	{
		printf("Too many arguments\n");
		printf("\tUsage: %s <.AP file name>\n", argv[0]);
		return(-1);
	}

	// Open the input file.
	char inName[256];
	strcpy(inName, argv[1]);
	int len = strlen(inName);
	if ((len <= 3) || stricmp(inName + len - 3, ".AP"))
		strcat(inName, ".AP");
	FILE* inFile = fopen(argv[1], "r");
	if (inFile == NULL)
	{
		printf("Cannot open input file %s\n", inName);
		printf("\tUsage: %s <.AP file name>\n", argv[0]);
		return(-1);
	}

	// Form the output file name.
	char* outName = NULL;
	char* token = strtok(inName, ":/\\");
	while(token != NULL)
	{
		outName = token;
		token = strtok(NULL, ":/\\");
	}
	if ((outName == NULL) || (outName[0] == '\0') || (strlen(outName) <= 3) ||
			stricmp(outName + strlen(outName) - 3, ".AP"))
	{
		printf("Failed to form output file name\n");
		printf("\tUsage: %s <.AP file name>\n", argv[0]);
		return(-1);
	}
	outName[strlen(outName) - 3] = '\0';
	strcat(outName, ".txt");

	// Open the output file.
	FILE* outFile = fopen(outName, "w");
	if (outFile == NULL)
	{
		printf("Cannot open input file %s\n", outName);
		return(-1);
	}

	// Set up the field array.
	char fieldName[MAX_NUM_FIELDS][MAX_FIELD_LEN];
	int fieldCount = 0;

	// Process each line.
	int i;
	char buffer[MAX_LINE];
	char* line = fgets(buffer, MAX_LINE, inFile);
	while(line != NULL)
	{
		// Strip off leading spaces and returns.
		line = strtok(line, " \n");

		// Only process lines that are not empty or instance identifiers.
		if ((line != NULL) && (line[0] != '\0') && (line[0] != '['))
		{
			// Find the field name (string up to =). Indicate an error if the
			// line starts with =.
			char* name = strtok(line, "=");
			if (name != line)
			{
				printf("Badly formed line in input (%s)\n", buffer);
				return(-1);
			}

			// Line must have = in it.
			char* value = strtok(NULL, "");
			if (value != NULL)
			{
				// See of this field has already been found.
				for (i = 0; i < fieldCount; i++)
				{
					if (!strcmp(name, fieldName[i]))
						break;
				}

				// This is a new field.
				if (i >= fieldCount)
				{
					if (fieldCount >= MAX_NUM_FIELDS)
					{
						printf("Too many fields.\n");
						return(-1);
					}
					strcpy(fieldName[fieldCount], name);
					fieldCount++;
				}
			}
		}
		line = fgets(buffer, MAX_LINE, inFile);
	}

	// Write the headers.
	fprintf(outFile, "Instance");
	for (i = 0; i < fieldCount; i++)
	{
		fprintf(outFile, "\t%s", fieldName[i]);
	}
	fprintf(outFile, "\n");

	// Prepare to write the output file.
	rewind(inFile);
	char fieldValue[MAX_NUM_FIELDS][MAX_FIELD_LEN];
	bool foundInstance = false;

	// Process each line.
	line = fgets(buffer, MAX_LINE, inFile);
	while(line != NULL)
	{
		// Strip off leading spaces and returns.
		line = strtok(line, " \n");

		// Only process lines that are not empty.
		if ((line != NULL) && (line[0] != '\0'))
		{
			if (line[0] == '[')
			{
				// Get the instance name. Make sure there is a name.
				line = strtok(line, "[]");
				if ((line == NULL) || (line[0] == '\0'))
				{
					printf("Badly formed line in input (%s)\n", buffer);
					return(-1);
				}

				if (foundInstance)
				{
					for (i = 0; i < fieldCount; i++)
					{
						fprintf(outFile, "\t");
						if (fieldValue[i][0] != '\0')
							fprintf(outFile, "%s", fieldValue[i]);
					}
					fprintf(outFile, "\n");
				}
				for (i = 0; i < fieldCount; i++)
				{
					fieldValue[i][0] = '\0';
				}
				fprintf(outFile, "%s", line);
				foundInstance = true;
			}
			else
			{
				// Find the field name (string up to =). Indicate an error if the
				// line starts with =.
				char* name = strtok(line, "=");
				if (name != line)
				{
					printf("Badly formed line in input (%s)\n", buffer);
					return(-1);
				}

				// Get the value.
				char* value = strtok(NULL, "\n");
				if ((value != NULL) && (value[0] != '\0'))
				{
					// Find the field
					for (i = 0; i < fieldCount; i++)
					{
						if (!strcmp(name, fieldName[i]))
						{
							if (!foundInstance)
							{
								printf("Field value set with no instance\n");
								return(-1);
							}
							if (fieldValue[i][0] != '\0')
							{
								printf("Field multiply defined %s\n", name);
								return(-1);
							}
							strcpy(fieldValue[i], value);
							break;
						}
					}

					// This is a new field.
					if (i >= fieldCount)
					{
						printf("Unrecognized field %s\n", name);
						return(-1);
					}
				}
			}
		}
		line = fgets(buffer, MAX_LINE, inFile);
	}

	// Write the last record.
	if (foundInstance)
	{
		for (i = 0; i < fieldCount; i++)
		{
			fprintf(outFile, "\t");
			if (fieldValue[i][0] != '\0')
				fprintf(outFile, "%s", fieldValue[i]);
		}
		fprintf(outFile, "\n");
	}

	return(0);
}
