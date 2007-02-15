#include "stdafx.h"
#include "CommandLine.h"

static const char *spaceString 	= "; ,	|\n";     // semi-colon, comma, space, tab.
static const char	*cmdString		= "//+-";

static const float DEFAULT_ERROR_TOLERANCE  = 0.001f;
static const float DEFAULT_TRANSLATION_SCALE_FACTOR = 1.0f;

CommandLine::CommandLine(void) : 
	m_bVerbose(false),
	m_bPauseWhenDone(false),
	m_bExcludeModelBone(false),
	m_bWriteNamesAsChunk(false),
	m_transErrorTolerance(DEFAULT_ERROR_TOLERANCE),
	m_rotErrorTolerance(DEFAULT_ERROR_TOLERANCE),
	m_scaleErrorTolerance(DEFAULT_ERROR_TOLERANCE),
	m_bInputFileSetOutputFileName(false),
	m_bInputFileSetHeaderFileName(false)
{
}

bool CommandLine::ParseCommandLine(int argc, char **argv)
{
	const int MAX_CMD_LENGTH = 128;
	int	index;
	TupString commandString;

	for(index = 1; index < argc; index++)
	{
		commandString = argv[index];

		while(((index+1) < argc) && (!IsArg(argv[index+1])))
		{
			index++;
			commandString += argv[index];
		}
		ProcessCommand(commandString);
	}
	// return true if we have an input file
	return (m_inputFileName.GetLength()!=0);
}


/* -----------------2/11/99 1:02PM-------------------
* processCommand looks for a command line parameter
* and calls the appropriate function for the type.
* --------------------------------------------------*/
void CommandLine::ProcessCommand(const char *pCmd)
{
	if(IsArg(pCmd) == false)
	{
		PrintError("Unknown Command Prefix\n");
		return;
	}

	pCmd++;

	switch(*pCmd)
	{
		case 'f':
		case 'F':
			pCmd++;
			pCmd = EatWhiteSpace(pCmd);
			if(pCmd == NULL)
			{
				PrintError("No Input File Name Found\n");
				break;
			}
			printf("Motion Database Input file:	%s\n", pCmd);
			m_inputFileName = pCmd;
			break;

		case 'o':
		case 'O':
			pCmd++;
			pCmd = EatWhiteSpace(pCmd);
			if(pCmd == NULL)
			{
				PrintError("No Output File Name Found\n");
				break;
			}
			printf("Motion Database Output file:	%s\n", pCmd);
			m_outputFileName = pCmd;
			break;

		case 'h':
		case 'H':
			pCmd++;
			pCmd = EatWhiteSpace(pCmd);
			if(pCmd == NULL)
			{
				PrintError("No Output Header File Name Found\n");
				break;
			}
			printf("Motion Header Output file:	%s\n", pCmd);
			m_headerFileName = pCmd;
			break;

		case 'g':
		case 'G':
			pCmd++;
			pCmd = EatWhiteSpace(pCmd);
			if(pCmd == NULL)
			{
				PrintError("No Bone Group Table File Name Found\n");
				break;
			}
			printf("Motion Database Bone Group Table file:	%s\n", pCmd);
			m_boneGroupTableFileName = pCmd;
			break;

		case 's':
		case 'S':
			pCmd++;
			pCmd = EatWhiteSpace(pCmd);
			if(pCmd == NULL)
			{
				PrintError("No Save MSet File Name Found\n");
				break;
			}
			printf("Save MSet file:	%s\n", pCmd);
			m_outputMSetFileName = pCmd;
			break;


		case 'v':
		case 'V':
			m_bVerbose = true;
			break;

		case 'm':
		case 'M':
			m_bExcludeModelBone = true;
			break;

		case 'p':
		case 'P':
			m_bPauseWhenDone = true;
			break;

		case 'n':
		case 'N':
			m_bWriteNamesAsChunk = true;
			break;

		case 't':
		case 'T':
			{
				pCmd++;
				switch (*pCmd)
				{
					case 't':
					case 'T':
						pCmd++;
						pCmd = EatWhiteSpace(pCmd);
						if( sscanf(pCmd, "%f", &m_transErrorTolerance) != 1 )
							PrintError("Invlaid floating point number following '-tt' option - using default\n");
						break;
					case 'r':
					case 'R':
						pCmd++;
						pCmd = EatWhiteSpace(pCmd);
						if( sscanf(pCmd, "%f", &m_rotErrorTolerance) != 1 )
							PrintError("Invlaid floating point number following '-tr' option - using default\n");
						break;
					case 's':
					case 'S':
						pCmd++;
						pCmd = EatWhiteSpace(pCmd);
						if( sscanf(pCmd, "%f", &m_scaleErrorTolerance) != 1 )
							PrintError("Invlaid floating point number following '-ts' option - using default\n");
						break;
					default:
						pCmd = EatWhiteSpace(pCmd);
						if( sscanf(pCmd, "%f", &m_transErrorTolerance) != 1 )
						{
							PrintError("Invlaid floating point number following '-t' option - using default\n");
						}
						else
						{
							m_rotErrorTolerance = m_transErrorTolerance;
							m_scaleErrorTolerance = m_transErrorTolerance;
						}
						break;
				}
			}
			break;

		default:
			PrintError("Unknown Command Type");
			printf("Commands: -f filename (input file)\n");
			printf("				-o filename (output database file)\n");
			printf("				-h filename (output header file)\n");
			printf("				-g filename (input group bone table file)\n");
			printf("				-v (verbose mode)\n");
			printf("				-m (exclude model bone mode)\n");
			printf("				-t value (set error tolerance trans/rot/scale)\n");
			printf("				-tt value (set error tolerance translation only)\n");
			printf("				-tr value (set error tolerance rotation only)\n");
			printf("				-ts value (set error tolerance scale only)\n");
			printf("				-n (include motion names in output database file instead of header file)\n");
			printf("				-s filename (save .mset file)\n");
			break;
	}
}

const char *CommandLine::EatWhiteSpace(const char *pString)
{
	char curChar;

	while(*pString != 0)
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

/* -----------------2/11/99 12:16PM------------------
* Check to see if the commandline argument is a
* Command symbol.
* --------------------------------------------------*/
bool CommandLine::IsArg(const char *pArg)
{
	char cmdChar;

	cmdChar = *pArg;
	if(strchr(cmdString, cmdChar) == NULL)
		return(false);
	else
		return(true);
}

void CommandLine::PrintCommands(void)
{
	printf("MotionSet5.exe [-v] [-p] [-m] [-n] [-t f.fff] -f inputfile [-h headerfile] [-o outputfile] [-g GBTfile] [-s saveMSet file]\n");
	printf("  -f inputfile = input TXT file to process\n");
	printf("  -o outputfile = output MDB file to create (use filename on MOTION_SET_NAME of inputfile if outputfile not specified)\n");
	printf("  -h headerfile = output .h file with motion names\n");
	printf("  -g GBTfile = input GBT file to for bone-group mapping (use filename on MOTION_GROUPBONETABLE of inputfile if GBTfile not specified)\n");
	printf("  -s saveMSetfile = save a result mset file from input file\n");
	printf("  -v = verbose: LOTS of debug data is spewed\n");
	printf("  -p = pause: wait for keypress when done\n");
	printf("  -m = Exclude processing the model bone (bone zero)\n");
	printf("  -t value (set error tolerance trans/rot/scale)\n");
	printf("	 -tt value (set error tolerance translation only)\n");
	printf("	 -tr value (set error tolerance rotation only)\n");
	printf("	 -ts value (set error tolerance scale only)\n");
	printf("  -n = write motion names as separate chunk in output file\n");
	printf("\n");
	printf("inputfile commands:\n");
	printf("\n");
	printf("     # this is a comment\n");
	printf("     MOTION_FILE: filename\n");
	printf("     MOTION_GROUP_BONE_TABLE: GroupBoneTableFile\n");
	printf("     MOTION_ID: STRING_TO_EQUATE_TO_THIS_MOTIOND\n");
	printf("     MOTION_BONE_GROUP_FILTER: 2|3  (process only bonegroup 2 and 3 motion)\n");
	printf("     MOTION_BONE_GROUP_EXCLUDE: 2|3  (process all but bonegroup 2 and 3 motion)\n");
	printf("     MOTION_ERROR_TOLERANCE: float (set trans/rot/scale tolerances)\n");
	printf("     MOTION_ERROR_TOLERANCE_MULTIPLIER: float (set trans/rot/scale tolerance multipliers\n");
	printf("     MOTION_TRANSLATION_ERROR_TOLERANCE: float\n");
	printf("     MOTION_TRANSLATION_ERROR_TOLERANCE_MULTIPLIER: float\n");
	printf("     MOTION_ROTATION_ERROR_TOLERANCE: float\n");
	printf("     MOTION_ROTATION_ERROR_TOLERANCE_MULTIPLIER: float\n");
	printf("     MOTION_SCALE_ERROR_TOLERANCE: float\n");
	printf("     MOTION_SCALE_ERROR_TOLERANCE_MULTIPLIER: float\n");
	printf("     MOTION_USER_DATA: key=value (set user data using key and value\n");

}

void CommandLine::PrintError(const char *pString)
{
	printf("ERROR: %s", pString);
}
