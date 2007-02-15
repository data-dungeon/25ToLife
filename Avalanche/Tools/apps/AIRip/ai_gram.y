%{
extern int yyparse(void);
#include "airip.h"
#include <stdio.h>
%}





%union
{
	int	nValue;
	float	fValue;
	char	*pcString;
}

%token  <pcString> LABEL_
%token  <pcString> TEXT_
%token  <nValue> INTEGER_
%token  <fValue> FLOAT_

%type   <fValue> floatexp

%token	CR_
%token	AI_TABLE
%token	DW
%token	JUMP
%token	MOVETO
%token	ORIGIN
%token	DELTA
%token	INCLUDE
%token	DEFINE

%%

program:
	| statementlist
	;

statementlist:
	statementlist statement
	| statement
	;

statement:
	 command CR_  {iLineCount++;}
	/*| command EF_*/
	| label CR_  {iLineCount++;}
	| CR_         {iLineCount++;}
	;

command:
	|	DW TEXT_ {ProcessDW ( $2);}
	|	AI_TABLE TEXT_ {ProcessAiTable ( $2);}
	|	JUMP TEXT_ {ProcessJump ( $2);}
	|	MOVETO floatexp floatexp floatexp {ProcessMoveto ( $2, $3, $4);}
	|	ORIGIN floatexp floatexp floatexp {ProcessOrigin ( $2, $3, $4);}
	|	DELTA floatexp {ProcessDelta ( $2);}
	|	MOVETO floatexp floatexp floatexp {ProcessMoveto ( $2, $3, $4);}
	;

label:
	LABEL_ {ProcessLabel ( $1);}
	;

floatexp:
	FLOAT_             { $$ = $1; }
  	| INTEGER_         { $$ = (float) $1; }
  	;

%%
int			iLineCount = 0;
char			szFileName[256];

int yyerror( unsigned char *pStr)
{
	if( (char *)yychar == NULL )
		printf( "ERROR: Unexpected EOF.  (Missing CR at end of file?)\n" );
	else
	{
		char buf[1024];
		printf( "ERROR in file %s on line %d: \n    %s\n", szFileName, iLineCount, pStr);
		if( yyExpandName( yysmap[yyps[0]], 0, buf, 1024 ) )
			printf( "  %s\n", buf );
	}
	return 0;
}