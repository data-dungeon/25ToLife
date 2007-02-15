#include "stdafx.h"
#include "hfileTools.h"
#include "hmacros.h"

void fileTools::parseLine(std::ifstream &fin, UString label, int *n) {
	UString fline,label2;
	HArgArray args;

	fin >> fline;

	parseLine(fline, label2, args);

	if (label != label2) {
		THROW_FATAL_ERROR("Couldn't parse line.");
	}

	*n = args.getInt(0);
}

void fileTools::parseLine(std::ifstream &fin, UString label, float *n) {
	UString fline,label2;
	HArgArray args;

	fin >> fline;

	parseLine(fline, label2, args);

	if (label != label2) {
		THROW_FATAL_ERROR("Couldn't parse line.");
	}

	*n = (float)args.getFloat(0);
}

void fileTools::parseLine(std::ifstream &fin, UString label, bool *n) {
	UString fline,label2;
	HArgArray args;

	fin >> fline;

	parseLine(fline, label2, args);

	if (label != label2) {
		THROW_FATAL_ERROR("Couldn't parse line.");
	}

	*n = args.getBool(0);
}

bool fileTools::parseLine(std::ifstream &fin, UString label, HArgArray &args) {
	UStringArray sarray,sarray2;
	UString fline;

	fin >> fline;

	args.clear();

	fline.split('=',sarray);

	if (sarray.length() < 2)
		return false;
		
	if (label != sarray[0])
		return false;

	sarray[1].split(' ',sarray2);

	HArg arg;

	for (unsigned i = 0; i < sarray2.length(); i++) {
		arg.set(sarray2[i]);
		args.append(arg);
	}
	return true;
}

bool fileTools::parseLine(UString fline, UString &label, HArgArray &args) {
	UStringArray sarray,sarray2;

	args.clear();

	fline.split('=',sarray);

	if (sarray.length() < 2)
	{
		fline.split(' ',sarray);
		if (sarray.length() < 2)
		{
			label = sarray[0];
			return false;
		}
	}
		
	label = sarray[0];

	sarray[1].split(' ',sarray2);

	HArg arg;

	for (unsigned i = 0; i < sarray2.length(); i++) {
		arg.set(sarray2[i]);
		args.append(arg);
	}
	return true;
}

void fileTools::parseLine(UString fline, HArgArray &args) {

	UStringArray sarray;

	args.clear();

	fline.split(' ',sarray);

	HArg arg;

	for (unsigned i = 0; i < sarray.length(); i++) {
		arg.set(sarray[i]);
		args.append(arg);
	}
}

void fileTools::parseLine(std::ifstream &fin, HArgArray &args) {
	UString fline;
	UStringArray sarray;

	fin >> fline;

	args.clear();

	fline.split(' ',sarray);

	HArg arg;

	for (unsigned i = 0; i < sarray.length(); i++) {
		arg.set(sarray[i]);
		args.append(arg);
	}
}

void fileTools::skipTo(std::ifstream &fin, UString label) {
	UString fline;
	fin >> fline;
	while (fline != label) {
		fin >> fline;
		if ( fline == "[ENDMODELFILE]" ) {
			THROW_FATAL_ERROR("Corrupt file.");
		}
	}
}
