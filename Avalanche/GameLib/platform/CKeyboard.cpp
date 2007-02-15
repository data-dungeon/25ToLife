// ----------------------------------------------------------------------------
// File			-> CKeyboard.cpp
// Created		-> April 25, 2004
// Author		-> Dwain Skinner
// Description	-> Platform independent keyboard support
// ----------------------------------------------------------------------------

#include "platform/PlatformPCH.h"
#include "platform/CKeyboard.h"

CKeyboard::eKeyboardLanguage CKeyboard::m_language = CKeyboard::KEYBOARD_ENGLISH;


void CKeyboard::setLanguage( eKeyboardLanguage lang /* = KEYBOARD_ENGLISH  */ )
{
	m_language = lang;
}