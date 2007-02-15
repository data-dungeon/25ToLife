#pragma once
#include "Image.h"
#include "ImageContainer.h"
#include "PolyModel.h"
#include "ModelContainer.h"
#include "Camera.h"
#include "Light.h"
#include "Scene.h"
#include "Motion.h"
#include "Interface/MainInterface.h"
#include "MotionSetContainer.h"

class Project : public HierObject
{
public:
   DECLARE_SNIPEOBJECT_CREATE(Project, HierObject)
   DECLARE_COMMANDOBJECT()

   static FileInfoProperty::Info m_fileinfoinfo;
   static FloatProperty::Info    m_fpsinfo;

   FileInfoProperty m_fileinfo;
   FloatProperty    m_fps;

   ModelContainer   m_modelcontainer;
   CameraContainer  m_cameracontainer;
   LightContainer   m_lightcontainer;
   ImageContainer   m_imagecontainer;
   MotionContainer  m_motioncontainer;
   SceneContainer   m_scenecontainer;
   MotionSetContainer m_motionsetcontainer;

            Project();
   virtual ~Project();

// Operations
   UINT AddModel( PolyModel *pModel );
   PolyModel *AddPolyModel(const char *name=NULL );
   Camera *AddCamera( const char *name=NULL );
   Light *AddLight( const char *name=NULL );
   Motion *AddMotion( const char *name=NULL );
   SnipeObjectList *ImportMotions( StringList &strFileNames );
   Scene *AddScene( const char *name=NULL ); 
   Image *AddImage();
   Image *AddImage( String strFileName );
   MotionSet *AddMotionSet( const char *name=NULL );

   void NotifyEndProjectLoad();

// InstanceObject overrides
   virtual UINT GetIconIndex() { return ICONINDEX_PROJECT; };

// SnipeObject overrides
   virtual FileInfoProperty *GetFileInfo() { return &m_fileinfo; }
   virtual catch_msg SnipeObject *CreateChild( const SnipeObjectInfo *pInfo );
   virtual catch_msg void OnEndObjectLoad();
};
