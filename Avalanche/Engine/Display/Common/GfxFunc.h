#ifndef __D_GFXFUNC_H
#define __D_GFXFUNC_H

/******************* includes *****************************/

/* engine includes */

#include "platform/BaseType.h"				// low-level include
#include "Layers/LayersStruct.h"	// low-level include

#if defined(__cplusplus)
extern "C" {
#endif

/******************* forward declarations ****************/

/******************* defines *****************************/

/******************* macros ******************************/

/******************* structures **************************/

struct ts_Viewport;

/******************* global variables ********************/

/******************* global prototypes *******************/

//yuch-- this will be pc-specific implementation
#ifdef WIN32
bool	gfxCreateWindow(void);
bool	gfxOnCreate( HWND hwnd);
bool	gfxOnCreated( HWND hwnd);
void	gfxOnDestroy( void);
void	gfxOnResize( short sWidth, short sHeight);
void  gfxWindowNormal( );
void  gfxWindowMaximize( );
void  gfxToggleWindowMaximize( );

#ifdef DIRECTX
void  gfxDisableDevice( void );
void	gfxEnableDevice( void );

bool  gfxDeviceAvailable( void );
#endif
#endif //WIN32

void	gfxDebug(void);
void gfxSetFrameBuffer(u32 u32VRAMAddr, u32 u32Width, u32 u32Height, u32 u32Format);
void	gfxClearFrameBuffer(u32 u32ClearMode, ts_bRGBA *pColor, ts_Viewport *pViewport = NULL);

#ifdef DIRECTX_PC
bool gfxStartRender(bool toTexture = false);
#else
bool gfxStartRender(void);
#endif
bool gfxEndRender( bool bImmediateMode = false);
#ifdef PS2
void gfxSwapBuffers(int bufferidx);
#else
void gfxSwapBuffers(void);
#endif

#if defined(__cplusplus)
}
#endif

#endif //__D_GFXFUNC_H

