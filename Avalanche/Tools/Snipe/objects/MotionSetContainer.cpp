//

#include "stdafx.h"
#include "MotionSetContainer.h"
#include "..\GUI\MainFrm.h"

IMPLEMENT_SNIPEOBJECT_CREATE( MotionSetContainer )

MotionSetContainer::MotionSetContainer()
{
   m_strName = "Motion Sets";

   GetMainFrame()->GetMainRenderWnd()->m_glPanelBar.m_glExplorerBar.m_glMotionSetTree.SetObjectHead( this );
}

MotionSetContainer::~MotionSetContainer()
{
   GetMainFrame()->GetMainRenderWnd()->m_glPanelBar.m_glExplorerBar.m_glMotionSetTree.SetObjectHead( NULL );
}
