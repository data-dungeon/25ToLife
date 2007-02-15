//
#include "stdafx.h"
#include "Snipe.h"
#include "MotionDoc.h"
#include "Motion.h"
#include "Scene.h"
#include "SceneObject.h"
#include "CommandPluginManager.h"


IMPLEMENT_DYNCREATE(CMotionDoc, CSceneDoc)

BEGIN_MESSAGE_MAP(CMotionDoc, CSceneDoc)
END_MESSAGE_MAP()

CMotionDoc::CMotionDoc()
{
}

CMotionDoc::~CMotionDoc()
{
}

void CMotionDoc::SetObjectHead( HierObject *pObjectHead )
{
   CModelDoc::SetObjectHead( pObjectHead );
   m_pScene = (Scene *)pObjectHead->IsKindOf( SOT_Motion );
}

void CMotionDoc::ApplyModel( TransformableObject *pModelCache )
{
   SceneObject *pSceneObject = (SceneObject*)m_pScene->AddInstance( pModelCache );
   m_pScene->ApplyMotionToModel( (Motion*)GetCommandObject(), pSceneObject );
}