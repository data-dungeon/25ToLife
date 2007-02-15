//FileEncrypter.cpp
//******************************************************
//contains all of the class definitions for encryption
//of a given file type.  
//
//******************************************************

#include "Audio/AudioPCH.h"

#include "FileEncrypter.h"

bool FileEncrypter::BeginEncryption()
{
	// create our header to prepend to the files
	unsigned char* header = new unsigned char[headerSize];
	srand(GetTickCount());
	for (int c=0; c < headerSize; ++c)
	{
		header[c] = rand() & 255;
	}

	char * test2 = new char[10000];


	// place the magic number at the correct position
	int* tempAddress = (int*)(header+headerOffset);
	*tempAddress = magicNumber;

	// start grabbing our files and prepending headers
	char outFileName[OUTPUT_FILENAME_SIZE];
	char errorString[ERROR_STRING_SIZE];
	std::list<char*>::iterator iter = filenames.begin();
	char * test1 = new char[10000];
	FILE* inFile;
	FILE* outFile;
	while (iter != filenames.end())
	{
		char * test3 = new char[10000];
		// open the input file
		strnset(outFileName, 0, OUTPUT_FILENAME_SIZE);
		inFile = fopen(*iter, "rb");
		if (!inFile)
		{
			strcpy(errorString, "error opening file ");
			strcat(errorString, *iter);
			strcat(errorString, "\n");
			setLastError( errorString );
			fclose(inFile);
			return false;
		}
		
		// open the output file
		strncpy(outFileName, *iter, strlen(*iter) - 4 );
		strcat(outFileName, OUTPUT_EXTENSION);
		outFile = fopen(outFileName, "wb");
		if (!outFile)
		{
			strcpy(errorString, "error opening output file ");
			strcat(errorString, outFileName);
			strcat(errorString, "\n");
			setLastError( errorString );
			fclose(inFile);
			return false;
		}

		// write the header
		fwrite(header, headerSize*sizeof(unsigned char), 1, outFile);
		
		//write the contents of the file
		fseek(inFile, 0, SEEK_END);
		int fileSize = ftell(inFile);
		char* buffer = new char[fileSize];
		rewind(inFile);
		fread(buffer, fileSize, 1, inFile);
		fwrite(buffer, fileSize, 1, outFile);
		delete [] buffer;
        
		if (outputCallbackPtr)
		{
			outputCallbackPtr( outFileName );
		}
		// advance our iterator
		++iter;
		// close our files
		fclose(inFile);
		fclose(outFile);
	}
	return true;
}

void FileEncrypter::setLastError
(
	char* errorString
	)
{
	// clear the error string
	strnset(lastError, 0, ERROR_STRING_SIZE);
	strcpy(lastError, errorString);
}

FILE* FileEncrypter::getDecryptedFile( FILE* encryptedFile )
{
	// make sure we have the decryption settings defined
	if (!DECRYPT_HEADER_SIZE || !DECRYPT_HEADER_OFFSET || !DECRYPT_MAGIC_NUMBER)
		return NULL;

	// try to decrypt the file
	// read in the header
	char* header = new char[DECRYPT_HEADER_SIZE];
	fread(header, DECRYPT_HEADER_SIZE*sizeof(char), 1, encryptedFile);
	// check the magic number 
	int* testNumber = (int*)(header + DECRYPT_HEADER_OFFSET);
	if ( *testNumber = DECRYPT_MAGIC_NUMBER ) 
	{
		// success!
		return encryptedFile;
	}
	//not success!
	return NULL;
}


	

