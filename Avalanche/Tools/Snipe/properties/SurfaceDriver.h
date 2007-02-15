#pragma once

#include "PropertyDriver.h"

class Poly;
class TessPoly;
class Surface;

class SurfaceDriver : public PropertyDriver
{
public:
   DECLARE_SNIPEOBJECT_PURE(SurfaceDriver, PropertyDriver)

   SurfaceDriver() {}

   Surface *GetSurface() { return (Surface *)GetObjectDriven(); }

// Overridables
   virt_base virtual BOOL IsValid() { return TRUE; }
   virt_base virtual BOOL IsValid( Poly *pPoly, LPARAM &lParam ) { return TRUE; }
   virt_base virtual BOOL IsValid( TessPoly *pTessPoly, LPARAM &lParam ) { return TRUE; }
   virt_base virtual void Bind()=0;
   virt_base virtual void UnBind()=0;
   virt_base virtual Vector2 GetUVAt( const Matrix44 &viewmatrix, UINT nIndex, Poly *pPoly, LPARAM lParam )=0;
   virt_base virtual Vector2 GetUVAt( const Matrix44 &viewmatrix, UINT nIndex, TessPoly *pPoly, LPARAM lParam )=0;

// SnipeObject overrides
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
   virtual BOOL GetPossiblePointerPropertyChoices( Property *pPointerProperty, SnipeObjectList *pSnipeObjectList );
};
