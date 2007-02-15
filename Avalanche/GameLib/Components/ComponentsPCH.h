//============================================================================
//=
//= ComponentsPCH.h - Precompiled headers
//=
//============================================================================

#ifndef __COMPONENTS_PCH_H
#define __COMPONENTS_PCH_H

#if defined(WIN32) || defined(CW_PCH) || ( defined(PS2) && defined(SN) )

// system

#include <string.h>
#include <stdio.h>
#include <new.h>

// engine

#include "platform/BaseType.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Layers/Memory.h"
#include "Layers/Assert/Assert.h"
#include "Layers/Debug.h"

// gamelib

#include "collide/collsys.h"
#include "collide/Event.h"
#include "CollideHelper/EventMiner.h"
#include "stage/protagonist.h"
#include "stage/CCompActor.h"
#include "GameHelper/Message.h"
#include "GameHelper/Sticky.h"

#endif // defined(WIN32) || defined(CW_PCH)
#endif // __COMPONENTS_PCH_H
