/*
**
**  File:   InterfacesPCH.h
**  Date:   February 4, 2004
**  By:     Bryant Collard
**  Desc:   Interfaces pre-compiled header.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Interfaces/InterfacesPCH.h $
**  $Revision: #3 $
**  $DateTime: 2005/03/29 17:16:52 $
**    $Author: Rob $
**
*/

#ifndef __INTERFACES_PCH_H
#define __INTERFACES_PCH_H

#if defined(WIN32) || defined(CW_PCH) || ( defined(PS2) && defined(SN) )

/* system includes */

#include <string.h>
#include <stdio.h>
#include <new.h>

/* engine includes */

#include "platform/BaseType.h"
#include "Math/MathClass.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Layers/Memory.h"
#include "Layers/Assert/Assert.h"
#include "Layers/Debug.h"

/* gamelib includes */

#endif // defined(WIN32) || defined(CW_PCH)
#endif // __INTERFACES_PCH_H
