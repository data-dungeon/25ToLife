#pragma once
#include "TransformableObject.h"
#include "ContainerObject.h"
#include "Interface/MainInterface.h"

class Motion;
class SceneObject;

class Scene : public TransformableObject
{
   DECLARE_MESSAGE_MAP()
public:
   DECLARE_SNIPEOBJECT_CREATE(Scene, TransformableObject)
   DECLARE_COMMANDOBJECT()

   static FileInfoProperty::Info m_fileinfoinfo;
   static Property::Info         m_PlayRangeInfo;
      static TimeProperty::Info     m_tPlaybackStartInfo;
      static TimeProperty::Info     m_tPlaybackEndInfo;

   FileInfoProperty m_fileinfo;
   Property         m_PlayRange;
      TimeProperty     m_tPlaybackStart;
      TimeProperty     m_tPlaybackEnd;

            Scene();
   virtual ~Scene();

   HierObject *AddInstance( HierObject *pCache );
   HierObject *ApplyMotionToModel( Motion *pMotionCache, SceneObject *pSceneObject );
   FileInfoProperty *GetFileInfoProperty() { return &m_fileinfo; }

   // Message Maps
   afx_msg void OnUpdateExportScene(CCmdUI* pCmdUI);
   afx_msg void OnExportScene();

// HierObject overrides
   virtual CSnipeDocTemplate *GetDocTemplate();

// SnipeObject overrides
   virtual UINT GetIconIndex() { return ICONINDEX_SCENE; };
   virtual UINT GetDefaultMenuItemID();
   virtual FileInfoProperty *GetFileInfo() { return &m_fileinfo; }
   virtual BOOL OnObjectNotify( ObjectNotificationMsg msg, WPARAM wParam, LPARAM lParam );
};

class SceneContainer : public ContainerObject
{
public:
   DECLARE_SNIPEOBJECT_CREATE(SceneContainer, ContainerObject)

            SceneContainer();
   virtual ~SceneContainer();

// SnipeObject overrides
   virtual BOOL IsDeletable() { return FALSE; }
};