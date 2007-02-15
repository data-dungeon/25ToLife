// NavigatePCH.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef NAVIGATEPCH_H
#define NAVIGATEPCH_H

// TODO: reference additional headers your program requires here
#include "platform/BaseType.h"
#include "collide/cube.h"
#include "collide/aabb.h"
#include "collide/tritest.h"
#include "container/Darray.h"
#include "container/dllist.h"
#include "container/pool.h"
#include "container/pqueue.h"
#include "container/skiplist.h"
#include "container/pool.h"
#include "EngineHelper/Singleton.h"
#include "GameHelper/loadsom.h"
#include "linearAlgebra/semiseg.h"
#include "Math/Vector.h"
#include "Script/ScriptVM.h"
#include "Script/ScriptSystem.h"

#include "Navigate/NavMesh.h"
#include "Navigate/NavTree.h"
#include "Navigate/NavAgents.h"
#include "Navigate/NavManager.h"
#include "Navigate/NavIterators.h"
#include "Navigate/NavPath.h"
#include "Navigate/OneWayPath.h"
#include "Navigate/BackAndForthPath.h"
#include "Navigate/CircuitPath.h"

#endif