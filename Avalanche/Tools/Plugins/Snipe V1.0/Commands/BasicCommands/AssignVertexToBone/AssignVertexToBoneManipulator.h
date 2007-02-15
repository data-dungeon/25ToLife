#pragma once

//#include "HManipulatorPluginHelper.h"
#include "../Select/SelectManipulator.h"

class HCommandPlugin;
class HTransformableObject;

// Plugin
class AssignVertexToBoneManipulatorPlugin : public SelectManipulatorPlugin
{
public:
   DECLARE_MANIPULATOR_PLUGINHELPER( AssignVertexToBoneManipulatorPlugin, SelectManipulatorPlugin )
   HCommandPlugin *m_pAssignVertexToBoneCommandPlugin;

   AssignVertexToBoneManipulatorPlugin();

// HManipulatorPluginHelper overrides
   virtual catch_msg ManipulatorInstance *CreateInstance();

// HCommandUIPluginHelper Overides
   virtual const char *GetTitleName() const;
   virtual UINT GetBitmapID() const;
   virtual catch_msg void Init(HCommandPluginManager *pCommandManager);
   virtual catch_msg BOOL SupportSOT(SnipeObjectType nType);
//   virtual void GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const;
};


// Instance

class HVertex;
class HBone;

class AssignVertexToBoneManipulator : public SelectManipulator
{
protected:
   BOOL m_bValidateSelection;
   CArray<BOOL> m_vertexlookup;
   HBone *m_pBone;
   CArray<HVertex*> m_vertexarray;

public:
   DECLARE_MANIPULATORINSTANCE( AssignVertexToBoneManipulator, SelectManipulator )

            AssignVertexToBoneManipulator( AssignVertexToBoneManipulatorPlugin *pPluginHelper );
   virtual ~AssignVertexToBoneManipulator();

   AssignVertexToBoneManipulatorPlugin *GetAssignVertexToBoneManipulatorPlugin() { return (AssignVertexToBoneManipulatorPlugin *) GetManipulatorPluginHelper(); }

// Operations
 

// ManipulatorInstance overrides
   virtual catch_msg void PostDraw(HCGLBaseView *pView);
   virtual catch_msg BOOL OnLButtonUp( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );

   virtual void DoPick( HCGLBaseView *pView, HPickHitArray *pPickHitArray, const CRect &rcRect, BOOL *bOutSinglePick=NULL );
   virtual BOOL OnSelect( HCGLBaseView *pView, HSnipeObjectList *pSelectList, BOOL bTemp, BOOL *bSingleSelect=NULL, HSnipeObject *pFocus=NULL ); // User selected something, no the command must be put on the stack

private:
   void DrawVertices(HCGLBaseView *pView,HHierObject *pHierObject,HSelectionList *pSelectionList);
   void DrawBoneVertices(HCGLBaseView *pView,HTransformableObject *pTransformableObject,HSelectionList *pSelectionList);
};
