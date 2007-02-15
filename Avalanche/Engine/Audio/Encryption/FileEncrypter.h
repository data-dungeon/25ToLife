//FileEncrypter.h
//******************************************************
//contains all of the class declarations for encryption
//and decryption of a files.  Derives from 
//EncryptPreferences class
//
//******************************************************


#ifndef _FILEENCRYPTER_H_
#define _FILEENCRYPTER_H_

#include "EncryptPreferences.h"

// #defines
#define OUTPUT_EXTENSION		".emt"
#define ERROR_STRING_SIZE		256
#define OUTPUT_FILENAME_SIZE	64

// define these when you know your encryption settings
#define DECRYPT_HEADER_SIZE		137		
#define DECRYPT_HEADER_OFFSET	42
#define DECRYPT_MAGIC_NUMBER	10171980


class FileEncrypter : public EncryptPreferences
{
public:
	// accessors/mutators
	FILE* getDecryptedFile( char* filename);
	FILE* getDecryptedFile( FILE* encryptedFile);
	char* getLastError() { return lastError; }
	void setLastError( char* errorString );
	void setCallbackFunction( void functionPtr(char*) ) { outputCallbackPtr = functionPtr; }
	// operations
	bool BeginEncryption();
protected:
	char lastError[ERROR_STRING_SIZE];
	void (*outputCallbackPtr)(char *);
};

#endif

