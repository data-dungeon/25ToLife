////////////////////////////////////////////////////////////////////////////
//
// TriStripPch
//
// File to handle creation of precompiled header
//
////////////////////////////////////////////////////////////////////////////


#ifndef __TRI_STRIP_PCH__H
#define __TRI_STRIP_PCH__H

#include <windows.h>
#include <stdio.h>
#include <assert.h>

#include <tupperware/aggregate.h>
#include <tupperware/scalar.h>
#include <tupperware/list.h>
#include <tupperware/tupSort.h>
#include <tupperware/compareator.h>
#include <tupperware/keeper.h>

#include <tupper3d/scene.h>
#include <tupper3d/node.h>
#include <tupper3d/object.h>
#include <tupper3d/objectpool.h>
#include <tupper3d/trimesh.h>
#include <tupper3d/trimeshmap.h>
#include <tupper3d/trianglestrip.h>
#include <Tupper3d/material.h>
#include <Tupper3d/materialpool.h>
#include <Tupper3d/standardmaterial.h>
#include <Tupper3d/standardmaterialmap.h>
#include <Tupper3d/multisubobjectmaterial.h>
#include <Tupper3d/multisubobjectmaterialsubmaterial.h>
#include <Tupper3d/map.h>
#include <Tupper3d/mappool.h>
#include <Tupper3d/bitmapmap.h>
#include <Tupper3d/image.h>
#include <Tupper3d/imagepool.h>

#include <Tupper3d/tupvertex.h>

#include <TupperFilters/Libraries/Plugins/FilterPluginDesc.h>
#include <TupperFilters/Libraries/Plugins/FilterPlugin.h>
#include <TupperFilters/Libraries/Plugins/Options.h>
#include <TupperFilters/Libraries/Plugins/DataManager.h>
#include <TupperFilters/Libraries/Plugins/PluginError.h>
#include <TupperFilters/Libraries/Plugins/PluginLog.h>
#include <TupperFilters/Libraries/Plugins/BuildUVTransforms.h>
#include <TupperFilters/Libraries/Plugins/ApplyUVTransforms.h>
#include <TupperFilters/Libraries/Plugins/UVTransforms.h>
#include <TupperFilters/Libraries/Plugins/ImageManager.h>
#include <TupperFilters/Libraries/Plugins/AnnotateHelper.h>


#endif
