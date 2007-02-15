/* --------------------------------------------------
 * File    : ANIMRIP.C
 * Created : Wed Sep  2 11:05:48 1998
 * Descript: Animation script compiler
 * --------------------------------------------------*/
/* --------------------------------------------------
 * Includes
 * --------------------------------------------------*/
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <windows.h>
#include <process.h>
#include <dbwriter.h>
#include <fn_wrap.h>
#include "t_chunk.h"
#include <conio.h>
#include <direct.h>
#include <game/database/DBFile.h>

#include <assert.h>
#include <iostream.h>
#include <strstrea.h>

#include "anm_gram.h"
#include "animrip.h"
#include "EngineHelper/Hash.h"

extern "C" int yyparse(void);
extern "C" int zz_reset(void);
extern "C" ts_Hdr *FindDataByLineNumber( int lineNumber );
extern "C" ts_Symbol *FindVariableNameByIndex( int index );
extern "C" ts_Symbol *FindFunctionNameByIndex( int index );
extern "C" ts_Symbol *FindStringByIndex( int index );
extern "C" ts_Symbol *FindSymbolByAddress( int address );
extern "C" ts_Symbol *FindSymbolByLine( int nLine );
extern "C" int FindStringIndex( char *strValue );
extern "C" FILE *zzin;
extern "C" int iLineCount;
extern "C" int unprocessedFileLine;
extern "C" char unprocessedFileName[];
extern "C" char szFileName[];
extern "C" int zzlineno;

/* --------------------------------------------------
 * Local Defines
 * --------------------------------------------------*/
#define SCRIPT_OP_STACK_SIZE 32
#define SCRIPT_CALL_STACK_SIZE 16

 /* --------------------------------------------------
 * Local Prototypes
 * --------------------------------------------------*/

static int FixupOffsets( void );
static void FormatOutput( void );
static void usage( void );
static void OptimizeExpressions( void );
static char *CStringify( char *src );

extern "C" BOOL		bVerbose,
							bBigEndian,
							bPauseWhenDone,
							bPCOutput,
              bCPP,
              bDebug,
              bListing;

extern "C" int			nGlobalOffset;

extern "C" ts_Hdr		DataList,
							*pDataList;

extern "C" ts_BackPatch	PatchList,
							*pPatchList;

extern "C" ts_Symbol	SymbolList,
							*pSymbolList;

extern "C" ts_Symbol  FunctionList,
              *pFunctionList;

extern "C" ts_Symbol  VarList,
              *pVarList;

extern "C" ts_Symbol  StringList,
              *pStringList;

extern "C"	FILE 		*fInfile,
				 			*fOutfile;

extern "C"	unsigned long 	*pOutputArray;

extern "C"	char		*pOutputFileName = NULL;

extern "C" char *pOpStackSize;
extern "C" char *pCallStackSize;

extern "C" char **debugOffsetTable;

extern "C" char *debugText;

/* --------------------------------------------------
 * Local Data
 * --------------------------------------------------*/

const char *usage_text[] = {
"AnimRip -- Copyright (c) Avalanche Software 2000, 2001, 2002",
"ANIMRIP [options] <infile>",
"  Where [options] are:",
"    -I <path> --> Include path for #include statements",
"    -N64 --> Output data for N64 (defaults to PC)",
"    -V --> Bytecode Listing as <animscriptname.lst>",
"    -L --> Bytecode Listing as <animscriptname.lst>",
"    -W --> Wait for a keypress after processing",
"    -OS <size>",
"       -- overrides the default operand stack size (32)",
"    -CS <size>",
"       -- overrides the default call stack size (16)",
"    -D",
"       -- Debug mode - outputs a debug version script",
"    -F",
"       -- output file name - specifies the output file name",
"          if not specified, program uses the input file (script) file name"
"          output file extension always .ADB (ai) or .SDB (anim)",
"",
"  Animation Scripts are now run through the Boland C++ 5.5 preprocessor",
"   before being compiled by Animrip.exe.  *NOTE: This preprocessor seems",
"   to have trouble with lines longer that about 160 characters in length.",
"",
"  Animrip now supports expressions as parameters for the OPCODES.  These",
"   expressions can be the result of #define's being expanded, or directly",
"   written in the script.  In a non-debug version of the script, literal",
"   expressions are collapsed.  For example, the following line of script:",
"",
"      PUSH (1 + 2)",
"",
"   (in a debug script) results in:",
"",
"      PUSHI 1",
"      PUSHI 2",
"      ADD",
"",
"   (and in an optimized script) results in:",
"",
"      PUSHI 3",
"",
"   If a label occurs anywhere between the first parameter, and the",
"   associated math function call, this optimization does not occur.",
"   In a debug version of the script, the full expression is placed in",
"   the bytecode stream.",
"   Variables may also be freely used in expressions.  Expressions with",
"   variables can not be collapsed.  For example, the following is",
"   acceptable:",
"",
"      PUSH ( (actorPosX + actorPosY) / 2 )",
"",
"   debug and normal result:",
"",
"      LOAD actorPosX",
"      LOAD actorPosY",
"      ADD",
"      PUSHI 2",
"      DIV",
"",
"  Animrip has had an Operand Stack, a Call Stack, a Function Lookup Table,",
"   and a Variable Lookup Table added to it's structure.  These features are",
"   more fully detailed in the OPCODES section below.",
"",
"  Animrip's verbose (listing) mode has changed.  Now, a bytecode listing",
"   file is generated in this mode.  The file is named <animscriptname> with",
"   the extension changed to '.lst' from '.scr'.  This file has each line of",
"   the original script printed in a comment followed by a recompilable",
"   expansion of the line of text.  Following each expansion line, an offset",
"   showing where that OPCODE begins in the bytecode is shown as a comment.",
"   Immediately following the offset is the hex representation of the",
"   OPCODE.",
"",
"  Debugging support has also been added.  When a script is compiled with",
"   the '-D' flag, the script line number index that generated each OPCODE",
"   is placed in the bytecode stream immediately following the OPCODE, and",
"   before any parameters for the OPCODE.  A char* array is placed at the end",
"   of the resulting compiled script. This array uses the index following the",
"   OPCODE to reference into a text buffer at the end of the file which",
"   contains a null-terminated string for each line of the original script.",
"   This information is available to the running application for debug",
"   purposes.",
"",
"  VARIABLE DECLARATION:",
"",
"   Variables must be declared before they are used in the program.  The",
"   variables may be initialized at declaration with an initial value, also",
"   There are three known data-types: int, float, and string.  An example",
"   follows:",
"     int iVarName = 12;",
"     float fVarName = 15.4f;",
"     string sVarName = \"Here's a string constant\n\";",
"",
"  OPCODES:",
"   In this section, while referring to the operand stack, stack[0]",
"   refers to the top of the operand stack, while stack[1] refers to",
"   the value immediately under the top value, and so forth.",
"",
"   *Note: the math OPCODES do not work on string constants.  No conversion",
"      is attempted to make strings into float or int, or back again.",
"",
"   PUSH <literal>",
"      -- Push <literal> onto the top of the operand stack.  This operation",
"         records the type (int, float, or C-String constant) of the",
"         operand.",
"   POP",
"      -- Pop top value from the operand stack.",
"   PUSHREG",
"      -- Push a copy of the value of the internal (old opcode) status register",
"   POPREG",
"      -- Pop the top operand, and store the value into the internal register",
"   SWAP [<index>]",
"      -- SWAPs the stack[0] value with the stack[<index>] value.  <index>",
"         defaults to 1.",
"   DUP [<index>]",
"      -- DUPlicates the stack[<index>] value onto the top of the stack.",
"   LOAD <variableName>",
"      -- PUSHes the value of <variableName> onto the operand stack.",
"   STORE <destVarName> [<srcVarName>|<literal>]",
"      -- POPs the operand stack, storing the value in <destVarName>.  If",
"         an alternate source is included, that source is first PUSHed onto",
"         the stack before being STOREd.",
"   DEREF_INT",
"      -- Interprets the top value on the stack as an int*.  POPs the pointer",
"         and PUSHes the dereferenced value.",
"   DEREF_FLOAT",
"      -- As DEREF_INT, but treats the pointer as a float*.",
"",
"   MATH OPCODES:",
"   ADD",
"      -- POPs the top two values from the operand stack, performs:",
"         (stack[1] + stack[0]), and PUSHes the result.  If both operands",
"         are int, then the result is an int, otherwise, the non-float values",
"         are converted to floats, and the result is a float.",
"   SUB",
"      -- As ADD, but performs:",
"         stack[1] - stack[0], and PUSHes the result.",
"   MUL",
"      -- As ADD, but performs:",
"         stack[1] * stack[0], and PUSHes the result.",
"   DIV",
"      -- As ADD, but performs:",
"         stack[1] / stack[0], and PUSHes the result.",
"   MOD",
"      -- As ADD, but performs: (converting to integers as necessary)",
"         stack[1] % stack[0], and PUSHes the result.",
"   MIN",
"      -- Pops the top 2 operands, and Pushes the smallest value.",
"   MAX",
"      -- Pops the top 2 operands, and Pushes the largest value.",
"   ABS",
"      -- Pops the top operand, and Pushes it's absolute value.",
"   BIT_AND",
"      -- Pops the top 2 operands, treats the bits as unsigned int, performs",
"         the bit-wise AND operation (stack[1] & stack[0]) on them,",
"         pushing the result as an int.",
"   BIT_OR",
"      -- As BIT_AND, except performs a bit-wise OR operation.",
"   BIT_XOR",
"      -- As BIT_AND, except performs a bit-wise XOR operation.",
"   BIT_NOT",
"      -- As BIT_AND, except only POPs stack[0] and performs a bit-wise",
"         NOT operation, PUSHing the result.",
"   AND",
"      -- Pops the top 2 operands, and performs the logical AND operation:",
"         (stack[1] AND stack[0]).  Any value other than 0 is true,",
"         and the result is an int 1 for true, and int 0 for false.",
"   OR",
"      -- Performs the same as AND, except a logical OR operation is",
"         performed.",
"   NOT",
"      -- Performs the same as AND, except that only stack[0] is POPed, and",
"         a logical NOT operation is performed, PUSHing the result.",
"   INT",
"      -- Converts the top value on the operand stack from float to int.",
"   FLOAT",
"      -- Converts the top value on the operand stack from int to float.",
"   INCR",
"      -- Increments the top value on the stack by 1.",
"   DECR",
"      -- Decrements the top value on the stack by 1.",
"   SHL",
"      -- Shifts left: stack[1] << stack[0].  Use '<<' for this operation",
"         in an expression.",
"   SHR",
"      -- Shifts right with sign: stack[1] >> stack[0].  Use '>>' for this",
"         operation in an expression.",
"   SHRZ",
"      -- Shifts right without sign: stack[1] >> stack[0].  Use '>>>' for",
"         this operation in an expression.",
"",
"   FLOW CONTROL OPCODES:",
"",
"   CMP",
"      -- POPs the top two values from the operand stack, and compares the",
"         values.  If (stack[1] > stack[0]), CMP pushes a 1 (int) onto",
"         the stack, 0 (int) if (stack[1] == stack[0]), and -1 (int) if",
"         (stack[1] < stack[0]).",
"   JNE <label>",
"      -- POPs the stack, and JUMPs to <label> if CMP pushed a non 0 result.",
"   JE <label>",
"      -- As JNE, but checks for an equal (0) CMP result.",
"   JL <label>",
"      -- As JNE, but checks for a less-than (-1) CMP result.",
"   JG <label>",
"      -- As JNE, but checks for a greater-than (1) CMP result.",
"   JLE <label>",
"      -- As JNE, but checks for a less-than or equal-to !(1) CMP result.",
"   JGE <label>",
"      -- As JNE, but checks for a greater-than or equal-to !(-1) CMP result.",
"   BNE <label>",
"      -- POPs the stack, and CALLs to <label> if CMP pushed a non 0 result.",
"   BE <label>",
"      -- As BNE, but checks for an equal (0) CMP result.",
"   BL <label>",
"      -- As BNE, but checks for a less-than (-1) CMP result.",
"   BG <label>",
"      -- As BNE, but checks for a greater-than (1) CMP result.",
"   BLE <label>",
"      -- As BNE, but checks for a less-than or equal-to !(1) CMP result.",
"   BGE <label>",
"      -- As BNE, but checks for a greater-than or equal-to !(-1) CMP result.",
"   CALLC <functionName>",
"      -- Calls the \"C\" function registered with the VM passing a pointer",
"         to the VM's state machine as the only parameter.",
"   CALL <label>",
"      -- Jumps to the indicated label in the script - pushing it's current",
"         PC onto the call stack.",
"   RETURN",
"      -- Jumps to the PC most recently pushed onto the call stack with",
"         CALL command, popping the call stack in the process.",
"   PUSH_CALL <label_name>",
"      -- Pushes the address of <label_name> onto the top of the call",
"         stack.",
"   POP_CALL",
"      -- Pops the top address off the call stack.",
"",
"   ANIMATION OPCODES:",
"",
"   START_MOTION <nMotionNumber> <iFrame> <iFlags>",
"      -- Start motion <nMotionNumber> at frame <iFrame> using",
"         motion flags <iFlags>",
"   KILL_MOTION",
"      -- Kill the motion for this animation",
"   KEY_FRAME <iFrame>",
"      -- Display frame <iFrame> of the current motion",
"   INTERP_FRAME (nFrameOffset, fPercent)",
"      -- Interpolate between the current frame of animation and the",
"         specified frame.  The interpolation will calculate the new frame",
"         fPercent away from the current frame.",
"   DELAY (nFrames)",
"      -- Delay animation script processing for nFrames number of ticks.",
"   END_ANIM",
"      -- Stop Anim script processing.",
"   FORCE <fX> <fY> <fZ>",
"      -- Apply this force vector to the actor.",
"   IMPULSE (fX, fY, fZ)",
"      -- Directly apply this velocity vector to the actor.",
"   IMPULSE_X (fX)",
"      -- Directly apply this velocity vector to the actor.",
"   IMPULSE_Y (fY)",
"      -- Directly apply this velocity vector to the actor.",
"   IMPULSE_Z (fZ)",
"      -- Directly apply this velocity vector to the actor.",
"   OFFSET (fX, fY, fZ)",
"      -- DEPRICATED - do not use",
"   JUMP <AnimLabel>",
"      -- Jump to this offset to perform the next animation script command.",
"         The offset will be calculated by the script compiler.",
"   NEW_ANIM <nAnimNumber>",
"      -- Jump to this animation number and begin processing that script.",
"         This is different from the JUMP command in that the actor's",
"         animation number will change (for ai purposes).",
"   CODE (nFunctionNumber)",
"      -- Call function table entry <nFunctionNumber>.",
"   IF_SET (nAnimLabel)",
"      -- If nAnimVar is true, then jump to the label.",
"   IF_NOT_SET (nAnimLabel)",
"      -- If nAnimVar is false, then jump to the label.",
"   IF_EQ   (nValue, nAnimLabel)",
"      -- If nAnimVar is equal to nValue, then jump to the label.",
"   IF_LE <animLabel>",
"      -- if VM's register is <= 0, jump to <animLabel>.",
"   IF_LT <animLabel>",
"      -- if VM's register is < 0, jump to <animLabel>.",
"   IF_NE <animLabel>",
"      -- if VM's register is != 0, jump to <animLabel>.",
"   IF_GE <animLabel>",
"      -- if VM's register is >= 0, jump to <animLabel>.",
"   IF_GT <animLabel>",
"      -- if VM's register is > 0, jump to <animLabel>.",
"   ACTION (nAction)",
"      -- Set the actor's action to this value.  SET_ACTION is a macro",
"         for this opcode.",
"   WAIT_HIT_GROUND",
"      -- Do not go past this command until the actor has landed on the",
"         ground",
"   WAIT_TIL_HOLD",
"      -- Do not go past this command until the actor is at a hold in his",
"         motion process.",
"   WAIT_INTERP_DONE",
"      -- Do not go past this command until the actor has completed any",
"         scripted interpolation commands.",
"   WAIT_FOR_FRAME <iFrame>",
"      -- Do not go past this command until the actor is at frame <iFrame>",
"   PLAY_RANDOM_SOUND <iExpr> <iExpr> <iExpr>",
"      -- NOT IMPLEMENTED",
"   PLAY_SOUND (iSound)",
"      -- NOT IMPLEMENTED",
"         Play the sound iSound.",
"   PLAY_SOUND_VOLUME (iSound, iVolume)",
"      -- NOT IMPLEMENTED",
"         Play the sound iSound at volume iVolume.",
"   COLLISION_DELAY (nFrames)",
"      -- NOT IMPLEMENTED",
"         Process collisions during the specified number of frames.",
"   SET_FLAG (nBitMask)",
"      -- Bitwise OR this value into the actor's flags field",
"   CLR_FLAG   (nBitMask)",
"      -- Bitwise AND the NEGATED value with the actor's flags field",
"   CREATE_PROC   (nProcNumber, nProcID)",
"      -- Get the address of the process from the Process Table and create it",
"         using ProcID as the process ID",
"   KILL_PROC (nProcID)",
"      -- Kill all instances of nProcID",
"   SOUND   (nSequence)",
"      -- Trigger nSequence to play",
"   ATTACH_OBJ",
"   DETACH_OBJ",
"   SLAVE_POS (fX, fY, fZ)",
"   SLAVE_FORCE (fX, fY, fZ)",
"   SLAVE_IMPULSE (fX, fY, fZ)",
"   CHECK_DEAD",
"      -- Check to see if this object is dead, if so, run the DIE script",
"   COLLISION (nType)",
"      -- Check for an offensive collision of nType",
"   GET_ACTION",
"      -- Loads the current action of the script into the VM's register.",
"         Use PUSH_REG/POP_REG to manipulate the VM's register value.",
"   GET_FRAME",
"      -- Loads the current animation frame into the VM's register.",
"   GET_RANDOM <iExpr>",
"      -- Loads a random value from [0..<iExpr>] into the VM's register.",
"   SET_ANIM_RATE <fExpr>",
"      -- Sets the animation rate for primary channel.",
"   SET_ANIM_RATE2 <fExpr>",
"      -- Sets the animation rate for the secondary channel.",
"   SET_ACTION_LO <iExpr>",
"      -- Sets the low word of the action with the low bytes of <iExpr>.",
"   GET_ACTION_LO",
"      -- Pushes the low word of the action onto the operand stack.",
"   SET_ACTION_HI <iExpr>",
"      -- Sets the high word of the action with the high bytes of <iExpr>.",
"   GET_ACTION_HI",
"      -- Pushes the high word of the action onto the operand stack.",
"",
"",
"",
"   The following is a description of the C-API that has been added to",
"    Animproc.c/.h:",
"",
"typedef void (*fpAnimScriptCall)( void* pActor );",
"",
"fpAnimScriptCall vmScriptRegister( void *vm, fpAnimScriptCall cCall, const char *pFuncName );",
"   This function registers a C-callback function that the VM should call in",
"   response to a CALLC OPCODE.",
"",
"int vmScriptPeekType( void *vm );",
"   This function returns the type of the value on the top of the stack:",
"      INT_TYPE, FLOAT_TYPE, STRING_TYPE, or DYNAMIC_STRING_TYPE.",
"",
"void vmScriptPush( void *vm, int iValue );",
"   Push a float value onto the operand stack.",
"",
"void vmScriptPush( void *vm, float fValue );",
"   Push an int value onto the operand stack.",
"",
"void vmScriptPush( void *vm, char *sValue, bool bDeleteOnPop = false );",
"   Push a string value onto the operand stack.  bDeleteOnPop tells the VM if",
"it",
"   should delete the character array when it is popped from the opstack by a",
"   call to the POP OPCODE.",
"",
"int vmScriptPopInt( void *vm );",
"   Pop an integer from the opstack, and return it.",
"",
"float vmScriptPopFloat( void *vm );",
"   Pop a float from the opstack, and return it.",
"",
"char *vmScriptPopString( void *vm, bool &bDeleteWhenDone );",
"   Pop a char* from the opstack, and return it.  bDeleteWhenDone tells the",
"   application if it is now responsible for deleting the character array",
"   when it is done processing it, or not.",
"",
"void vmScriptSwap( void *vm, int iIndex = 1 );",
"   Swap the top stack value with iIndex stack value.",
"",
"void vmScriptDup( void *vm, int iIndex = 0 );",
"   Push a copy of stack value at iIndex.",
"",
"void vmScriptStore( void *vm, const char *pVarName );",
"   STORE the stack[0] to variable named pVarName.  ASSERTs if pVarName",
"wasn't",
"   declared in the script - the C-API can't create new variable names at run",
"   time.",
"",
"void vmScriptLoad( void *vm, const char *pVarName );",
"   LOAD from the variable named pVarName to stack[0].  ASSERTs if pVarName",
"   wasn't declared in the script - the C-API can't create new variable names",
"   at run time.",
"",
NULL,
};

BOOL bChangedByteCodes = FALSE;

const char *opCodeNames[ CMD_OP_CODE_COUNT ] = {
  "START_MOTION",
  "KILL_MOTION",
  "KEY_FRAME",
  "INTERP_FRAME",
  "DELAY",
  "END_ANIM",
  "FORCE",
  "IMPULSE",
  "JUMP",
  "NEW_ANIM",
  "CODE",
  "IF_SET",
  "IF_NOT_SET",
  "IF_EQ",
  "ACTION",
  "WAIT_HIT_GROUND",
  "WAIT_TIL_HOLD",
  "WAIT_FOR_PAUSE",
  "WAIT_FOR_FRAME",
  "WAIT_FOR_END",
  "SET_FLAG",
  "CLR_FLAG",
  "CREATE_PROC",
  "KILL_PROC",
  "SOUND",
  "ATTACH_OBJ",
  "DETACH_OBJ",
  "SLAVE_POS",
  "SLAVE_FORCE",
  "SLAVE_IMPULSE",
  "CHECK_DEAD",
  "COLLISION",
  "ROTATE",
  "IMPULSE_X",
  "IMPULSE_Y",
  "IMPULSE_Z",
  "OFFSET",
  "CODE_1F",
  "CODE_2F",
  "CODE_3F",
  "WAIT_INTERP_DONE",
  "PLAY_RANDOM_SOUND",
  "PLAY_SOUND",
  "PLAY_SOUND_VOLUME",
  "COLLISION_DELAY",
  "PAUSE_MOTION",
  "RESUME_MOTION",
  "CODE_1I",
  "IF_LE",
  "IF_LT",
  "IF_NE",
  "IF_GE",
  "IF_GT",
  "GET_RANDOM",
  "GET_FRAME",
  "GET_ACTION",
  "TEST_VALUE",
  "TEST_FLAG",
  "TEST_ACTION",
  "TEST_ENDMOTION",
  "TEST_RANDOM",
  "TEST_FRAME",
  "TEST_INTERPDONE",
  "CASE",
  "RES_OP0A",
  "RES_OP0B",
  "RES_OP1F",
  "RES_OP1I",
  "TEST_ONGROUND",
  "PUSHI",
  "PUSHF",
  "POP",
  "SWAP",
  "DUP",
  "LOAD",
  "STORE",
  "ADD",
  "SUB",
  "MUL",
  "DIV",
  "MOD",
  "BIT_AND",
  "BIT_OR",
  "BIT_XOR",
  "BIT_NOT",
  "AND",
  "OR",
  "NOT",
  "INT",
  "FLOAT",
  "CMP",
  "JNE",
  "JE",
  "JL",
  "JG",
  "JLE",
  "JGE",
  "CALLC",
  "CALL",
  "RETURN",
  "PUSHS",
  "SLEEP",
  "START_MOTION2",
  "KILL_MOTION2",
  "TEST_ENDMOTION2",
  "TEST_FRAME2",
  "WAIT_FOR_END2",
  "WAIT_FOR_FRAME2",
  "INTERP_FRAME2",
  "WAIT_INTERP_DONE2",
  "PAUSE_MOTION2",
  "RESUME_MOTION2",
  "GET_FRAME2",
  "MIN",
  "MAX",
  "ABS",
  "BNE",
  "BE",
  "BL",
  "BG",
  "BLE",
  "BGE",
  "PUSHREG",
  "POPREG",
  "DEREF",
  "DEREF_INT",
  "DEREF_FLOAT",
  "PUSH_CALL",
  "POP_CALL",
  "LOAD_CALL",
  "STORE_CALL",
  "SET_ANIM_RATE",
  "SET_ANIM_RATE2",
  "SET_ACTION_LO",
  "GET_ACTION_LO",
  "SET_ACTION_HI",
  "GET_ACTION_HI",
  "SHL",
  "SHR",
  "SHRZ",
  "PLAY_SOUND_STR",
  "PLAY_SOUND_VOLUME_STR",
};

char ListingFile[_MAX_PATH];
FILE *fListingFile = NULL;

/* --------------------------------------------------
 * Global Data
 * --------------------------------------------------*/

extern "C" int errorCount;
int nExportType = EXPORT_ANIM_SCRIPT;

/* --------------------------------------------------
 * Exported Functions
 * --------------------------------------------------*/

/* --------------------------------------------------
 * Local Functions
 * --------------------------------------------------*/
static void CreateDataFile( char *pDataName, char *sourceFile );
static void PrintListing( void );

/* --------------------------------------------------
 * MAIN
 * --------------------------------------------------*/
extern "C" int zzdebug;

/* this is now the only chunk key used, no longer allowed to be overwritten by parameter. it used to be, a long
time ago, that we would have multiple compiled script files in a merged .dbl file, and we needed to be able to
tell them apart, so each type of actor had its own key. but no more-- we tell script files apart by file name. */

char *pKey = "1000";

int main( int argc, char *argv[])
{
  char  drive[_MAX_DRIVE],
			dir[_MAX_DIR],
			file[_MAX_FNAME],
			hfile[_MAX_FNAME],
			ext[_MAX_EXT],
			path[_MAX_PATH];

	if (argc < 2)
	{
		usage ();
		return 1;
	}

	bVerbose = FALSE;
	bBigEndian = FALSE;
  bCPP = TRUE;
  bDebug = FALSE;

	/* we are using the visual c compiler as a pre-processor */

	char PreProcessorCommand[_MAX_PATH];
	_splitpath(argv[0], drive, dir, file, ext);
	_makepath(PreProcessorCommand, drive, dir, "_cl", "exe");

	/* can handle multiple include paths */

#define MAX_INCLUDE_PATHS	10
	int nIncludePathCount = 0;
	char *pIncludePath[MAX_INCLUDE_PATHS];

  /* parse the command line */
	while (argv[1][0] == '-')
	{
		switch (argv[1][1])
		{

			case 'I':
			case 'i':
				assert(nIncludePathCount < MAX_INCLUDE_PATHS);
				if (nIncludePathCount < MAX_INCLUDE_PATHS)
					pIncludePath[nIncludePathCount++] = argv[2];
				argv += 2;
				break;

			case 'F':
			case 'f':
				pOutputFileName = argv[2];
				argv += 2;
				break;

// see comment at declaration of pKey, above
//			case 'K':
//			case 'k':
//				pKey = argv[2];
//				argv += 2;
//				break;

      case 'S':
      case 's':
        argv++;
        break;

			case 'N':
			case 'n':
				bBigEndian = TRUE;
				argv++;
				break;

			case 'V':
			case 'v':
				bVerbose = TRUE;
        bListing = TRUE;
				argv++;
				break;

			case 'W':
			case 'w':
				bPauseWhenDone = TRUE;
				argv++;
				break;

      case 'C':
      case 'c':
        switch ( argv[1][2] )
        {
        case 'S':
        case 's':
          pCallStackSize = argv[2];
          argv += 2;
          break;
        default:
          /* don't do c preprocessing */
          bCPP = FALSE;
          argv++;
          break;
        }
      case 'D':
      case 'd':
        bDebug = TRUE;
        argv++;
        break;

      case 'L':
      case 'l':
        bListing = TRUE;
        argv++;
        break;
        
      case 'O':
      case 'o':
        switch ( argv[1][2] )
        {
        case 'S':
        case 's':
          pOpStackSize = argv[2];
          argv += 2;
          break;
        default:
          break;
        }
        break;
      default:
				argv++;
				break;
		}
	}

	/* always output pc data! default is old PSX data, with fixed point values for floats! */

	bPCOutput = TRUE;
	
	pDataList = &DataList;
	pPatchList = &PatchList;
	pSymbolList = &SymbolList;
  pFunctionList = &FunctionList;
  pVarList = &VarList;
  pStringList = &StringList;

	DataList.pNext = NULL;
	DataList.nType = 0;
	PatchList.pNext = NULL;
	PatchList.pData = NULL;
	SymbolList.pNext = NULL;
	SymbolList.ulValue = 0;
  FunctionList.pNext = NULL;
  FunctionList.ulValue = 0;
  VarList.pNext = NULL;
  VarList.ulValue = 0;

  // preprocess the file
  char sourceFile[_MAX_PATH];
  char scriptFile[_MAX_PATH];
  int retval = 0;

  strcpy( scriptFile, argv[1] );

	if (bCPP == TRUE)	// -[C|c] turns OFF C preprocessing
	{
		/* build up argument string-- may be variable number of include paths */

		char OptionString[10000];	// sure, it's ridiculous
		strcpy(OptionString, " /nologo /P /E ");
		for (int i = 0; i < nIncludePathCount; i++)
		{
			char IncludeString[1000];
			sprintf(IncludeString, "/I%s ", pIncludePath[i]);
			strcat(OptionString, IncludeString);
		}

		/* call the pre-processor */

		retval = _spawnl( _P_WAIT, PreProcessorCommand, OptionString, scriptFile, NULL );
		if ( retval != 0 )
		{
			printf( "C-Preprocessor not found, or failed\n" );
			ExitWithError( );
		}

    /* VC++'s preprocessor puts the output file in the current working directory - not where the processed file lives */
		/* source file is now a ".i" file produced by the pre-processor */

    /* get the current working directory */
    char cwd[_MAX_PATH];
    _getcwd( cwd, _MAX_PATH );
    strcat( cwd, "\\" );

    /* get the filename */
		_splitpath( scriptFile, drive, dir, file, ext );

    /* get the rest of the path */
    _splitpath( cwd, drive, dir, NULL, NULL );

    /* conglom the two pieces together */
		_makepath( sourceFile, drive, dir, file, ".i" );
	}
	else
	{
		/* no preprocessor, use the source file as is */

		strcpy( sourceFile, scriptFile );
	}

  _splitpath( scriptFile, drive ,dir ,file ,ext );
  _makepath( ListingFile, drive, dir, file, "lst" );

	// Open infile
	fInfile = fopen( sourceFile, "rt" );
	strcpy( szFileName, sourceFile );
	strcpy( unprocessedFileName, sourceFile );
	if (fInfile == NULL)
	{
		printf ("Command file %s not found.\n", sourceFile );
		usage ( );
		return 1;
	}
	zzin = fInfile;

	_splitpath( scriptFile, drive ,dir ,file ,ext );

	//open .h file to output action values to
	strcpy ( ext, ".h" );
	strcpy ( hfile, file );
	strcat ( hfile, "_S" );
	_makepath ( path, drive, dir, hfile, ext );

//	zzdebug = 1;
	//ProcessIncludes( 0 );

	// Parse the input file
	//zz_reset( );
	yyparse( );

	if(errorCount)
		ExitWithError( );

 	// Run through patch list and fix-up offsets and #defines
	if ( FixupOffsets( ) == FALSE )
	{
		ExitWithError( );
	}

	// Open outfile
	switch(nExportType)
	{
	default:
		assert(FALSE);
		/* and fall through... */
	case EXPORT_ANIM_SCRIPT:
		strcpy ( ext, ".sdb" );
		break;
	case EXPORT_AI_SCRIPT:
		strcpy ( ext, ".adb" );
		break;
	}

	/* may be specifying an output file name */

	if (pOutputFileName)
		_splitpath(pOutputFileName, drive, dir, file, ext);

	/* build up the output file name */

	_makepath ( path, drive, dir, file, ext );
	CreateDataFile( path, sourceFile );

  // Print a listing of the resulting opcodes
  if ( bListing == TRUE )
  {
    PrintListing( );
  }

	/* list variables names and hash values */

	printf( "\nVariable list:\n\n" );
	int nIndex = 0;
	pVarList = &VarList;
	while ( pVarList->pNext != NULL )
	{
		pVarList = pVarList->pNext;
		printf("%-2d\t0x%08x\t%s\n", nIndex, pVarList->ulValue, pVarList->caName);
		nIndex++;
	}
	printf("\n");

	/* list routine names and hash values */

	printf("\nFunction list:\n\n");
	nIndex = 0;
	pFunctionList = &FunctionList;	// this one is just the head of the linked list, not a valid entry in list
	while (pFunctionList->pNext != NULL)
	{
		pFunctionList = pFunctionList->pNext;
		printf("%d\t0x%08x\t%s\n", nIndex, pFunctionList->ulValue, pFunctionList->caName);
		nIndex++;
	}
	printf("\n");

	// Clean up and exit
	FreeMem( );
	fclose( fInfile );

	/* success */

	printf("Successfully compiled script %s into output file %s, key = %s\n", argv[1], path, pKey);

	if( bPauseWhenDone )
		getch( );
	return(0);
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
			fprintf( stderr, "%s(0) : ERROR - Symbol not found '%s'\n", szFileName, pPatchList->caName );
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

#define MAKE_DWORD(a) { a = a & 0x03 ? ( a / 4 ) + 1 : ( a / 4 ); }

//-----------------------------------------------------------------------------
// CreateDataFile
//-----------------------------------------------------------------------------

#define SCRIPT_VERSION_2_MAGIC 0xFEEDBEEF

struct ts_funcTableRec
{
  unsigned int    funcNameHash;
  unsigned int    funcNameIndex;
};

struct ts_varTableRec
{
  unsigned int    varNameHash;
  unsigned int    varInitialized;
  unsigned int    varNameIndex;
  float           varInitialValue;
  unsigned int    varDataType;
};

struct ts_Script
{
  unsigned int    Magic;
  unsigned int    OperandStackCount;
  unsigned int    CallStackCount;
  unsigned int    ByteCodesByteCount;
  unsigned int    FuncTableCount;
  unsigned int    VarTableCount;
  unsigned int    StringTableCount;
  unsigned int    DebugLineCount;
  unsigned int    ByteCodesOffset;
  unsigned int    FuncTableHashOffset;
  unsigned int    VarTableHashOffset;
  unsigned int    StringTableOffset;
  unsigned int    StringTextOffset;
  unsigned int    DebugInfoOffset;
  unsigned int    DebugDataOffset;
};

void CreateDataFile( char *pDataName, char *sourceFile )
{
  int           funcTableCount, varTableCount, dataTableCount, stringTableCount, stringTableSize;
  ts_Symbol     *curr = NULL;
	DBWriter      *pDataWriter = NULL;
	FNWrapper     DataWrapper;
  ifstream      *source = NULL;
  int           iFileSize = 0;
  char          linebuff[4096];
  int           linecount = 0;
  char          **pStringTable;
  char          *pStringTableText;
  char          *currbuff = NULL;


  /* BEGIN prepare debug informaion */
  source = new ifstream( );
  source->open( sourceFile, ios::in );

  /* get file size */
  source->seekg(0,ios::end);
  iFileSize = source->tellg( );
  source->seekg(0,ios::beg);
  source->unsetf( ios::skipws );

  if ( iFileSize == 0 )
  {
    return;
  }

  debugOffsetTable = ( char ** )malloc( sizeof( char * ) * iLineCount );

  if ( ( iFileSize & 0x03 ) != 0 )
  {
    iFileSize += 4 - ( iFileSize & 0x03 );
  }

  debugText = ( char * )malloc( iFileSize );
  currbuff = debugText;

  memset( debugText, 0, iFileSize );

  /* create an offset table, and a text buffer */
  int currline = 0;
  iLineCount = 0;
  while ( !source->eof( ) )
  {
    iLineCount++;
    source->getline( linebuff, 4096 );
    strcpy( currbuff, linebuff );
    debugOffsetTable[ currline++ ] = ( char * )( currbuff - debugText );
    currbuff += strlen( linebuff ) + 1;
  }

  source->close( );
  delete source;
  /* END prepare debug informaion */

  // create the data writer
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

	if ( pDataWriter->stream_open() == false )
	{
		// windows crap to get a decent error message.
		LPVOID lpMsgBuf;
		FormatMessage( 
			 FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			 FORMAT_MESSAGE_FROM_SYSTEM | 
			 FORMAT_MESSAGE_IGNORE_INSERTS,
			 NULL,
			 GetLastError(),
			 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			 (LPTSTR) &lpMsgBuf,
			 0,
			 NULL 
		);
		fprintf( stderr, "ERROR: Can't open '%s' for write: %s\n", (char*)DataWrapper, lpMsgBuf );
		LocalFree( lpMsgBuf );

		ExitWithError( );
	}

	pDataWriter->DBStart( 0 );
	switch(nExportType)
	{
	default:
		assert(FALSE);
		/* and fall through... */
	case EXPORT_ANIM_SCRIPT:
		pDataWriter->ChunkStart( DB_ANIMSCRIPT, 0, DB_ANIMSCRIPT_VERSION, pKey );
		break;
	case EXPORT_AI_SCRIPT:
		pDataWriter->ChunkStart( DB_AISCRIPT, 0, DB_AISCRIPT_VERSION, pKey );
		break;
	}

  funcTableCount = 0;
  pFunctionList = &FunctionList;
  while( pFunctionList->pNext != NULL )
  {
    pFunctionList = pFunctionList->pNext;
    funcTableCount++;
  }

  varTableCount = 0;
  pVarList = &VarList;
  while( pVarList->pNext != NULL )
  {
    pVarList = pVarList->pNext;
    varTableCount++;
  }

  dataTableCount = 0;
  pDataList = &DataList;
  while( pDataList->pNext != NULL )
  {
    pDataList = pDataList->pNext;
    dataTableCount++;
  }

  stringTableCount = 0;
  stringTableSize = 0;
  pStringList = &StringList;
  while( pStringList->pNext != NULL )
  {
    pStringList = pStringList->pNext;
    stringTableSize += strlen( pStringList->caName ) + 1;
    stringTableCount++;
  }
  
  // make sure the buffer for the string constants is 4-byte even
  if ( ( stringTableSize & 0x03 ) != 0 )
  {
    stringTableSize += 4 - ( stringTableSize & 0x03 );
  }

  pStringTableText = ( char * )malloc( stringTableSize );
  pStringTable = ( char ** )malloc( sizeof ( char * ) * stringTableCount );
  currbuff = pStringTableText;
  memset( pStringTableText, 0, stringTableSize );

  /* create an offset table, and a text buffer */
  pStringList = &StringList;
  currline = 0;
  while( pStringList->pNext != NULL )
  {
    pStringList = pStringList->pNext;
    strcpy( currbuff, pStringList->caName );
    pStringTable[ currline++ ] = ( char * )( currbuff - pStringTableText );
    currbuff += strlen( pStringList->caName ) + 1;
  }




  ts_Script script;

  script.Magic = SCRIPT_VERSION_2_MAGIC;
  if ( pOpStackSize != NULL )
  {
    script.OperandStackCount = atoi( pOpStackSize );
  }
  else
  {
    script.OperandStackCount = SCRIPT_OP_STACK_SIZE;
  }
  if ( pCallStackSize != NULL )
  {
    script.CallStackCount = atoi( pCallStackSize );
  }
  else
  {
    script.CallStackCount = SCRIPT_CALL_STACK_SIZE;
  }
  script.ByteCodesByteCount = dataTableCount * sizeof( unsigned int );
  script.FuncTableCount = funcTableCount;
  script.VarTableCount = varTableCount;
  script.StringTableCount = stringTableCount;
  script.DebugLineCount = iLineCount;
  script.ByteCodesOffset = sizeof( ts_Script );

  script.FuncTableHashOffset = ( script.ByteCodesOffset + script.ByteCodesByteCount );
  script.VarTableHashOffset = ( ( funcTableCount * sizeof( ts_funcTableRec ) ) + script.FuncTableHashOffset );
  script.StringTableOffset = ( ( varTableCount * sizeof( ts_varTableRec ) ) + script.VarTableHashOffset );
  script.StringTextOffset = ( ( stringTableCount * sizeof( char * ) ) + script.StringTableOffset );;

  if ( bDebug )
  {
    script.DebugInfoOffset = ( stringTableSize + script.StringTextOffset );
    script.DebugDataOffset = ( ( iLineCount * sizeof( unsigned int ) ) + script.DebugInfoOffset );
  }
  else
  {
    script.DebugInfoOffset = 0;
    script.DebugDataOffset = 0;
  }

  /* reset these values back to 0 if there's no func/vars */
  if ( funcTableCount == 0 )
    script.FuncTableHashOffset = 0;
  if ( varTableCount == 0 )
    script.VarTableHashOffset = 0;

  /* begin writing values */
  pDataWriter->stream() << script.Magic;
  pDataWriter->stream() << script.OperandStackCount;
  pDataWriter->stream() << script.CallStackCount;
  pDataWriter->stream() << script.ByteCodesByteCount;
  pDataWriter->stream() << script.FuncTableCount;
  pDataWriter->stream() << script.VarTableCount;
  pDataWriter->stream() << script.StringTableCount;
  pDataWriter->stream() << script.DebugLineCount;
  pDataWriter->stream() << script.ByteCodesOffset;
  pDataWriter->stream() << script.FuncTableHashOffset;
  pDataWriter->stream() << script.VarTableHashOffset;
  pDataWriter->stream() << script.StringTableOffset;
  pDataWriter->stream() << script.StringTextOffset;
  pDataWriter->stream() << script.DebugInfoOffset;
  pDataWriter->stream() << script.DebugDataOffset;

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

  /* write out the function name hash values in order */
  pFunctionList = &FunctionList;
  while( pFunctionList->pNext != NULL )
  {
    pFunctionList = pFunctionList->pNext;
    pDataWriter->stream() << pFunctionList->ulValue;
    pDataWriter->stream() << pFunctionList->iNameIndex;
  }

  /* write out the variable name hash values in order */
  pVarList = &VarList;
  while( pVarList->pNext != NULL )
  {
    pVarList = pVarList->pNext;
    pDataWriter->stream() << pVarList->ulValue;
    pDataWriter->stream() << pVarList->bInitialized;
    pDataWriter->stream() << pVarList->iNameIndex;
    pDataWriter->stream() << pVarList->iInitialValue;
    pDataWriter->stream() << pVarList->uVarType;
  }

  /* write the string table, and the string buffer */
  for( int currString = 0; currString < stringTableCount; currString++ )
  {
    pDataWriter->stream() << ( unsigned int )pStringTable[ currString ];
  }
  pDataWriter->stream().Write( ( unsigned char * )pStringTableText, stringTableSize );

  if ( bDebug )
  {
    /* write the offset table */
    for ( int i = 0; i < iLineCount; i++ )
    {
      pDataWriter->stream() << ( unsigned int )debugOffsetTable[ i ];
    }
    
    /* write the text buffer */
    pDataWriter->stream( ).Write( ( unsigned char * )debugText, iFileSize );
  }

  pDataWriter->ChunkEnd( );
	pDataWriter->DBEnd( );
}

/* --------------------------------------------------
 * 	usage
 * --------------------------------------------------*/
void usage (void)
{
  char *curr_line = ( char * )usage_text[0];
  int i = 0;
  while( curr_line != NULL )
  {
    printf( "%s\n", curr_line );
    curr_line = ( char * )usage_text[i++];
  }
}

ts_Hdr *PrintOPCode( ts_DataInt *pData )
{
  ts_Hdr *retval = NULL;

  switch( pData->nData )
  {
  case CMD_START_MOTION:
  case CMD_KILL_MOTION:
  case CMD_KEY_FRAME:
  case CMD_INTERP_FRAME:
  case CMD_DELAY:
  case CMD_END_ANIM:
  case CMD_FORCE:
  case CMD_IMPULSE:
  case CMD_JUMP:
  case CMD_NEW_ANIM:
  case CMD_CODE:
  case CMD_IF_SET:
  case CMD_IF_NOT_SET:
  case CMD_IF_EQ:
  case CMD_ACTION:
  case CMD_WAIT_HIT_GROUND:
  case CMD_WAIT_TIL_HOLD:
  case CMD_WAIT_FOR_PAUSE:
  case CMD_WAIT_FOR_FRAME:
  case CMD_WAIT_FOR_END:
  case CMD_SET_FLAG:
  case CMD_CLR_FLAG:
  case CMD_CREATE_PROC:
  case CMD_KILL_PROC:
  case CMD_SOUND:
  case CMD_ATTACH_OBJ:
  case CMD_DETACH_OBJ:
  case CMD_SLAVE_POS:
  case CMD_SLAVE_FORCE:
  case CMD_SLAVE_IMPULSE:
  case CMD_CHECK_DEAD:
  case CMD_COLLISION:
  case CMD_ROTATE:
  case CMD_IMPULSE_X:
  case CMD_IMPULSE_Y:
  case CMD_IMPULSE_Z:
  case CMD_OFFSET:
  case CMD_CODE_1F:
  case CMD_CODE_2F:
  case CMD_CODE_3F:
  case CMD_WAIT_INTERP_DONE:
  case CMD_PLAY_RANDOM_SOUND:
  case CMD_PLAY_SOUND:
  case CMD_PLAY_SOUND_VOLUME:
  case CMD_COLLISION_DELAY:
  case CMD_PAUSE_MOTION:
  case CMD_RESUME_MOTION:
  case CMD_CODE_1I:
  case CMD_IF_LE:
  case CMD_IF_LT:
  case CMD_IF_NE:
  case CMD_IF_GE:
  case CMD_IF_GT:
  case CMD_GET_RANDOM:
  case CMD_GET_FRAME:
  case CMD_GET_ACTION:
  case CMD_TEST_VALUE:
  case CMD_TEST_FLAG:
  case CMD_TEST_ACTION:
  case CMD_TEST_ENDMOTION:
  case CMD_TEST_RANDOM:
  case CMD_TEST_FRAME:
  case CMD_TEST_INTERPDONE:
  case CMD_RES_OP0A:
  case CMD_RES_OP0B:
  case CMD_RES_OP1F:
  case CMD_RES_OP1I:
  case CMD_TEST_ONGROUND:
  case CMD_POP:
  case CMD_SWAP:
  case CMD_DUP:
  case CMD_INT:
  case CMD_FLOAT:
  case CMD_ADD:
  case CMD_SUB:
  case CMD_MUL:
  case CMD_DIV:
  case CMD_MOD:
  case CMD_BIT_AND:
  case CMD_BIT_OR:
  case CMD_BIT_XOR:
  case CMD_BIT_NOT:
  case CMD_AND:
  case CMD_OR:
  case CMD_NOT:
  case CMD_CMP:
  case CMD_JNE:
  case CMD_JE:
  case CMD_JL:
  case CMD_JG:
  case CMD_JLE:
  case CMD_JGE:
  case CMD_CALL:
  case CMD_RETURN:
  case CMD_SLEEP:
  case CMD_START_MOTION2:
  case CMD_KILL_MOTION2:
  case CMD_TEST_ENDMOTION2:
  case CMD_TEST_FRAME2:
  case CMD_WAIT_FOR_END2:
  case CMD_WAIT_FOR_FRAME2:
  case CMD_INTERP_FRAME2:
  case CMD_WAIT_INTERP_DONE2:
  case CMD_PAUSE_MOTION2:
  case CMD_RESUME_MOTION2:
  case CMD_GET_FRAME2:
  case CMD_MIN:
  case CMD_MAX:
  case CMD_ABS:
  case CMD_BNE:
  case CMD_BE:
  case CMD_BL:
  case CMD_BG:
  case CMD_BLE:
  case CMD_BGE:
  case CMD_PUSHREG:
  case CMD_POPREG:
  case CMD_DEREF:
  case CMD_DEREF_INT:
  case CMD_DEREF_FLOAT:
  case CMD_POP_CALL:
  case CMD_SET_ANIM_RATE:
  case CMD_SET_ANIM_RATE2:
  case CMD_SET_ACTION_LO:
  case CMD_GET_ACTION_LO:
  case CMD_SET_ACTION_HI:
  case CMD_GET_ACTION_HI:
  case CMD_SHL:
  case CMD_SHR:
  case CMD_SHRZ:
  case CMD_PLAY_SOUND_STR:
  case CMD_PLAY_SOUND_VOLUME_STR:
    {
      retval = pData->hBlock.pNext;
      if ( bDebug )
      {
        retval = retval->pNext;
      }
      fprintf( fListingFile, "%24.24s\t\t//%#x: %#x\n", opCodeNames[ pData->nData ], nGlobalOffset, pData->nData );

      if ( bDebug )
        nGlobalOffset += INT_LEN;
      nGlobalOffset += OPCODE_LEN;
    }
    break;
  case CMD_PUSHI:
  case CMD_PUSHF:
  case CMD_PUSHS:
  case CMD_LOAD:
  case CMD_STORE:
  case CMD_CALLC:
  case CMD_PUSH_CALL:
  case CMD_LOAD_CALL:
  case CMD_STORE_CALL:
    {
      ts_Hdr *pParam = pData->hBlock.pNext;
      if ( bDebug )
      {
        pParam = pParam->pNext;
      }

      fprintf( fListingFile, "%24.24s\t", opCodeNames[ pData->nData ] );

      char *cString = NULL;

      switch( pParam->nType )
      {
      case LIST_INTEGER:
        switch( pData->nData )
        {
        case CMD_PUSHS:
          cString = CStringify( ( char * )( FindStringByIndex( ( ( ts_DataInt * )pParam )->nData )->ulValue ) );
          fprintf( fListingFile, "\"%s\"", cString );
          free( cString );
          break;
        case CMD_LOAD:
        case CMD_STORE:
        case CMD_LOAD_CALL:
        case CMD_STORE_CALL:
          fprintf( fListingFile, "%s", FindVariableNameByIndex( ( ( ts_DataInt * )pParam )->nData )->caName );
          break;
        case CMD_CALLC:
          fprintf( fListingFile, "%s", FindFunctionNameByIndex( ( ( ts_DataInt * )pParam )->nData )->caName );
          break;
        default:
          {
            fprintf( fListingFile, "%d", ( ( ts_DataInt * )pParam )->nData );
          }
          break;
        }
        break;
      case LIST_FLOAT:
        fprintf( fListingFile, "%f ", ( ( ts_DataFloat * )pParam )->fData );
        break;
      case LIST_ADDRESS:
        fprintf( fListingFile, "%%%s", FindSymbolByAddress( ( ( ts_DataInt * )pParam )->nData )->caName, ( ( ts_DataInt * )pParam )->nData );
        break;
      case LIST_OPCODE:
        printf( "\nERROR WRITING LISTING FROM LINE %d\n", pData->hBlock.line );
      default:
        ExitWithError( );
        break;
      }

      retval = pData->hBlock.pNext->pNext;
      if ( bDebug )
      {
        retval = retval->pNext;
      }

      /* now print offset information */
      fprintf( fListingFile, "\t//%#x: %#x", nGlobalOffset, pData->nData );

      switch( pData->nData )
      {
      case CMD_PUSHS:
        fprintf( fListingFile, "\t(string index: %d)\n", ( ( ts_DataInt * )pParam )->nData );
        break;
      case CMD_CALLC:
        fprintf( fListingFile, "\t(function index: %d) (function name string index: %d)\n", ( ( ts_DataInt * )pParam )->nData, FindStringIndex( FindFunctionNameByIndex( ( ( ts_DataInt * )pParam )->nData )->caName ) );
        break;
      case CMD_LOAD:
      case CMD_STORE:
        fprintf( fListingFile, "\t(variable index: %d)\n", ( ( ts_DataInt * )pParam )->nData );
        break;
      default:
        fprintf( fListingFile, "\n" );
        break;
      }

      if ( bDebug )
        nGlobalOffset += INT_LEN;
      nGlobalOffset += OPCODE_LEN + ADDRESS_LEN;
    }
    break;
  case CMD_CASE:
    {
      ts_DataInt *pLabelCount = ( ts_DataInt * )pData->hBlock.pNext;
      ts_DataInt *pCurrAddress = ( ts_DataInt * )pLabelCount->hBlock.pNext;

      if ( bDebug )
      {
        pLabelCount = pCurrAddress;
        pCurrAddress = ( ts_DataInt * )pCurrAddress->hBlock.pNext;
      }

      fprintf( fListingFile, "%24.24s\t", opCodeNames[ pData->nData ] );

      for( int caseOffset = 0; caseOffset < pLabelCount->nData; caseOffset++ )
      {
        if ( pCurrAddress->hBlock.nType != LIST_ADDRESS )
        {
          ExitWithError( );
        }
        fprintf( fListingFile, " %s", FindSymbolByAddress( pCurrAddress->nData )->caName );
        if ( pCurrAddress->hBlock.pNext == NULL )
        {
          ExitWithError( );
        }
        pCurrAddress = ( ts_DataInt * )pCurrAddress->hBlock.pNext;
      }

      fprintf( fListingFile, "\t//%#x: %#x\n", nGlobalOffset, pData->nData );

      retval = ( ts_Hdr * )pCurrAddress;

      if ( bDebug )
        nGlobalOffset += INT_LEN;
      nGlobalOffset += OPCODE_LEN + INT_LEN + ( pLabelCount->nData * ADDRESS_LEN );
    }
    break;
  default:
    return( NULL );
    break;
  }

  return( retval );
}

void PrintLabels( void )
{
  bool bFound = false;

	pPatchList = &PatchList;

	while ( pPatchList->pNext != NULL )
	{
		pPatchList = pPatchList->pNext;
		pSymbolList = &SymbolList;

		if ( ( ( ts_DataAddress * )pPatchList->pData )->ulData == ( unsigned int )nGlobalOffset )
    { // this is a place in the file that has been patched, so print it's label
		  
      // Run through each entry in the symbol list to match the name with the patch
		  bFound = false;
		  while ( pSymbolList->pNext != NULL )
		  {
			  pSymbolList = pSymbolList->pNext;
			  if ( ( !strcmp( pSymbolList->caName, pPatchList->caName ) ) && ( pSymbolList->line != -1 ) )
			  {
          pSymbolList->line = -1;
	  	    fprintf( fListingFile, "%s:\t//%#x\n", pSymbolList->caName, nGlobalOffset );
				  break;
			  }
		  }
    }

	}
}

void PrintVariables( )
{
  pVarList = &VarList;

  while( pVarList->pNext != NULL )
  {
    pVarList = pVarList->pNext;

    if ( pVarList->uVarType & PUBLISHED_TYPE )
    {
      fprintf( fListingFile, "published " );
    }

    switch ( pVarList->uVarType & ~BASE_TYPE_MASK )
    {
    case INT_TYPE:
      if ( pVarList->uVarType & UNINITIALIZED_TYPE )
      {
        fprintf( fListingFile, "int %s;\t// (var name string index: %d)\n", pVarList->caName, FindStringIndex( pVarList->caName ) );
      }
      else
      {
        fprintf( fListingFile, "int %s = %d;\t// (var name string index: %d)\n", pVarList->caName, pVarList->iInitialValue, FindStringIndex( pVarList->caName ) );
      }
      break;
    case FLOAT_TYPE:
      if ( pVarList->uVarType & UNINITIALIZED_TYPE )
      {
        fprintf( fListingFile, "float %s;\t// (var name string index: %d)\n", pVarList->caName, FindStringIndex( pVarList->caName ) );
      }
      else
      {
        fprintf( fListingFile, "float %s = %f;\t// (var name string index: %d)\n", pVarList->caName, *( float * )&( pVarList->iInitialValue ), FindStringIndex( pVarList->caName ) );
      }
      break;
    case STRING_TYPE:
      if ( pVarList->uVarType & UNINITIALIZED_TYPE )
      {
        fprintf( fListingFile, "string %s;\t// (var name string index: %d)\n", pVarList->caName, FindStringIndex( pVarList->caName ) );
      }
      else
      {
        ts_Symbol *pSymbol = FindStringByIndex( pVarList->iInitialValue );
		char *stringify = NULL;
		if ( pSymbol != NULL )
		{
          stringify = CStringify( pSymbol->caName );
          fprintf( fListingFile, "string %s = \"%s\";\t// (var name string index: %d) (string index: %d)\n", pVarList->caName, stringify, FindStringIndex( pVarList->caName ), pVarList->iInitialValue );
          free( stringify );
		}
		else
		{
           fprintf( fListingFile, "string %s;\t// (var name string index: %d) (string index: %d)\n", pVarList->caName, FindStringIndex( pVarList->caName ), pVarList->iInitialValue );
		}
      }
      break;
    default:
      ExitWithError( );
      break;
    }
  }
}

void PrintListing( void )
{
  bool usedLine = false;
  ts_Hdr *currData = &DataList;

  fListingFile = fopen( ListingFile, "wt" );

  // first, delcare variables with/with out initial values
  PrintVariables( );

  // reset the stream offset counter
  nGlobalOffset = 0;

  currData = currData->pNext;
  while ( currData != NULL )
  { // print the opcode stream to a text file

    // first, determine if there are any labels at the current offset
    PrintLabels( );

    if ( currData->nType == LIST_DATAWORD )
    { // check if this was a raw address push (DW opcode)
      int symbolAddress = ( ( ts_DataInt * )currData )->nData;
      char *symbolName = FindSymbolByAddress( symbolAddress )->caName;
      fprintf( fListingFile, "%24.24s\t%s\t//%#x\n", "DW", symbolName, symbolAddress );

      nGlobalOffset += DATAWORD_LEN;
      currData = currData->pNext;
    }
    else
    { // print the opcode
      currData = PrintOPCode( ( ts_DataInt * )currData );
    }
  }

  fclose( fListingFile );
  fListingFile = NULL;
}

char *CStringify( char *src )
{
  char *destBuffer, *dst;
  char *curr;

  if ( src == NULL )
  {
    return NULL;
  }

  destBuffer = ( char * ) malloc ( 256 );
  memset( destBuffer, 0, 256 );

  dst = destBuffer;
  curr = src;
  while( *curr != '\0' )
  {
    switch( *curr )
    {
    case '\\':
      *dst++ = '\\';
      break;
    case '"':
      *dst++ = '\\';
      break;
    case '\n':
      *dst++ = '\\';
      *dst++ = 'n';
      *curr++;
      continue;
    case '\t':
      *dst++ = '\\';
      *dst++ = 't';
      *curr++;
      continue;
    case '\r':
      *dst++ = '\\';
      *dst++ = 'r';
      *curr++;
      continue;
    case '\b':
      *dst++ = '\\';
      *dst++ = 'b';
      *curr++;
      continue;
    case '\f':
      *dst++ = '\\';
      *dst++ = 'f';
      *curr++;
      continue;
    default:
      break;
    }

    *dst++ = *curr++;
  }

  return( destBuffer );
}

// allow C modules to call C++ hash function
extern "C" unsigned int Hash__CalcChecksum( const void* buf, int length, unsigned int initVal )
{
	return Hash::CalcChecksum( buf, length, initVal );
}
