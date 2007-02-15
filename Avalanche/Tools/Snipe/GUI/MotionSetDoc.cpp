//
#include "stdafx.h"
#include "Snipe.h"
#include "MotionSetDoc.h"
#include "MotionSet.h"
#include "Scene.h"
#include "SceneObject.h"
#include "CommandPluginManager.h"


IMPLEMENT_DYNCREATE(CMotionSetDoc, CSceneDoc)

BEGIN_MESSAGE_MAP(CMotionSetDoc, CSceneDoc)
END_MESSAGE_MAP()

CMotionSetDoc::CMotionSetDoc()
{
   m_MotionSetSelectableObjectTypeArray.Add( SOT_MotionChannelBiasObject );
   m_MotionSetSelectableObjectTypeArray.SetCurrentIndex( 0 );
}

CMotionSetDoc::~CMotionSetDoc()
{
}

void CMotionSetDoc::SetObjectHead( HierObject *pObjectHead )
{
   CModelDoc::SetObjectHead( pObjectHead );
   m_pScene = (Scene *)pObjectHead->IsKindOf( SOT_MotionSet );
}

void CMotionSetDoc::AddMotion( Motion *pMotion )
{
   ((MotionSet*)GetCommandObject())->AddMotion(pMotion);
}