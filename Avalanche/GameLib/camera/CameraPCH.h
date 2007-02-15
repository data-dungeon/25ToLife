///////////////////////////////////////////////////////////////////////////////
// Camera system rewrite, by jeff
///////////////////////////////////////////////////////////////////////////////
#ifndef __CAMERA_PCH_H
#define __CAMERA_PCH_H

#include <string.h>
#include <stdio.h>

#if defined (_XBOX)
#include "Math/MathUndeprecate.h"
#include <xtl.h>
#include "Math/MathDeprecate.h"
#endif

#include "Math/MathClass.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Layers/Debug.h"
#include "WatchClient/WatchClient.h"
#include "collide/collsys.h"
#include "EngineHelper/timer.h"
#include "Display/Common/RenderState.h"
#include "Display/Common/DisplayState.h"
#include "Game/Database/DBFile.h"
#include "Game/Misc/FastVars.h"

// The camera library
#include "camera/world2view.h"
#include "camera/projection.h"
#include "camera/defproj.h"
#include "camera/viewport.h"
#include "camera/defviewport.h"
#include "camera/camera.h"
#include "camera/camscene.h"
#include "camera/camgroup.h"
#include "camera/director.h"

// The sample implementations
#include "camera/util/flextarg.h"
#include "camera/orbitcam/orbitcam.h"
#include "camera/targetcam/targetcam.h"
#include "camera/pathcam/pathcam.h"
#include "camera/generalcam/generalcam.h"
#include "camera/controlcam/controlcam.h"
#include "camera/collidablecontrolcam/collidablecontrolcam.h"
#include "camera/StationaryCam/StationaryCam.h"
#include "camera/StationaryCam/StationaryCamDbl.h"

#endif // __CAMERA_PCH_H
