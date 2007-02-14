#ifndef _INCLUDED_UIX_AUDIO_PLUGIN_
#define _INCLUDED_UIX_AUDIO_PLUGIN_

#include <xonline.h>
#include <xtl.h>
#include <xfont.h>
#include <UIX.h>

class UIXAudioPlugin : public ITitleAudioPlugin
{
public:
	virtual ULONG STDMETHODCALLTYPE Release();
	virtual HRESULT STDMETHODCALLTYPE DoWork();
	virtual HRESULT STDMETHODCALLTYPE PlaySound( LPCSTR soundName );
};

#endif