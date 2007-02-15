#pragma once

#include "../Transform/TransformManipulator.h"

// Plugin
class TranslateManipulatorPlugin : public TransformManipulatorPlugin
{
public:
   DECLARE_MANIPULATOR_PLUGINHELPER( TranslateManipulatorPlugin, TransformManipulatorPlugin )
   HCommandPlugin *m_pAttachCommandPlugin;
   HCommandPlugin *m_pWeldCommandPlugin;

   TranslateManipulatorPlugin()
   {
      m_pAttachCommandPlugin = NULL;
      m_pWeldCommandPlugin = NULL;
   }

// HManipulatorPluginHelper Overrides
   virtual catch_msg ManipulatorInstance *CreateInstance();

// HCommandUIPluginHelper Overides
   virtual const char *GetTitleName() const;
   virtual UINT GetBitmapID() const;
   virtual void GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const;
   virtual catch_msg void Init(HCommandPluginManager *pManager);
};

class HVertex;
class HEdge;

// Instance
class TranslateManipulator : public TransformManipulator
{
public:
   UINT m_nDownRepeatCount;

   DECLARE_MANIPULATORINSTANCE( TranslateManipulator, TransformManipulator )

            TranslateManipulator( TranslateManipulatorPlugin *pPluginHelper );
   virtual ~TranslateManipulator();

   BOOL Attach(HCGLBaseView *pView);
   BOOL AttachContinuous(HCGLBaseView *pView);

   TranslateManipulatorPlugin *GetTranslateManipulatorPlugin() { return (TranslateManipulatorPlugin *) GetManipulatorPluginHelper(); }

   // SelectManipulator Overrides
   virtual void OnSingleSelect( CPoint point, HCGLBaseView *pView );

   // ManipulatorInstance overrides
   virtual catch_msg BOOL OnMButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
   virtual catch_msg BOOL OnRButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
   virtual catch_msg KeyModifiers GetKeyModifiers( HCGLBaseView *pView, MouseStates mousestate ) const;
   virtual catch_msg BOOL OnArrowKeyDown(UINT nChar, UINT nRepCnt, HCGLBaseView *pView);
   virtual catch_msg BOOL OnArrowKeyUp(UINT nChar, UINT nRepCnt, HCGLBaseView *pView);
};
