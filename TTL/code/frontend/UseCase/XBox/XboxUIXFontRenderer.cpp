#include "TTLPCH.h"
#include "XboxUIXFontRenderer.hpp"
#include <stdio.h>
#include "OSI/XBox/OSIXBox.hpp"

#include "platform/Xbox/xboxapp.h"
#include "platform/Xbox/xbfont.h"

#include <xfont.h>


/////////////////////////////////////////////////////////////////////////////////////////////////////
///
/////////////////////////////////////////////////////////////////////////////////////////////////////
bool UIXRenderCallback(void *dataPtr)
{
	if( !OSI::isCreated() )
	{
		return true;
	}

	// Draw the UIX screen
    OSIXbox* osix = reinterpret_cast< OSIXbox* >( OSI::getSingletonPtr() );
	ASSERT( osix );
	if( !osix->uixIsStarted() )
	{
		return true;
	}

    UIXFontRenderer *fontRendererPtr = (UIXFontRenderer *)(dataPtr);

    if( osix->uixMustRender() )
	{
        fontRendererPtr->BeginFrame();

		IDirect3DSurface8* backbuf;
		DirectX_Device->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &backbuf );
		osix->uixRenderNow( backbuf );
		backbuf->Release();

        fontRendererPtr->EndFrame();
	}

    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Create the Text objects for rendering and set default values for class members.
/////////////////////////////////////////////////////////////////////////////////////////////////////
UIXFontRenderer::UIXFontRenderer() :
    _Color(0xFFFFFFFF),
    _Height(20.0f)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Clean up.
/////////////////////////////////////////////////////////////////////////////////////////////////////
UIXFontRenderer::~UIXFontRenderer()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Decrease the reference count.  Since this class is only created/deleted once, we clean up.
///
/// \return The new reference count.
/////////////////////////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(ULONG) UIXFontRenderer::Release()
{
	delete this;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Called by UIX to draw a string to the screen
///
/// \param pText Pointer to a null-terminated Unicode string that specifies the text to draw.
///
/// \param X X pixel coordinate of the text. 
///
/// \param Y Y pixel coordinate of the text. 
///
/// \param MaxWidth Maximum width to draw. Text extending beyond the specified width should be clipped.
///
/// \return S_OK if successful, an error code otherwise.
/////////////////////////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP UIXFontRenderer::DrawText(LPCWSTR pText, DWORD X, DWORD Y, DWORD MaxWidth)
{
	g_Font.Begin();
	g_Font.SetScaleFactors( _Scale, _Scale );
	g_Font.DrawText( (float)X, (float)Y, _Color, pText );
	g_Font.End();
    return S_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Called by UIX to figure out the size of a string: used to space the screen properly.
///
/// \param pText Pointer to null-terminated Unicode string
///
/// \param pWidth Pointer to a DWORD variable that receives the width, in pixels, of the text. 
///
/// \param pHeight Pointer to a DWORD variable that receives the height, in pixels, of the text. 
///
/// \return S_OK if successful, an error code otherwise 
/////////////////////////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP UIXFontRenderer::GetTextSize(LPCWSTR pText, DWORD *pWidth, DWORD *pHeight)
{
	float fwidth, fheight;

	g_Font.SetScaleFactors( _Scale, _Scale );
	g_Font.GetTextExtent( pText, &fwidth, &fheight );
	*pWidth = (DWORD) fwidth;
	*pHeight = (DWORD) fheight;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Sets the current text color
///
/// \param The new color to use
///
/// \return S_OK if successful, an error code otherwise 
/////////////////////////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP UIXFontRenderer::SetColor(D3DCOLOR Color)
{
    _Color = Color;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Sets the current text height
///
/// \param The new text size to use
///
/// \return S_OK if successful, an error code otherwise 
/////////////////////////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP UIXFontRenderer::SetHeight(DWORD Height)
{
    _Height = (float)Height;
	_Scale = _Height / g_Font.GetFontHeight();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Called at the end of every frame so that we know to restart at the beginning of
/// the text objects for the next frame.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UIXFontRenderer::EndFrame()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Called at the start of every frame so that we can reset the text objects.
/////////////////////////////////////////////////////////////////////////////////////////////////////
void UIXFontRenderer::BeginFrame()
{
}
