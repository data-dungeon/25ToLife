%{
#include <string.h>

#include "k2a_gram.h"
#include "n_label.h"

//#define dbgLex(str) puts( (str))
#define dbgLex(str)

extern int				iLineCount;
extern NamedLabel		K2ALabels[];

int 						Lookup( NamedLabel *pLabels, unsigned char *pLabel);
unsigned char *		LooseQuotes( unsigned char *pText);

int						iLookup;

%}

DIG					[0-9]
DIG_H					[0-9a-fA-F]

EXP					[dDeE][-+]?{DIG}+

%%

{DIG}+ |
0x{DIG_H}+ 			{ 	yylval.iInteger = strtol( (char *) yytext, (char **) 0, 0);
			 				dbgLex( "L:Integer"); return TOK_INTEGER;
			 			}

-?{DIG}+"."{DIG}*({EXP})? |
-?{DIG}*"."{DIG}+({EXP})? |
-?{DIG}+{EXP}		{ 	yylval.fFloat = atof( (char *) yytext);
					  		dbgLex( "L:Float"); return TOK_FLOAT;
						}

\*[A-Z_0-9]+ 		{ 	iLookup = Lookup( K2ALabels, yytext + 1);
							if( iLookup > -1)
							{
								/*printf( "%s %d ", yytext + 1, iLookup);*/
								dbgLex( "L:Lookup1");
								return iLookup;
							}
						}

\".*\" 				{ yylval.pString = LooseQuotes( (unsigned char *) yytext);
							dbgLex( "L:QString"); return TOK_QSTRING;
						}

[a-zA-Z][a-zA-Z0-9]+	{
							iLookup = Lookup( K2ALabels, yytext);
							if( iLookup > -1)
							{
								dbgLex( "L:Lookup2");
								return iLookup;
							}
							else
							{
								yylval.pString = strdup( yytext);
								dbgLex( "L:String");
								return TOK_STRING;
							}
						}

\{						{ dbgLex( "LP"); return LP; }
\}						{ dbgLex( "RP"); return RP; }

[ \t]					;
\n						{ iLineCount++; }


%%
int yywrap( void)
{
	return 1;
}

/* -----------------01-06-99 10:56pm-----------------
 * Lookup( ) - Lookup Label
 * --------------------------------------------------*/
int Lookup( NamedLabel *pLabels, unsigned char *pThisLabel)
{
	while( pLabels->pLabel)
	{
		if( strcmp( pLabels->pLabel, pThisLabel) == 0)
			{
			return pLabels->iRetVal;
			}

		pLabels++;
	}

	return -1;

}

/* -----------------01-06-99 10:56pm-----------------
 * LooseQuotes( ) - Dup String w/o quotes
 * --------------------------------------------------*/
unsigned char *LooseQuotes( unsigned char *pText)
{
unsigned char *		pRetString;

int								iLen;

	iLen = strlen( pText) - 2;

	if( iLen)
	{
		pRetString = (unsigned char *) malloc( iLen + 1);

  	memcpy( pRetString, pText + 1, iLen);
		pRetString[iLen] = 0;
	}
	else
		pRetString = NULL;

	return pRetString;
}
