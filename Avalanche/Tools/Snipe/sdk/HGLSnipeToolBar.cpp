//

#include "stdafx.h"
#include "HGLSnipeToolBar.h"
#include "GUI/GLSnipeToolbar.h"

BOOL HGLSnipeToolBar::AddButton( UINT nCmdID/*=ID_SEPARATOR*/, int nPos /*=-1*/ )
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
   return ((CGLToolBarControl *)this)->AddButton( nCmdID, nPos );
}

