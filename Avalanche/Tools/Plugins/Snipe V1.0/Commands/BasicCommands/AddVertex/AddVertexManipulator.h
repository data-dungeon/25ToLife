#pragma once

#include "HManipulatorPluginHelper.h"
#include "Vector.h"
#include "RGBColor.h"
#include "HVertex.h"
#include "HEdge.h"

class HCommandPlugin;
class HEdge;
class HPoly;

#define ADDVERTEX_SNAPTOLERANCE 0.10f

// Plugin
class AddVertexManipulatorPlugin : public HManipulatorPluginHelper
{
public:
   DECLARE_MANIPULATOR_PLUGINHELPER( AddVertexManipulatorPlugin, HManipulatorPluginHelper )
   HCommandPlugin *m_pAddVertexCommandPlugin;
   HCommandPlugin *m_pStoreValueCommandPlugin;

   AddVertexManipulatorPlugin();

// HManipulatorPluginHelper overrides
   virtual catch_msg ManipulatorInstance *CreateInstance();

// HCommandUIPluginHelper Overides
   virtual const char *GetTitleName() const;
   virtual UINT GetBitmapID() const;
   virtual catch_msg void Init(HCommandPluginManager *pCommandManager);
   virtual catch_msg BOOL SupportSOT(SnipeObjectType nType);
   virtual void GetAccelerators( UINT &nCount, CommandUIAccel *&pAccel ) const;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////

class AddVertexManipulator;

class VertexManipulator
{
public:
   // Pure
   virt_base virtual BOOL OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView, AddVertexManipulator *pAddVertexManipulator )  { return FALSE; }
   virt_base virtual BOOL OnLButtonUp( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView, AddVertexManipulator *pAddVertexManipulator ) { return FALSE; }
   virt_base virtual BOOL OnRButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView, AddVertexManipulator *pAddVertexManipulator )  { return FALSE; }
   virt_base virtual BOOL OnRButtonUp( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView, AddVertexManipulator *pAddVertexManipulator ) { return FALSE; }
   virt_base virtual BOOL OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView, AddVertexManipulator *pAddVertexManipulator ) { return FALSE; }
   virt_base virtual Vector GetPos() const = 0;
};

class AlongEdgeManipulator : public VertexManipulator
{
private:
   HEdge *m_pEdge;
   float m_fPercentage;
   Vector m_vDrawPos;

public:
   AlongEdgeManipulator( HEdge *pEdge, float fPercentage );
   void SetPercentage( float fPercent, BOOL bSnap );
   float GetPercentage() const { return m_fPercentage; }
   HEdge *GetEdge() const { return m_pEdge; }

   void ComputePosition( HCGLBaseView *pView, CPoint &point, const KeyModifiers &keymodifiers );
   Vector GetPos() const { return m_vDrawPos; }

   virtual BOOL OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView, AddVertexManipulator *pAddVertexManipulator );
   virtual BOOL OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView, AddVertexManipulator *pAddVertexManipulator );
   virtual BOOL OnLButtonUp( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView, AddVertexManipulator *pAddVertexManipulator );
};

class AlongEdgeRingManipulator : public AlongEdgeManipulator
{
public:
   AlongEdgeRingManipulator( HEdge *pEdge, float fPercentage ) : AlongEdgeManipulator( pEdge, fPercentage ) {}

   virtual BOOL OnRButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView, AddVertexManipulator *pAddVertexManipulator );
   virtual BOOL OnRButtonUp( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView, AddVertexManipulator *pAddVertexManipulator );
   virtual BOOL OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView, AddVertexManipulator *pAddVertexManipulator );
};

class AtPosManipulator : public VertexManipulator
{
public:
   Vector m_vPos;
   HVectorProperty *m_vPositionProperty;

   AtPosManipulator( const Vector &vPos ) : VertexManipulator()
   { 
      m_vPos = vPos; 
      m_vPositionProperty = NULL;
   }

   void AtPosManipulator::ReExecuteAddVertexCommand(AddVertexManipulator *pAddVertexManipulator);
   void ExecuteAddVertexCommand(AddVertexManipulator *pManipulator);
   void ComputePosition( HCGLBaseView *pView, CPoint &point, const KeyModifiers &keymodifiers );

   virtual BOOL OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView, AddVertexManipulator *pAddVertexManipulator );
   virtual BOOL OnRButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView, AddVertexManipulator *pAddVertexManipulator );
   virtual BOOL OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView, AddVertexManipulator *pAddVertexManipulator );
   virtual Vector GetPos() const { return m_vPos; }
};

///////////////////////////////////////////////////////////////////////////////////////////////////////

// Instance
class AddVertexManipulator : public ManipulatorInstance
{
private:
   HVertex *m_pPrevVertex;
   String   m_strPrevAddString;

   HVertex *m_pCurrentVertex;
   String   m_strCurrentAddString;

   HEdge *m_pCurrentEdge;
   HEdge *m_pPrevContinuousEdge;

   VertexManipulator *m_pCurrentVertexManipulator;

public:
   DECLARE_MANIPULATORINSTANCE( AddVertexManipulator, ManipulatorInstance )

            AddVertexManipulator( AddVertexManipulatorPlugin *pPluginHelper );
   virtual ~AddVertexManipulator();

   AddVertexManipulatorPlugin *GetAddVertexManipulatorPlugin() { return (AddVertexManipulatorPlugin *) GetManipulatorPluginHelper(); }

// Operations
   void Reset();

   void SetCurrentVertexManipulator( VertexManipulator *pManipulator )
   {
      if (m_pCurrentVertexManipulator)
         delete m_pCurrentVertexManipulator;

      m_pCurrentVertexManipulator = pManipulator;
   }

   VertexManipulator *GetCurrentVertexManipulator() const
   {
      return m_pCurrentVertexManipulator;
   }

   HVertex *GetPrevVertex() const { return m_pPrevVertex; }
   String GetPrevAddString() const { return m_strPrevAddString; }
   void SetPrevVertex( HVertex *pPrevVertex, const String &strPrevAddString) 
   { 
      m_pPrevVertex = pPrevVertex; 
      m_strPrevAddString = strPrevAddString;
   }

   HVertex *GetCurrentVertex() const { return m_pCurrentVertex; }
   String GetCurrentAddString() const { return m_strCurrentAddString; }
   void SetCurrentVertex( HVertex *pCurrentVertex, const String &strCurrentAddString ) 
   { 
      m_pCurrentVertex = pCurrentVertex; 
      m_strCurrentAddString = strCurrentAddString;
   }

   HEdge *GetCurrentEdge() const { return m_pCurrentEdge; }
   void SetCurrentEdge( HEdge *pEdge ) { m_pCurrentEdge = pEdge; }

   HEdge *GetPrevContinuousEdge() const { return m_pPrevContinuousEdge; }
   void SetPrevContinuousEdge( HEdge *pEdge ) { m_pPrevContinuousEdge = pEdge; }

   void SplitRing( HEdge *pEdge, float fPercent );

// ManipulatorInstance overrides
   virtual catch_msg BOOL OnLButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
   virtual catch_msg BOOL OnLButtonUp( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
   virtual catch_msg BOOL OnRButtonDown( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
   virtual catch_msg BOOL OnRButtonUp( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
   virtual catch_msg BOOL OnMouseMove( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
   virtual catch_msg BOOL OnApply( HCGLBaseView *pView );
   virtual catch_msg BOOL OnCancel( HCGLBaseView *pView );
   virtual catch_msg const char *GetStatusbarInstructions( KeyModifiers keymodifier, MouseStates mousestate, HCGLBaseView *pView ) const;
   virtual catch_msg KeyModifiers GetKeyModifiers( HCGLBaseView *pView, MouseStates mousestate ) const;
   virtual catch_msg BOOL OnSetCursor( KeyModifiers keymodifier, MouseStates mousestate, CPoint point, HCGLBaseView *pView );
};
