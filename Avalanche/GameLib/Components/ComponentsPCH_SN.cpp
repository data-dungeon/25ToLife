#ifdef CW
#define CW_PCH	// enable contents of .h file, see comments in .h file
#endif //CW
#include "ComponentsPCH.h"

/* For SN, we include all the source files from the projects here */
/* this is done to reduce compile times under SN (which currently */
/* does not support precompiled headers) */
#if defined(PS2) && defined(SN)

#include "GroundFollower.cpp"
#include "KineControl.cpp"
#include "PathFollower.cpp"
#include "PathFollowerCam.cpp"
#include "BasicMover.cpp"
#include "CollisionInfo.cpp"
#include "ComponentCreator.cpp"
#include "ComponentList.cpp"
#include "ComponentsScriptFunc.cpp"
#include "InteractionMonitor.cpp"
#include "Property.cpp"
#include "RigidBody.cpp"
#include "RigidBodyTable.cpp"
#include "Teleport.cpp"
#include "ViewComponent.cpp"
#include "InventoryList.cpp"

#include "weapons/aimsight.cpp"
#include "weapons/aimsim.cpp"
#include "weapons/projectile.cpp"
#include "weapons/projlib.cpp"
#include "weapons/sightlib.cpp"
#include "weapons/weapinv.cpp"
#include "weapons/weaplib.cpp"
#include "weapons/weapon.cpp"

#endif // defined(PS2) && defined(SN)
