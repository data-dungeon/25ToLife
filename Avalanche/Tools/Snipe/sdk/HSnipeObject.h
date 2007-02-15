#pragma once

#ifndef SNIPEEXPORT
#define SNIPEEXPORT
#endif

#include "Enum.h"

class HSnipeObjectList;
class Vector;
class Matrix44;
class HSelectionContainer;
class HSelection;
class HSnipeObject;
class HCGLBaseView;

typedef BOOL (* PFN_ONOBJECTNOTIFY)( void *pData, ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );

class SNIPEEXPORT HSnipeObject
{
public:
   class SNIPEEXPORT HDeleteInfo
   {
   public:
      HSnipeObject *GetSnipeObject();

      void UnDelete();
      void Delete();

      void operator delete (void *pVoid);
   };

   // Operators
   void operator delete (void *pVoid);

   //Type
   SnipeObjectType GetSOT() const;
   const HSnipeObject *IsKindOf( SnipeObjectType nType ) const;
   BOOL IsComponent() const;

   //Operations
   String GetTitle() const;

   BOOL Delete( HDeleteInfo *&pDeleteInfo, String &strOutError );

   HSnipeObject *GetParent() const;
   HSnipeObject *GetOwner();
   String GetMatchName() const;

   BOOL IsSelected() const;
   BOOL IsDeletable() const;
   void SetVisible( BOOL bVisible );
   BOOL IsVisible() const;
   BOOL IsHidable() const;
   void SetLocked( BOOL bLocked );
   BOOL IsLocked() const;
   BOOL IsLockable() const;
   BOOL IsUsedBy( HSnipeObject *pObject ) const;
   BOOL IsInProject() const;

   int GetNumChildren() const;
   HSnipeObject *GetChildAt( int index );

   HSnipeObjectList *CreateListFromSOT(SnipeObjectType nSOT);//Delete it when you are done
   HSnipeObjectList *CreateListFromSOT(const String &strMatchName); //Delete it when you are done

   const Matrix44 &GetObjectToWorldMatrix();
   const Matrix44 &GetWorldToObjectMatrix();
   BOOL GetWorldPos( Vector &pos );
   void PreTransform();
   void Transform( const Matrix44 &matrix );

   void DirtyDependentFaces();

   void Copy( HSnipeObject *pSnipeObject );// copies argument into this
   HSnipeObject *Duplicate();

   void SetDrawColor(const RGBAFloat &color);
   void Draw(HCGLBaseView *pView);

   // Selections
   HSelectionContainer *GetSelectionContainer();
   HSelection *AddSelection();

   // Callbacks
   BOOL AddCallback( UINT msgs, PFN_ONOBJECTNOTIFY pfnOnObjectNotifyCallback, void *pData );
   BOOL RemoveCallback( PFN_ONOBJECTNOTIFY pfnOnObjectNotifyCallback, void *pData, UINT msgs=NOTIFYOBJ_ALL );
};
