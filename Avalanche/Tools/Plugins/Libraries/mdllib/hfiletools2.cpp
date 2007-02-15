#include "stdafx.h"
#include "hfileTools.h"
#include "hfileTools2.h"
#include "hmacros.h"
#include "uberstring.h"

#include "hfiletools2.h"

void fileTools2::loopForAttr(std::ifstream &fin, UString &attr, HArgArray &args, UString &EOB) {
	UString fline,lbl;
//	unsigned long pos = fin.tellg()-1;
	unsigned long pos = fin.tellg();
	fin.seekg(pos);
	do {
		fin >> fline;
		if (fline != "") {
			if ( fline == EOB ) {
				THROW_FATAL_ERROR("Corrupt file. [findlabel]");
			}
			fileTools::parseLine(fline,lbl,args);
		}
		else
			lbl = "";
	} while ( lbl != attr );
	fin.seekg(pos);
}

void fileTools2::loopForAttr(std::ifstream &fin, UString attr, HArgArray &args, UString EOB) {
	UString fline,lbl;
//	unsigned long pos = fin.tellg()-1;
	unsigned long pos = fin.tellg();
	fin.seekg(pos);
	do {
		fin >> fline;
		if (fline != "") {
			if ( fline == EOB ) {
				THROW_FATAL_ERROR("Corrupt file. [findlabel]");
			}
			fileTools::parseLine(fline,lbl,args);
		}
		else
			lbl = "";
	} while ( lbl != attr );
	fin.seekg(pos);
}

void fileTools2::goToAttr(std::ifstream &fin, UString &attr, HArgArray &args, UString &EOB) {
	UString fline,lbl;
	do {
		fin >> fline;
		if ( fline == EOB ) {
			THROW_FATAL_ERROR("Corrupt file. [findlabel]");
		}
		fileTools::parseLine(fline,lbl,args);
	} while ( lbl != attr );
}

void fileTools2::goToAttr(std::ifstream &fin, UString attr, HArgArray &args, UString EOB) {
	UString fline,lbl;
	do {
		fin >> fline;
		if ( fline == EOB ) {
			THROW_FATAL_ERROR("Corrupt file. [findlabel]");
		}
		fileTools::parseLine(fline,lbl,args);
	} while ( lbl != attr );
}

void fileTools2::goToLabel(std::ifstream &fin, UString &label, UString &EOB) {
	UString fline,lbl;
	do {
		fin >> fline;
		if ( fline == EOB ) {
			THROW_FATAL_ERROR("Corrupt file. [findlabel]");
		}
	} while ( fline != label );
}

void fileTools2::goToLabel(std::ifstream &fin, UString label, UString EOB) {
	UString fline,lbl;
	do {
		fin >> fline;
		if ( fline == EOB ) {
			THROW_FATAL_ERROR("Corrupt file. [findlabel]");
		}
	} while ( fline != label );
}

bool fileTools2::doesLabelExist(std::ifstream &fin, UString &label, UString &EOB) {
	UString fline,lbl;
	do {
		fin >> fline;
		if ( fline == label )
			return true;
	} while ( fline != EOB );
	return false;
}

bool fileTools2::doesLabelExist(std::ifstream &fin, UString label, UString EOB) {
	UString fline,lbl;
	do {
		fin >> fline;
		if ( fline == label )
			return true;
	} while ( fline != EOB );
	return false;
}
