#ifndef __T_TYPES_H
#define __T_TYPES_H

/*-----------------9/2/98 11:03AM-------------------
 * Types
 * --------------------------------------------------*/
typedef unsigned char	t_Byte;
typedef unsigned char	BYTE;

typedef unsigned short	t_Word;
typedef unsigned short	WORD;

typedef unsigned long	t_DWord;
typedef unsigned long	DWORD;

typedef unsigned char	t_bool;

typedef unsigned long	t_error;

typedef unsigned long	t_handle;

typedef unsigned long 	t_lflagbits;

typedef unsigned short	t_sflagbits;

#ifndef __cplusplus
#ifndef _BOOL_

typedef unsigned char	bool;

#define false				(bool) 0
#define true				(bool) 1
#define _BOOL_

#endif //_BOOL_
#endif //__cplusplus

/*-----------------9/2/98 11:03AM-------------------
 * Defines
 * --------------------------------------------------*/
#define FLAG_SET(flg,bit) (flg) |= (bit)

#define FLAG_UNSET(flg,bit) (flg) &= ~(bit);

#endif