// EncryptPreferences.cpp
//******************************************************
//contains all of the class definitions for the functions
//in EncryptPreferences.h
//
//
//******************************************************

#include "Audio/AudioPCH.h"

#include "EncryptPreferences.h"

//using namespace std;

// the destructor needs to clear the filename list
EncryptPreferences::~EncryptPreferences()
{
	//std::list<char*>::iterator iter = filenames.begin();
	//while (iter != filenames.end())
	//{
	//	delete *iter++;
	//}
	//filenames.clear();
}


// this function will read in the preferences and file
// names to encrypt from the passed in file
// and store the preferences to be read from the
// encryption algorythm
bool EncryptPreferences::readPreferencesFromFile
(
	char* prefsFileName
	)
{
//	int linenum =0;
//	// attempt to open the file
//	FILE* prefsFile = fopen(prefsFileName, "r");
//#define FILE_READ_FAILURE														\
//	{																			\
//		if (prefsFile) fclose(prefsFile);										\
//		return false;															\
//	}																			
//	if (!prefsFile) FILE_READ_FAILURE;
//
//	// loop until we reach the end of the file,
//	// reading in the information we need
//	char nextLine[256];
//	while ( !feof(prefsFile) )
//	{
//		// get our next line
//		strnset( nextLine, 0, 256 );
//		if ( !fgets(nextLine, 256, prefsFile) ) continue;
//		linenum++;
//
//		// skip comments
//		if ( !strncmp( "//", nextLine, 2) ) continue;
//		
//		// look for file header size
//		else if ( getIntValueFromString( nextLine, "header size =", 13, headerSize ) )
//		{
//			if ( headerSize >= 4) continue;
//			else FILE_READ_FAILURE;
//		}
//		// and then header offset
//		else if ( getIntValueFromString( nextLine, "header offset =", 15, headerOffset ) ) continue;
//		// and then magic number
//		else if ( getIntValueFromString( nextLine, "magic number =", 14, magicNumber ) ) continue;
//		// and then our file name strings;
//		else if ( !strncmp( "file names =", nextLine, 12) )
//		{
//			char inputFileName[INPUT_FILE_NAME_SIZE];
//			// read the next line from the file and expect a "{"
//			if ( !fgets( inputFileName, INPUT_FILE_NAME_SIZE, prefsFile ) ) FILE_READ_FAILURE;
//			linenum++;
//			if ( inputFileName[0] != '{' ) FILE_READ_FAILURE;
//
//			// read in file names and add them to the list
//			strnset(inputFileName, 0, INPUT_FILE_NAME_SIZE);
//			fgets(inputFileName, INPUT_FILE_NAME_SIZE, prefsFile);
//
//			while ( inputFileName[0] != '}' )
//			{
//				char * entry = new char[INPUT_FILE_NAME_SIZE];
//				// format the file name so we can open it in 
//				// the encryption program
//				strnset(entry, 0, INPUT_FILE_NAME_SIZE);
//				strncpy(entry, inputFileName, strlen(inputFileName) - 1);
//				filenames.push_back(entry);
//				strnset(inputFileName, 0, INPUT_FILE_NAME_SIZE);
//				fgets(inputFileName, INPUT_FILE_NAME_SIZE, prefsFile);
//			}
//		}
//		else FILE_READ_FAILURE;
//	}
//	fclose(prefsFile);
	return true;
}

// helper files

// gets a nonzero integer value from the string if the
// comparison string is found
bool EncryptPreferences::getIntValueFromString
( 
	char* line,
	char* comparison,
	int compsize,
	int& value 
	)
{
	//if ( !strncmp( comparison, line, compsize) )
	//{
	//	int tempValue = 0;
	//	// read in the int value
	//	tempValue= atoi(line+compsize);
	//	if ( !tempValue )
	//	{
	//		// try again to support a space after the ='s sign
	//		tempValue= atoi(line+compsize+1);
	//		// ok, now we can just die
	//		if ( !tempValue ) return false;
	//	}
	//	value = tempValue;
	//	return true;
	//}
	return false;
}

