#pragma once

#include "HManipulatorPluginHelper.h"
#include "Vector.h"
#include "RGBColor.h"

class HCommandPlugin;
class HEdge;
class HPoly;

#define SPLITPOLY_SNAPTOLERANCE 0.10f

// Plugin
class SplitPolyManipulatorPlugin : public HManipulatorPluginHelper
{
public:
   DECLARE_MANIPULATOR_PLUGINHELPER( SplitPolyManipulatorPlugin, HManipulatorPluginHelper )
   HCommandPlugin *m_pAddVertexCommandPlugin;

   SplitPolyManipulatorPlugin();

// HManipulatorPluginHelper overrides
   virtual catch_msg ManipulatorInstance *CreateInstance();

// HCommandUIPluginHelper Overides
   virtual const char *GetTitleName() const;
   virtual UINT GetBitmapID() const;
   virtual catch_msg void Init(HCommandPluginManager *pCommandManager);
   virtual catch_msg BOOL SupportSOT(SnipeObjectType nType);
   virtual void GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const;
};

class Manipulator
{
private:
   HEdge *m_pEdge;
   float m_fPos;
   Vector m_vDrawPos;

public:
   static Manipulator *m_pMouseOver;

   Manipulator( HEdge *pEdge, float fPos )
   {
      m_pEdge = pEdge;
      SetPos( fPos, TRUE );
   }

   HEdge *GetEdge() const { return m_pEdge; }
   float GetPercentage() const { return m_fPos; }

   void SetPos( float fPos, BOOL bSnap );
   Vector GetPos() { return m_vDrawPos; }

   void Draw( RGBAFloat &color );
   void ComputePosition( HCGLBaseView *pView, CPoint &point, const KeyModifiers &keymodifiers );
};

// Instance

class SplitPolyManipulator : public ManipulatorInstance
{
protected:
   CList<Manipulator *> m_vertexmanipulatorlist;
   CMap<HEdge *, HEdge *, HEdge *, HEdge *> m_usededges;
   enum OperationIfClicked { OPERATION_NONE, OPERATION_ADD_ALONGEDGE, OPERATION_NOADD_ALONGEDGE, OPERATION_REPOSITION_ALONGEDGE };
   OperationIfClicked m_nIfClicked;

   Manipulator *m_pCurrentManipulator;
   
public:
   DECLARE_MANIPULATORINSTANCE( SplitPolyManipulator, ManipulatorInstance )

            SplitPolyManipulator( SplitPolyManipulatorPlugin *pPluginHelper );
   virtual ~SplitPolyManipulator();

   SplitPolyManipulatorPlugin *GetSplitPolyManipulatorPlugin() { return (SplitPolyManipulatorPlugin *) GetManipulatorPluginHelper(); }

// Operations
   Manipulator *AddVertexManipulator( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
   void ExecuteTempCommand();
   void Reset();

// ManipulatorInstance overrides
   virtual catch_msg void PostDraw(HCGLBaseView *pView);
   virtual catch_msg BOOL OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
   virtual catch_msg BOOL OnLButtonUp( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
   virtual catch_msg BOOL OnRButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
   virtual catch_msg BOOL OnRButtonUp( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
   virtual catch_msg BOOL OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
   virtual catch_msg BOOL OnApply( HCGLBaseView *pView );
   virtual catch_msg BOOL OnCancel( HCGLBaseView *pView );
   virtual catch_msg const char *GetStatusbarInstructions( KeyModifiers keymodifier, MouseStates mousestate, HCGLBaseView *pView ) const;
   virtual catch_msg KeyModifiers GetKeyModifiers( HCGLBaseView *pView, MouseStates mousestate ) const;
   virtual catch_msg void DrawPick( HCGLBaseView *pView );
   virtual catch_msg BOOL OnSetCursor( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
};
