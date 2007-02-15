/*
**
**  File:   NetworkPCH.h
**  Date:   November 17, 2003
**  By:     Bryant Collard
**  Desc:   Network precompiled header.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetworkPCH.h $
**  $Revision: #7 $
**  $DateTime: 2005/07/11 20:33:34 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef NETWORK_PCH_H
#define NETWORK_PCH_H
#if defined(WIN32) || defined(CW_PCH) || ( defined(PS2) && defined(SN) )

// System includes
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

// Engine includes
#include "Layers/Assert/Assert.h"
#include "Layers/Memory.h"

// Gamelib includes
#include "stage/cactor.h"
#include "stage/protagonist.h"
#include "GameHelper/Message.h"

#ifdef GCN
#include "platform/GCN/GCNMWPort.h"
#endif

#endif // defined(WIN32) || defined(CW_PCH) || ( defined(PS2) && defined(SN) )
#endif // NETWORK_PCH_H
