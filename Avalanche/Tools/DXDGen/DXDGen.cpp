// DXDGen.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "XPRFileData.h"
#include "DBLFileData.h"
#include "DXDTypes.h"

unsigned int gSysAlign = 4;
unsigned int gVidAlign = 128;

enum VERBOSE_LEVEL
{
	VERBOSE_NONE,
	VERBOSE_LOW,
	VERBOSE_MED,
	VERBOSE_HIGH,
};

int g_verboseLevel;

void Print(int verboseLevel,const char *pcFmt, ...)
{
	if (verboseLevel<=g_verboseLevel)
	{
		int nLen;
		static char acMessage[800];
		va_list PrintArgs;

		/* create message */

		va_start(PrintArgs, pcFmt);
		vsprintf(acMessage, pcFmt, PrintArgs);
		va_end(PrintArgs);

		/* anything to do? */
		if ((nLen = strlen(acMessage)) == 0)
			return;

		printf("%s",acMessage);
	}
}


int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		int nStartTickCount = GetTickCount();
		g_verboseLevel = VERBOSE_NONE;

		printf("DXDGen - XBOX resource file combiner and separator\n");
		printf("Version %s - Build %s %s\n",g_pDXDVersion,g_pDXDDate,g_pDXDTime);

		if( (argc < 6) )
		{
			printf("Usage:\n");
			printf("\tDXDGen outputDBLFile outputDXDFile inputDBLFile inputXPRFile inputXVDFile [options]\n"); 
			printf("\toptions:\n"); 
			printf("\t\t-d delete xpr & xvd files\n"); 
			printf("\t\t-v verbose mode\n"); 
			return 1;
		}

		bool bDeleteFiles = false;

		for (int argIndex = 6; argIndex < argc; argIndex++)
		{
			if (strcmp(argv[argIndex],"-d")==0)
			{
				bDeleteFiles = true;
			}
			else if (strcmp(argv[argIndex],"-v")==0)
			{
				g_verboseLevel = VERBOSE_HIGH;
			}
		}

		TupString outputDBLFile(argv[1]);
		TupString outputDXDFile(argv[2]);
		TupString inputDBLFile(argv[3]);
		TupString inputXPRFile(argv[4]);
		TupString inputXVDFile(argv[5]);

		XPRFileData inputXPRFileData;
		XPRFileData inputXVDFileData;
		DBLFileData inputDBLFileData;

		try
		{
			inputXPRFileData.ReadFile(inputXPRFile);
		}
		catch (Exception e)
		{
			Print(VERBOSE_LOW,"Unable to load XPR file %s skipping\n  %s",(const char *)inputXPRFile,(const char *)(e));
		}

		try
		{
			inputXVDFileData.ReadFile(inputXVDFile);
		}
		catch (Exception e)
		{
			Print(VERBOSE_LOW,"Unable to load XVD file %s skipping\n  %s",(const char *)inputXVDFile,(const char *)(e));
		}

		try
		{
			inputDBLFileData.ReadFile(inputDBLFile);
		}
		catch (Exception e)
		{
			throw(Exception(ERROR_FATAL,"Error Reading Input DBL File %s\n  %s",(const char *)inputDBLFile,(const char *)(e)));
		}

		if (!inputXPRFileData.m_bFileRead && !inputXVDFileData.m_bFileRead)
		{
			throw(Exception(ERROR_FATAL,"Error no files to process"));
		}

		// write the DXD file
		FILE * pDXDFile = fopen( outputDXDFile, "wb" );

		if (pDXDFile == NULL)
		{
			throw(Exception(ERROR_FATAL,"Error Opening DXD file %s for writing\n",(const char *)outputDXDFile));
		}

		int numChunks = 0;
		DXDChunkHeader xprChunkHeader;
		memset(&xprChunkHeader,0,sizeof(xprChunkHeader));
		DXDChunkHeader xvdChunkHeader;
		memset(&xvdChunkHeader,0,sizeof(xvdChunkHeader));

		if (inputXPRFileData.m_bFileRead)
		{
			xprChunkHeader.dwType = TYPE_XPR;
			xprChunkHeader.dwVideoMemorySize = inputXPRFileData.m_videoBufferSize;
			xprChunkHeader.dwVideoMemoryOffset = ftell(pDXDFile);
			numChunks++;
			if (fwrite(inputXPRFileData.m_pVideoBuffer,inputXPRFileData.m_videoBufferSize,1,pDXDFile)!=1)
			{
				throw(Exception(ERROR_FATAL,"Error Writing XPR video data to DXD file %s\n",(const char *)outputDXDFile));
			}
		}

		unsigned int currPos = ftell( pDXDFile );
		unsigned int padBetweenChunks = 0;

		if( currPos % 128 )
		{
			char				padChar = (char) 0xCA;
			padBetweenChunks = 128 - ( currPos % 128 );

			if( fwrite( & padChar, 1, padBetweenChunks, pDXDFile ) != padBetweenChunks )
			{
				throw(Exception(ERROR_FATAL,"Error Writing Pad to DXD file %s\n",(const char *)outputDXDFile));
			}
		}

		if (inputXVDFileData.m_bFileRead)
		{
			xvdChunkHeader.dwType = TYPE_XVD;
			xvdChunkHeader.dwVideoMemorySize = inputXVDFileData.m_videoBufferSize;
			xvdChunkHeader.dwVideoMemoryOffset = ftell(pDXDFile);
			numChunks++;
			if (fwrite(inputXVDFileData.m_pVideoBuffer,inputXVDFileData.m_videoBufferSize,1,pDXDFile)!=1)
			{
				throw(Exception(ERROR_FATAL,"Error Writing XVD video data to DXD file %s\n",(const char *)outputDXDFile));
			}
		}
		unsigned int DXDSize = ftell( pDXDFile	);

		fclose( pDXDFile);

		// write the DBL file
		FILE * pDBLFile = fopen( outputDBLFile, "wb" );

		if (pDBLFile == NULL)
		{
			throw(Exception(ERROR_FATAL,"Error Opening DBL file %s for writing\n",(const char *)outputDBLFile));
		}

		if (fwrite(inputDBLFileData.m_pDataBuffer,sizeof(inputDBLFileData.m_fileHeader.cSkipCount)+inputDBLFileData.m_padSize,1,pDBLFile)!=1)
		{
			throw(Exception(ERROR_FATAL,"Error Writing DBL Header data to file %s\n",(const char *)outputDBLFile));
		}

		unsigned int dxdChunkOffset = 8+(numChunks*sizeof(DXDChunkHeader));
		if (inputXPRFileData.m_bFileRead)
		{
			xprChunkHeader.dwSystemMemorySize = sizeof(inputXPRFileData.m_fileHeader)+inputXPRFileData.m_headerBufferSize;
			xprChunkHeader.dwSystemMemoryOffset = dxdChunkOffset;
			dxdChunkOffset += xprChunkHeader.dwSystemMemorySize;
		}

		if (inputXVDFileData.m_bFileRead)
		{
			xvdChunkHeader.dwSystemMemorySize = sizeof(inputXVDFileData.m_fileHeader)+inputXVDFileData.m_headerBufferSize;
			xvdChunkHeader.dwSystemMemoryOffset = dxdChunkOffset;
			dxdChunkOffset += xvdChunkHeader.dwSystemMemorySize;
		}

		unsigned int dxdChunkSizeAligned = dxdChunkOffset;
		if (dxdChunkSizeAligned % 32)
		{
			dxdChunkSizeAligned += 32 - (dxdChunkSizeAligned % 32);
		}

		if (dxdChunkSizeAligned > inputDBLFileData.m_fileHeader.u32MaxChunkSize)
			inputDBLFileData.m_fileHeader.u32MaxChunkSize = dxdChunkSizeAligned;

		inputDBLFileData.m_fileHeader.u32Chunks ++; // 1 more chunk

		if (fwrite(&inputDBLFileData.m_fileHeader.cID,sizeof(inputDBLFileData.m_fileHeader)-sizeof(inputDBLFileData.m_fileHeader.cSkipCount),1,pDBLFile)!=1)
		{
			throw(Exception(ERROR_FATAL,"Error Writing DBL Header data to file %s\n",(const char *)outputDBLFile));
		}


		ts_DBChunkHdr dxdChunkHeader;
		memset(&dxdChunkHeader,0 ,sizeof (dxdChunkHeader));
		dxdChunkHeader.u16Type = DB_XBOX_DXD;
		dxdChunkHeader.u16Version = DB_XBOX_DXD_VERSION;
		dxdChunkHeader.u16Flags = DBL_HEADER_32BYTE | DBL_NAMED_CHUNK;
		dxdChunkHeader.u32Size = dxdChunkSizeAligned;
		strncpy( dxdChunkHeader.cName, "1000", DBL_MAXNAME);

		if (fwrite(&dxdChunkHeader,sizeof(dxdChunkHeader),1,pDBLFile)!=1)
		{
			throw(Exception(ERROR_FATAL,"Error Writing DXD chunk header data to file %s\n",(const char *)outputDBLFile));
		}

		if (fwrite(&numChunks,sizeof(numChunks),1,pDBLFile)!=1)
		{
			throw(Exception(ERROR_FATAL,"Error Writing DXD number of chunks to file %s\n",(const char *)outputDBLFile));
		}

		if (fwrite(&DXDSize,sizeof(DXDSize),1,pDBLFile)!=1)
		{
			throw(Exception(ERROR_FATAL,"Error Writing DXD chunk DXD size to file %s\n",(const char *)outputDBLFile));
		}

		if (inputXPRFileData.m_bFileRead)
		{
			if (fwrite(&xprChunkHeader,sizeof(xprChunkHeader),1,pDBLFile)!=1)
			{
				throw(Exception(ERROR_FATAL,"Error Writing xpr chunk system file header to file %s\n",(const char *)outputDBLFile));
			}
		}

		if (inputXVDFileData.m_bFileRead)
		{
			if (fwrite(&xvdChunkHeader,sizeof(xvdChunkHeader),1,pDBLFile)!=1)
			{
				throw(Exception(ERROR_FATAL,"Error Writing xvd chunk system file header to file %s\n",(const char *)outputDBLFile));
			}
		}

		if (inputXPRFileData.m_bFileRead)
		{
			if (fwrite(&inputXPRFileData.m_fileHeader,sizeof(inputXPRFileData.m_fileHeader),1,pDBLFile)!=1)
			{
				throw(Exception(ERROR_FATAL,"Error Writing XPR chunk system file header to file %s\n",(const char *)outputDBLFile));
			}

			if (fwrite(inputXPRFileData.m_pHeaderBuffer,inputXPRFileData.m_headerBufferSize,1,pDBLFile)!=1)
			{
				throw(Exception(ERROR_FATAL,"Error Writing XVD chunk header data to file %s\n",(const char *)outputDBLFile));
			}
		}

		if (inputXVDFileData.m_bFileRead)
		{
			if (fwrite(&inputXVDFileData.m_fileHeader,sizeof(inputXVDFileData.m_fileHeader),1,pDBLFile)!=1)
			{
				throw(Exception(ERROR_FATAL,"Error Writing XVD chunk system file header to file %s\n",(const char *)outputDBLFile));
			}

			if (fwrite(inputXVDFileData.m_pHeaderBuffer,inputXVDFileData.m_headerBufferSize,1,pDBLFile)!=1)
			{
				throw(Exception(ERROR_FATAL,"Error Writing XVD chunk header data to file %s\n",(const char *)outputDBLFile));
			}
		}

		// write pad after chunnk
		unsigned int numPadBytes = dxdChunkSizeAligned-dxdChunkOffset;
		if (numPadBytes)
		{
			char padChar = (char) 0xCA;
			if( fwrite( & padChar, 1, numPadBytes, pDBLFile ) != numPadBytes )
			{
				throw(Exception(ERROR_FATAL,"Error Writing Pad to DXD chunk %s\n",(const char *)outputDBLFile));
			}
		}

		// write other dbl chunks
		if (fwrite(inputDBLFileData.m_pDataBuffer+sizeof(inputDBLFileData.m_fileHeader)+inputDBLFileData.m_padSize,inputDBLFileData.m_dataBufferSize-(sizeof(inputDBLFileData.m_fileHeader)+inputDBLFileData.m_padSize),1,pDBLFile)!=1)
		{
			throw(Exception(ERROR_FATAL,"Error Writing DBL data to file %s\n",(const char *)outputDBLFile));
		}

		fclose( pDBLFile);

		// delete xpr & xvd files
		if (bDeleteFiles)
		{
			Print(VERBOSE_HIGH,"Deleting Files");
			if (inputXPRFileData.m_bFileRead)
			{
				if (!DeleteFile(inputXPRFile) != S_OK)
				{
					throw(Exception(ERROR_FATAL,"Error Deleting XPR file %s\n",(const char *)inputXPRFile));
				}
			}

			if (inputXVDFileData.m_bFileRead)
			{
				if (!DeleteFile(inputXVDFile) != S_OK)
				{
					throw(Exception(ERROR_FATAL,"Error Deleting XVD file %s\n",(const char *)inputXVDFile));
				}
			}
		}

		if (inputXPRFileData.m_bFileRead)
		{
			Print(VERBOSE_HIGH,"XPR - System Chunk Size %d bytes\n",inputXPRFileData.m_headerBufferSize);
			Print(VERBOSE_HIGH,"XPR - Video Chunk Size %d bytes\n",inputXPRFileData.m_videoBufferSize);
		}

		if (inputXVDFileData.m_bFileRead)
		{
			Print(VERBOSE_HIGH,"XVD - Header Chunk Size %d bytes\n",inputXVDFileData.m_headerBufferSize);
			Print(VERBOSE_HIGH,"XVD - Video Chunk Size %d bytes\n",inputXVDFileData.m_videoBufferSize);
		}
		Print(VERBOSE_HIGH,"Pad between XPR & XVD in DXD file %d bytes\n",padBetweenChunks);

		int nTicks = GetTickCount() - nStartTickCount;
		Print(VERBOSE_HIGH,"Total Time: %02d Min %02d Sec\n", (nTicks / 1000) / 60, (nTicks / 1000) % 60);

	}
	catch (Exception e)
	{
		Print(VERBOSE_NONE,"Error Processing Files\n  %s\n",(const char *)e);
//pab		return 1;
		return 0;
	}
	catch (...)
	{
		Print(VERBOSE_NONE,"Unhandled Exception\nProcessing Aborted\n");
		return 1;
	}
	return 0;
}

