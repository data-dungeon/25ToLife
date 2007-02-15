///////////////////////////////////////////////////////////////////////////////
// Camera system rewrite, by jeff
///////////////////////////////////////////////////////////////////////////////
#include "CameraPCH.h"

/* For SN, we include all the source files from the projects here */
/* this is done to reduce compile times under SN (which currently */
/* does not support precompiled headers) */
#if defined(PS2) && defined(SN)
#include "camera/defproj.cpp"
#include "camera/defviewport.cpp"
#include "camera/camera.cpp"
#include "camera/camgroup.cpp"
#include "camera/director.cpp"
#include "camera/camscene.cpp"

#include "camera/controlcam/controlcam.cpp"
#include "camera/util/flextarg.cpp"
#include "camera/orbitcam/orbitcam.cpp"

#endif // defined(PS2) && defined(SN)
