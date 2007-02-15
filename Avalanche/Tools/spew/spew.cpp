// spew.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <assert.h>

int _tmain(int argc, _TCHAR* argv[])
{
	printf("Spew v1.0 by Food\n");
	if (argc < 3)
	{
		printf("\nUsage: Spew filename filesize<k|m|g|s>\n");
		printf("Where: k = kilobytes, m = megabytes, g = gigabytes, s = Sony Sectors (2048)\n");
		exit(1);
	}

	const char *file = argv[1];

	size_t size = atoi(argv[2]);
	size_t len = strlen(argv[2]);
	if (len > 0)
	{
		if (tolower(argv[2][len - 1]) == 'k')
			size *= 1024;
		else if (tolower(argv[2][len - 1]) == 'm')
			size *= 1024 * 1024;
		else if (tolower(argv[2][len - 1]) == 'g')
			size *= 1024 * 1024 * 1024;
		else if (tolower(argv[2][len - 1]) == 's')
			size *= 2048;
	}

	printf("\nSpewing %d bytes into %s.\n", size, file);

	FILE *fh = fopen(file, "wb");
	if (!fh)
	{
		printf("\nError: cannot open file %s\n", file);
		exit(1);
	}

	const int chunkSize = 1024;
	char buffer[chunkSize];
	const char *fill = "F00d";
	size_t fillLength = strlen(fill);
	assert((chunkSize % fillLength) == 0);
	for (size_t i = 0; i < chunkSize; i += fillLength)
		memcpy(&buffer[i], fill, fillLength);

	// How many chunks are we going to write
	size_t chunks = size / chunkSize;
	size_t total = 0;
	for (size_t i = 0; i < chunks; i++)
	{
		if (fwrite(buffer, chunkSize, 1, fh) != 1)
		{
			fclose(fh);
			printf("\nError: Error writing to %s, wrote %d out of %d\n", file, total, size);
			goto Exit;
		}
		total += chunkSize;
	}

	// Do the leftovers
	size_t leftOver = size % chunkSize;
	if (leftOver != 0)
	{
		if (fwrite(buffer, leftOver, 1, fh) != 1)
		{
			printf("\nError: Error writing to %s, wrote %d out of %d\n", file, total, size);
			goto Exit;
		}
		total += leftOver;
	}

Exit:
	fclose(fh);
	return (total == size) ? 0 : 1;
}

