//
#include "stdafx.h"
#include "ModelDoc.h"
#include "GLModelView.h"
#include "Objects/Project.h"
#include "MainFrm.h"
#include "GLModelFrame.h"
#include "Resource.h"

IMPLEMENT_DYNCREATE(CModelDoc, C3DDoc)

BEGIN_MESSAGE_MAP(CModelDoc, C3DDoc)
   ON_COMMAND(ID_FILE_SAVE, OnFileSave)
   ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
END_MESSAGE_MAP()

CModelDoc::CModelDoc()
{
   m_pCurModel = NULL;

//   m_SelectableObjectTypeArray.Add( SOT_TransformableObject );IDS_SOT_TransformableObject
   m_SelectableObjectTypeArray.Add( SOT_Vertex );
   m_SelectableObjectTypeArray.Add( SOT_Edge );
   m_SelectableObjectTypeArray.Add( SOT_Poly );
   m_SelectableObjectTypeArray.Add( SOT_Bone );
   m_SelectableObjectTypeArray.SetCurrentIndex( 0 );

   m_TexCoordSelectableObjectTypeArray.Add( SOT_UV );
   m_TexCoordSelectableObjectTypeArray.SetCurrentIndex( 0 );

//   m_SchematicSelectableObjectTypeArray.Add( SOT_TransformableObject );IDS_SOT_TransformableObject
   m_SchematicSelectableObjectTypeArray.SetCurrentIndex( 0 );

   m_fPulse = 1.0f;
   m_tPrevPulseTime.SetTagged();
}

CModelDoc::~CModelDoc()
{
}

const BoundingSphere &CModelDoc::GetBoundingSphere()
{
   return m_pCurModel->GetBoundingSphere();
}

void CModelDoc::SetObjectHead( HierObject *pObjectHead )
{
   C3DDoc::SetObjectHead( pObjectHead );

   m_pCurModel = (PolyModel *)pObjectHead->IsKindOf( SOT_PolyModel );
}

BOOL CModelDoc::OnNewDocument()
{
	if (!C3DDoc::OnNewDocument())
		return FALSE;

	return TRUE;
}

BOOL CModelDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
//   if (!C3DDoc::OnOpenDocument(lpszPathName))
//      return FALSE;

   return TRUE;
}

BOOL CModelDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
//   return C3DDoc::OnSaveDocument(lpszPathName);
   return FALSE;
}

void CModelDoc::Serialize(CArchive& ar)
{
}

void CModelDoc::OnUpdateFileSave(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(TRUE);
}

void CModelDoc::OnFileSave()
{
   ::AfxMessageBox("TODO: OnFileSave", MB_OK);
}

BOOL CModelDoc::OnSnipeIdle()
{
   BOOL bMoreIdles = C3DDoc::OnSnipeIdle();

   GetApp().DrawInvalidControls();

   LARGE_INTEGER liCurTime;
   QueryPerformanceCounter( &liCurTime );
   LARGE_INTEGER liFrequency;
   QueryPerformanceFrequency( &liFrequency );

   float fLength = 1.0f;
   float fCurTime = (float)liCurTime.QuadPart / (float)liFrequency.QuadPart;
   fCurTime = fmod( fCurTime, fLength );
   Time tCurTime( fCurTime );
   tCurTime.SnapToFrame( 30.0 );

   if (tCurTime == m_tPrevPulseTime)
      return bMoreIdles;

   m_tPrevPulseTime = tCurTime;

   m_fPulse = (1.0f + sin( fCurTime*360.0f*DEGTORAD/fLength )) / 2.0f;

   for (POSITION pos=m_viewlist.GetHeadPosition(); pos; )
   {
      CGLBaseView *pView = m_viewlist.GetNext( pos );
      bMoreIdles |= pView->OnSnipeIdle();
   }

   return bMoreIdles;
}
