///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "Layers/LayersPCH.h"
#include "Layers/Semaphore/Semaphore.h"

// Include the rest the implementation
#if defined(PS2)
#include "Semaphore_ps2.cpp"
#elif defined(GCN)
#include "Semaphore_gcn.cpp"
#elif defined (WIN32)
#include "Semaphore_win.cpp"
#endif



