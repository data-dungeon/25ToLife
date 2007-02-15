%{
#include <stdio.h>
#include <stdlib.h>
#include "ai_gram.h"

YYSTYPE yylval;

%}




%%


[ \t]+              		;

-?[0-9]+"."[0-9]* |
"."[0-9]+           		{ yylval.fValue = atof( yytext); return FLOAT_; }

-?[0-9]+            		{ yylval.nValue = atoi( yytext); return INTEGER_; }

[a-zA-Z0-9_\!\@\./]+\: 	{ yylval.pcString = strdup( yytext); return LABEL_; }

AI_TABLE					{ return AI_TABLE		;}
DW								{ return DW					;}
JUMP							{ return JUMP				;}
MOVETO						{ return MOVETO				;}
ORIGIN						{ return ORIGIN				;}
DELTA						{ return DELTA				;}
\#include					{ return INCLUDE			;}
\#define						{ return DEFINE			;}

[a-zA-Z0-9_\!\@\./]+  	{ yylval.pcString = strdup( yytext); return TEXT_; }

\;.*\n						{ return CR_; }
\n                  		{ return CR_; }

.                   		{ return 0; }
%%
int yywrap( void)
{
	return 1;
}

