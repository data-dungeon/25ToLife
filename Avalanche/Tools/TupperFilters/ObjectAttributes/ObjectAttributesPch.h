////////////////////////////////////////////////////////////////////////////
//
// ObjectAttributesPch
//
// File to handle creation of precompiled header
//
////////////////////////////////////////////////////////////////////////////


#ifndef __OBJECT_ATTRIBUTES_PCH__H
#define __OBJECT_ATTRIBUTES_PCH__H

#include <windows.h>
#include <stdio.h>
#include <assert.h>

// Tupperware
#include "tupperware/aggregate.h"
#include "tupperware/scalar.h"
#include "tupperware/list.h"
#include "tupperware/loadsave.h"
#include "tupperware/comment.h"
#include "tupperware/pool.h"
#include "tupperware/keeper.h"

// TupperAttrib
#include "tupperattrib/AccessRoot.h"
#include "tupperattrib/AccessInstance.h"
#include "tupperattrib/AccessPath.h"
#include "tupperattrib/ConfigRoot.h"
#include "tupperattrib/KeyValue.h"
#include "tupperattrib/KeyValueContext.h"

// Tupper3d
#include "tupper3d/scene.h"
#include "tupper3d/node.h"
#include "tupper3d/nodepool.h"
#include "tupper3d/objectpool.h"
#include "Tupper3d/object.h"
#include "tupper3d/beziershape.h"
#include "tupper3d/splineshape.h"
#include <Tupper3d/transform.h>
#include <Tupper3d/staticmatrix.h>

// Plugins
#include "TupperFilters/Libraries/Plugins/FilterPluginDesc.h"
#include "TupperFilters/Libraries/Plugins/FilterPlugin.h"
#include "TupperFilters/Libraries/Plugins/Options.h"
#include "TupperFilters/Libraries/Plugins/DataManager.h"
#include "TupperFilters/Libraries/Plugins/PluginError.h"
#include "TupperFilters/Libraries/Plugins/PluginLog.h"
#include "TupperFilters/Libraries/Plugins/FilePath.h"
#include "TupperFilters/Libraries/Plugins/FilePathArray.h"
#include "TupperFilters/Libraries/Plugins/AnnotateHelper.h"
#include "TupperFilters/Libraries/Plugins/CRC.h"

// DBL game headers
#include <Platform/BaseType.h>
#include <Math/Matrix.h>
#include <Game/Database/Instance.h>

#endif
