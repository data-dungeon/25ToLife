////////////////////////////////////////////////////////////////////////////
//
// LoadSCFiles
//
// Class to handle getting texture names from Dan's front end tool sc files
//
////////////////////////////////////////////////////////////////////////////

#include "ExportTexturesPch.h"
#include "LoadSCFiles.h"

TupArray<TupString> m_filePathEntries;
TupArray<TupString> m_scFileEntries;

LoadSCFiles::LoadSCFiles(void)
{
}

LoadSCFiles::~LoadSCFiles()
{
}

struct FileMarker
{
	DWORD PreMarker;
	DWORD Marker;
	DWORD Version;
	DWORD Length;
};

void LoadSCFiles::ScanSCFile(const TupString &filePathName,TupArray<TupString> &textureFileNames)
{
	textureFileNames.RemoveAll();

	FILE* f = fopen( filePathName, "rb" );

	if ( f == NULL )
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
			"Error loading SC File %s",(const char *)filePathName);
		return;
	}

	// get length of file
	fseek(f, 0, SEEK_END);
	int fileLength = ftell(f); 
	fseek(f, 0, SEEK_SET);

	if (fileLength==0)
	{
		fclose( f );
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
			"File %s has no data",(const char *)filePathName);
		return;
	}

	// allocate a buffer to hold the entire file
	char *pBuffer = new char[fileLength];
	// read it all in
	int bytesRead = fread(pBuffer,1,fileLength,f);
	fclose( f );

	int nBufferPos = 0;

	FileMarker *pFileMarker = (FileMarker *)&pBuffer[nBufferPos];
	nBufferPos += sizeof(FileMarker);

	if (pFileMarker->PreMarker != 0xcdfe3c5b)
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
			"Error scanning SC File %s - header mismatch",(const char *)filePathName);
	}

	if (pFileMarker->Marker != 0x5c8e3c1b)
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
			"Error scanning SC File %s - project header not first",(const char *)filePathName);
	}

	if (pFileMarker->Version != 0x00000100)
	{
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
			"Error scanning SC File %s - project header version mismatch",(const char *)filePathName);
	}

	while ((nBufferPos+(int)sizeof(FileMarker))<fileLength)
	{
		FileMarker *pFileMarker = (FileMarker *)&pBuffer[nBufferPos];
		if (pFileMarker->PreMarker != 0xcdfe3c5b)
		{
			PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
				"Error scanning SC File %s - header mismatch",(const char *)filePathName);
			break;
		}
		// texture chunk
		if (pFileMarker->Marker == 0x887c3b1a)
		{
			if (pFileMarker->Version != 0x0100)
			{
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
					"Error scanning SC File %s - texture chunk version number mismatch",(const char *)filePathName);
				break;
			}
			DWORD numStrings = *(DWORD *)&pBuffer[nBufferPos + sizeof(FileMarker)];
			int nStringPos = nBufferPos + sizeof(FileMarker) + sizeof(DWORD);

			while (numStrings)
			{
				TupString filePath = &pBuffer[nStringPos];
				textureFileNames.Add(filePath);
				nStringPos += filePath.GetLength()+1;
				numStrings--;
			}
		}
		nBufferPos += pFileMarker->Length;
	}
	delete [] pBuffer;
}

