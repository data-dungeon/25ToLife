#include "stdafx.h"
#include "Scene.h"
#include "GUI/Snipe.h"
#include "Interface/Implement.h"
#include "resource.h"
#include "SceneTranslatorPluginManager.h"
#include "SceneObject.h"
#include "Motion.h"

/////////////////////////////////////////////////////
// SceneContainer

IMPLEMENT_SNIPEOBJECT_CREATE( SceneContainer )

SceneContainer::SceneContainer()
{
   m_strName = "Scenes";
}

SceneContainer::~SceneContainer()
{
}

/////////////////////////////////////////////////////
// Scene

IMPLEMENT_SNIPEOBJECT_CREATE( Scene )
IMPLEMENT_COMMANDOBJECT( Scene )

BEGIN_MESSAGE_MAP(Scene, TransformableObject)
   SNIPEOBJECT_ON_UPDATE_COMMAND_UI(ID_EXPORT_SCENE, OnUpdateExportScene)
   SNIPEOBJECT_ON_COMMAND(ID_EXPORT_SCENE, OnExportScene)
END_MESSAGE_MAP()

FileInfoProperty::Info Scene::m_fileinfoinfo( "File Information", "FileInfo", "Untitled.snipescene", "Scene File (*.snipescene)|*.snipescene||", "Scene");
Property::Info         Scene::m_PlayRangeInfo( "Play Range", "PlayRange" );
   TimeProperty::Info     Scene::m_tPlaybackStartInfo( "Start", "Start", 0, -MAXTIME, MAXTIME );
   TimeProperty::Info     Scene::m_tPlaybackEndInfo( "End", "End", 1, -MAXTIME, MAXTIME );

Scene::Scene()
{
   m_strName = "Scene1";
   m_fileinfo.Init( &m_fileinfoinfo, this );
   m_PlayRange.Init( &m_PlayRangeInfo, this );
      m_tPlaybackStart.Init( &m_tPlaybackStartInfo, &m_PlayRange );
      m_tPlaybackStartInfo.m_bUndoable = FALSE;
      m_tPlaybackEnd.Init( &m_tPlaybackEndInfo, &m_PlayRange );
      m_tPlaybackEndInfo.m_bUndoable = FALSE;
}

Scene::~Scene()
{
}

void Scene::PreDestruct()
{
   m_commandlist.Empty();
   TransformableObject::PreDestruct();
}

UINT Scene::GetDefaultMenuItemID()
{
   return ID_OBJECT_EDIT;
}

CSnipeDocTemplate *Scene::GetDocTemplate()
{
   return GetApp().GetSceneDocTemplate();
}

void Scene::OnUpdateExportScene(CCmdUI* pCmdUI) 
{
   PluginList *pPluginList = GetMainInterface()->GetSceneTranslatorPluginManager()->GetPluginList();

   BOOL bEnable = FALSE;
   for (POSITION pos = pPluginList->GetHeadPosition(); pos; )
   {
      SceneTranslatorPlugin *pPlugin = (SceneTranslatorPlugin *)pPluginList->GetNext( pos );
      if (pPlugin->GetExportCallback()) {
         bEnable = TRUE;
         break;
      }
   }
   pCmdUI->Enable(bEnable);
}

void Scene::OnExportScene() 
{
   GetMainInterface()->GetSceneTranslatorPluginManager()->Export(this);
}

BOOL Scene::OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam )
{
   switch (msg)
   {
      case NOTIFYOBJ_VALUECHANGED:
      {
         Property *pProperty = (Property *)lParam;
         if (pProperty == &m_tPlaybackStart && m_tPlaybackStart.GetCurrentValue() > m_tPlaybackEnd.GetCurrentValue())
            m_tPlaybackEnd.StoreValue( m_tPlaybackStart, 0, FALSE );
         else if (pProperty == &m_tPlaybackEnd && m_tPlaybackEnd.GetCurrentValue() < m_tPlaybackStart.GetCurrentValue())
            m_tPlaybackStart.StoreValue( m_tPlaybackEnd, 0, FALSE );
         break;
      }
   }

   return TransformableObject::OnObjectNotify( msg, wParam, lParam );
}

HierObject *Scene::AddInstance(HierObject *pCache)
{
   HierObject *pInstance = (HierObject *)pCache->CreateInstance();
   AddChildAtTail( pInstance );
   pInstance->SetName(pCache->GetMatchName());
   return pInstance;
}

HierObject *Scene::ApplyMotionToModel( Motion *pMotionCache, SceneObject *pSceneObject )
{
/*
   SceneObject *pSceneObject = (SceneObject *)pModelCache->CreateInstance();
   AddChildAtTail( pSceneObject );*/

   MotionInstance *pMotionInstance = (MotionInstance*)pMotionCache->CreateInstance();
   pSceneObject->AddMotionInstance( pMotionInstance );
   return (HierObject*)pSceneObject;
}