///////////////////////////////////////////////////////////////////////////////
// The thread implementation
///////////////////////////////////////////////////////////////////////////////
#include "Layers/LayersPCH.h"
#include "Layers/OSThreading/OSThread.h"

// Include the implementation
#if defined(PS2)
#include "OSThread_ps2.cpp"
#elif defined(GCN)
#include "OSThread_gcn.cpp"
#elif defined (WIN32)
#include "OSThread_win.cpp"
#endif

