/*---------------------------------------------------------------------------*
  Project:  Dolphin OS ROM font API
  File:     OSFont.h

  Copyright 2001 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: /Avalanche/tools/TupperFilters/ExportGCN/Dolphin/os/OSFont.h $
 * 
 * 1     6/20/03 4:20p Todd Blackburn
    
    3     8/05/02 18:04 Shiki
    Added const keywords to relevant function prototypes.

    2     01/04/25 14:25 Shiki
    Modified OSGetFontWidth() interface.

    1     01/04/19 13:27 Shiki
    Initial check-in.
  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef __OSFONT_H__
#define __OSFONT_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct OSFontHeader
{
    u16 fontType;       // font type
    u16 firstChar;      // the first character code defined in the font
    u16 lastChar;       // the last character code defined in the font
    u16 invalChar;      // the font code to be substituted for invalid characters
    u16 ascent;         // the ascent (units above the base line) of characters
    u16 descent;        // the descent (units below the base line) of characters
    u16 width;          // the width of the widest character
    u16 leading;        // the leading (space) between rows

    u16 cellWidth;      // the cell (a single character) width in a sheet
    u16 cellHeight;     // the cell (a single character) height in a sheet
    u32 sheetSize;      // the size of a sheet in bytes
    u16 sheetFormat;    // the texture format of a sheet (GX_TF_*)
    u16 sheetColumn;    // the number of characters in a raw
    u16 sheetRow;       // the number of lines in a sheet
    u16 sheetWidth;     // the sheet width (texture width)
    //32B

    u16 sheetHeight;    // the sheet height (texture height)
    u16 widthTable;     // offset to character width table (const)

    u32 sheetImage;     // offset to sheet image
    u32 sheetFullSize;  // the size of all sheets

    u8  c0;             // font color table
    u8  c1;
    u8  c2;
    u8  c3;
    // 48B
} OSFontHeader;

//
// Common functions
//

// For OSGetFontEncode()
#define OS_FONT_ENCODE_ANSI     0u
#define OS_FONT_ENCODE_SJIS     1u

u16   OSGetFontEncode ( void );

char* OSGetFontWidth  ( const char* string, s32* width );

//
// High level functions
//

// For OSInitFont(fontData)
#define OS_FONT_SIZE_ANSI       (288 + 131072)      // 9 sheets
#define OS_FONT_SIZE_SJIS       (3840 + 1179648)    // 1 sheet

BOOL  OSInitFont      ( OSFontHeader* fontData );
char* OSGetFontTexture( const char* string, void** image, s32 *x, s32 *y, s32* width );

//
// Low level functions
//

// For OSLoadFont(*, temp)
#define OS_FONT_ROM_SIZE_ANSI   12288   // 0x03000
#define OS_FONT_ROM_SIZE_SJIS   315392  // 0x4D000

// For OSLoadFont(fontData, *)
#define OS_FONT_DATA_SIZE_ANSI  65824
#define OS_FONT_DATA_SIZE_SJIS  593636

u32   OSLoadFont      ( OSFontHeader* fontData, void* temp );
char* OSGetFontTexel  ( const char* string, void* image, s32 pos, s32 stride, s32* width );

#ifdef __cplusplus
}
#endif

#endif // __OSFONT_H__
