///////////////////////////////////////////////////////////////////////////////
// Wee!
///////////////////////////////////////////////////////////////////////////////
#ifndef FILESTRESS_H
#define FILESTRESS_H

class FileStress
{
public:
	// You must call LoadMaster or use Test(fileName) later
	FileStress();

	// load the master copy of the file
	FileStress(const char *fileName);

	// Done
	~FileStress();

	// Load the master copy of the file (only does it once)
	bool LoadMaster(const char *fileName);

	// Read the file again and test the results
	bool Test();

	// Just call this is you used the default constructor
	bool Test(const char *fileName)
		{ LoadMaster(fileName); return Test(); }

	// Free everyting
	void Unload();

private:
	char			m_filename[128];
	char			*m_data;
	int			m_size;
};

#endif
