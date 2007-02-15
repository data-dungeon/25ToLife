//*************************************************************************
// Wave file converter
//*************************************************************************
#ifndef WAVEAT_H
#define WAVEAT_H

#include <ToolHelper\filelist.h>

class WavEat
{
public:
	WavEat(void) { verbose = false; }
	~WavEat(void) {}

	// Do it!
	bool Go(char *commandLineString);

	// Kill the program
	static void HelpExit(void);
	static void ErrorExit(char *reason);

private:
	// Internal proto's
	bool EnumerateFilesDirs(void);

	// process a file
	bool ProcessFile(char *waveFile, char *outDir = 0);
	void BuildOutputFile(const char *waveFile, char *eatFile);
	void BuildOutputFile(const char *waveFile, const char *outDir, char *eatFile);

	//	The command line parser
	CommandLine			commandLine;
	
	// verbose?
	bool					verbose;

	// the directory tree
	FileList				fileList;
};

#endif
