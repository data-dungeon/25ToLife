///////////////////////////////////////////////////////////////////////////////
// Program to determine the best size for mib/mih segment!
///////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

// Get the file size
int FileSize(const char *inputFile);

// Figure the best size
int BestInterleave(int size, int minInterleave, int maxInterleave, int &remainder, int step = 1024);
int SingleBuffer(int size, int &remainder);

// Is this a valid interleave for this size?
bool ValidInterleave(int size, int interleave);

// Exit with an error
void ErrorExit(char *format, ...);

// Force a file extention (pass in '.'-whatever for the ext)
void StripExt(char *pathFile);
void ForceExtension(char *pathFile, const char *newExt);
void JustFileName(const char *pathFile, char *file);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int main(
int argc,
char *argv[])
{
	// Version
	printf("MIBitch v6.0 - Given an input MSV tell you the best interleave!\n");

	// Duh?
	if (argc != 4)
		ErrorExit("Usage: mibitch <input MSV file> <min interleave k> <max interleave k>\n");

	// Verify the the input
	int minInterleave = atoi(argv[2]) * 1024;
	int maxInterleave = atoi(argv[3]) * 1024;

	if (minInterleave == 0 || maxInterleave == 0 || minInterleave > maxInterleave)
		ErrorExit("Error: Invalid interleave sizes given %d %d\n", minInterleave);

	// Get the file size
	int size = FileSize(argv[1]);
	if (size < 0)
		ErrorExit("Error: File %s not found\n", argv[1]);
	printf("Size of %s is %d\n", argv[1], size);

	// Figure the best size
	int remainder;
	int best = BestInterleave(size, minInterleave, maxInterleave, remainder);

	// Did we fail?
	if (!ValidInterleave(size, best))
	{
		// Probably a really small file
		int tryAgainMax = ((size + 1023) / 1024) * 1024;
		if (tryAgainMax > maxInterleave + 1024)
			tryAgainMax = maxInterleave + 1024;
		int tryAgainMin = 8 * 1024;

		// Emergency!
		best = BestInterleave(size, tryAgainMin, tryAgainMax, remainder);

		// Did we fail yet again?
		if (!ValidInterleave(size, best))
			best = BestInterleave(size, tryAgainMin, tryAgainMax, remainder, 768);

		// Holy crap, did we fail yet again?
		if (!ValidInterleave(size, best) && size <= maxInterleave)
			best = SingleBuffer(size, remainder);
	}		

	// Give them the command line
	printf("The best interleave value is: %d (last track has %d bytes (%d %%))\n", best, remainder, remainder * 100 / best);
	int returnValue = 0;
	if (!ValidInterleave(size, best))
	{
		printf("*************  \007\007WARNING the data in the last track may cause PS2 to hang!!!!\n");
		returnValue = 1;
	}

	// Write the batch file
	FILE *batch = fopen("setmibitch.bat", "wt");
	if (!batch)
		ErrorExit("Error: Could not create setmibitch.bat!!!\n");
	fprintf(batch, "@set MIBITCH=%d", best);
	fclose(batch);

	return returnValue;
}

///////////////////////////////////////////////////////////////////////////////
// Is this a valid interleave for this size?
///////////////////////////////////////////////////////////////////////////////
bool ValidInterleave(
int size,
int interleave)
{
	// How much data is left?
	int remainder = size % interleave;

	// We want more the 60% to be sure!
	int percent = remainder * 100 / interleave;
	if (remainder == 0 || percent < 60 || percent >= 99)
		return false;
	else
		return true;
}

///////////////////////////////////////////////////////////////////////////////
// Figure the best interleave size
///////////////////////////////////////////////////////////////////////////////
int BestInterleave(
int size,
int minInterleave,
int maxInterleave,
int &remainder,
int step)
{
	// What is the best?
	int bestInterleave = 0;
	int bestRemainder = 0;

	for (int test = maxInterleave; test >= minInterleave; test -= step)
	{
		// What is the remainder?
		int r = size % test;

#ifdef _DEBUG
		printf("Test interleave: %d - last buffer %d (%d%%)", test, r, r * 100 / test);
		if (!ValidInterleave(size, test))
			printf(" ***** FAILED ******");
#endif
		// Is this better?
		if (ValidInterleave(size, test) && (bestInterleave == 0 || r >= bestRemainder))
		{
			bestInterleave = test;
			bestRemainder = r;
#ifdef _DEBUG
			printf(" ***** BEST ******");
#endif
		}

#ifdef _DEBUG
		printf("\n");
#endif
	}

	// Did we find one?
	if (bestInterleave == 0)
	{
		bestInterleave = maxInterleave;
		bestRemainder = size % bestInterleave;
	}

	// Give them back the steeze
	remainder = bestRemainder;
	return bestInterleave;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int SingleBuffer(int size, int &remainder)
{
	int interleave = ((size + 1023) / 1024) * 1024 + 1024;
	remainder = size % interleave;
	return interleave;
}

///////////////////////////////////////////////////////////////////////////////
// How big is this file?
///////////////////////////////////////////////////////////////////////////////
int FileSize(
const char *inputFile)
{
	FILE *in = fopen(inputFile, "rb");
	if (!in)
		return(-1);

	// how big is the file
	fseek(in, 0, SEEK_END);
	int size = ftell(in);
	fclose(in);

	return size;
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
	exit(2);
}


