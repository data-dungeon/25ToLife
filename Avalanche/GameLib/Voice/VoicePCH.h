// ---------------------------------------------------------------------------
// File			-> VoicePCH.cpp
// Author		-> Dwain Skinner
// Created		-> December 18, 2003
// Description	-> Voice precompiled header
//
//  Copyright (c) 2003, Avalanche Software Inc.
//  All Rights Reserved.
// ---------------------------------------------------------------------------

#pragma once

#ifndef VOICE_PCH_H
#define VOICE_PCH_H
#if defined( WIN32 ) || defined( CW_PCH ) || ( defined(PS2) && defined(SN) )

#if defined(PS2) && defined(SN)
#ifndef PS2
#define PS2
#endif
#endif

// System includes
#include <stdio.h>

#if defined (_XBOX)
#include "Math/MathUndeprecate.h"
#include <xtl.h>
#include "Math/MathDeprecate.h"
#endif

// Engine includes
#include "Layers/Assert/Assert.h"
#include "Layers/Debug.h"

// Gamelib includes
#include "platform/BaseType.h"
#include "platform/ctroller.h"
#include "platform/systemi.h"
#include "Voice/CVoiceDevice.h"
#include "Network/Session.h"

#ifdef PS2
#include <eekernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <sifcmd.h>
#include <sifrpc.h>
#include <sifdev.h>
#include <libnet.h>
#include <libmrpc.h>
#include <libinsck.h>
#include <errno.h>
#include <eeregs.h>
#include <eetypes.h>
#include <libgraph.h>
#include <libdev.h>
#include <eekernel.h>
#include <libpad.h>
#include "platform/PS2/PS2app.h"
#include "Voice/Platform/PS2/CVoiceCodec.h"
#include "Voice/Platform/PS2/CVoiceOverIP.h"
#include "Voice/Platform/PS2/CPlatformVoiceDevice.h"
#include "Voice/CVoiceOverIPRoot.h"
#endif

#ifdef GCN
#include "Voice/CVoiceOverIPRoot.h"
#include "voice/platform/gcn/cvoiceoverip.h"
#include "voice/platform/gcn/cplatformvoicedevice.h"
#endif

#if defined (_XBOX)
#include "Voice/Platform/XBOX/CVoiceCodec.h"
#include "Voice/CVoiceOverIPRoot.h"
#include "Voice/Platform/XBOX/CVoiceOverIP.h"
#include "Voice/Platform/XBOX/CPlatformVoiceDevice.h"
#endif

#if defined (WIN32) && !defined (_XBOX)
#include "platform/Win32/Winapp.h"
#include "Voice/Platform/Win32/CVoiceCodec.h"
#include "Voice/CVoiceOverIPRoot.h"
#include "Voice/Platform/Win32/CVoiceOverIP.h"
#include "Voice/Platform/Win32/CPlatformVoiceDevice.h"
#endif


#endif // defined(WIN32) || defined(CW_PCH) || ( defined(PS2) && defined(SN) )
#endif // VOICE_PCH_H
