////////////////////////////////////////////////////////////////////////////
//
// LoadSCFiles
//
// Class to handle getting texture names from Dan's front end tool sc files
//
////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////

#ifndef LOAD_SC_FILES_H
#define LOAD_SC_FILES_H

class LoadSCFiles
{
	TupArray<TupString> m_filePathEntries;

public:
	LoadSCFiles(void);
	~LoadSCFiles();

	void ScanSCFile(const TupString &filePathName,TupArray<TupString> &textureFileNames);
};

#endif
