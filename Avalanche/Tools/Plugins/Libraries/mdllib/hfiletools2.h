#ifndef __FILETOOLS2_H
#define __FILETOOLS2_H

#include "harg.h"
#include "uberstring.h"
#include "hstatus.h"

class fileTools2
{
public:
	static void loopForAttr(std::ifstream &fin, UString &attr, HArgArray &args, UString &EOB);
	static void loopForAttr(std::ifstream &fin, UString attr, HArgArray &args, UString EOB);

	static void goToAttr(std::ifstream &fin, UString &attr, HArgArray &args, UString &EOB);
	static void goToAttr(std::ifstream &fin, UString attr, HArgArray &args, UString EOB);

	static void goToLabel(std::ifstream &fin, UString &label, UString &EOB);
	static void goToLabel(std::ifstream &fin, UString label, UString EOB);

	static bool doesLabelExist(std::ifstream &fin, UString &label, UString &EOB);
	static bool doesLabelExist(std::ifstream &fin, UString label, UString EOB);
};

#endif