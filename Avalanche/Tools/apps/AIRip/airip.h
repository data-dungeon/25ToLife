/* --------------------------------------------------
 * File    : ANIMRIP.H
 * Created : Fri Apr  9 16:24:45 1999
 * Descript:
 * --------------------------------------------------*/
#ifndef __ANIMRIP_H
#define __ANIMRIP_H

/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Defines
 * --------------------------------------------------*/
#ifndef MATT
#define CMD_MOVETO 100
#define CMD_ORIGIN 101
#endif
#define CMD_START_MOTION	0
#define CMD_STOP_MOTION	1
#define CMD_KEY_FRAME	2
#define CMD_INTERP_FRAME	3
#define CMD_DELAY	4
#define CMD_FORCE	5
#define CMD_IMPULSE	6
#define CMD_JUMP	7
#define CMD_NEW_ANIM	8
#define CMD_CODE	9
#define CMD_IF_SET	10
#define CMD_IF_NOT_SET	11
#define CMD_IF_EQ	12
#define CMD_ACTION	13
#define CMD_WAIT_HIT_GROUND	14
#define CMD_WAIT_TIL_HOLD	15
#define CMD_SET_FLAG	16
#define CMD_CLR_FLAG	17
#define CMD_CREATE_PROC	18
#define CMD_KILL_PROC	19
#define CMD_SOUND	20
#define CMD_ATTACH_OBJ	21
#define CMD_DETACH_OBJ	22
#define CMD_SLAVE_POS	23
#define CMD_SLAVE_FORCE	24
#define CMD_SLAVE_IMPULSE 25
#define CMD_CHECK_DEAD	26
#define CMD_COLLISION	27
#define CMD_ROTATE		28
#define CMD_IMPULSE_X 29
#define CMD_IMPULSE_Y 30
#define CMD_IMPULSE_Z 31
#define CMD_OFFSET 32
#define CMD_CODE_1F 33
#define CMD_CODE_2F 34
#define CMD_CODE_3F 35
#define CMD_WAIT_INTERP_DONE 36
#define CMD_PLAY_RANDOM_SOUND 37
#define CMD_PLAY_SOUND 38
#define CMD_COLLISION_DELAY 39

#define MAX_NAME_LEN 100
#define LIST_INTEGER 1
#define LIST_FLOAT 2
#define LIST_OPCODE 3
#define LIST_ADDRESS 4
#define NEEDS_PATCH -1
#define ADDRESS_LEN 4
#define INT_LEN 4
#define FLOAT_LEN 4
#define OPCODE_LEN 4

typedef struct Hdr
{
	struct Hdr	*pNext;
	int			nType;
} ts_Hdr;

typedef struct DataInt
{
	ts_Hdr	hBlock;
	int		nData;
} ts_DataInt;

typedef struct DataFloat
{
	ts_Hdr		hBlock;
	float			fData;
} ts_DataFloat;

typedef struct DataAddress
{
	ts_Hdr			hBlock;
	unsigned long	ulData;
} ts_DataAddress;

typedef struct PatchDef
{
	struct PatchDef	*pNext;
	struct Hdr			*pData;
	char					caName[MAX_NAME_LEN];
} ts_Patch;

typedef struct SymbolDef
{
	struct SymbolDef	*pNext;
	unsigned long		ulValue;
	char					caName[MAX_NAME_LEN];

} ts_Symbol;

/* --------------------------------------------------
 * Exported Data
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
int ProcessAnimTable (char *pcString);
int ProcessDW (char *pcString);
int ProcessAnim (char *pcString);
int ProcessStartMotion (char *pcString);
int ProcessStopMotion ();
int ProcessKeyFrame (char *pcString);
int ProcessInterpFrame (char *pcString, int nFrames);
int ProcessDelay (int nDelay);
int ProcessForce (float fX, float fY, float fZ);
int ProcessImpulse (float fX, float fY, float fZ);
int ProcessImpulse_X (float fX);
int ProcessImpulse_Y (float fY);
int ProcessImpulse_Z (float fZ);
int ProcessOffset (float fX, float fY, float fZ);
int ProcessJump (char *pcString);
int ProcessNewAnim (char *pcString);
int ProcessCode (char *pcString);
int ProcessCode1F (char *pcString, float fX);
int ProcessCode2F (char *pcString, float fX, float fY);
int ProcessCode3F (char *pcString, float fX, float fY, float fZ);
int ProcessIfSet (char *pcString);
int ProcessIfNotSet (char *pcString);
int ProcessIfEq (char *pcString);
int ProcessAction (char *pcString);
int ProcessWaitHitGround ();
int ProcessWaitTilHold ();
int ProcessSetFlag (char *pcString);
int ProcessClrFlag (char *pcString);
int ProcessCreateProc (char *pcString, char *pcString1);
int ProcessKillProc (char *pcString);
int ProcessSound (char *pcString);
int ProcessAttachObj ();
int ProcessDetachObj ();
int ProcessSlavePos (float fX, float fY, float fZ);
int ProcessSlaveForce (float fX, float fY, float fZ);
int ProcessSlaveImpulse (float fX, float fY, float fZ);
int ProcessCheckDead ();
int ProcessCollision (char *pcString);
int ProcessRotate (float fX, float fY, float fZ);
int ProcessLabel (char *pcString);
int ProcessInclude (char *pcString);
void ProcessIncludes( void);
int ProcessDefine (char *pcString, int nValue);
int ProcessDefineHex (char *pcString, char *pcString1);
int ProcessWaitInterpDone ();
int ProcessPlayRandomSound (char *pcSoundTableIndex, char *pcChannel, char *pcVolume);
int ProcessPlaySound (char *pcSound, char *pcChannel, char *pcVolume);
int ProcessCollisionDelay (int nCollDelay);

int AddInt( char *pcRoutine, int nType, int nData);
int AddFloat( char *pcRoutine, float fData);
int AddAddress( char *pcRoutine, char *pcString);
int AddSymbol( char *pcRoutine, char *pcString, unsigned long ulValue);
void ExitWithError( void);

void AddToDataList( ts_Hdr *pCurData);
void AddToPatchList( ts_Patch *pCurPatch);
void AddToSymbolList( ts_Symbol *pCurSymbol);
void FreeMem( void);
#ifdef __cplusplus
}
#endif

#endif __ANIMRIP_H
