/****************************************************************************

 Module:
 hidkbcode.h

 Description:
 Logitech USB mouse/keyboard handler library file for Sony PlayStation(tm)2

 COPYRIGHT (C) 2002 LOGITECH, INC. ALL RIGHTS RESERVED.

 This program is a trade secret of LOGITECH, and it is not to be
 reproduced, published, disclosed to others, copied, adapted,
 distributed or displayed without the prior authorization of LOGITECH.

 Licensee agrees to attach or embed this notice on all copies of the
 program, including partial copies or modified versions thereof.

****************************************************************************/

#ifndef INCLUDE_HIDKBCODE_H
#define INCLUDE_HIDKBCODE_H

#define     HK_ErEOF            0x00
#define     HK_KEY69            0x00
#define     HK_KEY74            0x00
#define     HK_DUP_PA1          0x00
#define     HK_NO_KEY           0x00
#define     HK_ERR_ROLL_OVER    0x01
#define     HK_ERR_POST_FAIL    0x02
#define     HK_ERR_UNDEFINED    0x03


#define     HK_A                0x04
#define     HK_B                0x05
#define     HK_C                0x06
#define     HK_D                0x07
#define     HK_E                0x08
#define     HK_F                0x09
#define     HK_G                0x0a
#define     HK_H                0x0b
#define     HK_I                0x0c
#define     HK_J                0x0d
#define     HK_K                0x0e
#define     HK_L                0x0f
#define     HK_M                0x10
#define     HK_N                0x11
#define     HK_O                0x12
#define     HK_P                0x13
#define     HK_Q                0x14
#define     HK_R                0x15
#define     HK_S                0x16
#define     HK_T                0x17
#define     HK_U                0x18
#define     HK_V                0x19
#define     HK_W                0x1A
#define     HK_X                0x1B
#define     HK_Y                0x1C
#define     HK_Z                0x1D
#define     HK_1                0x1E
#define     HK_2                0x1F
#define     HK_3                0x20
#define     HK_4                0x21
#define     HK_5                0x22
#define     HK_6                0x23
#define     HK_7                0x24
#define     HK_8                0x25
#define     HK_9                0x26
#define     HK_0                0x27

#define     HK_ENTER            0x28
#define     HK_ESCAPE           0x29
#define     HK_BACKSPACE        0x2A
#define     HK_TAB              0x2B
#define     HK_SPACEBAR         0x2C

#define     HK_MINUS            0x2D
#define     HK_EQUAL            0x2E
#define     HK_SQ_OPEN_BRACKET  0x2F
#define     HK_SQ_CLOSE_BRACKET 0x30
#define     HK_BACK_SLASH       0x31
#define     HK_NON_US_HASH      0x32
#define     HK_SEMICOLEN        0x33
#define     HK_QUOTE            0x34
#define     HK_GRAVE_ACCENT     0x35
#define     HK_COMMA            0x36
#define     HK_DOT              0x37
#define     HK_FORWARD_SLASH    0x38

// All charectors upto FORWARD_SLASH has corresponding ASCII codes.

#define     HK_CAPS_LOCK        0x39
#define     HK_F1               0x3A
#define     HK_F2               0x3B
#define     HK_F3               0x3C
#define     HK_F4               0x3D
#define     HK_F5               0x3E
#define     HK_F6               0x3F
#define     HK_F7               0x40
#define     HK_F8               0x41
#define     HK_F9               0x42
#define     HK_F10              0x43
#define     HK_F11              0x44
#define     HK_F12              0x45
#define     HK_PRINT_SCR        0x46
#define     HK_SCROLL           0x47
#define     HK_PAUSE            0x48
#define     HK_INSERT           0x49
#define     HK_HOME             0x4A
#define     HK_PAGE_UP			0x4b

#define     HK_DELETE           0x4C
#define     HK_END              0x4D
#define     HK_PAGE_DOWN		0x4e

#define     HK_RIGHT_ARROW      0x4F
#define     HK_LEFT_ARROW       0x50
#define     HK_DOWN_ARROW       0x51
#define     HK_UP_ARROW         0x52

#define     HK_KP_NUMLOCK       0x53
#define     HK_KP_SLASH         0x54
#define     HK_KP_STAR          0x55
#define     HK_KP_MINUS         0x56
#define     HK_KP_PLUS          0x57
#define     HK_KP_ENTER         0x58
#define     HK_KP_1             0x59
#define     HK_KP_2             0x5A
#define     HK_KP_3             0x5B
#define     HK_KP_4             0x5C
#define     HK_KP_5             0x5D
#define     HK_KP_6             0x5E
#define     HK_KP_7             0x5F
#define     HK_KP_8             0x60
#define     HK_KP_9             0x61
#define     HK_KP_0             0x62
#define     HK_KP_DOT           0x63
#define     HK_NON_US_SLASH     0x64
#define     HK_KBD_APPLICATION  0x65
#define     HK_KBD_POWER        0x66
#define     HK_KP_EQUAL         0x67

#define     HK_F13              0x68
#define     HK_F14              0x69
#define     HK_F15              0x6A
#define     HK_F16              0x6B
#define     HK_F17              0x6C
#define     HK_F18              0x6D
#define     HK_F19              0x6E
#define     HK_F20              0x6F
#define     HK_F21              0x70
#define     HK_F22              0x71
#define     HK_F23              0x72
#define     HK_F24              0x73
#define     HK_EXECUTE          0x74
#define     HK_HELP             0x75
#define     HK_MENU             0x76
#define     HK_SELECT           0x77
#define     HK_STOP             0x78
#define     HK_AGAIN            0x79
#define     HK_UNDO             0x7a
#define     HK_CUT              0x7b
#define     HK_COPY             0x7c
#define     HK_PASTE            0x7d
#define     HK_FIND             0x7e
#define     HK_MUTE             0x7F
#define     HK_VOLUME_UP        0x80
#define     HK_VOLUME_DOWN      0x81
#define     HK_LOCKING_CAPSLOCK 0x82
#define     HK_LOCKING_NUMLOCK  0x83
#define     HK_LOCKING_SCROLL_LOCK   0x84
#define     HK_KP_COMMA         0x85
#define     HK_KP_EQUAL_SIGN    0x86
#define     HK_KBD_KANJI1       0x87
#define     HK_KBD_KANJI2       0x88
#define     HK_KBD_KANJI3       0x89
#define     HK_KBD_KANJI4       0x8a
#define     HK_KBD_KANJI5       0x8b
#define     HK_KBD_KANJI6       0x8c
#define     HK_KBD_KANJI7       0x8d
#define     HK_KBD_KANJI8       0x8e
#define     HK_KBD_KANJI9       0x8f
#define     HK_KBD_LANG1        0x90
#define     HK_KBD_LANG2        0x91
#define     HK_KBD_LANG3        0x92
#define     HK_KBD_LANG4        0x93
#define     HK_KBD_LANG5        0x94
#define     HK_KBD_LANG6        0x95
#define     HK_KBD_LANG7        0x96
#define     HK_KBD_LANG8        0x97
#define     HK_KBD_LANG9        0x98
#define     HK_ALTERNATE_ERASE  0x99
#define     HK_ATTN_SYSREQ      0x9a

#define     HK_CANCEL           0x9B
#define     HK_CLEAR            0x9c
#define     HK_PRIOR            0x9d
#define     HK_RETURN           0x9e
#define     HK_SEPERATOR        0x9f
#define     HK_OUT              0xa0
#define     HK_OPER             0xa1
#define     HK_CLEAR_AGAIN      0xa2
#define     HK_CrSEL            0xa3
#define     HK_ExSEL            0xa4

// codes a5 to df are reserved.
#define     HK_RESERVED_A5      0xa5
#define     HK_RESERVED_A6      0xa6
#define     HK_RESERVED_A7      0xa7
#define     HK_RESERVED_A8      0xa8
#define     HK_RESERVED_A9      0xa9
#define     HK_RESERVED_AA      0xaa
#define     HK_RESERVED_AB      0xab
#define     HK_RESERVED_AC      0xac
#define     HK_RESERVED_AD      0xad
#define     HK_RESERVED_AE      0xae
#define     HK_RESERVED_AF      0xaf

#define     HK_RESERVED_B0      0xb0
#define     HK_RESERVED_B1      0xb1
#define     HK_RESERVED_B2      0xb2
#define     HK_RESERVED_B3      0xb3
#define     HK_RESERVED_B4      0xb4
#define     HK_RESERVED_B5      0xb5
#define     HK_RESERVED_B6      0xb6
#define     HK_RESERVED_B7      0xb7
#define     HK_RESERVED_B8      0xb8
#define     HK_RESERVED_B9      0xb9
#define     HK_RESERVED_BA      0xba
#define     HK_RESERVED_BB      0xbb
#define     HK_RESERVED_BC      0xbc
#define     HK_RESERVED_BD      0xbd
#define     HK_RESERVED_BE      0xbe
#define     HK_RESERVED_BF      0xbf

#define     HK_RESERVED_C0      0xc0
#define     HK_RESERVED_C1      0xc1
#define     HK_RESERVED_C2      0xc2
#define     HK_RESERVED_C3      0xc3
#define     HK_RESERVED_C4      0xc4
#define     HK_RESERVED_C5      0xc5
#define     HK_RESERVED_C6      0xc6
#define     HK_RESERVED_C7      0xc7
#define     HK_RESERVED_C8      0xc8
#define     HK_RESERVED_C9      0xc9
#define     HK_RESERVED_CA      0xca
#define     HK_RESERVED_CB      0xcb
#define     HK_RESERVED_CC      0xcc
#define     HK_RESERVED_CD      0xcd
#define     HK_RESERVED_CE      0xce
#define     HK_RESERVED_CF      0xcf

#define     HK_RESERVED_C0      0xc0
#define     HK_RESERVED_C1      0xc1
#define     HK_RESERVED_C2      0xc2
#define     HK_RESERVED_C3      0xc3
#define     HK_RESERVED_C4      0xc4
#define     HK_RESERVED_C5      0xc5
#define     HK_RESERVED_C6      0xc6
#define     HK_RESERVED_C7      0xc7
#define     HK_RESERVED_C8      0xc8
#define     HK_RESERVED_C9      0xc9
#define     HK_RESERVED_CA      0xca
#define     HK_RESERVED_CB      0xcb
#define     HK_RESERVED_CC      0xcc
#define     HK_RESERVED_CD      0xcd
#define     HK_RESERVED_CE      0xce
#define     HK_RESERVED_CF      0xcf

#define     HK_RESERVED_D0      0xd0
#define     HK_RESERVED_D1      0xd1
#define     HK_RESERVED_D2      0xd2
#define     HK_RESERVED_D3      0xd3
#define     HK_RESERVED_D4      0xd4
#define     HK_RESERVED_D5      0xd5
#define     HK_RESERVED_D6      0xd6
#define     HK_RESERVED_D7      0xd7
#define     HK_RESERVED_D8      0xd8
#define     HK_RESERVED_D9      0xd9
#define     HK_RESERVED_DA      0xda
#define     HK_RESERVED_DB      0xdb
#define     HK_RESERVED_DC      0xdc
#define     HK_RESERVED_DD      0xdd
#define     HK_RESERVED_DE      0xde
#define     HK_RESERVED_DF      0xdf


// codes 0xa5 to 0xdf are reserved

#define     HK_LEFT_CTRL        0xE0
#define     HK_LEFT_SHIFT       0xE1
#define     HK_LEFT_ALT         0xE2
#define     HK_LEFT_GUI         0xE3
#define     HK_RIGHT_CTRL       0xE4
#define     HK_RIGHT_SHIFT      0xE5
#define     HK_RIGHT_ALT        0xE6
#define     HK_RIGHT_GUI        0xE7



#define     HIDCODE_IN_ASCII_RANGE_UPPER_LIMIT  HK_FORWARD_SLASH
#define     HIDCODE_IN_ASCII_RANGE_LOWER_LIMIT  HK_A

#define IsHidCodeAlphabet(x) (((x >= HK_A) && (x <= HK_Z)) ? 1 : 0 )

#endif

/*************************************************************************
  $Log: /PlayStation2/lgkbm/ee/include/hidkbcode.h $
 * 
 * 7     4/01/03 4:32p Ashinde
 * 
 * 6     4/01/03 3:05p Ashinde
 * changed the literal names.
 * 
 * 5     9/10/02 10:14a Ashinde
 * corrected the file header description.
 * 
 * 4     8/30/02 10:31a Ashinde
 * 
 * 3     8/14/02 6:26p Ashinde
 * Added micro IsHidCodeAlphabet
 * 
 * 2     8/12/02 12:29p Ashinde
 * 
 * 1     8/02/02 10:47a Ashinde
 * Initial Version.
**************************************************************************/
