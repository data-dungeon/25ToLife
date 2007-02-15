typedef union {
	int	nValue;
	float	fValue;
	char	*pcString;
} YYSTYPE;
#define LABEL_	257
#define TEXT_	258
#define INTEGER_	259
#define FLOAT_	260
#define CR_	261
#define AI_TABLE	262
#define DW	263
#define JUMP	264
#define MOVETO	265
#define ORIGIN	266
#define DELTA	267
#define INCLUDE	268
#define DEFINE	269
extern YYSTYPE yylval;
