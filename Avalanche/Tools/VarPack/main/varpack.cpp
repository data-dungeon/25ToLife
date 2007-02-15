//***************************************************************************
//***************************************************************************
#include <windows.h>
#include <assert.h>
#include <stdio.h>

#include <toolhelper\cmdline.h>
#include <toolhelper\pathfile.h>

#include "varpack.h"
#include "resource.h"

// Bootstrap ourselves
VarPack varPack;
WindowsApp *WindowsApp::me = &varPack;

// The command line parser
CommandLine commandLine;

//***************************************************************************
//***************************************************************************
VarPack::VarPack(void)
{
	startDir = outfile = 0;

	statsDlg = NULL;
	waitForClose = false;
}

//***************************************************************************
//***************************************************************************
VarPack::~VarPack(void)
{
}

//***************************************************************************
// Init everything, load the world, convert, exit
//***************************************************************************
bool VarPack::InitInstance(
HINSTANCE hPrevInst,
LPSTR cmdLine,
int nCmdShow)
{
	// Did the fully specify the conversion?
	if (!ParseCommandLine(cmdLine))
		return(ErrorExit("Usage:\nVarPack -d <startdir> -o <outputfile> [-w (wait)]"));

	// Load the directory tree
	if (!fileList.Build(startDir, "var", true))
		return(ErrorExit("Cannot find directory."));

	if (fileList.Count() == 0)
		return(ErrorExit("No files found."));

	// Create the stats dlg
	g_stats = &stats;
	stats.Clear();
	stats.startDir = startDir;
	stats.outputFile = outfile;
	stats.totalFiles = fileList.Count();
	stats.SetStatus("Converting scripts...");
	statsDlg = new StatsDisplayDialog(hInst, hWnd, stats);
	if (statsDlg)
		statsDlg->Update();

	return(true);
}

//***************************************************************************
// Clean up
//***************************************************************************
void VarPack::TerminateInstance(void)
{
	delete statsDlg;
}

//***************************************************************************
// Called every processing "frame"
//***************************************************************************
void VarPack::Process(void)
{
	// Process returns true when done
	if (!stats.done)
	{
		// Process a file
		DynString *script = fileList.Get(stats.onFile);
		stats.SetFile(script->Data());
		ProcessScript(script->Data());

		// are we done
		++stats.onFile;
		stats.done = (stats.onFile == stats.totalFiles);
		stats.percentageDone = (float)stats.onFile / (float)stats.totalFiles * 100.0f;

		if (stats.done)
		{
			// Change the message
			if (statsDlg)
			{
				stats.SetStatus("Writing output file...");
				statsDlg->Update();
			}

			// Write it to a database when done
			if (!WriteFile(outfile))
				ErrorExit("Cannot write output file!");

			// We are done
			if (!waitForClose)
				Quit();
			else
			{
				// Wait for them to press it
				stats.SetStatus("Done.");
				if (statsDlg)
					statsDlg->Update();
			}
		}
	}

	// Update the stats dialog
	if (!stats.done && statsDlg)
		statsDlg->Update();

	// Did they press cancel?
	if (statsDlg && statsDlg->Quit())
		Quit();
}

//***************************************************************************
//***************************************************************************
bool VarPack::ParseCommandLine(
char *cmdLine)
{
	// Call the parse
	if (!commandLine.Parse(cmdLine))
		return(false);

	// Get the options into locals
	startDir = commandLine.GetParam('d');
	if (!startDir || *startDir == '\0')
		return(false);

	// get the output file
	DynString *tempStr = commandLine.GetParamDynString('o');
	if (!tempStr)
		return(false);
	if (!PathFile::HasExt(tempStr->Data()))
		tempStr->Cat(".vap");
	outfile = tempStr->Data();

	// Do we need to wait?
	if (commandLine.GetParam('w') != NULL)
		waitForClose = true;

	// Good command line!
	return(true);
}

//*************************************************************************
// Here's were you put your accelerators
//*************************************************************************
bool VarPack::HandleMessage(
MSG &msg)
{
	if (statsDlg)
		return(statsDlg->HandleMessage(msg));
	else
		return(WindowsApp::HandleMessage(msg));
}

//***************************************************************************
// Exit the program with a reason
//***************************************************************************
bool VarPack::ErrorExit(
char *reason,
bool ret)
{
	MessageBox(hWnd, reason, "Error!", MB_ICONHAND | MB_OK);
	return(ret);
}

//***************************************************************************
// process a script
//***************************************************************************
bool VarPack::ProcessScript(
char *script)
{
	// Filter off a leading ./
	if (strncmp(script, ".\\", 2) == 0)
		script += 2;

	// add it's name to the contents
	SCRIPTTOC_ENTRY e;
	e.name = _strlwr(script);

	// Load in the script
	FILE *fh = fopen(script, "rt");
	if (!fh)
		return(false);

	// Remove the extension (if it's .var)
	int len = strlen(e.name);
	if (len > 3 && strncmp(&e.name[len - 4], ".var", 4) == 0)
		e.name[len - 4] = '\0';

	// How big?
	fseek(fh, 0, SEEK_END);
	uint size = ftell(fh);
	if (size == 0)
	{
		fclose(fh);
		return(true);
	}
	fseek(fh, 0, SEEK_SET);

	// set it up
	e.data = new char [size];
	e.data[0] = '\0';

	// copy in non-commented lines
	char line[1024];
	char mainContext[64];
	mainContext[0] = '\0';
	while (fgets(line, sizeof(line) - 1, fh))
	{
		// skip leading blanks
		char *parse = line;
		while (*parse == ' ' || *parse == '\t')
			++parse;

		// Comment or blank line?
		if (*parse == '#' || *parse == '\0' || *parse == '\n')
			continue;

		// parse up to the dot
		char *context = parse;
		while (*parse != '.' && *parse != '\0')
			++parse;
		uint contextLen = (uint)(parse - context);

		// do we have the context yet?
		if (*mainContext == '\0')
		{
			strncpy(mainContext, context, contextLen);
			mainContext[contextLen] = '\0';
			strcat(e.data, "context ");
			strcat(e.data, mainContext);
			strcat(e.data, "\n");
		}


		// make sure we match!!!
		if (strncmp(context, mainContext, strlen(mainContext)) != 0)
		{
			// crap, no match write the whole line
			strcat(e.data, context);
		}
		else
		{
			// Skip the
			if (*parse == '.')
				++parse;

			// add into the main buffer
			strcat(e.data, parse);
		}
	}

	// eol
	strcat(e.data, "end\n");

	// lower it
	_strlwr(e.data);

	// fixup the size
	stats.totalSize += strlen(e.data);

	// Add it to the toc
	contents.Add(e);

	fclose(fh);
	return(true);
}

//***************************************************************************
//***************************************************************************
int __cdecl VarPack::TOC_Compare(
const void *e1,
const void *e2)
{
	const char *s1 = ((SCRIPTTOC_ENTRY *)e1)->name;
	const char *s2 = ((SCRIPTTOC_ENTRY *)e2)->name;

	int cmp = strcmp(s1, s2);

	return(cmp);
}

//***************************************************************************
// Write the datafile
//***************************************************************************
bool VarPack::WriteFile(
char *outFile)
{
	// Make sure this is gone
	remove(outFile);

	// anything to do
	if (contents.Count() == 0)
		return(true);

	// sort it
	qsort(contents.GetPtr(), contents.Count(), sizeof(SCRIPTTOC_ENTRY), TOC_Compare);

	// Make a header
	SCRIPTTOC_HDR hdr;
	memset(&hdr, '\0', sizeof(hdr));

	hdr.version = SCRIPTTOC_CURRENTVERSION;
	hdr.entries = contents.Count();

	// Open the file
	FILE *fh = fopen(outFile, "wb");
	if (!fh)
		return(false);

	// write the header
	if (fwrite(&hdr, sizeof(hdr), 1, fh) != 1)
	{
		fclose(fh);
		return(false);
	}

	// Make a "patched" version of teh TOC
	SCRIPTTOC_ENTRY *temp = new SCRIPTTOC_ENTRY [hdr.entries];
	memset(temp, '\0', sizeof(SCRIPTTOC_ENTRY) * hdr.entries);

	uint dataOffset = sizeof(hdr) + sizeof(SCRIPTTOC_ENTRY) * hdr.entries;
	uint i;
	for (i = 0; i < hdr.entries; i++)
	{
		temp[i].name = (char *)dataOffset;
		dataOffset += strlen(contents[i].name) + 1;
		temp[i].data = (char *)dataOffset;
		dataOffset += strlen(contents[i].data) + 1;
	}

	// Write the entries
	if (fwrite(temp, sizeof(SCRIPTTOC_ENTRY), hdr.entries, fh) != hdr.entries)
	{
		fclose(fh);
		return(false);
	}

	// write all the data
	bool ret = true;
	for (i = 0; i < hdr.entries && ret; i++)
	{
		uint size = strlen(contents[i].name) + 1;
		if (fwrite(contents[i].name, 1, size, fh) != size)
			ret = false;

		size = strlen(contents[i].data) + 1;
		if (fwrite(contents[i].data, 1, size, fh) != size)
			ret = false;
	}

	return(ret);
}

