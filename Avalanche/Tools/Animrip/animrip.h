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

enum OP_CODES {
  CMD_START_MOTION = 0,
  CMD_KILL_MOTION,
  CMD_KEY_FRAME,
  CMD_INTERP_FRAME,
  CMD_DELAY,
  CMD_END_ANIM,
  CMD_FORCE,
  CMD_IMPULSE,
  CMD_JUMP,
  CMD_NEW_ANIM,
  CMD_CODE,
  CMD_IF_SET,
  CMD_IF_NOT_SET,
  CMD_IF_EQ,
  CMD_ACTION,
  CMD_WAIT_HIT_GROUND,
  CMD_WAIT_TIL_HOLD,
  CMD_WAIT_FOR_PAUSE,
  CMD_WAIT_FOR_FRAME,
  CMD_WAIT_FOR_END,
  CMD_SET_FLAG,
  CMD_CLR_FLAG,
  CMD_CREATE_PROC,
  CMD_KILL_PROC,
  CMD_SOUND,
  CMD_ATTACH_OBJ,
  CMD_DETACH_OBJ,
  CMD_SLAVE_POS,
  CMD_SLAVE_FORCE,
  CMD_SLAVE_IMPULSE,
  CMD_CHECK_DEAD,
  CMD_COLLISION,
  CMD_ROTATE,
  CMD_IMPULSE_X,
  CMD_IMPULSE_Y,
  CMD_IMPULSE_Z,
  CMD_OFFSET,
  CMD_CODE_1F,
  CMD_CODE_2F,
  CMD_CODE_3F,
  CMD_WAIT_INTERP_DONE,
  CMD_PLAY_RANDOM_SOUND,
  CMD_PLAY_SOUND,
  CMD_PLAY_SOUND_VOLUME,
  CMD_COLLISION_DELAY,
  CMD_PAUSE_MOTION,
  CMD_RESUME_MOTION,
  CMD_CODE_1I,
  CMD_IF_LE,
  CMD_IF_LT,
  CMD_IF_NE,
  CMD_IF_GE,
  CMD_IF_GT,
  CMD_GET_RANDOM,
  CMD_GET_FRAME,
  CMD_GET_ACTION,
  CMD_TEST_VALUE,
  CMD_TEST_FLAG,
  CMD_TEST_ACTION,
  CMD_TEST_ENDMOTION,
  CMD_TEST_RANDOM,
  CMD_TEST_FRAME,
  CMD_TEST_INTERPDONE,
  CMD_CASE,
  CMD_RES_OP0A,
  CMD_RES_OP0B,
  CMD_RES_OP1F,
  CMD_RES_OP1I,
  CMD_TEST_ONGROUND,

  CMD_PUSHI,
  CMD_PUSHF,
  CMD_POP,
  CMD_SWAP,
  CMD_DUP,
  CMD_LOAD,
  CMD_STORE,
  CMD_ADD,
  CMD_SUB,
  CMD_MUL,
  CMD_DIV,
  CMD_MOD,
  CMD_BIT_AND,
  CMD_BIT_OR,
  CMD_BIT_XOR,
  CMD_BIT_NOT,
  CMD_AND,
  CMD_OR,
  CMD_NOT,
  CMD_INT,
  CMD_FLOAT,
  CMD_CMP,
  CMD_JNE,
  CMD_JE,
  CMD_JL,
  CMD_JG,
  CMD_JLE,
  CMD_JGE,
  CMD_CALLC,
  CMD_CALL,
  CMD_RETURN,
  CMD_PUSHS,

  CMD_SLEEP,

  CMD_START_MOTION2,
  CMD_KILL_MOTION2,
  CMD_TEST_ENDMOTION2,
  CMD_TEST_FRAME2,
  CMD_WAIT_FOR_END2,
  CMD_WAIT_FOR_FRAME2,
  CMD_INTERP_FRAME2,
  CMD_WAIT_INTERP_DONE2,
  CMD_PAUSE_MOTION2,
  CMD_RESUME_MOTION2,
  CMD_GET_FRAME2,

  CMD_MIN,
  CMD_MAX,
  CMD_ABS,

  CMD_BNE,
  CMD_BE,
  CMD_BL,
  CMD_BG,
  CMD_BLE,
  CMD_BGE,

  CMD_PUSHREG,
  CMD_POPREG,

  CMD_DEREF,
  CMD_DEREF_INT,
  CMD_DEREF_FLOAT,

  CMD_PUSH_CALL,
  CMD_POP_CALL,
  CMD_LOAD_CALL,
  CMD_STORE_CALL,

  CMD_SET_ANIM_RATE,
  CMD_SET_ANIM_RATE2,

  CMD_SET_ACTION_LO,
  CMD_GET_ACTION_LO,
  CMD_SET_ACTION_HI,
  CMD_GET_ACTION_HI,

  CMD_SHL,
  CMD_SHR,
  CMD_SHRZ,

  CMD_PLAY_SOUND_STR,
  CMD_PLAY_SOUND_VOLUME_STR,

  CMD_OP_CODE_COUNT
};

#define MAX_NAME_LEN    100
#define LIST_INTEGER    0x01
#define LIST_FLOAT      0x02
#define LIST_OPCODE     0x03
#define LIST_ADDRESS    0x04
#define LIST_DATAWORD   0x05

#define NEEDS_PATCH     -1
#define ADDRESS_LEN     sizeof( void * )
#define INT_LEN         sizeof( int )
#define FLOAT_LEN       sizeof( float )
#define OPCODE_LEN      sizeof( unsigned int )
#define STRING_LEN      sizeof( char * )
#define DATAWORD_LEN    sizeof( unsigned int )
#define INT_TYPE        0x01
#define FLOAT_TYPE      0x02
#define STRING_TYPE     0x03
#define DATAWORD_TYPE   0x04

#define PUBLISHED_TYPE      0x80
#define UNINITIALIZED_TYPE  0x40
#define REFERENCE_TYPE      0x20
#define UNUSED_TYPE_FLAG    0x10
#define BASE_TYPE_MASK      0xF0

/* known chunk types for db output */

#define EXPORT_ANIM_SCRIPT		0
#define EXPORT_AI_SCRIPT		1

typedef struct Hdr
{
	struct Hdr	*pNext;
  struct Hdr  *pPrev;
	int			nType;
  int     line;
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

typedef struct BackPatchDef
{
	struct BackPatchDef	*pNext;
	struct Hdr			*pData;
	char					caName[MAX_NAME_LEN];
} ts_BackPatch;

typedef struct SymbolDef
{
	struct SymbolDef	*pNext;
	unsigned long		  ulValue;
  int               line;
  unsigned int      uVarType;
  int               iInitialValue;
  int               iNameIndex;
  int               bInitialized;
	char					    caName[MAX_NAME_LEN];
} ts_Symbol;

#ifdef __cplusplus
extern "C" {
#endif
/* --------------------------------------------------
 * Exported Data
 * --------------------------------------------------*/

extern int nExportType;

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/
/*
int ProcessVar( char* varName );
int ProcessVarInitF( char* varName, float initVal );
int ProcessVarInitI( char* varName, int initVal );
*/
int ProcessCppLine( int line, char* file );
int ProcessPushF( float fVal );
int ProcessPushI( int iVal );
int ProcessPushA( char* pcString );
int ProcessPushS( char* pcString );
int ProcessPop( void );
int ProcessSwap( void );
int ProcessDup( void );
int ProcessLoad( char* varName );
int ProcessStore( char* varName );
int ProcessStoreVar( char* dstName, char* srcName );
int ProcessStoreF( char* dstName, float fVal );
int ProcessStoreI( char* dstName, int iVal );
int ProcessAdd( void );
int ProcessSub( void );
int ProcessMul( void );
int ProcessDiv( void );
int ProcessMod( void );
int ProcessBitAnd( void );
int ProcessBitOr( void );
int ProcessBitXor( void );
int ProcessBitNot( void );
int ProcessAnd( void );
int ProcessOr( void );
int ProcessNot( void );
int ProcessInt( void );
int ProcessFloat( void );
int ProcessCMP( void );
int ProcessJNET( char* jumpAddr );
int ProcessJNE( void );
int ProcessJET( char* jumpAddr );
int ProcessJE( void );
int ProcessJLT( char* jumpAddr );
int ProcessJL( void );
int ProcessJGT( char* jumpAddr );
int ProcessJG( void );
int ProcessJLET( char* jumpAddr );
int ProcessJLE( void );
int ProcessJGET( char* jumpAddr );
int ProcessJGE( void );
int ProcessCallC( char* funcName );
int ProcessCallT( char* pcString );
int ProcessCall( void );
int ProcessReturn( void );
int ProcessAIScript( void );

int ProcessBNET( char *jumpAddr );
int ProcessBNE( void );
int ProcessBET( char *jumpAddr );
int ProcessBE( void );
int ProcessBLT( char *jumpAddr );
int ProcessBL( void );
int ProcessBGT( char *jumpAddr );
int ProcessBG( void );
int ProcessBLET( char *jumpAddr );
int ProcessBLE( void );
int ProcessBGET( char *jumpAddr );
int ProcessBGE( void );

int ProcessPushReg( void );
int ProcessPopReg( void );

int ProcessAnimTable( char *pcString );
int ProcessDW( char *pcString );
int ProcessAnim( char *pcString );
int ProcessStartMotion( int nMotionNumber, int nStartFrame, int nFlags );
int ProcessKillMotion( void );
int ProcessKeyFrame( int iFrame );
int ProcessInterpFrame( int nMotionNumber, int nStartFrame, int nFlags, int nFrames );
int ProcessDelay( int nDelay );
int ProcessForce( float fX, float fY, float fZ );
int ProcessForceT( char *pX, char *pY, char *pZ );
int ProcessImpulse( float fX, float fY, float fZ );
int ProcessImpulse_X( float fX );
int ProcessImpulse_Y( float fY );
int ProcessImpulse_Z( float fZ );
int ProcessOffset( float fX, float fY, float fZ );
int ProcessJump( void );
int ProcessJumpT( char *pcString );
int ProcessNewAnim( int nAnim );
int ProcessCode( int nFuncIndex );
int ProcessCode1I( int nFuncIndex, int iVal );
int ProcessCode1F( int nFuncIndex, float fX );
int ProcessCode2F( int nFuncIndex, float fX, float fY );
int ProcessCode3F( int nFuncIndex, float fX, float fY, float fZ );
int ProcessIfSet( char *pcString );
int ProcessIfNotSet( char *pcString );
int ProcessIfEq( char *pcString );
int ProcessAction( int nAction );
int ProcessWaitHitGround( void );
int ProcessWaitTilHold( void );
int ProcessSetFlag( int nFlags );
int ProcessClrFlag( int nFlags );
int ProcessCreateProc( int nProcNumber, int nProcID );
int ProcessKillProc( int nProcID );
int ProcessSound( int nSequence );
int ProcessPlayRandomSound( int nSoundTableIndex, int nChannel, int nVolume );
int ProcessPlaySound( int nSoundTableIndex );
int ProcessPlaySoundStr( char *pcString );
int ProcessPlaySoundVolume( int nSoundTableIndex, int nVolume );
int ProcessPlaySoundVolumeStr( char *pcString, int nVolume );
int ProcessAttachObj( void );
int ProcessDetachObj( void );
int ProcessSlavePos( float fX, float fY, float fZ );
int ProcessSlaveForce( float fX, float fY, float fZ );
int ProcessSlaveImpulse( float fX, float fY, float fZ );
int ProcessCheckDead( void );
int ProcessCollision( int nType );
int ProcessRotate( float fX, float fY, float fZ );
int ProcessLabel( char *pcString );
int ProcessWaitInterpDone( void );
int ProcessCollisionDelay( int nTicks );
int ProcessWaitForEnd( void );
int ProcessEndAnim( void );
int ProcessWaitForPause( void );
int ProcessWaitForFrame( int iFrame );
int ProcessPreInclude( char *pcString );
int ProcessPauseMotion( void );
int ProcessResumeMotion( void );


int ProcessIfLE( char *pcString );
int ProcessIfLT( char *pcString );
int ProcessIfNE( char *pcString );
int ProcessIfGE( char *pcString );
int ProcessIfGT( char *pcString );
int ProcessGetRandom( int max );
int ProcessGetFrame( void );
int ProcessGetAction( void );
int ProcessTestValue( int nVal );
int ProcessTestFlag( int nFlag );
int ProcessTestAction( int nAction );
int ProcessTestEndMotion( void );
int ProcessTestOnGround( void );
int ProcessTestRandom( float fThreshold );
int ProcessTestFrame( int frame );
int ProcessTestInterpDone( void );
void CreateEmptyLabelList( void );
void AppendLabelToLabelList( char *pcString, char *pcString2 );
int ProcessCase( char *pcString );
int ProcessRESOP0A( void );
int ProcessRESOP0B( void );
int ProcessRESOP1F( float fParm );
int ProcessRESOP1I( int iParm );

int ProcessSleep( float fVal );

int ProcessStartMotion2( int nMotionNumber, int nStartFrame, int nFlags, int nMotnInfIndex );
int ProcessKillMotion2( int nMotnInfIndex );
int ProcessTestEndMotion2( int nMotnInfIndex );
int ProcessTestFrame2( int iFrame, int nMotnInfIndex );
int ProcessWaitForEnd2( int nMotnInfIndex );
int ProcessWaitForFrame2( int iFrame, int nMotnInfIndex );
int ProcessInterpFrame2( int nMotionNumber, int nStartFrame, int nFlags, int nFrames, int nMotnInfIndex );
int ProcessWaitInterpDone2( int nMotnInfIndex );
int ProcessPauseMotion2( int nMotnInfIndex );
int ProcessResumeMotion2( int nMotnInfIndex );
int ProcessGetFrame2( int nMotnInfIndex );

int ProcessMin( void );
int ProcessMax( void );
int ProcessAbs( void );

int ProcessIntDecl( char *varName, int published );
int ProcessIntDeclInitial( char *varName, int initialValue, int published );
int ProcessFloatDecl( char *varName, int published );
int ProcessFloatDeclInitial( char *varName, float initialValue, int published );
int ProcessStringDecl( char *varName, int published );
int ProcessStringDeclInitial( char *varName, char *initialValue, int published );

int ProcessDeref( void );
int ProcessDerefInt( void );
int ProcessDerefFloat( void );

int ProcessPushCall( char *pcString );
int ProcessPopCall( void );
int ProcessLoadCall( char* varName );
int ProcessStoreCall( char* varName );

int ProcessSetAnimRate( float fRate );
int ProcessSetAnimRate2( float fRate );

int ProcessSetActionLo( int action );
int ProcessGetActionLo( void );
int ProcessSetActionHi( int action );
int ProcessGetActionHi( void );

int ProcessShiftLeft( void );
int ProcessShiftRight( void );
int ProcessShiftRightUnsigned( void );

int AddInt( char *pcRoutine, int nType, int nData );
int AddFloat( char *pcRoutine, float fData );
int AddAddress( char *pcRoutine, char *pcString );
int AddSymbol( char *pcRoutine, char *pcString, unsigned long ulValue );
int AddVar( char *pcRoutine, char *pcString, unsigned long ulValue );
int AddFunction( char *pcRoutine, char *pcString, unsigned long ulValue );
int AddString( char *pcRoutine, char *pcString );
int AddDW( char *pcRoutine, char *pcString );
void ExitWithError( void );

void AddToDataList( ts_Hdr *pCurData );
void AddToPatchList( ts_BackPatch *pCurPatch );
void AddToSymbolList( ts_Symbol *pCurSymbol );
void AddToVarList( ts_Symbol *pCurSymbol );
void AddToFunctionList( ts_Symbol *pCurSymbol );
void AddToStringList( ts_Symbol *pcCurSymbol );
void FreeMem( void );

int HandleIntParam( char *pParam );
float HandleFloatParam( char *pParam );
void HandleStringParam( char *pParam );
int HandleParameter( char *pcString );
#ifdef __cplusplus
}
#endif

#endif __ANIMRIP_H
