

#ifdef PS2
#include "PS2/PlatformImage.h"
#endif

#ifdef DIRECTX
#ifdef _XBOX
#include "XBox/PlatformImage.h"
#else
#include "DirectX/PlatformImage.h"
#endif
#endif


#ifndef __PLATFORM_IMAGE_H__
#error Please define the PlatformImage class for the platform.
#endif

