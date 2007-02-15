//***************************************************************************
//***************************************************************************
#include <assert.h>
#include <stdio.h>
#include <direct.h>
#include <conio.h>

#define NOLAYERS

#include <toolhelper\pathfile.h>
#include <toolhelper\writepad.h>

// Define this for using a "real" CRC
//#define REALCRC
#ifdef REALCRC
#include "crc.h"
#else
#include "hash.h"
#endif

#include "byteswap.h"
#include "helix.h"
#include "hog.h"

//***************************************************************************
//***************************************************************************
int main(
int argc,
char *argv[])
{
	// Create it!
	Hog hog(argc, argv);

	// Do the work
	hog.Process();

	// Weee
	return 0;
}

//********7*******************************************************************
//***************************************************************************
Hog::Hog(
int argc,
char *argv[]) :
pathFilter(8, 8, false),
originalFile(256, 64, false)
{
	// Set the defaults
	outfile = NULL;
	reportfile = NULL;
	listfile = NULL;
	baseDir = NULL;
	startDir = NULL;
	alignment = 16;
	justFileName = false;
	beQuiet = false;
	outputBigEndian = false;
	encrypt = false;
	waitOnError = false;
	totalFileSize = 0;

	// Header
	stats.Echo("Whole Hog Version 10.2, by Food\n");

	// Did the fully specify the conversion?
	if (!ParseCommandLine(argc, argv))
		UsageExit();

	// Doh
	if (beQuiet)
		stats.SetVerbosity(Stats::VERBOSITY_ERROR);

	// Load the directory tree
	if (!EnumerateFilesDirs())
		ErrorExit("At least one -f <file> -d <add dir> or -r <recurse dir> and one -m <mask> must be specified.");

	// Delete the exclusions
	EnumerateKills();
	EnumerateSubstringKills();

	// show the alignment
	stats.Echo("Data alignment %d\n", alignment);
}

//***************************************************************************
// Shouldn't this do something?
//***************************************************************************
Hog::~Hog()
{
}

//***************************************************************************
// Called every processing "frame"
//***************************************************************************
void Hog::Process()
{
	// Setup the progress
	stats.SetProgressText("Building...");

	// Process returns true when done
	int onFile = 0;
	int totalFiles = fileList.Count();

	if (totalFiles == 0)
		ErrorExit("No files specified or found to hog!");

	while (onFile < totalFiles)
	{
		// Process a file
		DynString *file = fileList.Get(onFile++);
		ProcessFile(file->Data());
		stats.UpdateProgress(onFile, totalFiles);
	}

	// Change the message
	stats.SetProgressText("Writing output...");

	// Write it to a database when done
	if (!WriteFile(outfile))
		ErrorExit("Cannot write output file!");

	if (!WriteReport(reportfile))
		ErrorExit("Cannot write report file!");

	if (!WriteListing(listfile))
		ErrorExit("Cannot write listing file!");

	stats.Echo("Success!\n");
}

//***************************************************************************
//***************************************************************************
bool Hog::ParseCommandLine(
int argc,
char *argv[])
{
	// Call the parse
	if (!commandLine.Parse(argc, argv))
		return false;

	// First things first
	if (commandLine.GetParam('h') != NULL)
		UsageExit(true, 0);

	// get the output file
	DynString *tempStr = commandLine.GetParamDynString('o');
	if (!tempStr)
		return false;
	if (!PathFile::HasExt(tempStr->Data()))
		tempStr->Cat(".hog");
	outfile = tempStr->Data();

	// get the report file
	tempStr = commandLine.GetParamDynString('v');
	if (tempStr)
	{
		if (!PathFile::HasExt(tempStr->Data()))
			tempStr->Cat(".log");
		reportfile = tempStr->Data();
	}

	// get the listing file
	tempStr = commandLine.GetParamDynString('l');
	if (tempStr)
	{
		if (!PathFile::HasExt(tempStr->Data()))
			tempStr->Cat(".lst");
		listfile = tempStr->Data();
	}

	// base dir
	tempStr = commandLine.GetParamDynString('b');
	if (tempStr)
	{
		int len = tempStr->Len();
		if (len > 0 && tempStr->Data()[len - 1] != '\\')
			tempStr->Cat("\\");
		baseDir = tempStr->Data();
	}

	// start dir
	tempStr = commandLine.GetParamDynString('s');
	if (tempStr)
		startDir = tempStr->Data();

	// Alignment
	tempStr = commandLine.GetParamDynString('a');
	if (tempStr)
	{
		// Get it
		alignment = atoi(tempStr->Data());
		if (alignment < 0)
			alignment = 16;

		// Make sure this is a power of two
		if (alignment != 0)
		{
			int a = alignment;
			int onPower = 0;
			int highestPower = -1;
			while (a)
			{
				if (a & 1)
					highestPower = onPower;
				++onPower;
				a = (a >> 1);
			}

			ASSERT(highestPower != -1);
			alignment = (1 << highestPower);
		}
	}

	// Files only?
	if (commandLine.GetParam('f') != NULL)
		justFileName = true;

	// quiet?
	if (commandLine.GetParam('q') != NULL)
		beQuiet = true;
	if (commandLine.GetParam('w') != NULL)
		waitOnError = true;

	// endian
	tempStr = commandLine.GetParamDynString('e');
	if (tempStr)
	{
		if (stricmp("big", tempStr->Data()) == 0)
			outputBigEndian = true;
		if (stricmp("ncrypt", tempStr->Data()) == 0)
			encrypt = true;
	}

	// Enumerate the path filters
	EnumeratePathFilters();

	// Good command line!
	return true;
}

//*************************************************************************
// Load the directory tree
//*************************************************************************
bool Hog::EnumerateFilesDirs()
{
	// Switch to the start dir
	if (startDir && _chdir(startDir) == 0)
		stats.Echo("Changing directory: %s\n", startDir);

	uint dirsFiles = 0;
	for (uint p = 0; p < commandLine.Parameters(); p++)
	{
		CommandLineParameter *param = commandLine.GetParam(p);

		// Is this a mask parameter?
		char *fileDir = param->Value().Data();

		// single file?
		if (param->Switch() == '\0')
		{
			if (!fileList.AddFile(fileDir))
				AddError("Couldn't add file: ", fileDir);
			++dirsFiles;
		}
		else if (param->Switch() == 'r' || param->Switch() == 'd')
		{
			// Scanning a directory
			if (!EnumerateMasks(fileDir, param->Switch() == 'r'))
				AddError("Coultn't add dir: ", fileDir);
			// got a dir!
			++dirsFiles;
		}
	}

	return(dirsFiles > 0);
}

//*************************************************************************
// Add the masks for this directory
//*************************************************************************
bool Hog::EnumerateMasks(
char *forDir,
bool recurse)
{
	uint masks = 0;
	DynString maskStr("");
	for (uint p = 0; p < commandLine.Parameters(); p++)
	{
		CommandLineParameter *param = commandLine.GetParam(p);

		// Is this a mask parameter?
		if (param->Switch() != 'm')
			continue;

		// Make sure they didn't put a  *.something
		char *ext = param->Value().Data();
		if (*ext == '*')
			ext += 2;

		// Update the stats
 		if (masks != 0)
 			maskStr.Cat(", ");
		maskStr.Cat(param->Value().Data());

		// Special case *.* = NULL
		if (*ext == '*')
			ext = NULL;

		// Build it
		stats.Echo("Enumerating %s with masks \"%s\"\n", forDir, maskStr.Data());
		if (!BuildDir(forDir, ext, recurse))
			return false;

		++masks;
	}

	// No masks?
	if (masks == 0)
	{
		// Build the whole thing
		stats.Echo("Enumerating %s for \"*.*\"\n", forDir);
		if (!BuildDir(forDir, NULL, recurse))
			return false;
	}

	return true;
}

//*************************************************************************
//*************************************************************************
bool Hog::EnumerateKills()
{
	DynString kills("");
	for (uint p = 0; p < commandLine.Parameters(); p++)
	{
		CommandLineParameter *param = commandLine.GetParam(p);

		// Is this a mask parameter?
		if (param->Switch() != 'k')
			continue;

		// Make sure they didn't put a  *.something
		char *ext = param->Value().Data();
		if (*ext == '*')
			ext += 2;

		// Special case *.*, are they dumb?
		if (*ext == '*')
			ErrorExit("Yo, fool, yeah you! Don't kill all the files you added.");

		// Add it
		if (kills.Len())
			kills.Cat(", ");
		kills.Cat(ext);

		// Build it
		fileList.DeleteFiles(ext);
	}

	if (kills.Len())
		stats.Echo("Killing: %s\n", kills.Data());

	return true;
}

//*************************************************************************
//*************************************************************************
bool Hog::EnumerateSubstringKills()
{
	DynString kills("");
	for (uint p = 0; p < commandLine.Parameters(); p++)
	{
		CommandLineParameter *param = commandLine.GetParam(p);

		// Is this a mask parameter?
		if (param->Switch() != 'x')
			continue;

		// Make sure they put something
		char *substring = param->Value().Data();
		if (!substring || *substring == '\0')
			ErrorExit("Malformed substring exterminate.");

		// Add it
		if (kills.Len())
			kills.Cat(", ");
		kills.Cat("\"");
		kills.Cat(substring);
		kills.Cat("\"");

		// Build it
		fileList.DeleteSubstring(substring);
	}

	if (kills.Len())
		stats.Echo("Killing substrings: %s\n", kills.Data());

	return true;
}

//*************************************************************************
//*************************************************************************
bool Hog::EnumeratePathFilters()
{
	for (uint p = 0; p < commandLine.Parameters(); p++)
	{
		CommandLineParameter *param = commandLine.GetParam(p);

		// Is this a mask parameter?
		if (param->Switch() == 'p')
		{
			DynString *filter = new DynString(param->Value());
			_strupr(filter->Data());
			int len = filter->Len();
			if (filter->Data()[len - 1] != '\\')
				filter->Cat("\\");
			pathFilter.Add(filter);
		}
	}

	return true;
}

//*************************************************************************
//*************************************************************************
bool Hog::BuildDir(
char *dir,
char *ext,
bool recurse)
{
	if (fileList.Build(dir, ext, recurse))
		return true;

	// error!
	AddError("Cannot find directory: ", dir, true);
	return false;
}

//***************************************************************************
// Exit the program with a reason
//***************************************************************************
void Hog::ErrorExit(
char *reason,
int code)
{
	printf("Error: %s!\n\n", reason);

	if (waitOnError)
	{
		// Read a key
		printf("Press any key to continue...");
		_getch();
		printf("\n");
	}

	exit(code);
}

//***************************************************************************
//***************************************************************************
void Hog::UsageExit(
bool full,
int code)
{
	stats.Echo("\n");
	stats.Echo("Hog -o <outputfile> <file> (<file> ...)\n");
	stats.Echo("	[-d <add dir> (-d <add dir> ...)\n");
	stats.Echo("	-r <recurse dir> (-r <recurse dir> ...)\n");
	stats.Echo("	-m <mask> (-m <mask> ...)\n");
	stats.Echo("	-k <kill mask> (-k <kill mask>\n");
	stats.Echo("	-x <kill substring> (-x <kill substring>\n");
	stats.Echo("	-v <verbose dump>\n");
	stats.Echo("	-l <listing>\n");
	stats.Echo("	-a <powerOf2>\n");
	stats.Echo("	-e <big>\n");
	stats.Echo("	-e <ncrypt>\n");
	stats.Echo("	-b <baseDir>\n");
	stats.Echo("	-s <startDir>\n");
	stats.Echo("	-p <filterPath> (-p <filterPath>)]\n");
	stats.Echo("	-filesonly (filenames only in TOC, no paths)\n");
	stats.Echo("	-help (show full help)\n");
	stats.Echo("	-quiet (be quiet!)\n");
	stats.Echo("	-wait (wait for key on fatal errors!)\n");
	stats.Echo("\n");
	if (full)
	{
		stats.Echo("-o specifies the final .hog file (if you leave the .hog off, it adds it)\n");
		stats.Echo("\n");
		stats.Echo("<file> you can have any number of files directly listed on the command line\n");
		stats.Echo("-d adds a directory of files, any number of -d's can be specified\n");
		stats.Echo("-r recursive adds a directory of files, and number of -r's can be specified\n");
		stats.Echo("\n");
	             //*****************************************************************************
		stats.Echo("The ordering of the switches is relevant: they are executed in left to right\n");
		stats.Echo("order. If duplicate files arise (using the -p option below) the first (or \n");
		stats.Echo("leftmost) file is kept while the latter is discarded.\n");
		stats.Echo("\n");
		stats.Echo("-m specifies a allowable mask, any number of masks can be specified.  If no\n");
		stats.Echo("   mask isspecified, *.* is assumed. Masks are not fully implemented, just\n");
		stats.Echo("   *.<ext> or just <ext>. No question marks or partial extensions are allowed.\n");
		stats.Echo("-k specifies what files to not add, any number of kill masks can be specified.\n");
		stats.Echo("-x specifies a substring to search for in a path/file for removal.\n");
		stats.Echo("\n");
		stats.Echo("-v Gives a verbose dump of the resulting -hog file.  If no extension is given,\n");
		stats.Echo("   .log is used.\n");
		stats.Echo("-l makes a response file that has all files listed explicitly, along with your\n");
		stats.Echo("   command line switches. If no extension is given, .lst is used.\n");
		stats.Echo("-a Specifies a byte alignment for all data (must be a power of two), it is\n");
		stats.Echo("   recommened you set this to 16 for the PS2.\n");
		stats.Echo("-e big makes the data compatible with the Gamecube.\n");
		stats.Echo("-e ncrypt encrypts the data.\n");
		stats.Echo("-b specifies a \"base dir\" to be prepended on all paths in the hog.\n");
		stats.Echo("-s specifies a directory to \"cd\" into before starting the build.\n");
		stats.Echo("-p specifies a beginning part of the path that should be removed.  Say for\n");
		stats.Echo("   instance if you don't want \"data\\\" preceeding all files, you would say\n");
		stats.Echo("   \"-p data\".  You can specify multiple -p's\n");
		stats.Echo("-filesonly instructs hog to not store any path information with files. All\n");
		stats.Echo("   files are stored without any leading path info.\n");
		stats.Echo("\n");
		stats.Echo("A sample command line for making Sensei's data directory:\n");
		stats.Echo("\n");
		stats.Echo("hog -o data -r data_PC -r data -k *.scc -k *.dbc -k *.xls -k *.h -k *.pdb -k *.mdl -p data -p data_PC -v data -l data\n");
	}

	exit(code);
}

//***************************************************************************
// process a script
//***************************************************************************
bool Hog::ProcessFile(
char *file)
{
	// Load in the file
	FILE *fh = fopen(file, "rb");
	if (!fh)
	{
		AddError("Cannot open: ", file);
		return false;
	}

	// Create an entry for it
	TOCENTRY e;
	memset(&e, '\0', sizeof(e));

	// setup the path
	int len = strlen(file);
	if (baseDir)
		len += strlen(baseDir);
	e.name = new char [len + 1];
	if (!e.name)
	{
		AddError("Out of memory adding: ", file);
		fclose(fh);
		return false;
	}

	// Apply the base dir
	if (baseDir == NULL)
		strcpy(e.name, file);
	else
	{
		strcpy(e.name, baseDir);
		strcat(e.name, file);
	}

	// uppercase it (damn PS2 file system)
	_strupr(e.name);

	// clean it
	if (justFileName)
		PathFile::JustFileName(e.name, e.name, true);
	else
	{
		// Clean off the crap in the path
		PathFile::CleanPathFile(e.name);

		// Apply the path filters
		FilterPath(e.name);
	}

	// Do we have a dup?
	int exists = FileExistsInTOC(e.name);
	if (exists > 0)
	{
		char temp[256];
		if (strcmpi(file, originalFile[exists]->Data()) == 0)
			sprintf(temp, "Dup'd file \"%s\".", file);
		else
			sprintf(temp, "Duplicate file \"%s\",\r\n\t using \"%s\".", file, originalFile[exists]->Data());
		stats.Echo(temp);
		delete [] e.name;
		return true;
	}

	// How big is it?
	fseek(fh, 0, SEEK_END);
	e.size = ftell(fh);
	if (e.size == 0)
	{
		AddError("Warning: zero length file ", file, true);
		fclose(fh);
		bool ret = contents.Add(e);
		ASSERT(ret);
		if (ret)
		{
			DynString *s = new DynString(file);
			_strlwr(s->Data());
			bool ret = originalFile.Add(s);
			ASSERT(ret);
		}
		return true;
	}

	// create the array
	e.data = new char [e.size];
	ASSERT(e.data);
	if (!e.data)
	{
		AddError("Out of memory adding: ", file);
		fclose(fh);
		return false;
	}

	// Load it in
	fseek(fh, 0, SEEK_SET);
	if (fread(e.data, e.size, 1, fh) != 1)
	{
		AddError("Error reading: ", file);
		fclose(fh);
		return false;
	}

	// done with the file
	fclose(fh);

	// fixup the size
	totalFileSize += e.size;

	// Do the encryltion
	if (encrypt)
	{
		DWORD betterBeZero = g_helix.GetEncryptionHeaderSize();
		DWORD encryptedSize = g_helix.GetEncryptionSize((const BYTE *)e.data, e.size);
		if (betterBeZero != 0 || encryptedSize != e.size)
			ErrorExit("Encryption API has changed for the worse!");

		// Do it!
		g_helix.Encrypt((const BYTE *)e.data, e.size, (BYTE *)e.data);
	}

	// Compute the crc
	e.crc = ComputeCRC(e.data, e.size);

	// Add it to the contents
	bool ret = contents.Add(e);
	ASSERT(ret);
	if (ret)
	{
		DynString *s = new DynString(file);
		_strlwr(s->Data());
		bool ret = originalFile.Add(s);
		ASSERT(ret);
	}

#ifdef _DEBUG
	// checkIt checkIt
	int tocCount = contents.Count();
	int fileCount = originalFile.Count();
	ASSERT(tocCount == fileCount);
	for (int checkIt = 0; checkIt < contents.Count(); checkIt++)
	{
		char *tocFile = contents[checkIt].name;
		int tocFileLen = strlen(tocFile);
		char *fileName = originalFile[checkIt]->Data();
		int len = strlen(fileName);
		ASSERT(strcmpi(tocFile, &fileName[len - tocFileLen]) == 0);
	}
#endif

	return(ret);
}

//***************************************************************************
// Apply the path filters
//***************************************************************************
void Hog::FilterPath(
char *file)
{
	for (int p = 0; p < pathFilter.Count(); p++)
	{
		DynString *filter = pathFilter[p];
		int filterLen = strlen(filter->Data());
		int fileLen = strlen(file);
		if (filterLen < fileLen && strncmp(filter->Data(), file, filterLen) == 0)
		{
			strcpy(file, &file[filterLen]);
			return;
		}
	}
}

//***************************************************************************
// Do we have a dup?
//***************************************************************************
int Hog::FileExistsInTOC(
const char *file)
{
	for (int c = 0; c < contents.Count(); c++)
	{
		TOCENTRY &e = contents[c];
		if (strcmp(file, e.name) == 0)
			return(c);
	}
	return(-1);
}

//***************************************************************************
// Write the datafile
//***************************************************************************
bool Hog::WriteFile(
char *outFile)
{
	// Make sure this is gone
	remove(outFile);

	// anything to do
	if (contents.Count() == 0)
		return true;

	// Build the index
	TOCINDEX *index = BuildIndex();
	if (index == NULL)
		return false;

	// Hello
	stats.Echo("Writing output file: %s\n", outFile);

	// sort the index
	qsort(index, contents.Count(), sizeof(TOCINDEX), TOCINDEX_Compare);

	// Make a header
	HOGTOC_HDR hdr, diskHdr;
	memset(&hdr, '\0', sizeof(hdr));

	hdr.version = HOGTOC_CURRENTVERSION;
	hdr.hdrSize = Align_RoundToN(sizeof(hdr), alignment);
	hdr.encrypt = encrypt ? 0xf00d : 0;
	hdr.entries = contents.Count();
	hdr.tocSize = hdr.entries * sizeof(HOGTOC_ENTRY);

	// How big is the string table?
	int stringTableSize = 0;
	for (uint s = 0; s < hdr.entries; s++)
		stringTableSize += strlen(index[s].entry->name) + 1;
	hdr.tocSize += stringTableSize;

	// What is our total size going to be?
	int totalSize = hdr.hdrSize +	hdr.tocSize + totalFileSize;

	// Make a "patched" version of the TOC
	char *diskTOC = new char[hdr.tocSize];
	HOGTOC_ENTRY *diskTOCEntry = (HOGTOC_ENTRY *)diskTOC;
	memset(diskTOC, '\0', hdr.tocSize);

	// Put all the dir strings first (in index order)
	int stringTableOffset = hdr.hdrSize + sizeof(HOGTOC_ENTRY) * hdr.entries;
	uint i;
	for (i = 0; i < hdr.entries; i++)
	{
		diskTOCEntry[i].name = (char *)stringTableOffset;
		int len = strlen(index[i].entry->name) + 1;
		stringTableOffset += len;
	}

	// Figure out the dataoffsets for everyone (in contents order)
	int fileOffset = stringTableOffset;
	for (i = 0; i < hdr.entries; i++)
	{
		if (alignment != 0)
			fileOffset = Align_RoundToN(fileOffset, alignment);
		contents[i].dataOffset = fileOffset;
		fileOffset += contents[i].size;
	}

	// Now patch up the data for the indexed array
	for (i = 0; i < hdr.entries; i++)
	{
		diskTOCEntry[i].data = (char *)index[i].entry->dataOffset;
		diskTOCEntry[i].size = index[i].entry->size;
		diskTOCEntry[i].crc = index[i].entry->crc;
		if (outputBigEndian)
		{
			ByteSwap(&diskTOCEntry[i].name);
			ByteSwap(&diskTOCEntry[i].data);
			ByteSwap(diskTOCEntry[i].size);
			ByteSwap(diskTOCEntry[i].crc);
		}
	}

	// Assemble the string table
	char *stringTable = (char *)&diskTOC[sizeof(HOGTOC_ENTRY) * hdr.entries];
	int stringOffset = 0;
	for (i = 0; i < hdr.entries; i++)
	{
		int size = strlen(index[i].entry->name) + 1;
		memcpy(&stringTable[stringOffset], index[i].entry->name, size);
		stringOffset += size;
	}

	// Encrypt the toc
	if (encrypt)
		g_helix.Encrypt((const BYTE *)diskTOC, hdr.tocSize, (BYTE *)diskTOC);

	// Compute the toc CRC
	hdr.tocCRC = ComputeCRC(diskTOC, hdr.tocSize);

	// Now we can compute the final hdr crc
	hdr.crc = ComputeCRC(&hdr, sizeof(HOGTOC_HDR));

	// Our bytes written counter
	int onByte = 0;

	// Open the file
	bool fileOk = true;
	FILE *fh = fopen(outFile, "wb");
	if (!fh)
	{
		fileOk = false;
		goto DoneWrite;
	}

	// Make a endian version of the hdr for output
	diskHdr = hdr;
	if (outputBigEndian)
	{
		ByteSwap(diskHdr.version);
		ByteSwap(diskHdr.hdrSize);
		ByteSwap(diskHdr.encrypt);
		ByteSwap(diskHdr.crc);
		ByteSwap(diskHdr.entries);
		ByteSwap(diskHdr.tocSize);
		ByteSwap(diskHdr.tocCRC);
	}

	// write the header
	if (fwrite(&diskHdr, sizeof(diskHdr), 1, fh) != 1)
	{
		fileOk = false;
		goto DoneWrite;
	}
	WritePad(fh, hdr.hdrSize - sizeof(hdr));
	onByte += hdr.hdrSize;
	stats.UpdateProgress(onByte, totalSize);

	// Write the TOC
	if (fwrite(diskTOC, hdr.tocSize, 1, fh) != 1)
	{
		fileOk = false;
		goto DoneWrite;
	}
	onByte += hdr.tocSize;
	stats.UpdateProgress(onByte, totalSize);

	// Write all the data
	for (i = 0; i < hdr.entries; i++)
	{
		// Make sure we are aligned
		int pos = ftell(fh);
		int want = (int)contents[i].dataOffset;
		if (pos != want)
			WritePad(fh, want - pos);
		ASSERT(ftell(fh) == want);

		// Write it
		if (fwrite(contents[i].data, 1, contents[i].size, fh) != contents[i].size)
		{
			fileOk = false;
			goto DoneWrite;
		}

		onByte += contents[i].size;
		stats.UpdateProgress(onByte, totalSize);
	}

DoneWrite:
	// Done with the file
	if (fh)
		fclose(fh);

	// Free our memory
	delete [] diskTOC;
	delete [] index;

	return fileOk;
}

//***************************************************************************
// Build the index
//***************************************************************************
Hog::TOCINDEX *Hog::BuildIndex()
{
	// Create the index
	int count = contents.Count();
	TOCINDEX *index = new TOCINDEX[count];
	if (!index)
		return(NULL);

	// fill it out
	for (int i = 0; i < count; i++)
		index[i].entry = &contents[i];

	// Give it back
	return(index);
}

//***************************************************************************
//***************************************************************************
int __cdecl Hog::TOCINDEX_Compare(
const void *e1,
const void *e2)
{
	const char *s1 = ((TOCINDEX *)e1)->entry->name;
	const char *s2 = ((TOCINDEX *)e2)->entry->name;

	int cmp = strcmp(s1, s2);

	return(cmp);
}

//***************************************************************************
// Write a report
//***************************************************************************
bool Hog::WriteReport(
char *reportfile)
{
	// Do they really want one?
	if (!reportfile || *reportfile == '\0')
		return true;

	// Get rid of it
	remove(reportfile);

	// Open the file
	FILE *fh = fopen(reportfile, "wt");
	if (!fh)
		return false;

	// Hello
	stats.Echo("Writing report file: %s\n", reportfile);

	// Write out the header
	fprintf(fh, "Report for %s:\n\n", outfile);

	// Write out the errors
	for (int i = 0; i < errors.Count(); i++)
	{
		DynString *error = errors[i];
		fprintf(fh, "%s\n", error->Data());
	}

	// Write out the toc
	for (i = 0; i < contents.Count(); i++)
	{
		TOCENTRY &entry = contents[i];
		char out[512];
		_strlwr(contents[i].name);
		sprintf(out,"\"%s\"", entry.name);
		_strupr(contents[i].name);
		while (strlen(out) < 45)
			strcat(out, " ");
		fprintf(fh, "%s size: %8d (offset %8d) (crc 0x%x).\n", out, entry.size, entry.dataOffset, entry.crc);
	}

	// Done!
	fclose(fh);
	return true;
}

//***************************************************************************
// Write a report
//***************************************************************************
bool Hog::WriteListing(
char *listfile)
{
	// Do they really want one?
	if (!listfile || *listfile == '\0')
		return true;

	// Get rid of it
	remove(listfile);

	// Open the file
	FILE *fh = fopen(listfile, "wt");
	if (!fh)
		return false;

	// Hello
	stats.Echo("Writing listing file: %s\n", listfile);

	// Do the output
	for (uint p = 0; p < commandLine.Parameters(); p++)
	{
		CommandLineParameter *param = commandLine.GetParam(p);

		// Skip the file builders
		if (param->Switch() == 'd' ||
				param->Switch() == 'r' ||
				param->Switch() == '\0' ||
				param->Switch() == 'm' ||
				param->Switch() == 'k' ||
				param->Switch() == 'l')
			continue;

		// Add the others
		fprintf(fh, "-%c %s\n", param->Switch(), _strlwr(param->Value().Data()));
	}

	// Write out the toc
	for (int i = 0; i < originalFile.Count(); i++)
	{
		DynString *file = originalFile[i];
		fprintf(fh, "%s\n", file->Data());
	}

	// Done!
	fclose(fh);
	return true;
}

//***************************************************************************
//***************************************************************************
bool Hog::AddError(
char *errorString,
char *file,
bool warning)
{
	// Make a new one
	DynString *s = new DynString;
	if (!s)
		return false;

	// Build the string
	s->Cpy(errorString);
	s->Cat(file);

	// Add it to the list of errors
	errors.Add(s);

	// Report it
	if (warning)
		stats.Notify("%s\n", s->Data());
	else
		stats.Error("%s\n", s->Data());

	// Make them see it!
	return true;
}

//***************************************************************************
// Utility for computing a CRC
//***************************************************************************
uint Hog::ComputeCRC(
const void *data,
int size)
{
#ifdef REALCRC
	CRCCompute crcComputer;
	crcComputer.PutString((unsigned char *)data, size);
	return crcComputer.Done();
#else
	return Hash::CalcChecksum(data, size);
#endif
}

