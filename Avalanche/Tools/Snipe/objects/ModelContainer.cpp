//

#include "stdafx.h"
#include "ModelContainer.h"
#include "..\GUI\MainFrm.h"

IMPLEMENT_SNIPEOBJECT_CREATE( ModelContainer )

ModelContainer::ModelContainer()
{
   m_strName = "Models";

   GetMainFrame()->GetMainRenderWnd()->m_glPanelBar.m_glExplorerBar.m_glModelTree.SetObjectHead( this );
}

ModelContainer::~ModelContainer()
{
   GetMainFrame()->GetMainRenderWnd()->m_glPanelBar.m_glExplorerBar.m_glModelTree.SetObjectHead( NULL );
}
