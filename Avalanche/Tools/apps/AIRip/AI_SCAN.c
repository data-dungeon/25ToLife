/* c:\mks\bin\lexe -l -o ai_scan.c .\ai_scan.l */
#define YYNEWLINE 10
#define INITIAL 0
#define yy_endst 65
#define yy_nxtmax 499
#define YY_LA_SIZE 9

static unsigned short yy_la_act[] = {
 0, 16, 16, 3, 13, 16, 13, 16, 13, 16, 13, 16, 13, 16, 13, 16,
 13, 16, 13, 16, 16, 16, 15, 16, 14, 12, 11, 13, 13, 4, 13, 13,
 13, 9, 13, 13, 13, 13, 13, 8, 13, 13, 13, 7, 13, 13, 6, 13,
 13, 13, 10, 13, 13, 13, 13, 13, 13, 13, 5, 13, 2, 13, 3, 13,
 1, 13, 1, 13, 3, 1, 1, 0, 0
};

static unsigned char yy_look[] = {
 0
};

static short yy_final[] = {
 0, 0, 2, 3, 6, 8, 10, 12, 14, 16, 18, 20, 21, 22, 23, 24,
 24, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 27, 28,
 29, 30, 31, 32, 33, 35, 36, 37, 38, 39, 41, 42, 43, 45, 46, 48,
 49, 50, 52, 53, 54, 55, 56, 57, 58, 60, 62, 64, 66, 68, 69, 70,
 71, 72
};
#ifndef yy_state_t
#define yy_state_t unsigned char
#endif

static yy_state_t yy_begin[] = {
 0, 0, 0
};

static yy_state_t yy_next[] = {
 14, 14, 14, 14, 14, 14, 14, 14, 14, 1, 13, 14, 14, 14, 14, 14,
 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
 1, 5, 14, 11, 14, 14, 14, 14, 14, 14, 14, 14, 14, 2, 4, 5,
 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 14, 12, 14, 14, 14, 14,
 5, 6, 5, 5, 7, 5, 5, 5, 5, 5, 8, 5, 5, 9, 5, 10,
 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 14, 14, 14, 14, 5,
 14, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 14, 14, 14, 14, 14,
 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 15, 15, 15, 15, 15,
 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
 18, 19, 20, 21, 22, 17, 23, 24, 25, 26, 27, 28, 29, 30, 33, 35,
 36, 30, 30, 37, 30, 30, 30, 30, 30, 38, 30, 30, 30, 30, 30, 30,
 30, 30, 30, 30, 30, 30, 32, 30, 41, 40, 30, 30, 30, 30, 30, 30,
 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 31, 30,
 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 42, 30, 30, 30, 30, 30,
 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
 30, 30, 30, 30, 30, 30, 30, 34, 39, 43, 44, 45, 30, 47, 30, 30,
 49, 48, 30, 30, 53, 30, 50, 30, 30, 30, 54, 52, 55, 46, 56, 30,
 30, 30, 30, 30, 51, 30, 62, 65, 65, 65, 30, 30, 57, 57, 57, 57,
 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57,
 59, 65, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 64, 65, 30, 65,
 65, 65, 65, 65, 65, 65, 65, 65, 30, 60, 60, 60, 60, 60, 60, 60,
 60, 60, 60, 64, 30, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 59,
 65, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 30, 61, 61, 61, 61,
 61, 61, 61, 61, 61, 61, 65, 30, 63, 63, 63, 63, 63, 63, 63, 63,
 63, 63, 65, 30, 0
};

static yy_state_t yy_check[] = {
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
 11, 18, 19, 20, 21, 11, 22, 17, 24, 25, 26, 27, 28, 10, 31, 34,
 35, 36, 30, 9, 35, 37, 9, 31, 34, 37, 10, 10, 10, 10, 10, 10,
 10, 10, 10, 10, 10, 10, 10, 39, 40, 39, 41, 40, 10, 10, 10, 10,
 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
 10, 10, 10, 10, 10, 10, 10, 8, 44, 46, 8, 10, 49, 10, 10, 10,
 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
 10, 10, 10, 10, 10, 10, 10, 33, 38, 42, 43, 7, 43, 45, 42, 47,
 48, 47, 33, 45, 52, 38, 6, 50, 7, 51, 53, 51, 54, 7, 55, 6,
 56, 48, 54, 5, 50, 52, 61, ~0, ~0, ~0, 53, 55, 4, 4, 4, 4,
 4, 4, 4, 4, 4, 4, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57,
 3, ~0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, ~0, 4, ~0,
 ~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, 57, 59, 59, 59, 59, 59, 59, 59,
 59, 59, 59, 1, 3, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 58,
 ~0, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 59, 2, 2, 2, 2,
 2, 2, 2, 2, 2, 2, ~0, 60, 62, 62, 62, 62, 62, 62, 62, 62,
 62, 62, ~0, 58, 0
};

static yy_state_t yy_default[] = {
 65, 65, 65, 10, 10, 10, 10, 10, 10, 10, 65, 65, 65, 65, 65, 12,
 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 10, 10,
 65, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 2, 65, 62,
 1, 0
};

static short yy_base[] = {
 0, 419, 428, 370, 348, 305, 301, 294, 245, 196, 236, 156, 128, 500, 500, 500,
 500, 153, 156, 156, 154, 150, 161, 500, 165, 157, 149, 167, 167, 500, 192, 197,
 500, 288, 198, 194, 191, 195, 291, 213, 217, 216, 284, 282, 246, 289, 247, 285,
 303, 250, 293, 295, 307, 312, 304, 313, 302, 358, 417, 393, 405, 344, 440, 500,
 500, 500
};


/*
 * Copyright 1988, 1992 by Mortice Kern Systems Inc.  All rights reserved.
 * All rights reserved.
 *
 * $Header: /u/rd/src/lex/RCS/yylex.c,v 1.44 1993/05/26 14:47:37 ignac Exp $
 *
 */
#include <stdlib.h>
#include <stdio.h>
#if	__STDC__
#define YY_ARGS(args)	args
#else
#define YY_ARGS(args)	()
#endif

#ifdef LEX_WINDOWS
#include <windows.h>

/*
 * define, if not already defined
 * the flag YYEXIT, which will allow
 * graceful exits from yylex()
 * without resorting to calling exit();
 */

#ifndef YYEXIT
#define YYEXIT	1
#endif

/*
 * the following is the handle to the current
 * instance of a windows program. The user
 * program calling yylex must supply this!
 */

extern HANDLE hInst;	

#endif	/* LEX_WINDOWS */

/*
 * Define m_textmsg() to an appropriate function for internationalized messages
 * or custom processing.
 */
#ifndef I18N
#define	m_textmsg(id, str, cls)	(str)
#else /*I18N*/
extern	char* m_textmsg YY_ARGS((int id, const char* str, char* cls));
#endif/*I18N*/

/*
 * Include string.h to get definition of memmove() and size_t.
 * If you do not have string.h or it does not declare memmove
 * or size_t, you will have to declare them here.
 */
#include <string.h>
/* Uncomment next line if memmove() is not declared in string.h */
/*extern char * memmove();*/
/* Uncomment next line if size_t is not available in stdio.h or string.h */
/*typedef unsigned size_t;*/
/* Drop this when LATTICE provides memmove */
#ifdef LATTICE
#define memmove	memcopy
#endif

/*
 * YY_STATIC determines the scope of variables and functions
 * declared by the lex scanner. It must be set with a -DYY_STATIC
 * option to the compiler (it cannot be defined in the lex program).
 */
#ifdef	YY_STATIC
/* define all variables as static to allow more than one lex scanner */
#define	YY_DECL	static
#else
/* define all variables as global to allow other modules to access them */
#define	YY_DECL	
#endif

/*
 * You can redefine yygetc. For YACC Tracing, compile this code
 * with -DYYTRACE to get input from yt_getc
 */
#ifdef YYTRACE
extern int	yt_getc YY_ARGS((void));
#define yygetc()	yt_getc()
#else
#define	yygetc()	getc(yyin) 	/* yylex input source */
#endif

/*
 * the following can be redefined by the user.
 */
#ifdef YYEXIT
#define	YY_FATAL(msg)	{ fprintf(yyout, "yylex: %s\n", msg); yyLexFatal = 1; }
#else /* YYEXIT */
#define	YY_FATAL(msg)	{ fprintf(stderr, "yylex: %s\n", msg); exit(1); }
#endif /* YYEXIT */

#define	ECHO		fputs(yytext, yyout)
#define	output(c)	putc((c), yyout) /* yylex sink for unmatched chars */
#define	YY_INTERACTIVE	1		/* save micro-seconds if 0 */
#define	YYLMAX		256		/* token and pushback buffer size */

/*
 * If %array is used (or defaulted), yytext[] contains the token.
 * If %pointer is used, yytext is a pointer to yy_tbuf[].
 */
YY_DECL char	yytext[YYLMAX+1];

#define	BEGIN		yy_start =
#define	REJECT		goto yy_reject
#define	NLSTATE		(yy_lastc = YYNEWLINE)
#define	YY_INIT \
	(yy_start = yyleng = yy_end = 0, yy_lastc = YYNEWLINE)
#define	yymore()	goto yy_more
#define	yyless(n)	if ((n) < 0 || (n) > yy_end) ; \
			else { YY_SCANNER; yyleng = (n); YY_USER; }

YY_DECL	void	yy_reset YY_ARGS((void));
YY_DECL	int	input	YY_ARGS((void));
YY_DECL	int	unput	YY_ARGS((int c));

/* functions defined in libl.lib */
extern	int	yywrap	YY_ARGS((void));
extern	void	yyerror	YY_ARGS((char *fmt, ...));
extern	void	yycomment	YY_ARGS((char *term));
extern	int	yymapch	YY_ARGS((int delim, int escape));


#include <stdio.h>
#include <stdlib.h>
#include "ai_gram.h"

YYSTYPE yylval;




#ifdef	YY_DEBUG
#undef	YY_DEBUG
#define	YY_DEBUG(fmt, a1, a2)	fprintf(stderr, fmt, a1, a2)
#else
#define	YY_DEBUG(fmt, a1, a2)
#endif

/*
 * The declaration for the lex scanner can be changed by
 * redefining YYLEX or YYDECL. This must be done if you have
 * more than one scanner in a program.
 */
#ifndef	YYLEX
#define	YYLEX yylex			/* name of lex scanner */
#endif

#ifndef YYDECL
#define	YYDECL	int YYLEX YY_ARGS((void))	/* declaration for lex scanner */
#endif

/*
 * stdin and stdout may not neccessarily be constants.
 * If stdin and stdout are constant, and you want to save a few cycles, then
 * #define YY_STATIC_STDIO 1 in this file or on the commandline when
 * compiling this file
 */
#ifndef YY_STATIC_STDIO
#define YY_STATIC_STDIO	0
#endif

#if YY_STATIC_STDIO
YY_DECL	FILE   *yyin = stdin;
YY_DECL	FILE   *yyout = stdout;
#else
YY_DECL	FILE   *yyin = (FILE *)0;
YY_DECL	FILE   *yyout = (FILE *)0;
#endif
YY_DECL	int	yylineno = 1;		/* line number */

/* yy_sbuf[0:yyleng-1] contains the states corresponding to yytext.
 * yytext[0:yyleng-1] contains the current token.
 * yytext[yyleng:yy_end-1] contains pushed-back characters.
 * When the user action routine is active,
 * yy_save contains yytext[yyleng], which is set to '\0'.
 * Things are different when YY_PRESERVE is defined. 
 */
static	yy_state_t yy_sbuf [YYLMAX+1];	/* state buffer */
static	int	yy_end = 0;		/* end of pushback */
static	int	yy_start = 0;		/* start state */
static	int	yy_lastc = YYNEWLINE;	/* previous char */
YY_DECL	int	yyleng = 0;		/* yytext token length */
#ifdef YYEXIT
static	int yyLexFatal;
#endif /* YYEXIT */

#ifndef YY_PRESERVE	/* the efficient default push-back scheme */

static	char yy_save;	/* saved yytext[yyleng] */

#define	YY_USER	{ /* set up yytext for user */ \
		yy_save = yytext[yyleng]; \
		yytext[yyleng] = 0; \
	}
#define	YY_SCANNER { /* set up yytext for scanner */ \
		yytext[yyleng] = yy_save; \
	}

#else		/* not-so efficient push-back for yytext mungers */

static	char yy_save [YYLMAX];
static	char *yy_push = yy_save+YYLMAX;

#define	YY_USER { \
		size_t n = yy_end - yyleng; \
		yy_push = yy_save+YYLMAX - n; \
		if (n > 0) \
			memmove(yy_push, yytext+yyleng, n); \
		yytext[yyleng] = 0; \
	}
#define	YY_SCANNER { \
		size_t n = yy_save+YYLMAX - yy_push; \
		if (n > 0) \
			memmove(yytext+yyleng, yy_push, n); \
		yy_end = yyleng + n; \
	}

#endif


#ifdef LEX_WINDOWS

/*
 * When using the windows features of lex,
 * it is necessary to load in the resources being
 * used, and when done with them, the resources must
 * be freed up, otherwise we have a windows app that
 * is not following the rules. Thus, to make yylex()
 * behave in a windows environment, create a new
 * yylex() which will call the original yylex() as
 * another function call. Observe ...
 */

/*
 * The actual lex scanner (usually yylex(void)).
 * NOTE: you should invoke yy_init() if you are calling yylex()
 * with new input; otherwise old lookaside will get in your way
 * and yylex() will die horribly.
 */
static int win_yylex();			/* prototype for windows yylex handler */

YYDECL {
	int wReturnValue;
	HANDLE hRes_table;
	unsigned short *old_yy_la_act;	/* remember previous pointer values */
	short *old_yy_final;
	yy_state_t *old_yy_begin;
	yy_state_t *old_yy_next;
	yy_state_t *old_yy_check;
	yy_state_t *old_yy_default;
	short *old_yy_base;

	/*
	 * the following code will load the required
	 * resources for a Windows based parser.
	 */

	hRes_table = LoadResource (hInst,
		FindResource (hInst, "UD_RES_yyLEX", "yyLEXTBL"));
	
	/*
	 * return an error code if any
	 * of the resources did not load
	 */

	if (hRes_table == NULL)
		return (0);
	
	/*
	 * the following code will lock the resources
	 * into fixed memory locations for the scanner
	 * (and remember previous pointer locations)
	 */

	old_yy_la_act = yy_la_act;
	old_yy_final = yy_final;
	old_yy_begin = yy_begin;
	old_yy_next = yy_next;
	old_yy_check = yy_check;
	old_yy_default = yy_default;
	old_yy_base = yy_base;

	yy_la_act = (unsigned short *)LockResource (hRes_table);
	yy_final = (short *)(yy_la_act + Sizeof_yy_la_act);
	yy_begin = (yy_state_t *)(yy_final + Sizeof_yy_final);
	yy_next = (yy_state_t *)(yy_begin + Sizeof_yy_begin);
	yy_check = (yy_state_t *)(yy_next + Sizeof_yy_next);
	yy_default = (yy_state_t *)(yy_check + Sizeof_yy_check);
	yy_base = (yy_state_t *)(yy_default + Sizeof_yy_default);


	/*
	 * call the standard yylex() code
	 */

	wReturnValue = win_yylex();

	/*
	 * unlock the resources
	 */

	UnlockResource (hRes_table);

	/*
	 * and now free the resource
	 */

	FreeResource (hRes_table);

	/*
	 * restore previously saved pointers
	 */

	yy_la_act = old_yy_la_act;
	yy_final = old_yy_final;
	yy_begin = old_yy_begin;
	yy_next = old_yy_next;
	yy_check = old_yy_check;
	yy_default = old_yy_default;
	yy_base = old_yy_base;

	return (wReturnValue);
}	/* end function */

static int win_yylex() {

#else /* LEX_WINDOWS */

/*
 * The actual lex scanner (usually yylex(void)).
 * NOTE: you should invoke yy_init() if you are calling yylex()
 * with new input; otherwise old lookaside will get in your way
 * and yylex() will die horribly.
 */
YYDECL {

#endif /* LEX_WINDOWS */

	register int c, i, yybase;
	unsigned	yyst;	/* state */
	int yyfmin, yyfmax;	/* yy_la_act indices of final states */
	int yyoldi, yyoleng;	/* base i, yyleng before look-ahead */
	int yyeof;		/* 1 if eof has already been read */




#if !YY_STATIC_STDIO
	if (yyin == (FILE *)0)
		yyin = stdin;
	if (yyout == (FILE *)0)
		yyout = stdout;
#endif

#ifdef YYEXIT
	yyLexFatal = 0;
#endif /* YYEXIT */

	yyeof = 0;
	i = yyleng;
	YY_SCANNER;

  yy_again:
	yyleng = i;
	/* determine previous char. */
	if (i > 0)
		yy_lastc = yytext[i-1];
	/* scan previously accepted token adjusting yylineno */
	while (i > 0)
		if (yytext[--i] == YYNEWLINE)
			yylineno++;
	/* adjust pushback */
	yy_end -= yyleng;
	memmove(yytext, yytext+yyleng, (size_t) yy_end);
	i = 0;

  yy_contin:
	yyoldi = i;

	/* run the state machine until it jams */
	yyst = yy_begin[yy_start + (yy_lastc == YYNEWLINE)];
	yy_sbuf[i] = (yy_state_t) yyst;
	do {
		YY_DEBUG(m_textmsg(1547, "<state %d, i = %d>\n", "I num1 num2"), yyst, i);
		if (i >= YYLMAX) {
			YY_FATAL(m_textmsg(1548, "Token buffer overflow", "E"));
#ifdef YYEXIT
			if (yyLexFatal)
				return -2;
#endif /* YYEXIT */
		}	/* endif */

		/* get input char */
		if (i < yy_end)
			c = yytext[i];		/* get pushback char */
		else if (!yyeof && (c = yygetc()) != EOF) {
			yy_end = i+1;
			yytext[i] = (char) c;
		} else /* c == EOF */ {
			c = EOF;		/* just to make sure... */
			if (i == yyoldi) {	/* no token */
				yyeof = 0;
				if (yywrap())
					return 0;
				else
					goto yy_again;
			} else {
				yyeof = 1;	/* don't re-read EOF */
				break;
			}
		}
		YY_DEBUG(m_textmsg(1549, "<input %d = 0x%02x>\n", "I num hexnum"), c, c);

		/* look up next state */
		while ((yybase = yy_base[yyst]+(unsigned char)c) > yy_nxtmax
		    || yy_check[yybase] != (yy_state_t) yyst) {
			if (yyst == yy_endst)
				goto yy_jammed;
			yyst = yy_default[yyst];
		}
		yyst = yy_next[yybase];
	  yy_jammed: ;
	  yy_sbuf[++i] = (yy_state_t) yyst;
	} while (!(yyst == yy_endst || YY_INTERACTIVE && yy_base[yyst] > yy_nxtmax && yy_default[yyst] == yy_endst));
	YY_DEBUG(m_textmsg(1550, "<stopped %d, i = %d>\n", "I num1 num2"), yyst, i);
	if (yyst != yy_endst)
		++i;

  yy_search:
	/* search backward for a final state */
	while (--i > yyoldi) {
		yyst = yy_sbuf[i];
		if ((yyfmin = yy_final[yyst]) < (yyfmax = yy_final[yyst+1]))
			goto yy_found;	/* found final state(s) */
	}
	/* no match, default action */
	i = yyoldi + 1;
	output(yytext[yyoldi]);
	goto yy_again;

  yy_found:
	YY_DEBUG(m_textmsg(1551, "<final state %d, i = %d>\n", "I num1 num2"), yyst, i);
	yyoleng = i;		/* save length for REJECT */
	
	/* pushback look-ahead RHS */
	if ((c = (int)(yy_la_act[yyfmin]>>9) - 1) >= 0) { /* trailing context? */
		unsigned char *bv = yy_look + c*YY_LA_SIZE;
		static unsigned char bits [8] = {
			1<<0, 1<<1, 1<<2, 1<<3, 1<<4, 1<<5, 1<<6, 1<<7
		};
		while (1) {
			if (--i < yyoldi) {	/* no / */
				i = yyoleng;
				break;
			}
			yyst = yy_sbuf[i];
			if (bv[(unsigned)yyst/8] & bits[(unsigned)yyst%8])
				break;
		}
	}

	/* perform action */
	yyleng = i;
	YY_USER;
	switch (yy_la_act[yyfmin] & 0777) {
	case 0:
	;
	break;
	case 1:
	case 2:
	{ yylval.fValue = atof( yytext); return FLOAT_; }
	break;
	case 3:
	{ yylval.nValue = atoi( yytext); return INTEGER_; }
	break;
	case 4:
	{ yylval.pcString = strdup( yytext); return LABEL_; }
	break;
	case 5:
	{ return AI_TABLE		;}
	break;
	case 6:
	{ return DW					;}
	break;
	case 7:
	{ return JUMP				;}
	break;
	case 8:
	{ return MOVETO				;}
	break;
	case 9:
	{ return ORIGIN				;}
	break;
	case 10:
	{ return DELTA				;}
	break;
	case 11:
	{ return INCLUDE			;}
	break;
	case 12:
	{ return DEFINE			;}
	break;
	case 13:
	{ yylval.pcString = strdup( yytext); return TEXT_; }
	break;
	case 14:
	{ return CR_; }
	break;
	case 15:
	{ return CR_; }
	break;
	case 16:
	{ return 0; }
	break;


	}
	YY_SCANNER;
	i = yyleng;
	goto yy_again;			/* action fell though */

  yy_reject:
	YY_SCANNER;
	i = yyoleng;			/* restore original yytext */
	if (++yyfmin < yyfmax)
		goto yy_found;		/* another final state, same length */
	else
		goto yy_search;		/* try shorter yytext */

  yy_more:
	YY_SCANNER;
	i = yyleng;
	if (i > 0)
		yy_lastc = yytext[i-1];
	goto yy_contin;
}
/*
 * Safely switch input stream underneath LEX
 */
typedef struct yy_save_block_tag {
	FILE	* oldfp;
	int	oldline;
	int	oldend;
	int	oldstart;
	int	oldlastc;
	int	oldleng;
	char	savetext[YYLMAX+1];
	yy_state_t	savestate[YYLMAX+1];
} YY_SAVED;

YY_SAVED *
yySaveScan(fp)
FILE * fp;
{
	YY_SAVED * p;

	if ((p = (YY_SAVED *) malloc(sizeof(*p))) == NULL)
		return p;

	p->oldfp = yyin;
	p->oldline = yylineno;
	p->oldend = yy_end;
	p->oldstart = yy_start;
	p->oldlastc = yy_lastc;
	p->oldleng = yyleng;
	(void) memcpy(p->savetext, yytext, sizeof yytext);
	(void) memcpy((char *) p->savestate, (char *) yy_sbuf,
		sizeof yy_sbuf);

	yyin = fp;
	yylineno = 1;
	YY_INIT;

	return p;
}
/*f
 * Restore previous LEX state
 */
void
yyRestoreScan(p)
YY_SAVED * p;
{
	if (p == NULL)
		return;
	yyin = p->oldfp;
	yylineno = p->oldline;
	yy_end = p->oldend;
	yy_start = p->oldstart;
	yy_lastc = p->oldlastc;
	yyleng = p->oldleng;

	(void) memcpy(yytext, p->savetext, sizeof yytext);
	(void) memcpy((char *) yy_sbuf, (char *) p->savestate,
		sizeof yy_sbuf);
	free(p);
}
/*
 * User-callable re-initialization of yylex()
 */
void
yy_reset()
{
	YY_INIT;
	yylineno = 1;		/* line number */
}
/* get input char with pushback */
YY_DECL int
input()
{
	int c;
#ifndef YY_PRESERVE
	if (yy_end > yyleng) {
		yy_end--;
		memmove(yytext+yyleng, yytext+yyleng+1,
			(size_t) (yy_end-yyleng));
		c = yy_save;
		YY_USER;
#else
	if (yy_push < yy_save+YYLMAX) {
		c = *yy_push++;
#endif
	} else
		c = yygetc();
	yy_lastc = c;
	if (c == YYNEWLINE)
		yylineno++;
	return c;
}

/*f
 * pushback char
 */
YY_DECL int
unput(c)
	int c;
{
#ifndef YY_PRESERVE
	if (yy_end >= YYLMAX) {
		YY_FATAL(m_textmsg(1552, "Push-back buffer overflow", "E"));
	} else {
		if (yy_end > yyleng) {
			yytext[yyleng] = yy_save;
			memmove(yytext+yyleng+1, yytext+yyleng,
				(size_t) (yy_end-yyleng));
			yytext[yyleng] = 0;
		}
		yy_end++;
		yy_save = (char) c;
#else
	if (yy_push <= yy_save) {
		YY_FATAL(m_textmsg(1552, "Push-back buffer overflow", "E"));
	} else {
		*--yy_push = c;
#endif
		if (c == YYNEWLINE)
			yylineno--;
	}	/* endif */
	return c;
}

int yywrap( void)
{
	return 1;
}

