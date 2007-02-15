//
#include "stdafx.h"
#include "ImageContainer.h"
#include "..\GUI\MainFrm.h"

/////////////////////////////////////////////////////
// ImageContainer

IMPLEMENT_SNIPEOBJECT_CREATE( ImageContainer )

ImageContainer::ImageContainer()
{
   m_strName = "Images";

   GetMainFrame()->GetMainRenderWnd()->m_glPanelBar.m_glExplorerBar.m_glImageTree.SetObjectHead( this );
}

ImageContainer::~ImageContainer()
{
   GetMainFrame()->GetMainRenderWnd()->m_glPanelBar.m_glExplorerBar.m_glImageTree.SetObjectHead( NULL );
}

BOOL ImageContainer::Load( BaseStream &bs )
{
#ifdef _DEBUG
   OutputWindowTimer timer( "Load Images" );
#endif // _DEBUG

   return ContainerObject::Load( bs );
}
