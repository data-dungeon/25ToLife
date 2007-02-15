%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "anm_gram.h"

YYSTYPE  yylval;
int      iLineCount = 1;
int		 unprocessedFileLine = 1;
char	 unprocessedFileName[ 256 ];
char     string_buf[ 256 ];
char     *string_buf_ptr;

%}

%option yylineno

%p 100000
%e 10000
%x str

%%


[ \t]+              		;

int                     { return INT_DECL          ;}
float                   { return FLOAT_DECL        ;}
string                  { return STRING_DECL       ;}
published               { return PUBLISHED_DECL    ;}

#line { return CPPLINE; }

\%[a-zA-Z_][a-zA-Z0-9_]+ { yylval.pcString = strdup( yytext ); return REF_; }

-?[0-9]+"."[0-9]*f? |
-?0?"."[0-9]+f?           { sscanf( yytext, "%f", &yylval.fValue ); return FLOAT_; }

-?[0-9]+            		{ yylval.nValue = atoi( yytext ); return INTEGER_; }

0[xX][0-9A-Fa-f]+       { yylval.nValue = strtoul( yytext, NULL, 16 ); return HEX_; }

[a-zA-Z_][a-zA-Z0-9_\@]+\:   { yylval.pcString = strdup( yytext ); return LABEL_; }


\"      string_buf_ptr = string_buf; BEGIN(str);

<str>\"        { /* saw closing quote - all done */
        BEGIN(INITIAL);
        *string_buf_ptr = '\0';
        /* return string constant token type and
         * value to parser
         */
         yylval.pcString = strdup( string_buf );
         string_buf[0] = '\0';
         return STRING_;
        }

<str>\n        {
		fprintf( stderr, "%s(%d) : ERROR unterminated string constant\n", unprocessedFileName, unprocessedFileLine );
		exit( 2 );
        }

<str>\\[0-7]{1,3} {
        /* octal escape sequence */
        int result;

        (void) sscanf( yytext + 1, "%o", &result );

        if ( result > 0xff )
        {
		 fprintf( stderr, "%s(%d) : ERROR constant is out-of-bounds\n", unprocessedFileName, unprocessedFileLine );
         exit( 3 );
        }

        *string_buf_ptr++ = result;
        }

<str>\\[0-9]+ {
		 fprintf( stderr, "%s(%d) : ERROR bad escape sequence\n", unprocessedFileName, unprocessedFileLine );
         exit( 4 );
        }

<str>\\n  *string_buf_ptr++ = '\n';
<str>\\t  *string_buf_ptr++ = '\t';
<str>\\r  *string_buf_ptr++ = '\r';
<str>\\b  *string_buf_ptr++ = '\b';
<str>\\f  *string_buf_ptr++ = '\f';

<str>\\(.|\n)  *string_buf_ptr++ = yytext[1];

<str>[^\\\n\"]+        {
        char *yptr = yytext;

        while ( *yptr )
                *string_buf_ptr++ = *yptr++;
        }

\/\/.*\n						{ return CR_               ;}

\<\<                    { return SHL               ;}
\>\>                    { return SHR               ;}
\>\>\>                  { return SHRZ              ;}
\&\&                    { return L_AND             ;}
\|\|                    { return L_OR              ;}
\!                      { return L_NOT             ;}

\|                      { return BIT_OR            ;}
\&                      { return BIT_AND           ;}
\^                      { return BIT_XOR           ;}
\~                      { return BIT_NOT           ;}

\-                      { return MINUS             ;}
\+                      { return PLUS              ;}
\*                      { return MULTIPLY          ;}
\/                      { return DIVIDE            ;}
\%                      { return MODULUS           ;}
\(                      { return PAREN_OPEN        ;}
\)                      { return PAREN_CLOSE       ;}
\;                      { return SEMI              ;}
\=                      { return ASSIGN            ;}

ANIM_TABLE					{ return ANIM_TABLE        ;}
DW								{ return DW                ;}
ANIM							{ return ANIM              ;}
START_MOTION				{ return START_MOTION      ;}
KILL_MOTION					{ return KILL_MOTION       ;}
KEY_FRAME					{ return KEY_FRAME         ;}
INTERP_FRAME				{ return INTERP_FRAME      ;}
DELAY							{ return DELAY             ;}
END_ANIM                { return END_ANIM          ;}
FORCE                   { return FORCE             ;}
IMPULSE						{ return IMPULSE           ;}
IMPULSE_X					{ return IMPULSE_X         ;}
IMPULSE_Y					{ return IMPULSE_Y         ;}
IMPULSE_Z					{ return IMPULSE_Z         ;}
OFFSET						{ return OFFSET            ;}
JUMP							{ return JUMP              ;}
NEW_ANIM						{ return NEW_ANIM          ;}
CODE							{ return CODE              ;}
CODE_1F						{ return CODE_1F           ;}
CODE_2F						{ return CODE_2F           ;}
CODE_3F						{ return CODE_3F           ;}
IF_SET						{ return IF_SET            ;}
IF_NOT_SET					{ return IF_NOT_SET        ;}
IF_EQ							{ return IF_EQ             ;}
ACTION						{ return ACTION            ;}
WAIT_HIT_GROUND			{ return WAIT_HIT_GROUND   ;}
WAIT_TIL_HOLD				{ return WAIT_TIL_HOLD     ;}
WAIT_FOR_PAUSE				{ return WAIT_FOR_PAUSE    ;}
WAIT_FOR_FRAME				{ return WAIT_FOR_FRAME    ;}
WAIT_FOR_END				{ return WAIT_FOR_END      ;}
SET_FLAG                { return SET_FLAG          ;}
CLR_FLAG						{ return CLR_FLAG          ;}
CREATE_PROC					{ return CREATE_PROC       ;}
KILL_PROC					{ return KILL_PROC         ;}
SOUND							{ return SOUND             ;}
ATTACH_OBJ					{ return ATTACH_OBJ        ;}
DETACH_OBJ					{ return DETACH_OBJ        ;}
SLAVE_POS					{ return SLAVE_POS         ;}
SLAVE_FORCE					{ return SLAVE_FORCE       ;}
SLAVE_IMPULSE				{ return SLAVE_IMPULSE     ;}
CHECK_DEAD					{ return CHECK_DEAD        ;}
COLLISION					{ return COLLISION         ;}
ROTATE						{ return ROTATE            ;}
WAIT_INTERP_DONE			{ return WAIT_INTERP_DONE  ;}
PLAY_RANDOM_SOUND			{ return PLAY_RANDOM_SOUND ;}
PLAY_SOUND					{ return PLAY_SOUND        ;}
PLAY_SOUND_VOLUME			{ return PLAY_SOUND_VOLUME ;}
COLLISION_DELAY			{ return	COLLISION_DELAY   ;}
PAUSE_MOTION				{ return PAUSE_MOTION      ;}
RESUME_MOTION				{ return RESUME_MOTION     ;}
CODE_1I						{ return CODE_1I           ;}
IF_LE                   { return IF_LE             ;}
IF_LT                   { return IF_LT             ;}
IF_NE                   { return IF_NE             ;}
IF_GE                   { return IF_GE             ;}
IF_GT                   { return IF_GT             ;}
GET_RANDOM					{ return GET_RANDOM        ;}
GET_FRAME					{ return GET_FRAME         ;}
GET_ACTION					{ return GET_ACTION        ;}
TEST_VALUE					{ return TEST_VALUE        ;}
TEST_FLAG					{ return TEST_FLAG         ;}
TEST_ACTION					{ return TEST_ACTION       ;}
TEST_ENDMOTION				{ return TEST_ENDMOTION    ;}
TEST_RANDOM					{ return TEST_RANDOM       ;}
TEST_FRAME					{ return TEST_FRAME        ;}
TEST_INTERPDONE         { return TEST_INTERPDONE   ;}
CASE                    { return CASE              ;}
RES_OP0A                { return RES_OP0A          ;}
RES_OP0B                { return RES_OP0B          ;}
RES_OP1F                { return RES_OP1F          ;}
RES_OP1I                { return RES_OP1I          ;}
TEST_ONGROUND           { return TEST_ONGROUND     ;}
PUSH                    { return PUSH              ;}
PUSHI                   { return PUSHI             ;}
PUSHF                   { return PUSHF             ;}
PUSHS                   { return PUSHS             ;}
PUSHA                   { return PUSHA             ;}
POP                     { return POP               ;}
SWAP                    { return SWAP              ;}
DUP                     { return DUP               ;}
LOAD                    { return LOAD              ;}
STORE                   { return STORE             ;}
ADD                     { return ADD               ;}
SUB                     { return SUB               ;}
MUL                     { return MUL               ;}
DIV                     { return DIV               ;}
MOD                     { return MOD               ;}
BIT_AND                 { return BIT_AND           ;}
BIT_OR                  { return BIT_OR            ;}
BIT_XOR                 { return BIT_XOR           ;}
BIT_NOT                 { return BIT_NOT           ;}
AND                     { return AND               ;}
OR                      { return OR                ;}
NOT                     { return NOT               ;}
INT                     { return INT               ;}
FLOAT                   { return FLOAT             ;}
CMP                     { return CMP               ;}
JNE                     { return JNE               ;}
JE                      { return JE                ;}
JL                      { return JL                ;}
JG                      { return JG                ;}
JLE                     { return JLE               ;}
JGE                     { return JGE               ;}
CALLC                   { return CALLC             ;}
CALL                    { return CALL              ;}
RETURN                  { return RETURN            ;}
AI_SCRIPT               { return AI_SCRIPT         ;}
ANIMRIP_AI_SCRIPT       { return ANIMRIP_AI_SCRIPT ;}

BNE                     { return BNE               ;}
BE                      { return BE                ;}
BL                      { return BL                ;}
BG                      { return BG                ;}
BLE                     { return BLE               ;}
BGE                     { return BGE               ;}

PUSHREG                 { return PUSHREG           ;}
POPREG                  { return POPREG            ;}

SLEEP                   { return SLEEP             ;}

START_MOTION2           { return START_MOTION2     ;}
KILL_MOTION2            { return KILL_MOTION2      ;}
TEST_ENDMOTION2         { return TEST_ENDMOTION2   ;}
TEST_FRAME2             { return TEST_FRAME2       ;}
WAIT_FOR_END2           { return WAIT_FOR_END2     ;}
WAIT_FOR_FRAME2         { return WAIT_FOR_FRAME2   ;}
INTERP_FRAME2           { return INTERP_FRAME2     ;}
WAIT_INTERP_DONE2       { return WAIT_INTERP_DONE2 ;}
PAUSE_MOTION2           { return PAUSE_MOTION2     ;}
RESUME_MOTION2          { return RESUME_MOTION2    ;}
GET_FRAME2					{ return GET_FRAME2        ;}

MIN                     { return MIN               ;}
MAX                     { return MAX               ;}
ABS                     { return ABS               ;}

INCR                    { return INCR              ;}
DECR                    { return DECR              ;}

LOAD\*                  { return LOAD_DEREF        ;}
LOAD_DEREF              { return LOAD_DEREF        ;}
DEREF                   { return DEREF             ;}
DEREF_INT               { return DEREF_INT         ;}
DEREF_FLOAT             { return DEREF_FLOAT       ;}

PUSH_CALL               { return PUSH_CALL         ;}
POP_CALL                { return POP_CALL          ;}
LOAD_CALL               { return LOAD_CALL         ;}
STORE_CALL              { return STORE_CALL        ;}

SET_ANIM_RATE           { return SET_ANIM_RATE     ;}
SET_ANIM_RATE2          { return SET_ANIM_RATE2    ;}

SET_ACTION              { return ACTION            ;}
SET_ACTION_LO           { return SET_ACTION_LO     ;}
GET_ACTION_LO           { return GET_ACTION_LO     ;}
SET_ACTION_HI           { return SET_ACTION_HI     ;}
GET_ACTION_HI           { return GET_ACTION_HI     ;}

SHL                     { return SHL               ;}
SHR                     { return SHR               ;}
SHRZ                    { return SHRZ              ;}

[a-zA-Z_][a-zA-Z0-9_@]+      {  yylval.pcString = strdup( yytext); return TEXT_; }

\/\*([\n\r\t ]|.)*\*\/     {  return CR_; }
\n                         {  return CR_; }

.                          {  return 0; }
%%
int zzwrap( void)
{
	return 1;
}

void *zz_get_current_buffer(void) {
	return YY_CURRENT_BUFFER;
}

#undef yylex
int yylex(YYSTYPE *lvalp, void *llocp) {
	int token;

	token = zzlex();
	*lvalp = yylval;
	return token;
}