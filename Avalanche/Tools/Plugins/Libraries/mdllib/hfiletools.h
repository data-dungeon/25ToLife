#ifndef __FILETOOLS_H
#define __FILETOOLS_H

#ifndef __HARG_H
#include "harg.h"
#endif
#ifndef __UBERSTRING_H
#include "uberstring.h"
#endif
#ifndef __HSTATUS_H
#include "hstatus.h"
#endif

class fileTools
{
public:
	static void parseLine(std::ifstream &fin, UString label, int *n);
	static void parseLine(std::ifstream &fin, UString label, float *n);
	static void parseLine(std::ifstream &fin, UString label, bool *n);

	static bool	parseLine(std::ifstream &fin, UString label, HArgArray &args);
	static void parseLine(std::ifstream &fin, HArgArray &args);

	static bool parseLine(UString fline, UString &label, HArgArray &args);
	static void parseLine(UString fline, HArgArray &args);

	static void skipTo(std::ifstream &fin, UString label);
};

#endif
