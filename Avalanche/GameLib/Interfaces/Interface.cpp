/*
**
**  File:   Interface.cpp
**  Date:   February 4, 2004
**  By:     Bryant Collard
**  Desc:   A COM like interface class.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Interfaces/Interface.cpp $
**  $Revision: #1 $
**  $DateTime: 2004/02/05 17:14:02 $
**    $Author: Bryant_Collard $
**
*/

#include "Interfaces/InterfacesPCH.h"
#include "Interfaces/Interface.h"

uint CInterface::s_interfaceID = INTERFACE_ID_INVALID;
uint CInterface::s_lastID = INTERFACE_ID_INVALID;
