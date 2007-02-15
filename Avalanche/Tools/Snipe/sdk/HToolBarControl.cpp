//
#include "stdafx.h"
#include "HToolBarControl.h"
#include "GUI/GLToolBar.h"
#include "SDK/HCommandUIPlugin.h"
#include "Plugin/CommandUIPlugin.h"

BOOL HCGLToolBarControl::AddButton( HCommandUIPlugin *pCommandUIPlugin/*=NULL*/, int nPos /*=-1*/ )
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

   return ((CGLToolBarControl *)this)->AddButton( (CommandUIPlugin *)pCommandUIPlugin, nPos );
}

