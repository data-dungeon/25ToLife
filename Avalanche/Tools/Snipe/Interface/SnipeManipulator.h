#pragma once

#include "GUI/GLImageList.h"
#include "Enum.h"
#include "SnipeObject.h"
#include "KeyModifiers.h"
#include "MouseStates.h"

class SelectionNode;
class CGLBaseView;
class ManipulatorPlugin;
class PropertyArray;
class SnipeManipulator;

class SnipeManipulatorInstance : public SnipeObject
{
public:
   SnipeManipulator *m_pSnipeManipulator;
   ManipulatorPlugin *m_pPlugin;
   PropertyArray  *m_pPropertyArray;

   void *m_pPluginInstanceData;

   SnipeManipulatorInstance(SnipeManipulator *pSnipeManipulator, void *pPluginInstanceData);

   ~SnipeManipulatorInstance()
   {
      DestroyCallback();
   }

   void SetPropertyArray( PropertyArray *pPropertyArray );

   // Callbacks
   PropertyArray *GetPropertyArrayCallback();
   void  DestroyCallback();
   void	DrawPick(const CGLBaseView *pView);
   void	PreDraw(CGLBaseView *pView);
   void	PostDraw(CGLBaseView *pView);
   const char *GetStatusbarInstructions( KeyModifiers keymodifier, MouseStates mousestate, CGLBaseView *pView );
   KeyModifiers GetKeyModifiers( CGLBaseView *pView, MouseStates mousestate );
   BOOL	OnLButtonDown(KeyModifiers keymodifier, MouseStates mousestate, CPoint point, CGLBaseView *pView);
   BOOL	OnLButtonDblClk(KeyModifiers keymodifier, MouseStates mousestate, CPoint point, CGLBaseView *pView);
   BOOL	OnLButtonUp(KeyModifiers keymodifier, MouseStates mousestate, CPoint point, CGLBaseView *pView);
   BOOL	OnRButtonDown(KeyModifiers keymodifier, MouseStates mousestate, CPoint point, CGLBaseView *pView);
   BOOL	OnRButtonDblClk(KeyModifiers keymodifier, MouseStates mousestate, CPoint point, CGLBaseView *pView);
   BOOL	OnRButtonUp(KeyModifiers keymodifier, MouseStates mousestate, CPoint point, CGLBaseView *pView);
   BOOL	OnMButtonDown(KeyModifiers keymodifier, MouseStates mousestate, CPoint point, CGLBaseView *pView);
   BOOL	OnMButtonDblClk(KeyModifiers keymodifier, MouseStates mousestate, CPoint point, CGLBaseView *pView);
   BOOL	OnMButtonUp(KeyModifiers keymodifier, MouseStates mousestate, CPoint point, CGLBaseView *pView);
   BOOL	OnMouseMove(KeyModifiers keymodifier, MouseStates mousestate, CPoint point, CGLBaseView *pView);
   BOOL  OnMouseWheel(KeyModifiers keymodifier, MouseStates mousestate, short zDelta, CPoint point, CGLBaseView *pView);
   BOOL  OnSetCursor( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, CGLBaseView *pView );
   UINT  GetToolTipText( CPoint point, String &strText, const CGLBaseView *pView );
   BOOL  OnApply(CGLBaseView *pView);
   BOOL  OnCancel(CGLBaseView *pView);
   BOOL	OnArrowKeyDown(UINT nChar, UINT nRepCnt, CGLBaseView *pView);
   BOOL	OnArrowKeyUp(UINT nChar, UINT nRepCnt, CGLBaseView *pView);

// SnipeObject overrides
   virtual void DrawTreeItem( CGLControl *pItem, float x=0.0f );
   virtual const char *GetClassName() const;
   virtual String GetMatchName() const;
   virtual String GetTitle() const;
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
   virtual PropertyArray *GetPropertyArray() { return m_pPropertyArray; }
};

// There is one of these per SnipeObjectTypeArray
class SnipeManipulator
{
public:
   ManipulatorPlugin *m_pPlugin;

            SnipeManipulator( ManipulatorPlugin *pManipulatorPlugin );
   virtual ~SnipeManipulator();

   BOOL IsComboInvalid();

   // Callbacks
   SnipeManipulatorInstance *CreateInstance();
   BOOL  SupportSOT( SnipeObjectType nType );
};
