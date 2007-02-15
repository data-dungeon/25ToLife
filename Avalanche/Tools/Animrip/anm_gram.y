%{
extern int yyparse(void);
#include "animrip.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>

extern int  iLineCount;
extern int  unprocessedFileLine;
extern char unprocessedFileName[];

char			szFileName[256];
int			errorCount = 0;
int         yylineno;
%}

%pure_parser

%union
{
	int	nValue;
	float	fValue;
	char	*pcString;
}

%left L_AND L_OR
%left BIT_OR BIR_AND BIT_XOR
%left MINUS PLUS
%left MULTIPLY DIVIDE MODULUS
%right L_NOT BIT_NOT

%token  <pcString>   LABEL_
%token  <pcString>   REF_
%token  <pcString>   TEXT_
%token  <pcString>   STRING_
%token  <nValue>     HEX_
%token  <nValue>     INTEGER_
%token  <fValue>     FLOAT_

%type   <fValue>     fExpr
%type   <fValue>     fAssign
%type   <fValue>     floatparam
%type   <fValue>     floatvar
%type   <fValue>     floatliteral

%type   <nValue>     iExpr
%type   <nValue>     iAssign
%type   <nValue>     intparam
%type   <nValue>     intvar
%type   <nValue>     intliteral

%type	<pcString>	sExpr
%type	<pcString>	stringparam
%type	<pcString>	stringvar
%type	<pcString>	stringliteral

%type   <pcString>   labellist

%token	CR_
%token	ANIM_TABLE
%token	DW
%token	ANIM
%token	START_MOTION
%token	KILL_MOTION
%token	KEY_FRAME
%token	INTERP_FRAME
%token	DELAY
%token	END_ANIM
%token	FORCE
%token	IMPULSE
%token	JUMP
%token	NEW_ANIM
%token	CODE
%token	CODE_1F
%token	CODE_2F
%token	CODE_3F
%token	IF_SET
%token	IF_NOT_SET
%token	IF_EQ
%token	ACTION
%token	WAIT_HIT_GROUND
%token	WAIT_TIL_HOLD
%token	WAIT_FOR_PAUSE
%token	WAIT_FOR_FRAME
%token	WAIT_FOR_END
%token	SET_FLAG
%token	CLR_FLAG
%token	CREATE_PROC
%token	KILL_PROC
%token	SOUND
%token	ATTACH_OBJ
%token	DETACH_OBJ
%token	SLAVE_POS
%token	SLAVE_FORCE
%token	SLAVE_IMPULSE
%token	CHECK_DEAD
%token	COLLISION
%token	ROTATE
%token	IMPULSE_X
%token	IMPULSE_Y
%token	IMPULSE_Z
%token	OFFSET
%token	WAIT_INTERP_DONE
%token	PLAY_RANDOM_SOUND
%token	PLAY_SOUND
%token	PLAY_SOUND_VOLUME
%token	COLLISION_DELAY
%token	PAUSE_MOTION
%token	RESUME_MOTION
%token	CODE_1I
%token	IF_LE
%token	IF_LT
%token	IF_NE
%token	IF_GE
%token	IF_GT
%token	GET_RANDOM
%token	GET_FRAME	 
%token	GET_ACTION	 
%token	TEST_VALUE
%token	TEST_FLAG
%token	TEST_ACTION
%token	TEST_ENDMOTION
%token	TEST_RANDOM
%token	TEST_FRAME
%token	TEST_INTERPDONE
%token	CASE 
%token	RES_OP0A
%token	RES_OP0B
%token	RES_OP1F 
%token	RES_OP1I 
%token	TEST_ONGROUND
%token   PUSH
%token   PUSHI
%token   PUSHF
%token   PUSHS
%token   PUSHA
%token   POP
%token   SWAP
%token   DUP
%token   LOAD
%token   LOAD_DEREF
%token   STORE
%token   ADD
%token   SUB
%token   MUL
%token   DIV
%token   MOD
%token   BIT_AND
%token   BIT_OR
%token   BIT_XOR
%token   BIT_NOT
%token   AND
%token   OR
%token   NOT
%token   INT
%token   FLOAT
%token   CMP
%token   JNE
%token   JE
%token   JL
%token   JG
%token   JLE
%token   JGE
%token   CALLC
%token   CALL
%token   RETURN
%token   AI_SCRIPT
%token   ANIMRIP_AI_SCRIPT
%token   SLEEP

%token   BNE
%token   BE
%token   BL
%token   BG
%token   BLE
%token   BGE

%token   PUSHREG
%token   POPREG

%token   START_MOTION2
%token   KILL_MOTION2
%token   TEST_ENDMOTION2
%token   TEST_FRAME2
%token   WAIT_FOR_END2
%token   WAIT_FOR_FRAME2
%token   INTERP_FRAME2
%token   WAIT_INTERP_DONE2
%token	PAUSE_MOTION2
%token	RESUME_MOTION2
%token	GET_FRAME2

%token   MIN
%token   MAX
%token   ABS

%token   PAREN_OPEN
%token   PAREN_CLOSE

%token   INT_DECL
%token   FLOAT_DECL
%token   STRING_DECL
%token   PUBLISHED_DECL

%token   SEMI
%token   ASSIGN

%token   INCR
%token   DECR

%token   DEREF
%token   DEREF_INT
%token   DEREF_FLOAT

%token   PUSH_CALL
%token   POP_CALL
%token   LOAD_CALL
%token   STORE_CALL

%token   SET_ANIM_RATE
%token   SET_ANIM_RATE2

%token   SET_ACTION_LO
%token   GET_ACTION_LO
%token   SET_ACTION_HI
%token   GET_ACTION_HI

%token   SHL
%token   SHR
%token   SHRZ

%token   CPPLINE

%%

program: /* empty line */
	| statementlist
	;

statementlist:
	statementlist statement
	| statement
	;

statement:
	 command SEMI                             { }
	| command CR_                             { iLineCount++; unprocessedFileLine++; }
	| CR_                                     { iLineCount++; unprocessedFileLine++; }
   | SEMI                                    { }
	;

labellist: /* empty */                       { CreateEmptyLabelList( ); }
	| labellist TEXT_                         { AppendLabelToLabelList( $1, $2 ); }
	;

command:
	   LABEL_                                 { ProcessLabel( $1 ); }
	|  CPPLINE INTEGER_ STRING_					  { ProcessCppLine( (int)$2, $3 ); }
	|  ANIM_TABLE TEXT_	                     { ProcessAnimTable( $2 ); }
	|	DW TEXT_                               { ProcessDW ( $2 ); }
	|	ANIM TEXT_                             { ProcessAnim ( $2 ); }
	|	START_MOTION iExpr iExpr iExpr         { ProcessStartMotion( $2, $3, $4 ); }
	|	KILL_MOTION                            { ProcessKillMotion( ); }
	|	KEY_FRAME iExpr                        { ProcessKeyFrame( $2 ); }
	|	INTERP_FRAME iExpr iExpr iExpr iExpr	{ ProcessInterpFrame( $2, $3, $4, $5 ); }
	|	DELAY iExpr                            { ProcessDelay( $2 ); }
	|	END_ANIM                               { ProcessEndAnim( ); }
	|	FORCE fExpr fExpr fExpr                { ProcessForce( $2, $3, $4 ); }
	|	IMPULSE fExpr fExpr fExpr              { ProcessImpulse( $2, $3, $4 ); }
	|	IMPULSE_X fExpr                        { ProcessImpulse_X( $2 ); }
	|	IMPULSE_Y fExpr                        { ProcessImpulse_Y( $2 ); }
	|	IMPULSE_Z fExpr                        { ProcessImpulse_Z( $2 ); }
	|	OFFSET fExpr fExpr fExpr               { ProcessOffset( $2, $3, $4 ); }
	|	JUMP TEXT_                             { ProcessJumpT( $2 ); }
   |  JUMP                                   { ProcessJump( ); }
	|	NEW_ANIM iExpr                         { ProcessNewAnim( $2 ); }
	|	CODE iExpr                             { ProcessCode( $2 ); }
	|	CODE_1I iExpr iExpr                    { ProcessCode1I( $2, $3 ); }
	|	CODE_1F iExpr fExpr                    { ProcessCode1F( $2, $3 ); }
	|	CODE_2F iExpr fExpr fExpr              { ProcessCode2F( $2, $3, $4 ); }
	|	CODE_3F iExpr fExpr fExpr fExpr        { ProcessCode3F( $2, $3, $4, $5 ); }
	|	IF_SET TEXT_                           { ProcessIfSet( $2 ); }
	|	IF_NOT_SET TEXT_                       { ProcessIfNotSet( $2 ); }
	|	IF_EQ TEXT_                            { ProcessIfEq( $2 ); }
	|	ACTION iExpr                           { ProcessAction( $2 ); }
	|	WAIT_HIT_GROUND                        { ProcessWaitHitGround( ); }
	|	WAIT_FOR_PAUSE                         { ProcessWaitForPause( ); }
	|	WAIT_FOR_FRAME iExpr							{ ProcessWaitForFrame( $2 ); }
	|	WAIT_FOR_END                           { ProcessWaitForEnd( ); }
	|	SET_FLAG iExpr                         { ProcessSetFlag( $2 ); }
	|	CLR_FLAG	iExpr                         { ProcessClrFlag( $2 ); }
	|	CREATE_PROC	iExpr iExpr                { ProcessCreateProc( $2, $3 ); }
	|	KILL_PROC iExpr                        { ProcessKillProc( $2 ); }
	|	SOUND	iExpr                            { ProcessSound( $2 ); }
	|	ATTACH_OBJ                             { ProcessAttachObj( ); }
	|	DETACH_OBJ                             { ProcessDetachObj( ); }
	|	SLAVE_POS fExpr fExpr fExpr            { ProcessSlavePos( $2, $3, $4); }
	|	SLAVE_FORCE fExpr fExpr fExpr          { ProcessSlaveForce( $2, $3, $4); }
	|	SLAVE_IMPULSE fExpr fExpr fExpr        { ProcessSlaveImpulse( $2, $3, $4); }
	|	CHECK_DEAD                             { ProcessCheckDead( ); }
	|	COLLISION iExpr                        { ProcessCollision( $2 ); }
	|	ROTATE fExpr fExpr fExpr               { ProcessRotate( $2, $3, $4 ); }
	|	WAIT_INTERP_DONE                       { ProcessWaitInterpDone( ); }
   |  PLAY_RANDOM_SOUND iExpr iExpr iExpr    { ProcessPlayRandomSound( $2, $3, $4 ); }
	|	PLAY_SOUND iExpr								{ ProcessPlaySound( $2 ); }
	|	PLAY_SOUND stringliteral					{ ProcessPlaySoundStr( $2 ); }
	|	PLAY_SOUND_VOLUME iExpr iExpr          { ProcessPlaySoundVolume( $2, $3 ); }
	|	PLAY_SOUND_VOLUME stringliteral iExpr	{ ProcessPlaySoundVolumeStr( $2, $3 ); }
	|	COLLISION_DELAY iExpr                  { ProcessCollisionDelay( $2 ); }
	|	PAUSE_MOTION                           { ProcessPauseMotion( ); }
	|	RESUME_MOTION                          { ProcessResumeMotion( ); }
	|	IF_LE TEXT_	                           { ProcessIfLE( $2 ); }
	|	IF_LT TEXT_	                           { ProcessIfLT( $2 ); }
	|	IF_NE TEXT_	                           { ProcessIfNE( $2 ); }
	|	IF_GE TEXT_	                           { ProcessIfGE( $2 ); }
	|	IF_GT TEXT_	                           { ProcessIfGT( $2 ); }
	|	GET_RANDOM iExpr                       { ProcessGetRandom( $2 ); }
	|	GET_FRAME                              { ProcessGetFrame( ); }
	|	GET_ACTION                             { ProcessGetAction( ); }
	|	TEST_VALUE iExpr                       { ProcessTestValue( $2 ); }
	|	TEST_FLAG iExpr                        { ProcessTestFlag( $2 ); }
	|	TEST_ACTION iExpr                      { ProcessTestAction( $2 ); }
	|	TEST_ENDMOTION                         { ProcessTestEndMotion( ); }
	|	TEST_RANDOM fExpr                      { ProcessTestRandom( $2 ); }
	|	TEST_FRAME iExpr                       { ProcessTestFrame( $2 ); }
	|	TEST_INTERPDONE                        { ProcessTestInterpDone( ); }
	|	CASE labellist                         { ProcessCase( $2 ); }
	|	RES_OP0A                               { ProcessRESOP0A( ); }
	|	RES_OP0B                               { ProcessRESOP0B( ); }
	|	RES_OP1F fExpr                         { ProcessRESOP1F( $2 ); }
	|	RES_OP1I iExpr                         { ProcessRESOP1I( $2 ); }
	|	TEST_ONGROUND                          { ProcessTestOnGround( ); }

   |  PUSH REF_                              { ProcessPushA( $2 ); }
   |  PUSHI REF_                             { ProcessPushA( $2 ); }
   |  PUSHA REF_                             { ProcessPushA( $2 ); }
   |  PUSH iExpr                             { }
   |  PUSHI iExpr                            { }
   |  PUSH fExpr                             { }
   |  PUSHF fExpr                            { }
   |  PUSH STRING_                           { ProcessPushS( $2 ); }
   |  PUSHS STRING_                          { ProcessPushS( $2 ); }
   |  PUSH TEXT_                             { ProcessLoad( $2 ); }
   |  POP                                    { ProcessPop( ); }
   |  POP TEXT_                              { ProcessStore( $2 ); }
   |  SWAP                                   { ProcessPushI( 1 ); ProcessSwap( ); }
   |  SWAP iExpr                             { ProcessSwap( ); }
   |  DUP                                    { ProcessPushI( 0 ); ProcessDup( ); }
   |  DUP iExpr                              { ProcessDup( ); }
   |  LOAD TEXT_                             { ProcessLoad( $2 ); }
   |  STORE TEXT_                            { ProcessStore( $2 ); }
   |  STORE TEXT_ TEXT_                      { ProcessLoad( $3 ); ProcessStore( $2 ); }
   |  STORE TEXT_ fExpr                      { ProcessStore( $2 ); }
   |  STORE TEXT_ iExpr                      { ProcessStore( $2 ); }
   |  ADD                                    { ProcessAdd( ); }
   |  SUB                                    { ProcessSub( ); }
   |  MUL                                    { ProcessMul( ); }
   |  DIV                                    { ProcessDiv( ); }
   |  MOD                                    { ProcessMod( ); }
   |  BIT_AND                                { ProcessBitAnd( ); }
   |  BIT_OR                                 { ProcessBitOr( ); }
   |  BIT_XOR                                { ProcessBitXor( ); }
   |  BIT_NOT                                { ProcessBitNot( ); }
   |  AND                                    { ProcessAnd( ); }
   |  OR                                     { ProcessOr( ); }
   |  NOT                                    { ProcessNot( ); }
   |  INT                                    { ProcessInt( ); }
   |  FLOAT                                  { ProcessFloat( ); }
   |  CMP                                    { ProcessCMP( ); }
   |  JNE TEXT_                              { ProcessJNET( $2 ); }
   |  JNE                                    { ProcessJNE( ); }
   |  JE TEXT_                               { ProcessJET( $2 ); }
   |  JE                                     { ProcessJE( ); }
   |  JL TEXT_                               { ProcessJLT( $2 ); }
   |  JL                                     { ProcessJL( ); }
   |  JG TEXT_                               { ProcessJGT( $2 ); }
   |  JG                                     { ProcessJG( ); }
   |  JLE TEXT_                              { ProcessJLET( $2 ); }
   |  JLE                                    { ProcessJLE( ); }
   |  JGE TEXT_                              { ProcessJGET( $2 ); }
   |  JGE                                    { ProcessJGE( ); }
   |  CALLC TEXT_                            { ProcessCallC( $2 ); }
   |  CALL TEXT_                             { ProcessCallT( $2 ); }
   |  CALL                                   { ProcessCall( ); }
   |  RETURN                                 { ProcessReturn ( ); }
   |  AI_SCRIPT                              { ProcessAIScript ( ); }
   |  ANIMRIP_AI_SCRIPT                      { ProcessAIScript ( ); }

   |  BNE TEXT_                              { ProcessBNET( $2 ); }
   |  BNE                                    { ProcessBNE( ); }
   |  BE TEXT_                               { ProcessBET( $2 ); }
   |  BE                                     { ProcessBE( ); }
   |  BL TEXT_                               { ProcessBLT( $2 ); }
   |  BL                                     { ProcessBL( ); }
   |  BG TEXT_                               { ProcessBGT( $2 ); }
   |  BG                                     { ProcessBG( ); }
   |  BLE TEXT_                              { ProcessBLET( $2 ); }
   |  BLE                                    { ProcessBLE( ); }
   |  BGE TEXT_                              { ProcessBGET( $2 ); }
   |  BGE                                    { ProcessBGE( ); }

   |  PUSHREG                                { ProcessPushReg( ); }
   |  POPREG                                 { ProcessPopReg( ); }

   |  SLEEP fExpr                            { ProcessSleep( $2 ); }

   |  START_MOTION2 iExpr iExpr iExpr iExpr  { ProcessStartMotion2( $2, $3, $4, $5 ); }
	|	KILL_MOTION2 iExpr                     { ProcessKillMotion2( $2 ); }
	|	TEST_ENDMOTION2 iExpr                  { ProcessTestEndMotion2( $2 ); }
	|	TEST_FRAME2 iExpr iExpr                { ProcessTestFrame2( $2, $3 ); }
	|	WAIT_FOR_END2 iExpr                    { ProcessWaitForEnd2( $2 ); }
	|	WAIT_FOR_FRAME2 iExpr iExpr            { ProcessWaitForFrame2( $2, $3 ); }
	|	INTERP_FRAME2 iExpr iExpr iExpr iExpr iExpr	{ ProcessInterpFrame2( $2, $3, $4, $5, $6 ); }
	|	WAIT_INTERP_DONE2 iExpr                { ProcessWaitInterpDone2( $2 ); }
	|	PAUSE_MOTION2 iExpr                    { ProcessPauseMotion2( $2 ); }
	|	RESUME_MOTION2 iExpr                   { ProcessResumeMotion2( $2 ); }
	|	GET_FRAME2 iExpr                       { ProcessGetFrame2( $2 ); }

   |  MIN                                    { ProcessMin( ); }
   |  MAX                                    { ProcessMax( ); }
   |  ABS                                    { ProcessAbs( ); }

   |  INT_DECL TEXT_ ASSIGN fAssign                  { ProcessIntDeclInitial( $2, ( int )$4, 0 ); }
   |  PUBLISHED_DECL INT_DECL TEXT_ ASSIGN fAssign   { ProcessIntDeclInitial( $3, ( int )$5, 1 ); }

   |  INT_DECL TEXT_ ASSIGN iAssign                  { ProcessIntDeclInitial( $2, $4, 0 ); }
   |  PUBLISHED_DECL INT_DECL TEXT_ ASSIGN iAssign   { ProcessIntDeclInitial( $3, $5, 1 ); }

   |  FLOAT_DECL TEXT_ ASSIGN fAssign                { ProcessFloatDeclInitial( $2, ( float )$4, 0 ); }
   |  PUBLISHED_DECL FLOAT_DECL TEXT_ ASSIGN fAssign { ProcessFloatDeclInitial( $3, ( float )$5, 1 ); }

   |  STRING_DECL TEXT_ ASSIGN STRING_                { ProcessStringDeclInitial( $2, $4, 0 ); }
   |  PUBLISHED_DECL STRING_DECL TEXT_ ASSIGN STRING_ { ProcessStringDeclInitial( $3, $5, 1 ); }

   |  INT_DECL TEXT_                         { ProcessIntDecl( $2, 0 ); }
   |  PUBLISHED_DECL INT_DECL TEXT_          { ProcessIntDecl( $3, 1 ); }
   |  FLOAT_DECL TEXT_                       { ProcessFloatDecl( $2, 0 ); }
   |  PUBLISHED_DECL FLOAT_DECL TEXT_        { ProcessFloatDecl( $3, 1 ); }
   |  STRING_DECL TEXT_                      { ProcessStringDecl( $2, 0 ); }
   |  PUBLISHED_DECL STRING_DECL TEXT_       { ProcessStringDecl( $3, 1 ); }

   |  INCR                                            { ProcessPushI( 1 ); ProcessAdd( ); }
   |  INCR TEXT_                                      { ProcessPushI( 1 ); ProcessLoad( $2 ); ProcessAdd( ); ProcessStore( $2 ); }
   |  DECR                                            { ProcessPushI( -1 ); ProcessAdd( ); }
   |  DECR TEXT_                                      { ProcessPushI( -1 ); ProcessLoad( $2 ); ProcessAdd( ); ProcessStore( $2 ); }

   |  DEREF_INT                              { ProcessDerefInt( ); }
   |  DEREF_FLOAT                            { ProcessDerefFloat( ); }
   |  DEREF                                  { ProcessDeref( ); }
   |  LOAD_DEREF TEXT_                       { ProcessLoad( $2 ); ProcessDeref( ); }

   |  PUSH_CALL REF_                         { ProcessPushCall( $2 ); }
   |  POP_CALL                               { ProcessPopCall( ); }
   |  LOAD_CALL TEXT_                        { ProcessLoadCall( $2 ); }
   |  STORE_CALL TEXT_                       { ProcessStoreCall( $2 ); }

   |  SET_ANIM_RATE fExpr                    { ProcessSetAnimRate( $2 ); }
   |  SET_ANIM_RATE2 fExpr                   { ProcessSetAnimRate2( $2 ); }

   |  SET_ACTION_LO iExpr                    { ProcessSetActionLo( $2 );}
   |  GET_ACTION_LO                          { ProcessGetActionLo( );}
   |  SET_ACTION_HI iExpr                    { ProcessSetActionHi( $2 );}
   |  GET_ACTION_HI                          { ProcessGetActionHi( );}

   |  SHL                                    { ProcessShiftLeft( ); }
   |  SHR                                    { ProcessShiftRight( ); }
   |  SHRZ                                   { ProcessShiftRightUnsigned( ); }
	;

iAssign:
   intliteral                           { $$ = $1; }
   | iAssign PLUS iAssign               { ProcessAdd( ); $$ = $1 + $3; }
   | iAssign MINUS iAssign              { ProcessSub( ); $$ = $1 - $3; }
   | iAssign MULTIPLY iAssign           { ProcessMul( ); $$ = $1 * $3; }
   | iAssign DIVIDE iAssign             { ProcessDiv( ); $$ = $1 / $3; }
   | iAssign MODULUS iAssign            { ProcessMod( ); $$ = $1 % $3; }
   | L_NOT iAssign %prec L_NOT          { ProcessNot( ); $$ = !($2); }
   | iAssign L_AND iAssign              { ProcessAnd( ); $$ = $1 && $3; }
   | iAssign L_OR iAssign               { ProcessOr( ); $$ = $1 || $3; }
   | iAssign BIT_AND iAssign            { ProcessBitAnd( ); $$ = $1 & $3; }
   | iAssign BIT_OR iAssign             { ProcessBitOr( ); $$ = $1 | $3; }
   | iAssign BIT_XOR iAssign            { ProcessBitXor( ); $$ = $1 ^ $3; }
   | BIT_NOT iAssign                    { ProcessBitNot( ); $$ = ~($2); }
   | iExpr SHL iExpr                    { ProcessShiftLeft( ); $$ = $1 << $3; }
   | iExpr SHR iExpr                    { ProcessShiftRight( ); $$ = $1 >> $3; }
   | iExpr SHRZ iExpr                   { ProcessShiftRightUnsigned( ); $$ = ( unsigned int )$1 >> $3; }
   | PAREN_OPEN iAssign PAREN_CLOSE     { $$ = $2; }
   ;

fAssign:
   floatliteral                         { $$ = $1; }
   | fAssign PLUS fAssign               { ProcessAdd( ); $$ = $1 + $3; }
   | fAssign MINUS fAssign              { ProcessSub( ); $$ = $1 - $3; }
   | fAssign MULTIPLY fAssign           { ProcessMul( ); $$ = $1 * $3; }
   | fAssign DIVIDE fAssign             { ProcessDiv( ); $$ = $1 / $3; }
   | PAREN_OPEN fAssign PAREN_CLOSE     { $$ = $2; }
   ;

iExpr:
   intparam                            { $$ = $1; }
   | iExpr PLUS iExpr                  { ProcessAdd( ); $$ = $1 + $3; }
   | iExpr MINUS iExpr                 { ProcessSub( ); $$ = $1 - $3; }
   | iExpr MULTIPLY iExpr              { ProcessMul( ); $$ = $1 * $3; }
   | iExpr DIVIDE iExpr                { ProcessDiv( ); $$ = $1 / $3; }
   | iExpr MODULUS iExpr               { ProcessMod( ); $$ = $1 % $3; }
   | L_NOT iExpr %prec L_NOT           { ProcessNot( ); $$ = !($2); }
   | iExpr L_AND iExpr                 { ProcessAnd( ); $$ = $1 && $3; }
   | iExpr L_OR iExpr                  { ProcessOr( ); $$ = $1 || $3; }
   | iExpr BIT_AND iExpr               { ProcessBitAnd( ); $$ = $1 & $3; }
   | iExpr BIT_OR iExpr                { ProcessBitOr( ); $$ = $1 | $3; }
   | iExpr BIT_XOR iExpr               { ProcessBitXor( ); $$ = $1 ^ $3; }
   | BIT_NOT iExpr                     { ProcessBitNot( ); $$ = ~($2); }
   | iExpr SHL iExpr                   { ProcessShiftLeft( ); $$ = $1 << $3; }
   | iExpr SHR iExpr                   { ProcessShiftRight( ); $$ = $1 >> $3; }
   | iExpr SHRZ iExpr                  { ProcessShiftRightUnsigned( ); $$ = ( unsigned int )$1 >> $3; }
   | PAREN_OPEN iExpr PAREN_CLOSE      { $$ = $2; }
   ;

intparam:
   intvar                           { $$ = $1; }
   | intliteral                     { $$ = $1; }
  	;

intliteral:
   INTEGER_                         { ProcessPushI( ( int )$1 ); $$ = ( int )$1; }
   | HEX_                           { ProcessPushI( $1 ); $$ = $1; }
   ;

intvar:
   TEXT_                            { $$ = HandleIntParam( $1 ); }
   ;

fExpr:
   floatparam                       { $$ = $1; }
   | fExpr PLUS fExpr               { ProcessAdd( ); $$ = $1 + $3; }
   | fExpr MINUS fExpr              { ProcessSub( ); $$ = $1 - $3; }
   | fExpr MULTIPLY fExpr           { ProcessMul( ); $$ = $1 * $3; }
   | fExpr DIVIDE fExpr             { ProcessDiv( ); $$ = $1 / $3; }
   | PAREN_OPEN fExpr PAREN_CLOSE   { $$ = $2; }
   ;

floatparam:
   floatvar                         { $$ = $1; }
   | floatliteral                   { $$ = $1; }
  	;

floatliteral:
   INTEGER_                         { ProcessPushF( ( float )$1 ); $$ = ( float )$1; }
   | FLOAT_                         { ProcessPushF( $1 ); $$ = $1; }
   ;

floatvar:
   TEXT_                            { $$ = HandleFloatParam( $1 ); }
   ;

sExpr:
	stringparam						{ $$ = NULL; }
	;

stringparam:
	stringvar						{ $$ = NULL; }
	| stringliteral					{ $$ = NULL; }
	;

stringliteral:
	STRING_							{ ProcessPushS( $1 ); }
	;

stringvar:
	TEXT_							{ HandleStringParam( $1 ); $$ = NULL; }
	;
%%
int yyerror( unsigned char *pStr)
{
	fprintf( stderr, "%s(%d) : %s\n", unprocessedFileName, unprocessedFileLine, pStr );
	fprintf( stderr, "%s(%d) : %s\n", szFileName, iLineCount, pStr );
	errorCount++;
	return 0;
}