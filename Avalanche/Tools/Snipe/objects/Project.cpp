#include "stdafx.h"
#include "Project.h"
#include "..\Interface\MainInterface.h"
#include "..\GUI\MainFrm.h"
#include "Interface/Implement.h"
#include "MotionSet.h"
#include "MotionTranslatorPluginManager.h"

FileInfoProperty::Info Project::m_fileinfoinfo( "File Information", "FileInfo", "Untitled.snipeproject", "Project File (*.snipeproject)|*.snipeproject||", "Project");
FloatProperty::Info Project::m_fpsinfo( "Frames Per Second", "FPS", 30, 1, 1000 );

IMPLEMENT_SNIPEOBJECT_CREATE( Project )
IMPLEMENT_COMMANDOBJECT( Project )

Project::Project()
{
   m_strName = "Project";
   m_pSelectionContainer = NULL;

   m_fileinfo.Init( &m_fileinfoinfo, this );
   m_fps.Init( &m_fpsinfo, this );
   m_fpsinfo.m_bUndoable = FALSE;

   GetMainFrame()->GetMainRenderWnd()->m_glPanelBar.m_glExplorerBar.m_glProjectTree.SetObjectHead( this );

   AddChildAtTail( &m_imagecontainer );
   m_imagecontainer.Reference();

   AddChildAtTail( &m_modelcontainer );
   m_modelcontainer.Reference();

   AddChildAtTail( &m_cameracontainer );
   m_cameracontainer.Reference();

   AddChildAtTail( &m_lightcontainer );
   m_lightcontainer.Reference();

   AddChildAtTail( &m_motioncontainer );
   m_motioncontainer.Reference();

   AddChildAtTail( &m_motionsetcontainer );
   m_motionsetcontainer.Reference();

   AddChildAtTail( &m_scenecontainer );
   m_scenecontainer.Reference();

   ResetPathMap();
}

Project::~Project()
{
   if (GetMainInterface()->GetProject() == this)
      GetMainInterface()->SetProject( NULL );

   GetMainFrame()->GetMainRenderWnd()->m_glPanelBar.m_glExplorerBar.m_glProjectTree.SetObjectHead( NULL );

   m_imagecontainer.PreDestruct();
   m_modelcontainer.PreDestruct();
   m_cameracontainer.PreDestruct();
   m_lightcontainer.PreDestruct();
   m_motioncontainer.PreDestruct();
   m_motionsetcontainer.PreDestruct();
   m_scenecontainer.PreDestruct();
}

void Project::PreDestruct()
{
   RemoveChild( &m_imagecontainer );
   RemoveChild( &m_modelcontainer );
   RemoveChild( &m_cameracontainer );
   RemoveChild( &m_lightcontainer );
   RemoveChild( &m_motioncontainer );
   RemoveChild( &m_motionsetcontainer );
   RemoveChild( &m_scenecontainer );

   m_commandlist.Empty();
   HierObject::PreDestruct();
}

//////////////////////////////////
// Operations

Image *Project::AddImage()
{
   Image *pImage = Image::Create();
   m_imagecontainer.AddChildAtTail( pImage );
   return pImage;
}

Image *Project::AddImage( String strFileName )
{
   if (strFileName.IsEmpty())
      return NULL;

   strFileName = GetAbsoluteFileName( strFileName );

   for (int i=0; i<m_imagecontainer.GetNumChildren(); i++)
   {
      Image *pImage = (Image *)m_imagecontainer.GetChildAt(i);
      if (pImage->GetFileName() == strFileName)
         return pImage;
   }

   String strOldName(strFileName);
   BOOL bChanged;
   if (ValidateFileName(strFileName, &bChanged))
   {
      Image *pImage = Image::Create();
      
      if (bChanged)
      {
         //   pImage->OnModified();
         String strError("%1 was remapped to %2.\n");
         strError.Replace("%1", strOldName);
         strError.Replace("%2", strFileName);
         g_outputWindow << strError;
      }

      pImage->m_fileinfo.m_strFileName.StoreValue( strFileName );
      if (TRUE/*pImage->IsLoaded()*/)
      {
         m_imagecontainer.AddChildAtTail( pImage );
         return pImage;
      }
      pImage->Destroy();
   }

   String strError("Image %1 was removed from project.\n");
   strError.Replace("%1", strFileName);
   g_outputWindow << strError;

   return NULL;
}

UINT Project::AddModel( PolyModel *pModel )
{
   return m_modelcontainer.AddChildAtTail( pModel );
}

PolyModel *Project::AddPolyModel(const char *name/*=NULL*/)
{
   PolyModel *pModel = PolyModel::Create();
   m_modelcontainer.AddChildAtTail( pModel );
   if (name)
      pModel->SetName(name);
   return pModel;
}

Camera *Project::AddCamera(const char *name/*=NULL*/)
{
   Camera *pCamera = Camera::Create();
   m_cameracontainer.AddChildAtTail( pCamera );
   if (name)
      pCamera->SetName(name);
   return pCamera;
}

Light *Project::AddLight(const char *name/*=NULL*/)
{
   Light *pLight = Light::Create();
   m_lightcontainer.AddChildAtTail( pLight );
   if (name)
      pLight->SetName(name);
   return pLight;
}

Motion *Project::AddMotion(const char *name/*=NULL*/)
{
   Motion *pMotion = Motion::Create();
   m_motioncontainer.AddChildAtTail( pMotion );
   if (name)
      pMotion->SetName(name);
   return pMotion;
}

SnipeObjectList *Project::ImportMotions( StringList &strFileNames )
{
   return GetMainInterface()->GetMotionTranslatorPluginManager()->Import( strFileNames );
}

Scene *Project::AddScene(const char *name/*=NULL*/)
{
   Scene *pScene = Scene::Create();
   m_scenecontainer.AddChildAtTail( pScene );
   if (name)
      pScene->SetName(name);
   return pScene;
}

MotionSet *Project::AddMotionSet(const char *name/*=NULL*/)
{
   MotionSet *pMotionSet = MotionSet::Create();
   m_motionsetcontainer.AddChildAtTail( pMotionSet );
   if (name)
      pMotionSet->SetName(name);
   return pMotionSet;
}

SnipeObject *Project::CreateChild( const SnipeObjectInfo *pInfo )
{
   switch (pInfo->m_nSOT)
   {
      case SOT_Project:
         return NULL;
      case SOT_ModelContainer:
         return &m_modelcontainer;
      case SOT_CameraContainer:
         return &m_cameracontainer;
      case SOT_LightContainer:
         return &m_lightcontainer;
      case SOT_ImageContainer:
         return &m_imagecontainer;
      case SOT_MotionContainer:
         return &m_motioncontainer;
      case SOT_SceneContainer:
         return &m_scenecontainer;
      case SOT_MotionSetContainer:
         return &m_motionsetcontainer;
      default:
         return HierObject::CreateChild( pInfo );
   }
}

static void NotifyEndProjectLoadBranch( SnipeObject *pObject )
{
   for (int i=0; i<pObject->GetNumChildren(); i++)
   {
      SnipeObject *pChild = pObject->GetChildAt( i );
      NotifyEndProjectLoadBranch( pChild );
      if (!pChild->m_bNotifiedEndProjectLoad)
      {
         pChild->OnEndProjectLoad();
         pChild->m_bNotifiedEndProjectLoad = TRUE;
      }
   }
}

void Project::NotifyEndProjectLoad()
{
   NotifyEndProjectLoadBranch( this );
}

void Project::OnEndObjectLoad()
{
   NotifyEndProjectLoad();
}

