/* --------------------------------------------------
 * File    : ANM_PROC.C
 * Created : Fri Apr  9 16:43:35 1999
 * Descript:
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
#include <conio.h>

#include "ai_gram.h"
#include "airip.h"

/* --------------------------------------------------
 * Local Defines
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Local Prototypes
 * --------------------------------------------------*/
extern int yyparse(void);
extern int yy_reset(void);
extern FILE *yyin;

/* --------------------------------------------------
 * Local Data
 * --------------------------------------------------*/
BOOL			bVerbose,
		      bBigEndian,
				bPauseWhenDone,
				bPCOutput;

int			nGlobalOffset = 0;


ts_Hdr		DataList,
 				*pDataList;

ts_Patch		PatchList,
 				*pPatchList;

ts_Symbol	SymbolList,
				*pSymbolList;

FILE 			*fInfile,
	 			*fOutfile;

unsigned long 	*pOutputArray;

extern int iLineCount;
extern char szFileName[];

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/
/* --------------------------------------------------
 * ProcessAiTable
 * --------------------------------------------------*/
int ProcessAiTable (char *pcString)
{
	if (bVerbose == TRUE)
	{
		printf("Got to AI_TABLE\n");
	}

	free(pcString);

	return TRUE;
}

/* --------------------------------------------------
 * ProcessDW
 * --------------------------------------------------*/
int ProcessDW (char *pcString)
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to DW\n");
	}

	nReturnVal = AddAddress( "DW", pcString);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += ADDRESS_LEN;
	free(pcString);

	return TRUE;
}

int	ProcessMoveto (float fX, float fY, float fZ)
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to MOVETO\n");
	}

	nReturnVal = AddInt( "MOVETO", LIST_OPCODE, CMD_MOVETO);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddFloat( "MOVETO", fX);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddFloat( "MOVETO", fY);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddFloat( "MOVETO", fZ);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN + FLOAT_LEN + FLOAT_LEN + FLOAT_LEN;

	return TRUE;
}

int	ProcessOrigin (float fX, float fY, float fZ)
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to ORIGIN\n");
	}

	nReturnVal = AddInt( "ORIGIN", LIST_OPCODE, CMD_ORIGIN);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddFloat( "ORIGIN", fX);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddFloat( "ORIGIN", fY);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddFloat( "ORIGIN", fZ);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN + FLOAT_LEN + FLOAT_LEN + FLOAT_LEN;

	return TRUE;
}

int	ProcessDelta (float fX)
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to DELTA\n");
	}

	nReturnVal = AddInt( "ORIGIN", LIST_OPCODE, CMD_ORIGIN);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddFloat( "ORIGIN", fX);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN + FLOAT_LEN;

	return TRUE;
}


/* --------------------------------------------------
 * ProcessAnim
 * --------------------------------------------------*/
int ProcessAnim (char *pcString)
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to ANIM\n");
	}

	nReturnVal = AddSymbol( "ANIM", pcString, (unsigned long)nGlobalOffset);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	free(pcString);

	return TRUE;
}

/* --------------------------------------------------
 * ProcessStartMotion
 * --------------------------------------------------*/
int ProcessStartMotion (char *pcString)
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to START_MOTION\n");
	}

	nReturnVal = AddInt( "START_MOTION", LIST_OPCODE, CMD_START_MOTION);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddAddress( "START_MOTION", pcString);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN + ADDRESS_LEN;
	free(pcString);

	return TRUE;
}

/* --------------------------------------------------
 * ProcessStopMotion
 * --------------------------------------------------*/
int ProcessStopMotion ()
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to STOP_MOTION\n");
	}

	nReturnVal = AddInt( "STOP_MOTION", LIST_OPCODE, CMD_STOP_MOTION);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessKeyFrame
 * --------------------------------------------------*/
int ProcessKeyFrame (char *pcString)
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to KEY_FRAME\n");
	}

	nReturnVal = AddInt( "KEY_FRAME", LIST_OPCODE, CMD_KEY_FRAME);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddAddress( "KEY_FRAME", pcString);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN + ADDRESS_LEN;
	free(pcString);

	return TRUE;
}

/* --------------------------------------------------
 * ProcessInterpFrame
 * --------------------------------------------------*/
int ProcessInterpFrame (char *pcString, int nFrames)
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to INTERP_FRAME\n");
	}

	nReturnVal = AddInt( "INTERP_FRAME", LIST_OPCODE, CMD_INTERP_FRAME);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddAddress( "INTERP_FRAME", pcString);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddInt( "INTERP_FRAME", LIST_INTEGER, nFrames);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN + ADDRESS_LEN + INT_LEN;
	free(pcString);

	return TRUE;
}

/* --------------------------------------------------
 * ProcessDelay
 * --------------------------------------------------*/
int ProcessDelay (int nDelay)
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to DELAY\n");
	}

	nReturnVal = AddInt( "DELAY", LIST_OPCODE, CMD_DELAY);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddInt( "DELAY", LIST_INTEGER, nDelay);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN + INT_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessForce
 * --------------------------------------------------*/
int ProcessForce (float fX, float fY, float fZ)
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to FORCE\n");
	}

	nReturnVal = AddInt( "FORCE", LIST_OPCODE, CMD_FORCE);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddFloat( "FORCE", fX);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddFloat( "FORCE", fY);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddFloat( "FORCE", fZ);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN + FLOAT_LEN + FLOAT_LEN + FLOAT_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessImpulse
 * --------------------------------------------------*/
int ProcessImpulse (float fX, float fY, float fZ)
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to IMPULSE\n");
	}

	nReturnVal = AddInt( "IMPULSE", LIST_OPCODE, CMD_IMPULSE);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddFloat( "IMPULSE", fX);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddFloat( "IMPULSE", fY);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddFloat( "IMPULSE", fZ);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN + FLOAT_LEN + FLOAT_LEN + FLOAT_LEN;

	return TRUE;
}

int ProcessImpulse_X ( float fX )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to IMPULSE_X\n");
	}

	nReturnVal = AddInt( "IMPULSE_X", LIST_OPCODE, CMD_IMPULSE_X);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddFloat( "IMPULSE_X", fX);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN + FLOAT_LEN;

	return TRUE;
}

int ProcessImpulse_Y ( float fY )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to IMPULSE_Y\n");
	}

	nReturnVal = AddInt( "IMPULSE_Y", LIST_OPCODE, CMD_IMPULSE_Y);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddFloat( "IMPULSE_Y", fY);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN + FLOAT_LEN;

	return TRUE;
}

int ProcessImpulse_Z ( float fZ )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to IMPULSE_Z\n");
	}

	nReturnVal = AddInt( "IMPULSE_Z", LIST_OPCODE, CMD_IMPULSE_Z);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddFloat( "IMPULSE_Z", fZ);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN + FLOAT_LEN;

	return TRUE;
}

int ProcessOffset( float fX, float fY, float fZ )
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to OFFSET\n");
	}

	nReturnVal = AddInt( "OFFSET", LIST_OPCODE, CMD_OFFSET);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddFloat( "OFFSET", fX);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddFloat( "OFFSET", fY);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddFloat( "OFFSET", fZ);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN + FLOAT_LEN + FLOAT_LEN + FLOAT_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessJump
 * --------------------------------------------------*/
int ProcessJump (char *pcString)
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to JUMP\n");
	}

	nReturnVal = AddInt( "JUMP", LIST_OPCODE, CMD_JUMP);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddAddress( "JUMP", pcString);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN + ADDRESS_LEN;
	free(pcString);

	return TRUE;
}

/* --------------------------------------------------
 * ProcessNewAnim
 * --------------------------------------------------*/
int ProcessNewAnim (char *pcString)
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to NEW_ANIM\n");
	}

	nReturnVal = AddInt( "NEW_ANIM", LIST_OPCODE, CMD_NEW_ANIM);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddAddress( "NEW_ANIM", pcString);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN + ADDRESS_LEN;
	free(pcString);

	return TRUE;
}

/* --------------------------------------------------
 * ProcessCode
 * --------------------------------------------------*/
int ProcessCode (char *pcString)
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to CODE\n");
	}

	nReturnVal = AddInt( "CODE", LIST_OPCODE, CMD_CODE);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddAddress( "CODE", pcString);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN + ADDRESS_LEN;
	free(pcString);

	return TRUE;
}

/* --------------------------------------------------
 * ProcessCode1F
 * --------------------------------------------------*/
int ProcessCode1F (char *pcString, float fX)
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to CODE\n");
	}

	nReturnVal = AddInt( "CODE", LIST_OPCODE, CMD_CODE);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddAddress( "CODE", pcString);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddFloat( "CODE", fX);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN + ADDRESS_LEN + FLOAT_LEN;
	free(pcString);

	return TRUE;
}

/* --------------------------------------------------
 * ProcessCode2F
 * --------------------------------------------------*/
int ProcessCode2F (char *pcString, float fX, float fY)
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to CODE\n");
	}

	nReturnVal = AddInt( "CODE", LIST_OPCODE, CMD_CODE);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddAddress( "CODE", pcString);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddFloat( "CODE", fX);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddFloat( "CODE", fY);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN + ADDRESS_LEN + FLOAT_LEN + FLOAT_LEN;
	free(pcString);

	return TRUE;
}

/* --------------------------------------------------
 * ProcessCode3F
 * --------------------------------------------------*/
int ProcessCode3F (char *pcString, float fX, float fY, float fZ)
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to CODE\n");
	}

	nReturnVal = AddInt( "CODE", LIST_OPCODE, CMD_CODE);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddAddress( "CODE", pcString);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddFloat( "CODE", fX);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddFloat( "CODE", fY);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddFloat( "CODE", fZ);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN + ADDRESS_LEN + FLOAT_LEN + FLOAT_LEN + FLOAT_LEN;
	free(pcString);

	return TRUE;
}

/* --------------------------------------------------
 * ProcessIfSet
 * --------------------------------------------------*/
int ProcessIfSet (char *pcString)
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to IF_SET\n");
	}

	nReturnVal = AddInt( "IF_SET", LIST_OPCODE, CMD_IF_SET);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddAddress( "IF_SET", pcString);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN + ADDRESS_LEN;
	free(pcString);

	return TRUE;
}

/* --------------------------------------------------
 * ProcessIfNotSet
 * --------------------------------------------------*/
int ProcessIfNotSet (char *pcString)
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to IF_NOT_SET\n");
	}

	nReturnVal = AddInt( "IF_NOT_SET", LIST_OPCODE, CMD_IF_NOT_SET);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddAddress( "IF_NOT_SET", pcString);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN + ADDRESS_LEN;
	free(pcString);

	return TRUE;
}

/* --------------------------------------------------
 * ProcessIfEq
 * --------------------------------------------------*/
int ProcessIfEq (char *pcString)
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to IF_EQ\n");
	}

	nReturnVal = AddInt( "IF_EQ", LIST_OPCODE, CMD_IF_EQ);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddAddress( "IF_EQ", pcString);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN + ADDRESS_LEN;
	free(pcString);

	return TRUE;
}

/* --------------------------------------------------
 * ProcessAction
 * --------------------------------------------------*/
int ProcessAction (char *pcString)
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to ACTION\n");
	}

	nReturnVal = AddInt( "ACTION", LIST_OPCODE, CMD_ACTION);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddAddress( "ACTION", pcString);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN + ADDRESS_LEN;
	free(pcString);

	return TRUE;
}

/* --------------------------------------------------
 * ProcessWaitHitGround
 * --------------------------------------------------*/
int ProcessWaitHitGround ()
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to WAIT_HIT_GROUND\n");
	}

	nReturnVal = AddInt( "WAIT_HIT_GROUND", LIST_OPCODE, CMD_WAIT_HIT_GROUND);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessWaitTilHold
 * --------------------------------------------------*/
int ProcessWaitTilHold ()
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to WAIT_TIL_HOLD\n");
	}

	nReturnVal = AddInt( "WAIT_TIL_HOLD", LIST_OPCODE, CMD_WAIT_TIL_HOLD);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessWaitInterpDone
 * --------------------------------------------------*/
int ProcessWaitInterpDone ()
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to WAIT_INTERP_DONE\n");
	}

	nReturnVal = AddInt( "WAIT_INTERP_DONE", LIST_OPCODE, CMD_WAIT_INTERP_DONE);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}

	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessPlayRandomSound
 * --------------------------------------------------*/
int ProcessPlayRandomSound (char *pcSoundTableIndex, char *pcChannel, char *pcVolume)
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to PLAY_RANDOM_SOUND\n");
	}

	nReturnVal = AddInt( "PLAY_RANDOM_SOUND", LIST_OPCODE, CMD_PLAY_RANDOM_SOUND);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddAddress( "PLAY_RANDOM_SOUND", pcSoundTableIndex);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddAddress( "PLAY_RANDOM_SOUND", pcChannel);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddAddress( "PLAY_RANDOM_SOUND", pcVolume);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN + ADDRESS_LEN + ADDRESS_LEN + ADDRESS_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessPlaySound
 * --------------------------------------------------*/
int ProcessPlaySound (char *pcSound, char *pcChannel, char *pcVolume)
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to PLAY_SOUND\n");
	}

	nReturnVal = AddInt( "PLAY_SOUND", LIST_OPCODE, CMD_PLAY_SOUND);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddAddress( "PLAY_SOUND", pcSound);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddAddress( "PLAY_SOUND", pcChannel);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddAddress( "PLAY_SOUND", pcVolume);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN + ADDRESS_LEN + ADDRESS_LEN + ADDRESS_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessCollisionDelay
 * --------------------------------------------------*/
int ProcessCollisionDelay (int nCollDelay)
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to COLLISION_DELAY\n");
	}

	nReturnVal = AddInt( "COLLISION_DELAY", LIST_OPCODE, CMD_COLLISION_DELAY);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddInt( "COLLISION_DELAY", LIST_INTEGER, nCollDelay);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN + INT_LEN;

	return TRUE;
}


/* --------------------------------------------------
 * ProcessSetFlag
 * --------------------------------------------------*/
int ProcessSetFlag (char *pcString)
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to SET_FLAG\n");
	}

	nReturnVal = AddInt( "SET_FLAG", LIST_OPCODE, CMD_SET_FLAG);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddAddress( "SET_FLAG", pcString);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN + ADDRESS_LEN;
	free(pcString);

	return TRUE;
}

/* --------------------------------------------------
 * ProcessClrFlag
 * --------------------------------------------------*/
int ProcessClrFlag (char *pcString)
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to CLR_FLAG\n");
	}

	nReturnVal = AddInt( "CLR_FLAG", LIST_OPCODE, CMD_CLR_FLAG);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddAddress( "CLR_FLAG", pcString);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN + ADDRESS_LEN;
	free(pcString);

	return TRUE;
}

/* --------------------------------------------------
 * ProcessCreateProc
 * --------------------------------------------------*/
int ProcessCreateProc (char *pcString, char *pcString1)
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to CREATE_PROC\n");
	}

	nReturnVal = AddInt( "CREATE_PROC", LIST_OPCODE, CMD_CREATE_PROC);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddAddress( "CREATE_PROC", pcString);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddAddress( "CREATE_PROC", pcString1);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN + ADDRESS_LEN + ADDRESS_LEN;
	free(pcString);
	free(pcString1);

	return TRUE;
}

/* --------------------------------------------------
 * ProcessKillProc
 * --------------------------------------------------*/
int ProcessKillProc (char *pcString)
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to KILL_PROC\n");
	}

	nReturnVal = AddInt( "KILL_PROC", LIST_OPCODE, CMD_KILL_PROC);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddAddress( "KILL_PROC", pcString);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN + ADDRESS_LEN;
	free(pcString);

	return TRUE;
}

/* --------------------------------------------------
 * ProcessSound
 * --------------------------------------------------*/
int ProcessSound (char *pcString)
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to SOUND\n");
	}

	nReturnVal = AddInt( "SOUND", LIST_OPCODE, CMD_SOUND);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddAddress( "SOUND", pcString);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN + ADDRESS_LEN;
	free(pcString);

	return TRUE;
}

/* --------------------------------------------------
 * ProcessAttachObj
 * --------------------------------------------------*/
int ProcessAttachObj ()
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to ATTACH_OBJ\n");
	}

	nReturnVal = AddInt( "ATTACH_OBJ", LIST_OPCODE, CMD_ATTACH_OBJ);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessDetachObj
 * --------------------------------------------------*/
int ProcessDetachObj ()
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to DETACH_OBJ\n");
	}

	nReturnVal = AddInt( "DETACH_OBJ", LIST_OPCODE, CMD_DETACH_OBJ);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessSlavePos
 * --------------------------------------------------*/
int ProcessSlavePos (float fX, float fY, float fZ)
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to SLAVE_POS\n");
	}

	nReturnVal = AddInt( "SLAVE_POS", LIST_OPCODE, CMD_SLAVE_POS);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddFloat( "SLAVE_POS", fX);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddFloat( "SLAVE_POS", fY);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddFloat( "SLAVE_POS", fZ);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN + FLOAT_LEN + FLOAT_LEN + FLOAT_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessSlaveForce
 * --------------------------------------------------*/
int ProcessSlaveForce (float fX, float fY, float fZ)
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to SLAVE_FORCE\n");
	}

	nReturnVal = AddInt( "SLAVE_FORCE", LIST_OPCODE, CMD_SLAVE_FORCE);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddFloat( "SLAVE_FORCE", fX);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddFloat( "SLAVE_FORCE", fY);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddFloat( "SLAVE_FORCE", fZ);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN + FLOAT_LEN + FLOAT_LEN + FLOAT_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessSlaveImpulse
 * --------------------------------------------------*/
int ProcessSlaveImpulse (float fX, float fY, float fZ)
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to SLAVE_IMPULSE\n");
	}

	nReturnVal = AddInt( "SLAVE_IMPULSE", LIST_OPCODE, CMD_SLAVE_IMPULSE);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddFloat( "SLAVE_IMPULSE", fX);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddFloat( "SLAVE_IMPULSE", fY);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddFloat( "SLAVE_IMPULSE", fZ);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN + FLOAT_LEN + FLOAT_LEN + FLOAT_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessCheckDead
 * --------------------------------------------------*/
int ProcessCheckDead ()
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to CHECK_DEAD\n");
	}

	nReturnVal = AddInt( "CHECK_DEAD", LIST_OPCODE, CMD_CHECK_DEAD);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessCollision
 * --------------------------------------------------*/
int ProcessCollision (char *pcString)
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to COLLISION\n");
	}

	nReturnVal = AddInt( "COLLISION", LIST_OPCODE, CMD_COLLISION);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddAddress( "COLLISION", pcString);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN + ADDRESS_LEN;
	free(pcString);

	return TRUE;
}

/* --------------------------------------------------
 * ProcessRotate
 * --------------------------------------------------*/
int ProcessRotate (float fX, float fY, float fZ)
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to ROTATE\n");
	}

	nReturnVal = AddInt( "ROTATE", LIST_OPCODE, CMD_ROTATE);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddFloat( "ROTATE", fX);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddFloat( "ROTATE", fY);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nReturnVal = AddFloat( "ROTATE", fZ);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	nGlobalOffset += OPCODE_LEN + FLOAT_LEN + FLOAT_LEN + FLOAT_LEN;

	return TRUE;
}

/* --------------------------------------------------
 * ProcessLabel
 * --------------------------------------------------*/
int ProcessLabel (char *pcString)
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to LABEL\n");
	}

 	// Remove the colon
	pcString[strlen(pcString)-1] = 0;

	nReturnVal = AddSymbol( "LABEL", pcString, (unsigned long)nGlobalOffset);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	free(pcString);

	return TRUE;
}

/* --------------------------------------------------
 * ProcessInclude
 * --------------------------------------------------*/
int ProcessInclude (char *pcString)
{
	free(pcString);
	return TRUE;
}

void ProcessIncludes (void)
{
	int iSaveLineCount;
	char szSaveFileName[256];

	FILE	*fIncludeFile,
			*fTempFile;

	long	lFilePos;

	char	caString[100],
			caString1[100];

	if (bVerbose == TRUE)
	{
		printf("Got to INCLUDE\n");
	}

	while (1)
	{
		lFilePos = ftell( fInfile);
		if (fscanf( fInfile, "%s %s", caString, caString1) == 2)
		{
			if (strcmp( "#include",caString) == 0)
			{
				// Open include file

				// save our current filename and line count
				iSaveLineCount = iLineCount;
				strcpy( szSaveFileName, szFileName );

				strcpy( szFileName, caString1 );
				iLineCount = 1;
				fIncludeFile = fopen( caString1, "rt");
				fTempFile = yyin;
				yyin = fIncludeFile;

				yy_reset();
				yyparse();

				yyin = fTempFile;

				fclose( fIncludeFile);

				// restore the filename and linecount, but add one to the linecount, for the include line, which
				// is not parsed by yacc
				iLineCount = iSaveLineCount;
				iLineCount++;
				strcpy( szFileName, szSaveFileName );
			}
			else
			{
				fseek( fInfile, lFilePos, SEEK_SET);	// Replace the file pointer to the right position
				break;
			}
		}
	}
}

/* --------------------------------------------------
 * ProcessDefine
 * --------------------------------------------------*/
int ProcessDefine (char *pcString, int nValue)
{
	int	nReturnVal;

	if (bVerbose == TRUE)
	{
		printf("Got to DEFINE\n");
	}

	nReturnVal = AddSymbol( "DEFINE", pcString, (unsigned long)nValue);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	free(pcString);

	return TRUE;
}

/* --------------------------------------------------
 * ProcessDefineHex
 * --------------------------------------------------*/
int ProcessDefineHex (char *pcString, char *pcString1)
{
	int	nReturnVal,
			nValue;
	char	*pcTemp;

	if (bVerbose == TRUE)
	{
		printf("Got to DEFINEHEX\n");
	}
	nValue = (int) strtol( pcString1, &pcTemp, 16);
	nReturnVal = AddSymbol( "DEFINE", pcString, (unsigned long)nValue);
	if (nReturnVal == FALSE)
	{
		ExitWithError();
	}
	free(pcString);

	return TRUE;
}

/* --------------------------------------------------
 * 	AddInt
 * --------------------------------------------------*/
int AddInt (char *pcRoutine, int nType, int nData)
{
	ts_DataInt 		*pCurOpcode;

	// Allocate the opcode link for the data list
	pCurOpcode = (ts_DataInt *) malloc( sizeof(ts_DataInt));
	if (pCurOpcode == NULL)
	{
		printf( "Could not allocate memory in %s (for data list opcode).\n", pcRoutine);
		return FALSE;
	}
	pCurOpcode->hBlock.pNext = NULL;
	pCurOpcode->hBlock.nType = nType;
	pCurOpcode->nData = nData;
	AddToDataList( (ts_Hdr *) pCurOpcode);

	return TRUE;
}

/* --------------------------------------------------
 * 	AddFloat
 * --------------------------------------------------*/
int AddFloat (char *pcRoutine, float fData)
{
	ts_DataFloat	*pCurFloat;

	// Allocate the float link for the data list
	pCurFloat = (ts_DataFloat *) malloc( sizeof(ts_DataFloat));
	if (pCurFloat == NULL)
	{
		printf( "Could not allocate memory in %s (for data list float).\n", pcRoutine);
		return FALSE;
	}
	pCurFloat->hBlock.pNext = NULL;
	pCurFloat->hBlock.nType = LIST_FLOAT;
	pCurFloat->fData = fData;
	AddToDataList( (ts_Hdr *) pCurFloat);

	return TRUE;
}

/* --------------------------------------------------
 * 	AddAddress
 * --------------------------------------------------*/
int AddAddress( char *pcRoutine, char *pcString)
{
	ts_DataAddress 	*pCurAddress;
	ts_Patch		*pCurPatch;

	// Allocate the address link for the data list
	pCurAddress = (ts_DataAddress *) malloc( sizeof(ts_DataAddress));
	if (pCurAddress == NULL)
	{
		printf( "Could not allocate memory in %s (for data list address).\n",pcRoutine);
		return FALSE;
	}
	pCurAddress->hBlock.pNext = NULL;
	pCurAddress->hBlock.nType = LIST_ADDRESS;
	pCurAddress->ulData = NEEDS_PATCH;
	AddToDataList( (ts_Hdr *) pCurAddress);

	// Allocate the new link for the patch list
	pCurPatch = (ts_Patch *) malloc( sizeof(ts_Patch));
	if (pCurPatch == NULL)
	{
		printf( "Could not allocate memory in %s (for patch list).\n", pcRoutine);
		return FALSE;
	}
	pCurPatch->pNext = NULL;
	pCurPatch->pData = (ts_Hdr *) pCurAddress;
	strcpy( pCurPatch->caName, pcString);
	AddToPatchList( pCurPatch);

	return TRUE;
}

/* --------------------------------------------------
 * 	AddSymbol
 * --------------------------------------------------*/
int AddSymbol( char *pcRoutine, char *pcString, unsigned long ulValue)
{
	ts_Symbol		*pCurSymbol;

	// Allocate the new link for the symbol list
	pCurSymbol = (ts_Symbol *) malloc( sizeof(ts_Symbol));
	if (pCurSymbol == NULL)
	{
		printf( "Could not allocate memory in %s (for symbol list).\n", pcRoutine);
		return FALSE;
	}
	pCurSymbol->pNext = NULL;
	pCurSymbol->ulValue = ulValue;
	strcpy( pCurSymbol->caName, pcString);
	AddToSymbolList( pCurSymbol);

	return TRUE;
}

/* --------------------------------------------------
 * 	AddToDataList
 * --------------------------------------------------*/
void AddToDataList (ts_Hdr *pCurData)
{
	pDataList->pNext = pCurData;
	pDataList = pCurData;
}

/* --------------------------------------------------
 * 	AddToPatchList
 * --------------------------------------------------*/
void AddToPatchList (ts_Patch *pCurPatch)
{
	pPatchList->pNext = pCurPatch;
	pPatchList = pCurPatch;
}

/* --------------------------------------------------
 * 	AddToSymbolList
 * --------------------------------------------------*/
void AddToSymbolList (ts_Symbol *pCurSymbol)
{
	pSymbolList->pNext = pCurSymbol;
	pSymbolList = pCurSymbol;
}

/* --------------------------------------------------
 * 	ExitWithError
 * --------------------------------------------------*/
void ExitWithError( void)
{
	// Perform cleanup of all allocated memory and files
	FreeMem();
  if( fInfile )
    fclose(fInfile);
  if( fOutfile )
    fclose(fOutfile);

  if( bPauseWhenDone )
    getch();

  exit(1);
}

/* --------------------------------------------------
 * 	FreeMem
 * --------------------------------------------------*/
void FreeMem( void)
{
	ts_Hdr		*pTempData;
	ts_Patch	*pTempPatch;
	ts_Symbol	*pTempSymbol;

	pDataList = &DataList;
	pDataList = pDataList->pNext;		//Skip anchor
	if (pDataList != NULL)
	{
		while (pDataList->pNext != NULL)
		{
			pTempData = pDataList;
			pDataList = pDataList->pNext;
			free(pTempData);
		}
		free(pDataList);
	}

	pPatchList = &PatchList;
	pPatchList = pPatchList->pNext;		//Skip anchor
	if (pPatchList != NULL)
	{
		while (pPatchList->pNext != NULL)
		{
			pTempPatch = pPatchList;
			pPatchList = pPatchList->pNext;
			free(pTempPatch);
		}
		free(pPatchList);
	}

	pSymbolList = &SymbolList;
	pSymbolList = pSymbolList->pNext;	//Skip anchor
	if (pSymbolList != NULL)
	{
		while (pSymbolList->pNext != NULL)
		{
			pTempSymbol = pSymbolList;
			pSymbolList = pSymbolList->pNext;
			free(pTempSymbol);
		}
		free(pSymbolList);
	}

	if (pOutputArray != NULL)
	{
		free(pOutputArray);
	}
}

/* --------------------------------------------------
 * Local Functions
 * --------------------------------------------------*/
