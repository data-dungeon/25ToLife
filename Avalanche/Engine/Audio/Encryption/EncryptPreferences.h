//EncryptPreferences.h
//******************************************************
//contains all of the class declarations for encryption
//preferences that the encryptmotron will use to create
//encrypted media files
//
//******************************************************


#ifndef _ENCRYPTPREFERENCES_H_
#define _ENCRYPTPREFERENCES_H_

#include <list>

#define INPUT_FILE_NAME_SIZE 64

//using namespace std;

class EncryptPreferences 
{
public:
	// constructors and destructors
	~EncryptPreferences();

	// accessors and mutators
	int		getHeaderSize()		{ return headerSize; }
	int		getHeaderOffset()	{ return headerOffset; }
	int		getMagicNumber()	{ return magicNumber; }
	char*	getNextFilename(); 
	
	// operations
	bool readPreferencesFromFile( char* filename );

protected:

	int headerSize;			// headersize in bytes
							// must be at least 4 bytes for magic number
	int headerOffset;		// offset from start to put magic number
	
	int magicNumber;		// magic number to verify file header

	std::list<char*> filenames;	// names read in from file name list

	char* destinationDir;	// directory to place encrypted files

	// helper methods
	bool getIntValueFromString( char* line, char* comparison, int compsize, int &value );

};



#endif
