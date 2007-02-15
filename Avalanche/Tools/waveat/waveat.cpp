//***************************************************************************
//***************************************************************************
#include <windows.h>
#include <assert.h>
#include <stdio.h>

#include <toolhelper\cmdline.h>
#include <toolhelper\pathfile.h>
#include <enginehelper\filestream.h>

#include "wavefile.h"
#include "waveat.h"
#include "aiff.h"

//***************************************************************************
//***************************************************************************
int main(
int argc,
char *argv[])
{
//	printf("WavEat Version 1.01, by Food, hax0red by Joe\n");
//	printf("----------------------------\n");

	// Are they dumb?
	if (argc < 2)
		WavEat::HelpExit();

	// Build up a command line
	DynString commandLine;
	for (int i = 1; i < argc; i++)
	{
		if (i != 1)
			commandLine.Cat(" ");
		commandLine.Cat(argv[i]);
	}

	// Convert!
	WavEat wavEat;
	if (!wavEat.Go(commandLine))
		return(-1);

	// We worked if we got here
	return(0);
}

//***************************************************************************
// Init everything, load the world, convert, exit
//***************************************************************************
bool WavEat::Go(
char *commandLineString)
{
	// Process the commandline
	if (!commandLine.Parse(commandLineString, "drvo"))
		HelpExit();

	// Verbose?		
	verbose = (commandLine.GetParam('v') != NULL);

	// explicit output filename
	char *outfile = commandLine.GetParam('o');

	// Load the directory tree
	if (!EnumerateFilesDirs())
		ErrorExit("At least one <file> -d <add dir> or -r <recurse dir> must be specified.");

	// Process the files
	for (uint i = 0; i < fileList.Count(); i++)
	{
		DynString *file = fileList.Get(i);
		ProcessFile(file->Data(), outfile);
	}

	// duh
	if (verbose)
		printf("Done.\n\n");

	return(true);
}

//*************************************************************************
// Load the directory tree
//*************************************************************************
bool WavEat::EnumerateFilesDirs(void)
{
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
				printf("Error: Couldn't add file: ", fileDir);
			++dirsFiles;
		}
		else if (param->Switch() == 'r' || param->Switch() == 'd')
		{
			// Scanning a directory
			if (!fileList.Build(fileDir, "wav", param->Switch() == 'r'))
				printf("Error: Couldn't add directory: ", fileDir);
			else if (verbose)
				printf("Scanning directory \"%s\" %s.\n", fileDir, param->Switch() == 'r' ? "recursively" : "non-recursively");
			++dirsFiles;
		}
	}

	return(dirsFiles > 0);
}

//***************************************************************************
// process a script
//***************************************************************************
bool WavEat::ProcessFile(
char *waveFile,
char *outDir)
{
	FILE* fh = NULL;

	// Make sure it is .wav
	char wave[256];
	strcpy(wave, waveFile);
	if (!PathFile::HasExt(wave))
		strcat(wave, ".wav");

	AIFF aiff;
	UINT				size;
	WAVEFORMATEX	*wfx;
	BYTE				*data;

	// Load the file
	if ( aiff.LoadFile( wave, &size, &wfx, &data ) )
	{
		// loaded aiff successfully...
	}
	else if (WaveFile::LoadFile(wave, &size, &wfx, &data) == WaveFile::OKAY)
	{
		// loaded wave successfully...
	}
	else
	{
		printf("Error: Cannot load \"%s\"\n", wave);
		return(false);
	}
	
	// Make the output file name
	char eatFile[256];
	if( !outDir)
		BuildOutputFile(wave, eatFile);
	else
		BuildOutputFile(wave, outDir, eatFile);

	// Be annoying, be very annoying
	if (verbose)
		printf("Converting \"%s\" to \"%s\".\n", wave, eatFile);

	// Create the output file
	fh = fopen(eatFile, "wb");
	if (!fh)
		goto errorExit;

	// Save the header
	if (fwrite(wfx, sizeof(WAVEFORMATEX), 1, fh) != 1)
		goto errorExit;

	// Write the data
	if (fwrite(data, size, 1, fh) != 1)
		goto errorExit;

	// Done!
	WaveFile::DoneFile(&wfx, &data);
	fclose(fh);
	return(true);

errorExit:
	// Doh!
	printf("Error: Can't process \"%s\"\n", waveFile);
	WaveFile::DoneFile(&wfx, &data);
	if (fh)
		fclose(fh);
	return(false);
}

//***************************************************************************
//***************************************************************************
void WavEat::BuildOutputFile(
const char *waveFile,
char *eatFile)
{
	strcpy(eatFile, waveFile);
	int len = (int) strlen(eatFile) - 1;
	while (len >= 0 && eatFile[len] != '.')
		--len;
	strcpy(&eatFile[len + 1], "eat");
}

void WavEat::BuildOutputFile(
const char *waveFile,
const char *outDir,
char *eatFile)
{
	strcpy( eatFile, outDir);

	// add a slash on end if necessary
	int len = (int) strlen(eatFile) - 1;
	if( eatFile[len] != ':'  &&  eatFile[len] != '\\')
		strcat( eatFile, "\\");

	len = (int)strlen(waveFile) - 1;
	while(len > 0  &&  waveFile[len - 1] != '\\'  &&  waveFile[len - 1] != ':')
		len--;

	strcat( eatFile, waveFile + len);

	len = (int)strlen(eatFile) - 1;
	while (len >= 0 && eatFile[len] != '.')
		--len;

	strcpy( &eatFile[len + 1], "eat");
}

//***************************************************************************
//***************************************************************************
void WavEat::HelpExit(void)
{
	printf("Usage: WavEat <file> (<file> ...) [-d <add dir> (-d <add dir> ...) -r <recurse dir> (-r <recurse dir> ...) -verbose -o <outdir>]\n");
	exit(-1);
}

//***************************************************************************
// Exit the program with a reason
//***************************************************************************
void WavEat::ErrorExit(
char *reason)
{
	printf("Error: %s\n", reason);
	exit(-1);
}


//***************************************************************************
// Some class is depending on this, so the tool isn't compiling...
//***************************************************************************
void memSetOneShotNewName(char *pcName)
{
	int i;
	i = 1;
}
