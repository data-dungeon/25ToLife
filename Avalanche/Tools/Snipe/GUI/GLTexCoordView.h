#pragma once
#include "GL2DView.h"
#include "PropertyBar.h"

class CModelDoc;
class CGLModelFrame;

class CGLTexCoordView : public CGL2DView
{
   DECLARE_BASEVIEW(CGLTexCoordView, CGL2DView)

public:
   static BoolProperty::Info m_bDisplayHiddenEdgesInfo;

   BoolProperty   m_bDisplayHiddenEdges;

   TexCoords     *m_pCurTexCoords;
   CGLObjectCombo m_glTexCoordsCombo;
   CGLObjectCombo m_glContextCombo;
   UINT           m_bRebuildTexCoordsCombo : 1,
                  m_bRebuildContextCombo  : 1;

            CGLTexCoordView();
   virtual ~CGLTexCoordView();

   CModelDoc *GetDocument() const { return (CModelDoc *)CGLBaseView::GetDocument(); }
   CGLModelFrame *GetHeaderFrame() { return (CGLModelFrame *)m_pBaseFrame; }
   TexCoords *GetTexCoords() { return m_pCurTexCoords; }

   void FillTexCoordsCombo();
   void FillContextCombo();

// CGLBaseView overrides
   virtual void DrawContents();
   virtual void DefaultZoomFit(SnipeObjectList *pList);
   virtual SnipeObjectTypeArray *GetSelectableObjectTypeArray() const;
   virtual void SetProjectionMatrix();
   virtual SnipeObject *GetObjectHead();

// HeaderView overrides
   virtual void UpdateHeader();

// CGLControl overrides
   virtual void PostInit();
   virtual BOOL OnControlNotify( ControlNotificationMsg msg, WPARAM wParam, LPARAM lParam );
};
