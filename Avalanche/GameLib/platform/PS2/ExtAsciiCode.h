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

#define     EASCII_INVERTED_EXCLAMATION 0xA1    // ¡
#define     EASCII_CENT_SIGN       0xA2         // ¢
#define     EASCII_POUND_SIGN      0xA3         // £
#define     EASCII_CURRENCY_SIGN   0xA4         // ¤
#define     EASCII_YEN_SIGN        0xA5			// ¥
#define     EASCII_BROKEN_BAR      0xA6         // ¦ 
#define     EASCII_SELECT_SIGN     0xA7         // § 
#define     EASCII_DIAERESIS       0xA8         // ¨ 

#define		EASCII_COPYRIGHT_SIGN  0xA9         // ©

#define     EASCII_FEMININE_ORDINAL 0xAA       // ª

#define     EASCII_LP_2_ANG_QUOTE  0xAB			// «

#define     EASCII_NOT_SIGN        0xAC         // ¬ 

#define     EASCII_SOFT_HYPHEN     0xAD         // ­

#define     EASCII_REGISTERED_SIGN 0xAE         // ®

#define     EASCII_MACRON          0xAF         // ¯

#define     EASCII_DEGREE          0xB0         // °

#define     EASCII_PLUS_MINUS      0xB1         // ±

#define     EASCII_SUPERSCRIPT_2   0xB2         // ²

#define     EASCII_SUPERSCRIPT_3   0xB3         // ³

#define     EASCII_ACUTE_ACCENT    0xB4         // ´

#define     EASCII_MICRO           0xB5         // µ

#define     EASCII_PILCROW         0xB6         // ¶

#define     EASCII_MIDDLE_DOT      0xB7         // ·

#define     EASCII_CEDILLA         0xB8         // ¸

#define     EASCII_SUPERSCRIPT_1   0xB9         // ¹

#define     EASCII_MASCULINE_ORDINAL 0xBA       // º

#define     EASCII_RP_2_ANG_QUOTE  0xBB			// »

#define     EASCII_ONE_QUARTER     0xBC         // ¼

#define     EASCII_ONE_HALF        0xBD         // ½

#define     EASCII_THREE_QUARTERS  0xBE         // ¾

#define     EASCII_INVERTED_QUESTION_MARK 0xBF  // ¿

#define     EASCII_A_GRAVE         0xC0         // À

#define     EASCII_A_ACUTE         0xC1         // Á 

#define     EASCII_A_CIRCUMFLEX    0xC2         // Â

#define     EASCII_A_TILDA         0xC3         // Ã

#define     EASCII_A_DIAERESIS     0xC4         // Ä

#define     EASCII_A_RING          0xC5         // Å

#define     EASCII_AE              0xC6         // Æ

#define     EASCII_C_CEDILLA       0xC7         // Ç

#define     EASCII_E_GRAVE         0xC8         // È

#define     EASCII_E_ACUTE         0xC9         // É

#define     EASCII_E_CIRCUMFLEX    0xCA         // Ê

#define     EASCII_E_DIAERESIS     0xCB         // Ë

#define     EASCII_I_GRAVE         0xCC         // Ì

#define     EASCII_I_ACUTE         0xCD         // Í

#define     EASCII_I_CIRCUMFLEX    0xCE         // Î

#define     EASCII_I_DIAERESIS     0xCF         // Ï

#define     EASCII_LETTER_ETH      0xD0         // Ð

#define     EASCII_N_TILDA         0xD1         // Ñ

#define     EASCII_O_GRAVE         0xD2         // Ò

#define     EASCII_O_ACUTE         0xD3         // Ó

#define     EASCII_O_CIRCUMFLEX    0xD4         // Ô

#define     EASCII_O_TILDA         0xD5         // Õ

#define     EASCII_O_DIAERESIS     0xD6         // Ö

#define     EASCII_MULTIPLICATION  0xD7         // ×

#define     EASCII_O_STROKE        0xD8         // Ø

#define     EASCII_U_GRAVE         0xD9         // Ù

#define     EASCII_U_ACUTE         0xDA         // Ú

#define     EASCII_U_CIRCUMFLEX    0xDB         // Û

#define     EASCII_U_DIAERESIS     0xDC         // Ü

#define     EASCII_Y_ACCUTE        0xDD         // Ý

#define     EASCII_LETTER_THORN    0xDE         // Þ

#define     EASCII_SHARP_S         0xDF         // ß

#define     EASCII_a_GRAVE         0xE0         // à

#define     EASCII_a_ACUTE         0xE1         // á

#define     EASCII_a_CIRCUMFLEX    0xE2         // â

#define     EASCII_a_TILDA         0xE3         // ã

#define     EASCII_a_DIAERESIS     0xE4         // ä

#define     EASCII_a_RING          0xE5         // å

#define     EASCII_ae              0xE6         // æ

#define     EASCII_c_CEDILLA       0xE7         // ç

#define     EASCII_e_GRAVE         0xE8         // è

#define     EASCII_e_ACUTE         0xE9         // é

#define     EASCII_e_CIRCUMFLEX    0xEA         // ê

#define     EASCII_e_DIAERESIS     0xEB         // ë

#define     EASCII_i_GRAVE         0xEC         // ì

#define     EASCII_i_ACUTE         0xED         // í

#define     EASCII_i_CIRCUMFLEX    0xEE         // î

#define     EASCII_i_DIAERESIS     0xEF         // ï

#define     EASCII_LETTER_eth      0xF0         // ð

#define     EASCII_n_TILDA         0xF1         // ñ

#define     EASCII_o_GRAVE         0xF2         // ò

#define     EASCII_o_ACUTE         0xF3         // ó

#define     EASCII_o_CIRCUMFLEX    0xF4         // ô

#define     EASCII_o_TILDA         0xF5         // õ

#define     EASCII_o_DIAERESIS     0xF6         // ö

#define     EASCII_DIVISION        0xF7         // ÷

#define     EASCII_o_STROKE        0xF8         // ø

#define     EASCII_u_GRAVE         0xF9         // ù

#define     EASCII_u_ACUTE         0xFA         // ú

#define     EASCII_u_CIRCUMFLEX    0xFB         // û

#define     EASCII_u_DIAERESIS     0xFC         // ü

#define     EASCII_y_ACCUTE        0xFD         // ý

#define     EASCII_LETTER_thorn    0xFE         // þ

#define     EASCII_y_DIAERESIS     0xFF         // ÿ

#endif
