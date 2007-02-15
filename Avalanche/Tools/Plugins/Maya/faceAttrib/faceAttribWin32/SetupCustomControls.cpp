//
// SetupCustomControls
//

/*
 * $History: SetupCustomControls.cpp $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 2/14/03    Time: 2:15p
 * Updated in $/Avalanche/tools/Plugins/Maya/faceAttrib/faceAttribWin32
 * open file notification
 * prefix on control windows
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 12/13/02   Time: 2:15p
 * Updated in $/Avalanche/tools/Plugins/Maya/objAttrib/objAttribWin32
 * first release of objectattributes for maya
 * 
 * *****************  Version 2  *****************
 * User: Bsafarik     Date: 12/03/02   Time: 3:48p
 * Updated in $/Avalanche/tools/Plugins/Maya/objAttrib/objAttribWin32
 * 
 * *****************  Version 1  *****************
 * User: Bsafarik     Date: 12/03/02   Time: 11:16a
 * Created in $/Avalanche/tools/Plugins/Maya/objAttrib/objAttribWin32
 * 
 * *****************  Version 1  *****************
 * User: Bsafarik     Date: 12/03/02   Time: 10:45a
 * Created in $/Avalanche/tools/Plugins/Maya/objAttrib
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 11/27/02   Time: 10:22a
 * Updated in $/Avalanche/tools/Plugins/Max/ObjectAttributes
 * new setup custom controls class
 */

// precomp'ed header files
#include "StdAfx.h"

// local resources
#include "resource.h"
#include "SetupCustomControls.h"

using namespace AttrControls;

void SetupCustomControls::Init(HINSTANCE hInstance)
{
	CustomControls::InitVariables(hInstance);
	// set up the custom colors
#if 0
	CustomControls::SetColor(COLOR_WINDOW,GetCustSysColor(COLOR_WINDOW));
	CustomControls::SetColor(COLOR_BTNFACE,GetCustSysColor(COLOR_BTNFACE));
	CustomControls::SetColor(COLOR_BTNHIGHLIGHT,GetCustSysColor(COLOR_BTNHIGHLIGHT));
	CustomControls::SetColor(COLOR_BTNSHADOW,GetCustSysColor(COLOR_BTNSHADOW));
	CustomControls::SetColor(COLOR_BTNTEXT,GetCustSysColor(COLOR_BTNTEXT));
	CustomControls::SetColor(CustomControls::COLOR_ROLLUP_TITLE_FACE,ColorMan()->GetColor(kRollupTitleFace));
	CustomControls::SetColor(CustomControls::COLOR_ROLLUP_TITLE_TEXT,ColorMan()->GetColor(kRollupTitleText));
	CustomControls::SetColor(CustomControls::COLOR_ROLLUP_TITLE_HILIGHT,ColorMan()->GetColor(kRollupTitleHilight));
	CustomControls::SetColor(CustomControls::COLOR_ROLLUP_TITLE_SHADOW,ColorMan()->GetColor(kRollupTitleShadow));
	CustomControls::SetColorBrush(COLOR_WINDOW,GetCustSysColor(COLOR_WINDOW));
	CustomControls::SetColorBrush(COLOR_BTNSHADOW,GetCustSysColor(COLOR_BTNSHADOW));
	CustomControls::SetColorBrush(COLOR_BTNFACE,GetCustSysColor(COLOR_BTNFACE));
	CustomControls::SetColorBrush(COLOR_BTNTEXT,GetCustSysColor(COLOR_BTNTEXT));
#endif
	CustomControls::InitCustomControls(hInstance,"FA"); // Initialize my custom Controls
#if 0
	CustomControls::SetEditSetFocusCallback(&EditSetFocusCallback);
	CustomControls::SetEditKillFocusCallback(&EditKillFocusCallback);
#endif
}

void CALLBACK SetupCustomControls::EditSetFocusCallback(void)
{
//	DisableAccelerators();
}

void CALLBACK SetupCustomControls::EditKillFocusCallback(void)
{
//	EnableAccelerators();
}




