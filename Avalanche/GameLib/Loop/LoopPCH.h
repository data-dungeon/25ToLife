/*
**
**  File:   LoopPCH.h
**  Date:   April 7, 2004
**  By:     Bryant Collard
**  Desc:   Loop pre-compiled header.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Loop/LoopPCH.h $
**  $Revision: #4 $
**  $DateTime: 2005/04/26 17:49:42 $
**    $Author: Bryant_Collard $
**
*/

#ifndef __LOOP_PCH_H
#define __LOOP_PCH_H

#if defined(WIN32) || defined(CW_PCH) || ( defined(PS2) && defined(SN) )

// system includes
#include <string.h>
#include <stdio.h>
#include <new.h>

// engine includes
#include "platform/BaseType.h"
#include "Math/MathClass.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Layers/Memory.h"
#include "Layers/Assert/Assert.h"
#include "Layers/Debug.h"

// gamelib includes
#include "GameHelper/Message.h"
#include "GameHelper/loadsom.h"
#include "GameHelper/DebugOutput.h"
#include "platform/cfocus.h"
#include "EngineHelper/timer.h"
#include "camera/director.h"

#endif // defined(WIN32) || defined(CW_PCH)
#endif // __LOOP_PCH_H
