// makepatch.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "RPatchCommon/RPatchCompiler.h"

typedef unsigned int u_int;

using namespace std;

static void PrintUsage(void);
static u_int ParseUInt(const char* text);

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc != 5) {
		PrintUsage();
		return 1;
	}

	_TCHAR*		originalDir = argv[1];
	_TCHAR*		modifiedDir = argv[2];
	u_int		version     = ParseUInt(argv[3]);
	_TCHAR*		patchName   = argv[4];

	RPatchCompiler patchCompiler(originalDir, modifiedDir);

	list<string> fileList1;
	if (!patchCompiler.GenerateFileList(fileList1, originalDir)) {
		fprintf(stderr, "Error in original dir spec '%s'; aborting!\n", originalDir);
		return 1;
	}

	list<string> fileList2;
	if (!patchCompiler.GenerateFileList(fileList2, modifiedDir)) {
		fprintf(stderr, "Error in modified dir spec '%s'; aborting!\n", modifiedDir);
		return 1;
	}

	list<string> diffList;
	patchCompiler.GetDifferenceList(fileList1, fileList2, diffList);

	if (!patchCompiler.CompilePatch(diffList, patchName, version)) {
		fprintf(stderr, "Error writing out patch file '%s'; aborting!\n", patchName);
		return 1;
	}

	return 0;
}

void PrintUsage(void)
{
	fprintf(stderr, "Usage: makepatch original-dir modified-dir version patchfile\n");
}

unsigned int ParseUInt(const char* text)
{
	int len = (int)strlen(text);
	u_int base = 10;
	u_int tmp = 0;
	u_int val = 0;
	u_int digit;

	for (int pos = 0; pos < len; pos++) {
		u_int c = (int)text[pos];

		// Always allow a digit
		if (isdigit(c)) {
			digit = c - '0';
			tmp = (val * base) + digit;
			if (((tmp - digit) / base) != val)	// Overflow
				break;
			val = tmp;
			continue;
		}

		// Allow an X if and only if it is the second character preceded by a zero
		if ((c == 'x') || (c == 'X')) {
			if ((pos == 1) && (text[0] == '0')) {
				base = 16;
				continue;
			}
		}

		// If we are hex, allow a-f
		if (base == 16) {
			tmp = toupper(c);
			if ((tmp >= 'A') && (tmp <= 'F')) {
				digit = tmp - 'A' + 10;
				tmp = (val * base) + digit;
				if (((tmp - digit) / base) != val)	// Overflow
					break;
				val = tmp;
				continue;
			}
		}

		// If we got here, the character is illegal!
		break;
	}

	// If a character was bad, zap it and return
	if (pos < len) {
		fprintf(stderr, "Unable to parse version number '%s'.\n", text);
		fprintf(stderr, "It must be a valid 32 bit unsigned integer.\n");
		fprintf(stderr, "You may enter it in hexadecimal also, such as: 0x50002\n");
		exit(1);
	}

	// The whole string was good
	return val;
}
