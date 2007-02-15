#pragma once
#include "ModelDoc.h"

class Scene;

class CSceneDoc : public CModelDoc
{
protected:
	DECLARE_BASEDOC(CSceneDoc, CModelDoc)

// Attributes
public:
   Scene   *m_pScene;
   BoundingSphere m_BoundingSphere; // TODO: set this baby
   LARGE_INTEGER m_liPlaybackStartTime;
   UINT     m_bPlaying      : 1,
            m_bLoopPlayback : 1;

         	CSceneDoc();
	virtual ~CSceneDoc();

// Operations
   afx_msg void OnGotoStart();
   afx_msg void OnGotoEnd();
   afx_msg void OnPlayFromStart();
   afx_msg void OnUpdatePlay( CCmdUI *pCmdUI );
   afx_msg void OnPlay();
   afx_msg void OnUpdateLoopPlayback( CCmdUI *pCmdUI );
   afx_msg void OnLoopPlayback();
   afx_msg void OnGoToNextFrame();
   afx_msg void OnUpdateGoToNextFrame( CCmdUI *pCmdUI );
   afx_msg void OnGoToPreviousFrame();
   afx_msg void OnUpdateGoToPreviousFrame( CCmdUI *pCmdUI );

// C3DDoc overrides
   virtual const BoundingSphere &GetBoundingSphere() { return m_BoundingSphere; }

// CBaseDoc overrides
   virtual SnipeDocType GetDocType() { return SDT_CSceneDoc; }
   virtual void SetObjectHead( HierObject *pObjectHead );
   virtual BOOL UsesObject( SnipeObject *pObject );
   virtual TimeProperty *GetPlaybackStartProperty();
   virtual TimeProperty *GetPlaybackEndProperty();
   virtual BOOL OnSnipeIdle();
};
