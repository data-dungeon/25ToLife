#pragma once

#include "HManipulatorPluginHelper.h"
#include "..\Utility\Vector.h"

class HCommandPlugin;

// Plugin
class AddBoneManipulatorPlugin : public HManipulatorPluginHelper
{
public:
   DECLARE_MANIPULATOR_PLUGINHELPER( AddBoneManipulatorPlugin, HManipulatorPluginHelper )
   HCommandPlugin *m_pAddBoneCommandPlugin;

   AddBoneManipulatorPlugin();

// HManipulatorPluginHelper overrides
   virtual catch_msg ManipulatorInstance *CreateInstance();

// HCommandUIPluginHelper Overides
   virtual const char *GetTitleName() const;
   virtual UINT GetBitmapID() const;
   virtual catch_msg void Init(HCommandPluginManager *pCommandManager);
   virtual catch_msg BOOL SupportSOT(SnipeObjectType nType);
};


// Instance
class HBone;

class AddBoneManipulator : public ManipulatorInstance
{
protected:
   Vector m_vPos,m_vPivot,m_vRotate;
   float m_fLength;
   HPolyModel *m_pPolyModel;
   BOOL m_bDragging;
   HBone *m_pParentBone;

public:
   DECLARE_MANIPULATORINSTANCE( AddBoneManipulator, ManipulatorInstance )

            AddBoneManipulator( AddBoneManipulatorPlugin *pPluginHelper );
   virtual ~AddBoneManipulator();

   AddBoneManipulatorPlugin *GetAddBoneManipulatorPlugin() { return (AddBoneManipulatorPlugin *) GetManipulatorPluginHelper(); }

// Operations
   void ComputePosition( HCGLBaseView *pView, CPoint &point, const KeyModifiers &keymodifiers );
   void ExecuteAddBone(HPolyModel *pPolyModel,const Vector &vStart,const Vector &vRotate,float fLength,HBone *pParentBone,BOOL bTempCommand);

// ManipulatorInstance overrides
   virtual catch_msg void PostDraw(HCGLBaseView *pView);
   virtual catch_msg BOOL OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
   virtual catch_msg BOOL OnLButtonUp( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
   virtual catch_msg BOOL OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
   virtual catch_msg KeyModifiers GetKeyModifiers( HCGLBaseView *pView, MouseStates mousestate ) const;
   virtual catch_msg BOOL OnApply( HCGLBaseView *pView );
   virtual catch_msg BOOL OnCancel( HCGLBaseView *pView );
};
