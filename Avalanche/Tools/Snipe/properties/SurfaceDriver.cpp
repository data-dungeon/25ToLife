//

#include "stdafx.h"
#include "SurfaceDriver.h"
#include "Surface.h"

IMPLEMENT_SNIPEOBJECT_PURE( SurfaceDriver )

BOOL SurfaceDriver::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
      case NOTIFYOBJ_REMOVEDFROMPROJECT:
      {
         Surface *pSurface = GetSurface();
         Property *pProperty = m_pProperty;
         if (pProperty)
            pProperty->RemoveNotifyNode( &m_pProperty, NOTIFYOBJ_DELETE );
         if (pSurface)
            pSurface->DestroyCompositedSurfaces();
         break;
      }

      case NOTIFYOBJ_VALUECHANGED:
      {
         Surface *pSurface = GetSurface();
         if (pSurface)
            pSurface->NotifyNodes( NOTIFYOBJ_ICONCHANGED, 0, (LPARAM)pSurface );
         break;
      }
   }

   return PropertyDriver::OnObjectNotify( msg, wParam, lParam );
}

BOOL SurfaceDriver::GetPossiblePointerPropertyChoices( Property *pPointerProperty, SnipeObjectList *pSnipeObjectList )
{
   if (pPointerProperty == &m_pProperty)
   {
      Surface *pSurface = GetSurface();
      ASSERT(pSurface);
      PropertyArray *pProperyArray = pSurface->GetPropertyArray();
      for (UINT i=0; i<pSurface->GetNumProperties(); i++)
         pSnipeObjectList->AddTail( pProperyArray->GetAt(i) );
      return TRUE;
   }

   return PropertyDriver::GetPossiblePointerPropertyChoices( pPointerProperty, pSnipeObjectList );
}
