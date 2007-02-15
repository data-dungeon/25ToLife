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

#ifndef ORDINAL
#define ORDINAL

// These are extended ASCII codes
#define     EASCII_EURO                    0x80    // 
#define     EASCII_SINGLE_LOW9_QUOTE       0x82	   // 
#define     EASCII_SMALL_f_HOOK	           0x83	   // 
#define     EASCII_DOUBLE_LOW9_QUOTE       0x84
#define     EASCII_HORIZONTAL_ELLIPSIS     0x85
#define     EASCII_DAGGER                  0x86
#define     EASCII_DOUBLE_DAGGER           0x87
#define     EASCII_CIRCUMFLEX_ACCENT       0x88
#define     EASCII_PER_MILLE_SIGN          0x89
#define     EASCII_S_CARON                 0x8A
#define     EASCII_SINGLE_LP_ANGLE_QUOTE   0x8B
#define     EASCII_CAPITAL_LINGATURE_OE    0x8C
#define     EASCII_LEFT_SINGLE_QUOTE       0x91
#define     EASCII_RIGHT_SINGLE_QUOTE      0x92
#define     EASCII_LEFT_DOUBLE_QUOTE       0x93
#define     EASCII_RIGHT_DOUBLE_QUOTE      0x94
#define     EASCII_BULLET                  0x95
#define     EASCII_En_DASH                 0x96
#define     EASCII_Em_DASH                 0x97
#define     EASCII_small_tilda             0x98
#define     EASCII_TRADE_MARK              0x99
#define     EASCII_s_CARON                 0x9A
#define     EASCII_SINGLE_RP_ANGLE_QUOTE   0x9B
#define     EASCII_SMALL_LINGATURE_OE      0x9C
#define     EASCII_Y_DIAERESIS             0x9F

#define     EASCII_INVERTED_EXCLAMATION 0xA1    // �
#define     EASCII_CENT_SIGN       0xA2         // �
#define     EASCII_POUND_SIGN      0xA3         // �
#define     EASCII_CURRENCY_SIGN   0xA4         // �
#define     EASCII_YEN_SIGN        0xA5			// �
#define     EASCII_BROKEN_BAR      0xA6         // � 
#define     EASCII_SELECT_SIGN     0xA7         // � 
#define     EASCII_DIAERESIS       0xA8         // � 

#define		EASCII_COPYRIGHT_SIGN  0xA9         // �

#define     EASCII_FEMININE_ORDINAL 0xAA       // �

#define     EASCII_LP_2_ANG_QUOTE  0xAB			// �

#define     EASCII_NOT_SIGN        0xAC         // � 

#define     EASCII_SOFT_HYPHEN     0xAD         // �

#define     EASCII_REGISTERED_SIGN 0xAE         // �

#define     EASCII_MACRON          0xAF         // �

#define     EASCII_DEGREE          0xB0         // �

#define     EASCII_PLUS_MINUS      0xB1         // �

#define     EASCII_SUPERSCRIPT_2   0xB2         // �

#define     EASCII_SUPERSCRIPT_3   0xB3         // �

#define     EASCII_ACUTE_ACCENT    0xB4         // �

#define     EASCII_MICRO           0xB5         // �

#define     EASCII_PILCROW         0xB6         // �

#define     EASCII_MIDDLE_DOT      0xB7         // �

#define     EASCII_CEDILLA         0xB8         // �

#define     EASCII_SUPERSCRIPT_1   0xB9         // �

#define     EASCII_MASCULINE_ORDINAL 0xBA       // �

#define     EASCII_RP_2_ANG_QUOTE  0xBB			// �

#define     EASCII_ONE_QUARTER     0xBC         // �

#define     EASCII_ONE_HALF        0xBD         // �

#define     EASCII_THREE_QUARTERS  0xBE         // �

#define     EASCII_INVERTED_QUESTION_MARK 0xBF  // �

#define     EASCII_A_GRAVE         0xC0         // �

#define     EASCII_A_ACUTE         0xC1         // � 

#define     EASCII_A_CIRCUMFLEX    0xC2         // �

#define     EASCII_A_TILDA         0xC3         // �

#define     EASCII_A_DIAERESIS     0xC4         // �

#define     EASCII_A_RING          0xC5         // �

#define     EASCII_AE              0xC6         // �

#define     EASCII_C_CEDILLA       0xC7         // �

#define     EASCII_E_GRAVE         0xC8         // �

#define     EASCII_E_ACUTE         0xC9         // �

#define     EASCII_E_CIRCUMFLEX    0xCA         // �

#define     EASCII_E_DIAERESIS     0xCB         // �

#define     EASCII_I_GRAVE         0xCC         // �

#define     EASCII_I_ACUTE         0xCD         // �

#define     EASCII_I_CIRCUMFLEX    0xCE         // �

#define     EASCII_I_DIAERESIS     0xCF         // �

#define     EASCII_LETTER_ETH      0xD0         // �

#define     EASCII_N_TILDA         0xD1         // �

#define     EASCII_O_GRAVE         0xD2         // �

#define     EASCII_O_ACUTE         0xD3         // �

#define     EASCII_O_CIRCUMFLEX    0xD4         // �

#define     EASCII_O_TILDA         0xD5         // �

#define     EASCII_O_DIAERESIS     0xD6         // �

#define     EASCII_MULTIPLICATION  0xD7         // �

#define     EASCII_O_STROKE        0xD8         // �

#define     EASCII_U_GRAVE         0xD9         // �

#define     EASCII_U_ACUTE         0xDA         // �

#define     EASCII_U_CIRCUMFLEX    0xDB         // �

#define     EASCII_U_DIAERESIS     0xDC         // �

#define     EASCII_Y_ACCUTE        0xDD         // �

#define     EASCII_LETTER_THORN    0xDE         // �

#define     EASCII_SHARP_S         0xDF         // �

#define     EASCII_a_GRAVE         0xE0         // �

#define     EASCII_a_ACUTE         0xE1         // �

#define     EASCII_a_CIRCUMFLEX    0xE2         // �

#define     EASCII_a_TILDA         0xE3         // �

#define     EASCII_a_DIAERESIS     0xE4         // �

#define     EASCII_a_RING          0xE5         // �

#define     EASCII_ae              0xE6         // �

#define     EASCII_c_CEDILLA       0xE7         // �

#define     EASCII_e_GRAVE         0xE8         // �

#define     EASCII_e_ACUTE         0xE9         // �

#define     EASCII_e_CIRCUMFLEX    0xEA         // �

#define     EASCII_e_DIAERESIS     0xEB         // �

#define     EASCII_i_GRAVE         0xEC         // �

#define     EASCII_i_ACUTE         0xED         // �

#define     EASCII_i_CIRCUMFLEX    0xEE         // �

#define     EASCII_i_DIAERESIS     0xEF         // �

#define     EASCII_LETTER_eth      0xF0         // �

#define     EASCII_n_TILDA         0xF1         // �

#define     EASCII_o_GRAVE         0xF2         // �

#define     EASCII_o_ACUTE         0xF3         // �

#define     EASCII_o_CIRCUMFLEX    0xF4         // �

#define     EASCII_o_TILDA         0xF5         // �

#define     EASCII_o_DIAERESIS     0xF6         // �

#define     EASCII_DIVISION        0xF7         // �

#define     EASCII_o_STROKE        0xF8         // �

#define     EASCII_u_GRAVE         0xF9         // �

#define     EASCII_u_ACUTE         0xFA         // �

#define     EASCII_u_CIRCUMFLEX    0xFB         // �

#define     EASCII_u_DIAERESIS     0xFC         // �

#define     EASCII_y_ACCUTE        0xFD         // �

#define     EASCII_LETTER_thorn    0xFE         // �

#define     EASCII_y_DIAERESIS     0xFF         // �

#endif
