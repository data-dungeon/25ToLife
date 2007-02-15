//*************************************************************************
// Command console variable file builder
//*************************************************************************
#ifndef VARPACK_H
#define VARPACK_H

#include <container\darray.h>
#include <platform\win32\winapp.h>
#include <toolhelper\dynstr.h>
#include <toolhelper\filelist.h>
#include <cmdcon\scrtoc.h>

#include "stats.h"
#include "statsdlg.h"

class VarPack : public WindowsApp
{
public:
	VarPack(void);
	~VarPack(void);

	// Initialize
	bool InitInstance(HINSTANCE hPrevInst, LPSTR cmdLine, int nCmdShow);

	// Terminate
	void TerminateInstance(void);

	// Called every processing "frame"
	void Process(void);

	// Do we handle this message for an accelerator/dlg
	bool HandleMessage(MSG &msg);

private:
	// Internal proto's
	bool ParseCommandLine(char *cmdLine);
	bool ErrorExit(char *reason, bool ret = false);

	// process a script
	bool ProcessScript(char *script);

	// Write the datafile
	bool WriteFile(char *outFile);

	// The main parameters
	char 									*startDir;
	char 									*outfile;

	// the directory tree
	FileList								fileList;

	// the data, ready to go
	DynamicArray<SCRIPTTOC_ENTRY>	contents;
	static int __cdecl TOC_Compare(const void *e1, const void *e2);

	// Our status display
	Stats									stats;
	StatsDisplayDialog				*statsDlg;
	bool									waitForClose;
};

#endif
