/* access routines */

inline float Display__GetNearClip(void)
{
	return(DisplayState.fNearClip);
}
inline float Display__GetFarClip(void)
{
	return(DisplayState.fFarClip);
}
inline float Display__GetFieldOfViewY(void)
{
	return(DisplayState.fFieldOfViewY);
}
inline float Display__GetNearFog(void)
{
	return(DisplayState.fNearFog);
}
inline float Display__GetFarFog(void)
{
	return(DisplayState.fFarFog);
}
inline float Display__GetFogDensity(void)
{
	return(DisplayState.fFogDensity);
}
inline int Display__GetFogMode(void)
{
	return(DisplayState.nFogMode);
}
inline ts_bRGBA *Display__GetFogColor(void)
{
	return(&DisplayState.FogColor);
}

inline u32 Display__GetClearMode( void)
{
	return( DisplayState.u32ClearMode);
}

inline ts_bRGBA *Display__GetClearColor( void)
{
	return( &DisplayState.ClearColor);
}

/* routines to access data */

inline int Display__GetScreenWidth(void)
{
	return(DisplayState.nScreenWidth);
}
inline int Display__GetScreenHeight(void)
{
	return(DisplayState.nScreenHeight);
}
inline bool Display__GetNTSCMode(void)
{
#ifdef PS2
	return(DisplayState.bNTSCMode);
#else
	return(false);
#endif
}

#ifdef WIN32
inline HWND Display__GetWnd(void)
{
	ASSERT(DisplayState.hWnd != NULL);	// if NULL, window hasn't been initialized yet
	return(DisplayState.hWnd);
}
inline HDC Display__GetDC(void)
{
	ASSERT(DisplayState.hDC != NULL);	// if NULL, window hasn't been initialized yet
	return(DisplayState.hDC);
}
#endif //WIN32

#ifdef PS2
inline int Display__GetRenderTargetWidth(void)
{
	return DisplayState.u32FrameBufferWidth;
}

inline int Display__GetRenderTargetHeight(void)
{
	return DisplayState.u32FrameBufferHeight;
}
#endif

