/* --------------------------------------------------
 * File    : ANIMRIP.C
 * Created : Wed Sep  2 11:05:48 1998
 * Descript: Animation script compiler
 * --------------------------------------------------*/
/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <windows.h>
#include <dbwriter.h>
#include <fn_wrap.h>
#include <t_chunk.h>
#include <conio.h>

#include "ai_gram.h"
#include "airip.h"
extern "C" int yyparse(void);
extern "C" int yy_reset(void);
extern "C" FILE *yyin;
extern "C" int iLineCount;
extern "C" char szFileName[];

/* --------------------------------------------------
 * Local Defines
 * --------------------------------------------------*/

 /* --------------------------------------------------
 * Local Prototypes
 * --------------------------------------------------*/
int FixupOffsets( void);
void FormatOutput( void);
void usage( void);

extern "C" BOOL		bVerbose,
							bBigEndian,
							bPauseWhenDone,
							bPCOutput;

extern "C" int			nGlobalOffset;

extern "C" ts_Hdr		DataList,
							*pDataList;

extern "C" ts_Patch	PatchList,
							*pPatchList;

extern "C" ts_Symbol	SymbolList,
							*pSymbolList;

extern "C"	FILE 		*fInfile,
				 			*fOutfile;

extern "C"	unsigned long 	*pOutputArray;

/* --------------------------------------------------
 * Local Data
 * --------------------------------------------------*/
/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Local Functions
 * --------------------------------------------------*/
void CreateDataFile (char *pDataName);

/* --------------------------------------------------
 * MAIN
 * --------------------------------------------------*/
extern "C" int yydebug;
int main (int argc, char *argv[])
{

  char  drive[_MAX_DRIVE],
			dir[_MAX_DIR],
			file[_MAX_FNAME],
			ext[_MAX_EXT],
			path[_MAX_PATH];

	if (argc < 2)
	{
		usage ();
		return 1;
	}

	bVerbose = FALSE;
	bBigEndian = FALSE;
	while (argv[1][0] == '/')
	{
		switch (argv[1][1])
		{
			case 'B':
			case 'b':
				bBigEndian = TRUE;
				argv++;
				break;

			case 'V':
			case 'v':
				bVerbose = TRUE;
				argv++;
				break;

			case 'P':
			case 'p':
				bPauseWhenDone = TRUE;
				argv++;
        break;

			case 'F':
			case 'f':
				bPCOutput = TRUE;
				argv++;
        break;

      default:
				argv++;
				break;
		}
	}

	pDataList = &DataList;
	pPatchList = &PatchList;
	pSymbolList = &SymbolList;

	DataList.pNext = NULL;
	DataList.nType = 0;
	PatchList.pNext = NULL;
	PatchList.pData = NULL;
	SymbolList.pNext = NULL;
	SymbolList.ulValue = 0;

	// Open infile
	fInfile = fopen( argv[1], "rt");
	strcpy( szFileName, argv[1] );
	if (fInfile == NULL)
	{
		printf ("Command file not found.\n");
		usage ();
		return 1;
	}
	yyin = fInfile;

//	yydebug = 1;
	ProcessIncludes();

	// Parse the input file
	yy_reset();
	yyparse();

 	// Run through patch list and fix-up offsets and #defines
	if (FixupOffsets() == FALSE)
	{
		ExitWithError();
	}

	// Open outfile
	_splitpath( argv[1], drive ,dir ,file ,ext);
	strcpy ( ext, ".sdb");
	_makepath ( path, drive, dir, file, ext);
	CreateDataFile( path);

//	fOutfile = fopen( path, "w");
//	FormatOutput();
	// Write output file
//	nNumWritten = fwrite( pOutputArray, sizeof(char), nGlobalOffset, fOutfile);
//	if (nNumWritten != nGlobalOffset)
//	{
//		printf( "Could not complete write operation to fOutfile.\n");
//		ExitWithError();
//	}

	// Clean up and exit
	FreeMem();
	fclose( fInfile);
//	fclose( fOutfile);

  if( bPauseWhenDone )
    getch();

	return 0;
}

/* --------------------------------------------------
 * 	FixupOffsets
 * --------------------------------------------------*/
int FixupOffsets( void)
{
	ts_DataAddress	*pCurAddress;

	BOOL	bFound;

	pPatchList = &PatchList;

	// Run through each entry in patch list and find address
	while (pPatchList->pNext != NULL)
	{
		pPatchList = pPatchList->pNext;
		pSymbolList = &SymbolList;

		// Run through each entry in the symbol list to match the name with the patch
		bFound = FALSE;
		while (pSymbolList->pNext != NULL)
		{
			pSymbolList = pSymbolList->pNext;
			if (!strcmp( pSymbolList->caName, pPatchList->caName))
			{
				bFound = TRUE;
				break;
			}
		}

		if (bFound == FALSE)
		{
			printf( "Syntax Error in file %s : \n    Symbol not found - %s\n", szFileName, pPatchList->caName);
			return FALSE;
		}

		pCurAddress = (ts_DataAddress *) pPatchList->pData;
		pCurAddress->ulData = pSymbolList->ulValue;
	}

	return TRUE;
}

/* --------------------------------------------------
 * 	FormatOutput
 * --------------------------------------------------*/
void FormatOutput( void)
{
	int	nIndex = 0;
	char 	*pcSource,
			*pcDest;

	pOutputArray = (unsigned long *) malloc(nGlobalOffset);
	if (pOutputArray == NULL)
	{
		printf( "Could not allocate memory for output array.");
		ExitWithError();
	}

	pDataList = &DataList;
	while (pDataList->pNext != NULL)
	{
		pDataList = pDataList->pNext;
		// This is an assumption that all data types that are output are
		// four byte values.  If it needs to be changed, the nType field
		// can be checked to handle different cases.
		if (bBigEndian == FALSE)
		{
			pOutputArray[nIndex] = ((ts_DataAddress *) pDataList)->ulData;
		}
		else //big endian conversion
		{
			pcDest = (char *) (&pOutputArray[nIndex]);
			pcSource = (char *) &(((ts_DataAddress *) pDataList)->ulData);
			pcDest[0] = pcSource[3];
			pcDest[1] = pcSource[2];
			pcDest[2] = pcSource[1];
			pcDest[3] = pcSource[0];
		}
	}
}

//-----------------------------------------------------------------------------
// CreateDataFile
//-----------------------------------------------------------------------------
void CreateDataFile (char *pDataName)
{
	DBWriter				*pDataWriter = NULL;
	FNWrapper			DataWrapper;


	DataWrapper = pDataName;
	if ( bBigEndian == TRUE)
	{
		pDataWriter = new DBWriter( DataWrapper, DBWriter::N64St);
	}
	else if ( bPCOutput == TRUE)
	{
		pDataWriter = new DBWriter( DataWrapper, DBWriter::PCSt);
	}
	else
	{
		pDataWriter = new DBWriter( DataWrapper, DBWriter::PSXSt);
	}

//	if( pDataWriter->bOwnStream == false)
//	{
//		printf("The output file could not be created.\n");
//		exit(1);
//	}

	pDataWriter->DBStart( 0);
	pDataWriter->ChunkStart( CHUNK_SCRIPT, 0, NULL);

	pDataList = &DataList;
	while (pDataList->pNext != NULL)
	{
		pDataList = pDataList->pNext;
		if (pDataList->nType == LIST_FLOAT)
		{
			pDataWriter->stream( ) << ((ts_DataFloat *) pDataList)->fData;
		}
		else	// all other types output as unsigned long
		{
			pDataWriter->stream( ) << ((ts_DataAddress *) pDataList)->ulData;
		}
	}

	pDataWriter->ChunkEnd( );
	pDataWriter->DBEnd( );
}

/* --------------------------------------------------
 * 	usage
 * --------------------------------------------------*/
void usage (void)
{
	printf ("AnimRip -- Copyright Avalanche Software 1998\n");
	printf ("ANIMRIP [options] <infile>\n");
	printf ("  Where [options] are:\n");
	printf ("    /B --> Output data as big endian (defaults to little endian)\n");
	printf ("    /V --> Verbose output\n");
	printf ("    /P --> Wait for a keypress after processing\n");
	printf ("    /F --> Output floating point values (instead of PSX fixed point)\n");
	printf ("    \n");
	printf ("  Where <infile> contains the following commands:\n");
	printf ("   START_MOTION (nMotionNumber) \n");
	printf ("		-- Start the motion processor on the motion number given\n");
	printf ("   STOP_MOTION\n");
	printf ("		-- Stop the motion processor for this animation\n");
	printf ("   KEY_FRAME (nFrameOffset)\n");
	printf ("		-- Display the current key frame of animation\n");
	printf ("   INTERP_FRAME (nFrameOffset, fPercent)\n");
	printf ("		-- Interpolate between the current frame of animation and and the\n");
	printf ("          specified frame.  The interpolation will calculate the new frame\n");
	printf ("		   fPercent away from the current frame.\n");
	printf ("   DELAY (nFrames)\n");
	printf ("		-- Delay animation script processing for nFrames number of ticks.\n");
	printf ("	FORCE (fX, fY, fZ)\n");
	printf ("		-- Apply this force vector to the actor\n");
	printf ("	IMPULSE (fX, fY, fZ)\n");
	printf ("		-- Directly apply this velocity vector to the actor\n");
	printf ("	IMPULSE_X (fX)\n");
	printf ("		-- Directly apply this velocity vector to the actor\n");
	printf ("	IMPULSE_Y (fY)\n");
	printf ("		-- Directly apply this velocity vector to the actor\n");
	printf ("	IMPULSE_Z (fZ)\n");
	printf ("		-- Directly apply this velocity vector to the actor\n");
	printf ("	OFFSET (fX, fY, fZ)\n");
	printf ("		-- change the actor's position by this much\n");
	printf ("   JUMP (nAnimLabel)				\n");
	printf ("		-- Jump to this offset to perform the next animation script command.\n");
	printf ("			The offset will be calculated by the script compiler.\n");
	printf ("   NEW_ANIM (nAnimNumber)		\n");
	printf ("		-- Jump to this animation number and begin processing that script.\n");
	printf ("			This is different from the JUMP command in that the actor's animation\n");
	printf ("			number will change (for ai purposes).\n");
	printf ("   CODE (nFunctionNumber)\n");
	printf ("		-- Lookup the function in the function table and execute that piece of code.\n");
	printf ("   IF_SET (nAnimLabel) \n");
	printf ("		-- If nAnimVar is true, then jump to the label\n");
	printf ("   IF_NOT_SET (nAnimLabel)		\n");
	printf ("		-- If nAnimVar is false, then jump to the label\n");
	printf ("   IF_EQ	(nValue, nAnimLabel)\n");
	printf ("		-- If nAnimVar is equal to nValue, then jump to the label\n");
	printf ("   ACTION (nAction)\n");
	printf ("		-- Set the actor's action to this value\n");
	printf ("   WAIT_HIT_GROUND\n");
	printf (" 	    -- Do not go past this command until the actor has landed on the ground\n");
	printf ("   WAIT_TIL_HOLD\n");
	printf ("		-- Do not go past this command until the actor is at a hold in his\n");
	printf ("			motion process.\n");
	printf ("   WAIT_INTERP_DONE\n");
	printf ("      -- Do not go past this command until the actor has completed any scripted\n");
	printf ("			 interpolation commands.\n");
	printf ("   PLAY_RANDOM_SOUND (nX, nY, nZ) \n");
	printf ("      -- Play the sound in the sound table indexed by nX through channel nY at\n");
	printf ("         volume nZ.\n");
	printf ("   PLAY_SOUND (nX, nY, nZ) \n");
	printf ("      -- Play the sound nX through channel nY at volume nZ.\n");
	printf ("   COLLISION_DELAY (nFrames) \n");
	printf ("      -- Process collisions during the specified number of frames.\n");
	printf ("   SET_FLAG (nBitMask)\n");
	printf ("		-- Bitwise OR this value into the actor's flags field		\n");
	printf ("   CLR_FLAG	(nBitMask)\n");
	printf ("		-- Bitwise AND the NEGATED value with the actor's flags field		\n");
	printf ("   CREATE_PROC	(nProcNumber, nProcID)\n");
	printf ("		-- Get the address of the process from the Process Table and create it\n");
	printf ("			using ProcID as the process ID\n");
	printf ("   KILL_PROC (nProcID)\n");
	printf ("		-- Kill all instances of nProcID\n");
	printf ("   SOUND	(nSequence)	\n");
	printf ("		-- Trigger nSequence to play\n");
	printf ("   ATTACH_OBJ\n");
	printf ("   DETACH_OBJ		\n");
	printf ("   SLAVE_POS (fX, fY, fZ)		\n");
	printf ("   SLAVE_FORCE (fX, fY, fZ)\n");
	printf ("   SLAVE_IMPULSE (fX, fY, fZ)\n");
	printf ("   CHECK_DEAD\n");
	printf ("		-- Check to see if this object is dead, if so, run the DIE script		\n");
	printf ("   COLLISION (nType)\n");
	printf ("		-- Check for an offensive collision of nType\n");
	printf ("   #include FILENAME\n");
	printf ("   #define ConstantName INT\n");
	printf ("    \n");
}