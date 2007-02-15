/****************************************************************************

 Module:
 asciicode.h

 Description:
 Logitech USB mouse/keyboard handler library file for Sony PlayStation(tm)2

 COPYRIGHT (C) 2002 LOGITECH, INC. ALL RIGHTS RESERVED.

 This program is a trade secret of LOGITECH, and it is not to be
 reproduced, published, disclosed to others, copied, adapted,
 distributed or displayed without the prior authorization of LOGITECH.

 Licensee agrees to attach or embed this notice on all copies of the
 program, including partial copies or modified versions thereof.

****************************************************************************/

#ifndef INCLUDE_ASCIICODE_H
#define INCLUDE_ASCIICODE_H

#define     ASCII_BACKSPACE         0x08        // 
#define     ASCII_TAB               0x09        // 
#define     ASCII_ENTER             0x0a        //
#define     ASCII_ESCAPE            0x1B        // 
#define     ASCII_SPACE             0x20        // 
#define     ASCII_EXCLAMATION       0x21        // !
#define     ASCII_DOUBLE_QUOTE      0x22        // "
#define     ASCII_HASH              0x23        // #
#define     ASCII_DOLLAR            0x24        // $
#define     ASCII_PERCENTAGE        0x25        // %
#define     ASCII_AMPHERSAND        0x26        // &
#define     ASCII_APOSTROPHE        0x27        // '
#define     ASCII_LEFT_PARENTHESIS  0x28        // (
#define     ASCII_RIGHT_PARENTHESIS 0x29        // )
#define     ASCII_STAR              0x2a        // *
#define     ASCII_PLUS              0x2b        // +
#define     ASCII_COMMA             0x2c        // ,
#define     ASCII_MINUS             0x2d        // -
#define     ASCII_DOT               0x2e        // .
#define     ASCII_FORWARD_SLASH     0x2f        // /
#define     ASCII_0                 0x30        // 0
#define     ASCII_1                 0x31        // 1
#define     ASCII_2                 0x32        // 2
#define     ASCII_3                 0x33        // 3
#define     ASCII_4                 0x34        // 4
#define     ASCII_5                 0x35        // 5
#define     ASCII_6                 0x36        // 6
#define     ASCII_7                 0x37        // 7
#define     ASCII_8                 0x38        // 8
#define     ASCII_9                 0x39        // 9
#define     ASCII_COLEN             0x3a        // :
#define     ASCII_SEMICOLEN         0x3b        // ;
#define     ASCII_ANGLE_BRACE_OPEN  0x3c        // <
#define     ASCII_EQUAL             0x3d        // =
#define     ASCII_ANGLE_BRACE_CLOSE 0x3e        // >
#define     ASCII_QUESTION_MARK     0x3f        // ?
#define     ASCII_AT                0x40        // @
#define     ASCII_A                 0x41        // A
#define     ASCII_B                 0x42        // B
#define     ASCII_C                 0x43        // C
#define     ASCII_D                 0x44        // D
#define     ASCII_E                 0x45        // E
#define     ASCII_F                 0x46        // F
#define     ASCII_G                 0x47        // G
#define     ASCII_H                 0x48        // H
#define     ASCII_I                 0x49        // I
#define     ASCII_J                 0x4a        // J
#define     ASCII_K                 0x4b        // K
#define     ASCII_L                 0x4c        // L
#define     ASCII_M                 0x4d        // M
#define     ASCII_N                 0x4e        // N
#define     ASCII_O                 0x4f        // O
#define     ASCII_P                 0x50        // P
#define     ASCII_Q                 0x51        // Q
#define     ASCII_R                 0x52        // R
#define     ASCII_S                 0x53        // S
#define     ASCII_T                 0x54        // T
#define     ASCII_U                 0x55        // U
#define     ASCII_V                 0x56        // V
#define     ASCII_W                 0x57        // W
#define     ASCII_X                 0x58        // X
#define     ASCII_Y                 0x59        // Y
#define     ASCII_Z                 0x5a        // Z
#define     ASCII_SQ_BRACE_OPEN     0x5b        // [
#define     ASCII_BACK_SLASH        0x5c        // \ 
#define     ASCII_SQ_BRACE_CLOSE    0x5d        // ]
#define     ASCII_CIRCUM_ACCENT     0x5e        // ^
#define     ASCII_UNDERSCORE        0x5f        // _
#define     ASCII_GRAVE_ACCENT      0x60        // `
#define     ASCII_a                 0x61        // a
#define     ASCII_b                 0x62        // b
#define     ASCII_c                 0x63        // c
#define     ASCII_d                 0x64        // d
#define     ASCII_e                 0x65        // e
#define     ASCII_f                 0x66        // f
#define     ASCII_g                 0x67        // g
#define     ASCII_h                 0x68        // h
#define     ASCII_i                 0x69        // i
#define     ASCII_j                 0x6a        // j
#define     ASCII_k                 0x6b        // k
#define     ASCII_l                 0x6c        // l
#define     ASCII_m                 0x6d        // m
#define     ASCII_n                 0x6e        // n
#define     ASCII_o                 0x6f        // o
#define     ASCII_p                 0x70        // p
#define     ASCII_q                 0x71        // q
#define     ASCII_r                 0x72        // r
#define     ASCII_s                 0x73        // s
#define     ASCII_t                 0x74        // t
#define     ASCII_u                 0x75        // u
#define     ASCII_v                 0x76        // v
#define     ASCII_w                 0x77        // w
#define     ASCII_x                 0x78        // x
#define     ASCII_y                 0x79        // y
#define     ASCII_z                 0x7a        // z
#define     ASCII_CURLY_BRACE_OPEN  0x7b        // {
#define     ASCII_VERTICAL_LINE     0x7c        // |
#define     ASCII_CURLY_BRACE_CLOSE 0x7d        // }
#define     ASCII_TILDA             0x7e        // ~
#define     ASCII_DELETE            0x7f        // 
#define     ASCII_KP_DEL_DOT        0x7f        // 






#endif

/*************************************************************************
  $Log: /PlayStation2/lgkbm/ee/sample/common/asciicode.h $
 * 
 * 7     3/08/04 3:07p Ashinde
 * Updated the ASCII and EASCII codes and the maptable file.
 * 
 * 6     4/01/03 3:07p Ashinde
 * changed the ascii mapping code for ENTER to 0x0a from 0x0d and
 * corrected the code for TAB to 0x09
 * 
 * 5     11/21/02 6:18p Ashinde
 * removed the extended ascii codes from the file.
 * 
 * 4     9/10/02 10:14a Ashinde
 * corrected the file header description.
 * 
 * 3     8/30/02 10:31a Ashinde
 * added extended ASCII codes 
 * 
 * 2     8/12/02 12:28p Ashinde
 * 
 * 1     8/02/02 10:47a Ashinde
 * Initial Version.
**************************************************************************/
