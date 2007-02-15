//
// SetupCustomControls
//

// precomp'ed header files
#include "pfdModPCH.h"

// local resources
#include "SetupCustomControls.h"

using namespace AttrControls;

void SetupCustomControls::Init(HINSTANCE hInstance)
{
	CustomControls::InitVariables(hInstance);
	// set up the custom colors
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
	CustomControls::InitCustomControls(hInstance,"OA"); // Initialize my custom Controls
	CustomControls::SetEditSetFocusCallback(&EditSetFocusCallback);
	CustomControls::SetEditKillFocusCallback(&EditKillFocusCallback);
	KeyValPanelController::SetRenameStartCallback(&EditSetFocusCallback);
	KeyValPanelController::SetRenameEndCallback(&EditKillFocusCallback);

}

void CALLBACK SetupCustomControls::EditSetFocusCallback(void)
{
	DisableAccelerators();
}

void CALLBACK SetupCustomControls::EditKillFocusCallback(void)
{
	EnableAccelerators();
}




