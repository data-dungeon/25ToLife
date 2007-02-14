#ifndef _INCLUDED_TITLE_FONT_RENDERER_
#define _INCLUDED_TITLE_FONT_RENDERER_

#include <xonline.h>
#include <xtl.h>
#include <xfont.h>
#include <UIX.h>
#include "hud/text.h"

extern bool UIXRenderCallback(void *dataPtr);

class UIXFontRenderer : public ITitleFontRenderer
{
public:

	UIXFontRenderer();
	~UIXFontRenderer();
	STDMETHODIMP_(ULONG)	Release();
	STDMETHODIMP			DrawText(LPCWSTR pText, DWORD X, DWORD Y, DWORD MaxWidth);
	STDMETHODIMP			GetTextSize(LPCWSTR pText, DWORD *pWidth, DWORD *pHeight);
	STDMETHODIMP			SetColor(D3DCOLOR Color);
	STDMETHODIMP			SetHeight(DWORD Height);
    void                    BeginFrame();
    void                    EndFrame();

private:

    DWORD       _Color;
    float       _Height;
	float		_Scale;
};

#endif //_INCLUDED_TITLE_FONT_RENDERER_