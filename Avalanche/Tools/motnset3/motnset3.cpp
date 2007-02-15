
#pragma hdrstop
#include <windows.h>
//djr#include <condefs.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <mpostr.h>
#include <dbwriter.h>
#include <GroupBoneTable.h>
#include "parse.h"
#include "motnset.h"
#include "parsefcn.h"


#ifdef BORLAND
//---------------------------------------------------------------------------
USEUNIT("\\Jonm\\d\\tools\\dbuilder\\dbwriter.cpp");
USEUNIT("\\Jonm\\d\\tools\\mpostr\\mpostr.cpp");
USEUNIT("\\Jonm\\d\\tools\\mpostr\\mpo_imp.cpp");
USEUNIT("parsefcn.cpp");
USEUNIT("parse.cpp");
USEUNIT("vector_f.c");
USEUNIT("quatern.c");
USEUNIT("\\Jonm\\d\\tools\\dave\\link_lib\\Slllist.c");
//---------------------------------------------------------------------------
#pragma argsused
#endif

#ifndef TRUE
#define TRUE 	1
#define FALSE  0
#endif

#define 	READ_BUF_LEN	32 * 1024
#define 	MAX_CMD_LEGNTH	128

extern char     *numString;
extern char     *spaceString;
char		*cmdString		= "//+-";
unsigned int numErrors = 0;
unsigned int numWarnings = 0;
bool bVerbose = false;
bool bPauseWhenDone = false;

#define DEFAULT_ERROR_TOLERANCE 0.001f
float errorTolerance = DEFAULT_ERROR_TOLERANCE;

#define DEFAULT_TRANSLATION_SCALE_FACTOR 1.0f
float translationScaleFactor = DEFAULT_TRANSLATION_SCALE_FACTOR;


void 	printError(char *pString);
void 	printCommands(char *programName);
void 	processCommandLine(int argc, char **argv);
void 	processCommand(char *pCmd);
char	*getFileName(char *file);
char 	*eatWhite(char *pString);
int 	isArg(char *pArg);
void 	removeFileExt(char *pString);
float GetErrorTolerance()
{ return errorTolerance; }
float GetTranslationScaleFactor()
{ return translationScaleFactor; }




char motnTxtFileName[80];

int main(int argc, char* argv[])
{
	initMotn();

   memset(motnTxtFileName, 0x00, sizeof(motnTxtFileName));
   processCommandLine(argc, argv);

	if(strlen(motnTxtFileName)==0)
	{
		printCommands(argv[0]);
		return EXIT_FAILURE;
	}

   printf("Using Default ErrorTolerance = %f\n\n", errorTolerance);
   printf("Using Default TranslationScale = %f\n\n", translationScaleFactor);

   if(strlen(motnTxtFileName))
   {
		ParseFileTxt(motnTxtFileName, parseInfArray[0].listPtr, parseInfArray[0].listSize, 0, PARSE_START_INDEX, EOF_INDEX, UNRECOGNIZED_INDEX);
		writeOutputData(glbMotnSet.pMotnList);
		deInitMotn();

   }

	if( bPauseWhenDone || numErrors )
	{
		printf("\nPress any key to continue...\n");
		getch( );
	}
   if(numErrors)
	{
		ASSERT(0);
		return EXIT_FAILURE;
	}
	else
		return EXIT_SUCCESS;
}

void SetMotionSetName( char *pName)
{
	strcpy(glbMotnSet.setName, pName);
}

void SetGBTName( char *pName)
{
	strcpy(glbMotnSet.setGBTFileName, pName);
	SetGroupBoneTableFile( glbMotnSet.setGBTFileName );
}



void printCommands(char *programName)
{
	printf("\n%s [-v] [-p] [-t f.fff] -f inputfile [-o outputfile] [-g GBTfile]\n\r", programName);
	printf("  inputfile = input TXT file to process\n\r");
	printf("  outputfile = output MDB file to create (use filename on MOTION_SET_NAME of inputfile if outputfile not specified)\n\r");
	printf("  GBTfile = input GBT file to for bone-group mapping (use filename on MOTION_GROUPBONETABLE of inputfile if GBTfile not specified)\n\r");
	printf("  inputfile = text file to process\n\r");
	printf("  -v = verbose: LOTS of debug data is spewed\n\r");
	printf("  -p = pause: wait for keypress when done\n\r");
	printf("  f.fff = compression error tolerance: 0.0 = no compression, 1.0 arbitrarily set max compression, 0.001 = default\n\r");
	printf("\n\r");
	printf("inputfile commands:\n\r");
	printf("\n\r");
	printf("     # this is a comment\n\r");
	printf("     MOTION_FILE: filename\n\r");
	printf("     MOTION_GROUPBONETABLE: GroupBoneTableFile\n\r");
	printf("     MOTION_ID: STRING_TO_EQUATE_TO_THIS_MOTIOND\n\r");
	printf("     MOTION_BONEGROUPFILTER: 2|3  (process only bonegroup 2 and 3 motion)\n\r");
	printf("     MOTION_BONEGROUPEXCLUDE: 2|3  (process all but bonegroup 2 and 3 motion)\n\r");
	printf("     MOTION_ERRORTOLERANCE: float\n\r");
	printf("     MOTION_TRANSLATION_SCALE_FACTOR: float\n\r");
	return;
}

void printError( const char *pcFmt, ...)
{
	char Buffer[1024];

	va_list				PrintArgs;

	va_start( PrintArgs, pcFmt);

	vsprintf( Buffer, pcFmt, PrintArgs);
	printf( "ERROR: %s", Buffer);

	va_end( PrintArgs);

	numErrors++;

}

void printWarning( const char *pcFmt, ...)
{
	char Buffer[1024];

	va_list				PrintArgs;

	va_start( PrintArgs, pcFmt);

	vsprintf( Buffer, pcFmt, PrintArgs);
	printf( "WARNING: %s", Buffer);

	va_end( PrintArgs);

	numWarnings++;

}

void printError(char *pString)
{
	printf("ERROR: %s", pString);
   return;
}

void printInfo( const char *pcFmt, ...)
{
	if(bVerbose)
	{
		char Buffer[1024];

		va_list				PrintArgs;

		va_start( PrintArgs, pcFmt);

		vsprintf( Buffer, pcFmt, PrintArgs);
		printf( "VERBOSE: %s\n", Buffer);

		va_end( PrintArgs);
	}
}


/* -----------------3/28/99 9:40PM-------------------
 * processCommandLine() concatinates together commands
 * and command parameters together for the call to
 * process command.
 * --------------------------------------------------*/
void processCommandLine(int argc, char **argv)
{
	int	index;
   char	cmdString[MAX_CMD_LEGNTH];

	for(index = 1; index < argc; index++)
   {
		/* -----------------2/11/99 1:15PM-------------------
		 * If next arguments arent command concatinate the
		 * commands together for processCommand.
		 * --------------------------------------------------*/
	   memset(cmdString, 0x00, MAX_CMD_LEGNTH);
      strcpy(cmdString, argv[index]);

		while(((index+1) < argc) && (isArg(argv[index+1]) == FALSE))
  	   {
			index++;
			strcat(cmdString, argv[index]);
      }

		/* -----------------2/11/99 6:17PM-------------------
		 * Process commands will fill in information for
		 * writeTpgWithChunk
		 * --------------------------------------------------*/
		processCommand(cmdString);
   }


}


/* -----------------2/11/99 1:02PM-------------------
 * processCommand looks for a command line parameter
 * and calls the appropriate function for the type.
 * --------------------------------------------------*/
void processCommand(char *pCmd)
{
	if(isArg(pCmd) == FALSE)
	{
		printError("Unknown Command Prefix");
		return;
	}

	pCmd++;

	switch(*pCmd)
	{
		case 'f':
		case 'F':
			pCmd++;
			pCmd = eatWhite(pCmd);
			if(pCmd == NULL)
			{
				printError("No Input File Name Found\n\r");
				break;
			}
			printf("Motion Database Input file:	%s\n\n\r", pCmd);
			strcpy(motnTxtFileName, pCmd);
         break;

		case 'o':
		case 'O':
			pCmd++;
			pCmd = eatWhite(pCmd);
			if(pCmd == NULL)
			{
				printError("No Output File Name Found\n\r");
				break;
			}
			if((strlen(pCmd)+ 1) > MAX_MOTION_SET_NAME_LEN)
			{
				char c[128];
				sprintf(c, "MOTION_SET_NAME: Longer than %d characters", MAX_MOTION_SET_NAME_LEN);
				printParseError(c);
				break;
			}
			printf("Motion Database Output file:	%s\n\n\r", pCmd);
			SetMotionSetName(pCmd);
         break;

		case 'g':
		case 'G':
			pCmd++;
			pCmd = eatWhite(pCmd);
			if(pCmd == NULL)
			{
				printError("No GBT File Name Found\n\r");
				break;
			}
			printf("Motion Database GBT file:	%s\n\n\r", pCmd);
			SetGBTName(pCmd);
         break;

		case 'v':
      case 'V':
			bVerbose = true;
         break;

		case 'p':
      case 'P':
			bPauseWhenDone = true;
         break;

 		case 't':
		case 'T':
			pCmd++;
			pCmd = eatWhite(pCmd);
			if( sscanf(pCmd, "%f", &errorTolerance) != 1 )
				printError("Invlaid floating point number following '-t' option - using default\n");
         break;

 		case 's':
		case 'S':
			pCmd++;
			pCmd = eatWhite(pCmd);
			if( sscanf(pCmd, "%f", &translationScaleFactor) != 1 )
				printError("Invalid floating point number following '-s' option - using default\n");
         break;



		default:
			printError("Unknown Command Type");
      	printf("Commands: -f xxxxxxxx.xxx	Input file\n\r");
         printf("				-v Verbose");
         printf("				-p Pause when done");
			break;
	}


}


/* -----------------2/11/99 12:42PM------------------
 * Returns a pointer skipping all space characters
 * returning a pointer to the first non white character.
 * --------------------------------------------------*/
#if 0 //djr: replicated in PARSE.CPP
char *eatWhite(char *pString)
{
	char curChar;

	while((char )*pString != 0)
	{
		curChar = *pString;

		/* -----------------2/11/99 12:47PM------------------
		 * If the curChar is not a space character then return
		 * a pointer to this character.
		 * --------------------------------------------------*/
		if(strchr(spaceString, curChar) == NULL)
			return(pString);

		pString++;
	}

	return(NULL);
}
#endif //djr

/* -----------------2/11/99 12:16PM------------------
 * Check to see if the commandline argument is a
 * Command symbol.
 * --------------------------------------------------*/
int isArg(char *pArg)
{
	char cmdChar;

	cmdChar = *pArg;
	if(strchr(cmdString, cmdChar) == NULL)
		return(FALSE);
	else
		return(TRUE);
}


/* -----------------2/11/99 3:56PM-------------------
 * Returns a pointer to the beginning of the
 * file name.
 * --------------------------------------------------*/
char *getFileName(char *pString)
{
   char *prevPtr, *curPtr;

   prevPtr = NULL;
   curPtr = pString;
   while((curPtr = strchr(curPtr, '\\')) != NULL)
   {
      curPtr++;
      prevPtr = curPtr;
   }
	if(prevPtr == NULL)
   	return(pString);
	else
	   return(prevPtr);
}


/* -----------------2/11/99 3:56PM-------------------
 * Replaces the '.' with a null terminator
 * --------------------------------------------------*/
void removeFileExt(char *pString)
{
	pString = strchr(pString, '.');
	if(pString != NULL)
		*pString = 0x00;
	return;
}

extern "C" {
void SendStatus(char *acMessage)
{
	//send out a warnign message from PluginError.c
	printf("%s", acMessage);
}
}